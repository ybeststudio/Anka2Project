import app
import ui
import net
import grp
import chat
import time
import guild
import player
import wndMgr
import localeInfo
import uiCommon
import uiCharacterDetails

# Baerim "Lonca Basvurusu" (uiGuildApplication): guild list with filters / details, leader settings + application
# list, and the sliding GuildJoinNotification. Server: /guild_app <sub> (ENABLE_GUILD_APPLICATION).

GA_PATH = "d:/ymir work/ui/game/guild_application/"
COMMON_PATH = "d:/ymir work/ui/game/common/"
SEARCH_PATH = "ikashop/sama/private_search/"

BOARD_WIDTH = 622
BOARD_HEIGHT = 335
TITLE_COLOR = 0xffb0f2b6		# reference green captions
TEXT_COLOR = 0xffcebb98
BUTTON_TEXT_COLOR = 0xffcabdab
WHITE_COLOR = 0xffdddddd
NOTIFY_TITLE_COLOR = 0xfff8e7b0

GUILD_LIST_ITEM_HEIGHT = 29
GUILD_LIST_ITEM_SPACING = 28
GUILD_PAGE_SIZE = 50
PAGEBUTTON_NUMBER_SIZE = 5
LIST_VISIBLE_ROWS = 7
APP_VISIBLE_ROWS = 8
DESCRIPTION_LINE_HEIGHT = 13
DESCRIPTION_LINE_WIDTH = 138
DESCRIPTION_MAX_LINES = 4
DESCRIPTION_MAX_LEN = 100
CLICK_LIMIT_TIME = 3

MODE_MEMBER = 0
MODE_LEADER = 1

g_interface = None	# set by interfaceModule (the guild window "Lonca Listesi" button opens the window through it)

STATUS_OPEN = 0
STATUS_CLOSED = 1
STATUS_APPROVAL = 2

LANGUAGE_ORDER = ["ALL", "EN", "PT", "ES", "FR", "DE", "RO", "PL", "IT", "CZ", "HU", "TR"]
LANGUAGE_DIC = {"ALL" : "", "EN" : "English", "PT" : "Portuguese", "ES" : "Spanish", "FR" : "French", "DE" : "German",
	"RO" : "Romanian", "PL" : "Polish", "IT" : "Italian", "CZ" : "Czech", "HU" : "Hungarian", "TR" : "Turkish"}


def T_ALL():
	return localeInfo.GUILD_APPLICATION_ALL


def GuildTypeName(status):
	return {STATUS_OPEN : localeInfo.GUILD_APPLICATION_STATUS_OPEN,
		STATUS_CLOSED : localeInfo.GUILD_APPLICATION_STATUS_CLOSED,
		STATUS_APPROVAL : localeInfo.GUILD_APPLICATION_STATUS_APPROVAL}.get(status, "")


def LangName(langID):
	if langID <= 0 or langID >= len(LANGUAGE_ORDER):
		return T_ALL()
	return LANGUAGE_DIC[LANGUAGE_ORDER[langID]]


def EmpireName(empire):
	return {1 : localeInfo.EMPIRE_A, 2 : localeInfo.EMPIRE_B, 3 : localeInfo.EMPIRE_C}.get(empire, T_ALL())


def JobName(job):
	return {0 : localeInfo.JOB_WARRIOR, 1 : localeInfo.JOB_ASSASSIN, 2 : localeInfo.JOB_SURA, 3 : localeInfo.JOB_SHAMAN, 4 : localeInfo.JOB_WOLFMAN}.get(job, "")


def Decode(text):
	text = text.replace("\x1f", " ")
	if text.startswith("#"):
		text = text[1:]
	return text


# ------------------------------------------------------------------ small builders
def _Keep(parent, window):
	# a ui window dies with its python object: keep every built child alive on its parent
	keep = getattr(parent, "_keepChildren", None)
	if keep is None:
		keep = []
		parent._keepChildren = keep
	keep.append(window)
	return window


def _Image(parent, path, x, y, pick = False, attach = True):
	image = ui.ExpandedImageBox()
	image.SetParent(parent)
	image.LoadImage(path)
	image.SetPosition(x, y)
	if pick and attach:
		image.AddFlag("attach")		# panel background: dragging it moves the window
	elif not pick:
		image.AddFlag("not_pick")
	image.Show()
	return _Keep(parent, image)


def _Text(parent, text, x, y, color = TEXT_COLOR, center = False, font = None):
	textLine = ui.TextLine()
	textLine.SetParent(parent)
	if font:
		textLine.SetFontName(font)
	if center:
		textLine.SetHorizontalAlignCenter()
	textLine.SetPackedFontColor(color)
	textLine.SetText(text)
	textLine.SetPosition(x, y)
	textLine.Show()
	return _Keep(parent, textLine)


def _Button(parent, prefix, x, y, text = "", disable = False, path = GA_PATH, normName = "norm", overName = "hover", downName = "select"):
	button = ui.Button()
	button.SetParent(parent)
	button.SetUpVisual(path + "%s_%s.png" % (prefix, normName))
	button.SetOverVisual(path + "%s_%s.png" % (prefix, overName))
	button.SetDownVisual(path + "%s_%s.png" % (prefix, downName))
	if disable:
		button.SetDisableVisual(path + "%s_disable.png" % prefix)
	button.SetPosition(x, y)
	if text:
		button.SetText(text)
		button.SetTextColor(BUTTON_TEXT_COLOR)
	button.Show()
	return _Keep(parent, button)


class DropDown(ui.Window):
	"""Option list under a select button (reference IkarusShopComboBox: 106x24 rows, wheel / scrollbar when long)."""
	ROW_HEIGHT = 24
	ROW_WIDTH = 106

	def __init__(self, parent, x, y, items, visibleRows, event):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.items = items
		self.event = event
		self.offset = 0
		self.visibleRows = min(visibleRows, len(items))
		self.hasScroll = len(items) > self.visibleRows
		width = self.ROW_WIDTH + (10 if self.hasScroll else 5)
		height = self.ROW_HEIGHT * self.visibleRows
		self.SetSize(width, height)
		self.SetPosition(x, y)

		self.board = ui.Board()
		self.board.SetParent(self)
		self.board.SetSize(width + 6, max(height + 4, 64))
		self.board.SetPosition(-2, -2)
		self.board.Show()

		self.rows = []
		for index in xrange(self.visibleRows):
			button = ui.Button()
			button.SetParent(self)
			button.SetUpVisual(SEARCH_PATH + "filter/option_normal.png")
			button.SetOverVisual(SEARCH_PATH + "filter/option_over.png")
			button.SetDownVisual(SEARCH_PATH + "filter/option_down.png")
			button.SetPosition(0, index * self.ROW_HEIGHT)
			button.SetEvent(ui.__mem_func__(self.__OnClickRow), index)
			button.Show()
			self.rows.append(button)

		self.scrollBar = None
		if self.hasScroll:
			self.scrollBar = uiCharacterDetails.BaerimScrollBar()
			self.scrollBar.SetParent(self)
			self.scrollBar.SetPosition(self.ROW_WIDTH - 2, 0)
			self.scrollBar.SetScrollBarSize(height)
			self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
			self.scrollBar.Show()
		self.__Refresh()
		self.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def __Refresh(self):
		for (index, button) in enumerate(self.rows):
			button.SetText(self.items[self.offset + index][1])

	def __OnClickRow(self, index):
		(value, text) = self.items[self.offset + index]
		self.Close()
		if self.event:
			self.event(value, text)

	def __OnScroll(self):
		maxOffset = len(self.items) - self.visibleRows
		self.offset = int(round(maxOffset * self.scrollBar.GetPos()))
		self.__Refresh()

	def Scroll(self, delta):
		if not self.hasScroll:
			return
		maxOffset = len(self.items) - self.visibleRows
		self.offset = max(0, min(maxOffset, self.offset - delta))
		self.scrollBar.SetPos(float(self.offset) / maxOffset, False)
		self.__Refresh()

	def Open(self):
		self.isOpen = True
		self.Show()
		self.SetTop()

	def Close(self):
		self.isOpen = False
		self.Hide()

	def Toggle(self):
		if self.isOpen:
			self.Close()
		else:
			self.Open()

	isOpen = False

	def IsIn(self):
		(mx, my) = wndMgr.GetMousePosition()
		(x, y) = self.GetGlobalPosition()
		return x <= mx < x + self.GetWidth() and y <= my < y + self.GetHeight()


class SelectBox(ui.Window):
	"""select_input button (111x22) with the current choice and a DropDown under it."""

	def __init__(self, parent, x, y, items, visibleRows, event, path = GA_PATH):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(111, 22)
		self.event = event
		self.value = items[0][0]
		self.button = _Button(self, "select_input", 0, 0, path = path)
		self.button.SetEvent(ui.__mem_func__(self.__OnClick))
		self.text = _Text(self.button, items[0][1], 8, 5, WHITE_COLOR)
		self.text.AddFlag("not_pick")
		self.dropDown = None
		self.dropItems = items
		self.dropRows = visibleRows
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def CreateDropDown(self, parent, x, y):
		# created by the window after every other child so it is drawn on top
		self.dropDown = DropDown(parent, x, y, self.dropItems, self.dropRows, ui.__mem_func__(self.__OnSelect))

	def __OnClick(self):
		# read the state first: closing the other lists must not reopen this one
		wasOpen = self.dropDown and self.dropDown.isOpen
		if self.closeOthers:
			self.closeOthers(self)
		if self.dropDown:
			if wasOpen:
				self.dropDown.Close()
			else:
				self.dropDown.Open()

	def __OnSelect(self, value, text):
		self.SetValue(value, text)
		if self.event:
			self.event(value)

	def SetValue(self, value, text = None):
		self.value = value
		if text is None:
			for (v, t) in self.dropItems:
				if v == value:
					text = t
		self.text.SetText(text or "")

	def GetValue(self):
		return self.value

	closeOthers = None


class GuildRow(ui.RadioButton):
	def __init__(self, parent):
		ui.RadioButton.__init__(self)
		self.SetParent(parent)
		self.SetUpVisual(GA_PATH + "list_item.png")
		self.SetOverVisual(GA_PATH + "list_item_hover.png")
		self.SetDownVisual(GA_PATH + "list_item_select.png")
		self.markBox = ui.MarkBox()
		self.markBox.SetParent(self)
		self.markBox.Load()
		self.markBox.SetPosition(9, 8)
		self.markBox.AddFlag("not_pick")
		self.markBox.Show()
		self.nameText = _Text(self, "", 32, 0)
		self.levelText = _Text(self, "", 139, 0, center = True)
		self.skillText = _Text(self, "", 204, 0, center = True)
		for textLine in (self.nameText, self.levelText, self.skillText):
			textLine.SetVerticalAlignCenter()
			textLine.SetWindowVerticalAlignCenter()
			textLine.AddFlag("not_pick")
		self.empireImage = _Image(self, GA_PATH + "empire_1.png", 255, 7)
		self.gid = 0

	def __del__(self):
		ui.RadioButton.__del__(self)

	def SetData(self, data):
		self.gid = data["gid"]
		self.markBox.SetIndex(data["gid"])
		self.nameText.SetText(data["name"])
		self.levelText.SetText(str(data["level"]))
		self.skillText.SetText(str(data["ladder"]))
		if 1 <= data["empire"] <= 3:
			self.empireImage.LoadImage(GA_PATH + "empire_%d.png" % data["empire"])
			self.empireImage.Show()
		else:
			self.empireImage.Hide()


class ApplicationRow(ui.Window):
	def __init__(self, parent, approveEvent, rejectEvent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetSize(481, GUILD_LIST_ITEM_HEIGHT)
		self.background = _Image(self, GA_PATH + "list_item_application.png", 0, 0, True)
		self.nameText = _Text(self, "", 12, 7)
		self.levelText = _Text(self, "", 160, 7, center = True)
		self.jobText = _Text(self, "", 225, 7, center = True)
		self.timeText = _Text(self, "", 322, 7, center = True)
		self.approveButton = _Button(self, "btn_accept", 372, 4)
		self.rejectButton = _Button(self, "btn_reject", 426, 4)
		self.approveButton.SetEvent(lambda : approveEvent(self.pid))
		self.rejectButton.SetEvent(lambda : rejectEvent(self.pid))
		self.pid = 0
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def SetData(self, data, timeText):
		self.pid = data["pid"]
		self.nameText.SetText(data["name"])
		self.levelText.SetText(str(data["level"]))
		self.jobText.SetText(JobName(data["job"]))
		self.timeText.SetText(timeText)


class GuildApplicationWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.mode = MODE_MEMBER
		self.guilds = []				# all rows from the server
		self.filtered = []
		self.receiving = []
		self.applications = []
		self.receivingApps = []
		self.settings = None			# (gid, status, lang, minLevel, desc) of the own guild
		self.currentGuildPage = 1
		self.listOffset = 0
		self.appOffset = 0
		self._selectedGuildId = 0
		self.lastRequestTime = 0
		self.selectBoxes = []
		self.popup = None
		self.__LoadWindow()

	def __del__(self):
		ui.Window.__del__(self)

	# ------------------------------------------------------------------ layout
	def __LoadWindow(self):
		self.SetSize(BOARD_WIDTH + 30, BOARD_HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetCenterPosition()

		self.memberButton = ui.RadioButton()
		self.memberButton.SetParent(self)
		self.memberButton.SetUpVisual(GA_PATH + "tab_guild_norm.png")
		self.memberButton.SetOverVisual(GA_PATH + "tab_guild_hover.png")
		self.memberButton.SetDownVisual(GA_PATH + "tab_guild_select.png")
		self.memberButton.SetPosition(2, 30)
		self.memberButton.SetEvent(ui.__mem_func__(self.OnClickMemberTab))
		self.memberButton.Show()

		self.leaderButton = ui.RadioButton()
		self.leaderButton.SetParent(self)
		self.leaderButton.SetUpVisual(GA_PATH + "tab_apply_norm.png")
		self.leaderButton.SetOverVisual(GA_PATH + "tab_apply_hover.png")
		self.leaderButton.SetDownVisual(GA_PATH + "tab_apply_select.png")
		self.leaderButton.SetPosition(2, 64)
		self.leaderButton.SetEvent(ui.__mem_func__(self.OnClickLeaderTab))
		self.leaderButton.Hide()
		self.leaderCounterBg = _Image(self.leaderButton, "ikashop/lite/business/counter.png", -5, -4)
		self.leaderCounterText = _Text(self.leaderCounterBg, "0", 10, 3, WHITE_COLOR, True)
		self.leaderCounterBg.Hide()

		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.AddFlag("attach")
		self.board.SetSize(BOARD_WIDTH, BOARD_HEIGHT)
		self.board.SetPosition(30, 0)
		self.board.SetTitleName(localeInfo.GUILD_APPLICATION_WINDOW_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.titleBar.SetPosition(42, 7)
		self.board.titleBar.SetWidth(BOARD_WIDTH - 49)
		self.board.Show()

		self.refreshButton = _Button(self.board, "button_titlebar_refresh", 11, 7, path = COMMON_PATH, normName = "norm", overName = "hover", downName = "down")
		self.refreshButton.SetEvent(ui.__mem_func__(self.RefreshData))

		self.__MakeMemberWindow()
		self.__MakeLeaderWindow()

		# drop downs last: drawn above every panel
		self.typeSelect.CreateDropDown(self.filterBg, 6, 76)
		self.langSelect.CreateDropDown(self.filterBg, 6, 129)
		self.empireSelect.CreateDropDown(self.filterBg, 6, 182)
		self.leaderTypeSelect.CreateDropDown(self.leaderFilterBg, 6, 73)
		self.leaderLangSelect.CreateDropDown(self.leaderFilterBg, 6, 121)
		for box in self.selectBoxes:
			box.closeOthers = ui.__mem_func__(self.__CloseDropDowns)
		# a click anywhere else in the window closes an open list
		for panel in (self, self.board, self.memberWindow, self.leaderWindow, self.filterBg, self.listBg, self.detailsBg,
				self.leaderFilterBg, self.appBg):
			panel.OnMouseLeftButtonDown = ui.__mem_func__(self.__OnClickPanel)

		self.ApplyModeUI()
		self.Hide()

	def __MakeMemberWindow(self):
		self.memberWindow = ui.Window()
		self.memberWindow.SetParent(self.board)
		self.memberWindow.AddFlag("attach")
		self.memberWindow.AddFlag("not_pick")	# full board size: must not swallow the title bar buttons
		self.memberWindow.SetSize(BOARD_WIDTH, BOARD_HEIGHT)
		self.memberWindow.SetPosition(0, 0)	# without a position the rect stays at screen 0,0 until the window is dragged
		self.memberWindow.Show()
		parent = self.memberWindow

		# ---- filter
		self.filterBg = _Image(parent, GA_PATH + "bg_filter.png", 8, 31, True)
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_FILTER_TITLE, 60, 5, TITLE_COLOR, True)
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_TYPE, 9, 34, TITLE_COLOR, font = "Tahoma:12b")
		self.typeSelect = SelectBox(self.filterBg, 4, 52, [(-1, T_ALL())] + [(s, GuildTypeName(s)) for s in (STATUS_OPEN, STATUS_CLOSED, STATUS_APPROVAL)], 4, ui.__mem_func__(self.OnFilterChanged))
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_LANGUAGE, 9, 87, TITLE_COLOR, font = "Tahoma:12b")
		self.langSelect = SelectBox(self.filterBg, 4, 105, [(i, LangName(i)) for i in xrange(len(LANGUAGE_ORDER))], 5, ui.__mem_func__(self.OnFilterChanged))
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_COLUMN_EMPIRE, 9, 140, TITLE_COLOR, font = "Tahoma:12b")
		self.empireSelect = SelectBox(self.filterBg, 4, 158, [(0, T_ALL())] + [(e, EmpireName(e)) for e in (1, 2, 3)], 4, ui.__mem_func__(self.OnFilterChanged))
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_REQUIRED_LEVEL, 9, 193, TITLE_COLOR, font = "Tahoma:12b")
		self.minLevelBox = _Image(self.filterBg, GA_PATH + "editline_bg.png", 4, 211, True, False)
		self.minLevelEdit = self.__MakeEdit(self.minLevelBox, localeInfo.GUILD_APPLICATION_MIN_LEVEL, 3, True)
		_Text(self.filterBg, localeInfo.GUILD_APPLICATION_GUILD_NAME, 9, 246, TITLE_COLOR, font = "Tahoma:12b")
		self.guildNameBox = _Image(self.filterBg, GA_PATH + "editline_bg.png", 4, 264, True, False)
		self.guildNameEdit = self.__MakeEdit(self.guildNameBox, localeInfo.GUILD_APPLICATION_GUILD_NAME, 12, False)
		self.selectBoxes += [self.typeSelect, self.langSelect, self.empireSelect]

		# ---- list
		self.listBg = _Image(parent, GA_PATH + "bg_list.png", 131, 31, True)
		_Text(self.listBg, localeInfo.GUILD_APPLICATION_LIST_TITLE, 153, 5, TITLE_COLOR, True)
		self.listTitleBg = _Image(self.listBg, GA_PATH + "title_list.png", 2, 28, True)
		_Text(self.listTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_NAME, 49, 5, TITLE_COLOR, True)
		_Text(self.listTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_LEVEL, 141, 5, TITLE_COLOR, True)
		_Text(self.listTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_SKILL, 206, 5, TITLE_COLOR, True)
		_Text(self.listTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_EMPIRE, 267, 5, TITLE_COLOR, True)

		self.guildRows = []
		for index in xrange(LIST_VISIBLE_ROWS):
			row = GuildRow(self.listBg)
			row.SetPosition(2, 54 + index * GUILD_LIST_ITEM_SPACING)
			row.SetEvent(ui.__mem_func__(self.OnSelectGuildRow), index)
			row.Hide()
			self.guildRows.append(row)

		self.listScrollBar = uiCharacterDetails.BaerimScrollBar()
		self.listScrollBar.SetParent(self.listBg)
		self.listScrollBar.SetPosition(293, 54)
		self.listScrollBar.SetScrollBarSize(211)
		self.listScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnListScroll))
		self.listScrollBar.Hide()

		self.pagePrevButton = _Button(self.listBg, "btn_next_left", 65, 276, path = SEARCH_PATH, normName = "normal", overName = "over", downName = "down")
		self.pagePrevButton.SetEvent(ui.__mem_func__(self.OnClickGuildPrevPage))
		self.pageNextButton = _Button(self.listBg, "btn_next_right", 239, 276, path = SEARCH_PATH, normName = "normal", overName = "over", downName = "down")
		self.pageNextButton.SetEvent(ui.__mem_func__(self.OnClickGuildNextPage))
		self.pageButtons = []
		for index in xrange(PAGEBUTTON_NUMBER_SIZE):
			button = _Button(self.listBg, "btn_page", 84 + index * 29, 269, "1", path = SEARCH_PATH, normName = "normal", overName = "over", downName = "down")
			button.SetEvent(ui.__mem_func__(self.OnClickGuildPageButton), index)
			button.ButtonText.SetPosition(button.GetWidth() / 2, button.GetHeight() / 2 - 2)
			self.pageButtons.append(button)

		# ---- details
		self.detailsBg = _Image(parent, GA_PATH + "details_bg.png", 439, 31, True)
		_Text(self.detailsBg, localeInfo.GUILD_APPLICATION_DETAILS_TITLE, 87, 5, TITLE_COLOR, True)
		self.detailsMark = ui.MarkBox()
		self.detailsMark.SetParent(self.detailsBg)
		self.detailsMark.Load()
		self.detailsMark.SetPosition(30, 60)
		self.detailsMark.AddFlag("not_pick")
		self.detailsMark.Show()
		self.detailsName = _Text(self.detailsBg, "", 119, 37, TEXT_COLOR, True)
		self.detailsLevel = _Text(self.detailsBg, "", 119, 63, TEXT_COLOR, True)
		self.detailsEmpire = _Image(self.detailsBg, GA_PATH + "empire_1.png", 106, 88)
		_Text(self.detailsBg, localeInfo.GUILD_APPLICATION_MEMBERS, 14, 122)
		self.detailsMembers = _Text(self.detailsBg, "", 139, 122, TEXT_COLOR, True)
		_Text(self.detailsBg, localeInfo.GUILD_APPLICATION_REQUIRED_LEVEL, 14, 148)
		self.detailsReqLevel = _Text(self.detailsBg, "", 139, 148, TEXT_COLOR, True)
		_Text(self.detailsBg, localeInfo.GUILD_APPLICATION_LANGUAGE, 14, 173)
		self.detailsLang = _Text(self.detailsBg, "", 145, 173, TEXT_COLOR, True)
		self.detailsDescLines = [_Text(self.detailsBg, "", 14, 197 + i * DESCRIPTION_LINE_HEIGHT) for i in xrange(DESCRIPTION_MAX_LINES)]
		self.detailsApplyButton = _Button(self.detailsBg, "btn_apply", 39, 261, " ", True)
		self.detailsApplyButton.SetEvent(ui.__mem_func__(self.OnClickDetailsApply))
		self.detailsApplyButton.ButtonText.SetPosition(self.detailsApplyButton.GetWidth() / 2, self.detailsApplyButton.GetHeight() / 2 - 2)
		self.__SetDetailsVisible(False)

	def __MakeLeaderWindow(self):
		self.leaderWindow = ui.Window()
		self.leaderWindow.SetParent(self.board)
		self.leaderWindow.AddFlag("attach")
		self.leaderWindow.AddFlag("not_pick")	# full board size: must not swallow the title bar buttons
		self.leaderWindow.SetSize(BOARD_WIDTH, BOARD_HEIGHT)
		self.leaderWindow.SetPosition(0, 0)	# without a position the rect stays at screen 0,0 until the window is dragged
		self.leaderWindow.Hide()
		parent = self.leaderWindow

		self.leaderFilterBg = _Image(parent, GA_PATH + "bg_filter.png", 8, 31, True)
		_Text(self.leaderFilterBg, localeInfo.GUILD_APPLICATION_MANAGE_GUILD, 60, 5, TITLE_COLOR, True)
		_Text(self.leaderFilterBg, localeInfo.GUILD_APPLICATION_CHANGE_GUILD_TYPE, 9, 31, TITLE_COLOR, font = "Tahoma:12b")
		self.leaderTypeSelect = SelectBox(self.leaderFilterBg, 5, 49, [(s, GuildTypeName(s)) for s in (STATUS_OPEN, STATUS_CLOSED, STATUS_APPROVAL)], 3, None)
		_Text(self.leaderFilterBg, localeInfo.GUILD_APPLICATION_CHANGE_LANGUAGE, 9, 79, TITLE_COLOR, font = "Tahoma:12b")
		self.leaderLangSelect = SelectBox(self.leaderFilterBg, 5, 97, [(i, LangName(i)) for i in xrange(len(LANGUAGE_ORDER))], 5, None)
		_Text(self.leaderFilterBg, localeInfo.GUILD_APPLICATION_CHANGE_MIN_LEVEL, 9, 124, TITLE_COLOR, font = "Tahoma:12b")
		self.leaderMinLevelBox = _Image(self.leaderFilterBg, GA_PATH + "editline_bg.png", 5, 142, True, False)
		self.leaderMinLevelEdit = self.__MakeEdit(self.leaderMinLevelBox, "", 3, True)
		_Text(self.leaderFilterBg, localeInfo.GUILD_APPLICATION_DESCRIPTION, 9, 169, TITLE_COLOR, font = "Tahoma:12b")
		self.leaderDescBox = _Image(self.leaderFilterBg, GA_PATH + "desc_bg.png", 5, 187, True, False)
		self.leaderDescEdit = ui.EditLine()
		self.leaderDescEdit.SetParent(self.leaderDescBox)
		self.leaderDescEdit.SetSize(99, 47)
		self.leaderDescEdit.SetPosition(6, 6)
		self.leaderDescEdit.SetMax(DESCRIPTION_MAX_LEN)
		self.leaderDescEdit.SetMultiLine()
		self.leaderDescEdit.SetLimitWidth(99)
		self.leaderDescEdit.SetPackedFontColor(WHITE_COLOR)
		self.leaderDescEdit.Show()
		self.leaderDescBox.OnMouseLeftButtonDown = ui.__mem_func__(self.leaderDescEdit.SetFocus)
		self.leaderApplyButton = _Button(self.leaderFilterBg, "btn_apply", 13, 259, localeInfo.GUILD_APPLICATION_APPLY)
		self.leaderApplyButton.SetEvent(ui.__mem_func__(self.OnLeaderApply))
		self.selectBoxes += [self.leaderTypeSelect, self.leaderLangSelect]

		self.appBg = _Image(parent, GA_PATH + "bg_list_application.png", 129, 29, True)
		_Text(self.appBg, localeInfo.GUILD_APPLICATION_APPLICATION_LIST, 242, 7, TITLE_COLOR, True)
		self.appTitleBg = _Image(self.appBg, GA_PATH + "car_title_application.png", 4, 29, True)
		_Text(self.appTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_NAME, 40, 4, TITLE_COLOR, True)
		_Text(self.appTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_LEVEL, 162, 4, TITLE_COLOR, True)
		_Text(self.appTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_CLASS, 227, 4, TITLE_COLOR, True)
		_Text(self.appTitleBg, localeInfo.GUILD_APPLICATION_COLUMN_LAST_PLAY, 324, 4, TITLE_COLOR, True)

		self.appRows = []
		for index in xrange(APP_VISIBLE_ROWS):
			row = ApplicationRow(self.appBg, ui.__mem_func__(self.OnApprovePid), ui.__mem_func__(self.OnRejectPid))
			row.SetPosition(2, 54 + index * GUILD_LIST_ITEM_SPACING)
			row.Hide()
			self.appRows.append(row)
		self.appScrollBar = uiCharacterDetails.BaerimScrollBar()
		self.appScrollBar.SetParent(self.appBg)
		self.appScrollBar.SetPosition(476, 54)
		self.appScrollBar.SetScrollBarSize(APP_VISIBLE_ROWS * GUILD_LIST_ITEM_SPACING + 1)
		self.appScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnAppScroll))
		self.appScrollBar.Hide()

	def __MakeEdit(self, box, placeHolder, maxLen, numberMode):
		edit = ui.EditLine()
		edit.SetParent(box)
		edit.SetSize(99, 14)
		edit.SetPosition(6, 4)
		edit.SetMax(maxLen)
		if numberMode:
			edit.SetNumberMode()
		edit.SetPackedFontColor(WHITE_COLOR)
		if placeHolder:
			edit.SetInfoMessage(placeHolder)
		edit.SetUpdateEvent(ui.__mem_func__(self.OnFilterChanged))
		edit.SetEscapeEvent(ui.__mem_func__(self.Close))
		edit.SetReturnEvent(ui.__mem_func__(edit.KillFocus))
		edit.Show()
		box.OnMouseLeftButtonDown = ui.__mem_func__(edit.SetFocus)
		return edit

	def __OnClickPanel(self):
		self.__CloseDropDowns()
		return False

	def __CloseDropDowns(self, keep = None):
		for box in self.selectBoxes:
			if box is not keep and box.dropDown:
				box.dropDown.Close()

	# ------------------------------------------------------------------ open / close
	def Open(self, mode = MODE_MEMBER):
		self.mode = mode if self.__IsLeaderModeAvailable() else MODE_MEMBER
		self.ApplyModeUI()
		self.Show()
		self.SetTop()
		self.RefreshData()

	def Close(self):
		self.__CloseDropDowns()
		for edit in (self.minLevelEdit, self.guildNameEdit, self.leaderMinLevelEdit, self.leaderDescEdit):
			edit.KillFocus()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Destroy(self):
		self.Hide()

	def RefreshData(self):
		now = app.GetTime()
		if self.lastRequestTime and now - self.lastRequestTime < CLICK_LIMIT_TIME:
			return
		self.lastRequestTime = now
		if self.mode == MODE_LEADER:
			net.SendChatPacket("/guild_app requests")
		else:
			net.SendChatPacket("/guild_app list")
			if self.__IsLeaderModeAvailable():
				net.SendChatPacket("/guild_app requests")

	def __IsLeaderModeAvailable(self):
		if not guild.IsGuildEnable():
			return False
		mainName = player.GetMainCharacterName()
		if mainName == guild.GetGuildMasterName():
			return True
		return self.__HasGeneralAccess(mainName)

	def __HasGeneralAccess(self, mainName):
		try:
			for index in xrange(guild.GetMemberCount()):
				(pid, name, grade, job, level, offer, general) = guild.GetMemberData(index)
				if name == mainName:
					return general != 0
		except:
			pass
		return False

	def ApplyModeUI(self):
		isLeader = self.mode == MODE_LEADER
		if self.__IsLeaderModeAvailable():
			self.leaderButton.Show()
		else:
			self.leaderButton.Hide()
		if isLeader:
			self.memberWindow.Hide()
			self.leaderWindow.Show()
			self.leaderButton.Down()
			self.memberButton.SetUp()
		else:
			self.leaderWindow.Hide()
			self.memberWindow.Show()
			self.memberButton.Down()
			self.leaderButton.SetUp()
		self.__CloseDropDowns()

	def OnClickMemberTab(self):
		if self.mode != MODE_MEMBER:
			self.mode = MODE_MEMBER
			self.ApplyModeUI()
			if not self.guilds:
				self.lastRequestTime = 0
				self.RefreshData()
		else:
			self.memberButton.Down()

	def OnClickLeaderTab(self):
		if self.mode != MODE_LEADER and self.__IsLeaderModeAvailable():
			self.mode = MODE_LEADER
			self.ApplyModeUI()
			net.SendChatPacket("/guild_app requests")
		else:
			self.leaderButton.Down()

	# ------------------------------------------------------------------ server data
	def RecvGuildsListBegin(self):
		self.receiving = []

	def RecvGuildRow(self, data):
		self.receiving.append(data)

	def RecvGuildsListEnd(self):
		self.guilds = self.receiving
		self.receiving = []
		# reference order: level, then skill points
		self.guilds.sort(key = lambda g : (-g["level"], -g["ladder"], g["name"].lower()))
		self.ApplyFilters()

	def RecvApplied(self, gid, applied):
		for g in self.guilds:
			if g["gid"] == gid:
				g["applied"] = applied
		self._RefreshGuildDetails()

	def RecvApplicationListBegin(self):
		self.receivingApps = []

	def RecvApplication(self, data):
		self.receivingApps.append(data)

	def RecvApplicationListEnd(self):
		self.applications = self.receivingApps
		self.receivingApps = []
		self.appOffset = max(0, min(self.appOffset, len(self.applications) - APP_VISIBLE_ROWS))
		self._RefreshLeaderCounter()
		self.__RefreshApplications()

	def RecvSettings(self, gid, status, lang, minLevel, desc):
		self.settings = (gid, status, lang, minLevel, desc)
		self._SyncLeaderSettings()
		for g in self.guilds:
			if g["gid"] == gid:
				g["status"] = status
				g["lang"] = lang
				g["minLevel"] = minLevel
				g["desc"] = desc
		self._RefreshGuildDetails()

	# ------------------------------------------------------------------ filters / list
	def OnFilterChanged(self, *args):
		self.ApplyFilters()

	def _GetFilterState(self):
		name = self.guildNameEdit.GetText().strip().lower()
		try:
			minLevel = int(self.minLevelEdit.GetText())
		except ValueError:
			minLevel = 0
		return (name, minLevel, self.typeSelect.GetValue(), self.langSelect.GetValue(), self.empireSelect.GetValue())

	def ApplyFilters(self):
		(name, minLevel, typeIdx, langID, empireID) = self._GetFilterState()
		result = []
		for g in self.guilds:
			if name and g["name"].lower().find(name) < 0:
				continue
			if minLevel and g["minLevel"] < minLevel:
				continue
			if typeIdx >= 0 and g["status"] != typeIdx:
				continue
			if langID and g["lang"] != langID:
				continue
			if empireID and g["empire"] != empireID:
				continue
			result.append(g)
		self.filtered = result
		self._SetGuildPage(min(self.currentGuildPage, self._GetGuildPageCount()))

	def _GetGuildPageCount(self):
		return max(1, (len(self.filtered) + GUILD_PAGE_SIZE - 1) / GUILD_PAGE_SIZE)

	def _SetGuildPage(self, page):
		self.__CloseDropDowns()
		self.currentGuildPage = max(1, min(page, self._GetGuildPageCount()))
		self.listOffset = 0
		self.RefreshList()
		self._UpdateGuildPagination()

	def __PageGuilds(self):
		start = (self.currentGuildPage - 1) * GUILD_PAGE_SIZE
		return self.filtered[start:start + GUILD_PAGE_SIZE]

	def RefreshList(self):
		pageGuilds = self.__PageGuilds()
		maxOffset = max(0, len(pageGuilds) - LIST_VISIBLE_ROWS)
		self.listOffset = max(0, min(self.listOffset, maxOffset))
		for (index, row) in enumerate(self.guildRows):
			dataIndex = self.listOffset + index
			if dataIndex < len(pageGuilds):
				data = pageGuilds[dataIndex]
				row.SetData(data)
				if data["gid"] == self._selectedGuildId:
					row.Down()
				else:
					row.SetUp()
				row.Show()
			else:
				row.Hide()
		if maxOffset > 0:
			self.listScrollBar.SetMiddleBarSize(float(LIST_VISIBLE_ROWS) / len(pageGuilds))
			self.listScrollBar.SetPos(float(self.listOffset) / maxOffset, False)
			self.listScrollBar.Show()
		elif pageGuilds:
			# reference keeps the track visible next to the rows
			self.listScrollBar.SetMiddleBarSize(1.0)
			self.listScrollBar.SetPos(0.0, False)
			self.listScrollBar.Show()
		else:
			self.listScrollBar.Hide()
		if not self._GetSelectedGuildRow() and pageGuilds:
			self._selectedGuildId = pageGuilds[0]["gid"]
			self.RefreshList()
			return
		self._RefreshGuildDetails()

	def __OnListScroll(self):
		maxOffset = max(0, len(self.__PageGuilds()) - LIST_VISIBLE_ROWS)
		self.listOffset = int(round(maxOffset * self.listScrollBar.GetPos()))
		self.RefreshList()

	def _UpdateGuildPagination(self):
		totalPages = self._GetGuildPageCount()
		firstPage = max(1, min(self.currentGuildPage - PAGEBUTTON_NUMBER_SIZE / 2, totalPages - PAGEBUTTON_NUMBER_SIZE + 1))
		for (index, button) in enumerate(self.pageButtons):
			page = firstPage + index
			if page <= totalPages:
				button.SetText(str(page))
				if page == self.currentGuildPage:
					button.Down()
				else:
					button.SetUp()
				button.page = page
				button.Show()
			else:
				button.Hide()

	def OnClickGuildPrevPage(self):
		self._SetGuildPage(self.currentGuildPage - 1)

	def OnClickGuildNextPage(self):
		self._SetGuildPage(self.currentGuildPage + 1)

	def OnClickGuildPageButton(self, buttonIndex):
		self._SetGuildPage(getattr(self.pageButtons[buttonIndex], "page", 1))

	def OnSelectGuildRow(self, index):
		self.__CloseDropDowns()
		row = self.guildRows[index]
		self._selectedGuildId = row.gid
		self.RefreshList()

	def _GetSelectedGuildRow(self):
		for g in self.__PageGuilds():
			if g["gid"] == self._selectedGuildId:
				return g
		return None

	# ------------------------------------------------------------------ details
	def __SetDetailsVisible(self, show):
		for window in [self.detailsMark, self.detailsName, self.detailsLevel, self.detailsEmpire, self.detailsMembers,
				self.detailsReqLevel, self.detailsLang, self.detailsApplyButton] + self.detailsDescLines:
			if show:
				window.Show()
			else:
				window.Hide()

	def _RefreshGuildDetails(self):
		row = self._GetSelectedGuildRow()
		if not row:
			self.__SetDetailsVisible(False)
			return
		self.__SetDetailsVisible(True)
		self.detailsMark.SetIndex(row["gid"])
		self.detailsName.SetText(row["name"])
		self.detailsLevel.SetText("Level %d" % row["level"])
		if 1 <= row["empire"] <= 3:
			self.detailsEmpire.LoadImage(GA_PATH + "empire_%d.png" % row["empire"])
		else:
			self.detailsEmpire.Hide()
		self.detailsMembers.SetText("%d/%d" % (row["members"], row["maxMembers"]))
		self.detailsReqLevel.SetText(str(row["minLevel"]))
		self.detailsLang.SetText(LangName(row["lang"]) if row["lang"] else T_ALL())
		self._SetWrappedDetailsDescription(row["desc"] or localeInfo.GUILD_APPLICATION_NO_DESCRIPTION)
		self._RefreshDetailsApplyButton(row)

	def _SetWrappedDetailsDescription(self, text):
		lines = WrapText(text, DESCRIPTION_LINE_WIDTH, DESCRIPTION_MAX_LINES)
		for (index, lineObj) in enumerate(self.detailsDescLines):
			lineObj.SetText(lines[index] if index < len(lines) else "")

	def _IsGuildFull(self, row):
		return row["members"] >= row["maxMembers"]

	def _RefreshDetailsApplyButton(self, row):
		button = self.detailsApplyButton
		if guild.IsGuildEnable():
			button.Hide()
			return
		button.Show()
		button.Enable()
		if row["empire"] and row["empire"] != net.GetEmpireID():
			button.SetText(localeInfo.GUILD_APPLICATION_WRONG_EMPIRE)
			button.Disable()
		elif row["applied"]:
			button.SetText(localeInfo.GUILD_APPLICATION_APPLIED)
		elif self._IsGuildFull(row):
			button.SetText(localeInfo.GUILD_APPLICATION_FULL)
			button.Disable()
		elif row["status"] == STATUS_CLOSED:
			button.SetText(localeInfo.GUILD_APPLICATION_STATUS_CLOSED)
			button.Disable()
		elif row["status"] == STATUS_OPEN:
			button.SetText(localeInfo.GUILD_APPLICATION_JOIN)
		else:
			button.SetText(localeInfo.GUILD_APPLICATION_APPLY)
		button.SetTextColor(BUTTON_TEXT_COLOR)

	def OnClickDetailsApply(self):
		row = self._GetSelectedGuildRow()
		if row:
			self.Apply(row)

	def Apply(self, guildData):
		if guildData["applied"]:
			net.SendChatPacket("/guild_app withdraw %d" % guildData["gid"])
			return
		if player.GetStatus(player.LEVEL) < guildData["minLevel"]:
			self.__ShowLevelError()
			return
		net.SendChatPacket("/guild_app apply %d" % guildData["gid"])

	def __ShowLevelError(self):
		self.popup = uiCommon.PopupDialog()
		self.popup.SetText(localeInfo.GUILD_APPLICATION_LEVEL_REQUIRED_ERROR)
		self.popup.Open()

	# ------------------------------------------------------------------ leader
	def _SyncLeaderSettings(self):
		if not self.settings:
			return
		(gid, status, lang, minLevel, desc) = self.settings
		self.leaderTypeSelect.SetValue(status)
		self.leaderLangSelect.SetValue(lang)
		self.leaderMinLevelEdit.SetText(str(minLevel))
		self.leaderDescEdit.SetText(desc)

	def OnLeaderApply(self):
		if not self.settings:
			return
		(gid, status, lang, minLevel, desc) = self.settings
		newStatus = self.leaderTypeSelect.GetValue()
		newLang = self.leaderLangSelect.GetValue()
		try:
			newMin = max(1, int(self.leaderMinLevelEdit.GetText()))
		except ValueError:
			newMin = minLevel
		newDesc = self.leaderDescEdit.GetText().replace("\n", " ").replace("|", " ")[:DESCRIPTION_MAX_LEN].strip()
		if newStatus != status:
			net.SendChatPacket("/guild_app status %d" % newStatus)
		if newLang != lang:
			net.SendChatPacket("/guild_app lang %d" % newLang)
		if newMin != minLevel:
			net.SendChatPacket("/guild_app minlevel %d" % newMin)
		if newDesc != desc:
			net.SendChatPacket("/guild_app desc %s" % newDesc)

	def _RefreshLeaderCounter(self):
		count = len(self.applications)
		if count > 0 and self.leaderButton.IsShow():
			self.leaderCounterText.SetText(str(min(count, 99)))
			self.leaderCounterBg.Show()
		else:
			self.leaderCounterBg.Hide()

	def __RefreshApplications(self):
		maxOffset = max(0, len(self.applications) - APP_VISIBLE_ROWS)
		for (index, row) in enumerate(self.appRows):
			dataIndex = self.appOffset + index
			if dataIndex < len(self.applications):
				data = self.applications[dataIndex]
				row.SetData(data, self.FormatApplyTime(data["time"]))
				row.Show()
			else:
				row.Hide()
		if maxOffset > 0:
			self.appScrollBar.SetMiddleBarSize(float(APP_VISIBLE_ROWS) / len(self.applications))
			self.appScrollBar.SetPos(float(self.appOffset) / maxOffset, False)
			self.appScrollBar.Show()
		else:
			self.appScrollBar.Hide()

	def __OnAppScroll(self):
		maxOffset = max(0, len(self.applications) - APP_VISIBLE_ROWS)
		self.appOffset = int(round(maxOffset * self.appScrollBar.GetPos()))
		self.__RefreshApplications()

	def FormatApplyTime(self, ts):
		if ts <= 0:
			return "-"
		return time.strftime("%d.%m.%Y", time.localtime(ts))

	def OnApprovePid(self, pid):
		net.SendChatPacket("/guild_app approve %d" % pid)

	def OnRejectPid(self, pid):
		net.SendChatPacket("/guild_app reject %d" % pid)

	# ------------------------------------------------------------------ wheel
	def OnRunMouseWheel(self, nLen):
		for box in self.selectBoxes:
			if box.dropDown and box.dropDown.IsShow() and box.dropDown.IsIn():
				box.dropDown.Scroll(1 if nLen > 0 else -1)
				return True
		delta = -1 if nLen > 0 else 1
		if self.mode == MODE_MEMBER:
			self.listOffset += delta
			self.RefreshList()
		else:
			self.appOffset = max(0, min(self.appOffset + delta, len(self.applications) - APP_VISIBLE_ROWS))
			self.__RefreshApplications()
		return True


def Toggle(mode = MODE_MEMBER):
	if g_interface:
		g_interface.ToggleGuildApplication(mode)


def WrapText(text, maxWidth, maxLines):
	"""Word wrap by pixel width (reference _WrapDescriptionByWidth), the last line is cut with '...'."""
	measure = ui.TextLine()
	lines = []
	current = ""
	for word in text.split(" "):
		candidate = (current + " " + word) if current else word
		measure.SetText(candidate)
		if measure.GetTextSize()[0] <= maxWidth:
			current = candidate
			continue
		if current:
			lines.append(current)
		current = word
		measure.SetText(current)
		while measure.GetTextSize()[0] > maxWidth and len(current) > 1:	# one long word: hard cut
			cut = len(current) - 1
			measure.SetText(current[:cut])
			while measure.GetTextSize()[0] > maxWidth and cut > 1:
				cut -= 1
				measure.SetText(current[:cut])
			lines.append(current[:cut])
			current = current[cut:]
			measure.SetText(current)
	if current:
		lines.append(current)
	if len(lines) > maxLines:
		lines = lines[:maxLines]
		last = lines[-1]
		measure.SetText(last + "...")
		while measure.GetTextSize()[0] > maxWidth and len(last) > 1:
			last = last[:-1]
			measure.SetText(last + "...")
		lines[-1] = last + "..."
	return lines


class GuildJoinNotification(ui.Window):
	"""Sliding "<guild> loncasi uye ariyor!" card (reference GuildJoinNotification, server Guild_JoinNotify)."""
	MARGIN_X = 12
	MARGIN_Y = 90
	SLIDE_TIME = 1.2
	LIFE_TIME = 10.0
	MAX_DESC_LINES = 4
	DESC_LINE_HEIGHT = 13
	WIDTH = 362
	HEIGHT = 132

	def __init__(self):
		ui.Window.__init__(self)
		self.guildID = 0
		self.openTime = 0.0
		self.Build()

	def __del__(self):
		ui.Window.__del__(self)

	def Build(self):
		self.SetSize(self.WIDTH, self.HEIGHT)
		self.AddFlag("float")
		self.background = _Image(self, GA_PATH + "bg_notif.png", 0, 0, True)
		self.titleText = _Text(self, "", 12, 11, NOTIFY_TITLE_COLOR, font = "Tahoma:14b")
		self.closeButton = ui.Button()
		self.closeButton.SetParent(self)
		self.closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		self.closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		self.closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		self.closeButton.SetPosition(self.WIDTH - 24, 9)
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		self.closeButton.Show()
		_Text(self, localeInfo.GUILD_APPLICATION_NOTIFICATION_DESC_TITLE, 12, 30, NOTIFY_TITLE_COLOR)
		self.descLines = [_Text(self, "", 17, 45 + i * self.DESC_LINE_HEIGHT, WHITE_COLOR) for i in xrange(self.MAX_DESC_LINES)]
		self.joinButton = _Button(self, "button_xsmall", 12, 101, localeInfo.GUILD_APPLICATION_NOTIFICATION_JOIN, path = COMMON_PATH, normName = "norm", overName = "hover", downName = "down")
		self.joinButton.SetEvent(ui.__mem_func__(self.OnClickJoin))
		_Text(self, localeInfo.GUILD_APPLICATION_NOTIFICATION_JOIN_HINT, 100, 105, BUTTON_TEXT_COLOR)
		self.Hide()

	def Open(self, guildID, guildName, description = ""):
		self.guildID = guildID
		title = localeInfo.GUILD_APPLICATION_NOTIFICATION_RECRUITING_TITLE
		self.titleText.SetText(title.replace("{}", guildName))
		lines = WrapText(description or localeInfo.GUILD_APPLICATION_EMPTY_DESCRIPTION, self.WIDTH - 34, self.MAX_DESC_LINES)
		for (index, line) in enumerate(self.descLines):
			line.SetText(lines[index] if index < len(lines) else "")
		self.openTime = app.GetTime()
		self.SetPosition(-self.WIDTH, self.MARGIN_Y)
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()
		return True

	def OnClickJoin(self):
		if self.guildID:
			net.SendChatPacket("/guild_app apply %d" % self.guildID)
		self.Close()

	def OnUpdate(self):
		elapsed = app.GetTime() - self.openTime
		if elapsed >= self.LIFE_TIME:
			self.Close()
			return
		# slide in from the left edge, slide back out in the last SLIDE_TIME
		if elapsed < self.SLIDE_TIME:
			t = elapsed / self.SLIDE_TIME
		elif elapsed > self.LIFE_TIME - self.SLIDE_TIME:
			t = (self.LIFE_TIME - elapsed) / self.SLIDE_TIME
		else:
			t = 1.0
		t = 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t)	# ease out
		x = int(-self.WIDTH + (self.WIDTH + self.MARGIN_X) * t)
		self.SetPosition(x, self.MARGIN_Y)
