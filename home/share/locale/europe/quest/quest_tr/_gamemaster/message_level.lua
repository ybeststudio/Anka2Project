------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------

quest message_level begin
	state start begin 
		when levelup begin
			if pc.level == 55 or pc.level == 75 or pc.level == 90 or pc.level == 100 or pc.level == 110 then
				notice_all(614, string.format("%s#%s", pc.get_name(), pc.get_level()))
			elseif pc.level == 120 then
				notice_all(615, pc.get_name())
			end
		end
	end
end
