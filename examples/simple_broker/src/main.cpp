#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"
#include <chrono>
#include <thread>
#include <future>
#include <chrono>

static const std::chrono::duration<double> LIVE_TIME(2.0);

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
    
    auto exchange1 = msgSystem->createExchange({"localhost:5672"});
    
    auto customer = msgSystem->createConsumer({"localhost:5672", false, "customer1"});
    auto customer2 = msgSystem->createConsumer({"localhost:5672", false, "customer2"});
    
    auto producer1 = msgSystem->createProducer({"localhost:5672/customer1"});
    auto producer2 = msgSystem->createProducer({"localhost:5672/customer2"});
    
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        
        producer1->publish();
        producer2->publish();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds > LIVE_TIME)
            break;
    }
    
    return 0;
}
