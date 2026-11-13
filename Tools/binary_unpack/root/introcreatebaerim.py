if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import wndMgr
import snd
import event
import constInfo
import musicInfo
import localeInfo
import systemSetting
import playerSettingModule
import uiScriptLocale

# Baerim introCreate (Cython) rebuilt from its runtime dump: Config, START_STAT and the method names are theirs
class Config:
	FACE_PATH = "d:/ymir work/ui/intro/public_intro/face/"
	SELECT_PATH = "d:/ymir work/ui/intro/select/"
	PLAYER_SLOT_COUNT = 4
	# slot -> race for each gender
	PLAYER_VALUES = {
		"m" : { 0 : 0, 1 : 5, 2 : 2, 3 : 7 },
		"w" : { 0 : 4, 1 : 1, 2 : 6, 3 : 3 },
	}
	# Baerim dumps -35.0, but with our camera that stands the character ~40 px higher than their screen
	RENDER_CENTER_Z = -20.0
	DESC_LINE_HEIGHT = 17
	DESC_TEXT_COLOR = 0xff3e372f
	DESC_TEXT_WIDTH = 165
	DESC_VISIBLE_LINE_COUNT = 13

MALE = "m"
FEMALE = "w"

JOB_FACE_NAMES = ("warrior", "assassin", "sura", "shaman")
JOB_NAME_WINDOWS = ("name_warrior", "name_assassin", "name_sura", "name_shaman")

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

class CreateCharacterWindow(ui.Window):
	# indexed by race
	START_STAT = (
		(4, 3, 6, 3), (3, 3, 4, 6), (3, 5, 5, 3), (4, 6, 3, 3),
		(4, 3, 6, 3), (3, 3, 4, 6), (3, 5, 5, 3), (4, 6, 3, 3),
	)
	DESCRIPTION_LOCALE_KEYS = ("JOBDESC_WARRIOR", "JOBDESC_ASSASSIN", "JOBDESC_SURA", "JOBDESC_SHAMAN")
	# measured on Baerim: HP 4 fills a third of the bar
	STAT_GAUGE_MAX = 12.0

	class CharacterRenderer(ui.Window):
		def OnRender(self):
			# a viewport that does not start at x 0 still draws from the left edge here, so render over the
			# whole screen: the camera looks at the character, the panels are symmetric, it ends up between them
			sw = float(wndMgr.GetScreenWidth())
			sh = float(wndMgr.GetScreenHeight())
			(x, y, w, h) = (0.0, 0.0, sw, sh)

			grp.ClearDepthBuffer()
			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			grp.SetViewport(x / sw, y / sh, w / sw, h / sh)

			app.SetCenterPosition(0.0, 0.0, Config.RENDER_CENTER_Z)
			app.SetCamera(1550.0, 15.0, 180.0, 95.0)
			grp.SetPerspective(10.0, w / h, 1000.0, 3000.0)

			(mx, my) = app.GetCursorPosition()
			grp.SetCursorPosition(mx, my)

			chr.Deform()
			chr.Render()

			grp.RestoreViewport()
			grp.PopState()
			grp.SetInterfaceRenderState()

	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, self)
		self.stream = stream

		self.dlgBoard = None
		self.chrRenderer = None
		self.slot = -1
		self.gender = MALE
		self.genderBySlot = [MALE] * Config.PLAYER_SLOT_COUNT
		self.shapeBySlot = [0] * Config.PLAYER_SLOT_COUNT
		self.race = -1
		self.statpoint = [0, 0, 0, 0]
		self.createSuccess = FALSE
		self.motionStart = FALSE
		self.motionTime = 0.0
		self.descLines = []
		self.descLineWindows = []
		self.descStartLine = 0
		self.madeRaces = []

	def __del__(self):
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, 0)
		ui.Window.__del__(self)

	def Open(self):
		if not self.__LoadScript():
			return

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		self.SetEmpire(net.GetEmpireID())
		self.__CreateDescLineWindows()
		self.ChooseSlot(0)
		self.EnableWindow()

		self.dlgBoard.Show()
		self.Show()
		app.ShowCursor()

		if musicInfo.createMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.createMusic)

	def Close(self):
		if musicInfo.createMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.createMusic)

		for race in self.madeRaces:
			chr.DeleteInstance(race)
		self.madeRaces = []

		self.__ClearDescLineWindows()

		if self.dlgBoard:
			self.dlgBoard.ClearDictionary()
			self.dlgBoard.Hide()

		self.dlgBoard = None
		self.chrRenderer = None
		self.stream = None

		self.Hide()
		app.HideCursor()
		event.Destroy()

	def __LoadScript(self):
		try:
			dlgBoard = ui.ScriptWindow()
			ui.PythonScriptLoader().LoadScriptFile(dlgBoard, "UIScript/createcharacterwindow_baerim.py")
		except:
			import exception
			exception.Abort("CreateCharacterWindow.__LoadScript.LoadObject")
			return FALSE

		self.dlgBoard = dlgBoard
		Get = dlgBoard.GetChild

		try:
			self.background = Get("Background")
			self.renderWindow = Get("character_render_window")
			self.jobNameImages = [Get(name) for name in JOB_NAME_WINDOWS]
			self.slotButtons = [Get("slot_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.raceImages = [Get("race_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.raceLabels = [Get("race_label_%d" % i) for i in xrange(Config.PLAYER_SLOT_COUNT)]
			self.kingdomImage = Get("kingdom_image")
			self.kingdomText = Get("kingdom")
			self.genderButtons = { MALE : Get("btn_gender_02"), FEMALE : Get("btn_gender_01") }
			self.shapeButtons = [Get("btn_shape_01"), Get("btn_shape_02")]
			self.discFace = Get("DiscFace")
			self.raceNameText = Get("raceName_Text")
			self.textBoard = Get("text_board")
			self.prevButton = Get("prev_button")
			self.nextButton = Get("next_button")
			self.gauges = [Get(name) for name in ("gauge_hp", "gauge_int", "gauge_str", "gauge_dex")]
			self.statValues = [Get(name) for name in ("text_value_hp", "text_value_int", "text_value_str", "text_value_dex")]
			self.editName = Get("edit_name")
			self.createButton = Get("btn_create")
			self.exitButton = Get("btn_exit")
		except:
			import exception
			exception.Abort("CreateCharacterWindow.__LoadScript.BindObject")
			return FALSE

		for i in xrange(Config.PLAYER_SLOT_COUNT):
			self.slotButtons[i].SetEvent(ui.__mem_func__(self.ChooseSlot), i)

		self.genderButtons[MALE].SetEvent(ui.__mem_func__(self.__ChangeGender), MALE)
		self.genderButtons[FEMALE].SetEvent(ui.__mem_func__(self.__ChangeGender), FEMALE)
		for i in xrange(len(self.shapeButtons)):
			self.shapeButtons[i].SetEvent(ui.__mem_func__(self.__ChangeShape), i)

		self.prevButton.SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
		self.nextButton.SetEvent(ui.__mem_func__(self.NextDescriptionPage))

		self.createButton.SetEvent(ui.__mem_func__(self.__CreateCharacter))
		self.exitButton.SetEvent(ui.__mem_func__(self.CancelCreate))

		self.editName.SetText("")
		self.editName.SetReturnEvent(ui.__mem_func__(self.__CreateCharacter))
		self.editName.SetEscapeEvent(ui.__mem_func__(self.CancelCreate))

		self.chrRenderer = self.CharacterRenderer()
		self.chrRenderer.SetParent(self.renderWindow)
		self.chrRenderer.SetSize(self.renderWindow.GetWidth(), self.renderWindow.GetHeight())
		self.chrRenderer.AddFlag("not_pick")
		self.chrRenderer.Show()
		return TRUE

	def __MakeCharacter(self, race):
		# Baerim builds a character only when its slot is first shown
		if race in self.madeRaces:
			return
		self.madeRaces.append(race)

		chr.CreateInstance(race)
		chr.SelectInstance(race)
		chr.SetVirtualID(race)
		chr.SetNameString(str(race))

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.SetHair(0)

		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)

		chr.SetRotation(7.0)
		chr.Hide()

	# selection

	def __ClickRadioButton(self, buttonList, buttonIndex):
		for i in xrange(len(buttonList)):
			if i == buttonIndex:
				buttonList[i].Down()
			else:
				buttonList[i].SetUp()

	def ChooseSlot(self, slot):
		if self.motionStart:
			self.__ClickRadioButton(self.slotButtons, self.slot)
			return

		self.__SelectSlot(slot)

	def __SelectSlot(self, slot):
		self.slot = slot
		self.gender = self.genderBySlot[slot]
		self.__ClickRadioButton(self.slotButtons, slot)
		self.__ClickRadioButton([self.genderButtons[MALE], self.genderButtons[FEMALE]], 0 if self.gender == MALE else 1)
		self.__SetJobDescription(slot)
		self.__UpdateCharacterVisuals()

	def __ChangeGender(self, gender):
		if self.motionStart or self.slot < 0:
			return

		self.genderBySlot[self.slot] = gender
		self.__SelectSlot(self.slot)

	def __ChangeShape(self, shape):
		if self.motionStart or self.slot < 0:
			return

		self.shapeBySlot[self.slot] = shape
		self.__UpdateCharacterVisuals()

	def __UpdateCharacterVisuals(self):
		jobNames = (localeInfo.JOB_WARRIOR, localeInfo.JOB_ASSASSIN, localeInfo.JOB_SURA, localeInfo.JOB_SHAMAN)

		for i in xrange(Config.PLAYER_SLOT_COUNT):
			genderKey = self.genderBySlot[i]
			image = "1.sub" if i == self.slot else "2.sub"
			self.raceImages[i].LoadImage("%sface_%s_%s_0%s" % (Config.FACE_PATH, JOB_FACE_NAMES[i], genderKey, image))
			self.raceLabels[i].SetText(jobNames[i])

			if i == self.slot:
				self.jobNameImages[i].Show()
			else:
				self.jobNameImages[i].Hide()

		job = self.slot
		self.race = Config.PLAYER_VALUES[self.gender][job]
		self.discFace.LoadImage("icon/face/%s_%s.png" % (JOB_FACE_NAMES[job], self.gender))
		self.raceNameText.SetText(jobNames[job])

		shape = self.shapeBySlot[job]
		self.__ClickRadioButton(self.shapeButtons, shape)

		# hide the one shown so far before a new instance takes the selection
		if self.madeRaces:
			chr.Hide()
		self.__MakeCharacter(self.race)
		chr.SelectInstance(self.race)
		chr.ChangeShape(shape)
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.Show()

		self.ResetStat()

	# stats

	def ResetStat(self):
		for i in xrange(len(self.statpoint)):
			self.statpoint[i] = self.START_STAT[self.race][i]
		self.RefreshStat()

	def RefreshStat(self):
		for i in xrange(len(self.statpoint)):
			self.statValues[i].SetText(str(self.statpoint[i]))
			self.gauges[i].SetPercentage(self.statpoint[i], self.STAT_GAUGE_MAX)

	# job description

	def __GetJobDescText(self, jobIndex):
		return getattr(localeInfo, self.DESCRIPTION_LOCALE_KEYS[jobIndex], "")

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

	def __ClearDescLineWindows(self):
		for line in self.descLineWindows:
			line.Hide()
		self.descLineWindows = []

	def __SetJobDescription(self, jobIndex):
		self.descLines = self.__WrapJobDescText(self.__GetJobDescText(jobIndex), Config.DESC_TEXT_WIDTH)
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

	# create

	def EnableWindow(self):
		for btn in self.slotButtons + self.shapeButtons + self.genderButtons.values():
			btn.Enable()
		self.createButton.Enable()
		self.exitButton.Enable()
		self.editName.Enable()
		self.editName.SetFocus()

	def DisableWindow(self):
		for btn in self.slotButtons + self.shapeButtons + self.genderButtons.values():
			btn.Disable()
		self.createButton.Disable()
		self.exitButton.Disable()
		self.editName.Disable()

	def __CheckCreateCharacter(self, name):
		if len(name) == 0:
			self.PopupMessage(localeInfo.CREATE_INPUT_NAME, self.EnableWindow)
			return FALSE

		if name.find(localeInfo.CREATE_GM_NAME) != -1:
			self.PopupMessage(localeInfo.CREATE_ERROR_GM_NAME, self.EnableWindow)
			return FALSE

		if net.IsInsultIn(name):
			self.PopupMessage(localeInfo.CREATE_ERROR_INSULT_NAME, self.EnableWindow)
			return FALSE

		return TRUE

	def __CreateCharacter(self):
		if self.slot < 0 or self.motionStart:
			return

		textName = self.editName.GetText()
		if not self.__CheckCreateCharacter(textName):
			return

		self.DisableWindow()

		chrSlot = self.stream.GetCharacterSlot()
		stat = self.START_STAT[self.race]
		chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED)
		net.SendCreateCharacterPacket(chrSlot, textName, self.race, self.shapeBySlot[self.slot], stat[0], stat[1], stat[2], stat[3])

		self.motionStart = TRUE
		self.motionTime = app.GetTime()

	def OnCreateSuccess(self):
		# Baerim goes back right away; the official screen waited 2 s for the intro motion
		self.createSuccess = TRUE
		self.motionStart = FALSE
		self.stream.SetSelectCharacterPhase()

	def OnCreateFailure(self, errorType):
		self.motionStart = FALSE
		chr.BlendLoopMotion(chr.MOTION_INTRO_WAIT, 0.1)

		if 1 == errorType:
			self.PopupMessage(localeInfo.CREATE_EXIST_SAME_NAME, self.EnableWindow)
		else:
			self.PopupMessage(localeInfo.CREATE_FAILURE, self.EnableWindow)

	def PopupMessage(self, message, callback = 0):
		if not callback:
			callback = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(message, callback, localeInfo.UI_OK)

	def EmptyFunc(self):
		pass

	def CancelCreate(self):
		if self.motionStart:
			return

		self.stream.SetSelectCharacterPhase()

	def OnPressExitKey(self):
		self.CancelCreate()
		return TRUE

	def OnUpdate(self):
		chr.Update()
