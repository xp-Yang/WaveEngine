#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>
#include "../../Platform/RHI/rhi.hpp"

// ½Ó¿ÚÀà
class RenderPass {
public:
	virtual void init() = 0;
	virtual void prepare(FrameBuffer* framebuffer) = 0;
	virtual void draw() = 0;
	virtual FrameBuffer* getFrameBuffer() = 0;
};

class ScreenQuad {
public:
    unsigned int create();
    unsigned int getVAO();
private:
    unsigned int m_vao = 0;
};

#endif // !RenderPass_hpp

