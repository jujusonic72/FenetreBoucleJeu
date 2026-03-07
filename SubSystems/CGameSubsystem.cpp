#include "CGameSubsystem.h"
#include "CInputSubsystem.h"
#include "CGameEngine.h"
#include "CEntity.h"
#include "CComponent.h"
#include "GObject.h"
#include "GScene.h"
#include "CComponentRegistry.h"
#include "CSceneRegistry.h"
#include "imgui.h"

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>



// --- Statics ---
std::vector<GObject*> CGameSubsystem::s_subscribers;
ImDrawList*           CGameSubsystem::s_drawList         = nullptr;
ImVec2                CGameSubsystem::s_gameWindowOrigin = { 0.0f, 0.0f };

// ---------------------------------------------------------------------------
// Subscribe / Unsubscribe
// ---------------------------------------------------------------------------
void CGameSubsystem::Subscribe(GObject* obj)
{
    if (!obj) return;
    // Éviter les doublons
    auto it = std::find(s_subscribers.begin(), s_subscribers.end(), obj);
    if (it == s_subscribers.end())
        s_subscribers.push_back(obj);
}

void CGameSubsystem::Unsubscribe(GObject* obj)
{
    if (!obj) return;
    auto it = std::find(s_subscribers.begin(), s_subscribers.end(), obj);
    if (it != s_subscribers.end())
        s_subscribers.erase(it);
}

// ---------------------------------------------------------------------------
// API de primitives
// ---------------------------------------------------------------------------
void CGameSubsystem::DrawCircle(float x, float y, float radius, ImU32 color, int segments, float thickness)
{
    if (!s_drawList) return;
    s_drawList->AddCircle(
        { s_gameWindowOrigin.x + x, s_gameWindowOrigin.y + y },
        radius, color, segments, thickness);
}

void CGameSubsystem::DrawCircleFilled(float x, float y, float radius, ImU32 color, int segments)
{
    if (!s_drawList) return;
    s_drawList->AddCircleFilled(
        { s_gameWindowOrigin.x + x, s_gameWindowOrigin.y + y },
        radius, color, segments);
}

void CGameSubsystem::DrawRect(float x, float y, float w, float h, ImU32 color, float thickness)
{
    if (!s_drawList) return;
    ImVec2 min = { s_gameWindowOrigin.x + x,     s_gameWindowOrigin.y + y };
    ImVec2 max = { s_gameWindowOrigin.x + x + w, s_gameWindowOrigin.y + y + h };
    s_drawList->AddRect(min, max, color, 0.0f, 0, thickness);
}

void CGameSubsystem::DrawRectFilled(float x, float y, float w, float h, ImU32 color)
{
    if (!s_drawList) return;
    ImVec2 min = { s_gameWindowOrigin.x + x,     s_gameWindowOrigin.y + y };
    ImVec2 max = { s_gameWindowOrigin.x + x + w, s_gameWindowOrigin.y + y + h };
    s_drawList->AddRectFilled(min, max, color);
}

void CGameSubsystem::DrawLine(float x1, float y1, float x2, float y2, ImU32 color, float thickness)
{
    if (!s_drawList) return;
    s_drawList->AddLine(
        { s_gameWindowOrigin.x + x1, s_gameWindowOrigin.y + y1 },
        { s_gameWindowOrigin.x + x2, s_gameWindowOrigin.y + y2 },
        color, thickness);
}

void CGameSubsystem::DrawText(float x, float y, ImU32 color, const char* text)
{
    if (!s_drawList || !text) return;
    s_drawList->AddText(
        { s_gameWindowOrigin.x + x, s_gameWindowOrigin.y + y },
        color, text);
}

ImVec2 CGameSubsystem::GetGameWindowOrigin()
{
    return s_gameWindowOrigin;
}

// ---------------------------------------------------------------------------
// Subsystem lifecycle
// ---------------------------------------------------------------------------
bool CGameSubsystem::Start()
{
    ScanScenes();

    return true;
}

void CGameSubsystem::Update(float deltaSeconds)
{
    frames[index] = deltaSeconds;
    timer += deltaSeconds;
    if( timer >= 0.5f){
        timer = 0;
        moyenne = 0;
        for(float frame : frames){
            moyenne += frame;
        }
        moyenne = moyenne/frameCount;
    }
    index = (index + 1) % 100;
    frameCount = (frameCount < 100) ? ++frameCount : 100;

    // --- Fenêtres moteur ---
    DrawAllocatorWindow();
    DrawSceneSelectorWindow();
    DrawECSWindow();

    // --- Fenêtre de jeu (dispatche Start/Update aux subscribers) ---
    DrawGameWindow(deltaSeconds);
}

void CGameSubsystem::Shutdown()
{
    s_subscribers.clear();

    auto* alloc = CGameEngine::GetInstance().GetAllocator();
    if (alloc)
    {
        if (m_activeScene)
        {
            m_activeScene->Save();
            m_activeScene->ClearEntities(alloc);
            alloc->FreeObject(m_activeScene);
            m_activeScene = nullptr;
        }
    }
    s_subscribers.clear();
}

// ---------------------------------------------------------------------------
// ScanScenes — parcourt Scenes/*.json et remplit m_sceneNames
// ---------------------------------------------------------------------------
void CGameSubsystem::ScanScenes()
{
    m_sceneNames.clear();
    std::filesystem::create_directories("Scenes");

    for (const auto& entry : std::filesystem::directory_iterator("Scenes"))
    {
        if (entry.path().extension() == ".json")
            m_sceneNames.push_back(entry.path().stem().string());
    }

    std::sort(m_sceneNames.begin(), m_sceneNames.end());
}

// ---------------------------------------------------------------------------
// LoadScene
// ---------------------------------------------------------------------------
void CGameSubsystem::LoadScene(const std::string& sceneName)
{
    auto* alloc = CGameEngine::GetInstance().GetAllocator();

    if (m_activeScene)
    {
        if (!m_isPlaying) m_activeScene->Save();
        m_activeScene->ClearEntities(alloc);
        alloc->FreeObject(m_activeScene);
        m_activeScene  = nullptr;
        m_sceneStarted = false;
        m_isPlaying    = false;
        selectedEntity = -1;
    }

    // Priorité : scène compilée (CSceneRegistry) sinon GScene générique
    GScene* newScene = CSceneRegistry::CreateInstance(sceneName, alloc);
    if (!newScene)
    {
        newScene = alloc->NewObject<GScene>();
        if (!newScene) return;
        newScene->SetName(sceneName);
    }

    newScene->Load(alloc); // charge le .json si existant — sinon scène vide, c'est ok
    m_activeScene  = newScene;
    m_sceneStarted = false;
}

// ---------------------------------------------------------------------------
// CreateScene — génère .json + .h + .cpp scaffold
// ---------------------------------------------------------------------------
void CGameSubsystem::CreateScene(const std::string& sceneName)
{
    if (sceneName.empty()) return;

    std::filesystem::create_directories("Scenes");

    const std::string jsonPath = "Scenes/" + sceneName + ".json";
    const std::string hPath    = "Scenes/" + sceneName + ".h";
    const std::string cppPath  = "Scenes/" + sceneName + ".cpp";

    // --- JSON minimal ---
    if (!std::filesystem::exists(jsonPath))
    {
        std::ofstream f(jsonPath);
        f << "{\n"
          << "    \"Scene\": \"" << sceneName << "\",\n"
          << "    \"GObjects\": []\n"
          << "}\n";
    }

    // --- Header scaffold ---
    if (!std::filesystem::exists(hPath))
    {
        std::ofstream f(hPath);
        f << "#pragma once\n\n"
          << "#include \"GScene.h\"\n\n"
          << "/*\n"
          << " * " << sceneName << "\n"
          << " * Level script genere automatiquement.\n"
          << " * Ajoutez REGISTER_SCENE(" << sceneName << ") dans le .cpp pour\n"
          << " * que la scene apparaisse dans le selecteur apres recompilation.\n"
          << " */\n"
          << "class " << sceneName << " : public GScene\n"
          << "{\n"
          << "    GENERATE_REFLECTION\n\n"
          << "public:\n"
          << "    const char* GetSceneName() const override { return \"" << sceneName << "\"; }\n\n"
          << "    void Start()          override;\n"
          << "    void Update(float dt) override;\n"
          << "};\n";
    }

    // --- CPP scaffold ---
    if (!std::filesystem::exists(cppPath))
    {
        std::ofstream f(cppPath);
        f << "#include \"" << sceneName << ".h\"\n"
          << "#include \"SubSystems/CGameSubsystem.h\"\n"
          << "#include \"CSceneRegistry.h\"\n\n"
          << "IMPLEMENT_REFLECTION(" << sceneName << ", GScene)\n"
          << "REGISTER_SCENE(" << sceneName << ")\n\n"
          << "void " << sceneName << "::Start()\n"
          << "{\n"
          << "    // Initialisation de la scene\n"
          << "}\n\n"
          << "void " << sceneName << "::Update(float dt)\n"
          << "{\n"
          << "    // Logique et rendu de la scene\n"
          << "}\n";
    }

    // Rescan pour que la scène apparaisse immédiatement dans la liste
    ScanScenes();
}

// ---------------------------------------------------------------------------
// Play / Stop
// ---------------------------------------------------------------------------
void CGameSubsystem::Play()
{
    if (!m_activeScene || m_isPlaying) return;

    // Sauvegarder l'état éditeur — c'est le snapshot de référence pour Stop()
    m_activeScene->Save();
    m_snapshotPath = m_activeScene->GetJsonPath();

    m_isPlaying    = true;
    m_sceneStarted = false; // forcer un Start() au premier frame de jeu
}

void CGameSubsystem::Stop()
{
    if (!m_isPlaying) return;
    m_isPlaying = false;

    // Recharger depuis le snapshot sauvegardé au Play — annule les changements runtime
    if (m_activeScene)
    {
        auto* alloc = CGameEngine::GetInstance().GetAllocator();
        m_activeScene->ClearEntities(alloc);
        m_activeScene->Load(alloc);
        m_sceneStarted = false;
        selectedEntity = -1;
    }
}

// ---------------------------------------------------------------------------
// DrawSceneSelectorWindow
// ---------------------------------------------------------------------------
void CGameSubsystem::DrawSceneSelectorWindow()
{
    ImGui::Begin("Scenes");

    // --- Création de scène ---
    ImGui::SeparatorText("Nouvelle scene");
    ImGui::InputText("##NomScene", m_newSceneNameBuffer, sizeof(m_newSceneNameBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Creer"))
    {
        CreateScene(std::string(m_newSceneNameBuffer));
        m_newSceneNameBuffer[0] = '\0';
    }

    // --- Rafraîchir ---
    ImGui::SameLine();
    if (ImGui::Button("Rafraichir"))
        ScanScenes();

    ImGui::Spacing();

    // --- Liste des scènes scannées ---
    ImGui::SeparatorText("Scenes disponibles");

    if (m_sceneNames.empty())
    {
        ImGui::TextDisabled("Aucun fichier .json dans Scenes/");
    }
    else
    {
        for (const std::string& name : m_sceneNames)
        {
            bool isActive = (m_activeScene && m_activeScene->GetSceneName() == name);

            if (isActive)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));

            if (ImGui::Button(name.c_str(), ImVec2(-1, 0)) && !isActive)
                LoadScene(name);

            if (isActive)
                ImGui::PopStyleColor();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();

    // --- Actions scène active ---
    if (m_activeScene)
    {
        ImGui::Text("Active : %s", m_activeScene->GetSceneName());

        // Boutons Play / Stop
        if (!m_isPlaying)
        {
            if (ImGui::Button("▶  Jouer", ImVec2(-1, 0)))
                Play();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("■  Stop", ImVec2(-1, 0)))
                Stop();
            ImGui::PopStyleColor();
        }

        // Sauvegarde manuelle — désactivée en mode Play
        ImGui::BeginDisabled(m_isPlaying);
        if (ImGui::Button("Sauvegarder", ImVec2(-1, 0)))
            m_activeScene->Save();
        if (ImGui::Button("Recharger depuis fichier", ImVec2(-1, 0)))
        {
            auto* alloc = CGameEngine::GetInstance().GetAllocator();
            m_activeScene->ClearEntities(alloc);
            m_activeScene->Load(alloc);
            m_sceneStarted = false;
            selectedEntity = -1;
        }
        ImGui::EndDisabled();
    }
    else
    {
        ImGui::TextDisabled("Aucune scene active.");
    }

    ImGui::End();
}

// ---------------------------------------------------------------------------
// DrawGameWindow
// ---------------------------------------------------------------------------
void CGameSubsystem::DrawGameWindow(float deltaSeconds)
{
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Jeu");

    if (!m_activeScene)
    {
        ImGui::TextDisabled("Aucune scene chargee.");
        ImGui::TextDisabled("Utilisez la fenetre 'Scenes' pour charger une scene.");
        ImGui::End();
        return;
    }

    s_gameWindowOrigin = ImGui::GetCursorScreenPos();
    s_drawList         = ImGui::GetWindowDrawList();

    if (m_isPlaying)
    {
        // --- Mode Play ---
        // Start() une seule fois après Play()
        if (!m_sceneStarted)
        {
            m_sceneStarted = true;
            m_activeScene->Start();

            if (!m_subscribersStarted)
            {
                m_subscribersStarted = true;
                for (GObject* obj : s_subscribers) obj->Start();
            }
        }

        m_activeScene->Update(deltaSeconds);

        std::vector<GObject*> snapshot = s_subscribers;
        for (GObject* obj : snapshot) obj->Update(deltaSeconds);
    }
    else
    {
        // --- Mode Éditeur ---
        // Affiche la scène statique (entités via DrawList) sans appeler Update()
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "[ MODE EDITEUR — Appuyez sur Jouer pour simuler ]");
    }

    s_drawList = nullptr;
    ImGui::End();
}

// ---------------------------------------------------------------------------
// DrawECSWindow — travaille sur les entités de la scène active
// ---------------------------------------------------------------------------
void CGameSubsystem::DrawECSWindow()
{
    auto* alloc = CGameEngine::GetInstance().GetAllocator();
    if (!alloc) return;

    ImGui::Begin("Editeur ECS");

    if (!m_activeScene)
    {
        ImGui::TextDisabled("Chargez une scene pour editer ses entites.");
        ImGui::End();
        return;
    }

    // Verrouillé en mode Play — on n'édite pas pendant la simulation
    ImGui::BeginDisabled(m_isPlaying);

    auto& entities = m_activeScene->GetEntities();

    // --- Création d'entité ---
    ImGui::SeparatorText("Creer une entite");
    ImGui::InputText("Nom", entityNameBuffer, sizeof(entityNameBuffer));
    if (ImGui::Button("Creer Entite"))
    {
        CEntity* entity = alloc->NewObject<CEntity>();
        if (entity)
        {
            strncpy(entity->Name, entityNameBuffer, sizeof(entity->Name) - 1);
            entity->Name[sizeof(entity->Name) - 1] = '\0';
            entities.push_back(entity);
        }
    }

    // --- Liste des entités ---
    ImGui::Separator();
    ImGui::SeparatorText("Entites");
    for (int i = 0; i < (int)entities.size(); i++)
    {
        bool isSelected = (selectedEntity == i);
        if (ImGui::Selectable(entities[i]->Name, isSelected))
            selectedEntity = i;
    }

    // --- Détails de l'entité sélectionnée ---
    ImGui::Separator();
    if (selectedEntity >= 0 && selectedEntity < (int)entities.size())
    {
        CEntity* entity = entities[selectedEntity];

        ImGui::SeparatorText("Details");
        ImGui::Text("Nom  : %s", entity->Name);
        ImGui::Text("Type : %s", entity->GetClass()->GetName());

        if (ImGui::Button("Detruire Entite"))
        {
            for (CComponent* comp : entity->GetComponents())
                alloc->FreeObject(comp);
            alloc->FreeObject(entity);
            entities.erase(entities.begin() + selectedEntity);
            selectedEntity = -1;
        }
        else
        {
            // Composants existants
            ImGui::SeparatorText("Composants");
            const auto& components = entity->GetComponents();
            for (int c = 0; c < (int)components.size(); c++)
            {
                CComponent* comp = components[c];
                ImGui::PushID(c);
                if (ImGui::TreeNode(comp->GetClass()->GetName()))
                {
                    DrawPropertyEditor(comp);
                    if (ImGui::Button("Retirer"))
                    {
                        entity->RemoveComponent(comp);
                        alloc->FreeObject(comp);
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // Ajout dynamique via CComponentRegistry
            ImGui::SeparatorText("Ajouter un composant");
            for (const std::string& compName : CComponentRegistry::GetAllNames())
            {
                if (ImGui::Button(("+ " + compName).c_str()))
                {
                    GObject* obj = CComponentRegistry::CreateInstance(compName, alloc);
                    if (CComponent* comp = Cast<CComponent>(obj))
                        entity->AddComponent(comp);
                }
            }
        }
    }
    else
    {
        ImGui::Text("Selectionnez une entite pour voir ses details.");
    }

    ImGui::EndDisabled();
    ImGui::End();
}

// ---------------------------------------------------------------------------
// DrawAllocatorWindow
// ---------------------------------------------------------------------------
void CGameSubsystem::DrawAllocatorWindow()
{
    auto* alloc = CGameEngine::GetInstance().GetAllocator();
    if (!alloc) return;

    ImGui::Begin("Allocateur Memoire");
    int used = alloc->GetUsedCount(), free = alloc->GetFreeCount(), total = alloc->GetTotalCount();
    ImGui::Text("Pages utilisees : %d", used);
    ImGui::Text("Pages libres    : %d", free);
    ImGui::Text("Pages totales   : %d", total);
    float ratio = (total > 0) ? (float)used / (float)total : 0.0f;
    ImGui::ProgressBar(ratio, ImVec2(-1, 0), "");
    ImGui::Text("Utilisation : %.4f%%", ratio * 100.0f);
    ImGui::End();
}

void CGameSubsystem::DrawPropertyEditor(class GObject* obj)
{
    if (!obj) return;
    for (const CProperty& prop : obj->GetClass()->GetAllProperties())
    {
        void* valuePtr = prop.GetValuePtr(obj);

        switch (prop.Type)
        {
            case EPropertyType::Int:
                ImGui::InputInt(prop.Name, (int*)valuePtr);
                break;

            case EPropertyType::Float:
                ImGui::DragFloat(prop.Name, (float*)valuePtr, 0.1f);
                break;

            case EPropertyType::Bool:
                ImGui::Checkbox(prop.Name, (bool*)valuePtr);
                break;

            case EPropertyType::String:
                // Lecture seule
                ImGui::Text("%s : %s", prop.Name, *(const char**)valuePtr);
                break;
        }
    }
}


