#ifndef RenderShaderObject_hpp
#define RenderShaderObject_hpp

#include "Base/Math/Math.hpp"
#include "Logical/Shader.hpp"

using GL_RESOURCE_HANLE = unsigned int;

class RenderShaderObject {
public:
    static RenderShaderObject* getShaderObject(const ShaderType& type);

    RenderShaderObject(const Shader& shader);

    void start_using() const;
    void stop_using() const;
    // uniform setter
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const;
    void setFloat4(const std::string& name, const Vec4& value) const;
    void setFloat3(const std::string& name, const Vec3& value) const;
    void setMatrix(const std::string& name, int count, const Mat4& mat_value) const;
    void setTexture(const std::string& name, int texture_unit, unsigned int texture_id) const;
    void setCubeTexture(const std::string& name, int texture_unit, unsigned int texture_id) const;

private:
    GL_RESOURCE_HANLE m_id;
    std::string m_vertexCode;
    std::string m_fragmentCode;
    std::string m_geometryCode;
};

#endif // !RenderShaderObject_hpp
