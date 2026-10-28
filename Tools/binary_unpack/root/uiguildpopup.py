if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import localeInfo
import ime
import uiScriptLocale
import uiCommon

class GuildLandDealVoteDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog__()
		self.acceptEvent = lambda* arg : None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog__(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildLandDealVoteDialog.py")
		except:
			import exception
			exception.Abort("GuildLandDealVoteDialog.LoadDialog.BindObject")

		self.dealguildname = self.GetChild("dealguildname_slot")
		self.text1 = self.GetChild("message1")
		self.text2 = self.GetChild("message2")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.IsARABIC():
				self.text1.SetHorizontalAlignCenter()
				self.text1.SetVerticalAlignCenter()
				self.text1.SetPosition(self.GetWidth()/2,45)
				self.text2.SetHorizontalAlignCenter()
				self.text2.SetVerticalAlignCenter()
				self.text2.SetPosition(self.GetWidth()/2,60)
		self.LeftButton = self.GetChild("Lbutton")

		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.GetChild("Rbutton").SetEvent(ui.__mem_func__(self.Close))
		self.dealguildname.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		self.dealguildname.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.LeftButton.SetEvent(ui.__mem_func__(self.OnAccept))

	def Close(self):
		self.Hide()

	def Open(self):
		self.__LoadDialog__()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def SetText1(self, text1):
		self.text1.SetText(text1)

	def SetText2(self, text2):
		self.text2.SetText(text2)

	def OnAccept(self):
		guildname = self.dealguildname.GetText()
		net.SendGuildVoteLandDeal(guildname)
		self.Close()

	def OnPressEscaptKey(self):
		self.Close()
		return True

class GuildLandDealDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog__()
		self.acceptEvent = lambda *arg : None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog__(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildLandDealDialog.py")
		except:
			import exception
			exception.Abort("GuildLandDealDialog.LoadDialog.BindObject")

		self.dealguildname = self.GetChild("dealguildname_slot")
		self.dealguildmoney = self.GetChild("dealguildmoney_slot")
		self.OkButton = self.GetChild("Lbutton")

		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.GetChild("Rbutton").SetEvent(ui.__mem_func__(self.Close))
		self.dealguildname.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		self.dealguildname.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.dealguildmoney.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		self.dealguildmoney.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.OkButton.SetEvent(ui.__mem_func__(self.OnAccept))

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscaptKey(self):
		self.Close()
		return True

	def OnAccept(self):
		guildname = self.dealguildname.GetText()
		dealmoney = self.dealguildmoney.GetText()
		if(int(dealmoney) > 2000000000):
			return
		net.SendGuildLandDeal(guildname,int(dealmoney))
		self.Close()

class GuildVoteResultDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildResultPopupDialog.py")
		except:
			import exception
			exception.Abort("VoteDialog.LoadDialog.BindObject")

		self.board = self.GetChild("board")
		self.titlebar = self.GetChild("titlename")
		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.message1 = self.GetChild("message1")
		self.message2 = self.GetChild("message2")
		self.message3 = self.GetChild("message3")
		self.message4 = self.GetChild("message4")
		self.message5 = self.GetChild("message5")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.IsARABIC():
				self.message1.SetHorizontalAlignCenter()
				self.message1.SetVerticalAlignCenter()
				self.message1.SetPosition(self.GetWidth()/2,45)
				self.message2.SetHorizontalAlignCenter()
				self.message2.SetVerticalAlignCenter()
				self.message2.SetPosition(self.GetWidth()/2,60)
				self.message3.SetHorizontalAlignCenter()
				self.message3.SetVerticalAlignCenter()
				self.message3.SetPosition(self.GetWidth()/2,75)
				self.message4.SetHorizontalAlignCenter()
				self.message4.SetVerticalAlignCenter()
				self.message4.SetPosition(self.GetWidth()/2,90)
				self.message5.SetHorizontalAlignCenter()
				self.message5.SetVerticalAlignCenter()
				self.message5.SetPosition(self.GetWidth()/2,105)
		self.Button = self.GetChild("Okbutton")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Close()

	def SetText1(self, text):
		self.message1.SetText(text)

	def SetText2(self, text):
		self.message2.SetText(text)

	def SetText3(self, text):
		self.message3.SetText(text)

	def SetText4(self, text):
		self.message4.SetText(text)

	def SetText5(self, text):
		self.message5.SetText(text)

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetTitleBarText(self, text):
		self.titlebar.SetText(text)

	def SetButtonText(self, text):
		self.Button.SetText(text)

	def SetAcceptEvent(self, event):
		self.Button.SetEvent(event)

	def OnPressEscaptKey(self):
		self.Close()
		return True

class GuildVoteDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildVotePopupDialog.py")
		except:
			import exception
			exception.Abort("VoteDialog.LoadDialog.BindObject")

		self.board = self.GetChild("board")
		self.titlebar = self.GetChild("titlename")
		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.message1 = self.GetChild("message1")
		self.message2 = self.GetChild("message2")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.IsARABIC():
				self.message1.SetHorizontalAlignCenter()
				self.message1.SetVerticalAlignCenter()
				self.message1.SetPosition(self.GetWidth()/2,45)
				self.message2.SetHorizontalAlignCenter()
				self.message2.SetVerticalAlignCenter()
				self.message2.SetPosition(self.GetWidth()/2,60)
		self.LeftButton = self.GetChild("Lbutton")
		self.RightButton = self.GetChild("Rbutton")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Close()

	def SetText1(self, text):
		self.message1.SetText(text)

	def SetText2(self, text):
		self.message2.SetText(text)

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetTitleBarText(self, text):
		self.titlebar.SetText(text)

	def SetNormalButton(self):
		self.LeftButton.SetText(uiScriptLocale.OK)
		self.RightButton.SetText(uiScriptLocale.NO)

	def SetAcceptEvent(self, event):
		self.LeftButton.SetEvent(event)

	def SetCancleEvent(self, event):
		self.RightButton.SetEvent(event)

	def SetAcceptText(self, text):
		self.LeftButton.SetText(text)

	def SetCancleText(self, text):
		self.RightButton.SetText(text)

	def OnPressEscaptKey(self):
		self.Close()
		return True

class GuildVoteDialog2(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildVotePopupDialog2.py")
		except:
			import exception
			exception.Abort("GuildVotePopupDialog2.LoadDialog.BindObject")

		self.board = self.GetChild("board")
		self.titlebar = self.GetChild("titlename")
		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.message1 = self.GetChild("message1")
		self.message2 = self.GetChild("message2")
		self.message3 = self.GetChild("message3")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.IsARABIC():
				self.message1.SetHorizontalAlignCenter()
				self.message1.SetVerticalAlignCenter()
				self.message1.SetPosition(self.GetWidth()/2,45)
				self.message2.SetHorizontalAlignCenter()
				self.message2.SetVerticalAlignCenter()
				self.message2.SetPosition(self.GetWidth()/2,60)
				self.message3.SetHorizontalAlignCenter()
				self.message3.SetVerticalAlignCenter()
				self.message3.SetPosition(self.GetWidth()/2,75)
		self.LeftButton = self.GetChild("Lbutton")
		self.RightButton = self.GetChild("Rbutton")
		self.RightButton.SetEvent(ui.__mem_func__(self.Destroy))

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Close()

	def SetText1(self, text):
		self.message1.SetText(text)
	
	def SetText2(self, text):
		self.message2.SetText(text)

	def SetText3(self, text):
		self.message3.SetText(text)

	def SetTitleBarText(self, text):
		self.titlebar.SetText(text)

	def SetNormalButton(self):
		self.LeftButton.SetText(uiScriptLocale.OK)
		self.RightButton.SetText(uiScriptLocale.NO)

	def SetAcceptEvent(self, event):
		self.LeftButton.SetEvent(event)

	def SetCancleEvent(self, event):
		self.RightButton.SetEvent(event)

	def SetAcceptText(self, text):
		self.LeftButton.SetText(text)

	def SetCancleText(self, text):
		self.RightButton.SetText(text)

	def OnPressEscaptKey(self):
		self.Close()
		return True

class GuildPopupDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/GuildPopupDialog.py")
		except:
			import exception
			exception.Abort("VoteDialog.LoadDialog.BindObject")

		self.board = self.GetChild("board")
		self.titlebar = self.GetChild("titlename")
		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.message1 = self.GetChild("message1")
		self.message2 = self.GetChild("message2")
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.IsARABIC():
				self.message1.SetHorizontalAlignCenter()
				self.message1.SetVerticalAlignCenter()
				self.message1.SetPosition(self.GetWidth()/2,45)
				self.message2.SetHorizontalAlignCenter()
				self.message2.SetVerticalAlignCenter()
				self.message2.SetPosition(self.GetWidth()/2,60)
		self.LeftButton = self.GetChild("Lbutton")
		self.LeftButton.SetEvent(ui.__mem_func__(self.__OnAcceptEventButton))
		self.RightButton = self.GetChild("Rbutton")
		self.RightButton.SetEvent(ui.__mem_func__(self.Close))

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.Close()

	def SetText1(self, text):
		self.message1.SetText(text)

	def SetText2(self, text):
		self.message2.SetText(text)

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetTitleBarText(self, text):
		self.titlebar.SetText(text)

	def SetNormalButton(self):
		self.LeftButton.SetText(uiScriptLocale.OK)
		self.RightButton.SetText(uiScriptLocale.NO)

	def SetAcceptEvent(self, event):
		self.acceptEvent = event

	def __OnAcceptEventButton(self):
		self.acceptEvent()
