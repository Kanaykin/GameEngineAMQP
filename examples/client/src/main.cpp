#include <assert.h>
#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"

int main()
{
    auto msgSystem = messages_system::createMsgSystem("ProtonMsg");
    auto producer = msgSystem->createProducer();
    return 0;
}
