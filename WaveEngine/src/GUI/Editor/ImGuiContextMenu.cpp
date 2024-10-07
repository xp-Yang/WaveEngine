#include "ImGuiContextMenu.hpp"

#include <imgui.h>

#include "ImGuiEditor.hpp"
#include "Logical/FrameWork/Scene.hpp"

void ImGuiContextMenu::render()
{
    if (m_menu_opened) {
        ImGui::OpenPopup("context_menu");
        m_menu_opened = false;
    }

    if (ImGui::BeginPopup("context_menu")) {
        if (m_context == ContextType::Object) {
            auto& context_objs = m_parent->ref_scene->getPickedObjects();
            static bool visilbe_selected = false;
            if (ImGui::MenuItem("Visible", "", visilbe_selected, true))
                for (auto obj : context_objs) {
                    obj->setVisible(visilbe_selected);
                }
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