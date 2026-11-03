if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import uiToolTip
import localeInfo
import time
import event
import ui
import constInfo
# import item
import renderTarget
import chat
import nonplayer


ITEM_LIST_BY_CAT = {
	0 : { 0 : 71036, 1: 72348 },
	1 : { 0 : 71037, 1: 72348 },
}

BLOCKED_CAT_LIST = [2, 3, 4]
RENDER_INDEX = 10
CAT_LIST = [
	"Biyolog",
	"Koleksiyoncu",
	"Canavar",
	"Patron",
	"Metin"
]
LOCATION_BY_MOBID = {
	0: "0",

	2302: localeInfo.COLLECT_LOCATION_RED_FOREST,
	1061: localeInfo.COLLECT_LOCATION_DEVIL_TOWER,

	1601: localeInfo.COLLECT_LOCATION_GIANT_LAND,
	1501: localeInfo.COLLECT_LOCATION_GIANT_LAND,

	2411: localeInfo.COLLECT_LOCATION_SPIDER_DUNGEON,
	2311: localeInfo.COLLECT_LOCATION_SPIDER_DUNGEON,
	2034: localeInfo.COLLECT_LOCATION_SPIDER_DUNGEON,
	2075: localeInfo.COLLECT_LOCATION_SPIDER_DUNGEON,

	3190: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,
	3102: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,
	3103: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,
	3104: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,

	8213: localeInfo.COLLECT_LOCATION_PHARAOH_DESERT,
	9673: localeInfo.COLLECT_LOCATION_PHARAOH_DESERT,
	9671: localeInfo.COLLECT_LOCATION_PHARAOH_DESERT,

	6775: localeInfo.COLLECT_LOCATION_SNOW_LAND,
	6781: localeInfo.COLLECT_LOCATION_SNOW_LAND,
	6779: localeInfo.COLLECT_LOCATION_SNOW_LAND,

	6006: localeInfo.COLLECT_LOCATION_FIRE_LAND,
	34600: localeInfo.COLLECT_LOCATION_FIRE_LAND,
	6002: localeInfo.COLLECT_LOCATION_FIRE_LAND,

	3302: localeInfo.COLLECT_LOCATION_ENCHANTED_FOREST,
	3304: localeInfo.COLLECT_LOCATION_ENCHANTED_FOREST,

	591: localeInfo.COLLECT_LOCATION_SECOND_VILLAGES,
	3191: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,
	8214: localeInfo.COLLECT_LOCATION_PHARAOH_DESERT,

	6789: localeInfo.COLLECT_LOCATION_SNOW_LAND,
	34601: localeInfo.COLLECT_LOCATION_FIRE_LAND,
	3391: localeInfo.COLLECT_LOCATION_ENCHANTED_FOREST,

	8006: localeInfo.COLLECT_LOCATION_SECOND_VILLAGES,
	8008: localeInfo.COLLECT_LOCATION_ORC_VALLEY,
	2095: localeInfo.COLLECT_LOCATION_SPIDER_DUNGEON,
	8052: localeInfo.COLLECT_LOCATION_CYCLOPS_VALLEY,

	8210: localeInfo.COLLECT_LOCATION_PHARAOH_DESERT,
	6798: localeInfo.COLLECT_LOCATION_SNOW_LAND,
	6801: localeInfo.COLLECT_LOCATION_FIRE_LAND,
	6803: localeInfo.COLLECT_LOCATION_ENCHANTED_FOREST,
}

BONUS_LIST = {
	0 : { 0 : {
			# biologist.lua - Category 0 (Biyolog) - Subcategory 0
			0: localeInfo.COLLECT_BONUS_MONSTER % "10",         # {63, 10}
			1: localeInfo.COLLECT_BONUS_ATTACK % "200",                   # {53, 200}
			2: localeInfo.COLLECT_BONUS_AVERAGE % "5",                     # {72, 5}
			3: localeInfo.COLLECT_BONUS_METIN % "5",             # {97, 5}
			4: localeInfo.COLLECT_BONUS_METIN % "10",            # {97, 10}
			5: localeInfo.COLLECT_BONUS_METIN % "15",            # {97, 15}
			6: "-",
			7: "-",
			8: "-",
		},
		1: {
			# biologist.lua - Category 0 (Biyolog) - Subcategory 1
			0: "-",
			1: "-",
			2: "-",
			3: localeInfo.COLLECT_BONUS_MONSTER % "5",           # {63, 5}
			4: localeInfo.COLLECT_BONUS_BOSS % "10",           # {98, 10}
			5: localeInfo.COLLECT_BONUS_BOSS % "15",           # {98, 15}
			6: "-",
			7: "-",
			8: "-",
		},
		2: {
			# biologist.lua - Category 0 (Biyolog) - Subcategory 2
			0: "-",
			1: "-",
			2: "-",
			3: "-",
			4: localeInfo.COLLECT_BONUS_AVERAGE % "5",                     # {72, 5}
			5: localeInfo.COLLECT_BONUS_MONSTER % "30",          # {63, 30}
			6: "-",
			7: "-",
			8: "-",
		},
	},
		1 : { 0 : {
			# collector.lua - Category 1 (Koleksiyoncu) - Subcategory 0
			0: localeInfo.COLLECT_BONUS_BOSS % "10",          # {98, 10}
			1: localeInfo.COLLECT_BONUS_BOSS % "15",          # {98, 15}
			2: localeInfo.COLLECT_BONUS_BOSS % "10",          # {98, 10}
			3: localeInfo.COLLECT_BONUS_AVERAGE % "15",                   # {72, 15}
			4: localeInfo.COLLECT_BONUS_METIN % "20",           # {97, 20}
			5: localeInfo.COLLECT_BONUS_METIN % "10",           # {97, 10}
			6: localeInfo.COLLECT_BONUS_METIN % "10",           # {97, 10}
			7: localeInfo.COLLECT_BONUS_METIN % "20",           # {97, 20} - Extra bonus (index 7, 1. bonus)
			8: "-",
		},
		1: {
			# collector.lua - Category 1 (Koleksiyoncu) - Subcategory 1
			0: localeInfo.COLLECT_BONUS_MONSTER % "10",         # {63, 10}
			1: localeInfo.COLLECT_BONUS_MONSTER % "15",         # {63, 15}
			2: "-",
			3: "-",
			4: localeInfo.COLLECT_BONUS_MONSTER % "20",         # {63, 20}
			5: "-",
			6: localeInfo.COLLECT_BONUS_BOSS % "5",           # {98, 5}
			7: localeInfo.COLLECT_BONUS_BOSS % "10",          # {98, 10} - Extra bonus (index 7, 2. bonus)
			8: "-",
		},
		2: {
			# collector.lua - Category 1 (Koleksiyoncu) - Subcategory 2
			0: "-",
			1: "-",
			2: "-",
			3: "-",
			4: "-",
			5: "-",
			6: localeInfo.COLLECT_BONUS_AVERAGE % "10",                   # {72, 10}
			7: localeInfo.COLLECT_BONUS_MONSTER % "10",         # {63, 10} - Extra bonus (index 7, 3. bonus)
			8: "-",
		},
	},
	2 : { 0 : {
			0: localeInfo.COLLECT_BONUS_MONSTER % "1%",
			1: localeInfo.COLLECT_BONUS_MONSTER % "2%",
			2: localeInfo.COLLECT_BONUS_MONSTER % "3%",
			3: localeInfo.COLLECT_BONUS_MONSTER % "4%",
			4: localeInfo.COLLECT_BONUS_MONSTER % "5%",
			5: localeInfo.COLLECT_BONUS_MONSTER % "6%",
			6: localeInfo.COLLECT_BONUS_MONSTER % "8%",
			7: localeInfo.COLLECT_BONUS_MONSTER % "10%",
			8: "-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	},
	3 : { 0 : {
			# bossologia.lua - Category 3 (Patron)
			0: localeInfo.COLLECT_BONUS_BOSS % "1",    # {98, 1}
			1: localeInfo.COLLECT_BONUS_BOSS % "2",    # {98, 2}
			2: localeInfo.COLLECT_BONUS_BOSS % "3",    # {98, 3}
			3: localeInfo.COLLECT_BONUS_BOSS % "4",    # {98, 4}
			4: localeInfo.COLLECT_BONUS_BOSS % "5",    # {98, 5}
			5: localeInfo.COLLECT_BONUS_BOSS % "7",    # {98, 7}
			6: localeInfo.COLLECT_BONUS_BOSS % "10",   # {98, 10}
			7: localeInfo.COLLECT_BONUS_BOSS % "14",   # {98, 14}
			8: "-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	},
	4 : { 0 : {
			# metinologia.lua - Category 4 (Metin)
			0: localeInfo.COLLECT_BONUS_METIN % "1",     # {97, 1}
			1: localeInfo.COLLECT_BONUS_METIN % "2",     # {97, 2}
			2: localeInfo.COLLECT_BONUS_METIN % "3",     # {97, 3}
			3: localeInfo.COLLECT_BONUS_METIN % "4",     # {97, 4}
			4: localeInfo.COLLECT_BONUS_METIN % "5",     # {97, 5}
			5: localeInfo.COLLECT_BONUS_METIN % "7",     # {97, 7}
			6: localeInfo.COLLECT_BONUS_METIN % "10",    # {97, 10}
			7: localeInfo.COLLECT_BONUS_METIN % "15",    # {97, 15}
			8:"-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	}
}

CATEGORY_BUTTON_NAME = [
	"Biyolog",
	"Koleksiyoncu",
	"Canavar",
	"Patron",
	"Metin"
]
QUEST_COUNTER = {
	0 : 6,
	1 : 7,
	2 : 8,
	3 : 8,
	4 : 8,
}

class CollectWindow(ui.ScriptWindow):		
	def __init__(self):				
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.selectedWindow = 0
		self.realwindow = 0
		self.data = {i : {"ITEM_VNUM" : 0, "TIME" : -1, "COUNT" : 0, "COUNT_TOTAL" : 0, "TAKE_CHANCE" : 0, "RENDERTARGET_VNUM" : 0, "QUEST_INDEX" : 0, "REQUIRED_LEVEL" : 0, } for i in xrange(len(constInfo.CollectWindowQID))}
		self.progress = {i : {"COMPLETED" : 0} for i in xrange(len(constInfo.CollectWindowQID))}
		self.CategoryButtonList = []
		self.bonus_text = {}
		# self.categorytext = {}

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/collectwindow.py")
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			for i in xrange(3):
				self.bonus_text[i] = self.GetChild("bonustext_{}".format(i))

			# for i in xrange(5):
			# 	self.categorytext[i] = self.GetChild("categorytext{}".format(i))

			for i in xrange(len(constInfo.CollectWindowQID)):
				self.CategoryButtonList.append(self.GetChild("CategoryButton_%d" % i))
				self.CategoryButtonList[i].SetEvent(ui.__mem_func__(self.__ChangeCategory), i)

			self.header_text_main = self.GetChild("header_text_main")
			self.time_left = self.GetChild("time_left")
			self.item_count = self.GetChild("item_count")
			self.chance_text = self.GetChild("chance_text")
			self.ItemSlot = self.GetChild("item_slot")
			self.ItemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
			self.ItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
			self.blocked_slot = self.GetChild("blocked_slot")

			self.render = self.GetChild("RenderTarget")
			self.location_text = self.GetChild("location_text")
			self.mobname_text = self.GetChild("mobname_text")

			self.collect_button = self.GetChild("collect_button")
			self.collect_button.SetEvent(ui.__mem_func__(self.__ClickCollectButton))
			self.time_button1 = self.GetChild("time_button1")
			self.chance_button = self.GetChild("chance_button")
			self.chance_button.SetEvent(ui.__mem_func__(self.__useitem), 0)
			self.time_button1.SetEvent(ui.__mem_func__(self.__useitem), 1)

			self.PageText = self.GetChild("PageText")
			self.PrevPageBtn = self.GetChild("PrevPageBtn")
			self.NextPageBtn = self.GetChild("NextPageBtn")

			self.PrevPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 0)
			self.NextPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 1)

			self.progressbar = self.GetChild("progressbar")
			
			self.lowlevel_bg = self.GetChild("lowlevel_bg")
			self.lowlevel_text = self.GetChild("lowlevel_text")
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.BindObject")

		self.__ChangeCategory(self.selectedWindow)
		self.SetCenterPosition()

	def Open(self):
		self.__LoadWindow()
		
		# Panel açýldýðýnda tüm kategoriler için veri çek
		try:
			for category in xrange(len(constInfo.CollectWindowQID)):
				if constInfo.CollectWindowQID[category] > 0:
					qid = constInfo.CollectWindowQID[category]
					# openwindow event'ini tetikle (buttonIndex = 2)
					event.QuestButtonClick(qid, 2)
		except:
			pass
		
		ui.ScriptWindow.Show(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __ChangeCategory(self, category):
		self.realwindow = category
		self.selectedWindow = category

		for obj in self.CategoryButtonList:
			obj.Enable()
			obj.SetUp()
		
		# Kategori deðiþtiðinde server'dan veri çek - ilgili quest index'in openwindow event'ini tetikle
		try:
			if category < len(constInfo.CollectWindowQID) and constInfo.CollectWindowQID[category] > 0:
				qid = constInfo.CollectWindowQID[category]
				# openwindow event'ini tetikle (buttonIndex = 2)
				event.QuestButtonClick(qid, 2)
		except:
			pass

		if (player.GetLevel() < self.data[self.selectedWindow]["REQUIRED_LEVEL"]):
			self.lowlevel_bg.Show()
			self.lowlevel_text.SetText("Gereken Seviye: " + str(self.data[self.selectedWindow]["REQUIRED_LEVEL"]))
		else:
			self.lowlevel_bg.Hide()

		self.header_text_main.SetText(CAT_LIST[category])
		self.ItemSlot.SetItemSlot(0, self.data[category]["ITEM_VNUM"], 0)
		self.item_count.SetText(str(self.data[category]["COUNT"])+"/"+str(self.data[category]["COUNT_TOTAL"]))

		if category not in BLOCKED_CAT_LIST:
			self.chance_text.Show()
			self.time_left.Show()
			# self.chance_text.SetText(str(self.data[category]["TAKE_CHANCE"]) + "%")
			self.blocked_slot.Hide()

			self.collect_button.Show()
			self.chance_button.Show()
			self.time_button1.Show()
		else:
			self.chance_text.Hide()
			self.blocked_slot.Show()
			self.time_left.Hide()

			self.collect_button.Hide()
			self.chance_button.Hide()
			self.time_button1.Hide()

		renderTarget.SetBackground(RENDER_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
		
		rendertarget_vnum = self.data[category]["RENDERTARGET_VNUM"]
		
		if rendertarget_vnum > 0:
			try:
				renderTarget.SelectModel(RENDER_INDEX, rendertarget_vnum)
				renderTarget.SetVisibility(RENDER_INDEX, True)
				
				# LOCATION_BY_MOBID dictionary'sinden güvenli eriþim
				location = LOCATION_BY_MOBID.get(rendertarget_vnum, "-")
				self.location_text.SetText("|cFFf1e6c0Konum: |r" + str(location))
				
				# Monster bilgilerini güvenli þekilde al
				try:
					monster_name = nonplayer.GetMonsterName(rendertarget_vnum)
					monster_level = nonplayer.GetMonsterLevel(rendertarget_vnum)
					self.mobname_text.SetText(str(monster_name) + " |cFFa7ff33(Lv. " + str(monster_level) + ")")
				except:
					self.mobname_text.SetText("-")
				
				# Bonus bilgilerini ayarla
				try:
					quest_index = self.data[category]["QUEST_INDEX"]
					for i in xrange(3):
						if category in BONUS_LIST and i in BONUS_LIST[category] and quest_index in BONUS_LIST[category][i]:
							self.bonus_text[i].SetText(BONUS_LIST[category][i][quest_index])
						else:
							self.bonus_text[i].SetText("-")
				except:
					for i in xrange(3):
						self.bonus_text[i].SetText("-")
			except:
				# Hata durumunda model'i reset et
				renderTarget.SetVisibility(RENDER_INDEX, False)
				self.location_text.SetText("|cFFf1e6c0Konum: |r-")
				self.mobname_text.SetText("-")
				for i in xrange(3):
					self.bonus_text[i].SetText("-")
		else:
			# RENDERTARGET_VNUM 0 veya geçersiz ise model'i gizle, SelectModel çaðýrma
			renderTarget.SetVisibility(RENDER_INDEX, False)
			
			self.location_text.SetText("|cFFf1e6c0Konum: |r-")
			self.mobname_text.SetText("-")
			for i in xrange(3):
				self.bonus_text[i].SetText("-")

		# for i in xrange(5):
		# 	self.categorytext[i].SetText(CATEGORY_BUTTON_NAME[i]+" |cFFfffeab("+str(self.data[i]["QUEST_INDEX"])+" / "+str(QUEST_COUNTER[i])+")")

		self.PageText.SetText(str(self.data[category]["QUEST_INDEX"]))

		self.progressbar.SetPercentage(self.data[category]["COUNT"], self.data[category]["COUNT_TOTAL"])

		self.CategoryButtonList[category].Disable()
		self.CategoryButtonList[category].Down()

	def SendCommand(self, type):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		questid = self.data[self.selectedWindow]["QUEST_INDEX"]
		if self.selectedWindow == 1:
			if type == 0:
				if questid == 0 or questid < 0:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid-1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			elif type == 1:
				if questid < 0 or questid == 6 or questid > 6:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid+1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			self.PageText.SetText(str(self.data[self.selectedWindow]["QUEST_INDEX"]))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.COLLECT_ONLY_COLLECTOR)

	def	__OverInItem(self, slot_num):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.data[self.selectedWindow]["ITEM_VNUM"], 0, 0)
		
	def	__OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.HideToolTip()
		
	def AddData(self, windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredLevel):
		self.data[windowType]["ITEM_VNUM"] = itemVnum
		# Server'dan gelen 'time' kalan saniye (remain_sec) - eðer 0'dan büyükse gelecekteki bir zaman damgasý oluþtur
		if time > 0:
			self.data[windowType]["TIME"] = app.GetTime() + time
		else:
			# Eðer time 0 veya negatifse, zamanlama yok (hemen kullanýlabilir)
			self.data[windowType]["TIME"] = 0
		self.data[windowType]["COUNT"] = count
		self.data[windowType]["COUNT_TOTAL"] = countTotal
		self.data[windowType]["TAKE_CHANCE"] = chance
		self.data[windowType]["RENDERTARGET_VNUM"] = rendertargetvnum
		self.data[windowType]["QUEST_INDEX"] = questindex
		self.data[windowType]["REQUIRED_LEVEL"] = requiredLevel
		self.quest = questindex
		# if windowType != self.realwindow:
		self.__ChangeCategory(self.realwindow)

		#chat.AppendChat(chat.CHAT_TYPE_INFO, str(self.data[windowType]))

	def OnUpdate(self):
		# qid = constInfo.CollectWindowQID[self.selectedWindow]
		# chat.AppendChat(chat.CHAT_TYPE_INFO, str(qid))
		if self.selectedWindow not in BLOCKED_CAT_LIST:
			# TIME deðeri 0 ise zamanlama yok, hemen kullanýlabilir
			if self.data[self.selectedWindow]["TIME"] == 0:
				self.time_left.SetText("00:00:00")
			else:
				remaining_time = max(float(self.data[self.selectedWindow]["TIME"])-app.GetTime(), 0)
				if remaining_time <= 0:
					self.time_left.SetText("00:00:00")
				else:
					self.time_left.SetText(time.strftime("%H:%M:%S", time.gmtime(remaining_time)))
			self.chance_text.SetText(str(self.data[self.selectedWindow]["TAKE_CHANCE"])+"%")

	def __ClickCollectButton(self):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		event.QuestButtonClick(qid, 1)

	def __useitem(self, value):
		item_map = {
			0: {1: 71037, 0: 71036},
			1: {1: 72349, 0: 72348}
		}

		item_to_search = item_map.get(value, {}).get(self.selectedWindow)
		if item_to_search is None:
			return

		inventory_ranges = [
			range(player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT),
			range(820, 954)
		]

		for inventory_range in inventory_ranges:
			for i in inventory_range:
				if player.GetItemIndex(i) == item_to_search:
					net.SendItemUsePacket(i)
					return

	def SendTime(self, val, time):
		val_conv = str(val)
		time_conv = float(time)
		# Server'dan gelen 'time' kalan saniye (remain_sec) - eðer 0'dan büyükse gelecekteki bir zaman damgasý oluþtur
		if time_conv > 0:
			time_value = app.GetTime() + time_conv
		else:
			time_value = 0
		if val_conv == str(0):
			self.data[0]["TIME"] = time_value
		elif val_conv == str(1):
			self.data[1]["TIME"] = time_value

	def SendChance(self, val, chance):
		chat.AppendChat(chat.CHAT_TYPE_INFO, str(val)+","+str(chance))
		chance_conv = str(chance)
		val_conv = str(val)
		if val_conv == str(0):
			self.data[0]["TAKE_CHANCE"] = chance_conv
		elif val_conv == str(1):
			self.data[1]["TAKE_CHANCE"] = chance_conv

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True