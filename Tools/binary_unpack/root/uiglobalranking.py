if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import guild
import localeInfo
import uiToolTip

# Baerim uiRanking (Cython) rebuilt from its runtime dump (Reference/pydump_runtime/rankprobe):
# window layout, category tree (CATEGORY_BUTTONS) and the /global_rank protocol are theirs.

RANKING_PATH = "d:/ymir work/ui/game/ranking/%s"
ARROW_PATH = "ikashop/sama/private_search/categories/arrow_%s.png"

PAGE_PLAYER = 0
PAGE_GUILD = 1
PAGE_MEMBER = 2

ROW_COUNT = 10
TEXT_COLOR = 0xffb1a085	# Baerim beige: column titles and rows (same as the list and the timer)
LIST_COLOR = 0xffb1a085	# left category list: same beige as the reset timer (user, 2026-11-06)
TIME_COLOR = 0xffb1a085	# Baerim reset timer, measured pixel-exact on their screen (outlined)
LOADING_SPEED = 360.0	# degrees per second

# metin dps categories show "/s" after the value (Baerim IsDpsRank)
DPS_CATEGORIES = (28, 29, 30, 31, 42)

def _Sub(*items):
	return [(data, getattr(localeInfo, "GLOBAL_RANK_" + key)) for (data, key) in items]

def _Category(data, key, icon, children = None):
	return { "data" : data, "name" : getattr(localeInfo, "GLOBAL_RANK_" + key), "icon" : icon, "items" : children or [] }

def _Levels(first, second, third, fourth, fifth):
	return _Sub((first, "LV_30"), (second, "LV_55"), (third, "LV_75"), (fourth, "LV_91"), (fifth, "LV_106"))

def _BuildCategories():
	metin = _Sub((2, "LV_30"), (3, "LV_55"), (4, "LV_75"), (5, "LV_91"), (41, "LV_106"), (25, "RELIC"))
	boss = _Sub((32, "BOSS_1"), (33, "BOSS_2"), (34, "BOSS_3"), (35, "BOSS_4"), (40, "BOSS_5"))
	switch = _Sub((16, "SWITCH_ITEM"), (17, "SWITCH_COSTUME"), (18, "SWITCH_SASH"), (20, "SWITCH_MOUNT"), (38, "BLESSING"), (39, "CATALYST"))

	player = [
		_Category(1, "QUEST_BOOK", "quest_book.png"),
		_Category(-1, "METIN", "stone.png", metin),
		_Category(-1, "BOSS", "boss_eliminated.png", boss),
		_Category(7, "DUNGEON", "dungeons.png"),
		_Category(8, "MINING", "mining.png"),
		_Category(9, "FISH", "fish.png"),
		_Category(-1, "METIN_DAMAGE", "stone_damage.png", _Levels(10, 11, 12, 13, 43)),
		_Category(-1, "METIN_DPS", "stone_damage.png", _Levels(28, 29, 30, 31, 42)),
		_Category(14, "REFINE_FAIL", "refine_failed.png"),
		_Category(15, "REFINE_SUCCESS", "refine_success.png"),
		_Category(-1, "SWITCH", "switch.png", switch),
		_Category(27, "PVP", "pvp.png"),
		_Category(36, "EXPLORATION", "flag.png"),
	]
	guilds = [
		_Category(1, "QUEST_BOOK", "quest_book.png"),
		_Category(-1, "METIN", "stone.png", metin),
		_Category(-1, "BOSS", "boss_eliminated.png", boss),
		_Category(7, "DUNGEON", "dungeons.png"),
		_Category(8, "MINING", "mining.png"),
		_Category(9, "FISH", "fish.png"),
		_Category(14, "REFINE_FAIL", "refine_failed.png"),
		_Category(15, "REFINE_SUCCESS", "refine_success.png"),
		_Category(-1, "SWITCH", "quest_book.png", switch),
		_Category(21, "ETERNITY_PIT", "dungeons.png"),
		_Category(-1, "KARUAK", "boss_eliminated.png", _Sub((22, "EASY"), (23, "NORMAL"), (24, "HARD"), (44, "EXTREME"))),
	]
	members = list(player)
	members[-1] = _Category(36, "EXPLORATION", "quest_book.png")
	return (player, guilds, members)

def FormatValue(category, value):
	# Baerim groups thousands with dots: 1.400.400
	digits = str(abs(long(value)))
	groups = []
	while len(digits) > 3:
		groups.insert(0, digits[-3:])
		digits = digits[:-3]
	groups.insert(0, digits)
	text = ("-" if value < 0 else "") + ".".join(groups)
	if category in DPS_CATEGORIES:
		return text + " /s"
	return text

def _Image(parent, name, x, y, expanded = False):
	image = ui.ExpandedImageBox() if expanded else ui.ImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	image.AddFlag("not_pick")
	image.Show()
	return image

def _Text(parent, x, y, text = "", center = True, color = None):
	line = ui.TextLine()
	line.SetParent(parent)
	line.SetPosition(x, y)
	if center:
		line.SetHorizontalAlignCenter()
	if color:
		line.SetPackedFontColor(color)
	line.SetText(text)
	line.Show()
	return line


class CategoryItem(ui.Window):
	HEIGHT = 38

	def __init__(self, owner, data):
		ui.Window.__init__(self)
		self.owner = owner
		self.data = data
		self.isSelected = False
		self.isExpanded = False
		self.isOver = False
		self.SetSize(180, 42)

		self.background = _Image(self, RANKING_PATH % "rank_btn_norm.png", 0, 0)
		icon = RANKING_PATH % ("icons/" + data["icon"])
		self.icon = _Image(self, icon, 13, 0)
		self.icon.SetPosition(13, 21 - self.icon.GetHeight() / 2)
		self.text = _Text(self, 50, 13, "  " + data["name"], False, LIST_COLOR)
		self.arrow = _Image(self, ARROW_PATH % "down", 158, 13)
		if not data["items"]:
			self.arrow.Hide()

		self.subItems = []
		for (subData, subName) in data["items"]:
			self.subItems.append(CategorySubItem(owner, self, subData, subName))

	def SetMask(self, window):
		# rows cut by the list edge are drawn in part, like Baerim's
		if app.ENABLE_CLIP_MASKING:
			self.SetClippingMaskWindow(window)
			for sub in self.subItems:
				sub.SetClippingMaskWindow(window)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		for sub in self.subItems:
			sub.Destroy()
		self.subItems = []
		self.owner = None

	def HasChildren(self):
		return len(self.subItems) > 0

	def SetSelected(self, flag):
		self.isSelected = flag
		self.__Refresh()

	def SetExpanded(self, flag):
		self.isExpanded = flag and self.HasChildren()
		self.__Refresh()

	def __Refresh(self):
		if self.isSelected or self.isExpanded:
			self.background.LoadImage(RANKING_PATH % "rank_btn_select.png")
		elif self.isOver:
			self.background.LoadImage(RANKING_PATH % "rank_btn_over.png")
		else:
			self.background.LoadImage(RANKING_PATH % "rank_btn_norm.png")
		self.arrow.LoadImage(ARROW_PATH % ("up" if (self.isSelected or self.isExpanded) else "down"))

	def OnMouseOverIn(self):
		self.isOver = True
		self.__Refresh()

	def OnMouseOverOut(self):
		self.isOver = False
		self.__Refresh()

	def OnMouseLeftButtonDown(self):
		if self.owner:
			self.owner.OnClickCategory(self)

	def OnRunMouseWheel(self, nLen):
		if self.owner:
			self.owner.ScrollList(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)


class CategorySubItem(ui.Window):
	HEIGHT = 23

	def __init__(self, owner, parentItem, data, name):
		ui.Window.__init__(self)
		self.owner = owner
		self.parentItem = parentItem
		self.data = data
		self.isSelected = False
		self.isOver = False
		self.SetSize(172, 25)

		self.background = _Image(self, RANKING_PATH % "subcategory_norm.png", 0, 0)
		self.text = _Text(self, 7, 5, name, False, LIST_COLOR)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.owner = None
		self.parentItem = None

	def SetSelected(self, flag):
		self.isSelected = flag
		self.__Refresh()

	def __Refresh(self):
		if self.isSelected:
			self.background.LoadImage(RANKING_PATH % "subcategory_selected.png")
		elif self.isOver:
			self.background.LoadImage(RANKING_PATH % "subcategory_hover.png")
		else:
			self.background.LoadImage(RANKING_PATH % "subcategory_norm.png")

	def OnMouseOverIn(self):
		self.isOver = True
		self.__Refresh()

	def OnMouseOverOut(self):
		self.isOver = False
		self.__Refresh()

	def OnMouseLeftButtonDown(self):
		if self.owner:
			self.owner.OnClickSubCategory(self)

	def OnRunMouseWheel(self, nLen):
		if self.owner:
			self.owner.ScrollList(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)


class ListScrollBar(ui.Window):
	# Baerim scrollBarListbox: 4 px track with a 1 px frame, 2 px thumb
	def __init__(self, height):
		ui.Window.__init__(self)
		self.SetSize(4, height)
		self.event = None
		self.pos = 0.0
		self.thumbHeight = height

		self.frame = ui.Bar()
		self.frame.SetParent(self)
		self.frame.SetColor(0xff565656)
		self.frame.SetSize(4, height)
		self.frame.AddFlag("not_pick")
		self.frame.Show()

		self.track = ui.Bar()
		self.track.SetParent(self)
		self.track.SetColor(0xff000000)
		self.track.SetPosition(1, 1)
		self.track.SetSize(2, height - 2)
		self.track.AddFlag("not_pick")
		self.track.Show()

		self.thumb = ui.DragButton()
		self.thumb.SetParent(self)
		self.thumb.AddFlag("movable")
		self.thumb.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.thumb.Show()

		self.thumbBar = ui.Bar()
		self.thumbBar.SetParent(self.thumb)
		self.thumbBar.SetColor(0xff565656)
		self.thumbBar.AddFlag("not_pick")
		self.thumbBar.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.event = None

	def SetScrollEvent(self, event):
		self.event = event

	def SetPageScale(self, scale):
		height = self.GetHeight() - 2
		self.thumbHeight = max(12, int(height * min(1.0, scale)))
		self.thumb.SetSize(2, self.thumbHeight)
		self.thumbBar.SetSize(2, self.thumbHeight)
		self.thumb.SetRestrictMovementArea(1, 1, 2, height)
		self.SetPos(self.pos)

	def SetPos(self, pos):
		self.pos = max(0.0, min(1.0, pos))
		movable = self.GetHeight() - 2 - self.thumbHeight
		self.thumb.SetPosition(1, 1 + int(movable * self.pos))

	def GetPos(self):
		return self.pos

	def __OnMove(self):
		movable = self.GetHeight() - 2 - self.thumbHeight
		(x, y) = self.thumb.GetLocalPosition()
		self.pos = float(y - 1) / movable if movable > 0 else 0.0
		if self.event:
			self.event()


class RankingWindow(ui.BoardWithTitleBar):
	WIDTH = 660
	HEIGHT = 462

	LIST_X = 15
	LIST_Y = 82
	LIST_HEIGHT = 356

	ROW_Y = 153
	ROW_STEP = 26
	OWN_ROW_Y = 415
	COLUMN_INDEX = 228
	COLUMN_NAME = 311
	COLUMN_FLAG = 396
	COLUMN_LEVEL = 509
	COLUMN_VALUE = 605

	RACE_BUTTONS = ("btn_0", "btn_warrior", "btn_ninja", "btn_sura", "btn_shaman")
	PAGE_BUTTONS = (("btn_perso_norm", "btn_perso_hover", "btn_perso_select"),
					("guild_btn_norm", "guild_btn_hover", "guild_btn_selected"),
					("btn_myguild_norm", "btn_myguild_hover", "btn_myguild_selected"))

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.page = PAGE_PLAYER
		self.filter = 0
		self.category = 0
		self.selectedItem = None
		self.selectedSub = None
		self.categoryItems = []
		self.scroll = 0
		self.listHeight = 0
		self.resetEnd = 0
		self.nextTimeRefresh = 0
		self.isLoaded = False
		self.pageToolTip = None

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __Load(self):
		if self.isLoaded:
			return
		self.isLoaded = True

		self.SetSize(self.WIDTH, self.HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetTitleName(localeInfo.GLOBAL_RANK_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))

		self.thinBoard = _Image(self, RANKING_PATH % "background.png", 5, 71)
		self.timeBg = _Image(self, RANKING_PATH % "background_reset.png", 423, 31)
		self.timeText = _Text(self, 433, 45, "", False, TIME_COLOR)
		self.timeText.SetOutline()
		self.leftBg = _Image(self, RANKING_PATH % "left_bg.png", 11, 78, True)
		self.bgSlot = _Image(self, RANKING_PATH % "background_right.png", 206, 76)
		self.rightBgSlot = _Image(self, RANKING_PATH % "ranking_rows.png", 211, 121)

		self.pageButtons = []
		tooltips = (localeInfo.GLOBAL_RANK_PAGE_PLAYER, localeInfo.GLOBAL_RANK_PAGE_GUILD, localeInfo.GLOBAL_RANK_PAGE_MEMBER)
		for i in xrange(3):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetPosition(7 + 134 * i, 30)
			(norm, hover, select) = self.PAGE_BUTTONS[i]
			button.SetUpVisual(RANKING_PATH % (norm + ".png"))
			button.SetOverVisual(RANKING_PATH % (hover + ".png"))
			button.SetDownVisual(RANKING_PATH % (select + ".png"))
			button.SetShowToolTipEvent(ui.__mem_func__(self.__ShowPageToolTip), tooltips[i])
			button.SetHideToolTipEvent(ui.__mem_func__(self.__HidePageToolTip))
			button.SetEvent(ui.__mem_func__(self.SetPage), i)
			button.Show()
			self.pageButtons.append(button)

		self.filterButtons = []
		for i in xrange(5):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetPosition(211 + 86 * i, 81)
			button.SetEvent(ui.__mem_func__(self.SetFilter), i)
			button.Show()
			self.filterButtons.append(button)

		self.titles = [
			_Text(self, self.COLUMN_NAME, 126, localeInfo.GLOBAL_RANK_COLUMN_NAME, True, TEXT_COLOR),
			_Text(self, 409, 126, localeInfo.GLOBAL_RANK_COLUMN_EMPIRE, True, TEXT_COLOR),
			_Text(self, self.COLUMN_LEVEL, 126, localeInfo.GLOBAL_RANK_COLUMN_GUILD, True, TEXT_COLOR),
			_Text(self, self.COLUMN_VALUE, 126, localeInfo.GLOBAL_RANK_COLUMN_VALUE, True, TEXT_COLOR),
		]

		self.rows = []
		for i in xrange(ROW_COUNT + 1):
			y = self.OWN_ROW_Y if i == ROW_COUNT else self.ROW_Y + self.ROW_STEP * i
			row = {
				"index" : _Text(self, self.COLUMN_INDEX, y, "", True, TEXT_COLOR),
				"name" : _Text(self, self.COLUMN_NAME, y, "", True, TEXT_COLOR),
				"flag" : _Image(self, RANKING_PATH % "empire_1.png", self.COLUMN_FLAG, y),
				"level" : _Text(self, self.COLUMN_LEVEL, y, "", True, TEXT_COLOR),
				"value" : _Text(self, self.COLUMN_VALUE, y, "", True, TEXT_COLOR),
			}
			row["flag"].Hide()
			self.rows.append(row)

		# Baerim "loading": common/loading_image.tga turning in the middle of the rows until the answer comes
		self.loading = _Image(self, "d:/ymir work/ui/game/common/loading_image.tga", 400, 250, True)
		self.loading.SetOrigin(25, 28)
		self.loading.Hide()
		self.loadingRotation = 0.0
		self.lastUpdate = 0.0

		self.listWindow = ui.Window()
		self.listWindow.SetParent(self)
		self.listWindow.SetPosition(self.LIST_X, self.LIST_Y)
		self.listWindow.SetSize(184, self.LIST_HEIGHT)
		self.listWindow.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnListWheel))
		self.listWindow.Show()

		self.scrollBar = ListScrollBar(353)
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(195, 84)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBar))
		self.scrollBar.Show()

		self.SetPage(PAGE_PLAYER)

	# ---------------------------------------------------------------- pages / filters

	# Baerim: a framed tooltip at the mouse, above every other window (buttontooltip ToolTip)
	def __ShowPageToolTip(self, text):
		if not self.pageToolTip:
			self.pageToolTip = uiToolTip.ToolTip(0)
			self.pageToolTip.HideToolTip()
		self.pageToolTip.ClearToolTip()
		self.pageToolTip.toolTipWidth = 0	# width follows the text (it only grows otherwise)
		self.pageToolTip.AutoAppendTextLine(text, 0xffc1c1c1)
		self.pageToolTip.AlignHorizonalCenter()
		self.pageToolTip.ShowToolTip()

	def __HidePageToolTip(self):
		if self.pageToolTip:
			self.pageToolTip.HideToolTip()

	def SetPage(self, page):
		self.page = page
		for i in xrange(3):
			if i == page:
				self.pageButtons[i].Down()
			else:
				self.pageButtons[i].SetUp()

		# guild page: empire filter (all + 3 empires), others: race filter (all + 4 races)
		for i in xrange(5):
			button = self.filterButtons[i]
			if page == PAGE_GUILD:
				if i >= 4:
					button.Hide()
					continue
				name = "empires/btn_%d" % i
			else:
				name = "races/" + self.RACE_BUTTONS[i]
			button.SetUpVisual(RANKING_PATH % (name + "_norm.png"))
			button.SetOverVisual(RANKING_PATH % (name + "_hover.png"))
			button.SetDownVisual(RANKING_PATH % (name + "_select.png"))
			button.Show()

		self.titles[2].SetText(localeInfo.GLOBAL_RANK_COLUMN_LEVEL if page == PAGE_GUILD else localeInfo.GLOBAL_RANK_COLUMN_GUILD)
		self.__BuildCategories()
		self.SetFilter(0, False)
		self.__SelectFirst()

	def SetFilter(self, index, request = True):
		self.filter = index
		for i in xrange(5):
			if i == index:
				self.filterButtons[i].Down()
			else:
				self.filterButtons[i].SetUp()
		if request:
			self.RequestLoadRank()

	# ---------------------------------------------------------------- category list

	def __BuildCategories(self):
		for item in self.categoryItems:
			item.Destroy()
			item.Hide()
		self.categoryItems = []
		self.selectedItem = None
		self.selectedSub = None
		self.scroll = 0

		for data in _BuildCategories()[self.page]:
			item = CategoryItem(self, data)
			item.SetParent(self.listWindow)
			self.categoryItems.append(item)
			for sub in item.subItems:
				sub.SetParent(self.listWindow)
			item.SetMask(self.listWindow)
		self.__RefreshList()

	def __SelectFirst(self):
		if self.categoryItems:
			self.OnClickCategory(self.categoryItems[0])

	def __RefreshList(self):
		y = -self.scroll
		for item in self.categoryItems:
			self.__Place(item, y, 42)
			if item.isExpanded:
				subY = y + CategoryItem.HEIGHT
				for sub in item.subItems:
					self.__Place(sub, subY, 25)
					subY += CategorySubItem.HEIGHT
				y = subY + 3
			else:
				for sub in item.subItems:
					sub.Hide()
				y += CategoryItem.HEIGHT + 3

		self.listHeight = y + self.scroll
		overflow = self.listHeight - self.LIST_HEIGHT
		if overflow > 0:
			self.scrollBar.SetPageScale(float(self.LIST_HEIGHT) / self.listHeight)
			self.scrollBar.SetPos(float(self.scroll) / overflow)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def __Place(self, window, y, height):
		window.SetPosition(0, y)
		# rows outside the list are hidden, rows on its edge are clipped by the mask
		if y + height > 0 and y < self.LIST_HEIGHT:
			window.Show()
		else:
			window.Hide()

	def __SetScroll(self, value):
		self.scroll = max(0, min(max(0, self.listHeight - self.LIST_HEIGHT), int(value)))
		self.__RefreshList()

	def ScrollList(self, nLen):
		self.__SetScroll(self.scroll + (-41 if nLen > 0 else 41))

	def __OnListWheel(self, isDown):
		self.__SetScroll(self.scroll + (41 if isDown else -41))

	def __OnScrollBar(self):
		self.scroll = int(self.scrollBar.GetPos() * max(0, self.listHeight - self.LIST_HEIGHT))
		self.__RefreshList()

	def OnClickCategory(self, item):
		if item.HasChildren():
			expand = not item.isExpanded
			for other in self.categoryItems:
				other.SetExpanded(False)
			item.SetExpanded(expand)
			if expand and item.subItems:
				self.OnClickSubCategory(item.subItems[0])
			self.__SetScroll(self.scroll)
			return

		for other in self.categoryItems:
			other.SetExpanded(False)
			other.SetSelected(False)
		self.__ClearSub()
		item.SetSelected(True)
		self.selectedItem = item
		self.category = item.data["data"]
		self.__SetScroll(self.scroll)
		self.RequestLoadRank()

	def OnClickSubCategory(self, sub):
		for other in self.categoryItems:
			if other != sub.parentItem:
				other.SetSelected(False)
		self.__ClearSub()
		sub.SetSelected(True)
		self.selectedSub = sub
		self.selectedItem = sub.parentItem
		self.category = sub.data
		self.RequestLoadRank()

	def __ClearSub(self):
		if self.selectedSub:
			self.selectedSub.SetSelected(False)
		self.selectedSub = None

	# ---------------------------------------------------------------- data

	def RequestLoadRank(self):
		if self.category <= 0:
			return
		self.ClearActualRank()
		self.SetLoadingStatus(True)
		net.SendChatPacket("/global_rank _load %d 0 %d %d" % (self.page, self.filter, self.category))

	def SetLoadingStatus(self, flag):
		if flag:
			self.loading.Show()
			self.loading.SetTop()
		else:
			self.loading.Hide()

	def ClearActualRank(self):
		for row in self.rows:
			for key in ("index", "name", "level", "value"):
				row[key].SetText("")
			row["flag"].Hide()

	def __SetRow(self, row, index, name, level, empire, value):
		row["index"].SetText(index)
		row["name"].SetText(name)
		row["level"].SetText(level)
		row["value"].SetText(value)
		if empire in (1, 2, 3):
			row["flag"].LoadImage(RANKING_PATH % ("empire_%d.png" % empire))
			row["flag"].Show()
		else:
			row["flag"].Hide()

	def LoadRankData(self, page, timeIdx, category, secondsToReset, ownValue, rankCmd):
		if page != self.page or category != self.category:
			return

		self.resetEnd = app.GetTime() + max(0, secondsToReset)
		self.nextTimeRefresh = 0
		self.SetLoadingStatus(False)
		self.ClearActualRank()

		if page == PAGE_GUILD:
			ownName = guild.GetGuildName() if guild.IsGuildEnable() else "-"
		else:
			ownName = player.GetName()
		ownIndex = ""

		if rankCmd != "empty":
			for (i, entry) in enumerate(rankCmd.split("#")[:ROW_COUNT]):
				fields = entry.replace("^", " ").split("?")
				if len(fields) < 5:
					continue
				(pos, name, level, empire, value) = fields[:5]
				try:
					empire = int(empire)
					value = long(value)
				except ValueError:
					continue
				self.__SetRow(self.rows[i], str(i + 1), name, level, empire, FormatValue(category, value))
				if name == ownName:
					ownIndex = str(i + 1)

		# own row: own player (or own guild) and its value from the server
		if page == PAGE_GUILD:
			ownLevel = str(guild.GetGuildLevel()) if guild.IsGuildEnable() else "-"
		else:
			ownLevel = player.GetGuildName() or "-"
		self.__SetRow(self.rows[ROW_COUNT], ownIndex or "10+", ownName, ownLevel, net.GetEmpireID(), FormatValue(category, ownValue))

	# ---------------------------------------------------------------- reset timer

	def OnUpdate(self):
		now = app.GetTime()
		if self.loading.IsShow():
			self.loadingRotation = (self.loadingRotation + LOADING_SPEED * max(0.0, now - self.lastUpdate)) % 360.0
			self.loading.SetRotation(self.loadingRotation)
		self.lastUpdate = now

		if now < self.nextTimeRefresh:
			return
		self.nextTimeRefresh = now + 1.0

		left = max(0, int(self.resetEnd - now))
		self.timeText.SetText(localeInfo.GLOBAL_RANK_RESET % (left / 86400, (left % 86400) / 3600, (left % 3600) / 60))

	# ---------------------------------------------------------------- open / close

	def Open(self):
		self.__Load()
		if self.IsShow():
			self.Close()
			return
		self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.RequestLoadRank()

	def Close(self):
		self.__HidePageToolTip()
		self.Hide()

	def Destroy(self):
		for item in self.categoryItems:
			item.Destroy()
		self.categoryItems = []
		if self.isLoaded:
			self.scrollBar.Destroy()

	def OnPressEscapeKey(self):
		self.Close()
		return True
