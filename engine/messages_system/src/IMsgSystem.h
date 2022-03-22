#pragma once
#include <memory>
#include <functional>
#include <map>
#include <string>

#include "IProducer.h"

namespace messages_system
{

class IMsgSystem
{
public:
    virtual ~IMsgSystem() {}
    
    virtual IProducerPtr createProducer() = 0;
};

}
