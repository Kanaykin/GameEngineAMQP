#pragma once

#include <memory>

namespace messages_system
{

struct ExchangeOptions
{
    std::string url;
};

class IExchange
{
public:
    virtual ~IExchange() {}
    
};

typedef std::shared_ptr<IExchange> IExchangePtr;

}

