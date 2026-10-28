if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import constInfo
import uiBeginnerEvent

if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
	import uiCommon
	import localeInfo

class GameButtonWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.beginnerPopup = None
		self.__LoadWindow("UIScript/gamewindow.py")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.popup = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self, filename):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, filename)

		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return FALSE

		try:
			self.gameButtonDict = {
				"STATUS" : self.GetChild("StatusPlusButton"),
				"SKILL" : self.GetChild("SkillPlusButton"),
				"QUEST" : self.GetChild("QuestButton"),
				"BUILD" : self.GetChild("BuildGuildBuilding"),
				"EXIT_OBSERVER" : self.GetChild("ExitObserver"),
				"GUIDE" : self.GetChild("GuidelineButton"),
				"BEGINNER" : self.GetChild("BeginnerQuestButton"),
				"NOTIFICATION_HISTORY" : self.GetChild("NotificationHistoryButton"),
				"BUFFI" : self.GetChild("SkillBuffiPlusButton"),
				#"BATTLEPASS" : self.GetChild("BattlePass"),##ENABLE_BATTLE_PASS
				#"BATTLEPASSPREMIUM" : self.GetChild("BattlePassPremium"),##ENABLE_BATTLE_PASS
			}

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.gameButtonDict["GUILDWAR"] = self.GetChild("GuildWarButton")
				self.gameButtonDict["GUILDWAR"].SetEvent(ui.__mem_func__(self.__RequestGuildWarEnter))

			self.gameButtonDict["EXIT_OBSERVER"].SetEvent(ui.__mem_func__(self.__OnClickExitObserver))
			#if app.ENABLE_BATTLE_PASS:
			#	self.gameButtonDict["BATTLEPASS"].SetEvent(ui.__mem_func__(self.__OnClickBattlePass))
			#	self.gameButtonDict["BATTLEPASSPREMIUM"].SetEvent(ui.__mem_func__(self.__OnClickBattlePassPremium))

			if app.ENABLE_GAYA_SYSTEM:
				# SKILL used to share the bottom-right corner with BUILD/EXIT_OBSERVER
				# and got nudged up to avoid overlapping there. It now lives in the
				# bottom-left Statu Gelistir stack (gamewindow.py), already spaced
				# correctly, so it must NOT get this adjustment anymore.
				posx, posy = self.gameButtonDict["BUILD"].GetGlobalPosition()
				self.gameButtonDict["BUILD"].SetPosition(posx, posy - 25 )

				posx, posy = self.gameButtonDict["EXIT_OBSERVER"].GetGlobalPosition()
				self.gameButtonDict["EXIT_OBSERVER"].SetPosition(posx, posy - 25 )

		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return FALSE

		self.__HideAllGameButton()
		import advanced_options_runtime
		if advanced_options_runtime.Get("view_guideline", True):
			self.gameButtonDict["GUIDE"].Show()
		self.gameButtonDict["BEGINNER"].SetEvent(ui.__mem_func__(self.__OnClickBeginner))
		self.gameButtonDict["BEGINNER"].Show()
		self.gameButtonDict["NOTIFICATION_HISTORY"].Show()
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.SetObserverMode(player.IsObserverMode(),True)
		else:
			self.SetObserverMode(player.IsObserverMode())
		return TRUE

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def __RequestGuildWarEnter(self):
			questionDialog = uiCommon.QuestionDialog()
			questionDialog.SetText(localeInfo.GAME_GUILDWAR_NOW_JOIN)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptGuildWarEnter))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.ClosePopUpDialog))
			questionDialog.Open()
			self.popup = questionDialog

		def AcceptGuildWarEnter(self):
			m2netm2g.SendChatPacket("/guildwar_request_enter")
			if self.popup:
				self.popup.Close()
			self.popup = None

		def ClosePopUpDialog(self):
			if self.popup:
				self.popup.Close()
			self.popup = None

		def ShowGuildWarButton(self):
			self.gameButtonDict["GUILDWAR"].Show()

		def HideGuildWarButton(self):
			self.gameButtonDict["GUILDWAR"].Hide()

	def Destroy(self):
		if self.beginnerPopup:
			self.beginnerPopup.Destroy()
			self.beginnerPopup = None

		for key in self.gameButtonDict:
			self.gameButtonDict[key].SetEvent(0)

		self.gameButtonDict={}

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.popup = None

	def __OnClickBeginner(self):
		if self.beginnerPopup is None:
			self.beginnerPopup = uiBeginnerEvent.BeginnerEventWindow()
		self.beginnerPopup.Open()
	def SetButtonEvent(self, name, event):
		try:
			self.gameButtonDict[name].SetEvent(event)

		except Exception, msg:
			print "GameButtonWindow.LoadScript - %s" % (msg)
			app.Abort()
			return

	def ShowBuildButton(self):
		self.gameButtonDict["BUILD"].Show()

	def HideBuildButton(self):
		self.gameButtonDict["BUILD"].Hide()

	#if app.ENABLE_BATTLE_PASS:
	#	def ShowPassButton(self):
	#		self.gameButtonDict["BATTLEPASS"].Show()
	#
	#	def HidePassButton(self):
	#		self.gameButtonDict["BATTLEPASS"].Hide()
	#
	#	def ShowPassButtonPremium(self):
	#		self.gameButtonDict["BATTLEPASSPREMIUM"].Show()
	#
	#	def HidePassButtonPremium(self):
	#		self.gameButtonDict["BATTLEPASSPREMIUM"].Hide()

	def CheckGameButton(self):

		if not self.IsShow():
			return

		statusPlusButton = self.gameButtonDict["STATUS"]
		skillPlusButton = self.gameButtonDict["SKILL"]

		if player.GetStatus(player.STAT) > 0:
			statusPlusButton.Show()
		else:
			statusPlusButton.Hide()

		if self.__IsSkillStat():
			skillPlusButton.Show()
		else:
			skillPlusButton.Hide()

		# Reference: GameButtonWindow.__OnClickBuffiSkill / SkillBuffiPlusButton ("Buffi'yi Yükselt"),
		# shown while the summoned Buffi has unused skill points. Same slot as the skill button.
		buffiButton = self.gameButtonDict["BUFFI"]
		interface = constInfo.GetInterfaceInstance() if hasattr(constInfo, "GetInterfaceInstance") else None
		companion = getattr(interface, "wndCompanion", None) if interface else None
		buffiSummoned = companion is not None and companion.buffiSummoned
		# Buffi sits right under the guide button (reference). When the skill button also needs that
		# slot, Buffi goes one row up and the guide/notification/beginner buttons move up with it.
		if not hasattr(self, "buffiBasePos"):
			self.buffiBasePos = {}
			for key in ("SKILL", "GUIDE", "NOTIFICATION_HISTORY", "BEGINNER"):
				self.buffiBasePos[key] = self.gameButtonDict[key].GetLocalPosition()

		shift = 40 if (buffiSummoned and skillPlusButton.IsShow()) else 0
		for key in ("GUIDE", "NOTIFICATION_HISTORY", "BEGINNER"):
			x, y = self.buffiBasePos[key]
			self.gameButtonDict[key].SetPosition(x, y - shift)

		if buffiSummoned:
			x, y = self.buffiBasePos["SKILL"]
			buffiButton.SetPosition(x, y - shift)
			buffiButton.Show()
		else:
			buffiButton.Hide()

	def __IsSkillStat(self):
		if player.GetStatus(player.SKILL_ACTIVE) > 0:
			return TRUE

		return FALSE

	def __OnClickExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __HideAllGameButton(self):
		for btn in self.gameButtonDict.values():
			btn.Hide()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def SetObserverMode(self, isEnable, isButtonShow):
			if isButtonShow:
				if isEnable:
					self.gameButtonDict["EXIT_OBSERVER"].Show()
				else:
					self.gameButtonDict["EXIT_OBSERVER"].Hide()
	else:
		def SetObserverMode(self, isEnable):
			if isEnable:
				self.gameButtonDict["EXIT_OBSERVER"].Show()
			else:
				self.gameButtonDict["EXIT_OBSERVER"].Hide()

	#if app.ENABLE_BATTLE_PASS:
	#	def __OnClickBattlePass(self):
	#		net.SendChatPacket("/open_battlepass")
	#
	#	def __OnClickBattlePassPremium(self):
	#		net.SendChatPacket("/open_battlepass_premium")
