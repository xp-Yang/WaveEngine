#include "ImGuiSlider.hpp"
#include "../ImGuiEditor.hpp"
#include "../ImGuiCanvas.hpp"
#include "Render/RenderSystem.hpp"

static const float  LEFT_MARGIN = 0.0f;
static const float  HORIZONTAL_SLIDER_WINDOW_HEIGHT = 64.0f;
static const float  VERTICAL_SLIDER_WINDOW_WIDTH = 124.0f;
static const float  GROOVE_WIDTH = 12.0f;
static const ImVec2 ONE_LAYER_MARGIN = ImVec2(20.0f, 20.0f);
static const ImVec2 ONE_LAYER_BUTTON_SIZE = ImVec2(28.0f, 28.0f);

static const ImU32 BACKGROUND_COLOR_LIGHT = IM_COL32(255, 255, 255, 255);
static const ImU32 GROOVE_COLOR_LIGHT = IM_COL32(206, 206, 206, 255);
static const ImU32 BRAND_COLOR = IM_COL32(0, 174, 66, 255);

bool slider_behavior(ImGuiID id, const ImRect& region, const ImS32 v_min, const ImS32 v_max, ImS32* out_value, ImRect* out_handle, ImGuiSliderFlags flags = 0, const int fixed_value = -1, const ImVec4& fixed_rect = ImVec4())
{
    ImGuiContext& context = *GImGui;
    ImGuiIO& io = ImGui::GetIO();

    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;

    const ImVec2 handle_sz = out_handle->GetSize();
    ImS32 v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
    const float region_usable_sz = (region.Max[axis] - region.Min[axis]);
    const float region_usable_pos_min = region.Min[axis];
    const float region_usable_pos_max = region.Max[axis];

    // Process interacting with the slider
    ImS32 v_new = *out_value;
    bool value_changed = false;

    // drag behavior
    if (context.ActiveId == id)
    {
        float mouse_pos_ratio = 0.0f;
        if (context.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (context.IO.MouseReleased[0])
            {
                ImGui::ClearActiveID();
            }
            if (context.IO.MouseDown[0])
            {
                const float mouse_abs_pos = context.IO.MousePos[axis];
                mouse_pos_ratio = (region_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - region_usable_pos_min) / region_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (axis == ImGuiAxis_Y)
                    mouse_pos_ratio = 1.0f - mouse_pos_ratio;
                v_new = v_min + (ImS32)(v_range * mouse_pos_ratio + 0.5f);
            }
        }
    }
    // click in fixed_rect behavior
    if (ImGui::ItemHoverable(fixed_rect, id, 0) && context.IO.MouseReleased[0])
    {
        v_new = fixed_value;
    }

    // apply result, output value
    if (*out_value != v_new)
    {
        *out_value = v_new;
        value_changed = true;
    }

    // Output handle position so it can be displayed by the caller
    const ImS32 v_clamped = (v_min < v_max) ? ImClamp(*out_value, v_min, v_max) : ImClamp(*out_value, v_max, v_min);
    float handle_pos_ratio = v_range != 0 ? ((float)(v_clamped - v_min) / (float)v_range) : 0.0f;
    handle_pos_ratio = axis == ImGuiAxis_Y ? 1.0f - handle_pos_ratio : handle_pos_ratio;
    const float handle_pos = region_usable_pos_min + (region_usable_pos_max - region_usable_pos_min) * handle_pos_ratio;

    ImVec2 new_handle_center = axis == ImGuiAxis_Y ? ImVec2(out_handle->GetCenter().x, handle_pos) : ImVec2(handle_pos, out_handle->GetCenter().y);
    *out_handle = ImRect(new_handle_center - handle_sz * 0.5f, new_handle_center + handle_sz * 0.5f);

    return value_changed;
}


ImGuiSlider::ImGuiSlider(ImGuiCanvas* parent, Orientation style)
    : m_parent(parent)
    , m_orientation(style)
{
    m_value_range[0] = 0;
    m_value_range[1] = 100;
    m_value_scope[0] = 0;
    m_value_scope[1] = 100;
    m_curr_handle = HandleType::Second;
}

void ImGuiSlider::initValueSpan(std::array<int, 2> span)
{
    m_value_range = span;
    m_value_scope = m_value_range;
}

void ImGuiSlider::SetLowerValue(const int lower_val)
{
    m_value_scope[0] = lower_val;

    if (m_parent->parent()->ref_render_system->gcodeViewer()->valid()) {
        if (m_orientation == Orientation::Vertical) {
            m_parent->parent()->ref_render_system->gcodeViewer()->set_layer_scope(m_value_scope);
            static_cast<PreviewCanvas*>(m_parent)->horizontal_slider()->initValueSpan(m_parent->parent()->ref_render_system->gcodeViewer()->get_move_range());
        }
        else
            m_parent->parent()->ref_render_system->gcodeViewer()->set_move_scope(m_value_scope);
    }
}

void ImGuiSlider::SetHigherValue(const int higher_val)
{
    m_value_scope[1] = higher_val;

    if (m_parent->parent()->ref_render_system->gcodeViewer()->valid()) {
        if (m_orientation == Orientation::Vertical) {
            m_parent->parent()->ref_render_system->gcodeViewer()->set_layer_scope(m_value_scope);
            static_cast<PreviewCanvas*>(m_parent)->horizontal_slider()->initValueSpan(m_parent->parent()->ref_render_system->gcodeViewer()->get_move_range());
        }
        else
            m_parent->parent()->ref_render_system->gcodeViewer()->set_move_scope(m_value_scope);
    }
}

void ImGuiSlider::on_mouse_wheel()
{
    auto moves_slider_window = ImGui::FindWindowByName("moves_slider");
    auto layers_slider_window = ImGui::FindWindowByName("laysers_slider");
    if (!moves_slider_window || !layers_slider_window) {
        assert(false); // "Couldn't find slider window";
        return;
    }

    float wheel = 0.0f;
    //wheel = evt.GetWheelRotation() > 0 ? 1.0f : -1.0f;
    if (wheel == 0.0f)
        return;

    ImVec2 mouse_pos = ImGui::GetMousePos();
    if (m_orientation == Orientation::Horizontal) {
        if (mouse_pos.x > moves_slider_window->Pos.x &&
            mouse_pos.x < moves_slider_window->Pos.x + moves_slider_window->Size.x &&
            mouse_pos.y > moves_slider_window->Pos.y &&
            mouse_pos.y < moves_slider_window->Pos.y + moves_slider_window->Size.y) {
            const int new_pos = GetHigherValue() + wheel;
            SetHigherValue(new_pos);
        }
    }
    else {
        if (mouse_pos.x > layers_slider_window->Pos.x &&
            mouse_pos.x < layers_slider_window->Pos.x + layers_slider_window->Size.x &&
            mouse_pos.y > layers_slider_window->Pos.y &&
            mouse_pos.y < layers_slider_window->Pos.y + layers_slider_window->Size.y) {
            if (is_one_layer()) {
                const int new_pos = GetHigherValue() + wheel;
                SetHigherValue(new_pos);
            }
            else {
                const int new_pos = m_curr_handle == HandleType::First ? GetLowerValue() + wheel : GetHigherValue() + wheel;
                m_curr_handle == HandleType::First ? SetLowerValue(new_pos) : SetHigherValue(new_pos);
            }
        }
    }
}

bool ImGuiSlider::render()
{
    bool result = false;
    /* style and colors */
    ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, ImVec4(0, 0.682f, 0.259f, 1.0f));

    int windows_flag = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse;

    int canvas_width = m_parent->getViewport().width;
    int canvas_height = m_parent->getViewport().height;
    ImVec2 canvas_pos = ImVec2(m_parent->getViewport().x, m_parent->getViewport().y);

    if (m_orientation == Orientation::Horizontal) {
        ImVec2 size = ImVec2(0.6f * canvas_width, HORIZONTAL_SLIDER_WINDOW_HEIGHT);
        ImGui::SetNextWindowPos(canvas_pos + ImVec2(0.2f * canvas_width, canvas_height - HORIZONTAL_SLIDER_WINDOW_HEIGHT));
        ImGui::BeginChild("moves_slider", size, 0, windows_flag);
        int value = GetHigherValue();
        if (horizontal_slider("moves_slider", &value, GetMinValue(), GetMaxValue(), size)) {
            result = true;
            SetHigherValue(value);
        }
        ImGui::EndChild();
    }
    else {
        ImVec2 size = ImVec2(VERTICAL_SLIDER_WINDOW_WIDTH, 0.7f * canvas_height);
        ImGui::SetNextWindowPos(canvas_pos + ImVec2(canvas_width - VERTICAL_SLIDER_WINDOW_WIDTH, 0.15f * canvas_height));
        ImGui::BeginChild("laysers_slider", size, 0, windows_flag);

        int higher_value = GetHigherValue();
        int lower_value = GetLowerValue();
        std::string higher_label = std::to_string(m_value_scope[1]);
        std::string lower_label = std::to_string(m_value_scope[0]);
        int temp_higher_value = higher_value;
        int temp_lower_value = lower_value;
        if (vertical_slider("laysers_slider", &higher_value, &lower_value, higher_label, lower_label, GetMinValue(), GetMaxValue(),
            size, m_curr_handle, is_one_layer())) {
            if (temp_higher_value != higher_value)
                SetHigherValue(higher_value);
            if (temp_lower_value != lower_value)
                SetLowerValue(lower_value);
            result = true;
        }
        ImGui::EndChild();
    }

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);

    return result;
}

bool ImGuiSlider::horizontal_slider(const char* str_id, int* value, int v_min, int v_max, const ImVec2& size)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& context = *GImGui;
    const ImGuiID id = window->GetID(str_id);

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect draw_region(pos, pos + size);
    ImGui::ItemSize(draw_region);

    const float  handle_dummy_width = 10.0f;
    const float  text_right_dummy = 50.0f;

    const float  handle_radius = 12.0f;
    const float  handle_border = 2.0f;

    const float  text_frame_rounding = 2.0f;
    const float  text_start_offset = 8.0f;
    const ImVec2 text_padding = ImVec2(5.0f, 2.0f);
    const float  triangle_offsets[3] = { -3.5f, 3.5f, -6.06f };

    const ImU32 white_bg = BACKGROUND_COLOR_LIGHT;
    const ImU32 handle_clr = BRAND_COLOR;
    const ImU32 handle_border_clr = BACKGROUND_COLOR_LIGHT;

    // calculate groove size
    const ImVec2 groove_start = ImVec2(pos.x + handle_dummy_width, pos.y + size.y - ONE_LAYER_MARGIN.y - ONE_LAYER_BUTTON_SIZE.y * 0.5f - GROOVE_WIDTH * 0.5f);
    const ImVec2 groove_size = ImVec2(size.x - 2 * handle_dummy_width - text_right_dummy, GROOVE_WIDTH);
    const ImRect groove = ImRect(groove_start, groove_start + groove_size);
    const ImRect bg_rect = ImRect(groove.Min - ImVec2(6.0f, 6.0f), groove.Max + ImVec2(6.0f, 6.0f));
    const float mid_y = groove.GetCenter().y;

    // set mouse active region. active region.
    bool  hovered = ImGui::ItemHoverable(draw_region, id, 0);
    if (hovered && context.IO.MouseDown[0]) {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
    }

    // draw background
    draw_background_and_groove(bg_rect, groove);

    // set scrollable region
    const ImRect slideable_region = ImRect(bg_rect.Min + ImVec2(handle_radius, 0.0f), bg_rect.Max - ImVec2(handle_radius, 0.0f));

    // initialize the handle
    float  handle_pos = get_pos_from_value(v_min, v_max, *value, groove);
    ImRect handle = ImRect(handle_pos - handle_radius, mid_y - handle_radius, handle_pos + handle_radius, mid_y + handle_radius);

    // update handle position and value
    bool   value_changed = slider_behavior(id, slideable_region, (const ImS32)v_min, (const ImS32)v_max, (ImS32*)value, &handle);
    ImVec2 handle_center = handle.GetCenter();

    // draw scroll line
    ImRect scroll_line = ImRect(groove.Min, ImVec2(handle_center.x, groove.Max.y));
    window->DrawList->AddRectFilled(scroll_line.Min, scroll_line.Max, handle_clr);

    // draw handle
    window->DrawList->AddCircleFilled(handle_center, handle_radius, handle_border_clr);
    window->DrawList->AddCircleFilled(handle_center, handle_radius - handle_border, handle_clr);

    // draw label
    auto text_utf8 = std::to_string(*value);
    ImVec2 text_content_size = ImGui::CalcTextSize(text_utf8.c_str());
    ImVec2 text_size = text_content_size + text_padding * 2;
    ImVec2 text_start = ImVec2(handle_center.x + handle_radius + text_start_offset, handle_center.y - 0.5 * text_size.y);
    ImRect text_rect(text_start, text_start + text_size);
    ImGui::RenderFrame(text_rect.Min, text_rect.Max, white_bg, false, text_frame_rounding);
    ImVec2 pos_1 = ImVec2(text_rect.Min.x, text_rect.GetCenter().y + triangle_offsets[0]);
    ImVec2 pos_2 = ImVec2(text_rect.Min.x, text_rect.GetCenter().y + triangle_offsets[1]);
    ImVec2 pos_3 = ImVec2(text_rect.Min.x + triangle_offsets[2], text_rect.GetCenter().y);
    window->DrawList->AddTriangleFilled(pos_1, pos_2, pos_3, white_bg);
    ImGui::RenderText(text_start + text_padding, std::to_string(*value).c_str());

    return value_changed;
}

void ImGuiSlider::draw_background_and_groove(const ImRect& bg_rect, const ImRect& groove)
{
    const ImU32 bg_rect_col = BACKGROUND_COLOR_LIGHT;
    const ImU32 groove_col = GROOVE_COLOR_LIGHT;

    // draw bg of slider
    ImGui::RenderFrame(bg_rect.Min, bg_rect.Max, bg_rect_col, false, 0.5 * bg_rect.GetWidth());
    // draw groove
    ImGui::RenderFrame(groove.Min, groove.Max, groove_col, false, 0.5 * groove.GetWidth());
}

bool ImGuiSlider::vertical_slider(const char* str_id, int* higher_value, int* lower_value, std::string& higher_label, std::string& lower_label, int v_min, int v_max, const ImVec2& size, HandleType& selection, bool one_layer_flag)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& context = *GImGui;
    const ImGuiID id = window->GetID(str_id);

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect draw_region(pos, pos + size);
    ImGui::ItemSize(draw_region);

    const float  text_dummy_height = 30.0f;

    const float  handle_radius = 12.0f;
    const float  handle_border = 2.0f;
    const float  line_width = 1.0f;
    const float  line_length = 12.0f;
    const float  one_handle_offset = 26.0f;
    const float  bar_width = 28.0f;

    const float  text_frame_rounding = 2.0f;
    const ImVec2 text_padding = ImVec2(5.0f, 2.0f);
    const ImVec2 triangle_offsets[3] = { ImVec2(2.0f, 0.0f), ImVec2(0.0f, 8.0f), ImVec2(9.0f, 0.0f) };
    ImVec2 text_content_size;
    ImVec2 text_size;

    const ImU32 white_bg = BACKGROUND_COLOR_LIGHT;
    const ImU32 handle_clr = BRAND_COLOR;
    const ImU32 handle_border_clr = BACKGROUND_COLOR_LIGHT;
    // calculate slider groove size
    const ImVec2 groove_start = ImVec2(pos.x + size.x - ONE_LAYER_MARGIN.x - ONE_LAYER_BUTTON_SIZE.x * 0.5f - GROOVE_WIDTH * 0.5f, pos.y + text_dummy_height);
    const ImVec2 groove_size = ImVec2(GROOVE_WIDTH, size.y - 2 * text_dummy_height);
    const ImRect groove = ImRect(groove_start, groove_start + groove_size);
    const ImRect bg_rect = ImRect(groove.Min - ImVec2(6.0f, 6.0f), groove.Max + ImVec2(6.0f, 6.0f));
    const float mid_x = groove.GetCenter().x;

    // set mouse active region.
    const ImRect active_region = ImRect(ImVec2(draw_region.Min.x + 35.0f, draw_region.Min.y), draw_region.Max);
    bool hovered = ImGui::ItemHoverable(active_region, id, 0);
    if (hovered && context.IO.MouseDown[0]) {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
    }

    // draw background
    draw_background_and_groove(bg_rect, groove);

    // Processing interacting
    // set scrollable region
    const ImRect region = ImRect(bg_rect.Min + ImVec2(0.0f, handle_radius), bg_rect.Max - ImVec2(0.0f, handle_radius));
    const ImRect higher_slideable_region = ImRect(region.Min, region.Max - ImVec2(0, handle_radius));
    const ImRect lower_slideable_region = ImRect(region.Min + ImVec2(0, handle_radius), region.Max);
    const ImRect one_slideable_region = region;

    // initialize the handles.
    float higher_handle_pos = get_pos_from_value(v_min, v_max, *higher_value, higher_slideable_region);
    ImRect higher_handle = ImRect(mid_x - handle_radius, higher_handle_pos - handle_radius, mid_x + handle_radius, higher_handle_pos + handle_radius);

    float  lower_handle_pos = get_pos_from_value(v_min, v_max, *lower_value, lower_slideable_region);
    ImRect lower_handle = ImRect(mid_x - handle_radius, lower_handle_pos - handle_radius, mid_x + handle_radius, lower_handle_pos + handle_radius);

    ImRect one_handle = ImRect(higher_handle.Min - ImVec2(one_handle_offset, 0), higher_handle.Max - ImVec2(one_handle_offset, 0));

    bool value_changed = false;
    if (!one_layer_flag)
    {
        // select higher handle by default
        static bool h_selected = (m_curr_handle == HandleType::Second);
        if (ImGui::ItemHoverable(higher_handle, id, 0) && context.IO.MouseClicked[0]) {
            selection = HandleType::Second;
            h_selected = true;
        }
        if (ImGui::ItemHoverable(lower_handle, id, 0) && context.IO.MouseClicked[0]) {
            selection = HandleType::First;
            h_selected = false;
        }

        // update handle position and value
        if (h_selected)
        {
            value_changed = slider_behavior(id, higher_slideable_region, v_min, v_max,
                higher_value, &higher_handle, ImGuiSliderFlags_Vertical);
        }
        if (!h_selected) {
            value_changed = slider_behavior(id, lower_slideable_region, v_min, v_max,
                lower_value, &lower_handle, ImGuiSliderFlags_Vertical);
        }

        ImVec2 higher_handle_center = higher_handle.GetCenter();
        ImVec2 lower_handle_center = lower_handle.GetCenter();
        if (higher_handle_center.y + handle_radius > lower_handle_center.y && h_selected)
        {
            lower_handle = higher_handle;
            lower_handle.TranslateY(handle_radius);
            lower_handle_center.y = higher_handle_center.y + handle_radius;
            *lower_value = *higher_value;
        }
        if (higher_handle_center.y + handle_radius > lower_handle_center.y && !h_selected)
        {
            higher_handle = lower_handle;
            higher_handle.TranslateY(-handle_radius);
            higher_handle_center.y = lower_handle_center.y - handle_radius;
            *higher_value = *lower_value;
        }

        // draw scroll line
        ImRect scroll_line = ImRect(ImVec2(groove.Min.x, higher_handle_center.y), ImVec2(groove.Max.x, lower_handle_center.y));
        window->DrawList->AddRectFilled(scroll_line.Min, scroll_line.Max, handle_clr);

        // draw handles
        window->DrawList->AddCircleFilled(higher_handle_center, handle_radius, handle_border_clr);
        window->DrawList->AddCircleFilled(higher_handle_center, handle_radius - handle_border, handle_clr);
        window->DrawList->AddCircleFilled(lower_handle_center, handle_radius, handle_border_clr);
        window->DrawList->AddCircleFilled(lower_handle_center, handle_radius - handle_border, handle_clr);
        if (h_selected) {
            window->DrawList->AddCircleFilled(higher_handle_center, handle_radius, handle_border_clr);
            window->DrawList->AddCircleFilled(higher_handle_center, handle_radius - handle_border, handle_clr);
            window->DrawList->AddLine(higher_handle_center + ImVec2(-0.5f * line_length, 0.0f), higher_handle_center + ImVec2(0.5f * line_length, 0.0f), white_bg, line_width);
            window->DrawList->AddLine(higher_handle_center + ImVec2(0.0f, -0.5f * line_length), higher_handle_center + ImVec2(0.0f, 0.5f * line_length), white_bg, line_width);
        }
        if (!h_selected) {
            window->DrawList->AddLine(lower_handle_center + ImVec2(-0.5f * line_length, 0.0f), lower_handle_center + ImVec2(0.5f * line_length, 0.0f), white_bg, line_width);
            window->DrawList->AddLine(lower_handle_center + ImVec2(0.0f, -0.5f * line_length), lower_handle_center + ImVec2(0.0f, 0.5f * line_length), white_bg, line_width);
        }

        // draw higher label
        auto text_utf8 = higher_label;
        text_content_size = ImGui::CalcTextSize(text_utf8.c_str());
        text_size = text_content_size + text_padding * 2;
        ImVec2 text_start = ImVec2(higher_handle.Min.x - text_size.x - triangle_offsets[2].x, higher_handle_center.y - text_size.y);
        ImRect text_rect(text_start, text_start + text_size);
        ImGui::RenderFrame(text_rect.Min, text_rect.Max, white_bg, false, text_frame_rounding);
        ImVec2 pos_1 = text_rect.Max - triangle_offsets[0];
        ImVec2 pos_2 = pos_1 - triangle_offsets[1];
        ImVec2 pos_3 = pos_1 + triangle_offsets[2];
        window->DrawList->AddTriangleFilled(pos_1, pos_2, pos_3, white_bg);
        ImGui::RenderText(text_start + text_padding, higher_label.c_str());
        // draw lower label
        text_utf8 = lower_label;
        text_content_size = ImGui::CalcTextSize(text_utf8.c_str());
        text_size = text_content_size + text_padding * 2;
        text_start = ImVec2(lower_handle.Min.x - text_size.x - triangle_offsets[2].x, lower_handle_center.y);
        text_rect = ImRect(text_start, text_start + text_size);
        ImGui::RenderFrame(text_rect.Min, text_rect.Max, white_bg, false, text_frame_rounding);
        pos_1 = ImVec2(text_rect.Max.x, text_rect.Min.y) - triangle_offsets[0];
        pos_2 = pos_1 + triangle_offsets[1];
        pos_3 = pos_1 + triangle_offsets[2];
        window->DrawList->AddTriangleFilled(pos_1, pos_2, pos_3, white_bg);
        ImGui::RenderText(text_start + text_padding, lower_label.c_str());
    }

    return value_changed;
}

float ImGuiSlider::get_pos_from_value(int v_min, int v_max, int value, const ImRect& rect)
{
    float pos_ratio = (v_max - v_min) != 0 ? ((float)(value - v_min) / (float)(v_max - v_min)) : 0.0f;
    float handle_pos;
    if (m_orientation == Orientation::Horizontal) {
        handle_pos = rect.Min.x + (rect.Max.x - rect.Min.x) * pos_ratio;
    }
    else {
        pos_ratio = 1.0f - pos_ratio;
        handle_pos = rect.Min.y + (rect.Max.y - rect.Min.y) * pos_ratio;
    }
    return handle_pos;
}
