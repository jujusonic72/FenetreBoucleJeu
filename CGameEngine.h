#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "CPageAlocator.h"

// Forward declarations
class ISubsystem;
class CCounter;
class CRendererSubsystem;
class CInputSubsystem;
class CGameSubsystem;

struct SDL_Window;
typedef struct SDL_GLContextState *SDL_GLContext;

using namespace std;

/*
 * Typedef pour l'allocateur du moteur.
 * NBytes = 128 : chaque page fait 128 bytes (assez pour nos composants/entités)
 * NObjects = 1024*1024 : ~1 million de pages = 128 Mo de mémoire pré-allouée
 */
using GameAllocator = CPageAllocator<128, 1024 * 1024>;
/**
 * class CGameEngine 
 * C'est la classe principale du moteur
 * Sert à initialiser, stocker et coordiner les subsystems.
 */
class CGameEngine
{
public:
    // Récupère l'instance unique du moteur et retourne une référence
    static CGameEngine& GetInstance();

    // Initialise le moteur et tous les subsystems
    // retourne true si l'initialisation a réussi
    bool Init();

    // Lance la boucle principale du jeu
    void Loop();

    // Arrête le moteur et libère les ressources
    void Shutdown();

    // Getters pour les Subsystems
    CRendererSubsystem* GetRenderer() const { return renderer; }
    CInputSubsystem* GetInput() const { return input; }
    CGameSubsystem* GetGame() const { return game; }

    // Getters SDL
    SDL_Window* GetWindow() const { return window; }
    SDL_GLContext GetGLContext() const { return gl_context; }

    // État du moteur
    bool IsRunning() const { return running; }
    void RequestQuit() { running = false; }

    GameAllocator* GetAllocator() const { return pageAlloc.get(); }


private:
    // Constructeur privé (Singleton)
    CGameEngine();
    ~CGameEngine();


    void RegisterSubsystem(ISubsystem* pSubsystem);


    void UpdateSubsystems(float deltaSeconds);


    void ShutdownSubsystems();

    // État du moteur
    bool running;
    bool initialized;

    // Ressources SDL / OpenGL
    SDL_Window* window;
    SDL_GLContext gl_context;

    // Compteur de temps (delta time)
    unique_ptr<CCounter> counter;

    // Subsystems (ordre d'initialisation important)
    CInputSubsystem* input;
    CRendererSubsystem* renderer;
    CGameSubsystem* game;

    // Liste de tous les subsystems pour itération
    vector<ISubsystem*> subsystems;

    unique_ptr<GameAllocator> pageAlloc;
}; 
