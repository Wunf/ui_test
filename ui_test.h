#ifndef _UI_TEST_H_
#define _UI_TEST_H_

#include "stdafx.h"

#define MAX_LOADSTRING 100
#define IDM_EXIT 105

// Global Variables:
HINSTANCE tphInst;								// current instance
HWND tphWnd;                                       // window handler
TCHAR szTitle[] = "GUI_TEST";					// The title bar text
TCHAR szWindowClass[] = "GUI_TEST";			// the main window class name
HWND curWnd;
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
void DrawRect(cv::Rect rect);
void DrawCircle(int x, int y);
void RePaint();

// Run script function
DWORD WINAPI UiTestMainFunc(LPVOID script);

// Script interface
int LRunExe(lua_State * ls);                // lua func RunExe(exePath) return 0 if failed
int LClickBtn(lua_State * ls);				// lua func ClickBtn(imgName, expectUI) return 0 if failed
int LDoubleClick(lua_State * ls);           // lua func DoubleClick(imgName, expectUI) return 0 if failed
int LMouseMove(lua_State * ls);				// lua func MouseMove(imgName) return 0 if failed
int LPressKey(lua_State * ls);				// lua func PressKey(virtualKey) return 0 if failed
int LExpectUI(lua_State * ls);				// lua func ExpectUI(imgName) return 0 if failed
int LExpectWnd(lua_State * ls);				// lua func ExpectWnd(wndName) return 0 if failed
int LSleep(lua_State * ls);					// lua func Sleep(milliseconds) return nothing
int LSetErrAcceptance(lua_State * ls);		// lua func SetErrAcceptance(OpenCVMatchErrAcceptance) 0 ~ 5e8 return nothing

enum action
{
	EXPRCT = 1,
	AVOID = 2
};

extern "C" 
{
	__declspec(dllexport) int ClickBtn(const char * btnImgName, int x, int y, const char * expectedUI, action act = EXPRCT);
	__declspec(dllexport) int Click(const char * wndName, int x, int y, const char * uiFile, action act = EXPRCT);
	__declspec(dllexport) int Expect(const char * uiFile);
	__declspec(dllexport) int ExpectWnd(const char * wndName);
	__declspec(dllexport) void SetErrAcceptance(double errAcpt);
	__declspec(dllexport) void RestoreWindow(const char * wndName);
};

#endif