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

void Object::init_mesh() {
}

void Object::set_material_diffuse_map(std::string map_path)
{
    m_material.diffuse_map = Mesh::generate_texture_from_file(map_path, false);
    m_material.diffuse_map_path = map_path;
}

void Object::set_material_specular_map(std::string map_path)
{
    m_material.specular_map = Mesh::generate_texture_from_file(map_path, false);
    m_material.specular_map_path = map_path;
}