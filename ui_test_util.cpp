#include "ui_test_util.h"
#include <comutil.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

namespace ui_test 
{
	bool displaymatch = false;
	bool buttonblink = true;
	const float errAcceptance = 20000000.f;

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

	void SetButtonBlink()
	{
		buttonblink = true;
	}

	void SetNotButtonBlink()
	{
		buttonblink = false;
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
		/*cv::namedWindow("img", CV_WINDOW_AUTOSIZE);
		cv::moveWindow("img", 0, 0);
		cv::setWindowProperty("img", CV_WINDOW_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		cv::imshow("img", img);
		HWND hwnd = FindWindow(0, "img");
		unsigned int flags = (SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
		flags &= ~SWP_NOSIZE;
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int w = img.cols;
		unsigned int h = img.rows;
		SetWindowPos(hwnd, HWND_NOTOPMOST, x, y, w, h, flags);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST);
		ShowWindow(hwnd, SW_SHOW);*/
		cv::waitKey(30);
		Sleep(1000);
		cv::destroyWindow("img");
	}

	cv::Rect FindUIRect(char * uiImgName)
	{
		cv::Rect uiRect;
		cv::Mat templ = cv::imread(uiImgName);
		cv::Mat wndImg = TakeScreenShot();

		if(templ.data == NULL)
		{
			std::cerr << "load ui templ error." << std::endl;
			return uiRect;
		}
		cv::Mat result;
		try{
			cv::matchTemplate(wndImg, templ, result, CV_TM_SQDIFF);
		}
		catch(...)
		{
			return uiRect;
		}
		if(result.data == NULL)
		{
			std::cerr << "templ match error." << std::endl;
			return uiRect;
		}
		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
		cv::imwrite("test1.bmp", wndImg);
		cv::imwrite("test2.bmp", templ);
		if(minVal >= errAcceptance)
		{
			std::cerr << "Try to match ui failed..." << std::endl;
			return uiRect;
		}
		uiRect = cv::Rect(minLoc.x, minLoc.y, templ.cols, templ.rows);

		cv::rectangle(wndImg, uiRect, cv::Scalar(0.0f, 0.0f, 255.0f, 0.0f), 2);

		if(displaymatch)
		{
			DisPlayImg(wndImg);
		}

		return uiRect;
	}

	void MouseClick(HWND hwnd, DWORD x, DWORD y)
	{
		// �ƶ����
		SetForegroundWindow(hwnd);
		//Sleep(100);
		SetCursorPos(x, y);

		// ����˸��
		if(buttonblink)
		{
			POINT point;
			point.x = x;
			point.y = y;
			HWND uihwnd = WindowFromPoint(point);
			HWND deskhwnd = GetDesktopWindow();
			HDC deskdc = GetWindowDC(deskhwnd);
			RECT uirect;
			GetWindowRect(uihwnd, &uirect);
			HGDIOBJ newpen = CreatePen(PS_INSIDEFRAME, 6, RGB(0, 0, 0));
			SelectObject(deskdc, newpen);
			int n = 5;
			while(n--)
			{
				Rectangle(deskdc, uirect.left, uirect.top, uirect.right, uirect.bottom);	
				Sleep(100);
			}
			DeleteObject(newpen);
			//DrawRectangleOnTransparent(uihwnd, uirect);
			//Sleep(2000);
			ReleaseDC(deskhwnd, deskdc);
		}

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

	BOOL ClickBtn(char * wndName, char * btnImgName)
	{
		HWND hwnd = FindWindow(NULL, wndName);
		if(!hwnd)
		{
			std::cerr << "Cannot find window." << std::endl;
			return FALSE;
		}

		SetForegroundWindow(hwnd);
		//Sleep(100);
		cv::Rect btnRect = FindUIRect(btnImgName);
		if(btnRect.width)
			std::cout << "Button matched!" << std::endl;
		else
		{
			std::cout << "Cannot match button." << std::endl;
			return FALSE;
		}
		MouseClick(hwnd, btnRect.x + btnRect.width / 2, btnRect.y + btnRect.height / 2);
		return TRUE;
	}

	BOOL ExpectUI(char * wndName, char * expUiImg)
	{
		BOOL flag = FALSE;
		int n = 10;
		while(n--)
		{
			HWND hwnd = FindWindow(NULL, wndName);
			if(!hwnd)
			{
				std::cout << "Finding window..." << std::endl;
				Sleep(1000);
				continue;
			}

			SetForegroundWindow(hwnd);
			//Sleep(100);
			cv::Rect uiRect = FindUIRect(expUiImg);
			if(uiRect.width)
			{
				flag = TRUE;
				std::cout << "Ui matched!" << std::endl;
				break;
			}
		}

		if(!flag)
		{
			std::cerr << "Expected ui doesn't match. Maybe a bug occurred." << std::endl;
			cv::imwrite("bug_screen.bmp", TakeScreenShot());
			cv::imwrite("bug_expectedui.bmp", cv::imread(expUiImg));
		}

		return flag;
	}

	void DrawRectangleOnTransparent(HWND hWnd, const RECT& rc)
	{
		HDC hDC = GetDC(hWnd);
		if (hDC)
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);

			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biWidth = rcClient.right;
			bmi.bmiHeader.biHeight = -rcClient.bottom;

			LPVOID pBits;
			HBITMAP hBmpSource = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, 0, 0);
			DWORD a = GetLastError();
			if (hBmpSource)
			{
				HDC hDCSource = CreateCompatibleDC(hDC);
				if (hDCSource)
				{
					// fill the background in red
					HGDIOBJ hOldBmp = SelectObject(hDCSource, hBmpSource);
					HBRUSH hBsh = CreateSolidBrush(RGB(0,0,255));
					FillRect(hDCSource, &rcClient, hBsh);
					DeleteObject(hBsh);

					// draw the rectangle in black
					HGDIOBJ hOldBsh = SelectObject(hDCSource, GetStockObject(NULL_BRUSH));
					HGDIOBJ hOldPen = SelectObject(hDCSource, CreatePen(PS_SOLID, 2, RGB(0,0,0)));
					Rectangle(hDCSource, rc.left, rc.top, rc.right, rc.bottom);
					DeleteObject(SelectObject(hDCSource, hOldPen));
					SelectObject(hDCSource, hOldBsh);

					GdiFlush();

					// fix up the alpha channel
					DWORD* pPixel = reinterpret_cast<DWORD*>(pBits);
					for (int y = 0; y < rcClient.bottom; y++)
					{
						for (int x = 0; x < rcClient.right; x++, pPixel++)
						{
							if ((*pPixel & 0x00ff0000) == 0x00ff0000)
								*pPixel |= 0x01000000; // transparent
							else
								*pPixel |= 0xff000000; // solid
						}
					}

					// Update the layered window
					POINT pt = { 0 };
					BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
					UpdateLayeredWindow(hWnd, hDC, NULL, NULL, hDCSource, &pt, 0, &bf, ULW_ALPHA);

					SelectObject(hDCSource, hOldBmp);
					DeleteDC(hDCSource);
				}
				DeleteObject(hBmpSource);
			}
			ReleaseDC(hWnd, hDC);
		}
	}
}