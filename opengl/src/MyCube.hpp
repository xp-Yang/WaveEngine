#ifndef MyCube_hpp
#define MyCube_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class MyCube {
public:
    MyCube(const char* texture_path);
    ~MyCube();

    GLuint get_vao_id() { return m_vao_id; }
    GLuint get_texture_id() { return m_texuture_id; }
    GLuint get_vbo_id() { return m_vbo_id; }
    GLuint get_ibo_id() { return m_ibo_id; }
    GLuint get_uv_id() { return m_uv_id; }

    int get_elements_count() { return 36; }

private:
    GLuint m_vao_id;
    GLuint m_texuture_id;
    GLuint m_vbo_id;
    GLuint m_ibo_id;
    GLuint m_uv_id;

    void create_vbo();
    void generate_texture(int width, int height, unsigned char* data);
    void create_vao();
};

#endif