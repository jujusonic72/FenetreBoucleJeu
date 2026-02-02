#include "CInputSubsystem.h"
#include "imgui_impl_sdl3.h"

bool CInputSubsystem::Start()
{
    return true;
}

void CInputSubsystem::Update(float deltaSeconds)
{
    //mettre tout les bouttons qui sont a JUST_PRESSED a DOWN
    for (auto& pair : buttons)
    {
        if(pair.second == EButtonState::JUST_PRESSED) pair.second = EButtonState::DOWN;
    }

    //update les inputs
    SDL_Event event;
    while (SDL_PollEvent(&event))// poll until all events are handled!
    {
        // Transmettre les events a ImGui
        ImGui_ImplSDL3_ProcessEvent(&event);

        if(event.type == SDL_EVENT_KEY_DOWN)
        {
            buttons.insert_or_assign(event.key.scancode, EButtonState::JUST_PRESSED);
        }
        if(event.type == SDL_EVENT_KEY_UP)
        {
            buttons.insert_or_assign(event.key.scancode, EButtonState::UP);
        }
        if (event.type == SDL_EVENT_QUIT)
        {
            QuitEvent = true;
        }
    }
}

CInputSubsystem::EButtonState CInputSubsystem::GetButtonState(SDL_Scancode key){
    auto it = buttons.find(key);
    if (it == buttons.end()) return EButtonState::UP;
    return it->second;
}

void CInputSubsystem::Shutdown()
{
}
