#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"
#include <chrono>
#include <thread>

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
    
//    auto exchange1 = msgSystem->createExchange("localhost:5673/client");
//    auto exchange2 = msgSystem->createExchange("localhost:5674/render");
//    auto exchange3 = msgSystem->createExchange("localhost:5675/sound");
    
    
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto exchange1 = msgSystem->createExchange("localhost:5672/client");
    auto customer = msgSystem->createConsumer("localhost:5673/render");
//    auto customer2 = msgSystem->createConsumer("localhost:5673/render");
    
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto producer1 = msgSystem->createProducer("localhost:5672/client");
//    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    auto producer2 = msgSystem->createProducer("localhost:5673/render");
    
//    msgSystem->run();
    
    while (true)
    {
//      double start = getCurrentTime();
//      processInput();
//      update();
//      render();
        producer2->publish();
        producer1->publish();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}
