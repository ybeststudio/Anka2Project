------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest moonlight_drop_event begin
	state start begin
		when login or event_begin with game.get_event_flag("moon_drop") > 0 begin
			send_letter("Meshure Hazineleri")
		end

		when event_end with game.get_event_flag("moon_drop") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("moon_drop") > 0 begin
			say_title("Meshure Hazineleri")
			say_new("[ENTER]Merhaba genç kahraman! Bugün ay en parlak halinde parliyor. Ve bunun nedeni bugün dolunay olmasi. Bu özel günde, avladiginiz her canavardan Meshure Kutulari elde edebilirsiniz. [ENTER]")
			say_reward_new("Dolunayin ve sizin için sakladigi hazinenin tadini çikarin! [ENTER]")
		end
	end
end