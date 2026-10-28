if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import snd
import dbg
import event
import wndMgr
import constInfo
import musicInfo
import localeInfo
import systemSetting
import playerSettingModule

import uiCommon
import uiToolTip
import uiScriptLocale

ENABLE_ENGNUM_DELETE_CODE = TRUE

M2_INIT_VALUE = -1

CHARACTER_SLOT_COUNT_MAX = 5

JOB_WARRIOR = 0
JOB_ASSASSIN = 1
JOB_SURA = 2
JOB_SHAMAN = 3

M2_CONST_ID = (
	(playerSettingModule.RACE_WARRIOR_M, playerSettingModule.RACE_WARRIOR_W),
	(playerSettingModule.RACE_ASSASSIN_M, playerSettingModule.RACE_ASSASSIN_W),
	(playerSettingModule.RACE_SURA_M, playerSettingModule.RACE_SURA_W),
	(playerSettingModule.RACE_SHAMAN_M, playerSettingModule.RACE_SHAMAN_W),
)

selected_race = None

class MyCharacters:
	class MyUnit:
		def __init__(self, const_id, name, level, race, playtime, guildname, form, hair, acce, aura, stat_str, stat_dex, stat_hth, stat_int, change_name):
			self.UnitDataDic = {
				"ID" : const_id,
				"NAME" : name,
				"LEVEL" : level,
				"RACE" : race,
				"PLAYTIME" : playtime,
				"GUILDNAME" : guildname,
				"FORM" : form,
				"HAIR" : hair,
				"ACCE" : acce,
				"AURA" : aura,
				"STR" : stat_str,
				"DEX" : stat_dex,
				"HTH" : stat_hth,
				"INT" : stat_int,
				"CHANGENAME" : change_name,
			}

		def __del__(self):
			self.UnitDataDic = None

		def GetUnitData(self):
			return self.UnitDataDic

	def __init__(self, stream):
		self.MainStream = stream
		self.PriorityData = []
		self.myUnitDic = {}
		self.HowManyChar = 0
		self.EmptySlot = []
		self.Race = [None, None, None, None, None]
		self.Job = [None, None, None, None, None]
		self.Guild_Name = [None, None, None, None, None]
		self.Play_Time = [None, None, None, None, None]
		self.Change_Name= [None, None, None, None, None]
		self.Stat_Point = { 0: None, 1: None, 2: None, 3: None}

	def __del__(self):
		self.MainStream = None 

		for i in xrange(self.HowManyChar):
			chr.DeleteInstance(i)

		self.PriorityData = None
		self.myUnitDic = None
		self.HowManyChar = None
		self.EmptySlot = None
		self.Race = None
		self.Job = None 
		self.Guild_Name = None
		self.Play_Time = None
		self.Change_Name = None
		self.Stat_Point = None

	def Show(self):
		self.LoadCharacterData()

	def LoadCharacterData(self):
		self.RefreshData()
		self.MainStream.All_ButtonInfoHide()

		for i in xrange(CHARACTER_SLOT_COUNT_MAX):
			pid = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID) 

			if not pid:
				self.EmptySlot.append(i)
				continue

			name = net.GetAccountCharacterSlotDataString(i, net.ACCOUNT_CHARACTER_SLOT_NAME)
			level = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_LEVEL)
			race = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_RACE)
			playtime = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
			guildname = net.GetAccountCharacterSlotDataString(i, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME)
			form = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_FORM)
			hair = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_HAIR)
			stat_str = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_STR)
			stat_dex = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_DEX)
			stat_hth = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_HTH)
			stat_int = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_INT)
			last_playtime = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME)
			change_name = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG)
			acce = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ACCE)
			aura = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_AURA)

			self.SetPriorityData(last_playtime, i)

			self.myUnitDic[i] = self.MyUnit(i, name, level, race, playtime, guildname, form, hair, acce, aura, stat_str, stat_dex, stat_hth, stat_int, change_name)

		try:
			self.PriorityData.sort(reverse = TRUE)
		except:
			pass

		for i in xrange(len(self.PriorityData)):
			time, index = self.PriorityData[i]
			DestDataDic = self.myUnitDic[index].GetUnitData()
			global selected_race
			selected_race = self.myUnitDic[index].GetUnitData()["RACE"]

			self.SetSortingData(i, DestDataDic["RACE"], DestDataDic["GUILDNAME"], DestDataDic["PLAYTIME"], DestDataDic["STR"], DestDataDic["DEX"], DestDataDic["HTH"], DestDataDic["INT"], DestDataDic["CHANGENAME"])

			self.MakeCharacter(i, DestDataDic["NAME"], DestDataDic["RACE"], DestDataDic["FORM"], DestDataDic["HAIR"], DestDataDic["ACCE"], DestDataDic["AURA"])

			self.MainStream.InitDataSet(i, DestDataDic["NAME"], DestDataDic["LEVEL"], DestDataDic["ID"])

		if self.HowManyChar:
			self.MainStream.SelectButton(0)

		return self.HowManyChar

	def SetPriorityData(self, last_playtime, index):
		self.PriorityData.append([last_playtime, index])

	def MakeCharacter(self, slot, name, race, form, hair, acce, aura):
		chr.CreateInstance(slot)
		chr.SelectInstance(slot)
		chr.SetVirtualID(slot)
		chr.SetNameString(name)

		chr.SetRace(race)
		chr.SetArmor(form)
		chr.SetHair(hair)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			chr.SetAcce(acce)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			chr.SetAura(aura)

		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)

		chr.SetRotation(0.0)
		chr.Hide()

	def SetSortingData(self, slot, race, guildname, playtime, pStr, pDex, pHth, pInt, changename):
		self.HowManyChar += 1
		self.Race[slot] = race
		self.Job[slot] = chr.RaceToJob(race)
		self.Guild_Name[slot] = guildname
		self.Play_Time[slot] = playtime
		self.Change_Name[slot] = changename
		self.Stat_Point[slot] = [pHth, pInt, pStr, pDex]

	def GetRace(self, slot):
		return self.Race[slot]

	def GetJob(self, slot):
		return self.Job[slot]

	def GetMyCharacterCount(self):
		return self.HowManyChar

	def GetEmptySlot(self):
		if not len(self.EmptySlot):
			return M2_INIT_VALUE

		return self.EmptySlot[0]

	def GetStatPoint(self, slot):
		return self.Stat_Point[slot]

	def GetGuildNamePlayTime(self, slot):
		return self.Guild_Name[slot], self.Play_Time[slot]

	def GetChangeName(self, slot):
		return self.Change_Name[slot]

	def SetChangeNameSuccess(self, slot):
		self.Change_Name[slot] = 0

	def RefreshData(self):
		self.HowManyChar = 0
		self.EmptySlot = []
		self.PriorityData = []
		self.Race = [None, None, None, None, None]
		self.Guild_Name = [None, None, None, None, None]
		self.Play_Time = [None, None, None, None, None]
		self.Change_Name= [None, None, None, None, None]
		self.Stat_Point = { 0: None, 1: None, 2: None, 3: None}

class SelectCharacterWindow(ui.Window):
	RACE_FACE_PATH = {
		playerSettingModule.RACE_WARRIOR_M : "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_0",
		playerSettingModule.RACE_ASSASSIN_W : "D:/ymir work/ui/intro/public_intro/face/face_assassin_w_0",
		playerSettingModule.RACE_SURA_M : "D:/ymir work/ui/intro/public_intro/face/face_sura_m_0",
		playerSettingModule.RACE_SHAMAN_W : "D:/ymir work/ui/intro/public_intro/face/face_shaman_w_0",
		playerSettingModule.RACE_WARRIOR_W : "D:/ymir work/ui/intro/public_intro/face/face_warrior_w_0",
		playerSettingModule.RACE_ASSASSIN_M : "D:/ymir work/ui/intro/public_intro/face/face_assassin_m_0",
		playerSettingModule.RACE_SURA_W : "D:/ymir work/ui/intro/public_intro/face/face_sura_w_0",
		playerSettingModule.RACE_SHAMAN_M : "D:/ymir work/ui/intro/public_intro/face/face_shaman_m_0",
	}

	class CharacterRenderer(ui.Window):
		def OnRender(self):
			global selected_race
			grp.ClearDepthBuffer()

			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			screenWidth = float(wndMgr.GetScreenWidth() / 2)
			screenHeight = float(wndMgr.GetScreenHeight())

			grp.SetViewport(0.26, 0.0, 0.5, 1.0)

			dbg.LogBox(selected_race)

			app.SetCenterPosition(0.0, 0.0, 0.0)
			app.SetCamera(1550.0, 15.0, 180.0, 95.0)

			grp.SetPerspective(15.0, screenWidth/screenHeight, 1000.0, 3000.0)
			(x, y) = app.GetCursorPosition()
			grp.SetCursorPosition(x, y)

			chr.Deform()
			chr.Render()

			grp.RestoreViewport()
			grp.PopState()
			grp.SetInterfaceRenderState()

	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, self)
		self.stream = stream

		self.SelectSlot = M2_INIT_VALUE
		self.SelectEmpire = FALSE
		self.ShowToolTip = FALSE
		self.select_race = M2_INIT_VALUE
		self.LEN_STATPOINT = 4
		self.statpoint = [0, 0, 0, 0]
		self.curGauge = [0.0, 0.0, 0.0, 0.0]
		self.Name_FontColor = grp.GenerateColor(229.0/255.0, 229.0/255.0, 229.0/255.0, 1.0)
		self.Level_FontColor = grp.GenerateColor(111.0/255.0, 111.0/255.0, 111.0/255.0, 1.0)
		self.Not_SelectMotion = FALSE
		self.MotionStart = FALSE
		self.MotionTime = 0.0
		self.RealSlot = []
		self.Disable = FALSE

		for i in xrange(len(M2_CONST_ID)):
			chr.DeleteInstance(i)

	def __del__(self):
		ui.Window.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, 0)

	def Open(self):
		if constInfo.SELECT_CHARACTER_ROTATION == TRUE:
			self.Rotation = 0

		dlgBoard = ui.ScriptWindow()
		self.dlgBoard = dlgBoard
		pythonScriptLoader = ui.PythonScriptLoader()
		pythonScriptLoader.LoadScriptFile( self.dlgBoard, "UIScript/selectcharacterwindow.py" ) 

		getChild = self.dlgBoard.GetChild

		self.backGround = getChild("BackGround")

		self.flag = getChild("EmpireFlag")
		self.flagDict = {
			net.EMPIRE_A: "d:/ymir work/ui/intro/empire/empireflag_a.sub",
			net.EMPIRE_B: "d:/ymir work/ui/intro/empire/empireflag_b.sub",
			net.EMPIRE_C: "d:/ymir work/ui/intro/empire/empireflag_c.sub",
		}

		self.PlayButton = getChild("PlayButton")
		self.EmptySlotButton = getChild("EmptySlotButton")
		self.DeleteButton = getChild("DeleteButton")
		self.ExitButton = getChild("ExitButton")

		self.FaceImage = [getChild("CharacterFace_{}".format(i)) for i in range(5)]
		self.CharacterButtonList = [getChild("CharacterSlot_{}".format(i)) for i in range(5)]

		for i in range(5):
			slot_name = "CharacterSlot_{}".format(i)
			slot = getChild(slot_name)
			slot.ShowToolTip = lambda arg=i: self.OverInToolTip(arg)
			slot.HideToolTip = lambda: self.OverOutToolTip()

		self.statValue = [getChild(stat + "_value") for stat in ["hth", "int", "str", "dex"]]
		self.GaugeList = [getChild(stat + "_gauge") for stat in ["hth", "int", "str", "dex"]]

		self.PlayButton.SetEvent(ui.__mem_func__(self.StartGameButton))
		self.EmptySlotButton.SetEvent(ui.__mem_func__(self.CreateCharacterButton))
		self.ExitButton.SetEvent(ui.__mem_func__(self.CloseButton))
		self.DeleteButton.SetEvent(ui.__mem_func__(self.InputPrivateCode))

		for i, button in enumerate(self.CharacterButtonList):
			button.SetEvent(ui.__mem_func__(self.SelectButton), i)

		for i in xrange(len(self.CharacterButtonList)):
			self.FaceImage[i].AddFlag("not_pick")

		self.mycharacters = MyCharacters(self)
		self.mycharacters.LoadCharacterData()

		if not self.mycharacters.GetMyCharacterCount():
			self.stream.SetCharacterSlot(self.mycharacters.GetEmptySlot())
			self.SelectEmpire = TRUE

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()

		self.dlgBoard.Show()
		self.Show()

		# Window'a focus ver (ENTER tuþu için gerekli)
		self.SetFocus()

		my_empire = net.GetEmpireID()
		self.SetEmpire(my_empire)

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] == -1:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2
				# Karakter seçim slotunu ayarla
				slot = constInfo.autoHuntAutoLoginDict.get("slot", 0)
				if slot < self.mycharacters.GetMyCharacterCount():
					self.SelectButton(slot)

		self.EmptySlotPosition()

		app.ShowCursor()

		if musicInfo.selectMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.selectMusic)

		self.chrRenderer = self.CharacterRenderer()
		self.chrRenderer.SetParent(self.backGround)
		self.chrRenderer.Show()

	def SelectButton(self, slot):
		if slot >= self.mycharacters.GetMyCharacterCount() or slot == self.SelectSlot:
			return

		if self.Not_SelectMotion or self.MotionTime != 0.0:
			self.CharacterButtonList[slot].SetUp()
			self.CharacterButtonList[slot].Over()
			return

		for button in self.CharacterButtonList:
			button.SetUp()

		self.SelectSlot = slot
		self.CharacterButtonList[self.SelectSlot].Down()
		self.stream.SetCharacterSlot(self.RealSlot[self.SelectSlot])

		self.ResetStat()

		self.select_race = self.mycharacters.GetRace(self.SelectSlot)
		for i in xrange(self.mycharacters.GetMyCharacterCount()):
			if slot == i:
				self.FaceImage[slot].LoadImage(self.RACE_FACE_PATH[self.select_race] + "1.sub")
			else:
				self.FaceImage[i].LoadImage(self.RACE_FACE_PATH[self.mycharacters.GetRace(i)] + "2.sub")

		chr.Hide()
		chr.SelectInstance(self.SelectSlot)
		chr.SetRotation(0.0)
		chr.Show()

	def Close(self):
		del self.mycharacters
		self.RACE_FACE_PATH = None

		self.SelectSlot = None
		self.SelectEmpire = None
		self.ShowToolTip = None
		self.LEN_STATPOINT = None
		self.statpoint = None
		self.curGauge = None
		self.Name_FontColor = None
		self.Level_FontColor = None
		self.Not_SelectMotion = None
		self.MotionStart = None
		self.MotionTime = None
		self.RealSlot = None

		self.select_race = None

		self.dlgBoard = None
		self.backGround = None
		self.flag = None
		self.flagDict = None
		self.PlayButton = None
		self.EmptySlotButton = None
		self.DeleteButton = None
		self.ExitButton = None
		self.FaceImage = None
		self.CharacterButtonList = None
		self.statValue = None
		self.GaugeList = None

		self.toolTip = None
		self.Disable = None

		if musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.selectMusic)

		self.Hide()
		self.KillFocus()
		app.HideCursor()
		event.Destroy()

	def SetEmpire(self, empire_id):
		if empire_id != net.EMPIRE_A:
			self.flag.LoadImage(self.flagDict[empire_id])
			self.flag.SetScale(0.55, 0.55)

	def CreateCharacterButton(self):
		slotNumber = self.mycharacters.GetEmptySlot()

		if slotNumber == M2_INIT_VALUE:
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.CREATE_FULL, 0, localeInfo.UI_OK)
			return 

		pid = self.GetCharacterSlotPID(slotNumber)

		if not pid:
			self.stream.SetCharacterSlot(slotNumber)

			if not self.mycharacters.GetMyCharacterCount():
				self.SelectEmpire = TRUE
			else:
				self.stream.SetCreateCharacterPhase()

	def EmptySlotPosition(self):
		self.EmptySlotButton.SetPosition(15, 50 + self.mycharacters.GetMyCharacterCount() * 60)

		if self.mycharacters.GetMyCharacterCount() >= 5:
			self.EmptySlotButton.Hide()
		else:
			self.EmptySlotButton.Show()

	def CloseButton(self):
		self.stream.SetLoginPhase()

	def StartGameButton(self):
		if not self.mycharacters.GetMyCharacterCount():
			return

		self.DisableWindow()

		IsChangeName = self.mycharacters.GetChangeName(self.SelectSlot)
		if IsChangeName:
			self.OpenChangeNameDialog()
			return

		chrSlot = self.stream.GetCharacterSlot()
		if musicInfo.selectMusic != "":
			snd.FadeLimitOutMusic("BGM/" + musicInfo.selectMusic, systemSetting.GetMusicVolume()*0.05)

		net.DirectEnter(chrSlot)
		playTime = net.GetAccountCharacterSlotDataInteger(chrSlot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
		player.SetPlayTime(playTime)
		import chat
		chat.Clear()
		self.Hide()

	def OnUpdate(self):
		if constInfo.SELECT_CHARACTER_ROTATION == TRUE:
			self.Rotation = (self.Rotation - 0.2) % 360.0
			chr.SetRotation(self.Rotation)

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] > 0 and constInfo.autoHuntAutoLoginDict["leftTime"] < app.GetGlobalTimeStamp():
				constInfo.autoHuntAutoLoginDict["leftTime"] = -2
				# Karakter seçim slotunu ayarla
				slot = constInfo.autoHuntAutoLoginDict.get("slot", 0)
				if slot < self.mycharacters.GetMyCharacterCount():
					self.SelectButton(slot)
					# Kýsa bir gecikme sonrasý oyuna gir
					if self.MotionTime == 0.0:
						self.StartGameButton()

		chr.Update()

		self.ToolTipProgress()

		if self.SelectEmpire:
			self.SelectEmpire = FALSE
			self.stream.SetReselectEmpirePhase()
			self.Hide()

		if self.MotionStart and app.GetTime() - self.MotionTime >= 0.1:
			self.MotionStart = FALSE
			chrSlot = self.stream.GetCharacterSlot()

			if musicInfo.selectMusic != "":
				snd.FadeLimitOutMusic("BGM/" + musicInfo.selectMusic, systemSetting.GetMusicVolume()*0.05)

			net.DirectEnter(chrSlot)
			playTime = net.GetAccountCharacterSlotDataInteger(chrSlot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)

			player.SetPlayTime(playTime)
			import chat
			chat.Clear()

		# LEN_STATPOINT, curGauge ve GaugeList None kontrolü
		if self.LEN_STATPOINT is not None and self.curGauge is not None and self.GaugeList is not None:
			for i in xrange(self.LEN_STATPOINT):
				# curGauge[i] deðerini integer'a çevir (TypeError önleme)
				if i < len(self.curGauge) and i < len(self.GaugeList):
					gaugeValue = self.curGauge[i]
					if gaugeValue is None:
						gaugeValue = 0
					else:
						try:
							gaugeValue = int(float(gaugeValue))
						except:
							gaugeValue = 0
					self.GaugeList[i].SetPercentage(gaugeValue, 1.0)

	def GetCharacterSlotPID(self, slotIndex):
		return net.GetAccountCharacterSlotDataInteger(slotIndex, net.ACCOUNT_CHARACTER_SLOT_ID)

	def All_ButtonInfoHide(self):
		for i in xrange(CHARACTER_SLOT_COUNT_MAX):
			self.CharacterButtonList[i].Hide()
			self.FaceImage[i].Hide()

	def InitDataSet(self, slot, name, level, real_slot):
		width = self.CharacterButtonList[slot].GetWidth()
		height = self.CharacterButtonList[slot].GetHeight()

		self.CharacterButtonList[slot].AppendTextLine(name, localeInfo.UI_DEF_FONT_LARGE, self.Name_FontColor, "left", width - 160, height/4 + 7)
		self.CharacterButtonList[slot].AppendTextLine("Lv. " + str(level), localeInfo.UI_DEF_FONT, self.Level_FontColor, "left", width - 159, height/4 + 23)
		self.CharacterButtonList[slot].Show()

		self.FaceImage[slot].LoadImage(self.RACE_FACE_PATH[self.mycharacters.GetRace(slot)] + "2.sub")
		self.FaceImage[slot].Show()

		self.RealSlot.append(real_slot)

	def InputPrivateCode(self):
		if not self.mycharacters.GetMyCharacterCount():
			return

		import uiCommon
		privateInputBoard = uiCommon.InputDialogWithDescription()
		privateInputBoard.SetTitle(localeInfo.INPUT_PRIVATE_CODE_DIALOG_TITLE)
		privateInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrivateCode))
		privateInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrivateCode))

		if ENABLE_ENGNUM_DELETE_CODE:
			pass
		else:
			privateInputBoard.SetNumberMode()

		privateInputBoard.SetSecretMode()
		privateInputBoard.SetMaxLength(7)

		privateInputBoard.SetBoardWidth(250)
		privateInputBoard.SetDescription(localeInfo.INPUT_PRIVATE_CODE_DIALOG_DESCRIPTION)
		privateInputBoard.Open()
		self.privateInputBoard = privateInputBoard
		self.DisableWindow()

		if not self.Not_SelectMotion:
			self.Not_SelectMotion = TRUE
			chr.PushOnceMotion(chr.MOTION_INTRO_NOT_SELECTED, 0.1)

	def AcceptInputPrivateCode(self):
		privateCode = self.privateInputBoard.GetText()
		if not privateCode:
			return

		pid = net.GetAccountCharacterSlotDataInteger(self.RealSlot[self.SelectSlot], net.ACCOUNT_CHARACTER_SLOT_ID)

		if not pid:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(self.RealSlot[self.SelectSlot], privateCode)
		self.PopupMessage(localeInfo.SELECT_DELEING)

		self.CancelInputPrivateCode()
		return TRUE

	def CancelInputPrivateCode(self):
		self.privateInputBoard = None
		self.Not_SelectMotion = FALSE
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		self.EnableWindow()
		# Focus'u geri ver (ENTER tuþu için gerekli)
		self.SetFocus()
		return TRUE

	def OnDeleteSuccess(self, slot):
		self.PopupMessage(localeInfo.SELECT_DELETED)
		for i in xrange(len(self.RealSlot)):
			chr.DeleteInstance(i)

		self.RealSlot = []
		self.SelectSlot = M2_INIT_VALUE

		for button in self.CharacterButtonList:
			button.AppendTextLineAllClear()

		if not self.mycharacters.LoadCharacterData():
			self.stream.popupWindow.Close()
			self.stream.SetCharacterSlot(self.mycharacters.GetEmptySlot())
			self.SelectEmpire = TRUE

		self.EmptySlotPosition()

	def OnDeleteFailure(self):
		self.PopupMessage(localeInfo.SELECT_CAN_NOT_DELETE)

	def EmptyFunc(self):
		pass

	def PopupMessage(self, msg, func=0):
		if not func:
			func=self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def RefreshStat(self):
		if self.LEN_STATPOINT is None or self.statpoint is None or self.statValue is None:
			return
			
		if len(self.statpoint) < 4:
			return
			
		statSummary = 90.0
		self.curGauge = [
			float(self.statpoint[0])/statSummary,
			float(self.statpoint[1])/statSummary,
			float(self.statpoint[2])/statSummary,
			float(self.statpoint[3])/statSummary,
		]

		for i in xrange(self.LEN_STATPOINT):
			if i < len(self.statValue) and i < len(self.statpoint):
				self.statValue[i].SetText(str(self.statpoint[i]))

	def ResetStat(self):
		if self.LEN_STATPOINT is None or self.statpoint is None:
			return
			
		myStatPoint = self.mycharacters.GetStatPoint(self.SelectSlot)

		if not myStatPoint:
			return

		for i in xrange(self.LEN_STATPOINT):
			if i < len(self.statpoint) and i < len(myStatPoint):
				self.statpoint[i] = myStatPoint[i]

		self.RefreshStat()

	def OverInToolTip(self, slot):
		GuildName = localeInfo.GUILD_NAME
		myGuildName, myPlayTime = self.mycharacters.GetGuildNamePlayTime(slot)
		pos_x, pos_y = self.CharacterButtonList[slot].GetGlobalPosition()

		if not myGuildName:
			myGuildName = localeInfo.SELECT_NOT_JOIN_GUILD

		guild_name = GuildName + ": " + myGuildName
		play_time = uiScriptLocale.SELECT_PLAYTIME + ":"
		day = myPlayTime / (60 * 24)
		if day: 
			play_time = play_time + " " + str(day) + localeInfo.DAY
		hour = (myPlayTime - (day * 60 * 24))/60
		if hour:
			play_time = play_time + " " + str(hour) + localeInfo.HOUR
		min = myPlayTime - (hour * 60) - (day * 60 * 24)

		play_time = play_time + " " + str(min) + localeInfo.MINUTE

		textlen = max(len(guild_name), len(play_time))
		tooltip_width = 6 * textlen + 22

		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(tooltip_width)

		self.toolTip.SetToolTipPosition(pos_x + 230 + tooltip_width/2, pos_y + 45)
		self.toolTip.AppendTextLine(guild_name, 0xffe4cb1b, FALSE)
		self.toolTip.AppendTextLine(play_time, 0xffe4cb1b, FALSE)

		self.toolTip.Show()

	def OverOutToolTip(self):
		self.toolTip.Hide()
		self.ShowToolTip = FALSE

	def ToolTipProgress(self):
		if self.ShowToolTip:
			pos_x, pos_y = wndMgr.GetMousePosition()
			self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)

	def SameLoginDisconnect(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, self.CloseButton, localeInfo.UI_OK)

	def OnKeyDown(self, key):
		if self.MotionTime != 0.0:
			return

		if 1 == key: #ESC
			self.CloseButton()
		elif 2 == key: #1
			self.SelectButton(0)
		elif 3 == key:
			self.SelectButton(1)
		elif 4 == key:
			self.SelectButton(2)
		elif 5 == key:
			self.SelectButton(3)
		elif 6 == key:
			self.SelectButton(4)
		elif 28 == key: #ENTER
				self.StartGameButton()
		elif 200 == key or 208 == key:
			self.KeyInputUpDown(key)
		else:
			return True

		return True

	def KeyInputUpDown(self, key):
		idx = self.SelectSlot
		maxValue = self.mycharacters.GetMyCharacterCount()
		if 200 == key:
			idx = idx - 1
			if idx < 0:
				idx = maxValue - 1

		elif 208 == key:
			idx = idx + 1
			if idx >= maxValue:
				idx = 0
		else:
			self.SelectButton(0)

		self.SelectButton(idx)

	def OnPressExitKey(self):
		self.CloseButton()
		return True

	def DisableWindow(self):
		self.PlayButton.Disable()
		self.EmptySlotButton.Disable()
		self.ExitButton.Disable()
		self.DeleteButton.Disable()
		self.toolTip.Hide()
		self.ShowToolTip = FALSE
		self.Disable = TRUE
		for button in self.CharacterButtonList:
			button.Disable()

		self.CharacterButtonList[self.SelectSlot].Down()

	def EnableWindow(self):
		self.PlayButton.Enable()
		self.EmptySlotButton.Enable()
		self.ExitButton.Enable()
		self.DeleteButton.Enable()
		self.Disable = FALSE
		for button in self.CharacterButtonList:
			button.Enable()

		self.CharacterButtonList[self.SelectSlot].Down()

	def OpenChangeNameDialog(self):
		import uiCommon
		nameInputBoard = uiCommon.InputDialogWithDescription()
		nameInputBoard.SetTitle(localeInfo.SELECT_CHANGE_NAME_TITLE)
		nameInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputName))
		nameInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputName))
		nameInputBoard.SetMaxLength(chr.PLAYER_NAME_MAX_LEN)
		nameInputBoard.SetBoardWidth(200)
		nameInputBoard.SetDescription(localeInfo.SELECT_INPUT_CHANGING_NAME)
		nameInputBoard.Open()
		nameInputBoard.slot = self.RealSlot[self.SelectSlot]
		self.nameInputBoard = nameInputBoard

	def AcceptInputName(self):
		changeName = self.nameInputBoard.GetText()
		if not changeName:
			return

		net.SendChangeNamePacket(self.nameInputBoard.slot, changeName)
		return self.CancelInputName()

	def CancelInputName(self):
		self.nameInputBoard.Close()
		self.nameInputBoard = None
		self.EnableWindow()
		return TRUE

	def OnCreateFailure(self, type):
		if 0 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_NAME)
		elif 1 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_ALREADY_EXIST_NAME)
		elif 100 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_INDEX)

	def OnChangeName(self, slot, name):
		for i in xrange(len(self.RealSlot)):
			if self.RealSlot[i] == slot:
				self.ChangeNameButton(i, name)
				self.SelectButton(i)
				self.PopupMessage(localeInfo.SELECT_CHANGED_NAME)
				break

	def ChangeNameButton(self, slot, name):
		self.CharacterButtonList[slot].SetAppendTextChangeText(0, name)
		self.mycharacters.SetChangeNameSuccess(slot)
