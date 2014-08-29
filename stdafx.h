#ifndef stdafx_h__
#define stdafx_h__

#include <windows.h>
#include <iostream>
#include <atlbase.h>
#include <atlcom.h>
#include <fstream>
#include <comutil.h>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

extern "C"
{
    #include <lua/lua.h>
    #include <lua/lualib.h>
    #include <lua/lauxlib.h>
};

#endif // stdafx_h__