#pragma once

#include <proton/receiver.hpp>
#include <proton/sender_options.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/container.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <condition_variable>
#include "IConsumer.h"

#include <iostream>

namespace messages_system
{

class ProtonConsumer : public proton::messaging_handler, public IConsumer
{
    class listener_ready_handler : public proton::listen_handler {
        void on_open(proton::listener& l) override {
            std::cout << "listening on " << l.port() << std::endl;
        }
    };
public:
    explicit ProtonConsumer(const ConsumerOptions& options);
    
    ~ProtonConsumer();
    
protected:
    void on_container_start(proton::container& c) override;
    
    // #TODO: Need error process after container close or container error
    
    void on_message(proton::delivery& delivery, proton::message& m) override;
    
    void on_sender_open(proton::sender &sender) override;
    
private:
    void restart();
    
    std::string _url;
    bool _direct = false;
    std::string _name;
    
    std::unique_ptr<proton::container>  _container;
    std::unique_ptr<std::thread>        _thread;
    std::mutex                          _lock;
    
    proton::receiver _receiver;
    proton::connection _connection;
    listener_ready_handler _listen_handler;
    
    typedef std::map<std::string, proton::sender> sender_map;
    sender_map senders;
};

}
