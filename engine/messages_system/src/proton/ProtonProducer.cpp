#include "ProtonProducer.h"
#include <thread>
#include <proton/message.hpp>
#include <proton/sender_options.hpp>
#include <proton/source_options.hpp>
#include <proton/receiver_options.hpp>
#include <proton/connection.hpp>
#include <proton/work_queue.hpp>

using namespace messages_system;

ProtonProducer::ProtonProducer()
{
    
}

void ProtonProducer::setSender(const proton::sender& sender)
{
    _sender = sender;
    proton::receiver_options opts = proton::receiver_options().source(proton::source_options().dynamic(true));
    _receiver = _sender.connection().open_receiver("", opts);
    
    _work_queue = &_sender.work_queue();
}

ProtonProducer::~ProtonProducer()
{
    
}

void ProtonProducer::publish()
{
    proton::message req;
    req.body("test");
    req.address("render");
    req.reply_to("client");
//    req.reply_to(_receiver.source().address());
    
    if (work_queue())
        work_queue()->add([=]() { _sender.send(req); });
}

proton::work_queue* ProtonProducer::work_queue()
{
    // Wait till work_queue_ and sender_ are initialized.
    std::unique_lock<std::mutex> l(lock_);
//    while (!_work_queue) sender_ready_.wait(l);
    return _work_queue;
}
