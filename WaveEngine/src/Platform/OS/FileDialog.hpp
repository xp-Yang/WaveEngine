#ifndef FileDialog_hpp
#define FileDialog_hpp

#include <WaveEngine/Application.hpp>
#include <WaveEngine/Window.hpp>
#include <GLFW/glfw3.h>
#include <string>
// TODO 依赖GLFW的部分改为
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class FileDialog
{
public:
    virtual std::string OpenFile(const char* filter) = 0;
    virtual std::string SaveFile(const char* filter) = 0;
    static FileDialog* create();

protected:
    FileDialog() = default;
    FileDialog(const FileDialog&) = delete;
    FileDialog& operator=(const FileDialog&) = delete;
    virtual ~FileDialog() = default;
};

#endif
