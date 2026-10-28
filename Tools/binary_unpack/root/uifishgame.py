if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import wndMgr
import math

# game-settings
game_max_click = 3
game_max_time = 15.0
after_click_sleep = 1.0

# fish-settings
fish_randomly_sleep = 1.0
fish_next_walk = 0.2
fish_speed = 20.0
fish_random_pos_min_distance = 40

#server settings
game_command = "fish_game"
game_debug_mode = FALSE

#ai_algorithm = FALSE
#ai_index = 0

class FishGameWindow(ui.ScriptWindow):
	def Close(self):
		net.SendChatPacket("/%s %d close"%(game_command, self.gameKey))
		self.Hide()

	def Destroy(self):
		self.fishImage = None
		self.circleImage = None
		self.goalText = None
		self.timerGauge = None
		self.effectImage = None
		self.waveEffect = None

		self.debug_fish_pos = None
		self.debug_circle_pos = None
		self.debug_mouse_pos = None

		self.gameKey = 0
		self.leftTime = 0
		self.sleepTime = 0
		self.sleepClick = 0
		self.effectProcess = 0
		self.waveProcess = 0

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Destroy()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/fishinggamewindow.py")

			bg = self.GetChild("fishing_background_water")
			bg.SAFE_SetStringEvent("MOUSE_OVER_IN",self.SetFishCursor, TRUE)
			bg.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.SetFishCursor, FALSE)
			bg.SetEvent(ui.__mem_func__(self.ClickGameArea),"mouse_click")

			fishImage = self.GetChild("Fish")
			fishImage.Show()
			fishImage.onMoveDoneFunc = ui.__mem_func__(self.MoveDone)
			fishImage.SetMoveSpeed(fish_speed)
			self.fishImage = fishImage

			self.circleImage = self.GetChild("fishing_goal_circle")
			self.goalText = self.GetChild("fishing_goal_count_text")
			self.timerGauge = self.GetChild("fishing_timer_gauge")

			self.debug_fish_pos=self.GetChild("debug_text_fish_pos")
			self.debug_circle_pos=self.GetChild("debug_text_circle_pos")
			self.debug_mouse_pos=self.GetChild("debug_text_mouse_pos")
			if not game_debug_mode:
				self.debug_fish_pos.Hide()
				self.debug_circle_pos.Hide()
				self.debug_mouse_pos.Hide()

			waveEffect = self.GetChild("waveEffect")
			waveEffect.Hide()
			self.waveEffect = waveEffect

			#global ai_algorithm
			#if ai_algorithm:
			#	button0 = ui.RadioButton()
			#	button0.SetParent(self)
			#	button0.SetUpVisual("d:/ymir work/ui/public/xsmall_button_01.sub")
			#	button0.SetOverVisual("d:/ymir work/ui/public/xsmall_button_02.sub")
			#	button0.SetDownVisual("d:/ymir work/ui/public/xsmall_button_03.sub")
			#	button0.SetPosition(0,5)
			#	button0.SAFE_SetEvent(self.clickBtn, 0)
			#	button0.SetText("0")
			#	button0.Show()
			#	self.button0 = button0

			#	button1 = ui.RadioButton()
			#	button1.SetParent(self)
			#	button1.SetUpVisual("d:/ymir work/ui/public/xsmall_button_01.sub")
			#	button1.SetOverVisual("d:/ymir work/ui/public/xsmall_button_02.sub")
			#	button1.SetDownVisual("d:/ymir work/ui/public/xsmall_button_03.sub")
			#	button1.SetPosition(button0.GetWidth(),5)
			#	button1.SAFE_SetEvent(self.clickBtn, 1)
			#	button1.SetText("1")
			#	button1.Show()
			#	self.button1 = button1

			#	button2 = ui.RadioButton()
			#	button2.SetParent(self)
			#	button2.SetUpVisual("d:/ymir work/ui/public/xsmall_button_01.sub")
			#	button2.SetOverVisual("d:/ymir work/ui/public/xsmall_button_02.sub")
			#	button2.SetDownVisual("d:/ymir work/ui/public/xsmall_button_03.sub")
			#	button2.SetPosition(button0.GetWidth()+button0.GetWidth(),5)
			#	button2.SAFE_SetEvent(self.clickBtn, 2)
			#	button2.SetText("2")
			#	button2.Show()
			#	self.button2 = button2
			#
			#	self.clickBtn(0)

			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("FishGame.__LoadWindow.UIScript/fishinggamewindow.py")

	#if ai_algorithm:
	#	def __ClickRadioButton(self, buttonList, buttonIndex):
	#		try:
	#			btn=buttonList[buttonIndex]
	#		except IndexError:
	#			return
	#		for eachButton in buttonList:
	#			eachButton.SetUp()
	#		btn.Down()
	#	def clickBtn(self, index):
	#		listofBtn = [self.button0,self.button1,self.button2]
	#		self.__ClickRadioButton(listofBtn, index)
	#		global ai_index
	#		ai_index = index
	#	def CheckAI(self):
	#		global ai_index
	#		if ai_index == 1:
	#			self.sleepTime = 0
	#			(boxX, boxY) = self.GetChild("fishing_water_navArea").GetGlobalPosition()
	#			(mouseX, mouseY) = wndMgr.GetMousePosition()
	#			mouseX-=boxX
	#			mouseY-=boxY
	#			self.fishImage.SetMovePos(mouseX, mouseY)
	#			self.fishImage.MoveStart()
	#		elif ai_index == 2:
	#			self.sleepTime = 0
	#			(boxX, boxY) = self.GetChild("fishing_water_navArea").GetGlobalPosition()
	#			(mouseX, mouseY) = wndMgr.GetMousePosition()
	#			mouseX-=boxX
	#			mouseY-=boxY
	#			mouseX+=app.GetRandom(50,100)
	#			mouseY+=app.GetRandom(50,100)
	#			self.fishImage.SetMovePos(mouseX, mouseY)
	#			self.fishImage.MoveStart()

	def SetFishCursor(self, cursorMode = TRUE):
		cursor = app.FISH

		if cursorMode == FALSE:
			cursor = app.NORMAL

		if app.GetCursor() != cursor:
			app.SetCursor(cursor)

	def SetFishScore(self, scoreValue):
		self.goalText.SetText(str(scoreValue)+"/"+str(game_max_click))

	def Open(self, gameKey):
		self.gameKey = gameKey

		self.SetFishScore(0)
		self.fishImage.SetPosition((278 - 30)/2,0)

		if self.effectImage:
			self.effectImage.Hide()

		if self.waveEffect.IsShow():
			self.waveEffect.Hide()

		self.leftTime = app.GetTime()+game_max_time
		self.OnUpdate()

		self.GiveRandomPos()

		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def CheckLeftTime(self):
		leftTime = self.leftTime-app.GetTime()
		timerGauge = self.timerGauge

		if leftTime <= 0:
			timerGauge.SetPercentage(0, game_max_time)
			self.Close()
			return

		percent = leftTime/(game_max_time/100.0)
		if percent <= 50:
			timerGauge.SetDiffuseColor(1,0,0,1)
		else:
			timerGauge.SetDiffuseColor(1,1,1,1)
		timerGauge.SetPercentage(leftTime, game_max_time)

	def CheckCircle(self):
		circleImage = self.circleImage
		(x, y) = self.fishImage.GetLocalPosition()
		if x >= 60 and x <= 187 and y >= 34 and y <= 161:
			circleImage.SetDiffuseColor(1, 0, 0, 1)
			return TRUE
		else:
			circleImage.SetDiffuseColor(1, 1, 1, 1)
		return FALSE

	def GiveRandomPos(self):
		#global ai_algorithm
		#global ai_index
		#if ai_algorithm:
		#	if ai_index != 0:
		#		return

		backBox = self.GetChild("fishing_water_navArea")

		(x, y) = self.fishImage.GetLocalPosition()
		(newX,newY) = (app.GetRandom(3,backBox.GetWidth()-6),app.GetRandom(3,backBox.GetHeight()-6))
		(dx, dy) = (newX-x, newY-y)

		nextPosDistance = math.sqrt(dx*dx+dy*dy)
		if nextPosDistance <= fish_random_pos_min_distance:
			self.GiveRandomPos()
			return

		self.fishImage.SetMovePos(newX, newY)
		if app.GetRandom(1,3) == 2:
			self.sleepTime = app.GetTime()+fish_randomly_sleep
		else:
			self.sleepTime = app.GetTime()+fish_next_walk

	def MoveDone(self):
		self.GiveRandomPos()

	def CheckFish(self):
		if self.sleepTime != 0:
			leftTime = self.sleepTime-app.GetTime()
			if leftTime <= 0:
				self.sleepTime = 0
				self.fishImage.MoveStart()
				self.ShowWaveEffect()

	def OnUpdate(self):
		self.CheckLeftTime()
		self.CheckCircle()
		self.CheckFish()
		self.CheckEffectProcess()
		if game_debug_mode:
			self.CheckDebug()

		#global ai_algorithm
		#if ai_algorithm:
		#	self.CheckAI()


	def CheckDebug(self):
		(fishX, fishY) = self.fishImage.GetLocalPosition()
		fishR = self.fishImage.GetRotation()
		self.debug_fish_pos.SetText("Fish: x: %d y: %d r: %.2f"%(fishX, fishY, fishR))
		(circleX, circleY) = self.circleImage.GetLocalPosition()
		self.debug_circle_pos.SetText("Circle: x: %d y: %d"%(circleX,circleY))

		(boxX, boxY) = self.GetChild("fishing_water_navArea").GetGlobalPosition()
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		mouseX-=boxX
		mouseY-=boxY
		self.debug_mouse_pos.SetText("Mouse: x: %d y: %d"%(mouseX, mouseY))

	def ClickGameArea(self, arg):
		if self.sleepClick > app.GetTime():
			return
		self.sleepClick = app.GetTime()+after_click_sleep

		if self.CheckCircle():
			self.CreateFishEffect(1)
			net.SendChatPacket("/%s %d score 1"%(game_command,self.gameKey))
		else:
			self.CreateFishEffect(0)
			net.SendChatPacket("/%s %d score 0"%(game_command,self.gameKey))

	def CheckEffectProcess(self):
		if self.effectImage:
			if self.effectImage.IsShow():
				(x,y) = self.effectImage.GetLocalPosition()

				if (self.effectProcess%2) == 0:
					self.effectImage.SetPosition(x, y-1)

				self.effectProcess+=1
				if self.effectProcess == 30:
					self.effectImage.Hide()

		if self.waveEffect.IsShow():
			self.waveProcess+=1
			if self.waveProcess == 30:
				self.waveEffect.Hide()

	def CreateFishEffect(self, effectIndex):
		if self.effectImage:
			self.effectImage.Hide()
			self.effectImage = None

		self.effectProcess = 0
		image = ["fishing_effect_miss", "fishing_effect_hit"]

		(x, y) = self.fishImage.GetLocalPosition()

		effectImage = ui.ImageBox()
		effectImage.SetParent(self.GetChild("fishing_water_navArea"))
		effectImage.LoadImage("d:/ymir work/ui/game/fishing/"+image[effectIndex]+".sub")
		effectImage.SetPosition(x,y)
		effectImage.Show()
		self.effectImage = effectImage

	def ShowWaveEffect(self):
		(x, y) = self.fishImage.GetLocalPosition()
		self.waveEffect.SetPosition(x, y)
		self.waveEffect.Show()
		self.waveProcess = 0

