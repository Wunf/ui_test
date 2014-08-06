#include "transpWnd.h"
#define IDM_EXIT 105
#define IDC_WIN32_TEST 109

namespace ui_test
{

	TranspWnd::TranspWnd()
	{

	}

	TranspWnd::~TranspWnd()
	{

	}

	TCHAR * TranspWnd::m_szTitle;					
	TCHAR * TranspWnd::m_szWindowClass;
	HWND TranspWnd::m_hWnd;

	DWORD WINAPI TranspWnd::Run(LPVOID phInstance)
	{
		MSG msg;
		HACCEL hAccelTable;
		HINSTANCE hInstance = *(HINSTANCE*)phInstance;

		// Initialize global strings
		m_szTitle = "TranpWnd";
		m_szWindowClass = "TranpWnd";
		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance (hInstance, 1))
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, NULL);

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (DWORD) msg.wParam;
	}

	void TranspWnd::ShutDown()
	{
		PostQuitMessage(0);
	}

	ATOM TranspWnd::MyRegisterClass(HINSTANCE hInstance)
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
		wcex.lpszClassName	= m_szWindowClass;
		wcex.lpszMenuName	= NULL;
		wcex.hIcon			= NULL;
		wcex.hIconSm		= NULL;

		return RegisterClassEx(&wcex);
	}

	BOOL TranspWnd::InitInstance(HINSTANCE hInstance, int nCmdShow)
	{
		HWND hWnd = CreateWindow(m_szWindowClass, m_szTitle, WS_POPUP,
			0, 0, 500, 500, NULL, NULL, hInstance, NULL);

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
		m_hWnd = hWnd;
		return TRUE;
	}

	LRESULT CALLBACK TranspWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int wmId, wmEvent;	
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
			Clear();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void TranspWnd::RunInAnotherThread()
	{
		DWORD threadId;
		HANDLE hthread = CreateThread(NULL,
			0,
			Run,
			(LPVOID)&m_hInst,
			0,
			&threadId);
	}

	void TranspWnd::Clear()
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(m_hWnd, &ps);
		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255));
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);
		EndPaint(m_hWnd, &ps);
	}

	void TranspWnd::DrawRect( RECT rect )
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(m_hWnd, &ps);
		HBRUSH hBrush = CreateSolidBrush(RGB(255,0,255));
		HPEN hpen = CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
		SelectObject(hdc, hBrush);
		SelectObject(hdc, hpen);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		DeleteObject(hBrush);
		DeleteObject(hpen);
		EndPaint(m_hWnd, &ps);
	}

}