#include "ImGuiCanvas.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include "Engine.hpp"

#if ENABLE_ECS
#include "Logical/FrameWork/ECS/World.hpp"
#include "Logical/FrameWork/ECS/Components.hpp"
#endif

void MainCanvas::render()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("MainCanvas", nullptr, window_flags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImVec2 content_pos = ImVec2(ImGui::GetWindowContentRegionMin().x + window_pos.x, ImGui::GetWindowContentRegionMin().y + window_pos.y);
        ImTextureID scene_tex_id = (ImTextureID)GetApp().renderSystem()->getSceneTexture();
        auto cursor_pos = ImGui::GetCursorPos();
        ImGui::Image(scene_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0)); // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#about-texture-coordinates
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetCursorPos(cursor_pos);
        ImGui::Text("FPS %.1f", io.Framerate);
        setViewPort({ (int)content_pos.x, (int)content_pos.y, (int)content_size.x, (int)content_size.y });

        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(content_pos.x, content_pos.y, content_size.x, content_size.y);
        renderGizmos();
    }
    ImGui::End();
}

void MainCanvas::renderGizmos()
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
        Light* light = GetApp().scene()->getPickedLight().get();
        if (auto point_light = dynamic_cast<PointLight*>(light)) {
            Mat4 model_matrix = Math::Translate(point_light->position);
            ImGuizmo::Manipulate((float*)(&camera.view), (float*)(&camera.projection), imguizmo_operation, ImGuizmo::LOCAL, (float*)(&model_matrix), NULL, NULL, NULL, NULL);
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents((float*)&model_matrix, matrixTranslation, matrixRotation, matrixScale);
            point_light->position = Vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
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

void PickingCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("PickingCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID picking_tex_id = (ImTextureID)GetApp().renderSystem()->getPickingTexture();
        ImGui::Image(picking_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({(int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y});
    }
    ImGui::End();
}

void ShadowCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("ShadowCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID shadow_tex_id = (ImTextureID)GetApp().renderSystem()->getShadowTexture();
        ImGui::Image(shadow_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y});
    }
    ImGui::End();
}

void GBufferCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("GBufferCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_pos = ImGui::GetWindowContentRegionMin();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        unsigned int begin_tex_id = GetApp().renderSystem()->getGBufferTexture();
        for (int i = 0; i < 4; ++i) {
            int row = i / 2;
            int col = i % 2;
            ImVec2 child_content_pos = ImVec2(content_pos.x + content_size.x / 2.0f * col, content_pos.y + content_size.y / 2.0f * row);
            ImVec2 child_content_size = content_size / 2.0f;
            ImGui::SetCursorPos(child_content_pos);
            ImTextureID tex_id = (ImTextureID)(begin_tex_id++);
            ImGui::Image(tex_id, child_content_size, ImVec2(0, 1), ImVec2(1, 0));
        }
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y });
    }
    ImGui::End();
}

void LightingCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("LightingCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID tex_id = (ImTextureID)GetApp().renderSystem()->getLightingTexture();
        ImGui::Image(tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y });
    }
    ImGui::End();
}

void BlurredCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("BlurredCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID tex_id = (ImTextureID)GetApp().renderSystem()->getBlurredTexture();
        ImGui::Image(tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y });
    }
    ImGui::End();
}
