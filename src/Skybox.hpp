#ifndef Skybox_hpp
#define Skybox_hpp

#include "Object.hpp"
#include "MyCube.hpp"

class Skybox : public MyCube {
public:
	Skybox();
	unsigned int get_texture_id() const { return m_texture_id; }
private:
	unsigned int generate_cube_map(std::vector<std::string> faces);
	unsigned int m_texture_id;
};

#endif