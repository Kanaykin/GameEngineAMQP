
#include "ProtonMsgListener.h"

#include <iostream>
#include <proton/listener.hpp>
#include <proton/connection_options.hpp>

using namespace messages_system;

void ProtonMsgListener::on_open(proton::listener& l)
{
    std::cout << "ProtonMsgListener listening on " << l.port() << std::endl;
    if (!_listener.expired())
        _listener.lock()->on_open(l);
}

proton::connection_options ProtonMsgListener::on_accept(proton::listener& l)
{
    if (!_listener.expired())
        return _listener.lock()->on_accept(l);
    return proton::connection_options();
}

void ProtonMsgListener::on_error(proton::listener& l, const std::string& err)
{
    std::cout << "ProtonMsgListener::on_error port " << " err " << err << std::endl;
}

void ProtonMsgListener::on_close(proton::listener& l)
{
    std::cout << "ProtonMsgListener::on_close " << std::endl;
}

void ProtonMsgListener::addListener(const std::weak_ptr<proton::listen_handler>& listener)
{
    _listener = listener;
}
