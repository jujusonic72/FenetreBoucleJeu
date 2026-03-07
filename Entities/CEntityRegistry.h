#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>

#include "CGameEngine.h"

class CEntity;

/*
 * CEntityRegistry
 * Registre singleton de tous les types d'entités disponibles.
 * Les entités s'enregistrent via REGISTER_ENTITY dans leur .cpp.
 * CEntity de base est enregistré manuellement dans CEntity.cpp.
 *
 * Usage depuis l'éditeur ECS :
 *   auto names = CEntityRegistry::GetAllNames();
 *   CEntity* e = CEntityRegistry::CreateInstance("CEntity", alloc);
 */
class CEntityRegistry
{
public:
    using FactoryFn = std::function<CEntity*(GameAllocator*)>;

    static void Register(const std::string& name, FactoryFn factory);
    static CEntity* CreateInstance(const std::string& name, GameAllocator* alloc);
    static const std::vector<std::string>& GetAllNames();

private:
    static std::map<std::string, FactoryFn>& GetRegistry();
    static std::vector<std::string>&         GetNames();
};

template<class T>
struct EntityRegistrar
{
    EntityRegistrar(const std::string& name)
    {
        CEntityRegistry::Register(name,
            [](GameAllocator* alloc) -> CEntity*
            {
                return alloc->NewObject<T>();
            });
    }
};

/*
 * REGISTER_ENTITY(ClassName)
 * À placer dans le .cpp de l'entité, au scope global.
 *
 * Exemple :
 *   // CAgentEntity.cpp
 *   REGISTER_ENTITY(CAgentEntity)
 */
#define REGISTER_ENTITY(ClassName) \
    static EntityRegistrar<ClassName> _entityReg_##ClassName(#ClassName);
