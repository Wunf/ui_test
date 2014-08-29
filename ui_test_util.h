#ifndef _UI_TEST_UTIL_H_
#define _UI_TEST_UTIL_H_

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

	void SetErrAcceptance(double errApt);

	cv::Mat TakeScreenShot();
	cv::Mat HWnd2Mat(HWND hwnd);
	void DisPlayImg(cv::Mat img);
	void BlinkUI(int times); // 根据当前鼠标位置选择UI
	cv::Rect FindUIRect(const char * uiImgName);
	cv::Rect MatchUI(HWND hWnd, const char * uiImgName);
	int AvoidUI(const char * uiImgName);
	void KeyDown(WORD vk);
	void KeyUp(WORD vk);
	void MouseClick();
	void Init();
	enum LogType{UTERROR, UTMESSAGE};
	void Log(LogType logtype, const char * message, const char * other = NULL);
	bool inline InRect(int x, int y, const RECT &rect);
}

#endif