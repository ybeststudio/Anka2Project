if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import item
import event
import constInfo
import localeInfo
import uiToolTip
import uiDungeonTrack
if app.ENABLE_TELEPORT_MAP_WINDOW:
	import uiTeleportBaerim

# Baerim uiTeleport dungeon page (Cython) rebuilt from its runtime dump (Reference/pydump_runtime/dungeonprobe):
# window 720x570, board at x 30, page tabs on the left, dungeon rows 426x77, right panel at x 475.
# Data, settings, entering and the ranking window stay the ones of uiDungeonTrack (same system, older ui).

DUNGEON_PATH = "d:/ymir work/ui/game/teleport_window/dungeon/%s"
COMMON_PATH = "d:/ymir work/ui/game/common/%s"

LABEL_COLOR = 0xffb1a085			# Baerim beige (feedback-baerim-beige-text): row "Lv", status
VALUE_COLOR = 0xffcabdab			# DUNGEON_STATUS_TIME_COLOR
NAME_COLOR = 0xffeee3d2			# row dungeon names, measured on Baerim's screen
INFO_LABEL_COLOR = 0xffc0b09b		# right panel labels (Gerekli Seviye:, Tamamlanmış ...)
INFO_VALUE_COLOR = 0xffcccac8		# right panel values
HEADER_COLOR = 0xffc9c3bc			# right panel titles (dungeon name, Zindan Bileti, İstatistiklerim)
INFO_FONT = "Tahoma:14"			# Baerim draws the info rows a size larger than the default font
AVAILABLE_COLOR = 0xff63d678		# DUNGEON_STATUS_AVAILABLE_COLOR
LEVEL_COLOR = 0xffffffff

ROW_STEP = 75
LIST_HEIGHT = 528

# our dungeons (uiDungeonTrack keys) drawn with Baerim's row art
ROW_IMAGES = {
	5163 : "monkey.png",
	2092 : "baronne.png",
	1093 : "tour.png",
	2493 : "beran.png",
	2598 : "azrael.png",
	6091 : "raza.png",
	6191 : "nemere.png",
	6192 : "jotun.png",
	9018 : "pirate.png",
	9838 : "panda.png",
	20442 : "meley.png",
	6500 : "earth.png",
	6606 : "raza.png",
	6605 : "nemere.png",
	6756 : "snake.png",
	6791 : "meleyice.png",
	6790 : "meleyice.png",
	9501 : "enigma.png",		# SungMa Istilasi: Baerim has no row art for it, the tower picture of Enigma
}

STATE_AVAILABLE = 0
STATE_LEVEL = 1
STATE_COOLDOWN = 2
STATE_DAILY = 3

STATE_COLOR = { STATE_AVAILABLE : "green", STATE_LEVEL : "red", STATE_COOLDOWN : "brown", STATE_DAILY : "gray" }

RESULT_TEXT = {
	1 : "DUNGEON_WINDOW_RESULT_LEVEL",
	2 : "DUNGEON_WINDOW_RESULT_COOLDOWN",
	3 : "DUNGEON_WINDOW_RESULT_DAILY",
	4 : "DUNGEON_WINDOW_RESULT_BUSY",
	5 : "DUNGEON_WINDOW_RESULT_NO_ENTRY",
	6 : "DUNGEON_WINDOW_RESULT_TEST",
	7 : "DUNGEON_WINDOW_RESULT_DAILY",
	8 : "DUNGEON_WINDOW_RESULT_PC_LIMIT",
}

def ShowResult(result, bossIdx):
	key = RESULT_TEXT.get(result)
	if not key:
		return
	import chat
	chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, key))


def _Image(parent, name, x, y):
	image = ui.ImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	image.AddFlag("not_pick")
	image.Show()
	return image

def _Text(parent, x, y, text = "", align = "left", color = None, font = None):
	line = ui.TextLine()
	line.SetParent(parent)
	if font:
		line.SetFontName(font)
	line.SetPosition(x, y)
	if align == "center":
		line.SetHorizontalAlignCenter()
	elif align == "right":
		line.SetHorizontalAlignRight()
	if color:
		line.SetPackedFontColor(color)
	line.SetText(text)
	line.Show()
	return line

def _Button(parent, x, y, norm, over, down, text = ""):
	button = ui.Button()
	button.SetParent(parent)
	button.SetPosition(x, y)
	button.SetUpVisual(norm)
	button.SetOverVisual(over)
	button.SetDownVisual(down)
	if text:
		button.SetText(text)
	button.Show()
	return button

def _Data(mobIdx):
	return uiDungeonTrack.dungeonData[mobIdx]

def GetState(data):
	level = player.GetStatus(player.LEVEL)
	if level < data["levelrange"][0] or level > data["levelrange"][1]:
		return STATE_LEVEL
	if data["server_cooldown"] > app.GetGlobalTimeStamp():
		return STATE_COOLDOWN
	limit = data.get("daily_limit", 0)
	if limit > 0:
		# ENABLE_DUNGEON_LIMIT_RENEWAL: the weekly stack raises the limit, the PC limit is checked first
		limit += data.get("daily_stack", 0)
		if data.get("current_daily_limit_pc", 0) >= limit or data.get("current_daily_limit", 0) >= limit:
			return STATE_DAILY
	return STATE_AVAILABLE


class DungeonRow(ui.Window):
	WIDTH = 426
	HEIGHT = 77

	def __init__(self, owner, mobIdx):
		ui.Window.__init__(self)
		self.owner = owner
		self.mobIdx = mobIdx
		self.isOver = False
		self.state = -1
		self.lastUpdate = 0.0
		self.SetSize(self.WIDTH, self.HEIGHT)

		data = _Data(mobIdx)
		self.background = _Image(self, DUNGEON_PATH % "btn_dj_green.png", 0, 0)
		self.image = _Image(self, DUNGEON_PATH % ("dungeon/" + ROW_IMAGES.get(mobIdx, "tour.png")), 2, 4)

		# Baerim centres the row texts vertically on these points; "Lv" and the level are centred on one column
		self.levelLabel = _Text(self, 15, 30, localeInfo.DUNGEON_WINDOW_LV, "center", LABEL_COLOR)
		self.levelText = _Text(self, 17, 42, str(data["levelrange"][0]), "center", LEVEL_COLOR, "Tahoma:12b")
		self.name = _Text(self, 60, 30, data["name"], "left", NAME_COLOR, "Tahoma:12b")
		self.status = _Text(self, 60, 45, "", "left", LABEL_COLOR)
		for line in (self.levelLabel, self.levelText, self.name, self.status):
			line.SetVerticalAlignCenter()

		self.celtic = _Image(self, DUNGEON_PATH % "celtic_1.png", 401, 29)

		self.upButton = _Button(self, 402, 6, DUNGEON_PATH % "btn_up_norm.png", DUNGEON_PATH % "btn_up_norm.png", DUNGEON_PATH % "btn_up_norm.png")
		self.upButton.SetEvent(ui.__mem_func__(self.__MoveUp))
		self.eyeButton = _Button(self, 402, 31, DUNGEON_PATH % "btn_hide_1.png", DUNGEON_PATH % "btn_hide_2.png", DUNGEON_PATH % "btn_hide_3.png")
		self.eyeButton.SetEvent(ui.__mem_func__(self.__Hide))
		self.downButton = _Button(self, 402, 54, DUNGEON_PATH % "btn_down_norm.png", DUNGEON_PATH % "btn_down_norm.png", DUNGEON_PATH % "btn_down_norm.png")
		self.downButton.SetEvent(ui.__mem_func__(self.__MoveDown))

		owner.BindToolTip(self.upButton, localeInfo.DUNGEON_TRACK_MOVE_UP)
		owner.BindToolTip(self.eyeButton, localeInfo.DUNGEON_TRACK_HIDE)
		owner.BindToolTip(self.downButton, localeInfo.DUNGEON_TRACK_MOVE_DOWN)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.owner = None

	def SetMask(self, window):
		if app.ENABLE_CLIP_MASKING:
			self.SetClippingMaskWindow(window)

	def GetIndex(self):
		return _Data(self.mobIdx)["listIndex"]

	def IsVisibleRow(self):
		return _Data(self.mobIdx)["showStatus"]

	def Refresh(self):
		data = _Data(self.mobIdx)
		state = GetState(data)
		self.state = state

		if state == STATE_AVAILABLE:
			self.status.SetPackedFontColor(AVAILABLE_COLOR)
			self.status.SetText(localeInfo.DUNGEON_WINDOW_AVAILABLE)
			# "available again" whisper of the old window
			if not data["whisper_info"]:
				if data["alarm"]:
					game = constInfo.GetGameInstance()
					if game != None:
						game.BINARY_OnRecvBulkWhisper(localeInfo.DUNGEON_TRACK_WHISPER_MESSAGE % data["name"])
				data["whisper_info"] = True
		elif state == STATE_COOLDOWN:
			self.status.SetPackedFontColor(VALUE_COLOR)
			self.status.SetText(uiDungeonTrack.FormatTime(data["server_cooldown"] - app.GetGlobalTimeStamp()))
		else:
			self.status.SetPackedFontColor(LABEL_COLOR)
			self.status.SetText(localeInfo.DUNGEON_WINDOW_UNAVAILABLE)

		self.__RefreshBackground()

	def __RefreshBackground(self):
		color = STATE_COLOR.get(self.state, "red")
		if _Data(self.mobIdx)["selected"]:
			name = "bg_select_%s.png" % color
		elif self.isOver:
			name = "bg_hover_%s.png" % color
		else:
			name = "btn_dj_%s.png" % color
		self.background.LoadImage(DUNGEON_PATH % name)

	def OnUpdate(self):
		now = app.GetTime()
		if now < self.lastUpdate:
			return
		self.lastUpdate = now + 0.3

		data = _Data(self.mobIdx)
		if self.state == STATE_COOLDOWN:
			left = data["server_cooldown"] - app.GetGlobalTimeStamp()
			if left <= 0:
				self.Refresh()
			else:
				self.status.SetText(uiDungeonTrack.FormatTime(left))

	def OnMouseOverIn(self):
		self.isOver = True
		self.__RefreshBackground()

	def OnMouseOverOut(self):
		self.isOver = False
		self.__RefreshBackground()

	def OnMouseLeftButtonDown(self):
		if self.owner:
			self.owner.SelectDungeon(self.mobIdx)

	def OnRunMouseWheel(self, nLen):
		if self.owner:
			self.owner.ScrollList(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)

	def __Swap(self, goingDown):
		dungeonData = uiDungeonTrack.dungeonData
		visible = sorted([(info["listIndex"], idx) for (idx, info) in dungeonData.items() if info["showStatus"]])
		order = [idx for (listIndex, idx) in visible]
		if self.mobIdx not in order:
			return
		pos = order.index(self.mobIdx)
		other = pos + (1 if goingDown else -1)
		if other < 0 or other >= len(order):
			return
		mine, theirs = dungeonData[self.mobIdx], dungeonData[order[other]]
		(mine["listIndex"], theirs["listIndex"]) = (theirs["listIndex"], mine["listIndex"])
		uiDungeonTrack.SaveSettings()
		self.owner.RefreshList()

	def __MoveUp(self):
		self.__Swap(False)

	def __MoveDown(self):
		self.__Swap(True)

	def __Hide(self):
		_Data(self.mobIdx)["showStatus"] = False
		uiDungeonTrack.SaveSettings()
		self.owner.RefreshList()


class DungeonWindow(ui.Window):
	WIDTH = 720
	HEIGHT = 570

	def __init__(self):
		ui.Window.__init__(self)
		self.rows = {}
		self.scroll = 0
		self.listHeight = 0
		self.toolTip = None
		self.ticketIcons = []
		self.decor = []
		self.autoBuy = False
		self.mapPage = None
		self.page = "DUNGEON"
		self.lastShopClick = 0
		self.isLoaded = False

	def __del__(self):
		ui.Window.__del__(self)

	# ---------------------------------------------------------------- build

	def __Load(self):
		if self.isLoaded:
			return
		self.isLoaded = True
		uiDungeonTrack.LoadSettings()

		self.SetSize(self.WIDTH, self.HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")

		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.SetPosition(30, 0)
		self.board.SetSize(690, 570)
		self.board.SetTitleName(localeInfo.DUNGEON_WINDOW_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.AddFlag("not_pick")
		self.board.Show()

		self.resetButton = _Button(self, 53, 9, DUNGEON_PATH % "btn_refresh_norm.png", DUNGEON_PATH % "btn_refresh_hover.png", DUNGEON_PATH % "btn_refresh_select.png")
		self.resetButton.SetEvent(ui.__mem_func__(self.__ResetHidden))
		self.BindToolTip(self.resetButton, localeInfo.DUNGEON_WINDOW_RESET_HIDDEN)

		# page tabs: teleport map and dungeons (without ENABLE_TELEPORT_MAP_WINDOW the map tab opens the old teleport window)
		if app.ENABLE_TELEPORT_MAP_WINDOW:
			self.mapTab = ui.RadioButton()
			self.mapTab.SetParent(self)
			self.mapTab.SetPosition(0, 30)
			self.mapTab.SetUpVisual(DUNGEON_PATH % "btn_map_norm.png")
			self.mapTab.SetOverVisual(DUNGEON_PATH % "btn_map_hover.png")
			self.mapTab.SetDownVisual(DUNGEON_PATH % "btn_map_select.png")
			self.mapTab.SetEvent(ui.__mem_func__(self.SetPage), "MAP")
			self.mapTab.Show()
		else:
			self.mapTab = _Button(self, 0, 30, DUNGEON_PATH % "btn_map_norm.png", DUNGEON_PATH % "btn_map_hover.png", DUNGEON_PATH % "btn_map_select.png")
			self.mapTab.SetEvent(ui.__mem_func__(self.__OpenTeleport))
		self.BindToolTip(self.mapTab, localeInfo.DUNGEON_WINDOW_TAB_TELEPORT)
		self.dungeonTab = ui.RadioButton()
		self.dungeonTab.SetParent(self)
		self.dungeonTab.SetPosition(0, 64)
		self.dungeonTab.SetUpVisual(DUNGEON_PATH % "btn_dungeon_norm.png")
		self.dungeonTab.SetOverVisual(DUNGEON_PATH % "btn_dungeon_hover.png")
		self.dungeonTab.SetDownVisual(DUNGEON_PATH % "btn_dungeon_select.png")
		self.dungeonTab.Down()
		if app.ENABLE_TELEPORT_MAP_WINDOW:
			self.dungeonTab.SetEvent(ui.__mem_func__(self.SetPage), "DUNGEON")
		self.dungeonTab.Show()
		self.BindToolTip(self.dungeonTab, localeInfo.DUNGEON_WINDOW_TAB_DUNGEON)

		# dungeon list
		self.listWindow = ui.Window()
		self.listWindow.SetParent(self)
		self.listWindow.SetPosition(40, 30)
		self.listWindow.SetSize(426, LIST_HEIGHT)
		self.listWindow.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnListWheel))
		self.listWindow.Show()

		self.scrollBar = uiDungeonTrack.ScrollBarSpecial()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(467, 32)
		self.scrollBar.SetSize(7, 525)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBar))
		self.scrollBar.Show()

		for mobIdx in uiDungeonTrack.dungeonData.keys():
			row = DungeonRow(self, mobIdx)
			row.SetParent(self.listWindow)
			row.SetMask(self.listWindow)
			self.rows[mobIdx] = row

		self.__BuildRightPanel()

		if app.ENABLE_TELEPORT_MAP_WINDOW:
			# Baerim teleportBg: board (8, 30) -> window (38, 30), covers the board inside
			self.mapPage = uiTeleportBaerim.TeleportMapPage(self)
			self.mapPage.SetParent(self)
			self.mapPage.SetPosition(38, 30)
			self.mapPage.EnableClip()
			self.mapPage.Hide()

	def __Keep(self, window):
		self.decor.append(window)
		return window

	def __BuildRightPanel(self):
		x = 475
		keep = self.__Keep
		keep(_Image(self, DUNGEON_PATH % "header_title.png", x, 30))
		keep(_Image(self, DUNGEON_PATH % "desc2_bg.png", x, 66))
		self.title = _Text(self, 592, 41, "", "center", HEADER_COLOR, "Tahoma:14b")

		self.wikiButton = _Button(self, 483, 37, COMMON_PATH % "button_wiki_norm.png", COMMON_PATH % "button_wiki_hover.png", COMMON_PATH % "button_wiki_down.png")
		self.wikiButton.SetEvent(ui.__mem_func__(self.__OpenWiki))
		self.BindToolTip(self.wikiButton, localeInfo.DUNGEON_WINDOW_WIKI)

		labels = (localeInfo.DUNGEON_WINDOW_LEVEL, localeInfo.DUNGEON_WINDOW_COOLDOWN, localeInfo.DUNGEON_WINDOW_DAILY_LIMIT,
				localeInfo.DUNGEON_WINDOW_AVERAGE_TIME, localeInfo.DUNGEON_WINDOW_BEST_TIME)
		self.infoValues = []
		for i in xrange(len(labels)):
			keep(_Text(self, 490, 80 + 20 * i, labels[i], "left", INFO_LABEL_COLOR, INFO_FONT))
			self.infoValues.append(_Text(self, 697, 80 + 20 * i, "", "right", INFO_VALUE_COLOR, INFO_FONT))
		# daily limit: value left of the help mark
		self.infoValues[2].SetPosition(678, 120)
		self.dailyHelp = ui.ImageBox()
		self.dailyHelp.SetParent(self)
		self.dailyHelp.LoadImage("d:/ymir work/ui/pattern/q_mark_01.tga")
		self.dailyHelp.SetPosition(682, 120)
		self.dailyHelp.Show()
		self.dailyHelp.OnMouseOverIn = ui.__mem_func__(self.__ShowDailyHelp)
		self.dailyHelp.OnMouseOverOut = ui.__mem_func__(self.__HideToolTip)

		keep(_Image(self, DUNGEON_PATH % "header_title.png", x, 190))
		keep(_Text(self, 592, 201, localeInfo.DUNGEON_WINDOW_TICKET, "center", HEADER_COLOR, "Tahoma:14b"))
		keep(_Image(self, DUNGEON_PATH % "ticket_bg.png", x, 226))
		self.autoBuyBox = ui.Button()
		self.autoBuyBox.SetParent(self)
		self.autoBuyBox.SetPosition(563, 275)
		self.autoBuyBox.SetEvent(ui.__mem_func__(self.__ToggleAutoBuy))
		self.autoBuyBox.Show()
		self.BindToolTip(self.autoBuyBox, localeInfo.DUNGEON_WINDOW_AUTO_BUY)

		self.shopButton = _Button(self, 660, 253, DUNGEON_PATH % "shop_btn.png", DUNGEON_PATH % "shop_btn.png", DUNGEON_PATH % "shop_btn.png")
		self.shopButton.SetEvent(ui.__mem_func__(self.__OpenShop))
		self.BindToolTip(self.shopButton, localeInfo.DUNGEON_WINDOW_SHOP)

		keep(_Image(self, DUNGEON_PATH % "header_title.png", x, 310))
		keep(_Text(self, 592, 321, localeInfo.DUNGEON_WINDOW_MY_STATS, "center", HEADER_COLOR, "Tahoma:14b"))
		keep(_Image(self, DUNGEON_PATH % "desc_bg.png", x, 346))
		self.statValues = []
		for (i, (label, icon)) in enumerate(((localeInfo.DUNGEON_WINDOW_COMPLETED, "icon_completed.png"),
				(localeInfo.DUNGEON_WINDOW_FASTEST, "icon_time.png"), (localeInfo.DUNGEON_WINDOW_BEST_DPS, "icon_dmg.png"))):
			center = 517 + 75 * i
			keep(_Text(self, center, 361, label, "center", INFO_LABEL_COLOR))
			keep(_Image(self, DUNGEON_PATH % icon, 498 + 75 * i, 376))
			self.statValues.append(_Text(self, center - 2, 411, "0", "center", INFO_VALUE_COLOR))

		warp = (DUNGEON_PATH % "warp_btn_norm.png", DUNGEON_PATH % "warp_btn_hover.png", DUNGEON_PATH % "warp_btn_select.png")
		self.rankingButton = _Button(self, 499, 434, warp[0], warp[1], warp[2], localeInfo.DUNGEON_WINDOW_RANKING)
		self.rankingButton.SetEvent(ui.__mem_func__(self.__OpenRanking))
		self.missionsButton = _Button(self, 605, 434, warp[0], warp[1], warp[2], localeInfo.DUNGEON_WINDOW_MISSIONS)
		self.missionsButton.SetEvent(ui.__mem_func__(self.__OpenMissions))

		keep(_Image(self, DUNGEON_PATH % "action_bg.png", x, 469))
		self.alarmBox = ui.Button()
		self.alarmBox.SetParent(self)
		self.alarmBox.SetPosition(490, 480)
		self.alarmBox.SetEvent(ui.__mem_func__(self.__ToggleAlarm))
		self.alarmBox.Show()
		keep(_Text(self, 515, 482, localeInfo.DUNGEON_WINDOW_NOTIFY, "left", INFO_VALUE_COLOR))

		self.enterButton = _Button(self, 499, 516, warp[0], warp[1], warp[2], localeInfo.DUNGEON_WINDOW_ENTER)
		self.enterButton.SetEvent(ui.__mem_func__(self.__ClickQuest), "ENTER")
		self.BindToolTip(self.enterButton, localeInfo.DUNGEON_WINDOW_ENTER_HELP)
		self.testButton = _Button(self, 605, 516, warp[0], warp[1], warp[2], localeInfo.DUNGEON_WINDOW_TEST)
		self.testButton.SetEvent(ui.__mem_func__(self.__ClickQuest), "TEST")
		self.BindToolTip(self.testButton, localeInfo.DUNGEON_WINDOW_TEST_HELP)

	# ---------------------------------------------------------------- tooltip (Baerim: framed, at the mouse)

	def BindToolTip(self, window, text):
		if isinstance(window, ui.Button):
			window.SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTip), text)
			window.SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))
		else:
			window.OnMouseOverIn = lambda arg = text: self.__ShowToolTip(arg)
			window.OnMouseOverOut = lambda: self.__HideToolTip()

	def __ShowToolTip(self, text):
		if not self.toolTip:
			self.toolTip = uiToolTip.ToolTip(0)
			self.toolTip.HideToolTip()
		self.toolTip.ClearToolTip()
		self.toolTip.toolTipWidth = 0
		for line in text.split("\\n"):
			self.toolTip.AutoAppendTextLine(line, 0xffc1c1c1)
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.ShowToolTip()

	def __ShowDailyHelp(self):
		if app.ENABLE_DUNGEON_LIMIT_RENEWAL:
			self.__ShowDailyHelpRenewal()
			return
		text = localeInfo.DUNGEON_WINDOW_DAILY_LIMIT_HELP
		mobIdx = self.GetSelected()
		if mobIdx != -1:
			left = _Data(mobIdx).get("server_daily_reset", 0) - app.GetGlobalTimeStamp()
			if left > 0:
				text += "\\n" + localeInfo.DUNGEON_WINDOW_DAILY_RESET % uiDungeonTrack.FormatTime(left)
		self.__ShowToolTip(text)

	# Baerim DUNGEON_DAILY_HELP_* tooltip: rules, PC / character limit with the weekly stack, weekly stack info table
	DAILY_HELP_WIDTH = 500
	DAILY_HELP_TABLE_HEADER = 0xFF2D1C11	# the table colours of uiToolTip.AppendShortcutTable (Baerim tables)
	DAILY_HELP_TABLE_BORDER = 0xFF42392F
	DAILY_HELP_TABLE_ROW = 0xFF1F1511
	DAILY_HELP_TABLE_LINE = 0xFF756657

	def __ShowDailyHelpRenewal(self):
		mobIdx = self.GetSelected()
		data = _Data(mobIdx) if mobIdx != -1 else {}
		limit = data.get("daily_limit", 0)
		stack = data.get("daily_stack", 0)

		if not getattr(self, "dailyToolTip", None):
			self.dailyToolTip = uiToolTip.ToolTip(self.DAILY_HELP_WIDTH)
			self.dailyToolTip.HideToolTip()
		tip = self.dailyToolTip
		tip.ClearToolTip()
		tip.toolTipWidth = self.DAILY_HELP_WIDTH

		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_LINE_1, tip.NORMAL_COLOR)
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_LINE_2, tip.NORMAL_COLOR)
		tip.AppendSpace(4)
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_PC_LIMIT.format(data.get("current_daily_limit_pc", 0), limit, stack))
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_CHARACTER_LIMIT.format(data.get("current_daily_limit", 0), limit, stack))
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_BONUS_LIMIT.format(stack))
		tip.AppendHorizontalLine()
		title = tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_WEEKLY_STACK_TITLE, tip.SPECIAL_TITLE_COLOR)
		title.SetFontName("Tahoma:12b")
		tip.AppendSpace(2)
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_LINE_3, tip.NORMAL_COLOR)
		tip.AppendTextLine(localeInfo.DUNGEON_DAILY_HELP_LINE_4, tip.NORMAL_COLOR)
		tip.AppendSpace(6)
		self.__AppendDailyHelpTable(tip, localeInfo.DUNGEON_DAILY_HELP_LINE_5, (
			localeInfo.DUNGEON_DAILY_HELP_RULE_1.format(limit),
			localeInfo.DUNGEON_DAILY_HELP_RULE_2,
			localeInfo.DUNGEON_DAILY_HELP_RULE_3,
		))
		tip.ShowToolTip()

	def __AppendDailyHelpTable(self, tip, header, rows):
		width = tip.toolTipWidth - 20
		left = 10
		top = tip.toolTipHeight

		def bar(x, y, w, h, color):
			window = ui.Bar()
			window.SetParent(tip)
			window.SetPosition(x, y)
			window.SetSize(w, h)
			window.SetColor(color)
			window.Show()
			tip.childrenList.append(window)

		def text(value, y, bold = False):
			line = ui.TextLine()
			line.SetParent(tip)
			line.SetFontName("Tahoma:12b" if bold else tip.defFontName)
			line.SetPackedFontColor(tip.NORMAL_COLOR)
			line.SetText(value)
			line.SetOutline()
			line.SetHorizontalAlignCenter()
			line.SetPosition(left + width / 2, y)
			line.Show()
			tip.childrenList.append(line)

		bar(left, top, width, 24, self.DAILY_HELP_TABLE_HEADER)
		bar(left, top, width, 1, self.DAILY_HELP_TABLE_BORDER)
		bar(left, top + 23, width, 1, self.DAILY_HELP_TABLE_BORDER)
		text(header, top + 5, True)
		for (index, value) in enumerate(rows):
			y = top + 24 + index * 24
			bar(left, y, width, 23, self.DAILY_HELP_TABLE_ROW)
			if index + 1 < len(rows):
				bar(left, y + 23, width, 1, self.DAILY_HELP_TABLE_LINE)
			text(value, y + 5)
		tip.toolTipHeight += 24 + len(rows) * 24 + 8
		tip.ResizeToolTip()

	def __HideToolTip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()
		if getattr(self, "dailyToolTip", None):
			self.dailyToolTip.HideToolTip()

	# ---------------------------------------------------------------- list

	def __VisibleRows(self):
		rows = [row for row in self.rows.values() if row.IsVisibleRow()]
		rows.sort(key = lambda row: row.GetIndex())
		return rows

	def RefreshList(self):
		rows = self.__VisibleRows()
		self.listHeight = len(rows) * ROW_STEP
		self.scroll = max(0, min(self.scroll, max(0, self.listHeight - LIST_HEIGHT)))

		y = -self.scroll
		for row in self.rows.values():
			row.Hide()
		for row in rows:
			row.Refresh()
			row.SetPosition(0, y)
			if y + row.HEIGHT > 0 and y < LIST_HEIGHT:
				row.Show()
			y += ROW_STEP

		if self.listHeight > LIST_HEIGHT:
			self.scrollBar.SetScale(LIST_HEIGHT, self.listHeight)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def ScrollList(self, nLen):
		self.__SetScroll(self.scroll + (-ROW_STEP if nLen > 0 else ROW_STEP))

	def __OnListWheel(self, isDown):
		self.__SetScroll(self.scroll + (ROW_STEP if isDown else -ROW_STEP))

	def __SetScroll(self, value):
		overflow = max(0, self.listHeight - LIST_HEIGHT)
		self.scroll = max(0, min(overflow, int(value)))
		if overflow:
			self.scrollBar.SetPos(float(self.scroll) / overflow)
		self.RefreshList()

	def __OnScrollBar(self):
		overflow = max(0, self.listHeight - LIST_HEIGHT)
		self.scroll = int(self.scrollBar.GetPos() * overflow)
		self.RefreshList()

	def OnMouseWheel(self, nLen):
		self.ScrollList(nLen)
		return True

	# ---------------------------------------------------------------- selection / right panel

	def GetSelected(self):
		for (mobIdx, info) in uiDungeonTrack.dungeonData.items():
			if info["selected"]:
				return mobIdx
		return -1

	def SelectDungeon(self, mobIdx):
		for info in uiDungeonTrack.dungeonData.values():
			info["selected"] = False
		uiDungeonTrack.dungeonData[mobIdx]["selected"] = True
		uiDungeonTrack.SaveSettings()
		self.Refresh()

	def Refresh(self):
		if not self.isLoaded:
			return
		self.RefreshList()

		mobIdx = self.GetSelected()
		if mobIdx == -1:
			rows = self.__VisibleRows()
			if not rows:
				return
			mobIdx = rows[0].mobIdx
			uiDungeonTrack.dungeonData[mobIdx]["selected"] = True
			self.RefreshList()

		data = _Data(mobIdx)
		self.title.SetText(data["name"])
		self.infoValues[0].SetText("%d - %d" % (data["levelrange"][0], data["levelrange"][1]))
		self.infoValues[1].SetText(uiDungeonTrack.FormatTime(data["cooldown"]))
		self.infoValues[2].SetText("%d / %d" % (data.get("current_daily_limit", 0), data.get("daily_limit", 0)))
		self.infoValues[3].SetText(uiDungeonTrack.FormatTime(data.get("server_average_fastest", 0)))
		self.infoValues[4].SetText(uiDungeonTrack.FormatTime(data.get("server_best_fastest", 0)))

		self.statValues[0].SetText(uiDungeonTrack.NumberToDecimalString(data["server_completed"]))
		self.statValues[1].SetText(uiDungeonTrack.FormatTime(data["server_fastest"]))
		self.statValues[2].SetText(uiDungeonTrack.NumberToDecimalString(data.get("server_dps", data.get("server_damage", 0))))

		check = "checkbox2_check.png" if data["alarm"] else "checkbox2_no.png"
		self.alarmBox.SetUpVisual(COMMON_PATH % check)
		self.alarmBox.SetOverVisual(COMMON_PATH % check)
		self.alarmBox.SetDownVisual(COMMON_PATH % check)

		self.__RefreshTickets(data)
		self.__RefreshAutoBuy()
		self.__KeepMapOnTop()

	def __RefreshTickets(self, data):
		for icon in self.ticketIcons:
			icon.Hide()
		self.ticketIcons = []
		tickets = data["ticket"]
		for (i, (vnum, count)) in enumerate(tickets):
			item.SelectItem(vnum)
			icon = ui.ImageBox()
			icon.SetParent(self)
			icon.LoadImage(item.GetIconImageFileName())
			# centred in the slot frame of ticket_bg.png (frame 99..135 x 25..60)
			icon.SetPosition(576 - (len(tickets) - 1 - i) * 37, 252)
			icon.OnMouseOverIn = lambda v = vnum: self.__ShowItemToolTip(v)
			icon.OnMouseOverOut = lambda: self.__HideItemToolTip()
			icon.Show()
			self.ticketIcons.append(icon)
			if count > 1:
				text = _Text(self, 576 - (len(tickets) - 1 - i) * 37 + 22, 252 + 20, str(count))
				text.SetOutline()
				self.ticketIcons.append(text)

	def __ShowItemToolTip(self, vnum):
		interface = constInfo.GetInterfaceInstance()
		if interface and getattr(interface, "tooltipItem", None):
			interface.tooltipItem.SetItemToolTip(vnum)
			interface.tooltipItem.ShowToolTip()

	def __HideItemToolTip(self):
		interface = constInfo.GetInterfaceInstance()
		if interface and getattr(interface, "tooltipItem", None):
			interface.tooltipItem.HideToolTip()

	# ---------------------------------------------------------------- actions (same as the old window)

	def __ClickQuest(self, questMode):
		mobIdx = self.GetSelected()
		if mobIdx == -1:
			return
		net.SendChatPacket("/dungeon_info %s %d" % ("enter" if questMode == "ENTER" else "test", mobIdx))

	def __OpenShop(self):
		mobIdx = self.GetSelected()
		if mobIdx == -1:
			return
		if self.lastShopClick > app.GetGlobalTimeStamp():
			import chat
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DUNGEON_TRACK_SLOW_INFO)
			return
		self.lastShopClick = app.GetGlobalTimeStamp() + 2
		net.SendChatPacket("/open_shop %d" % _Data(mobIdx)["server_shop_idx"])

	def __OpenRanking(self):
		self.__OpenStatWindow(0)

	def __OpenMissions(self):
		self.__OpenStatWindow(1)

	# Baerim "Zindan Istatistikleri": page 0 Siralama, 1 Gorevler
	def __OpenStatWindow(self, pageIdx):
		mobIdx = self.GetSelected()
		if mobIdx == -1:
			return
		if not hasattr(self, "statWindow") or not self.statWindow:
			import uiDungeonStat
			self.statWindow = uiDungeonStat.DungeonStatWindow()
		self.statWindow.Open(mobIdx, pageIdx)

	def LoadServerData(self, mobIdx, rankIdx, cmd):
		if hasattr(self, "logWindow") and self.logWindow:
			self.logWindow.LoadServerData(int(mobIdx), int(rankIdx), cmd)

	def OnStatRank(self, bossIdx, rankIdx, raceIdx, data):
		if hasattr(self, "statWindow") and self.statWindow:
			self.statWindow.OnRanking(int(bossIdx), int(rankIdx), int(raceIdx), data)

	def OnMissionData(self, bossIdx, fastest, dps, completed, claimedMask, data):
		if hasattr(self, "statWindow") and self.statWindow:
			self.statWindow.OnMissions(int(bossIdx), int(fastest), int(dps), int(completed), int(claimedMask), data)

	def OnMissionResult(self, result, bossIdx, no):
		if hasattr(self, "statWindow") and self.statWindow:
			self.statWindow.OnMissionResult(int(result), int(bossIdx), int(no))

	def __ToggleAlarm(self):
		mobIdx = self.GetSelected()
		if mobIdx == -1:
			return
		data = _Data(mobIdx)
		data["alarm"] = not data["alarm"]
		uiDungeonTrack.SaveSettings()
		self.Refresh()

	def __ToggleAutoBuy(self):
		self.autoBuy = not self.autoBuy
		self.__RefreshAutoBuy()

	def __RefreshAutoBuy(self):
		check = "checkbox2_check.png" if self.autoBuy else "checkbox2_no.png"
		self.autoBuyBox.SetUpVisual(COMMON_PATH % check)
		self.autoBuyBox.SetOverVisual(COMMON_PATH % check)
		self.autoBuyBox.SetDownVisual(COMMON_PATH % check)
		self.autoBuyBox.SetTop()
		self.__KeepMapOnTop()

	def __KeepMapOnTop(self):
		# the ticket icons are rebuilt on every refresh after the map page was created, so they draw over it
		# (SetTop only reorders float windows): hide them while the map tab is open
		if app.ENABLE_TELEPORT_MAP_WINDOW and self.mapPage and self.page == "MAP":
			for icon in self.ticketIcons:
				icon.Hide()

	def __ResetHidden(self):
		uiDungeonTrack.ClearSettings()
		self.scroll = 0
		self.Refresh()

	def __OpenWiki(self):
		pass

	def __OpenTeleport(self):
		interface = constInfo.GetInterfaceInstance()
		if interface and hasattr(interface, "OpenWarpWindow"):
			self.Close()
			interface.OpenWarpWindow()

	# ---------------------------------------------------------------- open / close

	def OnUpdate(self):
		for row in self.rows.values():
			if row.IsShow():
				row.OnUpdate()

	def Open(self, page = None):
		self.__Load()
		self.SetCenterPosition()
		self.Refresh()
		self.Show()
		self.SetTop()
		if app.ENABLE_TELEPORT_MAP_WINDOW:
			self.SetPage(page or self.page)

	def SetPage(self, page):
		# Baerim: the map tab draws over the dungeon page (same window, title follows the tab)
		if not app.ENABLE_TELEPORT_MAP_WINDOW or not self.mapPage:
			return
		self.page = page
		if page == "MAP":
			self.mapTab.Down()
			self.dungeonTab.SetUp()
			self.board.SetTitleName(localeInfo.TELEPORT_WINDOW_TITLE)
			self.__HideToolTip()
			self.__HideItemToolTip()
			self.mapPage.Show()
			self.mapPage.SetTop()
			self.mapPage.SetPage(self.mapPage.pageIdx)
			self.__KeepMapOnTop()
		else:
			self.dungeonTab.Down()
			self.mapTab.SetUp()
			self.board.SetTitleName(localeInfo.DUNGEON_WINDOW_TITLE)
			self.mapPage.Hide()
			for icon in self.ticketIcons:
				icon.Show()

	def GetPage(self):
		return self.page

	def Close(self):
		self.__HideToolTip()
		self.__HideItemToolTip()
		if self.mapPage:
			self.mapPage.Hide()
		self.Hide()

	def Destroy(self):
		for row in self.rows.values():
			row.Destroy()
		self.rows = {}
		if self.mapPage:
			self.mapPage.Destroy()
			self.mapPage = None
		if hasattr(self, "logWindow") and self.logWindow:
			self.logWindow.Destroy()
			self.logWindow = None
		if hasattr(self, "statWindow") and self.statWindow:
			self.statWindow.Destroy()
			self.statWindow = None

	def OnPressEscapeKey(self):
		self.Close()
		return True
