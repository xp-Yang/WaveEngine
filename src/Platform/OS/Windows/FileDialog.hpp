#ifndef FileDialog_hpp
#define FileDialog_hpp

// ʹ��windows sdk���ṩ��ͷ�ļ�
#include <commdlg.h>
// TODO ����GLFW�Ĳ��ָ�Ϊ
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <optional>
#include <string>

class FileDialog {
public:
	std::optional<std::string> OpenFile(GLFWwindow* window, const char* filter)
	{
		// �������ļ��Ĵ���, ��Ҫ����һ��OPENFILENAMEA����
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		// ���ofn����Ϣ
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		// 
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		// ����win32 api
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
		// ����win32 api
		//if (GetSaveFileNameA(&ofn) == TRUE)
		//	return ofn.lpstrFile;

		return {};
	}
private:

};


#endif // !FileDialog_hpp
