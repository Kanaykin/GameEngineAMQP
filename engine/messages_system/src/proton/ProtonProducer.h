#pragma once

#include <proton/sender.hpp>
#include <proton/receiver.hpp>
#include <proton/sender_options.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/container.hpp>
#include <thread>
#include "IProducer.h"

namespace messages_system
{

class ProtonProducer : public proton::messaging_handler, public IProducer
{
public:
    explicit ProtonProducer(const ProducerOptions& options);
    
    ~ProtonProducer();
    
    void publish() override;
    
//    void setSender(const proton::sender& sender);
    
protected:
    
    void on_container_start(proton::container &c) override;
    
    void on_container_stop(proton::container &c) override;
    
    void on_error(const proton::error_condition& error) override;
    
    void on_sender_open(proton::sender &s) override;
    
    void on_receiver_open(proton::receiver &) override;
    
    void on_message(proton::delivery&, proton::message&) override;
    
private:
    void restart();
    proton::work_queue* work_queue();
    
    std::unique_ptr<proton::container>  _container;
    std::unique_ptr<std::thread>        _thread;
    
    proton::sender      _sender;
    proton::receiver    _receiver;
    
    std::string _url;
//

    proton::work_queue* _workQueue = nullptr;
    std::mutex _lock;
};

}
