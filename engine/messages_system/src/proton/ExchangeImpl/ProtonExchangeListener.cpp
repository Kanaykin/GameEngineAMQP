#include "ProtonExchangeListener.h"

#include "MsgSysLog.h"

using namespace messages_system;

ProtonExchangeListener::ProtonExchangeListener(const ProtonExchangeQueueManagerWPtr& queues):
_queues(queues)
{}

proton::connection_options ProtonExchangeListener::on_accept(proton::listener&) 
{
    INFO_LOG("ProtonExchangeListener on_accept ");
    return proton::connection_options().handler(*(new ProtonExchangeConnHandler(_queues)));
}

void ProtonExchangeListener::on_open(proton::listener& l)
{
    INFO_LOG("ProtonExchangeListener listening on %1%", l.port());
}

void ProtonExchangeListener::on_error(proton::listener&, const std::string& s)
{
    INFO_LOG("ProtonExchangeListener listen error: %1%", s);
    throw std::runtime_error(s);
}
