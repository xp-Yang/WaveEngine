#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath)
{
    assert(!vertexPath.empty());
    assert(!fragmentPath.empty());
    bool has_geo_shader = true;
    if (geometryPath.empty()) {
        has_geo_shader = false;
    }
    // 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ��֤ifstream��������׳��쳣��
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // ���ļ�
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        if (has_geo_shader)
            gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // ��ȡ�ļ��Ļ������ݵ���������
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (has_geo_shader)
            gShaderStream << gShaderFile.rdbuf();
        // �ر��ļ�������
        vShaderFile.close();
        fShaderFile.close();
        if (has_geo_shader)
            gShaderFile.close();
        // ת����������string
        m_vertexCode = vShaderStream.str();
        m_fragmentCode = fShaderStream.str();
        if (has_geo_shader)
            m_geometryCode = gShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        assert(false);
    }
    const char* vShaderCode = m_vertexCode.c_str();
    const char* fShaderCode = m_fragmentCode.c_str();
    const char* gShaderCode = m_geometryCode.c_str();

    // 2. ������ɫ��
    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // ������ɫ��
    // TODO�����Ӧ����openglshader��װ�࣬������shader��Ӧ��ֻ����rhi
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // ��ӡ�����������еĻ���
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        assert(false);
    };

    //
    if (has_geo_shader) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            assert(false);
        };
    }

    // Ƭ����ɫ��Ҳ����
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<< infoLog << std::endl;
        assert(false);
    };

    // ��ɫ������
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    if (has_geo_shader)
        glAttachShader(m_id, geometry);
    glLinkProgram(m_id);
    // ��ӡ���Ӵ�������еĻ���
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        assert(false);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (has_geo_shader)
        glDeleteShader(geometry);
}

Shader::Shader(const std::string vertexPath, const std::string fragmentPath)
    :Shader(vertexPath, "", fragmentPath)
{
}

void Shader::start_using() const {
    glUseProgram(m_id);
}

void Shader::stop_using() const {
    glUseProgram(0);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform4f(location, value1, value2, value3, value4);
}
void Shader::setFloat4(const std::string& name, const Vec4& value) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Shader::setFloat3(const std::string& name, const Vec3& value) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}
void Shader::setMatrix(const std::string& name, int count, const Mat4& mat_value) const
{
    GLuint transformLoc = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix4fv(transformLoc, count, GL_FALSE, /*glm::value_ptr(mat_value)*/&(mat_value[0].x));
}
void Shader::setTexture(const std::string& name, int texture_unit, unsigned int texture_id) const
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    setInt(name, texture_unit);
    glActiveTexture(GL_TEXTURE0);
}

void Shader::setCubeTexture(const std::string& name, int texture_unit, unsigned int texture_id) const
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    setInt(name, texture_unit);
    glActiveTexture(GL_TEXTURE0);
}

Shader* Shader::getShader(const ShaderType& type)
{
    std::string resource_dir = RESOURCE_DIR;
    switch (type)
    {
    case ShaderType::GBufferShader:
        static Shader* g_shader = new Shader(resource_dir + "/shader/gBufferPass.vs", resource_dir + "/shader/gBufferPass.fs");
        return g_shader;
    case ShaderType::DeferredLightingShader:
        static Shader* light_shader = new Shader(resource_dir + "/shader/deferredLighting.vs", resource_dir + "/shader/deferredLighting.fs");
        return light_shader;
    case ShaderType::PBRShader:
        static Shader* pbr_shader = new Shader(resource_dir + "/shader/pbr.vs", resource_dir + "/shader/pbr.fs");
        return pbr_shader;
    case ShaderType::BorderShader:
        static Shader* border_shader = new Shader(resource_dir + "/shader/border.vs", resource_dir + "/shader/border.fs");
        return border_shader;
    case ShaderType::NormalShader:
        static Shader* normal_shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/normal.gs", resource_dir + "/shader/normal.fs");
        return normal_shader;
    case ShaderType::WireframeShader:
        static Shader* wireframe_shader = new Shader(resource_dir + "/shader/model.vs", resource_dir + "/shader/wireframe.gs", resource_dir + "/shader/wireframe.fs");
        return wireframe_shader;
    case ShaderType::PickingShader:
        static Shader* picking_shader = new Shader(resource_dir + "/shader/picking.vs", resource_dir + "/shader/picking.fs");
        return picking_shader;
    case ShaderType::RayTracingShader:
        static Shader* rt_shader = new Shader(resource_dir + "/shader/RayTracing.vs", resource_dir + "/shader/RayTracing.fs");
        return rt_shader;
    case ShaderType::QuadShader:
        static Shader* quad_shader = new Shader(resource_dir + "/shader/frame.vs", resource_dir + "/shader/frame.fs");
        return quad_shader;
    case ShaderType::DepthShader:
        static Shader* depth_shader = new Shader(resource_dir + "/shader/depth.vs", resource_dir + "/shader/depth.fs");
        return depth_shader;
    default:
        return nullptr;
    }
}
