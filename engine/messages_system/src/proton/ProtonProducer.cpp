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

ProtonProducer::ProtonProducer(const std::string& url):
_url(url)
{
    restart();
}

void ProtonProducer::restart()
{
    _container = std::make_unique<proton::container>(*this);
    _thread = std::make_unique<std::thread>([this]() {
        try
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
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
    _work_queue = &_sender.work_queue();
    
    // Create a receiver requesting a dynamically created queue
    // for the message source.
    proton::receiver_options opts = proton::receiver_options().source(proton::source_options().dynamic(true));
    _receiver = _sender.connection().open_receiver("", opts);
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

//void ProtonProducer::setSender(const proton::sender& sender)
//{
//    _sender = sender;
//    proton::receiver_options opts = proton::receiver_options().source(proton::source_options().dynamic(true));
//    _receiver = _sender.connection().open_receiver("", opts);
//
//    _work_queue = &_sender.work_queue();
//}

ProtonProducer::~ProtonProducer()
{
    _container->stop();
    if (_thread)
        _thread->join();
}

void ProtonProducer::publish()
{
    proton::message req;
    req.body("test");
//    req.address("render");
//    req.reply_to("client");
//    req.reply_to(_receiver.source().address());

    if (work_queue())
        work_queue()->add([=]() { _sender.send(req); });
}

void ProtonProducer::on_receiver_open(proton::receiver &)
{
    std::cout << "ProtonProducer::on_receiver_open " << std::endl;
//    send_request();
    proton::message req;
    req.body("test");
//    req.address("render");
//    req.reply_to("client");
//    req.reply_to(_receiver.source().address());

    _sender.send(req);
}

void ProtonProducer::on_sender_open(proton::sender &s)
{
    std::cout << "ProtonProducer::on_sender_open " << std::endl;
//    _sender = s;
//    _work_queue = &s.work_queue();
}

void ProtonProducer::on_message(proton::delivery& delivery, proton::message& m)
{
    std::string reply_to = m.reply_to();
    std::cout << "ProtonProducer::on_message " << m.body() << " reply_to" << reply_to << std::endl;
}

proton::work_queue* ProtonProducer::work_queue()
{
    // Wait till work_queue_ and sender_ are initialized.
//    std::unique_lock<std::mutex> l(_lock);
//    while (!_work_queue) sender_ready_.wait(l);
    return _work_queue;
}
