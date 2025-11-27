------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest fishing_event begin
	state start begin
		when login or event_begin with game.get_event_flag("fish_event") > 0 begin
			send_letter("Gizemli Kutu Etkinligi")
		end

		when button or info with game.get_event_flag("fish_event") > 0 begin
			say_title("Gizemli Kutu Etkinligi")
			say_new("[ENTER]3 adet Minnow'u bir Balikçi'ya getirin ve onlari, açarak baska esyalar alabileceginiz Gizemli Kutu ile degistireceksiniz. Kutunun içinde vahsi canavarlar olabileceginin farkinda olun! [ENTER]")
			say_reward_new("Etkinligin tadini çikarin ve balik tutarken iyi sanslar! [ENTER]")
		end

		when event_end with game.get_event_flag("fish_event") == 0 begin
			clear_letter()
		end

		when 9009.chat."Gizemli Kutu Etkinligi" with game.get_event_flag("fish_event") > 0 begin
			if pc.count_item(27802) < 3 then
				say_new("[ENTER]Üzgünüm ama ödül için yeterli baliga sahip degilsiniz![ENTER]Lütfen Gizemli Kutu almak için en az 3 Minnow ile geri gelin![ENTER]Iyi sanslar! [ENTER]")
			else
				say_new("[ENTER]Görüyorum ki balik tutmussunuz ve size takas için bir teklifim var. Bana 3 Minnow'unuzu verin ve sizi Gizemli Kutu ile ödüllendireyim. Içinde ya degerli esyalar var ya da içinden canavarlar çikiyor. [ENTER]")
				say_reward_new("Takas yapmak ister misiniz? [ENTER]")

				if select("Evet", "Hayir") == 1 then
					pc.remove_item(27802, 3)
					pc.give_item2(50033)
				end
			end
		end
	end
end
