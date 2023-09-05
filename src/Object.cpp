#include "Object.hpp"

Object::Object()
    : m_enable_relection(false)
    , m_explosion_ratio(0.0f)
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