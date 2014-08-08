function TestMinimize()
	local res = ClickBtn("NGP", "templ/btn_minimize.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_taskbar_ngp.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_minimize.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_systray.bmp")
	if res == 0 then
		return 0
	end
	local res = DoubleClick("NGP", "templ/btn_systray_ngp.bmp")
	if res == 0 then
		return 0
	end
	local res = ClickBtn("NGP", "templ/btn_systray2.bmp")
	if res == 0 then
		return 0
	end
end
