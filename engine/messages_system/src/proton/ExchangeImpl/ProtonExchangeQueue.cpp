
#include "ProtonExchangeQueue.h"
#include "ProtonExchangeSender.h"

#include <proton/message.hpp>

using namespace messages_system;

ProtonExchangeQueue::ProtonExchangeQueue(const std::weak_ptr<proton::container>& c, const std::string& n):
_container(c),
_name(n)
{
    
}

void ProtonExchangeQueue::queueMsg(const proton::message& m)
{
//    DOUT(std::cerr << "Queue:    " << this << "(" << name_ << ") queueMsg\n";);
    _messages.push_back(m);
    tryToSend();
}

void ProtonExchangeQueue::subscribe(ProtonExchangeSender* s)
{
//    DOUT(std::cerr << "Queue:    " << this << "(" << name_ << ") subscribe Sender: " << s << "\n";);
    _subscriptions[s] = 0;
}

void ProtonExchangeQueue::flow(ProtonExchangeSender* s, int c)
{
//    DOUT(std::cerr << "Queue:    " << this << "(" << name_ << ") flow: " << c << " to " << s << "\n";);
    _subscriptions[s] = c;
    tryToSend();
}

void ProtonExchangeQueue::tryToSend()
{
    int outOfCredit = 0;
    while (!_messages.empty() && outOfCredit < _subscriptions.size())
    {
        // If we got the end (or haven't started yet) start at the beginning
        if (_current == _subscriptions.end())
        {
            _current = _subscriptions.begin();
        }
        // If we have credit send the message
//        DOUT(std::cerr << "(" << current_->second << ") ";);
        if (_current->second > 0)
        {
//            DOUT(std::cerr << _current->first << " ";);
            _current->first->sendMsg(_messages.front());
            _messages.pop_front();
            --_current->second;
            ++_current;
        } else
        {
            ++outOfCredit;
        }
    }

}
