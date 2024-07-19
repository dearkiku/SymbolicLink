#include "Method.h"
#include <ShlObj.h>
//居中窗口
void CenterWindow(HWND hWnd)
{
	RECT rcWindow, rcScreen{};
	GetWindowRect(hWnd, &rcWindow);
	// 获取工作区（不包括任务栏）
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0); 

	// 计算窗口左上角的新位置
	int x = (rcScreen.right + rcScreen.left - (rcWindow.right - rcWindow.left)) / 2;
	int y = (rcScreen.bottom + rcScreen.top - (rcWindow.bottom - rcWindow.top)) / 2;

	// 调整窗口位置
	MoveWindow(hWnd, x, y, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, TRUE);
}

//  创建不存在的目录
bool createPath(LPCTSTR path) {
	if (!PathIsDirectory(path))
	{
		return CreateDirectory(path, NULL);
	}
	return true;
}

//  弹出错误提示
void ShowErrorMessageBox(HWND hWnd, DWORD errorCode) {
	LPVOID lpMsgBuf = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	MessageBox(hWnd, (LPCTSTR)lpMsgBuf, L"错误", MB_OK | MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

// 浏览文件夹对话框函数
HRESULT BrowseFolder(HWND owner, TCHAR* folderPath, const TCHAR* title) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileDialog* pfd = NULL;
		// 创建FileOpenDialog对象
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pfd));

		if (SUCCEEDED(hr)) {
			// 屏蔽这里用户就可以选择文件，但是不能选择文件夹
			DWORD dwOptions;
			hr = pfd->GetOptions(&dwOptions);
			if (SUCCEEDED(hr)) {
				hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}

			// 设置标题
			if (title) {
				hr = pfd->SetTitle(title);
			}

			// 显示对话框
			hr = pfd->Show(owner);

			// 获取返回值
			if (SUCCEEDED(hr)) {
				IShellItem* psi;
				hr = pfd->GetResult(&psi);
				if (SUCCEEDED(hr)) {
					PWSTR pszPath;
					hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
					if (SUCCEEDED(hr)) {
						wcscpy_s(folderPath, MAX_PATH, pszPath);
						CoTaskMemFree(pszPath);
					}
					psi->Release();
				}
			}
			pfd->Release();
		}
		CoUninitialize();
	}
	return hr;
}

//是否是符号链接
bool IsSymbolicLink(LPCTSTR lpSymlinkFileName) {
	DWORD attributes = GetFileAttributes(lpSymlinkFileName);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

//创建符号链接
bool CreateSymLink(LPCTSTR lpSymlinkFileName, LPCTSTR lpTargetFileName) {
	//这里验证目标是文件还是文件夹，
	// 因为来源{lpSymlinkFileName}可以不用文件的方式命名
	// 比如可以将目标填入的a.txt指向来源填入的a
	int isFile = IsFileOrDirectory(lpTargetFileName);
	if (isFile == -1)
	{
		//获取类型失败返回假
		return false;
	}
	else if (isFile == 1)
	{
		//目标是文件夹，检查文件夹是否存在，不存在且创建失败返回假
		if (!createPath(lpTargetFileName)) { return false; }
	}
	// SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE 指定此标志以允许在进程未提升时创建符号链接。 
	// 在 UWP 中，必须先在计算机上启用 开发人员模式 ，此选项才能正常工作。 在 MSIX 下，无需为此标志启用开发人员模式。
	return CreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, isFile | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE);
}

// 目录类型
int IsFileOrDirectory(LPCWSTR path) {
	DWORD attributes = GetFileAttributes(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		// 无法获取属性，可能路径无效
		return -1; // 返回 -1 表示错误
	}
	if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
		return 1; // 返回 1 表示路径是文件夹
	}
	else {
		return 0; // 返回 0 表示路径是文件
	}
}

// 删除符号链接的函数
// 如果应用程序尝试删除不存在的文件， DeleteFile 函数将失败并 ERROR_FILE_NOT_FOUND。 
// 如果文件是只读文件，则函数将失败并 ERROR_ACCESS_DENIED。
// 若要删除只读文件，首先必须删除只读属性。
// 函数出错可以调用另一个函数查看错误原因~
bool RemoveSymbolicLink(LPCTSTR symlnkPath, LPCTSTR targetPath) {
	//这里面应该实现删除{symlnkPath}这个已经成为符号链接的文件，然后把文件移动回去（原理是这样，但是我觉得用不上）
	if (!DeleteFile(symlnkPath))
	{
		return false;
	}
	return MoveFileOrFolder(symlnkPath, targetPath);
}

// 隐藏文件/夹
bool HideOriginalPath(LPCTSTR lpTargetFileName) {
	DWORD attributes = GetFileAttributes(lpTargetFileName);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return SetFileAttributes(lpTargetFileName, attributes | FILE_ATTRIBUTE_HIDDEN);
}

// 移动文件/夹
bool MoveFileOrFolder(const TCHAR* srcPath, const TCHAR* destPath) {
	SHFILEOPSTRUCT fileOp = { 0 };
	fileOp.wFunc = FO_MOVE;
	fileOp.pFrom = srcPath;
	fileOp.pTo = destPath;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;

	// 执行文件移动操作
	int result = SHFileOperation(&fileOp);
	//ERROR_SUCCESS（0） ：表示操作成功完成
	return result == 0;
	//其他非零错误代码：表示其他错误情况，可以根据具体的错误代码进行排查和处理
	//ERROR_CANCELLED ：表示用户取消了操作
	//ERROR_FILE_NOT_FOUND ：表示指定的文件或目录不存在
	//ERROR_INVALID_PARAMETER ：表示传递给函数的参数不正确
	//ERROR_OUTOFMEMORY ：表示内存不足，无法完成操作
	//ERROR_BAD_PATHNAME ：表示指定的路径名不正确
	//ERROR_ALREADY_EXISTS ：表示要复制或移动的文件已经存在，且指定了FOF_RENAMEONCOLLISION标志
	//ERROR_ACCESS_DENIED ：表示没有足够的权限执行操作
	//ERROR_TOO_MANY_OPEN_FILES：表示同时打开的文件数量超过系统限制
}

// 检查文件是否存在
bool FileExists(LPCTSTR Path) {
	DWORD fileAttributes = GetFileAttributes(Path);
	//文件读取出错直接返回假
	if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
		return false;
		// 检查是否是文件不存在的错误
		//DWORD errorCode = GetLastError();
		//if (errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND) {
		//	// 文件或路径不存在
		//	return false;
		//}
		// 可能有其他错误
	}
	// 文件存在
	return true;
}