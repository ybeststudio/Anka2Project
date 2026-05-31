if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
import os
import ui
import ime
import snd
import wndMgr
import musicInfo
import serverInfo
import systemSetting
import ServerStateChecker
import localeInfo
import constInfo
import uiCommon
import time
import serverCommandParser
import uiScriptLocale
import _winreg
from _weakref import proxy

if app.ENABLE_RENEWAL_SERVER_LIST:
	import uiToolTip

if app.ENABLE_MOB_DROP_INFO:
	import uiTarget

if app.ENABLE_CLIENT_PERFORMANCE:
	import uiGuild

REG_PATH = r"SOFTWARE\M2Project"

def set_reg(name, value):
	try:
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, REG_PATH)
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(registry_key, name, 0, _winreg.REG_SZ, value)
		_winreg.CloseKey(registry_key)
		return TRUE
	except WindowsError:
		return FALSE

def get_reg(name):
	try:
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_READ)
		value, regtype = _winreg.QueryValueEx(registry_key, name)
		_winreg.CloseKey(registry_key)
		return str(value)
	except WindowsError:
		return None

app.SetGuildMarkPath("test")

class ConnectingDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.eventTimeOver = lambda *arg: None
		self.eventExit = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/ConnectingDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.countdownMessage = self.GetChild("countdown_message")

		except:
			import exception
			exception.Abort("ConnectingDialog.LoadDialog.BindObject")

	def Open(self, waitTime):
		curTime = time.clock()
		self.endTime = curTime + waitTime

		self.Lock()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Unlock()
		self.Hide()

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def SetText(self, text):
		self.message.SetText(text)

	def SetCountDownMessage(self, waitTime):
		self.countdownMessage.SetText("%.0f%s" % (waitTime, localeInfo.SECOND))

	def SAFE_SetTimeOverEvent(self, event):
		self.eventTimeOver = ui.__mem_func__(event)

	def SAFE_SetExitEvent(self, event):
		self.eventExit = ui.__mem_func__(event)

	def OnUpdate(self):
		lastTime = max(0, self.endTime - time.clock())
		if 0 == lastTime:
			self.Close()
			self.eventTimeOver()
		else:
			self.SetCountDownMessage(self.endTime - time.clock())

	def OnPressExitKey(self):
		return TRUE

if app.ENABLE_RENEWAL_SERVER_LIST:
	class ServerListBox(ui.ListBox2):
		FLAGS_PATH = "d:/ymir work/ui/intro/login/flags/"
		STATE_IMAGE_PATH = (
			"",
			FLAGS_PATH + "choise_new.tga",
			FLAGS_PATH + "choise_special.tga",
			FLAGS_PATH + "choise_close.tga"
		)

		STATE_IMAGE_TOOLTIP = (
			"",
			localeInfo.SERVER_STATUS_NEW,
			localeInfo.SERVER_STATUS_SPECIAL,
			localeInfo.SERVER_STATUS_CLOSE
		)

		def __init__(self, layer = "UI"):
			ui.ListBox2.__init__(self, layer)
			self.stateList = []
			self.stateList2 = []
			self.stateDict = {}
			self.stateDict2 = {}
			self.serverFlagDict = {}
			self.toolTip = uiToolTip.ToolTip()

		def ClearItem(self):
			ui.ListBox2.ClearItem(self)
			self.stateList = []
			self.stateList2 = []
			self.stateDict = {}
			self.stateDict2 = {}
			self.serverFlagDict = {}

		def StateImageEventProgress(self, event_type, arg):
			if "mouse_click" == event_type:
				self.SelectItem(arg)

			elif "mouse_over_in" == event_type:
				arglen = len(self.STATE_IMAGE_TOOLTIP[arg])
				pos_x, pos_y = wndMgr.GetMousePosition()

				self.toolTip.ClearToolTip()
				self.toolTip.SetThinBoardSize(11 * arglen)
				self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
				self.toolTip.AppendTextLine(self.STATE_IMAGE_TOOLTIP[arg], 0xffffffff)
				self.toolTip.Show()

			elif "mouse_over_out" == event_type:
				self.toolTip.Hide()

		def GetState(self, line):
			if line >= len(self.itemList) or line < 0:
				return (0, 0)

			if line >= len(self.stateList) or line >= len(self.stateList2):
				return (0, 0)

			return self.stateList[line], self.stateList2[line]

		def _LocateItem(self):
			pos = (80, self.TEMPORARY_PLACE)

			self.showLineCount = 0
			for textLine in self.itemList:
				x, y = self._CalcRenderPos(pos, self.showLineCount)
				if self.serverFlagDict.get(self.showLineCount) != None:
					flagWidth = self.serverFlagDict[self.showLineCount].GetWidth()
					self.serverFlagDict[self.showLineCount].SetPosition(57, y - 2)

					textLine.SetHorizontalAlignCenter()
					textLine.SetPosition(x + flagWidth, y)
				else:
					textLine.SetPosition(x, y)

				textLine.Show()

				if self.stateDict.get(self.showLineCount) != None:
					w, h = textLine.GetTextSize()

					if self.serverFlagDict.get(self.showLineCount) != None:
						flagWidth = self.serverFlagDict[self.showLineCount].GetWidth()

					self.stateDict[self.showLineCount].SetPosition(x + w + 10 + flagWidth, y - 1)

					imageWidth = self.stateDict[self.showLineCount].GetWidth()

					if self.stateDict2.get(self.showLineCount) != None:
						self.stateDict2[self.showLineCount].SetPosition(x + w + imageWidth + 10, y)

				self.showLineCount += 1

		def InsertItem(self, number, text, state = 0, state2 = 0):
			self.keyDict[len(self.itemList)] = number
			self.textDict[len(self.itemList)] = text
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetText(text)
			textLine.Show()

			if self.itemCenterAlign:
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()

			# localeName = app.GetLocale()
			# if localeName == "en": localeName = "uk"

			# serverFlag = ui.ImageBox()
			# serverFlag.SetParent(self)
			# serverFlag.LoadImage("d:/ymir work/ui/intro/login/flags/server_flag_{}.sub".format(localeName))
			# serverFlag.Show()

			# self.serverFlagDict[len(self.itemList)] = serverFlag

			if state != 0:
				imgBox = ui.ExpandedImageBox()
				imgBox.SetParent(self)
				imgBox.LoadImage(self.STATE_IMAGE_PATH[state])
				imgBox.Show()

				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_click", len(self.itemList))
				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_in", state)
				imgBox.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_out", 0)

				self.stateDict[len(self.itemList)] = imgBox

			if state2 != 0:
				imgBox2 = ui.ExpandedImageBox()
				imgBox2.SetParent(self)
				imgBox2.LoadImage(self.STATE_IMAGE_PATH[state2])
				imgBox2.Show()

				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_click", len(self.itemList))
				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_in", state2)
				imgBox2.SetEvent(ui.__mem_func__(self.StateImageEventProgress), "mouse_over_out", 0)

				self.stateDict2[len(self.itemList)] = imgBox2

			self.itemList.append(textLine)
			self.stateList.append(state)
			self.stateList2.append(state2)

			self._LocateItem()
			self._RefreshForm()

class LoginWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		self.loginnedServer = None
		self.loginnedChannel = None
		app.loggined = FALSE

		self.inputDialog = None
		self.connectingDialog = None
		self.stream = stream

		self.isOnSelectServer = FALSE

		self.channelButtonsDict = {}
		self.channelStatusDict = {}
		self.channelStatusTextDict = {}

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.languageBoard = None
			self.languageList = [ None ] * 11

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		ServerStateChecker.Create(self)

		self.loginFailureMsgDict = {
			"ALREADY" : localeInfo.LOGIN_FAILURE_ALREAY,
			"NOID" : localeInfo.LOGIN_FAILURE_NOT_EXIST_ID,
			"WRONGPWD" : localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"FULL" : localeInfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN" : localeInfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR" : localeInfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK" : localeInfo.LOGIN_FAILURE_BLOCK_ID,
			"BESAMEKEY" : localeInfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL" : localeInfo.LOGIN_FAILURE_NOT_AVAIL,
			"NOBILL" : localeInfo.LOGIN_FAILURE_NOBILL,
			"BLKLOGIN" : localeInfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK" : localeInfo.LOGIN_FAILURE_WEB_BLOCK,
		}

		self.loginFailureFuncDict = {
			"WRONGPWD" : self.__DisconnectAndInputPassword,
			"QUIT" : app.Exit,
		}

		if app.ENABLE_RENEWAL_CLIENT_VERSION:
			self.loginFailureMsgDict["WRONGVER"] = localeInfo.LOGIN_FAILURE_WRONG_VERSION
			self.loginFailureFuncDict["WRONGVER"] = app.Exit

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		if not self.__LoadScript("UIScript/loginwindow.py"):
			dbg.TraceError("LoginWindow.Open - __LoadScript Error")
			return

		if app.loggined:
			self.loginFailureFuncDict = {
			"WRONGPWD" : app.Exit,
			"QUIT" : app.Exit,
		}

		if musicInfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		self.CheckAccount()

		ime.AddExceptKey(91)
		ime.AddExceptKey(93)

		self.Show()

		self.__OpenServerBoard()
		self.__OnSelectServer()

		app.ShowCursor()

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["leftTime"] == 0:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2 if constInfo.autoHuntAutoLoginDict["status"] == 1 else 0

	def Close(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		ServerStateChecker.Initialize(self)

		if musicInfo.loginMusic != "" and musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.loginMusic)

		self.idEditLine.SetTabEvent(0)
		self.idEditLine.SetReturnEvent(0)
		self.pwdEditLine.SetReturnEvent(0)
		self.pwdEditLine.SetTabEvent(0)

		self.connectBoard = None
		self.loginBoard = None
		self.idEditLine = None
		self.pwdEditLine = None
		self.inputDialog = None
		self.connectingDialog = None

		self.isOnSelectServer = FALSE

		self.channelButtonsDict = {}
		self.channelStatusDict = {}
		self.channelStatusTextDict = {}

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.languageBoard = None
			self.languageList = [ None ] * 11

		self.KillFocus()
		self.Hide()

		self.stream.popupWindow.Close()
		self.loginFailureFuncDict = None

		ime.ClearExceptKey()

		app.HideCursor()

	def __SaveChannelInfo(self):
		try:
			file = open("channel.inf", "w")
			file.write("%d %d %d" % (self.__GetServerID(), self.__GetChannelID(), self.__GetRegionID()))
		except:
			print "LoginWindow.__SaveChannelInfo - SaveError"

	def __LoadChannelInfo(self):
		try:
			file = open("channel.inf")
			lines = file.readlines()

			if len(lines)>0:
				tokens = lines[0].split()

				selServerID = int(tokens[0])
				selChannelID = int(tokens[1])

				if len(tokens) == 3:
					regionID = int(tokens[2])

				return regionID, selServerID, selChannelID

		except:
			print "LoginWindow.__LoadChannelInfo - OpenError"
			return -1, -1, -1

	def __ExitGame(self):
		app.Exit()

	def SetIDEditLineFocus(self):
		if self.idEditLine != None:
			self.idEditLine.SetFocus()

	def SetPasswordEditLineFocus(self):
		if self.pwdEditLine != None:
			self.pwdEditLine.SetFocus()

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["leftTime"] != 0:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2

	def OnHandShake(self):
		snd.PlaySound("sound/ui/loginok.wav")
		if app.ENABLE_CLIENT_PERFORMANCE:
			self.PopupDisplayMessage(localeInfo.LOGIN_CLIENT_LOADING)
		else:
			self.PopupDisplayMessage(localeInfo.LOGIN_CONNECT_SUCCESS)

	def OnLoginStart(self):
		self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)
		if app.ENABLE_CLIENT_PERFORMANCE:
			net.LoadResourcesInCache()

	def OnLoginFailure(self, error):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		try:
			loginFailureMsg = self.loginFailureMsgDict[error]
		except KeyError:
			loginFailureMsg = localeInfo.LOGIN_FAILURE_UNKNOWN + error

		loginFailureFunc = self.loginFailureFuncDict.get(error, self.SetPasswordEditLineFocus)

		if app.loggined:
			self.PopupNotifyMessage(loginFailureMsg, self.__ExitGame)
		else:
			self.PopupNotifyMessage(loginFailureMsg, loginFailureFunc)

		if app.__AUTO_HUNT__:
			if error == "ALREADY" and constInfo.autoHuntAutoLoginDict["leftTime"] != 0:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2

		snd.PlaySound("sound/ui/loginfail.wav")

	def __DisconnectAndInputID(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.SetIDEditLineFocus()
		net.Disconnect()

	def __DisconnectAndInputPassword(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.SetPasswordEditLineFocus()
		net.Disconnect()

	def __LoadScript(self, fileName):
		import dbg
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.LoadObject")
		try:
			GetObject = self.GetChild
			self.connectBoard = GetObject("ConnectBoard")
			self.loginBoard = GetObject("LoginBoard")
			self.idEditLine = GetObject("ID_EditLine")
			self.pwdEditLine = GetObject("PWD_EditLine")
			self.serverInfo = GetObject("ConnectName")
			self.serverButton = GetObject("ServerButton")
			self.loginButton = GetObject("LoginButton")
			self.exitButton = GetObject("ExitButton")

			for i in xrange(1, 5):
				self.channelButtonsDict[i] = GetObject("Channel%d_Button" % i)
				self.channelStatusDict[i] = GetObject("Channel%d_Status" % i)
				self.channelStatusTextDict[i] = GetObject("Channel%d_Status_Text" % i)

			self.accountData = {
				0 : [[self.GetChild("Delete_0_Button"), GetObject("Save_0_Button"), GetObject("Load_0_Button")], GetObject("Account_0_Text")],
				1 : [[self.GetChild("Delete_1_Button"), GetObject("Save_1_Button"), GetObject("Load_1_Button")], GetObject("Account_1_Text")],
				2 : [[self.GetChild("Delete_2_Button"), GetObject("Save_2_Button"), GetObject("Load_2_Button")], GetObject("Account_2_Text")],
				3 : [[self.GetChild("Delete_3_Button"), GetObject("Save_3_Button"), GetObject("Load_3_Button")], GetObject("Account_3_Text")]
			}

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.languageBoard = GetObject("LanguageBoard")
				self.languageList[0] = GetObject("Language_0_Button")
				self.languageList[1] = GetObject("Language_1_Button")
				self.languageList[2] = GetObject("Language_2_Button")
				self.languageList[3] = GetObject("Language_3_Button")
				self.languageList[4] = GetObject("Language_4_Button")
				self.languageList[5] = GetObject("Language_5_Button")
				self.languageList[6] = GetObject("Language_6_Button")
				self.languageList[7] = GetObject("Language_7_Button")
				self.languageList[8] = GetObject("Language_8_Button")
				self.languageList[9] = GetObject("Language_9_Button")
				self.languageList[10] = GetObject("Language_10_Button")

		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.BindObject")

		self.serverButton.SetEvent(ui.__mem_func__(self.__OnClickServerButton))

		self.loginButton.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.exitButton.SetEvent(ui.__mem_func__(self.__OnClickExitButton))

		self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))

		self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))

		self.listBoxBackground = ui.MakeImageBox(self.loginBoard, "d:/ymir work/ui/intro/login/connect/server_list.png", 76.5, -5)
		self.listBoxBackground.Hide()

		if app.ENABLE_RENEWAL_SERVER_LIST:
			self.listBox = ServerListBox()
			self.listBox.SetParent(self.listBoxBackground)
			self.listBox.SetPosition(0, 0)
			self.listBox.SetTextCenterAlign(0)
			self.listBox.SetRowCount(15)
			self.listBox.SetEvent(ui.__mem_func__(self.OnPressListBoxItem))
		else:
			self.listBox = ui.MakeListBox(self.listBoxBackground, 0, 1, 206, 50, 10, self.OnPressListBoxItem)
		self.listBox.Hide()

		for i in xrange(1, 5):
			self.channelButtonsDict[i].SetEvent(ui.__mem_func__(self.__OnClickChannelButton), i)

		for (key, item) in self.accountData.items():
			if isinstance(item[0], list):
				item[0][0].SetEvent(ui.__mem_func__(self.DeleteAccount), key)
				item[0][1].SetEvent(ui.__mem_func__(self.SaveAccount), key)
				item[0][2].SetEvent(ui.__mem_func__(self.LoadAccount), key)

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.languageList[0].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("cz"))
			self.languageList[1].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("de"))
			self.languageList[2].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("en"))
			self.languageList[3].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("es"))
			self.languageList[4].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("fr"))
			self.languageList[5].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("hu"))
			self.languageList[6].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("it"))
			self.languageList[7].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("pl"))
			self.languageList[8].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("pt"))
			self.languageList[9].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("ro"))
			self.languageList[10].SetEvent(lambda:ui.__mem_func__(self.__QuestionChangeLanguage)("tr"))
			for i in xrange(len(self.languageList)):
				if self.GetIndexByName(app.GetLocaleName()) == i:
					self.languageList[i].Down()
					self.languageList[i].Disable()
				else:
					self.languageList[i].Enable()
					self.languageList[i].SetUp()
			return 1

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def __QuestionChangeLanguage(self, language):
			if app.GetLocaleName() == language:
				return

			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(uiScriptLocale.LANGUAGE_SYSTEM_CHANGE)
			self.questionDialog.SetAcceptEvent(lambda arg = language: self.OnClickLanguageButton(arg))
			self.questionDialog.SetCancelEvent(lambda arg = language: self.__QuestionChangeLanguageCancel(arg))
			self.questionDialog.Open()
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		def __QuestionChangeLanguageCancel(self, language):
			self.OnCloseQuestionDialog()

		def GetIndexByName(self, language):
			langCode = {
				"cz" : 0,
				"de" : 1,
				"en" : 2,
				"es" : 3,
				"fr" : 4,
				"hu" : 5,
				"it" : 6,
				"pl" : 7,
				"pt" : 8,
				"ro" : 9,
				"tr" : 10,
			}

			try:
				return langCode[language]
			except:
				return 1

		def OnClickLanguageButton(self, language):
			#self.Languages = {lang: "65001 " + lang for lang in ["en", "tr"]}
			self.Languages = {
			lang: "65001 " + lang
			for lang in ["cz", "de", "en", "es", "fr", "hu", "it", "pl", "pt", "ro", "tr"]
			}

			file = open("locale.cfg", "w")
			file.write(self.Languages[str(language)])
			file.close()

			self.OnCloseQuestionDialog()

			app.Exit()
			os.popen('start Metin2Release.exe')

		def OnCloseQuestionDialog(self):
			if not self.questionDialog:
				return

			self.questionDialog.Close()
			self.questionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def CheckAccount(self):
		for i in xrange(4):
			if get_reg("id_%d" % i):
				self.accountData[i][1].SetText(str(get_reg("id_%d" % i)))
				self.accountData[i][0][1].Hide()
				self.accountData[i][0][0].Show()
			else:
				self.accountData[i][1].SetText(uiScriptLocale.LOGIN_ACCOUNT_EMPTY)
				self.accountData[i][0][1].Show()
				self.accountData[i][0][0].Hide()

	def DeleteAccount(self, key):
		if get_reg("id_%d" % key):
			set_reg("id_%d" % key, "")
			set_reg("pwd_%d" % key, "")
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_DELETED)
		else:
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_DELETE)

		self.CheckAccount()

	def LoadAccount(self, key):
		if get_reg("id_%d" % key):
			# self.idEditLine.SetInfoMessage("")
			# self.pwdEditLine.SetInfoMessage("")
			self.idEditLine.SetText(str(get_reg("id_%d" % key)))
			self.pwdEditLine.SetText(str(get_reg("pwd_%d" % key)))
			self.pwdEditLine.SetFocus()
		else:
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_NO_DATA)

	def SaveAccount(self, key):
		if get_reg("id_%d" % key):
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_SAVE)
			return

		if self.idEditLine.GetText() == "" or self.pwdEditLine.GetText() == "":
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_SAVE2)
			return

		set_reg("id_%d" % key, self.idEditLine.GetText())
		set_reg("pwd_%d" % key, self.pwdEditLine.GetText())
		self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_SAVED)
		self.CheckAccount()

	def __OnClickChannelButton(self, channelId):
		self.selectedChannel = channelId
		self.__OnClickSelectServerButton(TRUE)

		for key, button in self.channelButtonsDict.items():
			button.SetUp()

		self.channelButtonsDict[channelId].Down()

	def Connect(self, id, pwd):
		if app.ENABLE_RENEWAL_SERVER_LIST:
			regionID = self.__GetRegionID()
			serverID = self.__GetServerID()
			channelID = self.__GetChannelID()

			if (serverInfo.REGION_DICT.has_key(regionID)) and (serverInfo.REGION_DICT[regionID].has_key(serverID)):
				try:
					channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
				except KeyError:
					return

				try:
					state = channelDict[channelID]["state"]
				except KeyError:
					self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
					return

				tokens = serverInfo.REGION_DICT[regionID][serverID].get("state", "NONE").split("|")
				is_server_closed = FALSE

				for idx in xrange(len(tokens)):
					if tokens[idx].strip() == "CLOSE":
						is_server_closed = TRUE

				if is_server_closed == TRUE:
					self.PopupNotifyMessage(localeInfo.SERVER_NOTIFY_CLOSED)
					return

				if state == serverInfo.STATE_DICT[len(serverInfo.STATE_DICT)-1]:
					self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
					return

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_CONNETING, self.SetPasswordEditLineFocus, localeInfo.UI_CANCEL)

		if app.ENABLE_MOB_DROP_INFO:
			uiTarget.MONSTER_INFO_DATA.clear()

		self.stream.SetLoginInfo(id, pwd)
		self.stream.Connect()

	def __OnClickExitButton(self):
		self.stream.SetPhaseWindow(0)

	def __SetServerInfo(self, name):
		net.SetServerInfo(name.strip())
		self.serverInfo.SetText(name)

	def PopupDisplayMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg)

	def PopupNotifyMessage(self, msg, func = 0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def __OnCloseInputDialog(self):
		if self.inputDialog:
			self.inputDialog.Close()
		self.inputDialog = None
		return TRUE

	def OnPressExitKey(self):
		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return TRUE

	def OnExit(self):
		self.stream.popupWindow.Close()

	def OnUpdate(self):
		ServerStateChecker.Update()

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] > 0 and constInfo.autoHuntAutoLoginDict["leftTime"] < app.GetGlobalTimeStamp():
				autoHuntAutoLoginDict = constInfo.autoHuntAutoLoginDict
				self.stream.SetConnectInfo(autoHuntAutoLoginDict["addr"], autoHuntAutoLoginDict["port"], autoHuntAutoLoginDict["addr"], autoHuntAutoLoginDict["account_port"])
				self.Connect(autoHuntAutoLoginDict["id"], autoHuntAutoLoginDict["pwd"])
				constInfo.autoHuntAutoLoginDict["leftTime"] = -1


	def EmptyFunc(self):
		pass

	def __GetRegionID(self):
		return 0

	def __GetServerID(self):
		return self.selectedServer

	def __GetChannelID(self):
		return self.selectedChannel

	def __ServerIDToServerIndex(self, regionID, targetServerID):
		try:
			regionDict = serverInfo.REGION_DICT[regionID]
		except KeyError:
			return -1

		retServerIndex = 0
		for eachServerID, regionDataDict in regionDict.items():
			if eachServerID == targetServerID:
				return retServerIndex

			retServerIndex += 1

		return -1

	def __OpenServerBoard(self):
		loadRegionID, loadServerID, loadChannelID = self.__LoadChannelInfo()

		serverIndex = self.__ServerIDToServerIndex(loadRegionID, loadServerID)

		self.selectedServer = loadServerID

		if app.ENABLE_RENEWAL_SERVER_LIST and serverIndex == -1:
			newIdx = serverInfo.SERVER_STATE_DICT.get("NEW", 0)
			specialIdx = serverInfo.SERVER_STATE_DICT.get("SPECIAL", 0)
			closeIdx = serverInfo.SERVER_STATE_DICT.get("CLOSE", 0)

			serverSelect1st = -1;
			serverSelect2nd = -1;
			serverSelect3rd = -1;

			for idx in xrange(self.listBox.GetItemCount()):
				(state, state2) = self.listBox.GetState(idx)
				if state == newIdx or state2 == newIdx:
					serverSelect1st = idx
					break
				elif serverSelect2nd == -1 and (state == specialIdx or state2 == specialIdx):
					serverSelect2nd = idx
				elif serverSelect3rd == -1 and (state != closeIdx and state2 != closeIdx):
					serverSelect3rd = idx

			if serverSelect1st != -1:
				serverIndex = serverSelect1st
			elif serverSelect2nd != -1:
				serverIndex = serverSelect2nd
			elif serverSelect3rd != -1:
				serverIndex = serverSelect3rd

		if loadChannelID >= 0:
			self.__OnClickChannelButton(loadChannelID)

		if app.loggined:
			self.selectedServer = self.loginnedServer - 1
			self.__OnClickChannelButton(self.loginnedChannel - 1)
			self.__OnClickSelectServerButton(FALSE)
		else:
			self.__OnClickSelectServerButton(FALSE)

	def __OpenLoginBoard(self):
		if app.loggined:
			self.Connect(self.id, self.pwd)
			self.connectBoard.Hide()
			self.loginBoard.Hide()
		elif not self.stream.isAutoLogin:
			self.connectBoard.Show()
			self.loginBoard.Show()

		if self.idEditLine == None:
			self.idEditLine.SetText("")
		if self.pwdEditLine == None:
			self.pwdEditLine.SetText("")

		self.idEditLine.SetFocus()

	def __RefreshServerList(self):
		regionID = self.__GetRegionID()
		if not serverInfo.REGION_DICT.has_key(regionID):
			return

		regionDict = serverInfo.REGION_DICT[regionID]

		visible_index = 1
		if app.ENABLE_RENEWAL_SERVER_LIST:
			for id, regionDataDict in regionDict.items():
				name = regionDataDict.get("name", "noname")
				state = 0
				state2 = 0

				tokens = regionDataDict.get("state", "NONE").split("|")
				if len(tokens) == 1:
					state = serverInfo.SERVER_STATE_DICT.get(tokens[0].strip(), 0)
				elif len(tokens) == 2:
					state = serverInfo.SERVER_STATE_DICT.get(tokens[0].strip(), 0)
					state2 = serverInfo.SERVER_STATE_DICT.get(tokens[1].strip(), 0)

				try:
					server_id = serverInfo.SERVER_ID_DICT[id]
				except:
					server_id = visible_index

				self.listBox.InsertItem(id, "  %02d. %s" % (int(server_id), name), state, state2)

				visible_index += 1
		else:
			for id, regionDataDict in regionDict.items():
				name = regionDataDict.get("name", "noname")

				try:
					server_id = serverInfo.SERVER_ID_DICT[id]
				except:
					server_id = visible_index

				visible_index += 1

	def __OnSelectServer(self):
		self.__OnCloseInputDialog()
		self.__RequestServerStateList()
		self.__RefreshServerStateList()

	def __RequestServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()

		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			return

		ServerStateChecker.Initialize();
		for id, channelDataDict in channelDict.items():
			key = channelDataDict["key"]
			ip = channelDataDict["ip"]
			udp_port = channelDataDict["udp_port"]
			ServerStateChecker.AddChannel(key, ip, udp_port)

		ServerStateChecker.Request()

	def __RefreshServerStateList(self):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except:
			return

		for channelID, channelDataDict in channelDict.items():
			channelName = channelDataDict["name"]
			channelState = channelDataDict["state"]

			if self.channelStatusDict.has_key(channelID):
				self.channelStatusDict[channelID].LoadImage("d:/ymir work/ui/intro/login/channel/%s" % serverInfo.STATE_DICT_IMAGES[channelState])
				self.channelStatusTextDict[channelID].SetText(serverInfo.STATE_DICT_NAME[channelState])

	def __GetChannelName(self, regionID, selServerID, selChannelID):
		try:
			return serverInfo.REGION_DICT[regionID][selServerID]["channel"][selChannelID]["name"]
		except KeyError:
			if 9 == selChannelID:
				return localeInfo.CHANNEL_PVP
			else:
				return localeInfo.CHANNEL_NORMAL % (selChannelID)

	def NotifyChannelState(self, addrKey, state):
		try:
			stateName = serverInfo.STATE_DICT[state]
		except:
			stateName = serverInfo.STATE_NONE

		regionID = int(addrKey/1000)
		serverID = int(addrKey/10) % 100
		channelID = addrKey%10

		try:
			serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["state"] = stateName

			if self.channelStatusDict.has_key(channelID):
				self.channelStatusDict[channelID].LoadImage("d:/ymir work/ui/intro/login/channel/%s" % serverInfo.STATE_DICT_IMAGES[stateName])
				self.channelStatusTextDict[channelID].SetText(stateName)

			self.__RefreshServerStateList()
		except:
			import exception
			exception.Abort(localeInfo.CHANNEL_NOT_FIND_INFO)

	def __OnClickExitServerButton(self):
		self.__OpenLoginBoard()

	def __OnClickSelectServerButton(self, alreadyOpen = FALSE):
		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		channelID = self.__GetChannelID()

		if (not serverInfo.REGION_DICT.has_key(regionID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_REGION)
			return

		if (not serverInfo.REGION_DICT[regionID].has_key(serverID)):
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_SERVER)
			return

		try:
			channelDict = serverInfo.REGION_DICT[regionID][serverID]["channel"]
		except KeyError:
			return

		try:
			state = channelDict[channelID]["state"]
		except KeyError:
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
			return

		if app.ENABLE_RENEWAL_SERVER_LIST:
			tokens = serverInfo.REGION_DICT[regionID][serverID].get("state", "NONE").split("|")
			is_server_closed = FALSE

			for idx in xrange(len(tokens)):
				if tokens[idx].strip() == "CLOSE":
					is_server_closed = TRUE

			if is_server_closed == TRUE:
				self.PopupNotifyMessage(localeInfo.SERVER_NOTIFY_CLOSED)
				return

			if state == serverInfo.STATE_DICT[len(serverInfo.STATE_DICT)-1]:
				self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
				return
		else:
			if state == serverInfo.STATE_DICT[3]:
				self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
				return

		self.__SaveChannelInfo()

		try:
			serverName = serverInfo.REGION_DICT[regionID][serverID]["name"]
			channelName = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["name"]
			addrKey = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["key"]
		except:
			serverName = localeInfo.CHANNEL_EMPTY_SERVER
			channelName = localeInfo.CHANNEL_NORMAL % channelID

		self.__SetServerInfo("%s, %s " % (serverName, channelName))

		try:
			ip = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["ip"]
			tcp_port = serverInfo.REGION_DICT[regionID][serverID]["channel"][channelID]["tcp_port"]
		except:
			import exception
			exception.Abort("LoginWindow.__OnClickSelectServerButton")

		try:
			account_ip = serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["ip"]
			account_port = serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]["port"]
		except:
			account_ip = 0
			account_port = 0

		try:
			markKey = regionID*1000 + serverID*10
			markAddrValue = serverInfo.MARKADDR_DICT[markKey]
			net.SetMarkServer(markAddrValue["ip"], markAddrValue["tcp_port"])
			app.SetGuildMarkPath(markAddrValue["mark"])
			app.SetGuildSymbolPath(markAddrValue["symbol_path"])

		except:
			import exception
			exception.Abort("LoginWindow.__OnClickSelectServerButton")

		self.stream.SetConnectInfo(ip, tcp_port, account_ip, account_port)
		if not alreadyOpen:
			self.__OpenLoginBoard()

	def OnPressListBoxItem(self, idx, name):
		self.selectedServer = int(idx)
		self.__OnClickChannelButton(1)
		self.__OnSelectServer()
		self.__OnClickSelectServerButton(TRUE)

		self.listBoxBackground.Hide()
		self.listBox.Hide()
		self.listBox.ClearItem()

		self.serverButton.SetUpVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_norm.png")
		self.serverButton.SetOverVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_over.png")
		self.serverButton.SetDownVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_down.png")

		self.isOnSelectServer = FALSE

	def __OnClickServerButton(self):
		if self.isOnSelectServer:
			self.listBoxBackground.Hide()
			self.listBox.ClearItem()
			self.listBox.Hide()

			self.serverButton.SetUpVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_norm.png")
			self.serverButton.SetOverVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_over.png")
			self.serverButton.SetDownVisual("d:/ymir work/ui/intro/login/arrow/arrow_down_down.png")

			self.isOnSelectServer = FALSE
		else:
			self.listBoxBackground.Show()
			self.listBox.Show()

			self.listBox.ClearItem()

			for key, value in serverInfo.REGION_DICT[0].iteritems():
				self.listBox.InsertItem(key, value["name"])

			self.listBoxBackground.SetSize(195, 19*len(serverInfo.REGION_DICT[0]))
			self.listBox.SetSize(195, 25*len(serverInfo.REGION_DICT[0]))

			self.serverButton.SetUpVisual("d:/ymir work/ui/intro/login/arrow/arrow_up_norm.png")
			self.serverButton.SetOverVisual("d:/ymir work/ui/intro/login/arrow/arrow_up_over.png")
			self.serverButton.SetDownVisual("d:/ymir work/ui/intro/login/arrow/arrow_up_down.png")

			self.isOnSelectServer = TRUE

	def __OnClickLoginButton(self):
		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()

		if len(id) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_ID, self.SetIDEditLineFocus)
			return

		if len(pwd) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_PASSWORD, self.SetPasswordEditLineFocus)
			return

		self.Connect(id, pwd)

	def SameLogin_OpenUI(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, 0, localeInfo.UI_OK)

	def __LoginDirectly(self, j):
		self.LoadAccount(j-1)
		self.__OnClickLoginButton()

	def OnKeyDown(self, key):
		if app.DIK_F1 == key:
			self.__LoginDirectly(1)
		elif app.DIK_F2 == key:
			self.__LoginDirectly(2)
		elif app.DIK_F3 == key:
			self.__LoginDirectly(3)
		elif app.DIK_F4 == key:
			self.__LoginDirectly(4)
		return TRUE

	if app.ENABLE_CLIENT_PERFORMANCE:
		def BINARY_SetGuildBuildingList(self, obj):
			uiGuild.BUILDING_DATA_LIST = obj
