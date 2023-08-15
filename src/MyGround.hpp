#ifndef MyGround_hpp
#define MyGround_hpp
#include "MyCube.hpp"

class MyGround : public MyCube {
public:
	MyGround(const glm::vec4& color);
protected:
	virtual void create_vbo() override;
	virtual void create_vao() override;
};

#endif