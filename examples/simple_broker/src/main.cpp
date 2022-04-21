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
    
//    auto exchange1 = msgSystem->createExchange("localhost:5673/client");
//    auto exchange2 = msgSystem->createExchange("localhost:5674/render");
//    auto exchange3 = msgSystem->createExchange("localhost:5675/sound");
    
    
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto exchange1 = msgSystem->createExchange({"localhost:5672/client"});
    
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto customer = msgSystem->createConsumer({"localhost:5672/client", false, "customer1"});
    auto customer2 = msgSystem->createConsumer({"localhost:5672/client", false, "customer2"});
    
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto producer1 = msgSystem->createProducer({"localhost:5672/client"});
//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//    auto producer2 = msgSystem->createProducer("localhost:5673/render");
    
//    msgSystem->run();
    
    // #TODO: Need application class with right exit
    
    
//    auto a1 = std::async([producer1](){
//        while (true)
//        {
//            producer1->publish();
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//    });
    
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
//        int ch = std::getchar();
//      double start = getCurrentTime();
//      processInput();
//      update();
//      render();
        //producer2->publish();

//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        producer1->publish();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds > LIVE_TIME)
            break;
    }
    
    return 0;
}
