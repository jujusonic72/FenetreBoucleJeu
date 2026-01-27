#include "CGameSubsystem.h"
#include "CInputSubsystem.h"
#include "CGameEngine.h"
#include <vector>
bool CGameSubsystem::Start()
{
    SDL_Color blue = {0, 0, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    std::vector<SDL_Color> colors = {blue, red, green, black, white};
    palette = SDL_CreatePalette(5);
    if(!palette) return false;
    SDL_SetPaletteColors(palette, colors.data(), 0, 5);
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
    if(CGameEngine::GetInstance().GetInput()->GetButtonState(SDL_SCANCODE_SPACE) == CInputSubsystem::EButtonState::JUST_PRESSED) 
    {
        backgroundColor = (backgroundColor + 1) % 5;
    }
}

void CGameSubsystem::Shutdown()
{
    if (palette)
    {
        SDL_DestroyPalette(palette);
        palette = nullptr;
    }
}
