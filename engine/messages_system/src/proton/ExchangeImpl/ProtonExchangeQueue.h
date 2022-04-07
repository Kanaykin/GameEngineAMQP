#pragma once

#include <proton/container.hpp>
#include <proton/message.hpp>
#include <deque>

namespace messages_system
{
class ProtonExchangeSender;

class ProtonExchangeQueue
{
public:
    ProtonExchangeQueue(const std::weak_ptr<proton::container>& c, const std::string& n);
    
    void queueMsg(const proton::message& m);
    
    void subscribe(ProtonExchangeSender* s);
    
    void flow(ProtonExchangeSender* s, int c);
    
private:
    void tryToSend();
    
private:
    typedef std::map<ProtonExchangeSender*, int> SubscriptionsMap; // With credit
    
    const std::string _name;
    std::weak_ptr<proton::container> _container;
    std::deque<proton::message> _messages;
    SubscriptionsMap _subscriptions;
    SubscriptionsMap::iterator _current = _subscriptions.end();
    
};


}
