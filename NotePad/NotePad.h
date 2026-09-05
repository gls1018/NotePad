#pragma once
#include <windows.h>

class NotePad
{
public:
	NotePad(HINSTANCE hIns);
	~NotePad();
	bool Create();
	int Run();
	

private:
	bool RegisterWindowClass();
	bool CreateMainWindow();
	LRESULT HandleMessage(UINT msg, WPARAM wPara, LPARAM lPara);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wPara, LPARAM lPara);

private:
	bool CreateMenuBar();
	bool CreateAccelTable();
	bool CreateEditControl();
	bool CreateEditFont();

private:
	bool OnWMCreate();
	bool OnWMSize();
	bool OnWMPaint();
	bool OnWMCommand();
	bool OnWMSysCommand();


private:
	HINSTANCE m_hInstance; // 当前进程实例句柄
	HWND m_hMainWindow;  //主窗口句柄
	HMENU m_hMenuBar;   //菜单栏句柄
	HACCEL m_hAccelTable; //加速键表
	HWND m_hEditControl;
	HFONT m_hEditFont;
};