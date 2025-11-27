------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest cards_drop_event begin
	state start begin
		when login or event_begin with game.get_event_flag("okey_cards_drop") > 0 begin
			send_letter("Okey Karti")
		end

		when event_end with game.get_event_flag("okey_cards_drop") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("okey_cards_drop") > 0 begin
			say_title("Okey Karti")
			say_new("[ENTER]Merhaba genç kahraman! Bugün yeni bir oyun zamani. Okey Karti sayilara ve sansa dayanir. [ENTER]")
			say_reward_new("Ne kadar sanslisiniz? [ENTER]")
		end

		when 79505.pick with pc.count_item(79505) >= 24 begin
			pc.remove_item(79505, 24)
			pc.give_item2(79506)
		end
	end
end