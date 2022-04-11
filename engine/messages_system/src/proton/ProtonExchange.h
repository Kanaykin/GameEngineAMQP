#pragma once

#include <iostream>
#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/connection_options.hpp>

#include "IExchange.h"

namespace messages_system
{

class ProtonExchangeListener;
class ProtonExchangeQueueManager;

class ProtonExchange : public IExchange
{
    friend class ProtonMsgSystem;
public:
    explicit ProtonExchange(const ExchangeOptions& options);
    
    ~ProtonExchange() override;
    
private:
    typedef std::shared_ptr<ProtonExchangeQueueManager> ProtonExchangeQueueManagerPtr;
    
    std::shared_ptr<proton::container> _container;
    std::unique_ptr<ProtonExchangeListener> _listener;
    ProtonExchangeQueueManagerPtr _queues;
    
    std::unique_ptr<std::thread> _thread;
};

}

