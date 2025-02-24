#include "ImGuiGcodeLegend.hpp"
#include "GUI/Editor/ImGuiCanvas.hpp"
#include "GUI/Editor/ImGuiEditor.hpp"
#include "Render/RenderSystem.hpp"
#include "Logical/Gcode/GcodeViewer.hpp"
#include "Logical/Framework/World/Scene.hpp"
#include "GlobalContext.hpp"

static const std::array<std::string, ExtrusionRole::erCount> role_labels = {
    ("Undefined"),
    ("Inner wall"),
    ("Outer wall"),
    ("Overhang wall"),
    ("Sparse infill"),
    ("Internal solid infill"),
    ("Top surface"),
    ("Bottom surface"),
    ("Ironing"),
    ("Bridge"),
    ("Gap infill"),
    ("Skirt"),
    ("Brim"),
    ("Support"),
    ("Support interface"),
    ("Support transition"),
    ("Prime tower"),
    ("Custom"),
    ("Multiple"),
};

GcodeLegend::GcodeLegend(ImGuiCanvas* parent)
    : m_parent(parent)
{
}

void GcodeLegend::render()
{
    const auto& viewport = m_parent->getViewport();
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::SetNextWindowPos(ImVec2(viewport.x, viewport.y) + ImVec2(viewport.width - 124.0f, 0.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::BeginChild("Legend", ImVec2(300, 400), 0,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    {
        auto append_item = [this](
            const Color4& color,
            const std::string& text,
            bool visible,
            std::function<void()> callback = nullptr)
        {
            ImVec2 icon_pos = ImVec2(ImGui::GetCursorScreenPos().x + 12, ImGui::GetCursorScreenPos().y);
            float icon_size = ImGui::CalcTextSize("A").y - 2.0f;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0, 6.0));

            if (callback) {
                if (ImGui::Selectable(("##" + text).c_str(), visible, 0, ImVec2(300, ImGui::CalcTextSize("A").y)))
                    callback();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0, 0.0));
                ImGui::SameLine(ImGui::GetWindowWidth() - 40);
                ImGui::Checkbox(("##" + text).c_str(), &visible);
                ImGui::PopStyleVar(1);
            }

            {
                float dummy_size = ImGui::GetStyle().ItemSpacing.x + icon_size;
                ImGui::SameLine(dummy_size);
                ImGui::Text(text.c_str());
            }

            ImGui::PopStyleVar(1);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled({ icon_pos.x, icon_pos.y + 1.f }, { icon_pos.x + icon_size, icon_pos.y + icon_size + 1.f },
                ImGui::GetColorU32({ color[0], color[1], color[2], color[3] }));
        };

        std::shared_ptr<GcodeViewer> gcode_viewer = g_context.scene->gcodeViewer();
        for (size_t i = 1; i < ExtrusionRole::erCount; ++i) {
            append_item(Extrusion_Role_Colors[i], role_labels[i],
                gcode_viewer->is_visible(ExtrusionRole(i)),
                [this, i, gcode_viewer]() {
                    bool visible = gcode_viewer->is_visible(ExtrusionRole(i));
                    visible = !visible;
                    gcode_viewer->set_visible(ExtrusionRole(i), visible);
                });
        }
    }


    ImGui::EndChild();
}
