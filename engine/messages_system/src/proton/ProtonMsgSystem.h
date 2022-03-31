#pragma once

#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/work_queue.hpp>
#include <unordered_map>

#include <iostream>

#include "IMsgSystem.h"

namespace messages_system
{

class ProtonMsgListener;

class ProtonMsgSystem: public proton::messaging_handler, public IMsgSystem
{
public:
    ProtonMsgSystem();
    
    ~ProtonMsgSystem();
    
    void run() override;
    
    IProducerPtr createProducer(const std::string& url) override;
    
    IConsumerPtr createConsumer(const std::string& url) override;
    
    IExchangePtr createExchange(const std::string& url) override;
    
private:
    typedef std::shared_ptr<ProtonMsgListener> ProtonMsgListenerPtr;
    
    void on_container_start(proton::container &c) override;
    
    void on_container_stop(proton::container &c) override;
    
    void on_connection_close(proton::connection& connection) override;
    
    void on_sender_open(proton::sender &sender) override;
    
    void on_sender_detach(proton::sender&) override;

    void on_sender_close(proton::sender&) override;
    
    void on_message(proton::delivery&, proton::message&) override;
    
    void on_error(const proton::error_condition& error) override;
    
    ProtonMsgListenerPtr getListener(const std::string& url) const;
    
private:
    std::unique_ptr<proton::container> _container;
    std::unique_ptr<std::thread> _thread;
    bool _container_started = false;
    
    mutable std::mutex _lock;
    std::vector<proton::work> _work_queue;
    
    typedef std::unordered_map<std::string, ProtonMsgListenerPtr> ProtonMsgListenerMap;
    mutable ProtonMsgListenerMap _listeners;;
};

}
