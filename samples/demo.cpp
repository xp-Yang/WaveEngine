#include "demo.hpp"
#include "cubetest.hpp"
void Demo* createDemo(const std::string& name)
{
    if (name == "cubetest")
        return new Cubetest();
    return nullptr;
}
