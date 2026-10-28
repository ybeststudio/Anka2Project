if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

#dyanmic
import grp, chat, event, item, wndMgr

#static
import ui, _weakref, os, cPickle, operator, constInfo, localeInfo, math

#second
LOG_UPDATE_TIME = 1

IMG_DIR = "dungeon_info/"

BG_IMG_DIR = IMG_DIR+"bg/"
ICON_IMG_DIR = IMG_DIR+"icons/"
# NOTIFICATIONS_IMG_DIR = IMG_DIR+"notifications/"

def GetCloneDungeonData(infoDict):
	_dict = {
		"name" : "Bilinmeyen Zindan",
		"image" : "sd3_new.png",
		"levelrange" : [0, 0],
		"group" : False,
		"cooldown" : 0,
		"ticket": [],
		"server_shop_idx" : 1,
		"listIndex" : 0,
		"static_listIndex" : 0,
		"showStatus" : True,
		"alarm" : False,
		"server_cooldown": 0,
		"server_completed":0,
		"server_fastest":0,
		"server_damage":0,
		"selected":True,
		"whisper_info":False,
	}
	for key, value in infoDict.items():
		_dict[key] = value
	return _dict


dungeonData = {

	5127 : GetCloneDungeonData({
		"name": localeInfo.MONKEYDUNGEON,
		"image" : "ape_dungeon.png",
		"levelrange" : [50, 90],
		"group" : False,
		"cooldown" : 60 * 30 * 1,
		"ticket": [[30137, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 0,
		"static_listIndex" : 1,
	}),
	2092 : GetCloneDungeonData({
		"name": localeInfo.SPIDERBARONESA,
		"image" : "sd3_new.png",
		"levelrange" : [60, 90],
		"group" : False,
		"cooldown" : 60 * 60 * 1,
		"ticket": [[25164, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 1,
		"static_listIndex" : 2,
	}),
	1093 : GetCloneDungeonData({
		"name":localeInfo.DEVILTOWER,
		"image" : "deviltower_new.png",
		"levelrange" : [75, 90],
		"group" : False,
		"cooldown" : 60 * 30 * 1,
		"ticket": [[25163, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 2,
		"static_listIndex" : 3,
	}),
	2493 : GetCloneDungeonData({
		"name": localeInfo.BERANSETAOU,
		"image" : "dragon_lair.png",
		"levelrange" : [75, 105],
		"group" : False,
		"cooldown" : 60 * 60 * 1,
		"ticket": [[25165, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 3,
		"static_listIndex" : 4,
	}),
	2598 : GetCloneDungeonData({
		"name": localeInfo.CATACUMB,
		"image" : "devilcatacomb_new.png",
		"levelrange" : [75, 105],
		"group" : False,
		"cooldown" : 60 * 60 * 1,
		"ticket": [[25166, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 4,
		"static_listIndex" : 5,
	}),
	6091 : GetCloneDungeonData({
		"name": localeInfo.FLAMEDUNGEON,
		"image" : "flame_dungeon.png",
		"levelrange" : [90, 105],
		"group" : False,
		"cooldown" : 60 * 60 * 1,
		"ticket": [[25167, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 5,
		"static_listIndex" : 6,
	}),
	6191 : GetCloneDungeonData({
		"name": localeInfo.SNOWDUNGEON,
		"image" : "snow_dungeon.png",

		"levelrange" : [90, 105],
		"group" : False,
		"cooldown" : 60 * 60 * 1,
		"ticket": [[25168, 1]],
		"server_shop_idx" : 1,

		"listIndex" : 6,
		"static_listIndex" : 7,
	}),
	6192 : GetCloneDungeonData({
		"name": localeInfo.JOTUN_DUNGEON,
		"image" : "jotun_dungeon.png",
		"levelrange" : [105, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[30613, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 7,
		"static_listIndex" : 8,
	}),
	9018 : GetCloneDungeonData({
		"name": localeInfo.HYDRADUNGEON,
		"image" : "ship_defense.png",
		"levelrange" : [105, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[25169, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 8,
		"static_listIndex" : 9,
	}),
	9838 : GetCloneDungeonData({
		"name": localeInfo.ZODIACDUNGEON,
		"image" : "chen.png",
		"levelrange" : [106, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[71095, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 9,
		"static_listIndex" : 10,
	}),
	20442 : GetCloneDungeonData({
		"name": localeInfo.MELEYDUNGEON,
		"image" : "meley_dungeon.png",
		"levelrange" : [110, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[25170, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 10,
		"static_listIndex" : 11,
	}),
	6500 : GetCloneDungeonData({
		"name": localeInfo.AKZADURDUNGEON,
		"image" : "akzadur_dungeon2.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[49276, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 11,
		"static_listIndex" : 12,
	}),
	6606 : GetCloneDungeonData({
		"name": localeInfo.ZCFLAMEDUNGEON,
		"image" : "awaken_razador.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[49276, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 12,
		"static_listIndex" : 13,
	}),
	6605 : GetCloneDungeonData({
		"name": localeInfo.ZCSNOWDUNGEON,
		"image" : "awaken_nemere.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[49276, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 13,
		"static_listIndex" : 14,
	}),
	6756 : GetCloneDungeonData({
		"name": localeInfo.SERPENTDUNGEON,
		"image" : "snake.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[70430, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 14,
		"static_listIndex" : 15,
	}),
	6791 : GetCloneDungeonData({
		"name": localeInfo.ALASTORDUNGEON,
		"image" : "alastor.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[70410, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 15,
		"static_listIndex" : 16,
	}),
	6790 : GetCloneDungeonData({
		"name": localeInfo.ALASTORHARDDUNGEON,
		"image" : "alastor.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[70410, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 16,
		"static_listIndex" : 17,
	}),
	6755 : GetCloneDungeonData({
		"name": localeInfo.SUNGMADUNGEON,
		"image" : "Mahi.png",
		"levelrange" : [120, 120],
		"group" : False,
		"cooldown": 60 * 60 * 2,
		"ticket": [[71095, 1]],
		"server_shop_idx" : 1,
		"listIndex" : 17,
		"static_listIndex" : 18,
	}),
}

class DungeonManager(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
	def Destroy(self):
		self.__children={}
	def AddDungeon(self, dungeonBossIdx):
		global dungeonData
		if not dungeonData.has_key(dungeonBossIdx):
			return
		# image = CreateWindow(ui.ImageBox(), self, (0, 0), NOTIFICATIONS_IMG_DIR+dungeonData[dungeonBossIdx]["image"])
		# image.Hide()
		# image.SetEvent(ui.__mem_func__(self.__Click), "mouse_click", dungeonBossIdx)
		# image.leftTime = app.GetTime() + 10.0
		# self.__children[dungeonBossIdx] = image

		# image.text = CreateWindow(MultiTextLine(), image, (45, 8), "")
		# image.text.Hide()
		# image.text.SetTextRange(13)
		# image.text.SetText(localeInfo.DUNGEON_TRACK_NOTIFICATION % dungeonData[dungeonBossIdx]["name"])

		# self.Show()

	def __Click(self, emptyArg, mobIdx):
		if self.__children.has_key(mobIdx):
			leftTime = self.__children[mobIdx].leftTime - app.GetTime()
			self.__children[mobIdx].leftTime = app.GetTime() + (leftTime if leftTime < 1.0 else 1.0)

	def SortWithIndex(self, child):
		return child[1]

	def OnUpdate(self):
		global dungeonData
		(needRemoveList, imageList, now) = ([], [], app.GetTime())
		for key, image in self.__children.items():
			if image.leftTime < now:
				needRemoveList.append(key)
			else:
				imageList.append([key, dungeonData[key]["static_listIndex"], image])

		for mobIdx in needRemoveList:
			self.__children[mobIdx].text = None
			self.__children[mobIdx].Hide()
			del self.__children[mobIdx]

		if len(imageList):
			imageList.sort(key=ui.__mem_func__(self.SortWithIndex))
			xPos, yPos = (wndMgr.GetScreenWidth() / 2) - (119 / 2), 0
			self.SetPosition(xPos, 100)

			for imageListInfo in imageList:
				image = imageListInfo[2]
				image.SetPosition(0, yPos)
				yPos += image.GetHeight() + 4
				leftTime = image.leftTime - now
				if leftTime < 1.0:
					image.SetAlpha(leftTime)
				image.text.Show()
				image.Show()
			self.SetSize(119, yPos)
			self.Show()
		else:
			self.Hide()

#DungeonManagerEx = DungeonManager()

class DungeonTrack(ui.ExpandedImageBox):
	__children = {}
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.__LoadWindow()
	def Destroy(self):
		logManager = self.__children["logManager"] if self.__children.has_key("logManager") else None
		if logManager:
			logManager.Close()
			logManager.Destroy()
			self.__children["logManager"] = None

		self.__children = {}

	def __LoadWindow(self):
		self.Destroy()
		self.LoadImage(IMG_DIR+"bg.png")
		self.AddFlag("float")
		self.AddFlag("movable")
		self.AddFlag("attach")

		titleBar = CreateWindow(ui.TitleBar(), self, (35, 5))
		titleBar.MakeTitleBar(self.GetWidth()-40, "red")
		titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.__children["titleBar"] = titleBar

		titleName = CreateWindow(ui.TextLine(), titleBar, (0, 4), localeInfo.DUNGEON_TRACK_TITLE)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		self.__children["titleName"] = titleName

		scrollBar = CreateWindow(ScrollBarSpecial(False), self, (324, 35), "", "", (7, 376))
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.__children["scrollBar"] = scrollBar

		listWindow = CreateWindow(ui.Window(), self, (18, 41), "", "", (300, 364))
		listWindow.SetInsideRender(True)
		self.__children["listWindow"] = listWindow

		clearBtn = CreateWindow(ui.Button(), self, (8, 5))
		clearBtn.SetUpVisual("d:/ymir work/ui/game/windows/refresh_titlebar_up.sub")
		clearBtn.SetOverVisual("d:/ymir work/ui/game/windows/refresh_titlebar_over.sub")
		clearBtn.SetDownVisual("d:/ymir work/ui/game/windows/refresh_titlebar_down.sub")
		clearBtn.SetEvent(ui.__mem_func__(self.__ClearBtn))
		self.__children["clearBtn"] = clearBtn

		dungeonBtnDict = {}
		global dungeonData
		for mobIdx, dataDict in dungeonData.items():
			btn =  CreateWindow(Dungeon(mobIdx, self), listWindow, (0, 0))
			btn.SetEvent(ui.__mem_func__(self.__ClickDungeon), "mouse_click", mobIdx)
			dungeonBtnDict[str(mobIdx)+"btn"] = btn
		self.__children["dungeonBtnDict"] = dungeonBtnDict

		# Right Table

		rightBG = CreateWindow(ui.ImageBox(), self, (340, 39), BG_IMG_DIR+"ape_dungeon.png")
		self.__children["rightBG"] = rightBG
		#self.__children["clock"] = CreateWindow(ui.ImageBox(), rightBG, (10, 7), IMG_DIR+"clock.png")


		checkBox = CreateWindow(ui.Button(), rightBG, (34, 7))
		checkBox.SetUpVisual(IMG_DIR+"checkbox_new_unselected.tga")
		checkBox.SetOverVisual(IMG_DIR+"checkbox_new_unselected.tga")
		checkBox.SetDownVisual(IMG_DIR+"checkbox_new_unselected.tga")
		#checkBox.SetEvent(ui.__mem_func__(self.__ClickCheckBox))
		#checkBox.SetToolTipText(localeInfo.DUNGEON_TRACK_CHECKBOX)
		self.__children["checkBox"] = checkBox

		self.__children["title"] = CreateWindow(ui.TextLine(), rightBG, (125, 7), "Bilinmeyen Zindan", "horizontal:center")

		wikiBtn = CreateWindow(ui.Button(), rightBG, (222, 7))
		wikiBtn.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		wikiBtn.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
		wikiBtn.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		wikiBtn.SetEvent(ui.__mem_func__(self.__WikiBtn))
		wikiBtn.SetToolTipText("Vikipedi Rehberi")
		self.__children["wikiBtn"] = wikiBtn

		self.__children["level_info"] = CreateWindow(ui.TextLine(), rightBG, (10, 32), localeInfo.DUNGEON_TRACK_LEVEL_INFO)
		self.__children["level_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 32), "0 - 0", "horizontal:right")

		self.__children["group_info"] = CreateWindow(ui.TextLine(), rightBG, (10, 32 + (17 * 1)), localeInfo.DUNGEON_TRACK_GROUP_INFO)
		self.__children["group_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 32 + (17 * 1)), "False", "horizontal:right")

		self.__children["cooldown_info"] = CreateWindow(ui.TextLine(), rightBG, (10, 32 + (17 * 2)), localeInfo.DUNGEON_TRACK_COOLDOWN_INFO)
		self.__children["cooldown_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 32 + (17 * 2)), "0s", "horizontal:right")

		self.__children["ticket_info"] = CreateWindow(ui.TextLine(), rightBG, (10, 32 + (17 * 4)), localeInfo.DUNGEON_TRACK_TICKET_INFO)

		shopBtn = CreateWindow(ui.Button(), rightBG, (205, 96))
		shopBtn.SetUpVisual(IMG_DIR+"shop_0.png")
		shopBtn.SetOverVisual(IMG_DIR+"shop_1.png")
		shopBtn.SetDownVisual(IMG_DIR+"shop_2.png")
		shopBtn.SetEvent(ui.__mem_func__(self.__ShopBtn))
		shopBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_SHOP_INFO)
		self.__children["shopBtn"] = shopBtn

		enterBtn = CreateWindow(ui.Button(), rightBG, (40, 324))
		enterBtn.SetUpVisual(IMG_DIR+"teleport_into_0.png")
		enterBtn.SetOverVisual(IMG_DIR+"teleport_into_1.png")
		enterBtn.SetDownVisual(IMG_DIR+"teleport_into_2.png")
		enterBtn.SAFE_SetEvent(self.__ClickQuest, "ENTER")
		enterBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_TELEPORT_ENTER)
		self.__children["enterBtn"] = enterBtn

		warpBtn = CreateWindow(ui.Button(), rightBG, (10 + 50 + 80, 324))
		warpBtn.SetUpVisual(IMG_DIR+"teleport_0.png")
		warpBtn.SetOverVisual(IMG_DIR+"teleport_1.png")
		warpBtn.SetDownVisual(IMG_DIR+"teleport_2.png")
		warpBtn.SAFE_SetEvent(self.__ClickQuest, "WARP")
		warpBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_WARP)
		self.__children["warpBtn"] = warpBtn

		# testBtn = CreateWindow(ui.Button(), rightBG, (10 + 80 + 80, 324))
		# testBtn.SetUpVisual(IMG_DIR+"test_boss_0.png")
		# testBtn.SetOverVisual(IMG_DIR+"test_boss_1.png")
		# testBtn.SetDownVisual(IMG_DIR+"test_boss_2.png")
		# testBtn.SAFE_SetEvent(self.__ClickQuest, "TEST")
		# testBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_TEST)
		# self.__children["testBtn"] = testBtn

		self.__children["personal_info"] = CreateWindow(ui.TextLine(), rightBG, (125, 150), localeInfo.DUNGEON_TRACK_PERSONAL_STATISTICS, "horizontal:center")

		completedRank = CreateWindow(ui.Button(), rightBG, (10, 183))
		completedRank.SetUpVisual(IMG_DIR+"completed_0.png")
		completedRank.SetOverVisual(IMG_DIR+"completed_1.png")
		completedRank.SetDownVisual(IMG_DIR+"completed_2.png")
		completedRank.SAFE_SetEvent(self.__ClickRank, 0)
		completedRank.SetToolTipText(localeInfo.DUNGEON_TRACK_RANK_COMPLETED)
		self.__children["completedRank"] = completedRank

		self.__children["completed_info"] = CreateWindow(ui.TextLine(), rightBG, (10+38, 183 + 2), localeInfo.DUNGEON_TRACK_COMPLETED)
		self.__children["completed_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 183 + 2), "0", "horizontal:right")

		fastRank = CreateWindow(ui.Button(), rightBG, (10, 183 + 25))
		fastRank.SetUpVisual(IMG_DIR+"fastest_0.png")
		fastRank.SetOverVisual(IMG_DIR+"fastest_1.png")
		fastRank.SetDownVisual(IMG_DIR+"fastest_2.png")
		fastRank.SAFE_SetEvent(self.__ClickRank, 1)
		fastRank.SetToolTipText(localeInfo.DUNGEON_TRACK_RANK_FASTEST)
		self.__children["fastRank"] = fastRank

		self.__children["fast_info"] = CreateWindow(ui.TextLine(), rightBG, (10+38, 183 + 2 + 25), localeInfo.DUNGEON_TRACK_FASTEST)
		self.__children["fast_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 183 + 2 + 25), "0s", "horizontal:right")

		damageRank = CreateWindow(ui.Button(), rightBG, (10, 183 + 25 + 25))
		damageRank.SetUpVisual(IMG_DIR+"damage_0.png")
		damageRank.SetOverVisual(IMG_DIR+"damage_1.png")
		damageRank.SetDownVisual(IMG_DIR+"damage_2.png")
		damageRank.SAFE_SetEvent(self.__ClickRank, 2)
		damageRank.SetToolTipText(localeInfo.DUNGEON_TRACK_RANK_DMG)
		self.__children["damageRank"] = damageRank

		self.__children["damage_info"] = CreateWindow(ui.TextLine(), rightBG, (10+38, 183 + 2 + 25 + 25), localeInfo.DUNGEON_TRACK_DMG)
		self.__children["damage_text"] = CreateWindow(ui.TextLine(), rightBG, (222+14, 183 + 2 + 25 + 25), "0", "horizontal:right")

		self.SetCenterPosition()
		LoadSettings()
		self.Refresh()

	def GetCurrentSelectedDungeon(self):
		global dungeonData
		for mobIdx, dungeonInfo in dungeonData.items():
			if dungeonInfo["selected"]:
				return mobIdx
		return -1

	def LoadServerData(self, mobIdx, rankIdx, cmd):
		logManager = self.__children["logManager"] if self.__children.has_key("logManager") else None
		if logManager:
			logManager.LoadServerData(int(mobIdx), int(rankIdx), cmd)
	
	def __ClickRank(self, rankIdx):
		logManager = self.__children["logManager"] if self.__children.has_key("logManager") else None
		if not logManager:
			logManager = LogWindow()
			self.__children["logManager"] = logManager
		logManager.Open(self.GetCurrentSelectedDungeon(), rankIdx)

	def __ClickQuest(self, questMode):
		currentDungeon = self.GetCurrentSelectedDungeon()
		if currentDungeon == -1:
			return
		constInfo.dungeonInfoCMD = questMode + "#" + str(currentDungeon)
		event.QuestButtonClick(int(constInfo.dungeonInfoQuestIdx))

	def __ShopBtn(self):
		currentDungeon = self.GetCurrentSelectedDungeon()
		if currentDungeon != -1:
			lastShopBtnClick = self.__children["lastShopBtnClick"] if self.__children.has_key("lastShopBtnClick") else 0
			if lastShopBtnClick > app.GetGlobalTimeStamp():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DUNGEON_TRACK_SLOW_INFO)
				return
			self.__children["lastShopBtnClick"] = app.GetGlobalTimeStamp() + 2

			global dungeonData
			net.SendChatPacket("/open_shop 12".format(dungeonData[currentDungeon]["server_shop_idx"]))

	def __WikiBtn(self):
		currentDungeon = self.GetCurrentSelectedDungeon()
		__wikiCommandDict = {
			1093 : "article#0",
		}
		if __wikiCommandDict.has_key(currentDungeon):
			interface = constInfo.GetInterfaceInstance()
			if interface:
				interface.wndWiki.Open()
				interface.wndWiki.LoadData(__wikiCommandDict[currentDungeon])
				interface.wndWiki.SetTop()

	def __ClickCheckBox(self):
		currentDungeon = self.GetCurrentSelectedDungeon()
		if currentDungeon != -1:
			global dungeonData
			dungeonData[currentDungeon]["alarm"] = not dungeonData[currentDungeon]["alarm"]
			self.Refresh(False, False, True)

	def __ClickDungeon(self, emptyArg, mobIdx):
		global dungeonData
		for key, dungeonInfo in dungeonData.items():
			dungeonInfo["selected"] = False
		if dungeonData.has_key(mobIdx):
			dungeonData[mobIdx]["selected"] = True
		self.Refresh()
		SaveSettings()

	def __OnScroll(self):
		self.Refresh(False, False, False)

	def __ClearBtn(self):
		ClearSettings()
		self.Refresh(True)

	def SortWithIndex(self, child):
		return child.GetIndex()

	def OnRunMouseWheel(self, nLen):
		scrollBar = self.__children["scrollBar"] if self.__children.has_key("scrollBar") else None
		if scrollBar:
			if scrollBar.IsShow():
				if nLen > 0:
					scrollBar.OnUp()
				else:
					scrollBar.OnDown()
				return TRUE
		return FALSE

	def Refresh(self, resetPos = False, refreshItems = True, refreshRightTable = True):
		dungeonBtnDict = self.__children["dungeonBtnDict"]
		btnList = []
		for key, dungeonBtn in dungeonBtnDict.items():
			dungeonBtn.Refresh()
			if dungeonBtn.GetShowStatus():
				dungeonBtn.Show()
				btnList.append(dungeonBtn)
			else:
				dungeonBtn.Hide()
		btnList.sort(key=ui.__mem_func__(self.SortWithIndex))
		scrollBar = self.__children["scrollBar"]
		if resetPos:
			scrollBar.SetPos(0)
		if len(btnList):
			scrollLen = 0
			(screenSize, windowHeight) = (len(btnList) * (46 + 7), 364)
			if screenSize != 0:
				screenSize -= 7
			if screenSize > windowHeight:
				scrollLen = screenSize-windowHeight
				scrollBar.SetScale(windowHeight, screenSize)
				basePos = int(scrollBar.GetPos()*scrollLen)
			else:
				scrollBar.SetScale(376, 376)
				basePos = 0
			yPos = 0
			for child in btnList:
				child.SetPosition(0, yPos - basePos)
				yPos += child.GetHeight() + 7
		else:
			scrollBar.SetScale(376, 376)
		
		if refreshRightTable:
			currentDungeon = self.GetCurrentSelectedDungeon()
			if currentDungeon != -1:
				global dungeonData
				dungeonInfo = dungeonData[currentDungeon]
				self.__children["title"].SetText(dungeonInfo["name"])
				self.__children["level_text"].SetText(str(dungeonInfo["levelrange"][0]) + " - " + str(dungeonInfo["levelrange"][1]))
				self.__children["group_text"].SetText(localeInfo.DUNGEON_TRACK_TRUE if dungeonInfo["group"] else localeInfo.DUNGEON_TRACK_FALSE)
				self.__children["cooldown_text"].SetText(FormatTimeEx(dungeonInfo["cooldown"]))

				self.__children["completed_text"].SetText(NumberToDecimalString(dungeonInfo["server_completed"]))
				self.__children["fast_text"].SetText(FormatTimeEx(dungeonInfo["server_fastest"]))
				self.__children["damage_text"].SetText(NumberToDecimalString(dungeonInfo["server_damage"]))

				checkBox = self.__children["checkBox"]
				checkBox.SetUpVisual(IMG_DIR+"checkbox_new_unselected.tga" if not dungeonInfo["alarm"] else IMG_DIR+"checkbox_new_selected.tga")
				checkBox.SetOverVisual(IMG_DIR+"checkbox_new_unselected.tga" if not dungeonInfo["alarm"] else IMG_DIR+"checkbox_new_selected.tga")
				checkBox.SetDownVisual(IMG_DIR+"checkbox_new_unselected.tga" if not dungeonInfo["alarm"] else IMG_DIR+"checkbox_new_selected.tga")

				rightBG = self.__children["rightBG"]

				rightBG.LoadImage(BG_IMG_DIR+dungeonInfo["image"])

				# Clear old ticket images first
				if self.__children.has_key("ticketImageList"):
					for oldItem in self.__children["ticketImageList"]:
						if oldItem:
							oldItem.Hide()
							oldItem.SetParent(None)
							oldItem = None

				ticketItemList, ticketImageList = dungeonInfo["ticket"], []
				for j in xrange(len(ticketItemList)):
					item.SelectItem(ticketItemList[j][0])
					xCalculate = 163 - (((len(ticketItemList) - 1) - j) * 37)
					itemIcon = CreateWindow(ui.ExpandedImageBox(), rightBG, (xCalculate, 96), item.GetIconImageFileName())
					# Store itemIdx as attribute for event handler
					itemIdx = ticketItemList[j][0]
					itemIcon.itemIdx = itemIdx
					# Use SetOverInEvent/SetOverOutEvent with wrapper methods
					itemIcon.SetOverInEvent(ui.__mem_func__(self.__OverInItemWrapper))
					itemIcon.SetOverOutEvent(ui.__mem_func__(self.__OverOutItem))
					ticketImageList.append(itemIcon)
					if ticketItemList[j][1] > 1:
						itemCounText = CreateWindow(ui.TextLine(), rightBG, (xCalculate + 22, 96 + 22), str(ticketItemList[j][1]))
						itemCounText.SetOutline()
						itemCounText.AddFlag("not_pick")  # Make text not pickable so it doesn't block tooltip
						ticketImageList.append(itemCounText)
				self.__children["ticketImageList"] = ticketImageList

	def __OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()
	def __OverInItemWrapper(self):
		# Get itemIdx from the itemIcon that triggered the event
		# This is a workaround since SetOverInEvent doesn't pass parameters
		import wndMgr
		try:
			(x, y) = wndMgr.GetMousePosition()
			# Find which itemIcon the mouse is over
			if self.__children.has_key("ticketImageList"):
				for itemIcon in self.__children["ticketImageList"]:
					if isinstance(itemIcon, ui.ExpandedImageBox) and hasattr(itemIcon, 'itemIdx'):
						(ix, iy) = itemIcon.GetGlobalPosition()
						iw, ih = itemIcon.GetWidth(), itemIcon.GetHeight()
						if ix <= x <= ix + iw and iy <= y <= iy + ih:
							self.__OverInItem(itemIcon.itemIdx)
							return
		except:
			pass
	def __OverInItem(self, itemIdx):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.SetItemToolTip(itemIdx)
				interface.tooltipItem.ShowToolTip()
	def OnUpdate(self):
		dungeonBtnDict = self.__children["dungeonBtnDict"]
		for key, dungeonBtn in dungeonBtnDict.items():
			dungeonBtn.OnUpdate()
	def Open(self):
		self.Refresh()
		self.Show()
		self.SetTop()
	def Close(self):
		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True

class Dungeon(ui.ExpandedImageBox):
	def Destroy(self):
		self.__children={}
	def __init__(self, mobIdx, parent):
		ui.ExpandedImageBox.__init__(self)
		self.__Load(mobIdx, parent)
	
	def GetShowStatus(self):
		global dungeonData
		return dungeonData[self.__children["mobIdx"]]["showStatus"]

	def GetIndex(self):
		global dungeonData
		return dungeonData[self.__children["mobIdx"]]["listIndex"]

	def __Load(self, mobIdx, parent):
		self.Destroy()
		self.__children["mobIdx"] = mobIdx
		self.__children["parent"] = _weakref.proxy(parent)

		global dungeonData
		data = dungeonData[mobIdx]

		self.__children["name"] = CreateWindow(ui.TextLine(), self, (75, 17), data["name"])
		self.__children["image"] = CreateWindow(ui.ExpandedImageBox(), self, (1, 0), ICON_IMG_DIR+data["image"])
		self.__children["image"].AddFlag("not_pick")
		self.__children["status"] = CreateWindow(ui.TextLine(), self, (253, 10), "", "horizontal:right")
		self.__children["time"] = CreateWindow(ui.TextLine(), self, (243, 23), "", "horizontal:right")

		backBtn = CreateWindow(ui.Button(), self, (55, 5))
		backBtn.SetWindowName("not_render")
		backBtn.SetUpVisual(IMG_DIR+"up_0.png")
		backBtn.SetOverVisual(IMG_DIR+"up_1.png")
		backBtn.SetDownVisual(IMG_DIR+"up_2.png")
		backBtn.SetEvent(ui.__mem_func__(self.__BackBtn))
		backBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_MOVE_UP)
		self.__children["backBtn"] = backBtn

		nextBtn = CreateWindow(ui.Button(), self, (55, 25))
		nextBtn.SetWindowName("not_render")
		nextBtn.SetUpVisual(IMG_DIR+"down_0.png")
		nextBtn.SetOverVisual(IMG_DIR+"down_1.png")
		nextBtn.SetDownVisual(IMG_DIR+"down_2.png")
		nextBtn.SetEvent(ui.__mem_func__(self.__NextBtn))
		nextBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_MOVE_DOWN)
		self.__children["nextBtn"] = nextBtn

		eyeBtn = CreateWindow(ui.Button(), self, (0, 33))
		eyeBtn.SetUpVisual(IMG_DIR+"btn_hide_1.png")
		eyeBtn.SetOverVisual(IMG_DIR+"btn_hide_2.png")
		eyeBtn.SetDownVisual(IMG_DIR+"btn_hide_3.png")
		eyeBtn.SetEvent(ui.__mem_func__(self.__ShowBtn))
		eyeBtn.SetToolTipText(localeInfo.DUNGEON_TRACK_HIDE)
		self.__children["eyeBtn"] = eyeBtn

	def SortWithIndex(self, child):
		return child[1]
	def FindIndexInList(self, idx, goingType):
		global dungeonData
		btnList = []
		for mobIdx, dungeonDict in dungeonData.items():
			if not dungeonDict["showStatus"]:
				continue
			btnList.append([mobIdx, dungeonDict["listIndex"]])
		btnList.sort(key=ui.__mem_func__(self.SortWithIndex), reverse=not goingType)
		for child in btnList:
			if goingType:
				if child[1] >= idx:
					return child
			else:
				if child[1] <= idx:
					return child
		return -1

	def __BackBtn(self):
		global dungeonData
		listIdx = dungeonData[self.__children["mobIdx"]]["listIndex"]
		if listIdx <= 0:
			return
		destIndex = self.FindIndexInList(listIdx-1, False)
		if destIndex != -1:
			dungeonData[destIndex[0]]["listIndex"] = listIdx
			dungeonData[self.__children["mobIdx"]]["listIndex"] = destIndex[1]
		else:
			dungeonData[self.__children["mobIdx"]]["listIndex"] -= 1
		self.__children["parent"].Refresh(False, True, False)
		SaveSettings()

	def __NextBtn(self):
		global dungeonData
		listIdx = dungeonData[self.__children["mobIdx"]]["listIndex"]
		if listIdx >= len(dungeonData)-1:
			return
		destIndex = self.FindIndexInList(listIdx+1, True)
		if destIndex != -1:
			dungeonData[destIndex[0]]["listIndex"] = listIdx
			dungeonData[self.__children["mobIdx"]]["listIndex"] = destIndex[1]
		else:
			dungeonData[self.__children["mobIdx"]]["listIndex"] += 1

		self.__children["parent"].Refresh(False, True, False)
		SaveSettings()

	def __ShowBtn(self):
		global dungeonData
		dungeonData[self.__children["mobIdx"]]["showStatus"] = False
		self.__children["parent"].Refresh(True, True, False)
		SaveSettings()

	def Refresh(self):
		global dungeonData
		data = dungeonData[self.__children["mobIdx"]]

		isDeactive = True if data["server_cooldown"] > app.GetGlobalTimeStamp() or player.GetStatus(player.LEVEL) < data["levelrange"][0] or player.GetStatus(player.LEVEL) > data["levelrange"][1] else False
		#isDeactive = True if data["server_cooldown"] > app.GetGlobalTimeStamp() else False

		self.__children["status"].SetText(localeInfo.DUNGEON_TRACK_DEACTIVE if isDeactive else localeInfo.DUNGEON_TRACK_ACTIVE)
		
		if isDeactive:
			self.__children["time"].Show()
		else:
			self.__children["time"].Hide()

			if not data["whisper_info"]:
				if data["alarm"]:
					game = constInfo.GetGameInstance()
					if game != None:
						game.BINARY_OnRecvBulkWhisper(localeInfo.DUNGEON_TRACK_WHISPER_MESSAGE % data["name"])

					interface = constInfo.GetInterfaceInstance()
					if interface != None:
						interface.wndDungeonManager.AddDungeon(self.__children["mobIdx"])

					#global DungeonManagerEx
					#DungeonManagerEx.AddDungeon(self.__children["mobIdx"])

				data["whisper_info"] = True
		
		if player.GetStatus(player.LEVEL) < data["levelrange"][0]:
			self.__children["time"].SetText(localeInfo.DUNGEON_TRACK_LOW_LEVEL)
			self.__children["time"].Show()
		elif player.GetStatus(player.LEVEL) > data["levelrange"][1]:
			self.__children["time"].SetText(localeInfo.DUNGEON_TRACK_HIGH_LEVEL)
			self.__children["time"].Show()

		currentImage = IMG_DIR+"quest_red.png" if isDeactive else IMG_DIR+"quest_blue.png"
		if currentImage != (self.__children["currentImage"] if self.__children.has_key("currentImage") else ""):
			self.__children["currentImage"] = currentImage
			self.LoadImage(currentImage)
		self.SetAlpha(1.0 if data["selected"] else 0.6)

	def OnUpdate(self):
		global dungeonData

		lastUpdate = self.__children["lastUpdate"] if self.__children.has_key("lastUpdate") else 0
		if lastUpdate < app.GetTime():
			self.__children["lastUpdate"] = app.GetTime() + 0.3
			leftTime = dungeonData[self.__children["mobIdx"]]["server_cooldown"] - app.GetGlobalTimeStamp()
			if leftTime <= 0:
				self.Refresh()
			else:
				self.__children["time"].SetText(FormatTime(leftTime))

		if self.__children["eyeBtn"].IsIn() or self.__children["backBtn"].IsIn() or self.__children["nextBtn"].IsIn():
			return
		if self.IsIn():
			listIdx = dungeonData[self.__children["mobIdx"]]["listIndex"]
			if listIdx > 0:
				self.__children["backBtn"].Show()
			else:
				self.__children["backBtn"].Hide()
			if listIdx < len(dungeonData)-1:
				self.__children["nextBtn"].Show()
			else:
				self.__children["nextBtn"].Hide()
			self.__children["eyeBtn"].Show()
		else:
			self.__children["backBtn"].Hide()
			self.__children["nextBtn"].Hide()
			self.__children["eyeBtn"].Hide()

def NumberToDecimalString(n):
	return str('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

def FormatTimeEx(seconds):
	if seconds <= 0:
		return "0s"
	m, s = divmod(seconds, 60)
	h, m = divmod(m, 60)
	d, h = divmod(h, 24)
	text = ""
	if d > 0:
		text+="{}d ".format(d)
	if h > 0:
		text+="{}h ".format(h)
	if m > 0:
		text+="{}m ".format(m)
	if s > 0:
		text+="{}s ".format(s)
	return text[:len(text)-1] if len(text) > 0 else "0s"

def FormatTime(seconds):
	if seconds <= 0:
		return "00:00:00"
	m, s = divmod(seconds, 60)
	h, m = divmod(m, 60)
	d, h = divmod(h, 24)
	return "%02d:%02d:%02d"% (h, m, s)

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	window.SetPosition(*windowPos)
	if windowSize != (-1, -1):
		window.SetSize(*windowSize)
	if windowPositionRule:
		splitList = windowPositionRule.split(":")
		if len(splitList) == 2:
			(type, mode) = (splitList[0], splitList[1])
			if type == "horizontal":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetHorizontalAlignCenter()
					elif mode == "right":
						window.SetHorizontalAlignRight()
					elif mode == "left":
						window.SetHorizontalAlignLeft()
				else:
					if mode == "center":
						window.SetWindowHorizontalAlignCenter()
					elif mode == "right":
						window.SetWindowHorizontalAlignRight()
					elif mode == "left":
						window.SetWindowHorizontalAlignLeft()
			elif type == "vertical":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetVerticalAlignCenter()
					elif mode == "top":
						window.SetVerticalAlignTop()
					elif mode == "bottom":
						window.SetVerticalAlignBottom()
				else:
					if mode == "top":
						window.SetWindowVerticalAlignTop()
					elif mode == "center":
						window.SetWindowVerticalAlignCenter()
					elif mode == "bottom":
						window.SetWindowVerticalAlignBottom()
	if windowArgument:
		if isinstance(window, ui.TextLine):
			if windowFontName != -1:
				window.SetFontName(windowFontName)
			window.SetText(windowArgument)
		elif isinstance(window, ui.NumberLine):
			window.SetNumber(windowArgument)
		elif isinstance(window, ui.ExpandedImageBox) or isinstance(window, ui.ImageBox):
			window.LoadImage(windowArgument if windowArgument.find("gr2") == -1 else "icon/item/27995.tga")
	window.Show()
	return window
def ClearSettings():
	global dungeonData
	for mobIdx, dungeonInfo in dungeonData.items():
		dungeonInfo["listIndex"] = dungeonInfo["static_listIndex"]
		dungeonInfo["showStatus"] = True
		#dungeonInfo["alarm"] = False
	SaveSettings()
def LoadSettings():
	global dungeonData
	try:
		f = open("lib/{}_track".format(player.GetName()), "rb")
		currentSettings = cPickle.load(f)
		f.close()
		for mobIdx, settingData in currentSettings.items():
			if dungeonData.has_key(mobIdx):
				dungeonInfo = dungeonData[mobIdx]
				dungeonInfo["listIndex"] = settingData["listIndex"]
				dungeonInfo["showStatus"] = settingData["showStatus"]
				dungeonInfo["alarm"] = settingData["alarm"]
				dungeonInfo["selected"] = settingData["selected"]
	except IOError:
		pass
def SaveSettings():
	global dungeonData
	with open("lib/{}_track".format(player.GetName()), 'wb') as f:
		cPickle.dump(dungeonData, f)

class ScrollBarSpecial(ui.Window):
	BASE_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
	CORNERS_AND_LINES_COLOR = grp.GenerateColor(0.3411, 0.3411, 0.3411, 1.0)
	BAR_NUMB = 9 #This is static value! Please dont touch in him.
	SCROLL_WIDTH= 8
	class MiddleBar(ui.DragButton):
		MIDDLE_BAR_COLOR = grp.GenerateColor(0.6470, 0.6470, 0.6470, 1.0)
		def __init__(self, horizontal_scroll):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.horizontal_scroll = horizontal_scroll
			self.middle = ui.Bar()
			self.middle.SetParent(self)
			self.middle.AddFlag("attach")
			self.middle.AddFlag("not_pick")
			self.middle.SetColor(self.MIDDLE_BAR_COLOR)
			self.middle.SetSize(1, 1)
			self.middle.Show()
		def SetStaticScale(self, size):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, size)
				self.middle.SetSize(base_width, size)
			else:
				ui.DragButton.SetSize(self, size, base_height)
				self.middle.SetSize(size, base_height)
		def SetSize(self, selfSize, fullSize):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, operator.truediv(int(selfSize), int(fullSize)) * selfSize)
				self.middle.SetSize(base_width, operator.truediv(int(selfSize), int(fullSize)) * selfSize)
			else:
				ui.DragButton.SetSize(self, operator.truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
				self.middle.SetSize(operator.truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
		def SetStaticSize(self, size):
			size = max(2, size)
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, size, self.middle.GetHeight())
				self.middle.SetSize(size, self.middle.GetHeight())
			else:
				ui.DragButton.SetSize(self, self.middle.GetWidth(), size)
				self.middle.SetSize(self.middle.GetWidth(), size)
	def __init__(self, horizontal_scroll = False):
		ui.Window.__init__(self)
		self.horizontal_scroll = horizontal_scroll
		self.scrollEvent = None
		self.scrollSpeed = 50
		self.sizeScale = 1.0
		self.bars = []
		for i in xrange(self.BAR_NUMB):
			br = ui.Bar()
			br.SetParent(self)
			br.AddFlag("attach")
			br.AddFlag("not_pick")
			br.SetColor([self.CORNERS_AND_LINES_COLOR, self.BASE_COLOR][i == (self.BAR_NUMB-1)])
			if not (i % 2 == 0): br.SetSize(1, 1)
			br.Show()
			self.bars.append(br)
		self.middleBar = self.MiddleBar(self.horizontal_scroll)
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.OnScrollMove))
		self.middleBar.Show()
	def OnScrollMove(self):
		if not self.scrollEvent:
			return
		arg = float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else\
				float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
		self.scrollEvent(arg)
	def SetScrollEvent(self, func):
		self.scrollEvent = func
	def SetScrollSpeed(self, speed):
		self.scrollSpeed = speed
	def OnUp(self):
		if not self.IsShow():
			return False
		if not self.horizontal_scroll:
			currentPos = self.middleBar.GetLocalPosition()[1]
			step = max(1, int(self.scrollSpeed * 0.1))
			val = max(1, currentPos - step)
			self.middleBar.SetPosition(1, val)
		else:
			currentPos = self.middleBar.GetLocalPosition()[0]
			step = max(1, int(self.scrollSpeed * 0.1))
			val = max(1, currentPos - step)
			self.middleBar.SetPosition(val, 1)
		self.OnScrollMove()
		return True
	def OnDown(self):
		if not self.IsShow():
			return False
		if not self.horizontal_scroll:
			currentPos = self.middleBar.GetLocalPosition()[1]
			maxPos = self.GetHeight() - self.middleBar.GetHeight() - 1
			step = max(1, int(self.scrollSpeed * 0.1))
			val = min(maxPos, currentPos + step)
			self.middleBar.SetPosition(1, val)
		else:
			currentPos = self.middleBar.GetLocalPosition()[0]
			maxPos = self.GetWidth() - self.middleBar.GetWidth() - 1
			step = max(1, int(self.scrollSpeed * 0.1))
			val = min(maxPos, currentPos + step)
			self.middleBar.SetPosition(val, 1)
		self.OnScrollMove()
		return True
	def OnMouseWheel(self, length):
		if not self.IsShow():
			return False
		length = int((length * 0.01) * self.scrollSpeed)
		if not self.horizontal_scroll:
			val = min(max(1, self.middleBar.GetLocalPosition()[1] - (length * 0.01) * self.scrollSpeed * self.sizeScale), self.GetHeight() - self.middleBar.GetHeight() - 1)
			self.middleBar.SetPosition(1, val)
		else:
			val = min(max(1, self.middleBar.GetLocalPosition()[0] - (length * 0.01) *  self.scrollSpeed * self.sizeScale), self.GetWidth() - self.middleBar.GetWidth() - 1)
			self.middleBar.SetPosition(val, 1)
		self.OnScrollMove()
		return True
	def GetPos(self):
		return float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		if not self.horizontal_scroll:
			if xMouseLocalPosition == 0 or xMouseLocalPosition == self.GetWidth():
				return
			y_pos = (yMouseLocalPosition - self.middleBar.GetHeight() / 2)
			self.middleBar.SetPosition(1, y_pos)
		else:
			if yMouseLocalPosition == 0 or yMouseLocalPosition == self.GetHeight():
				return
			x_pos = (xMouseLocalPosition - self.middleBar.GetWidth() / 2)
			self.middleBar.SetPosition(x_pos, 1)
		self.OnScrollMove()
	def SetSize(self, w, h):
		(width, height) = (max(3, w), max(3, h))
		ui.Window.SetSize(self, width, height)
		self.bars[0].SetSize(1, (height - 2))
		self.bars[0].SetPosition(0, 1)
		self.bars[2].SetSize((width - 2), 1)
		self.bars[2].SetPosition(1, 0)
		self.bars[4].SetSize(1, (height - 2))
		self.bars[4].SetPosition((width - 1), 1)
		self.bars[6].SetSize((width - 2), 1)
		self.bars[6].SetPosition(1, (height - 1))
		self.bars[8].SetSize((width - 2), (height - 2))
		self.bars[8].SetPosition(1, 1)
		self.bars[1].SetPosition(0, 0)
		self.bars[3].SetPosition((width - 1), 0)
		self.bars[5].SetPosition((width - 1), (height - 1))
		self.bars[7].SetPosition(0, (height - 1))
		if not self.horizontal_scroll:
			self.middleBar.SetStaticSize(width - 2)
			self.middleBar.SetSize(12, self.GetHeight())
		else:
			self.middleBar.SetStaticSize(height - 2)
			self.middleBar.SetSize(12, self.GetWidth())
		self.middleBar.SetRestrictMovementArea(1, 1, width - 2, height - 2)
	def SetScale(self, selfSize, fullSize):
		self.sizeScale = float(selfSize)/float(fullSize)
		if self.sizeScale <= 0.0305:
			self.sizeScale = 0.05
		self.middleBar.SetSize(selfSize, fullSize)
	def SetStaticScale(self, r_size):
		self.middleBar.SetStaticScale(r_size)
	def SetPosScale(self, fScale):
		pos = (math.ceil((self.GetHeight() - 2 - self.middleBar.GetHeight()) * fScale) + 1) if not self.horizontal_scroll else (math.ceil((self.GetWidth() - 2 - self.middleBar.GetWidth()) * fScale) + 1)
		self.SetPos(pos)
	def SetPos(self, pos):
		wPos = (1, pos) if not self.horizontal_scroll else (pos, 1)
		self.middleBar.SetPosition(*wPos)

class MultiTextLine(ui.Window):
	def Destroy(self):
		self.textRules = {}
	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
		self.AddFlag("not_pick")
		self.textRules["textRange"] = 15
		self.textRules["text"] = ""
		self.textRules["textType"] = ""
		self.textRules["fontName"] = ""
		self.textRules["hexColor"] = 0
		self.textRules["fontColor"] = 0
		self.textRules["outline"] = 0
	def SetTextType(self, textType):
		self.textRules["textType"] = textType
		self.Refresh()
	def SetTextRange(self, textRange):
		self.textRules["textRange"] = textRange
		self.Refresh()
	def SetOutline(self, outline):
		self.textRules["outline"] = outline
		self.Refresh()
	def SetPackedFontColor(self, hexColor):
		self.textRules["hexColor"] = hexColor
		self.Refresh()
	def SetFontColor(self, r, g, b):
		self.textRules["fontColor"] =[r, g, b]
		self.Refresh()
	def SetFontName(self, fontName):
		self.textRules["fontName"] = fontName
		self.Refresh()
	def SetText(self, newText):
		self.textRules["text"] = newText
		self.Refresh()
	def Refresh(self):
		textRules = self.textRules
		if textRules["text"] == "":
			return
		self.children=[]
		outline = textRules["outline"]
		fontColor = textRules["fontColor"]
		hexColor = textRules["hexColor"]
		yRange = textRules["textRange"]
		fontName = textRules["fontName"]
		textType = textRules["textType"].split("#")
		totalTextList = textRules["text"].split("#")

		(xPosition, yPosition) = (0, 0)
		width = 0
		for text in totalTextList:
			childText = ui.TextLine()
			childText.SetParent(self)
			childText.AddFlag("not_pick")
			childText.SetPosition(xPosition, yPosition)
			if fontName != "":
				childText.SetFontName(fontName)
			if hexColor != 0:
				childText.SetPackedFontColor(hexColor)
			if fontColor != 0:
				childText.SetFontColor(*fontColor)
			if outline:
				childText.SetOutline()
			self.AddTextType(childText, textType)
			childText.SetText(str(text))
			if childText.GetTextSize()[0] > width:
				width = childText.GetTextSize()[0]
			childText.Show()
			self.children.append(childText)
			yPosition+=yRange

	def AddTextType(self, text,  typeArg):
		if len(typeArg) != 2:
			return
		_typeDict = {
			"vertical": {
				"top":text.SetVerticalAlignTop,
				"bottom":text.SetVerticalAlignBottom,
				"center":text.SetVerticalAlignCenter,
			},
			"horizontal": {
				"left":text.SetHorizontalAlignLeft,
				"right":text.SetHorizontalAlignRight,
				"center":text.SetHorizontalAlignCenter,
			},
			"all_align": {
				"1" : [text.SetHorizontalAlignCenter,text.SetVerticalAlignCenter,text.SetWindowHorizontalAlignCenter,text.SetWindowVerticalAlignCenter],
			},
		}
		(firstToken, secondToken) = tuple(typeArg)
		if _typeDict.has_key(firstToken):
			textType = _typeDict[firstToken][secondToken] if _typeDict[firstToken].has_key(secondToken) else None
			if textType != None:
				if isinstance(textType, list):
					for rule in textType:
						rule()
				else:
					textType()

class LogWindow(ui.BoardWithTitleBar):
	__children = {}
	class LogItem(ui.ExpandedImageBox):
		def Destroy(self):
			self.__children = {}
		def __init__(self, idx, name, val, level, rankIdx):
			ui.ExpandedImageBox.__init__(self)
			self.__LoadWindow(idx, name, val, level, rankIdx)

		def __LoadWindow(self, idx, name, val, level, rankIdx):
			self.Destroy()
			self.LoadImage(IMG_DIR+"log_item.tga")

			indexText = CreateWindow(ui.TextLine(), self, (20, 3), str(idx+1), "horizontal:center")
			self.__children["indexText"] = indexText

			nameText = CreateWindow(ui.TextLine(), self, (110, 3), name, "horizontal:center")
			self.__children["nameText"] = nameText

			levelText = CreateWindow(ui.TextLine(), self, (203, 3), str(level), "horizontal:center")
			self.__children["levelText"] =levelText

			valueComparedTest = FormatTimeEx(val) if rankIdx == 1 else NumberToDecimalString(val)
			valueText = CreateWindow(ui.TextLine(), self, (268, 3), valueComparedTest, "horizontal:center")
			self.__children["valueText"] = valueText

			if name == player.GetName():
				indexText.SetPackedFontColor(0xFFffcc00)
				nameText.SetPackedFontColor(0xFFffcc00)
				levelText.SetPackedFontColor(0xFFffcc00)
				valueText.SetPackedFontColor(0xFFffcc00)

	def Destroy(self):
		listBox = self.__children["listBox"] if self.__children.has_key("listBox") else None
		if listBox:
			listBox.RemoveAllItems()
		self.__children = {}
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()
		self.__LoadWindow()

	def __LoadWindow(self):
		self.SetSize(362,200)
		self.SetCenterPosition()
		self.AddFlag("float")
		self.AddFlag("movable")
		self.AddFlag("attach")
		# self.AddFlag("animate")  # Bu flag tanımlı değil, kaldırıldı

		logTitle = CreateWindow(ui.ImageBox(), self, (19, 39), IMG_DIR+"log_title.tga")
		self.__children["logTitle"] = logTitle

		self.__children["rankText"] = CreateWindow(ui.TextLine(), logTitle, (20, 3), localeInfo.DUNGEON_TRACK_RANK_INDEX, "horizontal:center")
		self.__children["nameText"] = CreateWindow(ui.TextLine(), logTitle, (110, 3), localeInfo.DUNGEON_TRACK_RANK_NAME, "horizontal:center")
		self.__children["levelText"] = CreateWindow(ui.TextLine(), logTitle, (203, 3), localeInfo.DUNGEON_TRACK_RANK_LEVEL, "horizontal:center")
		self.__children["valueText"] = CreateWindow(ui.TextLine(), logTitle, (268, 3), "", "horizontal:center")

		listBox = CreateWindow(ui.ListBoxEx(), self, (19,39+logTitle.GetHeight()+5), "", "", (308, 23*5))
		listBox.AddFlag("attach")
		listBox.SetItemSize(308, 21)
		listBox.SetItemStep(23)
		listBox.SetViewItemCount(5)
		self.__children["listBox"] = listBox

		self.__children["loadingImage"] = CreateWindow(ui.ExpandedImageBox(), listBox, ((listBox.GetWidth()/2)-16, (listBox.GetHeight()/2)-16), IMG_DIR+"load_.tga")
		self.__children["loadingImage"].Hide()
		self.__children["loadingImageRotation"] = 0

		scrollBar = CreateWindow(ui.ScrollBar(), self, (19+logTitle.GetWidth()+5,39+logTitle.GetHeight()))
		scrollBar.SetScrollBarSize(21 * 5 + 15)
		listBox.SetScrollBar(scrollBar)
		self.__children["scrollBar"] = scrollBar
	
		self.__children["rankData"] = {}

	def LoadServerData(self, mobIdx, rankIdx, cmd):
		rankData = self.__children["rankData"][mobIdx] if self.__children["rankData"].has_key(mobIdx) else {}
		data = rankData[rankIdx] if rankData.has_key(rankIdx) else {}
		rankList = []
		if cmd != "-":
			cmdSplit = cmd.split("#")
			for cmdInfo in cmdSplit:
				cmdInfoSplit = cmdInfo.split("|")
				if len(cmdInfoSplit) != 3:
					continue
				rankList.append({"name":cmdInfoSplit[0],"val":int(cmdInfoSplit[1]),"level":int(cmdInfoSplit[2])})

		data["leftTime"] = app.GetGlobalTimeStamp() + LOG_UPDATE_TIME
		data["data"] = rankList

		rankData[rankIdx] = data
		self.__children["rankData"][mobIdx] = rankData
		self.__LoadData(mobIdx, rankIdx)
		
	def __LoadData(self, mobIdx, rankIdx):
		self.__children["listBox"].RemoveAllItems()
		global dungeonData
		if not dungeonData.has_key(mobIdx):
			return
		__titleNames = [localeInfo.DUNGEON_TRACK_RANK_COMPLETED_TITLE, localeInfo.DUNGEON_TRACK_RANK_FASTEST_TITLE, localeInfo.DUNGEON_TRACK_RANK_DMG_TITLE]
		self.__children["valueText"].SetText(__titleNames[rankIdx])
		self.SetTitleName(dungeonData[mobIdx]["name"] + " - " + __titleNames[rankIdx])

		(self.__children["mobIdx"], self.__children["rankIdx"]) = (mobIdx, rankIdx)

		if self.__CheckRankNeedUpdate(mobIdx, rankIdx):
			self.__children["loadingImage"].Show()
			net.SendChatPacket("/dungeon_info rank {} {}".format(mobIdx, rankIdx))
		else:
			self.__children["loadingImage"].Hide()

			rankData = self.__GetData(mobIdx, rankIdx)
			if rankData != None:
				rankList = rankData["data"]
				for j in xrange(len(rankList)):
					self.__children["listBox"].AppendItem(self.LogItem(j, rankList[j]["name"], rankList[j]["val"], rankList[j]["level"], rankIdx))
		self.Show()
		self.SetTop()

	def __GetData(self, mobIdx, rankIdx):
		rankData = self.__children["rankData"]
		if rankData.has_key(mobIdx):
			if rankData[mobIdx].has_key(rankIdx):
				return rankData[mobIdx][rankIdx]
		return None
	def __GetLeftTime(self, mobIdx, rankIdx):
		rankData = self.__children["rankData"]
		if rankData.has_key(mobIdx):
			if rankData[mobIdx].has_key(rankIdx):
				return rankData[mobIdx][rankIdx]["leftTime"]
		return -1
	def __CheckRankNeedUpdate(self, mobIdx, rankIdx):
		return False if self.__GetLeftTime(mobIdx, rankIdx) > app.GetGlobalTimeStamp() else True
	def OnUpdate(self):
		loadingImage = self.__children["loadingImage"] if self.__children.has_key("loadingImage") else None
		if loadingImage:
			if loadingImage.IsShow():
				newRotation = self.__children["loadingImageRotation"] = self.__children["loadingImageRotation"] + 10
				self.__children["loadingImage"].SetRotation(newRotation)
	def Open(self, mobIdx, rankIdx):
		self.__LoadData(mobIdx, rankIdx)
	def Close(self):
		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True
