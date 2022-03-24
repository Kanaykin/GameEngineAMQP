#pragma once

#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/sender_options.hpp>
#include <thread>
#include "IProducer.h"

namespace messages_system
{

class ProtonProducer : public IProducer
{
public:
    explicit ProtonProducer(const proton::sender& sender);
    
    ~ProtonProducer();
    
    void publish() override;
    
private:
    proton::work_queue* work_queue();
    
    proton::sender _sender;
    proton::receiver _receiver;
    proton::work_queue* _work_queue = nullptr;
    std::mutex lock_;
};

}
