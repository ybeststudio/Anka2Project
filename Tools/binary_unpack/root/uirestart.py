if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
import ui

if app.ENABLE_RENEWAL_DEAD_PACKET:
	import localeInfo

if app.ENABLE_RESTART_INSTANT:
	import uiCommon

if app.ENABLE_SUNG_MAHI_TOWER:
	import wndMgr

class RestartDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/restartdialog.py")
		except Exception, msg:
			import sys
			(type, msg, tb) = sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0

		try:
			self.restartHereButton = self.GetChild("restart_here_button")
			self.restartTownButton = self.GetChild("restart_town_button")

			if app.ENABLE_RESTART_INSTANT:
				self.restartInstantButton = self.GetChild("restart_instant_button")
		except:
			import sys
			(type, msg, tb) = sys.exc_info()
			dbg.TraceError("RestartDialog.LoadDialog - %s:%s" % (type, msg))
			app.Abort()
			return 0

		self.restartHereButton.SetEvent(ui.__mem_func__(self.RestartHere))
		self.restartTownButton.SetEvent(ui.__mem_func__(self.RestartTown))

		if app.ENABLE_SUNG_MAHI_TOWER:
			self.isSungMahiMap = False

			self.dlgBox = ui.Board()
			self.dlgBox.SetSize(300, 100)
			self.dlgBox.SetPosition((wndMgr.GetScreenWidth() / 2) - 150, (wndMgr.GetScreenHeight() / 2) - 100)

			self.dlgBoxText = ui.TextLine()
			self.dlgBoxText.SetParent(self.dlgBox)
			self.dlgBoxText.SetPosition(40, 35)
			self.dlgBoxText.SetText("O infrangere amara! Calatoria ta se termina aici.")
			self.dlgBoxText.Show()

			self.dlgBoxButton = ui.Button()
			self.dlgBoxButton.SetParent(self.dlgBox)
			self.dlgBoxButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
			self.dlgBoxButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
			self.dlgBoxButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
			self.dlgBoxButton.SetPosition(125, 60)
			self.dlgBoxButton.SetEvent(ui.__mem_func__(self.RestartHere))
			self.dlgBoxButton.SetText("OK")
			self.dlgBoxButton.Show()

			self.dlgBox.Hide()

		if app.ENABLE_RESTART_INSTANT:
			self.restartInstantButton.SetEvent(ui.__mem_func__(self.RestartInstantMessage))

		if app.ENABLE_RENEWAL_DEAD_PACKET:
			self.reviveTimeStamp = [0 for i in xrange(player.REVIVE_TYPE_MAX)]
			self.reviveTimeTexts = map(lambda i:self.GetChild("T{:02d}".format(i)), xrange(player.REVIVE_TYPE_MAX))

		return 1

	def Destroy(self):
		self.restartHereButton = 0
		self.restartTownButton = 0

		if app.ENABLE_RESTART_INSTANT:
			self.restartInstantButton = 0

		if app.ENABLE_RENEWAL_DEAD_PACKET:
			del self.reviveTimeStamp[:]
			del self.reviveTimeTexts[:]

		self.ClearDictionary()

	if app.ENABLE_SUNG_MAHI_TOWER:
		def SetSungMahiRestartDialog(self):
			self.isSungMahiMap = True

	if app.ENABLE_RENEWAL_DEAD_PACKET:
		def OpenDialog(self, times):
			if app.ENABLE_SUNG_MAHI_TOWER:
				if self.isSungMahiMap:
					self.dlgBox.Show()
					return
			import chat
			chat.AppendChat(1,"{}".format(times))

			for x in xrange(player.REVIVE_TYPE_MAX):
				self.reviveTimeStamp[x] = app.GetTime() + times[x]
				self.reviveTimeTexts[x].Show()

			self.restartHereButton.Disable()
			self.restartTownButton.Disable()
			self.Show()

		def OnUpdate(self):
			for x in xrange(player.REVIVE_TYPE_MAX): 
				endtime = self.reviveTimeStamp[x] - app.GetTime()
				if endtime <= .1:
					self.reviveTimeTexts[x].Hide()
					if x == player.REVIVE_TYPE_HERE:
						self.restartHereButton.Enable()
					if x == player.REVIVE_TYPE_TOWN:
						self.restartTownButton.Enable()

				if x == player.REVIVE_TYPE_AUTO_TOWN:
					self.reviveTimeTexts[x].SetText(localeInfo.REVIVE_AUTO_TOWN_MESSAGE.format(localeInfo.SecondToMS(endtime)))
				else:
					self.reviveTimeTexts[x].SetText("{0:.1f}".format(endtime))
	else:
		def OpenDialog(self):
			self.Show()

	def Close(self):
		self.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.dlgBox.IsShow():
				self.dlgBox.Hide()

		return TRUE

	def RestartHere(self):
		net.SendChatPacket("/restart_here")

	def RestartTown(self):
		net.SendChatPacket("/restart_town")

	if app.ENABLE_RESTART_INSTANT:
		def RestartInstantMessage(self):
			acceptQuestionDialog = uiCommon.QuestionDialog("thin")
			acceptQuestionDialog.SetText(localeInfo.RESTART_INSTANT_MESSAGE)
			acceptQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.RestartInstant())
			acceptQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.OnCloseQuestionDialog())
			acceptQuestionDialog.Open()
			self.acceptQuestionDialog = acceptQuestionDialog

		def RestartInstant(self):
			self.acceptQuestionDialog = None
			net.SendChatPacket("/restart_now")

	def OnCloseQuestionDialog(self):
		if self.acceptQuestionDialog:
			self.acceptQuestionDialog.Close()

		self.acceptQuestionDialog = None

	def OnPressExitKey(self):
		return TRUE

	def OnPressEscapeKey(self):
		return TRUE
