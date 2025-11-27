------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest mining_event begin
	state start begin
		when login or event_begin with game.get_event_flag("mining_event") > 0 begin
			if pc.get_map_index() == 230 then
				notice_all(653, "")
				notice_all(654, "")
				notice_all(655, "")
				notice_all(656, "")
			end
			send_letter("Madencilik Etkinligi")
		end

		when event_end with game.get_event_flag("mining_event") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("mining_event") > 0 begin
			say_title("Madencilik Etkinligi")
			say_new("[ENTER]Madencilik Etkinligi Etkinlik Haritasi'nda gerçeklesiyor. Damarlar 3 farkli bölgede doguyor, her bölge benzer seviyelerde damarlar içeriyor. [ENTER]")
			say_reward_new("Katilmak isterseniz, Ekip Üyeleri tarafindan belirlenen kurallari kabul etmis olursunuz. [ENTER]")

			if select("Girmek istiyorum", "Fikrimi degistirdim") == 1 then
				if game.get_event_flag("mining_event") > 0 then
					if pc.get_map_index() == 230 then
						syschat("Zaten Etkinlik Haritasi'ndasiniz. [ENTER]")
						return
					end
					pc.warp(985600, 755200)
				else
					say_title("Madencilik Etkinligi")
					say_new("Etkinlige katilmak için biraz geç kaldiniz! Endiselenmeyin, bir dahaki sefere etkinlige katilmak için birçok firsat olacak. [ENTER]")
				end
			end
		end
	end
end
