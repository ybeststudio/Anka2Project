------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest metin_event begin
	state start begin
		when login or event_begin with game.get_event_flag("metin_event") > 0 begin
			if pc.get_map_index() == 230 then
				notice(648, "")
				notice(649, "")
				notice(650, "")
				notice(651, "")
				notice(652, "")
			end
			send_letter("Metin Yagmuru Etkinligi")
		end

		when event_end with game.get_event_flag("metin_event") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("metin_event") > 0 begin
			say_title("Metin Yagmuru Etkinligi")
			say_new("[ENTER]Metin Yagmuru Etkinlik Haritasi'na indi. Metinler 3 farkli bölgede doguyor, her bölge benzer seviyelerde Metinler içeriyor. [ENTER]")
			say_reward_new("Katilmak isterseniz, Ekip Üyeleri tarafindan belirlenen kurallari kabul etmis olursunuz. [ENTER]")

			if select("Girmek istiyorum", "Fikrimi degistirdim") == 1 then
				if game.get_event_flag("metin_event") > 0 then
					if pc.get_map_index() == 230 then
						syschat("Zaten Etkinlik Haritasi'ndasiniz. [ENTER]")
						return
					end
					pc.warp(384000, 76800)
				else
					say_title("Metin Yagmuru Etkinligi")
					say_new("Etkinlige katilmak için biraz geç kaldiniz! Endiselenmeyin, bir dahaki sefere etkinlige katilmak için birçok firsat olacak. [ENTER]")
				end
			end
		end
	end
end
