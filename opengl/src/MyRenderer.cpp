#include "MyRenderer.hpp"

void MyRenderer::draw(const MyShader& shader, GLuint vao_id, DrawMode mode, int indices_count, int array_count, std::vector<Texture> textures)
{
    shader.start_using();

    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string name;
        aiTextureType type = textures[i].type;
        if (type == aiTextureType_DIFFUSE) {
            name = "texture_diffuse";
            name += std::to_string(diffuseNr++);
        }
        else if (type == aiTextureType_SPECULAR) {
            name = "texture_specular";
            name += std::to_string(specularNr++);
        }
        else if (type == aiTextureType_NORMALS) {
            name = "texture_normal";
            name += std::to_string(normalNr++);
        }
        else if (type == aiTextureType_HEIGHT) {
            name = "texture_height";
            name += std::to_string(heightNr++);
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.get_id(), name.c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(vao_id);

    if (mode == DrawMode::Arrays) {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else if (mode == DrawMode::Indices) {
        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}