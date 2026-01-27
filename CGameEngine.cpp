#include "CGameEngine.h"
#include "SubSystems/ISubsystem.h"
#include "CCounter.h"
#include "SubSystems/CRendererSubsystem.h"
#include "SubSystems/CInputSubsystem.h"
#include "SubSystems/CGameSubsystem.h"

#include <SDL3/SDL.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

// Singleton Implementation

CGameEngine& CGameEngine::GetInstance()
{
    static CGameEngine instance;
    return instance;
}

CGameEngine::CGameEngine()
    : running(false)
    , initialized(false)
    , window(nullptr)
    , SDLRenderer(nullptr)
    , counter(nullptr)
    , input(nullptr)
    , renderer(nullptr)
    , game(nullptr)
{
}

CGameEngine::~CGameEngine()
{
    // S'assurer que Shutdown a été appelé
    if (initialized)
    {
        Shutdown();
    }
}

// Initialization
bool CGameEngine::Init()
{
    std::cout << "Init: start" << std::endl;
    // Si je suis déjà initialisé on initalise pas
    if (initialized)
    {
        std::cerr << "[CGameEngine] Already initialized!" << std::endl;
        return false;
    }
    
    std::cout << "Init: calling SDL_Init..." << std::endl;
    // Initialisation de SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) //Ce init va aussi initialiser le subSystem des Events
    {
        std::cerr << "[CGameEngine] SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    std::cout << "Init: SDL_Init OK, creating window..." << std::endl;
    SDL_Window* window = SDL_CreateWindow("Fenetre Boucle de Jeu", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::cout << "Init: window created, creating counter..." << std::endl;
    // Initialisation du compteur de temps (unique)
    counter = std::make_unique<CCounter>();

    std::cout << "Init: counter created, creating subsystems..." << std::endl;
    // Création et initialisation des Subsystems

    std::cout << "Init: creating CInputSubsystem..." << std::endl;
    // Input Subsystem
    input = new CInputSubsystem();
    std::cout << "Init: registering CInputSubsystem..." << std::endl;
    RegisterSubsystem(input);

    std::cout << "Init: creating CGameSubsystem..." << std::endl;
    // Game Subsystem
    game = new CGameSubsystem();
    std::cout << "Init: registering CGameSubsystem..." << std::endl;
    RegisterSubsystem(game);

    std::cout << "Init: creating CRendererSubsystem..." << std::endl;
    // Renderer Subsystem
    renderer = new CRendererSubsystem();
    std::cout << "Init: registering CRendererSubsystem..." << std::endl;
    RegisterSubsystem(renderer);

    std::cout << "Init: starting subsystems..." << std::endl;
    // Démarrage de tous les subsystems
    for (ISubsystem* pSubsystem : subsystems)
    {
        std::cout << "Init: calling Start() on a subsystem..." << std::endl;
        if (!pSubsystem->Start())
        {
            std::cerr << "[CGameEngine] Failed to start a subsystem!" << std::endl;
            Shutdown();
            return false;
        }
        std::cout << "Init: subsystem started OK" << std::endl;
    }

    // Si on s'est rendu jusqu'ici ça veut dire que l'initialisation a réussie et que le moteur roule
    initialized = true;
    running = true;

    return true;
}

// Main Loop
void CGameEngine::Loop()
{
    //au as où la loop est appelé avant l'initialisation ou si l'initialisation s'est mal passée
    if (!initialized)
    {
        std::cerr << "[CGameEngine] Cannot run loop - not initialized!" << std::endl;
        return;
    }

    while (running)
    {
        // Calculer le delta time
        counter->UpdateCounter();
        float deltaSeconds = counter->GetDeltaSeconds();

        // Mettre à jour tous les subsystems
        UpdateSubsystems(deltaSeconds);

        running = !input->QuitEvent;
    }

    // fin de la main loop
    Shutdown();
}

// Shutdown
void CGameEngine::Shutdown() 
{
    // rien à shutdown si on est pas initialisé
    if (!initialized)
    {
        return;
    }

    // Arrêter tous les subsystems (ordre inverse)
    ShutdownSubsystems();

    // Libérer les subsystems
    delete renderer;
    renderer = nullptr;

    delete game;
    game = nullptr;

    delete input;
    input = nullptr;

    subsystems.clear();

    // Libérer le compteur
    counter.reset();

    // Libérer les ressources SDL
    if (SDLRenderer)
    {
        SDL_DestroyRenderer(SDLRenderer);
        SDLRenderer = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit(); //termine le cleanup si on a oublié qqch puis ferme

    initialized = false;
    running = false;

    //Shutdown terminé
}

// Ajout des subsystems dans le vecteur en préservant l'ordre
void CGameEngine::RegisterSubsystem(ISubsystem *pSubsystem)
{
    subsystems.push_back(pSubsystem);
}

// Appel des Update() pour les Subsystems
void CGameEngine::UpdateSubsystems(float deltaSeconds)
{
    for (ISubsystem* pSubsystem : subsystems)
    {
        pSubsystem->Update(deltaSeconds);
    }
}

// Shutdown des subsystems dans l'ordre inverse pour éviter les ereurs de dépendances
void CGameEngine::ShutdownSubsystems()
{
    for (int i = subsystems.size() - 1; i >= 0; --i)
    {
        subsystems[i]->Shutdown();
    }
}

