------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest spider_dungeon begin
	state start begin
		when 20371.chat."Barones'in Yuvasi" with pc.get_map_index() == 217 begin
			if pc.get_level() < 70 then
				say_title(mob_name(20371))
				say_new("[ENTER]Minimum seviye 70 ve maksimum 90'dir. [ENTER]")
				return
			end

			local cooldown = pc.getf("spider_dungeon", "cooldown")
			local real_time = get_global_time()
			local r_cooldown = pc.getf("spider_dungeon", "cooldown") - real_time
			if not pc.is_gm() and cooldown > real_time then
				say_title(mob_name(20371))
				say_new("")
				say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
				return
			end

			say_title(mob_name(20371))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				say_title(mob_name(20371))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local c = select("Evet", "Iptal")
				if c == 1 then
					if pc.count_item(30324) < 1 then
						say_title(mob_name(20371))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(30324, 1)
						return
					else
						pc.remove_item(30324, 1)
						d.new_jump(217, 881*100, 6148*100)
						if d.select(d.get_map_index()) then
							d.setf("DungeonBlock", 1)
							d.setf("first_regen", 1)
						end
					end
				else
					return
				end
			elseif x == 2 then
				if party.is_party() then
					if party.get_near_count() < 2 then
						say_title(mob_name(20371))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					say_title(mob_name(20371))
					say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

					local c = select("Evet", "Iptal")
					if c == 1 then
						if pc.count_item(30324) < 1 then
							say_title(mob_name(20371))
							say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
							say_item_vnum(30324, 1)
							return
						else
							local user_fail_level = {}
							local user_fail_cooldown = {}
							local user_fail_ticket = {}
							local LevelCheck = true
							local CooldownCheck = true
							local TicketCheck = true

							local pids = {party.get_member_pids()}

							for i, pid in next, pids, nil do
								q.begin_other_pc_block(pid)

								if pc.get_level() < 50 then
									table.insert(user_fail_level, pc.get_name())
									LevelCheck = false
								end

								if pc.getf("spider_dungeon", "cooldown") > get_global_time() then
									table.insert(user_fail_cooldown, pc.get_name())
									CooldownCheck = false
								end

								if pc.count_item(30324) < 1 then
									table.insert(user_fail_ticket, pc.get_name())
									TicketCheck = false
								end

								q.end_other_pc_block()
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

							if TicketCheck == false then
								say_new("Oyuncunun anahtari yok: [ENTER]")
								local a = 1
								for i, name in next, user_fail_ticket, nil do
									say_new(string.format("%d. %s", a, name))
									a = a+1
								end
								return
							else
								for i, pid in next, pids, nil do
									q.begin_other_pc_block(pid)
									pc.remove_item(30324, 1)
									q.end_other_pc_block()
								end
							end

							d.new_jump_party(217,881,6148)
							if d.select(d.get_map_index()) then
								d.setf("DungeonBlock", 1)
								d.setf("first_regen", 1)
							end
						end
					else
						return
					end
				else
					say_title(mob_name(20371))
					say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
					return
				end
			else
				return
			end
		end

		when barones_1.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("first_regen") == 1 then
					d.setf("first_regen", 0)
					d.spawn_mob(2095, 400, 566)
					d.spawn_mob(2095, 400, 594)
					d.spawn_mob(2095, 362, 600)
					d.spawn_mob(2095, 337, 599)
					d.spawn_mob(2095, 335, 581)
					d.spawn_mob(2095, 344, 562)
					d.spawn_mob(2095, 364, 588)
					d.spawn_mob(2095, 379, 562)
					d.spawn_mob(2095, 368, 525)

					local kingVid = d.spawn_mob(2092, 367, 588)
					d.setf("king_vid", kingVid)
					d.setf("remain_egg", 9)
					d.notice(arg, 674, "", false)

					GetMissionInfo(1, 20*60)

					d.setf("real_time", get_global_time()+20*60)
					d.setf("DungeonBlock", 1)
					server_timer("spider_full_timer", 60*20, arg)
				end
			end
		end

		when spider_full_timer.server_timer begin
			local index = get_server_timer_arg()
			if d.select(index) then
				d.purge()
				d.setf("DungeonBlock", 0)
				d.global_warp_all_to_base(689*100, 6111*100, index, 1)
			end
		end

		when 2095.kill with pc.get_map_index() >= 2170000 and pc.get_map_index() <= 2179990 begin
			if d.select(pc.get_map_index()) then
				d.notice(index, 675, "", false)
				d.notice(index, 676, "", false)
				local kingVid = d.getf("king_vid")
				local remain_egg = d.getf("remain_egg")
				d.setf("remain_egg", remain_egg-1)
				npc.set_vid_attack_mul(kingVid, 3/(remain_egg + 1))
				npc.set_vid_damage_mul(kingVid, 10/(remain_egg + 1))
			end
		end

		when 2092.kill with pc.get_map_index() >= 2170000 and pc.get_map_index() <= 2179990 begin
			server_timer("spider_barones_dead_timer", 1, d.get_map_index())
			d.spawn_mob(30130, pc.get_local_x()-5, pc.get_local_y()+5)
			d.setf("dungeon_empire", pc.get_empire())
		end

		when spider_barones_dead_timer.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("DungeonBlock") == 1 then
					GetMissionInfo(1, 60)
					d.global_warp_all_to_base(68900, 611100, arg, 60)
					d.notice(arg, 673, "", false)
					d.setqf2("spider_dungeon", "cooldown", get_global_time()+60*60)

					--Track Window Dungeon Status
					d.track_update(2092)

					d.setf("level", 0)
					d.setf("king_vid", 0)
					d.setf("remain_egg", 0)
					d.setf("real_time", 0)
					d.setf("DungeonBlock",0)
					clear_server_timer("spider_full_timer", arg)
					clear_server_timer("barones_1", arg)
				end
			end
		end

		when logout with pc.get_map_index() >= 2170000 and pc.get_map_index() <= 2179990 begin
			if d.getf("DungeonBlock") == 1 then
				pc.setf("spider_dungeon", "map", d.get_map_index())
				pc.setf("spider_dungeon", "time", get_global_time()+15*60)
				pc.setf("spider_dungeon", "channel", pc.get_channel_id())
			else
				pc.setf("spider_dungeon", "map", 0)
				pc.setf("spider_dungeon", "time", 0)
				pc.setf("spider_dungeon", "channel", 0)
			end
		end

		when login with pc.get_map_index() >= 2170000 and pc.get_map_index() <= 2179990 begin
			pc.set_warp_location_local(217, 689, 6111)
			local index = d.get_map_index()
			if index >= 2170000 and index <= 2179990 then
				if d.getf("DungeonBlock") == 1 then
					if d.getf("first_regen") == 1 then
						server_timer("barones_1", 1, index)
					end
				else
					pc.warp(68900, 611100)
				end
				cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time") - get_global_time()))
			end
		end

		when 20371.chat."Geri dön..." with pc.getf("spider_dungeon", "map") >= 1 begin
			local dungeonChannel = pc.getf("spider_dungeon", "channel")

			if pc.get_channel_id() == dungeonChannel then
				local dungeonMapIndex = pc.getf("spider_dungeon", "map")

				if d.select(dungeonMapIndex) then
					local dwLevel = d.getf_from_map_index("level", dungeonMapIndex)
					local ticket = d.getf_from_map_index("ticket", dungeonMapIndex)

					if get_global_time() > pc.getf("spider_dungeon", "time") then
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("spider_dungeon", "map", 0)
						pc.setf("spider_dungeon", "time", 0)
						pc.setf("spider_dungeon", "channel", 0)
						return
					end
					pc.warp(881*100, 6148*100, dungeonMapIndex)
				else
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")
					pc.setf("spider_dungeon", "map", 0)
					pc.setf("spider_dungeon", "time", 0)
					pc.setf("spider_dungeon", "channel", 0)
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
				return
			end
		end
	end
end
