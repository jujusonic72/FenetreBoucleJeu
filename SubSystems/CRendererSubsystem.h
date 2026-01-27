#pragma once
#include "ISubsystem.h"
#include "SDL3/SDL.h"
class CRendererSubsystem : public ISubsystem
{
public:
    bool Start() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
};
