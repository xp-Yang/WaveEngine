#include "Object.hpp"
#include "stb_image.h"

Object::Object()
    : m_model_matrix(glm::mat4(1.0f))
{

}

Object::~Object() {
    //glDeleteTextures(1, &m_texture_id);
    //glDeleteVertexArrays(1, &m_vao_id);
}

void Object::init_mesh() {}

void Object::init_material() {}