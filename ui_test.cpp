#include "ui_test_util.h"
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <iostream>
extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
};

#define MAX_LOADSTRING 100
#define IDM_EXIT 105

// Global Variables:
HINSTANCE tphInst;								// current instance
HWND tphWnd;                                       // window handler
TCHAR szTitle[] = "Transparent";					// The title bar text
TCHAR szWindowClass[] = "Transparent";			// the main window class name
lua_State * l;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void Clear(HDC hdc, HWND hWnd);
void DrawRect(HDC hdc);
DWORD WINAPI UiTestMainFunc(LPVOID);
int LRunExe(lua_State * ls);
int LClickBtn(lua_State * ls);
int LExpectUI(lua_State * ls);
int LSleep(lua_State * ls);
void RePaint();

cv::Rect UIRect;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, NULL);

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, UiTestMainFunc, NULL, 0, &threadId );

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			DWORD code;
			GetExitCodeThread(hThread, &code);
			if(code == 1)
				PostQuitMessage(0);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= szWindowClass;
	wcex.lpszMenuName	= NULL;
	wcex.hIcon			= NULL;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	tphInst = hInstance;

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TOPMOST | WS_EX_LAYERED);
	BOOL bres = SetLayeredWindowAttributes(hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);
	if(!bres)
		return FALSE;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	tphWnd = hWnd;

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;	
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Clear(hdc, hWnd);
		DrawRect(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Clear(HDC hdc, HWND hWnd)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255));
	RECT rect;
	GetClientRect(hWnd, &rect);
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

void DrawRect(HDC hdc)
{
	if(UIRect.width)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255));
		HPEN hpen = CreatePen(PS_SOLID, 6, RGB(255, 0, 0));
		SelectObject(hdc, hBrush);
		SelectObject(hdc, hpen);
		Rectangle(hdc, UIRect.x, UIRect.y, UIRect.x + UIRect.width, UIRect.y + UIRect.height);
		DeleteObject(hBrush);
		DeleteObject(hpen);
	}
}

int LRunExe(lua_State * ls)
{
	const char * exeName = lua_tostring(ls, 1);
	UINT res = WinExec(exeName, SW_SHOW);
	if(res > 31)
	{
		lua_pushinteger(l, 1);
		return 1;
	}
	else
	{
		lua_pushinteger(l, 0);
		return 0;
	}
}

int LClickBtn(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * btnImgName = lua_tostring(ls, 2);

	RePaint();

	// 查找窗口
	HWND hwnd = FindWindow(NULL, wndName);
	if(!hwnd)
	{
		ui_test::Log(ui_test::UTERROR, "Cannot find window:", wndName);
		lua_pushinteger(l, 0);
		return 0;
	}

	// 匹配UI
	SetForegroundWindow(hwnd);
	UIRect = ui_test::FindUIRect(btnImgName);
	if(UIRect.width)
	{
		RePaint();
		ui_test::Log(ui_test::UTMESSAGE, "Button matched:", btnImgName);
		Sleep(1000);
	}
	else
	{
		ui_test::Log(ui_test::UTERROR, "Cannot match button:", btnImgName);
		lua_pushinteger(l, 0);
		return 0;
	}

	// 移动鼠标
	SetForegroundWindow(hwnd);
	SetCursorPos(UIRect.x + UIRect.width / 2, UIRect.y + UIRect.height / 2);

	//点击
	UIRect.width = 0;
	RePaint();
	ui_test::MouseClick();

	lua_pushinteger(l, 1);
	return 1;
}

int LExpectUI(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * expUiImg = lua_tostring(ls, 2);

	RePaint();

	int flag = 0;
	int n = 10; // 失败重试次数
	while(n--)
	{
		// 查找窗口
		HWND hwnd = FindWindow(NULL, wndName);
		if(!hwnd)
		{
			ui_test::Log(ui_test::UTMESSAGE, "Finding window...:", wndName);
			Sleep(1000);
			continue;
		}

		// 匹配UI
		SetForegroundWindow(hwnd);
		UIRect = ui_test::FindUIRect(expUiImg);
		if(UIRect.width)
		{
			flag = 1;
			RePaint();
			ui_test::Log(ui_test::UTMESSAGE, "Ui matched:", expUiImg);
			Sleep(1000);
			break;
		}
	}

	if(!flag)
	{
		ui_test::Log(ui_test::UTERROR, "Expected ui doesn't match. Maybe a bug occurred while matching:", expUiImg);
		ui_test::Log(ui_test::UTERROR, "Saved current screen to bug_screen.bmp.");
		cv::imwrite("bug_screen.bmp", ui_test::TakeScreenShot());
	}

	lua_pushinteger(l, flag);
	return flag;
}

int LSleep(lua_State * ls)
{
	int n = lua_tointeger(ls, 1);
	Sleep(n);
	return 1;
}

void RePaint()
{
	SetForegroundWindow(tphWnd);
	InvalidateRect(tphWnd, NULL, TRUE);
	UpdateWindow(tphWnd);
}

DWORD WINAPI UiTestMainFunc(LPVOID)
{
	// ./ui_test.log
	ui_test::Init();

	l = luaL_newstate();
	if(l == NULL) 
		return 1; 
	int ret = luaL_loadfile(l, "action.lua");
	if(ret != 0) 
	{
		const char *msg = lua_tostring(l, -1);
		Log(ui_test::UTERROR, msg);
		lua_pop(l, 1);
		return 1;
	}

	lua_pushcfunction(l, LRunExe);        
	lua_setglobal(l, "RunExe");  
	lua_pushcfunction(l, LClickBtn);        
	lua_setglobal(l, "ClickBtn");          
	lua_pushcfunction(l, LExpectUI);        
	lua_setglobal(l, "ExpectUI"); 
	lua_pushcfunction(l, LSleep);        
	lua_setglobal(l, "Sleep");

	ret = lua_pcall(l, 0, 0, 0) ;
	if(ret != 0) 
	{
		const char *msg = lua_tostring(l, -1);
		Log(ui_test::UTERROR, msg);
		lua_pop(l, 1);
		return 1;
	}

	lua_close(l) ;
	Log(ui_test::UTMESSAGE, "Finished.");
	return 1;
}