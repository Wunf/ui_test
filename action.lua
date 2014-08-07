for i = 1, 2 do
	local res = RunExe("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("网易游戏平台 安装", "templ/expected_newerverfound.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ClickBtn("网易游戏平台 安装", "templ/ok_btn_templ.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("网易游戏平台 安装", "templ/expected_install.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ClickBtn("网易游戏平台 安装", "install_btn_templ.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("网易游戏平台 安装", "templ/expected_installing.bmp")
	print(res)
	if res == 0 then
		break;
	end
	Sleep(8000);
	local res = ExpectUI("NGP", "templ/expected_ngp.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ClickBtn("NGP", "templ/ngp_close_btn_templ.bmp")
	print(res)
	if res == 0 then
		break;
	end
end
