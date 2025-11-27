quest pyuzuk begin
	state start begin
		function BuildMasterSkillList(job, group)
		local MASTER_SKILL_LEVEL = 20
		local PERFECT_MASTER_SKILL_LEVEL = 40

		local skill_list = special.active_skill_list[job+1][group]
		local ret_vnum_list = {}
		local ret_name_list = {}

		table.foreach(skill_list,
			function(i, skill_vnum)
				local skill_level = pc.get_skill_level(skill_vnum)

				if skill_level >= MASTER_SKILL_LEVEL and skill_level < PERFECT_MASTER_SKILL_LEVEL then

					table.insert(ret_vnum_list, skill_vnum)
					local name=locale.GM_SKILL_NAME_DICT[skill_vnum]
					if name == nil then name=skill_vnum end
					table.insert(ret_name_list, name)
				end
			end)
        return {ret_vnum_list, ret_name_list}
        end
		
		when 40002.use begin
		say_title("Perfect Master beceri eðitimi")
			if  pc.skillgroup == 0 then
			say("Bu yüzüðü kullanabilmen için öncelikle")
			say("beceri öðretmenlerinden eðitimini alman")
			say("gerekir.")
			return
			end
		local result = pyuzuk.BuildMasterSkillList(pc.get_job(), pc.get_skill_group()) 
		local vnum_list = result[1] 
		local name_list = result[2] 
			if table.getn(vnum_list) == 0 then 
			say("Becerilerininin hiç biri master seviyesinde deðil.") 
			return 
			end 
		say("Perfect master seviyesine ulaþtýrmak istediðin") 
		say ("beceriyi seç.")  
		local menu_list = {} 
		table.foreach(name_list, function(i, name) table.insert(menu_list, name) end) 
		table.insert(menu_list, "Kapat") 
		local s = select_table(menu_list) 
			if table.getn(menu_list) == s then 
			return 
			end 
		local skill_name = name_list[s] 
		local skill_vnum = vnum_list[s]
		say_title("Perfect Master beceri eðitimi")
		say(skill_name.." becerin perfect master olacak ve bu")
		say("yüzük yok olacak. Onaylýyor musun?")
			if select("Evet","Hayýr") == 2 then
			return
			end
			if pc.count_item("40002") == 0 then
			return
			end
		pc.remove_item("40002",1)
		pc.set_skill_level(skill_vnum,40)
		end
	end
end