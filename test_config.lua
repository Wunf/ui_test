function TestConfig()
	local res = MouseMove("NGP", "templ/ui_junlin.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_config.bmp")
	if res == 0 then
		return 0
	end
--[[	local res = ClickBtn("NGP", "templ/btn_effectlow.bmp")
	if res == 0 then
		return 0
	end]]
	local res = ClickBtn("NGP", "templ/btn_improvecpu.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_improvecpu_checked.bmp")
	if res == 0 then
		return 0
	end
	local res = ExpectUI("NGP", "templ/ui_improvecpu.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_config_ok.bmp")
	if res == 0 then
		return 0
	end
	return 1
end
