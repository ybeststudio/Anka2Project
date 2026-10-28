if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
import snd
import wndMgr

import background
import playerSettingModule

import ui
import uiPhaseCurtain
import constInfo
import localeInfo

class PopupDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.CloseEvent = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		PythonScriptLoader = ui.PythonScriptLoader()
		PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")

	def Open(self, Message, event = 0, ButtonName = localeInfo.UI_CANCEL):

		if TRUE == self.IsShow():
			self.Close()

		self.Lock()
		self.SetTop()
		self.CloseEvent = event

		AcceptButton = self.GetChild("accept")
		# AcceptButton.SetText(ButtonName)
		AcceptButton.SetEvent(ui.__mem_func__(self.Close))

		self.GetChild("message").SetText(Message)
		self.Show()

	def Close(self):

		if FALSE == self.IsShow():
			self.CloseEvent = 0
			return

		self.Unlock()
		self.Hide()

		if 0 != self.CloseEvent:
			self.CloseEvent()
			self.CloseEvent = 0

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnIMEReturn(self):
		self.Close()
		return TRUE

class MainStream(object):
	isChrData=0	

	def __init__(self):
		print "NEWMAIN STREAM ----------------------------------------------------------------------------"
		net.SetHandler(self)
		net.SetTCPRecvBufferSize(128*1024)
		net.SetTCPSendBufferSize(4096)
		net.SetUDPRecvBufferSize(4096)

		self.id = ""
		self.pwd = ""
		self.addr = ""
		self.port = 0
		self.account_addr = 0
		self.account_port = 0
		self.slot = 0
		self.isAutoSelect = 0
		self.isAutoLogin = 0

		# self.curtain = 0
		self.curPhaseWindow = 0
		self.newPhaseWindow = 0

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE MAIN STREAM "

	def Destroy(self):
		if self.curPhaseWindow:
			self.curPhaseWindow.Close()
			self.curPhaseWindow = 0

		if self.newPhaseWindow:
			self.newPhaseWindow.Close()
			self.newPhaseWindow = 0

		self.popupWindow.Destroy()
		self.popupWindow = 0

		# self.curtain = 0

	def Create(self):
		self.CreatePopupDialog()

	def SetPhaseWindow(self, newPhaseWindow):
		self.newPhaseWindow = newPhaseWindow
		self.__ChangePhaseWindow()

	def __ChangePhaseWindow(self):
		oldPhaseWindow = self.curPhaseWindow
		newPhaseWindow = self.newPhaseWindow
		self.curPhaseWindow = 0
		self.newPhaseWindow = 0

		if oldPhaseWindow:
			oldPhaseWindow.Close()

		if newPhaseWindow:
			newPhaseWindow.Open()

		self.curPhaseWindow = newPhaseWindow

		if not self.curPhaseWindow:
			app.Exit()

	def CreatePopupDialog(self):
		self.popupWindow = PopupDialog()
		self.popupWindow.LoadDialog()
		self.popupWindow.SetCenterPosition()
		self.popupWindow.Hide()

	def SetLogoPhase(self):
		net.Disconnect()

		import introLogo
		self.SetPhaseWindow(introLogo.LogoWindow(self))

	def SetLoginPhase(self):
		net.Disconnect()

		import constInfo
		if constInfo.ENABLE_UI_DEBUG_WINDOW:
			self.SetPhaseWindow(DebugWindow(self))
		else:
			import introLogin
			self.SetPhaseWindow(introLogin.LoginWindow(self))

	def SameLogin_SetLoginPhase(self):
		net.Disconnect()

		import introLogin
		introInst = introLogin.LoginWindow(self)
		self.SetPhaseWindow(introInst)
		introInst.SameLogin_OpenUI()

	def SetSelectEmpirePhase(self):
		try:
			import introEmpire
			self.SetPhaseWindow(introEmpire.SelectEmpireWindow(self))
		except:
			import exception
			exception.Abort("networkModule.SetSelectEmpirePhase")

	def SetReselectEmpirePhase(self):
		try:
			import introEmpire
			self.SetPhaseWindow(introEmpire.ReselectEmpireWindow(self))
		except:
			import exception
			exception.Abort("networkModule.SetReselectEmpirePhase")

	def SetSelectCharacterPhase(self):
		try:
			localeInfo.LoadLocaleData()
			import introSelect
			self.popupWindow.Close()
			self.SetPhaseWindow(introSelect.SelectCharacterWindow(self))
		except:
			import exception
			exception.Abort("networkModule.SetSelectCharacterPhase")

	def SetCreateCharacterPhase(self):
		try:
			import introCreate
			self.SetPhaseWindow(introCreate.CreateCharacterWindow(self))
		except:
			import exception
			exception.Abort("networkModule.SetCreateCharacterPhase")

	def SetLoadingPhase(self, isWarp = FALSE):
		try:
			class LoadingWindow(ui.Window):
				def __init__(self, stream, isWarp = FALSE):
					ui.Window.__init__(self)
					self.isWarp = isWarp

				def Open(self):
					pass

				def Close(self):
					pass

			self.SetPhaseWindow(LoadingWindow(self, isWarp))
		except:
			import exception
			exception.Abort("networkModule.SetLoadingPhase")

	def SetGamePhase(self):
		try:
			import game
			self.popupWindow.Close()
			self.SetPhaseWindow(game.GameWindow(self))
		except:
			raise
			import exception
			exception.Abort("networkModule.SetGamePhase")

	def Connect(self):
		import constInfo
		if constInfo.KEEP_ACCOUNT_CONNETION_ENABLE:
			net.ConnectToAccountServer(self.addr, self.port, self.account_addr, self.account_port)
		else:
			net.ConnectTCP(self.addr, self.port)

	def SetConnectInfo(self, addr, port, account_addr = 0, account_port = 0):
		self.addr = addr
		self.port = port
		self.account_addr = account_addr
		self.account_port = account_port

		if app.__AUTO_HUNT__:
			import constInfo
			constInfo.autoHuntAutoLoginDict["addr"] = self.addr
			constInfo.autoHuntAutoLoginDict["port"] = self.port
			constInfo.autoHuntAutoLoginDict["account_addr"] = self.account_addr
			constInfo.autoHuntAutoLoginDict["account_port"] = self.account_port

	def GetConnectAddr(self):
		return self.addr

	def SetLoginInfo(self, id, pwd):
		self.id = id
		self.pwd = pwd

		if app.__AUTO_HUNT__:
			import constInfo
			constInfo.autoHuntAutoLoginDict["id"] = self.id
			constInfo.autoHuntAutoLoginDict["pwd"] = self.pwd

		net.SetLoginInfo(id, pwd)

	def CancelEnterGame(self):
		pass

	def SetCharacterSlot(self, slot):
		self.slot = slot

		if app.__AUTO_HUNT__:
			import constInfo
			constInfo.autoHuntAutoLoginDict["slot"] = slot

	def GetCharacterSlot(self):
		return self.slot

	def EmptyFunction(self):
		pass

import dbg, ime, time, ui, constInfo, wndMgr
class DebugWindow(ui.ScriptWindow):
    def __init__(self, stream):
        ui.ScriptWindow.__init__(self)
        net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
        net.SetAccountConnectorHandler(self)
        self.stream=stream
        # extra
        self.autoUpdate = True
        self.lastUpdate = 0
        self.debugUpdate = 0
        self.hideTipUI = False
        self.squares = [[0,0],[0,0]]
        self.mousePress = False

    def __del__(self):
        net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
        net.SetAccountConnectorHandler(0)
        ui.ScriptWindow.__del__(self)

    def LoadDefaultWindow(self):
        self.blackboard = ui.Bar("GAME")#bottom ui
        self.blackboard.AddFlag("attach")
        self.blackboard.SetPosition(0, 0)
        self.blackboard.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
        self.blackboard.SetColor(0xFF7f7f7f)#0xFF000000)
        self.blackboard.Show()
        self.infotips = ui.TextLine()
        self.infotips.SetParent(self.blackboard)
        self.infotips.SetPackedFontColor(0xFFffaaaa)
        self.infotips.SetOutline()
        self.infotips.SetFontName("Arial:24")
        self.infotips.SetText("F5 MANUAL-REFRESH - F6 AUTOMATIC-REFRESH - F7 HIDE TIPS")
        self.infotips.SetPosition(wndMgr.GetScreenWidth() / 2 - self.infotips.GetTextSize()[0] / 2, 20)
        self.infotips.Show()
        self.updatetips = ui.TextLine()
        self.updatetips.SetParent(self.blackboard)
        self.updatetips.SetPackedFontColor(0xFFff6666)
        self.updatetips.SetOutline()
        self.updatetips.SetFontName("Arial:20")
        self.updatetips.SetText("AUTOREFRESH STATUS: {}".format("ON" if self.autoUpdate else "OFF"))
        self.updatetips.SetPosition(wndMgr.GetScreenWidth() / 2 - self.updatetips.GetTextSize()[0] / 2, 40)
        self.updatetips.Show()
        self.debugtips = ui.TextLine()
        self.debugtips.SetParent(self.blackboard)
        self.debugtips.SetPackedFontColor(0xFF66ff66)
        self.debugtips.SetOutline()
        self.debugtips.SetFontName("Arial:20")
        self.debugtips.SetText("ScreenWidth {}, ScreenHeight {}, MouseX {}, MouseY {}")
        self.debugtips.SetPosition(wndMgr.GetScreenWidth() / 2 - self.debugtips.GetTextSize()[0] / 2, 60)
        self.debugtips.Show()
        self.squaretips = ui.TextLine()
        self.squaretips.SetParent(self.blackboard)
        self.squaretips.SetPackedFontColor(0xFF6666ff)
        self.squaretips.SetOutline()
        self.squaretips.SetFontName("Arial:20")
        self.squaretips.SetText("Selected Box: Pos [xxx,xxx],[xxx,xxx] Size [xxx,xxx]")
        self.squaretips.SetPosition(wndMgr.GetScreenWidth() / 2 - self.squaretips.GetTextSize()[0] / 2, 80)
        self.squaretips.Hide()
        self.squarebox = ui.Box("CURTAIN")#top ui
        self.squarebox.AddFlag("attach")
        self.squarebox.SetPosition(0, 0)
        self.squarebox.SetSize(10, 10)
        self.squarebox.SetColor(0xFF6666ff)
        self.squarebox.Hide()
        self.blackboard.OnMouseLeftButtonDown = self.OnMouseLeftButtonDown
        self.blackboard.OnMouseLeftButtonUp = self.OnMouseLeftButtonUp

    def OnMouseLeftButtonDown(self):
        xMouse, yMouse = wndMgr.GetMousePosition()
        xBoard, yBoard = self.blackboard.GetGlobalPosition()
        realX = xMouse-xBoard
        realY = yMouse-yBoard
        if realX < 0 or realX > wndMgr.GetScreenWidth():
            return False
        if realY < 0 or realY > wndMgr.GetScreenHeight():
            return False
        self.squares[0] = [realX, realY]
        self.squares[1] = [0, 0]
        self.mousePress = True

    def OnMouseLeftButtonUp(self):
        def run():
            xMouse, yMouse = wndMgr.GetMousePosition()
            xBoard, yBoard = self.blackboard.GetGlobalPosition()
            realX = xMouse-xBoard
            realY = yMouse-yBoard
            if realX < 0 or realX > wndMgr.GetScreenWidth():
                self.squares[1] = [0,0]
                return False
            if realY < 0 or realY > wndMgr.GetScreenHeight():
                self.squares[1] = [0,0]
                return False
            self.squares[1] = [realX, realY]
            if self.squares[0] == self.squares[1]:
                self.squares[1] = [0,0]
        run()
        self.mousePress = False
        self.RefreshSquareBox()

    def RefreshDebugInfo(self):
        xMouse, yMouse = wndMgr.GetMousePosition()
        xBoard, yBoard = self.blackboard.GetGlobalPosition()
        self.debugtips.SetText("ScreenWidth {}, ScreenHeight {}, X {}, Y {}".format(
            wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight(), xMouse-xBoard, yMouse-yBoard
        ))

    def RefreshSquareBox(self):
        squares = list(self.squares)
        if squares[0] != [0,0] and squares[1] != [0,0]:
            if squares[0] > squares[1]:
                squares[0], squares[1] = squares[1], squares[0]
            sizes = [squares[1][i] - squares[0][i] for i in range(len(squares[0]))]
            self.squarebox.SetPosition(*squares[0])
            self.squarebox.SetSize(*sizes)
            self.squarebox.Show()
            self.squaretips.SetText("Selected Box: Pos {} Des {} Size {}".format(str(squares[0]), str(squares[1]), str(sizes)))
            self.squaretips.Show()
        else:
            self.squarebox.Hide()
            self.squaretips.Hide()

    def LoadWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptData(self, open("DebugWindow.py", "rb").read())
        except SystemExit as e:
            dbg.TraceError("MakeWindow::LoadWindow SystemExit: {}".format(e))
        except Exception as e:
            dbg.TraceError("MakeWindow::LoadWindow Error: {}".format(e))

    def Open(self):
        self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
        self.SetWindowName("MakeWindow")
        # load default ui
        self.LoadDefaultWindow()
        self.LoadWindow()
        self.Show()
        app.ShowCursor()

    def Close(self):
        ime.ClearExceptKey()
        app.HideCursor()

    def OnPressExitKey(self):
        if self.stream.popupWindow:
            self.stream.popupWindow.Close()
        self.stream.SetPhaseWindow(0)
        return True

    def Refresh(self):
        self.ClearDictionary()
        del self.Children
        self.LoadWindow()
        def SetOnMouseChildren(child):
            child.OnMouseLeftButtonDown = self.OnMouseLeftButtonDown
            child.OnMouseLeftButtonUp = self.OnMouseLeftButtonUp
            if hasattr(child, "Children") and isinstance(child.Children, list):
                for child2 in child.Children:
                    SetOnMouseChildren(child2)
        SetOnMouseChildren(self)

    def OnKeyDown(self, key):
        if app.DIK_F5 == key:
            self.Refresh()
        elif app.DIK_F6 == key:
            self.autoUpdate = not self.autoUpdate
            self.updatetips.SetText("AUTOREFRESH STATUS: {}".format("ON" if self.autoUpdate else "OFF"))
            dbg.LogBox("AutoUpdate {}".format("activated" if self.autoUpdate else "disabled"))
        elif app.DIK_F7 == key:
            self.hideTipUI = not self.hideTipUI
            if self.hideTipUI:
                self.infotips.Hide()
                self.updatetips.Hide()
                self.debugtips.Hide()
                self.squaretips.Hide()
                self.squaretips.Hide()
            else:
                self.infotips.Show()
                self.updatetips.Show()
                self.debugtips.Show()
                self.squaretips.Show()
        return True

    def OnUpdate(self):
        if self.autoUpdate:
            if self.lastUpdate < time.clock():
                self.Refresh()
                self.lastUpdate = time.clock()+1
        if self.debugUpdate < time.clock():
            self.debugUpdate = time.clock()+0.05
            self.RefreshDebugInfo()
            if self.mousePress:
                xMouse, yMouse = wndMgr.GetMousePosition()
                xBoard, yBoard = self.blackboard.GetGlobalPosition()
                realX = xMouse-xBoard
                realY = yMouse-yBoard
                self.squares[1] = [realX, realY]
                self.RefreshSquareBox()
        return True