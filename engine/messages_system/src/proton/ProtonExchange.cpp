#include "ProtonExchange.h"
#include "ExchangeImpl/ProtonExchangeListener.h"
#include "ExchangeImpl/ProtonExchangeQueueManager.h"

#include "MsgSysLog.h"

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
            ERROR_LOG("ProtonExchange Error %1%", e.what());
        }
    });
}

ProtonExchange::~ProtonExchange()
{
    // #TODO: Need base class for all classes with container and thread
    _container->stop();
    if (_thread)
        _thread->join();
}
