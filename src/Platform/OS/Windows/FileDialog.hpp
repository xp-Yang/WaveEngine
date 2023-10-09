#ifndef FileDialog_hpp
#define FileDialog_hpp

// 使用windows sdk里提供的头文件
#include <commdlg.h>
// TODO 依赖GLFW的部分改为
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <optional>
#include <string>

class FileDialog {
public:
	std::optional<std::string> OpenFile(GLFWwindow* window, const char* filter)
	{
		// 打开搜索文件的窗口, 需要传入一个OPENFILENAMEA对象
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		// 填充ofn的信息
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		// 
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		// 调用win32 api
		//if (GetOpenFileNameA(&ofn) == TRUE)
		//	return ofn.lpstrFile;

		return {};
	}

	std::optional<std::string> SaveFile(GLFWwindow* window, const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
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
private:

};


#endif // !FileDialog_hpp
