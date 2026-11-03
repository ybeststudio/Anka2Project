if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui, item, localeInfo, constInfo
from uiToolTip import ItemToolTip

# Character details side panel (reference Baerim uiCharacterDetails.CharacterDetailsUI):
# "Bonus" tab = searchable, collapsible bonus categories, "Istatistik" tab = bg_stat.png cards.
IMG_DIR = "characterdetails/"
NEW_IMG_DIR = "d:/ymir work/ui/game/characterdetails/new/"
ADVANCED_OPTION_IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"
MESSENGER_IMG_DIR = "d:/ymir work/ui/game/messenger/"

WIDTH = 247
HEIGHT = 410
TAB_TEXT_COLOR = 0xffd1b183		# colours measured on the reference panel
LABEL_COLOR = 0xffcabdab
HINT_COLOR = 0xff808080
CATEGORY_STEP = 30			# header image: 30 px visible, steps of 30 like the reference
PAGE_TOP = 36			# pages start under the tab buttons so they never cover them
LIST_TOP = 69 - PAGE_TOP
LIST_BOTTOM = HEIGHT - PAGE_TOP - 10




# Reference bonus_list (apply names; Baerim-only bonuses we do not have are skipped, aliases map their names to ours)
_ALIAS = {"ATTBONUS_METIN": "ATTBONUS_STONE", "ATTBONUS_ELEMENTARY": "ATTBONUS_ELEMENTS", "RESIST_ELEMENTARY": "ENCHANT_ELEMENTS"}
_REFERENCE_BONUS_LIST = (
	("PvM", ("ATT_VALUE_PVE", "NORMAL_HIT_DAMAGE_BONUS_PVE", "ATTBONUS_MONSTER", "ATTBONUS_METIN", "ATTBONUS_BOSS", "RESIST_BOSS",
		"ATTBONUS_DUNGEON", "RESIST_HUMAN", "ATTBONUS_ANIMAL", "ATTBONUS_ORC", "ATTBONUS_MILGYO", "ATTBONUS_UNDEAD", "ATTBONUS_DEVIL",
		"ATTBONUS_BOSS_CAPE", "ATTBONUS_TORGNY", "RESIST_TORGNY", "ATTBONUS_RAZADOR", "ATTBONUS_NEMERE", "ATTBONUS_EREBUS",
		"ATTBONUS_MELEY", "ATTBONUS_OCHAO", "POWER_CRITICAL_PVM")),
	("Elements", ("RESIST_ELEMENTARY", "ATTBONUS_ELEMENTARY", "RESIST_WIND", "ENCHANT_WIND", "RESIST_DARK", "ENCHANT_DARK", "RESIST_ELEC",
		"ENCHANT_ELECT", "RESIST_EARTH", "ENCHANT_EARTH", "RESIST_FIRE", "ENCHANT_FIRE", "RESIST_ICE", "ENCHANT_ICE")),
	("General (Offensive)", ("SKILL_DAMAGE_BONUS", "NORMAL_HIT_DAMAGE_BONUS", "CRITICAL_PCT", "POWER_CRITICAL", "PENETRATE_PCT",
		"POWER_PENETRATE", "MELEE_MAGIC_ATTBONUS_PER", "POISON_PCT")),
	("General (Defensive)", ("SKILL_DEFEND_BONUS", "NORMAL_HIT_DEFEND_BONUS", "RESIST_MAGIC", "ANTI_CRITICAL_PCT", "ANTI_PENETRATE_PCT",
		"BLOCK", "REFLECT_MELEE", "POISON_REDUCE")),
	("PvP (Offensive)", ("ATTBONUS_HUMAN", "ATTBONUS_WARRIOR", "ATTBONUS_ASSASSIN", "ATTBONUS_SURA", "ATTBONUS_SHAMAN")),
	("PvP (Defensive)", ("RESIST_MAGIC", "RESIST_HUMAN", "PVP_RESIST_BONUS", "RESIST_WARRIOR", "RESIST_ASSASSIN", "RESIST_SURA",
		"RESIST_SHAMAN", "RESIST_SWORD", "RESIST_TWOHAND", "RESIST_DAGGER", "RESIST_BELL", "RESIST_FAN", "RESIST_BOW")),
	("Status", ("STEAL_HP", "HP_REGEN", "MALL_EXPBONUS", "EXP_DOUBLE_BONUS", "PET_EXP", "GOLD_DOUBLE_BONUS", "ITEM_DROP_BONUS",
		"DOUBLE_DROP_BOSS", "DOUBLE_DROP_METIN", "DOUBLE_DROP_DUNGEON", "DOUBLE_DROP_GOLD_DUNGEON", "DROP_BONUS_WORLDBOSS",
		"DOUBLE_DROP_MINING", "DOUBLE_DROP_FISHING", "REDUCED_DUNGEON_CD", "REFINE_CHANCE", "MOUNT_SPEED", "ADD_ORE_CHANCE")),
)


def _BuildBonusList():
	result = []
	for (category, names) in _REFERENCE_BONUS_LIST:
		pairs = []
		for name in names:
			apply = getattr(item, "APPLY_" + _ALIAS.get(name, name), None)
			if apply is None or apply not in ItemToolTip.AFFECT_DICT:
				continue
			point = item.GetApplyPoint(apply) if hasattr(item, "GetApplyPoint") else 0
			if point:
				pairs.append([apply, point])
		if pairs:
			result.append([category, pairs])
	return result


bonus_list = _BuildBonusList()

# Reference stat_list (bg_stat.png cards, top to bottom)
stat_list = (
	localeInfo.DETAILS_STAT_STONE,
	localeInfo.DETAILS_STAT_BOSS,
	localeInfo.DETAILS_STAT_DUNGEON,
	localeInfo.DETAILS_STAT_FISH,
	localeInfo.DETAILS_STAT_MINE,
)
STAT_CARD_HEIGHT = 72


def _BonusName(apply):
	text = ItemToolTip.AFFECT_DICT[apply](0)
	return "".join(c for c in text if c not in "%+:" and not c.isdigit()).strip()


class CharacterDetailsUI(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.uiCharacterStatus = parent
		self.btnIndex = 0
		self.keep = []
		self.children = {}
		self.categoryOpen = [True] * len(bonus_list)
		self.rows = []			# (categoryIndex, name, apply/point pair, image, nameText, valueText)
		self.__LoadWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.Hide()
		scrollBar = self.children.get("scrollBar")
		if scrollBar:
			scrollBar.Destroy()
		self.children = {}
		self.rows = []
		self.keep = []

	# ------------------------------------------------------------------ helpers
	def __Image(self, parent, path, x, y, pick = False):
		img = ui.ExpandedImageBox()
		img.SetParent(parent)
		img.LoadImage(path)
		img.SetPosition(x, y)
		if not pick:
			img.AddFlag("not_pick")
		img.Show()
		self.keep.append(img)
		return img

	def __Text(self, parent, text, x, y, align = "left", font = None):
		line = ui.TextLine()
		line.SetParent(parent)
		line.AddFlag("not_pick")
		if font:
			line.SetFontName(font)
		line.SetPosition(x, y)
		if align == "center":
			line.SetHorizontalAlignCenter()
		elif align == "right":
			line.SetHorizontalAlignRight()
		line.SetText(text)
		line.Show()
		self.keep.append(line)
		return line

	def __TabButton(self, text, x, index):
		btn = ui.RadioButton()
		btn.SetParent(self)
		btn.SetPosition(x, 8)
		btn.SetUpVisual(NEW_IMG_DIR + "btn_norm.png")
		btn.SetOverVisual(NEW_IMG_DIR + "btn_hover.png")
		btn.SetDownVisual(NEW_IMG_DIR + "btn_select.png")
		btn.SetText(text)
		btn.SetTextColor(TAB_TEXT_COLOR)
		btn.SetEvent(lambda: self.SetCategory(index))
		btn.Show()
		self.keep.append(btn)
		return btn

	# ------------------------------------------------------------------ layout
	def __LoadWindow(self):
		self.AddFlag("float")
		self.SetSize(WIDTH, HEIGHT)

		board = ui.Board()
		board.SetParent(self)
		board.SetPosition(0, 0)
		board.SetSize(WIDTH, HEIGHT)
		board.AddFlag("not_pick")
		board.Show()
		self.keep.append(board)

		self.tabs = [self.__TabButton(localeInfo.DETAILS_TAB_BONUS, 8, 0),
			self.__TabButton(localeInfo.DETAILS_TAB_STAT, 125, 1)]

		# ---------------- bonus page
		bonusPage = ui.Window()
		bonusPage.SetParent(self)
		bonusPage.SetPosition(0, PAGE_TOP)
		bonusPage.SetSize(WIDTH, HEIGHT - PAGE_TOP)
		bonusPage.Show()
		self.keep.append(bonusPage)
		self.children["bonusPage"] = bonusPage

		SEARCH_X = (WIDTH - 170) / 2
		self.__Image(bonusPage, NEW_IMG_DIR + "bg_output.png", (WIDTH - 238) / 2, -3)	# band behind the search box
		self.__Image(bonusPage, NEW_IMG_DIR + "search_bg.png", SEARCH_X, 6)

		search = ui.EditLine()
		search.SetParent(bonusPage)
		search.SetPosition(SEARCH_X + 7, 9)
		search.SetSize(140, 16)
		search.SetMax(30)
		search.OnIMEUpdate = ui.__mem_func__(self.__OnSearchUpdate)
		search.Show()
		self.children["search"] = search
		self.children["searchHint"] = self.__Text(bonusPage, localeInfo.DETAILS_SEARCH_HINT, SEARCH_X + 7, 9)
		self.children["searchHint"].SetPackedFontColor(HINT_COLOR)

		clear = ui.Button()
		clear.SetParent(bonusPage)
		clear.SetUpVisual(NEW_IMG_DIR + "search_clear_0.png")
		clear.SetOverVisual(NEW_IMG_DIR + "search_clear_1.png")
		clear.SetDownVisual(NEW_IMG_DIR + "search_clear_2.png")
		clear.SetPosition(SEARCH_X + 170 - clear.GetWidth() - 3, 6 + (20 - clear.GetHeight()) / 2)
		clear.SetEvent(ui.__mem_func__(self.__ClearSearch))
		clear.Show()
		self.keep.append(clear)

		listWindow = ui.Window()
		listWindow.SetParent(bonusPage)
		listWindow.SetPosition(0, LIST_TOP)
		listWindow.SetSize(WIDTH, LIST_BOTTOM - LIST_TOP)
		listWindow.Show()
		self.keep.append(listWindow)
		self.children["listWindow"] = listWindow

		self.categories = []
		for i, (category, pairs) in enumerate(bonus_list):
			header = ui.Button()
			header.SetParent(listWindow)
			self.__SetHeaderVisual(header, True)
			header.SetEvent(ui.__mem_func__(self.__ToggleCategory), i)
			header.Show()
			self.keep.append(header)
			label = self.__Text(header, category, 12, 10, "left")
			label.SetPackedFontColor(LABEL_COLOR)
			arrow = self.__Image(header, ADVANCED_OPTION_IMG_DIR + "arrow_down.png", 228 - 24, 13)
			self.categories.append((header, label, arrow))
			for pair in pairs:
				row = self.__Image(listWindow, NEW_IMG_DIR + "bonus.png", 9, 0, True)
				name = _BonusName(pair[0])
				row.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OverInBonus, name)
				row.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OverOutBonus)
				nameText = self.__Text(row, name, 8, 5, "left")
				while nameText.GetTextSize()[0] > 160 and len(nameText.GetText()) > 3:
					nameText.SetText(nameText.GetText()[:-3] + "..")
				valueText = self.__Text(row, "0", 212, 5, "right")
				self.rows.append((i, name.lower(), pair, row, nameText, valueText))

		scrollBar = BaerimScrollBar()
		scrollBar.SetParent(bonusPage)
		scrollBar.SetPosition(WIDTH - scrollBar.GetWidth() - 1, LIST_TOP)
		scrollBar.SetScrollBarSize(LIST_BOTTOM - LIST_TOP)
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__RefreshBonus))
		scrollBar.Hide()
		self.children["scrollBar"] = scrollBar

		# ---------------- stat page
		statPage = ui.Window()
		statPage.SetParent(self)
		statPage.SetPosition(0, PAGE_TOP)
		statPage.SetSize(WIDTH, HEIGHT - PAGE_TOP)
		statPage.Hide()
		self.keep.append(statPage)
		self.children["statPage"] = statPage

		statBg = self.__Image(statPage, NEW_IMG_DIR + "bg_stat.png", (WIDTH - 238) / 2, 2)
		self.statValues = []
		for i, title in enumerate(stat_list):
			self.__Text(statBg, title, 150, 7 + i * STAT_CARD_HEIGHT, "center").SetPackedFontColor(LABEL_COLOR)
			value = self.__Text(statBg, "0", 150, 38 + i * STAT_CARD_HEIGHT, "center")
			value.SetPackedFontColor(LABEL_COLOR)
			self.statValues.append(value)

		self.SetCategory(0)

	# ------------------------------------------------------------------ interface used by uiCharacter
	def Show(self):
		if self.uiCharacterStatus and not getattr(self, '_placed', False):
			(x, y) = self.uiCharacterStatus.GetGlobalPosition()
			self.AdjustPosition(x, y)
			self._placed = True
		ui.Window.Show(self)
		self.SetTop()
		self.Refresh()

	def Close(self):
		self.Hide()

	def AdjustPosition(self, x, y):
		self.SetPosition(x + self.uiCharacterStatus.GetWidth() - 4, y)

	def RefreshLabel(self):
		self.Refresh()

	def SetCategory(self, categoryIndex):
		self.btnIndex = categoryIndex
		for i, tab in enumerate(self.tabs):
			if i == categoryIndex:
				tab.Down()
			else:
				tab.SetUp()
		self.children["search"].KillFocus()
		if categoryIndex == 0:
			self.children["bonusPage"].Show()
			self.children["statPage"].Hide()
		else:
			self.children["bonusPage"].Hide()
			self.children["statPage"].Show()
			if hasattr(net, "SendChatPacket"):
				net.SendChatPacket("/character_stats")
		self.Refresh()

	def Refresh(self):
		if self.btnIndex == 0:
			self.__RefreshBonus()
		else:
			self.__RefreshStats()

	def OnUpdate(self):
		if app.GetTime() >= getattr(self, "_nextRefresh", 0.0):
			self._nextRefresh = app.GetTime() + 0.5
			self.Refresh()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnRunMouseWheel(self, nLen):
		scrollBar = self.children.get("scrollBar")
		if self.btnIndex == 0 and self.IsInPosition() and scrollBar and scrollBar.IsShow():
			if nLen > 0:
				scrollBar.OnUp()
			else:
				scrollBar.OnDown()
			return True
		return False

	# ------------------------------------------------------------------ bonus tab
	def __SetHeaderVisual(self, header, isOpen):
		header.SetUpVisual(NEW_IMG_DIR + ("category_open.png" if isOpen else "category_closed.png"))
		header.SetOverVisual(NEW_IMG_DIR + "category_open.png")
		header.SetDownVisual(NEW_IMG_DIR + "category_open.png")

	def __ToggleCategory(self, categoryIndex):
		self.categoryOpen[categoryIndex] = not self.categoryOpen[categoryIndex]
		self.__RefreshBonus()

	def __OnSearchUpdate(self):
		search = self.children["search"]
		ui.EditLine.OnIMEUpdate(search)
		self.children["scrollBar"].SetPos(0, False)
		self.__RefreshBonus()

	def __ClearSearch(self):
		self.children["search"].SetText("")
		self.children["scrollBar"].SetPos(0, False)
		self.__RefreshBonus()

	def __OverInBonus(self, bonusName):
		interface = constInfo.GetInterfaceInstance()
		if interface and interface.tooltipItem:
			interface.tooltipItem.ClearToolTip()
			interface.tooltipItem.AppendTextLine(bonusName)
			interface.tooltipItem.ShowToolTip()

	def __OverOutBonus(self):
		interface = constInfo.GetInterfaceInstance()
		if interface and interface.tooltipItem:
			interface.tooltipItem.HideToolTip()

	def __RefreshBonus(self):
		if not self.children:
			return
		search = self.children["search"].GetText().lower()
		if search:
			self.children["searchHint"].Hide()
		else:
			self.children["searchHint"].Show()

		# layout: (widget, height) in order
		layout = []
		for i, (header, label, arrow) in enumerate(self.categories):
			rows = [r for r in self.rows if r[0] == i and (not search or search in r[1])]
			if search and not rows:
				layout_header = None
			else:
				layout_header = header
			if layout_header:
				open_ = self.categoryOpen[i] or bool(search)
				if getattr(header, "isOpen", None) != open_:
					header.isOpen = open_
					self.__SetHeaderVisual(header, open_)
				arrow.LoadImage(ADVANCED_OPTION_IMG_DIR + ("arrow_up.png" if open_ else "arrow_down.png"))
				layout.append((header, 5, CATEGORY_STEP))
				if open_:
					for r in rows:
						layout.append((r[3], 9, 26))
			for r in self.rows:
				if r[0] == i:
					r[3].Hide()
			header.Hide()

		viewHeight = LIST_BOTTOM - LIST_TOP
		total = sum(h for _, _, h in layout)
		scrollBar = self.children["scrollBar"]
		offset = 0
		if total > viewHeight:
			scrollBar.SetMiddleBarSize(float(viewHeight) / float(total))
			scrollBar.SetScrollStep(min(1.0, 26.0 / (total - viewHeight)))
			scrollBar.Show()
			offset = int(scrollBar.GetPos() * (total - viewHeight))
		else:
			scrollBar.Hide()

		y = -offset
		for widget, x, h in layout:
			if y >= 0 and y + widget.GetHeight() <= viewHeight:
				widget.SetPosition(x, y)
				widget.Show()
			y += h

		for (i, name, pair, row, nameText, valueText) in self.rows:
			if row.IsShow():
				valueText.SetText(str(player.GetStatus(pair[1])))

	# ------------------------------------------------------------------ stat tab
	def __RefreshStats(self):
		data = getattr(constInfo, "KILL_STATISTICS_DATA", None) or [0] * 11
		extra = getattr(constInfo, "CHARACTER_STATS_DATA", None) or (0, 0, 0)
		values = (data[8], data[7], extra[0], extra[1], extra[2])
		for text, value in zip(self.statValues, values):
			text.SetText(localeInfo.DottedNumber(value) if hasattr(localeInfo, "DottedNumber") else str(value))



class BaerimScrollBar(ui.Window):
	"""new/scroll_top|mid|bot track with the new/scroll_bar.png thumb (reference uiCharacterDetails.ScrollBarNew)."""

	def __init__(self):
		ui.Window.__init__(self)
		self.pos = 0.0
		self.step = 0.1
		self.eventScroll = None
		self.top = ui.ExpandedImageBox()
		self.mid = ui.ExpandedImageBox()
		self.bot = ui.ExpandedImageBox()
		for (img, name) in ((self.top, "scroll_top.png"), (self.mid, "scroll_mid.png"), (self.bot, "scroll_bot.png")):
			img.SetParent(self)
			img.LoadImage(NEW_IMG_DIR + name)
			img.AddFlag("not_pick")
			img.Show()
		self.thumb = ui.DragButton()
		self.thumb.SetParent(self)
		self.thumb.AddFlag("movable")
		self.thumb.AddFlag("restrict_x")
		self.thumb.SetUpVisual(NEW_IMG_DIR + "scroll_bar.png")
		self.thumb.SetOverVisual(NEW_IMG_DIR + "scroll_bar.png")
		self.thumb.SetDownVisual(NEW_IMG_DIR + "scroll_bar.png")
		self.thumb.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.thumb.Show()
		self.SetSize(self.thumb.GetWidth(), 0)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.eventScroll = None
		self.thumb = None

	def SetScrollBarSize(self, height):
		w = self.GetWidth()
		x = (w - self.top.GetWidth()) / 2
		self.SetSize(w, height)
		self.top.SetPosition(x, 0)
		self.bot.SetPosition(x, height - self.bot.GetHeight())
		self.mid.SetPosition(x, self.top.GetHeight())
		midHeight = max(1, height - self.top.GetHeight() - self.bot.GetHeight())
		self.mid.SetScale(1.0, float(midHeight) / float(self.mid.GetHeight()))
		self.thumb.SetRestrictMovementArea(0, 0, w, height)
		self.__Place()

	def SetMiddleBarSize(self, scale):
		pass	# the reference thumb has a fixed size

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetScrollStep(self, step):
		self.step = max(0.01, step)

	def GetPos(self):
		return self.pos

	def SetPos(self, pos, moveEvent = True):
		self.pos = max(0.0, min(1.0, pos))
		self.__Place()
		if moveEvent and self.eventScroll:
			self.eventScroll()

	def OnUp(self):
		self.SetPos(self.pos - self.step)

	def OnDown(self):
		self.SetPos(self.pos + self.step)

	def __Place(self):
		travel = max(1, self.GetHeight() - self.thumb.GetHeight())
		self.thumb.SetPosition(0, int(travel * self.pos))

	def __OnMove(self):
		(x, y) = self.thumb.GetLocalPosition()
		travel = max(1, self.GetHeight() - self.thumb.GetHeight())
		self.pos = max(0.0, min(1.0, float(y) / float(travel)))
		if self.eventScroll:
			self.eventScroll()

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 7
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 0
	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scrollbar/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scrollbar/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scrollbar/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle
		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())
	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.4
		self.SetWindowName("NONAME_ScrollBar")
	def __del__(self):
		ui.Window.__del__(self)
	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar/scroll_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar/scroll_bottom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar/scroll_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None
	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args
	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)
	def SetScrollStep(self, step):
		self.scrollStep = step
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def GetScrollStep(self):
		return self.scrollStep
	def GetPos(self):
		return self.curPos
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()
	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)
	def LockScroll(self):
		self.lockFlag = True
	def UnlockScroll(self):
		self.lockFlag = False

class SearchSlotBoard(ui.Window):
	CORNER_WIDTH = 7
	CORNER_HEIGHT = 7
	LINE_WIDTH = 7
	LINE_HEIGHT = 7
	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3
	def __init__(self):
		ui.Window.__init__(self)
		self.MakeBoard()
		self.MakeBase()
	def MakeBoard(self):
		cornerPath = IMG_DIR+"board/corner_"
		linePath = IMG_DIR+"board/"
		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("left", "right", "top", "bottom") ]
		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ui.ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)
		self.Lines = []
		for fileName in LineFileNames:
			Line = ui.ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)
		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)
	def MakeBase(self):
		self.Base = ui.ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage(IMG_DIR+"board/base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()
	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.Base=0
		self.Corners=0
		self.Lines=0
		self.CORNER_WIDTH = 0
		self.CORNER_HEIGHT = 0
		self.LINE_WIDTH = 0
		self.LINE_HEIGHT = 0
		self.LT = 0
		self.LB = 0
		self.RT = 0
		self.RB = 0
		self.L = 0
		self.R = 0
		self.T = 0
		self.B = 0
	def SetSize(self, width, height):
		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		ui.Window.SetSize(self, width, height)
		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)
		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)
