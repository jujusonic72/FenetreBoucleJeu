#pragma once
#include "ISubsystem.h"
#include <unordered_map>
#include <SDL3/SDL_events.h>
class CInputSubsystem : public ISubsystem
{
public:
    enum class EButtonState { UP, DOWN, JUST_PRESSED };
    bool Start() override;
    void Update(float deltaSeconds) override;
    void Shutdown() override;
    EButtonState GetButtonState(SDL_Scancode key);
    std::unordered_map<SDL_Scancode,EButtonState> buttons;
    bool QuitEvent = false;
};
