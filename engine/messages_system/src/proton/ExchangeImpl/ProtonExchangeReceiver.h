#pragma once

#include <proton/messaging_handler.hpp>
#include <proton/receiver.hpp>
#include <proton/message.hpp>

#include <deque>

namespace messages_system
{

class ProtonExchangeQueue;
typedef std::weak_ptr<ProtonExchangeQueue> ProtonExchangeQueueWPtr;

class ProtonExchangeReceiver : public proton::messaging_handler
{
public:
    explicit ProtonExchangeReceiver(const proton::receiver& receiver/*, senders& ss*/) :
    _receiver(receiver)//, senders_(ss), work_queue_(s.work_queue()), queue_(0), pending_credit_(0)
    {}
    
    void boundQueue(const ProtonExchangeQueueWPtr& q, const std::string& qn);
    
protected:
    // Messaging handlers
    void on_message(proton::delivery &, proton::message &m) override;
    
    void queueMsgs();
    
private:
    proton::receiver _receiver;
    std::deque<proton::message> _messages;
    ProtonExchangeQueueWPtr _queue;
};


}
