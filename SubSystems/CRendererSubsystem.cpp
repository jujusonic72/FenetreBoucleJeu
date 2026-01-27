#include "CRendererSubsystem.h"
#include "CGameEngine.h"
#include <string>
#include "CGameSubsystem.h"
bool CRendererSubsystem::Start()
{
    if((m_renderer = CGameEngine::GetInstance().GetSDLRenderer()) && (m_window = CGameEngine::GetInstance().GetWindow())) return true;
    else return false;
}

void CRendererSubsystem::Update(float deltaSeconds)
{
    float moyenne = CGameEngine::GetInstance().GetGame()->GetAverage();
    std::string str = std::to_string(moyenne);
    const char* message = str.c_str();
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(m_renderer, &w, &h);
    SDL_SetRenderScale(m_renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_Color couleur = CGameEngine::GetInstance().GetGame()->GetBackgroundColor();
    SDL_SetRenderDrawColor(m_renderer,couleur.r, couleur.g, couleur.b, couleur.a);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(m_renderer, x, y, message);
    SDL_RenderPresent(m_renderer);

}

void CRendererSubsystem::Shutdown()
{
}
