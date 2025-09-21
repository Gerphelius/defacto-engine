#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ui_debug/ui_manager.hpp"
#include "ui_debug/world_outliner/world_outliner.hpp"
#include "render/window.hpp"

namespace DF::UI::Debug
{
    std::vector<std::unique_ptr<IDebugWidget>> UIManager::s_widgets{};

    void UIManager::init(const Render::Window* window)
    {
        // TODO: find a way to properly register widgets
        s_widgets.emplace_back(std::make_unique<WorldOutliner>());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->getRawWindow()), true);
    }

    void UIManager::render()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto& widget : s_widgets)
        {
            ImGui::Begin(widget->getName().c_str());
            widget->render();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void UIManager::destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
