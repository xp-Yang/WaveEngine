#include "Object.hpp"
#include "stb_image.h"

Object::Object()
    : m_model_matrix(glm::mat4(1.0f))
    , m_is_renderable(true)
    , m_render_as_indices(true)
    , m_id(++id_count)
{
}

Object::~Object() {
    //glDeleteTextures(1, &m_texture_id);
    //glDeleteVertexArrays(1, &m_vao_id);
}

int Object::id_count = 0;
Mesh Object::default_mesh = Mesh();
Material Object::default_material = Material();