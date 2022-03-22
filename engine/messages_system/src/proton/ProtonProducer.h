#pragma once

#include <proton/container.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/message.hpp>
#include <proton/message_id.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/sender.hpp>
#include <proton/sender_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>
#include <proton/delivery.hpp>
#include <proton/receiver_options.hpp>
#include "IProducer.h"

namespace messages_system
{

class ProtonProducer : public IProducer
{
public:
    explicit ProtonProducer(const proton::sender& sender);
    
    ~ProtonProducer();
    
    void publish() override {}
    
private:
    proton::sender _sender;
};

}
