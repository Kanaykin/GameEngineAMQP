#include <amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <assert.h>
#include <AMQPcpp.h>

#include <proton/container.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/sender_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cctype>

static const int fixed_channel_id = 1;
static const char test_queue_name[] = "test_queue";

class server : public proton::messaging_handler {
  private:
    class listener_ready_handler : public proton::listen_handler {
        void on_open(proton::listener& l) override {
            std::cout << "listening on " << l.port() << std::endl;
        }
    };

    typedef std::map<std::string, proton::sender> sender_map;
    listener_ready_handler listen_handler;
    std::string url;
    sender_map senders;
    int address_counter;

  public:
    server(const std::string &u) : url(u), address_counter(0) {}

    void on_container_start(proton::container &c) override {
        c.listen(url, listen_handler);
    }

    std::string to_upper(const std::string &s) {
        std::string uc(s);
        size_t l = uc.size();

        for (size_t i=0; i<l; i++)
            uc[i] = static_cast<char>(std::toupper(uc[i]));

        return uc;
    }

    std::string generate_address() {
        std::ostringstream addr;
        addr << "server" << address_counter++;

        return addr.str();
    }

    void on_sender_open(proton::sender &sender) override {
        if (sender.source().dynamic()) {
            std::string addr = generate_address();
            sender.open(proton::sender_options().source(proton::source_options().address(addr)));
            senders[addr] = sender;
        }
    }

    void on_message(proton::delivery &, proton::message &m) override {
        std::cout << "Received " << m.body() << std::endl;

        std::string reply_to = m.reply_to();
        sender_map::iterator it = senders.find(reply_to);

        if (it == senders.end()) {
            std::cout << "No link for reply_to: " << reply_to << std::endl;
        } else {
            proton::sender sender = it->second;
            proton::message reply;

            reply.to(reply_to);
            reply.body(to_upper(proton::get<std::string>(m.body())));
            reply.correlation_id(m.correlation_id());

            sender.send(reply);
        }


    }
};

int main()
{
//    amqp_connection_state_t connection_state = amqp_new_connection();
//    amqp_socket_t *socket = amqp_tcp_socket_new(connection_state);
    
//    int rc = amqp_socket_open(socket, "localhost", AMQP_PROTOCOL_PORT);
//    assert(rc == AMQP_STATUS_OK);
    
//    amqp_rpc_reply_t rpc_reply = amqp_login(
//                                            connection_state, "/", 1, AMQP_DEFAULT_FRAME_SIZE,
//        AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
//    assert(rpc_reply.reply_type == AMQP_RESPONSE_NORMAL);

//    amqp_channel_open_ok_t *res =
//        amqp_channel_open(connection_state, fixed_channel_id);
//    assert(res != NULL);
    
//    try
//    {
//        AMQP amqp;
//
//        AMQPExchange * ex = amqp.createExchange("hello-exchange");
//        ex->Declare("hello-exchange", "direct");
//
//        AMQPQueue * queue = amqp.createQueue("hello-queue");
//        queue->Declare();
//        queue->Bind( "hello-exchange", "hola");
//    }
//    catch (AMQPException& e)
//    {
//        std::cout << e.getMessage() << std::endl;
////        boost::this_thread::sleep( boost::posix_time::milliseconds(3000) );
////        publish();
//    }

    try
    {
        server hw("amqp://127.0.0.1:5672/examples");
        proton::container(hw).run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
