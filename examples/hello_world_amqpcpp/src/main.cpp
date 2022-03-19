#include <amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <assert.h>
#include <AMQPcpp.h>


static const int fixed_channel_id = 1;
static const char test_queue_name[] = "test_queue";

int main()
{

    
    try
    {
        AMQP amqp;

        AMQPExchange * ex = amqp.createExchange("hello-exchange");
        ex->Declare("hello-exchange", "direct");

        AMQPQueue * queue = amqp.createQueue("hello-queue");
        queue->Declare();
        queue->Bind( "hello-exchange", "hola");
    }
    catch (AMQPException& e)
    {
        std::cout << e.getMessage() << std::endl;
//        boost::this_thread::sleep( boost::posix_time::milliseconds(3000) );
//        publish();
    }
    
    return 0;
}
