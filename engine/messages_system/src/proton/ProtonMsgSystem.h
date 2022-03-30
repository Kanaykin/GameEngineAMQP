#pragma once

#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>

#include <iostream>

#include "IMsgSystem.h"

namespace messages_system
{

class ProtonMsgSystem: public proton::messaging_handler, public IMsgSystem
{
private:
    class listener_ready_handler : public proton::listen_handler {
        void on_open(proton::listener& l) override {
            std::cout << "listening on " << l.port() << std::endl;
        }
    };
public:
    ProtonMsgSystem();
    
    ~ProtonMsgSystem();
    
    IProducerPtr createProducer(const std::string& url) override;
    
    IConsumerPtr createConsumer(const std::string& url) override;
    
    void on_container_start(proton::container &c) override;
    
    void on_container_stop(proton::container &c) override;
    
    void on_connection_close(proton::connection& connection) override;
    
    void on_sender_open(proton::sender &sender) override;
    
    void on_sender_detach(proton::sender&) override;

    void on_sender_close(proton::sender&) override;
    
    void on_message(proton::delivery&, proton::message&) override;
    
    void on_error(const proton::error_condition& error) override;
    
private:
    std::unique_ptr<proton::container> _container;
    std::unique_ptr<std::thread> _thread;
    bool _container_started = false;
    proton::listen_handler _listener;
    
    std::mutex _lock;
    std::condition_variable _sender_ready;
    bool work_queue_ = false;
    proton::sender sender;
};

}
