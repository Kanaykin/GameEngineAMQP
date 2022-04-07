#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
    auto consumer = msgSystem->createConsumer({"127.0.0.1:5672", true});
    auto producer = msgSystem->createProducer({"127.0.0.1:5672"});
    
    while (true)
    {
        producer->publish();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}
