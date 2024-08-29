#include "ImGuiDebugWindow.hpp"

#include <imgui.h>

#include <Engine.hpp>

void ImGuiDebugWindow::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    ImGui::Begin("Debug Window", nullptr, 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGuiID debug_dock_id = ImGui::GetID("Debug Dock");
    ImGui::DockSpace(debug_dock_id);
    ImGui::End();

    ImGui::Begin("Main Camera Info", nullptr, ImGuiWindowFlags_NoCollapse);
    auto& camera = GetApp().scene()->getMainCamera();
    ImGui::NewLine();
    ImGui::Text("view matrix:");
    std::string test_view = Utils::mat4ToStr(camera.view);
    ImGui::Text(test_view.c_str());
    ImGui::NewLine();
    ImGui::Text("inverse view matrix:");
    std::string inverse_view = Utils::mat4ToStr(Inverse(camera.view));
    ImGui::Text(inverse_view.c_str());
    ImGui::NewLine();
    ImGui::Text("camera position:");
    std::string test_camera_pos = Utils::vec3ToStr(camera.pos);
    ImGui::Text(test_camera_pos.c_str());
    ImGui::NewLine();
    ImGui::Text("camera direction:");
    std::string test_camera_dir = Utils::vec3ToStr(camera.direction);
    ImGui::Text(test_camera_dir.c_str());

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}
