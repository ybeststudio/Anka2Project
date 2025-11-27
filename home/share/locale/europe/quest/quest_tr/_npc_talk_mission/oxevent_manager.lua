------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest oxevent_manager begin
	state start begin
		when login with pc.get_map_index() == 113 begin
			say_title("OX Yarismasi")
			say_new("[ENTER]Burada, zeka yarismasi için üç kralligin en parlak zekalari toplanir. Bu test sirasinda, herhangi bir konuda daha fazla soru görüntülenecektir. Cevabin dogru oldugunu düsünüyorsaniz O'ya, yanlis oldugunu düsünüyorsaniz X'e geçin. Sadece en iyiler ödüllendirilecek! [ENTER]")
		end

		when letter with pc.get_map_index() == 113 begin
			send_letter("OX Yarismasi'ndan ayril")
		end

		when button or info begin
			say_title("OX Yarismasi'ndan ayril")
			say_new("[ENTER]Ayrilmak istediginizden emin misiniz? [ENTER]")

			local s = select("Evet", "Hayir")
			if s == 2 then
				send_letter("OX Yarismasi'ndan ayril")
				return
			end

			local empire = pc.get_empire()
			if empire == 1 then
				pc.warp(473900, 954600)
			elseif empire == 2 then
				pc.warp(63200, 166700)
			elseif empire == 3 then
				pc.warp(959600, 269700)
			end
		end

		when 20011.chat."OX Yarismasi" begin
			say_title(mob_name(20011))
			say_new("[ENTER]Hey, sen oradaki! Evet, evet sen - buraya gel! Sana ilginç bir sey göstereyim. OX Yarismasi. Seni oraya götürebilirim. Ilginç ödüller kazanabilecegini duydum! [ENTER]")
			wait()

			if pc.get_level() < 5 then
				say_title(mob_name(20011))
				say_new("[ENTER]Üzgünüm, ancak katilmak için yeterli seviyede degilsiniz. [ENTER]")
				say_reward_new("Katilim için minimum seviye: 5 [ENTER]")
				return
			end

			if game.get_event_flag("oxevent_status") == 0 then
				say_title(mob_name(20011))
				say_new("[ENTER]Su anda herhangi bir yarisma duymadim. Ögrenince size haber veririm. [ENTER]")
			elseif game.get_event_flag("oxevent_status") == 1 then
				say_title(mob_name(20011))
				say_new("[ENTER]Yarismaya katilmak mi yoksa sadece izlemek mi istiyorsunuz? [ENTER]")

				local s = select("Katil", "Izleyici", "Iptal")
				if s == 1 then
					say_title(mob_name(20011))
					say_new("[ENTER]Sizi hemen yarismaya götürecegim. Basarilar. [ENTER]")
					horse.unride()
					horse.unsummon()
					wait()
					pc.warp(896500, 24600)
				elseif s == 2 then
					say_title(mob_name(20011))
					say_new("[ENTER]Sadece izleyici olmak mi istiyorsunuz? Tamam, sizin seçiminiz. Hemen isinlanacaksiniz. [ENTER]")
					horse.unride()
					horse.unsummon()
					wait()
					pc.warp(896300, 28900)
				end

			elseif game.get_event_flag("oxevent_status") == 2 then
				say_title(mob_name(20011))
				say_new("[ENTER]Üzgünüm, ancak yarisma çoktan basladi... Ama, isterseniz, hala bir izleyici olarak katilabilirsiniz. [ENTER]")

				local s = select("Katil", "Iptal")
				if s == 1 then
					say_title(mob_name(20011))
					say_new("[ENTER]Hemen isinlanacaksiniz. [ENTER]")
					horse.unride()
					horse.unsummon()
					wait()
					pc.warp(896300, 28900)
				end
			end
		end

		when 20358.chat."OX Yarismasi Paneli" with pc.is_gm() begin
			local stat = oxevent.get_status()
			if stat == 0 then
				say_title("OX Yarismasi Paneli")
				say_new("[ENTER]OX baslatilsin mi? [ENTER]")

				local s = select("Evet", "Hayir")
				if s == 2 then
					return
				end

				local v = oxevent.open()
				if v == 0 then
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]Sorulari yükleyemedim. [ENTER]")
					return
				elseif v == 1 then
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]OX Yarismasi basladi. [ENTER]")
					notice_all(635, "")
				else
					say_new("[ENTER]Hata [ENTER]")
					return
				end
			elseif stat == 1 then
				say_title("OX Yarismasi Paneli")
				say_new("[ENTER]Kapilar hala açik.[ENTER]Oyuncular: " ..oxevent.get_attender())

				local s = select("Kapilari kapat", "Iptal")
				if s == 1 then
					notice_all(636, "")
					oxevent.close()
				end
			elseif stat == 2 then
				say_title("OX Yarismasi Paneli")
				say_new("[ENTER]Ne yapmak istiyorsunuz? [ENTER]")

				local s = select("Soru sor", "Katilimcilara ödül teklif et", "Yarismayi kapat", "Yarismayi iptal et", "Iptal")
				if s == 1 then
					local v = oxevent.quiz(1, 30)
					if v == 0 then
						say_title("OX Yarismasi Paneli")
						say_new("[ENTER]Soru hatasi [ENTER]")
						return
					else
						say_title("OX Yarismasi Paneli")
						say_new("[ENTER]Soru basariyla soruldu. [ENTER]")
						return
					end
				elseif s == 2 then
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]Sifre: [ENTER]")

					local sname = input()
					if sname == "m2project" then
						say_title("OX Yarismasi Paneli")
						say_new("[ENTER]Esya ekle: [ENTER]")

						local item_vnum = input()

						say_title("OX Yarismasi Paneli")
						say_new("[ENTER]Miktar ekle: [ENTER]")

						local item_count = input()
						oxevent.give_item(item_vnum, item_count)

						say_title("OX Yarismasi Paneli")
						say_new("[ENTER]Katilimcilar sunu aldi: " ..item_name(item_vnum))

						notice_all(637, string.format("%s#%s", item_count, item_name(item_vnum)))
					end
				elseif s == 3 then
					oxevent.end_event()
					notice_all(638, "")
					setskin(0)
				elseif s == 4 then
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]Yarismayi durdurmak istediginizden emin misiniz? [ENTER]")

					local s = select("Evet", "Hayir")
					if s == 1 then
						oxevent.end_event_force()
					end
				elseif s == 5 then
					return
				end
			elseif stat == 3 then
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]Su anda sorulmakta olan bir soru var [ENTER]")
				else
					say_title("OX Yarismasi Paneli")
					say_new("[ENTER]Hata [ENTER]")
				return
			end
		end
	end
end