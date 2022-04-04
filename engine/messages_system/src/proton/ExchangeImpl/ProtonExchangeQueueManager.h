#pragma once

#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/work_queue.hpp>

namespace messages_system
{

class ProtonExchangeQueue;

class ProtonExchangeQueueManager
{
private:
    typedef std::shared_ptr<ProtonExchangeQueue> ProtonExchangeQueuePtr;
    typedef std::map<std::string, ProtonExchangeQueuePtr> _queues;
    proton::work_queue _workQueue;
};

}
