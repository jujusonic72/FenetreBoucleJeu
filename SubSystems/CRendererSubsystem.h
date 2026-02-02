#pragma once
#include "ISubsystem.h"

struct SDL_Window;

class CRendererSubsystem : public ISubsystem
{
public:
    bool Start() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;

    void OnBeginFrame();
    void OnEndFrame();

private:
    SDL_Window* m_window = nullptr;
};
