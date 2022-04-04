
#include "ProtonExchangeConnHandler.h"

#include <proton/sender_options.hpp>
#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/target.hpp>
#include <proton/source.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>

#include "ProtonExchangeSender.h"

using namespace messages_system;

ProtonExchangeConnHandler::ProtonExchangeConnHandler(const ProtonExchangeQueueManagerWPtr& queues):
_queues(queues)
{
    
}

void ProtonExchangeConnHandler::on_sender_open(proton::sender &sender)
{
    std::cout << "ProtonExchangeConnHandler::on_sender_open " << std::endl;
    std::string qn = sender.source().dynamic() ? "" : sender.source().address();
    auto s = std::make_shared<ProtonExchangeSender>(sender);//, senders_);
    _senders[sender] = s;
//        queue_manager_.add(make_work(&QueueManager::findQueueSender, &queue_manager_, s, qn));
}

void ProtonExchangeConnHandler::on_connection_open(proton::connection& c)
{
    std::cout << "ProtonExchangeConnHandler::on_connection_open " << std::endl;
    c.open();            // Accept the connection
}


// A receiver receives messages from a publisher to a queue.
void ProtonExchangeConnHandler::on_receiver_open(proton::receiver &receiver)
{
    std::cout << "ProtonExchangeConnHandler::on_receiver_open " << std::endl;

    const std::string qname = receiver.target().address();
    receiver.open(proton::receiver_options()
        .source((proton::source_options().address(qname))));
//        std::string qname = receiver.target().address();
//        Receiver* r = new Receiver(receiver);
//        queue_manager_.add(make_work(&QueueManager::findQueueReceiver, &queue_manager_, r, qname));
}

void ProtonExchangeConnHandler::on_session_close(proton::session &session)
{
    std::cout << "ProtonExchangeConnHandler::on_session_close " << std::endl;
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
    std::cout << "protocol error: " << e.what() << std::endl;
}

void ProtonExchangeConnHandler::on_message(proton::delivery&, proton::message&)
{
    std::cout << "ProtonExchangeConnHandler::on_message " << std::endl;
}

// The container calls on_transport_close() last.
void ProtonExchangeConnHandler::on_transport_close(proton::transport& t)
{
    std::cout << "ProtonExchangeConnHandler::on_transport_close " << std::endl;
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
