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

	bool Initilize();

private:
	bool OnWMCreate();
	bool OnWMSize();
	bool OnWMPaint();
	bool OnWMCommand(WPARAM wPara, LPARAM lPara);
	bool OnWMSysCommand();

private:
	bool OnEditControlNotify(WPARAM wPara, LPARAM lPara);


// 实现查找功能
private:
	FINDREPLACEW m_FindReplace;
	WCHAR m_FindBuffer[256]{ };
	UINT m_WMFindReplaceMsg;
	void ShowFindDialog();


	
private:
	//实现 IDM_FILE_OPEN 
	OPENFILENAMEW OpenFileName;
	wchar_t FileNameBuffer[MAX_PATH] = { 0 };
	bool OnOpenFile();
	bool LoadTextFile();
	HANDLE hOpenFile;   //打开的文件句柄



private:
	BOOL bUntitled;    //本地磁盘上是否有文件
	BOOL bSaved;       // 有没有保存




private:
	HINSTANCE m_hInstance; // 当前进程实例句柄
	HWND m_hMainWindow;  //主窗口句柄
	HMENU m_hMenuBar;   //菜单栏句柄
	HACCEL m_hAccelTable; //加速键表
	HWND m_hEditControl;
	HFONT m_hEditFont;
};