for i = 1, 2 do
	local res = RunExe("D:\\work\\trunk\\program\\misc\\dailybuild\\NGP.exe")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_newerverfound.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ClickBtn("������Ϸƽ̨ ��װ", "templ/ok_btn_templ.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_install.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ClickBtn("������Ϸƽ̨ ��װ", "install_btn_templ.bmp")
	print(res)
	if res == 0 then
		break;
	end
	local res = ExpectUI("������Ϸƽ̨ ��װ", "templ/expected_installing.bmp")
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
