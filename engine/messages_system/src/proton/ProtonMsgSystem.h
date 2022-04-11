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

class ProtonMsgSystem: public IMsgSystem
{
public:
    ProtonMsgSystem();
    
    ~ProtonMsgSystem() override;
    
    IProducerPtr createProducer(const ProducerOptions& options) override;
    
    IConsumerPtr createConsumer(const ConsumerOptions& options) override;
    
    IExchangePtr createExchange(const ExchangeOptions& options) override;
};

}
