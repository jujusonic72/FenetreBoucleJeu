#pragma once
#include "ISubsystem.h"
#include "SDL3/SDL.h"
class CGameSubsystem : public ISubsystem
{
public:
    bool Start() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;
    float GetAverage() {return moyenne; }
    SDL_Color GetBackgroundColor() { return palette->colors[backgroundColor]; }
private:
    float frames[100] = {}, timer = 0, moyenne = 0;
    int index = 0, frameCount = 0, backgroundColor = 0;
    SDL_Palette* palette;
    bool show_demo_window = true;
};