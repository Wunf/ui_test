function TestLogin()
	local res = ClickBtn("NGP", "templ/btn_psw.bmp") -- 1 key
	if res == 0 then
		return 0
	end
	local res = PressKey("NGP", 0x31) -- 1 key
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_login.bmp")
	if res == 0 then
		return 0
	end
end

function TestLogout()
	local res = ClickBtn("NGP", "templ/ui_ngp_logo.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_change_account.bmp")
	if res == 0 then
		return 0
	end
end

function TestGameSwitch()
	local res = ClickBtn("NGP", "templ/btn_qn.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_jl.bmp")
	if res == 0 then
		return 0
	end
end

function TestChangeServer()
	local res = ClickBtn("NGP", "templ/btn_change_server.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_167jian.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_tianxiahui.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_back.bmp")
	if res == 0 then
		return 0
	end
end

function TestConfig()
	local res = ClickBtn("NGP", "templ/btn_config.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_improvecpu.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_config_reset.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_config_ok.bmp")
	if res == 0 then
		return 0
	end
end
