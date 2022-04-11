#include "ProtonProducer.h"
#include <thread>
#include <proton/message.hpp>
#include <proton/sender_options.hpp>
#include <proton/source_options.hpp>
#include <proton/receiver_options.hpp>
#include <proton/connection.hpp>
#include <proton/work_queue.hpp>

#include <iostream>

using namespace messages_system;

ProtonProducer::ProtonProducer(const ProducerOptions& options):
_url(options.url)
{
    restart();
}

void ProtonProducer::restart()
{
    // #FIXME: Need thread pool
    _container = std::make_unique<proton::container>(*this);
    _thread = std::make_unique<std::thread>([this]() {
        try
        {
            _container->run();
        }
        catch (const proton::error& e)
        {
            std::cout << "Error " << e.what() << std::endl;
        }
        catch (...)
        {
//            std::cout << "Error " << e.what() << std::endl;
        }
    });
}

void ProtonProducer::on_container_start(proton::container &c)
{
    std::cout << "ProtonProducer::on_container_start " << std::endl;
    _sender = c.open_sender(_url);
//    _workQueue = &_sender.work_queue();
    
    // Create a receiver requesting a dynamically created queue
    // for the message source.
    proton::receiver_options opts = proton::receiver_options().source(proton::source_options().dynamic(true));
    _receiver = _sender.connection().open_receiver("render", opts);
}

void ProtonProducer::on_container_stop(proton::container &c)
{
    std::cout << "ProtonProducer::on_container_stop " << std::endl;
}
//
void ProtonProducer::on_error(const proton::error_condition& error)
{
    // #TODO: need thread pool for restart
//    restart();
    std::cout << "ProtonProducer::on_error " << error.name() << " description " << error.description() << std::endl;
}

ProtonProducer::~ProtonProducer()
{
    // #TODO: Need base class for all classes with container and thread
    _container->stop();
    if (_thread)
        _thread->join();
}

void ProtonProducer::publish()
{
    
    proton::message req;
    req.body("test " + _url);
    req.address("server");
//    req.address("render");
//    req.reply_to("client");
//    req.reply_to(_receiver.source().address());

    if (auto queue = getWorkQueue())
        queue->add([=]() { _sender.send(req); });
}

void ProtonProducer::on_receiver_open(proton::receiver &)
{
    std::cout << "ProtonProducer::on_receiver_open " << std::endl;
    proton::message req;
    req.body("test");
    _workQueue = &_sender.work_queue();

    _sender.send(req);
}

void ProtonProducer::on_sender_open(proton::sender &s)
{
    std::cout << "ProtonProducer::on_sender_open " << std::endl;
    _sender = s;
    _workQueue = &s.work_queue();
}

void ProtonProducer::on_message(proton::delivery& delivery, proton::message& m)
{
    std::string reply_to = m.reply_to();
    std::cout << "ProtonProducer::on_message " << m.body() << " reply_to" << reply_to << std::endl;
}

proton::work_queue* ProtonProducer::getWorkQueue() const
{
    return _workQueue;
}
