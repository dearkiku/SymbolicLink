#include "Method.h"
#include <ShlObj.h>
//���д���
void CenterWindow(HWND hWnd)
{
	RECT rcWindow, rcScreen{};
	GetWindowRect(hWnd, &rcWindow);
	// ��ȡ����������������������
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0); 

	// ���㴰�����Ͻǵ���λ��
	int x = (rcScreen.right + rcScreen.left - (rcWindow.right - rcWindow.left)) / 2;
	int y = (rcScreen.bottom + rcScreen.top - (rcWindow.bottom - rcWindow.top)) / 2;

	// ��������λ��
	MoveWindow(hWnd, x, y, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, TRUE);
}

//  ���������ڵ�Ŀ¼
bool createPath(LPCTSTR path) {
	if (!PathIsDirectory(path))
	{
		return CreateDirectory(path, NULL);
	}
	return true;
}

//  ����������ʾ
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
	MessageBox(hWnd, (LPCTSTR)lpMsgBuf, L"����", MB_OK | MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

// ����ļ��жԻ�����
HRESULT BrowseFolder(HWND owner, TCHAR* folderPath, const TCHAR* title) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileDialog* pfd = NULL;
		// ����FileOpenDialog����
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&pfd));

		if (SUCCEEDED(hr)) {
			// ���������û��Ϳ���ѡ���ļ������ǲ���ѡ���ļ���
			DWORD dwOptions;
			hr = pfd->GetOptions(&dwOptions);
			if (SUCCEEDED(hr)) {
				hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}

			// ���ñ���
			if (title) {
				hr = pfd->SetTitle(title);
			}

			// ��ʾ�Ի���
			hr = pfd->Show(owner);

			// ��ȡ����ֵ
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

//�Ƿ��Ƿ�������
bool IsSymbolicLink(LPCTSTR lpSymlinkFileName) {
	DWORD attributes = GetFileAttributes(lpSymlinkFileName);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

//������������
bool CreateSymLink(LPCTSTR lpSymlinkFileName, LPCTSTR lpTargetFileName) {
	//������֤Ŀ�����ļ������ļ��У�
	// ��Ϊ��Դ{lpSymlinkFileName}���Բ����ļ��ķ�ʽ����
	// ������Խ�Ŀ�������a.txtָ����Դ�����a
	int isFile = IsFileOrDirectory(lpTargetFileName);
	if (isFile == -1)
	{
		//��ȡ����ʧ�ܷ��ؼ�
		return false;
	}
	else if (isFile == 1)
	{
		//Ŀ�����ļ��У�����ļ����Ƿ���ڣ��������Ҵ���ʧ�ܷ��ؼ�
		if (!createPath(lpTargetFileName)) { return false; }
	}
	// SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE ָ���˱�־�������ڽ���δ����ʱ�����������ӡ� 
	// �� UWP �У��������ڼ���������� ������Աģʽ ����ѡ��������������� �� MSIX �£�����Ϊ�˱�־���ÿ�����Աģʽ��
	return CreateSymbolicLink(lpSymlinkFileName, lpTargetFileName, isFile | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE);
}

// Ŀ¼����
int IsFileOrDirectory(LPCWSTR path) {
	DWORD attributes = GetFileAttributes(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		// �޷���ȡ���ԣ�����·����Ч
		return -1; // ���� -1 ��ʾ����
	}
	if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
		return 1; // ���� 1 ��ʾ·�����ļ���
	}
	else {
		return 0; // ���� 0 ��ʾ·�����ļ�
	}
}

// ɾ���������ӵĺ���
// ���Ӧ�ó�����ɾ�������ڵ��ļ��� DeleteFile ������ʧ�ܲ� ERROR_FILE_NOT_FOUND�� 
// ����ļ���ֻ���ļ���������ʧ�ܲ� ERROR_ACCESS_DENIED��
// ��Ҫɾ��ֻ���ļ������ȱ���ɾ��ֻ�����ԡ�
// ����������Ե�����һ�������鿴����ԭ��~
bool RemoveSymbolicLink(LPCTSTR symlnkPath, LPCTSTR targetPath) {
	//������Ӧ��ʵ��ɾ��{symlnkPath}����Ѿ���Ϊ�������ӵ��ļ���Ȼ����ļ��ƶ���ȥ��ԭ���������������Ҿ����ò��ϣ�
	if (!DeleteFile(symlnkPath))
	{
		return false;
	}
	return MoveFileOrFolder(symlnkPath, targetPath);
}

// �����ļ�/��
bool HideOriginalPath(LPCTSTR lpTargetFileName) {
	DWORD attributes = GetFileAttributes(lpTargetFileName);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return SetFileAttributes(lpTargetFileName, attributes | FILE_ATTRIBUTE_HIDDEN);
}

// �ƶ��ļ�/��
bool MoveFileOrFolder(const TCHAR* srcPath, const TCHAR* destPath) {
	SHFILEOPSTRUCT fileOp = { 0 };
	fileOp.wFunc = FO_MOVE;
	fileOp.pFrom = srcPath;
	fileOp.pTo = destPath;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;

	// ִ���ļ��ƶ�����
	int result = SHFileOperation(&fileOp);
	//ERROR_SUCCESS��0�� ����ʾ�����ɹ����
	return result == 0;
	//�������������룺��ʾ����������������Ը��ݾ���Ĵ����������Ų�ʹ���
	//ERROR_CANCELLED ����ʾ�û�ȡ���˲���
	//ERROR_FILE_NOT_FOUND ����ʾָ�����ļ���Ŀ¼������
	//ERROR_INVALID_PARAMETER ����ʾ���ݸ������Ĳ�������ȷ
	//ERROR_OUTOFMEMORY ����ʾ�ڴ治�㣬�޷���ɲ���
	//ERROR_BAD_PATHNAME ����ʾָ����·��������ȷ
	//ERROR_ALREADY_EXISTS ����ʾҪ���ƻ��ƶ����ļ��Ѿ����ڣ���ָ����FOF_RENAMEONCOLLISION��־
	//ERROR_ACCESS_DENIED ����ʾû���㹻��Ȩ��ִ�в���
	//ERROR_TOO_MANY_OPEN_FILES����ʾͬʱ�򿪵��ļ���������ϵͳ����
}

// ����ļ��Ƿ����
bool FileExists(LPCTSTR Path) {
	DWORD fileAttributes = GetFileAttributes(Path);
	//�ļ���ȡ����ֱ�ӷ��ؼ�
	if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
		return false;
		// ����Ƿ����ļ������ڵĴ���
		//DWORD errorCode = GetLastError();
		//if (errorCode == ERROR_FILE_NOT_FOUND || errorCode == ERROR_PATH_NOT_FOUND) {
		//	// �ļ���·��������
		//	return false;
		//}
		// ��������������
	}
	// �ļ�����
	return true;
}