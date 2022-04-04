#include "ProtonExchangeListener.h"

using namespace messages_system;

ProtonExchangeListener::ProtonExchangeListener(const ProtonExchangeQueueManagerWPtr& queues):
_queues(queues)
{}

proton::connection_options ProtonExchangeListener::on_accept(proton::listener&) 
{
    std::cout << "ProtonExchangeListener on_accept " << std::endl;
    return proton::connection_options().handler(*(new ProtonExchangeConnHandler(_queues)));
}

void ProtonExchangeListener::on_open(proton::listener& l)
{
    std::cout << "ProtonExchangeListener listening on " << l.port() << std::endl;
}

void ProtonExchangeListener::on_error(proton::listener&, const std::string& s)
{
    std::cerr << "ProtonExchangeListener listen error: " << s << std::endl;
    throw std::runtime_error(s);
}
