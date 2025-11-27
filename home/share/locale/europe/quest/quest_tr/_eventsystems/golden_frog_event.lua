------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest golden_frog_event begin
	state start begin
		when login or event_begin with game.get_event_flag("golden_frog_event") > 0 begin
			if pc.get_map_index() == 230 then
				notice_all(644, "")
				notice_all(645, "")
				notice_all(646, "")
				notice_all(647, "")
			end
			send_letter("Altin Kurbaga Etkinligi")
		end

		when event_end with game.get_event_flag("golden_frog_event") == 0 begin
			clear_letter()
		end

		when button or info with game.get_event_flag("golden_frog_event") > 0 begin
			say_title("Altin Kurbaga Etkinligi")
			say_new("[ENTER]Altin Kurbagalar Etkinlik Haritasi'na indi. Efsanelere göre degerli esyalari sakliyorlar, ama derileri bu dünyadaki en sertlerinden biri. [ENTER]")
			say_reward_new("Katilmak isterseniz, Ekip Üyeleri tarafindan belirlenen kurallari kabul etmis olursunuz. [ENTER]")

			if select("Girmek istiyorum", "Fikrimi degistirdim") == 1 then
				if game.get_event_flag("golden_frog_event") > 0 then
					if pc.get_map_index() == 230 then
						syschat("Zaten Etkinlik Haritasi'ndasiniz. [ENTER]")
						return
					end
					pc.warp(985600, 755200)
				else
					say_title("Altin Kurbaga Etkinligi")
					say_new("Etkinlige katilmak için biraz geç kaldiniz! Endiselenmeyin, bir dahaki sefere etkinlige katilmak için birçok firsat olacak. [ENTER]")
				end
			end
		end
	end
end
