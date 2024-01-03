#ifndef Logger_hpp
#define Logger_hpp

#include <glm/glm.hpp>
#include <string>

std::string matrix_log(const glm::mat4 mat);

std::string vec3_log(const glm::vec3 vec);

#endif