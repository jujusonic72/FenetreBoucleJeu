
#define SDL_MAIN_USE_CALLBACKS 0  /* use the main() instead of callbacks */

#include <SDL3/SDL.h>
#include "CGameEngine.h"
#include <iostream>
int main(int argc, char* argv[])
{
    std::cout << "1. Before GetInstance..." << std::endl;
    CGameEngine& engine = CGameEngine::GetInstance();
    
    std::cout << "2. After GetInstance, before Init..." << std::endl;
    if (!engine.Init())
    {
        std::cout << "Init failed!" << std::endl;
        return 1;
    }
    
    std::cout << "3. Init OK, entering loop..." << std::endl;
    engine.Loop();
    
    return 0;
}