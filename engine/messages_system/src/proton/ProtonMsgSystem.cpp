#include <proton/error.hpp>
#include <proton/message.hpp>
#include <proton/work_queue.hpp>

#include "ProtonMsgSystem.h"
#include "ProtonProducer.h"
#include "ProtonConsumer.h"
#include "ProtonExchange.h"

#include <iostream>

using namespace messages_system;

ProtonMsgSystem::ProtonMsgSystem()
{
}

ProtonMsgSystem::~ProtonMsgSystem()
{
}

IProducerPtr ProtonMsgSystem::createProducer(const ProducerOptions& options)
{
    return std::make_shared<ProtonProducer>(options);
}

IConsumerPtr ProtonMsgSystem::createConsumer(const ConsumerOptions& options)
{
    return std::make_shared<ProtonConsumer>(options);
}

IExchangePtr ProtonMsgSystem::createExchange(const ExchangeOptions& options)
{
    return std::make_shared<ProtonExchange>(options);
}

