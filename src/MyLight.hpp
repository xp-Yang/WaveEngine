#ifndef MyLight_hpp
#define MyLight_hpp
#include "Object.hpp"

class MyLight : public Object {
public:
	MyLight();

protected:
    void init_meshes();
    void init_materials() {};
};

#endif