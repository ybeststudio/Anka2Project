if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiScriptLocale
import snd
import mouseModule
import constInfo

class WebWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self, "TOP_MOST")

		self.oldPos = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/WebWindow.py")
		except:
			import exception
			exception.Abort("WebWindow.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.titleBar = GetObject("TitleBar")

		except:
			import exception
			exception.Abort("WebWindow.LoadDialog.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.__OnCloseButtonClick))

	def Destroy(self):
		app.HideWebPage()
		self.ClearDictionary()
		self.titleBar = None

	def Open(self, url):
		# self.Refresh()
		# self.Show()
		# self.SetCenterPosition()

		# x, y = self.GetGlobalPosition()
		# sx, sy = x + 10, y + 30
		# ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40

		# app.ShowWebPage(url, (sx, sy, ex, ey))
		app.OpenWebBrowser(url)

	def Close(self):
		app.HideWebPage()
		self.Hide()

	def Clear(self):
		self.Refresh()

	def Refresh(self):
		pass

	def __OnCloseButtonClick(self):
		print "close_web:click_close_button"
		self.Close()

	def OnPressEscapeKey(self):
		print "close_web:esc_key"
		self.Close()
		return TRUE

	def OnUpdate(self):
		newPos = self.GetGlobalPosition()
		if newPos == self.oldPos:
			return

		self.oldPos = newPos

		x, y = newPos
		sx, sy = x + 10, y + 30
		ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40
		app.MoveWebPage((sx, sy, ex, ey))
