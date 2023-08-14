#ifndef MyCube_hpp
#define MyCube_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class TextureType {
    None,
    Normal,
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class MyCube {
public:
    MyCube(const char* texture_path = nullptr, const glm::vec3& color = glm::vec3(1.0f));
    virtual ~MyCube();

    GLuint get_vao_id() { return m_vao_id; }
    GLuint get_texture_id() { return m_texture_id; }
    GLuint get_specular_map_id() { return m_specular_map_id; }
    GLuint get_vbo_id() { return m_vbo_id; }
    GLuint get_ibo_id() { return m_ibo_id; }
    int get_elements_count() { return 36; }
    glm::vec3 get_color() { return m_color; }
    void set_color(const glm::vec3& color) { m_color = color; }
    const glm::mat4& get_model_matrix() { return m_model_matrix; }
    void set_model_matrix(const glm::mat4& mat) { m_model_matrix = mat; }
    const Material& get_material() { return m_material; }
    void set_material(const Material& material) { m_material = material; }

protected:
    TextureType m_texture_type;

    GLuint m_vao_id;
    GLuint m_texture_id;
    GLuint m_specular_map_id;
    GLuint m_vbo_id;
    GLuint m_ibo_id;

    glm::vec3 m_color;
    glm::mat4 m_model_matrix;

    Material m_material;

    virtual void create_vbo();
    virtual unsigned int generate_texture(int width, int height, unsigned char* data);
    virtual void create_vao();
};

#endif