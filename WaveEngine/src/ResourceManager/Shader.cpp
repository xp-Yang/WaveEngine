#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        if (geometryPath != nullptr)
            gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (geometryPath != nullptr)
            gShaderStream << gShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        if (geometryPath != nullptr)
            gShaderFile.close();
        // 转换数据流到string
        m_vertexCode = vShaderStream.str();
        m_fragmentCode = fShaderStream.str();
        if (geometryPath != nullptr)
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

    // 2. 编译着色器
    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // 顶点着色器
    // TODO这个类应该是openglshader封装类，真正的shader类应该只调用rhi
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 打印编译错误（如果有的话）
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        assert(false);
    };

    //
    if (geometryPath != nullptr) {
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

    // 片段着色器也类似
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

    // 着色器程序
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    if (geometryPath != nullptr)
        glAttachShader(m_id, geometry);
    glLinkProgram(m_id);
    // 打印连接错误（如果有的话）
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        assert(false);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

Shader::Shader(const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath)
    : Shader(vertexPath.c_str(), fragmentPath.c_str(), geometryPath.empty() ? nullptr : geometryPath.c_str())
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
