#include "ProtonConsumer.h"
#include <thread>

using namespace messages_system;

ProtonConsumer::ProtonConsumer(const proton::receiver& receiver):
_receiver(receiver)
{
}

ProtonConsumer::~ProtonConsumer()
{
    
}
