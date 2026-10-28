if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiOption

import uiGameOption
import uiScriptLocale
import networkModule
import constInfo
import localeInfo

if app.ENABLE_CHANGE_CHANNEL:
	import chat
	import serverInfo
	import ServerStateChecker

if app.ENABLE_ADVANCED_GAME_OPTIONS:
	import uiAdvancedGameOptions

class SystemDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()

	def __Initialize(self):
		self.interface = None
		self.gameOptionDlg = None
		if app.ENABLE_CHANGE_CHANNEL:
			self.moveChannelDlg = None

	def LoadDialog(self):
		self.__LoadSystemMenu_Default()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def __LoadSystemMenu_Default(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/systemdialog.py")

		self.GetChild("game_option_button").SAFE_SetEvent(self.__ClickGameOptionButton)
		self.GetChild("change_button").SAFE_SetEvent(self.__ClickChangeCharacterButton)
		if app.ENABLE_CHANGE_CHANNEL:
			self.GetChild("movechannel_button").SAFE_SetEvent(self.__ClickMoveChannel)
		self.GetChild("logout_button").SAFE_SetEvent(self.__ClickLogOutButton)
		self.GetChild("exit_button").SAFE_SetEvent(self.__ClickExitButton)
		self.GetChild("instant_exit_button").SAFE_SetEvent(self.__ClickInstantExitButton)
		self.GetChild("cancel_button").SAFE_SetEvent(self.Close)

		if constInfo.IN_GAME_SHOP_ENABLE:
			self.GetChild("mall_button").SAFE_SetEvent(self.__ClickInGameShopButton)

	def Destroy(self):
		self.ClearDictionary()

		if self.gameOptionDlg:
			self.gameOptionDlg.Destroy()

		if app.ENABLE_CHANGE_CHANNEL:
			if self.moveChannelDlg:
				self.moveChannelDlg.Destroy()

		self.__Initialize()

	def OpenDialog(self):
		self.Show()

	def __ClickChangeCharacterButton(self):
		self.Close()

		net.ExitGame()

	def __OnClosePopupDialog(self):
		self.popup = None

	def __ClickLogOutButton(self):
		self.Close()
		net.LogOutGame()

	def __ClickExitButton(self):
		self.Close()
		net.ExitApplication()

	def __ClickInstantExitButton(self):
		app.Exit()

	def __ClickGameOptionButton(self):
		self.Close()

		if not self.gameOptionDlg:
			if app.ENABLE_ADVANCED_GAME_OPTIONS:
				self.gameOptionDlg = uiAdvancedGameOptions.AdvancedGameOptionsWindow()
			else:
				self.gameOptionDlg = uiGameOption.OptionDialog()

		self.gameOptionDlg.Show()

	if app.ENABLE_CHANGE_CHANNEL:
		def __ClickMoveChannel(self):
			self.Close()
			if not self.moveChannelDlg:
				self.moveChannelDlg = MoveChannelWindow()

			self.moveChannelDlg.Show()

	def __ClickInGameShopButton(self):
		self.Close()
		print "rampage_up"

		if app.ENABLE_ITEMSHOP:
			net.SendChatPacket("/in_game_mall")
		else:
			# net.SendChatPacket("/in_game_mall")
			# gift icon hide when click mall icon
			self.wndGiftBox.Hide()
			import event
			constInfo.ITEMSHOP["questCMD"] = 'LOAD#'+str(constInfo.ITEMSHOP['tableUpdate'])
			event.QuestButtonClick(int(constInfo.ITEMSHOP["qid"]))

	def Close(self):
		self.Hide()
		return TRUE

	def OnBlockMode(self, mode):
		uiGameOption.blockMode = mode
		if self.gameOptionDlg:
			self.gameOptionDlg.OnBlockMode(mode)

	def OnChangePKMode(self):
		if self.gameOptionDlg:
			self.gameOptionDlg.OnChangePKMode()

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		def OnChangePickUPMode(self):
			if self.gameOptionDlg:
				self.gameOptionDlg.OnChangePickUPMode()

	def OnPressExitKey(self):
		self.Close()
		return TRUE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

if app.ENABLE_CHANGE_CHANNEL:
	class MoveChannelWindow(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.titleBar = None
			ServerStateChecker.Create(self)
			self.channelButtonList = []
			self.currentChannel = 0
			self.__LoadBoard()
			self.RefreshChannelButtons()
			self.Show()
			self.SetCenterPosition()
			self.RefreshBeforeShow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __LoadBoard(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/MoveChannelDialog.py")
			except:
				import exception
				exception.Abort("MoveChannelWindow.__LoadBoard.LoadScript")

			try:
				self.board = self.GetChild("MoveChannelBoard")
				self.titleBar = self.GetChild("MoveChannelTitle")
				self.blackBoard = self.GetChild("BlackBoard")
				self.okButton = self.GetChild("AcceptButton")
				self.cancelButton = self.GetChild("CancelButton")
			except:
				import exception
				exception.Abort("MoveChannelWindow.__LoadBoard.BindObject")

			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.okButton.SetEvent(ui.__mem_func__(self.ChangeChannel))
			self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
			self.SetCurentChannel()
			self.AddChannels()

		def SetCurentChannel(self):
			try:
				self.currentChannel = int( int(net.GetServerInfo().split(",")[1][-1:]) - 1 )
			except:
				return

		def __GetServerID(self):
			for i in serverInfo.REGION_DICT[0].keys():
				if serverInfo.REGION_DICT[0][i]["name"] == net.GetServerInfo().split(",")[0]:
					serverID = int(i)
					break

			return serverID

		def __GetChannelNumber(self):
			serverID = self.__GetServerID()
			try:
				channelDict = serverInfo.REGION_DICT[0][serverID]["channel"]
			except:
				return

			return len(channelDict)

		def RefreshBeforeShow(self):
			self.__RequestServerStateList()

		def __RequestServerStateList(self):
			try:
				channelDict = serverInfo.REGION_DICT[0][self.__GetServerID()]["channel"]
			except:
				return

			ServerStateChecker.Initialize(self)
			for id, channelDataDict in channelDict.items():
				key = channelDataDict["key"]
				ip = channelDataDict["ip"]
				udp_port = channelDataDict["udp_port"]
				ServerStateChecker.AddChannel(key, ip, udp_port)
			ServerStateChecker.Request()

		def NotifyChannelState(self, addrKey, state):
			try:
				stateName = serverInfo.STATE_DICT[state]
			except:
				stateName = serverInfo.STATE_NONE

			regionID  = int(addrKey / 1000)
			serverID  = int(addrKey / 10) % 100
			channelID = addrKey % 10
			try:
				serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["state"] = stateName
				self.__RefreshChannelStateList()
			except:
				pass

		def AddChannels(self):
			self.SetSize(190,80+30*self.__GetChannelNumber())
			self.board.SetSize(190,80+30*self.__GetChannelNumber())
			self.blackBoard.SetSize(163,7+30*self.__GetChannelNumber())

			for i in xrange(self.__GetChannelNumber()):
				self.channelButtonList.append(ui.MakeRadioButton(self.blackBoard, 7, 7+30*i, "d:/ymir work/ui/game/myshop_deco/", "select_btn_01.sub", "select_btn_02.sub", "select_btn_03.sub"))
				self.channelButtonList[i].SetText(str(serverInfo.REGION_DICT[0][self.__GetServerID()]["channel"][i+1]["name"]))
				self.channelButtonList[i].SetEvent(lambda arg=i: self.SelectChannel(arg))
				self.channelButtonList[i].Show()

		def SelectChannel(self,channel):
			self.currentChannel = channel
			self.RefreshChannelButtons()

		def RefreshChannelButtons(self):
			for i in xrange(self.__GetChannelNumber()):
				if i == self.currentChannel:
					self.channelButtonList[i].Down()
				else:
					self.channelButtonList[i].SetUp()

		def ChangeChannel(self):
			ServerStateChecker.Update()
			channelID = self.currentChannel+1
			channelState = serverInfo.REGION_DICT[0][self.__GetServerID()]["channel"][channelID]["state"]
			if not channelID:
				return

			if channelState == serverInfo.STATE_NONE or channelState == serverInfo.STATE_DICT[0]:
				chat.AppendChat(chat.CHAT_TYPE_INFO , localeInfo.MOVE_CHANNEL_OFFLINE)
				return

			self.Close()
			net.SendChatPacket("/change_channel " + str(channelID))

		def Destroy(self):
			self.ClearDictionary()
			self.Hide()

		def Close(self):
			ServerStateChecker.Initialize(self)
			self.Hide()

		def OnUpdate(self):
			ServerStateChecker.Update()

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE
