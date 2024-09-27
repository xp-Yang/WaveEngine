#include "ImGuiContextMenu.hpp"

#include <imgui.h>

#include <Engine.hpp>

void ImGuiContextMenu::render()
{
    if (m_menu_opened)
        ImGui::OpenPopup("context_menu");
    if (ImGui::BeginPopup("context_menu")) {
        bool hovered = false;
        {
            if (ImGui::MenuItem("Add Cube", "", false, true))
                ;

            if (ImGui::MenuItem("Add Sphere", "", false, true))
                ;
        }

        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Add 1", "", false, true))
                ;
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void ImGuiContextMenu::popUp()
{
    m_menu_opened = true;
}

void ImGuiContextMenu::dismiss()
{
    m_menu_opened = false;
}
