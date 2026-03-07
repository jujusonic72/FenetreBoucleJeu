#include "GScene.h"
#include "CEntity.h"
#include "CComponent.h"
#include "CProperty.h"
#include "CComponentRegistry.h"
#include "CGameEngine.h"
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstring>

using json = nlohmann::json;

CClass GScene::s_Class("GScene", GObject::StaticClass());

GScene::~GScene()
{
    if (!m_entities.empty())
        std::cerr << "[GScene] Attention : " << m_entities.size()
                  << " entite(s) non liberee(s) a la destruction de '"
                  << m_name << "'." << std::endl;
}

CClass* GScene::StaticClass()         { return &s_Class; }
const CClass* GScene::GetClass() const { return &s_Class; }

std::string GScene::GetJsonPath() const
{
    return std::string("Scenes/") + m_name + ".json";
}

// ---------------------------------------------------------------------------
// Update — propage aux entités (et leurs composants via CEntity::Update)
// ---------------------------------------------------------------------------
void GScene::Update(float dt)
{
    for (CEntity* entity : m_entities)
        entity->Update(dt);
}

// ---------------------------------------------------------------------------
// Helpers propriétés
// ---------------------------------------------------------------------------
static void SerializeProperty(json& j, const CProperty& prop, const void* obj)
{
    const void* ptr = prop.GetValuePtr(obj);
    switch (prop.Type)
    {
        case EPropertyType::Int:    j[prop.Name] = *static_cast<const int*>(ptr);          break;
        case EPropertyType::Float:  j[prop.Name] = *static_cast<const float*>(ptr);        break;
        case EPropertyType::Bool:   j[prop.Name] = *static_cast<const bool*>(ptr);         break;
        case EPropertyType::String: j[prop.Name] = *static_cast<const char* const*>(ptr);  break;
    }
}

static void DeserializeProperty(const json& j, const CProperty& prop, void* obj)
{
    if (!j.contains(prop.Name)) return;
    void* ptr = prop.GetValuePtr(obj);
    switch (prop.Type)
    {
        case EPropertyType::Int:   *static_cast<int*>(ptr)   = j[prop.Name].get<int>();   break;
        case EPropertyType::Float: *static_cast<float*>(ptr) = j[prop.Name].get<float>(); break;
        case EPropertyType::Bool:  *static_cast<bool*>(ptr)  = j[prop.Name].get<bool>();  break;
        case EPropertyType::String: break;
    }
}

// ---------------------------------------------------------------------------
// Save
// ---------------------------------------------------------------------------
bool GScene::Save() const
{
    std::filesystem::create_directories("Scenes");

    json jScene;
    jScene["Scene"]    = m_name;
    jScene["GObjects"] = json::array();

    for (const CEntity* entity : m_entities)
    {
        json jEntity;
        jEntity["Type"]       = entity->GetClass()->GetName();
        jEntity["Name"]       = entity->Name;
        jEntity["Components"] = json::array();

        for (const CComponent* comp : entity->GetComponents())
        {
            json jComp;
            jComp["Type"] = comp->GetClass()->GetName();
            for (const CProperty& prop : comp->GetClass()->GetAllProperties())
                SerializeProperty(jComp, prop, comp);
            jEntity["Components"].push_back(jComp);
        }

        jScene["GObjects"].push_back(jEntity);
    }

    std::ofstream file(GetJsonPath());
    if (!file.is_open())
    {
        std::cerr << "[GScene] Impossible d'ecrire : " << GetJsonPath() << std::endl;
        return false;
    }
    file << jScene.dump(4);
    return true;
}

// ---------------------------------------------------------------------------
// Load
// ---------------------------------------------------------------------------
bool GScene::Load(GameAllocator* alloc)
{
    if (!alloc) return false;

    std::ifstream file(GetJsonPath());
    if (!file.is_open())
    {
        std::cerr << "[GScene] Fichier introuvable : " << GetJsonPath() << std::endl;
        return false;
    }

    json jScene;
    try { jScene = json::parse(file); }
    catch (const json::parse_error& e)
    {
        std::cerr << "[GScene] Erreur de parsing JSON : " << e.what() << std::endl;
        return false;
    }

    ClearEntities(alloc);

    if (!jScene.contains("GObjects")) return true;

    for (const auto& jEntity : jScene["GObjects"])
    {
        CEntity* entity = alloc->NewObject<CEntity>();
        if (!entity) continue;

        if (jEntity.contains("Name"))
        {
            std::string name = jEntity["Name"].get<std::string>();
            strncpy(entity->Name, name.c_str(), sizeof(entity->Name) - 1);
            entity->Name[sizeof(entity->Name) - 1] = '\0';
        }

        if (jEntity.contains("Components"))
        {
            for (const auto& jComp : jEntity["Components"])
            {
                if (!jComp.contains("Type")) continue;
                std::string typeName = jComp["Type"].get<std::string>();

                GObject* obj = CComponentRegistry::CreateInstance(typeName, alloc);
                if (!obj) { std::cerr << "[GScene] Composant inconnu : " << typeName << std::endl; continue; }

                CComponent* comp = Cast<CComponent>(obj);
                if (!comp) continue;

                for (const CProperty& prop : comp->GetClass()->GetAllProperties())
                    DeserializeProperty(jComp, prop, comp);

                entity->AddComponent(comp);
            }
        }

        m_entities.push_back(entity);
    }

    return true;
}

// ---------------------------------------------------------------------------
// ClearEntities
// ---------------------------------------------------------------------------
void GScene::ClearEntities(GameAllocator* alloc)
{
    if (!alloc) return;
    for (CEntity* entity : m_entities)
    {
        for (CComponent* comp : entity->GetComponents())
            alloc->FreeObject(comp);
        alloc->FreeObject(entity);
    }
    m_entities.clear();
}