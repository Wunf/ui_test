#include "stdafx.h"
#include "ui_test_util.h"
#include "ui_test.h"

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

	// Create a console to present message
	if(AllocConsole())
		freopen("CONOUT$", "w", stdout);
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

	system("pause");
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
	wcex.hIcon			= LoadIcon(tphInst, "16.ico");
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

void DrawRect(cv::Rect rect)
{
	UIRect = rect;
	circle.left = circle.right;
	SetForegroundWindow(tphWnd);
	RePaint();
	Sleep(1000);
	UIRect.width = 0;
	RePaint();
	SetForegroundWindow(curWnd);
	Sleep(1000);
}

void DrawCircle(int x, int y)
{
	circle.left = x - 20;
	circle.right = x + 20;
	circle.top = y - 20;
	circle.bottom = y + 20;
	UIRect.width = 0;
	SetForegroundWindow(tphWnd);
	RePaint();
	Sleep(1000);
	circle.right = circle.left;
	RePaint();
	SetForegroundWindow(curWnd);
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
	const char * btnImgName = lua_tostring(ls, 1);
	const char * expectedUI = lua_tostring(ls, 2);

//	int flag = ClickBtn(btnImgName, expectedUI);
	//lua_pushinteger(l, flag);
	return 1;
}

int LDoubleClick(lua_State * ls)
{
	const char * btnImgName = lua_tostring(ls, 1);
	const char * expectedUI = lua_tostring(ls, 2);

	RePaint();

	UIRect = ui_test::MatchUI(curWnd, btnImgName);
	if(UIRect.width)
	{	
		RePaint();
		Sleep(1000);
		int mouse_x = UIRect.x + UIRect.width / 2, mouse_y = UIRect.y + UIRect.height / 2;
		int n = 5;
		while(n--)
		{	
			SetCursorPos(mouse_x, mouse_y);
			UIRect.width = 0;
			ui_test::MouseClick();
			Sleep(100);
			ui_test::MouseClick(); // :) double click
			circle.left = mouse_x - 20;
			circle.right = mouse_x + 20;
			circle.top = mouse_y - 20;
			circle.bottom = mouse_y + 20;
			RePaint();
			Sleep(1000);
			circle.left = circle.right;
			RePaint();
			UIRect = ui_test::MatchUI(curWnd, expectedUI);
			if(UIRect.width)
				break;
			else
				ui_test::Log(ui_test::UTMESSAGE, "Clicking maybe failed, retrying...");
		}
		if(n != -1)
			lua_pushinteger(l, 1);	
		else
			lua_pushinteger(l, 0);	

	}
	else
		lua_pushinteger(l, 0);
	circle.right = circle.left;
	UIRect.width = 0;
	RePaint();
	return 1;
}

int LMouseMove(lua_State * ls)
{
	const char * uiImgName = lua_tostring(ls, 1);

	RePaint();

	UIRect = ui_test::MatchUI(curWnd, uiImgName);

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
	WORD virtualKey = lua_tointeger(ls, 1);
	ui_test::KeyDown(virtualKey);
	ui_test::KeyUp(virtualKey);
	return 0;
}

int LExpectUI(lua_State * ls)
{
	const char * expUiImg = lua_tostring(ls, 1);
	int r = Expect(expUiImg);
	lua_pushinteger(l, r);
	return 1;
}

int LExpectWnd(lua_State * ls)
{
	const char * wndName = lua_tostring(ls, 1);
	int r = ExpectWnd(wndName);
	lua_pushinteger(l, r);
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
	double n = static_cast<double>(lua_tonumber(ls, 1));
	SetErrAcceptance(n);
	return 0;
}

int ClickBtn(const char * btnImgName, int x, int y, const char * expectedUI, action act/* = EXPRCT*/)
{
	cv::Rect rect = ui_test::MatchUI(curWnd, btnImgName);
	if(rect.width)
	{	
		DrawRect(rect);
		int mouse_x = UIRect.x + x, mouse_y = UIRect.y + y;
		int n = 5;
		while(n--)
		{	
			SetCursorPos(mouse_x, mouse_y);
			ui_test::MouseClick();
			DrawCircle(mouse_x, mouse_y);
			if(act == EXPRCT)
			{
				rect = ui_test::MatchUI(curWnd, expectedUI);
				if(rect.width)
				{
					DrawRect(rect);
					break;
				}
				else
					ui_test::Log(ui_test::UTMESSAGE, "Clicking maybe failed, retrying...");
			}
			else if(act == AVOID)
			{
				if(!ui_test::AvoidUI(expectedUI))
					ui_test::Log(ui_test::UTMESSAGE, "Clicking maybe failed, retrying...");
				else
					break;
			}
		}
		if(n != -1)
			return 1;	
		else
			return 0;	

	}
	else
		return 0;
}

int Click(const char * wndName, int x, int y, const char * uiFile, action act/* = EXPRCT*/)
{
	HWND hWnd = FindWindow(NULL, wndName);
	RECT wndRect;
	GetWindowRect(hWnd, &wndRect);
	int mouse_x = wndRect.left + x, mouse_y = wndRect.top + y;
	int n = 5; //  ß∞‹÷ÿ ‘¥Œ ˝
	while(n--)
	{	
		SetCursorPos(mouse_x, mouse_y);
		ui_test::MouseClick();
		DrawCircle(mouse_x, mouse_y);
		if(act == EXPRCT)
		{
			cv::Rect rect = ui_test::MatchUI(curWnd, uiFile);
			if(rect.width)
			{
				DrawRect(rect);
				break;
			}
			else
				ui_test::Log(ui_test::UTMESSAGE, "Clicking maybe failed, retrying...");
		}
		else if(act == AVOID)
		{
			if(!ui_test::AvoidUI(uiFile))
				ui_test::Log(ui_test::UTMESSAGE, "Clicking maybe failed, retrying...");
			else
				break;
		}
	}
	if(n != -1)
		return 1;	
	else
		return 0;
}

int Expect(const char * uiFile)
{
	cv::Rect rect = ui_test::MatchUI(curWnd, uiFile);
	if(rect.width)
	{
		DrawRect(rect);
		return 1;
	}
	else
		return 0;
}

int ExpectWnd(const char * wndName)
{
	if(!wndName)
	{
		curWnd = GetDesktopWindow();
		return 1;
	}

	HWND hWnd;
	int n = 5;
	while(n--)
	{
		hWnd = FindWindow(NULL, wndName);
		if(!hWnd)
		{
			ui_test::Log(ui_test::UTMESSAGE, "Cannot find window, retrying...:", wndName);
			Sleep(1000);
		}
		else
			break;
	}
	if(n != -1)
	{
		curWnd = hWnd;
		return 1;
	}
	else
	{
		ui_test::Log(ui_test::UTERROR, "Cannot find window:", wndName);
		return 0;
	}
}

void SetErrAcceptance(double errAcpt)
{
	ui_test::SetErrAcceptance(errAcpt);
}

void RestoreWindow(const char * wndName)
{
	HWND hWnd = FindWindow(NULL, wndName);
	ShowWindow(hWnd, SW_RESTORE);
}

void RePaint()
{
	SetWindowPos(tphWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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

	/*lua_pushcfunction(l, LRunExe);        
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
	lua_pushcfunction(l, LExpectWnd);        
	lua_setglobal(l, "ExpectWnd"); 
	lua_pushcfunction(l, LSleep);        
	lua_setglobal(l, "Sleep");
	lua_pushcfunction(l, LSetErrAcceptance);        
	lua_setglobal(l, "SetErrAcceptance");*/

	int ret = luaL_dofile(l, LPTSTR(script));
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