#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderParser {
public:
    struct GlslLine {
        enum Tag {
            Include,
            Code,
        };
        Tag tag;
        std::string line;
        GlslLine(Tag tag_, const std::string& line_) :tag(tag_), line(line_) {}
    };

public: 
    ShaderParser(const std::string& filename) {
        load_file(filename);
    }

    std::string getProcessedSourceCode() {
        std::string res;
        for (const auto& line : m_lines) {
            if (line.tag == GlslLine::Code) {
                res += (line.line + "\n");
            }
        }
        return res;
    }

protected:
    bool load_file(const std::string& filename) {
        std::string directory = filename.substr(0, filename.find_last_of('/'));

        std::stringstream buffer;
        try
        {
            std::ifstream file_stream;
            file_stream.open(filename);
            if (file_stream.is_open()) {
                buffer << file_stream.rdbuf();
                file_stream.close();
            }
            else {
                assert(false);
                return false;
            }

        }
        catch (...)
        {
            std::cout << "ERROR::SHADER FILE NOT SUCCESFULLY READ" << std::endl;
            assert(false);
            return false;
        }

        while (!buffer.eof()) {
            std::string line;
            std::getline(buffer, line);

            size_t pos = line.find_first_not_of(" ");
            if (pos == std::string::npos) {
                m_lines.push_back(GlslLine(GlslLine::Code, line));
                continue;
            }

            if (line[pos] != '#') {
                m_lines.push_back(GlslLine(GlslLine::Code, line));
                continue;
            }

            // the line is started by '#'
            std::stringstream stm(line);
            std::string tag;
            stm >> tag;

            if (tag == "#include") {
                stm >> tag;
                tag = tag.substr(0, tag.find("//")); // 过滤注释
                tag = trim(tag, " \t\r\n\"<>");

                // 加载 include 文件
                std::string include_filename = directory + '/' + tag;
                if (!this->load_file(include_filename)) {
                    assert(false);
                    return false;
                }

                m_lines.push_back(GlslLine(GlslLine::Include, std::string("// #include ") + include_filename));
            }
            else {
                m_lines.push_back(GlslLine(GlslLine::Code, line));
            }
        }

        return true;
    }

    std::string trim(const std::string& str, const std::string& spaces = " \t\r\n")
    {
        size_t a = str.find_first_not_of(spaces);
        size_t b = str.find_last_not_of(spaces) + 1;
        if (b <= a) {
            return std::string();
        }
        return str.substr(a, b - a);
    }

private:
    std::vector<GlslLine> m_lines;
};

Shader::Shader(const std::string vertexPath, const std::string geometryPath, const std::string fragmentPath)
{
    assert(!vertexPath.empty());
    assert(!fragmentPath.empty());
    bool has_geo_shader = !geometryPath.empty();

    // 1. read the source code and process file include
    ShaderParser v_parser(vertexPath);
    m_vertexCode = v_parser.getProcessedSourceCode();

    ShaderParser f_parser(fragmentPath);
    m_fragmentCode = f_parser.getProcessedSourceCode();

    if (has_geo_shader) {
        ShaderParser g_parser(geometryPath);
        m_geometryCode = g_parser.getProcessedSourceCode();
    }

    const char* vShaderCode = m_vertexCode.c_str();
    const char* fShaderCode = m_fragmentCode.c_str();
    const char* gShaderCode = m_geometryCode.c_str();

    // 2. compile
    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // 顶点着色器
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
    if (has_geo_shader)
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
    case ShaderType::CheckerboardShader:
        static Shader* grid_shader = new Shader(resource_dir + "/shader/checkerboard.vs", resource_dir + "/shader/checkerboard.fs");
        return grid_shader;
    case ShaderType::PickingShader:
        static Shader* picking_shader = new Shader(resource_dir + "/shader/picking.vs", resource_dir + "/shader/picking.fs");
        return picking_shader;
    case ShaderType::RayTracingShader:
        static Shader* rt_shader = new Shader(resource_dir + "/shader/RayTracing.vs", resource_dir + "/shader/RayTracing.fs");
        return rt_shader;
    case ShaderType::QuadShader:
        static Shader* quad_shader = new Shader(resource_dir + "/shader/screenQuad.vs", resource_dir + "/shader/screenQuad.fs");
        return quad_shader;
    case ShaderType::DepthShader:
        static Shader* depth_shader = new Shader(resource_dir + "/shader/depth.vs", resource_dir + "/shader/depth.fs");
        return depth_shader;
    default:
        return nullptr;
    }
}
