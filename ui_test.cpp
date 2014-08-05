#include "ui_test_util.h"
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>

int main(int argc, char * argv[])
{
	ui_test::SetNotDisplayMatch();

	int n = 1;
	while(n--)
	{
		WinExec("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe", SW_SHOW);
		if(!ui_test::ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_newerverfound.bmp"))
			break;
		if(!ui_test::ClickBtn("������Ϸƽ̨ ��װ", "templ/ok_btn_templ.bmp"))
			break;
		if(!ui_test::ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_install.bmp"))
			break;
		if(!ui_test::ClickBtn("������Ϸƽ̨ ��װ", "templ/install_btn_templ.bmp"))
			break;
		if(!ui_test::ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_installing.bmp"))
			break;
		Sleep(5000);
		if(!ui_test::ExpectUI("NGP", "templ/expected_ngp.bmp"))
			break;
		if(!ui_test::ClickBtn("NGP", "templ/ngp_close_btn_templ.bmp"))
			break;
	}

	system("pause");
	return 0;
}