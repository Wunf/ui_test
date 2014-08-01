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

		//得到父亲支持的IEnumVARIANT接口
		hr = paccParent->QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

		if(pEnum)
			pEnum->Reset();

		//取得父亲拥有的可访问的子的数目
		paccParent->get_accChildCount(&numChildren);

		//搜索并比较每一个子ID，找到名字、角色、类与输入相一致的。
		for(index = 1; index <= numChildren && !found; index++)
		{
			pCAcc = NULL;
			// 如果支持IEnumVARIANT接口，得到下一个子ID
			// 以及其对应的 IDispatch 接口
			if (pEnum)
				hr = pEnum->Next(1, &varChild, &numFetched);
			else
			{
				//如果一个父亲不支持IEnumVARIANT接口，子ID就是它的序号
				varChild.vt = VT_I4;
				varChild.lVal = index;
			}

			// 找到此子ID对应的 IDispatch 接口
			if (varChild.vt == VT_I4)
			{
				//通过子ID序号得到对应的 IDispatch 接口
				pDisp = NULL;
				hr = paccParent->get_accChild(varChild, &pDisp);
			}
			else
			{
				//如果父支持IEnumVARIANT接口可以直接得到子IDispatch 接口
				pDisp = varChild.pdispVal;
			}

			// 通过 IDispatch 接口得到子的 IAccessible 接口 pCAcc
			if (pDisp)
			{
				hr = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
				hr = pDisp->Release();
			}

			// Get information about the child
			if (pCAcc)
			{
				//如果子支持IAccessible 接口，那么子ID就是CHILDID_SELF
				VariantInit(&varChild);
				varChild.vt = VT_I4;
				varChild.lVal = CHILDID_SELF;

				*paccChild = pCAcc;
			}
			else
			{
				//如果子不支持IAccessible 接口
				*paccChild = paccParent;
			}

			//跳过了有不可访问状态的元素
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
			// 通过get_accName得到Name
			GetObjectName(*paccChild, &varChild, szObjName, sizeof(szObjName));
			// 通过get_accRole得到Role
			GetObjectRole(*paccChild, &varChild, szObjRole, sizeof(szObjRole));
			// 通过WindowFromAccessibleObject和GetClassName得到Class
			GetObjectClass(*paccChild, szObjClass, sizeof(szObjClass));
			// 以上实现代码比较简单，大家自己看代码吧。
			// 如果这些参数与输入相符或输入为NULL
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
				// 以这次得到的子接口为父递归调用
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