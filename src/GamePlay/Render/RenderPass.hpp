#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>
#include "../../Platform/OpenGL/FrameBuffer.hpp"

// ½Ó¿ÚÀà
class RenderPass {
public:
	virtual void init() = 0;
	virtual void config() {} // TODO
	virtual void configSamples(int samples) = 0;
	virtual void prepare(FrameBuffer* framebuffer) = 0;
	virtual void draw() = 0;
	virtual FrameBuffer* getFrameBuffer() = 0;
};

#endif // !RenderPass_hpp

