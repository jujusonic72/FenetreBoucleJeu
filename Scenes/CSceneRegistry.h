#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>

#include "CGameEngine.h"

class GScene;

/*
 * CSceneRegistry
 * Même pattern que CComponentRegistry, mais pour les scènes.
 * Les scènes s'enregistrent via REGISTER_SCENE dans leur .cpp.
 *
 * Usage depuis le sélecteur de scène :
 *   auto names = CSceneRegistry::GetAllNames();
 *   GScene* s  = CSceneRegistry::CreateInstance("CTP2Scene", alloc);
 */
class CSceneRegistry
{
public:
    using FactoryFn = std::function<GScene*(GameAllocator*)>;

    static void Register(const std::string& name, FactoryFn factory);
    static GScene* CreateInstance(const std::string& name, GameAllocator* alloc);
    static const std::vector<std::string>& GetAllNames();

private:
    static std::map<std::string, FactoryFn>& GetRegistry();
    static std::vector<std::string>&         GetNames();
};

template<class T>
struct SceneRegistrar
{
    SceneRegistrar(const std::string& name)
    {
        CSceneRegistry::Register(name,
            [](GameAllocator* alloc) -> GScene*
            {
                return alloc->NewObject<T>();
            });
    }
};

/*
 * REGISTER_SCENE(ClassName)
 * À placer dans le .cpp de la scène, au scope global.
 *
 * Exemple :
 *   // CTP2Scene.cpp
 *   REGISTER_SCENE(CTP2Scene)
 */
#define REGISTER_SCENE(ClassName) \
    static SceneRegistrar<ClassName> _sceneReg_##ClassName(#ClassName);