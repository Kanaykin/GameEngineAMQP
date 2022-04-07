
#include "ProtonExchangeQueueManager.h"

using namespace messages_system;

ProtonExchangeQueueManager::ProtonExchangeQueueManager(const std::weak_ptr<proton::container>& container):
_container(container),
_workQueue(*container.lock().get())
{
    
}

bool ProtonExchangeQueueManager::add(const proton::work& f)
{
    return _workQueue.add(f);
}

void ProtonExchangeQueueManager::findQueueSender(ProtonExchangeSender* s, const std::string& qn)
{
//    add(proton::make_work([=](){ findQueue(s, qn);}));
    findQueue(s, qn);
}

void ProtonExchangeQueueManager::findQueueReceiver(ProtonExchangeReceiver* r, const std::string& qn)
{
//    add(proton::make_work([=](){ findQueue(r, qn);}));
    findQueue(r, qn);
}
