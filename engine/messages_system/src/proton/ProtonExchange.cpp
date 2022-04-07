#include "ProtonExchange.h"
#include "ExchangeImpl/ProtonExchangeListener.h"
#include "ExchangeImpl/ProtonExchangeQueueManager.h"

using namespace messages_system;


ProtonExchange::ProtonExchange(const ExchangeOptions& options)
{
    _container = std::make_shared<proton::container>("broker");
    _queues = std::make_shared<ProtonExchangeQueueManager>(_container);
    _listener = std::make_unique<ProtonExchangeListener>(_queues);
    
    _container->listen(options.url, *_listener.get());
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

ProtonExchange::~ProtonExchange()
{
    
}

//std::shared_ptr<proton::listen_handler> ProtonExchange::getListener() const
//{
//    return _listener;
//}
