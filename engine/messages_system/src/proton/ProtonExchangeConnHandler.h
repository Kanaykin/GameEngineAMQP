#pragma once

#include <iostream>
#include <thread>

#include <proton/messaging_handler.hpp>
#include <proton/connection.hpp>
#include <proton/message.hpp>

namespace messages_system
{

class ProtonExchangeConnHandler : public proton::messaging_handler
{
//    QueueManager& queue_manager_;
//    senders senders_;

public:
    ProtonExchangeConnHandler(/*QueueManager& qm*/)// :
//        queue_manager_(qm)
    {}

    void on_connection_open(proton::connection& c) override {
        std::cout << "ProtonExchangeConnHandler::on_connection_open " << std::endl;
//        c.open();            // Accept the connection
    }

    // A sender sends messages from a queue to a subscriber.
    void on_sender_open(proton::sender &sender) override {
        std::cout << "ProtonExchangeConnHandler::on_sender_open " << std::endl;
//        std::string qn = sender.source().dynamic() ? "" : sender.source().address();
//        Sender* s = new Sender(sender, senders_);
//        senders_[sender] = s;
//        queue_manager_.add(make_work(&QueueManager::findQueueSender, &queue_manager_, s, qn));
        
//        proton::message req;
//        req.body("test");
//        req.address("render");
//        req.reply_to("client");
//    //    req.reply_to(_receiver.source().address());
//        
//        sender.send(req);
    }

    // A receiver receives messages from a publisher to a queue.
    void on_receiver_open(proton::receiver &receiver) override {
        std::cout << "ProtonExchangeConnHandler::on_receiver_open " << std::endl;
//        std::string qname = receiver.target().address();
//        Receiver* r = new Receiver(receiver);
//        queue_manager_.add(make_work(&QueueManager::findQueueReceiver, &queue_manager_, r, qname));
    }

    void on_session_close(proton::session &session) override {
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

    void on_error(const proton::error_condition& e) override {
        std::cout << "protocol error: " << e.what() << std::endl;
    }
    
    void on_message(proton::delivery&, proton::message&) override
    {
        std::cout << "ProtonExchangeConnHandler::on_message " << std::endl;
    }

    // The container calls on_transport_close() last.
    void on_transport_close(proton::transport& t) override {
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
};

}
