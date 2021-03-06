#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"

static const std::chrono::duration<double> LIVE_TIME(2.0);

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
    auto consumer = msgSystem->createConsumer({"127.0.0.1:5672", true, "consumer"});
    
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        if (elapsed_seconds > LIVE_TIME)
            break;
    }
    
    return 0;
}
