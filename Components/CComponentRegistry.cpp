#include "CComponentRegistry.h"

std::map<std::string, CComponentRegistry::FactoryFn>& CComponentRegistry::GetRegistry()
{
    // Fonction-local static — garanti d'être initialisé avant le premier accès.
    // Évite le static initialization order fiasco avec les ComponentRegistrar globaux.
    static std::map<std::string, FactoryFn> s_registry;
    return s_registry;
}

std::vector<std::string>& CComponentRegistry::GetNames()
{
    static std::vector<std::string> s_names;
    return s_names;
}

void CComponentRegistry::Register(const std::string& name, FactoryFn factory)
{
    GetRegistry()[name] = factory;
    GetNames().push_back(name);
}

GObject* CComponentRegistry::CreateInstance(const std::string& name, GameAllocator* alloc)
{
    auto& reg = GetRegistry();
    auto it = reg.find(name);
    if (it == reg.end()) return nullptr;
    return it->second(alloc);
}

const std::vector<std::string>& CComponentRegistry::GetAllNames()
{
    return GetNames();
}