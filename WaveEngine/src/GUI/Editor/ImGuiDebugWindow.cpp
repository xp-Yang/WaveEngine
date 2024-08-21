#include "ImGuiDebugWindow.hpp"

#include <imgui.h>

#include <Engine.hpp>

void ImGuiDebugWindow::render()
{
    ImGui::Begin("Main Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

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
    ImGui::End();
}
