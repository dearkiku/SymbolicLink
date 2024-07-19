#pragma once
#include <windows.h>

#include <shlwapi.h>

#ifndef METHOD_H
#define METHOD_H
//�ƶ���ť
constexpr auto BUTTON_OK = 100;
//���Ŀ¼
constexpr auto BUTTON_BROWSE_SOURCE = 101;
//���Ŀ¼
constexpr auto BUTTON_BROWSE_TARGET = 102;
//�˳���ť
constexpr auto BUTTON_EXIT = 103;
//��С��ť
constexpr auto BUTTON_HIDE = 104;
//��ѡ��
constexpr auto CHECK_HIDE_STATE = 200;

//��ָ�����ھ���
void CenterWindow(HWND hWnd);

//�ж�Ŀ¼�Ƿ���ڣ�����������򴴽������ش���������������ֱ�ӷ�����
bool createPath(LPCTSTR path);

// ��ȡ����ʾ������Ϣ�ĺ���
// hWnd��������Ϣ����������ʾ
// errorCode���������������ݴ���
void ShowErrorMessageBox(HWND hWnd, DWORD errorCode);

// �����������Ƿ���ڵĺ���
// ��������Ƿ���� FILE_ATTRIBUTE_REPARSE_POINT �� FILE_ATTRIBUTE_DIRECTORY��
//���������������һ���ʾ��Ŀ¼��һ���������ӡ�
bool IsSymbolicLink(LPCTSTR lpSymlinkFileName);

//
//   �����������ӵĺ��� ,����ʹ�þ���·�������·��������������
//   Ĭ������£���������ΪMAX_PATH���ַ��� ��Ҫ����������չ�� 32,767 �����ַ�������·��ǰ����ӡ�\\��\����
// 
// -lpSymlinkFileName  Ҫ�����ķ������ӡ�
// -lpTargetFileName   Ҫ�����ķ������ӵ�Ŀ������ơ�
// 
//�˺�����lpTargetFileName���ļ���lpSymlinkFileName�д���һ������
//�������������lpSymlinkFileName��ʱ���Զ�ָ��lpSymlinkFileName
//
//�������ӿ����Ǿ������ӻ�������ӡ� ����������ָ��·�����Ƶ�ÿ�����ֵ����ӣ����������������������˵������ָ��·���е�λ��ȷ���ġ�
// ����1���ܾ����� 
// ԭ��Ȩ�޲���
// ����2�����ļ��Ѵ���ʱ���޷��������ļ�
// ԭ��Ҫ��{lpSymlinkFileName}ɾ���������޷���������Ϊ���ֲ�����ͬ��
bool CreateSymLink(LPCTSTR lpSymlinkFileName, LPCTSTR lpTargetFileName);

//�ж�·�����ļ������ļ���
// -1 ���� 0 �ļ� 1 �ļ���
int IsFileOrDirectory(LPCWSTR path);
//ɾ����������
bool RemoveSymbolicLink(LPCTSTR lpSymlinkFileName);
// ����ԭ·���ļ����ļ���
bool HideOriginalPath(LPCTSTR lpTargetFileName);

//�ƶ��ļ�/��
bool MoveFileOrFolder(const TCHAR* srcPath, const TCHAR* destPath);
//����ļ������ļ����Ƿ����
bool FileExists(LPCTSTR Path);

// ����ļ��жԻ�����
HRESULT BrowseFolder(HWND owner, TCHAR* folderPath, const TCHAR* title);
#endif // METHOD_H