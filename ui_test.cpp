#include "ui_test_util.h"
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <iostream>

#define MAX_LOADSTRING 100
#define IDM_EXIT 105

// Global Variables:
HINSTANCE tphInst;								// current instance
HWND tphWnd;                                       // window handler
TCHAR szTitle[] = "Transparent";					// The title bar text
TCHAR szWindowClass[] = "Transparent";			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void Clear(HDC hdc, HWND hWnd);
void DrawRect(HDC hdc);
DWORD WINAPI UiTestMainFunc(LPVOID);
BOOL ClickBtn(char * wndName, char * btnImgName);
BOOL ExpectUI(char * wndName, char * expUiImg);
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

BOOL ClickBtn(char * wndName, char * btnImgName)
{
	RePaint();

	// 查找窗口
	HWND hwnd = FindWindow(NULL, wndName);
	if(!hwnd)
	{
		ui_test::Log(ui_test::UTERROR, "Cannot find window:", wndName);
		return FALSE;
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
		return FALSE;
	}

	// 移动鼠标
	SetForegroundWindow(hwnd);
	SetCursorPos(UIRect.x + UIRect.width / 2, UIRect.y + UIRect.height / 2);

	//点击
	UIRect.width = 0;
	RePaint();
	ui_test::MouseClick();

	return TRUE;
}

BOOL ExpectUI(char * wndName, char * expUiImg)
{
	RePaint();

	BOOL flag = FALSE;
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
			flag = TRUE;
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

	return flag;
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

	int n = 1;
	while(n--)
	{
		WinExec("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe", SW_SHOW);
		if(!ExpectUI("网易游戏平台 安装", "templ/expected_newerverfound.bmp"))
			break;
		if(!ClickBtn("网易游戏平台 安装", "templ/ok_btn_templ.bmp"))
			break;
		if(!ExpectUI("网易游戏平台 安装", "templ/expected_install.bmp"))
			break;
		if(!ClickBtn("网易游戏平台 安装", "templ/install_btn_templ.bmp"))
			break;
		if(!ExpectUI("网易游戏平台 安装", "templ/expected_installing.bmp"))
			break;
		Sleep(8000);
		if(!ExpectUI("NGP", "templ/expected_ngp.bmp"))
			break;
		if(!ClickBtn("NGP", "templ/ngp_close_btn_templ.bmp"))
			break;
	}

	Log(ui_test::UTMESSAGE, "Finished.");
	return 1;
}