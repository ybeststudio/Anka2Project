------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest hexegonal_drop_event begin
	state start begin
		when login or event_begin with game.get_event_flag("hexegonal_drop") > 0 begin
			send_letter("Altigen Kutu Etkinligi")
		end

		when button or info with game.get_event_flag("hexegonal_drop") > 0  begin
			say_title("Altigen Kutu Etkinligi")
			say_new("[ENTER]Çin Yeni Yili'ndaki geleneklerden biri, hediyelesme ve iyi sans dilekleridir. Çin kültüründe, Altigen Kutunun alti yüzü bu dilekleri temsil eder. [ENTER]")
			say_reward_new("Herhangi bir canavardan Altigen Kutular toplayin! Altigen kutular rastgele yükseltme malzemesi içerir. Hazineyi ve içeriginin tadini çikarin! [ENTER]")
		end

		when event_end with game.get_event_flag("hexegonal_drop") == 0 begin
			clear_letter()
		end
	end
end