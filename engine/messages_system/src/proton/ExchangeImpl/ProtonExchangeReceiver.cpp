#include "ProtonExchangeReceiver.h"

#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>

#include "ProtonExchangeQueue.h"

#include <iostream>

using namespace messages_system;

void ProtonExchangeReceiver::on_message(proton::delivery &, proton::message &m)
{
    std::cout << "ProtonExchangeReceiver::on_message " << std::endl;
    
    _messages.push_back(m);

    if (!_queue.expired())
    {
        queueMsgs();
    }
}

void ProtonExchangeReceiver::queueMsgs()
{
//    DOUT(std::cerr << "Receiver: " << this << " queueing " << messages_.size() << " msgs to: " << queue_ << "\n";);
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
    std::cout <<  "ProtonExchangeReceiver::boundQueue bound to Queue:  (" << qn << ")\n"  << std::endl;
    _queue = q;
    _receiver.open(proton::receiver_options()
        .source((proton::source_options().address(qn)))
        .handler(*this));
    std::cout << "receiving to " << qn << std::endl;

    queueMsgs();
}
