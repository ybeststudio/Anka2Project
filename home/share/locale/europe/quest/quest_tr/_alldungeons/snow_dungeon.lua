------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest snow_dungeon begin
	state start begin
		when 20395.chat."Nemere'nin Gözetleme Kulesi" begin
			say_title(mob_name(20395))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				say_title(mob_name(20395))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then

					if pc.get_level() < 90 then
						say_title(mob_name(20395))
						say_new("[ENTER]Girmek için gerekli seviyeye sahip degilsiniz! [ENTER]")
						return
					end

					if not pc.is_gm() and pc.getf("snow_dungeon", "cooldown") > get_global_time() then
						local r_cooldown = pc.getf("snow_dungeon", "cooldown") - get_global_time()
						say_title(mob_name(20395))
						say_new("")
						say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
						return
					end

					if pc.count_item(71095) >= 1 then
						pc.remove_item(71095, 1)
					else
						say_title(mob_name(20395))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(71095, 1)
						return
					end

					d.new_jump(352, (5120+171)*100, (1536+271)*100)
					d.spawn_mob_dir(20397, 173, 262, 1)
					d.setf("DungeonBlock", 1)
					d.setf("first_regen", 1)
				end

			elseif x == 2 then
				say_title(mob_name(20395))
				say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					if not party.is_party() then
						say_title(mob_name(20395))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					if party.get_near_count() < 2 then
						say_title(mob_name(20395))
						say_new("[ENTER]Bir grup üyesi müsait degil! [ENTER]")
						return
					end

					local user_fail_level = {}
					local user_fail_ticket = {}
					local user_fail_cooldown = {}
					local ticketCheck = true
					local LevelCheck = true
					local CooldownCheck = true

					local pids = {party.get_member_pids()}

					for i, pid in next, pids, nil do
						q.begin_other_pc_block(pid)

						if pc.get_level() < 90 then
							table.insert(user_fail_level, pc.get_name())
							LevelCheck = false
						end

						if pc.count_item(71095) < 1 then
							table.insert(user_fail_ticket, pc.get_name())
							ticketCheck = false
						end

						if not pc.is_gm() and pc.getf("snow_dungeon", "cooldown") > get_global_time() then
							table.insert(user_fail_cooldown, pc.get_name())
							CooldownCheck = false
						end

						q.end_other_pc_block()
					end

					if ticketCheck == false then
						say_new("Oyuncunun anahtari yok: [ENTER]")
						local a = 1
						for i, name in next, user_fail_ticket, nil do
							say_new(string.format("%d. %s", a, name))
							a = a+1
						end
						say_item_vnum(71095, 1)
						return
					end

					if LevelCheck == false then
						say_new("Oyuncu beklemeli: [ENTER]")
						local a = 1
						for i, name in next, user_fail_level, nil do
							say_new(string.format("%d. %s", a, name))
							a = a+1
						end
						return
					end

					if CooldownCheck == false then
						say_new("Oyuncu beklemeli: [ENTER]")
						local a = 1
						for i, name in next, user_fail_cooldown, nil do
							say_new(string.format("%d. %s", a, name))
							a = a+1
						end
						return
					end

					if ticketCheck == true and LevelCheck == true and CooldownCheck == true then
						for i, pid in next, pids, nil do
							q.begin_other_pc_block(pid)
							pc.remove_item(71095, 1)
							q.end_other_pc_block()
						end
						d.new_jump_party(352, (5120+171), (1536+271))
						if d.select(d.get_map_index()) then
							d.spawn_mob_dir(20397, 173, 262, 1)
							d.setf("DungeonBlock", 1)
							d.setf("first_regen", 1)
						end
					end
				else
					return
				end
			end
		end

		when 20397.chat."Zindana gir" begin
			if d.select(pc.get_map_index()) then
				say_npc()
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					server_timer("nemere_1", 1, d.get_map_index())
					npc.purge()
				else
					return
				end
			end
		end

		when nemere_1.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("first_regen") == 1 then
					d.setf("first_regen", 0)
					server_loop_timer("nemere_loop_timer", 10, arg)
					d.regen_file("data/dungeon/ice_dungeon/zone_1.txt")
					server_timer("nemere_full_timer", 60*60, arg)
					d.setf("real_time", get_global_time()+60*60)
					d.setf("level", 1)
					GetMissionInfo(1, 60*60)
					d.notice(arg, 657, "", false)
				end
			end
		end

		when login with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 begin
			pc.set_warp_location_local(61, 4319, 1647)
			local index = pc.get_map_index()
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 1 then
					cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time") - get_global_time()))
				else
					pc.warp(432100, 166700)
				end
			end
		end

		when nemere_full_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.setf("DungeonBlock", 0)
				d.global_warp_all_to_base(4319*100, 1647*100, index, 60)
			end
		end

		when nemere_loop_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				local level = d.getf("level")
				local mob_count = d.count_monster()

				if level == 1 and mob_count <= 5 then
					d.notice(d.get_map_index(), 658, "", false)
					server_timer("Jp_2", 5, d.get_map_index())
					GetMissionInfo(1, 5)
				elseif level == 3 and mob_count <= 5 then
					d.notice(d.get_map_index(), 658, "", false)
					server_timer("Jp_4", 5, d.get_map_index())
					GetMissionInfo(3, 5)
				elseif level == 4 and mob_count <= 5 then
					d.notice(d.get_map_index(), 658, "", false)
					server_timer("Jp_5", 5, d.get_map_index())
					GetMissionInfo(5, 5)
				elseif level == 6 and mob_count <= 5 then
					d.notice(d.get_map_index(), 659, "", false)
					d.regen_file("data/dungeon/ice_dungeon/zone_6.txt")
					d.spawn_mob(8058, 570, 649)
				end
			end
		end

		when Jp_2.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 1 then
					d.clear_regen()
					d.purge()
					d.setf("level", 2)
					d.jump_all((5120+761), (1536+270))
					d.set_regen_file("data/dungeon/ice_dungeon/zone_2.txt")
					d.notice(d.get_map_index(), 721, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when 30331.use with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 2 and d.getf("Frost_Key") == 0 then
					if number(1, 15) == 1 then
						d.notice(d.get_map_index(), 658, "", false)
						server_timer("Jp_3", 5, d.get_map_index())
						GetMissionInfo(2, 5)
						d.setf("Frost_Key", 1)
					else
						d.notice(d.get_map_index(), 722, "", false)
					end
				end
				item.remove()
			end
		end

		when Jp_3.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 2 then
					d.clear_regen()
					d.purge()
					d.setf("level", 3)
					d.setf("Frost_Key", 0)
					d.dungeon_remove_all(30331)
					d.regen_file("data/dungeon/ice_dungeon/zone_3.txt")
					d.jump_all((5120+187), (1536+491))
					d.notice(d.get_map_index(), 657, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when Jp_4.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 3 then
					d.clear_regen()
					d.purge()
					d.setf("level", 4)
					d.jump_all((5120+421), (1536+259))
					d.regen_file("data/dungeon/ice_dungeon/zone_4.txt")
					d.notice(d.get_map_index(), 657, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when Jp_5.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 4 then
					d.clear_regen()
					d.purge()
					d.setf("level", 5)
					d.jump_all((5120+419), (1536+530))
					d.set_regen_file("data/dungeon/ice_dungeon/zone_5.txt")
					d.notice(d.get_map_index(), 723, "", false)

					local c = {
					{432, 508}, {437, 493}, {448, 492}, {448, 476}, {467, 475}, {466, 464}, {462, 450},
					{458, 428}, {446, 439}, {431, 431}, {415, 416}, {402, 427}, {397, 438}, {375, 434},
					{374, 456}, {390, 468}, {385, 492}, {402, 505}, {404, 489}, {386, 482}}

					for i = 1, 6, 1 do
						local n = number(1, table.getn(c))
						d.set_unique("stone5_"..i, d.spawn_mob(20398, c[n][1], c[n][2]))
						table.remove(c, n)
					end

					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when 20398.take with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 and item.get_vnum() == 30332 begin
			if d.select(pc.get_map_index()) then
				local vid = npc.get_vid()
				if vid == d.get_unique_vid("stone5_1") then
					d.setf("stonekill", 2)
					say_new("Küp kayboldu!")
					item.remove()
				elseif vid == d.get_unique_vid("stone5_2") then
					if d.getf("stonekill") == 2 then
						say_new("Küp kayboldu!")
						d.setf("stonekill", 3)
						item.remove()
					else
						item.remove()
						say_new("Sira dogru degil!")
						return
					end
				elseif vid == d.get_unique_vid("stone5_3") then
					if d.getf("stonekill") == 3 then
						say_new("Küp kayboldu!")
						d.setf("stonekill", 4)
						item.remove()
					else
						item.remove()
						say_new("Sira dogru degil!")
						return
					end
				elseif vid == d.get_unique_vid("stone5_4") then
					if d.getf("stonekill") == 4 then
						say_new("Küp kayboldu!")
						d.setf("stonekill", 5)
						item.remove()
					else
						item.remove()
						say_new("Sira dogru degil!")
						return
					end
				elseif vid == d.get_unique_vid("stone5_5") then
					if d.getf("stonekill") == 5 then
						say_new("Küp kayboldu!")
						d.setf("stonekill", 6)
						item.remove()
					else
						item.remove()
						say_new("Sira dogru degil!")
						return
					end
				else
					if d.getf("stonekill") == 6 then
						d.notice(d.get_map_index(), 658, "", false)
						server_timer("Jp_6", 5, d.get_map_index())
						GetMissionInfo(5, 5)
						item.remove()
						d.dungeon_remove_all(30332)
					else
						item.remove()
						say_new("Sira dogru degil!")
						return
					end
				end
			end
			npc.purge()
		end

		when Jp_6.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 5 then
					d.clear_regen()
					d.purge()
					d.dungeon_remove_all(30332)
					d.clear_regen()
					d.purge()
					d.setf("level", 6)
					d.regen_file("data/dungeon/ice_dungeon/zone_6.txt")
					d.jump_all((5120+571), (1536+706))
					d.notice(d.get_map_index(), 657, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when Jp_7.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 6 then
					d.setf("level", 7)
					d.clear_regen()
					d.purge()

					local c = {{752, 499}, {758, 479}, {772, 452}, {763, 444}, {750, 451}, {728, 441}, {726, 455}, {718, 482}, {715, 491}, {731, 473}, {748, 429}}
					local n = number(1, table.getn(c))

					d.spawn_mob(6151, c[n][1], c[n][2])
					d.jump_all((5120+746), (1536+534))
					d.regen_file("data/dungeon/ice_dungeon/zone_7.txt")
					d.notice(d.get_map_index(), 724, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when Jp_8.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 7 then
					d.clear_regen()
					d.purge()
					d.setf("level", 8)
					d.jump_all((5120+303), (1536+710))
					d.set_regen_file("data/dungeon/ice_dungeon/zone_8.txt")
					d.notice(d.get_map_index(), 721, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when 30333.use with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 8 and d.getf("Frostflower_Key") == 0 then
					if number(1,15) == 1 then
						d.setf("Frostflower_Key", 1)
						d.notice(d.get_map_index(), 658, "", false)
						server_timer("Jp_9", 5, d.get_map_index())
						GetMissionInfo(8, 5)
					else
						d.notice(d.get_map_index(), 722, "", false)
					end
				end
			end
			item.remove()
		end

		when Jp_9.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 8 then
					d.clear_regen()
					d.purge()
					d.setf("level", 9)
					d.setf("Frostflower_Key", 0)
					d.regen_file("data/dungeon/ice_dungeon/zone_9.txt")
					d.dungeon_remove_all(30333)
					d.jump_all((5120+848), (1536+693))
					d.notice(d.get_map_index(), 725, "", false)
					d.spawn_mob(20399, 849 ,655)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when Jp_Boss.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("level") == 9 then
					d.clear_regen()
					d.purge()
					d.spawn_group(6062, 928, 335, 5, 1, 1)
					d.setf("level", 10)
					d.jump_all((5120+927), (1536+391))
					d.regen_file("data/dungeon/ice_dungeon/zone_boss.txt")
					d.notice(d.get_map_index(), 726, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
				end
			end
		end

		when kill with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 begin
			if d.select(pc.get_map_index()) then
				local dwLevel = d.getf("level")
				local dwRace = npc.get_race()

				if dwLevel == 2 and dwRace >= 6101 and dwRace <= 6108 and 1 == number(1, 10) then
					game.drop_item(30331, 1)
				elseif dwLevel == 5 and dwRace >= 6101 and dwRace <= 6108 and 1 == number(1, 15) then
					game.drop_item(30332, 1)
				elseif dwLevel == 6 and dwRace == 8058 then
					server_timer("Jp_7", 5, d.get_map_index())
					d.notice(d.get_map_index(), 658, "", false)
					GetMissionInfo(7, 5)
				elseif dwLevel == 8 and dwRace >= 6101 and dwRace <= 6108 and 1 == number(1, 15) then
					game.drop_item(30333, 1)
				elseif dwLevel == 9 and dwRace == 20399 then
					server_timer("Jp_Boss", 5, d.get_map_index())
					d.notice(d.get_map_index(), 658, "", false)
					GetMissionInfo(9, 5)
				elseif dwLevel == 7 and dwRace == 6151 then
					d.notice(d.get_map_index(), 658, "", false)
					server_timer("Jp_8", 5, d.get_map_index())
					GetMissionInfo(7, 5)
					d.notice(d.get_map_index(), 658, "", false)
				elseif dwLevel == 10 and dwRace == 6191 then
					d.spawn_mob(30130, pc.get_local_x(), pc.get_local_y())
					server_timer("nemere_dead_timer", 1, d.get_map_index())
					d.setf("dungeon_empire", pc.get_empire())
				end
			end
		end

		when nemere_dead_timer.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("DungeonBlock") == 1 then
					d.notice(arg, 673, "", false)
					d.setqf2("snow_dungeon", "cooldown", get_global_time()+60*60)

					--Track Window Dungeon Status
					d.track_update(6191)

					d.setf("DungeonBlock", 0)
					d.global_warp_all_to_base(4319*100, 1647*100, arg, 60)
					GetMissionInfo(10, 60)
					clear_server_timer("nemere_1", arg)
					clear_server_timer("Jp_2", arg)
					clear_server_timer("Jp_3", arg)
					clear_server_timer("Jp_4", arg)
					clear_server_timer("Jp_5", arg)
					clear_server_timer("Jp_6", arg)
					clear_server_timer("Jp_7", arg)
					clear_server_timer("Jp_8", arg)
					clear_server_timer("Jp_Boss", arg)
					clear_server_timer("nemere_full_timer", arg)
					clear_server_timer("nemere_loop_timer", arg)
					d.setf("real_time", 0)
					d.setf("level", 0)
					d.setf("Frost_Key", 0)
					d.setf("Frostflower_Key", 0)
				end
			end
		end

		when logout with pc.get_map_index() >= 3520000 and pc.get_map_index() <= 3529990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 1 then
					pc.setf("snow_dungeon", "map", d.get_map_index())
					pc.setf("snow_dungeon", "time", get_global_time()+15*60)
					pc.setf("snow_dungeon", "channel", pc.get_channel_id())
				else
					pc.setf("snow_dungeon", "map", 0)
					pc.setf("snow_dungeon", "time", 0)
					pc.setf("snow_dungeon", "channel", 0)
				end
			end
		end

		when 20395.chat."Geri dön..." with pc.getf("snow_dungeon", "map") >= 1 begin
			local dungeonChannel = pc.getf("snow_dungeon", "channel")
			if dungeonChannel == pc.get_channel_id() then
				local dungeonMapIndex = pc.getf("snow_dungeon", "map")
				if d.select(dungeonMapIndex) then
					local level = d.getf_from_map_index("level", dungeonMapIndex)
					if get_global_time() > pc.getf("snow_dungeon", "time") then
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("snow_dungeon", "map", 0)
						pc.setf("snow_dungeon", "time", 0)
						pc.setf("snow_dungeon", "channel", 0)
						return
					end

					local warp_to_level = {171, 271}
					if level == 1 then
						warp_to_level = {171, 271}
					elseif level == 2 then
						warp_to_level = {761, 270}
					elseif level == 3 then
						warp_to_level = {187, 491}
					elseif level == 4 then
						warp_to_level = {421, 259}
					elseif level == 5 then
						warp_to_level = {419, 530}
					elseif level == 6 then
						warp_to_level = {571, 706}
					elseif level == 7 then
						warp_to_level = {746, 534}
					elseif level == 8 then
						warp_to_level = {303, 710}
					elseif level == 9 then
						warp_to_level = {848, 693}
					elseif level == 10 then
						warp_to_level = {927, 391}
					end

					pc.warp((5120+warp_to_level[1])*100, (1536+warp_to_level[2])*100, dungeonMapIndex)
				else
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")
					pc.setf("snow_dungeon", "map", 0)
					pc.setf("snow_dungeon", "time", 0)
					pc.setf("snow_dungeon", "channel", 0)
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
				return
			end
		end
	end
end
