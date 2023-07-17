#ifndef MyShader_hpp
#define MyShader_hpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class MyShader{
public:
	MyShader(const char* vertexPath, const char* fragmentPath);

	void start_using();
	void stop_using();
	GLuint get_id() { return m_id; }
	// uniform¹¤¾ßº¯Êý
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	GLuint m_id;
};

#endif

