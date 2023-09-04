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
	ambient_strength = 0.1f;
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
    ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);

    // TODO 应该移动至entity的editor
    //ImGui::SliderFloat("ground diffuse strength", &ground.material().diffuse_strength, 0, 1);
    //ImGui::SliderFloat("ground specular strength", &ground.material().specular_strength, 0, 1);
    //ImGui::SliderFloat("ground shininess", &ground.material().shininess, 0, 512.f);

    //if (editor.icosphere_accuracy != sphere->get_recursive_depth()) {
	//    sphere->create_icosphere(editor.icosphere_accuracy);
	//    editor.icosphere_accuracy = sphere->get_recursive_depth();
	//}

	//static float time_value = 0.0f;
	//if (editor.stop_rotate) {
	//    ;
	//}
	//if (!editor.stop_rotate) {
	//    time_value = glfwGetTime();
	//}
	//auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
	//glm::mat4 displacement(1.0f);
	//displacement[3].x = 12.0f;
	//glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), time_value * 3, glm::vec3(0.0f, 1.0f, 0.0f));
	//auto translate = glm::translate(glm::mat4(1.0f), { 0.0f, 12.0f, 0.0f });

    // TODO 应该移动至entity的editor
    //ImGui::SliderInt("icosphere accuracy", &icosphere_accuracy, 0, 10);

    ImGui::Checkbox("stop rotate", &stop_rotate);
    ImGui::Checkbox("open normal debug", &normal_debug);

    // log
    // TODO 应该移动至entity的editor
    {
        //ImGui::NewLine();
        //ImGui::Text("light matrix:");
        //std::string test_light = matrix_log(light.get_model_matrix());
        //ImGui::Text(test_light.c_str());

        //ImGui::NewLine();
        //ImGui::Text("cube matrix:");
        //std::string test_cube = matrix_log(cube.get_model_matrix());
        //ImGui::Text(test_cube.c_str());

        //ImGui::NewLine();
        //ImGui::Text("ground matrix:");
        //std::string test_ground = matrix_log(ground.get_model_matrix());
        //ImGui::Text(test_ground.c_str());
    }

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
    auto& world = ecs::World::get();

    ecs::Entity curr_entity = -1;
    for (auto entity : world.entityView<ecs::PickedComponent, ecs::MeshComponent, ecs::MaterialComponent, ecs::TransformComponent>()) {
        curr_entity = entity;
        auto& mesh = *world.getComponent<ecs::MeshComponent>(entity);
        auto& material = *world.getComponent<ecs::MaterialComponent>(entity);
        auto& model_matrix = *world.getComponent<ecs::TransformComponent>(entity);


        //bool visible = curr_entity->renderable();
        //bool reflection = curr_entity->is_enable_reflection();
        glm::vec3 color = material.materials[0].color;
        float shininess = material.materials[0].shininess;
        //float shininess = curr_entity->material().shininess;
        //float explosion_ratio = curr_entity->get_explostion_ratio();

        std::string obj_name = world.getComponent<ecs::NameComponent>(entity)->name;
        ImGui::Begin((obj_name + " controller").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        //ImGui::Checkbox((std::string("visible") + "##" + obj_name).c_str(), &visible);
        //ImGui::Checkbox((std::string("enable reflection") + "##" + obj_name).c_str(), &reflection);
        //ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion_ratio, 0.0f, 10.0f);
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
        for (int i = 0; i < material.materials.size(); i++) {
            //material.materials[i].ambient_strength;
            material.materials[i].color = color;
            material.materials[i].shininess = shininess;
            //curr_entity->material(i).set_diffuse_map();
            //curr_entity->material(i).set_specular_map();
        }
        //// 编辑可见性
        //curr_entity->set_renderable(visible);
        //// 是否开启法向量检查
        //;
        //// 编辑对象的explosion ratio
        //curr_entity->set_explostion_ratio(explosion_ratio);
        //// 编辑对象是否开启反射
        //curr_entity->enable_reflection(reflection);
        ImGui::End();
        break; //只选择一个
    }
}