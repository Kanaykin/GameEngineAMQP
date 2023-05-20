/*
 * Copyright 2023 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */
#include <openssl/ssl.h>
#include <openssl/quic.h>
#include <openssl/bio.h>
#include <openssl/lhash.h>
#include "internal/quic_tserver.h"
#include "internal/quic_ssl.h"
#include "testutil.h"

static const char *certfile, *keyfile;

typedef struct stream_info {
    const char      *name;
    SSL             *c_stream;
    uint64_t        s_stream_id;
} STREAM_INFO;

DEFINE_LHASH_OF_EX(STREAM_INFO);

struct helper {
    int                     s_fd;
    BIO                     *s_net_bio, *s_net_bio_own;
    BIO_ADDR                *s_net_bio_addr;
    QUIC_TSERVER            *s;
    LHASH_OF(STREAM_INFO)   *s_streams;

    int                     c_fd;
    BIO                     *c_net_bio, *c_net_bio_own;
    SSL_CTX                 *c_ctx;
    SSL                     *c_conn;
    LHASH_OF(STREAM_INFO)   *c_streams;

    OSSL_TIME       start_time;
    int             init, blocking, check_spin_again;
    int             free_order;
};

struct script_op {
    uint32_t        op;
    const void      *arg0;
    size_t          arg1;
    int             (*check_func)(struct helper *h, const struct script_op *op);
    const char      *stream_name;
    uint64_t        arg2;
};

#define OPK_END                                     0
#define OPK_CHECK                                   1
#define OPK_C_SET_ALPN                              2
#define OPK_C_CONNECT_WAIT                          3
#define OPK_C_WRITE                                 4
#define OPK_S_WRITE                                 5
#define OPK_C_READ_EXPECT                           6
#define OPK_S_READ_EXPECT                           7
#define OPK_C_EXPECT_FIN                            8
#define OPK_S_EXPECT_FIN                            9
#define OPK_C_CONCLUDE                              10
#define OPK_S_CONCLUDE                              11
#define OPK_C_DETACH                                12
#define OPK_C_ATTACH                                13
#define OPK_C_NEW_STREAM                            14
#define OPK_S_NEW_STREAM                            15
#define OPK_C_ACCEPT_STREAM                         16
#define OPK_C_ACCEPT_STREAM_NONE                    17
#define OPK_C_FREE_STREAM                           18
#define OPK_C_SET_DEFAULT_STREAM_MODE               19
#define OPK_C_SET_INCOMING_STREAM_POLICY            20
#define OPK_C_SHUTDOWN                              21
#define OPK_C_EXPECT_CONN_CLOSE_INFO                22
#define OPK_S_EXPECT_CONN_CLOSE_INFO                23
#define OPK_S_BIND_STREAM_ID                        24
#define OPK_C_WAIT_FOR_DATA                         25
#define OPK_C_WRITE_FAIL                            26
#define OPK_S_WRITE_FAIL                            27
#define OPK_C_READ_FAIL                             28
#define OPK_C_STREAM_RESET                          29

#define EXPECT_CONN_CLOSE_APP       (1U << 0)
#define EXPECT_CONN_CLOSE_REMOTE    (1U << 1)

#define C_BIDI_ID(ordinal) \
    (((ordinal) << 2) | QUIC_STREAM_INITIATOR_CLIENT | QUIC_STREAM_DIR_BIDI)
#define S_BIDI_ID(ordinal) \
    (((ordinal) << 2) | QUIC_STREAM_INITIATOR_SERVER | QUIC_STREAM_DIR_BIDI)
#define C_UNI_ID(ordinal) \
    (((ordinal) << 2) | QUIC_STREAM_INITIATOR_CLIENT | QUIC_STREAM_DIR_UNI)
#define S_UNI_ID(ordinal) \
    (((ordinal) << 2) | QUIC_STREAM_INITIATOR_SERVER | QUIC_STREAM_DIR_UNI)

#define OP_END  \
    {OPK_END}
#define OP_CHECK(func, arg2)  \
    {OPK_CHECK, NULL, 0, (func), NULL, (arg2)},
#define OP_C_SET_ALPN(alpn) \
    {OPK_C_SET_ALPN, (alpn), 0, NULL, NULL},
#define OP_C_CONNECT_WAIT() \
    {OPK_C_CONNECT_WAIT, NULL, 0, NULL, NULL},
#define OP_C_WRITE(stream_name, buf, buf_len)   \
    {OPK_C_WRITE, (buf), (buf_len), NULL, #stream_name},
#define OP_S_WRITE(stream_name, buf, buf_len)   \
    {OPK_S_WRITE, (buf), (buf_len), NULL, #stream_name},
#define OP_C_READ_EXPECT(stream_name, buf, buf_len)   \
    {OPK_C_READ_EXPECT, (buf), (buf_len), NULL, #stream_name},
#define OP_S_READ_EXPECT(stream_name, buf, buf_len)   \
    {OPK_S_READ_EXPECT, (buf), (buf_len), NULL, #stream_name},
#define OP_C_EXPECT_FIN(stream_name) \
    {OPK_C_EXPECT_FIN, NULL, 0, NULL, #stream_name},
#define OP_S_EXPECT_FIN(stream_name) \
    {OPK_S_EXPECT_FIN, NULL, 0, NULL, #stream_name},
#define OP_C_CONCLUDE(stream_name) \
    {OPK_C_CONCLUDE, NULL, 0, NULL, #stream_name},
#define OP_S_CONCLUDE(stream_name) \
    {OPK_S_CONCLUDE, NULL, 0, NULL, #stream_name},
#define OP_C_DETACH(stream_name) \
    {OPK_C_DETACH, NULL, 0, NULL, #stream_name},
#define OP_C_ATTACH(stream_name) \
    {OPK_C_ATTACH, NULL, 0, NULL, #stream_name},
#define OP_C_NEW_STREAM_BIDI(stream_name, expect_id) \
    {OPK_C_NEW_STREAM, NULL, 0, NULL, #stream_name, (expect_id)},
#define OP_C_NEW_STREAM_UNI(stream_name, expect_id) \
    {OPK_C_NEW_STREAM, NULL, 1, NULL, #stream_name, (expect_id)},
#define OP_S_NEW_STREAM_BIDI(stream_name, expect_id) \
    {OPK_S_NEW_STREAM, NULL, 0, NULL, #stream_name, (expect_id)},
#define OP_S_NEW_STREAM_UNI(stream_name, expect_id) \
    {OPK_S_NEW_STREAM, NULL, 1, NULL, #stream_name, (expect_id)},
#define OP_C_ACCEPT_STREAM(stream_name) \
    {OPK_C_ACCEPT_STREAM, NULL, 0, NULL, #stream_name},
#define OP_C_ACCEPT_STREAM_NONE() \
    {OPK_C_ACCEPT_STREAM_NONE, NULL, 0, NULL, NULL},
#define OP_C_FREE_STREAM(stream_name) \
    {OPK_C_FREE_STREAM, NULL, 0, NULL, #stream_name},
#define OP_C_SET_DEFAULT_STREAM_MODE(mode) \
    {OPK_C_SET_DEFAULT_STREAM_MODE, NULL, (mode), NULL, NULL},
#define OP_C_SET_INCOMING_STREAM_POLICY(policy) \
    {OPK_C_SET_INCOMING_STREAM_POLICY, NULL, (policy), NULL, NULL},
#define OP_C_SHUTDOWN() \
    {OPK_C_SHUTDOWN, NULL, 0, NULL, NULL},
#define OP_C_EXPECT_CONN_CLOSE_INFO(ec, app, remote)                \
    {OPK_C_EXPECT_CONN_CLOSE_INFO, NULL,                            \
        ((app) ? EXPECT_CONN_CLOSE_APP : 0) |                       \
        ((remote) ? EXPECT_CONN_CLOSE_REMOTE : 0),                  \
        NULL, NULL, (ec)},
#define OP_S_EXPECT_CONN_CLOSE_INFO(ec, app, remote) \
    {OPK_S_EXPECT_CONN_CLOSE_INFO, NULL,            \
        ((app) ? EXPECT_CONN_CLOSE_APP : 0) |       \
        ((remote) ? EXPECT_CONN_CLOSE_REMOTE : 0),  \
        NULL, NULL, (ec)},
#define OP_S_BIND_STREAM_ID(stream_name, stream_id) \
    {OPK_S_BIND_STREAM_ID, NULL, 0, NULL, #stream_name, (stream_id)},
#define OP_C_WAIT_FOR_DATA(stream_name) \
    {OPK_C_WAIT_FOR_DATA, NULL, 0, NULL, #stream_name},
#define OP_C_WRITE_FAIL(stream_name)  \
    {OPK_C_WRITE_FAIL, NULL, 0, NULL, #stream_name},
#define OP_S_WRITE_FAIL(stream_name)  \
    {OPK_S_WRITE_FAIL, NULL, 0, NULL, #stream_name},
#define OP_C_READ_FAIL(stream_name)  \
    {OPK_C_READ_FAIL, NULL, 0, NULL, #stream_name},
#define OP_C_STREAM_RESET(stream_name, aec)  \
    {OPK_C_STREAM_RESET, NULL, 0, NULL, #stream_name, (aec)},

static int check_rejected(struct helper *h, const struct script_op *op)
{
    uint64_t stream_id = op->arg2;

    if (!ossl_quic_tserver_stream_has_peer_stop_sending(h->s, stream_id, NULL)
        || !ossl_quic_tserver_stream_has_peer_reset_stream(h->s, stream_id, NULL)) {
        h->check_spin_again = 1;
        return 0;
    }

    return 1;
}

static int check_stream_reset(struct helper *h, const struct script_op *op)
{
    uint64_t stream_id = op->arg2, aec = 0;

    if (!ossl_quic_tserver_stream_has_peer_reset_stream(h->s, stream_id, &aec)) {
        h->check_spin_again = 1;
        return 0;
    }

    return TEST_uint64_t_eq(aec, 42);
}

static int check_stream_stopped(struct helper *h, const struct script_op *op)
{
    uint64_t stream_id = op->arg2;

    if (!ossl_quic_tserver_stream_has_peer_stop_sending(h->s, stream_id, NULL)) {
        h->check_spin_again = 1;
        return 0;
    }

    return 1;
}

static unsigned long stream_info_hash(const STREAM_INFO *info)
{
    return OPENSSL_LH_strhash(info->name);
}

static int stream_info_cmp(const STREAM_INFO *a, const STREAM_INFO *b)
{
    return strcmp(a->name, b->name);
}

static void cleanup_stream(STREAM_INFO *info)
{
    SSL_free(info->c_stream);
    OPENSSL_free(info);
}

static void helper_cleanup_streams(LHASH_OF(STREAM_INFO) **lh)
{
    if (*lh == NULL)
        return;

    lh_STREAM_INFO_doall(*lh, cleanup_stream);
    lh_STREAM_INFO_free(*lh);
    *lh = NULL;
}

static void helper_cleanup(struct helper *h)
{
    if (h->free_order == 0) {
        /* order 0: streams, then conn */
        helper_cleanup_streams(&h->c_streams);

        SSL_free(h->c_conn);
        h->c_conn = NULL;
    } else {
        /* order 1: conn, then streams */
        SSL_free(h->c_conn);
        h->c_conn = NULL;

        helper_cleanup_streams(&h->c_streams);
    }

    helper_cleanup_streams(&h->s_streams);
    ossl_quic_tserver_free(h->s);
    h->s = NULL;

    BIO_free(h->s_net_bio_own);
    h->s_net_bio_own = NULL;

    BIO_free(h->c_net_bio_own);
    h->c_net_bio_own = NULL;

    if (h->s_fd >= 0) {
        BIO_closesocket(h->s_fd);
        h->s_fd = -1;
    }

    if (h->c_fd >= 0) {
        BIO_closesocket(h->c_fd);
        h->c_fd = -1;
    }

    BIO_ADDR_free(h->s_net_bio_addr);
    h->s_net_bio_addr = NULL;

    SSL_CTX_free(h->c_ctx);
    h->c_ctx = NULL;
}

static int helper_init(struct helper *h, int free_order)
{
    short port = 8186;
    struct in_addr ina = {0};
    QUIC_TSERVER_ARGS s_args = {0};

    memset(h, 0, sizeof(*h));
    h->c_fd = -1;
    h->s_fd = -1;
    h->free_order = free_order;

    if (!TEST_ptr(h->s_streams = lh_STREAM_INFO_new(stream_info_hash,
                                                    stream_info_cmp)))
        goto err;

    if (!TEST_ptr(h->c_streams = lh_STREAM_INFO_new(stream_info_hash,
                                                    stream_info_cmp)))
        goto err;

    ina.s_addr = htonl(0x7f000001UL);

    h->s_fd = BIO_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0);
    if (!TEST_int_ge(h->s_fd, 0))
        goto err;

    if (!TEST_true(BIO_socket_nbio(h->s_fd, 1)))
        goto err;

    if (!TEST_ptr(h->s_net_bio_addr = BIO_ADDR_new()))
        goto err;

    if (!TEST_true(BIO_ADDR_rawmake(h->s_net_bio_addr, AF_INET, &ina, sizeof(ina),
                                    htons(port))))
        goto err;

    if (!TEST_true(BIO_bind(h->s_fd, h->s_net_bio_addr, 0)))
        goto err;

    if (!TEST_int_gt(BIO_ADDR_rawport(h->s_net_bio_addr), 0))
        goto err;

    if  (!TEST_ptr(h->s_net_bio = h->s_net_bio_own = BIO_new_dgram(h->s_fd, 0)))
        goto err;

    if (!BIO_up_ref(h->s_net_bio))
        goto err;

    s_args.net_rbio = h->s_net_bio;
    s_args.net_wbio = h->s_net_bio;

    if (!TEST_ptr(h->s = ossl_quic_tserver_new(&s_args, certfile, keyfile)))
        goto err;

    h->s_net_bio_own = NULL;

    h->c_fd = BIO_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0);
    if (!TEST_int_ge(h->c_fd, 0))
        goto err;

    if (!TEST_true(BIO_socket_nbio(h->c_fd, 1)))
        goto err;

    if (!TEST_ptr(h->c_net_bio = h->c_net_bio_own = BIO_new_dgram(h->c_fd, 0)))
        goto err;

    if (!TEST_true(BIO_dgram_set_peer(h->c_net_bio, h->s_net_bio_addr)))
        goto err;


    if (!TEST_ptr(h->c_ctx = SSL_CTX_new(OSSL_QUIC_client_method())))
        goto err;

    if (!TEST_ptr(h->c_conn = SSL_new(h->c_ctx)))
        goto err;

    /* Takes ownership of our reference to the BIO. */
    SSL_set0_rbio(h->c_conn, h->c_net_bio);
    h->c_net_bio_own = NULL;

    if (!TEST_true(BIO_up_ref(h->c_net_bio)))
        goto err;

    SSL_set0_wbio(h->c_conn, h->c_net_bio);

    if (!TEST_true(SSL_set_blocking_mode(h->c_conn, 0)))
        goto err;

    h->start_time   = ossl_time_now();
    h->init         = 1;
    return 1;

err:
    helper_cleanup(h);
    return 0;
}

static STREAM_INFO *get_stream_info(LHASH_OF(STREAM_INFO) *lh,
                                    const char *stream_name)
{
    STREAM_INFO key, *info;

    if (!TEST_ptr(stream_name))
        return NULL;

    if (!strcmp(stream_name, "DEFAULT"))
        return NULL;

    key.name = stream_name;
    info = lh_STREAM_INFO_retrieve(lh, &key);
    if (info == NULL) {
        info = OPENSSL_zalloc(sizeof(*info));
        if (info == NULL)
            return NULL;

        info->name          = stream_name;
        info->s_stream_id   = UINT64_MAX;
        lh_STREAM_INFO_insert(lh, info);
    }

    return info;
}

static int helper_set_c_stream(struct helper *h, const char *stream_name,
                               SSL *c_stream)
{
    STREAM_INFO *info = get_stream_info(h->c_streams, stream_name);

    if (info == NULL)
        return 0;

    info->c_stream      = c_stream;
    info->s_stream_id   = UINT64_MAX;
    return 1;
}

static SSL *helper_get_c_stream(struct helper *h, const char *stream_name)
{
    STREAM_INFO *info;

    if (!strcmp(stream_name, "DEFAULT"))
        return h->c_conn;

    info = get_stream_info(h->c_streams, stream_name);
    if (info == NULL)
        return NULL;

    return info->c_stream;
}

static int
helper_set_s_stream(struct helper *h, const char *stream_name,
                    uint64_t s_stream_id)
{
    STREAM_INFO *info;

    if (!strcmp(stream_name, "DEFAULT"))
        return 0;

    info = get_stream_info(h->s_streams, stream_name);
    if (info == NULL)
        return 0;

    info->c_stream      = NULL;
    info->s_stream_id   = s_stream_id;
    return 1;
}

static uint64_t helper_get_s_stream(struct helper *h, const char *stream_name)
{
    STREAM_INFO *info;

    if (!strcmp(stream_name, "DEFAULT"))
        return UINT64_MAX;

    info = get_stream_info(h->s_streams, stream_name);
    if (info == NULL)
        return UINT64_MAX;

    return info->s_stream_id;
}

static int is_want(SSL *s, int ret)
{
    int ec = SSL_get_error(s, ret);

    return ec == SSL_ERROR_WANT_READ || ec == SSL_ERROR_WANT_WRITE;
}

static int run_script(const struct script_op *script, int free_order)
{
    size_t op_idx = 0;
    int testresult = 0;
    int no_advance = 0, first = 1;
    const struct script_op *op = NULL;
    struct helper h;
    unsigned char *tmp_buf = NULL;
    int connect_started = 0;
    OSSL_TIME op_start_time = ossl_time_zero(), op_deadline = ossl_time_zero();
    size_t offset = 0;

    if (!TEST_true(helper_init(&h, free_order)))
        goto out;

#define SPIN_AGAIN() { no_advance = 1; continue; }

    for (;;) {
        SSL *c_tgt              = h.c_conn;
        uint64_t s_stream_id    = UINT64_MAX;

        if (no_advance) {
            no_advance = 0;
        } else {
            if (!first)
                ++op_idx;

            first           = 0;
            offset          = 0;
            op_start_time   = ossl_time_now();
            op_deadline     = ossl_time_add(op_start_time, ossl_ms2time(2000));
        }

        if (!TEST_int_le(ossl_time_compare(ossl_time_now(), op_deadline), 0)) {
            TEST_error("op %zu timed out", op_idx + 1);
            goto out;
        }

        op = &script[op_idx];

        if (op->stream_name != NULL) {
            c_tgt       = helper_get_c_stream(&h, op->stream_name);
            s_stream_id = helper_get_s_stream(&h, op->stream_name);
        }

        ossl_quic_tserver_tick(h.s);
        if (connect_started)
            SSL_tick(h.c_conn);

        switch (op->op) {
        case OPK_END:
            testresult = 1;
            goto out;

        case OPK_CHECK:
            {
                int ok = op->check_func(&h, op);
                if (h.check_spin_again) {
                    h.check_spin_again = 0;
                    SPIN_AGAIN();
                }

                if (!TEST_true(ok))
                    goto out;
            }
            break;

        case OPK_C_SET_ALPN:
            {
                const char *alpn = op->arg0;
                size_t alpn_len = strlen(alpn);

                if (!TEST_size_t_le(alpn_len, UINT8_MAX)
                    || !TEST_ptr(tmp_buf = (unsigned char *)OPENSSL_malloc(alpn_len + 1)))
                    goto out;

                memcpy(tmp_buf + 1, alpn, alpn_len);
                tmp_buf[0] = (unsigned char)alpn_len;

                /* 0 is the success case for SSL_set_alpn_protos(). */
                if (!TEST_false(SSL_set_alpn_protos(h.c_conn, tmp_buf,
                                                    alpn_len + 1)))
                    goto out;

                OPENSSL_free(tmp_buf);
                tmp_buf = NULL;
            }
            break;

        case OPK_C_CONNECT_WAIT:
            {
                int ret;

                connect_started = 1;

                ret = SSL_connect(h.c_conn);
                if (!TEST_true(ret == 1
                               || (!h.blocking && is_want(h.c_conn, ret))))
                    goto out;

                if (!h.blocking && ret != 1)
                    SPIN_AGAIN();
            }
            break;

        case OPK_C_WRITE:
            {
                size_t bytes_written = 0;

                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_true(SSL_write_ex(c_tgt, op->arg0, op->arg1,
                                            &bytes_written))
                    || !TEST_size_t_eq(bytes_written, op->arg1))
                    goto out;
            }
            break;

        case OPK_S_WRITE:
            {
                size_t bytes_written = 0;

                if (!TEST_uint64_t_ne(s_stream_id, UINT64_MAX))
                    goto out;

                if (!TEST_true(ossl_quic_tserver_write(h.s, s_stream_id,
                                                       op->arg0, op->arg1,
                                                       &bytes_written))
                    || !TEST_size_t_eq(bytes_written, op->arg1))
                    goto out;
            }
            break;

        case OPK_C_CONCLUDE:
            {
                if (!TEST_true(SSL_stream_conclude(c_tgt, 0)))
                    goto out;
            }
            break;

        case OPK_S_CONCLUDE:
            {
                if (!TEST_uint64_t_ne(s_stream_id, UINT64_MAX))
                    goto out;

                ossl_quic_tserver_conclude(h.s, s_stream_id);
            }
            break;

        case OPK_C_WAIT_FOR_DATA:
            {
                char buf[1];
                size_t bytes_read = 0;

                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!SSL_peek_ex(c_tgt, buf, sizeof(buf), &bytes_read)
                    || bytes_read == 0)
                    SPIN_AGAIN();
            }
            break;

        case OPK_C_READ_EXPECT:
            {
                size_t bytes_read = 0;

                if (op->arg1 > 0 && tmp_buf == NULL
                    && !TEST_ptr(tmp_buf = OPENSSL_malloc(op->arg1)))
                    goto out;

                if (!SSL_read_ex(c_tgt, tmp_buf + offset, op->arg1 - offset,
                                 &bytes_read))
                    SPIN_AGAIN();

                if (bytes_read + offset != op->arg1) {
                    offset += bytes_read;
                    SPIN_AGAIN();
                }

                if (op->arg1 > 0
                    && !TEST_mem_eq(tmp_buf, op->arg1, op->arg0, op->arg1))
                    goto out;

                OPENSSL_free(tmp_buf);
                tmp_buf = NULL;
            }
            break;

        case OPK_S_READ_EXPECT:
            {
                size_t bytes_read = 0;

                if (!TEST_uint64_t_ne(s_stream_id, UINT64_MAX))
                    goto out;

                if (op->arg1 > 0 && tmp_buf == NULL
                    && !TEST_ptr(tmp_buf = OPENSSL_malloc(op->arg1)))
                    goto out;

                if (!TEST_true(ossl_quic_tserver_read(h.s, s_stream_id,
                                                      tmp_buf + offset,
                                                      op->arg1 - offset,
                                                      &bytes_read)))
                    goto out;

                if (bytes_read + offset != op->arg1) {
                    offset += bytes_read;
                    SPIN_AGAIN();
                }

                if (op->arg1 > 0
                    && !TEST_mem_eq(tmp_buf, op->arg1, op->arg0, op->arg1))
                    goto out;

                OPENSSL_free(tmp_buf);
                tmp_buf = NULL;
            }
            break;

        case OPK_C_EXPECT_FIN:
            {
                char buf[1];
                size_t bytes_read = 0;

                if (!TEST_false(SSL_read_ex(c_tgt, buf, sizeof(buf),
                                            &bytes_read))
                    || !TEST_size_t_eq(bytes_read, 0))
                    goto out;

                if (is_want(c_tgt, 0))
                    SPIN_AGAIN();

                if (!TEST_int_eq(SSL_get_error(c_tgt, 0),
                                 SSL_ERROR_ZERO_RETURN))
                    goto out;
            }
            break;

        case OPK_S_EXPECT_FIN:
            {
                if (!TEST_uint64_t_ne(s_stream_id, UINT64_MAX))
                    goto out;

                if (!ossl_quic_tserver_has_read_ended(h.s, s_stream_id))
                    SPIN_AGAIN();
            }
            break;

        case OPK_C_DETACH:
            {
                SSL *c_stream;

                if (!TEST_ptr_null(c_tgt))
                    goto out; /* don't overwrite existing stream with same name */

                if (!TEST_ptr(c_stream = ossl_quic_detach_stream(h.c_conn)))
                    goto out;

                if (!TEST_true(helper_set_c_stream(&h, op->stream_name, c_stream)))
                    goto out;
            }
            break;

        case OPK_C_ATTACH:
            {
                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_true(ossl_quic_attach_stream(h.c_conn, c_tgt)))
                    goto out;

                if (!TEST_true(helper_set_c_stream(&h, op->stream_name, NULL)))
                    goto out;
            }
            break;

        case OPK_C_NEW_STREAM:
            {
                SSL *c_stream;
                uint64_t flags = 0;

                if (!TEST_ptr_null(c_tgt))
                    goto out; /* don't overwrite existing stream with same name */

                if (op->arg1 != 0)
                    flags |= SSL_STREAM_FLAG_UNI;

                if (!TEST_ptr(c_stream = SSL_new_stream(h.c_conn, flags)))
                    goto out;

                if (op->arg2 != UINT64_MAX
                    && !TEST_uint64_t_eq(SSL_get_stream_id(c_stream),
                                         op->arg2))
                    goto out;

                if (!TEST_true(helper_set_c_stream(&h, op->stream_name, c_stream)))
                    goto out;
            }
            break;

        case OPK_S_NEW_STREAM:
            {
                uint64_t stream_id = UINT64_MAX;

                if (!TEST_uint64_t_eq(s_stream_id, UINT64_MAX))
                    goto out; /* don't overwrite existing stream with same name */

                if (!TEST_true(ossl_quic_tserver_stream_new(h.s,
                                                            op->arg1 > 0,
                                                            &stream_id)))
                    goto out;

                if (op->arg2 != UINT64_MAX
                    && !TEST_uint64_t_eq(stream_id, op->arg2))
                    goto out;

                if (!TEST_true(helper_set_s_stream(&h, op->stream_name,
                                                   stream_id)))
                    goto out;
            }
            break;

        case OPK_C_ACCEPT_STREAM:
            {
                SSL *c_stream;

                if (!TEST_ptr_null(c_tgt))
                    goto out; /* don't overwrite existing stream with same name */

                if ((c_stream = SSL_accept_stream(h.c_conn, 0)) == NULL)
                    SPIN_AGAIN();

                if (!TEST_true(helper_set_c_stream(&h, op->stream_name,
                                                   c_stream)))
                    goto out;
            }
            break;

        case OPK_C_ACCEPT_STREAM_NONE:
            {
                SSL *c_stream;

                if (!TEST_ptr_null(c_stream = SSL_accept_stream(h.c_conn, 0))) {
                    SSL_free(c_stream);
                    goto out;
                }
            }
            break;

        case OPK_C_FREE_STREAM:
            {
                if (!TEST_ptr(c_tgt)
                    || !TEST_true(!SSL_is_connection(c_tgt)))
                    goto out;

                if (!TEST_true(helper_set_c_stream(&h, op->stream_name, NULL)))
                    goto out;

                SSL_free(c_tgt);
                c_tgt = NULL;
            }
            break;

        case OPK_C_SET_DEFAULT_STREAM_MODE:
            {
                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_true(SSL_set_default_stream_mode(c_tgt, op->arg1)))
                    goto out;
            }
            break;

        case OPK_C_SET_INCOMING_STREAM_POLICY:
            {
                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_true(SSL_set_incoming_stream_policy(c_tgt,
                                                              op->arg1, 0)))
                    goto out;
            }
            break;

        case OPK_C_SHUTDOWN:
            {
                int ret;

                if (!TEST_ptr(c_tgt))
                    goto out;

                ret = SSL_shutdown_ex(c_tgt, 0, NULL, 0);
                if (!TEST_int_ge(ret, 0))
                    goto out;

            }
            break;

        case OPK_C_EXPECT_CONN_CLOSE_INFO:
            {
                SSL_CONN_CLOSE_INFO cc_info = {0};
                int expect_app = (op->arg1 & EXPECT_CONN_CLOSE_APP) != 0;
                int expect_remote = (op->arg1 & EXPECT_CONN_CLOSE_REMOTE) != 0;
                uint64_t error_code = op->arg2;

                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!SSL_get_conn_close_info(c_tgt, &cc_info, sizeof(cc_info)))
                    SPIN_AGAIN();

                if (!TEST_int_eq(expect_app, !cc_info.is_transport)
                    || !TEST_int_eq(expect_remote, !cc_info.is_local)
                    || !TEST_uint64_t_eq(error_code, cc_info.error_code))
                    goto out;
            }
            break;

        case OPK_S_EXPECT_CONN_CLOSE_INFO:
            {
                const QUIC_TERMINATE_CAUSE *tc;
                int expect_app = (op->arg1 & EXPECT_CONN_CLOSE_APP) != 0;
                int expect_remote = (op->arg1 & EXPECT_CONN_CLOSE_REMOTE) != 0;
                uint64_t error_code = op->arg2;

                if (!ossl_quic_tserver_is_term_any(h.s))
                    SPIN_AGAIN();

                if (!TEST_ptr(tc = ossl_quic_tserver_get_terminate_cause(h.s)))
                    goto out;

                if (!TEST_uint64_t_eq(error_code, tc->error_code)
                    || !TEST_int_eq(expect_app, tc->app)
                    || !TEST_int_eq(expect_remote, tc->remote))
                    goto out;
            }
            break;

        case OPK_S_BIND_STREAM_ID:
            {
                if (!TEST_uint64_t_eq(s_stream_id, UINT64_MAX))
                    goto out;

                if (!TEST_true(helper_set_s_stream(&h, op->stream_name, op->arg2)))
                    goto out;
            }
            break;

        case OPK_C_WRITE_FAIL:
            {
                size_t bytes_written = 0;

                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_false(SSL_write_ex(c_tgt, "apple", 5, &bytes_written)))
                    goto out;
            }
            break;

        case OPK_S_WRITE_FAIL:
            {
                size_t bytes_written = 0;

                if (!TEST_uint64_t_ne(s_stream_id, UINT64_MAX))
                    goto out;

                if (!TEST_false(ossl_quic_tserver_write(h.s, s_stream_id,
                                                       (const unsigned char *)"apple", 5,
                                                       &bytes_written)))
                    goto out;
            }
            break;

        case OPK_C_READ_FAIL:
            {
                size_t bytes_read = 0;
                char buf[1];

                if (!TEST_ptr(c_tgt))
                    goto out;

                if (!TEST_false(SSL_read_ex(c_tgt, buf, sizeof(buf), &bytes_read)))
                    goto out;
            }
            break;

        case OPK_C_STREAM_RESET:
            {
                SSL_STREAM_RESET_ARGS args = {0};

                if (!TEST_ptr(c_tgt))
                    goto out;

                args.quic_error_code = op->arg2;

                if (!TEST_true(SSL_stream_reset(c_tgt, &args, sizeof(args))))
                    goto out;
            }
            break;

        default:
            TEST_error("unknown op");
            goto out;
        }
    }

out:
    if (!testresult)
        TEST_error("failed at script op %zu\n", op_idx + 1);

    OPENSSL_free(tmp_buf);
    helper_cleanup(&h);
    return testresult;
}

/* 1. Simple single-stream test */
static const struct script_op script_1[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()
    OP_C_WRITE              (DEFAULT, "apple", 5)
    OP_C_CONCLUDE           (DEFAULT)
    OP_S_BIND_STREAM_ID     (a, C_BIDI_ID(0))
    OP_S_READ_EXPECT        (a, "apple", 5)
    OP_S_EXPECT_FIN         (a)
    OP_S_WRITE              (a, "orange", 6)
    OP_S_CONCLUDE           (a)
    OP_C_READ_EXPECT        (DEFAULT, "orange", 6)
    OP_C_EXPECT_FIN         (DEFAULT)
    OP_END
};

/* 2. Multi-stream test */
static const struct script_op script_2[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()
    OP_C_SET_INCOMING_STREAM_POLICY(SSL_INCOMING_STREAM_POLICY_ACCEPT)
    OP_C_WRITE              (DEFAULT,  "apple", 5)
    OP_S_BIND_STREAM_ID     (a, C_BIDI_ID(0))
    OP_S_READ_EXPECT        (a, "apple", 5)
    OP_S_WRITE              (a, "orange", 6)
    OP_C_READ_EXPECT        (DEFAULT, "orange", 6)

    OP_C_NEW_STREAM_BIDI    (b, C_BIDI_ID(1))
    OP_C_WRITE              (b, "flamingo", 8)
    OP_C_CONCLUDE           (b)
    OP_S_BIND_STREAM_ID     (b, C_BIDI_ID(1))
    OP_S_READ_EXPECT        (b, "flamingo", 8)
    OP_S_EXPECT_FIN         (b)
    OP_S_WRITE              (b, "gargoyle", 8)
    OP_S_CONCLUDE           (b)
    OP_C_READ_EXPECT        (b, "gargoyle", 8)
    OP_C_EXPECT_FIN         (b)

    OP_C_NEW_STREAM_UNI     (c, C_UNI_ID(0))
    OP_C_WRITE              (c, "elephant", 8)
    OP_C_CONCLUDE           (c)
    OP_S_BIND_STREAM_ID     (c, C_UNI_ID(0))
    OP_S_READ_EXPECT        (c, "elephant", 8)
    OP_S_EXPECT_FIN         (c)
    OP_S_WRITE_FAIL         (c)

    OP_C_ACCEPT_STREAM_NONE ()

    OP_S_NEW_STREAM_BIDI    (d, S_BIDI_ID(0))
    OP_S_WRITE              (d, "frog", 4)
    OP_S_CONCLUDE           (d)

    OP_C_ACCEPT_STREAM      (d)
    OP_C_ACCEPT_STREAM_NONE ()
    OP_C_READ_EXPECT        (d, "frog", 4)
    OP_C_EXPECT_FIN         (d)

    OP_S_NEW_STREAM_BIDI    (e, S_BIDI_ID(1))
    OP_S_WRITE              (e, "mixture", 7)
    OP_S_CONCLUDE           (e)

    OP_C_ACCEPT_STREAM      (e)
    OP_C_READ_EXPECT        (e, "mixture", 7)
    OP_C_EXPECT_FIN         (e)
    OP_C_WRITE              (e, "ramble", 6)
    OP_S_READ_EXPECT        (e, "ramble", 6)
    OP_C_CONCLUDE           (e)
    OP_S_EXPECT_FIN         (e)

    OP_S_NEW_STREAM_UNI     (f, S_UNI_ID(0))
    OP_S_WRITE              (f, "yonder", 6)
    OP_S_CONCLUDE           (f)

    OP_C_ACCEPT_STREAM      (f)
    OP_C_ACCEPT_STREAM_NONE ()
    OP_C_READ_EXPECT        (f, "yonder", 6)
    OP_C_EXPECT_FIN         (f)
    OP_C_WRITE_FAIL         (f)

    OP_C_SET_INCOMING_STREAM_POLICY(SSL_INCOMING_STREAM_POLICY_REJECT)
    OP_S_NEW_STREAM_BIDI    (g, S_BIDI_ID(2))
    OP_S_WRITE              (g, "unseen", 6)
    OP_S_CONCLUDE           (g)

    OP_C_ACCEPT_STREAM_NONE ()

    OP_C_SET_INCOMING_STREAM_POLICY(SSL_INCOMING_STREAM_POLICY_AUTO)
    OP_S_NEW_STREAM_BIDI    (h, S_BIDI_ID(3))
    OP_S_WRITE              (h, "UNSEEN", 6)
    OP_S_CONCLUDE           (h)

    OP_C_ACCEPT_STREAM_NONE ()

    /*
     * Streams g, h should have been rejected, so server should have got
     * STOP_SENDING/RESET_STREAM.
     */
    OP_CHECK                (check_rejected, S_BIDI_ID(2))
    OP_CHECK                (check_rejected, S_BIDI_ID(3))

    OP_END
};

/* 3. Default stream detach/reattach test */
static const struct script_op script_3[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_WRITE              (DEFAULT, "apple", 5)
    OP_C_DETACH             (a)             /* DEFAULT becomes stream 'a' */
    OP_C_WRITE_FAIL         (DEFAULT)

    OP_C_WRITE              (a, "by", 2)

    OP_S_BIND_STREAM_ID     (a, C_BIDI_ID(0))
    OP_S_READ_EXPECT        (a, "appleby", 7)

    OP_S_WRITE              (a, "hello", 5)
    OP_C_READ_EXPECT        (a, "hello", 5)

    OP_C_WRITE_FAIL         (DEFAULT)
    OP_C_ATTACH             (a)
    OP_C_WRITE              (DEFAULT, "is here", 7)
    OP_S_READ_EXPECT        (a, "is here", 7)

    OP_C_DETACH             (a)
    OP_C_CONCLUDE           (a)
    OP_S_EXPECT_FIN         (a)

    OP_END
};

/* 4. Default stream mode test */
static const struct script_op script_4[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_NONE)
    OP_C_WRITE_FAIL         (DEFAULT)

    OP_S_NEW_STREAM_BIDI    (a, S_BIDI_ID(0))
    OP_S_WRITE              (a, "apple", 5)

    OP_C_READ_FAIL          (DEFAULT)

    OP_C_ACCEPT_STREAM      (a)
    OP_C_READ_EXPECT        (a, "apple", 5)

    OP_C_ATTACH             (a)
    OP_C_WRITE              (DEFAULT, "orange", 6)
    OP_S_READ_EXPECT        (a, "orange", 6)

    OP_END
};

/* 5. Test stream reset functionality */
static const struct script_op script_5[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_NONE)
    OP_C_NEW_STREAM_BIDI    (a, C_BIDI_ID(0))

    OP_C_WRITE              (a, "apple", 5)
    OP_C_STREAM_RESET       (a, 42)

    OP_S_BIND_STREAM_ID     (a, C_BIDI_ID(0))
    OP_S_READ_EXPECT        (a, "apple", 5)
    OP_CHECK                (check_stream_reset, C_BIDI_ID(0))

    OP_END
};

/* 6. Test STOP_SENDING functionality */
static const struct script_op script_6[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_NONE)
    OP_S_NEW_STREAM_BIDI    (a, S_BIDI_ID(0))
    OP_S_WRITE              (a, "apple", 5)

    OP_C_ACCEPT_STREAM      (a)
    OP_C_FREE_STREAM        (a)
    OP_C_ACCEPT_STREAM_NONE ()

    OP_CHECK                (check_stream_stopped, S_BIDI_ID(0))

    OP_END
};

/* 7. Unidirectional default stream mode test (client sends first) */
static const struct script_op script_7[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_AUTO_UNI)
    OP_C_WRITE              (DEFAULT, "apple", 5)

    OP_S_BIND_STREAM_ID     (a, C_UNI_ID(0))
    OP_S_READ_EXPECT        (a, "apple", 5)
    OP_S_WRITE_FAIL         (a)

    OP_END
};

/* 8. Unidirectional default stream mode test (server sends first) */
static const struct script_op script_8[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_AUTO_UNI)
    OP_S_NEW_STREAM_UNI     (a, S_UNI_ID(0))
    OP_S_WRITE              (a, "apple", 5)
    OP_C_READ_EXPECT        (DEFAULT, "apple", 5)
    OP_C_WRITE_FAIL         (DEFAULT)

    OP_END
};

/* 9. Unidirectional default stream mode test (server sends first on bidi) */
static const struct script_op script_9[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_SET_DEFAULT_STREAM_MODE(SSL_DEFAULT_STREAM_MODE_AUTO_UNI)
    OP_S_NEW_STREAM_BIDI    (a, S_BIDI_ID(0))
    OP_S_WRITE              (a, "apple", 5)
    OP_C_READ_EXPECT        (DEFAULT, "apple", 5)
    OP_C_WRITE              (DEFAULT, "orange", 6)
    OP_S_READ_EXPECT        (a, "orange", 6)

    OP_END
};

/* 10. Shutdown */
static const struct script_op script_10[] = {
    OP_C_SET_ALPN           ("ossltest")
    OP_C_CONNECT_WAIT       ()

    OP_C_WRITE              (DEFAULT, "apple", 5)
    OP_S_BIND_STREAM_ID     (a, C_BIDI_ID(0))
    OP_S_READ_EXPECT        (a, "apple", 5)

    OP_C_SHUTDOWN           ()
    OP_C_EXPECT_CONN_CLOSE_INFO(0, 1, 0)
    OP_S_EXPECT_CONN_CLOSE_INFO(0, 1, 1)

    OP_END
};

static const struct script_op *const scripts[] = {
    script_1,
    script_2,
    script_3,
    script_4,
    script_5,
    script_6,
    script_7,
    script_8,
    script_9,
    script_10,
};

static int test_script(int idx)
{
    int script_idx = idx >> 1;
    int free_order = idx & 1;

    TEST_info("Running script %d (order=%d)", script_idx + 1, free_order);
    return run_script(scripts[script_idx], free_order);
}

OPT_TEST_DECLARE_USAGE("certfile privkeyfile\n")

int setup_tests(void)
{
    if (!test_skip_common_options()) {
        TEST_error("Error parsing test options\n");
        return 0;
    }

    if (!TEST_ptr(certfile = test_get_argument(0))
        || !TEST_ptr(keyfile = test_get_argument(1)))
        return 0;

    ADD_ALL_TESTS(test_script, OSSL_NELEM(scripts) * 2);
    return 1;
}
