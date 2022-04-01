#pragma once
#include <memory>
#include <functional>
#include <map>
#include <string>

#include "IProducer.h"
#include "IConsumer.h"
#include "IExchange.h"

namespace messages_system
{

class IMsgSystem
{
public:
    virtual ~IMsgSystem() {}
    
    virtual IProducerPtr createProducer(const std::string& url) = 0;
    
    virtual IConsumerPtr createConsumer(const std::string& url) = 0;
    
    virtual IExchangePtr createExchange(const std::string& url) = 0;
    
//    virtual void run() = 0;
};

}
