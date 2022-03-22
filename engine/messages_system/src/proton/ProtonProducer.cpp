#include "ProtonProducer.h"
#include <thread>

using namespace messages_system;

ProtonProducer::ProtonProducer(const proton::sender& sender):
_sender(sender)
{
}

ProtonProducer::~ProtonProducer()
{
    
}
