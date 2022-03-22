#include "MsgSystemCreator.h"
#include "proton/ProtonMsgSystem.h"

namespace messages_system
{
    
IMsgSystemPtr createMsgSystem(const std::string& name)
{
    static RegisterCreateSystem<ProtonMsgSystem> protonMsgSystem("ProtonMsg");
    return creatorMsgSystems[name]();
}

}
