------------------------------------------------
--        Author: Best Studio        		  --
--  Github: https://github.com/ybeststudio    --
------------------------------------------------
-- NOT: Bu dosya Unix (LF) line ending ve Windows-1254 (Türkçe ANSI) encoding ile kaydedilmelidir
-- Encoding: Windows-1254 (Türkçe ANSI) - Türkçe karakterleri destekler
-- Line Ending: LF (Unix formati)
------------------------------------------------

quest socket_clean_stone_plus begin
	state start begin
		function extract_metin_stone_from_pos(pos)
			item.select_cell(pos)

			local info_name = {}
			local info_pos = {}

			for i=0 , 2 do
				local val = item.get_socket(i)

				if val > 2 and val != 28960 then
					table.insert(info_name, item_name(val))
					table.insert(info_pos, i)
				end
			end

			table.insert(info_name, "Geri")

			local s = select_table(info_name)

			if s == table.getn(info_name) then
				return
			end

			if pc.get_empty_inventory_count() <= 0 then
				say_title("Düzeltme parsomeni")
				say_new("[ENTER]Envanterinizde yeterli alan yok. [ENTER]")
				return
			end

			pc.give_item2(item.get_socket(info_pos[s]))
			item.set_socket(info_pos[s], 1)
			pc.remove_item(71109)
		end

		when 71109.use begin
			say_title("Düzeltme parsomeni")
			say_new("[ENTER]Bu parsomen, bir nesneden seçtiginiz bir tasi çikarma seçenegi sunar. [ENTER]")
			say_reward_new("Istediginiz nesneyi seçin ve tasi seçin. [ENTER]")

			local data = pc.get_socket_items()

			local sel_item = {}

			if table.getn( data ) > 6 then
				local sel_pos = {}

				while table.getn(data) > 0 do
					for i = 1, math.min(6, table.getn(data)) do
						table.insert(sel_item, i, data[1][1])
						table.insert(sel_pos, i, data[1][2])
						table.remove(data,1)
					end

					if table.getn(sel_item) < 6 then
						table.insert(sel_item, "Geri")
					else
						table.insert(sel_item, "Ileri")
					end

					local s = select_table(sel_item)

					if s == table.getn(sel_item) then
						while table.getn(sel_item) > 0 do
							table.remove(sel_item)
						end

						while table.getn(sel_pos) > 0 do
							table.remove(sel_pos)
						end
					else
						socket_clean_stone_plus.extract_metin_stone_from_pos(sel_pos[s])
						break
					end
				end
			else
				for i = 1, table.getn(data) do
					table.insert(sel_item, data[i][1])
				end

				table.insert(sel_item, "Geri")
				local s = select_table(sel_item)

				if s != table.getn(sel_item) then
					socket_clean_stone_plus.extract_metin_stone_from_pos(data[s][2])
				end
			end
		end
	end
end
