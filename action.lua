require "test_install"
require "test_minimize"
require "test_config"

SetErrAcceptance(2000000)

for i = 1, 1 do
	--local res = TestInstall()
	--if res == 0 then
	--	break;
	--end
	--Sleep(8000)
	--RunExe("C:\\Program Files (x86)\\Netease\\NGP\\out\\Release\\LootHoarder.exe")
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
--	rese= TestMinimize()
--	if res == 0 then
--		break;
--	end
	res = TestConfig()
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
