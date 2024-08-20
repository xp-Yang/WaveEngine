#ifndef WindowsFileDialog_hpp
#define WindowsFileDialog_hpp

#include "GUI/FileDialog.hpp"

class WindowsFileDialog : public FileDialog {
public:
	std::string OpenFile(const char* filter) override;
	std::string SaveFile(const char* filter) override;
};


#endif // !FileDialog_hpp
