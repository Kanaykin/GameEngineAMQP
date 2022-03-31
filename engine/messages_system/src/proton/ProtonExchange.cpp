#include "ProtonExchange.h"

using namespace messages_system;


ProtonExchange::ProtonExchange(const std::string& url):
_listener(std::make_shared<ProtonExchangeListener>())
//_container(std::make_unique<proton::container>())
{
//    container.listen(url, _listener);
//    _thread = std::make_unique<std::thread>([this]() {
//        try
//        {
//            _container->run();
//        }
//        catch (const proton::error& e)
//        {
//            std::cout << "Error " << e.what() << std::endl;
//        }
//    });
}

ProtonExchange::~ProtonExchange()
{
    
}

std::shared_ptr<proton::listen_handler> ProtonExchange::getListener() const
{
    return _listener;
}
