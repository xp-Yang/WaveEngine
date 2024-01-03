#include "FileDialog.hpp"
#include "Windows/WindowsFileDialog.hpp"

FileDialog* FileDialog::create()
{
    static WindowsFileDialog file_dialog;
#ifdef __LINUX__
    auto file_dialog;
#endif
    return &file_dialog;
}
