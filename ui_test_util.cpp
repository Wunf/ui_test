#include "ui_test_util.h"
#include <comutil.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace ui_test 
{
	bool displaymatch = false;
	bool uiblink = false;
	float errAcceptance = 30000000.f;

	BOOL FindChildByInfo(
		IAccessible* paccParent,
		LPSTR szName, 
		LPSTR szRole,
		LPSTR szClass,
		IAccessible** paccChild,
		VARIANT* pvarChild)
	{
		HRESULT hr;
		long numChildren;
		unsigned long numFetched;
		VARIANT varChild;
		int index;
		IAccessible* pCAcc = NULL;
		IEnumVARIANT* pEnum = NULL;
		IDispatch* pDisp = NULL;
		BOOL found = false;
		char szObjName[256], szObjRole[256], szObjClass[256], szObjState[256];

		//�õ�����֧�ֵ�IEnumVARIANT�ӿ�
		hr = paccParent->QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

		if(pEnum)
			pEnum->Reset();

		//ȡ�ø���ӵ�еĿɷ��ʵ��ӵ���Ŀ
		paccParent->get_accChildCount(&numChildren);

		//�������Ƚ�ÿһ����ID���ҵ����֡���ɫ������������һ�µġ�
		for(index = 1; index <= numChildren && !found; index++)
		{
			pCAcc = NULL;
			// ���֧��IEnumVARIANT�ӿڣ��õ���һ����ID
			// �Լ����Ӧ�� IDispatch �ӿ�
			if (pEnum)
				hr = pEnum->Next(1, &varChild, &numFetched);
			else
			{
				//���һ�����ײ�֧��IEnumVARIANT�ӿڣ���ID�����������
				varChild.vt = VT_I4;
				varChild.lVal = index;
			}

			// �ҵ�����ID��Ӧ�� IDispatch �ӿ�
			if (varChild.vt == VT_I4)
			{
				//ͨ����ID��ŵõ���Ӧ�� IDispatch �ӿ�
				pDisp = NULL;
				hr = paccParent->get_accChild(varChild, &pDisp);
			}
			else
			{
				//�����֧��IEnumVARIANT�ӿڿ���ֱ�ӵõ���IDispatch �ӿ�
				pDisp = varChild.pdispVal;
			}

			// ͨ�� IDispatch �ӿڵõ��ӵ� IAccessible �ӿ� pCAcc
			if (pDisp)
			{
				hr = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
				hr = pDisp->Release();
			}

			// Get information about the child
			if (pCAcc)
			{
				//�����֧��IAccessible �ӿڣ���ô��ID����CHILDID_SELF
				VariantInit(&varChild);
				varChild.vt = VT_I4;
				varChild.lVal = CHILDID_SELF;

				*paccChild = pCAcc;
			}
			else
			{
				//����Ӳ�֧��IAccessible �ӿ�
				*paccChild = paccParent;
			}

			//�������в��ɷ���״̬��Ԫ��
			GetObjectState(
				*paccChild,
				&varChild,
				szObjState,
				sizeof(szObjState));
			if(NULL != strstr(szObjState, "unavailable"))
			{
				if(pCAcc)
					pCAcc->Release();
				continue;
			}
			// ͨ��get_accName�õ�Name
			GetObjectName(*paccChild, &varChild, szObjName, sizeof(szObjName));
			// ͨ��get_accRole�õ�Role
			GetObjectRole(*paccChild, &varChild, szObjRole, sizeof(szObjRole));
			// ͨ��WindowFromAccessibleObject��GetClassName�õ�Class
			GetObjectClass(*paccChild, szObjClass, sizeof(szObjClass));
			// ����ʵ�ִ���Ƚϼ򵥣�����Լ�������ɡ�
			// �����Щ�������������������ΪNULL
			if ((!szName || !strcmp(szName, szObjName)) &&
				(!szRole || !strcmp(szRole, szObjRole)) &&
				(!szClass || !strcmp(szClass, szObjClass)))
			{
				found = true;
				*pvarChild = varChild;
				break;
			}
			if(!found && pCAcc)
			{
				// ����εõ����ӽӿ�Ϊ���ݹ����
				found = FindChildByInfo(
					pCAcc,
					szName,
					szRole,
					szClass,
					paccChild,
					pvarChild);

				if(*paccChild != pCAcc)
					pCAcc->Release();
			}
		}//End for

		// Clean up
		if(pEnum)
			pEnum -> Release();

		return found;
	}

	UINT GetObjectState(IAccessible* pacc, VARIANT* pvarChild, LPSTR lpszState, UINT cchState)
	{
		HRESULT hr;
		VARIANT varRetVal;

		*lpszState = 0;

		VariantInit(&varRetVal);

		hr = pacc->get_accState(*pvarChild, &varRetVal);

		if (!SUCCEEDED(hr))
			return(0);

		DWORD dwStateBit;
		int cChars = 0;
		if (varRetVal.vt == VT_I4)
		{
			// Convert state flags to comma separated list.
			for (dwStateBit = STATE_SYSTEM_UNAVAILABLE; dwStateBit < STATE_SYSTEM_ALERT_HIGH; dwStateBit <<= 1)
			{
				if (varRetVal.lVal & dwStateBit)
				{
					cChars += GetStateTextA(dwStateBit, lpszState + cChars, cchState - cChars);
					*(lpszState + cChars++) = ',';
				}
			}
			if(cChars > 1)
				*(lpszState + cChars - 1) = '\0';
		}
		else if (varRetVal.vt == VT_BSTR)
		{
			WideCharToMultiByte(CP_ACP, 0, varRetVal.bstrVal, -1, lpszState,
				cchState, NULL, NULL);
		}

		VariantClear(&varRetVal);

		return(lstrlenA(lpszState));
	}

	void GetObjectName(IAccessible * pacc, VARIANT * varChild, char * szObjName, int size)
	{
		BSTR bsName;
		HRESULT hr;
		if(S_OK == (hr = pacc->get_accName(*varChild, &bsName)))
		{
			_bstr_t bstr;
			if(bsName)
				bstr = bsName;
			else
				bstr = "";
			strcpy_s(szObjName, size, bstr);
		}
	}

	void GetObjectRole(IAccessible * pacc, VARIANT * varChild, char * szObjRole, int size)
	{
		DWORD roleId;
		if (pacc == NULL)
		{
			return;    
		}
		VARIANT varResult;
		HRESULT hr = pacc->get_accRole(*varChild, &varResult);
		if ((hr == S_OK) && (varResult.vt == VT_I4))
		{
			roleId = varResult.lVal;
			// Get the string. 
			GetRoleText(roleId, szObjRole, size);
		}
	}

	void GetObjectClass(IAccessible * pacc, char * szObjClass, int size)
	{
		HWND tempWnd;
		HRESULT hr;
		if(S_OK == (hr = WindowFromAccessibleObject(pacc, &tempWnd)))
			GetClassName(tempWnd, szObjClass, size);
	}

	void SetDisplayMatch()
	{
		displaymatch = true;
	}

	void SetNotDisplayMatch()
	{
		displaymatch = false;
	}

	void SetUiBlink()
	{
		uiblink = true;
	}

	void SetNotuiBlink()
	{
		uiblink = false;
	}

	void SetErrAcceptance( float errApt )
	{
		errAcceptance = errApt;
	}

	cv::Mat TakeScreenShot()
	{
		HDC hwindowDC, hwindowCompatibleDC;

		int height, width, srcheight, srcwidth;
		HBITMAP hbwindow;
		cv::Mat src;
		BITMAPINFOHEADER  bi;

		hwindowDC = GetDC(NULL);
		hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
		SetStretchBltMode(hwindowCompatibleDC,COLORONCOLOR);  

		RECT windowsize;    
		windowsize.left = 0;
		windowsize.right = 0;
		windowsize.right = GetSystemMetrics(SM_CXSCREEN);
		windowsize.bottom = GetSystemMetrics(SM_CYSCREEN);

		srcheight = windowsize.bottom;
		srcwidth = windowsize.right;
		height = windowsize.bottom;  //change this to whatever size you want to resize to
		width = windowsize.right;

		src.create(height,width,CV_8UC4);

		// create a bitmap
		hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
		bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
		bi.biWidth = width;    
		bi.biHeight = -height;  //this is the line that makes it draw upside down or not
		bi.biPlanes = 1;    
		bi.biBitCount = 32;    
		bi.biCompression = BI_RGB;    
		bi.biSizeImage = 0;  
		bi.biXPelsPerMeter = 0;    
		bi.biYPelsPerMeter = 0;    
		bi.biClrUsed = 0;    
		bi.biClrImportant = 0;

		SelectObject(hwindowCompatibleDC, hbwindow);
		StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); 
		GetDIBits(hwindowCompatibleDC,hbwindow,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS); 

		DeleteObject (hbwindow); 
		DeleteDC(hwindowCompatibleDC); 

		cv::cvtColor(src, src, CV_RGBA2RGB, 3);

		return src;
	}

	cv::Mat HWnd2Mat(HWND hwnd)
	{
		HDC hwindowDC, hwindowCompatibleDC;

		int height, width, srcheight, srcwidth;
		HBITMAP hbwindow;
		cv::Mat src;
		BITMAPINFOHEADER  bi;

		hwindowDC = GetDC(hwnd);
		hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
		SetStretchBltMode(hwindowCompatibleDC,COLORONCOLOR);  

		RECT windowsize;    
		GetClientRect(hwnd, &windowsize);

		srcheight = windowsize.bottom;
		srcwidth = windowsize.right;
		height = windowsize.bottom;  //change this to whatever size you want to resize to
		width = windowsize.right;

		src.create(height,width,CV_8UC4);

		// create a bitmap
		hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
		bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
		bi.biWidth = width;    
		bi.biHeight = -height;  //this is the line that makes it draw upside down or not
		bi.biPlanes = 1;    
		bi.biBitCount = 32;    
		bi.biCompression = BI_RGB;    
		bi.biSizeImage = 0;  
		bi.biXPelsPerMeter = 0;    
		bi.biYPelsPerMeter = 0;    
		bi.biClrUsed = 0;    
		bi.biClrImportant = 0;

		SelectObject(hwindowCompatibleDC, hbwindow);
		StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); 
		GetDIBits(hwindowCompatibleDC,hbwindow,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS); 

		DeleteObject (hbwindow); 
		DeleteDC(hwindowCompatibleDC); 
		ReleaseDC(hwnd, hwindowDC);

		cv::cvtColor(src, src, CV_RGBA2RGB, 3);

		return src;
	}

	void DisPlayImg(cv::Mat img)
	{	
		cv::namedWindow("img", CV_WINDOW_NORMAL);
		cv::setWindowProperty("img", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		cv::imshow("img", img);
		cv::waitKey(30);
		Sleep(1000);
		cv::destroyWindow("img");
	}

	void BlinkUI(int times)
	{
		POINT point;
		GetCursorPos(&point);
		HWND uihwnd = WindowFromPoint(point);
		HWND deskhwnd = GetDesktopWindow();
		HDC deskdc = GetWindowDC(deskhwnd);
		RECT uirect;
		GetWindowRect(uihwnd, &uirect);
		HGDIOBJ newpen = CreatePen(PS_INSIDEFRAME, 6, RGB(0, 0, 0));
		SelectObject(deskdc, newpen);
		while(times--)
		{
			Rectangle(deskdc, uirect.left, uirect.top, uirect.right, uirect.bottom);	
			Sleep(100);
		}
		DeleteObject(newpen);
		//DrawRectangleOnTransparent(uihwnd, uirect);
		//Sleep(2000);
		ReleaseDC(deskhwnd, deskdc);
	}

	cv::Rect FindUIRect(const char * uiImgName)
	{
		cv::Rect uiRect;
		cv::Mat templ = cv::imread(uiImgName);
		cv::Mat wndImg = TakeScreenShot();

		if(templ.data == NULL)
		{
			Log(UTERROR, "Load ui templ error:", uiImgName);
			return uiRect;
		}
		cv::Mat result;
		try{
			cv::matchTemplate(wndImg, templ, result, CV_TM_SQDIFF);
		}
		catch(cv::Exception & e)
		{
			Log(UTERROR, "OpenCV matchTemplate error.");
			Log(UTERROR, e.msg.c_str());
			return uiRect;
		}
		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		if(minVal >= errAcceptance)
			return uiRect;
		uiRect = cv::Rect(minLoc.x, minLoc.y, templ.cols, templ.rows);	

		if(displaymatch)
		{
			cv::rectangle(wndImg, uiRect, cv::Scalar(0.0f, 0.0f, 255.0f, 0.0f), 2);
			DisPlayImg(wndImg);
		}

		return uiRect;
	}

	cv::Rect MatchUI(const char * uiImgName)
	{
		cv::Rect UIRect;
		int n = 10; // ʧ�����Դ���
		while(n--)
		{
			// ƥ��UI
			//SetForegroundWindow(hwnd);
			//Sleep(200);
			UIRect = FindUIRect(uiImgName);
			if(UIRect.width)
			{
				Log(UTMESSAGE, "Ui matched:", uiImgName);
				break;
			}
			else
			{
				Log(UTMESSAGE, "Tried to match ui failed, retrying...:", uiImgName);
				Sleep(1000);
			}
		}

		if(!UIRect.width)
		{
			Log(UTMESSAGE, "Expected ui doesn't match while matching img:", uiImgName);
			Log(UTMESSAGE, "Saved current screen to screenshot.bmp.");
			cv::imwrite("screenshot.bmp", TakeScreenShot());
		}

		return UIRect;
	}

	void MouseClick()
	{
		// �������
		INPUT Input = {0};													

		Input.type        = INPUT_MOUSE;									
		Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;							
		SendInput(1, &Input, sizeof(INPUT));								

		ZeroMemory(&Input,sizeof(INPUT));									
		Input.type        = INPUT_MOUSE;									
		Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;								
		SendInput(1, &Input, sizeof(INPUT));
	}

	void PressKey(WORD vk)
	{
		INPUT Input = {0};

		Input.type = INPUT_KEYBOARD;
		Input.ki.wVk = vk;
		SendInput(1, &Input, sizeof(INPUT));

		ZeroMemory(&Input,sizeof(INPUT));
		Input.type = INPUT_KEYBOARD;
		Input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &Input, sizeof(INPUT));
		//Sleep(100);
	}

	void Init()
	{
		std::ofstream ofs("ui_test.log");
		ofs << "[MSG] Starting a new log..." << std::endl;
		ofs << "[MSG] lua usage:" << std::endl;
		ofs << "[MSG] lua func RunExe(exePath) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func ClickBtn(imgName, expectUI) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func DoubleClick(imgName, expectUI) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func MouseMove(imgName) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func PressKey(virtualKey) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func ExpectUI(imgName) return 0 if failed" << std::endl;
		ofs << "[MSG] lua func Sleep(milliseconds) return nothing" << std::endl;
		ofs << "[MSG] lua func SetErrAcceptance(OpenCVMatchErrAcceptance) 0 ~ 5e8 return nothing" << std::endl;
		ofs.close();
	}

	void Log(LogType logtype, const char * message, const char * other /*= NULL*/)
	{
		std::string line;
		if(logtype == UTERROR)
			line = "[ERR]";
		else
			line = "[MSG]";
		std::string msg(message);
		line = line + " " + msg;
		if(other)
		{
			std::string oth(other);
			line = line + " " + oth;
		}
		std::ofstream ofs;
		ofs.open("ui_test.log", std::ios::app);
		ofs << line << std::endl;
		ofs.close();
	}
}