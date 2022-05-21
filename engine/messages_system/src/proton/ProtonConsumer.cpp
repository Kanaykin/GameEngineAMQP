#include "ProtonConsumer.h"
#include <thread>

#include <proton/message.hpp>
#include <proton/source_options.hpp>

#include "MsgSysLog.h"

using namespace messages_system;

ProtonConsumer::ProtonConsumer(const ConsumerOptions& options):
_url(options.url),
_direct(options.direct),
_name(options.name)
{
    restart();
}

ProtonConsumer::~ProtonConsumer()
{
    // #TODO: Need base class for all classes with container and thread
    if (_connection.active())
    {
        _connection.close();
    }
    _container->stop();
    if (_thread)
        _thread->join();
}

void ProtonConsumer::restart()
{
    // #TODO: Need thread pool
    _container = std::make_unique<proton::container>(*this);
    _thread = std::make_unique<std::thread>([this]() {
        try
        {
            _container->run();
        }
        catch (const proton::error& e)
        {
            ERROR_LOG("Error %1%", e.what());
        }
    });
}

void ProtonConsumer::on_sender_open(proton::sender &sender)
{
    DEBUG_LOG("ProtonConsumer::on_sender_open");
    if (sender.source().dynamic())
    {
        std::string addr = "sound";
        sender.open(proton::sender_options().source(proton::source_options().address(addr)));
        senders[addr] = sender;
    }
}

void ProtonConsumer::on_message(proton::delivery& delivery, proton::message& m)
{
    std::string reply_to = m.reply_to();
    INFO_LOG("ProtonConsumer::on_message [%1%] %2% reply_to %3%", _name, m.body(), reply_to);
}

void ProtonConsumer::on_container_start(proton::container& c)
{
    INFO_LOG("ProtonConsumer::on_container_start to %1%", _url);
    
    if (_direct)
    {
        c.listen(_url, _listen_handler);
    }
    else
    {
        _connection = c.connect(_url);
        _connection.open_receiver(_name);
    }
}
