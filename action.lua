require "test_install"
require "test_minimize"
require "test_login"

SetErrAcceptance(2000000)
--local res = TestInstall()
--if res == 0 then
--	break;
--end
--Sleep(8000)

function MainAction()
	RunExe("C:\\Program Files (x86)\\Netease\\NGP\\out\\Release\\LootHoarder.exe")
	local res = ClickBtn("NGP", "templ/btn_ngp_cancel.bmp")
	if res ~= 0 then
		for i = 1, 100 do
			local res = TestLogin()
			if res == 0 then
				break
			end
		--	Sleep(5000)
		--	local res = ExpectUI("NGP", "templ/ui_junlin.bmp")
		--	if res == 0 then
		--		break;
		--	end
		--	rese= TestMinimize()
		--	if res == 0 then
		--		break;
		--	end
		--	res = TestConfig()
		--	if res == 0 then
		--		break;
		--	end
		--	local res = MouseMove("NGP", "templ/ui_junlin.bmp")
		--	if res == 0 then
		--		break;
		--	end
		--	local res = ExpectUI("NGP", "templ/ui_sidebar.bmp")
		--	if res == 0 then
		--		break;
		--	end
		end
	end
end

MainAction()
