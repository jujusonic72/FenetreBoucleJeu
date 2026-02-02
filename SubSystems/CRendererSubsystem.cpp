#include "CRendererSubsystem.h"
#include "CGameEngine.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include <SDL3/SDL.h>

bool CRendererSubsystem::Start()
{
    m_window = CGameEngine::GetInstance().GetWindow();
    return m_window != nullptr;
}

void CRendererSubsystem::OnBeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void CRendererSubsystem::Update(float deltaSeconds)
{
    // Rien ici - la logique ImGui UI est dans CGameSubsystem::Update
}

void CRendererSubsystem::OnEndFrame()
{
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
}

void CRendererSubsystem::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
