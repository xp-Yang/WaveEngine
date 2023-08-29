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

    const std::vector<Mesh>& get_meshes() const { return m_meshes; }
    const std::vector<Material>& get_materials() const { return m_materials; }
    const Mesh& mesh(const int index = 0) const { return index < m_meshes.size() ? m_meshes[index] : Object::default_mesh; }
    const Material& material(const int index = 0) const { return index < m_materials.size() ? m_materials[index] : Object::default_material; }
    Mesh& mesh(const int index = 0) { return index < m_meshes.size() ? m_meshes[index] : Object::default_mesh; }
    Material& material(const int index = 0) { return index < m_materials.size() ? m_materials[index] : Object::default_material; }
    void append_mesh(const Mesh& mesh) { m_meshes.push_back(mesh); }
    void append_material(const Material& material) { m_materials.push_back(material); }
    const glm::mat4& get_model_matrix() const { return m_model_matrix; }
    void set_model_matrix(const glm::mat4& mat) { m_model_matrix = mat; }
    const glm::vec3& get_world_pos() const { return m_model_matrix[3]; }
    bool renderable() const { return m_is_renderable; }
    void set_renderable(bool renderable) { m_is_renderable = renderable; }
    bool render_as_indices() { return m_render_as_indices; }
    void set_render_as_indices(bool as_indices) { m_render_as_indices = as_indices; }
    int get_id() const { return m_id; }
    bool is_picked() const { return m_picked; }
    void set_picked(bool picked) { m_picked = picked; }

protected:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    glm::mat4 m_model_matrix;
    bool m_is_renderable;
    bool m_render_as_indices;
    bool m_picked;
    int m_id;

    virtual void init_meshes() {}
    virtual void init_materials() {}

private:
    static Mesh default_mesh;
    static Material default_material;
    static int id_count;
};

#endif
