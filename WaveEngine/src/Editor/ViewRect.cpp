#include "ViewRect.hpp"
#include <WaveEngine/Window.hpp>
#include <WaveEngine/Application.hpp>

class ViewRect {
public:
    virtual void render() = 0;
};

class MainView : public ViewRect {
public:
    void render() override;
};

class PickingView : public ViewRect {
public:
    void render() override;
};

class ShadowView : public ViewRect {
public:
    void render() override;
};

class RayTracingView : public ViewRect {
public:
    void render() override;
};

void MainView::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(1280, 720 + 20), ImGuiCond_Appearing);
    if (ImGui::Begin("MainView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y + 20, (int)window_size.x, (int)window_size.y - 20, Viewport::Coordinates::ScreenCoordinates };
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();
        // ImGuizmoµÄ»æÖÆ·¶Î§ÊÇmain viewport
        ImGuizmo::SetRect(viewport.x, viewport.y, viewport.width, viewport.height);
        Application::GetApp().getWindow()->setMainViewport(viewport.transToGLCoordinates());
        //renderGizmos();
    }
    ImGui::End();
}

void PickingView::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("PickingView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = ImGui::GetIO().WantPassThroughMouse || hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::Pick, viewport.transToGLCoordinates());
    }
    ImGui::End();
}

void ShadowView::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("ShadowView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::Shadow, viewport.transToGLCoordinates());
    }
    ImGui::End();
}

void RayTracingView::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(800, 450), ImGuiCond_Appearing);
    if (ImGui::Begin("RayTracingView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = ImGui::GetIO().WantPassThroughMouse || hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::RayTracing, viewport.transToGLCoordinates());
    }
    ImGui::End();
}


ViewRectManager::ViewRectManager()
{
    ViewRect* main_view = new MainView();
    ViewRect* picking_view = new PickingView();
    ViewRect* shadow_view = new ShadowView();
    ViewRect* rt_view = new RayTracingView();
    m_views.assign({ main_view, picking_view, shadow_view, rt_view });
}

ViewRectManager::~ViewRectManager()
{
    for (auto& view : m_views) {
        delete view;
    }
    m_views.clear();
}

void ViewRectManager::render()
{
    for (auto& view : m_views) {
        view->render();
    }
}
