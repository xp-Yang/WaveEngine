#ifndef Texture_hpp
#define Texture_hpp

#include <string>
#include <vector>

class Texture {
public:
    static unsigned int generate_texture_from_file(const char* path, const std::string& directory, bool gamma);
    static unsigned int generate_texture_from_file(const std::string& full_path, bool gamma);
    static unsigned int generate_cube_map(std::vector<std::string> faces);

private:
//    unsigned int id;
//    aiTextureType type;
//    //aiString path;
};

#endif
