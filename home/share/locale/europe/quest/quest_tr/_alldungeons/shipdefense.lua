------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest shipdefense begin
	state start begin
		when logout with pc.get_map_index() >= 3550000 and pc.get_map_index() <= 3559990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 0 or d.getf("Complete") == 1 then
					pc.setf("shipdefense", "map", 0)
					pc.setf("shipdefense", "time", 0)
					pc.setf("shipdefense", "channel", 0)
				else
					pc.setf("shipdefense", "channel", pc.get_channel_id())
					pc.setf("shipdefense", "map", d.get_map_index())
					pc.setf("shipdefense", "time", get_global_time()+15*60)
				end
			end
		end

		when 9009.chat."Gemi Savunmasi" with pc.get_map_index() == 301 begin
			say_title(mob_name(9009))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				say_title(mob_name(9009))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then

					if pc.get_level() < 100 or pc.get_level() > 120 then
						say_title(mob_name(9009))
						say_new("[ENTER]Girmek için gerekli seviyeye sahip degilsiniz! [ENTER]")
						return
					end

					if not pc.is_gm() and pc.getf("shipdefense", "cooldown") > get_global_time() then
						local r_cooldown = pc.getf("shipdefense", "cooldown") - get_global_time()
						say_title(mob_name(9009))
						say_new("")
						say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
						return
					end

					if pc.count_item(71095) < 1 then
						say_title(mob_name(9009))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(71095, 1)
						return
					else
						pc.remove_item(71095, 1)
						d.new_jump(355, 4929*100, 6515*100)
						if d.select(d.get_map_index()) then
							d.setf("first_regen", 1)
							d.setf("DungeonBlock", 1)
						end
					end
				else
					return
				end

			elseif x == 2 then
				say_title(mob_name(9009))
				say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					if not party.is_party() then
						say_title(mob_name(9009))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					if party.get_near_count() < 2 then
						say_title(mob_name(9009))
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

						if pc.get_level() < 100 then
							table.insert(user_fail_level, pc.get_name())
							LevelCheck = false
						end

						if pc.count_item(71095) < 1 then
							table.insert(user_fail_ticket, pc.get_name())
							ticketCheck = false
						end

						if not pc.is_gm() and pc.getf("shipdefense", "cooldown") > get_global_time() then
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
						d.new_jump_party(355, 4929, 6515)
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

		when login with pc.get_map_index() >= 3550000 and pc.get_map_index() <= 3559990 begin
			pc.set_warp_location_local(301, 11082, 17824)
			local index = pc.get_map_index()
			if d.select(index) then
				if d.getf("DungeonBlock") == 1 then
					if d.getf("first_regen") == 1 then
						d.setf("first_regen", 0)
						server_timer("hydra_0", 0, index)
					end
					cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time")-get_global_time()))
				else
					pc.warp(1108200, 1782400)
				end
			end
		end

		when hydra_0.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				d.notice(index, 727, "", false)
				d.regen_file("data/dungeon/hydra/hidra.txt")
				server_timer("hydra_kill", 5, index)
				d.regen_file("data/dungeon/hydra/wall.txt")
			end
		end

		when hydra_kill.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.clear_regen()
				d.new_kill_all()
				d.setf("rudder", 1)
				d.regen_file("data/dungeon/hydra/wall.txt")
				d.regen_file("data/dungeon/hydra/dumen.txt")
			end
		end

		when 9015.click with pc.get_map_index() >= 3550000 and pc.get_map_index() <= 3559990 and d.getf("rudder") == 1 begin
			if d.select(pc.get_map_index()) then
				say_title(mob_name(9015))
				say_new("[ENTER]Dümeni yok olmamasi için savunun! [ENTER]")
				d.setf("rudder", 0)
				server_timer("hydra_1", 30, d.get_map_index())
				GetMissionInfo(0, 30)
				d.setf("real_time", get_global_time()+30)
				d.spawn_mob_new(6801, 321, 159)
				d.notice(d.get_map_index(), 728, "", false)
			end
		end

		when hydra_1.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				d.new_kill_all()
				d.clear_regen()
				d.setf("level", 1)
				server_timer("hydra_full_timer", 60*10, index)
				GetMissionInfo(1, 60*10)
				d.setf("real_time", get_global_time()+60*10)
				d.notice(index, 729, "", false)
				d.regen_file("data/dungeon/hydra/first.txt")
				d.regen_file("data/dungeon/hydra/spawn3.txt")
				d.regen_file("data/dungeon/hydra/spawn1.txt")
				d.regen_file("data/dungeon/hydra/spawn2.txt")
			end
		end

		when hydra_full_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.setf("DungeonBlock", 0)
				d.global_warp_all_to_base(1108200, 1782400, get_server_timer_arg(), 0)
			end
		end

		when 6812.kill with pc.get_map_index() >= 3550000 and pc.get_map_index() <= 3559990 begin
			if d.select(pc.get_map_index()) then
				local level = d.getf("level")
				if level == 1 then
					local index = d.get_map_index()
					d.clear_regen()
					d.new_kill_all()
					d.notice(index, 730, "", false)
					server_timer("hydra_2", 30, index)
					GetMissionInfo(1, 30)
					d.setf("real_time", get_global_time()+30)
					d.setf("level", 0)
					server_timer("defensawe_clear", 1, index)
					server_timer("jump_all", 3, index)
					clear_server_timer("hydra_full_timer", index)
				elseif level == 2 then
					local index = d.get_map_index()
					server_timer("jump_all", 3, index)
					d.notice(index, 731, "", false)
					server_timer("hydra_3", 30, index)
					GetMissionInfo(2, 30)
					d.setf("real_time", get_global_time()+30)
					d.setf("level", 0)
					server_timer("defensawe_clear", 1, index)
					clear_server_timer("hydra_full_timer", index)
				elseif level == 3 then
					local index = d.get_map_index()
					d.notice(index, 732, "", false)
					server_timer("defensawe_clear_end", 1, index)
					clear_server_timer("hydra_full_timer", index)
				else
					return
				end
			end
		end

		when defensawe_clear_end.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.new_kill_all()
				d.regen_file("data/dungeon/hydra/sandik.txt")
				d.clear_regen()
			end
		end

		when jump_all.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.jump_all(4929, 6515)
			end
		end

		when defensawe_clear.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.new_kill_all()
				d.clear_regen()
				d.regen_file("data/dungeon/hydra/wall.txt")
				d.clear_regen()
			end
		end

		when defensawe_destroy.timer begin
			if d.select(get_server_timer_arg()) then
				d.new_kill_all()
				d.kill_all()
				d.global_warp_all_to_base(1108200, 1782400, pc.get_map_index(), 15)
			end
		end

		when hydra_2.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				d.new_kill_all()
				d.notice(index, 733, "", false)
				d.setf("level", 2)
				d.regen_file("data/dungeon/hydra/second.txt")
				server_timer("hydra_full_timer", 60*10, index)
				GetMissionInfo(2, 60*10)
				d.setf("real_time", get_global_time()+60*10)
				d.regen_file("data/dungeon/hydra/spawn3.txt")
				d.regen_file("data/dungeon/hydra/spawn1.txt")
				d.regen_file("data/dungeon/hydra/spawn2.txt")
				d.clear_regen()
			end
		end

		when hydra_3.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				d.new_kill_all()
				d.clear_regen()
				d.notice(index, 734, "", false)
				d.setf("level", 3)
				d.regen_file("data/dungeon/hydra/final.txt")
				server_timer("hydra_full_timer", 60*10, index)
				GetMissionInfo(3, 60*10)
				d.setf("real_time", get_global_time()+60*10)
				d.regen_file("data/dungeon/hydra/spawn3.txt")
				d.regen_file("data/dungeon/hydra/spawn1.txt")
				d.regen_file("data/dungeon/hydra/spawn2.txt")
			end
		end

		when 9018.kill with pc.get_map_index() >= 3550000 and pc.get_map_index() <= 3559990 begin
			if d.select(pc.get_map_index()) then
				server_timer("hydra_4", 1, d.get_map_index())
			d.setf("dungeon_empire", pc.get_empire())
			end
		end

		when hydra_4.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				if d.getf("DungeonBlock") == 1 then
					d.notice(index, 673, "", false)
					d.global_warp_all_to_base(1108200, 1782400, index, 60)
					GetMissionInfo(3, 60*1)
					d.setf("real_time", 0)
					d.setf("level", 0)
					d.setqf2("shipdefense", "cooldown", get_global_time()+3*60*60)

					--Track Window Dungeon Status
					d.track_update(9018)

					clear_server_timer("hydra_full_timer", index)
					clear_server_timer("hydra_0", index)
					clear_server_timer("hydra_1", index)
					clear_server_timer("hydra_2", index)
					clear_server_timer("hydra_3", index)
					clear_server_timer("defensawe_clear", index)
					clear_server_timer("jump_all", index)
					clear_server_timer("defensawe_clear_end", index)
					d.setf("DungeonBlock", 0)
				end
			end
		end

		when 9009.chat."Geri dön..." with pc.getf("shipdefense", "map") >= 1 and pc.get_map_index() == 301 begin
			local dungeonChannel = pc.getf("shipdefense", "channel")
			if dungeonChannel == pc.get_channel_id() then
				local dungeonMapIndex = pc.getf("shipdefense", "map")
				if d.select(dungeonMapIndex) then
					if get_global_time() > pc.getf("shipdefense", "time") then
						say_title(mob_name(9009))
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("shipdefense", "map", 0)
						pc.setf("shipdefense", "time", 0)
						pc.setf("shipdefense", "channel", 0)
						return
					end
					pc.warp(4929*100, 6515*100, dungeonMapIndex)
				else
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")
					pc.setf("shipdefense", "map", 0)
					pc.setf("shipdefense", "time", 0)
					pc.setf("shipdefense", "channel", 0)
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
				return
			end
		end
	end
end
