function TestInstall()
	local res = RunExe("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe")
	if res == 0 then
		return 0
	end
	local res = ExpectUI("������Ϸƽ̨ ��װ", "templ/ui_newerverfound.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("������Ϸƽ̨ ��װ", "templ/btn_ok.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("������Ϸƽ̨ ��װ", "templ/btn_install.bmp")
	if res == 0 then
		return 0
	end
	local res = ExpectUI("������Ϸƽ̨ ��װ", "templ/ui_installing.bmp")
	if res == 0 then
		return 0
	end
	return 1
end
