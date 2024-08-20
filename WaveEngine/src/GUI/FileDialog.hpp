#ifndef FileDialog_hpp
#define FileDialog_hpp

#include <Engine.hpp>

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
