------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest training_grandmaster_skill begin
	state start begin
		when 50513.use begin
			say_title("Ruh tasi")
			local seelicount = pc.count_item(50513)
			if pc.get_skill_group() == 0 then
				say_new("[ENTER]Egitilebilecek yeteneginiz yok. [ENTER]")
				return
			end

			if get_time() < pc.getqf("next_time") then
				if pc.is_skill_book_no_delay() then
					say_new("[ENTER]Seytan çikarma parsömeni kullandiniz. Artik bir ruh tasi okuyabilirsiniz. [ENTER]")
					wait()
					say_title("Ruh tasi")
				else
					say_new("[ENTER]Bir ruh tasi okumak veya seytan çikarma parsömeni kullanmak için 1 gün beklemeniz gerekiyor. [ENTER]")
					return
				end
			end

			local result = training_grandmaster_skill.BuildGrandMasterSkillList(pc.get_job(), pc.get_skill_group())

			local vnum_list = result[1]
			local name_list = result[2]

			if table.getn(vnum_list) == 0 then
				say_new("[ENTER]Egitilebilecek yeteneginiz yok. [ENTER]")
				return
			end

			say_new("[ENTER]Egitmek istediginiz yetenegi seçin. [ENTER]")

			local menu_list = {}
			table.foreach(name_list, function(i, name) table.insert(menu_list, name) end)
			table.insert(menu_list, "Iptal")

			local s=select_table(menu_list)
			if table.getn(menu_list) == s then
				return
			end

			local skill_name = name_list[s]
			local skill_vnum = vnum_list[s]
			local skill_level = pc.get_skill_level(skill_vnum)
			local cur_alignment = pc.get_real_alignment()
			local need_alignment = 1000+500*(skill_level-30)

			local title = string.format("%s yetenegini egit", skill_name)
			if pc.count_item(50513) != seelicount then
				say_new("[ENTER]Ruh tasiniz yok! [ENTER]")
				return
			end

			say_title(title)
			say_new("[ENTER]Bir ruh tasi okumak için hizalama puanlarina ihtiyaciniz var. [ENTER]")

			if cur_alignment<-19000 + need_alignment then
				say_reward_new("[ENTER]Hizalama puanlariniz çok düsük. [ENTER]")
				return
			end

			if cur_alignment<0 then
				say_reward_new("[ENTER]Egitimden sonra hizalama puanlariniz düsecek. [ENTER]")
				say_reward_new(string.format("[ENTER]%d ile %d arasinda hizalama puani kaybedeceksiniz. [ENTER]", need_alignment, need_alignment*2))
				need_alignment = need_alignment*2
			elseif cur_alignment<need_alignment then
				say_new(string.format("[ENTER]Gerekli hizalama puani: %d. [ENTER]", need_alignment))
				say_reward_new("[ENTER]Egitimden sonra rütbenizin negatif olma ihtimali var. [ENTER]")
			else
				say_reward_new(string.format("[ENTER]Gerekli hizalama puani: %d. [ENTER]", need_alignment))
			end

			local s = select("Egit", "Iptal")
			if s == 2 then
				return
			end

			if cur_alignment >= 0 and cur_alignment < need_alignment then
				say_title(string.format("%s ", title))
				say_new("[ENTER]Bir ruh tasi ile egitim yapmak için asagidaki kutucuga kelimeyi yazmalisiniz [ENTER]")
				say_reward_new("Metin2")
				say_reward_new("[ENTER]Egitimden sonra rütbenizin negatif olma ihtimali var. [ENTER]")

				local s = input()
				if s != "Metin22" then
					return
				end
			end

			if get_time() < pc.getqf("next_time") then
				if pc.is_skill_book_no_delay() then
					pc.remove_skill_book_no_delay()
				end
			end

			pc.setqf("next_time", get_time() + time_hour_to_sec(number(8, 12)))
			if need_alignment > 0 then
				if pc.learn_grand_master_skill(skill_vnum) then
					pc.change_alignment(-need_alignment)
					say_title(string.format("%s", title))

					if 40 == pc.get_skill_level(skill_vnum) then
						say_new(string.format("[ENTER]%s basariyla egitildi. [ENTER]", skill_name))
						pc.remove_item(50513, 1)
					else
						say_new("[ENTER]Egitim basarili oldu. [ENTER]")
						pc.remove_item(50513, 1)
					end

					say_reward (string.format("[ENTER]Hizalama puanlari su seviyeye düstü: %d. [ENTER]", need_alignment))
				else
					say_title(string.format("%s", title))
					say_reward_new("[ENTER]Egitim basarisiz oldu. [ENTER]")
					pc.change_alignment(-number(need_alignment/3, need_alignment/2))
					pc.remove_item(50513, 1)
				end
			end
		end

		function BuildGrandMasterSkillList(job, group)
			GRAND_MASTER_SKILL_LEVEL = 30
			PERFECT_MASTER_SKILL_LEVEL = 40

			local skill_list = special.active_skill_list[job+1][group]
			local ret_vnum_list = {}
			local ret_name_list = {}

			table.foreach(skill_list,
				function(i, skill_vnum)
					local skill_level = pc.get_skill_level(skill_vnum)

					if skill_level >= GRAND_MASTER_SKILL_LEVEL and skill_level < PERFECT_MASTER_SKILL_LEVEL then
						table.insert(ret_vnum_list, skill_vnum)
						local name = skill_name(skill_vnum)
						if name == nil then name = skill_vnum end
						table.insert(ret_name_list, name)
					end
				end
			)
			return {ret_vnum_list, ret_name_list}
		end
	end
end
