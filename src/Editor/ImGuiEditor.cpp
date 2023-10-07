#include "ImGuiEditor.hpp"
#include <string>
#include <glm/glm.hpp>
#include "../../imgui/imgui.h"
#include "../GamePlay/ECS/Components.hpp"
#include "../Core/Logger.hpp"

ImGuiEditor::ImGuiEditor()
{
	stop_rotate = false;
    global_ambient_strength = 0.1f;
    icosphere_accuracy = 8;

    // TODO: editor依赖system，能操控渲染管线，能操控运动，能编辑transform，能编辑mesh，能编辑material...
}

void ImGuiEditor::init(std::shared_ptr<ecs::RenderSystem> render_system, std::shared_ptr<ecs::MotionSystem> motion_system)
{
    ref_render_system = render_system;
    ref_motion_system = motion_system;
}

void ImGuiEditor::render()
{
   render_global_editor();
   render_camera_editor();
   render_entity_editor();
   update_render_params();
}

void ImGuiEditor::render_global_editor() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As..")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Global Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    static unsigned int pipeline_type_option = (int)m_render_params.pipeline_type;
    if (ImGui::BeginCombo("Render Pipeline Type", pipeline_type_option == 0 ? "Forward" : "Deferred")) {
        for (int i = 0; i < 2; i++) {
            bool selected = pipeline_type_option == i;
            std::string label = i == 0 ? "Forward" : "Deferred";
            if (ImGui::Selectable(label.c_str(), selected)) {
                pipeline_type_option = i;
                m_render_params.pipeline_type = (PIPELINE_TYPE)pipeline_type_option;
            }
        }
        ImGui::EndCombo();
    }
    
    static unsigned int curr_item = 1;
    if (ImGui::BeginCombo("MSAA", (std::to_string((int)std::pow(4, curr_item)) + "x").c_str())) {
        for (int i = 0; i < 3; i++) {
            bool selected = curr_item == i;
            std::string label = std::to_string((int)std::pow(4, i)) + "x";
            if (ImGui::Selectable(label.c_str(), selected)) {
                curr_item = i;
                m_render_params.msaa_sample_count = (int)std::pow(4, i);
            }
        }
        ImGui::EndCombo();
    }
    static unsigned int shadow_curr_item = 1;
    if (ImGui::BeginCombo("Shadow Map Resolution", (std::to_string((int)std::pow(4, shadow_curr_item)) + "x").c_str())) {
        for (int i = 0; i < 3; i++) {
            bool selected = shadow_curr_item == i;
            std::string label = std::to_string((int)std::pow(4, i)) + "x";
            if (ImGui::Selectable(label.c_str(), selected)) {
                shadow_curr_item = i;
                m_render_params.shadow_map_sample_count = (int)std::pow(4, i);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Checkbox("shadow", &m_render_params.shadow);
    ImGui::Checkbox("reflection", &m_render_params.reflection);
    ImGui::Checkbox("normal", &m_render_params.normal_debug);
    ImGui::Checkbox("wireframe", &m_render_params.wireframe);
    ImGui::SliderInt("pixel style", &m_render_params.pixelate_level, 1, 16);
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}

void ImGuiEditor::render_camera_editor()
{
    ImGui::Begin("Camera Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto& world = ecs::World::get();

    for (auto entity : world.entityView<ecs::CameraComponent>()) {
        auto& camera = *world.getComponent<ecs::CameraComponent>(entity);
        ImGui::NewLine();
        ImGui::Text("view matrix:");
        std::string test_view = matrix_log(camera.view);
        ImGui::Text(test_view.c_str());

        ImGui::NewLine();
        ImGui::Text("inverse view matrix:");
        std::string inverse_view = matrix_log(glm::inverse(camera.view));
        ImGui::Text(inverse_view.c_str());

        ImGui::NewLine();
        ImGui::Text("camera position:");
        std::string test_camera_pos = vec3_log(camera.pos);
        ImGui::Text(test_camera_pos.c_str());

        ImGui::NewLine();
        ImGui::Text("camera direction:");
        std::string test_camera_dir = vec3_log(camera.direction);
        ImGui::Text(test_camera_dir.c_str());
    }
    ImGui::End();
}

void ImGuiEditor::render_entity_editor()
{
    // TODO 考虑对材质的编辑放入MaterialSystem
    auto& world = ecs::World::get();

    for (auto entity : world.entityView<ecs::NameComponent, ecs::PickedComponent, ecs::RenderableComponent, ecs::TransformComponent>()) {
        auto& renderable = *world.getComponent<ecs::RenderableComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);
        std::string obj_name = world.getComponent<ecs::NameComponent>(entity)->name;


        glm::vec4 color = renderable.primitives[0].material.color;
        float shininess = renderable.primitives[0].material.shininess;

        ImGui::Begin((obj_name + " controller").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::ColorEdit3((std::string("color") + "##" + obj_name).c_str(), (float*)&color);
        ImGui::SliderFloat((std::string("shininess") + "##" + obj_name).c_str(), &shininess, 0.1f, 512.0f);
        ImGui::PushItemWidth(85.0f);
        ImGui::SliderFloat((std::string("##x") + "##" + obj_name).c_str(), &model_matrix.translation.x, -10.0f, 10.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("##y") + "##" + obj_name).c_str(), &model_matrix.translation.y, -10.0f, 10.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("xyz") + "##" + obj_name).c_str(), &model_matrix.translation.z, -10.0f, 10.0f);
        ImGui::PopItemWidth();
        // 编辑对象材质属性
        //ecs::MaterialSystem::onUpdate();
        for (int i = 0; i < renderable.primitives.size(); i++) {
            auto& material = renderable.primitives[i].material;
            //material.materials[i].ambient_strength;
            material.color = color;
            material.shininess = shininess;
            //curr_entity->material(i).set_diffuse_map();
            //curr_entity->material(i).set_specular_map();
        }
        //// 是否开启法向量检查
        //;
        //// 编辑对象是否开启反射
        //if (enable_reflection) {
        //    world.addComponent<ecs::ReceiveReflectionComponent>(entity);
        //}
        //else {
        //    world.removeComponent<ecs::ReceiveReflectionComponent>(entity);
        //}
        // 编辑对象的explosion ratio
        if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
            auto& explosion = *world.getComponent<ecs::ExplosionComponent>(entity);
            ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion.explosionRatio, 0.0f, 1.0f);
        }

        // log
        {
            ImGui::NewLine();
            ImGui::Text("model matrix:");
            std::string test_light = matrix_log(model_matrix.transform());
            ImGui::Text(test_light.c_str());
        }

        //if (obj_name == "sphere") {
        //    ImGui::SliderInt("icosphere accuracy", &icosphere_accuracy, 0, 10);
        //    if (icosphere_accuracy != sphere->get_recursive_depth()) {
        //        mesh.meshes.pop_back();
        //        mesh.meshes.push_back(Mesh::create_icosphere_mesh(icosphere_accuracy));
        //    }
        //}

        ImGui::End();
        break; //只选择一个
    }
}

void ImGuiEditor::update_render_params()
{
    ref_render_system->setRenderParams(m_render_params);
}
