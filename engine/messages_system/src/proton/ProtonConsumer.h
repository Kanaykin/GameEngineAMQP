#pragma once

#include <proton/receiver.hpp>
#include <proton/sender_options.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/container.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
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
    explicit ProtonConsumer(const std::string& url);
    
    ~ProtonConsumer();
    
protected:
    void on_container_start(proton::container& c) override;
    
//    void on_connection_open(proton::connection& connection) override;
    
    void on_message(proton::delivery& delivery, proton::message& m) override;
    
    void on_sender_open(proton::sender &sender) override;
    
private:
    void restart();
    
    std::string _url;
    
    std::unique_ptr<proton::container>  _container;
    std::unique_ptr<std::thread>        _thread;
    
//    proton::receiver _receiver;
//    proton::connection _connection;
    listener_ready_handler _listen_handler;
    
    typedef std::map<std::string, proton::sender> sender_map;
    sender_map senders;
};

}
