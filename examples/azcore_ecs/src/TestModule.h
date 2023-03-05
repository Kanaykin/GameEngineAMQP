#pragma once

#include "AzCore/Module/Module.h"

class TmpModule
	: public AZ::Module
{
public:

	AZ_RTTI(TmpModule, "{CFC64EAF-7566-4D30-AAF4-A6FF19fff7DB}", AZ::Module);
	
	TmpModule(){
		m_descriptors.insert(m_descriptors.end(), {
			TmpComponent::CreateDescriptor()
		});
	};
	~TmpModule() override = default;

//    AZ::ComponentTypeList GetRequiredSystemComponents() const override;
};

AZ_DECLARE_MODULE_CLASS(Gem_TmpModule, TmpModule)
