if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
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
import uiScriptLocale
import introLogin

if app.ENABLE_MOB_DROP_INFO:
	import uiTarget

if app.ENABLE_CLIENT_PERFORMANCE:
	import uiGuild

# Baerim introLogin (Cython) rebuilt from its runtime dump: constants, method names and layouts are theirs
LOGIN_INTERFACE = "d:/ymir work/ui/login/login_interface/"

# The login screen always uses this server (serverInfo.REGION_DICT[0])
LOGIN_SERVER_ID = 2

ACCOUNT_SLOT_COUNT = 6
ACCOUNT_VISIBLE_COUNT = 5
CHANNEL_TOTAL_COUNT = 6
CHANNEL_VISIBLE_COUNT = 4

LIST_ROW_HEIGHT = 30
LIST_ROW_START_X = 6
LIST_ROW_START_Y = 6

SCROLLBAR_WIDTH = 8
SCROLLBAR_RIGHT_MARGIN = 7

LOGIN_FLAG_TOP = ("en", "cz", "it", "fr", "pt", "hu")
LOGIN_FLAG_BOTTOM = ("ro", "de", "es", "pl", "tr")

# Empty link: the button does nothing
LINK_DISCORD = "https://discord.gg/NXmc6JrwYr"
LINK_HOMEPAGE = "https://www.beststudio.dev/"
LINK_REGISTER = "https://www.beststudio.dev/kayit"
LINK_RECOVER = "https://www.beststudio.dev/giris"
LINK_SUPPORT = ""
LINK_FORUM = ""

# Small screens move the widgets; anything taller keeps the uiscript positions (1080 design)
LOGIN_RESPONSIVE_LAYOUTS = (
	{
		"max_h" : 720,
		"elements" : {
			"logo" : { "x" : 0, "y" : 20, "image" : "logo_small.png", "show" : False },
			"input_user" : { "x" : 0, "y" : 20, "show" : True },
			"input_password" : { "x" : 0, "y" : 90, "show" : True },
			"login_button" : { "x" : 0, "y" : 165, "show" : True },
			"CreateAccount" : { "x" : -80, "y" : 220, "show" : True },
			"ForgotPassword" : { "x" : 80, "y" : 220, "show" : True },
			"Board_Channel" : { "x" : 0, "y" : 250, "show" : True },
			"Board_Account" : { "x" : 0, "y" : 390, "show" : True },
			"Header" : { "x" : 0, "y" : 155, "show" : False },
			"flags" : { "show" : False },
			"exit_game" : { "y" : 10, "show" : True },
			"Discord" : { "x" : 350, "y" : 75, "show" : True },
			"HomepageBtn" : { "x" : 350, "y" : 125, "show" : True },
		},
	},
	{
		"max_h" : 900,
		"elements" : {
			"logo" : { "x" : 0, "y" : 20, "image" : "logo.png", "show" : True },
			"input_user" : { "x" : 0, "y" : 150, "show" : True },
			"input_password" : { "x" : 0, "y" : 220, "show" : True },
			"login_button" : { "x" : 0, "y" : 295, "show" : True },
			"CreateAccount" : { "x" : -80, "y" : 360, "show" : True },
			"ForgotPassword" : { "x" : 80, "y" : 360, "show" : True },
			"Board_Channel" : { "x" : 0, "y" : 400, "show" : True },
			"Board_Account" : { "x" : 0, "y" : 555, "show" : True },
			"Header" : { "x" : 0, "y" : 155, "show" : False },
			"flags" : { "show" : False },
			"exit_game" : { "y" : 10, "show" : True },
			"Discord" : { "x" : 350, "y" : 75, "show" : True },
			"HomepageBtn" : { "x" : 350, "y" : 125, "show" : True },
		},
	},
)

class ScrollbarDyn(ui.Window):
	CORNERS_AND_LINES_COLOR = 0xff605345
	MIDDLE_BAR_COLOR = 0xffc9a475

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")

			self.bar = ui.Bar()
			self.bar.SetParent(self)
			self.bar.SetColor(ScrollbarDyn.MIDDLE_BAR_COLOR)
			self.bar.AddFlag("not_pick")
			self.bar.Show()

		def __del__(self):
			ui.DragButton.__del__(self)

		def SetSize(self, width, height):
			ui.DragButton.SetSize(self, width, height)
			self.bar.SetSize(width, height)

	def __init__(self):
		ui.Window.__init__(self)
		self.scrollEvent = None
		self.pos = 0.0
		self.lines = []

		for i in xrange(4):
			line = ui.Bar()
			line.SetParent(self)
			line.SetColor(self.CORNERS_AND_LINES_COLOR)
			line.AddFlag("not_pick")
			line.Show()
			self.lines.append(line)

		self.middleBar = self.MiddleBar()
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.OnScrollMove))
		self.middleBar.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.scrollEvent = None
		self.middleBar = None
		self.lines = []

	def SetScrollEvent(self, event):
		self.scrollEvent = event

	def SetScale(self, visibleSize, fullSize):
		w, h = self.GetWidth(), self.GetHeight()
		(top, bottom, left, right) = self.lines
		top.SetPosition(0, 0)
		top.SetSize(w, 1)
		bottom.SetPosition(0, h - 1)
		bottom.SetSize(w, 1)
		left.SetPosition(0, 0)
		left.SetSize(1, h)
		right.SetPosition(w - 1, 0)
		right.SetSize(1, h)

		track = h - 4
		barHeight = max(10, int(track * float(visibleSize) / float(max(1, fullSize))))
		self.middleBar.SetSize(w - 4, barHeight)
		self.middleBar.SetRestrictMovementArea(2, 2, w - 4, track)
		self.SetPos(self.pos)

	def __GetTrack(self):
		return max(1, self.GetHeight() - 4 - self.middleBar.GetHeight())

	def SetPos(self, fScale):
		self.pos = max(0.0, min(1.0, fScale))
		self.middleBar.SetPosition(2, 2 + int(self.__GetTrack() * self.pos))

	def GetPos(self):
		return self.pos

	def OnScrollMove(self):
		(x, y) = self.middleBar.GetLocalPosition()
		self.pos = max(0.0, min(1.0, float(y - 2) / float(self.__GetTrack())))
		if self.scrollEvent:
			self.scrollEvent(self.pos)

class LoginWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		app.loggined = FALSE

		self.stream = stream
		self.questionDialog = None
		self.channelRows = []
		self.accountSlots = []
		self.channelScrollbar = None
		self.accountScrollbar = None
		self.channelOffset = 0
		self.accountOffset = 0
		self.channelIDs = []
		self.selectedChannel = 0
		self.deleteSlot = -1
		self.flagButtons = {}

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		ServerStateChecker.Create(self)
		self.__BuildFailureDicts()

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		if not self.__LoadScript("UIScript/loginwindow_baerim.py"):
			dbg.TraceError("LoginWindow.Open - __LoadScript Error")
			return

		self.__PlayLoginMusic()
		self.__InitIME()

		self.__LoadChannels()
		self.__RefreshAccountSlots()
		self.__SetupLanguageFlags()

		self.Show()
		self.idEditLine.SetFocus()
		app.ShowCursor()

	def Close(self):
		ServerStateChecker.Initialize(self)

		if musicInfo.loginMusic != "" and musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.loginMusic)

		self.__CloseQuestionDialog()

		if self.idEditLine:
			self.idEditLine.SetTabEvent(0)
			self.idEditLine.SetReturnEvent(0)
		if self.pwdEditLine:
			self.pwdEditLine.SetTabEvent(0)
			self.pwdEditLine.SetReturnEvent(0)

		if self.channelScrollbar:
			self.channelScrollbar.Destroy()
		if self.accountScrollbar:
			self.accountScrollbar.Destroy()

		self.__DropReferences()

		self.KillFocus()
		self.Hide()

		self.stream.popupWindow.Close()
		self.loginFailureFuncDict = None

		ime.ClearExceptKey()
		app.HideCursor()

	def __DropReferences(self):
		self.idEditLine = None
		self.pwdEditLine = None
		self.boardChannel = None
		self.boardAccount = None
		self.channelRows = []
		self.accountSlots = []
		self.channelScrollbar = None
		self.accountScrollbar = None
		self.flagButtons = {}

	def __BuildFailureDicts(self):
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
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.loginFailureMsgDict["INVLANG"] = localeInfo.LOGIN_FAILURE_INVLANG
			self.loginFailureMsgDict["NOLANG"] = localeInfo.LOGIN_FAILURE_NOLANG

		self.loginFailureFuncDict = {
			"WRONGPWD" : self.__DisconnectAndInputPassword,
			"QUIT" : app.Exit,
		}

		if app.ENABLE_RENEWAL_CLIENT_VERSION:
			self.loginFailureMsgDict["WRONGVER"] = localeInfo.LOGIN_FAILURE_WRONG_VERSION
			self.loginFailureFuncDict["WRONGVER"] = app.Exit

	def __PlayLoginMusic(self):
		if musicInfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

	def __InitIME(self):
		ime.AddExceptKey(91)
		ime.AddExceptKey(93)

	def __LoadScript(self, fileName):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.LoadObject")
			return FALSE

		try:
			self.__BindStaticWidgets()
			self.__BindChannelRows()
			self.__BindAccountSlots()
		except:
			import exception
			exception.Abort("LoginWindow.__LoadScript.BindObject")
			return FALSE

		self.__ApplyResponsiveLayout()
		self.__WireEvents()
		self.__InitListScrollbars()
		return TRUE

	def __BindStaticWidgets(self):
		GetObject = self.GetChild
		self.board = GetObject("board")
		self.idEditLine = GetObject("id")
		self.pwdEditLine = GetObject("pwd")
		self.loginButton = GetObject("login_button")
		self.exitButton = GetObject("exit_game")
		self.boardChannel = GetObject("Board_Channel")
		self.boardAccount = GetObject("Board_Account")

	def __BindChannelRows(self):
		for i in xrange(CHANNEL_TOTAL_COUNT):
			idx = i + 1
			btn = self.GetChild("channel_button_%d" % idx)
			self.channelRows.append({
				"button" : btn,
				"name" : self.GetChild("ch%d_name" % idx),
				"online" : self.GetChild("ch%d_online" % idx),
				"offline" : self.GetChild("ch%d_offline" % idx),
			})

	def __BindAccountSlots(self):
		for i in xrange(ACCOUNT_SLOT_COUNT):
			idx = i + 1
			self.accountSlots.append({
				"button" : self.GetChild("f%d_login_button" % idx),
				"text" : self.GetChild("f%d_account_text" % idx),
				"save" : self.GetChild("f%d_save_button" % idx),
				"delete" : self.GetChild("f%d_delete_button" % idx),
			})

	def __GetChildSafe(self, name):
		return self.GetChild2(name)

	def __GetResponsiveLayout(self, sh):
		for layout in LOGIN_RESPONSIVE_LAYOUTS:
			if sh <= layout["max_h"]:
				return layout
		return None

	def __ApplyResponsiveLayout(self):
		# the board image is 1080 tall: anchor it to the top, a centred one loses its top on smaller screens
		self.board.SetWindowVerticalAlignTop()
		self.board.SetPosition(0, 0)

		layout = self.__GetResponsiveLayout(wndMgr.GetScreenHeight())
		if not layout:
			return

		for (name, data) in layout["elements"].items():
			if name == "flags":
				self.__ShowFlags(data["show"])
				continue

			wnd = self.__GetChildSafe(name)
			if not wnd:
				continue

			if data.has_key("image"):
				wnd.LoadImage(LOGIN_INTERFACE + data["image"])

			(x, y) = wnd.GetLocalPosition()
			wnd.SetPosition(data.get("x", x), data.get("y", y))
			if data["show"]:
				wnd.Show()
			else:
				wnd.Hide()

	def __ShowFlags(self, show):
		for (name, wnd) in self.ElementDictionary.items():
			if not name.startswith("flag_"):
				continue

			if show:
				wnd.Show()
			else:
				wnd.Hide()

	def __WireEvents(self):
		self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))

		self.loginButton.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.exitButton.SetEvent(ui.__mem_func__(self.ExitWindow))

		for i in xrange(len(self.channelRows)):
			self.channelRows[i]["button"].SetEvent(ui.__mem_func__(self.SetChannel), i)

		for i in xrange(len(self.accountSlots)):
			slot = self.accountSlots[i]
			slot["button"].SetEvent(ui.__mem_func__(self.__OnClickAccountSlot), i)
			slot["save"].SetEvent(ui.__mem_func__(self.SaveAccount), i)
			slot["delete"].SetEvent(ui.__mem_func__(self.DeleteAccount), i)

		for (name, link) in (("Discord", LINK_DISCORD), ("HomepageBtn", LINK_HOMEPAGE)):
			btn = self.__GetChildSafe(name)
			if btn:
				btn.SetEvent(ui.__mem_func__(self.OpenLink), link)

		for (name, link) in (("CreateAccount", LINK_REGISTER), ("ForgotPassword", LINK_RECOVER),
							("HomePage", LINK_HOMEPAGE), ("Register", LINK_REGISTER),
							("Support", LINK_SUPPORT), ("Forum", LINK_FORUM)):
			wnd = self.__GetChildSafe(name)
			if wnd:
				wnd.SetOnMouseLeftButtonUpEvent(lambda arg = link: self.OpenLink(arg))

	def __MakeListScrollbar(self, parent, visibleCount, totalCount, onScroll):
		sb = ScrollbarDyn()
		sb.SetParent(parent)
		sb.SetPosition(parent.GetWidth() - SCROLLBAR_RIGHT_MARGIN - SCROLLBAR_WIDTH, LIST_ROW_START_Y)
		sb.SetSize(SCROLLBAR_WIDTH, visibleCount * LIST_ROW_HEIGHT - 2)
		sb.SetScale(visibleCount, totalCount)
		sb.SetScrollEvent(ui.__mem_func__(onScroll))
		return sb

	def __InitListScrollbars(self):
		self.accountScrollbar = self.__MakeListScrollbar(self.boardAccount, ACCOUNT_VISIBLE_COUNT, ACCOUNT_SLOT_COUNT, self.__OnScrollAccount)
		self.accountScrollbar.Show()
		self.__UpdateAccountList()

	def __OnScrollChannel(self, fScale):
		maxRows = max(0, len(self.channelIDs) - CHANNEL_VISIBLE_COUNT)
		self.channelOffset = int(round(fScale * maxRows))
		self.__UpdateChannelList()

	def __OnScrollAccount(self, fScale):
		maxRows = ACCOUNT_SLOT_COUNT - ACCOUNT_VISIBLE_COUNT
		self.accountOffset = int(round(fScale * maxRows))
		self.__UpdateAccountList()

	def __UpdateScrollList(self, rows, count, visibleCount, offset):
		for i in xrange(len(rows)):
			wnd = rows[i]
			if i < offset or i >= min(count, offset + visibleCount):
				wnd.Hide()
				continue

			wnd.SetPosition(LIST_ROW_START_X, LIST_ROW_START_Y + (i - offset) * LIST_ROW_HEIGHT)
			wnd.Show()

	def __UpdateChannelList(self):
		self.__UpdateScrollList([row["button"] for row in self.channelRows], len(self.channelIDs), CHANNEL_VISIBLE_COUNT, self.channelOffset)

	def __UpdateAccountList(self):
		self.__UpdateScrollList([slot["button"] for slot in self.accountSlots], ACCOUNT_SLOT_COUNT, ACCOUNT_VISIBLE_COUNT, self.accountOffset)

	def __WheelScroll(self, scrollbar, length, totalCount, visibleCount, offset):
		maxRows = totalCount - visibleCount
		if not scrollbar or maxRows <= 0:
			return FALSE

		row = offset + (-1 if length > 0 else 1)
		row = max(0, min(maxRows, row))
		scrollbar.SetPos(float(row) / float(maxRows))
		scrollbar.OnScrollMove()
		return TRUE

	def OnRunMouseWheel(self, length):
		if self.boardAccount and self.boardAccount.IsShow() and self.boardAccount.IsInPosition():
			return self.__WheelScroll(self.accountScrollbar, length, ACCOUNT_SLOT_COUNT, ACCOUNT_VISIBLE_COUNT, self.accountOffset)

		if self.boardChannel and self.boardChannel.IsShow() and self.boardChannel.IsInPosition():
			return self.__WheelScroll(self.channelScrollbar, length, len(self.channelIDs), CHANNEL_VISIBLE_COUNT, self.channelOffset)

		return FALSE

	# channels

	def __GetRegionID(self):
		return 0

	def __GetServerID(self):
		return LOGIN_SERVER_ID

	def __GetChannelDict(self):
		try:
			return serverInfo.REGION_DICT[self.__GetRegionID()][self.__GetServerID()]["channel"]
		except KeyError:
			return {}

	def __LoadChannels(self):
		channelDict = self.__GetChannelDict()
		self.channelIDs = sorted(channelDict.keys())[:CHANNEL_TOTAL_COUNT]

		for i in xrange(len(self.channelRows)):
			row = self.channelRows[i]
			row["online"].Hide()
			row["offline"].Show()

		if len(self.channelIDs) > CHANNEL_VISIBLE_COUNT:
			self.channelScrollbar = self.__MakeListScrollbar(self.boardChannel, CHANNEL_VISIBLE_COUNT, len(self.channelIDs), self.__OnScrollChannel)
			self.channelScrollbar.Show()

		self.__UpdateChannelList()
		self.__RequestServerStateList()

		if not self.channelIDs:
			return

		selected = 0
		(regionID, serverID, channelID) = self.__LoadChannelInfo()
		if serverID == self.__GetServerID() and channelID in self.channelIDs:
			selected = self.channelIDs.index(channelID)

		self.SetChannel(selected)

	def __RequestServerStateList(self):
		ServerStateChecker.Initialize()
		channelDict = self.__GetChannelDict()
		for channelID in self.channelIDs:
			data = channelDict[channelID]
			ServerStateChecker.AddChannel(data["key"], data["ip"], data["udp_port"])
		ServerStateChecker.Request()

	def NotifyChannelState(self, key, state):
		try:
			stateName = serverInfo.STATE_DICT[state]
		except KeyError:
			stateName = serverInfo.STATE_NONE

		channelDict = self.__GetChannelDict()
		for i in xrange(len(self.channelIDs)):
			data = channelDict[self.channelIDs[i]]
			if data["key"] != key:
				continue

			data["state"] = stateName
			online = state != 0
			row = self.channelRows[i]
			if online:
				row["online"].Show()
				row["offline"].Hide()
			else:
				row["online"].Hide()
				row["offline"].Show()

	def __LoadChannelInfo(self):
		try:
			tokens = open("channel.inf").readline().split()
			if len(tokens) == 3:
				return int(tokens[2]), int(tokens[0]), int(tokens[1])
		except:
			pass

		return -1, -1, -1

	def __SaveChannelInfo(self, channelID):
		try:
			open("channel.inf", "w").write("%d %d %d" % (self.__GetServerID(), channelID, self.__GetRegionID()))
		except:
			dbg.TraceError("LoginWindow.__SaveChannelInfo - SaveError")

	def SetChannel(self, ch):
		if ch < 0 or ch >= len(self.channelIDs):
			return

		self.selectedChannel = ch
		for i in xrange(len(self.channelRows)):
			if i == ch:
				self.channelRows[i]["button"].Down()
			else:
				self.channelRows[i]["button"].SetUp()

		regionID = self.__GetRegionID()
		serverID = self.__GetServerID()
		channelID = self.channelIDs[ch]
		server = serverInfo.REGION_DICT[regionID][serverID]
		channel = server["channel"][channelID]

		try:
			auth = serverInfo.REGION_AUTH_SERVER_DICT[regionID][serverID]
			accountIP, accountPort = auth["ip"], auth["port"]
		except KeyError:
			accountIP, accountPort = 0, 0

		try:
			markAddr = serverInfo.MARKADDR_DICT[regionID * 1000 + serverID * 10]
			net.SetMarkServer(markAddr["ip"], markAddr["tcp_port"])
			app.SetGuildMarkPath(markAddr["mark"])
			app.SetGuildSymbolPath(markAddr["symbol_path"])
		except KeyError:
			dbg.TraceError("LoginWindow.SetChannel - no mark server for %d" % serverID)

		net.SetServerInfo(("%s, %s" % (server["name"], channel["name"])).strip())
		self.stream.SetConnectInfo(channel["ip"], channel["tcp_port"], accountIP, accountPort)
		self.__SaveChannelInfo(channelID)

	# accounts

	def __GetAccount(self, slot):
		accountID = introLogin.get_reg("id_%d" % slot)
		if not accountID:
			return None

		return (accountID, introLogin.get_reg("pwd_%d" % slot) or "")

	def __RefreshAccountSlots(self):
		for i in xrange(len(self.accountSlots)):
			slot = self.accountSlots[i]
			data = self.__GetAccount(i)
			if data:
				slot["text"].SetText(data[0])
				slot["save"].Hide()
				slot["delete"].Show()
			else:
				slot["text"].SetText(uiScriptLocale.LOGIN_WINDOW_FREE_SLOT)
				slot["save"].Show()
				slot["delete"].Hide()

	def LoadAccount(self, slotIndex):
		data = self.__GetAccount(slotIndex)
		if not data:
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_NO_DATA)
			return FALSE

		self.idEditLine.SetText(data[0])
		self.pwdEditLine.SetText(data[1])
		return TRUE

	def __OnClickAccountSlot(self, slotIndex):
		if self.LoadAccount(slotIndex):
			self.__OnClickLoginButton()

	def SaveAccount(self, slotIndex):
		if self.__GetAccount(slotIndex):
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_SAVE)
			return

		user = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()
		if not user or not pwd:
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_SAVE2)
			return

		introLogin.set_reg("id_%d" % slotIndex, user)
		introLogin.set_reg("pwd_%d" % slotIndex, pwd)
		self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_SAVED)
		self.__RefreshAccountSlots()

	def DeleteAccount(self, slotIndex):
		if not self.__GetAccount(slotIndex):
			self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_CANNOT_DELETE)
			return

		self.deleteSlot = slotIndex
		self.__CloseQuestionDialog()
		self.questionDialog = uiCommon.QuestionDialog("thin")
		self.questionDialog.SetText(uiScriptLocale.LOGIN_ACCOUNT_ASK_DELETE)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptDeleteAccount))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.CancelDeleteAccount))
		self.questionDialog.Open()

	def AcceptDeleteAccount(self):
		slotIndex = self.deleteSlot
		self.CancelDeleteAccount()
		if slotIndex < 0:
			return

		introLogin.set_reg("id_%d" % slotIndex, "")
		introLogin.set_reg("pwd_%d" % slotIndex, "")
		self.PopupNotifyMessage(uiScriptLocale.LOGIN_ACCOUNT_DELETED)
		self.__RefreshAccountSlots()

	def CancelDeleteAccount(self):
		self.deleteSlot = -1
		self.__CloseQuestionDialog()

	def __CloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None

	# languages

	def __SetupLanguageFlags(self):
		cur = app.GetLocaleName()
		for (prefix, codes) in (("flag_top", LOGIN_FLAG_TOP), ("flag_bottom", LOGIN_FLAG_BOTTOM)):
			for lang in codes:
				flag = self.__GetChildSafe("%s_%s" % (prefix, lang))
				if not flag:
					continue

				self.flagButtons[lang] = flag
				flag.SetEvent(ui.__mem_func__(self.__ClickLanguage), lang)
				if lang == cur:
					flag.Down()
					flag.Disable()

	def __ClickLanguage(self, lang):
		if app.GetLocaleName() == lang:
			return

		self.__CloseQuestionDialog()
		self.questionDialog = uiCommon.QuestionDialog("thin")
		self.questionDialog.SetText(uiScriptLocale.LANGUAGE_SYSTEM_CHANGE)
		self.questionDialog.SetAcceptEvent(lambda arg = lang: self.ConfirmRestartClient(arg))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
		self.questionDialog.Open()

	def ConfirmRestartClient(self, lang):
		self.__CloseQuestionDialog()
		try:
			open("locale.cfg", "w").write("65001 %s" % lang)
		except IOError:
			dbg.TraceError("LoginWindow.ConfirmRestartClient - locale.cfg")
			return

		app.ShellExecute(app.GetName())
		app.Exit()

	# connection

	def OpenLink(self, link):
		if link:
			app.OpenWebBrowser(link)	# ShellExecute exits the client (restart helper)

	def __OnClickLoginButton(self):
		uid = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()

		if len(uid) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_ID, self.SetIDEditLineFocus)
			return

		if len(pwd) == 0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_PASSWORD, self.SetPasswordEditLineFocus)
			return

		self.Connect(uid, pwd)

	def Connect(self, id, pwd):
		if not self.channelIDs:
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
			return

		channel = self.__GetChannelDict()[self.channelIDs[self.selectedChannel]]
		if channel["state"] == serverInfo.STATE_DICT[len(serverInfo.STATE_DICT) - 1]:
			self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
			return

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_CONNETING, self.SetPasswordEditLineFocus, localeInfo.UI_CANCEL)

		if app.ENABLE_MOB_DROP_INFO:
			uiTarget.MONSTER_INFO_DATA.clear()

		self.stream.SetLoginInfo(id, pwd)
		self.stream.Connect()

	def SetIDEditLineFocus(self):
		if self.idEditLine:
			self.idEditLine.SetFocus()

	def SetPasswordEditLineFocus(self):
		if self.pwdEditLine:
			self.pwdEditLine.SetFocus()

	def OnHandShake(self):
		snd.PlaySound("sound/ui/loginok.wav")
		self.PopupDisplayMessage(localeInfo.LOGIN_CONNECT_SUCCESS)

	def OnLoginStart(self):
		self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)

	def OnConnectFailure(self):
		# the server (auth) does not answer: closed, maintenance or no network
		snd.PlaySound("sound/ui/loginfail.wav")
		self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.SetPasswordEditLineFocus)

	def OnLoginFailure(self, error):
		msg = self.loginFailureMsgDict.get(error, localeInfo.LOGIN_FAILURE_UNKNOWN + error)
		func = self.loginFailureFuncDict.get(error, self.SetPasswordEditLineFocus)
		self.PopupNotifyMessage(msg, func)
		snd.PlaySound("sound/ui/loginfail.wav")

	def __DisconnectAndInputPassword(self):
		self.SetPasswordEditLineFocus()
		net.Disconnect()

	def PopupDisplayMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg)

	def PopupNotifyMessage(self, msg, func = 0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def SameLogin_OpenUI(self):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_FAILURE_SAMELOGIN, 0, localeInfo.UI_OK)

	def EmptyFunc(self):
		pass

	def ExitWindow(self):
		self.stream.SetPhaseWindow(0)

	def OnPressExitKey(self):
		if self.questionDialog:
			self.__CloseQuestionDialog()
			return TRUE

		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return TRUE

	def OnKeyDown(self, key):
		slotKeys = (app.DIK_F1, app.DIK_F2, app.DIK_F3, app.DIK_F4, app.DIK_F5, app.DIK_F6)
		if key in slotKeys:
			self.__OnClickAccountSlot(slotKeys.index(key))
		return TRUE

	def OnUpdate(self):
		ServerStateChecker.Update()

	if app.ENABLE_CLIENT_PERFORMANCE:
		def BINARY_SetGuildBuildingList(self, obj):
			uiGuild.BUILDING_DATA_LIST = obj
