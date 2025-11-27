-----------------------------------
--        Author: r_tw1x1        --
--  Server: Nirvana2 v1.0  2022  --
-----------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
-----------------------------------

quest tanaka_event begin
	state start begin
		when login or event_begin with game.get_event_flag("tanaka_event") > 0 begin
			send_letter("Korsan Tanaka Etkinligi")
		end

		when button or info with game.get_event_flag("tanaka_event") > 0 begin
			say_title("Korsan Tanaka Etkinligi")
			say_new("[ENTER]Korsan Tanaka, Yongbi Çölü'nde rastgele yerlerde görünmeye baslayacak. Amaciniz onlari bulup avlamak. Tanaka hala hareket halinde, bu yüzden yeterince hizli ve çevik olmalisiniz. Etkinlik sona erdiginde, topladiginiz esyalari Yonah'a teslim edip baska esyalar alabileceksiniz. [ENTER]")
			say_reward_new("Iyi avlar ve etkinligin tadini çikarin! [ENTER]")
		end

		when event_end with game.get_event_flag("tanaka_event") == 0 begin
			clear_letter()
		end

		when 20005.chat."Korsan Tanaka Etkinligi" with game.get_event_flag("tanaka_event") > 0 begin
			if pc.count_item(30202) < 1 then
				say_title(mob_name(20005))
				say_new("[ENTER]Arghh, öylece durmayi birak ve bana Tanaka'nin Kulagini getir! [ENTER]")
			else
				say_title(mob_name(20005))
				say_new("[ENTER]Görünüse göre bu hirsizlari avlamakla mesgulmüssün. Tanaka'nin Kulagi karsiliginda seni bir seyle ödüllendirecegim, sadece ne istediginden emin degilim. [ENTER]")

				say_reward_new("Tanaka'nin Kulagini rastgele bir esya ile degistirmek ister misiniz? [ENTER]")

				if select("Evet", "Hayir") == 1 then

				local Reward = {
					[1] = {11299, 1}, --**Test
					[2] = {11299, 1}, --**Test
				}

					local item = math.random(1, 2)
					pc.remove_item(30202, 1)
					pc.give_item(Reward[item][1], Reward[item][2])
				end
			end
		end
	end
end
