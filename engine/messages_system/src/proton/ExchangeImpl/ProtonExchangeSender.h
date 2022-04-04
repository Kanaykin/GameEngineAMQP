#pragma once

#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>

namespace messages_system
{

class ProtonExchangeSender : public proton::messaging_handler
{
//    friend class connection_handler;
    
public:
    explicit ProtonExchangeSender(proton::sender s/*, senders& ss*/) :
    _sender(s)//, senders_(ss), work_queue_(s.work_queue()), queue_(0), pending_credit_(0)
    {}
    
protected:
    // Messaging handlers
    void on_sendable(proton::sender &sender) override{};
    void on_sender_close(proton::sender &sender) override{};
    
private:
    proton::sender _sender;
};


}
