#include <proton/error.hpp>
#include <proton/message.hpp>
#include <proton/work_queue.hpp>

#include "ProtonMsgSystem.h"
#include "ProtonProducer.h"
#include "ProtonConsumer.h"
#include "ProtonExchange.h"
#include "ProtonMsgListener.h"

#include <iostream>

using namespace messages_system;

ProtonMsgSystem::ProtonMsgSystem():
_container(std::make_unique<proton::container>(*this))
{
}

void ProtonMsgSystem::run()
{
    if (_thread)
    {
        // already run
        assert(false);
        return;
    }
    _thread = std::make_unique<std::thread>([this]() {
        try
        {
            _container->run();
        }
        catch (const proton::error& e)
        {
            std::cout << "Error " << e.what() << std::endl;
        }
    });
}

ProtonMsgSystem::~ProtonMsgSystem()
{
    _container->stop();
    if (_thread)
        _thread->join();
}

IProducerPtr ProtonMsgSystem::createProducer(const std::string& url)
{
    std::unique_lock<std::mutex> l(_lock);

    getListener(url);
    
    auto producer = std::make_shared<ProtonProducer>();
    auto&& openSender = proton::work([=](){
        auto sender = _container->open_sender(url);
        producer->setSender(sender);
    });
    if (!_container_started)
    {
        _work_queue.emplace_back(openSender);
    }
    else
    {
        auto&& duration = proton::duration(10000);
        _container->schedule(duration, openSender);
    }
    
    return producer;
}

IConsumerPtr ProtonMsgSystem::createConsumer(const std::string& url)
{
    auto receiver = _container->open_receiver(url);
    return std::make_shared<ProtonConsumer>(receiver);
}

IExchangePtr ProtonMsgSystem::createExchange(const std::string& url)
{
    std::unique_lock<std::mutex> l(_lock);
    // We can create exchange after container started.
    auto listener = getListener(url);
    auto exchange = std::make_shared<ProtonExchange>(url);
    listener->addListener(exchange->getListener());
    return exchange;
}

ProtonMsgSystem::ProtonMsgListenerPtr ProtonMsgSystem::getListener(const std::string& url) const
{
    auto iter = _listeners.find(url);
    if (iter != _listeners.end())
    {
        return (*iter).second;
    }
    auto listener = std::make_shared<ProtonMsgListener>();
    _listeners[url] = listener;
    return listener;
}

void ProtonMsgSystem::on_container_start(proton::container &c)
{
    std::unique_lock<std::mutex> l(_lock);
    
    std::cout << "on_container_start " << std::endl;
//    c.listen("localhost:5673/render", _listener);
    
    for(auto& [key, value] : _listeners)
    {
        c.listen(key, *value.get());
    }
    
//    c.connect("127.0.0.1:5672");
    _container_started = true;
    
    for (auto work: _work_queue)
    {
        work();
    }
    _work_queue.clear();
}

void ProtonMsgSystem::on_container_stop(proton::container &c)
{
    std::cout << "on_container_stop " << std::endl;
}

void ProtonMsgSystem::on_sender_open(proton::sender &s)
{
    std::cout << "on_sender_open " << std::endl;
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
