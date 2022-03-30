#include <proton/error.hpp>
#include <proton/message.hpp>
#include <proton/work_queue.hpp>

#include "ProtonMsgSystem.h"
#include "ProtonProducer.h"
#include "ProtonConsumer.h"

#include <iostream>

using namespace messages_system;

ProtonMsgSystem::ProtonMsgSystem():
_container(std::make_unique<proton::container>(*this))
{
    _thread = std::make_unique<std::thread>([this]() {
        try
        {
            _container->run();
        }
        catch (const proton::error& e)
        {
            std::cout << "Error " << e.what() << std::endl;
        }
    }
                                                );
}

ProtonMsgSystem::~ProtonMsgSystem()
{
    _container->stop();
    _thread->join();
}

IProducerPtr ProtonMsgSystem::createProducer(const std::string& url)
{
    std::unique_lock<std::mutex> l(_lock);
    auto duration = proton::duration(10);
    auto work = proton::work([=](){
        _container->open_sender(url);
    });
    _container->schedule(duration, work);

    work_queue_ = false;
    while (!work_queue_) _sender_ready.wait(l);
    
    auto sender = _container->open_sender(url);
    return std::make_shared<ProtonProducer>(sender);
}

IConsumerPtr ProtonMsgSystem::createConsumer(const std::string& url)
{
    auto receiver = _container->open_receiver(url);
    return std::make_shared<ProtonConsumer>(receiver);
}

void ProtonMsgSystem::on_container_start(proton::container &c)
{
    std::cout << "on_container_start " << std::endl;
    c.listen("localhost:5673/render", _listener);
//    c.connect("127.0.0.1:5672");
    _container_started = true;
}

void ProtonMsgSystem::on_container_stop(proton::container &c)
{
    std::cout << "on_container_stop " << std::endl;
}

void ProtonMsgSystem::on_sender_open(proton::sender &s)
{
    std::cout << "on_sender_open " << std::endl;
    std::lock_guard<std::mutex> l(_lock);
    work_queue_ = true;
    sender = sender;
    _sender_ready.notify_all();
}

void ProtonMsgSystem::on_sender_detach(proton::sender&)
{
    std::cout << "on_sender_detach " << std::endl;
}

void ProtonMsgSystem::on_sender_close(proton::sender&)
{
    std::cout << "on_sender_close " << std::endl;
}

void ProtonMsgSystem::on_connection_close(proton::connection& connection)
{
    std::cout << "on_connection_close " << std::endl;
}

void ProtonMsgSystem::on_message(proton::delivery& delivery, proton::message& m)
{
    std::string reply_to = m.reply_to();
    std::cout << "Received " << m.body() << " reply_to" << reply_to << std::endl;
}

void ProtonMsgSystem::on_error(const proton::error_condition& error)
{
    std::cout << "On error " << error.name() << " description " << error.description() << std::endl;
}
