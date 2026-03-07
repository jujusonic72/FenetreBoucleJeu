#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>

#include "CGameEngine.h"  // GameAllocator typedef

class GObject;

/*
 * CComponentRegistry
 * Registre singleton de tous les composants disponibles dans le projet.
 * Les composants s'y enregistrent automatiquement via la macro REGISTER_COMPONENT
 * placée dans leur .cpp — avant main(), grâce à l'initialisation statique.
 *
 * Usage depuis l'éditeur :
 *   auto names = CComponentRegistry::GetAllNames();   // liste pour l'UI
 *   GObject* c = CComponentRegistry::CreateInstance("CHealthComponent", alloc);
 */
class CComponentRegistry
{
public:
    // Signature de la factory : reçoit l'allocateur au moment de l'appel
    using FactoryFn = std::function<GObject*(GameAllocator*)>;

    // Enregistre un composant — appelé par ComponentRegistrar avant main()
    static void Register(const std::string& name, FactoryFn factory);

    // Crée une instance via l'allocateur — retourne nullptr si name inconnu
    static GObject* CreateInstance(const std::string& name, GameAllocator* alloc);

    // Retourne la liste des noms pour affichage dans l'éditeur
    static const std::vector<std::string>& GetAllNames();

private:
    // Accès unique à la map — évite le static initialization order fiasco
    static std::map<std::string, FactoryFn>& GetRegistry();
    static std::vector<std::string>&         GetNames();
};

/*
 * ComponentRegistrar<T>
 * Objet temporaire dont le constructeur appelle CComponentRegistry::Register.
 * Une instance statique globale est créée par REGISTER_COMPONENT — ce qui
 * déclenche l'enregistrement avant main().
 */
template<class T>
struct ComponentRegistrar
{
    ComponentRegistrar(const std::string& name)
    {
        CComponentRegistry::Register(name,
            [](GameAllocator* alloc) -> GObject*
            {
                return alloc->NewObject<T>();
            });
    }
};

/*
 * REGISTER_COMPONENT(ClassName)
 * À placer dans le .cpp du composant, au scope global (hors de toute fonction).
 *
 * Exemple :
 *   // CHealthComponent.cpp
 *   REGISTER_COMPONENT(CHealthComponent)
 */
#define REGISTER_COMPONENT(ClassName) \
    static ComponentRegistrar<ClassName> _componentReg_##ClassName(#ClassName);