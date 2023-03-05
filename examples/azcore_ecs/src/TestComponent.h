#pragma once

#include <AzCore/Component/Component.h>
#include "TestBus.h"

class TmpComponent
	: public AZ::Component
	, public AZ::TickBus::Handler
	, public TestBus::Handler
{
public:
	AZ_COMPONENT(TmpComponent, "{D5A73BCC-0098-4d1e-8FE4-C86101E37sss}", AZ::Component)
	
	void Activate() override
	{
		AZ::TickBus::Handler::BusConnect();
	}
	
	using EntityAddedEvent = AZ::Event<float>;
	
	void RegisterTestHandler(EntityAddedEvent::Handler& handler)
	{
//		TestBus::Events tmp;
//		TestBus::Handler::BusConnect(id);
//		using EntityAddedEvent = AZ::Event<float>;
		EntityAddedEvent tmp;
		handler.Connect(tmp);
	}
	
	static void Reflect(AZ::ReflectContext* context){}
	void Deactivate()  override {}
	void OnTick(float deltaTime, AZ::ScriptTimePoint time) override {
		int jjj = 0;
	}
	
	void OnTest(float deltaTime) override {
		
	}
};

