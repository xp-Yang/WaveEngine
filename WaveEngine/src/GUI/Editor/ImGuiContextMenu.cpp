#include "ImGuiContextMenu.hpp"

#include <imgui.h>

#include <Engine.hpp>

void ImGuiContextMenu::render()
{
    if (m_menu_opened)
        ImGui::OpenPopup("context_menu");

    if (ImGui::BeginPopup("context_menu")) {
        if (m_context == ContextType::Object) {
            // TODO
            if (ImGui::MenuItem("Visible", "", false, true))
                ;
            if (ImGui::MenuItem("Transparent", "", false, true))
                ;
        }

        if (m_context == ContextType::Void) {
            if (ImGui::BeginMenu("Add")) {
                if (ImGui::MenuItem("Add Cube", "", false, true))
                    ;
                if (ImGui::MenuItem("Add Sphere", "", false, true))
                    ;
                ImGui::EndMenu();
            }
        }
        ImGui::EndPopup();
    }
}

void ImGuiContextMenu::popUp(ContextType context)
{
    m_menu_opened = true;
    m_context = context;
}

void ImGuiContextMenu::dismiss()
{
    m_menu_opened = false;
}
