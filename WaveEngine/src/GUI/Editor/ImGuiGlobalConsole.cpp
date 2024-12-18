#include "ImGuiGlobalConsole.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "ImGuiEditor.hpp"
#include "Render/RenderSystem.hpp"

void ImGuiGlobalConsole::render() {
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoCollapse);

    ImVec2 dummy = ImGui::CalcTextSize("A");
    auto separator = [dummy]() {
        ImGui::Dummy(dummy);
        ImGui::Separator();
        ImGui::Dummy(dummy);
    };

    auto& render_params = m_parent->ref_render_system->renderParams();

    ImGui::Dummy(dummy);
    ImGui::PushItemWidth(150.0f);
    int path_type_option = (int)m_parent->ref_render_system->getRenderPathType();
    std::array<std::string, 3> combo_strs = { "Forward", "Deferred", "RayTracing" };
    ImGui::Text("Choose Render Path:");
    if (ImGui::BeginCombo("##Render Path", combo_strs[path_type_option].c_str())) {
        for (int i = 0; i < combo_strs.size(); i++) {
            bool selected = path_type_option == i;
            if (ImGui::Selectable(combo_strs[i].c_str(), selected)) {
                m_parent->ref_render_system->setRenderPathType(RenderPathType(i));
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    separator();

    ImGui::Text("Choose Shader:");
    if (ImGui::RadioButton("BlinnPhong", !render_params.checkerboard && !render_params.pbr)) {
        render_params.pbr = false;
        render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("PBR", render_params.pbr)) {
        render_params.pbr = true;
        render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Checkerboard", render_params.checkerboard)) {
        render_params.pbr = false;
        render_params.checkerboard = true;
    }
    //ImGui::SliderInt("pixel style", &render_params.pixelate_level, 1, 16);

    separator();

    ImGui::Text("Params:");
    if (m_parent->ref_render_system->getRenderPathType() == RenderPathType::Forward) {
        ImGui::PushItemWidth(50.0f);
        //static unsigned int curr_item = 1;
        //if (ImGui::BeginCombo("MSAA", (std::to_string((int)std::pow(4, curr_item)) + "x").c_str())) {
        //    for (int i = 0; i < 3; i++) {
        //        bool selected = curr_item == i;
        //        std::string label = std::to_string((int)std::pow(4, i)) + "x";
        //        if (ImGui::Selectable(label.c_str(), selected)) {
        //            curr_item = i;
        //            render_params.msaa_sample_count = (int)std::pow(4, i);
        //        }
        //    }
        //    ImGui::EndCombo();
        //}
        //ImGui::SameLine();
        //static unsigned int shadow_curr_item = 0;
        //if (ImGui::BeginCombo("Shadow Map Resolution", (std::to_string((int)std::pow(4, shadow_curr_item)) + "x").c_str())) {
        //    for (int i = 0; i < 3; i++) {
        //        bool selected = shadow_curr_item == i;
        //        std::string label = std::to_string((int)std::pow(4, i)) + "x";
        //        if (ImGui::Selectable(label.c_str(), selected)) {
        //            shadow_curr_item = i;
        //            render_params.shadow_map_sample_count = (int)std::pow(4, i);
        //        }
        //    }
        //    ImGui::EndCombo();
        //}
        ImGui::PopItemWidth();
        ImGui::SameLine();
    }
    ImGui::Checkbox("skybox", &render_params.skybox); ImGui::SameLine();
    ImGui::Checkbox("shadow", &render_params.shadow); ImGui::SameLine();
    //ImGui::Checkbox("reflection", &render_params.reflection); ImGui::SameLine();
    ImGui::Checkbox("wireframe", &render_params.wireframe); ImGui::SameLine();
    ImGui::Checkbox("normal", &render_params.normal_debug); ImGui::SameLine();
    ImGui::Checkbox("bloom", &render_params.bloom);

    separator();

    // TODO Add/Delete point light
    //ImGui::Text("Add/Delete point light:");
    //auto scene_hierarchy = scene();
    //int point_light_count = scene_hierarchy->pointLightCount();
    //float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    //ImGui::PushButtonRepeat(true);
    //if (ImGui::Button((std::string("+") + "##pointLight").c_str())) {
    //    scene_hierarchy->addPointLight();
    //}
    //ImGui::SameLine(0.0f, spacing);
    //if (ImGui::Button((std::string("-") + "##pointLight").c_str())) {
    //    int last_point_entity_id = -1;
    //    auto it = world.entityView<ecs::PointLightComponent>().begin();
    //    while(it != world.entityView<ecs::PointLightComponent>().end())
    //    {
    //        last_point_entity_id = (*it).getId();
    //        it++;
    //    }
    //    if (last_point_entity_id != -1)
    //        scene_hierarchy->removeObject(ecs::Entity(last_point_entity_id));
    //}
    //ImGui::PopButtonRepeat();
    //ImGui::SameLine();
    //ImGui::Text("point light number: %d", point_light_count);

    ImGui::End();
}
