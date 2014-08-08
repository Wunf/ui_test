require "install"

SetErrAcceptance(20000000)
--RunExe("C:\\Program Files (x86)\\Netease\\NGP\\out\\Release\\LootHoarder.exe")

for i = 1, 10 do
	--local res = TestInstall()
	--if res == 0 then
	--	break;
	--end
	--Sleep(8000)
	--local res = ExpectUI("NGP", "templ/ui_ngp.bmp")
	--if res == 0 then
	--	break;
--	end
--	local res = ClickBtn("NGP", "templ/btn_offline.bmp")
--	if res == 0 then
--		break;
--	end
--	Sleep(5000)
--	local res = ExpectUI("NGP", "templ/ui_junlin.bmp")
--	if res == 0 then
--		break;
--	end
--	local res = ClickBtn("NGP", "templ/btn_minimize.bmp")
--	if res == 0 then
--		break;
--	end
--	local res = ClickBtn("NGP", "templ/btn_taskbar_ngp.bmp")
--	if res == 0 then
--		break;
--	end
	local res = ClickBtn("NGP", "templ/btn_minimize.bmp")
	if res == 0 then
		break;
	end
	local res = ClickBtn("NGP", "templ/btn_systray.bmp")
	if res == 0 then
		break;
	end
	local res = DoubleClick("NGP", "templ/btn_systray_ngp.bmp")
	if res == 0 then
		break;
	end
	local res = ClickBtn("NGP", "templ/btn_systray2.bmp")
	if res == 0 then
		break;
	end
--	local res = MouseMove("NGP", "templ/ui_junlin.bmp")
--	if res == 0 then
--		break;
--	end
--	local res = ExpectUI("NGP", "templ/ui_sidebar.bmp")
--	if res == 0 then
--		break;
--	end
end
