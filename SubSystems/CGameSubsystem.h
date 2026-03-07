#pragma once

#include "GObject.h"
#include "ISubsystem.h"
#include "SDL3/SDL.h"
#include "imgui.h"
#include <vector>

class CEntity;
class GObject;
class GScene;

class CGameSubsystem : public ISubsystem
{
public:
    bool Start() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;
    float GetAverage() {return moyenne; }
    SDL_Color GetBackgroundColor() { return { 15, 15, 15, 255 }; }

    //fonctions publiques pour le Observer pattern

    /*
     * Subscribe : enregistre un GObject pour recevoir Start() et Update(dt).
     * À appeler depuis le constructeur du script.
     * Le subscriber reste propriétaire de sa propre mémoire.
     */
    static void Subscribe(GObject* obj);

    /*
     * Unsubscribe : retire un GObject de la liste.
     * À appeler depuis Start() si le script n'a pas besoin d'Update,
     * ou depuis le destructeur du script.
     */
    static void Unsubscribe(GObject* obj);

    //API pour l'affichage de primitives
    static void DrawCircle(float x, float y, float radius, ImU32 color, int segments = 32, float thickness = 1.0f);
    static void DrawCircleFilled(float x, float y, float radius, ImU32 color, int segments = 32);

    static void DrawRect(float x, float y, float w, float h, ImU32 color, float thickness = 1.0f);
    static void DrawRectFilled(float x, float y, float w, float h, ImU32 color);

    static void DrawLine(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f);

    static void DrawText(float x, float y, ImU32 color, const char* text);

    /*
     * Retourne l'origine (coin supérieur gauche) de la fenêtre de jeu en coordonnées écran.
     * Utile pour convertir des coordonnées monde en coordonnées écran.
     */
    static ImVec2 GetGameWindowOrigin();

private:
    // --- Stats delta time ---
    float frames[100] = {}, timer = 0, moyenne = 0;
    int index = 0, frameCount = 0, backgroundColor = 0;

    // --- Scène active ---
    GScene* m_activeScene  = nullptr;
    bool    m_sceneStarted = false;

    // --- Play / Stop ---
    // m_isPlaying    : true si le jeu tourne (Update des entités actif)
    // m_snapshotPath : chemin du JSON sauvegardé au moment du Play
    //                  utilisé pour recharger la scène au Stop
    bool        m_isPlaying    = false;
    std::string m_snapshotPath;

    // --- Scanner de scènes ---
    // Liste des noms de scènes trouvés dans Scenes/*.json
    std::vector<std::string> m_sceneNames;

    // --- Création de scène ---
    char m_newSceneNameBuffer[64] = "";

    // --- Éditeur ECS ---
    char entityNameBuffer[64] = "NewEntity";
    int selectedEntity = -1;

    // --- Subscribers (GObjects sans scène) ---
    static std::vector<GObject*> s_subscribers;
    bool m_subscribersStarted = false;

    // DrawList actif — valide uniquement à l'intérieur de DrawGameWindow()
    static ImDrawList* s_drawList;
    static ImVec2 s_gameWindowOrigin;

    // --- Fenêtres ImGui ---

    /*
     * DrawAllocatorWindow : affiche l'état de l'allocateur (Étape 1)
     * - Pages libres / utilisées / total
     * - Barre de progression visuelle
     */
    void DrawAllocatorWindow();

    /*
     * DrawECSWindow : affiche l'éditeur ECS complet (Étape 3)
     * - Création d'entités nommées
     * - Liste des entités avec sélection
     * - Ajout de composants à l'entité sélectionnée
     * - Destruction d'entités
     * - Édition des propriétés via réflexion (Étape 2)
     */
    void DrawECSWindow();

    /*
     * DrawSceneSelectorWindow : liste toutes les scènes enregistrées via REGISTER_SCENE.
     * Bouton "Charger" : Save(ancienne) → ClearEntities(ancienne) → free(ancienne) → Load(nouvelle).
     * Bouton "Sauvegarder" : Save(scène active).
     */
    void DrawSceneSelectorWindow();

    /*
     * DrawPropertyEditor : affiche l'éditeur de propriétés pour un objet
     * Utilise GetAllProperties() pour itérer sur les propriétés et
     * affiche le bon widget ImGui selon le type de propriété.
     * C'est ici que la réflexion (Étape 2) rencontre l'interface (Étape 3).
     */
    void DrawPropertyEditor(class GObject* obj);

    /*
     * DrawGameWindow : ouvre la fenêtre de jeu, configure s_drawList et s_gameWindowOrigin,
     * puis dispatche Update(dt) à tous les subscribers.
     */
    void DrawGameWindow(float deltaSeconds);

    // --- Helpers ---

    /*
     * ScanScenes : parcourt Scenes/*.json et remplit m_sceneNames.
     * Appelé au démarrage (Start) et à la demande (bouton Rafraîchir).
     */
    void ScanScenes();

    /*
     * LoadScene : Save(ancienne) → Clear(ancienne) → Free(ancienne) → Load(nouvelle).
     * Priorité : CSceneRegistry (scènes compilées) puis GScene générique.
     */
    void LoadScene(const std::string& sceneName);

    /*
     * CreateScene : génère Scenes/<nom>.json, Scenes/<nom>.h, Scenes/<nom>.cpp
     * puis rescanne le dossier.
     */
    void CreateScene(const std::string& sceneName);

    // Play : sauvegarde la scène puis active l'update des entités
    void Play();

    // Stop : recharge la scène depuis le snapshot sauvegardé au Play
    void Stop();
};
