if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import snd
import event
import wndMgr
import constInfo
import musicInfo
import localeInfo
import systemSetting

import uiCommon
import uiToolTip
import uiScriptLocale

if app.ENABLE_COUNTRY_FLAG:
	import uiCountryFlag

# Baerim introSelect (Cython) rebuilt from its runtime dump: Config and the method names are theirs.
# Character data, delete and change name work like our previous introselect.py.
class Config:
	FACE_PATH = "d:/ymir work/ui/intro/public_intro/face/"
	SELECT_PATH = "d:/ymir work/ui/intro/select/"
	PLAYER_SLOT_COUNT = 4
	# same camera as introCreateBaerim (checked against Baerim there)
	RENDER_CENTER_Z = -20.0
	DESC_LINE_HEIGHT = 17
	DESC_TEXT_COLOR = 0xffcabdab
	DESC_TEXT_WIDTH = 165
	DESC_VISIBLE_LINE_COUNT = 13

ACCOUNT_SLOT_COUNT = 5
STAT_GAUGE_MAX = 90.0

JOB_FACE_NAMES = ("warrior", "assassin", "sura", "shaman")
JOB_NAME_WINDOWS = ("name_warrior", "name_assassin", "name_sura", "name_shaman")
DESCRIPTION_LOCALE_KEYS = ("JOBDESC_WARRIOR", "JOBDESC_ASSASSIN", "JOBDESC_SURA", "JOBDESC_SHAMAN")

EMPIRE_NAME_COLOR = {
	net.EMPIRE_A : (0.7450, 0.0, 0.0),
	net.EMPIRE_B : (0.8666, 0.6156, 0.1843),
	net.EMPIRE_C : (0.2235, 0.2549, 0.7490),
}

EMPIRE_DATA = {
	net.EMPIRE_A : ("empireflag_a.sub", "empire_sinsu.sub"),
	net.EMPIRE_B : ("empireflag_b.sub", "empire_chunjo.sub"),
	net.EMPIRE_C : ("empireflag_c.sub", "empire_jinno.sub"),
}

def GetSlotData(realSlot, key):
	return net.GetAccountCharacterSlotDataInteger(realSlot, key)

class CountryFlagBox(ui.Button):
	IMAGE_PATH = "d:/ymir work/ui/game/common/button_xlarge_%s.png"
	ARROW_IMAGE = "d:/ymir work/ui/game/advanced_game_options/arrow_down.png"

	def __init__(self):
		ui.Button.__init__(self)
		self.SetUpVisual(self.IMAGE_PATH % "norm")
		self.SetOverVisual(self.IMAGE_PATH % "hover")
		self.SetDownVisual(self.IMAGE_PATH % "down")

		self.flag = ui.ImageBox()
		self.flag.SetParent(self)
		self.flag.AddFlag("not_pick")

		self.name = ui.TextLine()
		self.name.SetParent(self)
		self.name.AddFlag("not_pick")
		self.name.SetPosition(46, self.GetHeight() / 2 - 7)
		self.name.SetPackedFontColor(uiCountryFlag.TEXT_COLOR)
		self.name.Show()

		self.arrow = ui.ImageBox()
		self.arrow.SetParent(self)
		self.arrow.AddFlag("not_pick")
		self.arrow.LoadImage(self.ARROW_IMAGE)
		self.arrow.SetPosition(self.GetWidth() - self.arrow.GetWidth() - 9, (self.GetHeight() - self.arrow.GetHeight()) / 2)
		self.arrow.Show()

	def __del__(self):
		ui.Button.__del__(self)

	def SetCountry(self, code):
		if code:
			self.flag.LoadImage(uiCountryFlag.FLAG_PATH % code)
			self.flag.SetPosition(7, (self.GetHeight() - self.flag.GetHeight()) / 2)
			self.flag.Show()
			self.name.SetText(uiCountryFlag.GetCountryName(code))
		else:
			self.flag.Hide()
			self.name.SetText("-")

class SelectCharacterWindow(ui.Window):
	class CharacterRenderer(ui.Window):
		def OnRender(self):
			# full screen viewport: an offset one still draws from the left edge on our client
			sw = float(wndMgr.GetScreenWidth())
			sh = float(wndMgr.GetScreenHeight())

			grp.ClearDepthBuffer()
			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			grp.SetViewport(0.0, 0.0, 1.0, 1.0)

			app.SetCenterPosition(0.0, 0.0, Config.RENDER_CENTER_Z)
			app.SetCamera(1550.0, 15.0, 180.0, 95.0)
			grp.SetPerspective(10.0, sw / sh, 1000.0, 3000.0)

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

		self.dlgBoard = None
		self.chrRenderer = None
		self.toolTip = None
		self.privateInputBoard = None
		self.nameInputBoard = None

		self.filledSlots = []		# real slots with a character, most recently played first
		self.madeSlots = []
		self.page = 0
		self.displaySlot = -1		# index into filledSlots
		self.reselectEmpire = FALSE
		self.motionStart = FALSE
		self.descLines = []
		self.descLineWindows = []
		self.descStartLine = 0
		self.hoverIndex = -1
		self.countryFlagBoard = None
		self.countryFlagBox = None
		self.countryFlagDropdown = None

		for i in xrange(ACCOUNT_SLOT_COUNT):
			chr.DeleteInstance(i)

	def __del__(self):
		ui.Window.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, 0)

	def Open(self):
		constInfo.MESSENGER_BUTTON_CLOSED = False	# a new login / character brings the [MESSENGER] envelope back

		if not self.__LoadBoardDialog():
			return

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()

		self.SetEmpire(net.GetEmpireID())
		self.__CreateDescLineWindows()
		self.Refresh()

		self.dlgBoard.Show()
		self.Show()
		self.SetFocus()
		app.ShowCursor()

		if musicInfo.selectMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.selectMusic)

	def Close(self):
		if musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.selectMusic)

		for i in self.madeSlots:
			chr.DeleteInstance(i)
		self.madeSlots = []

		for line in self.descLineWindows:
			line.Hide()
		self.descLineWindows = []

		if self.toolTip:
			self.toolTip.Hide()
		self.toolTip = None

		if self.countryFlagDropdown:
			self.countryFlagDropdown.Destroy()
		self.countryFlagDropdown = None
		self.countryFlagBox = None

		if self.dlgBoard:
			self.dlgBoard.ClearDictionary()
			self.dlgBoard.Hide()
		self.dlgBoard = None
		self.chrRenderer = None

		self.Hide()
		self.KillFocus()
		app.HideCursor()
		event.Destroy()

	def __LoadBoardDialog(self):
		try:
			dlgBoard = ui.ScriptWindow()
			ui.PythonScriptLoader().LoadScriptFile(dlgBoard, "UIScript/selectcharacterwindow_baerim.py")
		except:
			import exception
			exception.Abort("SelectCharacterWindow.__LoadBoardDialog.LoadObject")
			return FALSE

		self.dlgBoard = dlgBoard
		Get = dlgBoard.GetChild

		try:
			self.background = Get("Background")
			self.renderWindow = Get("character_render_window")
			self.jobNameImages = [Get(name) for name in JOB_NAME_WINDOWS]
			self.kingdomImage = Get("kingdom_image")
			self.kingdomText = Get("kingdom")
			self.slotButtons = [Get("slot_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.emptySlots = [Get("empty_slot_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.raceImages = [Get("race_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.nameTexts = [Get("text_playername_new%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.levelTexts = [Get("text_levelvalue_new%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.createButton = Get("btn_create")
			self.deleteButton = Get("btn_delete")
			self.startButton = Get("btn_login")
			self.exitButton = Get("btn_exit")
			self.discFace = Get("DiscFace")
			self.raceNameText = Get("raceName_Text")
			self.textBoard = Get("text_board")
			self.prevButton = Get("prev_button")
			self.nextButton = Get("next_button")
			self.gauges = [Get(name) for name in ("gauge_hp", "gauge_int", "gauge_str", "gauge_dex")]
			self.statValues = [Get(name) for name in ("text_value_hp", "text_value_int", "text_value_str", "text_value_dex")]
			self.pageArrows = [Get("arrow_0"), Get("arrow_1")]
		except:
			import exception
			exception.Abort("SelectCharacterWindow.__LoadBoardDialog.BindObject")
			return FALSE

		for i in xrange(Config.PLAYER_SLOT_COUNT):
			self.slotButtons[i].SetEvent(ui.__mem_func__(self.ChooseSlot), i)
			self.slotButtons[i].ShowToolTip = lambda arg = i: self.__ShowSlotToolTip(arg)
			self.slotButtons[i].HideToolTip = lambda: self.__HideSlotToolTip()

		self.createButton.SetEvent(ui.__mem_func__(self.CreateCharacter))
		self.deleteButton.SetEvent(ui.__mem_func__(self.InputPrivateCode))
		self.startButton.SetEvent(ui.__mem_func__(self.StartGame))
		self.exitButton.SetEvent(ui.__mem_func__(self.ExitSelect))
		self.prevButton.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
		self.nextButton.SetEvent(ui.__mem_func__(self.NextDescriptionPage))
		# leftovers of the old select layout in Baerim's uiscript (__HideSelectArrows)
		for arrow in self.pageArrows:
			arrow.Hide()

		self.chrRenderer = self.CharacterRenderer()
		self.chrRenderer.SetParent(self.background)
		self.chrRenderer.AddFlag("not_pick")
		self.chrRenderer.Show()

		self.__CreateCountryFlagControls()
		return TRUE

	# country flag

	def __CreateCountryFlagControls(self):
		self.countryFlagBoard = self.dlgBoard.GetChild2("country_flag_board")
		self.countryFlagBox = None
		self.countryFlagDropdown = None
		if not self.countryFlagBoard:
			return

		if not app.ENABLE_COUNTRY_FLAG:
			self.countryFlagBoard.Hide()
			return

		box = CountryFlagBox()
		box.SetParent(self.countryFlagBoard)
		box.SetPosition((self.countryFlagBoard.GetWidth() - box.GetWidth()) / 2, (self.countryFlagBoard.GetHeight() - box.GetHeight()) / 2)
		box.SetEvent(ui.__mem_func__(self.ToggleCountryFlagDropdown))
		box.Show()
		self.countryFlagBox = box

		self.countryFlagDropdown = uiCountryFlag.CountryFlagDropdown()
		self.countryFlagDropdown.SetEvent(ui.__mem_func__(self.OnSelectCountryFlag))

	def __GetSelectedCountryFlag(self):
		if self.displaySlot < 0:
			return ""
		return net.GetAccountCharacterSlotDataString(self.filledSlots[self.displaySlot], net.ACCOUNT_CHARACTER_SLOT_COUNTRY_FLAG)

	def __RefreshCountryFlagControls(self):
		if not self.countryFlagBox:
			return

		if self.displaySlot < 0:
			self.countryFlagBoard.Hide()
			self.CloseCountryFlagDropdown()
			return

		self.countryFlagBoard.Show()
		self.countryFlagBox.SetCountry(self.__GetSelectedCountryFlag())

	def ToggleCountryFlagDropdown(self):
		if not self.countryFlagDropdown or self.displaySlot < 0:
			return

		if self.countryFlagDropdown.IsOpened():
			self.CloseCountryFlagDropdown()
			return

		# open below the box, or above it when the screen ends first (Baerim _Reposition)
		(x, y) = self.countryFlagBox.GetGlobalPosition()
		height = self.countryFlagDropdown.GetHeight()
		belowY = y + self.countryFlagBox.GetHeight() + 2
		dropY = belowY if belowY + height <= wndMgr.GetScreenHeight() else y - height - 2
		self.countryFlagDropdown.SetCurrent(self.__GetSelectedCountryFlag())
		self.countryFlagDropdown.Open(x, dropY)

	def CloseCountryFlagDropdown(self):
		if self.countryFlagDropdown and self.countryFlagDropdown.IsOpened():
			self.countryFlagDropdown.Close()

	def OnSelectCountryFlag(self, code):
		if self.displaySlot < 0:
			return

		net.SendChangeCountryFlagPacket(self.filledSlots[self.displaySlot], code)
		self.__RefreshCountryFlagControls()

	# slots

	def __RefreshSlotOrder(self):
		filled = []
		for realSlot in xrange(ACCOUNT_SLOT_COUNT):
			if GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_ID):
				filled.append((GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME), realSlot))
		filled.sort(reverse = TRUE)
		self.filledSlots = [realSlot for (lastPlayTime, realSlot) in filled]

	def __GetFirstEmptySlot(self):
		for realSlot in xrange(ACCOUNT_SLOT_COUNT):
			if not GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_ID):
				return realSlot
		return -1

	def __GetPageCount(self):
		# Baerim shows no arrows; we need them only for a 5th character (create always takes the first free slot)
		return max(1, (len(self.filledSlots) + Config.PLAYER_SLOT_COUNT - 1) / Config.PLAYER_SLOT_COUNT)

	def Refresh(self):
		self.__RefreshSlotOrder()
		self.page = 0
		self.displaySlot = -1
		self.__RefreshPage()
		if self.filledSlots:
			self.ChooseSlot(0)
		else:
			self.__ClearSelectedInfo()

	def __RefreshPage(self):
		start = self.page * Config.PLAYER_SLOT_COUNT
		for i in xrange(Config.PLAYER_SLOT_COUNT):
			index = start + i
			if index < len(self.filledSlots):
				realSlot = self.filledSlots[index]
				self.slotButtons[i].Show()
				self.emptySlots[i].Hide()
				self.nameTexts[i].SetText(net.GetAccountCharacterSlotDataString(realSlot, net.ACCOUNT_CHARACTER_SLOT_NAME))
				self.levelTexts[i].SetText("%s %d" % (uiScriptLocale.SELECT_LEVEL, GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_LEVEL)))
			else:
				self.slotButtons[i].Hide()
				if index < ACCOUNT_SLOT_COUNT:
					self.emptySlots[i].Show()
				else:
					self.emptySlots[i].Hide()

		self.UpdateRaceImages()

	def UpdateRaceImages(self):
		start = self.page * Config.PLAYER_SLOT_COUNT
		for i in xrange(Config.PLAYER_SLOT_COUNT):
			index = start + i
			if index >= len(self.filledSlots):
				continue

			race = GetSlotData(self.filledSlots[index], net.ACCOUNT_CHARACTER_SLOT_RACE)
			image = "1.sub" if index == self.displaySlot else "2.sub"
			self.raceImages[i].LoadImage("%sface_%s_%s_0%s" % (Config.FACE_PATH, JOB_FACE_NAMES[chr.RaceToJob(race)], self.__GetGenderKey(race), image))

			if index == self.displaySlot:
				self.slotButtons[i].Down()
			else:
				self.slotButtons[i].SetUp()

	def __GetGenderKey(self, race):
		return "w" if chr.RaceToSex(race) == 0 else "m"

	def DecreaseSlotIndex(self):
		if self.page > 0:
			self.page -= 1
			self.__RefreshPage()

	def IncreaseSlotIndex(self):
		if self.page + 1 < self.__GetPageCount():
			self.page += 1
			self.__RefreshPage()

	def ChooseSlot(self, slot):
		self.SelectSlot(self.page * Config.PLAYER_SLOT_COUNT + slot)

	def SelectSlot(self, index):
		if index < 0 or index >= len(self.filledSlots) or self.motionStart:
			self.UpdateRaceImages()
			return

		self.displaySlot = index
		realSlot = self.filledSlots[index]
		self.stream.SetCharacterSlot(realSlot)

		race = GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_RACE)
		job = chr.RaceToJob(race)

		self.UpdateRaceImages()
		self.UpdateSelectedRaceInfo(race)
		self.UpdateClassName(job)
		self.__SetJobDescription(job)

		stats = (
			GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_HTH),
			GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_INT),
			GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_STR),
			GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_DEX),
		)
		for i in xrange(len(stats)):
			self.statValues[i].SetText(str(stats[i]))
			self.gauges[i].SetPercentage(min(stats[i], STAT_GAUGE_MAX), STAT_GAUGE_MAX)

		self.__ShowCharacter(realSlot)
		self.__RefreshCountryFlagControls()

	def __ShowCharacter(self, realSlot):
		if self.madeSlots:
			chr.Hide()

		if realSlot not in self.madeSlots:
			self.MakeCharacter(realSlot)

		chr.SelectInstance(realSlot)
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.Show()

	def MakeCharacter(self, realSlot):
		self.madeSlots.append(realSlot)

		chr.CreateInstance(realSlot)
		chr.SelectInstance(realSlot)
		chr.SetVirtualID(realSlot)
		chr.SetNameString(net.GetAccountCharacterSlotDataString(realSlot, net.ACCOUNT_CHARACTER_SLOT_NAME))

		chr.SetRace(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_RACE))
		chr.SetArmor(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_FORM))
		chr.SetHair(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_HAIR))

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			chr.SetAcce(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_ACCE))

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			chr.SetAura(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_AURA))

		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.SetRotation(0.0)
		chr.Hide()

	def UpdateSelectedRaceInfo(self, race):
		job = chr.RaceToJob(race)
		jobNames = (localeInfo.JOB_WARRIOR, localeInfo.JOB_ASSASSIN, localeInfo.JOB_SURA, localeInfo.JOB_SHAMAN)
		self.discFace.LoadImage("icon/face/%s_%s.png" % (JOB_FACE_NAMES[job], self.__GetGenderKey(race)))
		self.raceNameText.SetText(jobNames[job])
		self.discFace.Show()

	def UpdateClassName(self, job):
		for i in xrange(len(self.jobNameImages)):
			if i == job:
				self.jobNameImages[i].Show()
			else:
				self.jobNameImages[i].Hide()

	def __ClearSelectedInfo(self):
		self.__RefreshCountryFlagControls()
		self.UpdateClassName(-1)
		self.discFace.Hide()
		self.raceNameText.SetText("")
		self.descLines = []
		self.__RefreshDescText()
		self.prevButton.Hide()
		self.nextButton.Hide()
		for i in xrange(len(self.gauges)):
			self.statValues[i].SetText("0")
			self.gauges[i].SetPercentage(0, STAT_GAUGE_MAX)

	# job description (same as introCreateBaerim)

	def __WrapJobDescText(self, descText, maxWidth):
		measure = ui.TextLine()
		lines = []
		line = ""
		for word in descText.split():
			testLine = word if not line else line + " " + word
			measure.SetText(testLine)
			if measure.GetTextSize()[0] <= maxWidth or not line:
				line = testLine
			else:
				lines.append(line)
				line = word
		if line:
			lines.append(line)
		return lines

	def __CreateDescLineWindows(self):
		x = (self.textBoard.GetWidth() - Config.DESC_TEXT_WIDTH) / 2
		for i in xrange(Config.DESC_VISIBLE_LINE_COUNT):
			textLine = ui.TextLine()
			textLine.SetParent(self.textBoard)
			textLine.SetPosition(x, 8 + i * Config.DESC_LINE_HEIGHT)
			textLine.SetPackedFontColor(Config.DESC_TEXT_COLOR)
			textLine.Show()
			self.descLineWindows.append(textLine)

	def __SetJobDescription(self, jobIndex):
		self.descLines = self.__WrapJobDescText(getattr(localeInfo, DESCRIPTION_LOCALE_KEYS[jobIndex], ""), Config.DESC_TEXT_WIDTH)
		self.descStartLine = 0
		self.__RefreshDescText()

		if len(self.descLines) > Config.DESC_VISIBLE_LINE_COUNT:
			self.prevButton.Show()
			self.nextButton.Show()
		else:
			self.prevButton.Hide()
			self.nextButton.Hide()

	def __RefreshDescText(self):
		for i in xrange(len(self.descLineWindows)):
			lineIndex = self.descStartLine + i
			if lineIndex < len(self.descLines):
				self.descLineWindows[i].SetText(self.descLines[lineIndex])
			else:
				self.descLineWindows[i].SetText("")

	def PrevDescriptionPage(self):
		self.descStartLine = max(0, self.descStartLine - Config.DESC_VISIBLE_LINE_COUNT)
		self.__RefreshDescText()

	def NextDescriptionPage(self):
		if self.descStartLine + Config.DESC_VISIBLE_LINE_COUNT >= len(self.descLines):
			return

		self.descStartLine += Config.DESC_VISIBLE_LINE_COUNT
		self.__RefreshDescText()

	# empire

	def SetEmpire(self, id):
		if not EMPIRE_DATA.has_key(id):
			return

		(flag, bg) = EMPIRE_DATA[id]
		self.kingdomImage.LoadImage(Config.SELECT_PATH + "empire/" + flag)
		self.kingdomImage.SetScale(0.45, 0.45)
		self.kingdomText.SetText({ net.EMPIRE_A : uiScriptLocale.EMPIRE_A, net.EMPIRE_B : uiScriptLocale.EMPIRE_B, net.EMPIRE_C : uiScriptLocale.EMPIRE_C }[id])
		(r, g, b) = EMPIRE_NAME_COLOR[id]
		self.kingdomText.SetFontColor(r, g, b)
		self.background.LoadImage(Config.SELECT_PATH + "empire/background/" + bg)
		self.background.SetScale(float(wndMgr.GetScreenWidth()) / 1024.0, float(wndMgr.GetScreenHeight()) / 768.0)

	# slot tooltip

	def __FormatPlayTime(self, playTime):
		text = uiScriptLocale.SELECT_PLAYTIME + " :"
		day = playTime / (60 * 24)
		if day:
			text += " %d%s" % (day, localeInfo.DAY)
		hour = (playTime - day * 60 * 24) / 60
		if hour:
			text += " %d%s" % (hour, localeInfo.HOUR)
		text += " %d%s" % (playTime - hour * 60 - day * 60 * 24, localeInfo.MINUTE)
		return text

	def __ShowSlotToolTip(self, index):
		realIndex = self.page * Config.PLAYER_SLOT_COUNT + index
		if realIndex >= len(self.filledSlots) or not self.toolTip:
			return

		realSlot = self.filledSlots[realIndex]
		guild = net.GetAccountCharacterSlotDataString(realSlot, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME) or localeInfo.SELECT_NOT_JOIN_GUILD
		guildLine = "%s : %s" % (localeInfo.GUILD_NAME, guild)
		timeLine = self.__FormatPlayTime(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME))

		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(6 * max(len(guildLine), len(timeLine)) + 22)
		self.toolTip.AppendTextLine(guildLine, 0xffe4cb1b, FALSE)
		self.toolTip.AppendTextLine(timeLine, 0xffe4cb1b, FALSE)
		self.hoverIndex = index
		(x, y) = self.slotButtons[index].GetGlobalPosition()
		self.toolTip.SetToolTipPosition(x + self.slotButtons[index].GetWidth() / 2, y - 2)
		self.toolTip.Show()

	def __UpdateSlotToolTipPosition(self):
		(x, y) = wndMgr.GetMousePosition()
		self.toolTip.SetToolTipPosition(x + 50, y + 50)

	def __HideSlotToolTip(self):
		self.hoverIndex = -1
		if self.toolTip:
			self.toolTip.Hide()

	# buttons

	def HasEmptyCharacterSlot(self):
		return self.__GetFirstEmptySlot() >= 0

	def CreateCharacter(self):
		# Baerim shows 4 slots and no arrows, so a 5th character could not be picked
		realSlot = self.__GetFirstEmptySlot()
		if realSlot < 0 or len(self.filledSlots) >= Config.PLAYER_SLOT_COUNT:
			self.PopupMessage(localeInfo.CREATE_FULL)
			return

		self.stream.SetCharacterSlot(realSlot)
		# like Baerim: an empty account stays on this screen and picks its empire on create
		if not self.filledSlots:
			self.reselectEmpire = TRUE
		else:
			self.stream.SetCreateCharacterPhase()

	def StartGame(self):
		if self.displaySlot < 0 or self.motionStart:
			return

		realSlot = self.filledSlots[self.displaySlot]
		if GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG):
			self.OpenChangeNameDialog()
			return

		self.motionStart = TRUE
		self.__SetButtonsEnabled(FALSE)

		if musicInfo.selectMusic != "":
			snd.FadeLimitOutMusic("BGM/" + musicInfo.selectMusic, systemSetting.GetMusicVolume() * 0.05)

		net.DirectEnter(realSlot)
		player.SetPlayTime(GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME))
		import chat
		chat.Clear()

	def ExitSelect(self):
		self.stream.SetLoginPhase()

	def __SetButtonsEnabled(self, enable):
		for btn in self.slotButtons + [self.createButton, self.deleteButton, self.startButton, self.exitButton]:
			if enable:
				btn.Enable()
			else:
				btn.Disable()

		if self.toolTip:
			self.toolTip.Hide()

		self.UpdateRaceImages()

	# delete

	def InputPrivateCode(self):
		if self.displaySlot < 0:
			return

		privateInputBoard = uiCommon.InputDialogWithDescription()
		privateInputBoard.SetTitle(localeInfo.INPUT_PRIVATE_CODE_DIALOG_TITLE)
		privateInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrivateCode))
		privateInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrivateCode))
		privateInputBoard.SetSecretMode()
		privateInputBoard.SetMaxLength(7)
		privateInputBoard.SetBoardWidth(250)
		privateInputBoard.SetDescription(localeInfo.INPUT_PRIVATE_CODE_DIALOG_DESCRIPTION)
		privateInputBoard.Open()
		self.privateInputBoard = privateInputBoard
		self.__SetButtonsEnabled(FALSE)
		chr.PushOnceMotion(chr.MOTION_INTRO_NOT_SELECTED, 0.1)

	def AcceptInputPrivateCode(self):
		privateCode = self.privateInputBoard.GetText()
		if not privateCode:
			return

		realSlot = self.filledSlots[self.displaySlot]
		if not GetSlotData(realSlot, net.ACCOUNT_CHARACTER_SLOT_ID):
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(realSlot, privateCode)
		self.PopupMessage(localeInfo.SELECT_DELEING)
		self.CancelInputPrivateCode()
		return TRUE

	def CancelInputPrivateCode(self):
		if self.privateInputBoard:
			self.privateInputBoard.Close()
		self.privateInputBoard = None
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		self.__SetButtonsEnabled(TRUE)
		self.SetFocus()
		return TRUE

	def OnDeleteSuccess(self, slot):
		self.PopupMessage(localeInfo.SELECT_DELETED)

		for i in self.madeSlots:
			chr.DeleteInstance(i)
		self.madeSlots = []

		self.Refresh()

	def OnDeleteFailure(self):
		self.PopupMessage(localeInfo.SELECT_CAN_NOT_DELETE)

	# change name

	def OpenChangeNameDialog(self):
		nameInputBoard = uiCommon.InputDialogWithDescription()
		nameInputBoard.SetTitle(localeInfo.SELECT_CHANGE_NAME_TITLE)
		nameInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputName))
		nameInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputName))
		nameInputBoard.SetMaxLength(chr.PLAYER_NAME_MAX_LEN)
		nameInputBoard.SetBoardWidth(200)
		nameInputBoard.SetDescription(localeInfo.SELECT_INPUT_CHANGING_NAME)
		nameInputBoard.Open()
		nameInputBoard.slot = self.filledSlots[self.displaySlot]
		self.nameInputBoard = nameInputBoard

	def AcceptInputName(self):
		changeName = self.nameInputBoard.GetText()
		if not changeName:
			return

		net.SendChangeNamePacket(self.nameInputBoard.slot, changeName)
		return self.CancelInputName()

	def CancelInputName(self):
		if self.nameInputBoard:
			self.nameInputBoard.Close()
		self.nameInputBoard = None
		return TRUE

	def OnCreateFailure(self, type):
		if 0 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_NAME)
		elif 1 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_ALREADY_EXIST_NAME)
		elif 100 == type:
			self.PopupMessage(localeInfo.SELECT_CHANGE_FAILURE_STRANGE_INDEX)

	def OnChangeName(self, id, name):
		self.__RefreshPage()
		self.PopupMessage(localeInfo.SELECT_CHANGED_NAME)

	# misc

	def SameLoginDisconnect(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, self.ExitSelect, localeInfo.UI_OK)

	def EmptyFunc(self):
		pass

	def PopupMessage(self, msg, func = 0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnKeyDown(self, key):
		if self.motionStart:
			return TRUE

		if app.DIK_RETURN == key:
			self.StartGame()
		elif app.DIK_UP == key and self.displaySlot > 0:
			self.SelectSlot(self.displaySlot - 1)
			self.page = self.displaySlot / Config.PLAYER_SLOT_COUNT
			self.__RefreshPage()
		elif app.DIK_DOWN == key and self.displaySlot + 1 < len(self.filledSlots):
			self.SelectSlot(self.displaySlot + 1)
			self.page = self.displaySlot / Config.PLAYER_SLOT_COUNT
			self.__RefreshPage()
		elif app.DIK_1 <= key <= app.DIK_4:
			self.ChooseSlot(key - app.DIK_1)
		return TRUE

	def OnPressExitKey(self):
		if self.countryFlagDropdown and self.countryFlagDropdown.IsOpened():
			self.CloseCountryFlagDropdown()
			return TRUE

		self.ExitSelect()
		return TRUE

	def OnUpdate(self):
		chr.Update()

		if self.reselectEmpire:
			self.reselectEmpire = FALSE
			self.stream.SetReselectEmpirePhase()
			self.Hide()
