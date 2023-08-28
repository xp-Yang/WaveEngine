#ifndef Object_hpp
#define Object_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Mesh.hpp"
#include "Material.hpp"


class Object {
public:
    Object();
    ~Object();

    const Mesh& get_mesh() const { return m_mesh; }
    const Material& get_material() const { return m_material; }
    Material& material() { return m_material; }
    void set_material(const Material& material) { m_material = material; }
    const glm::mat4& get_model_matrix() const { return m_model_matrix; }
    void set_model_matrix(const glm::mat4& mat) { m_model_matrix = mat; }
    const glm::vec3& get_world_pos() const { return m_model_matrix[3]; }
    bool renderable() const { return m_is_renderable; }
    void set_renderable(bool renderable) { m_is_renderable = renderable; }

protected:
    Mesh     m_mesh;
    Material m_material;
    glm::mat4 m_model_matrix;
    bool m_is_renderable{ true };

    virtual void init_mesh();
    virtual void init_material();
};

#endif
