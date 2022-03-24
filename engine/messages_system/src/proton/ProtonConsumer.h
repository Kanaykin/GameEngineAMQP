#pragma once

#include <proton/receiver.hpp>
#include <proton/sender_options.hpp>
#include "IConsumer.h"

namespace messages_system
{

class ProtonConsumer : public IConsumer
{
public:
    explicit ProtonConsumer(const proton::receiver& receiver);
    
    ~ProtonConsumer();
    
private:
    proton::receiver _receiver;
};

}
