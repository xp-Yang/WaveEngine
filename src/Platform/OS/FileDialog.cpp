#include "FileDialog.hpp"
#include "Windows/WindowsFileDialog.hpp"

FileDialog* FileDialog::create()
{
    auto file_dialog = new WindowsFileDialog();
#ifdef __LINUX__
    auto file_dialog;
#endif
    return file_dialog;
}