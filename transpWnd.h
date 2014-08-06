#ifndef _TRANSP_WND_H_
#define _TRANSP_WND_H_


#include <windows.h>

namespace ui_test
{
	class TranspWnd
	{
	public:
		TranspWnd();
		~TranspWnd();

		void RunInAnotherThread();
		void ShutDown();
		static void DrawRect(RECT);
		static void Clear();
		static DWORD WINAPI Run(LPVOID phInstance);

	private:
		HINSTANCE m_hInst;								// current instance
		static HWND m_hWnd;                                   // current window handler
		static TCHAR * m_szTitle;					// The title bar text
		static TCHAR * m_szWindowClass;			// the main window class name

		static ATOM MyRegisterClass(HINSTANCE hInstance);
		static BOOL InitInstance(HINSTANCE, int);
		static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
		
	};
}

#endif