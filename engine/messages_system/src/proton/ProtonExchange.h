#pragma once

#include <iostream>
#include <thread>

#include <proton/container.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/listen_handler.hpp>
#include <proton/listener.hpp>
#include <proton/connection_options.hpp>

#include "IExchange.h"
#include "proton/ProtonExchangeConnHandler.h"

namespace messages_system
{

class ProtonExchange : public IExchange
{
    friend class ProtonMsgSystem;
public:
    explicit ProtonExchange(const std::string& url);
    
    ~ProtonExchange();
    
private:
    struct ProtonExchangeListener : public proton::listen_handler
    {
//        listener(QueueManager& c) : queues_(c) {}

        proton::connection_options on_accept(proton::listener&) override{
            std::cout << "ProtonExchangeListener on_accept " << std::endl;
            return proton::connection_options().handler(*(new ProtonExchangeConnHandler(/*queues_*/)));
//            return proton::connection_options();
        }

        void on_open(proton::listener& l) override {
            std::cout << "ProtonExchangeListener listening on " << l.port() << std::endl;
        }

        void on_error(proton::listener&, const std::string& s) override {
            std::cerr << "ProtonExchangeListener listen error: " << s << std::endl;
            throw std::runtime_error(s);
        }
//        QueueManager& queues_;
    };
    
    std::shared_ptr<proton::listen_handler> getListener() const;
    
//    std::unique_ptr<proton::container> _container;
//    QueueManager queues_;
    std::shared_ptr<ProtonExchangeListener> _listener;
    
    std::unique_ptr<std::thread> _thread;
};

}

