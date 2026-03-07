#include "CSceneRegistry.h"

std::map<std::string, CSceneRegistry::FactoryFn>& CSceneRegistry::GetRegistry()
{
    static std::map<std::string, FactoryFn> s_registry;
    return s_registry;
}

std::vector<std::string>& CSceneRegistry::GetNames()
{
    static std::vector<std::string> s_names;
    return s_names;
}

void CSceneRegistry::Register(const std::string& name, FactoryFn factory)
{
    GetRegistry()[name] = factory;
    GetNames().push_back(name);
}

GScene* CSceneRegistry::CreateInstance(const std::string& name, GameAllocator* alloc)
{
    auto& reg = GetRegistry();
    auto it = reg.find(name);
    if (it == reg.end()) return nullptr;
    return it->second(alloc);
}

const std::vector<std::string>& CSceneRegistry::GetAllNames()
{
    return GetNames();
}