#ifndef OpenGLFrameBuffer_hpp
#define OpenGLFrameBuffer_hpp

#include "Render/RHI/OpenGL/rhi_opengl.hpp"

class OpenGLFrameBuffer : public RhiFrameBuffer {
public:
	OpenGLFrameBuffer() = default;
	OpenGLFrameBuffer(const RhiAttachment& colorAttachment, const Vec2& pixelSize_, int sampleCount_ = 1);
	bool create() override; // true generate and bind frameBuffer
	void bind() override;
	void unBind() override;
	void clear(Color4 clear_color) override;
	void blitTo(RhiFrameBuffer* dest, RhiTexture::Format format = RhiTexture::Format::RGB16F) override;
};

#endif // !OpenGLFrameBuffer_hpp
