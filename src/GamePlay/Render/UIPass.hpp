#ifndef UIPass_hpp
#define UIPass_hpp

#include "RenderPass.hpp"

class UIPass : public RenderPass {
public:
    void init();
    // TODO ��������
    //void initialize_imgui(ImGuiEditor* editor);
    void draw();
};

#endif
