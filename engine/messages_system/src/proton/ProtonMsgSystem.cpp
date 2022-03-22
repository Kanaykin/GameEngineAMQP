#include "ProtonMsgSystem.h"
#include "ProtonProducer.h"

using namespace messages_system;

ProtonMsgSystem::ProtonMsgSystem():
_container(std::make_unique<proton::container>(*this))
{
//    _thread->detach();
//    _thread->join();
    
    _thread = std::make_unique<std::thread>([this](){
        try
        {
            _container->run();
        } catch (...) {
    //        std::cerr << e.what() << std::endl;
        }
        }
                                                );
}

ProtonMsgSystem::~ProtonMsgSystem()
{
//    _thread->join();
//    _producers.clear();
    _container->stop();
    _thread->join();
}

IProducerPtr ProtonMsgSystem::createProducer()
{
    auto sender = _container->open_sender("url");
    return std::make_shared<ProtonProducer>(sender);
}

void ProtonMsgSystem::on_container_start(proton::container &c)
{
    
}
