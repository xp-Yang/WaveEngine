#include "ImGuiSceneHierarchy.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "Logical/FrameWork/Scene.hpp"
#include <Engine.hpp>

ImGuiSceneHierarchy::ImGuiSceneHierarchy()
    : m_ref_scene(GetApp().scene())
{
}

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
    for (int i = 0; i < node->children().size(); i++)
    {
        auto child_node = node->children()[i];
        GObjectID child_id = child_node->ID();
        std::string child_name = child_node->name();
        std::string display_text = child_name + " (ID: " + std::to_string(child_id.id) + ")";

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        const auto& original_picked_ids = m_ref_scene->getPickedObjectIDs();
        if (std::find(original_picked_ids.begin(), original_picked_ids.end(), child_id) != original_picked_ids.end())
            node_flags |= ImGuiTreeNodeFlags_Selected;
        else
            node_flags &= ~ImGuiTreeNodeFlags_Selected;

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            m_ref_scene->onPickedChanged({ child_id }, original_picked_ids);
        }
        if (node_open)
        {
            if (child_node->isLeaf()) {
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
            if (field.is_array) { // TODO field.is_array
                ImGui::TreePop();
                return;
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
    if (ImGui::Begin(("Scene Hierarchy"), nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGuiWindow* scene_hierarchy_window = ImGui::GetCurrentWindow();
        auto root_node = m_ref_scene->rootObject();
        renderNode(root_node);
    }
    ImGui::End();

    // TODO render lights node
}

#if ENABLE_ECS
void ImGuiEditor::renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities)
{
    if (picked_entities.empty())
        return;
    auto entity = picked_entities[0];
    if (!world.hasComponent<ecs::NameComponent>(entity))
        return;
    std::string obj_name = world.getComponent<ecs::NameComponent>(entity)->name;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    ImGui::Begin((obj_name).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

    if (world.hasComponent<ecs::RenderableComponent>(entity)) {
        auto renderable = world.getComponent<ecs::RenderableComponent>(entity);
        // 编辑对象材质属性
        for (int i = 0; i < renderable->sub_meshes.size(); i++) {
            auto& material = renderable->sub_meshes[i].material;

            ImGui::PushItemWidth(80.0f);
            ImGui::SliderFloat((std::string("##albedo.x") + "##" + obj_name).c_str(), &material.albedo.x, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::SliderFloat((std::string("##albedo.y") + "##" + obj_name).c_str(), &material.albedo.y, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::SliderFloat((std::string("albedo") + "##" + obj_name).c_str(), &material.albedo.z, 0.0f, 1.0f);
            ImGui::PopItemWidth();

            ImGui::SliderFloat((std::string("metallic") + "##" + obj_name).c_str(), &material.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat((std::string("roughness") + "##" + obj_name).c_str(), &material.roughness, 0.01f, 1.0f);
            ImGui::SliderFloat((std::string("ao") + "##" + obj_name).c_str(), &material.ao, 0.0f, 1.0f);
        }
    }
    if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
        auto& explosion = *world.getComponent<ecs::ExplosionComponent>(entity);
        ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion.explosionRatio, 0.0f, 1.0f);
    }
    if (world.hasComponent<ecs::PointLightComponent>(entity)) {
        Vec4& luminousColor = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
        float* radius = &world.getComponent<ecs::PointLightComponent>(entity)->radius;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
        ImGui::SliderFloat((std::string("Radius") + "##" + obj_name).c_str(), radius, 5.0f, 50.0f);
    }
    if (world.hasComponent<ecs::DirectionalLightComponent>(entity)) {
        auto dir_light_component = world.getComponent<ecs::DirectionalLightComponent>(entity);
        // dir_light_component->direction = ;
        Vec4& luminousColor = dir_light_component->luminousColor;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    }

    ImGui::End();
}
#endif // ENABLE_ECS