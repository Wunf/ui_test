#include <windows.h>
#include "ui_test_util.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char * argv[])
{
	HWND hwnd;
	if( NULL == (hwnd = FindWindow(NULL, "网易游戏平台 安装")))
	{
		MessageBox(NULL, "error", "找不到窗口", MB_OK);
	}
	SetForegroundWindow(hwnd);
	Sleep(1000);
	ui_test::MouseClick(hwnd, 23, 305);
	Sleep(1000);
	//cv::Mat src = ui_test::HWnd2Mat(hwnd);
	//cv::imwrite("output.bmp", src);
	system("pause");
	return 0;
}