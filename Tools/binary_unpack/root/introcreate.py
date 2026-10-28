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

MAN = 0
WOMAN = 1

class CreateCharacterWindow(ui.Window):
	M2STATPOINT = (
		( 4, 3, 6, 3 ),
		( 3, 3, 4, 6 ),
		( 3, 5, 5, 3 ),
		( 4, 6, 3, 3 ),
	)

	M2JOBLIST = {
		0 : localeInfo.JOB_WARRIOR,
		1 : localeInfo.JOB_ASSASSIN,
		2 : localeInfo.JOB_SURA,
		3 : localeInfo.JOB_SHAMAN,
	}

	M2_CONST_ID = (
		(playerSettingModule.RACE_WARRIOR_M, playerSettingModule.RACE_WARRIOR_W),
		(playerSettingModule.RACE_ASSASSIN_M, playerSettingModule.RACE_ASSASSIN_W),
		(playerSettingModule.RACE_SURA_M, playerSettingModule.RACE_SURA_W),
		(playerSettingModule.RACE_SHAMAN_M, playerSettingModule.RACE_SHAMAN_W),
	)

	LEN_JOBLIST = len(M2JOBLIST)
	LEN_STATPOINT = len(M2STATPOINT[0])
	M2_INIT_VALUE = -1

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
			grp.ClearDepthBuffer()
			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			screenWidth = float(wndMgr.GetScreenWidth() / 2)
			screenHeight = float(wndMgr.GetScreenHeight())

			grp.SetViewport(0.26, 0.0, 0.5, 1.0)

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
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, self)
		self.stream = stream

		self.createSuccess = FALSE
		self.MotionStart = FALSE
		self.MotionTime = 0.0
		self.gender = self.M2_INIT_VALUE
		self.job_id = self.M2_INIT_VALUE
		self.shape = self.M2_INIT_VALUE
		self.statpoint = [0, 0, 0, 0]
		self.curGauge = [0.0, 0.0, 0.0, 0.0]

		self.charGender = []
		self.shapeList = [0, 0, 0, 0, 0]

	def __del__(self):
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, 0)
		ui.Window.__del__(self)

	def Open(self):
		if constInfo.SELECT_CHARACTER_ROTATION == TRUE:
			self.Rotation = 0

		dlgBoard = ui.ScriptWindow()
		self.dlgBoard = dlgBoard
		pythonScriptLoader = ui.PythonScriptLoader()
		pythonScriptLoader.LoadScriptFile(self.dlgBoard, "UIScript/CreateCharacterWindow.py")

		getChild = self.dlgBoard.GetChild

		self.JobList = [getChild(job) for job in ["Warrior", "Assassin", "Sura", "Shaman"]]

		self.SelectBtnFaceList = [getChild("CharacterFace_" + str(i)) for i in range(4)]

		self.genderButtonList = [getChild("GenderButton1"), getChild("GenderButton2")]
		self.shapeButtonList = [getChild("ShapeButton1"), getChild("ShapeButton2")]

		self.statValue = [getChild(stat + "_value") for stat in ["hth", "int", "str", "dex"]]
		self.GaugeList = [getChild(stat + "_gauge") for stat in ["hth", "int", "str", "dex"]]

		self.CreateButton = getChild("CreateButton")
		self.ExitButton = getChild("ExitButton")

		self.flag = getChild("EmpireFlag")
		self.flagDict = {
			net.EMPIRE_A : "d:/ymir work/ui/intro/empire/empireflag_a.sub",
			net.EMPIRE_B : "d:/ymir work/ui/intro/empire/empireflag_b.sub",
			net.EMPIRE_C : "d:/ymir work/ui/intro/empire/empireflag_c.sub",
		}

		self.editCharacterName = getChild("character_name_value")

		self.backGround = getChild("BackGround")

		self.JobSex = {i: app.GetRandom(MAN, WOMAN) for i in range(4)}

		self.editCharacterName.SetText("")

		self.EnableWindow()

		for i in range(4):
			self.JobList[i].SetEvent(ui.__mem_func__(self.SelectJob), i)

		for gender in [MAN, WOMAN]:
			self.genderButtonList[gender].SetEvent(ui.__mem_func__(self.SelectGender), gender)

		for i, button in enumerate(self.shapeButtonList):
			button.SetEvent(ui.__mem_func__(self.SelectShape), i)


		self.CreateButton.SetEvent(ui.__mem_func__(self.CreateCharacterButton))
		self.ExitButton.SetEvent(ui.__mem_func__(self.CancelButton))

		self.editCharacterName.SetReturnEvent(ui.__mem_func__(self.CreateCharacterButton))
		self.editCharacterName.SetEscapeEvent(ui.__mem_func__(self.CancelButton))

		my_empire = net.GetEmpireID()
		self.SetEmpire(my_empire)

		self.chrRenderer = self.CharacterRenderer()
		self.chrRenderer.SetParent(self.backGround)
		self.chrRenderer.Show()

		races = [
			playerSettingModule.RACE_WARRIOR_M,
			playerSettingModule.RACE_ASSASSIN_M,
			playerSettingModule.RACE_SURA_M,
			playerSettingModule.RACE_SHAMAN_M,
			playerSettingModule.RACE_WARRIOR_W,
			playerSettingModule.RACE_ASSASSIN_W,
			playerSettingModule.RACE_SURA_W,
			playerSettingModule.RACE_SHAMAN_W
		]

		for race in races:
			self.MakeCharacter(race)

		for i in xrange(self.LEN_JOBLIST):
			if i == 4:
				self.charGender.append(0)
			self.charGender.append(app.GetRandom(0, 1))

		self.dlgBoard.Show()
		self.Show()
		app.ShowCursor()

		if musicInfo.createMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.createMusic)

		self.SelectJob(app.GetRandom(0, self.LEN_JOBLIST - 1))

	def Close(self):
		self.createSuccess = None
		self.MotionStart = None
		self.MotionTime = None
		self.gender = None
		self.job_id = None
		self.shape = None
		self.statpoint = None
		self.curGauge  = None

		for i in xrange(len(self.M2_CONST_ID)):
			chr.DeleteInstance(i)

		self.stream = None
		self.M2STATPOINT = None
		self.M2JOBLIST = None
		self.M2_CONST_ID = None
		self.LEN_JOBLIST = None
		self.LEN_STATPOINT = None
		self.M2_INIT_VALUE = None
		self.RACE_FACE_PATH = None
		self.JobList = None
		self.SelectBtnFaceList = None
		self.genderButtonList = None
		self.shapeButtonList = None
		self.statValue = None
		self.GaugeList = None
		self.flag = None
		self.flagDict = None

		self.shapeList = None

		self.CreateButton = None
		self.ExitButton = None
		self.editCharacterName = None
		self.backGround = None
		self.JobSex = None
		self.dlgBoard.ClearDictionary()

		if musicInfo.createMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.createMusic)

		self.dlgBoard.Hide()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def OnCreateSuccess(self):
		self.createSuccess = TRUE

	def OnCreateFailure(self, type):
		self.MotionStart = FALSE
		chr.BlendLoopMotion(chr.MOTION_INTRO_WAIT, 0.1)

		if 1 == type:
			self.PopupMessage(localeInfo.CREATE_EXIST_SAME_NAME, self.EnableWindow)
		else:
			self.PopupMessage(localeInfo.CREATE_FAILURE, self.EnableWindow)

	def EnableWindow(self):
		for i in xrange(self.LEN_JOBLIST):
			self.JobList[i].Enable()

		self.CreateButton.Enable()
		self.ExitButton.Enable()
		self.editCharacterName.SetFocus()
		self.editCharacterName.Enable()

		for i in xrange(2):
			self.genderButtonList[i].Enable()
			self.shapeButtonList[i].Enable()

	def DisableWindow(self):
		for i in xrange(self.LEN_JOBLIST):
			self.JobList[i].Disable()

		self.CreateButton.Disable()
		self.ExitButton.Disable()
		self.editCharacterName.Disable()

		for i in xrange(2):
			self.genderButtonList[i].Disable()
			self.shapeButtonList[i].Disable()

	def MakeCharacter(self, race):
		chr_id = race

		chr.CreateInstance(chr_id)
		chr.SelectInstance(chr_id)
		chr.SetVirtualID(chr_id)
		chr.SetNameString(str(race))

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.SetHair(0)

		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)

		chr.SetRotation(-7.0)
		chr.Hide()

	def SelectJob(self, job_id):
		if self.MotionStart :
			self.JobList[job_id].SetUp()
			return

		for button in self.JobList:
			button.SetUp()

		self.job_id = job_id
		self.JobList[self.job_id].Down()
		self.SelectGender(self.JobSex[job_id])

		self.ResetStat()

		self.genderButtonList[WOMAN].Show()

	def SelectGender(self, gender):
		for button in self.genderButtonList:
			button.SetUp()

		self.gender = gender
		self.genderButtonList[self.gender].Down()

		if self.M2_INIT_VALUE == self.job_id:
			return

		self.JobSex[self.job_id] = self.gender
		self.race = self.M2_CONST_ID[self.job_id][self.gender]

		for i in xrange(self.LEN_JOBLIST):
			if self.job_id == i:
				self.SelectBtnFaceList[i].LoadImage(self.RACE_FACE_PATH[self.race] + "1.sub")
			else:
				self.SelectBtnFaceList[i].LoadImage( self.RACE_FACE_PATH[self.M2_CONST_ID[i][self.JobSex[i]]] + "2.sub")

		if self.M2_INIT_VALUE == self.shape:
			self.shape = 0

		self.SelectShape(self.shapeList[self.job_id])

	def SelectShape(self, shape):
		self.shapeList[self.job_id] = shape
		for button in self.shapeButtonList:
			button.SetUp()

		self.shape = shape
		self.shapeButtonList[self.shape].Down()

		if self.M2_INIT_VALUE == self.job_id:
			return

		chr.Hide()
		chr.SelectInstance(self.race)
		chr.ChangeShape(self.shape)
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		chr.Show()

	def RefreshStat(self):
		statSummary = 8.0
		self.curGauge = [
			float(self.statpoint[0])/statSummary,
			float(self.statpoint[1])/statSummary,
			float(self.statpoint[2])/statSummary,
			float(self.statpoint[3])/statSummary,
		]

		for i in xrange(self.LEN_STATPOINT):
			self.statValue[i].SetText(str(self.statpoint[i]))

	def ResetStat(self):
		for i in xrange(self.LEN_STATPOINT) :
			self.statpoint[i] = self.M2STATPOINT[self.job_id][i]
		self.RefreshStat()

	def CreateCharacterButton(self):
		if self.job_id == self.M2_INIT_VALUE or self.MotionStart :
			return 

		textName = self.editCharacterName.GetText()

		if FALSE == self.CheckCreateCharacterName(textName): 
			return

		self.DisableWindow()

		chrSlot = self.stream.GetCharacterSlot()
		raceIndex = self.M2_CONST_ID[self.job_id][self.gender]
		shapeIndex = self.shape
		statCon = self.M2STATPOINT[self.job_id][0]
		statInt = self.M2STATPOINT[self.job_id][1]
		statStr = self.M2STATPOINT[self.job_id][2]
		statDex = self.M2STATPOINT[self.job_id][3]

		chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED)
		net.SendCreateCharacterPacket(chrSlot, textName, raceIndex, shapeIndex, statCon, statInt, statStr, statDex)

		self.MotionStart = TRUE
		self.MotionTime = app.GetTime()

	def CancelButton(self):
		chr.DeleteInstance(self.M2_CONST_ID[self.job_id][self.gender])
		self.stream.SetSelectCharacterPhase()

	def EmptyFunc(self):
		pass

	def PopupMessage(self, msg, func=0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnPressExitKey(self):
		self.CancelButton()
		return TRUE

	def CheckCreateCharacterName(self, name):
		if len(name) == 0:
			self.PopupMessage(localeInfo.CREATE_INPUT_NAME, self.EnableWindow)
			return FALSE

		if name.find(localeInfo.CREATE_GM_NAME)!=-1:
			self.PopupMessage(localeInfo.CREATE_ERROR_GM_NAME, self.EnableWindow)
			return FALSE

		if net.IsInsultIn(name):
			self.PopupMessage(localeInfo.CREATE_ERROR_INSULT_NAME, self.EnableWindow)
			return FALSE

		return TRUE

	def SetEmpire(self, empire_id):
		if empire_id != net.EMPIRE_A :
			self.flag.LoadImage(self.flagDict[empire_id])
			self.flag.SetScale(0.55, 0.55)

	def OnUpdate(self):
		if constInfo.SELECT_CHARACTER_ROTATION == TRUE:
			self.Rotation = (self.Rotation - 0.2) % 360.0
			chr.SetRotation(self.Rotation)

		chr.Update()

		for i in xrange(self.LEN_STATPOINT):
			self.GaugeList[i].SetPercentage(self.curGauge[i], 1.0)

		if self.MotionStart and self.createSuccess and app.GetTime() - self.MotionTime >= 2.0:
			chr.DeleteInstance(self.M2_CONST_ID[self.job_id][self.gender])

			self.MotionStart = FALSE
			self.stream.SetSelectCharacterPhase()
			self.Hide()
