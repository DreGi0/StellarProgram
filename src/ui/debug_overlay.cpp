/**
 * @file debug_overlay.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include "debug_overlay.h"
#include "core/window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Stellar
{
    DebugOverlay::DebugOverlay(GLFWwindow* const window)
    {
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void DebugOverlay::beginFrame() const
    {
        ImGui_ImplOpenGL3_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void DebugOverlay::endFrame() const
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    DebugOverlay::~DebugOverlay()
    {
        ImGui_ImplOpenGL3_Shutdown();

        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }
} // Stellar
