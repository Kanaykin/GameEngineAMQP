#pragma once

#include "AzCore/EBus/EBus.h"

class TestEvents
	: public AZ::EBusTraits
{
public:
	
	virtual void OnTest(float deltaTime) = 0;
	
};

typedef AZ::EBus<TestEvents>    TestBus;
