if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))
import item
import wndMgr

import ui
import uiToolTip
import localeInfo
import chat
import uiDungeonTrack

# Baerim uiMokokoRewards (Cython) rebuilt from its runtime dump (Reference/pydump_runtime/mods/uiMokokoRewards.json consts,
# dungeonprobe/full_TeleportWindow_*.json "mokokoRewardWindow" layout): 610x574 board, collection gauge with 5 chests
# (100/200/300/400/450), milestone cards (one every 10 seeds) in rows of 5 inside a scrolled list. The server owns the
# progress and the rewards: /mokoko_info, /mokoko_rewards, /mokoko_claim <id>.

MOKOKO_PATH = "d:/ymir work/ui/game/teleport_window/warp/mokoko/%s"
DAILY_PATH = "d:/ymir work/ui/game/new_mission/daily_mission/%s"
GUILD_PATH = "d:/ymir work/ui/game/guild/5/%s"
CHECK_ICON = "d:/ymir work/ui/game/battlepass/battlepass/check_icon.png"
BOUND_ITEM_MARKER = "icon/item/trade.png"
MOKOKO_SMALL_ICON = "|Eemoji/mokoko_small|e"
HEADER_X = 13						# Baerim: gauge, chests and progress title are children of the header (13, 32)
HEADER_Y = 32

WINDOW_WIDTH = 610
WINDOW_HEIGHT = 574
CARDS_PER_ROW = 5
CARD_WIDTH = 98
CARD_HEIGHT = 112
CARD_GAP = 15
ROW_STEP = CARD_HEIGHT + 5
ROW_X = 9
LIST_WIDTH = 568
LIST_HEIGHT = 364
CHEST_THRESHOLDS = (100, 200, 300, 400, 450)
CHEST_POSITIONS = (90, 200, 315, 425, 525)
CHEST_ASSET_IDS = (20, 40, 60, 80, 100)
CHEST_SIZES = ((43, 40), (43, 40), (42, 39), (43, 41), (44, 42))
GAUGE_X = 25
GAUGE_WIDTH = 532
MOKOKO_ROW_LOCK_ENABLED = True

TITLE_COLOR = 0xfff3e9c8
TEXT_COLOR = 0xffc1c1c1
GOLD_COLOR = 0xffd7b25b
# chest tooltip, measured on a Baerim screenshot: 350x100 board, gold title, 300 px table inset 25 px from both sides,
# header 26 px (no column lines), one 35 px row per item with column lines at 60 / 240
CHEST_TOOLTIP_WIDTH = 350
CHEST_TOOLTIP_TABLE_X = 25
CHEST_TOOLTIP_TABLE_WIDTH = 300
CHEST_TOOLTIP_TITLE_Y = 12
CHEST_TOOLTIP_TABLE_TOP = 33
CHEST_TOOLTIP_BOTTOM = 6
CHEST_TOOLTIP_TITLE_COLOR = GOLD_COLOR
CHEST_TOOLTIP_COLUMNS = (30, 150, 270)			# column centres relative to the table
CHEST_TOOLTIP_SEPARATORS = (60, 240)
CHEST_TOOLTIP_HEADER_HEIGHT = 26
CHEST_TOOLTIP_ROW_HEIGHT = 35
CHEST_TOOLTIP_HEADER_COLOR = 0xff2d1c11
CHEST_TOOLTIP_BORDER_COLOR = 0xff3f352c
CHEST_TOOLTIP_ROW_COLOR = 0xff18100d
CHEST_TOOLTIP_ROW_EDGE_COLOR = 0xff251b15
CHEST_TOOLTIP_LINE_COLOR = 0xff756657
AVAILABLE_COLOR = 0xff8fd16a
LOCKED_COLOR = 0xff8a7f72

RESULT_TEXT = {
	0 : "MOKOKO_REWARD_SUCCESS",
	1 : "MOKOKO_REWARD_INVALID",
	2 : "MOKOKO_REWARD_LOCKED_ERROR",
	3 : "MOKOKO_REWARD_ALREADY_CLAIMED",
	4 : "MOKOKO_REWARD_INVENTORY_FULL",
	5 : "MOKOKO_REWARD_DATABASE_ERROR",
}

# ---------------------------------------------------------------- state shared with the teleport map page

class _State(object):
	found = 0
	total = 530
	pending = 0
	regions = {}		# region id -> [found, total]
	listeners = []

STATE = _State()

def _Notify():
	for listener in list(STATE.listeners):
		try:
			listener()
		except Exception:
			STATE.listeners.remove(listener)

def AddListener(func):
	if func not in STATE.listeners:
		STATE.listeners.append(func)

def RemoveListener(func):
	if func in STATE.listeners:
		STATE.listeners.remove(func)

def RequestInfo():
	net.SendChatPacket("/mokoko_info")

def OnData(found, total, pending, regions):
	STATE.found = int(found)
	STATE.total = int(total)
	STATE.pending = int(pending)
	STATE.regions = {}
	for entry in regions.split("#"):
		parts = entry.split("|")
		if len(parts) == 3:
			STATE.regions[int(parts[0])] = [int(parts[1]), int(parts[2])]
	_Notify()

def OnFound(found, total, region, regionFound, regionTotal, pending):
	STATE.found = int(found)
	STATE.total = int(total)
	STATE.pending = int(pending)
	STATE.regions[int(region)] = [int(regionFound), int(regionTotal)]
	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOKOKO_FOUND_NOTIFICATION)
	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOKOKO_COLLECTION_PROGRESS.format(STATE.found, STATE.total))
	_Notify()
	if WINDOW and WINDOW.IsShow():
		net.SendChatPacket("/mokoko_rewards")

# ---------------------------------------------------------------- reward window singleton

WINDOW = None

def GetWindow():
	global WINDOW
	if not WINDOW:
		WINDOW = MokokoRewardsWindow()
	return WINDOW

def Open():
	GetWindow().Open()

def Destroy():
	global WINDOW
	if WINDOW:
		WINDOW.Destroy()
		WINDOW = None
	STATE.listeners = []

def OnRewardsBegin(found, total):
	GetWindow().BeginData(int(found), int(total))

def OnRewardEntry(rewardID, rewardType, required, claimed, items):
	GetWindow().AddReward(int(rewardID), int(rewardType), int(required), int(claimed), items)

def OnRewardsEnd(pending):
	GetWindow().EndData(int(pending))

def OnRewardResult(result, rewardID):
	key = RESULT_TEXT.get(int(result))
	if key and hasattr(localeInfo, key):
		chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, key))


class MokokoRewardCard(object):
	# not a window container: every part is a direct child of the reward list, because the engine clips (mask /
	# inside render) only direct children; Position() moves the parts together
	def __init__(self, owner, parent):
		self.owner = owner
		self.parent = parent
		self.reward = None
		self.rowLocked = False
		self.rowUnlockRequired = 0
		self.parts = []			# (window, dx, dy, w, h, kind) kind: "clip" (masked) or "whole" (shown only fully inside)
		self.visible = {}
		self.x = 0
		self.y = 0
		self.shown = False
		self.__Build()

	def __Add(self, wnd, dx, dy, w, h, kind = "clip"):
		wnd.SetParent(self.parent)
		if app.ENABLE_CLIP_MASKING and kind == "clip":
			wnd.SetClippingMaskWindow(self.parent)
		self.parts.append((wnd, dx, dy, w, h, kind))
		self.visible[wnd] = True
		return wnd

	def __Build(self):
		self.board = ui.Button()
		self.board.SetUpVisual(GUILD_PATH % "silver/bg_item.png")
		self.board.SetOverVisual(GUILD_PATH % "silver/bg_item_hover.png")
		self.board.SetDownVisual(GUILD_PATH % "silver/bg_item_select.png")
		self.board.SetEvent(ui.__mem_func__(self.__Claim))
		self.__Add(self.board, 0, 0, CARD_WIDTH, 106)

		# masked text draws black where it is cut, so the header shows only while the row is fully visible
		self.title = self.__Add(self.__Text(GOLD_COLOR), CARD_WIDTH / 2, 4, 1, 14, "whole")

		self.slot = ui.SlotWindow()
		self.slot.SetSize(32, 32)
		self.slot.AppendSlot(0, 0, 0, 32, 32)
		self.slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
		self.slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
		self.slot.SetSelectItemSlotEvent(ui.__mem_func__(self.__ClaimFromSlot))
		self.slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__ClaimFromSlot))
		self.__Add(self.slot, 33, 48, 32, 32, "whole")

		self.boundMarker = self.__Image(BOUND_ITEM_MARKER)
		self.__Add(self.boundMarker, 33, 48, 32, 32)

		self.claimed = ui.Bar()
		self.claimed.SetSize(CARD_WIDTH, 106)
		self.claimed.SetColor(0x99000000)
		self.claimed.AddFlag("not_pick")
		self.__Add(self.claimed, 0, 0, CARD_WIDTH, 106, "whole")

		self.claimedCheck = self.__Add(self.__Image(CHECK_ICON), 35, 39, 28, 28)
		self.locked = self.__Add(self.__Image(GUILD_PATH % "bg_lock.png"), 1, 20, 96, 86)
		# no mask: the long "N Mokoko ile açýlýr" line came out black where the masked text pass dropped glyphs;
		# it stays inside the list horizontally, so it is simply shown only while the row is fully visible
		self.lockedText = self.__Add(self.__Text(TEXT_COLOR), CARD_WIDTH / 2, 50, 1, 14, "whole")

	def __Text(self, color):
		text = ui.TextLine()
		text.SetHorizontalAlignCenter()
		text.SetPackedFontColor(color)
		text.SetOutline()
		return text

	def __Image(self, name):
		# ImageBox, not ExpandedImageBox: the engine applies the clipping mask only to ImageBox
		image = ui.ImageBox()
		image.LoadImage(name)
		image.AddFlag("not_pick")
		return image

	def SetRowLocked(self, isLocked, unlockRequired):
		self.rowLocked = isLocked
		self.rowUnlockRequired = unlockRequired

	def RefreshState(self, reward, found):
		self.reward = reward
		(rewardID, rewardType, required, claimed, items) = reward
		# Baerim card header: "10 <small mokoko icon>"
		self.title.SetText("%d %s" % (required, MOKOKO_SMALL_ICON))
		self.slot.ClearSlot(0)
		bound = False
		if items:
			(vnum, count) = items[0]
			self.slot.SetItemSlot(0, vnum, count)
			item.SelectItem(vnum)
			bound = item.IsAntiFlag(item.ITEM_ANTIFLAG_GIVE)
		self.slot.RefreshSlot()

		isLocked = MOKOKO_ROW_LOCK_ENABLED and self.rowLocked
		self.visible[self.boundMarker] = bound and not isLocked
		self.visible[self.slot] = True	# Baerim: the reward stays visible under the lock
		self.visible[self.locked] = isLocked
		self.visible[self.lockedText] = isLocked
		if isLocked:
			self.lockedText.SetText(localeInfo.MOKOKO_REWARD_ROW_UNLOCK.format(self.rowUnlockRequired))
		self.visible[self.claimed] = bool(claimed) and not isLocked
		self.visible[self.claimedCheck] = bool(claimed) and not isLocked
		self.__Apply()

	def Position(self, x, y):
		self.x = x
		self.y = y
		self.__Apply()

	def SetShow(self, show):
		self.shown = show
		self.__Apply()

	def __Apply(self):
		for (wnd, dx, dy, w, h, kind) in self.parts:
			ax = self.x + dx
			ay = self.y + dy
			wnd.SetPosition(int(ax), int(ay))
			inside = ay + h > 0 and ay < LIST_HEIGHT
			if kind == "whole":
				inside = ay >= 0 and ay + h <= LIST_HEIGHT
			if self.shown and self.visible.get(wnd, True) and inside:
				wnd.Show()
			else:
				wnd.Hide()

	def __Claim(self):
		if self.reward:
			self.owner.ClaimReward(self.reward[0])

	def __ClaimFromSlot(self, slotIndex, window = None):
		self.__Claim()

	def __OverInItem(self, slotIndex, window = None):
		if self.reward and self.reward[4]:
			self.owner.ShowItemToolTip(self.reward[4][0][0])

	def __OverOutItem(self, window = None):
		self.owner.HideItemToolTip()


class MokokoRewardRow(object):
	def __init__(self, owner, parent):
		self.cards = [MokokoRewardCard(owner, parent) for i in xrange(CARDS_PER_ROW)]

	def Position(self, x, y):
		for (i, card) in enumerate(self.cards):
			card.Position(x + i * (CARD_WIDTH + CARD_GAP), y)


class MokokoRewardsWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.rewards = {}
		self.foundCount = 0
		self.totalCount = 530
		self.pendingCount = 0
		self.rows = []
		self.scroll = 0
		self.chestButtons = {}
		self.toolTip = None
		self.itemToolTip = None
		self.__BuildWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.HideItemToolTip()
		self.rows = []
		self.chestButtons = {}
		self.Hide()

	def __Image(self, parent, x, y, name):
		image = ui.ExpandedImageBox()
		image.SetParent(parent)
		image.SetPosition(x, y)
		image.LoadImage(name)
		image.AddFlag("not_pick")
		image.Show()
		return image

	def __Text(self, parent, x, y, text, color, font = None):
		line = ui.TextLine()
		line.SetParent(parent)
		if font:
			line.SetFontName(font)
		line.SetPosition(x, y)
		line.SetHorizontalAlignCenter()
		line.SetPackedFontColor(color)
		line.SetText(text)
		line.Show()
		return line

	def __BuildWindow(self):
		self.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
		self.SetTitleName(localeInfo.MOKOKO_REWARDS_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.AddFlag("movable")
		self.AddFlag("float")

		self.header = self.__Image(self, HEADER_X, HEADER_Y, DAILY_PATH % "header_bg.png")
		self.gaugeEmpty = self.__Image(self, HEADER_X + 22, HEADER_Y + 75, DAILY_PATH % "gauge_activity_empty.png")
		self.gaugeFull = self.__Image(self, HEADER_X + GAUGE_X, HEADER_Y + 78, DAILY_PATH % "gauge_activity_full.png")
		self.progressTitle = self.__Text(self, HEADER_X + 583 / 2, HEADER_Y + 16, "", TITLE_COLOR, "Tahoma:12b")
		self.__BuildChests()

		self.milestoneTitle = self.__Image(self, 12, 158, MOKOKO_PATH % "title_milestone.png")
		self.milestoneBg = self.__Image(self, 12, 187, MOKOKO_PATH % "bg_milestone.png")
		# text on the window, not on the image box (a text child of the image was not drawn)
		self.milestoneText = self.__Text(self, 12 + 587 / 2, 158 + 7, localeInfo.MOKOKO_MILESTONE_REWARDS, TITLE_COLOR, "Tahoma:12b")

		self.rewardList = ui.Window()
		self.rewardList.SetParent(self)
		self.rewardList.SetPosition(12 + 4, 187 + 5)
		self.rewardList.SetSize(LIST_WIDTH, LIST_HEIGHT)
		# no SetInsideRender: its scissor cut the long lock texts of the last column (drawn black); the card parts
		# clip with the mask (images / texts / buttons) or are shown only when fully inside (slot / bar)
		self.rewardList.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheel))
		self.rewardList.Show()

		self.scrollBar = uiDungeonTrack.ScrollBarSpecial()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(12 + 576, 187 + 5)
		self.scrollBar.SetSize(7, LIST_HEIGHT)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBar))
		self.scrollBar.Hide()

	def __BuildChests(self):
		for (index, threshold) in enumerate(CHEST_THRESHOLDS):
			assetID = CHEST_ASSET_IDS[index]
			button = ui.Button()
			button.SetParent(self)
			button.SetPosition(HEADER_X + CHEST_POSITIONS[index], HEADER_Y + 59)
			button.SetUpVisual(DAILY_PATH % ("chest_%d_close_norm.png" % assetID))
			button.SetOverVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
			button.SetDownVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
			button.SetEvent(ui.__mem_func__(self.__ClaimChest), threshold)
			button.SetShowToolTipEvent(ui.__mem_func__(self.__SetupChestTooltip), threshold)
			button.SetHideToolTipEvent(ui.__mem_func__(self.__HideChestTooltip))
			button.Show()
			self.chestButtons[threshold] = button

	# ---------------------------------------------------------------- data from the server

	def BeginData(self, foundCount, totalCount):
		self.foundCount = foundCount
		self.totalCount = totalCount
		self.rewards = {}

	def AddReward(self, rewardID, rewardType, required, claimed, itemPayload):
		items = []
		for itemEntry in itemPayload.split(","):
			parts = itemEntry.split(":")
			if len(parts) == 2:
				(vnum, count) = (int(parts[0]), int(parts[1]))
				items.append((vnum, count))
		self.rewards[rewardID] = (rewardID, rewardType, required, claimed, items)

	def EndData(self, pendingCount):
		self.pendingCount = pendingCount
		STATE.found = self.foundCount
		STATE.total = self.totalCount
		STATE.pending = pendingCount
		_Notify()
		self.__Refresh()

	def GetFoundCount(self):
		return self.foundCount

	# ---------------------------------------------------------------- refresh

	def __Refresh(self):
		self.progressTitle.SetText(localeInfo.MOKOKO_COLLECTION_PROGRESS.format(self.foundCount, self.totalCount))
		self.__RefreshGauge()
		self.__RefreshChests()

		milestones = sorted([reward for reward in self.rewards.values() if reward[1] == 0], key = lambda reward: reward[2])
		rowCount = (len(milestones) + CARDS_PER_ROW - 1) / CARDS_PER_ROW
		while len(self.rows) < rowCount:
			row = MokokoRewardRow(self, self.rewardList)
			self.rows.append(row)
		for (rowIndex, row) in enumerate(self.rows):
			startIndex = rowIndex * CARDS_PER_ROW
			# a row opens when the last milestone of the row above is reached
			unlockRequired = milestones[startIndex - 1][2] if 0 < startIndex <= len(milestones) else 0
			for (i, card) in enumerate(row.cards):
				if startIndex + i < len(milestones):
					card.SetRowLocked(self.foundCount < unlockRequired, unlockRequired)
					card.RefreshState(milestones[startIndex + i], self.foundCount)
					card.SetShow(True)
				else:
					card.SetShow(False)
		self.rows = self.rows[:rowCount]

		overflow = self.__Overflow()
		if overflow > 0:
			# thumb size: the engine drops float sizes, so set it as an integer (ScrollBarSpecial.SetScale gives a float)
			self.scrollBar.SetScale(LIST_HEIGHT, LIST_HEIGHT + overflow)
			self.scrollBar.SetStaticScale(max(12, int(LIST_HEIGHT * LIST_HEIGHT / (LIST_HEIGHT + overflow))))
			self.__SetThumb(overflow)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()
		self.scroll = min(self.scroll, max(0, overflow))
		self.__Layout()

	def __RefreshGauge(self):
		# the bar runs through the chests: piecewise between their centres
		points = [(0, GAUGE_X)]
		for (index, threshold) in enumerate(CHEST_THRESHOLDS):
			points.append((threshold, CHEST_POSITIONS[index] + CHEST_SIZES[index][0] / 2))
		x = GAUGE_X
		for i in xrange(1, len(points)):
			(t0, x0) = points[i - 1]
			(t1, x1) = points[i]
			if self.foundCount >= t1:
				x = x1
				continue
			if self.foundCount > t0:
				x = x0 + (x1 - x0) * (self.foundCount - t0) / (t1 - t0)
			break
		if self.foundCount >= CHEST_THRESHOLDS[-1]:
			x = GAUGE_X + GAUGE_WIDTH
		ratio = max(0.0, min(1.0, float(x - GAUGE_X) / GAUGE_WIDTH))
		self.gaugeFull.SetRenderingRect(0.0, 0.0, ratio - 1.0, 0.0)

	def __RefreshChests(self):
		for (index, threshold) in enumerate(CHEST_THRESHOLDS):
			button = self.chestButtons[threshold]
			assetID = CHEST_ASSET_IDS[index]
			reward = self.__ChestReward(threshold)
			claimed = reward and reward[3]
			available = reward and not claimed and self.foundCount >= threshold
			if claimed:
				openImage = DAILY_PATH % ("chest_%d_open.png" % assetID)
				button.SetUpVisual(openImage)
				button.SetOverVisual(openImage)
				button.SetDownVisual(openImage)
				button.SetPosition(HEADER_X + CHEST_POSITIONS[index] - 2, HEADER_Y + 59 - 15)	# the open chest is taller (lid up)
			elif available:
				button.SetUpVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
				button.SetOverVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
				button.SetDownVisual(DAILY_PATH % ("chest_%d_close_norm.png" % assetID))
				button.SetPosition(HEADER_X + CHEST_POSITIONS[index], HEADER_Y + 59)
			else:
				button.SetUpVisual(DAILY_PATH % ("chest_%d_close_norm.png" % assetID))
				button.SetOverVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
				button.SetDownVisual(DAILY_PATH % ("chest_%d_close_hover.png" % assetID))
				button.SetPosition(HEADER_X + CHEST_POSITIONS[index], HEADER_Y + 59)

	def __ChestReward(self, threshold):
		for reward in self.rewards.values():
			if reward[1] == 1 and reward[2] == threshold:
				return reward
		return None

	def __SetupChestTooltip(self, threshold):
		# Baerim chest tooltip: gold title, then a table "Item | Name | Count" with one icon row per reward item
		reward = self.__ChestReward(threshold)
		if not reward:
			return
		if not self.toolTip:
			self.toolTip = uiToolTip.ToolTip(CHEST_TOOLTIP_WIDTH)
		tip = self.toolTip
		tip.ClearToolTip()
		tip.toolTipWidth = CHEST_TOOLTIP_WIDTH
		left = CHEST_TOOLTIP_TABLE_X
		width = CHEST_TOOLTIP_TABLE_WIDTH
		top = CHEST_TOOLTIP_TABLE_TOP

		def addBar(x, y, w, h, color):
			bar = ui.Bar()
			bar.SetParent(tip)
			bar.SetPosition(x, y)
			bar.SetSize(w, h)
			bar.SetColor(color)
			bar.AddFlag("not_pick")
			bar.Show()
			tip.childrenList.append(bar)

		def addText(value, x, y, color, font = None, center = True):
			line = ui.TextLine()
			line.SetParent(tip)
			if font:
				line.SetFontName(font)
			line.SetPackedFontColor(color)
			line.SetText(value)
			line.SetOutline()
			if center:
				line.SetHorizontalAlignCenter()
			else:
				line.SetHorizontalAlignRight()
			line.SetPosition(x, y)
			line.Show()
			tip.childrenList.append(line)

		def addImage(name, x, y):
			image = ui.ImageBox()
			image.SetParent(tip)
			image.LoadImage(name)
			image.SetPosition(x, y)
			image.AddFlag("not_pick")
			image.Show()
			tip.childrenList.append(image)

		addText(localeInfo.MOKOKO_REWARD_TOOLTIP_TITLE.format(threshold), CHEST_TOOLTIP_WIDTH / 2, CHEST_TOOLTIP_TITLE_Y, CHEST_TOOLTIP_TITLE_COLOR, "Tahoma:12b")
		(colItem, colName, colCount) = [left + c for c in CHEST_TOOLTIP_COLUMNS]
		addBar(left, top, width, CHEST_TOOLTIP_HEADER_HEIGHT, CHEST_TOOLTIP_HEADER_COLOR)
		addBar(left, top, width, 1, CHEST_TOOLTIP_BORDER_COLOR)
		addBar(left, top + CHEST_TOOLTIP_HEADER_HEIGHT - 1, width, 1, CHEST_TOOLTIP_BORDER_COLOR)
		addBar(left, top, 1, CHEST_TOOLTIP_HEADER_HEIGHT, CHEST_TOOLTIP_BORDER_COLOR)
		addBar(left + width - 1, top, 1, CHEST_TOOLTIP_HEADER_HEIGHT, CHEST_TOOLTIP_BORDER_COLOR)
		headerY = top + CHEST_TOOLTIP_HEADER_HEIGHT / 2 - 7
		addText(localeInfo.MOKOKO_REWARD_TOOLTIP_ITEM, colItem, headerY, TITLE_COLOR)
		addText(localeInfo.MOKOKO_REWARD_TOOLTIP_NAME, colName, headerY, TITLE_COLOR)
		addText(localeInfo.MOKOKO_REWARD_TOOLTIP_QTY, colCount, headerY, TITLE_COLOR)

		y = top + CHEST_TOOLTIP_HEADER_HEIGHT
		for (index, (vnum, count)) in enumerate(reward[4]):
			addBar(left, y, width, CHEST_TOOLTIP_ROW_HEIGHT, CHEST_TOOLTIP_ROW_COLOR)
			addBar(left, y, 1, CHEST_TOOLTIP_ROW_HEIGHT, CHEST_TOOLTIP_ROW_EDGE_COLOR)
			addBar(left + width - 1, y, 1, CHEST_TOOLTIP_ROW_HEIGHT, CHEST_TOOLTIP_ROW_EDGE_COLOR)
			for x in CHEST_TOOLTIP_SEPARATORS:
				addBar(left + x, y, 1, CHEST_TOOLTIP_ROW_HEIGHT, CHEST_TOOLTIP_LINE_COLOR)
			if index > 0:
				addBar(left, y, width, 1, CHEST_TOOLTIP_LINE_COLOR)
			item.SelectItem(vnum)
			iconY = y + (CHEST_TOOLTIP_ROW_HEIGHT - 32) / 2
			addImage(item.GetIconImageFileName(), colItem - 16, iconY)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_GIVE):
				addImage(BOUND_ITEM_MARKER, colItem - 16, iconY)
			if count > 1:
				addText(str(count), colItem + 17, iconY + 19, 0xffffffff, None, False)	# slot-style count, bottom right of the icon
			textY = y + CHEST_TOOLTIP_ROW_HEIGHT / 2 - 7
			addText(item.GetItemName(), colName, textY, TEXT_COLOR)
			addText(str(count), colCount, textY, TEXT_COLOR)
			y += CHEST_TOOLTIP_ROW_HEIGHT

		tip.toolTipHeight = y + CHEST_TOOLTIP_BOTTOM - tip.TOOL_TIP_HEIGHT
		tip.ResizeToolTip()
		tip.ShowToolTip()

	def __HideChestTooltip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	def ShowItemToolTip(self, vnum):
		if not self.itemToolTip:
			self.itemToolTip = uiToolTip.ItemToolTip()
		self.itemToolTip.SetItemToolTip(vnum)
		self.itemToolTip.ShowToolTip()

	def HideItemToolTip(self):
		if getattr(self, "itemToolTip", None):
			self.itemToolTip.HideToolTip()
		if getattr(self, "toolTip", None):
			self.toolTip.HideToolTip()

	# ---------------------------------------------------------------- scroll

	def __Overflow(self):
		return max(0, len(self.rows) * ROW_STEP - 5 - LIST_HEIGHT)

	def __Layout(self):
		for (rowIndex, row) in enumerate(self.rows):
			row.Position(ROW_X, rowIndex * ROW_STEP - self.scroll)

	def __SetThumb(self, overflow):
		# integer thumb position (SetPosScale would hand the engine a float)
		track = LIST_HEIGHT - 2 - self.scrollBar.middleBar.GetHeight()
		self.scrollBar.SetPos(int(1 + track * (float(self.scroll) / overflow if overflow else 0)))

	def __OnWheel(self, isDown):
		overflow = self.__Overflow()
		self.scroll = max(0, min(overflow, self.scroll + (50 if isDown else -50)))
		if overflow:
			self.__SetThumb(overflow)
		self.__Layout()

	def __OnScrollBar(self, pos = None):
		overflow = self.__Overflow()
		self.scroll = int(self.scrollBar.GetPos() * overflow)
		self.__Layout()

	def OnMouseWheel(self, nLen):
		self.__OnWheel(nLen < 0)
		return True

	def OnRunMouseWheel(self, nLen):
		return self.OnMouseWheel(nLen)

	# ---------------------------------------------------------------- claim

	def __ClaimAchievementAt(self, threshold, reward):
		self.ClaimReward(reward[0])

	def __ClaimChest(self, threshold):
		reward = self.__ChestReward(threshold)
		if reward:
			self.__ClaimAchievementAt(threshold, reward)

	def ClaimReward(self, rewardID):
		reward = self.rewards.get(rewardID)
		if not reward:
			return
		if reward[3]:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOKOKO_REWARD_ALREADY_CLAIMED)
			return
		if self.foundCount < reward[2]:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOKOKO_REWARD_LOCKED_ERROR)
			return
		net.SendChatPacket("/mokoko_claim %d" % rewardID)

	def ShowResult(self, resultCode, rewardID):
		OnRewardResult(resultCode, rewardID)

	# ---------------------------------------------------------------- open / close

	def Open(self):
		if not self.IsShow():
			(w, h) = (wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
			self.SetPosition((w - WINDOW_WIDTH) / 2, (h - WINDOW_HEIGHT) / 2)
		self.Show()
		self.SetTop()
		net.SendChatPacket("/mokoko_rewards")

	def Close(self):
		self.HideItemToolTip()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
