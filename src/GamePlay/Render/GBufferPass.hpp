#ifndef GBufferPass_hpp
#define GBufferPass_hpp

#include "RenderPass.hpp"

class GBufferPass : public RenderPass {
public:
    void init();
    void prepare_data(unsigned int fbo = -1, unsigned int map = -1);
    void draw();

private:
    unsigned int m_position_map;
    unsigned int m_normal_map;
    unsigned int m_color_spec_map;
};

#endif // !GBufferPass_hpp
