#pragma once

#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/work_queue.hpp>

#include "ProtonExchangeQueue.h"
#include "ProtonExchangeSender.h"
#include "ProtonExchangeReceiver.h"

namespace messages_system
{

class ProtonExchangeQueueManager
{
public:
    explicit ProtonExchangeQueueManager(const std::weak_ptr<proton::container>& container);
    
    void findQueueSender(ProtonExchangeSender* s, const std::string& qn);

    void findQueueReceiver(ProtonExchangeReceiver* r, const std::string& qn);
private:
    bool add(const proton::work& f);
    
    template <class T>
    void findQueue(T& connection, const std::string& qn);
    
    typedef std::shared_ptr<ProtonExchangeQueue> ProtonExchangeQueuePtr;
    typedef std::map<std::string, ProtonExchangeQueuePtr> ProtonExchangeQueueMap;
    
    ProtonExchangeQueueMap _queues;
    proton::work_queue _workQueue;
    std::weak_ptr<proton::container> _container;
};

template <class T>
void ProtonExchangeQueueManager::findQueue(T& connection, const std::string& qn)
{
    // #TODO: implement method of creating of dynamic id
//        if (qn.empty()) {
//            // Dynamic queue creation
//            std::ostringstream os;
//            os << "_dynamic_" << next_id_++;
//            qn = os.str();
//        }
    ProtonExchangeQueuePtr q;
    ProtonExchangeQueueMap::iterator i = _queues.find(qn);
    if (i==_queues.end())
    {
        q = std::make_shared<ProtonExchangeQueue>(_container, qn);
        _queues[qn] = q;
    }
    else
    {
        q = i->second;
    }
//    _workQueue.add(proton::make_work([=](){ connection->boundQueue(q, qn);}));
    connection->boundQueue(q, qn);
}

}
