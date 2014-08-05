#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <opencv2/core/core.hpp>

namespace ui_test 
{
	// MSAA util
	BOOL FindChildByInfo(
		IAccessible* paccParent, 
		LPSTR szName, 
		LPSTR szRole, 
		LPSTR szClass, 
		IAccessible** paccChild, 
		VARIANT* pvarChild);

	UINT GetObjectState(
		IAccessible* pacc, 
		VARIANT* pvarChild, 
		LPSTR lpszState, 
		UINT cchState);

	void GetObjectName(IAccessible * pacc, VARIANT * varChild, char * szObjName, int size);
	void GetObjectRole(IAccessible * pacc, VARIANT * varChild, char * szObjRole, int size);
	void GetObjectClass(IAccessible * pacc, char * szObjClass, int size);
	// MSAA util end

	void SetDisplayMatch();
	void SetNotDisplayMatch();
	cv::Mat TakeScreenShot();
	cv::Mat HWnd2Mat(HWND hwnd);
	void DisPlayImg(cv::Mat img);
	cv::Rect FindUIRect(char * uiImgName);
	void MouseClick(HWND hwnd, DWORD x, DWORD y);
	BOOL ClickBtn(char * wndName, char * btnImgName);
	BOOL ExpectUI(char * wndName, char * expUiImg);
}