#ifndef MyShader_hpp
#define MyShader_hpp
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../imgui/imgui.h"
#include <iostream>

class MyShader{
public:
	MyShader(const char* vertexPath, const char* fragmentPath);

	void start_using() const;
	void stop_using() const;
	GLuint get_id() { return m_id; }
	// uniform ¹¤¾ßº¯Êý
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const;
	void setFloat4(const std::string& name, const glm::vec4& value) const;
	void setFloat3(const std::string& name, const glm::vec3& value) const;
	void setMatrix(const std::string& name, int count, const glm::mat4& mat_value) const;

private:
	GLuint m_id;
};

#endif

