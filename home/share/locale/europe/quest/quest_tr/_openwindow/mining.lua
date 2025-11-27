------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest mining begin
	state start begin
		when 20015.chat."Marketi ac" begin
			setskin(NOWINDOW)
			npc.open_shop()
		end

		when 20015.take with item.vnum >= 29101 and item.vnum < 29200 and item.get_socket(0) < item.get_value(2) begin
			say_title(mob_name(20015))
			say_new("[ENTER]Kazmaniz henüz yükseltilemez! [ENTER]")
		end

		when 20015.take with item.vnum >= 29101 and item.vnum < 29200 and item.get_socket(0) >= item.get_value(2) begin
			say_title(mob_name(20015))
			say_new("[ENTER]Peki ... kazmanizi yükseltmek istiyor musunuz? Bakalim ... [ENTER]")
			say_reward_new("[ENTER]Kazmanizin seviyesi "..item.get_level()..".[ENTER]")

			if item.get_value(3) == 100 then
				say_title(mob_name(20015))
				say_new("[ENTER]Onu yükseltmek istiyor musunuz? [ENTER]")
			else
				say_reward_new("[ENTER]"..(100-item.get_value(3)).."% ihtimal var "..(100-item.get_value(3)).."% ihtimalle kazma basarisiz olacak. [ENTER]")
				say_new("[ENTER]Eger bu olursa, kazmaniz bir seviye düsecek. Yine de yükseltmek istiyor musunuz? [ENTER]")
			end

			local s = select("Evet", "Hayir")
			if s == 2 then
				say_title(mob_name(20015))
				say_new("[ENTER]Fikrinizi degistirirseniz geri gelin. [ENTER]")
				return
			end

			local f = __refine_pick(item.get_cell())
			if f == 2 then
				say_title(mob_name(20015))
				say_new("[ENTER]Bir sorun olustu. Daha sonra gelin. [ENTER]")

			elseif f == 1 then
				say_title(mob_name(20015))
				say_new("[ENTER]Basardim. Kazma basariyla yükseltildi! [ENTER]")
			else
				say_title(mob_name(20015))
				say_new("[ENTER]Oh hayir, bir hata yaptim.[ENTER]Basarisiz oldum. Kazma bir seviye düstü. [ENTER]")
			end
		end

		when 20047.click or 20048.click or 20049.click or 20050.click or
			20051.click or 20052.click or 20053.click or 20054.click or
			20055.click or 20056.click or 20057.click or 20058.click or 20059.click begin

				if pc.is_mount() != true then
					if game.get_event_flag("labor_day") > 0 then
							char_log(0, "MINING", "Log for Labor Day")
						end
					pc.mining()
				end
		end
	end
end
