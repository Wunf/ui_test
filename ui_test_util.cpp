#include "ui_test_util.h"
#include <comutil.h>

namespace ui_test 
{
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
	
	cv::Mat HWnd2Mat(HWND hwnd)
	{
		HDC hwindowDC, hwindowCompatibleDC;

		int height, width, srcheight, srcwidth;
		HBITMAP hbwindow;
		cv::Mat src;
		BITMAPINFOHEADER  bi;

		hwindowDC = GetDC(hwnd);
		hwindowCompatibleDC=CreateCompatibleDC(hwindowDC);
		SetStretchBltMode(hwindowCompatibleDC,COLORONCOLOR);  

		RECT windowsize;    // get the height and width of the screen
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

		// use the previously created device context with the bitmap
		SelectObject(hwindowCompatibleDC, hbwindow);
		// copy from the window device context to the bitmap device context
		StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
		GetDIBits(hwindowCompatibleDC,hbwindow,0,height,src.data,(BITMAPINFO *)&bi,DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
		
		DeleteObject (hbwindow); 
		DeleteDC(hwindowCompatibleDC); 
		ReleaseDC(hwnd, hwindowDC);

		return src;
	}

	void MouseClick(HWND hwnd, DWORD wndX, DWORD wndY)
	{
		RECT windowsize;    
		GetWindowRect(hwnd, &windowsize);
		DWORD absX = windowsize.left + wndX, absY = windowsize.top + wndY;
		//mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, absX, absY, 0, 0);
		SetCursorPos(absX, absY);
		INPUT    Input = {0};													// Create our input.

		Input.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
		Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;							// We are setting left mouse button down.
		SendInput( 1, &Input, sizeof(INPUT) );								// Send the input.

		ZeroMemory(&Input,sizeof(INPUT));									// Fills a block of memory with zeros.
		Input.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
		Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;								// We are setting left mouse button up.
		SendInput( 1, &Input, sizeof(INPUT) );
	}
}