#ifndef Shader_hpp
#define Shader_hpp
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader{
public:
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	void start_using() const;
	void stop_using() const;
	unsigned int get_id() const { return m_id; }
	// uniform ¹¤¾ßº¯Êý
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const;
	void setFloat4(const std::string& name, const glm::vec4& value) const;
	void setFloat3(const std::string& name, const glm::vec3& value) const;
	void setMatrix(const std::string& name, int count, const glm::mat4& mat_value) const;
	void setTexture(const std::string& name, int texture_unit, unsigned int texture_id) const;
	void setCubeTexture(const std::string& name, int texture_unit, unsigned int texture_id) const;

private:
	unsigned int m_id;
};

#endif

