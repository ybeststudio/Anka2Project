------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest dragonlair begin
	state start begin
		when 30121.chat."Ejderhanin Odasi" with pc.get_map_index() == 73 begin
			say_title(mob_name(30121))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				say_title(mob_name(30121))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then

					if pc.get_level() < 75 then
						say_title(mob_name(30121))
						say_new("[ENTER]Girmek için gerekli seviyeye sahip degilsiniz! [ENTER]")
						return
					end

					if not pc.is_gm() and pc.getf("dragonlair", "cooldown") > get_global_time() then
						local r_cooldown = pc.getf("dragonlair", "cooldown") - get_global_time()
						say_title(mob_name(30121))
						say_new("")
						say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
						return
					end

					if pc.count_item(30179) < 3 then
						say_title(mob_name(30121))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(30179, 3)
						return
					else
						pc.remove_item(30179, 3)
						d.new_jump(208, 8436*100, 10669*100)
						d.setf("first_regen", 1)
						d.setf("DungeonBlock", 1)
					end
				else
					return
				end
			elseif x == 2 then
				say_title(mob_name(30121))
				say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					if not party.is_party() then
						say_title(mob_name(30121))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					if party.get_near_count() < 2 then
						say_title(mob_name(30121))
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

						if pc.get_level() < 75 then
							table.insert(user_fail_level, pc.get_name())
							LevelCheck = false
						end

						if pc.count_item(30179) < 3 then
							table.insert(user_fail_ticket, pc.get_name())
							ticketCheck = false
						end

						if pc.getf("dragonlair", "cooldown") > get_global_time() then
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
						say_item_vnum(30179, 3)
						return
					end

					if LevelCheck == false then
						say_new("Oyuncunun seviyesi çok düsük: [ENTER]")
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
							if pc.count_item(30179) >= 3 then
								pc.remove_item(30179, 3)
							end
							q.end_other_pc_block()
						end

						d.new_jump_party(208, 8436, 10669)
						d.setf("first_regen", 1)
						d.setf("DungeonBlock", 1)
					end
				else
					return
				end
			end
		end

		when logout with pc.get_map_index() >= 2080000 and pc.get_map_index() <= 2089990 begin
			if d.getf("DungeonBlock") == 1 then
				pc.setf("dragonlair", "map", d.get_map_index())
				pc.setf("dragonlair", "time", get_global_time()+15*60)
				pc.setf("dragonlair", "channel", pc.get_channel_id())
			else
				pc.setf("dragonlair", "map", 0)
				pc.setf("dragonlair", "time", 0)
				pc.setf("dragonlair", "channel", 0)
			end
		end

		when login with pc.get_map_index() >= 2080000 and pc.get_map_index() <= 2089990 begin
			pc.set_warp_location_local(73, 1800, 12199)
			local real_time = get_global_time()
			local index = d.get_map_index()
			if index >= 2080000 and index <= 2089990 then
				if not pc.is_gm() and pc.getf("dragonlair", "cooldown") > get_global_time() then
					pc.setf("dragonlair", "map",0)
					pc.setf("dragonlair", "time",0)
					pc.setf("dragonlair", "channel",0)
					pc.warp(1800*100, 12199*100)
					return
				end
				if d.getf("DungeonBlock") == 1 then
					if d.getf("first_regen") == 1 then
						server_timer("beran_1", 1, index)
					end
					cmdchat(string.format("RefreshDungeonTimer %d %d", 1, d.getf("real_time") - real_time))
				else
					pc.warp(1800*100, 12199*100)
				end
			end
		end

		when beran_1.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("first_regen") == 1 then
					d.setf("first_regen", 0)
					d.spawn_mob(8034, 188, 178)
					d.spawn_mob(8034, 177, 178)
					d.spawn_mob(8034, 177, 168)
					d.spawn_mob(8034, 188, 168)
					d.notice(arg, 677, "", false)
					server_timer("beran_full_timer", 60*30, arg)
					d.setf("real_time", get_global_time()+60*30)
					GetMissionInfo(1, 60*30)
					d.regen_file("data/dungeon/dragonlair/regen.txt")
					d.setf("stones", 0)
					d.setf("step", 0)
				end
			end
		end

		when beran_full_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.setf("DungeonBlock", 0)
				d.setf("IsOn", 0)
				d.setf("step", 0)
				d.setf("stones", 0)
				d.setf("level", 0)
				d.setf("real_time", 0)
				d.global_warp_all_to_base(1800*100, 12199*100, get_server_timer_arg(), 1)
			end
		end

		when 8034.kill with pc.get_map_index() >= 2080000 and pc.get_map_index() <= 2089990 begin
			local new = d.getf("stones")+1
			d.setf("stones", new)
			if new == 4 then
				d.setf("stones", 0)
				local step = d.getf("step")+1
				d.setf("step", step)
				if step == 3 then
					d.notice(d.get_map_index(), 678, "", false)
					d.spawn_group(2430, 183, 173, 0, 1, 1)
				else
					d.spawn_mob(2491, 156, 174)
				end
			end
		end

		when 2491.kill with pc.get_map_index() >= 2080000 and pc.get_map_index() <= 2089990 begin
			d.spawn_mob(8034, 188, 178)
			d.spawn_mob(8034, 177, 178)
			d.spawn_mob(8034, 177, 168)
			d.spawn_mob(8034, 188, 168)
		end

		when 2493.kill with pc.get_map_index() >= 2080000 and pc.get_map_index() <= 2089990 begin
			d.spawn_mob(30130, pc.get_local_x()-5, pc.get_local_y()+5)
			server_timer("beran_dead_timer", 1, d.get_map_index())
			d.setf("dungeon_empire", pc.get_empire())
		end

		when beran_dead_timer.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("DungeonBlock") == 1 then
					d.setf("DungeonBlock", 0)
					d.notice(arg, 673, "", false)
					GetMissionInfo(1, 60)
					d.setf("IsOn", 0)
					d.setf("step", 0)
					d.setf("stones", 0)
					d.setf("level", 0)
					d.setf("real_time", 0)
					d.setqf2("dragonlair", "cooldown", get_global_time()+60*60)

					--Track Window Dungeon Status
					d.track_update(2493)

					clear_server_timer("beran_full_timer", arg)
					clear_server_timer("beran_1", arg)
					d.global_warp_all_to_base(1800*100, 12199*100, arg, 60)
				end
			end
		end

		when 30121.chat."Geri dön..." with pc.getf("dragonlair","map") >= 1 begin
			if get_global_time() > pc.getf("dragonlair", "time") then
				say_title(mob_name(30121))
				say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")

				pc.setf("dragonlair", "map", 0)
				pc.setf("dragonlair", "time", 0)
				pc.setf("dragonlair", "channel", 0)
				return
			end

			local dungeonChannel = pc.getf("dragonlair", "channel")
			if dungeonChannel == pc.get_channel_id() then
				local dungeonMap = pc.getf("dragonlair", "map")
				if d.select(dungeonMap) then
					say_title(mob_name(30121))
					say_new("[ENTER]Geri dönmek istiyor musunuz? [ENTER]")

					local x = select("Evet", "Iptal")
					if x == 1 then
						pc.warp(8436*100, 10669*100, dungeonMap)
					else
						return
					end
				else
					say_title(mob_name(30121))
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")

					pc.setf("dragonlair", "map", 0)
					pc.setf("dragonlair", "time", 0)
					pc.setf("dragonlair", "channel", 0)
					return
				end
			else
				say_title(mob_name(30121))
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
			end
		end
	end
end
