#include "ImGuiCanvas.hpp"
#include "ImGuiToolbar.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include "EngineAPI.hpp"

#if ENABLE_ECS
#include "Logical/FrameWork/ECS/World.hpp"
#include "Logical/FrameWork/ECS/Components.hpp"
#endif

void MainCanvas::render()
{
    if (!m_toolbar)
        m_toolbar = new ImGuiToolbar(this);

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
        ImTextureID scene_tex_id = (ImTextureID)GetApp().renderSystem()->renderPassTexture(RenderPass::Type::Combined);
        auto cursor_pos = ImGui::GetCursorPos();
        ImGui::Image(scene_tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0)); // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#about-texture-coordinates
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetCursorPos(cursor_pos);
        ImGui::Text("FPS %.1f", io.Framerate);
        setViewPort({ (int)content_pos.x, (int)content_pos.y, (int)content_size.x, (int)content_size.y });

        m_toolbar->render();
    }
    ImGui::End();
}

void PickingCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("PickingCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID picking_tex_id = (ImTextureID)GetApp().renderSystem()->renderPassTexture(RenderPass::Type::Picking);
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
        ImTextureID shadow_tex_id = (ImTextureID)GetApp().renderSystem()->renderPassTexture(RenderPass::Type::Shadow);
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
        unsigned int begin_tex_id = GetApp().renderSystem()->renderPassTexture(RenderPass::Type::GBuffer);
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
        ImTextureID tex_id = (ImTextureID)GetApp().renderSystem()->renderPassTexture(RenderPass::Type::DeferredLighting);
        ImGui::Image(tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y });
    }
    ImGui::End();
}

void BrightCanvas::render()
{
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Appearing);
    if (ImGui::Begin("BrightCanvas", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 content_size = ImGui::GetContentRegionAvail();
        ImTextureID tex_id = (ImTextureID)(GetApp().renderSystem()->renderPassTexture(RenderPass::Type::Bloom));
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
        ImTextureID tex_id = (ImTextureID)GetApp().renderSystem()->renderPassTexture(RenderPass::Type::Bloom);
        ImGui::Image(tex_id, content_size, ImVec2(0, 1), ImVec2(1, 0));
        setViewPort({ (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y });
    }
    ImGui::End();
}
