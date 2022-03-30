#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"
#include <chrono>
#include <thread>

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto producer1 = msgSystem->createProducer("localhost:5673/client");
    auto producer2 = msgSystem->createProducer("localhost:5673/render");
    
    while (true)
    {
//      double start = getCurrentTime();
//      processInput();
//      update();
//      render();
        producer1->publish();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}