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

// Paint function
cv::Rect UIRect;
RECT circle;
void Clear(HDC hdc, HWND hWnd);
void DrawRect(HDC hdc);
void DrawCircle(HDC hdc);
void RePaint();

// Run script function
DWORD WINAPI UiTestMainFunc(LPVOID script);

// Script interface
int LRunExe(lua_State * ls);                // lua func RunExe(exePath) return 0 if failed
int LClickBtn(lua_State * ls);				// lua func ClickBtn(wndName, imgName) return 0 if failed
int LDoubleClick(lua_State * ls);           // lua func DoubleClick(wndName, imgName) return 0 if failed
int LMouseMove(lua_State * ls);				// lua func MouseMove(wndName, imgName) return 0 if failed
int LPressKey(lua_State * ls);				// lua func PressKey(virtualKey) return 0 if failed
int LExpectUI(lua_State * ls);				// lua func ExpectUI(wndName, imgName) return 0 if failed
int LSleep(lua_State * ls);					// lua func Sleep(milliseconds) return nothing
int LSetErrAcceptance(lua_State * ls);		// lua func SetErrAcceptance(OpenCVMatchErrAcceptance) 0 ~ 5e8 return nothing

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

	// Create a thread to run script
	if(!lpCmdLine || !(*lpCmdLine))
	{
		MessageBox(NULL, "Need a lua script as parameter.", "Error", MB_OK);
		PostQuitMessage(0);
	}
	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, UiTestMainFunc, LPVOID(lpCmdLine), 0, &threadId );

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			// Close when script is over
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
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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
		DrawCircle(hdc);
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

void DrawCircle(HDC hdc)
{
	if((circle.right - circle.left) != 0)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255));
		HPEN hpen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		SelectObject(hdc, hBrush);
		SelectObject(hdc, hpen);
		Ellipse(hdc, circle.left, circle.top, circle.right, circle.bottom);
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
		return 1;
	}
}

int LClickBtn(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * btnImgName = lua_tostring(ls, 2);

	RePaint();

	UIRect = ui_test::MatchUI(wndName, btnImgName);

	HWND hwnd = FindWindow(NULL, wndName);
	if(UIRect.width)
	{		
		lua_pushinteger(l, UIRect.width);
		RePaint();
		Sleep(1000);
		int mouse_x = UIRect.x + UIRect.width / 2, mouse_y = UIRect.y + UIRect.height / 2;
		SetCursorPos(mouse_x, mouse_y);
		circle.left = mouse_x - 20;
		circle.right = mouse_x + 20;
		circle.top = mouse_y - 20;
		circle.bottom = mouse_y + 20;
		UIRect.width = 0;
		//SetForegroundWindow(hwnd);	
		Sleep(100);
		ui_test::MouseClick();
		RePaint();
		Sleep(1000);
		circle.right = circle.left;
	}
	else
		lua_pushinteger(l, 0);
	UIRect.width = 0;
	RePaint();
	return 1;
}

int LDoubleClick(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * btnImgName = lua_tostring(ls, 2);

	RePaint();

	UIRect = ui_test::MatchUI(wndName, btnImgName);
	HWND hwnd = FindWindow(NULL, wndName);
	if(UIRect.width)
	{		
		lua_pushinteger(l, UIRect.width);
		RePaint();
		Sleep(1000);
		
		int mouse_x = UIRect.x + UIRect.width / 2, mouse_y = UIRect.y + UIRect.height / 2;
		SetCursorPos(mouse_x, mouse_y);
		circle.left = mouse_x - 20;
		circle.right = mouse_x + 20;
		circle.top = mouse_y - 20;
		circle.bottom = mouse_y + 20;
		UIRect.width = 0;
		Sleep(100);
		//SetForegroundWindow(hwnd);
		ui_test::MouseClick();
		Sleep(100);
		ui_test::MouseClick(); // :) double click
		RePaint();
		Sleep(1000);
		circle.right = circle.left;
	}
	else
		lua_pushinteger(l, 0);

	UIRect.width = 0;
	RePaint();
	return 1;
}

int LMouseMove(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * uiImgName = lua_tostring(ls, 2);

	RePaint();

	UIRect = ui_test::MatchUI(wndName, uiImgName);

	HWND hwnd = FindWindow(NULL, wndName);
	//SetForegroundWindow(hwnd);
	if(UIRect.width)
	{		
		RePaint();
		Sleep(1000);
		SetCursorPos(UIRect.x + UIRect.width / 2, UIRect.y + UIRect.height / 2);
	}

	lua_pushinteger(l, UIRect.width);
	UIRect.width = 0;
	RePaint();
	return 1;
}

int LPressKey(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	WORD virtualKey = lua_tointeger(ls, 2);
	HWND hwnd = FindWindow(NULL, wndName);
	//SetForegroundWindow(hwnd);
	ui_test::PressKey(virtualKey);
	return 0;
}

int LExpectUI(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	const char * expUiImg = lua_tostring(ls, 2);

	RePaint();

	UIRect = ui_test::MatchUI(wndName, expUiImg);

	HWND hwnd = FindWindow(NULL, wndName);
	//SetForegroundWindow(hwnd);
	if(UIRect.width)
	{
		RePaint();
		Sleep(1000);
	}

	lua_pushinteger(l, UIRect.width);
	UIRect.width = 0;
	RePaint();
	return 1;
}

int LSleep(lua_State * ls)
{
	int n = static_cast<int>(lua_tointeger(ls, 1));
	Sleep(n);
	return 0;
}

int LSetErrAcceptance(lua_State * ls)
{
	float n = static_cast<float>(lua_tonumber(ls, 1));
	ui_test::SetErrAcceptance(n);
	return 0;
}

void RePaint()
{
	//SetForegroundWindow(tphWnd);
	InvalidateRect(tphWnd, NULL, TRUE);
	UpdateWindow(tphWnd);
}

DWORD WINAPI UiTestMainFunc(LPVOID script)
{
	// ./ui_test.log
	ui_test::Init();

	l = luaL_newstate();
	luaL_openlibs(l);
	if(l == NULL) 
		return 1; 
	int ret = luaL_loadfile(l, LPTSTR(script));
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
	lua_pushcfunction(l, LDoubleClick);        
	lua_setglobal(l, "DoubleClick");  
	lua_pushcfunction(l, LMouseMove);        
	lua_setglobal(l, "MouseMove");
	lua_pushcfunction(l, LPressKey);        
	lua_setglobal(l, "PressKey");
	lua_pushcfunction(l, LExpectUI);        
	lua_setglobal(l, "ExpectUI"); 
	lua_pushcfunction(l, LSleep);        
	lua_setglobal(l, "Sleep");
	lua_pushcfunction(l, LSetErrAcceptance);        
	lua_setglobal(l, "SetErrAcceptance");

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