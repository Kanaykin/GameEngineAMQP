#include "ProtonExchangeReceiver.h"

#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>

#include "ProtonExchangeQueue.h"

#include <iostream>

#include "MsgSysLog.h"

using namespace messages_system;

void ProtonExchangeReceiver::on_message(proton::delivery &, proton::message &m)
{
    INFO_LOG("ProtonExchangeReceiver::on_message ");
    
    _messages.push_back(m);

    if (!_queue.expired())
    {
        queueMsgs();
    }
}

void ProtonExchangeReceiver::queueMsgs()
{
    INFO_LOG("ProtonExchangeReceiver:: Receiver  queueing %1%  msgs", _messages.size());
    while (!_messages.empty())
    {
//        queue_->add(make_work(&Queue::queueMsg, queue_, messages_.front()));
        if (!_queue.expired())
        {
            _queue.lock()->queueMsg(_messages.front());
        }
        _messages.pop_front();
    }
}

void ProtonExchangeReceiver::boundQueue(const ProtonExchangeQueueWPtr& q, const std::string& qn)
{
    INFO_LOG("ProtonExchangeReceiver::boundQueue bound to Queue:  (%1%)\n", qn);
    _queue = q;
    _receiver.open(proton::receiver_options()
        .source((proton::source_options().address(qn)))
        .handler(*this));
    INFO_LOG("ProtonExchangeReceiver::receiving to %1%", qn);

    queueMsgs();
}
