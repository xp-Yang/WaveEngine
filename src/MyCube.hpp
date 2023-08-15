#ifndef MyCube_hpp
#define MyCube_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Material.hpp"

class MyCube {
public:
    MyCube();
    MyCube(const Material& material);
    virtual ~MyCube();

    GLuint get_vao_id() const { return m_vao_id; }
    GLuint get_vbo_id() const { return m_vbo_id; }
    GLuint get_ibo_id() const { return m_ibo_id; }
    int get_elements_count() const { return m_indices_count; }
    const glm::mat4& get_model_matrix() const { return m_model_matrix; }
    void set_model_matrix(const glm::mat4& mat) { m_model_matrix = mat; }
    const Material& get_material() const { return m_material; }
    Material& material() { return m_material; }
    void set_material(const Material& material) { m_material = material; }
    void set_material_diffuse_map(std::string map_path);
    void set_material_specular_map(std::string map_path);

protected:
    GLuint m_vao_id;
    GLuint m_vbo_id;
    GLuint m_ibo_id;

    glm::mat4 m_model_matrix;
    int m_indices_count;

    Material m_material;

    virtual void create_vbo();
    virtual unsigned int generate_texture(int width, int height, unsigned char* data);
    virtual void create_vao();
};

#endif