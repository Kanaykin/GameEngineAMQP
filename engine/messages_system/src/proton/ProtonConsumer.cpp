#include "ProtonConsumer.h"
#include <thread>

#include <proton/message.hpp>
#include <proton/source_options.hpp>

using namespace messages_system;

ProtonConsumer::ProtonConsumer(const std::string& url):
_url(url)
{
    restart();
}

ProtonConsumer::~ProtonConsumer()
{
    _container->stop();
    if (_thread)
        _thread->join();
}

void ProtonConsumer::restart()
{
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
    });
}

//void ProtonConsumer::on_connection_open(proton::connection& connection)
//{
//    std::cout << "ProtonConsumer::on_connection_start " << std::endl;
//}

void ProtonConsumer::on_sender_open(proton::sender &sender)
{
    std::cout << "ProtonConsumer::on_sender_open " << std::endl;
    if (sender.source().dynamic())
    {
        std::string addr = "1";
        sender.open(proton::sender_options().source(proton::source_options().address(addr)));
        senders[addr] = sender;
    }
}

void ProtonConsumer::on_message(proton::delivery& delivery, proton::message& m)
{
    std::string reply_to = m.reply_to();
    std::cout << "ProtonConsumer::on_message " << m.body() << " reply_to" << reply_to << std::endl;
}

void ProtonConsumer::on_container_start(proton::container& c)
{
    std::cout << "ProtonConsumer::on_container_start to " << _url << std::endl;
    
//    _connection = c.connect(_url);
//    _connection.open_receiver("examples");
    c.listen(_url, _listen_handler);
}
