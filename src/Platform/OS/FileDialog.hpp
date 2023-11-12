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
    FileDialog() = default;
    FileDialog(const FileDialog&) = delete;
    FileDialog& operator=(const FileDialog&) = delete;
    virtual ~FileDialog() = default;
    virtual std::string OpenFile(const char* filter) = 0;
    virtual std::string SaveFile(const char* filter) = 0;
    static FileDialog* create();
};

#endif
