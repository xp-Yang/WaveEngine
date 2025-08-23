#include "ImGuiSceneHierarchy.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "GUI/Editor/ImGuiEditor.hpp"
#include "Logical/FrameWork/World/Scene.hpp"
#include "GlobalContext.hpp"

ImGuiSceneHierarchy::ImGuiSceneHierarchy(ImGuiEditor* parent)
    : m_parent(parent)
{
    m_widget_creator[Meta::MetaTypeOf<GObject>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        auto& object = inst.getValue<GObject&>();
        GObjectID id = object.ID();
        std::string display_text = name + " (ID: " + std::to_string(id.id) + ")";

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        const auto& original_picked_ids = g_context.scene->getPickedObjectIDs();
        if (std::find(original_picked_ids.begin(), original_picked_ids.end(), id) != original_picked_ids.end())
            node_flags |= ImGuiTreeNodeFlags_Selected;
        else
            node_flags &= ~ImGuiTreeNodeFlags_Selected;

        bool node_open = ImGui::TreeNodeEx(display_text.c_str(), node_flags);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            g_context.scene->onPickedChanged({ id }, original_picked_ids);
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            g_context.scene->onPickedChanged({ id }, original_picked_ids);
            m_parent->popUpMenu();
        }
        if (node_open)
        {
            if (object.isLeaf()) {
                for (auto& com : object.getComponents()) {
                    Meta::Instance inst{ *com };
                    if (m_widget_creator.find(inst.typeName()) != m_widget_creator.end())
                        m_widget_creator[inst.typeName()](inst.typeName(), inst);
                }
            }
            else {
                for (auto& child : object.children()) {
                    Meta::Instance inst{ *child };
                    m_widget_creator[Meta::MetaTypeOf<GObject>().typeName()](inst.typeName(), inst);
                }
            }
            ImGui::TreePop();
        }
    };
    m_widget_creator[Meta::MetaTypeOf<DirectionalLight>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        auto& light = inst.getValue<DirectionalLight&>();
        LightID id = light.ID();
        std::string display_text = name + " (ID: " + std::to_string(id.id) + ")";

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        bool node_open = ImGui::TreeNodeEx(display_text.c_str(), node_flags);
        if (node_open)
        {
            for (auto& prop : inst.metaType().properties()) {
                auto& inst_ = prop.getValue(inst);
                if (m_widget_creator.find(inst_.typeName()) != m_widget_creator.end())
                    m_widget_creator[inst_.typeName()](prop.name, inst_);
            }
            ImGui::TreePop();
        }
    };
    m_widget_creator[Meta::MetaTypeOf<PointLight>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        auto& light = inst.getValue<PointLight&>();
        LightID id = light.ID();
        std::string display_text = name + " (ID: " + std::to_string(id.id) + ")";

        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
        bool node_open = ImGui::TreeNodeEx(display_text.c_str(), node_flags);
        if (node_open)
        {
            for (auto& prop : inst.metaType().properties()) {
                auto& inst_ = prop.getValue(inst);
                if (m_widget_creator.find(inst_.typeName()) != m_widget_creator.end())
                    m_widget_creator[inst_.typeName()](prop.name, inst_);
            }
            ImGui::TreePop();
        }
    };
    m_widget_creator[Meta::MetaTypeOf<TransformComponent>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
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

        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
        bool                   node_open = false;
        node_open = ImGui::TreeNodeEx(inst.typeName().c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
        if (node_open)
        {
            Meta::MetaType meta = inst.metaType();
            for (auto& prop : meta.properties()) {
                if (prop.isType<Vec3>())
                    DrawVecControl(prop.name, prop.getValue<Vec3&>(inst));
            }
            ImGui::TreePop();
        }
    };
    m_widget_creator[Meta::MetaTypeOf<MeshComponent>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        auto DrawSubMeshControl = [](const std::string& label, Mesh& sub_mesh, float columnWidth = 100.0f)
        {
            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushItemWidth(ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::Text("vertices size %s", std::to_string(sub_mesh.vertices.size()).c_str());

            ImGui::Text("indices size %s", std::to_string(sub_mesh.indices.size()).c_str());

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            ImGui::Columns(1);
            ImGui::PopID();
        };

        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
        bool                   node_open = false;
        node_open = ImGui::TreeNodeEx(inst.typeName().c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
        if (node_open)
        {
            MeshComponent& mesh_ptr = inst.getValue<MeshComponent&>();
            for (auto& sub_mesh : mesh_ptr.sub_meshes) {
                DrawSubMeshControl(std::string("SubMesh id ") + std::to_string(sub_mesh->sub_mesh_idx), *sub_mesh);
            }

            ImGui::TreePop();
        }
    };
    m_widget_creator[Meta::MetaTypeOf<Vec4>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        Vec4& vec = inst.getValue<Vec4&>();
        float    val[4] = { vec.x, vec.y, vec.z, vec.w };
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::DragFloat4(full_label.c_str(), val, 0.01f, 0.0f, 1.0f);
        }
        vec.x = val[0];
        vec.y = val[1];
        vec.z = val[2];
        vec.w = val[3];
    };
    m_widget_creator[Meta::MetaTypeOf<Vec3>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        Vec3& vec = inst.getValue<Vec3&>();
        float    val[3] = { vec.x, vec.y, vec.z };
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::DragFloat3(full_label.c_str(), val);
        }
        vec.x = val[0];
        vec.y = val[1];
        vec.z = val[2];
    };
    m_widget_creator[Meta::MetaTypeOf<bool>().typeName()] = [this](const std::string& name, const Meta::Instance& inst)  -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", name.c_str());
            ImGui::Checkbox(full_label.c_str(), &inst.getValue<bool&>());
        }
    };
    m_widget_creator[Meta::MetaTypeOf<int>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputInt(full_label.c_str(), &inst.getValue<int&>());
        }
    };
    m_widget_creator[Meta::MetaTypeOf<float>().typeName()] = [this](const std::string& name, const Meta::Instance& inst) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputFloat(full_label.c_str(), &inst.getValue<float&>());
        }
    };
}

void ImGuiSceneHierarchy::render()
{
    if (ImGui::Begin(("Scene Hierarchy"), nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGuiWindow* scene_hierarchy_window = ImGui::GetCurrentWindow();

        const std::vector<std::shared_ptr<Light>>& lights = g_context.scene->getLightManager()->lights();
        for (int i = 0; i < lights.size(); i++)
        {
            auto light = lights[i];
            std::string child_name = light->name();
            Meta::Instance inst{ *light };
            if (m_widget_creator.find(Meta::MetaTypeOf<DirectionalLight>().typeName()) != m_widget_creator.end())
                m_widget_creator[Meta::MetaTypeOf<DirectionalLight>().typeName()](child_name, inst);
        }

        const std::vector<std::shared_ptr<GObject>>& objects = g_context.scene->getObjects();
        for (int i = 0; i < objects.size(); i++)
        {
            auto object = objects[i];
            std::string child_name = object->name();
            Meta::Instance inst{ *object };
            if (m_widget_creator.find(Meta::MetaTypeOf<GObject>().typeName()) != m_widget_creator.end())
                m_widget_creator[Meta::MetaTypeOf<GObject>().typeName()](child_name, inst);
        }
    }
    ImGui::End();
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
        // �༭�����������
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