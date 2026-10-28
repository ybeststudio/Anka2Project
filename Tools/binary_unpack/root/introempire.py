if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import wndMgr
import dbg
import event
import _weakref
import localeInfo
import constInfo
import uiScriptLocale

class SelectEmpireWindow(ui.ScriptWindow):

	empireId = {
		0 : net.EMPIRE_A,
		1 : net.EMPIRE_B,
		2 : net.EMPIRE_C
	}

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)
		self.stream = stream

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)

	def Close(self):
		self.ClearDictionary()
		self.exitButton = None
		self.KillFocus()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def Open(self):
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("SelectEmpireWindow")
		self.Show()

		if not self.__LoadScript("uiScript/SelectEmpireWindow.py"):
			dbg.TraceError("SelectEmpireWindow.Open - __LoadScript Error")
			return

		app.ShowCursor()

	def __LoadScript(self, fileName):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.LoadObject")

		try:
			self.selectButtons = {
				0 : self.GetChild("ShinsooSelectButton"),
				1 : self.GetChild("ChunjoSelectButton"),
				2 : self.GetChild("JinnoSelectButton"),
			}
			self.exitButton = self.GetChild("ExitButton")
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.BindObject")

		self.exitButton.SetEvent(ui.__mem_func__(self.ClickExitButton))
		for id in range(len(self.selectButtons)):
			self.selectButtons[id].SetEvent(ui.__mem_func__(self.SelectEmpire), id)

		return 1

	def SelectEmpire(self, id):
		net.SendSelectEmpirePacket(self.empireId[id])
		self.stream.SetSelectCharacterPhase()

	def ClickExitButton(self):
		self.stream.SetLoginPhase()

	def OnPressEscapeKey(self):
		self.ClickExitButton()
		return TRUE

class ReselectEmpireWindow(SelectEmpireWindow):
	def SelectEmpire(self, id):
		net.SendSelectEmpirePacket(self.empireId[id])
		self.stream.SetCreateCharacterPhase()

	def ClickExitButton(self):
		self.stream.SetSelectCharacterPhase()
