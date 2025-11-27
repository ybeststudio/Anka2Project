CONFIRM_NO = 0
CONFIRM_YES = 1
CONFIRM_OK = 1
CONFIRM_TIMEOUT = 2

EVENT_TYPE_EXPERIENCE = 1
EVENT_TYPE_ITEM_DROP = 2
EVENT_TYPE_BOSS = 3
EVENT_TYPE_METIN = 4
EVENT_TYPE_MINING = 5
EVENT_TYPE_GOLD_FROG = 6
EVENT_TYPE_MOONLIGHT = 7
EVENT_TYPE_HEXEGONAL_CHEST = 8
EVENT_TYPE_FISHING = 9
EVENT_TYPE_HIDE_AND_SEEK = 10
EVENT_TYPE_OX = 11
EVENT_TYPE_TANAKA = 12
EVENT_TYPE_YANG = 13
EVENT_TYPE_OKEY_CARDS = 14

MALE = 0
FEMALE = 1

setstate = q.setstate
newstate = q.setstate

q.set_clock = function(name, value) q.set_clock_name(name) q.set_clock_value(value) end
q.set_counter = function(name, value) q.set_counter_name(name) q.set_counter_value(value) end
c_item_name = function(vnum) return ("[ITEM value;"..vnum.."]") end
c_mob_name = function(vnum) return ("[MOB value;"..vnum.."]") end

function color256(r, g, b) return "[COLOR r;"..(r/255.0).."|g;"..(g/255.0).."|b;"..(b/255.0).."]" end
function color(r,g,b) return "[COLOR r;"..r.."|g;"..g.."|b;"..b.."]" end
function delay(v) return "[DELAY value;"..v.."]" end
function setcolor(r,g,b) raw_script(color(r,g,b)) end
function setdelay(v) raw_script(delay(v)) end
function resetcolor(r,g,b) raw_script("[/COLOR]") end
function resetdelay(v) raw_script("[/DELAY]") end

function trim(s) return (string.gsub(s, "^%s*(.-)%s*$", "%1")) end

function addmapsignal(x,y) raw_script("[ADDMAPSIGNAL x;"..x.."|y;"..y.."]") end
function clearmapsignal() raw_script("[CLEARMAPSIGNAL]") end
function setbgimage(src) raw_script("[BGIMAGE src;") raw_script(src) raw_script("]") end
function addimage(x,y,src) raw_script("[IMAGE x;"..x.."|y;"..y) raw_script("|src;") raw_script(src) raw_script("]") end

function notice_multiline(str, func)
	local p = 0
	local i = 0
	while true do
		i = string.find(str, "%[ENTER%]", i+1)
		if i == nil then
			if string.len(str) > p then
				func(string.sub(str, p, string.len(str)))
			end
			break
		end
		func( string.sub( str, p, i-1 ) )
		p = i + 7
	end
end

function makequestbutton(name)
	raw_script("[QUESTBUTTON idx;")
	raw_script(""..q.getcurrentquestindex()) 
	raw_script("|name;")
	raw_script(name) raw_script("]")
end

function make_quest_button_ex(name, icon_type, icon_name)
	test_chat(icon_type)
	test_chat(icon_name)
	raw_script("[QUESTBUTTON idx;")
	raw_script(""..q.getcurrentquestindex()) 
	raw_script("|name;")
	raw_script(name)
	raw_script("|icon_type;")
	raw_script(icon_type)
	raw_script("|icon_name;")
	raw_script(icon_name)
	raw_script("]")
end

function make_quest_button(name) makequestbutton(name) end

function send_letter_ex(name, icon_type, icon_name) make_quest_button_ex(name, icon_type, icon_name) set_skin(NOWINDOW) q.set_title(name) q.start() end

function send_letter(name) makequestbutton(name) set_skin(NOWINDOW) q.set_title(name) q.start() end
function clear_letter() q.done() end
function say_title(name) say(color256(255, 230, 186)..name..color256(196, 196, 196)) end
function say_reward(name) say(color256(255, 200, 200)..name..color256(196, 196, 196)) end
function say_pc_name() say(pc.get_name()..":") end
function say_size(width, height) say("[WINDOW_SIZE width;"..width.."|height;"..height.."]") end

function setmapcenterposition(x,y)
	raw_script("[SETCMAPPOS x;")
	raw_script(x.."|y;")
	raw_script(y.."]")
end

function say_item(name, vnum, desc)
	say("[INSERT_IMAGE image_type;item|idx;"..vnum.."|title;"..name.."|desc;"..desc.."|index;".. 0 .."|total;".. 1 .."]")
end

function say_show_item(vnum)
	say("[INSERT_IMAGE image_type;item|idx;"..vnum.."|index;".. 0 .."|total;".. 1 .."]")
end

function say_item_vnum(vnum)
	say_item(item_name(vnum), vnum, "")
end

function say_item_vnum_inline(vnum,index,total)
	if index >= total then
		return
	end
	if total > 3 then
		return
	end
	raw_script("[INSERT_IMAGE image_type;item|idx;"..vnum.."|title;"..item_name(vnum).."|desc;".."".."|index;"..index.."|total;"..total.."]")
end

function say_new(msg)
	local max_length = 69
	local msg_length = string.len(msg)

	if msg_length > max_length then
		local i = max_length
		local last_word = nil

		while last_word == nil and i > 0 do
			local char = string.sub(msg, i, i)
			if char == " " or char == "." or char == "," then
				last_word = i
			end
			i = i - 1
		end

		if last_word then
			say(string.sub(msg, 1, last_word))
			say_new(string.sub(msg, last_word + 1))
		else
			say(string.sub(msg, 1, max_length))
			say_new(string.sub(msg, max_length + 1))
		end
	else
		say(msg)
	end
end

function say_reward_new(msg)
	local max_length = 69
	local msg_length = string.len(msg)

	if msg_length > max_length then
		local i = max_length
		local last_word = nil

		while last_word == nil and i > 0 do
			local char = string.sub(msg, i, i)
			if char == " " or char == "." or char == "," then
				last_word = i
			end
			i = i - 1
		end

		if last_word then
			say_reward(color256(255, 200, 200) .. string.sub(msg, 1, last_word) .. color256(196, 196, 196))
			say_reward_new(string.sub(msg, last_word + 1))
		else
			say_reward(color256(255, 200, 200) .. string.sub(msg, 1, max_length) .. color256(196, 196, 196))
			say_reward_new(string.sub(msg, max_length + 1))
		end
	else
		say_reward(color256(255, 200, 200) .. msg .. color256(196, 196, 196))
	end
end

function pc_is_novice()
	if pc.get_skill_group()==0 then
		return true
	else
		return false
	end
end

function pc_get_exp_bonus(exp, text)
	say_reward(text)
	pc.give_exp2(exp)
	set_quest_state("levelup", "run")
end

function pc_get_village_map_index(index)
	return village_map[pc.get_empire()][index]
end

function pc_has_even_id()
	return math.mod(pc.get_player_id(), 2) == 0
end

function pc_get_account_id()
	return math.mod(pc.get_account_id(), 2) !=0
end

village_map = {
	{1, 3},
	{21, 23},
	{41, 43},
}

function npc_is_same_empire()
	if pc.get_empire()==npc.empire then
		return true
	else
		return false
	end
end

function npc_get_skill_teacher_race(pc_empire, pc_job, sub_job)
	if 1 == sub_job then
		if 0 == pc_job then
			return WARRIOR1_NPC_LIST[pc_empire]
		elseif 1 == pc_job then
			return ASSASSIN1_NPC_LIST[pc_empire]
		elseif 2 == pc_job then
			return SURA1_NPC_LIST[pc_empire]
		elseif 3 == pc_job then
			return SHAMAN1_NPC_LIST[pc_empire]
		end	
	elseif 2 == sub_job then
		if 0 == pc_job then
			return WARRIOR2_NPC_LIST[pc_empire]
		elseif 1 == pc_job then
			return ASSASSIN2_NPC_LIST[pc_empire]
		elseif 2 == pc_job then
			return SURA2_NPC_LIST[pc_empire]
		elseif 3 == pc_job then
			return SHAMAN2_NPC_LIST[pc_empire]
		end
	end
	return 0
end

function pc_find_square_guard_vid()
	if pc.get_empire() == 1 then
		return find_npc_by_vnum(11000)
	elseif pc.get_empire() == 2 then
		return find_npc_by_vnum(11002)
	elseif pc.get_empire() == 3 then
		return find_npc_by_vnum(11004)
	end
	return 0
end

function pc_find_skill_teacher_vid(sub_job)
	local vnum = npc_get_skill_teacher_race(pc.get_empire(), pc.get_job(), sub_job)
	return find_npc_by_vnum(vnum)
end

function pc_find_square_guard_vid()
	local pc_empire = pc.get_empire()
	if pc_empire == 1 then
		return find_npc_by_vnum(11000)
	elseif pc_empire == 2 then
		return find_npc_by_vnum(11002)
	elseif pc_empire == 3 then
		return find_npc_by_vnum(11004)
	end
end

function npc_is_same_job()
	local pc_job=pc.get_job()
	local npc_vnum=npc.get_race()
	if pc_job == 0 then
		if table_is_in(WARRIOR1_NPC_LIST, npc_vnum) then return true end
		if table_is_in(WARRIOR2_NPC_LIST, npc_vnum) then return true end
	elseif pc_job == 1 then
		if table_is_in(ASSASSIN1_NPC_LIST, npc_vnum) then return true end
		if table_is_in(ASSASSIN2_NPC_LIST, npc_vnum) then return true end
	elseif pc_job == 2 then
		if table_is_in(SURA1_NPC_LIST, npc_vnum) then return true end
		if table_is_in(SURA2_NPC_LIST, npc_vnum) then return true end
	elseif pc_job == 3 then
		if table_is_in(SHAMAN1_NPC_LIST, npc_vnum) then return true end
		if table_is_in(SHAMAN2_NPC_LIST, npc_vnum) then return true end
	end
	return false
end

function npc_get_job()
	local npc_vnum = npc.get_race()
	if table_is_in(WARRIOR1_NPC_LIST, npc_vnum) then return COND_WARRIOR_1 end
	if table_is_in(WARRIOR2_NPC_LIST, npc_vnum) then return COND_WARRIOR_2 end
	if table_is_in(ASSASSIN1_NPC_LIST, npc_vnum) then return COND_ASSASSIN_1 end
	if table_is_in(ASSASSIN2_NPC_LIST, npc_vnum) then return COND_ASSASSIN_2 end
	if table_is_in(SURA1_NPC_LIST, npc_vnum) then return COND_SURA_1 end
	if table_is_in(SURA2_NPC_LIST, npc_vnum) then return COND_SURA_2 end
	if table_is_in(SHAMAN1_NPC_LIST, npc_vnum) then return COND_SHAMAN_1 end
	if table_is_in(SHAMAN2_NPC_LIST, npc_vnum) then return COND_SHAMAN_2 end
	return 0
end

function time_min_to_sec(value)
	return 60*value
end

function time_hour_to_sec(value)
	return 3600*value
end

function next_time_set(value, test_value)
	local nextTime = get_time()+value
	if is_test_server() then
		nextTime=get_time()+test_value
	end

	pc.setqf("__NEXT_TIME__", nextTime)
end

function next_time_is_now(value)
	if get_time() >= pc.getqf("__NEXT_TIME__") then
		return true
	else
		return false
	end
end

function table_get_random_item(self)
	return self[number(1, table.getn(self))]
end

function table_is_in(self, test)
	for i = 1, table.getn(self) do
		if self[i]==test then
			return true
		end
	end
	return false
end

function restart_quest()
	set_state("start")
	q.done()
end

function complete_quest()
	set_state("__COMPLETE__")
	q.done()
end

function complete_quest_state(state_name)
	set_state(state_name)
	q.done()
end

function test_chat(log)
	if is_test_server() then
		chat(log)
	end
end

function bool_to_str(is)
	if is then
		return "true"
	else
		return "false"
	end
end

WARRIOR1_NPC_LIST = {20300, 20320, 20340, }
WARRIOR2_NPC_LIST = {20301, 20321, 20341, }
ASSASSIN1_NPC_LIST = {20302, 20322, 20342, }
ASSASSIN2_NPC_LIST = {20303, 20323, 20343, }
SURA1_NPC_LIST = {20304, 20324, 20344, }
SURA2_NPC_LIST = {20305, 20325, 20345, }
SHAMAN1_NPC_LIST = {20306, 20326, 20346, }
SHAMAN2_NPC_LIST = {20307, 20327, 20347, }

npc_index_table = {
	['race'] = npc.getrace,
	['empire'] = npc.get_empire,
}

pc_index_table = {
	['weapon'] = pc.getweapon,
	['level'] = pc.get_level,
	['hp'] = pc.gethp,
	['maxhp'] = pc.getmaxhp,
	['sp'] = pc.getsp,
	['maxsp'] = pc.getmaxsp,
	['exp'] = pc.get_exp,
	['nextexp'] = pc.get_next_exp,
	['job'] = pc.get_job,
	['money'] = pc.getmoney,
	['gold'] = pc.getmoney,
	['name'] = pc.getname,
	['playtime'] = pc.getplaytime,
	['leadership'] = pc.getleadership,
	['empire'] = pc.getempire,
	['skillgroup'] = pc.get_skill_group,
	['x'] = pc.getx,
	['y'] = pc.gety,
	['local_x'] = pc.get_local_x,
	['local_y'] = pc.get_local_y,
}

item_index_table = {
	['vnum'] = item.get_vnum,
	['name'] = item.get_name,
	['size'] = item.get_size,
	['count'] = item.get_count,
	['type'] = item.get_type,
	['sub_type'] = item.get_sub_type,
	['refine_vnum'] = item.get_refine_vnum,
	['level'] = item.get_level,
}

function npc_index(t, i)
	local npit = npc_index_table
	if npit[i] then
		return npit[i]()
	else
		return rawget(t,i)
	end
end

function pc_index(t, i)
	local pit = pc_index_table
	if pit[i] then
		return pit[i]()
	else
		return rawget(t,i)
	end
end

function item_index(t, i)
	local iit = item_index_table
	if iit[i] then
		return iit[i]()
	else
		return rawget(t, i)
	end
end

setmetatable(pc,{__index = pc_index})
setmetatable(npc,{__index = npc_index})
setmetatable(item,{__index = item_index})

function select(...)
	return q.yield('select', arg)
end

function select_table(table)
	return q.yield('select', table)
end

function wait()
	q.yield('wait')
end

function input()
	return q.yield('input')
end

function confirm(vid, msg, timeout)
	return q.yield('confirm', vid, msg, timeout)
end

function select_item()
	setskin(NOWINDOW)
	return q.yield('select_item')
end

NOWINDOW = 0
NORMAL = 1
CINEMATIC = 2
SCROLL = 3

WARRIOR = 0
ASSASSIN = 1
SURA = 2
SHAMAN = 3

COND_WARRIOR_1 = 16
COND_WARRIOR_2 = 32

COND_ASSASSIN_1 = 128
COND_ASSASSIN_2 = 256

COND_SURA_1 = 1024
COND_SURA_2 = 2048

COND_SHAMAN_1 = 8192
COND_SHAMAN_2 = 16384

special = {}

special.active_skill_list = {
	{
		{ 1, 2, 3, 4, 5},
		{ 16, 17, 18, 19, 20},
	},
	{
		{31, 32, 33, 34, 35},
		{46, 47, 48, 49, 50},
	},
	{
		{61, 62, 63, 64, 65, 66},
		{76, 77, 78, 79, 80, 81},
	},
	{
		{91, 92, 93, 94, 95, 96},
		{106, 107, 108, 109, 110, 111},
	},
}

special.devil_tower = {
	{ 2048+126, 6656+384 },
	{ 2048+134, 6656+147 },
	{ 2048+369, 6656+629 },
	{ 2048+369, 6656+401 },
	{ 2048+374, 6656+167 },
	{ 2048+579, 6656+616 },
	{ 2048+578, 6656+392 },
	{ 2048+575, 6656+148 },
}

function get_skill_name_by_vnum(vnum)
	return skill_name(vnum)
end

function BuildSkillList(job, group)
	local skill_vnum_list = {}
	local skill_name_list = {}

	if pc.get_skill_group() != 0 then
		local skill_list = special.active_skill_list[job + 1][group]

		table.foreachi(skill_list,
			function(i, t)
				local lev = pc.get_skill_level(t)

				if lev > 0 then
					local name = skill_name(t)

					if name != nil then
						table.insert(skill_vnum_list, t)
						table.insert(skill_name_list, name)
					end
				end
			end
		)
	end

	table.insert(skill_vnum_list, 0)
	table.insert(skill_name_list, gameforge[LC()][99])

	return {skill_vnum_list, skill_name_list}
end

char_name_list = {}
char_name_list[1] = {}
char_name_list[2] = {}
char_name_list[3] = {}
char_name_list[4] = {}
char_name_list[5] = {}
char_name_list[6] = {}
char_name_list[7] = {}
char_name_list[8] = {}
char_name_list[9] = {}
char_name_list[10] = {}

function store_charname_by_id(id, charname, charid)
	char_name_list[id]["name"] = charname
	char_name_list[id]["eid"] = charid
	return nil
end

function return_charname_by_id(charid)
	local counter = 11
	repeat
		counter = counter -1
	until char_name_list[counter]["eid"] == charid
	return char_name_list[counter]["name"]
end

function input_number(sentence)
	say (sentence)
	local n = nil
	while n == nil do
		n = tonumber (input())
		if n != nil then
			break
		end
		say ("input number")
	end
	return n
end

function get_today_count(questname, flag_name)
	local today = math.floor(get_global_time() / 86400)
	local today_flag = flag_name.."_today"
	local today_count_flag = flag_name.."_today_count"
	local last_day = pc.getf(questname, today_flag)
	if last_day == today then
		return pc.getf(questname, today_count_flag)
	else
		return 0
	end
end

function inc_today_count(questname, flag_name, count)
	local today = math.floor(get_global_time() / 86400)
	local today_flag = flag_name.."_today"
	local today_count_flag = flag_name.."_today_count"
	local last_day = pc.getqf(questname, today_flag)
	if last_day == today then
		pc.setf(questname, today_count_flag, pc.getf(questname, today_count_flag) + 1)
	else
		pc.setf(questname, today_flag, today)
		pc.setf(questname, today_count_flag, 1)
	end
end

function drop_gamble_with_flag(drop_flag)
	local drop_chance = game.get_event_flag(drop_flag)
	if drop_chance < 1 or drop_chance > 100 then
		drop_chance = 10
	end
	return drop_chance >= number(1, 100)
end

function say_npc()
	say_title(""..mob_name(npc.get_race())..":")
end

function get_random_vnum_from_table(items)
	local temp_table = {}
	local playerLevel = pc.get_level()
	table.foreachi(items, 
		function(index, item)
			local itemProbability = item[2]
			local itemVnum = item[1]
			local meetsLevelLimit = true
			if table.getn(item) > 2 then
				if playerLevel < item[3] then
					meetsLevelLimit = false
				end
				if table.getn(item) > 3 then
					if playerLevel > item[4] then
						meetsLevelLimit = false
					end
				end
			end
			if meetsLevelLimit then
				for amount = 1, itemProbability do
					table.insert(temp_table, itemVnum)
				end
			end
		end
	)
	return temp_table[math.random(table.getn(temp_table))]
end

function count_item_range(firstVnum, lastVnum)
	local amount = 0
	for item = firstVnum, lastVnum, 1 do
		 local i = pc.count_item(item)
		 amount = amount + i
	end
	return amount
end

function remove_item_range(amountLeft, firstVnum, lastVnum)
	if count_item_range(firstVnum, lastVnum) < amountLeft then
		return false
	end
	for currentVnum = firstVnum, lastVnum, 1 do
		local currentAmount = pc.count_item(currentVnum)
		if currentAmount > 0 then
			if currentAmount < amountLeft then
				pc.remove_item(currentVnum, currentAmount)
				amountLeft = amountLeft - currentAmount
			else
				pc.remove_item(currentVnum, amountLeft)
				return true
			end
		end
	end
end

function is_destination_village(maps)
	local map_lookup = {}
	if maps == 1 then
		map_lookup = {[1]='', [21]='', [41]='' }
	elseif maps==2 then
		map_lookup = {[3]='', [23]='', [43]='' }
	elseif maps==3 then
		map_lookup = {[1]='', [21]='', [41]='',[3]='', [23]='', [43]='' }
	elseif maps==65 then
		map_lookup = {[65]='' }
	end
	return map_lookup[pc.get_map_index()]
end

function say_important_title(name) say(color256(255, 230, 186)..name..color256(196, 196, 196)) end
function say_important(name) say(color256(255, 230, 186)..name..color256(196, 196, 196)) end

function split(str, delim, maxNb)
	if str == nil then return str end
	if string.find(str, delim) == nil then return { str } end
	if maxNb == nil or maxNb < 1 then maxNb = 0 end

	local result = {}
	local pat = "(.-)" .. delim .. "()"
	local nb = 0
	local lastPos

	for part, pos in string.gfind(str, pat) do
		nb = nb + 1
		result[nb] = part
		lastPos = pos
		if nb == maxNb then break end
	end

	if nb ~= maxNb then
		result[nb + 1] = string.sub(str, lastPos)
	end

	return result
end

function GetMissionInfo(Floor, Time)
	if Time == 99 then
		d.command(string.format("RefreshDungeonFloor %d", Floor))
	else
		d.command(string.format("RefreshDungeonTimer %d %d", Floor, Time))
	end
end

function getinput(par)
	cmdchat("getinputbegin")
	local ret = input(cmdchat(par))
	cmdchat("getinputend")
	return ret
end
