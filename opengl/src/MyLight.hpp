#ifndef MyLight_hpp
#define MyLight_hpp
#include "MyCube.hpp"
#include "../imgui/imgui.h"

class MyLight : public MyCube {
public:
	MyLight(const glm::vec4& color);
protected:
	virtual void create_vbo() override;
	virtual void create_vao() override;
};

#endif