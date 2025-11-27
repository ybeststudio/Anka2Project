-----------------------------------
--        Author: r_tw1x1        --
--  Server: Nirvana2 v1.0  2022  --
-----------------------------------
-- Encoding: Unix (LF) line endings, Windows-1254 (Türkçe ANSI) encoding
-- Bu dosya translate.lua'dan baðýmsýzdýr, Türkçe metinler doðrudan dosyaya gömülmüþtür

quest boss_event begin
	state start begin
		when login or event_begin with game.get_event_flag("boss_event") > 0 begin
			if pc.get_map_index() == 230 then
				notice_all(639, "")
				notice_all(640, "")
				notice_all(641, "")
				notice_all(642, "")
				notice_all(643, "")
			end
			send_letter("Boss Etkinliði ")
		end

		when event_end with game.get_event_flag("boss_event") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("boss_event") > 0 begin
			say_title("Boss Etkinliði ")
			say_new("[ENTER]Boss Etkinliði Etkinlik Haritasý'nda gerçekleþiyor. Bosslar 3 farklý bölgede doðuyor, her bölge benzer seviyelerde bosslar içeriyor. [ENTER]")
			say_reward_new("Katýlmak isterseniz, Ekip Üyeleri tarafýndan belirlenen kurallarý kabul etmiþ olursunuz. [ENTER]")

			if select("Girmek istiyorum ", "Fikrimi deðiþtirdim ") == 1 then
				if game.get_event_flag("boss_event") > 0 then
					if pc.get_map_index() == 230 then
						syschat("Zaten Etkinlik Haritasý'ndasýnýz. [ENTER]")
						return
					end
					pc.warp(985600, 755200)
				else
					say_title("Boss Etkinliði ")
					say_new("Etkinliðe katýlmak için biraz geç kaldýnýz! Endiþelenmeyin, bir dahaki sefere etkinliðe katýlmak için birçok fýrsat olacak. [ENTER]")
				end
			end
		end
	end
end
