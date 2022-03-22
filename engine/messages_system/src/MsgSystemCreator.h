#pragma once

#include "IMsgSystem.h"

namespace messages_system
{

typedef std::shared_ptr<IMsgSystem> IMsgSystemPtr;
typedef std::function<IMsgSystemPtr()> CreateSystemFunc;
typedef std::map<std::string, CreateSystemFunc> CreateSystemsFuncMap;
static CreateSystemsFuncMap creatorMsgSystems;


template<class System>
struct RegisterCreateSystem
{
    explicit RegisterCreateSystem(const std::string& name)
    {
        creatorMsgSystems[name] = [](){ return std::make_shared<System>();};
    }
};

IMsgSystemPtr createMsgSystem(const std::string& name);


}
