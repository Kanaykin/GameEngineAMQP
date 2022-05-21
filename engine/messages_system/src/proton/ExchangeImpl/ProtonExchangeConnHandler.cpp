
#include "ProtonExchangeConnHandler.h"

#include <proton/sender_options.hpp>
#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/target.hpp>
#include <proton/source.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>

#include "ProtonExchangeSender.h"
#include "ProtonExchangeReceiver.h"
#include "ProtonExchangeQueueManager.h"

#include "MsgSysLog.h"

using namespace messages_system;

ProtonExchangeConnHandler::ProtonExchangeConnHandler(const ProtonExchangeQueueManagerWPtr& queues):
_queues(queues)
{
    
}

void ProtonExchangeConnHandler::on_sender_open(proton::sender &sender)
{
    INFO_LOG("ProtonExchangeConnHandler::on_sender_open ");
    std::string qn = sender.source().dynamic() ? "" : sender.source().address();
    auto s = std::make_shared<ProtonExchangeSender>(sender);//, senders_);
    _senders[sender] = s;
    if(!_queues.expired())
        _queues.lock()->findQueueSender(s.get(), qn);
}

void ProtonExchangeConnHandler::on_connection_open(proton::connection& c)
{
    INFO_LOG("ProtonExchangeConnHandler::on_connection_open ");
    c.open();            // Accept the connection
}


// A receiver receives messages from a publisher to a queue.
void ProtonExchangeConnHandler::on_receiver_open(proton::receiver &receiver)
{
    INFO_LOG("ProtonExchangeConnHandler::on_receiver_open ");

    const std::string qname = receiver.target().address();
    auto r = std::make_shared<ProtonExchangeReceiver>(receiver);//, senders_);
    
    _receivers[receiver] = r;
    
    if(!_queues.expired())
        _queues.lock()->findQueueReceiver(r.get(), qname);
    
//    receiver.open(proton::receiver_options()
//        .source((proton::source_options().address(qname))));
}

void ProtonExchangeConnHandler::on_session_close(proton::session &session)
{
    INFO_LOG("ProtonExchangeConnHandler::on_session_close ");
    // Unsubscribe all senders that belong to session.
//        for (proton::sender_iterator i = session.senders().begin(); i != session.senders().end(); ++i) {
//            senders::iterator j = senders_.find(*i);
//            if (j == senders_.end()) continue;
//            Sender* s = j->second;
//            if (s->queue_) {
//                s->queue_->add(make_work(&Queue::unsubscribe, s->queue_, s));
//            }
//            senders_.erase(j);
//        }
}

void ProtonExchangeConnHandler::on_error(const proton::error_condition& e)
{
    ERROR_LOG(" ProtonExchangeConnHandler protocol error: %1%", e.what());
}

void ProtonExchangeConnHandler::on_message(proton::delivery&, proton::message&)
{
    INFO_LOG("ProtonExchangeConnHandler::on_message ");
}

// The container calls on_transport_close() last.
void ProtonExchangeConnHandler::on_transport_close(proton::transport& t)
{
    INFO_LOG("ProtonExchangeConnHandler::on_transport_close ");
//        // Unsubscribe all senders.
//        for (proton::sender_iterator i = t.connection().senders().begin(); i != t.connection().senders().end(); ++i) {
//            senders::iterator j = senders_.find(*i);
//            if (j == senders_.end()) continue;
//            Sender* s = j->second;
//            if (s->queue_) {
//                s->queue_->add(make_work(&Queue::unsubscribe, s->queue_, s));
//            }
//        }
//        delete this;            // All done.
}
