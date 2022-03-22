#pragma once

#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>

#include "IMsgSystem.h"

namespace messages_system
{

class ProtonMsgSystem: public proton::messaging_handler, public IMsgSystem
{
public:
    ProtonMsgSystem();
    
    ~ProtonMsgSystem();
    
    IProducerPtr createProducer() override;
    
    void on_container_start(proton::container &c) override;
    
private:
    std::unique_ptr<proton::container> _container;
    std::unique_ptr<std::thread> _thread;
};

}
