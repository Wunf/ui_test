#include "ui_test_util.h"
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>

int main(int argc, char * argv[])
{
	ui_test::SetDisplayMatch();

	int n = 20;
	while(n--)
	{
		WinExec("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe", SW_SHOW);
		if(!ui_test::ExpectUI("网易游戏平台 安装", "templ/expected_newerverfound.bmp"))
			break;
		if(!ui_test::ClickBtn("网易游戏平台 安装", "templ/ok_btn_templ.bmp"))
			break;
		if(!ui_test::ExpectUI("网易游戏平台 安装", "templ/expected_install.bmp"))
			break;
		if(!ui_test::ClickBtn("网易游戏平台 安装", "templ/install_btn_templ.bmp"))
			break;
		if(!ui_test::ExpectUI("网易游戏平台 安装", "templ/expected_installing.bmp"))
			break;
		if(!ui_test::ExpectUI("网易游戏平台 安装", "templ/expected_installing_text_change.bmp"))
			break;
		Sleep(2000);
		if(!ui_test::ExpectUI("NGP", "templ/expected_ngp.bmp"))
			break;
		if(!ui_test::ClickBtn("NGP", "templ/ngp_close_btn_templ.bmp"))
			break;
	}

	system("pause");
	return 0;
}