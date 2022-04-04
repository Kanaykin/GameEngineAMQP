#pragma once

#include <iostream>
#include <thread>

#include <proton/messaging_handler.hpp>
#include <proton/connection.hpp>
#include <proton/message.hpp>

namespace messages_system
{

class ProtonExchangeSender;
class ProtonExchangeQueueManager;

class ProtonExchangeConnHandler : public proton::messaging_handler
{
//    QueueManager& queue_manager_;
//    senders senders_;

public:
    typedef std::weak_ptr<ProtonExchangeQueueManager> ProtonExchangeQueueManagerWPtr;
    explicit ProtonExchangeConnHandler(const ProtonExchangeQueueManagerWPtr& queues);

    void on_connection_open(proton::connection& c) override;

    // A sender sends messages from a queue to a subscriber.
    void on_sender_open(proton::sender &sender) override;

    // A receiver receives messages from a publisher to a queue.
    void on_receiver_open(proton::receiver &receiver) override;

    void on_session_close(proton::session &session) override;
    void on_error(const proton::error_condition& e) override;
    
    void on_message(proton::delivery&, proton::message&) override;

    // The container calls on_transport_close() last.
    void on_transport_close(proton::transport& t) override ;
    
    typedef std::shared_ptr<ProtonExchangeSender> ProtonExchangeSenderPtr;
    typedef std::map<proton::sender, ProtonExchangeSenderPtr> ProtonExchangeSenderMap;
    
    ProtonExchangeSenderMap _senders;
    ProtonExchangeQueueManagerWPtr _queues;
};

}
