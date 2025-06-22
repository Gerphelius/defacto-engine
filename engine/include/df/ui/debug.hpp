#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "render/window.hpp"

namespace DF::Core {
    class Engine;
}

namespace DF::UI
{
    class Debug
    {
    private:
        static inline std::unordered_map<std::string, std::function<void()>> m_widgets{};

    public:
        static void addWidget(const std::string& name, std::function<void()> body)
        {
            m_widgets[name] = body;
        }

    private:
        static void init(const Render::Window* window)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            ImGui_ImplOpenGL3_Init();
            ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->getRawWindow()), true);
        }

        static void render()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            for (const auto& [name, body] : m_widgets)
            {
                ImGui::Begin(name.c_str());
                body();
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        static void destroy()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        friend class Core::Engine;
    };
}
