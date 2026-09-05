#include "NotePad.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <print>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


#define IDM_FILE_NEW 1001
#define IDM_FILE_OPEN 1002
#define IDM_FILE_OPEN_RECENT 1003
#define IDM_FILE_SAVE 1004
#define IDM_FILE_SAVE_AS 1005
#define IDM_FILE_EXIT 1006

#define IDM_EDIT_REVOKE 2001
#define IDM_EDIT_CUT 2002
#define IDM_EDIT_COPY 2003
#define IDM_EDIT_PASTE 2004
#define IDM_EDIT_DELETE 2005
#define IDM_EDIT_FIND 2006
#define IDM_EDIT_FIND_NEXT 2007
#define IDM_EDIT_FIND_PREVIOUS 2008
#define IDM_EDIT_REPLACE 2009

#define IDC_EDIT 3000
NotePad::NotePad(HINSTANCE hIns)
{
	this->m_hInstance = hIns;
	this->m_hEditControl = NULL;
	this->m_hMenuBar = NULL;
	this->m_hMainWindow = NULL;
	this->m_hAccelTable = NULL;
	this->m_hEditFont = NULL;
}

NotePad::~NotePad()
{
	if (this->m_hEditFont)
		DeleteObject(this->m_hEditFont);
	this->m_hEditFont = NULL;
}

bool NotePad::Create()
{
	if (!this->RegisterWindowClass())
		return false;

	if(!this->CreateMenuBar())
		return false;

	if (!this->CreateMainWindow())
		return false;

	if (!this->CreateAccelTable())
		return false;

	ShowWindow(this->m_hMainWindow, SW_NORMAL);
	UpdateWindow(this->m_hMainWindow);
	return true;
}



LRESULT NotePad::HandleMessage(UINT msg, WPARAM wPara, LPARAM lPara)
{	
	switch (msg)
	{
		case WM_CREATE:
		{
			if (!this->OnWMCreate())
				return -1;	
		} break;

		case WM_SIZE:
			OnWMSize();
			break;


		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProcW(this->m_hMainWindow, msg, wPara, lPara);
	}
	return DefWindowProcW(this->m_hMainWindow, msg, wPara, lPara);
}

LRESULT NotePad::WindowProc(HWND hWnd, UINT msg, WPARAM wPara, LPARAM lPara)
{
	NotePad* pThis = NULL;
	switch (msg)
	{
		case WM_NCCREATE:
		{
			pThis = (NotePad*)reinterpret_cast<CREATESTRUCT*>(lPara)->lpCreateParams;
			pThis->m_hMainWindow = hWnd;
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
		} break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
		{
			pThis = (NotePad*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
			if (pThis != NULL)
				return pThis->HandleMessage(msg, wPara, lPara);
			else
				return DefWindowProcW(hWnd, msg, wPara, lPara);
		}
	}
	return DefWindowProcW(hWnd, msg, wPara, lPara);
}

//注册窗口类
bool NotePad::RegisterWindowClass()
{
	WNDCLASSEXW wc{ 0 };
	wc.cbSize = sizeof(wc);
	wc.hInstance = GetModuleHandleW(NULL);    // 应用程序实例句柄
	wc.lpszClassName = L"MyWindowClass";
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_HAND);
	wc.hIcon = (HICON)LoadImageW(NULL, L"./notepad.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImageW(NULL, L"./notepad.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.lpfnWndProc = WindowProc;

	if (!RegisterClassExW(&wc))
		return false;
	return true;
}

bool NotePad::CreateAccelTable()
{
	std::vector<ACCEL> vecAccelTable;
	vecAccelTable.push_back({ FCONTROL | FVIRTKEY, 'S', IDM_FILE_SAVE });
	vecAccelTable.push_back({FCONTROL | FSHIFT | FVIRTKEY, 'P', IDM_FILE_SAVE_AS});

	this->m_hAccelTable = CreateAcceleratorTableW(vecAccelTable.data(), vecAccelTable.size());
	if (this->m_hAccelTable == NULL)
		return false;
	return true;
}

bool NotePad::CreateEditControl()
{
	RECT rc;
	GetClientRect(this->m_hMainWindow, &rc);
	int Width = rc.right - rc.left;
	int Height = rc.bottom - rc.top;

	DWORD dwExStyle = WS_EX_ACCEPTFILES;
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
	this->m_hEditControl = CreateWindowExW(dwExStyle, L"Edit", NULL, dwStyle,
		5, 5, Width - 10, Height - 10, this->m_hMainWindow, (HMENU)IDC_EDIT, this->m_hInstance, NULL);
	if (this->m_hEditControl == NULL)
		return false;
	return true;
}

bool NotePad::CreateEditFont()
{
	this->m_hEditFont = CreateFontW(
		40,                    // 高度
		0,                      // 宽度，0 = 自动
		0,                      // Escapement
		0,                      // Orientation
		FW_NORMAL,              // 字体粗细
		FALSE,                  // Italic
		FALSE,                  // Underline
		FALSE,                  // StrikeOut
		DEFAULT_CHARSET,        // 字符集
		OUT_DEFAULT_PRECIS,     // 输出精度
		CLIP_DEFAULT_PRECIS,    // 剪裁精度
		CLEARTYPE_QUALITY,      // 输出质量
		DEFAULT_PITCH | FF_DONTCARE,
		L"Consolas"             // 字体名称
	);
	SendMessageW(this->m_hEditControl, WM_SETFONT, (WPARAM)this->m_hEditFont, (LPARAM)TRUE);
	return true;
}

bool NotePad::OnWMCreate()
{
	if (!this->CreateMenuBar())
		return false;
	SetMenu(this->m_hMainWindow, this->m_hMenuBar);

	if (!this->CreateAccelTable())
		return false;

	if (!this->CreateEditControl())
		return false;

	if (!this->CreateEditFont())
		return false;
	return true;
}

bool NotePad::OnWMSize()
{
	RECT rc;
	GetClientRect(this->m_hMainWindow, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	if (!MoveWindow(this->m_hEditControl, 2, 2, width, height, TRUE))
		return false;

	return true;
}

bool NotePad::CreateMainWindow()
{
	// 获取显示器的分辨率.
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	DWORD screenWidth = dm.dmPelsWidth;
	DWORD screenHeight = dm.dmPelsHeight;

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD dwExStyle = WS_EX_ACCEPTFILES;  //接收拖拽文件.
	this->m_hMainWindow = CreateWindowExW(dwExStyle, L"MyWindowClass", L"NotePad", dwStyle,
		screenWidth / 8, screenHeight / 8, screenWidth / 3, screenHeight / 3, NULL, this->m_hMenuBar, this->m_hInstance, this);

	if (this->m_hMainWindow == NULL)
	{
		int ErrorCode = GetLastError();
		return false;
	}
		
	return true;
}

bool NotePad::CreateMenuBar()
{
	this->m_hMenuBar = CreateMenu();
	HMENU hFileMenu = CreatePopupMenu();
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_NEW, L"新建");
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_OPEN, L"打开");
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_OPEN_RECENT, L"打开最近文件...");
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_SAVE, L"保存");
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_SAVE_AS, L"另存为");
	AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"退出");

	HMENU hEditMenu = CreatePopupMenu();
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_REVOKE, L"撤销");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_CUT, L"剪切");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_COPY, L"复制");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_PASTE, L"粘贴");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_DELETE, L"删除");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_FIND, L"查找");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_FIND_NEXT, L"查找下一个");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_FIND_PREVIOUS, L"查找上一个");
	AppendMenuW(hEditMenu, MF_STRING, IDM_EDIT_REPLACE, L"替换");

	HMENU hViewMenu = CreatePopupMenu();
	HMENU hHelpMenu = CreatePopupMenu();

	AppendMenuW(this->m_hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"文件");
	AppendMenuW(this->m_hMenuBar, MF_POPUP, (UINT_PTR)hEditMenu, L"编辑");
	AppendMenuW(this->m_hMenuBar, MF_POPUP, (UINT_PTR)hViewMenu, L"视图");
	AppendMenuW(this->m_hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, L"帮助");
	return true;
}


int NotePad::Run()
{
	MSG msg{};
	while (GetMessageW(&msg, NULL, NULL, NULL))
	{
		if (!TranslateAcceleratorW(this->m_hMainWindow, this->m_hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}