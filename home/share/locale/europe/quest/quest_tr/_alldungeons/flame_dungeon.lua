------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest flame_dungeon begin
	state start begin
		when 20394.chat."Cehennem Arafi" begin
			say_title(mob_name(20394))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				say_title(mob_name(20394))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then

					if pc.get_level() < 90 then
						say_title(mob_name(20394))
						say_new("[ENTER]Girmek için gerekli seviyeye sahip degilsiniz! [ENTER]")
						return
					end

					if not pc.is_gm() and pc.getf("flame_dungeon", "cooldown") > get_global_time() then
						local r_cooldown = pc.getf("flame_dungeon", "cooldown") - get_global_time()
						say_title(mob_name(20394))
						say_new("")
						say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
						return
					end

					if pc.count_item(71095) >= 1 then
						pc.remove_item(71095, 1)
					else
						say_title(mob_name(20394))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(71095, 1)
						return
					end

					d.new_jump(351, 7766*100, 6719*100)
					d.setf("first_regen", 1)
					d.setf("DungeonBlock", 1)
				else
					return
				end
			elseif x == 2 then
				say_title(mob_name(20394))
				say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					if not party.is_party() then
						say_title(mob_name(20394))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					if party.get_near_count() < 2 then
						say_title(mob_name(20394))
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

						if pc.get_level() < 90 or pc.get_level() > 120 then
							table.insert(user_fail_level, pc.get_name())
							LevelCheck = false
						end

						if pc.count_item(71095) < 1 then
							table.insert(user_fail_ticket, pc.get_name())
							ticketCheck = false
						end

						if not pc.is_gm() and pc.getf("flame_dungeon", "cooldown") > get_global_time() then
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
						d.new_jump_party(351, 7766, 6719)
						if d.select(d.get_map_index()) then
							d.setf("first_regen", 1)
							d.setf("DungeonBlock", 1)
						end
					end
				else
					return
				end
			end
		end

		when login with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			pc.set_warp_location_local(62, 5984, 7073)
			local index = pc.get_map_index()
			if d.select(index) then
				if d.getf("DungeonBlock") == 1 then
					if d.getf("first_regen") == 1 then
						server_timer("razador_1", 1, index)
					end
					cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time") - get_global_time()))
				else
					pc.warp(598000, 707500)
				end
			end
		end

		when razador_1.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("first_regen") == 1 then
					d.setf("first_regen", 0)
					d.regen_file("data/dungeon/flame_dungeon/npc.txt")

					local doors_pos = {{320, 394, 135}, {293, 359, 90}, {333, 321, 210}, {378, 320, 152}, {400, 355, 90}, {394, 401, 223}}
					local idoors_pos = {{268, 447, 135}, {234, 359, 90}, {300, 264, 210}, {454, 217, 135}, {470, 355, 90}, {467, 469, 239}}

					for i=1,6 do
						d.set_unique("door"..i, d.spawn_mob_ac_dir(20387, doors_pos[i][1], doors_pos[i][2], doors_pos[i][3]))
					end

					for i=1,6 do
						d.set_unique("idoor"..i, d.spawn_mob_ac_dir(20388, idoors_pos[i][1], idoors_pos[i][2], idoors_pos[i][3]))
					end

					d.setf("counter_11", 180)
					d.setf("counter_13", 180)
					d.setf("level", 1)
					server_timer("flame_dungeon_timer", 60*60, arg)
					d.setf("real_time", get_global_time()+60*60)
					d.notice(arg, 713, "", false)
				end
			end
		end

		when 20385.chat."Am-heh'i Uyandir" begin
			local index = pc.get_map_index()
			if d.select(index) then
				local level = d.getf("level")
				local is_on = d.getf("is_on")

				if level == 1 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")

						d.notice(index, 714, "", false)
						GetMissionInfo(1, d.getf("real_time") - get_global_time())

						d.kill_unique("door1")
						d.kill_unique("idoor1")
						d.regen_file ("data/dungeon/flame_dungeon/fd_a.txt")
						d.setf("is_on", 1)
						return
					else
						say_new("Odadaki tüm seytanlari yenmediniz.")
						return
					end

				elseif level == 2 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")
						d.spawn_mob(20386, 195, 352)
						d.notice(index, 715, "", false)
						d.kill_unique("door2")
						d.kill_unique("idoor2")
						GetMissionInfo(2, d.getf("real_time") - get_global_time())
						d.setf("is_on", 1)
						d.set_regen_file ("data/dungeon/flame_dungeon/fd_b.txt")
						return
					else
						say_new("Altin Disli Çark bulamadiniz.")
					end

				elseif level == 3 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")
						d.notice(index, 714, "", false)
						d.kill_unique("door3")
						d.kill_unique("idoor3")
						GetMissionInfo(3, d.getf("real_time") - get_global_time())
						d.setf("is_on", 1)
						d.regen_file ("data/dungeon/flame_dungeon/fd_c.txt")
						return
					else
						say_new("Odadaki tüm seytanlari yenmediniz.")
						return
					end

				elseif level == 4 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")
						d.notice(index, 716, "", false)
						d.kill_unique("door4")
						d.kill_unique("idoor4")
						GetMissionInfo(4, d.getf("real_time") - get_global_time())
						d.setf("is_on", 1)
						d.set_regen_file ("data/dungeon/flame_dungeon/fd_d.txt")
						d.spawn_mob(6051, 470, 175)
						return
					else
						say_new("Ignitor'u yenmediniz.")
						return
					end

				elseif level == 5 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")
						d.setf("is_on", 1)
						d.notice(index, 717, "", false)
						d.kill_unique("door5")
						d.kill_unique("idoor5")
						d.setf("level", 5)
						GetMissionInfo(5, d.getf("real_time") - get_global_time())
						d.set_regen_file ("data/dungeon/flame_dungeon/fd_e.txt")

						local vis = {0, 0, 0, 0, 0, 0, 0}
						local STONE_pos = {{486, 345}, {511, 336}, {525, 349}, {521, 365}, {503, 372}, {486, 365}, {500, 354}}

						for i=1,7 do
							vis[i] = 0
						end

						for i=1,7 do
							local ran = number(1, 7)
							local st = 0

							for j=1,50 do
								st = st + 1

								if st > 7 then
									st = 1
								end

								if vis[st] == 0 then
									ran = ran - 1
									if ran == 0 then
										vis[st] = 1
										d.set_unique("stone5_"..st, d.spawn_mob(20386, STONE_pos[i][1], STONE_pos[i][2]))
										break
									end
								end
							end
						end
						return
					else
						say_new("Açilacak damgalariniz hala var. Acele edin!")
						return
					end

				elseif level == 6 then
					if is_on == 0 then
						say_new("Bir kapi açtiniz.")
						d.setf("is_on", 1)
						d.notice(index, 718, "", false)
						d.kill_unique("door6")
						d.kill_unique("idoor6")
						GetMissionInfo(6, d.getf("real_time") - get_global_time())
						d.set_regen_file ("data/dungeon/flame_dungeon/fd_f.txt")
						d.spawn_mob(8057, 511, 480)
						return
					else
						say_new("Araf Atesi Metin'ini yok etmediniz.")
						return
					end

				elseif level == 7 then
					say_new("Bir kapi açtiniz.")
					if d.getf("spawn") == 0 then
						server_timer("razador_2", 1, d.get_map_index())
					else
						return
					end
				else
					return
				end
			end
		end

		when razador_2.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 7 then
					if d.getf("spawn") == 0 then
						d.setf("spawn", 1)
						d.spawn_mob(6091, 686, 637)
						d.notice(d.get_map_index(), 719, "", false)
					else
						return
					end
					d.jump_all(8109,6867)
				end
			end
		end

		when flame_dungeon_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.global_warp_all_to_base(5984*100, 7073*100, index, 60)
				d.setf("DungeonBlock", 0)
			end
		end

		when razador_3.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				if d.getf("DungeonBlock") == 1 then
					d.clear_regen()
					d.notice(index, 673, "", false)
					d.setqf2("flame_dungeon","cooldown",get_global_time()+60*60)
					GetMissionInfo(7, 60)
					d.global_warp_all_to_base(5984*100, 7073*100, index, 60)
					d.setf("DungeonBlock", 0)
					d.setf("is_on", 0)
					d.setf("level", 0)
					d.setf("real_time", 0)
					d.setf("spawn", 0)
					d.setf("counter_11", 0)
					d.setf("counter_13", 0)
					cleartimer("Purge")
					clear_server_timer("razador_1", index)
					clear_server_timer("razador_2", index)
					clear_server_timer("flame_dungeon_timer", index)
				end
			end
		end

		when kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and d.getf("level") == 1 begin
			if d.select(pc.get_map_index()) then
				local counter = d.getf("counter_11")
				if counter == 0 then
					d.notice(d.get_map_index(), 720, "", false)
					d.setf("level", 2)
					d.clear_regen()
					timer("Purge", 2)
					d.setf("is_on", 0)
				else
					d.setf("counter_11", counter-1)
				end
			end
		end

		when kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and d.getf("level") == 2 begin
			if d.select(pc.get_map_index()) then
				if d.getf("is_on") == 1 then
					local i = number(1, 125)
					if i == 1 then
						game.drop_item (30329, 1)
					end
				end
			end
		end

		when kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and d.getf("level") == 3 begin
			if d.select(pc.get_map_index()) then
				if d.getf("counter_13") == 0 then
					d.clear_regen()
					d.notice(d.get_map_index(), 720, "", false)
					d.setf("level", 4)
					d.setf("is_on", 0)
					timer("Purge", 2)
				else
					d.setf("counter_13", d.getf("counter_13")-1)
				end
			end
		end

		when kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and d.getf("level") == 5 begin
			if d.select(pc.get_map_index()) then
				if d.getf("is_on") == 1 then
					local i = number(1, 30)
					if i == 1 then
						game.drop_item (30330, 1)
					end
				end
			end
		end

		when 6051.kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 4 then
					d.clear_regen()
					d.notice(d.get_map_index(), 720, "", false)
					timer("Purge", 2)
					d.setf("level", 5)
					d.setf("is_on", 0)
				end
			end
		end

		when 8057.kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 6 then
					d.clear_regen()
					d.setf("level", 7)
					d.setf("is_on", 0)
					d.notice(d.get_map_index(), 720, "", false)
					timer("Purge", 2)
				end
			end
		end

		when 6091.kill with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 7 then
					server_timer("razador_3", 1, d.get_map_index())
					d.spawn_mob(30130, pc.get_local_x(), pc.get_local_y())
					d.setf("dungeon_empire", pc.get_empire())

					--Track Window Dungeon Status
					d.track_update(6091)
				end
			end
		end

		when Purge.timer with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			if d.select(pc.get_map_index()) then
				d.kill_area(750000, 620000, 817400, 689400)
			end
		end

		when 20386.take with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and item.get_vnum() == 30329 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 2 then
					local i = number(1, 5)
					if i == 1 then
						npc.purge()
						item.remove()
						d.setf("level", 3)
						d.clear_regen()
						timer("Purge", 2)
						d.setf("is_on", 0)
						d.notice(d.get_map_index(), 720, "", false)
						d.dungeon_remove_all(30329)
					else
						item.remove()
						say_new("Parça kirildi!")
					end
				end
			end
		end

		when 20386.take with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 and item.get_vnum() == 30330 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 5 then
					local vid = npc.get_vid()
					if d.count_monster() < 100 then
						d.regen_file ("data/dungeon/flame_dungeon/fd_e.txt")
					end

					if vid == d.get_unique_vid("stone5_1") then
						npc.purge()
						item.remove()
						d.setf("stonekill", 2)
						say_new("Damga açildi!")
					elseif vid == d.get_unique_vid("stone5_2") then 
						if d.getf("stonekill") == 2 then
							npc.purge()
							item.remove()
							say_new("Damga açildi!")
							d.setf("stonekill", 3)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					elseif vid == d.get_unique_vid("stone5_3") then
						if d.getf("stonekill") == 3 then
							npc.purge()
							item.remove()
							say_new("Damga açildi!")
							d.setf("stonekill", 4)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					elseif vid == d.get_unique_vid("stone5_4") then
						if d.getf("stonekill") == 4 then
							npc.purge()
							item.remove()
							say_new("Damga açildi!")
							d.setf("stonekill", 5)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					elseif vid == d.get_unique_vid("stone5_5") then
						if d.getf("stonekill") == 5 then
							npc.purge()
							item.remove()
							say_new("Damga açildi!")
							d.setf("stonekill", 6)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					elseif vid == d.get_unique_vid("stone5_6") then
						if d.getf("stonekill") == 6 then
							npc.purge()
							item.remove()
							say_new("Damga açildi!")
							d.setf("stonekill", 7)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					else
						if d.getf("stonekill") == 7 then
							npc.purge()
							item.remove()
							d.notice(d.get_map_index(), 720, "", false)
							d.setf("level", 6)
							d.setf("is_on", 0)
							d.clear_regen()
							timer("Purge", 2)
							d.dungeon_remove_all(30330)
						else
							item.remove()
							say_new("Sira dogru degil!")
							return
						end
					end
				end
			end
		end

		when logout with pc.get_map_index() >= 3510000 and pc.get_map_index() <= 3519990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 1 then
					pc.setf("flame_dungeon", "map", d.get_map_index())
					pc.setf("flame_dungeon", "time", get_global_time()+15*60)
					pc.setf("flame_dungeon", "channel", pc.get_channel_id())
				else
					pc.setf("flame_dungeon", "map", 0)
					pc.setf("flame_dungeon", "time", 0)
					pc.setf("flame_dungeon", "channel", 0)
				end
			end
		end

		when 20394.chat."Geri dön..." with pc.getf("flame_dungeon", "map") >= 1 begin
			local dungeonChannel = pc.getf("flame_dungeon", "channel")
			if dungeonChannel == pc.get_channel_id() then
				local dungeonMapIndex = pc.getf("flame_dungeon", "map")
				if d.select(dungeonMapIndex) then
					local dwLevel = d.getf_from_map_index("level", dungeonMapIndex)
					if get_global_time() > pc.getf("flame_dungeon", "time") then
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("flame_dungeon", "map", 0)
						pc.setf("flame_dungeon", "time", 0)
						pc.setf("flame_dungeon", "channel", 0)
						return
					end
					if dwLevel == 7 then
						pc.warp(8109*100, 6867*100, dungeonMapIndex)
					else
						pc.warp(7766*100, 6719*100, dungeonMapIndex)
					end
				else
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")
					pc.setf("flame_dungeon", "map", 0)
					pc.setf("flame_dungeon", "time", 0)
					pc.setf("flame_dungeon", "channel", 0)
					return
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
				return
			end
		end
	end
end
