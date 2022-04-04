#pragma once

#include <iostream>
#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/connection_options.hpp>

#include "IExchange.h"
#include "ProtonExchangeConnHandler.h"

namespace messages_system
{

class ProtonExchangeQueueManager;

class ProtonExchangeListener : public proton::listen_handler
{
public:
    typedef std::weak_ptr<ProtonExchangeQueueManager> ProtonExchangeQueueManagerWPtr;
    explicit ProtonExchangeListener(const ProtonExchangeQueueManagerWPtr& queues);

protected:
    
    proton::connection_options on_accept(proton::listener&) override ;

    void on_open(proton::listener& l) override ;

    void on_error(proton::listener&, const std::string& s) override ;
    
private:
    ProtonExchangeQueueManagerWPtr _queues;
};

}

