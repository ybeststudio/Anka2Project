import app
import grp
import ui
import net
import chat
import guild
import player
import messenger
import localeInfo
import constInfo
import uiCommon
import ime
import uiGuild
import uiMessengerRenewal
import uiToolTip
import uiPickMoney
import item
import time
import wndMgr
from _weakref import proxy

# Baerim guild window (uiGuild.GuildWindow 659x317): five tabs on the left, pages at (40, 28).
# Phase 1: Info page and Member page. The tabs of the later phases (quest, skill, shop) stay disabled. Everything the
# old window still handles (war score board, guild list, skill use, land) goes to a hidden legacy uiGuild.GuildWindow.

GUILD_PATH = "d:/ymir work/ui/game/guild/"
PAGE_1 = GUILD_PATH + "1/"
PAGE_3 = GUILD_PATH + "3/"
PAGE_2 = GUILD_PATH + "2/"
PAGE_4 = GUILD_PATH + "4/"
PAGE_5 = GUILD_PATH + "5/"
BLESSING_ICON_PATH = "icon/item/item_benediction/"

WINDOW_WIDTH = 659
WINDOW_HEIGHT = 317
PAGE_X = 40
PAGE_Y = 28
TAB_Y = 30
TAB_STEP = 39

TEXT_COLOR = 0xffcabdab
NAME_COLOR = 0xfff3e9c8
HEADER_COLOR = 0xffd4c1a5
TITLE_COLOR = 0xfffee3ae

PAGE_INFO = "GUILD_INFO"
PAGE_QUEST = "QUEST"
PAGE_MEMBER = "MEMBER"
PAGE_SKILL = "SKILL"
PAGE_SHOP = "SHOP"
# reference tab order: home, quest, members, skills, shop
TABS = ((PAGE_INFO, "home"), (PAGE_QUEST, "quest"), (PAGE_MEMBER, "members"), (PAGE_SKILL, "skills"), (PAGE_SHOP, "shop"))
READY_PAGES = (PAGE_INFO, PAGE_QUEST, PAGE_MEMBER, PAGE_SKILL, PAGE_SHOP)

# reference GUILD_BLESSING_ITEMS 59000-59005 (phase 3 fills the timers)
BLESSINGS = (
	("GUILD_WINDOW_BLESSING_EXP", "exp_bene.png"),
	("GUILD_WINDOW_BLESSING_DROP", "drop_bene.png"),
	("GUILD_WINDOW_BLESSING_YANG", "yang_bene.png"),
	("GUILD_WINDOW_BLESSING_STONE", "double_stone_bene.png"),
	("GUILD_WINDOW_BLESSING_BOSS", "double_boss_bene.png"),
	("GUILD_WINDOW_BLESSING_FISH_MINE", "double_fish_mine_bene.png"),
)
BLESSING_ROW_HEIGHT = 37
BLESSING_VIEW_HEIGHT = 140
BLESSING_SCROLL_BAR_SCALE = 0.35

JOB_ICONS = ("icon_warbig.png", "icon_ninjabig.png", "icon_surabig.png", "icon_shamanbig.png")

MEMBER_VIEW_X = 3
MEMBER_VIEW_Y = 0
MEMBER_VIEW_WIDTH = 595
MEMBER_VIEW_HEIGHT = 272
TOTAL_ROW_HEIGHT = 45
HEADER_HEIGHT = 36
HEADER_GAP = 2
CARD_ROW_HEIGHT = 42
CARD_ROW_GAP = 3
CARDS_PER_ROW = 3
CARD_X = 7
CARD_STEP = 195
CARD_WIDTH = 189
LEADER_CARD_WIDTH = 291
DAILY_DONATION_MAX = 30		# reference daily donation limit
SCROLL_SPEED = 30
MAX_GRADE = 10		# reference list ends at Rank10
CO_LEADER = -1

COMMON_PATH = "d:/ymir work/ui/game/common/"
WIKI_BUTTON = "d:/ymir work/ui/game/wiki/layout/button_wiki_small_"
GUILD_APPLICATION_PATH = "d:/ymir work/ui/game/guild_application/"
GRADE_WIDTH = 380
GRADE_HEIGHT = 155
GRADE_EXPANDED_HEIGHT = 354
GRADE_NAME_MAX = 8		# server GUILD_GRADE_NAME_MAX_LEN
GRADE_ROW_WIDTH = 314
GRADE_ROW_STEP = 22
# reference row order; bits: server GUILD_AUTH_*
GRADE_PERMISSIONS = (
	("GUILD_GRADE_AUTH_INVITE", 1 << 0),
	("GUILD_GRADE_AUTH_REMOVE", 1 << 1),
	("GUILD_GRADE_AUTH_USE_SKILL", 1 << 3),
	("GUILD_GRADE_AUTH_QUEST", 1 << 6),
	("GUILD_GRADE_AUTH_DUNGEON", 1 << 8),
	("GUILD_GRADE_AUTH_UP_SKILL", 1 << 7),
	("GUILD_GRADE_AUTH_SHOP", 1 << 5),
	("GUILD_GRADE_AUTH_WHISPER", 1 << 2),
	("GUILD_GRADE_AUTH_WAR", 1 << 4),
)
GRADE_SELECT_WIDTH = 237
GRADE_SELECT_ROW_WIDTH = 193
GRADE_SELECT_ROW_HEIGHT = 22
GRADE_SELECT_ROW_STEP = 24
GRADE_SELECT_NORMAL_COLOR = 0xff191512
GRADE_SELECT_HOVER_COLOR = 0xff372b24
GRADE_SELECT_CURRENT_COLOR = 0xff52392b
GRADE_SELECT_LINE_COLOR = 0xff382e24
GRADE_SELECT_TEXT_COLOR = 0xffd7c9b9
GRADE_SELECT_HOVER_TEXT_COLOR = 0xffffe7c0
# reference guild.json "quest"; indexes are difficulty - 1, columns are quest types 1 to 4
QUEST_COLUMNS = (("GUILD_QUEST_TITLE_1", "GUILD_QUEST_OBJECTIVE_1", "bg_metin.png"), ("GUILD_QUEST_TITLE_2", "GUILD_QUEST_OBJECTIVE_2", "bg_dungeon.png"),
	("GUILD_QUEST_TITLE_3", "GUILD_QUEST_OBJECTIVE_3", "bg_boss.png"), ("GUILD_QUEST_TITLE_4", "GUILD_QUEST_OBJECTIVE_4", "bg_fish_mine.png"))
QUEST_OBJECTIVES = ((500, 15, 20, 250), (1500, 25, 30, 500), (4000, 40, 50, 1000))
QUEST_REWARD_EXP = (300000, 375000, 450000)
QUEST_REWARD_GOLD = (140, 180, 225)
QUEST_WEEKLY = 4
QUEST_DURATION_TEXT = "01:00:00"
QUEST_COLUMN_STEP = 147
QUEST_REFRESH_INTERVAL = 10
QUEST_RANK_ROWS = 8
QUEST_WARNING_COLOR = 0xffe9383f
QUEST_WARNING_DIALOG_WIDTH = 254
QUEST_START_DIALOG_WIDTH = 208
QUEST_RESULTS = {
	0 : "GUILD_QUEST_RESULT_STARTED",
	1 : "GUILD_QUEST_RESULT_NO_AUTH",
	2 : "GUILD_QUEST_RESULT_ACTIVE",
	3 : "GUILD_QUEST_RESULT_WEEKLY",
	4 : "GUILD_QUEST_RESULT_NOT_FINISHED",
	5 : "GUILD_QUEST_RESULT_REWARDED",
	6 : "GUILD_QUEST_RESULT_CANCELLED",
	7 : "GUILD_QUEST_RESULT_COMPLETED",
}
QUEST_SIDEBAR_TITLE_COLOR = 0xfff4d03f
SIDEBAR_PATH = "d:/ymir work/ui/game/dynamic_sidebar/"
# reference uiGuildConfig: skill icon, level box and + button of each tab
SKILL_TABS = (
	("skill_1/", ((1, 218, 61, 235, 107, 253, 104), (2, 259, 155, 276, 201, 294, 198), (3, 358, 95, 375, 141, 393, 138),
		(5, 446, 155, 463, 201, 481, 198), (4, 487, 60, 504, 106, 522, 103))),
	("skill_2/", ((1, 192, 158, 209, 204, 227, 201), (2, 259, 63, 276, 109, 294, 106), (3, 358, 124, 375, 170, 393, 167),
		(4, 446, 63, 463, 109, 481, 106), (5, 507, 158, 524, 204, 542, 201))),
)
SKILL_VALUES = ((0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15), (0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15), (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10),
	(0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50), (0, 2, 4, 6, 8, 10, 12, 15, 20, 25, 30), (0, 2, 4, 6, 9, 12, 15, 18, 21, 25, 30),
	(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), (0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50), (0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50),
	(0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15))
SKILL_MAX_LEVEL = 10
SKILL_ACTIVE_COUNT = 5
SKILL_POINT_EXP_MAX = 100000
SKILL_POINT_COST = (1, 2, 3, 5, 7, 9, 11, 15, 30, 50)
SKILL_GOLD_COST = (750, 1000, 2000, 4000, 7000, 10000, 13000, 16000, 20000, 25000)		# server s_aiGoldCost
SKILL_UPGRADE_DIALOG_WIDTH = 452
SKILL_ENERGY_COST = 50		# server GUILD_SKILL_ENERGY_COST, for 3 hours
SKILL_DURATIONS = (3, 12, 24)
SKILL_DONATION_ITEMS = ((90010, 1000), (90011, 2500), (90012, 5000))
SKILL_BOOST_ITEMS = (65110, 65111)
SKILL_TOOLTIP_WIDTH = 330
SKILL_ITEM_TOOLTIP_WIDTH = 300		# room for the 288 px shortcut table
SKILL_NAME_COLOR = 0xffb0f2b6
SKILL_NEXT_COLOR = 0xffff4040
SKILL_NOTE_COLOR = 0xfff4d03f
# reference guild.json "shop", same order as the server lists: vnum, count, price, guild level
SHOP_SILVER_ITEMS = ((71001, 1, 5, 1), (25040, 5, 3, 1), (71094, 1, 7, 2), (70064, 10, 4, 3), (70063, 10, 2, 3), (70043, 1, 10, 4), (50260, 10, 30, 5), (70005, 1, 10, 5), (80019, 10, 20, 6), (100300, 1, 10, 6), (71153, 1, 40, 7), (90010, 1, 40, 8), (40220, 1, 10, 8), (90011, 1, 100, 9), (70035, 1, 75, 9), (90012, 1, 200, 10), (71025, 1, 200, 10), (71001, 40, 140, 11), (71094, 40, 200, 11), (40209, 10, 120, 12), (40210, 5, 50, 14), (50260, 200, 500, 15), (80019, 200, 350, 16), (40220, 5, 40, 16), (71153, 5, 105, 17), (90010, 5, 160, 18), (90011, 5, 400, 19), (70035, 5, 300, 19), (90012, 5, 800, 20), (71025, 5, 800, 20),)
SHOP_GOLD_ITEMS = ((65112, 1, 300, 1), (65120, 1, 1000, 1),)
SHOP_COLUMNS = 5
SHOP_VIEW_X = 6
SHOP_VIEW_Y = 35
SHOP_VIEW_WIDTH = 582
SHOP_VIEW_HEIGHT = 231
SHOP_NAME_WIDTH = 84
SHOP_ASK_WIDTH = 340
SHOP_TOOLTIP_WIDTH = 300		# room for the 288 px shortcut table
SHOP_LOCK_TOOLTIP_WIDTH = 207
SHOP_MAX_STACK = 200
SHOP_STEP_X = 113
SHOP_STEP_Y = 113
SILVER_LIMIT = 100		# reference SetLimitSilverCoins
LOG_DAYS = 7
LOG_VISIBLE_ROWS = 9
LOG_ROW_HEIGHT = 26
LOG_ROW_COLUMNS = ((11, 120), (132, 280), (413, 117))		# reference LogItem boxes: x, width
LOG_ROW_BOX_HEIGHT = 25
LOG_ROW_COLOR = 0xff2a211a
LOG_LOADING_MIN_TIME = 0.5		# a local server answers at once: keep the spinner visible
LOG_LOADING_TIMEOUT = 10.0
LOG_LOADING_ROTATION = 10
LOG_DONATION = 1
LOG_SKILL = 2
DONATION_NAME_COLOR = 0xfffee3ae
BLESSING_TOOLTIP_WIDTH = 300
BLESSING_DESC_COLOR = 0xffc1c1c1
BLESSING_TIME_COLOR = 0xff89b88d
DONATION_BUTTON_COLOR = 0xffcebb98
DONATION_HELP_WIDTH = 290
DONATION_HELP_TABLE_WIDTH = 240
DONATION_HELP_ICON_WIDTH = 40
DONATION_HELP_HEAD_HEIGHT = 26
DONATION_HELP_ROW_HEIGHT = 38
MEMBER_TOOLTIP_WIDTH = 300
MEMBER_TOOLTIP_AURA_COLOR = 0xfff4d03f
MEMBER_TOOLTIP_COUNT_COLOR = 0xffb0f2b6
MEMBER_HOVER_HEIGHT = 24
WHISPER_PRICE = 2		# server GUILD_WHISPER_PRICE
WHISPER_SENDER_MAX = 24
WHISPER_TEXT_MAX = 110
WHISPER_SENDER_INVALID = " |[]#:"
WHISPER_POPUP_WIDTH = 350
# name key, icon, yang price (0 = not shown), daily limit, gold and silver gain
DONATIONS = (
	("GUILD_DONATION_SCROLL", "refine_stone_1.png", 100000, 20, 1),
	("GUILD_DONATION_PREMIUM", "refine_stone_3.png", 0, 10, 10),
)
DONATION_RESULT_WHISPER_SENT = 7
# server EGuildDonationResult
DONATION_MESSAGES = {
	0 : "GUILD_DONATION_OK",
	1 : "GUILD_DONATION_LIMIT",
	2 : "GUILD_DONATION_NO_YANG",
	3 : "GUILD_DONATION_NO_GOLD",
	4 : "GUILD_DONATION_NO_AUTH",
	6 : "GUILD_DONATION_BUSY",
	DONATION_RESULT_WHISPER_SENT : "GUILD_DONATION_WHISPER_SENT",
	8 : "GUILD_DONATION_NO_SCROLL",
	9 : "GUILD_DONATION_NO_PREMIUM",
}


def _Image(parent, path, x, y, pick = False):
	image = ui.ExpandedImageBox()
	image.SetParent(parent)
	if not pick:
		image.AddFlag("not_pick")
	image.LoadImage(path)
	image.SetPosition(x, y)
	image.Show()
	return image


def _Text(parent, text, x, y, color = TEXT_COLOR, font = "Tahoma:12", center = False, outline = False):
	line = ui.TextLine()
	line.SetParent(parent)
	line.AddFlag("not_pick")
	line.SetFontName(font)
	if center:
		line.SetHorizontalAlignCenter()
	line.SetPackedFontColor(color)
	if outline:
		line.SetOutline()
	line.SetText(text)
	line.SetPosition(x, y)
	line.Show()
	return line


def _Button(parent, prefix, x, y, text = "", textColor = None):
	button = ui.Button()
	button.SetParent(parent)
	button.SetUpVisual(prefix + "_norm.png")
	button.SetOverVisual(prefix + "_hover.png")
	button.SetDownVisual(prefix + "_select.png")
	button.SetPosition(x, y)
	if text:
		button.SetText(text)
		button.ButtonText.SetPosition(button.GetWidth() / 2, button.GetHeight() / 2 - 1)
		if textColor:
			button.ButtonText.SetPackedFontColor(textColor)
			button.ButtonText.SetOutline()
	button.Show()
	return button


def _ThinBoard(parent, x, y, width, height):
	board = ui.ThinBoard()
	board.SetParent(parent)
	board.AddFlag("not_pick")
	path = "d:/ymir work/ui/pattern/new_thinboard/"
	for (image, name) in zip(board.Corners, ("LeftTop", "LeftBottom", "RightTop", "RightBottom")):
		image.LoadImage(path + "ThinBoard_Corner_" + name + ".png")
	for (image, name) in zip(board.Lines, ("Left", "Right", "Top", "Bottom")):
		image.LoadImage(path + "ThinBoard_Line_" + name + ".png")
	board.Base.SetColor(grp.GenerateColor(28.0 / 255.0, 23.0 / 255.0, 20.0 / 255.0, 178.0 / 255.0))		# new_thinboard/base.png
	board.SetPosition(x, y)
	board.SetSize(width, height)
	board.Show()
	return board


class MemberToolTip(uiToolTip.ToolTip):
	"""Reference info_member_tooltip: 300 wide, under the member row and centered on the mouse."""

	def __init__(self):
		uiToolTip.ToolTip.__init__(self, MEMBER_TOOLTIP_WIDTH)
		self.anchorY = 0

	def SetAnchorY(self, y):
		self.anchorY = y

	def OnUpdate(self):
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		x = max(0, min(mouseX - MEMBER_TOOLTIP_WIDTH / 2, wndMgr.GetScreenWidth() - MEMBER_TOOLTIP_WIDTH))
		self.SetPosition(x, self.anchorY)


class DonationHelpToolTip(uiToolTip.ToolTip):
	"""Reference info_item_1 tooltip: one line and an Item / Elde Etme table (240 wide, columns 40 and 200)."""

	HEAD_COLOR = 0xFF2D1C11
	ROW_COLOR = 0xFF1F1511
	LINE_COLOR = 0xFF756657

	def __init__(self):
		uiToolTip.ToolTip.__init__(self, DONATION_HELP_WIDTH)
		self.AppendTextLine(localeInfo.GUILD_DONATION_HELP, TEXT_COLOR)
		left = (DONATION_HELP_WIDTH - DONATION_HELP_TABLE_WIDTH) / 2
		top = self.toolTipHeight + 3
		self.__AddBar(left, top, DONATION_HELP_TABLE_WIDTH, DONATION_HELP_HEAD_HEIGHT, self.HEAD_COLOR)
		self.__AddText(localeInfo.GUILD_DONATION_HELP_ITEM, left + DONATION_HELP_ICON_WIDTH / 2, top + 6, self.TITLE_COLOR, True)
		self.__AddText(localeInfo.GUILD_DONATION_HELP_TITLE, left + DONATION_HELP_ICON_WIDTH + 100, top + 6, self.TITLE_COLOR, True)
		y = top + DONATION_HELP_HEAD_HEIGHT
		rows = (("refine_stone_1.png", (localeInfo.GUILD_DONATION_SOURCE_SCROLL_1, localeInfo.GUILD_DONATION_SOURCE_SCROLL_2)),
			("refine_stone_3.png", (localeInfo.GUILD_DONATION_SOURCE_PREMIUM,)))
		for (icon, lines) in rows:
			self.__AddBar(left, y, DONATION_HELP_TABLE_WIDTH, DONATION_HELP_ROW_HEIGHT - 1, self.ROW_COLOR)
			self.__AddBar(left + DONATION_HELP_ICON_WIDTH, y, 1, DONATION_HELP_ROW_HEIGHT - 1, self.LINE_COLOR)
			self.childrenList.append(_Image(self, "icon/item/item_guild/" + icon, left + 4, y + 3))
			textY = y + (DONATION_HELP_ROW_HEIGHT - 1 - len(lines) * 14) / 2
			for (index, line) in enumerate(lines):
				self.__AddText(line, left + DONATION_HELP_ICON_WIDTH + 8, textY + index * 14, TEXT_COLOR, False)
			y += DONATION_HELP_ROW_HEIGHT
		self.toolTipHeight = y - 5
		self.ResizeToolTip()

	def __AddBar(self, x, y, width, height, color):
		bar = ui.Bar()
		bar.SetParent(self)
		bar.AddFlag("not_pick")
		bar.SetPosition(x, y)
		bar.SetSize(width, height)
		bar.SetColor(color)
		bar.Show()
		self.childrenList.append(bar)

	def __AddText(self, text, x, y, color, center):
		self.childrenList.append(_Text(self, text, x, y, color, center = center, outline = True))


class BlessingToolTip(uiToolTip.ToolTip):
	"""Reference blessing tooltip: name, description, bonus, duration and the shortcut table of an item tooltip."""

	def __init__(self):
		uiToolTip.ToolTip.__init__(self, BLESSING_TOOLTIP_WIDTH)

	def SetBlessing(self, index):
		key = BLESSINGS[index][0]
		self.ClearToolTip()
		self.AppendTextLine(getattr(localeInfo, key), self.TITLE_COLOR)
		self.AppendSpace(5)
		for line in getattr(localeInfo, key + "_DESC").split("#"):
			self.AppendTextLine(line, BLESSING_DESC_COLOR)
		self.AppendSpace(6)
		for line in getattr(localeInfo, key + "_BONUS").split("#"):
			self.AppendTextLine(line, self.CONDITION_COLOR)
		self.AppendSpace(2)
		self.AppendTextLine(localeInfo.GUILD_WINDOW_BLESSING_DURATION, BLESSING_TIME_COLOR)
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", False):
			rows.append((("ctrl", "mouse_right"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		if getattr(app, "ENABLE_WIKI_SYSTEM", False):
			rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
		self.AppendShortcutTable(rows)


class HoverWindow(ui.Window):
	def __init__(self, overIn, overOut):
		ui.Window.__init__(self)
		self.overIn = ui.__mem_func__(overIn)
		self.overOut = ui.__mem_func__(overOut)

	def OnMouseOverIn(self):
		self.overIn()

	def OnMouseOverOut(self):
		self.overOut()


def _IsMemberOnline(name):
	if name == player.GetName():
		return True
	interface = constInfo.GetInterfaceInstance()
	wnd = getattr(interface, "wndMessenger", None) if interface else None
	if wnd and hasattr(wnd, "IsGuildMemberOnline"):
		return wnd.IsGuildMemberOnline(name)
	return False


class InfoPage(ui.Window):
	"""Reference Info page: guild data on the left, blessings top right, guild actions bottom right."""

	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = proxy(owner)
		self.keep = []		# static decorations: a window without a python reference is destroyed at once
		self.SetSize(624, 315)
		self.keep.append(_Image(self, PAGE_1 + "background.png", 3, 0))
		self.keep.append(_Text(self, localeInfo.GUILD_WINDOW_INFO_TITLE, 162, 10, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_WINDOW_BLESSING_TITLE, 464, 10, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_WINDOW_ACTION_TITLE, 452, 183, TITLE_COLOR, center = True, outline = True))

		# mark slot: always the no logo picture, as on the reference
		self.markSlot = _Image(self, PAGE_1 + "no_logo.png", 42, 47)
		self.nameText = _Text(self, "", 99, 50)

		self.uploadButton = _Button(self, PAGE_1 + "btn_upload", 217, 33, localeInfo.GUILD_WINDOW_UPLOAD)
		self.uploadButton.SetEvent(ui.__mem_func__(self.owner.OnClickMark))
		self.managementButton = _Button(self, PAGE_1 + "btn_upload", 217, 58, localeInfo.GUILD_WINDOW_MANAGEMENT)
		self.managementButton.SetEvent(ui.__mem_func__(self.owner.OnClickManagement))

		rows = ((localeInfo.GUILD_WINDOW_LEVEL, 91), (localeInfo.GUILD_WINDOW_EXP, 122), (localeInfo.GUILD_WINDOW_MEMBERS, 153),
			(localeInfo.GUILD_WINDOW_LEADER, 185), (localeInfo.GUILD_WINDOW_GOLD, 218), (localeInfo.GUILD_WINDOW_AVERAGE_LEVEL, 250))
		for (text, y) in rows:
			self.keep.append(_Text(self, text, 20, y))
		self.levelValue = _Text(self, "", 282, 91, center = True)
		self.expEmpty = _Image(self, PAGE_1 + "jaugebar_empty.png", 190, 120)
		self.expFull = _Image(self, PAGE_1 + "jaugebar_full.png", 190, 120)
		self.expValue = _Text(self, "", 253, 123, center = True)
		self.memberValue = _Text(self, "", 264, 153, center = True)
		self.memberHover = HoverWindow(self.owner.OnOverInMembers, self.owner.OnOverOutMembers)
		self.memberHover.SetParent(self)
		self.memberHover.SetPosition(210, 147)
		self.memberHover.SetSize(112, MEMBER_HOVER_HEIGHT)
		self.memberHover.Show()
		self.leaderValue = _Text(self, "", 264, 185, center = True)
		self.keep.append(_Image(self, PAGE_1 + "slot_coins.png", 222, 213))
		self.goldValue = _Text(self, "", 246, 218, center = True)
		self.donateButton = _Button(self, PAGE_1 + "btn_donate", 275, 214)
		self.donateButton.SetEvent(ui.__mem_func__(self.owner.OnClickDonate))
		self.averageValue = _Text(self, "", 283, 251, center = True)

		# blessings (phase 3: timers)
		self.blessingView = ui.Window()
		self.blessingView.SetParent(self)
		self.blessingView.SetPosition(328, 31)
		self.blessingView.SetSize(264, BLESSING_VIEW_HEIGHT)
		self.blessingView.SetInsideRender(True)
		self.blessingView.Show()
		self.blessingContent = ui.Window()
		self.blessingContent.SetParent(self.blessingView)
		self.blessingContent.AddFlag("not_pick")
		self.blessingContent.SetSize(264, BLESSING_ROW_HEIGHT * len(BLESSINGS) + 1)
		self.blessingContent.Show()
		self.blessingTimes = []
		self.blessingPos = 0
		for (index, (key, icon)) in enumerate(BLESSINGS):
			row = _Image(self.blessingContent, PAGE_1 + "bg_blessing.png", 0, index * BLESSING_ROW_HEIGHT)
			self.keep.append(row)
			self.keep.append(_Image(row, BLESSING_ICON_PATH + icon, 3, 2))
			self.keep.append(_Text(row, getattr(localeInfo, key), 42, 11))
			self.blessingTimes.append(_Text(row, "00:00:00", 223, 11, center = True))
		self.hoverBlessing = -1
		self.blessingHover = HoverWindow(self.__OnBlessingOverIn, self.__OnBlessingOverOut)
		self.blessingHover.SetParent(self)
		self.blessingHover.SetPosition(328, 31)
		self.blessingHover.SetSize(264, BLESSING_VIEW_HEIGHT)
		self.blessingHover.OnRunMouseWheel = ui.__mem_func__(self.owner.OnRunMouseWheel)
		self.blessingHover.OnMouseWheel = ui.__mem_func__(self.owner.OnMouseWheel)
		self.blessingHover.Show()
		self.isBlessingHover = False
		self.blessingScroll = uiMessengerRenewal.ThinScrollBar()
		self.blessingScroll.SetParent(self)
		self.blessingScroll.SetPosition(593, 33)
		self.blessingScroll.SetSize(5, 136)
		self.blessingScroll.SetMiddleBarScale(BLESSING_SCROLL_BAR_SCALE)
		self.blessingScroll.SetScrollEvent(ui.__mem_func__(self.__OnBlessingScroll))
		self.blessingScroll.SetPosScale(0.0)
		self.blessingScroll.Show()

		self.warButton = _Button(self, PAGE_1 + "btn_declare", 363, 243, localeInfo.GUILD_WINDOW_DECLARE_WAR)
		self.warButton.SetEvent(ui.__mem_func__(self.owner.OnClickDeclareWar))
		self.whisperButton = _Button(self, PAGE_1 + "btn_declare", 473, 243, localeInfo.GUILD_WINDOW_SEND_WHISPER)
		self.whisperButton.SetEvent(ui.__mem_func__(self.owner.OnClickWhisper))

	def __OnBlessingOverIn(self):
		self.isBlessingHover = True
		self.hoverBlessing = -1

	def __OnBlessingOverOut(self):
		self.isBlessingHover = False
		self.owner.HideBlessingToolTip()

	def OnUpdate(self):
		if not self.isBlessingHover:
			return
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		(x, y) = self.blessingHover.GetGlobalPosition()
		row = max(0, min(len(BLESSINGS) - 1, (mouseY - y + self.blessingPos) / BLESSING_ROW_HEIGHT))
		if row != self.hoverBlessing:
			self.hoverBlessing = row
			self.owner.ShowBlessingToolTip(row)

	def __OnBlessingScroll(self, scale):
		maxScroll = max(0, BLESSING_ROW_HEIGHT * len(BLESSINGS) + 1 - BLESSING_VIEW_HEIGHT)
		self.blessingPos = int(maxScroll * scale)
		self.blessingContent.SetPosition(0, -self.blessingPos)

	def Scroll(self, nLen):
		maxScroll = max(1, BLESSING_ROW_HEIGHT * len(BLESSINGS) + 1 - BLESSING_VIEW_HEIGHT)
		pos = max(0, min(maxScroll, self.blessingPos + (-SCROLL_SPEED if nLen > 0 else SCROLL_SPEED)))
		self.blessingScroll.SetPosScale(float(pos) / maxScroll)
		self.__OnBlessingScroll(float(pos) / maxScroll)

	def Refresh(self):
		self.nameText.SetText(guild.GetGuildName())
		self.levelValue.SetText(str(guild.GetGuildLevel()))
		(curExp, needExp) = guild.GetGuildExperience()
		total = curExp + needExp
		percent = (curExp * 100 / total) if total > 0 else 0
		self.expValue.SetText("%d / %d (%d%%)" % (curExp, total, percent))
		ratio = float(curExp) / total if total > 0 else 0.0
		self.expFull.SetRenderingRect(0.0, 0.0, -1.0 + ratio, 0.0)
		(curMember, maxMember) = guild.GetGuildMemberCount()
		self.memberValue.SetText("%d / %d" % (curMember, maxMember))
		self.leaderValue.SetText(guild.GetGuildMasterName())
		self.goldValue.SetText(str(self.owner.guildGold))
		self.averageValue.SetText(str(guild.GetGuildMemberLevelAverage()))
		isMaster = player.GetName() == guild.GetGuildMasterName()
		if isMaster:
			self.uploadButton.Show()
		else:
			self.uploadButton.Hide()
		if isMaster or guild.MainPlayerHasAuthority(guild.AUTH_NOTICE):
			self.warButton.Show()
		else:
			self.warButton.Hide()


class QuestColumn(object):
	"""One quest card: the idle widgets, the running widgets and the lock over the other cards."""

	def __init__(self, page, index):
		self.index = index
		self.keepIdle = []
		self.keepActive = []
		self.level = 1
		x = index * QUEST_COLUMN_STEP
		center = x + 80
		(titleKey, self.objectiveKey, image) = QUEST_COLUMNS[index]
		title = getattr(localeInfo, titleKey)
		self.keepIdle.append(_Image(page, PAGE_4 + "bg_unstart.png", x + 7, 2))
		self.keepActive.append(_Image(page, PAGE_4 + "bg_start.png", x + 7, 2))
		self.keepIdle.append(_Image(page, PAGE_4 + image, x + 10, 30))
		self.keepActive.append(_Image(page, PAGE_4 + image, x + 8, 29))
		self.keepIdle.append(_Text(page, title, center, 9, TITLE_COLOR, center = True, outline = True))
		self.keepActive.append(_Text(page, title, center, 9, TITLE_COLOR, center = True, outline = True))
		self.remainingText = self.__Idle(_Text(page, "", center, 57, TITLE_COLOR, center = True, outline = True))
		self.__Idle(_Text(page, localeInfo.GUILD_QUEST_OBJECTIVE, center, 80, TITLE_COLOR, center = True, outline = True))
		self.objectiveText = self.__Idle(_Text(page, "", center, 99, center = True, outline = True))
		self.__Idle(_Text(page, localeInfo.GUILD_QUEST_TIME, center, 118, TITLE_COLOR, center = True, outline = True))
		self.__Idle(_Text(page, QUEST_DURATION_TEXT, center, 137, center = True, outline = True))
		self.__Idle(_Text(page, localeInfo.GUILD_QUEST_REWARD, center, 158, TITLE_COLOR, center = True, outline = True))
		self.rewardText = self.__Idle(_Text(page, "", center, 177, center = True, outline = True))
		self.__Idle(_Text(page, localeInfo.GUILD_QUEST_DIFFICULTY, center, 196, TITLE_COLOR, center = True, outline = True))
		self.difficulty = []
		for (level, name) in enumerate(("easy", "med", "hard")):
			button = ui.RadioButton()
			button.SetParent(page)
			button.SetUpVisual(PAGE_4 + "btn_" + name + "_norm.png")
			button.SetOverVisual(PAGE_4 + "btn_" + name + "_hover.png")
			button.SetDownVisual(PAGE_4 + "btn_" + name + "_select.png")
			button.SetPosition(x + 15 + level * 44, 219)
			button.SetEvent(lambda col = index, arg = level + 1, wnd = proxy(page) : wnd.OnSelectLevel(col, arg))
			button.Show()
			self.difficulty.append(button)
			self.keepIdle.append(button)
		self.startButton = self.__Idle(_Button(page, PAGE_4 + "btn_start", x + 15, 246, localeInfo.GUILD_QUEST_START, TEXT_COLOR))
		self.startButton.SetEvent(lambda arg = index, wnd = proxy(page) : wnd.OnClickStart(arg))

		self.__Active(_Text(page, localeInfo.GUILD_QUEST_OBJECTIVE, center, 85, TITLE_COLOR, center = True, outline = True))
		self.currentObjective = self.__Active(_Text(page, "", center, 102, center = True, outline = True))
		self.__Active(_Image(page, PAGE_4 + "jaugebar_empty.png", x + 15, 118))
		self.gaugeFull = self.__Active(_Image(page, PAGE_4 + "jaugebar_full.png", x + 15, 118))
		self.progressText = self.__Active(_Text(page, "", center, 120, center = True, outline = True))
		self.__Active(_Text(page, localeInfo.GUILD_QUEST_TIME_LEFT, center, 144, TITLE_COLOR, center = True, outline = True))
		self.timeText = self.__Active(_Text(page, "", center, 162, center = True, outline = True))
		self.__Active(_Text(page, localeInfo.GUILD_QUEST_OPTION, center, 182, TITLE_COLOR, center = True, outline = True))
		self.rankButton = self.__Active(_Button(page, PAGE_4 + "btn_rank", x + 27, 204))
		self.rankButton.SetEvent(lambda wnd = proxy(page) : wnd.OnClickRank())
		self.refreshButton = self.__Active(_Button(page, PAGE_4 + "btn_refresh", x + 82, 204))
		self.refreshButton.SetEvent(lambda wnd = proxy(page) : wnd.Request())
		self.abortButton = _Button(page, PAGE_4 + "btn_start", x + 15, 241, localeInfo.GUILD_QUEST_ABORT, TEXT_COLOR)
		self.abortButton.SetEvent(lambda wnd = proxy(page) : wnd.OnClickAbort())
		self.claimButton = _Button(page, PAGE_4 + "btn_start", x + 15, 241, localeInfo.GUILD_QUEST_CLAIM, TEXT_COLOR)
		self.claimButton.SetEvent(lambda wnd = proxy(page) : wnd.OnClickClaim())
		self.lockImage = _Image(page, PAGE_4 + "bg_lock.png", x + 7, 30)
		self.SetLevel(1)

	def __Idle(self, window):
		self.keepIdle.append(window)
		return window

	def __Active(self, window):
		self.keepActive.append(window)
		return window

	def SetLevel(self, level):
		self.level = level
		for (index, button) in enumerate(self.difficulty):
			if index + 1 == level:
				button.Down()
			else:
				button.SetUp()
		objective = QUEST_OBJECTIVES[level - 1][self.index]
		self.objectiveText.SetText(getattr(localeInfo, self.objectiveKey) % localeInfo.NumberToMoneyStringNEW(objective))
		self.rewardText.SetText(localeInfo.GUILD_QUEST_REWARD_VALUE % (localeInfo.NumberToMoneyStringNEW(QUEST_REWARD_EXP[level - 1]), QUEST_REWARD_GOLD[level - 1]))

	def SetRemaining(self, remaining):
		self.remainingText.SetText(localeInfo.GUILD_QUEST_REMAINING % remaining)

	def SetProgress(self, current, target):
		left = max(0, target - current)
		self.currentObjective.SetText(getattr(localeInfo, self.objectiveKey) % localeInfo.NumberToMoneyStringNEW(left))
		self.progressText.SetText("%d / %d" % (current, target))
		ratio = min(1.0, float(current) / target) if target > 0 else 0.0
		self.gaugeFull.SetRenderingRect(0.0, 0.0, -1.0 + ratio, 0.0)
		isFinished = target > 0 and current >= target
		if isFinished:
			self.abortButton.Hide()
			self.claimButton.Show()
		else:
			self.claimButton.Hide()
			self.abortButton.Show()

	def SetTime(self, seconds):
		self.timeText.SetText("%02d:%02d:%02d" % (seconds / 3600, seconds / 60 % 60, seconds % 60))

	def SetState(self, isActive, isLocked):
		for window in self.keepIdle:
			if isActive:
				window.Hide()
			else:
				window.Show()
		for window in self.keepActive:
			if isActive:
				window.Show()
			else:
				window.Hide()
		if not isActive:
			self.abortButton.Hide()
			self.claimButton.Hide()
		if isLocked:
			self.lockImage.Show()
		else:
			self.lockImage.Hide()


class QuestPage(ui.Window):
	"""Reference Quest page: four cards, one running quest locks the others."""

	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = proxy(owner)
		self.SetSize(624, 315)
		self.lastRequest = 0
		self.background = _Image(self, PAGE_4 + "background.png", 3, 0)
		self.columns = [QuestColumn(self, index) for index in xrange(len(QUEST_COLUMNS))]

	def __del__(self):
		self.columns = []
		ui.Window.__del__(self)

	def Scroll(self, nLen):
		pass

	def Request(self):
		self.lastRequest = app.GetTime()
		net.SendChatPacket("/guild_quest info")

	def Refresh(self):
		quest = self.owner.questData
		active = quest["type"] - 1
		for (index, column) in enumerate(self.columns):
			column.SetRemaining(max(0, QUEST_WEEKLY - quest["used"][index]))
			column.SetState(index == active, active >= 0 and index != active)
		if active >= 0:
			self.columns[active].SetProgress(quest["cur"], quest["target"])
			self.__RefreshTime()

	def __RefreshTime(self):
		quest = self.owner.questData
		if quest["type"]:
			self.columns[quest["type"] - 1].SetTime(self.owner.GetQuestSecondsLeft())

	def OnUpdate(self):
		self.__RefreshTime()
		if app.GetTime() - self.lastRequest >= QUEST_REFRESH_INTERVAL:
			self.Request()

	def OnSelectLevel(self, index, level):
		self.columns[index].SetLevel(level)

	def OnClickStart(self, index):
		self.owner.AskQuest(localeInfo.GUILD_QUEST_START_QUESTION_1, "/guild_quest start %d %d" % (index + 1, self.columns[index].level), None, TEXT_COLOR, QUEST_START_DIALOG_WIDTH)

	def OnClickAbort(self):
		self.owner.AskQuest(localeInfo.GUILD_QUEST_ABORT_QUESTION_1, "/guild_quest abort", localeInfo.GUILD_QUEST_ABORT_QUESTION_2, QUEST_WARNING_COLOR, QUEST_WARNING_DIALOG_WIDTH)

	def OnClickClaim(self):
		net.SendChatPacket("/get_guild_quest_reward")

	def OnClickRank(self):
		self.owner.OpenQuestRank()


class QuestQuestionDialog(ui.Board):
	"""Reference wndPopupDialog: a plain board, two centered lines, tick and cross buttons."""

	WIDTH = 260
	HEIGHT = 105

	def __init__(self):
		ui.Board.__init__(self)
		self.command = ""
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetSize(self.WIDTH, self.HEIGHT)
		self.line1 = _Text(self, "", self.WIDTH / 2, 19, center = True, outline = True)
		self.line2 = _Text(self, localeInfo.GUILD_QUEST_START_QUESTION_2, self.WIDTH / 2, 39, center = True, outline = True)
		self.accept = self.__Button("button_accept", 0)
		self.accept.SetEvent(ui.__mem_func__(self.__OnAccept))
		self.cancel = self.__Button("button_cancel", 0)
		self.cancel.SetEvent(ui.__mem_func__(self.Close))
		self.Hide()

	def __del__(self):
		ui.Board.__del__(self)

	def __Button(self, name, x):
		button = ui.Button()
		button.SetParent(self)
		button.SetUpVisual(COMMON_PATH + name + "_norm.png")
		button.SetOverVisual(COMMON_PATH + name + "_hover.png")
		button.SetDownVisual(COMMON_PATH + name + "_down.png")
		button.SetPosition(x, 63)
		button.Show()
		return button

	def Open(self, text, command, line2 = None, color = TEXT_COLOR, width = 0):
		width = width or self.WIDTH
		self.command = command
		self.SetSize(width, self.HEIGHT)
		self.line1.SetPosition(width / 2, 19)
		self.line2.SetPosition(width / 2, 39)
		self.accept.SetPosition(width / 2 - 65, 63)
		self.cancel.SetPosition(width / 2 + 8, 63)
		self.line1.SetText(text)
		self.line2.SetPackedFontColor(color)
		self.line2.SetText(line2 or localeInfo.GUILD_QUEST_START_QUESTION_2)
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def __OnAccept(self):
		net.SendChatPacket(self.command)
		self.Close()

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class QuestRankWindow(ui.BoardWithTitleBar):
	"""Reference GuildQuestRank 265x225: rank, name and score of the running quest."""

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.keep = []
		self.rows = []
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetSize(265, 225)
		self.SetTitleName(localeInfo.GUILD_QUEST_RANK_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.keep.append(_ThinBoard(self, 8, 30, 248, 187))
		self.keep.append(_Image(self, PAGE_4 + "rank/title.png", 10, 32))
		self.keep.append(_Text(self, localeInfo.GUILD_QUEST_RANK_RANK, 21, 37, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_QUEST_RANK_NAME, 121, 37, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_QUEST_RANK_SCORE, 221, 37, TITLE_COLOR, center = True, outline = True))
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def SetRows(self, data):
		for window in self.rows:
			window.Hide()
		self.rows = []
		y = 62
		for (index, (name, score)) in enumerate(data):
			image = "rank/slot_%d.png" % (index + 1) if index < 3 else "rank/slot.png"
			row = _Image(self, PAGE_4 + image, 8, y)
			height = row.GetHeight()
			if y + height > 215:
				row.Hide()
				break
			self.rows.append(row)
			self.rows.append(_Text(row, str(index + 1), 10, 4, center = True, outline = True))
			self.rows.append(_Text(row, name, 114, 4, center = True, outline = True))
			self.rows.append(_Text(row, str(score), 216, 4, center = True, outline = True))
			y += height

	def Open(self):
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class QuestSideBar(ui.ThinBoard):
	"""Reference uiSidebar.GuildQuestSideBar: running quest at the left of the screen, also with the guild window closed."""

	WIDTH = 142
	HEIGHT = 110

	def __init__(self, owner):
		ui.ThinBoard.__init__(self)
		self.owner = proxy(owner)
		self.lastRequest = 0
		self.AddFlag("movable")
		self.AddFlag("float")
		path = "d:/ymir work/ui/pattern/new_thinboard_second/"
		for (image, name) in zip(self.Corners, ("LeftTop", "LeftBottom", "RightTop", "RightBottom")):
			image.LoadImage(path + "ThinBoard_Corner_" + name + ".tga")
		for (image, name) in zip(self.Lines, ("Left", "Right", "Top", "Bottom")):
			image.LoadImage(path + "ThinBoard_Line_" + name + ".tga")
		self.Base.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 133.0 / 255.0))
		self.SetSize(self.WIDTH, self.HEIGHT)
		self.banner = _Image(self, PAGE_4 + "bg_metin.png", 1, 1)
		self.header = _Text(self, localeInfo.GUILD_QUEST_SIDEBAR, self.WIDTH / 2, 28, QUEST_SIDEBAR_TITLE_COLOR, "Tahoma:12b", True, True)
		self.refreshButton = _Button(self, SIDEBAR_PATH + "refresh", self.WIDTH - 20, 5)
		self.refreshButton.SetEvent(ui.__mem_func__(self.__OnRefresh))
		self.titleText = _Text(self, "", self.WIDTH / 2, 50, center = True, outline = True)
		self.timeText = _Text(self, "", self.WIDTH / 2, 63, center = True, outline = True)
		self.barBack = _Image(self, SIDEBAR_PATH + "progress_bar_0.png", 10, 81)
		self.barFill = _Image(self, SIDEBAR_PATH + "progress_bar_1.png", 10, 81)
		self.progressText = _Text(self, "", self.WIDTH / 2, 90, center = True, outline = True)
		self.SetPosition(8, wndMgr.GetScreenHeight() / 2 - self.HEIGHT / 2)
		self.Hide()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def __OnRefresh(self):
		self.lastRequest = app.GetTime()
		net.SendChatPacket("/guild_quest info")

	def Refresh(self):
		quest = self.owner.questData
		if not quest["type"]:
			self.Hide()
			return
		index = quest["type"] - 1
		(titleKey, objectiveKey, image) = QUEST_COLUMNS[index]
		self.banner.LoadImage(PAGE_4 + image)
		self.titleText.SetText(getattr(localeInfo, titleKey))
		self.progressText.SetText("%d / %d" % (quest["cur"], quest["target"]))
		ratio = min(1.0, float(quest["cur"]) / quest["target"]) if quest["target"] > 0 else 0.0
		self.barFill.SetRenderingRect(0.0, 0.0, -1.0 + ratio, 0.0)
		self.Show()

	def OnUpdate(self):
		seconds = self.owner.GetQuestSecondsLeft()
		self.timeText.SetText(localeInfo.GUILD_QUEST_SIDEBAR_TIME % (seconds / 60, seconds % 60))
		if app.GetTime() - self.lastRequest >= QUEST_REFRESH_INTERVAL:		# members on another core
			self.__OnRefresh()


class SkillIcon(ui.Window):
	"""One skill of the tree: the picture is the hover area of the tooltip."""

	def __init__(self, page, skill, x, y, path):
		ui.Window.__init__(self)
		self.SetParent(page)
		self.SetPosition(x, y)
		self.SetSize(50, 50)
		self.skill = skill
		self.overIn = ui.__mem_func__(page.OnOverInSkill)
		self.overOut = ui.__mem_func__(page.OnOverOutSkill)
		self.icon = _Image(self, path, 0, 0)

	def OnMouseOverIn(self):
		self.overIn(self.skill)

	def OnMouseOverOut(self):
		self.overOut()


class SkillDonationItem(ui.Window):
	"""Reference donation choice: click selects the stone."""

	def __init__(self, page, index, x, y, vnum):
		ui.Window.__init__(self)
		self.SetParent(page)
		self.SetPosition(x, y)
		self.SetSize(32, 32)
		self.event = ui.__mem_func__(page.OnSelectDonation)
		self.overIn = ui.__mem_func__(page.OnOverInDonation)
		self.overOut = ui.__mem_func__(page.OnOverOutSkill)
		self.index = index
		self.background = _Image(self, COMMON_PATH + "slots/slot_32x32.png", 0, 0)
		self.slot = ui.SlotWindow()		# the rotating selection is the active slot effect of the inventory
		self.slot.SetParent(self)
		self.slot.AddFlag("not_pick")
		self.slot.SetPosition(0, 0)
		self.slot.SetSize(32, 32)
		self.slot.AppendSlot(0, 0, 0, 32, 32)
		self.slot.SetItemSlot(0, vnum, 0)
		self.slot.RefreshSlot()
		self.slot.Show()
		self.Show()

	def SetSelected(self, selected):
		if selected:
			self.slot.ActivateSlot(0, wndMgr.COLOR_TYPE_YELLOW)
		else:
			self.slot.DeactivateSlot(0)

	def OnMouseLeftButtonDown(self):
		self.event(self.index)
		return True

	def OnMouseOverIn(self):
		self.overIn(self.index)

	def OnMouseOverOut(self):
		self.overOut()


class SkillBoostCheck(ui.Window):
	"""Reference sk_checkbox_boost: box_uncheck / box_check over the corner of the item icon."""

	def __init__(self, parent, x, y, event):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(15, 15)
		self.event = event
		self.empty = _Image(self, PAGE_2 + "box_uncheck.png", 0, 0)
		self.checked = _Image(self, PAGE_2 + "box_check.png", 0, 0)
		self.checked.Hide()
		self.Show()

	def SetCheck(self, checked):
		if checked:
			self.checked.Show()
		else:
			self.checked.Hide()

	def OnMouseLeftButtonDown(self):
		self.event()
		return True


class SkillPage(ui.Window):
	"""Reference Skill page: donation on the left, active and passive trees on the right."""

	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = proxy(owner)
		self.SetSize(624, 315)
		self.keep = []
		self.tab = 0
		self.selected = 0
		self.boost = [False, False]
		self.tooltip = None
		self.itemToolTip = None
		self.activeWindows = ([], [])
		self.levelTexts = {}
		self.plusButtons = {}
		wnd = proxy(self)
		self.keep.append(_Image(self, PAGE_2 + "background.png", 3, 0))
		self.keep.append(_Text(self, localeInfo.GUILD_SKILL_TAB_DONATION, 81, 10, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_SKILL_POINT, 15, 33, outline = True))
		self.donationLogButton = _Button(self, PAGE_2 + "donation_log", 68, 30)
		self.donationLogButton.SetEvent(lambda arg = wnd : arg.owner.OpenLog(LOG_DONATION))
		if getattr(app, "ENABLE_WIKI_SYSTEM", False):
			self.wikiButton = ui.Button()
			self.wikiButton.SetParent(self)
			self.wikiButton.SetUpVisual(WIKI_BUTTON + "norm.png")
			self.wikiButton.SetOverVisual(WIKI_BUTTON + "hover.png")
			self.wikiButton.SetDownVisual(WIKI_BUTTON + "down.png")
			self.wikiButton.SetPosition(92, 29)
			self.wikiButton.SetEvent(lambda arg = wnd : arg.OnClickWiki())
			self.wikiButton.Show()
		self.pointText = _Text(self, "0", 127, 33, center = True, outline = True)
		self.refreshButton = _Button(self, PAGE_2 + "btn_refresh", 141, 31)
		self.refreshButton.SetEvent(lambda arg = wnd : arg.OnClickRefresh())
		self.keep.append(_Image(self, PAGE_2 + "jaugebar_empty.png", 9, 52))
		self.gauge = _Image(self, PAGE_2 + "jaugebar_full.png", 9, 52)
		self.gaugeText = _Text(self, "", 84, 56, center = True, outline = True)
		self.donationItems = []
		for (index, (vnum, exp)) in enumerate(SKILL_DONATION_ITEMS):
			self.donationItems.append(SkillDonationItem(self, index, 24 + index * 41, 88, vnum))
		self.gainText = _Text(self, "", 84, 132, center = True, outline = True)
		self.donateButton = _Button(self, PAGE_2 + "btn_donate", 23, 152, localeInfo.GUILD_SKILL_DONATE, TEXT_COLOR)
		self.donateButton.SetEvent(lambda arg = wnd : arg.OnClickDonate())
		self.boostNames = []
		self.boostChecks = []
		for (index, vnum) in enumerate(SKILL_BOOST_ITEMS):
			y = 191 + index * 41
			item.SelectItem(vnum)
			self.keep.append(_Image(self, item.GetIconImageFileName(), 13, y))
			self.boostNames.append(_Text(self, "", 51, y, SKILL_NAME_COLOR, outline = True))
			self.keep.append(_Text(self, getattr(localeInfo, "GUILD_SKILL_BOOST_%d_DESC" % (index + 1)), 51, y + 14, outline = True))
			check = SkillBoostCheck(self, 10, y + 20, lambda arg = index, page = wnd : page.OnToggleBoost(arg))
			self.boostChecks.append(check)
		self.tabButtons = []
		for (index, key) in enumerate(("GUILD_SKILL_TAB_ACTIVE", "GUILD_SKILL_TAB_PASSIVE")):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetUpVisual(PAGE_2 + "category_norm.png")
			button.SetOverVisual(PAGE_2 + "category_hover.png")
			button.SetDownVisual(PAGE_2 + "category_select.png")
			button.SetPosition(165 + index * 216, 4)
			button.SetText(getattr(localeInfo, key))
			button.SetEvent(lambda arg = index, page = wnd : page.SelectTab(arg))
			button.Show()
			self.tabButtons.append(button)
		for (tab, (folder, skills)) in enumerate(SKILL_TABS):
			windows = self.activeWindows[tab]
			windows.append(_Image(self, PAGE_2 + ("bg1.png" if tab == 0 else "bg2.png"), 162, 29))
			for (index, (image, iconX, iconY, boxX, boxY, plusX, plusY)) in enumerate(skills):
				skill = tab * SKILL_ACTIVE_COUNT + index
				windows.append(SkillIcon(self, skill, iconX, iconY, PAGE_2 + folder + "%d.png" % image))
				windows.append(_Image(self, PAGE_2 + "box_skill_level.png", boxX, boxY))
				text = _Text(self, "0", boxX + 9, boxY + 2, center = True, outline = True)
				self.levelTexts[skill] = text
				windows.append(text)
				plus = _Button(self, PAGE_2 + "btn_add", plusX, plusY)
				plus.SetEvent(lambda arg = skill, page = wnd : page.OnClickUpgrade(arg))
				self.plusButtons[skill] = plus
		active = self.activeWindows[0]
		active.append(_Image(self, PAGE_2 + "energybar_empty.png", 168, 239))
		self.energyFull = _Image(self, PAGE_2 + "energybar_full.png", 168, 239)
		active.append(self.energyFull)
		self.energyText = _Text(self, "", 310, 243, center = True, outline = True)
		active.append(self.energyText)
		self.activateButton = _Button(self, PAGE_2 + "btn_active", 453, 240, localeInfo.GUILD_SKILL_ACTIVATE, TEXT_COLOR)
		self.activateButton.SetEvent(lambda arg = wnd : arg.OnClickActivate())
		active.append(self.activateButton)
		self.rechargeButton = _Button(self, PAGE_2 + "btn_recharge", 523, 240, localeInfo.GUILD_SKILL_RECHARGE, TEXT_COLOR)
		self.rechargeButton.SetEvent(lambda arg = wnd : arg.OnClickRecharge())
		active.append(self.rechargeButton)
		self.activeWindows[1].append(_Text(self, localeInfo.GUILD_SKILL_PASSIVE_DESC, 381, 243, SKILL_NAME_COLOR, center = True, outline = True))
		# created after bg1 / bg2 so they do not draw over it
		self.skillLogButton = _Button(self, PAGE_2 + "skill_log", 564, 34)
		self.skillLogButton.SetEvent(lambda arg = wnd : arg.owner.OpenLog(LOG_SKILL))
		self.SelectDonation(0)
		self.SelectTab(0)

	def __del__(self):
		ui.Window.__del__(self)

	def Scroll(self, nLen):
		pass

	def OnClickWiki(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			interface.OpenWikiWindow()

	def SelectTab(self, tab):
		self.tab = tab
		for (index, button) in enumerate(self.tabButtons):
			if index == tab:
				button.Down()
			else:
				button.SetUp()
		for (index, windows) in enumerate(self.activeWindows):
			for window in windows:
				if index == tab:
					window.Show()
				else:
					window.Hide()
		self.Refresh()

	def SelectDonation(self, index):
		self.selected = index
		for (position, donationItem) in enumerate(self.donationItems):
			donationItem.SetSelected(position == index)
		self.gainText.SetText(localeInfo.GUILD_SKILL_EXP_GAIN % SKILL_DONATION_ITEMS[index][1])

	def OnSelectDonation(self, index):
		self.SelectDonation(index)

	def OnToggleBoost(self, index):
		self.boost[index] = not self.boost[index]
		self.boostChecks[index].SetCheck(self.boost[index])

	def OnClickDonate(self):
		net.SendChatPacket("/guild_skill donate %d %d %d" % (self.selected, int(self.boost[0]), int(self.boost[1])))

	def OnClickUpgrade(self, skill):
		level = self.owner.skillData["levels"][skill]
		if level >= SKILL_MAX_LEVEL:
			return
		self.owner.AskQuest(localeInfo.GUILD_SKILL_UPGRADE_QUESTION, "/guild_skill up %d" % skill,
			localeInfo.GUILD_SKILL_UPGRADE_COST % (SKILL_POINT_COST[level], localeInfo.NumberToMoneyStringNEW(SKILL_GOLD_COST[level])), TEXT_COLOR, SKILL_UPGRADE_DIALOG_WIDTH)

	def OnClickActivate(self):
		self.owner.AskSkillActivation()

	def OnClickRefresh(self):
		net.SendChatPacket("/guild_skill info")

	def OnClickRecharge(self):
		net.SendChatPacket("/guild_skill recharge")

	def Refresh(self):
		data = self.owner.skillData
		self.pointText.SetText(str(data["points"]))
		self.gaugeText.SetText("%s / %s" % (localeInfo.NumberToMoneyStringNEW(data["exp"]), localeInfo.NumberToMoneyStringNEW(SKILL_POINT_EXP_MAX)))
		self.gauge.SetRenderingRect(0.0, 0.0, -1.0 + float(data["exp"]) / SKILL_POINT_EXP_MAX, 0.0)
		self.energyText.SetText("%d / %d" % (data["energy"], data["maxEnergy"]))
		ratio = float(data["energy"]) / data["maxEnergy"] if data["maxEnergy"] > 0 else 0.0
		self.energyFull.SetRenderingRect(0.0, 0.0, -1.0 + ratio, 0.0)
		for (index, vnum) in enumerate(SKILL_BOOST_ITEMS):
			self.boostNames[index].SetText(getattr(localeInfo, "GUILD_SKILL_BOOST_%d" % (index + 1)) % player.GetItemCountByVnum(vnum))
		for (skill, text) in self.levelTexts.items():
			level = data["levels"][skill]
			text.SetText(str(level))
			plus = self.plusButtons[skill]
			if skill / SKILL_ACTIVE_COUNT == self.tab and level < SKILL_MAX_LEVEL and data["points"] >= SKILL_POINT_COST[level]:
				plus.Show()
			else:
				plus.Hide()

	def OnOverInSkill(self, skill):
		if not self.tooltip:
			self.tooltip = uiToolTip.ToolTip(SKILL_TOOLTIP_WIDTH)
		tooltip = self.tooltip
		level = self.owner.skillData["levels"][skill]
		effect = getattr(localeInfo, "GUILD_SKILL_EFFECT_%d" % skill)
		tooltip.ClearToolTip()
		tooltip.AppendTextLine(getattr(localeInfo, "GUILD_SKILL_NAME_%d" % skill), SKILL_NAME_COLOR)
		tooltip.AppendSpace(5)
		tooltip.AppendTextLine(localeInfo.GUILD_SKILL_LEVEL % level, BLESSING_DESC_COLOR)
		tooltip.AppendTextLine(effect % SKILL_VALUES[skill][level], BLESSING_DESC_COLOR)
		if skill == SKILL_ACTIVE_COUNT + 1:
			tooltip.AppendSpace(5)
			tooltip.AppendTextLine(localeInfo.GUILD_SKILL_QUEST_NOTE, SKILL_NOTE_COLOR)
		if level < SKILL_MAX_LEVEL:
			tooltip.AppendSpace(5)
			tooltip.AppendTextLine(localeInfo.GUILD_SKILL_NEXT % (level + 1), SKILL_NEXT_COLOR)
			tooltip.AppendTextLine(effect % SKILL_VALUES[skill][level + 1], SKILL_NEXT_COLOR)
		tooltip.ShowToolTip()

	def OnOverInDonation(self, index):
		if not self.itemToolTip:
			self.itemToolTip = uiToolTip.ToolTip(SKILL_ITEM_TOOLTIP_WIDTH)
		tooltip = self.itemToolTip
		item.SelectItem(SKILL_DONATION_ITEMS[index][0])
		tooltip.ClearToolTip()
		tooltip.AppendTextLine(item.GetItemName(), tooltip.TITLE_COLOR)
		tooltip.AppendSpace(5)
		for line in localeInfo.GUILD_SKILL_DONATION_ITEM_DESC.split("#"):
			tooltip.AppendTextLine(line, BLESSING_DESC_COLOR)
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", False):
			rows.append((("ctrl", "mouse_right"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		if getattr(app, "ENABLE_WIKI_SYSTEM", False):
			rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
		tooltip.AppendShortcutTable(rows)
		tooltip.ShowToolTip()

	def OnOverOutSkill(self):
		if self.tooltip:
			self.tooltip.HideToolTip()
		if self.itemToolTip:
			self.itemToolTip.HideToolTip()


class ShopItem(ui.Button):
	"""Reference shop card: name, price, item and the lock of a too low guild level."""

	def __init__(self, page, kind, index, entry):
		ui.Button.__init__(self)
		self.SetParent(page.view)
		path = PAGE_5 + ("gold/bg_item_" if kind else "silver/bg_item")
		if kind:
			self.SetUpVisual(path + "norm.png")
			self.SetOverVisual(path + "hover.png")
			self.SetDownVisual(path + "select.png")
		else:
			self.SetUpVisual(path + ".png")
			self.SetOverVisual(path + "_hover.png")
			self.SetDownVisual(path + "_select.png")
		(self.vnum, self.count, self.price, self.level) = entry
		self.page = proxy(page)
		self.index = index
		self.kind = kind
		self.locked = False
		center = self.GetWidth() / 2
		item.SelectItem(self.vnum)
		self.itemName = item.GetItemName()
		self.nameText = _Text(self, self.itemName, 0, 3, outline = True)
		self.__FitName()
		self.__Center(self.nameText, center, 3)
		self.price_bg = _Image(self, PAGE_5 + ("gold/bg_price.png" if kind else "silver/bg_price.png"), 1, 21)
		self.priceText = _Text(self, str(self.price), 0, 21, outline = True)
		self.__Center(self.priceText, 15, 21)
		# a slot window ignores the clipped scroll view: draw the item with clipped widgets instead
		self.icon = _Image(self, item.GetIconImageFileName(), 31, 58)
		self.countText = _Text(self, str(self.count), 0, 77, outline = True)
		self.countText.SetPosition(62 - self.countText.GetTextSize()[0], 77)
		self.boundMarker = _Image(self, "icon/item/trade.png", 31, 58)
		self.lockImage = _Image(self, PAGE_5 + "bg_lock.png", 1, 20)
		self.lockText = _Text(self, localeInfo.GUILD_SHOP_UNLOCK % self.level, 0, 70, outline = True)
		self.__Center(self.lockText, center, 70)
		self.SetEvent(lambda arg = proxy(self) : arg.OnClick())
		self.ShowToolTip = ui.__mem_func__(self.__ShowTip)
		self.HideToolTip = ui.__mem_func__(self.__HideTip)

	def __del__(self):
		ui.Button.__del__(self)

	def __Center(self, line, x, y):
		# the clipped scroll view cuts a center aligned text: place it left aligned instead
		line.SetPosition(x - line.GetTextSize()[0] / 2, y)

	def __FitName(self):
		name = self.itemName
		if self.nameText.GetTextSize()[0] <= SHOP_NAME_WIDTH:
			return
		while len(name) > 1:
			name = name[:-1]
			self.nameText.SetText(name.rstrip() + "...")
			if self.nameText.GetTextSize()[0] <= SHOP_NAME_WIDTH:
				return

	def __ShowTip(self):
		self.page.OnOverInItem(self.vnum, self.count, self.price, self.kind, self.locked)

	def __HideTip(self):
		self.page.OnOverOutItem()

	def SetLevel(self, guildLevel):
		# stays enabled: the tooltip of a locked item needs the mouse over event
		self.locked = guildLevel < self.level
		if self.locked:
			self.lockImage.Show()
			self.lockText.Show()
		else:
			self.lockImage.Hide()
			self.lockText.Hide()

	def OnClick(self):
		if self.locked:
			return
		self.page.OnClickItem(self.kind, self.index, self.itemName, self.count, self.price)


class ShopPage(ui.Window):
	"""Reference Shop page: silver shop for the member, gold shop for the guild."""

	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = proxy(owner)
		self.SetSize(624, 315)
		self.keep = []
		self.tab = 0
		self.offset = 0
		self.itemToolTip = None
		self.lockToolTip = None
		self.multiBuy = None
		self.multiBuyDialog = uiPickMoney.PickMoneyDialog()
		self.multiBuyDialog.LoadDialog()
		self.multiBuyDialog.SetTitleName(localeInfo.GUILD_SHOP_MULTI_BUY)
		self.multiBuyDialog.SetAcceptEvent(ui.__mem_func__(self.__OnMultiBuy))
		self.multiBuyDialog.SetMax(3)
		self.multiBuyDialog.Hide()
		wnd = proxy(self)
		self.keep.append(_Image(self, PAGE_5 + "bg_shop.png", 3, 0))
		self.tabs = []
		for (index, key) in enumerate(("GUILD_SHOP_SILVER", "GUILD_SHOP_GOLD")):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetUpVisual(PAGE_5 + "btn_category_norm.png")
			button.SetOverVisual(PAGE_5 + "btn_category_hover.png")
			button.SetDownVisual(PAGE_5 + "btn_category_select.png")
			button.SetPosition(7 + index * 294, 3)
			button.SetEvent(lambda arg = index, page = wnd : page.SelectTab(arg))
			button.Show()
			self.tabs.append(button)
			self.keep.append(_Text(button, getattr(localeInfo, key), 144, 6, TITLE_COLOR, center = True, outline = True))
		self.keep.append(_Image(self, PAGE_5 + "bg_silver.png", 222, 10))
		self.silverText = _Text(self, "", 262, 10, center = True, outline = True)
		self.keep.append(_Image(self, PAGE_5 + "bg_gold.png", 516, 10))
		self.goldText = _Text(self, "", 552, 10, center = True, outline = True)
		self.view = ui.Window()
		self.view.SetParent(self)
		self.view.SetPosition(SHOP_VIEW_X, SHOP_VIEW_Y)
		self.view.SetSize(SHOP_VIEW_WIDTH, SHOP_VIEW_HEIGHT)
		self.view.SetInsideRender(True)
		self.view.Show()
		self.scrollBar = uiMessengerRenewal.ThinScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(588, 36)
		self.scrollBar.SetSize(6, SHOP_VIEW_HEIGHT)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar.Hide()
		self.items = ([], [])
		for (kind, entries) in enumerate((SHOP_SILVER_ITEMS, SHOP_GOLD_ITEMS)):
			for (index, entry) in enumerate(entries):
				self.items[kind].append(ShopItem(self, kind, index, entry))
		self.SelectTab(0)

	def __del__(self):
		ui.Window.__del__(self)

	def SelectTab(self, tab):
		self.tab = tab
		self.offset = 0
		for (index, button) in enumerate(self.tabs):
			if index == tab:
				button.Down()
			else:
				button.SetUp()
		self.Refresh()

	def __MaxScroll(self):
		rows = (len(self.items[self.tab]) + SHOP_COLUMNS - 1) / SHOP_COLUMNS
		return max(0, rows * SHOP_STEP_Y - SHOP_VIEW_HEIGHT)

	def __OnScroll(self, scale):
		self.offset = int(self.__MaxScroll() * scale)
		self.__Layout()

	def Scroll(self, nLen):
		maxScroll = self.__MaxScroll()
		if maxScroll <= 0:
			return
		self.offset = max(0, min(maxScroll, self.offset + (-SCROLL_SPEED if nLen > 0 else SCROLL_SPEED)))
		self.scrollBar.SetPosScale(float(self.offset) / maxScroll)
		self.__Layout()

	def __Layout(self):
		level = guild.GetGuildLevel()
		for (kind, cards) in enumerate(self.items):
			for (index, card) in enumerate(cards):
				y = index / SHOP_COLUMNS * SHOP_STEP_Y - self.offset
				if kind == self.tab and y + SHOP_STEP_Y > 0 and y < SHOP_VIEW_HEIGHT:
					card.SetPosition(15 + index % SHOP_COLUMNS * SHOP_STEP_X, y)
					card.SetLevel(level)
					card.Show()
				else:
					card.Hide()

	def Refresh(self):
		self.silverText.SetText("%d / %d" % (self.owner.guildSilver, SILVER_LIMIT))
		self.goldText.SetText(str(self.owner.guildGold))
		maxScroll = self.__MaxScroll()
		if maxScroll > 0:
			self.scrollBar.SetMiddleBarScale(float(SHOP_VIEW_HEIGHT) / (maxScroll + SHOP_VIEW_HEIGHT))
			self.scrollBar.SetPosScale(float(self.offset) / maxScroll)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()
		self.__Layout()

	def __PriceText(self, kind, price):
		return (localeInfo.GUILD_SHOP_ASK_PRICE_GOLD if kind else localeInfo.GUILD_SHOP_ASK_PRICE) % localeInfo.NumberToMoneyStringNEW(price)

	def __ShowLockToolTip(self, vnum, count, price, kind):
		if not self.lockToolTip:
			self.lockToolTip = uiToolTip.ToolTip(SHOP_LOCK_TOOLTIP_WIDTH)
		tooltip = self.lockToolTip
		item.SelectItem(vnum)
		tooltip.ClearToolTip()
		tooltip.AppendTextLine(item.GetItemName(), tooltip.TITLE_COLOR)
		tooltip.AppendSpace(5)
		center = tooltip.toolTipWidth / 2
		icon = _Image(tooltip, item.GetIconImageFileName(), 0, tooltip.toolTipHeight)
		icon.SetPosition(center - icon.GetWidth() / 2, tooltip.toolTipHeight)
		tooltip.childrenList.append(icon)
		tooltip.AppendSpace(icon.GetHeight() + 3)
		tooltip.AppendTextLine("x%d" % count, TEXT_COLOR)
		tooltip.AppendSpace(4)
		tooltip.AppendTextLine("%s |Eemoji/%s|e" % (localeInfo.NumberToMoneyStringNEW(price), "guild_gold" if kind else "silver"), TEXT_COLOR)
		tooltip.ShowToolTip()

	def OnOverInItem(self, vnum, count, price, kind, locked):
		if locked:
			self.__ShowLockToolTip(vnum, count, price, kind)
			return
		if not self.itemToolTip:
			self.itemToolTip = uiToolTip.ItemToolTip()
		tooltip = self.itemToolTip
		tooltip.ClearToolTip()
		tooltip.toolTipWidth = SHOP_TOOLTIP_WIDTH
		tooltip.AddItemData(vnum, [0, 0, 0, 0, 0, 0], 0)
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", False):
			rows.append((("ctrl", "mouse_right"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		rows.append((("ctrl", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_MULTI_BUY))
		if getattr(app, "ENABLE_WIKI_SYSTEM", False):
			rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
		tooltip.AppendShortcutTable(rows)
		tooltip.ShowToolTip()

	def OnOverOutItem(self):
		if self.itemToolTip:
			self.itemToolTip.HideToolTip()
		if self.lockToolTip:
			self.lockToolTip.HideToolTip()

	def __Ask(self, kind, index, name, count, price, buyCount):
		command = "/guild_shop buy %d %d %d" % (kind, index, buyCount)
		self.owner.AskQuest(localeInfo.GUILD_SHOP_ASK % (name, count * buyCount), command, self.__PriceText(kind, price * buyCount), TEXT_COLOR, SHOP_ASK_WIDTH)

	def __OnMultiBuy(self, buyCount):
		(kind, index, name, count, price) = self.multiBuy
		self.__Ask(kind, index, name, count, price, buyCount)

	def OnClickItem(self, kind, index, name, count, price):
		balance = self.owner.guildGold if kind else self.owner.guildSilver
		maxBuy = min(balance / price, SHOP_MAX_STACK / count)
		if maxBuy > 1 and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)):
			self.multiBuy = (kind, index, name, count, price)
			self.multiBuyDialog.Open(maxBuy, 1)
			return
		self.__Ask(kind, index, name, count, price, 1)


class GuildLogWindow(ui.BoardWithTitleBar):
	"""Reference GuildDonationLog / GuildSkillLog 552x306: seven day tabs, name, action and time."""

	def __init__(self, logType):
		ui.BoardWithTitleBar.__init__(self)
		self.logType = logType
		self.keep = []
		self.rows = []
		self.lines = []
		self.offset = 0
		self.day = 0
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetSize(552, 306)
		self.SetTitleName(localeInfo.GUILD_LOG_TITLE_DONATION if logType == LOG_DONATION else localeInfo.GUILD_LOG_TITLE_SKILL)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.keep.append(_Image(self, GUILD_PATH + "skill_log/background.png", 3, 27))
		self.noLog = _Text(self, localeInfo.GUILD_LOG_EMPTY, 276, 140, TITLE_COLOR, center = True, outline = True)
		self.loading = False
		self.loadingStart = 0.0
		self.loadingRotation = 0
		self.pendingLines = None
		self.loadingImage = _Image(self, COMMON_PATH + "loading_image.tga", 251, 125)
		self.loadingImage.Hide()
		self.dayButtons = []
		for index in xrange(LOG_DAYS):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetUpVisual(COMMON_PATH + "btn_category_norm.png")
			button.SetOverVisual(COMMON_PATH + "btn_category_hover.png")
			button.SetDownVisual(COMMON_PATH + "btn_category_select.png")
			button.SetPosition(3 + index * 78, 271)
			button.SetText(time.strftime("%d-%m-%Y", time.localtime(app.GetGlobalTimeStamp() - index * 86400)))
			button.SetEvent(lambda arg = index, wnd = proxy(self) : wnd.SelectDay(arg))
			button.Show()
			self.dayButtons.append(button)
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def SelectDay(self, day):
		self.day = day
		for (index, button) in enumerate(self.dayButtons):
			if index == day:
				button.Down()
			else:
				button.SetUp()
		self.__StartLoading()
		net.SendChatPacket("/guild_shop log %d %d" % (self.logType, day))

	def SetLines(self, lines):
		self.pendingLines = lines

	def __StartLoading(self):
		self.loading = True
		self.loadingStart = app.GetTime()
		self.pendingLines = None
		self.noLog.Hide()
		for window in self.rows:
			window.Hide()
		self.rows = []
		self.loadingImage.Show()

	def __StopLoading(self, lines):
		self.loading = False
		self.pendingLines = None
		self.loadingImage.Hide()
		self.lines = lines
		self.offset = 0
		self.__Build()

	def OnUpdate(self):
		if not self.loading:
			return
		self.loadingRotation = (self.loadingRotation + LOG_LOADING_ROTATION) % 360
		self.loadingImage.SetRotation(self.loadingRotation)
		elapsed = app.GetTime() - self.loadingStart
		if self.pendingLines is not None and elapsed >= LOG_LOADING_MIN_TIME:
			self.__StopLoading(self.pendingLines)
		elif elapsed >= LOG_LOADING_TIMEOUT:
			self.__StopLoading([])

	def __Build(self):
		for window in self.rows:
			window.Hide()
		self.rows = []
		if self.lines:
			self.noLog.Hide()
		else:
			self.noLog.Show()
		for (index, (name, action, stamp)) in enumerate(self.lines[self.offset:self.offset + LOG_VISIBLE_ROWS]):
			y = 31 + index * LOG_ROW_HEIGHT
			for (x, width) in LOG_ROW_COLUMNS:
				box = ui.Box()
				box.SetParent(self)
				box.AddFlag("not_pick")
				box.SetPosition(x, y)
				box.SetSize(width, LOG_ROW_BOX_HEIGHT)
				box.SetColor(LOG_ROW_COLOR)
				box.Show()
				self.rows.append(box)
			self.rows.append(_Text(self, name, 71, y + 7, center = True, outline = True))
			self.rows.append(_Text(self, action, 272, y + 7, center = True, outline = True))
			self.rows.append(_Text(self, stamp, 471, y + 7, center = True, outline = True))

	def OnRunMouseWheel(self, nLen):
		if self.loading:
			return True
		self.offset = max(0, min(max(0, len(self.lines) - LOG_VISIBLE_ROWS), self.offset + (-1 if nLen > 0 else 1)))
		self.__Build()
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)

	def Open(self):
		self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.SelectDay(0)

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class HitWindow(uiMessengerRenewal.HitWindow):
	def Place(self, x, y, width, height):
		top = max(0, y)
		bottom = min(MEMBER_VIEW_HEIGHT, y + height)
		if bottom <= top:
			self.Hide()
			return
		self.SetPosition(x, top)
		self.SetSize(width, bottom - top)
		self.Show()

	def OnMouseRightButtonDown(self):
		if getattr(self, "onRightClick", None):
			self.onRightClick()
		return True


class MemberPage(ui.Window):
	"""Reference Member page: online count + leader card, then one collapsible group per grade (co leaders first)."""

	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = proxy(owner)
		self.keep = []		# static decorations: a window without a python reference is destroyed at once
		self.SetSize(624, 315)
		self.expanded = {}
		self.scrollPos = 0
		self.contentHeight = 0
		self.rows = []		# (kind, data, y)
		self.visuals = []
		self.hits = []

		self.keep.append(_Image(self, PAGE_3 + "background.png", MEMBER_VIEW_X, MEMBER_VIEW_Y))
		self.view = ui.Window()
		self.view.SetParent(self)
		self.view.SetPosition(MEMBER_VIEW_X, MEMBER_VIEW_Y)
		self.view.SetSize(MEMBER_VIEW_WIDTH, MEMBER_VIEW_HEIGHT)
		self.view.SetInsideRender(True)
		self.view.Show()
		self.content = ui.Window()
		self.content.SetParent(self.view)
		self.content.AddFlag("not_pick")
		self.content.SetSize(MEMBER_VIEW_WIDTH, MEMBER_VIEW_HEIGHT)
		self.content.Show()
		self.scrollBar = uiMessengerRenewal.ThinScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(588, 3)
		self.scrollBar.SetSize(6, 266)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar.Hide()

	# ------------------------------------------------------------------ data
	def __Members(self):
		members = []
		for index in xrange(guild.GetMemberCount()):
			(pid, name, grade, job, level, offer, general) = guild.GetMemberData(index)
			if pid < 0 or not name:
				continue
			members.append({"index" : index, "pid" : pid, "name" : name, "grade" : grade, "job" : job, "level" : level,
				"offer" : offer, "general" : general, "online" : _IsMemberOnline(name)})
		return members

	def __Groups(self, members):
		master = guild.GetGuildMasterName()
		leader = None
		groups = {CO_LEADER : []}
		for grade in xrange(2, MAX_GRADE + 1):
			groups[grade] = []
		for member in members:
			if member["name"] == master:
				leader = member
			elif member["general"]:
				groups[CO_LEADER].append(member)
			else:
				groups[max(2, min(MAX_GRADE, member["grade"]))].append(member)		# the server keeps 15 grades
		for group in groups.values():
			group.sort(key = lambda m : (not m["online"], -m["level"], m["name"].lower()))
		return (leader, groups)

	def __GroupName(self, grade):
		if grade == CO_LEADER:
			return localeInfo.GUILD_WINDOW_CO_LEADER
		return guild.GetGradeName(grade)

	def IsExpanded(self, grade, count):
		return self.expanded.get(grade, count > 0)

	# ------------------------------------------------------------------ build
	def Refresh(self):
		for window in self.visuals + self.hits:
			window.Hide()
		self.visuals = []
		self.hits = []
		self.rows = []

		members = self.__Members()
		(leader, groups) = self.__Groups(members)
		online = len([m for m in members if m["online"]])
		isMaster = player.GetName() == guild.GetGuildMasterName()

		y = 0
		# online counter + leader card
		self.__AddVisual(_Image(self.content, PAGE_3 + "online.png", 11, y + 16))
		self.__AddVisual(_Text(self.content, str(online), 33, y + 17))
		if leader:
			self.__AddCard(leader, (MEMBER_VIEW_WIDTH - LEADER_CARD_WIDTH) / 2, y + 3, True)
		y += TOTAL_ROW_HEIGHT

		for grade in [CO_LEADER] + range(2, MAX_GRADE + 1):
			group = groups[grade]
			expanded = self.IsExpanded(grade, len(group))
			self.__AddHeader(grade, len(group), expanded, y, isMaster and grade != CO_LEADER)
			y += HEADER_HEIGHT
			if expanded and group:
				y += HEADER_GAP
				for start in xrange(0, len(group), CARDS_PER_ROW):
					for (column, member) in enumerate(group[start:start + CARDS_PER_ROW]):
						self.__AddCard(member, CARD_X + column * CARD_STEP, y, False)
					y += CARD_ROW_HEIGHT + CARD_ROW_GAP
		self.contentHeight = y
		self.content.SetSize(MEMBER_VIEW_WIDTH, max(MEMBER_VIEW_HEIGHT, y))
		self.__Layout()

	def __AddVisual(self, window):
		self.visuals.append(window)
		return window

	def __AddHeader(self, grade, count, expanded, y, editable):
		self.__AddVisual(_Image(self.content, PAGE_3 + "collapse.png", 5, y))
		self.__AddVisual(_Text(self.content, "%s (%d)" % (self.__GroupName(grade), count), 16, y + 9, HEADER_COLOR, "Tahoma:13b"))
		self.__AddVisual(_Image(self.content, "d:/ymir work/ui/game/advanced_game_options/" + ("arrow_up.png" if expanded else "arrow_down.png"), 562, y + 14))
		owner = self.owner
		hit = HitWindow(self.view)
		hit.onClick = lambda arg = grade : owner.OnToggleGrade(arg)
		self.rows.append(("hit", hit, (5, y, 582, HEADER_HEIGHT)))
		self.hits.append(hit)
		if editable:
			edit = ui.Button()
			edit.SetParent(self.view)
			edit.SetUpVisual("d:/ymir work/ui/game/fast_equip/edit_norm.png")
			edit.SetOverVisual("d:/ymir work/ui/game/fast_equip/edit_hover.png")
			edit.SetDownVisual("d:/ymir work/ui/game/fast_equip/edit_select.png")
			edit.SetEvent(lambda arg = grade : owner.OnClickGradeConfig(arg))
			self.rows.append(("button", edit, (537, y + 9, 19, 17)))
			self.hits.append(edit)

	def __AddCard(self, member, x, y, isLeader):
		background = "bg_leader.png" if isLeader else "bg_opponent.png"
		self.__AddVisual(_Image(self.content, PAGE_3 + background, x, y))
		job = member["job"] % len(JOB_ICONS)
		self.__AddVisual(_Image(self.content, PAGE_3 + JOB_ICONS[job], x + 2, y + 2))
		self.__AddVisual(_Text(self.content, "%s [%d]" % (member["name"], member["level"]), x + 48, y + 2, NAME_COLOR))
		self.__AddVisual(_Text(self.content, localeInfo.GUILD_WINDOW_TOTAL_DONATION % member["offer"], x + 48, y + 16, TEXT_COLOR, "Tahoma:11"))
		self.__AddVisual(_Text(self.content, localeInfo.GUILD_WINDOW_DAILY_DONATION % (self.owner.memberDaily.get(member["pid"], 0), DAILY_DONATION_MAX), x + 48, y + 28, TEXT_COLOR, "Tahoma:11"))
		if isLeader:
			self.__AddVisual(_Image(self.content, PAGE_3 + ("crown.png" if member["online"] else "crown_offline.png"), x + 265, y + 14))
		else:
			self.__AddVisual(_Image(self.content, PAGE_3 + ("online.png" if member["online"] else "offline.png"), x + 167, y + 13))
		owner = self.owner
		hit = HitWindow(self.view)
		hit.onDoubleClick = lambda arg = member["name"] : owner.OnWhisperMember(arg)
		hit.onRightClick = lambda arg = member : owner.OnRightClickMember(arg)
		self.rows.append(("hit", hit, (x, y, LEADER_CARD_WIDTH if isLeader else CARD_WIDTH, CARD_ROW_HEIGHT)))
		self.hits.append(hit)

	# ------------------------------------------------------------------ scroll / layout
	def __MaxScroll(self):
		return max(0, self.contentHeight - MEMBER_VIEW_HEIGHT)

	def __Layout(self):
		self.scrollPos = max(0, min(self.scrollPos, self.__MaxScroll()))
		offset = -self.scrollPos
		self.content.SetPosition(0, offset)
		for (kind, window, (x, y, w, h)) in self.rows:
			if kind == "hit":
				window.Place(x, y + offset, w, h)
			elif y + offset >= 0 and y + offset + h <= MEMBER_VIEW_HEIGHT:
				window.SetPosition(x, y + offset)
				window.Show()
			else:
				window.Hide()
		if self.__MaxScroll() > 0:
			self.scrollBar.SetMiddleBarScale(float(MEMBER_VIEW_HEIGHT) / self.contentHeight)
			self.scrollBar.SetPosScale(float(self.scrollPos) / self.__MaxScroll())
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def __OnScroll(self, scale):
		self.scrollPos = int(self.__MaxScroll() * scale)
		self.__Layout()

	def Scroll(self, nLen):
		if self.__MaxScroll() <= 0:
			return
		self.scrollPos += -SCROLL_SPEED if nLen > 0 else SCROLL_SPEED
		self.__Layout()

	def ToggleGrade(self, grade, count):
		self.expanded[grade] = not self.IsExpanded(grade, count)
		self.Refresh()


class GradeCheckRow(ui.Window):
	"""Reference CheckBox of GuildGradePermissionDropdown: box and text, the whole row is clickable."""

	def __init__(self, parent, x, y, text, event):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(GRADE_ROW_WIDTH, GRADE_ROW_STEP)
		self.event = event
		self.box = _Image(self, COMMON_PATH + "checkbox_new_unselected.png", 0, 2)
		self.check = _Image(self, COMMON_PATH + "checkbox_new_selected.png", 0, 2)
		self.text = _Text(self, text, 20, 0)
		self.check.Hide()

	def SetChecked(self, checked):
		if checked:
			self.check.Show()
		else:
			self.check.Hide()

	def OnMouseLeftButtonDown(self):
		# plain windows never get the button-up of a click they did not capture: run on press
		self.event()
		return True


class GradeConfigDialog(ui.BoardWithTitleBar):
	"""Reference GuildGradeConfigDialog 380x155, 354 with the permission list open."""

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.keep = []
		self.grade = 0
		self.authority = 0
		self.isExpanded = False
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetTitleName(localeInfo.GUILD_GRADE_CONFIG_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.background = _ThinBoard(self, 8, 30, 364, GRADE_HEIGHT - 38)
		self.keep.append(_Text(self, localeInfo.GUILD_GRADE_CONFIG_NAME, 18, 43))
		self.keep.append(_Image(self, COMMON_PATH + "editline_bg.png", 196, 37))
		self.nameEdit = ui.EditLine()
		self.nameEdit.SetParent(self)
		self.nameEdit.SetPosition(203, 42)
		self.nameEdit.SetSize(143, 18)
		self.nameEdit.SetMax(GRADE_NAME_MAX)
		self.nameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.nameEdit.Show()
		self.keep.append(_Text(self, localeInfo.GUILD_GRADE_CONFIG_PERMISSIONS, 18, 77))
		self.selectButton = _Button(self, GUILD_APPLICATION_PATH + "select_input", 218, 71)
		self.selectButton.SetEvent(ui.__mem_func__(self.__OnToggleList))
		self.selectText = _Text(self.selectButton, "", 8, 4)
		self.listBoard = _ThinBoard(self, 26, 98, 330, 210)
		self.listBoard.Hide()
		self.rows = []
		dialog = proxy(self)
		for (index, (key, bit)) in enumerate(GRADE_PERMISSIONS):
			row = GradeCheckRow(self, 34, 104 + index * GRADE_ROW_STEP, getattr(localeInfo, key), lambda arg = bit, wnd = dialog : wnd.Toggle(arg))
			self.rows.append((row, bit))
		self.acceptButton = _Button(self, PAGE_1 + "btn_upload", 90, 116, localeInfo.GUILD_GRADE_CONFIG_ACCEPT)
		self.acceptButton.SetEvent(ui.__mem_func__(self.__OnAccept))
		self.cancelButton = _Button(self, PAGE_1 + "btn_upload", 202, 116, localeInfo.GUILD_GRADE_CONFIG_CANCEL)
		self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.__SetExpanded(False)
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __SetExpanded(self, expanded):
		self.isExpanded = expanded
		height = GRADE_EXPANDED_HEIGHT if expanded else GRADE_HEIGHT
		self.SetSize(GRADE_WIDTH, height)
		self.background.SetSize(364, height - 38)
		self.acceptButton.SetPosition(90, height - 39)
		self.cancelButton.SetPosition(202, height - 39)
		for (row, bit) in self.rows:
			if expanded:
				row.Show()
			else:
				row.Hide()
		if expanded:
			self.listBoard.Show()
		else:
			self.listBoard.Hide()

	def __OnToggleList(self):
		self.__SetExpanded(not self.isExpanded)

	def __Refresh(self):
		count = 0
		for (row, bit) in self.rows:
			checked = bool(self.authority & bit)
			row.SetChecked(checked)
			if checked:
				count += 1
		self.selectText.SetText(localeInfo.GUILD_GRADE_CONFIG_SELECTED % count)

	def Open(self, grade):
		self.grade = grade
		(name, self.authority) = guild.GetGradeData(grade)
		self.nameEdit.SetText(name)
		self.__Refresh()
		self.__SetExpanded(False)
		self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.nameEdit.SetFocus()

	def Toggle(self, bit):
		self.authority ^= bit
		self.__Refresh()

	def __OnAccept(self):
		(name, authority) = guild.GetGradeData(self.grade)
		newName = self.nameEdit.GetText().strip()
		if newName and newName != name:
			net.SendGuildChangeGradeNamePacket(self.grade, newName)
		if self.authority != authority:
			net.SendGuildChangeGradeAuthorityPacket(self.grade, self.authority)
		self.Close()

	def Close(self):
		self.nameEdit.KillFocus()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class GradeSelectRow(ui.Window):
	"""Reference GuildMemberContextMenuItem of GuildMemberGradeDialog."""

	def __init__(self, parent, x, y, text, selected, event):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(GRADE_SELECT_ROW_WIDTH, GRADE_SELECT_ROW_HEIGHT)
		self.event = event
		self.normalColor = GRADE_SELECT_CURRENT_COLOR if selected else GRADE_SELECT_NORMAL_COLOR
		self.bg = ui.Bar()
		self.bg.SetParent(self)
		self.bg.AddFlag("not_pick")
		self.bg.SetPosition(0, 0)
		self.bg.SetSize(GRADE_SELECT_ROW_WIDTH, GRADE_SELECT_ROW_HEIGHT)
		self.bg.SetColor(self.normalColor)
		self.bg.Show()
		self.text = _Text(self, text, 10, 6, GRADE_SELECT_HOVER_TEXT_COLOR if selected else GRADE_SELECT_TEXT_COLOR)
		self.textColor = GRADE_SELECT_HOVER_TEXT_COLOR if selected else GRADE_SELECT_TEXT_COLOR
		self.Show()

	def OnMouseOverIn(self):
		self.bg.SetColor(GRADE_SELECT_HOVER_COLOR)
		self.text.SetPackedFontColor(GRADE_SELECT_HOVER_TEXT_COLOR)

	def OnMouseOverOut(self):
		self.bg.SetColor(self.normalColor)
		self.text.SetPackedFontColor(self.textColor)

	def OnMouseLeftButtonDown(self):
		self.event()
		return True


class GradeSelectDialog(ui.BoardWithTitleBar):
	"""Reference GuildMemberGradeDialog 237 wide: one row per grade, the current grade is marked."""

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.pid = 0
		self.keep = []
		self.rows = []
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetTitleName(localeInfo.GUILD_WINDOW_MENU_CHANGE_GRADE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.SetSize(GRADE_SELECT_WIDTH, 270)
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Open(self, pid, currentGrade):
		self.pid = pid
		for window in self.keep + self.rows:
			window.Hide()
		self.keep = []
		self.rows = []
		dialog = proxy(self)
		count = MAX_GRADE - 1
		height = 30 + 8 + count * GRADE_SELECT_ROW_STEP + 16
		self.SetSize(GRADE_SELECT_WIDTH, height)
		background = ui.OutlineWindow()
		background.SetParent(self)
		background.AddFlag("not_pick")
		background.SetPosition(8, 30)
		background.MakeOutlineWindow(213, height - 38)
		background.Show()
		self.keep.append(background)
		for (index, grade) in enumerate(xrange(2, MAX_GRADE + 1)):
			y = 38 + index * GRADE_SELECT_ROW_STEP
			self.rows.append(GradeSelectRow(self, 18, y, guild.GetGradeName(grade), grade == currentGrade, lambda arg = grade, wnd = dialog : wnd.Select(arg)))
			if index + 1 < count:
				line = ui.Bar()
				line.SetParent(self)
				line.AddFlag("not_pick")
				line.SetPosition(18, y + GRADE_SELECT_ROW_HEIGHT + 1)
				line.SetSize(GRADE_SELECT_ROW_WIDTH, 1)
				line.SetColor(GRADE_SELECT_LINE_COLOR)
				line.Show()
				self.keep.append(line)
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def Select(self, grade):
		net.SendGuildChangeMemberGradePacket(self.pid, grade)
		self.Close()

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class WhisperDialog(ui.BoardWithTitleBar):
	"""Reference SendWhisperGuild 315x240: the text goes to every guild member for guild gold."""

	def __init__(self, owner):
		ui.BoardWithTitleBar.__init__(self)
		self.owner = proxy(owner)
		self.keep = []
		self.popup = None
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetSize(315, 240)
		self.SetTitleName(localeInfo.GUILD_WHISPER_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.keep.append(_ThinBoard(self, 8, 30, 298, 202))
		self.keep.append(_Text(self, localeInfo.GUILD_WHISPER_DESC, 157, 37, center = True, outline = True))
		for (y, height) in ((55, 25), (85, 100)):
			bar = ui.Bar()
			bar.SetParent(self)
			bar.AddFlag("not_pick")
			bar.SetPosition(15, y)
			bar.SetSize(284, height)
			bar.SetColor(0xff000000)
			bar.Show()
			self.keep.append(bar)
		self.senderEdit = self.__Edit(25, 60, 264, 15, WHISPER_SENDER_MAX, localeInfo.GUILD_WHISPER_SENDER)
		self.textEdit = self.__Edit(25, 95, 264, 80, WHISPER_TEXT_MAX, localeInfo.GUILD_WHISPER_TEXT)
		self.textEdit.SetLimitWidth(264)
		self.textEdit.SetMultiLine()
		self.senderEdit.SetTabEvent(ui.__mem_func__(self.textEdit.SetFocus))
		self.textEdit.SetTabEvent(ui.__mem_func__(self.senderEdit.SetFocus))
		self.sendButton = _Button(self, COMMON_PATH + "btn", 88, 192, " ")
		self.sendButton.SetEvent(ui.__mem_func__(self.__OnSend))
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __Edit(self, x, y, width, height, limit, hint):
		edit = ui.EditLine()
		edit.SetParent(self)
		edit.SetPosition(x, y)
		edit.SetSize(width, height)
		edit.SetMax(limit)
		edit.SetPackedFontColor(TEXT_COLOR)
		edit.SetInfoMessage(hint)
		edit.SetReturnEvent(ui.__mem_func__(self.__OnSend))
		edit.SetEscapeEvent(ui.__mem_func__(self.Close))
		edit.Show()
		return edit

	def RefreshPrice(self):
		color = "|cFFDC143C" if self.owner.guildGold < WHISPER_PRICE else "|cFFFFFFFF"
		self.sendButton.SetText(localeInfo.GUILD_WHISPER_PRICE % (color, WHISPER_PRICE))
		self.sendButton.ButtonText.SetPosition(self.sendButton.GetWidth() / 2, self.sendButton.GetHeight() / 2 - 1)

	def __OnSend(self):
		text = self.textEdit.GetText().replace("\n", " ").strip()
		if not text:
			return
		sender = self.senderEdit.GetText()
		for character in WHISPER_SENDER_INVALID:
			if character in sender:
				self.popup = uiCommon.PopupDialog()
				self.popup.SetWidth(WHISPER_POPUP_WIDTH)
				self.popup.SetText(localeInfo.GUILD_WHISPER_INVALID_SENDER)
				self.popup.Open()
				return
		net.SendChatPacket("/guild_whisper %s %s" % (sender or player.GetName(), text))
		self.textEdit.SetText("")

	def Open(self):
		self.senderEdit.SetText("")
		self.textEdit.SetText("")
		self.RefreshPrice()
		self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.textEdit.SetFocus()

	def Close(self):
		self.senderEdit.KillFocus()
		self.textEdit.KillFocus()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class DonationDialog(ui.BoardWithTitleBar):
	"""Reference DonationGoldWindow 300x225: two donation cards side by side."""

	def __init__(self, owner):
		ui.BoardWithTitleBar.__init__(self)
		self.owner = proxy(owner)
		self.keep = []
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetSize(300, 225)
		self.SetTitleName(localeInfo.GUILD_DONATION_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.keep.append(_ThinBoard(self, 8, 30, 283, 187))
		self.keep.append(_Text(self, localeInfo.GUILD_DONATION_DESC_1, 150, 42, center = True, outline = True))
		self.keep.append(_Text(self, localeInfo.GUILD_DONATION_DESC_2, 150, 57, center = True, outline = True))
		self.dailyTexts = []
		for (index, (name, icon, price, limit, gain)) in enumerate(DONATIONS):
			x = 33 + index * 125
			center = x + 55
			self.keep.append(_Image(self, PAGE_1 + "item_bg.png", x, 80))
			self.keep.append(_Image(self, COMMON_PATH + "slots/slot_32x32.png", x + 39, 88))
			self.keep.append(_Image(self, "icon/item/item_guild/" + icon, x + 39, 88))
			self.keep.append(_Text(self, getattr(localeInfo, name), center, 123, DONATION_NAME_COLOR, center = True, outline = True))
			if price:
				self.keep.append(_Text(self, localeInfo.GUILD_DONATION_PRICE % localeInfo.NumberToMoneyStringNEW(price), center, 137, center = True, outline = True))
			self.dailyTexts.append(_Text(self, "", center, 150, center = True, outline = True))
			button = _Button(self, COMMON_PATH + "button_xsmall", x + 17, 166, localeInfo.GUILD_DONATION_BUTTON, DONATION_BUTTON_COLOR)
			button.SetEvent(lambda arg = index, dialog = proxy(self) : dialog.Donate(arg))
			self.keep.append(button)
			self.keep.append(_Text(self, localeInfo.GUILD_DONATION_RESULT % (gain, gain), center - 2, 195, center = True, outline = True))
		self.helpButton = ui.Button()
		self.helpButton.SetParent(self)
		self.helpButton.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		self.helpButton.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
		self.helpButton.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		self.helpButton.SetPosition(250, 9)
		self.helpButton.ShowToolTip = ui.__mem_func__(self.__ShowHelp)
		self.helpButton.HideToolTip = ui.__mem_func__(self.__HideHelp)
		self.helpButton.Show()
		self.helpToolTip = DonationHelpToolTip()
		self.helpToolTip.HideToolTip()
		self.Hide()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __ShowHelp(self):
		self.helpToolTip.ShowToolTip()

	def __HideHelp(self):
		self.helpToolTip.HideToolTip()

	def Refresh(self):
		daily = self.owner.dailyDonation
		for (index, (name, icon, price, limit, gain)) in enumerate(DONATIONS):
			self.dailyTexts[index].SetText(localeInfo.GUILD_DONATION_DAILY % (daily[index], limit))

	def Donate(self, index):
		net.SendChatPacket("/donate_to_guild %d" % index)

	def Open(self):
		self.Refresh()
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def Close(self):
		self.helpToolTip.HideToolTip()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()


class GuildWindow(ui.Window):

	def __init__(self):
		self.__dict__["legacy"] = uiGuild.GuildWindow()	# everything the new pages do not cover
		ui.Window.__init__(self)
		self.guildGold = 0
		self.guildSilver = 0
		self.dailyDonation = [0, 0]
		self.memberDaily = {}		# pid : today's donation count
		self.whisperDialog = None
		self.donationDialog = None
		self.memberToolTip = None
		self.blessingToolTip = None
		self.questData = {"used" : [0, 0, 0, 0], "type" : 0, "level" : 0, "cur" : 0, "target" : 0}
		self.questEndTime = 0
		self.questQuestion = None
		self.questRank = None
		self.questRankRows = []
		self.questSideBar = None
		self.logWindows = {}
		self.logType = 0
		self.logLines = []
		self.skillData = {"exp" : 0, "points" : 0, "energy" : 0, "maxEnergy" : 0, "activeLeft" : 0, "levels" : [0] * 10}
		self.currentPage = PAGE_INFO
		self.questionDialog = None
		self.contextMenu = None
		self.gradeConfig = None
		self.gradeSelect = None
		self.pages = {}
		self.isLoaded = False		# built on the first Open, so a page asset problem can never break entering the game

	def __del__(self):
		ui.Window.__del__(self)

	def __getattr__(self, name):
		legacy = self.__dict__.get("legacy")
		if legacy is None or name.startswith("__"):
			raise AttributeError(name)
		return getattr(legacy, name)

	def __LoadWindow(self):
		self.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.AddFlag("attach")
		self.board.SetSize(614, 314)
		self.board.SetPosition(35, 0)
		self.board.SetTitleName(localeInfo.GUILD_WINDOW_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.Show()

		self.gradeConfig = GradeConfigDialog()
		self.gradeSelect = GradeSelectDialog()
		self.whisperDialog = WhisperDialog(self)
		self.donationDialog = DonationDialog(self)
		self.questQuestion = QuestQuestionDialog()
		self.questRank = QuestRankWindow()
		self.pages = {PAGE_INFO : InfoPage(self), PAGE_MEMBER : MemberPage(self), PAGE_QUEST : QuestPage(self), PAGE_SKILL : SkillPage(self), PAGE_SHOP : ShopPage(self)}
		for page in self.pages.values():
			page.SetParent(self)
			page.SetPosition(PAGE_X, PAGE_Y)
			page.Hide()

		self.tabs = {}
		for (index, (page, image)) in enumerate(TABS):
			tab = ui.RadioButton()
			tab.SetParent(self)
			tab.SetUpVisual(GUILD_PATH + image + "_norm.png")
			tab.SetOverVisual(GUILD_PATH + image + "_hover.png")
			tab.SetDownVisual(GUILD_PATH + image + "_select.png")
			tab.SetPosition(0, TAB_Y + index * TAB_STEP)
			tab.SetEvent(lambda arg = page, wnd = proxy(self) : wnd.SelectPage(arg))
			if page not in READY_PAGES:
				tab.Disable()
			tab.Show()
			self.tabs[page] = tab
		self.SetCenterPosition()
		self.Hide()

	# ------------------------------------------------------------------ open / close
	def CanOpen(self):
		return guild.IsGuildEnable()

	def Open(self):
		if not self.isLoaded:
			self.__LoadWindow()
			self.isLoaded = True
		self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.SelectPage(self.currentPage)
		net.SendChatPacket("/guild_donation_info")

	def Close(self):
		if self.whisperDialog:
			self.whisperDialog.Close()
		if self.donationDialog:
			self.donationDialog.Close()
		if self.memberToolTip:
			self.memberToolTip.HideToolTip()
		self.HideBlessingToolTip()
		if self.questQuestion:
			self.questQuestion.Close()
		if self.questRank:
			self.questRank.Close()
		if self.gradeConfig:
			self.gradeConfig.Close()
		if self.gradeSelect:
			self.gradeSelect.Close()
		if self.contextMenu:
			self.contextMenu.Close()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		return self.Close()

	def Destroy(self):
		self.Close()
		if self.questSideBar:
			self.questSideBar.Hide()
		legacy = self.__dict__.get("legacy")
		if legacy:
			legacy.Destroy()

	def DeleteGuild(self):
		self.legacy.DeleteGuild()
		self.SetQuestInfo(0, 0, 0, 0, 0, 0, 0, 0, 0)
		self.Close()

	def SelectPage(self, pageName):
		if pageName not in self.pages:
			pageName = PAGE_INFO
		self.currentPage = pageName
		for (name, page) in self.pages.items():
			if name == pageName:
				page.Show()
				page.Refresh()
				if name == PAGE_QUEST:
					page.Request()
				elif name == PAGE_SKILL:
					net.SendChatPacket("/guild_skill info")
				elif name == PAGE_SHOP:
					net.SendChatPacket("/guild_donation_info")
			else:
				page.Hide()
		for (name, tab) in self.tabs.items():
			if name == pageName:
				tab.Down()
			else:
				tab.SetUp()

	def OnRunMouseWheel(self, nLen):
		if self.currentPage in self.pages:
			self.pages[self.currentPage].Scroll(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)

	# ------------------------------------------------------------------ refresh entry points (game / interface)
	def RefreshGuildInfoPage(self):
		self.legacy.RefreshGuildInfoPage()
		if self.IsShow() and self.currentPage == PAGE_INFO:
			self.pages[PAGE_INFO].Refresh()

	def RefreshGuildMemberPage(self):
		self.legacy.RefreshGuildMemberPage()
		if self.IsShow() and self.currentPage == PAGE_MEMBER:
			self.pages[PAGE_MEMBER].Refresh()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.legacy.RefreshGuildMemberPageGradeComboBox()
		self.RefreshGuildMemberPage()

	def RefreshGuildGradePage(self):
		self.legacy.RefreshGuildGradePage()
		self.RefreshGuildMemberPage()

	# ------------------------------------------------------------------ info page actions
	def OnClickMark(self):
		self.legacy._GuildWindow__OnClickSelectGuildMarkButton()

	def OnClickManagement(self):
		if app.ENABLE_GUILD_APPLICATION:
			import uiGuildApplication
			uiGuildApplication.Toggle(uiGuildApplication.MODE_MEMBER)

	def OnClickDonate(self):
		self.donationDialog.Open()

	def OnClickWhisper(self):
		self.whisperDialog.Open()

	def OnClickDeclareWar(self):
		self.legacy._GuildWindow__OnClickDeclareWarButton()

	def ShowBlessingToolTip(self, index):
		if not self.blessingToolTip:
			self.blessingToolTip = BlessingToolTip()
		self.blessingToolTip.SetBlessing(index)
		self.blessingToolTip.ShowToolTip()

	def HideBlessingToolTip(self):
		if self.blessingToolTip:
			self.blessingToolTip.HideToolTip()

	def OnOverInMembers(self):
		if not self.memberToolTip:
			self.memberToolTip = MemberToolTip()
		(curMember, maxMember) = guild.GetGuildMemberCount()
		tooltip = self.memberToolTip
		tooltip.ClearToolTip()
		for key in ("GUILD_WINDOW_MEMBER_TOOLTIP_1", "GUILD_WINDOW_MEMBER_TOOLTIP_2", "GUILD_WINDOW_MEMBER_TOOLTIP_3", "GUILD_WINDOW_MEMBER_TOOLTIP_4"):
			tooltip.AppendTextLine(getattr(localeInfo, key), TEXT_COLOR)
		tooltip.AppendSpace(8)
		tooltip.AppendTextLine(localeInfo.GUILD_WINDOW_MEMBER_TOOLTIP_AURA % 0, MEMBER_TOOLTIP_AURA_COLOR)
		tooltip.AppendTextLine(localeInfo.GUILD_WINDOW_MEMBER_TOOLTIP_COUNT % (curMember, maxMember), MEMBER_TOOLTIP_COUNT_COLOR)
		(x, y) = self.pages[PAGE_INFO].memberHover.GetGlobalPosition()
		tooltip.SetAnchorY(y + MEMBER_HOVER_HEIGHT + 2)
		tooltip.ShowToolTip()

	def OnOverOutMembers(self):
		if self.memberToolTip:
			self.memberToolTip.HideToolTip()

	# ------------------------------------------------------------------ server commands (ENABLE_GUILD_SHOP)
	def OpenLog(self, logType):
		if logType not in self.logWindows:
			self.logWindows[logType] = GuildLogWindow(logType)
		self.logWindows[logType].Open()

	def LogBegin(self, logType):
		self.logType = logType
		self.logLines = []

	def LogRow(self, stamp, name, a, b, c):
		if self.logType == LOG_DONATION:
			item.SelectItem(a)
			exp = localeInfo.NumberToMoneyStringNEW(c)
			text = localeInfo.GUILD_LOG_DONATION % (b, item.GetItemName(), exp, exp)
		elif a < 0:
			text = localeInfo.GUILD_LOG_ACTIVATE % b
		else:
			text = localeInfo.GUILD_LOG_UPGRADE % (getattr(localeInfo, "GUILD_SKILL_NAME_%d" % a), b)
		self.logLines.append((name, text, stamp))

	def LogEnd(self):
		if self.logType in self.logWindows:
			self.logWindows[self.logType].SetLines(self.logLines)

	def OnShopResult(self, result):
		key = "GUILD_SHOP_RESULT_%d" % result
		if hasattr(localeInfo, key):
			chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, key))
		net.SendChatPacket("/guild_donation_info")

	# ------------------------------------------------------------------ server commands (ENABLE_GUILD_SKILL_TREE)
	def SetSkillInfo(self, exp, points, energy, maxEnergy, activeLeft, levels):
		self.skillData = {"exp" : exp, "points" : points, "energy" : energy, "maxEnergy" : maxEnergy, "activeLeft" : activeLeft, "levels" : levels}
		if self.isLoaded and self.IsShow() and self.currentPage == PAGE_SKILL:
			self.pages[PAGE_SKILL].Refresh()

	def OnSkillResult(self, result, value):
		key = "GUILD_SKILL_RESULT_%d" % result
		if not hasattr(localeInfo, key):
			return
		text = getattr(localeInfo, key)
		if result == 3:
			text = text % value
		if result in (8, 11):
			self.__ShowSkillPopup(text)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	def __ShowSkillPopup(self, text):
		self.skillPopup = uiCommon.PopupDialog()
		self.skillPopup.SetWidth(SKILL_TOOLTIP_WIDTH)
		self.skillPopup.SetText(text)
		self.skillPopup.Open()

	def AskSkillActivation(self):
		data = self.skillData
		if not sum(data["levels"][:SKILL_ACTIVE_COUNT]):
			self.__ShowSkillPopup(localeInfo.GUILD_SKILL_RESULT_8)
			return
		hours = SKILL_DURATIONS[0]
		reduction = SKILL_VALUES[9][data["levels"][9]]
		cost = SKILL_ENERGY_COST * hours / 3
		cost -= cost * reduction / 100
		self.questQuestion.Open(localeInfo.GUILD_SKILL_ACTIVATE_QUESTION % hours, "/guild_skill activate %d" % hours, localeInfo.GUILD_SKILL_ACTIVATE_COST % cost)

	# ------------------------------------------------------------------ server commands (ENABLE_GUILD_QUEST)
	def SetQuestInfo(self, used1, used2, used3, used4, questType, level, cur, target, secondsLeft):
		self.questData = {"used" : [used1, used2, used3, used4], "type" : questType, "level" : level, "cur" : cur, "target" : target}
		self.questEndTime = app.GetTime() + secondsLeft
		if self.isLoaded and self.IsShow() and self.currentPage == PAGE_QUEST:
			self.pages[PAGE_QUEST].Refresh()
		if questType and not self.questSideBar:
			self.questSideBar = QuestSideBar(self)
		if self.questSideBar:
			self.questSideBar.Refresh()

	def SetQuestProgress(self, cur, target):
		self.questData["cur"] = cur
		self.questData["target"] = target
		if self.isLoaded and self.IsShow() and self.currentPage == PAGE_QUEST:
			self.pages[PAGE_QUEST].Refresh()
		if self.questSideBar:
			self.questSideBar.Refresh()

	def GetQuestSecondsLeft(self):
		quest = self.questData
		if quest["target"] > 0 and quest["cur"] >= quest["target"]:
			return 0
		return max(0, int(self.questEndTime - app.GetTime()))

	def AskQuest(self, text, command, line2 = None, color = TEXT_COLOR, width = 0):
		self.questQuestion.Open(text, command, line2, color, width)

	def OpenQuestRank(self):
		self.questRank.Open()
		net.SendChatPacket("/guild_quest rank")

	def QuestRankBegin(self):
		self.questRankRows = []

	def QuestRankRow(self, name, score):
		self.questRankRows.append((name, score))

	def QuestRankEnd(self):
		if self.questRank:
			self.questRank.SetRows(self.questRankRows[:QUEST_RANK_ROWS])

	def OnQuestResult(self, result):
		if result in QUEST_RESULTS:
			chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, QUEST_RESULTS[result]))

	# ------------------------------------------------------------------ server commands (ENABLE_GUILD_DONATION)
	def SetGuildGold(self, gold):
		self.guildGold = gold
		if self.isLoaded and self.IsShow() and self.currentPage in (PAGE_INFO, PAGE_SHOP):
			self.pages[self.currentPage].Refresh()
		if self.whisperDialog:
			self.whisperDialog.RefreshPrice()

	def SetGuildSilver(self, silver):
		self.guildSilver = silver
		if self.isLoaded and self.IsShow() and self.currentPage == PAGE_SHOP:
			self.pages[PAGE_SHOP].Refresh()

	def SetDailyDonation(self, scroll, premium):
		self.dailyDonation = [scroll, premium]
		if self.donationDialog:
			self.donationDialog.Refresh()

	def SetMemberDailyDonation(self, payload):
		for entry in payload.split(","):
			if ":" in entry:
				(pid, amount) = entry.split(":")
				self.memberDaily[int(pid)] = int(amount)
		if self.isLoaded and self.IsShow() and self.currentPage == PAGE_MEMBER:
			self.pages[PAGE_MEMBER].Refresh()

	def OnDonationResult(self, result):
		if result in DONATION_MESSAGES:
			chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, DONATION_MESSAGES[result]))
		if result == DONATION_RESULT_WHISPER_SENT and self.whisperDialog:
			self.whisperDialog.Close()

	# ------------------------------------------------------------------ member page actions
	def OnToggleGrade(self, grade):
		page = self.pages[PAGE_MEMBER]
		count = 0
		for index in xrange(guild.GetMemberCount()):
			(pid, name, memberGrade, job, level, offer, general) = guild.GetMemberData(index)
			if (grade == CO_LEADER and general) or (grade != CO_LEADER and memberGrade == grade and not general):
				count += 1
		page.ToggleGrade(grade, count)

	def OnClickGradeConfig(self, grade):
		self.gradeConfig.Open(grade)

	def OnWhisperMember(self, name):
		if name == player.GetName():
			return
		interface = constInfo.GetInterfaceInstance()
		if interface:
			interface.OpenWhisperDialog(name)

	def OnRightClickMember(self, member):
		name = member["name"]
		if name == player.GetName():
			return
		isMaster = player.GetName() == guild.GetGuildMasterName()
		entries = [
			(localeInfo.GUILD_WINDOW_MENU_WHISPER, "menu_whisper.png", lambda : self.OnWhisperMember(name)),
			(localeInfo.GUILD_WINDOW_MENU_COPY_NAME, "menu_copy.png", lambda : self.__CopyName(name)),
			(localeInfo.GUILD_WINDOW_MENU_ADD_FRIEND, "menu_add_friend.png", lambda : net.SendMessengerAddByNamePacket(name)),
		]
		if isMaster and name != guild.GetGuildMasterName():
			entries.append((localeInfo.GUILD_WINDOW_MENU_CHANGE_GRADE, "menu_change_rank.png", lambda : self.gradeSelect.Open(member["pid"], member["grade"])))
			if member["general"]:
				entries.append((localeInfo.GUILD_WINDOW_MENU_REMOVE_CO_LEADER, "menu_remove_co_leader.png", lambda : net.SendGuildChangeMemberGeneralPacket(member["pid"], 0)))
			else:
				entries.append((localeInfo.GUILD_WINDOW_MENU_PROMOTE_CO_LEADER, "menu_promote_co_leader.png", lambda : net.SendGuildChangeMemberGeneralPacket(member["pid"], 1)))
		if name != guild.GetGuildMasterName() and guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER):
			entries.append((localeInfo.GUILD_WINDOW_MENU_EXCLUDE, "menu_exclude.png", lambda : self.__AskExclude(name, member["pid"])))
		if not self.contextMenu:
			import uiWhisperMessenger
			self.contextMenu = uiWhisperMessenger.MessageContextMenu()
		self.contextMenu.Open(entries)

	def __CopyName(self, name):
		ime.CopyTextToClipBoard(name)

	def __AskExclude(self, name, pid):
		dialog = uiCommon.QuestionDialog()
		dialog.SetText(localeInfo.GUILD_WINDOW_EXCLUDE_QUESTION % name)
		dialog.SetAcceptEvent(lambda : self.__Exclude(pid))
		dialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestion))
		dialog.Open()
		self.questionDialog = dialog

	def __Exclude(self, pid):
		net.SendGuildRemoveMemberPacket(pid)
		self.__CloseQuestion()

	def __CloseQuestion(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None
