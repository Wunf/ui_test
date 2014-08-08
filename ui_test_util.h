#ifndef _UI_TEST_UTIL_H_
#define _UI_TEST_UTIL_H_

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <opencv2/core/core.hpp>

namespace ui_test 
{
	// MSAA util
	// use AccessibleObjectFromWindow first
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
	void SetUiBlink();
	void SetNotUiBlink();

	void SetErrAcceptance(float errApt);

	cv::Mat TakeScreenShot();
	cv::Mat HWnd2Mat(HWND hwnd);
	void DisPlayImg(cv::Mat img);
	void BlinkUI(int times); // 根据当前鼠标位置选择UI
	cv::Rect FindUIRect(const char * uiImgName);
	cv::Rect MatchUI(const char * wndName, const char * uiImgName);
	void PressKey(WORD vk);
	void MouseClick();
	void Init();
	enum LogType{UTERROR, UTMESSAGE};
	void Log(LogType logtype, const char * message, const char * other = NULL);
}

#endif