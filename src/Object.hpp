#ifndef Object_hpp
#define Object_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>
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
    int get_id() const { return m_id; }
    bool is_enable_reflection() const { return m_enable_relection; }
    void enable_reflection(bool enable) { m_enable_relection = enable; }

protected:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    bool m_enable_relection;
    int m_id;

    virtual void init_meshes() {}
    virtual void init_materials() {}

private:
    static Mesh default_mesh;
    static Material default_material;
    static int id_count;
};

#endif
