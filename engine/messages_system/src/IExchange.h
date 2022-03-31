#pragma once

#include <memory>

namespace messages_system
{

class IExchange
{
public:
    virtual ~IExchange() {}
    
};

typedef std::shared_ptr<IExchange> IExchangePtr;

}

