------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest devilcatacomb_zone begin
	state start begin
		when 20367.chat."Seytanin Yeralti Mezari" begin
			if pc.get_level() < 75 then
				say_new("[ENTER]Girmek için gerekli seviyeye sahip degilsiniz! [ENTER]")
				return
			else
				local cooldown = pc.getf("devilcatacomb_zone", "cooldown")
				local real_time = get_global_time()
				local r_cooldown = pc.getf("devilcatacomb_zone", "cooldown") - real_time

				if not pc.is_gm() and cooldown > real_time then
					say_title(mob_name(20367))
					say_new("")
					say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
					return
				end

				pc.remove_item(30311, pc.count_item(30311))
				say_title(mob_name(30101))
				say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

				local x = select("Yalniz", "Grupla", "Iptal")
				if x == 1 then
					if pc.count_item(30319) <= 0 then
						say_title(mob_name(30101))
						say_new("[ENTER]Girmek için anahtariniz yok! [ENTER]")
						say_item_vnum(30319, 1)
						return
					end

					pc.remove_item(30319, 1)
					d.new_jump(216, (3072+550)*100, (12032 + 45)*100)

					if d.select(d.get_map_index()) then
						d.setf("first_regen", 1)
						d.setf("DungeonBlock", 1)
					end
				elseif x == 2 then
					if party.is_party() then
						local user_fail_cooldown = {}
						local user_fail_ticket = {}
						local CooldownCheck = true
						local ticketCheck = true

						local pids = {party.get_member_pids()}

						for i, pid in next, pids, nil do
							q.begin_other_pc_block(pid)
							if not pc.is_gm() and pc.getf("devilcatacomb_zone", "cooldown") > get_global_time() then
								table.insert(user_fail_cooldown, pc.get_name())
								CooldownCheck = false
							end

							if pc.count_item(30319) <= 0 then
								table.insert(user_fail_ticket, pc.get_name())
								ticketCheck = false
							end

							q.end_other_pc_block()
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

						if ticketCheck == false then
							say_new("Oyuncunun anahtari yok: [ENTER]")
							local a = 1
							for i, name in next, user_fail_ticket, nil do
								say_new(string.format("%d. %s", a, name))
								a = a+1
							end
							return
						end

						for i, pid in next, pids, nil do
							q.begin_other_pc_block(pid)
							pc.remove_item(30319, 1)
							q.end_other_pc_block()
						end

						d.new_jump_party(216, (3072+550), (12032+45))
						if d.select(d.get_map_index()) then
							d.setf("DungeonBlock", 1)
							d.setf("first_regen", 1)
						end
					else
						say_title(mob_name(30101))
						say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
						return
					end
				elseif x == 3 then
					return
				end
			end
		end

		when kill with pc.get_map_index() == 216 begin
			local j = number(1, 80)
			if j == 1 then
				game.drop_item(30311, 1)
			end
		end

		when 30101.take with pc.get_map_index() == 216 and item.get_vnum() == 30311 begin
			local cooldown = pc.getf("devilcatacomb_zone", "cooldown")
			local real_time = get_global_time()
			local r_cooldown = pc.getf("devilcatacomb_zone", "cooldown") - real_time

			if not pc.is_gm() and cooldown > real_time then
				say_title(mob_name(20367))
				say_new("")
				say_new(r_cooldown.. " saniye kaldi tekrar girmek için! [ENTER]")
				return
			end

			pc.remove_item(30311, pc.count_item(30311))
			say_title(mob_name(30101))
			say_new("[ENTER]Zindana girmek istiyor musunuz? [ENTER]")

			local x = select("Yalniz", "Grupla", "Iptal")
			if x == 1 then
				d.new_jump(216, (3072+550)*100, (12032 + 45)*100)
				if d.select(d.get_map_index()) then
					d.setf("first_regen", 1)
					d.setf("DungeonBlock", 1)
				end
			elseif x == 2 then
				if party.is_party() then
					local user_fail_cooldown = {}
					local CooldownCheck = true

					local pids = {party.get_member_pids()}

					for i, pid in next, pids, nil do
						q.begin_other_pc_block(pid)
						if not pc.is_gm() and pc.getf("devilcatacomb_zone", "cooldown") > get_global_time() then
							table.insert(user_fail_cooldown, pc.get_name())
							CooldownCheck = false
						end
						q.end_other_pc_block()
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

					d.new_jump_party(216, (3072+550), (12032+45))
					if d.select(d.get_map_index()) then
						d.setf("DungeonBlock", 1)
						d.setf("first_regen", 1)
					end
				else
					say_title(mob_name(30101))
					say_new("[ENTER]Bir grupta degilsiniz! [ENTER]")
					return
				end
			elseif x == 3 then
				return
			end
		end

		when login with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			pc.set_warp_location_local(65, 5918, 993)
			local real_time = get_global_time()
			local index = pc.get_map_index()

			if index >= 2160000 and index <= 2169990 then
				if d.select(index) then
					if d.getf("DungeonBlock") == 1 then
						if d.getf("first_regen") == 1 then
							if d.getf("ticket") == 1 then
								server_timer("devilcatacomb_2", 1, index)
							else
								server_timer("devilcatacomb_1", 1, index)
							end
							cmdchat(string.format("RefreshDungeonTimer %d %d", 1, d.getf("real_time") - real_time))
						else
							cmdchat(string.format("RefreshDungeonTimer %d %d", d.getf("level"), d.getf("real_time") - real_time))
						end
					else
						pc.warp(591400, 99200)
					end
				end
			end
		end

		when devilcatacomb_1.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("first_regen") == 1 then
					d.setqf2("devilcatacomb_zone", "catacombkey", 0)
					d.purge()
					d.clear_regen()

					d.setf("first_regen", 0)
					d.setf("level", 2)
					d.setf("tortoise_timer", 1)

					server_timer('devilcatacomb_full_timer', 60*60, d.get_map_index())
					d.setf("real_time", get_global_time()+60*60)

					d.regen_file ("data/dungeon/devilcatacomb/dc_2f_regen.txt")
					d.regen_file("data/dungeon/devilcatacomb/devilcatacomb_floor4_regen2.txt")

					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())

					d.notice(d.get_map_index(), 679, "", false)
				end
			end
		end

		when devilcatacomb_full_timer.server_timer begin
			if d.select(get_server_timer_arg()) then
				d.setf("DungeonBlock", 0)
				d.global_warp_all_to_base(5918*100, 993*100, get_server_timer_arg(), 60)
			end
		end

		when 30103.click with pc.get_map_index() >= 2160000 and pc.get_map_index() < 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 2 then
					if d.getf("tortoise_timer") == 1 then
						server_timer("devilcatacomb_2", 5, d.get_map_index())
					end
					npc.purge()
				end
			end
		end

		when devilcatacomb_2.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 2 and d.getf("tortoise_timer") == 1 or d.getf("ticket") == 1 and d.getf("level") == 1 then
					d.purge()
					d.clear_regen()

					d.setf("first_regen", 0)
					d.setf("tortoise_timer", 0)
					d.setf("level", 3)

					d.regen_file("data/dungeon/devilcatacomb/dc_3f_regen.txt")

					d.notice(d.get_map_index(), 677, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())

					local position = {{1231,357}, {1348,349}, {1349,150}, {1235,142}, {1140,245}, {1148,348}, {1150,154}}
					for i = 1, 7 do
						d.spawn_mob(8038, position[i][1], position[i][2])
					end

					d.setf("stones", 0)
					d.jump_all(1367+3072, 247+12032)
				end
			end
		end

		when devilcatacomb_3.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 3 then
					d.purge()
					d.clear_regen()

					d.setf ("level", 4)
					d.getf("stones", 0)

					d.jump_all(846+3072, 898+12032)
					d.notice(d.get_map_index(), 680, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
					d.regen_file("data/dungeon/devilcatacomb/dc_5f_regen.txt")

					local devil_king_pos = {{673,829}, {691,638}, {848,568}, {1026,642}, {1008,862}, {848, 742}}
					for i = 1, 6 do
						d.spawn_mob(2591, devil_king_pos[i][1], devil_king_pos[i][2])
					end
				end
			end
		end

		when 2591.kill with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 4 then
					local new = d.getf("stones") + 1
					d.setf("stones", new)
					if new == 5 then
						d.setf("stones", 0)
						server_timer("devilcatacomb_4", 5, d.get_map_index())
					end
				elseif d.getf("level") == 5 then
					local new = d.getf("stones") + 1
					d.setf("stones", new)
					if new == 5 then
						d.setf("stones", 0)
						game.drop_item(50084, 1)
					end
				end
			end
		end

		when 2597.kill with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 5 then
					game.drop_item(50084, 1)
					server_timer("devilcatacomb_5", 1, d.get_map_index())
				end
			end
		end

		when devilcatacomb_4.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 4 then
					d.clear_regen()
					d.purge()

					d.setf("level", 5)
					d.setf("q_level", 1)

					d.notice(d.get_map_index(), 681, "", false)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())

					d.jump_all (1362+3072, 666+12032)
					d.spawn_mob(20073, 1260, 674)
					d.spawn_mob(20073, 1302, 661)
					d.spawn_mob(20073, 1337, 691)
					d.spawn_mob(20073, 1312, 727)
					d.spawn_mob(20073, 1261, 706)
					d.spawn_mob(8038, 1302, 681)
					d.spawn_mob(8038, 1320, 699)
					d.spawn_mob(8038, 1308, 712)
					d.spawn_mob(8038, 1285, 713)
					d.spawn_mob(8038, 1275, 687)
				end
			end
		end

		when 8038.kill with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				local level = d.getf("level")
				if level == 3 then
					local new = d.getf("stones") + 1
					d.setf("stones", new)
					if new == 7 then
						d.setf("stones", 0)
						server_timer("devilcatacomb_3", 5, d.get_map_index())
					end
				elseif level == 5 then
					local q_level = d.getf("q_level")
					if q_level == 1 then
						local new = d.getf("stones") + 1
						d.setf("stones", new)
						if new == 5 then
							d.setf("stones", 0)
							game.drop_item(50084, 1)
						end
					elseif q_level == 4 then
						local new = d.getf("stones") + 1
						d.setf("stones", new)
						if new == 10 then
							d.setf("stones", 0)
							game.drop_item(50084, 1)
						end
					end
				end
			end
		end

		when 20073.take with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 and item.get_vnum() == 50084 begin
			if d.select(pc.get_map_index()) then
				local q_level = d.getf("q_level")
				if q_level == 1 then
					d.setf("stones", 15)
					d.setf("q_level", 2)

					d.spawn_group(2521, 1299, 709, 0, 1, 1)
					d.spawn_group(2521, 1305, 711, 0, 1, 1)
					d.spawn_group(2521, 1295, 720, 0, 1, 1)
				elseif q_level == 2 then
					d.setf("q_level", 3)

					d.spawn_mob(2591, 1302, 681)
					d.spawn_mob(2591, 1320, 699)
					d.spawn_mob(2591, 1308, 712)
					d.spawn_mob(2591, 1285, 713)
					d.spawn_mob(2591, 1275, 687)
				elseif q_level == 3 then
					d.setf("q_level", 4)

					d.spawn_mob(8038, 1302, 681)
					d.spawn_mob(8038, 1320, 699)
					d.spawn_mob(8038, 1308, 712)
					d.spawn_mob(8038, 1285, 713)
					d.spawn_mob(8038, 1275, 687)
					d.spawn_mob(8038, 1324, 660)
					d.spawn_mob(8038, 1262, 659)
					d.spawn_mob(8038, 1290, 733)
					d.spawn_mob(8038, 1350, 697)
					d.spawn_mob(8038, 1297, 700)
				elseif q_level == 4 then
					d.setf("q_level", 5)

					d.spawn_mob(2597, 1303, 704)
				elseif q_level == 5 then
					d.jump_all(3146, 13189)
					d.notice(d.get_map_index(), 682, "", false)
					d.setf("level", 6)
				end
				item.remove()
				npc.purge()
			end
		end

		when devilcatacomb_5.server_timer begin
			if d.select(get_server_timer_arg()) then
				if d.getf("level") == 5 then
					d.setf("level", 6)
					GetMissionInfo(d.getf("level"), d.getf("real_time") - get_global_time())
					d.regen_file ("data/dungeon/devilcatacomb/dc_7f_regen.txt")
					d.spawn_mob(2598, 74, 1103)
				end
			end
		end

		when kill with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 5 then
					if d.getf("q_level") == 2 then
						local new = d.getf("stones")-1
						d.setf("stones", new)
						if new == 0 then
							d.setf("stones", 0)
							game.drop_item(50084, 1)
							game.drop_item(50084, 1)
						end
					end
				end
			end
		end

		when 30103.chat."Odaya gir" with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 and d.getf("level") == 6 begin
			say_title(mob_name(30103))
			say_new("[ENTER]Gerekli her seye sahip misiniz? [ENTER]")
			say_item_vnum(30319, 1)
			say_new("")
			local s = select("Evet", "Iptal")
			if s == 2 then
				return
			else
				if pc.count_item(30319) < 1 then
					say_new("[ENTER]Gerekli her seye sahip degilsiniz! [ENTER]")
					say_item_vnum(30319, 1)
				else
					pc.remove_item(30319, 1)
					pc.jump(314600, 1318900)
					pc.setf("devilcatacomb_zone", "catacombkey", 1)
				end
			end
		end

		when 2598.kill with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("level") == 6 then
					server_timer("devil_king_dead_timer", 1, d.get_map_index())
					d.spawn_mob(30130, pc.get_local_x()-5, pc.get_local_y()+5)

					d.setf("dungeon_empire", pc.get_empire())
				end
			end
		end

		when devil_king_dead_timer.server_timer begin
			local arg = get_server_timer_arg()
			if d.select(arg) then
				if d.getf("DungeonBlock") == 1 then
					GetMissionInfo(6, 60)
					d.global_warp_all_to_base(5918*100, 993*100, arg, 60)
					d.notice(d.get_map_index(), 673, "", false)
					d.setqf2("devilcatacomb_zone", "catacombkey", 0)
					d.setqf2("devilcatacomb_zone", "cooldown", get_global_time()+60*60)

					--Track Window Dungeon Status
					d.track_update(2598)

					d.setf("DungeonBlock", 0)
					d.setf("IsOn", 0)
					d.setf("real_time", 0)
					d.setf("stones", 0)
					d.setf("ticket", 0)
					d.setf("q_level", 0)
					d.setf("tortoise_timer", 0)

					clear_server_timer("devilcatacomb_full_timer", arg)
					clear_server_timer("devilcatacomb_1", arg)
					clear_server_timer("devilcatacomb_2", arg)
					clear_server_timer("devilcatacomb_3", arg)
					clear_server_timer("devilcatacomb_4", arg)
					clear_server_timer("devilcatacomb_5", arg)
				end
			end
		end

		when logout with pc.get_map_index() >= 2160000 and pc.get_map_index() <= 2169990 begin
			if d.select(pc.get_map_index()) then
				if d.getf("DungeonBlock") == 1 then
					pc.setf("devilcatacomb_zone", "map", d.get_map_index())
					pc.setf("devilcatacomb_zone", "time", get_global_time()+15*60)
					pc.setf("devilcatacomb_zone", "channel", pc.get_channel_id())
				else
					pc.setf("devilcatacomb_zone", "map", 0)
					pc.setf("devilcatacomb_zone", "time", 0)
					pc.setf("devilcatacomb_zone", "channel", 0)
				end
			end
		end

		when 20367.chat."Geri dön..." with pc.getf("devilcatacomb_zone", "map") >= 1 begin
			local dungeonIndex = pc.getf("devilcatacomb_zone", "map")
			local dungeonChannel = pc.getf("devilcatacomb_zone", "channel")

			if dungeonChannel == pc.get_channel_id() then
				if d.select(dungeonIndex) then
					if get_global_time() > pc.getf("devilcatacomb_zone", "time") then
						say_new("[ENTER]Geri dönmek için 15 dakikaniz vardi! [ENTER]")
						pc.setf("devilcatacomb_zone", "map", 0)
						pc.setf("devilcatacomb_zone", "time", 0)
						pc.setf("devilcatacomb_zone", "channel", 0)
						return
					end

					local dwLevel = d.getf_from_map_index("level", dungeonIndex)
					local ticket = d.getf_from_map_index("ticket", dungeonIndex)
					if dwLevel == 2 then
						pc.warp((3072 + 550)*100, (12032 + 45)*100, dungeonIndex)
					elseif dwLevel == 3 then
						pc.warp((3072 + 1367)*100, (12032 + 247)*100, dungeonIndex)
					elseif dwLevel == 4 then
						pc.warp((3072 + 846)*100, (12032 + 898)*100, dungeonIndex)
					elseif dwLevel == 5 then
						pc.warp((3072 + 1362)*100, (12032 + 666)*100, dungeonIndex)
					elseif dwLevel == 6 then
						if ticket == 1 then
							pc.warp((3072 + 73)*100, (12032 + 1145)*100, dungeonIndex)
						else
							if pc.getf("devilcatacomb_zone", "catacombkey") == 1 then
								pc.warp(314600, 1318900, dungeonIndex)
							else
								pc.warp((3072 + 1362)*100, (12032 + 666)*100, dungeonIndex)
							end
						end
					end
				else
					say_new("Müsait degil, tekrar baslayabilirsiniz. [ENTER]")
					pc.setf("devilcatacomb_zone", "map", 0)
					pc.setf("devilcatacomb_zone", "time", 0)
					pc.setf("devilcatacomb_zone", "channel", 0)
				end
			else
				say_new("Zindan bu kanalda açilmadi! [ENTER]")
				say_new(string.format("Girmek için, %d kanalina gidin. [ENTER]", dungeonChannel))
			end
		end
	end
end
