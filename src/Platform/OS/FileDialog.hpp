#ifndef FileDialog_hpp
#define FileDialog_hpp

#include "Application.hpp"
#include <string>
// TODO ����GLFW�Ĳ��ָ�Ϊ
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class FileDialog
{
public:
    virtual std::string OpenFile(const char* filter) {};
    virtual std::string SaveFile(const char* filter) {};
    FileDialog* create();
};

#endif
