#include <assert.h>
#include "AzCore/Component/Entity.h"
#include "AzCore/UserSettings/UserSettingsComponent.h"
#include "AzCore/Module/ModuleManager.h"
#include "AzCore/Component/ComponentApplication.h"
#include "AzCore/Console/Console.h"
#include "AzCore/Asset/AssetManagerComponent.h"


#include "TestComponent.h"
#include "TestModule.h"

int main()
{
    AZ::ComponentApplication app;
    auto appEntity = app.Create(AZ::ComponentApplication::Descriptor(), AZ::ComponentApplication::StartupParameters());
    
    // Load static modules
    auto LoadStaticModules = [](AZ::ModuleManagerRequests* moduleManager)
    {
        auto PopulateStaticModules = [](AZStd::vector<AZ::Module*>& outModules)
        {
//            this->CreateStaticModules(outModules);
            outModules.emplace_back(aznew TmpModule());
        };
        moduleManager->LoadStaticModules(PopulateStaticModules, AZ::ModuleInitializationSteps::RegisterComponentDescriptors);
    };

    AZ::ModuleManagerRequestBus::Broadcast(LoadStaticModules);
//    auto console = app.consol
    
    auto entity = std::make_unique<AZ::Entity>();
    auto component = entity->CreateComponent(azrtti_typeid<AZ::UserSettingsComponent>());
//	auto componentAssetManager = entity->CreateComponent(azrtti_typeid<AZ::AssetManagerComponent>());
    auto component2 = entity->CreateComponent(azrtti_typeid<TmpComponent>());
	entity->GetId();
    
	appEntity->Activate();
	entity->Activate();
    app.AddEntity(entity.release());
    
//    app.GetDescriptor().GetComponentDescriptors();
    
    while (true)
    {
        app.Tick();
//		AZ::TickBus::Broadcast(&TickEvents::OnTick, deltaTimeSeconds, GetTimeAtCurrentTick());
    }
    
    return 0;
}
