#ifndef WindowsFileDialog_hpp
#define WindowsFileDialog_hpp

#include "../FileDialog.hpp"
#include <commdlg.h> // Windows SDK

class WindowsFileDialog : public FileDialog {
public:
	std::string OpenFile(const char* filter) override
	{
		// 打开搜索文件的窗口, 需要传入一个OPENFILENAMEA对象
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::GetApp().getWindow()->getNativeWindowHandle());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		
		// 调用win32 api
		//if (GetOpenFileNameA(&ofn) == TRUE)
		//	return ofn.lpstrFile;

		return {};
	}

	std::string SaveFile(const char* filter) override
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::GetApp().getWindow()->getNativeWindowHandle());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		
		// 调用win32 api
		//if (GetSaveFileNameA(&ofn) == TRUE)
		//	return ofn.lpstrFile;

		return {};
	}
};


#endif // !FileDialog_hpp
