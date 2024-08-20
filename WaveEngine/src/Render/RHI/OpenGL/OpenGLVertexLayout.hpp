#ifndef OpenGLVertexLayout_hpp
#define OpenGLVertexLayout_hpp

#include "Render/RHI/OpenGL/rhi_opengl.hpp"

class OpenGLVertexLayout : public RhiVertexLayout {
public:
	OpenGLVertexLayout(RhiBuffer* vbuffer, RhiBuffer* ibuffer);
	bool create() override;
};


#endif // !OpenGLVertexLayout
