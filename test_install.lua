function TestInstall()
	local res = RunExe("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe")
	if res == 0 then
		return 0
	end
	local res = ExpectUI("网易游戏平台 安装", "templ/ui_newerverfound.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("网易游戏平台 安装", "templ/btn_ok.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("网易游戏平台 安装", "templ/btn_install.bmp")
	if res == 0 then
		return 0
	end
	local res = ExpectUI("网易游戏平台 安装", "templ/ui_installing.bmp")
	if res == 0 then
		return 0
	end
end
