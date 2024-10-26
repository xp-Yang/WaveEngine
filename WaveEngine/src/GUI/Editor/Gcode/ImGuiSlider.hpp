#ifndef ImGuiSlider_hpp
#define ImGuiSlider_hpp

#include "Core/Common.hpp"
#include <imgui.h>
#include <imgui_internal.h>

enum class Orientation {
    Horizontal,
    Vertical
};

enum class HandleType {
    None,
    First,
    Second,
};

class ImGuiCanvas;
class ImGuiSlider {
public:
    ImGuiSlider(ImGuiCanvas* parent, Orientation style);
    ~ImGuiSlider() {}

    int    GetMinValue() const { return m_value_range[0]; }
    int    GetMaxValue() const { return m_value_range[1]; }
    int    GetLowerValue() const { return m_value_scope[0]; }
    int    GetHigherValue() const { return m_value_scope[1]; }
    //double GetMinLayerValue() { return m_layer_values.empty() ? 0. : m_layer_values[m_value_range[0]]; }
    //double GetMaxLayerValue() { return m_layer_values.empty() ? 0. : m_layer_values[m_value_range[1]]; }
    //double GetLowerLayerValue() { return m_layer_values.empty() ? 0. : m_layer_values[m_value_scope[0]]; }
    //double GetHigherLayerValue() { return m_layer_values.empty() ? 0. : m_layer_values[m_value_scope[1]]; }

    void initValueSpan(std::array<int, 2> span);
    void SetLowerValue(const int lower_val);
    void SetHigherValue(const int higher_val);
    //void SetSliderLayerValues(const std::vector<double>& values) { m_layer_values = values; }

    Orientation orientation() const { return m_orientation; }
    bool is_one_layer() const { return m_is_one_layer; }
    bool switch_one_layer_mode() { m_is_one_layer = !m_is_one_layer; }

    void on_mouse_wheel(const ImVec2& slider_pos, const ImVec2& slider_size);

    bool render();

protected:
    bool horizontal_slider(const char* str_id, int* value, int v_min, int v_max, const ImVec2& size);
    void draw_background_and_groove(const ImRect& bg_rect, const ImRect& groove);
    bool vertical_slider(const char* str_id, int* higher_value, int* lower_value,
        std::string& higher_label, std::string& lower_label,
        int v_min, int v_max, const ImVec2& size,
        HandleType& selection, bool one_layer_flag = false);

private:
    //std::string get_label(int tick, LabelType label_type = ltHeightWithLayer);
    float get_pos_from_value(int v_min, int v_max, int value, const ImRect& rect);

private:
    std::array<int, 2> m_value_range;
    std::array<int, 2> m_value_scope;
    std::vector<double> m_layer_values;

    Orientation m_orientation;

    HandleType m_curr_handle;

    float m_zero_layer_height = 0.0f;

    bool m_is_one_layer = false;

    ImGuiCanvas* m_parent;
};

#endif // !#define ImGuiSlider_hpp
