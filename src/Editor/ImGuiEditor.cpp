#include "ImGuiEditor.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logger.hpp"

ImGuiEditor::ImGuiEditor()
{
	pixel_style = false;
	stop_rotate = false;
	normal_debug = false;
	enable_shadow = false;
	enable_reflection = false;
    global_ambient_strength = 0.1f;
    icosphere_accuracy = 8;
}

void ImGuiEditor::render()
{
   render_global_editor();
   render_camera_editor();
   render_entity_editor();
}

void ImGuiEditor::render_global_editor() {
    ImGui::Begin("Global Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Checkbox("pixel style", &pixel_style);
    //ImGui::SliderFloat("global ambient strength", &global_ambient_strength, 0.0f, 1.0f);

	static float time_value = 0.0f;
	if (!stop_rotate) {
	    time_value = (float)glfwGetTime();
	}
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value, glm::vec3(0.0f, 1.0f, 0.0f));
    auto& world = ecs::World::get();
    for (auto entity : world.entityView<ecs::LightComponent>()) {
        auto& light_transform = *world.getComponent<ecs::TransformComponent>(entity);
        static auto original_translate = glm::vec4(light_transform.translation, 1.0f);
        light_transform.translation = rotate * original_translate;
    }
    ImGui::Checkbox("light: stop rotate", &stop_rotate);
    ImGui::Checkbox("enable normal debug", &normal_debug);
    ImGui::Checkbox("enable shadow", &enable_shadow);

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
        ImGui::Checkbox((std::string("enable reflection") + "##" + obj_name).c_str(), &enable_reflection);
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