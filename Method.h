#pragma once
#include <windows.h>

#include <shlwapi.h>

#ifndef METHOD_H
#define METHOD_H
//移动按钮
constexpr auto BUTTON_OK = 100;
//浏览目录
constexpr auto BUTTON_BROWSE_SOURCE = 101;
//浏览目录
constexpr auto BUTTON_BROWSE_TARGET = 102;
//退出按钮
constexpr auto BUTTON_EXIT = 103;
//缩小按钮
constexpr auto BUTTON_HIDE = 104;
//复选框
constexpr auto CHECK_HIDE_STATE = 200;

//将指定窗口居中
void CenterWindow(HWND hWnd);

//判断目录是否存在，如果不存在则创建，返回创建结果，如果存在直接返回真
bool createPath(LPCTSTR path);

// 获取并显示错误信息的函数
// hWnd将决定信息框在哪里显示
// errorCode将决定弹出的内容代码
void ShowErrorMessageBox(HWND hWnd, DWORD errorCode);

// 检查符号链接是否存在的函数
// 检查属性是否包含 FILE_ATTRIBUTE_REPARSE_POINT 和 FILE_ATTRIBUTE_DIRECTORY。
//这两个属性组合在一起表示该目录是一个符号链接。
bool IsSymbolicLink(LPCTSTR lpSymlinkFileName);

//
//   创建符号链接的函数 ,可以使用绝对路径或相对路径创建符号链接
//   默认情况下，名称限制为MAX_PATH个字符。 若要将此限制扩展到 32,767 个宽字符，请在路径前面添加“\\？\”。
// 
// -lpSymlinkFileName  要创建的符号链接。
// -lpTargetFileName   要创建的符号链接的目标的名称。
// 
//此函数将lpTargetFileName的文件在lpSymlinkFileName中创建一个链接
//在其它程序访问lpSymlinkFileName的时候将自动指向lpSymlinkFileName
//
//符号链接可以是绝对链接或相对链接。 绝对链接是指定路径名称的每个部分的链接；相对链接是相对于相对链接说明符在指定路径中的位置确定的。
// 错误1：拒绝访问 
// 原因：权限不够
// 错误2：当文件已存在时，无法创建该文件
// 原因：要将{lpSymlinkFileName}删除，否则无法创建，因为名字不能相同的
bool CreateSymLink(LPCTSTR lpSymlinkFileName, LPCTSTR lpTargetFileName);

//判断路径是文件还是文件夹
// -1 错误 0 文件 1 文件夹
int IsFileOrDirectory(LPCWSTR path);
//删除符号链接
bool RemoveSymbolicLink(LPCTSTR lpSymlinkFileName);
// 隐藏原路径文件或文件夹
bool HideOriginalPath(LPCTSTR lpTargetFileName);

//移动文件/夹
bool MoveFileOrFolder(const TCHAR* srcPath, const TCHAR* destPath);
//检查文件或者文件夹是否存在
bool FileExists(LPCTSTR Path);

// 浏览文件夹对话框函数
HRESULT BrowseFolder(HWND owner, TCHAR* folderPath, const TCHAR* title);
#endif // METHOD_H