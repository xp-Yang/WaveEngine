#ifndef ScreenPass_hpp
#define ScreenPass_hpp

#include "RenderPass.hpp"

class ScreenPass : public RenderPass {
public:
    void init() override;
    void draw() override;

private:
    unsigned int m_lighted_map;
    unsigned int m_blurred_bright_map;
    unsigned int m_border_map;
};

#endif
