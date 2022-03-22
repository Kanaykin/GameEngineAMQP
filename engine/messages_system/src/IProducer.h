#pragma once 

#include <memory>

namespace messages_system
{

class IProducer
{
public:
    virtual ~IProducer() {}
    
    virtual void publish() = 0;
};

typedef std::shared_ptr<IProducer> IProducerPtr;

}
