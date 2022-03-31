#pragma once

#include <vector>
#include <memory>

#include <proton/listen_handler.hpp>

namespace messages_system
{

class ProtonMsgListener : public proton::listen_handler
{
public:
    void addListener(const std::weak_ptr<proton::listen_handler>& listener);
    
protected:
    
    void on_open(proton::listener& l) override;
    
    proton::connection_options on_accept(proton::listener&) override;

    void on_error(proton::listener&, const std::string&) override;

    void on_close(proton::listener&) override;
    
private:
    typedef std::weak_ptr<proton::listen_handler> ListenHandlerWPtr;
//    typedef std::vector<ListenHandlerWPtr> ListenHandlerVector;
    
    ListenHandlerWPtr _listener;
};

}

