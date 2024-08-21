#include "ImGuiGlobalConsole.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <Engine.hpp>

ImGuiGlobalConsole::ImGuiGlobalConsole()
    : m_ref_render_system(GetApp().renderSystem())
{
}

void ImGuiGlobalConsole::render() {
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImVec2 dummy = ImGui::CalcTextSize("A");
    auto separator = [dummy]() {
        ImGui::Dummy(dummy);
        ImGui::Separator();
        ImGui::Dummy(dummy);
    };

    ImGui::Dummy(dummy);
    ImGui::PushItemWidth(150.0f);
    int path_type_option = (int)m_ref_render_system->getRenderPathType();
    std::array<std::string, 3> combo_strs = { "Forward", "Deferred", "RayTracing" };
    ImGui::Text("Choose Render Path:");
    if (ImGui::BeginCombo("##Render Path", combo_strs[path_type_option].c_str())) {
        for (int i = 0; i < combo_strs.size(); i++) {
            bool selected = path_type_option == i;
            if (ImGui::Selectable(combo_strs[i].c_str(), selected)) {
                m_ref_render_system->setRenderPathType(RenderPath::Type(i));
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    separator();

    ImGui::Text("Choose Shader:");
    if (ImGui::RadioButton("Default", !m_render_params.wireframe && !m_render_params.checkerboard && !m_render_params.pbr)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("PBR", m_render_params.pbr)) {
        m_render_params.pbr = true;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Wireframe", m_render_params.wireframe)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = true;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Checkerboard", m_render_params.checkerboard)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = true;
    }
    //ImGui::SliderInt("pixel style", &m_render_params.pixelate_level, 1, 16);

    separator();

    ImGui::Text("Params:");
    if (m_ref_render_system->getRenderPathType() == RenderPath::Type::Forward) {
        ImGui::PushItemWidth(50.0f);
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
        static unsigned int shadow_curr_item = 0;
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
        ImGui::PopItemWidth();
    }
    ImGui::Checkbox("skybox", &m_render_params.skybox); ImGui::SameLine();
    ImGui::Checkbox("shadow", &m_render_params.shadow); ImGui::SameLine();
    //ImGui::Checkbox("reflection", &m_render_params.reflection);
    ImGui::Checkbox("normal", &m_render_params.normal_debug);

    separator();

    // TODO Add/Delete point light
    //ImGui::Text("Add/Delete point light:");
    //auto scene_hierarchy = GetApp().scene();
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

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}

void ImGuiGlobalConsole::updateRenderParams()
{
    // TODO
    //m_ref_render_system->setRenderParams(m_render_params);
}
