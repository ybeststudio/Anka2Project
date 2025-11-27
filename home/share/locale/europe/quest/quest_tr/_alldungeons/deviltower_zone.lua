------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest deviltower_zone begin
	state start begin

		function clear_deviltower_flags()
			d.setf("stone_destroy", 0)
			d.setf("bug_1", 0)
			d.setf("IsOn", 0)
			d.setf("stone_count", 0)
			d.setf("real_time", 0)
			d.setf("level", 0)
			d.setf("Sa_Soe", 0)
			d.setf("Zin_Sa_Gui", 0)
			d.setf("7_stone_kill", 0)
			d.setqf2("deviltower_zone", "can_refine", 0)
		end

		function get_4floor_stone_pos()
			local positions =
				{
					{368, 629}, {419, 630}, {428, 653}, {422, 679},
					{395, 689}, {369, 679}, {361, 658},
			}
			for i = 1, 6 do
				local j = number(i, 7)
				if i != j then
					local t = positions[i];
					positions[i] = positions[j];
					positions[j] = t;
				end
			end
			return positions
		end

		when 20348.chat."Seytan Kulesi" begin
			say_title(mob_name(20348))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local playerSelect = select("Yalniz", "Grupla", "Iptal")
			if playerSelect == 1 then
				say_title(mob_name(20348))
				say_new("[ENTER]Baslamak istiyor musunuz? [ENTER]")

				local x = select("Evet", "Iptal")
				if x == 1 then
					if not pc.is_gm() and pc.getf("deviltower_zone", "cooldown") > get_global_time() then
						local r_cooldown = pc.getf("deviltower_zone", "cooldown") - get_global_time()
						say_title(mob_name(20348))
						say_new("")
						say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
						return
					end

					d.new_jump(66, 2174*100, 7040*100)
					d.setf("DungeonBlock", 1)
					d.setf("level", 2)
				else
					return
				end
			elseif playerSelect == 2 then
				say_title(mob_name(20348))
				say_new("[ENTER]Grupla girmek istiyor musunuz? [ENTER]")

				local x = select("Evet", "Iptal")
				if x == 1 then
					if not party.is_party() then
						say_title(mob_name(20348))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end

					if party.get_near_count() < 2 then
						say_title(mob_name(20348))
						say_new("[ENTER]Bir grup üyesi müsait degil! [ENTER]")
						return
					end

					local user_fail_level = {}
					local user_fail_ticket = {}
					local user_fail_cooldown = {}
					local LevelCheck = true
					local CooldownCheck = true

					local pids = {party.get_member_pids()}

					for i, pid in next, pids, nil do
						q.begin_other_pc_block(pid)
						if pc.get_level() < 40 then
							table.insert(user_fail_level, pc.get_name())
							LevelCheck = false
						end

						if not pc.is_gm() and pc.getf("deviltower_zone", "cooldown") > get_global_time() then
							table.insert(user_fail_cooldown, pc.get_name())
							CooldownCheck = false
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
						say_new("")
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

					if LevelCheck == true and CooldownCheck == true then
						for i, pid in next, pids, nil do
							q.begin_other_pc_block(pid)
							q.end_other_pc_block()
						end

						d.new_jump_party(66, 2174, 7040)

						if d.select(d.get_map_index()) then
							d.spawn_mob_dir(20397, 173, 262, 1)
							d.setf("DungeonBlock", 1)
							d.setf("level", 2)
						end
					end
				else
					return
				end
			else
				return
			end
		end

		when devil_stone1_1.timer with pc.get_map_index() == 66 begin
			d.new_jump_all(66, special.devil_tower[1][1], special.devil_tower[1][2])
			d.setf("level", 2)
			d.setf("DungeonBlock", 1)
		end

		when login with pc.get_map_index() >= 660000 and pc.get_map_index() <= 670000 begin
			pc.set_warp_location_local(65, 5905, 1105)
			cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time") - get_global_time()))

			if d.getf("bug_1") == 0 then
				if d.getf("level") == 2 then
					d.regen_file("data/dungeon/deviltower/deviltower2_regen.txt")
					d.clear_regen()
					d.notice(d.get_map_index(), 657, "", false)
					d.setf("level", 2)

					GetMissionInfo(2, 60*60)
					d.setf("real_time", get_global_time()+60*60)
					server_timer('deviltower_fail', 60*60, d.get_map_index())

					d.setf("bug_1", 1)
					d.setf("IsOn", 1)
					d.setf("DungeonBlock", 1)
					server_loop_timer("devil_one_two_floor", 5, d.get_map_index())
				end
			end

			if d.getf("level") == 2 then
				pc.jump(2174*100, 7040*100)
			elseif d.getf("level") == 3 then
				pc.jump(special.devil_tower[2][1]*100, special.devil_tower[2][2]*100)
			elseif d.getf("level") == 4 then
				pc.jump(special.devil_tower[3][1]*100, special.devil_tower[3][2]*100)
			elseif d.getf("level") == 5 then
				pc.jump(special.devil_tower[4][1]*100, special.devil_tower[4][2]*100)
			elseif d.getf("level") == 6 then
				pc.jump(special.devil_tower[5][1]*100, special.devil_tower[5][2]*100)
			elseif d.getf("level") == 7 then
				pc.jump((2048+590)*100, (6656+638)*100)
			elseif d.getf("level") == 8 then
				pc.jump((2048+590)*100, (6656+403)*100)
			elseif d.getf("level") == 9 then
				pc.jump((2048+590)*100, (6656+155)*100)
			end
			if d.getf("DungeonBlock") == 0 then
				pc.warp(590500, 110500)
			end
		end

		when devil_one_two_floor.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.count_monster() < 1 then
					if d.getf("level") == 2 and d.getf("IsOn") == 1 then
						server_timer('Devil_3', 10, d.get_map_index())
						GetMissionInfo(2, 10)
						d.notice(d.get_map_index(), 658, "", false)
						d.setf("IsOn",0)
					elseif d.getf("level") == 3 and d.getf("IsOn") == 1 then
						server_timer('Devil_4', 10, d.get_map_index())
						GetMissionInfo(3, 10)
						d.notice(d.get_map_index(), 658, "", false)
						d.setf("IsOn", 0)
					end
				end
			end
		end

		when Devil_3.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 2 then
					d.jump_all(special.devil_tower[2][1], special.devil_tower[2][2])
					d.setf("level", 3)
					d.setf("IsOn", 1)
					d.regen_file("data/dungeon/deviltower/deviltower3_regen.txt")
					d.clear_regen()
					d.notice(d.get_map_index(), 657, "", false)
					GetMissionInfo(3, d.getf("real_time") - get_global_time())
				end
			end
		end

		when Devil_4.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 3 then
					clear_server_timer('Devil_3', get_server_timer_arg())
					d.jump_all(special.devil_tower[3][1], special.devil_tower[3][2])
					d.setf("level", 4)
					d.setf("IsOn", 1)
					d.regen_file("data/dungeon/deviltower/deviltower4_regen.txt")
					d.clear_regen()
					d.notice(d.get_map_index(), 659, "", false)
					GetMissionInfo(4, d.getf("real_time") - get_global_time())
				end
			end
		end

		when 8016.kill with pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 begin
			GetMissionInfo(4, 10)
			d.notice(d.get_map_index(), 658, "", false)
			server_timer('Devil_4_1', 10, d.get_map_index())
			clear_server_timer('devil_one_two_floor', d.get_map_index())
		end

		when Devil_4_1.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 4 then
					clear_server_timer('Devil_4', get_server_timer_arg())

					local positions = deviltower_zone.get_4floor_stone_pos()
					for i = 1, 6 do
						d.set_unique("fake" .. i , d.spawn_mob(8017, positions[i][1], positions[i][2]))
					end
					local vid = d.spawn_mob(8017, positions[7][1], positions[7][2])
					d.set_unique("real", vid)
					d.notice(d.get_map_index(), 660, "", false)
					GetMissionInfo(4, d.getf("real_time") - get_global_time())
				end
			end
		end

		when deviltower_fail.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.global_warp_all_to_base(5905*100, 1105*100, get_server_timer_arg(), 0)
				d.setf("DungeonBlock", 0)
			end
		end

		when Devil_5.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 4 then
					clear_server_timer('Devil_4_1', get_server_timer_arg())

					d.setf("level", 5)
					d.setf("stone_count", 5)
					d.jump_all(special.devil_tower[4][1], special.devil_tower[4][2])
					d.set_regen_file("data/dungeon/deviltower/deviltower5_regen.txt")
					d.notice(d.get_map_index(), 661, "", false)

					GetMissionInfo(5, d.getf("real_time") - get_global_time())

					d.spawn_mob(20073, 421, 452)
					d.spawn_mob(20073, 380, 460)
					d.spawn_mob(20073, 428, 414)
					d.spawn_mob(20073, 398, 392)
					d.spawn_mob(20073, 359, 426)
				end
			end
		end

		when Devil_6.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 5 then
					clear_server_timer('Devil_5', get_server_timer_arg())
					d.setf("level", 6)
					d.clear_regen()
					d.regen_file("data/dungeon/deviltower/deviltower6_regen.txt")
					d.jump_all(special.devil_tower[5][1], special.devil_tower[5][2])

					GetMissionInfo(6, d.getf("real_time") - get_global_time())

					d.notice(d.get_map_index(), 662, "", false)
				end
			end
		end

		when Devil_7.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 6 then
					clear_server_timer('Devil_7', get_server_timer_arg())

					d.spawn_mob(8018, 639, 658)
					d.spawn_mob(8018, 611, 637)
					d.spawn_mob(8018, 596, 674)
					d.spawn_mob(8018, 629, 670)
					d.setf("level", 7)
					d.notice(d.get_map_index(), 663, string.format("%d/%d", d.getf("7_stone_kill"), 4), false)

					GetMissionInfo(7, d.getf("real_time") - get_global_time())

					d.jump_all(2048+590, 6656+638)
				end
			end
		end

		when Devil_8.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 7 then
					clear_server_timer('Devil_7', get_server_timer_arg())
					d.setf("level", 8)
					d.jump_all(2048+590, 6656+403)
					d.set_regen_file("data/dungeon/deviltower/deviltower8_regen.txt")
					d.notice(d.get_map_index(), 664, "", false)

					GetMissionInfo(8, d.getf("real_time") - get_global_time())

					d.spawn_mob(20366, 640, 460)
				end
			end
		end

		when Devil_9.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 8 then
					clear_server_timer('Devil_8', get_server_timer_arg())
					d.setf("level", 9)
					d.jump_all(2048+590, 6656+155)
					d.regen_file("data/dungeon/deviltower/deviltower9_regen.txt")
					d.clear_regen()

					GetMissionInfo(9, d.getf("real_time") - get_global_time())

					d.notice(d.get_map_index(), 665, "", false)
				end
			end
		end

		when 8017.kill with pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 and d.getf("level") == 4 begin
			if d.is_unique_dead("real") then
				d.notice(d.get_map_index(), 666, "", false)
				d.notice(d.get_map_index(), 658, "", false)
				d.new_kill_all()
				d.clear_regen()
				server_timer('Devil_5', 10, d.get_map_index())
				GetMissionInfo(4, 10)
			else
				d.notice(d.get_map_index(), 667, "", false)
			end
		end

		when 8040.kill with pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 and d.getf("level") == 5 begin
			game.drop_item(50084, 1)
		end

		when devil_stone5.take with item.get_vnum() == 50084 and pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 and d.getf("level") == 5 begin
			npc.purge()
			item.remove()
			d.setf("stone_count", d.getf("stone_count")-1)
			if d.getf("stone_count") <= 0 then
				d.dungeon_remove_all(50084)
				GetMissionInfo(5, 10)
				d.notice(d.get_map_index(), 658, "", false)
				server_timer("Devil_6", 10, d.get_map_index())
			end
		end

		when devil_stone6.kill with pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 and d.getf("level") == 6 begin
			d.kill_all()
			d.clear_regen()
			d.notice(d.get_map_index(), 668, "", false)
			d.check_eliminated()

			server_timer("Devil_7", 10, d.get_map_index())
			d.new_kill_all()
			d.clear_regen()
			GetMissionInfo(5, 10)
		end

		when 8018.kill with d.getf("level") == 7 and pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 begin
			local stone = d.getf("7_stone_kill")+1
			d.setf("7_stone_kill", stone)
			d.notice(d.get_map_index(), 669, string.format("%d/%d", d.getf("7_stone_kill"), 4), false)
			if stone >= 4 then
				d.setf("7_stone_kill", 0)
				d.set_regen_file("data/dungeon/deviltower/deviltower7_regen.txt")
				d.notice(d.get_map_index(), 670, string.format("%d", 200), false)
				d.setf("stone_destroy", 200)
			end
		end

		when 8019.kill with d.getf("level") == 7 and pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 begin
			d.setf("stone_destroy", d.getf("stone_destroy")-1)
			d.notice(d.get_map_index(), 670, string.format("%d", d.getf("stone_destroy")), false)

			local pct = number(1, 10)
			if pct == 1 then
				game.drop_item(30302, 1)
			end

			if d.getf("stone_destroy") < 1 then
				d.notice(d.get_map_index(), 671, "", false)
				d.setf("stone_destroy", 0)
				d.clear_regen()
				server_timer("Devil_8", 10, d.get_map_index())
				GetMissionInfo(7, 10)
			end
		end

		when 30302.use with d.getf("level") == 7 and pc.get_map_index() >= 660000 and pc.get_map_index() < 670000  begin
			if d.getf("Zin_Sa_Gui") == 0 then
				d.new_kill_all()
				d.clear_regen()
				pc.remove_item(30302, 1)
				server_timer("Devil_8", 10, d.get_map_index())
				d.notice(d.get_map_index(), 658, "", false)
				d.setf("Zin_Sa_Gui", 1)
				GetMissionInfo(7, 10)
				d.dungeon_remove_all(30302)
			else
				d.notice(d.get_map_index(), 672, "", false)
				return
			end
		end

		when 1040.kill with d.getf("level") == 8 and pc.get_map_index() >= 660000 and pc.get_map_index() < 670000 begin
			local lucky = number(1, 20)
			if lucky == 1 then
				game.drop_item(30304, 1)
			end
		end

		when 20366.take with item.get_vnum() == 30304 and d.getf("level") == 8 begin
			if d.getf("Sa_Soe") == 0 then
				item.remove()
				d.new_kill_all()
				d.clear_regen()
				d.setf("Sa_Soe", 1)
				server_timer("Devil_9", 10, d.get_map_index())
				GetMissionInfo(8, 10)
				d.notice(d.get_map_index(), 658, "", false)
				d.dungeon_remove_all(30304)
			else
				d.notice(d.get_map_index(), 672, "", false)
				return
			end
		end

		when 1093.kill with d.getf("level") == 9 and pc.get_map_index() >= 660000 and pc.get_map_index() <= 670000 begin
			local x = pc.get_map_index()
			clear_server_timer('Devil_9', pc.get_map_index())
			d.global_warp_all_to_base(5905*100, 1105*100, pc.get_map_index(), 60)
			deviltower_zone.clear_deviltower_flags()
			GetMissionInfo(9, 60)
			d.setf("real_time", get_global_time()+1*60)
			d.setf("WeKillAzo", 1)
			d.setf("DungeonBlock", 0)
			d.notice(d.get_map_index(), 673, "", false)
			d.setf("dungeon_empire", pc.get_empire())
			d.setqf2("deviltower_zone", "cooldown", get_global_time()+60*60)

			--Track Window Dungeon Status
			d.track_update(1093)

			--BattlePass Complete Dungeon
			d.battlepass_update_progress(1)
		end

		when 20348.chat."Geri dön..." with pc.getf("deviltower_zone", "map") >= 1 begin
			local dungeonIndex = pc.getf("deviltower_zone", "map")
			local dungeonChannel = pc.getf("deviltower_zone", "channel")

			if pc.get_channel_id() == dungeonChannel then
				if d.select(dungeonIndex) then
					local dungeonFloor = d.getf_from_map_index("level", dungeonIndex)

					if get_global_time() > pc.getf("deviltower_zone", "time") then
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("deviltower_zone", "map", 0)
						pc.setf("deviltower_zone", "time", 0)
						pc.setf("deviltower_zone", "channel", 0)
						return
					end

					say_title(mob_name(20348))
					say_new("[ENTER]Geri dönmek istiyor musunuz? [ENTER]")

					local x = select("Evet", "Iptal")
					if x == 1 then
						if dungeonFloor == 2 then
							pc.warp(special.devil_tower[1][1]*100, special.devil_tower[1][2]*100, dungeonIndex)
						elseif dungeonFloor == 2 then
							pc.warp(special.devil_tower[2][1]*100, special.devil_tower[2][2]*100, dungeonIndex)
						elseif dungeonFloor == 3 then
							pc.warp(special.devil_tower[3][1]*100, special.devil_tower[3][2]*100, dungeonIndex)
						elseif dungeonFloor >= 4 and dungeonFloor <= 5 then
							pc.warp(special.devil_tower[4][1]*100, special.devil_tower[4][2]*100, dungeonIndex)
						else
							pc.warp(special.devil_tower[5][1]*100, special.devil_tower[5][2]*100, dungeonIndex)
						end
					else
						return
					end
				else
					say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
					pc.setf("deviltower_zone", "map", 0)
					pc.setf("deviltower_zone", "time", 0)
					pc.setf("deviltower_zone", "channel", 0)
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
				return
			end
		end

		when logout with pc.get_map_index() >= 660000 and pc.get_map_index() <= 670000 begin
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 1 then
					pc.setf("deviltower_zone", "map", d.get_map_index())
					pc.setf("deviltower_zone", "time", get_global_time()+15*60)
					pc.setf("deviltower_zone", "channel", pc.get_channel_id())
				else
					pc.setf("deviltower_zone", "map", 0)
					pc.setf("deviltower_zone", "time", 0)
					pc.setf("deviltower_zone", "channel", 0)
				end
			end
		end
	end
end
