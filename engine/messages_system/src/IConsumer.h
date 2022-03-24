#pragma once

#include <memory>

namespace messages_system
{

class IConsumer
{
public:
    virtual ~IConsumer() {}
    
};

typedef std::shared_ptr<IConsumer> IConsumerPtr;

}
