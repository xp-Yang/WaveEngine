#include "RenderShaderObject.hpp"

#include <Platform/RHI/rhi.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

#include "Core/Utils/Utils.hpp"
#include "Core/Logger/Logger.hpp"

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
    ShaderParser(const std::string& filepath) {
        Logger::Logger::get().info("parsing the shader file {}", filepath);
        load_file(filepath);
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
    bool load_file(const std::string& filepath) {
        std::string directory = filepath.substr(0, filepath.find_last_of("/\\"));

        std::stringstream buffer;
        try
        {
            std::ifstream file_stream;
            file_stream.open(filepath);
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
            Logger::Logger::get().error("SHADER FILE NOT SUCCESFULLY READ");
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
                tag = Utils::trim(tag, " \t\r\n\"<>");

                // 加载 include 文件
                std::string include_filepath = directory + '/' + tag;
                if (!this->load_file(include_filepath)) {
                    assert(false);
                    return false;
                }

                m_lines.push_back(GlslLine(GlslLine::Include, std::string("// #include ") + include_filepath));
            }
            else {
                m_lines.push_back(GlslLine(GlslLine::Code, line));
            }
        }

        return true;
    }

private:
    std::vector<GlslLine> m_lines;
};


RenderShaderObject* RenderShaderObject::getShaderObject(const Asset::ShaderType& type)
{
    std::string resource_dir = RESOURCE_DIR;
    Asset::Shader shader_asset;
    switch (type)
    {
    case Asset::ShaderType::GBufferShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/gBufferPass.vs", resource_dir + "/shader/gBufferPass.fs" };
        break;
    case Asset::ShaderType::DeferredLightingShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/deferredLighting.vs", resource_dir + "/shader/deferredLighting.fs" };
        break;
    case Asset::ShaderType::PBRShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/pbr.vs", resource_dir + "/shader/pbr.fs" };
        break;
    case Asset::ShaderType::SkyboxShader:
        shader_asset = Asset::Shader{ type, std::string(RESOURCE_DIR) + "/shader/skybox.vs", std::string(RESOURCE_DIR) + "/shader/skybox.fs" };
        break;
    case Asset::ShaderType::BorderShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/border.vs", resource_dir + "/shader/border.fs" };
        break;
    case Asset::ShaderType::NormalShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/model.vs", resource_dir + "/shader/normal.fs", resource_dir + "/shader/normal.gs" };
        break;
    case Asset::ShaderType::WireframeShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/model.vs", resource_dir + "/shader/wireframe.fs", resource_dir + "/shader/wireframe.gs" };
        break;
    case Asset::ShaderType::CheckerboardShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/checkerboard.vs", resource_dir + "/shader/checkerboard.fs" };
        break;
    case Asset::ShaderType::PickingShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/picking.vs", resource_dir + "/shader/picking.fs" };
        break;
    case Asset::ShaderType::RayTracingShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/RayTracing.vs", resource_dir + "/shader/RayTracing.fs" };
        break;
    case Asset::ShaderType::QuadShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/screenQuad.vs", resource_dir + "/shader/screenQuad.fs" };
        break;
    case Asset::ShaderType::DepthShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/depth.vs", resource_dir + "/shader/depth.fs" };
        break;
    case Asset::ShaderType::CubeMapShader:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/cubeMap.vs", resource_dir + "/shader/cubeMap.fs" };
        //shader_asset = Asset::Shader{ type, resource_dir + "/shader/cubeMap2.vs", resource_dir + "/shader/cubeMap2.fs", resource_dir + "/shader/cubeMap2.gs" };
        break;
    case Asset::ShaderType::GaussianBlur:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/gaussianBlur.vs", resource_dir + "/shader/gaussianBlur.fs" };
        break;
    case Asset::ShaderType::EdgeDetection:
        shader_asset = Asset::Shader{ type, resource_dir + "/shader/edgeDetection.vs", resource_dir + "/shader/edgeDetection.fs" };
        break;
    default:
        break;
    }
    return new RenderShaderObject(shader_asset);
}

RenderShaderObject::RenderShaderObject(const Asset::Shader& shader_asset)
{
    if (shader_asset.vshader_filepath.empty()) {
        Logger::Logger::get().error("a vertex shader is needed!");
        assert(false);
    }
    if (shader_asset.fshader_filepath.empty()) {
        Logger::Logger::get().error("a fragment shader is needed!");
        assert(false);
    }
    bool has_geo_shader = !shader_asset.gshader_filepath.empty();

    // 1. read the source code and process file include
    ShaderParser v_parser(shader_asset.vshader_filepath);
    m_vertexCode = v_parser.getProcessedSourceCode();

    ShaderParser f_parser(shader_asset.fshader_filepath);
    m_fragmentCode = f_parser.getProcessedSourceCode();

    if (has_geo_shader) {
        ShaderParser g_parser(shader_asset.gshader_filepath);
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
        Logger::Logger::get().error("SHADER::VERTEX::COMPILATION_FAILED\n{}\n", infoLog);
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
            Logger::Logger::get().error("SHADER::GEOMETRY::COMPILATION_FAILED\n{}\n", infoLog);
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
        Logger::Logger::get().error("SHADER::FRAGMENT::COMPILATION_FAILED\n{}\n", infoLog);
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
        Logger::Logger::get().error("SHADER::PROGRAM::LINKING_FAILED\n{}\n", infoLog);
        assert(false);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (has_geo_shader)
        glDeleteShader(geometry);
}

void RenderShaderObject::start_using() const {
    glUseProgram(m_id);
}

void RenderShaderObject::stop_using() const {
    glUseProgram(0);
}

void RenderShaderObject::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}
void RenderShaderObject::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void RenderShaderObject::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
void RenderShaderObject::setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform4f(location, value1, value2, value3, value4);
}
void RenderShaderObject::setFloat4(const std::string& name, const Vec4& value) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}
void RenderShaderObject::setFloat3(const std::string& name, const Vec3& value) const
{
    GLint location = glGetUniformLocation(m_id, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}
void RenderShaderObject::setMatrix(const std::string& name, int count, const Mat4& mat_value) const
{
    GLuint transformLoc = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix4fv(transformLoc, count, GL_FALSE, /*glm::value_ptr(mat_value)*/&(mat_value[0].x));
}
void RenderShaderObject::setTexture(const std::string& name, int texture_unit, unsigned int texture_id) const
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    setInt(name, texture_unit);
    glActiveTexture(GL_TEXTURE0);
}

void RenderShaderObject::setCubeTexture(const std::string& name, int texture_unit, unsigned int texture_id) const
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    setInt(name, texture_unit);
    glActiveTexture(GL_TEXTURE0);
}
