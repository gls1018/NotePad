#include "NotePad.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <print>
#include <dwmapi.h>
#include <Commdlg.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Comdlg32.lib")


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

		case WM_COMMAND:
			OnWMCommand(wPara, lPara);
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

bool NotePad::Initilize()
{
	//IDM_FILE_OPEN 相关变量初始化
	this->hOpenFile = NULL;
	this->OpenFileName = { 0 };
	OpenFileName.lStructSize = sizeof(OpenFileName);
	OpenFileName.dwReserved = NULL;
	OpenFileName.pvReserved = NULL;
	OpenFileName.lpTemplateName = NULL;
	OpenFileName.lpstrTitle = L"请选择要打开的文本文件";
	OpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

	OpenFileName.hwndOwner = this->m_hMainWindow;
	OpenFileName.hInstance = NULL;
	OpenFileName.lpstrFile = FileNameBuffer;
	OpenFileName.nMaxFile = MAX_PATH;
	OpenFileName.lpstrFilter = L"文本文件 (*.txt)\0*.txt\0";
	OpenFileName.lpstrInitialDir = L"C:\\";

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

	//初始化相关变量
	if (!this->Initilize())
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

bool NotePad::OnWMCommand(WPARAM wPara, LPARAM lPara)
{
	int Id = LOWORD(wPara);
	switch (Id)
	{
		case IDM_FILE_OPEN:
			this->OnOpenFile();
			break;

		case IDM_EDIT_FIND:
			this->ShowFindDialog();
			break;

		default:
			break;
	}

	return true;
}

void NotePad::ShowFindDialog()
{
	this->m_FindReplace = {};
	this->m_FindReplace.lStructSize = sizeof(this->m_FindReplace);
	this->m_FindReplace.lpstrFindWhat = this->m_FindBuffer;
	this->m_FindReplace.wFindWhatLen = ARRAYSIZE(this->m_FindBuffer);
	this->m_FindReplace.Flags = FR_DOWN;
	this->m_FindReplace.hwndOwner = this->m_hMainWindow;
	FindTextW(&this->m_FindReplace);
}

bool NotePad::OnOpenFile()
{
	BOOL bRes = GetOpenFileNameW(&OpenFileName);
	if(!bRes)
		throw std::runtime_error("GetOpenFileNameW Error\n");

	return true;
}

bool NotePad::LoadTextFile()
{
	SECURITY_ATTRIBUTES se{0};
	DWORD dwAccess = GENERIC_READ | GENERIC_WRITE;
	this->hOpenFile = CreateFileW(FileNameBuffer, dwAccess, 0, &se, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("CreateFile Error\n");

	DWORD dwFileSize = GetFileSize(hOpenFile, NULL);
	if (dwFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(this->hOpenFile);
		this->hOpenFile = NULL;
		throw std::runtime_error("GetFileSize Error\n");
	}



	std::vector<BYTE> buffer(dwFileSize);
	DWORD dwByteRead = 0;
	if (!ReadFile(hOpenFile, buffer.data(), dwFileSize, &dwByteRead, NULL))
		throw std::runtime_error("ReadFile Error\n");

	// 判断编码
	if (dwByteRead >= 3 &&
		buffer[0] == 0xEF &&
		buffer[1] == 0xBB &&
		buffer[2] == 0xBF)
	{
		//UTF-8 BOM

	}
	
	else if (dwByteRead >= 2 &&
		buffer[0] == 0xFF &&
		buffer[1] == 0xFE)
	{
		// UTF-16 LE
	}

	else if (dwByteRead >= 2 &&
		buffer[0] == 0xFE &&
		buffer[1] == 0xFF)
	{
		// UTF-16 BE
	}

	else
	{

	}
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