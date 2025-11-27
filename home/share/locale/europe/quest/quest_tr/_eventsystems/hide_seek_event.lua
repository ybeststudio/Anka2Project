------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest hide_seek_event begin
	state start begin
		when login or event_begin with game.get_event_flag("hide_seek_event") > 0 begin
			send_letter("Saklambaç Etkinligi")
		end

		when button or info with game.get_event_flag("hide_seek_event") > 0 begin
			say_title("Saklambaç Etkinligi")
			say_new("[ENTER]Ekip üyeleri rastgele bir manzarada saklanacaklar. Sizin göreviniz ipuçlarini takip edip onlari bulmak. Ne kadar hizli olursaniz, ödül o kadar iyi olur. [ENTER]")
			say_reward_new("Sohbete dikkat edin ve ipuçlari için gözünüzü dört açin.[ENTER]Etkinligin tadini çikarin! [ENTER]")
		end

		when event_end with game.get_event_flag("hide_seek_event") == 0 begin
			clear_letter()
		end
	end
end