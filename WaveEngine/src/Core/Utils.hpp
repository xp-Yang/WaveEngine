#ifndef Utils_hpp
#define Utils_hpp

namespace Utils {

inline std::string trim(const std::string& str, const std::string& spaces = " \t\r\n")
{
    size_t a = str.find_first_not_of(spaces);
    size_t b = str.find_last_not_of(spaces) + 1;
    if (b <= a) {
        return std::string();
    }
    return str.substr(a, b - a);
}

inline std::string mat4ToStr(const Mat4 mat)
{
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].x, mat[1].x, mat[2].x, mat[3].x);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].y, mat[1].y, mat[2].y, mat[3].y);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].z, mat[1].z, mat[2].z, mat[3].z);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].w, mat[1].w, mat[2].w, mat[3].w);
    result += buf;
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

inline std::string vec3ToStr(const Vec3 vec) {
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f \n", vec.x, vec.y, vec.z);
    result += buf;
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

}

#endif // !Utils_hpp
