#include <rabbitmq-c/tcp_socket.h>
#include <assert.h>

static const int fixed_channel_id = 1;

int main()
{
    amqp_connection_state_t connection_state = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(connection_state);
    
    int rc = amqp_socket_open(socket, "127.0.0.1", AMQP_PROTOCOL_PORT);
    assert(rc == AMQP_STATUS_OK);
    
    amqp_rpc_reply_t rpc_reply = amqp_login(
                                            connection_state, "/", 1, AMQP_DEFAULT_FRAME_SIZE,
        AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

    amqp_channel_open_ok_t *res =
        amqp_channel_open(connection_state, fixed_channel_id);
    assert(res != NULL);
    
    return 0;
}
