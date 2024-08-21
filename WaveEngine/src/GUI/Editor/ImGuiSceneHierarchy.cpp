#include "ImGuiSceneHierarchy.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "Logical/FrameWork/Scene.hpp"
#include <Engine.hpp>

void ImGuiSceneHierarchy::init()
{
    m_gui_creator["TransformComponent"] = [this](const std::string& name, void* value_ptr) -> void {
        auto DrawVecControl = [](const std::string& label, Vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
        {
            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X", buttonSize))
                values.x = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
            if (ImGui::Button("Y", buttonSize))
                values.y = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            if (ImGui::Button("Z", buttonSize))
                values.z = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);
            ImGui::PopID();
        };

        {
            TransformComponent* trans_ptr = static_cast<TransformComponent*>(value_ptr);

            DrawVecControl("Position", trans_ptr->translation);
            DrawVecControl("Rotation", trans_ptr->rotation);
            DrawVecControl("Scale", trans_ptr->scale);
        }
    };
    m_gui_creator[Meta::traits::className<Vec3>()] = [this](const std::string& name, void* value_ptr) -> void {
        Vec3* vec_ptr = static_cast<Vec3*>(value_ptr);
        float    val[3] = { vec_ptr->x, vec_ptr->y, vec_ptr->z };

        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::DragFloat3(full_label.c_str(), val);
        }
        vec_ptr->x = val[0];
        vec_ptr->y = val[1];
        vec_ptr->z = val[2];
    };
    m_gui_creator["bool"] = [this](const std::string& name, void* value_ptr)  -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", name.c_str());
            ImGui::Checkbox(full_label.c_str(), static_cast<bool*>(value_ptr));
        }
    };
    m_gui_creator["int"] = [this](const std::string& name, void* value_ptr) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputInt(full_label.c_str(), static_cast<int*>(value_ptr));
        }
    };
    m_gui_creator["float"] = [this](const std::string& name, void* value_ptr) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputFloat(full_label.c_str(), static_cast<float*>(value_ptr));
        }
    };
}

void ImGuiSceneHierarchy::renderNode(GObject* node)
{
    //for (int i = 0; i < node->children().size(); i++)
    //{
    //    auto child_node = node->children()[i];
    //    const auto& childe_entity = child_node->entity();
    //    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    //    for (const auto& picked_entity : world.getPickedEntities()) {
    //        if (picked_entity == childe_entity)
    //            node_flags |= ImGuiTreeNodeFlags_Selected;
    //        else
    //            node_flags &= ~ImGuiTreeNodeFlags_Selected;
    //    }
    //    std::string node_name = world.getComponent<ecs::NameComponent>(childe_entity)->name;
    //    std::string display_text = node_name + " (Entity: " + std::to_string(childe_entity.getId()) + ")";
    //    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
    //    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    //        bool close = false;
    //        for (const auto& picked_entity : world.getPickedEntities()) {
    //            if (picked_entity.getId() == childe_entity.getId()) {
    //                world.removeComponent<ecs::PickedComponent>(childe_entity);
    //                close = true;
    //            }
    //        }
    //        if (!close) {
    //            for (const auto& picked_entity : world.getPickedEntities()) {
    //                world.removeComponent<ecs::PickedComponent>(picked_entity);
    //            }
    //            world.addComponent<ecs::PickedComponent>(childe_entity);
    //        }
    //    }
    //    if (node_open)
    //    {
    //        if (child_node->children().empty()) {
    //            // TODO display all components name_str belong to this entity
    //            ImGui::Text("Entity: %d", childe_entity);
    //            if (world.hasComponent<ecs::NameComponent>(childe_entity))
    //                ImGui::Text("<NameComponent>");
    //            if (world.hasComponent<ecs::TransformComponent>(childe_entity))
    //                ImGui::Text("<TransformComponent>");
    //            if (world.hasComponent<ecs::RenderableComponent>(childe_entity))
    //                ImGui::Text("<RenderableComponent>");
    //            if (world.hasComponent<ecs::ExplosionComponent>(childe_entity))
    //                ImGui::Text("<ExplosionComponent>");
    //            if (world.hasComponent<ecs::SkyboxComponent>(childe_entity))
    //                ImGui::Text("<SkyboxComponent>");
    //            if (world.hasComponent<ecs::PointLightComponent>(childe_entity))
    //                ImGui::Text("<PointLightComponent>");
    //            if (world.hasComponent<ecs::DirectionalLightComponent>(childe_entity))
    //                ImGui::Text("<DirectionalLightComponent>");
    //        }
    //        else {
    //            renderSceneHierarchyNode(child_node);
    //        }
    //        ImGui::TreePop();
    //    }
    //}

    for (int i = 0; i < node->children().size(); i++)
    {
        auto child_node = node->children()[i];
        GObjectID child_id = child_node->ID();
        std::string child_name = child_node->name();
        std::string display_text = child_name + " (ID: " + std::to_string(child_id.id) + ")";

        static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        //for (const auto& picked_entity : world.getPickedEntities()) {
        //    if (picked_entity == childe_entity)
        //        node_flags |= ImGuiTreeNodeFlags_Selected;
        //    else
        //        node_flags &= ~ImGuiTreeNodeFlags_Selected;
        //}

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            // TODO set gobject selected
        }
        if (node_open)
        {
            if (child_node->children().empty()) {
                for (auto& com : child_node->getComponents()) {
                    auto refl = Meta::DynamicReflectionInstance(com->typeName(), com.get());
                    renderLeafNode(refl);
                }
            }
            else {
                renderNode(child_node);
            }
            ImGui::TreePop();
        }
    }
}

void ImGuiSceneHierarchy::renderLeafNode(Meta::DynamicReflectionInstance& refl_instance)
{
    static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
    bool                   node_open = false;
    node_open = ImGui::TreeNodeEx(refl_instance.className().c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
    if (node_open)
    {
        for (int i = 0; i < refl_instance.fieldCount(); i++) {
            auto field = refl_instance.field(i);
            if (field.is_array) {
                ImGui::TreePop();
                return; // TODO
            }
            std::string field_type_name = field.field_type_name;
            std::string field_name = field.field_name;
            void* var = refl_instance.getFieldValue(i);
            if (m_gui_creator.find(field_type_name) != m_gui_creator.end())
                m_gui_creator[field_type_name](field_name, var);
            else {
                auto child_refl = Meta::DynamicReflectionInstance(field_type_name, var);
                renderLeafNode(child_refl);
            }
        }
        ImGui::TreePop();
    }
}

void ImGuiSceneHierarchy::render()
{
    if (ImGui::Begin(("Scene Hierarchy"), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGuiWindow* scene_hierarchy_window = ImGui::GetCurrentWindow();

        auto scene_hierarchy = GetApp().scene();
        auto root_node = scene_hierarchy->rootObject();
        renderNode(root_node);
    }
    ImGui::End();

    //ImVec2 pos = ImVec2(scene_hierarchy_window->Pos.x + scene_hierarchy_window->Size.x, scene_hierarchy_window->Pos.y);
    //renderPickedEntityController(pos, world.getPickedEntities());
}