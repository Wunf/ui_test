function TestLogin()
--[[	local res = PressKey("NGP", 0x09) -- tab key
	if res == 0 then
		return 0
	end
	local res = PressKey("NGP", 0x09) -- tab key
	if res == 0 then
		return 0
	end]]
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
	local res = ClickBtn("NGP", "templ/ui_ngp_logo.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_change_account.bmp")
	if res == 0 then
		return 0
	end
	return 1
end
