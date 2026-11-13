if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import snd
import dbg
import event
import wndMgr
import _weakref
import musicInfo
import localeInfo
import systemSetting
import uiToolTip
import uiCommon
import uiScriptLocale

# Official introempire.py on Baerim's layout (selectempirewindow_baerim.py): no description board.
EMPIRE_NAME_COLOR = {
	net.EMPIRE_A : (0.7450, 0.0, 0.0),
	net.EMPIRE_B : (0.8666, 0.6156, 0.1843),
	net.EMPIRE_C : (0.2235, 0.2549, 0.7490),
}

class SelectEmpireWindow(ui.ScriptWindow):
	class EmpireButton(ui.Window):
		def __init__(self, owner, arg):
			ui.Window.__init__(self)
			self.owner = owner
			self.arg = arg

		def OnMouseOverIn(self):
			self.owner.OnOverInEmpire(self.arg)

		def OnMouseOverOut(self):
			self.owner.OnOverOutEmpire(self.arg)

		def OnMouseLeftButtonDown(self):
			if self.owner.empireID != self.arg:
				self.owner.OnSelectEmpire(self.arg)

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)

		self.stream = stream
		self.empireID = net.GetEmpireID()
		if not EMPIRE_NAME_COLOR.has_key(self.empireID):
			self.empireID = net.EMPIRE_A

		self.toolTip = None
		self.showToolTip = FALSE
		self.questionDialog = None
		self.empireArea = {}
		self.empireAreaFlag = {}
		self.empireFlag = {}
		self.empireAreaButton = {}
		self.empireAreaCurAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }
		self.empireAreaDestAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }
		self.empireAreaFlagCurAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }
		self.empireAreaFlagDestAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }
		self.empireFlagCurAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }
		self.empireFlagDestAlpha = { net.EMPIRE_A : 0.0, net.EMPIRE_B : 0.0, net.EMPIRE_C : 0.0 }

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)

	def Open(self):
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("SelectEmpireWindow")
		self.Show()

		if not self.__LoadScript("UIScript/selectempirewindow_baerim.py"):
			dbg.TraceError("SelectEmpireWindow.Open - __LoadScript Error")
			return

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()

		self.OnSelectEmpire(self.empireID)
		self.__CreateButtons()
		app.ShowCursor()

		if musicInfo.selectMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.selectMusic)

	def Close(self):
		if musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.selectMusic)

		self.__CloseQuestionDialog()
		self.showToolTip = FALSE
		if self.toolTip:
			self.toolTip.Hide()
		self.toolTip = None

		self.ClearDictionary()
		self.empireArea = {}
		self.empireAreaFlag = {}
		self.empireFlag = {}
		self.empireAreaButton = {}
		self.empireName = None

		self.KillFocus()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def __LoadScript(self, fileName):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.LoadObject")
			return FALSE

		try:
			Get = self.GetChild
			self.leftButton = Get("left_button")
			self.rightButton = Get("right_button")
			self.selectButton = Get("select_button")
			self.exitButton = Get("exit_button")
			self.empireName = Get("EmpireName")
			for (empire, suffix) in ((net.EMPIRE_A, "A"), (net.EMPIRE_B, "B"), (net.EMPIRE_C, "C")):
				self.empireArea[empire] = Get("EmpireArea_" + suffix)
				self.empireAreaFlag[empire] = Get("EmpireAreaFlag_" + suffix)
				self.empireFlag[empire] = Get("EmpireFlag_" + suffix)

			flagButtons = { net.EMPIRE_A : Get("shinsoo_btn"), net.EMPIRE_B : Get("chunjo_btn"), net.EMPIRE_C : Get("jinno_btn") }
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.BindObject")
			return FALSE

		for (button, text) in ((self.leftButton, uiScriptLocale.EMPIRE_PREV), (self.rightButton, uiScriptLocale.EMPIRE_NEXT),
							(self.selectButton, uiScriptLocale.EMPIRE_SELECT), (self.exitButton, uiScriptLocale.EMPIRE_EXIT)):
			button.ShowToolTip = lambda arg = text: self.OverInToolTip(arg)
			button.HideToolTip = lambda: self.OverOutToolTip()

		self.selectButton.SetEvent(ui.__mem_func__(self.ClickSelectButton))
		self.exitButton.SetEvent(ui.__mem_func__(self.ClickExitButton))
		self.leftButton.SetEvent(ui.__mem_func__(self.ClickLeftButton))
		self.rightButton.SetEvent(ui.__mem_func__(self.ClickRightButton))

		for (empire, button) in flagButtons.items():
			button.SetEvent(ui.__mem_func__(self.OnSelectEmpire), empire)

		for flag in self.empireAreaFlag.values() + self.empireFlag.values():
			flag.SetAlpha(0.0)

		return TRUE

	def __CreateButtons(self):
		for (key, img) in self.empireArea.items():
			img.SetAlpha(0.0)

			(x, y) = img.GetGlobalPosition()
			btn = self.EmpireButton(_weakref.proxy(self), key)
			btn.SetParent(self)
			btn.SetPosition(x, y)
			btn.SetSize(img.GetWidth(), img.GetHeight())
			btn.Show()
			self.empireAreaButton[key] = btn

	def OnOverInEmpire(self, arg):
		self.empireAreaDestAlpha[arg] = 1.0

	def OnOverOutEmpire(self, arg):
		if arg != self.empireID:
			self.empireAreaDestAlpha[arg] = 0.0

	def OnSelectEmpire(self, arg):
		for key in self.empireArea.keys():
			self.empireAreaDestAlpha[key] = 0.0
			self.empireAreaFlagDestAlpha[key] = 0.0
			self.empireFlagDestAlpha[key] = 0.0
		self.empireAreaDestAlpha[arg] = 1.0
		self.empireAreaFlagDestAlpha[arg] = 1.0
		self.empireFlagDestAlpha[arg] = 1.0
		self.empireID = arg

		self.empireName.SetText({ net.EMPIRE_A : uiScriptLocale.EMPIRE_A, net.EMPIRE_B : uiScriptLocale.EMPIRE_B, net.EMPIRE_C : uiScriptLocale.EMPIRE_C }[arg])
		(r, g, b) = EMPIRE_NAME_COLOR[arg]
		self.empireName.SetFontColor(r, g, b)
		snd.PlaySound("sound/ui/click.wav")

	def ClickLeftButton(self):
		self.empireID -= 1
		if self.empireID < net.EMPIRE_A:
			self.empireID = net.EMPIRE_C
		self.OnSelectEmpire(self.empireID)

	def ClickRightButton(self):
		self.empireID += 1
		if self.empireID > net.EMPIRE_C:
			self.empireID = net.EMPIRE_A
		self.OnSelectEmpire(self.empireID)

	def ClickSelectButton(self):
		self.__CloseQuestionDialog()
		(r, g, b) = EMPIRE_NAME_COLOR[self.empireID]
		name = { net.EMPIRE_A : uiScriptLocale.EMPIRE_A, net.EMPIRE_B : uiScriptLocale.EMPIRE_B, net.EMPIRE_C : uiScriptLocale.EMPIRE_C }[self.empireID]
		coloredName = "|cff%02x%02x%02x%s|r" % (int(r * 255), int(g * 255), int(b * 255), name)

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(uiScriptLocale.EMPIRE_SELECT_QUESTION % coloredName)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptSelectEmpire))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
		self.questionDialog.Open()

	def __CloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None

	def __GetFirstEmptySlot(self):
		for slot in xrange(5):
			if not net.GetAccountCharacterSlotDataInteger(slot, net.ACCOUNT_CHARACTER_SLOT_ID):
				return slot
		return -1

	def AcceptSelectEmpire(self):
		self.__CloseQuestionDialog()
		net.SendSelectEmpirePacket(self.empireID)

		# an empty account goes straight to character create, the select screen would only send it back here
		emptySlot = self.__GetFirstEmptySlot()
		if emptySlot == 0 and not any(net.GetAccountCharacterSlotDataInteger(slot, net.ACCOUNT_CHARACTER_SLOT_ID) for slot in xrange(5)):
			self.stream.SetCharacterSlot(emptySlot)
			self.stream.SetCreateCharacterPhase()
		else:
			self.stream.SetSelectCharacterPhase()
		self.Hide()

	def ClickExitButton(self):
		# Baerim goes back to character select
		self.stream.SetSelectCharacterPhase()
		self.Hide()

	def OnUpdate(self):
		self.__UpdateAlpha(self.empireArea, self.empireAreaCurAlpha, self.empireAreaDestAlpha)
		self.__UpdateAlpha(self.empireAreaFlag, self.empireAreaFlagCurAlpha, self.empireAreaFlagDestAlpha)
		self.__UpdateAlpha(self.empireFlag, self.empireFlagCurAlpha, self.empireFlagDestAlpha)

		if self.showToolTip:
			(x, y) = wndMgr.GetMousePosition()
			self.toolTip.SetToolTipPosition(x + 50, y + 50)

	def __UpdateAlpha(self, imageDict, curAlphaDict, destAlphaDict):
		for (key, img) in imageDict.items():
			curAlpha = curAlphaDict[key]
			destAlpha = destAlphaDict[key]

			if abs(destAlpha - curAlpha) / 10 > 0.0001:
				curAlpha += (destAlpha - curAlpha) / 7
			else:
				curAlpha = destAlpha

			curAlphaDict[key] = curAlpha
			img.SetAlpha(curAlpha)

	def OverInToolTip(self, arg):
		if not self.toolTip:
			return

		(x, y) = wndMgr.GetMousePosition()
		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(11 * len(str(arg)))
		self.toolTip.SetToolTipPosition(x + 50, y + 50)
		self.toolTip.AppendTextLine(str(arg), 0xffffff00)
		self.toolTip.Show()
		self.showToolTip = TRUE

	def OverOutToolTip(self):
		# the buttons still report a mouse out after Close dropped the tooltip
		self.showToolTip = FALSE
		if self.toolTip:
			self.toolTip.Hide()

	def OnPressEscapeKey(self):
		if self.questionDialog:
			self.__CloseQuestionDialog()
			return TRUE

		self.ClickExitButton()
		return TRUE

	def OnPressExitKey(self):
		self.ClickExitButton()
		return TRUE

class ReselectEmpireWindow(SelectEmpireWindow):
	def AcceptSelectEmpire(self):
		self._SelectEmpireWindow__CloseQuestionDialog()
		net.SendSelectEmpirePacket(self.empireID)
		self.stream.SetCreateCharacterPhase()
		self.Hide()

