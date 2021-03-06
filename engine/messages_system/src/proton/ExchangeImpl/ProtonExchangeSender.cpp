
#include "ProtonExchangeSender.h"
#include "ProtonExchangeQueue.h"

#include "proton/sender_options.hpp"
#include "proton/source_options.hpp"
#include <proton/message.hpp>

#include <iostream>

#include "MsgSysLog.h"

using namespace messages_system;

void ProtonExchangeSender::boundQueue(const ProtonExchangeQueueWPtr& q, const std::string& qn)
{
    _queue = q;
    if(!q.expired())
        q.lock()->subscribe(this);

    _sender.open(proton::sender_options()
        .source((proton::source_options().address(qn)))
        .handler(*this));
    // #TODO: Need deal with pending_credit_
//    if (pending_credit_>0) {
//        queue_->add(make_work(&Queue::flow, queue_, this, pending_credit_));
//    }
    INFO_LOG("ProtonExchangeSender:: sending");
}

void ProtonExchangeSender::sendMsg(const proton::message& m)
{
//    DOUT(std::cerr << "Sender:   " << this << " sending\n";);
    _workQueue.add(proton::make_work([=](){_sender.send(m);}));
}

void ProtonExchangeSender::on_sendable(proton::sender &sender)
{
    if (!_queue.expired())
    {
        _queue.lock()->flow(this, sender.credit());
    }
    else
    {
        // #TODO: Need deal with pending_credit_
//        pending_credit_ = sender.credit();
    }
}

void ProtonExchangeSender::on_sender_close(proton::sender &sender)
{
    INFO_LOG("ProtonExchangeSender::on_sender_close ");
}
