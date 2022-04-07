#pragma once

#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/message.hpp>
#include <proton/work_queue.hpp>

namespace messages_system
{

class ProtonExchangeQueue;
typedef std::weak_ptr<ProtonExchangeQueue> ProtonExchangeQueueWPtr;

class ProtonExchangeSender : public proton::messaging_handler
{
//    friend class connection_handler;
    
public:
    explicit ProtonExchangeSender(const proton::sender& s/*, senders& ss*/) :
    _sender(s), _workQueue(s.work_queue())//, queue_(0), pending_credit_(0)
    {}
    
    void boundQueue(const ProtonExchangeQueueWPtr& q, const std::string& qn);
    
    void sendMsg(const proton::message& m);
    
protected:
    // Messaging handlers
    void on_sendable(proton::sender &sender) override;
    void on_sender_close(proton::sender &sender) override;
    
private:
    proton::sender _sender;
    ProtonExchangeQueueWPtr _queue;
    proton::work_queue& _workQueue;
};


}
