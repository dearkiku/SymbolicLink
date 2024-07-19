// SymbolicLinkMove.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "SymbolicLinkMove.h"
#include "Method.h"
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#include <sstream>

//#pragma comment(lib, "shell32.lib")
constexpr auto MAX_LOADSTRING = 100;

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND hEdit_sourcePath, hEdit_targetPath, hBrowseButton1, hBrowseButton2, hButton, hCheckBoxHide;//, hStaticImage;
HFONT hGlobalFont = nullptr;
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// 加载嵌入的字体
HFONT LoadEmbeddedFont(HINSTANCE hInstance) {
	HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_MYFONT), L"TTF");
	if (hRes) {
		HGLOBAL hMem = LoadResource(hInstance, hRes);
		DWORD dwSize = SizeofResource(hInstance, hRes);
		void* pFontData = LockResource(hMem);
		DWORD dwFonts = 0;
		HANDLE hFontResource = AddFontMemResourceEx(pFontData, dwSize, nullptr, &dwFonts);

		if (hFontResource) {
			LOGFONT lf = { 0 };
			lf.lfHeight = -18;  // 字体高度，可以根据需要调整
			_tcscpy_s(lf.lfFaceName, L".萍方-简");  // 使用字体文件中的字体名称

			HFONT hFont = CreateFontIndirect(&lf);
			return hFont;
		}
	}
	else
	{
		ShowErrorMessageBox(0, GetLastError());
		//MessageBox(NULL, _T("Failed to find resource."), _T("Error"), MB_OK | MB_ICONERROR);
	}

	return nullptr;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SYMBOLICLINKMOVE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYMBOLICLINKMOVE));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);// (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SYMBOLICLINKMOVE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中
	hGlobalFont = LoadEmbeddedFont(hInstance);

	// | 始终置顶窗口 | & ~WS_MAXIMIZEBOX 禁止最大化按钮
	HWND hWnd = CreateWindowEx(
		/*WS_EX_TOPMOST*/0,
		szWindowClass,
		szTitle,
		WS_POPUP,
		//WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 521, 215,
		NULL, NULL, hInstance, NULL
	);


	if (!hWnd)
	{
		return FALSE;
	}
	//if (hGlobalFont) {
	//	// 将字体应用到主窗口
	//	SendMessage(hWnd, WM_SETFONT, (WPARAM)hGlobalFont, TRUE);
	//}
	CenterWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HWND hwndCtl;
	HBRUSH hbr = CreateSolidBrush(RGB(37, 37, 37));
	COLORREF textColor = RGB(245, 245, 245);
	switch (message)
	{
	case WM_CREATE: {
		// 加载字体并设置给所有控件
		/*if (hGlobalFont) {
			SendMessage(hWnd, WM_SETFONT, (WPARAM)hGlobalFont, TRUE);
		}*/
		/*else
		{
			MessageBox(hWnd, L"字体载入失败", L"错误", MB_OK | MB_ICONERROR);
		}*/
		//创建标题
		CreateWindowEx(0,
			L"STATIC", L"SymbolicLinkMove",
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
			1, 1, 521, 25,
			hWnd, NULL, hInst, NULL);
		//// 创建静态控件用于显示LOGO
		//hStaticImage = CreateWindowEx(
		//	0, _T("STATIC"), NULL,
		//	WS_CHILD | WS_VISIBLE | SS_BITMAP,
		//	1, 1, 48, 48,
		//	hWnd, NULL, hInst, NULL);
		//// 从资源加载图标
		//HICON hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 48, 48, 0);
		//// 设置静态控件的图标
		//SendMessage(hStaticImage, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		//关闭按钮
		CreateWindow(
			L"BUTTON", L"X",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			495, 1, 25, 25,
			hWnd, (HMENU)BUTTON_EXIT, hInst, NULL);
		//缩小按钮
		CreateWindow(
			L"BUTTON", L"_",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			469, 1, 25, 25,
			hWnd, (HMENU)BUTTON_HIDE, hInst, NULL);
		// 创建来源目录编辑框
		hEdit_sourcePath = CreateWindowEx(
			WS_EX_STATICEDGE, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,//| ES_CENTER,
			60, 49, 334, 32,
			hWnd, (HMENU)1, hInst, NULL);
		//浏览来源目录
		hBrowseButton1 = CreateWindow(
			L"BUTTON", L"...",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			404, 49, 56, 32,
			hWnd, (HMENU)BUTTON_BROWSE_SOURCE, hInst, NULL);
		// 创建目标目录编辑框
		hEdit_targetPath = CreateWindowEx(
			WS_EX_STATICEDGE, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			60, 101, 334, 32,
			hWnd, (HMENU)2, hInst, NULL);
		//浏览目标目录
		hBrowseButton2 = CreateWindow(
			L"BUTTON", L"...",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			404, 101, 56, 32,
			hWnd, (HMENU)BUTTON_BROWSE_TARGET, hInst, NULL);
		// 创建按钮
		hButton = CreateWindow(
			L"BUTTON", L"移动并链接",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			312, 153, 148, 32,
			hWnd, (HMENU)BUTTON_OK, hInst, NULL);
		// 创建选择框
		hCheckBoxHide = CreateWindow(
			L"BUTTON", L"隐藏来源",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			60, 153, 220, 32,
			hWnd, (HMENU)CHECK_HIDE_STATE, hInst, NULL);
		//枚举组件发送字体更改的消息
		if (hGlobalFont) {
			//SendMessage(hWnd, WM_SETFONT, (WPARAM)hGlobalFont, TRUE);
			EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
				SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
				return TRUE;
				}, (LPARAM)hGlobalFont);
		}
		else
		{
			MessageBox(hWnd, L"字体载入失败！", L"错误", MB_OK | MB_ICONERROR);
		}
		//疑惑按钮
		/*CreateWindow(
			L"BUTTON", L"?",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			522, 1, 25, 25,
			hWnd, (HMENU)BUTTON_, hInst, NULL);*/
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case BUTTON_OK: {
			//第一步，获取编辑框中输入的内容，存储到变量中
			TCHAR sourcePath[MAX_PATH], targetPath[MAX_PATH];
			GetWindowText(hEdit_sourcePath, sourcePath, MAX_PATH);
			GetWindowText(hEdit_targetPath, targetPath, MAX_PATH);
			//第二步，确保路径非空
			if (sourcePath[0] == L'\0' || targetPath[0] == L'\0')
			{
				MessageBox(hWnd, L"不能填入空的源或目标目录！", L"错误", MB_OK | MB_ICONERROR);
				break;
			}
			//第三步，判断两个目录是否一致，确保源路径和目标路径不相同
			if (lstrcmp(sourcePath, targetPath) == 0)
			{
				MessageBox(hWnd, L"不能填入两个相同的目录！", L"错误", MB_OK | MB_ICONERROR);
				break;
			}
			//if (IsFileOrDirectory(sourcePath) != IsFileOrDirectory(targetPath)) {
			//	MessageBox(hWnd, L"两个路径应该都是文件或者文件夹！", L"错误", MB_OK | MB_ICONERROR);
			//	break;
			//}
			//第四步，判断需要源文件/夹是否存在，如果存在需要移动，如果不存在就直接创建符号链接（已经手动移动文件/夹）
			if (FileExists(sourcePath))
			{
				//4.A1步，文件/夹存在，检查是否已经成为符号链接
				if (IsSymbolicLink(sourcePath))
				{
					//如果源已经是符号链接，跳出
					MessageBox(hWnd, L"当前来源目录已经是符号链接！", L"提示", MB_OK | MB_ICONASTERISK);
					break;
				}
				// 4.A2步，执行到这里，文件/夹存在且非符号链接，在这里执行移动函数，将{sourcePath}移动到{targetPath}
				std::wostringstream woss;
				woss << L"即将把\n'" << sourcePath << L"'\n移动到\n'" << targetPath << L"'\n请确保你没有占用该目录下的文件，有足够的权限和磁盘空间进行操作！";
				std::wstring message = woss.str();
				//显示警告信息并且确认用户点击确定之后再继续执行
				int result = MessageBox(hWnd, message.c_str(), L"提示", MB_OKCANCEL | MB_ICONASTERISK);
				//用户未确认，跳出
				if (result != IDOK) { break; }
				//4.A3步，用户已经确认
				BOOL success = MoveFileOrFolder(sourcePath, targetPath);
				if (!success)
				{
					//错误才需要显示，如果执行成功就继续下面的操作
					ShowErrorMessageBox(hWnd, GetLastError());
					break;
				}

			}
			//else
			//{
			//	//4.B1步,源文件/夹不存在,检查目标文件夹是否存在，不存在就创建，然后直接设置符号链接
			//	MessageBox(hWnd, L"文件不存在!", L"提示", MB_OK);
			//	if (!FileExists(targetPath))
			//	{
			// 
			//	}
			//}

			//最终步骤，移动完毕，将{targetPath}映射到{sourcePath}
			//bool move = CreateSymLink(sourcePath, targetPath, SYMBOLIC_LINK_FLAG_DIRECTORY);
			bool move = CreateSymLink(sourcePath, targetPath);
			//失败了就弹出错误窗口并返回
			if (!move)
			{
				//错误才需要显示，如果执行成功就继续下面的操作
				ShowErrorMessageBox(hWnd, GetLastError());
				break;
			}
			//收尾步骤，判断是否隐藏来源目录的快捷方式
			LRESULT checkState = SendMessage(hCheckBoxHide, BM_GETCHECK, 0, 0);
			if (checkState == BST_CHECKED) {
				HideOriginalPath(sourcePath);
				MessageBox(hWnd, L"符号链接创建成功!并且已经将来源目录的快捷方式设为隐藏！", L"操作成功", MB_OK | MB_ICONASTERISK);
			}
			else
			{
				MessageBox(hWnd, L"符号链接创建成功!", L"操作成功", MB_OK | MB_ICONASTERISK);
			}
			break;
		}
		case BUTTON_BROWSE_SOURCE: {
			//浏览目录到TARGET输入框
			TCHAR folderPath[MAX_PATH] = { 0 };
			if (SUCCEEDED(BrowseFolder(hWnd, folderPath, L"选择来源文件夹")))
			{
				SetWindowText(hEdit_sourcePath, folderPath);
			}
			break;
		}
		case BUTTON_BROWSE_TARGET: {
			//浏览目录到TARGET输入框
			TCHAR folderPath[MAX_PATH] = { 0 };
			if (SUCCEEDED(BrowseFolder(hWnd, folderPath, L"选择目标文件夹")))
			{
				SetWindowText(hEdit_targetPath, folderPath);
			}
			break;
		}
		case BUTTON_HIDE: {
			ShowWindow(hWnd, SW_MINIMIZE);
			break;
		}
		case BUTTON_EXIT: {
			DestroyWindow(hWnd);
			break;
		}
						//case CHECK_HIDE_STATE: {
						//	LRESULT checkState = SendMessage(hCheckBoxSpace, BM_GETCHECK, 0, 0);
						//	if (checkState == BST_CHECKED) {
						//		MessageBox(hWnd, L"选中了！", L"Tips", MB_OK);
						//	}
						//	else if (checkState == BST_UNCHECKED) {
						//		MessageBox(hWnd, L"取消了！", L"Tips", MB_OK);
						//	}
						//	break;
						//}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);

		// 填充背景颜色
		HBRUSH hBrushRed = hbr;// CreateSolidBrush(RGB(240, 240, 240));
		FillRect(hdc, &rect, hBrushRed);
		DeleteObject(hBrushRed);
		//画一条线
		MoveToEx(hdc, 0, 27, NULL);
		LineTo(hdc, 600, 27);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_NCHITTEST: {
		//HTCLIENT 客户区 HTCAPTION 标题栏
		POINT pt{ LOWORD(lParam) ,HIWORD(lParam) };
		ScreenToClient(hWnd, &pt);
		if (pt.y < 28)//pt.x > 1 && pt.x < 100 || pt.y>1 &&
		{
			return HTCAPTION;
		}
		return HTCLIENT;
		break;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
		//设置 标签 复选框 编辑框 文本和背景色
		hdc = (HDC)wParam;
		hwndCtl = (HWND)lParam;
		// 设置文本颜色
		SetTextColor(hdc, textColor);
		SetBkMode(hdc, TRANSPARENT);
		//返回背景笔刷
		return (INT_PTR)hbr;
	case WM_DRAWITEM: {
		//绘制按钮
		DRAWITEMSTRUCT* pDrawItem = (DRAWITEMSTRUCT*)lParam;

		// 检查按钮控件ID
		int btnId = pDrawItem->CtlID;
		const char* text = "";
		switch (btnId)
		{
		case BUTTON_BROWSE_SOURCE:
		case BUTTON_BROWSE_TARGET:
			text = "...";
			break;
		case BUTTON_EXIT: {
			text = "×";
			break;
		}
		case BUTTON_HIDE: {
			text = "_";
			break;
		}case BUTTON_OK: {
			text = "移动并链接";
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			//break;
		}
		HDC hdc = pDrawItem->hDC;
		RECT rc = pDrawItem->rcItem;
		//HBRUSH hBrush;
		//if (!pDrawItem->itemState & ODS_SELECTED)
		//{
		//	hBrush = CreateSolidBrush(RGB(255, 0, 0)); // 按下时的背景色
		//}
		//else
		//{
		//	hBrush = CreateSolidBrush(RGB(0, 0, 255)); // 正常时的背景色
		//}
		HBRUSH hBrush = CreateSolidBrush(RGB(68, 68, 68));
		// 绘制背景
		FillRect(hdc, &rc, hBrush);
		// 绘制文本
		SetTextColor(hdc, textColor);
		//SetBkMode(hdc, OPAQUE);
		DrawTextA(hdc, text, strlen(text), &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		DeleteObject(hBrush);
		// 不需要再返回HBRUSH，因为已经处理了绘制
		break;

	}
	case WM_DESTROY:
		// 清理字体
		if (hGlobalFont) {
			DeleteObject(hGlobalFont);
		}
		// 清理笔刷
		if (hbr) {
			DeleteObject(hbr);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
