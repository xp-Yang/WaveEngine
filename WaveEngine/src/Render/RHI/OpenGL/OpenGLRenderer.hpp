#ifndef OpenGLRenderer_hpp
#define OpenGLRenderer_hpp

#include "Render/RHI/OpenGL/rhi_opengl.hpp"

class OpenGLRenderer {
public:
    static void drawIndexed(unsigned int vao_id, size_t indices_count);
    static void drawTriangle(unsigned int vao_id, size_t array_count);
private:
    OpenGLRenderer() = delete;
};

#endif