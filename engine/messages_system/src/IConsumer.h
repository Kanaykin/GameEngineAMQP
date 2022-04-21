#pragma once

#include <memory>

namespace messages_system
{

struct ConsumerOptions
{
    std::string url;
    bool direct = false;
    std::string name;
};

class IConsumer
{
public:
    virtual ~IConsumer() {}
    
};

typedef std::shared_ptr<IConsumer> IConsumerPtr;

}
