#include "FileDialog.hpp"
#include "Platform/Windows/WindowsFileDialog.hpp"

FileDialog* FileDialog::create()
{
    WindowsFileDialog* file_dialog = new WindowsFileDialog();
#ifdef __LINUX__
    auto file_dialog;
#endif
    return file_dialog;
}
