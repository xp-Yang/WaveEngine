#ifndef Object_hpp
#define Object_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "MyMesh.hpp"
#include "Material.hpp"


class Object {
public:
    Object();
    ~Object();

    const Mesh& get_mesh() const { return m_mesh; }
    const Material& get_material() const { return m_material; }
    Material& material() { return m_material; }
    void set_material(const Material& material) { m_material = material; }
    void set_material_diffuse_map(std::string map_path);
    void set_material_specular_map(std::string map_path);
    const glm::mat4& get_model_matrix() const { return m_model_matrix; }
    void set_model_matrix(const glm::mat4& mat) { m_model_matrix = mat; }

protected:
    Mesh     m_mesh;
    Material m_material;
    glm::mat4 m_model_matrix;

    void init_mesh();
    void init_material();
};

#endif
