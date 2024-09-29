#ifndef ImGuiToolbar_hpp
#define ImGuiToolbar_hpp

#include "Core/Common.hpp"
#include "ResourceManager/Texture.hpp"

enum class ToolbarType : int {
    Translate,
    Rotate,
    Scale,
};

class ImGuiCanvas;
class ImGuiToolbar {
public:
    ImGuiToolbar(ImGuiCanvas* parent);
    void render();

protected:
    void renderToolbar();
    void renderGizmos();
    ToolbarType m_toolbar_type{ ToolbarType::Translate };
    ImGuiCanvas* m_parent{ nullptr };

    unsigned int m_tranlate_icon_id{ 0 };
    unsigned int m_rotate_icon_id{ 0 };
    unsigned int m_scale_icon_id{ 0 };
};

#endif