#include "ImGuiGcodeLegend.hpp"
#include "../ImGuiCanvas.hpp"
#include "../ImGuiEditor.hpp"
#include "Render/RenderSystem.hpp"
#include "Logical/Gcode/GcodeViewer.hpp"

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

    std::shared_ptr<GcodeViewer> gcode_viewer = m_parent->parent()->ref_render_system->gcodeViewer();

    ImGui::SetNextWindowPos(ImVec2(viewport.width, 0.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.00f, 0.68f, 0.26f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.00f, 0.68f, 0.26f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.42f, 0.42f, 0.42f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0.93f, 0.93f, 0.93f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0.93f, 0.93f, 0.93f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Border, { 1, 0, 0, 0 });
    //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 1.f, 1.f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::BeginChild("Legend", ImVec2(400, 0.4f * viewport.height), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    {
        auto append_item = [this](
            const Color4& color,
            const std::string& text,
            bool* visible,
            std::function<void()> callback = nullptr)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            // render icon
            ImVec2 pos = ImVec2(ImGui::GetCursorScreenPos().x + 12, ImGui::GetCursorScreenPos().y);

            float icon_size = 12.0f;
            draw_list->AddRectFilled({ pos.x + 1.0f, pos.y + 3.0f }, { pos.x + icon_size - 1.0f, pos.y + icon_size + 1.0f },
                ImGui::GetColorU32({ color[0], color[1], color[2], color[3] }));

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0, 6.0));

            ImGui::Dummy({ 0.0, 0.0 });
            ImGui::SameLine();
            if (callback) {
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0, 0.0));
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.00f, 0.68f, 0.26f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.00f, 0.68f, 0.26f, 0.0f));
                if (ImGui::InvisibleButton(("##" + text).c_str(), ImVec2(400, ImGui::CalcTextSize("A").y)))
                    callback();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(2);

                //ImGui::SameLine(checkbox_offset);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0, 0.0));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.00f, 0.68f, 0.26f, 1.00f));
                ImGui::Checkbox(("##" + text).c_str(), visible);
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(1);
            }

            {
                float dummy_size = ImGui::GetStyle().ItemSpacing.x + icon_size;
                ImGui::SameLine(dummy_size);
                ImGui::Text(text.c_str());
            }

            ImGui::PopStyleVar(1);

        };

        for (size_t i = 1; i < ExtrusionRole::erCount; ++i) {
            bool visible = true;
            append_item(Extrusion_Role_Colors[i], role_labels[i],
                &visible, [this, i, visible, gcode_viewer]() {
                    gcode_viewer->set_visible(ExtrusionRole(i), visible);
                    //update_moves_slider();
                });
        }
    }


    ImGui::EndChild();
    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar(2);
}
