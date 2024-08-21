#include "ImGuiViewPortWindow.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include "Engine.hpp"

class ImGuiViewPortWindow {
public:
    virtual void render() = 0;
    void setViewPort(const Viewport& viewport) { m_viewport = viewport; }
    Viewport getViewport() const { return m_viewport; }
    ViewportType type() const { return m_type; }

protected:
    ViewportType m_type;
    Viewport m_viewport;
};

class MainViewPortWindow : public ImGuiViewPortWindow {
public:
    enum class ToolbarType : int {
        Translate,
        Rotate,
        Scale,
    };
    MainViewPortWindow() { m_type = ViewportType::Main; }
    void render() override;

protected:
    void renderGizmos();

private:
    ToolbarType m_toolbar_type{ ToolbarType::Translate };
};

class PickingViewPortWindow : public ImGuiViewPortWindow {
public:
    PickingViewPortWindow() { m_type = ViewportType::Pick; }
    void render() override;
};

class ShadowViewPortWindow : public ImGuiViewPortWindow {
public:
    ShadowViewPortWindow() { m_type = ViewportType::Shadow; }
    void render() override;
};

void MainViewPortWindow::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(1280, 720 + 20), ImGuiCond_Appearing);
    if (ImGui::Begin("MainViewPortWindow", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImVec2 content_pos = ImVec2(ImGui::GetWindowContentRegionMin().x + window_pos.x, ImGui::GetWindowContentRegionMin().y + window_pos.y);
        ImTextureID scene_tex_id = (ImTextureID)GetApp().renderSystem()->getSceneTexture();
        ImGui::Image(scene_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0)); // TODO uv0 ,uv1 ²ÎÊýº¬Òå
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(content_pos.x, content_pos.y, content_size.x, content_size.y);
        setViewPort({ (int)content_pos.x, (int)content_pos.y, (int)content_size.x, (int)content_size.y });
        renderGizmos();
    }
    ImGui::End();
}

void MainViewPortWindow::renderGizmos()
{
    ImGuizmo::OPERATION imguizmo_operation;
    switch (m_toolbar_type)
    {
    case ToolbarType::Translate:
        imguizmo_operation = ImGuizmo::TRANSLATE;
        break;
    case ToolbarType::Rotate:
        imguizmo_operation = ImGuizmo::ROTATE;
        break;
    case ToolbarType::Scale:
        imguizmo_operation = ImGuizmo::SCALE;
        break;
    default:
        break;
    }

    auto& camera = GetApp().scene()->getMainCamera();

    for (const auto& object : GetApp().scene()->getPickedObjects()) {
        TransformComponent* transform_component = object->getComponent<TransformComponent>();
        Mat4 model_matrix = transform_component->transform();
        ImGuizmo::Manipulate((float*)(&camera.view), (float*)(&camera.projection), imguizmo_operation, ImGuizmo::LOCAL, (float*)(&model_matrix), NULL, NULL, NULL, NULL);
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((float*)&model_matrix, matrixTranslation, matrixRotation, matrixScale);
        transform_component->translation = Vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
        transform_component->scale = Vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
        transform_component->rotation = Vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
    }

    ImVec2 air_window_size = ImVec2(128, 128);
    ImVec2 air_window_pos = ImVec2(m_viewport.x + m_viewport.width - air_window_size.x, m_viewport.y);
    float camDistance = 8.f;
    ImGuizmo::ViewManipulate((float*)(&camera.view), camDistance, air_window_pos, air_window_size, 0x10101010);

#if ENABLE_ECS
    ecs::TransformComponent* transform_component = nullptr;
    auto picked_entities = world.getPickedEntities();
    for (auto entity : picked_entities) {
        transform_component = world.getComponent<ecs::TransformComponent>(entity);
        break;
    }

    if (transform_component) {
        Mat4 model_matrix = transform_component->transform();
        ImGuizmo::Manipulate((float*)(&camera.view), (float*)(&camera.projection), imguizmo_operation, ImGuizmo::LOCAL, (float*)(&model_matrix), NULL, NULL, NULL, NULL);
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((float*)&model_matrix, matrixTranslation, matrixRotation, matrixScale);
        transform_component->translation = Vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
        transform_component->scale = Vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
        transform_component->rotation = Vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
    }

    if (camera) {
        Viewport viewport = GetApp().window()->getMainViewport().value_or(Viewport());
        viewport.transToScreenCoordinates();
        ImVec2 air_window_size = ImVec2(128, 128);
        float camDistance = 8.f;
        ImGuizmo::ViewManipulate((float*)(&camera->view), camDistance, ImVec2(viewport.x + viewport.width - air_window_size.x, viewport.y), air_window_size, 0x10101010);
    }
#endif
}

void PickingViewPortWindow::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("PickingViewPortWindow", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID picking_tex_id = (ImTextureID)GetApp().renderSystem()->getPickingTexture();
        ImGui::Image(picking_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({(int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y});
    }
    ImGui::End();
}

void ShadowViewPortWindow::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("ShadowViewPortWindow", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID shadow_tex_id = (ImTextureID)GetApp().renderSystem()->getShadowTexture();
        ImGui::Image(shadow_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y});
    }
    ImGui::End();
}


ImGuiViewPortWindowManager::ImGuiViewPortWindowManager()
{
    ImGuiViewPortWindow* main_view = new MainViewPortWindow();
    ImGuiViewPortWindow* picking_view = new PickingViewPortWindow();
    ImGuiViewPortWindow* shadow_view = new ShadowViewPortWindow();
    m_windows.assign({ main_view, picking_view, shadow_view });
}

ImGuiViewPortWindowManager::~ImGuiViewPortWindowManager()
{
    for (auto& view : m_windows) {
        delete view;
    }
    m_windows.clear();
}

void ImGuiViewPortWindowManager::render()
{
    for (auto& view : m_windows) {
        view->render();
    }
}

Viewport ImGuiViewPortWindowManager::getViewport(ViewportType type) const
{
    for (const auto& window : m_windows) {
        if (window->type() == type)
            return window->getViewport();
    }
    return {};
}

Viewport ImGuiViewPortWindowManager::getMainViewport() const
{
    return getViewport(ViewportType::Main);
}
