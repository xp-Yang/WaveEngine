#ifndef UIPass_hpp
#define UIPass_hpp

#include "RenderPass.hpp"

class UIPass : public RenderPass {
public:
    void init();
    // TODO “¿¿µ
    //void initialize_imgui(ImGuiEditor* editor);
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();
};

#endif
