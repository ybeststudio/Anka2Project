if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import item
import localeInfo
import uiToolTip
import constInfo
import uiDungeonTrack

# Baerim "Zindan Istatistikleri" (uiDungeonStat, logManager of the teleport window) rebuilt from its runtime dump
# (Reference/pydump_runtime/dungeonprobe): window 452x500, page tabs Siralama / Gorevler, dungeon selector bottom right.
# Siralama: 3 ranking tabs (completed / fastest / boss dps), 5 class filters, top 10 rows.
# Gorevler: 9 missions per dungeon (time / dps / completed) with a claimable reward.
# Server: /dungeon_info stat_rank|missions|mission_claim (game/src/dungeon_window.cpp).

RANKING_PATH = "d:/ymir work/ui/game/ranking/%s"
DUNGEON_PATH = "d:/ymir work/ui/game/teleport_window/dungeon/%s"
MISSION_PATH = "d:/ymir work/ui/game/teleport_window/dungeon/missions/%s"
COMMON_PATH = "d:/ymir work/ui/game/common/%s"
SELECT_PATH = "d:/ymir work/ui/game/guild_application/%s"

TEXT_COLOR = 0xffcebb98			# measured on Baerim's ranking (headers and rows)
SELECT_COLOR = 0xfff3e9c8		# dungeon selector text, measured
SELECTED_COLOR = 0xffffc800		# the selected dungeon in the open list (Baerim: yellow); to be measured from a PNG
PROGRESS_COLOR = 0xffeee3d2

WIDTH = 452
HEIGHT = 500

PAGE_RANKING = 0
PAGE_MISSIONS = 1

RANK_COMPLETED = 0
RANK_FASTEST = 1
RANK_DPS = 2

RANK_TABS = (
	(7, "btn_completed", "DUNGEON_STAT_RANK_COMPLETED", "DUNGEON_STAT_VALUE_COMPLETED"),
	(141, "btn_time", "DUNGEON_STAT_RANK_FASTEST", "DUNGEON_STAT_VALUE_TIME"),
	(275, "btn_damage", "DUNGEON_STAT_RANK_DPS", "DUNGEON_STAT_VALUE_DPS"),
)
RACE_BUTTONS = ("btn_0", "btn_warrior", "btn_ninja", "btn_sura", "btn_shaman")

ROWS = 10
ROW_Y = 179
ROW_STEP = 26
COLUMNS = (29, 111, 209, 307, 405)	# rank, name, level, class, value (centred)

MISSION_LIST_X = 14
MISSION_LIST_Y = 70
MISSION_LIST_W = 423
MISSION_LIST_H = 385
MISSION_STEP = 61
MISSION_ICONS = { "t" : "icon_time.png", "d" : "icon_dmg.png", "c" : "icon_completed.png" }

SELECT_ITEM_H = 22


def _Image(parent, name, x, y, pick = False, expanded = True):
	# ImageBox is clipped by a mask window, ExpandedImageBox is not (CExpandedImageBox::OnRender ignores the mask):
	# everything inside the scrolled mission list must be a plain ImageBox
	image = ui.ExpandedImageBox() if expanded else ui.ImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	if not pick:
		image.AddFlag("not_pick")
	image.Show()
	return image

def _Text(parent, x, y, text = "", align = "center", color = TEXT_COLOR):
	line = ui.TextLine()
	line.SetParent(parent)
	line.SetPosition(x, y)
	if align == "center":
		line.SetHorizontalAlignCenter()
	elif align == "right":
		line.SetHorizontalAlignRight()
	line.SetPackedFontColor(color)
	line.SetText(text)
	line.Show()
	return line

def _Button(parent, x, y, norm, over, down, radio = False):
	# "radio" buttons are plain buttons whose selected state is drawn by _Select (the C++ RadioButton of this client
	# did not keep its pressed look and then ignored the click on another tab)
	button = ui.Button()
	button.visuals = (norm, over, down)
	button.SetParent(parent)
	button.SetPosition(x, y)
	button.SetUpVisual(norm)
	button.SetOverVisual(over)
	button.SetDownVisual(down)
	button.Show()
	return button

def _Select(button, selected):
	(norm, over, down) = button.visuals
	if selected:
		(norm, over) = (down, down)
	button.SetUpVisual(norm)
	button.SetOverVisual(over)
	button.SetDownVisual(down)

def _RaceName(job):
	if job == 8:
		return localeInfo.JOB_WOLFMAN
	if 0 <= job < 8:
		return getattr(localeInfo, "DUNGEON_STAT_RACE_%d" % (job % 4 + 1))
	return "-"

def _FormatValue(rankIdx, value):
	if rankIdx == RANK_FASTEST:
		return uiDungeonTrack.FormatTime(value)
	return uiDungeonTrack.NumberToDecimalString(value)


class MissionScrollBar(ui.Window):
	# Baerim's ScrollBarSpecial look (5 px, grey frame, grey thumb) with integer sizes only:
	# uiDungeonTrack.ScrollBarSpecial sizes its thumb with floats, which the window manager ignores (invisible thumb)
	FRAME_COLOR = 0xff575757
	BASE_COLOR = 0xff000000
	THUMB_COLOR = 0xffa5a5a5

	def __init__(self):
		ui.Window.__init__(self)
		self.event = None
		self.thumbHeight = 10
		self.parts = []
		for color in (self.FRAME_COLOR, self.BASE_COLOR):
			bar = ui.Bar()
			bar.SetParent(self)
			bar.SetColor(color)
			bar.AddFlag("not_pick")
			bar.Show()
			self.parts.append(bar)

		self.thumb = ui.DragButton()
		self.thumb.SetParent(self)
		self.thumb.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.thumb.Show()
		self.thumbBar = ui.Bar()
		self.thumbBar.SetParent(self.thumb)
		self.thumbBar.SetColor(self.THUMB_COLOR)
		self.thumbBar.AddFlag("not_pick")
		self.thumbBar.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def SetScrollEvent(self, event):
		self.event = event

	def SetSize(self, width, height):
		ui.Window.SetSize(self, width, height)
		self.parts[0].SetPosition(0, 0)
		self.parts[0].SetSize(width, height)
		self.parts[1].SetPosition(1, 1)
		self.parts[1].SetSize(width - 2, height - 2)
		self.__ResizeThumb()

	def SetScale(self, viewSize, fullSize):
		inner = self.GetHeight() - 2
		self.thumbHeight = max(10, min(inner, int(inner * viewSize / max(1, fullSize))))
		self.__ResizeThumb()

	def __ResizeThumb(self):
		width = self.GetWidth() - 2
		self.thumb.SetSize(width, self.thumbHeight)
		self.thumbBar.SetSize(width, self.thumbHeight)
		self.thumb.SetRestrictMovementArea(1, 1, width, self.GetHeight() - 2)

	def __Range(self):
		return max(1, self.GetHeight() - 2 - self.thumbHeight)

	def SetPosScale(self, scale):
		scale = max(0.0, min(1.0, scale))
		self.thumb.SetPosition(1, 1 + int(round(self.__Range() * scale)))

	def GetPosScale(self):
		return max(0.0, min(1.0, float(self.thumb.GetLocalPosition()[1] - 1) / self.__Range()))

	def __OnMove(self):
		if self.event:
			self.event(self.GetPosScale())

	def OnMouseLeftButtonDown(self):
		(x, y) = self.GetMouseLocalPosition()
		self.thumb.SetPosition(1, max(1, min(1 + self.__Range(), y - self.thumbHeight / 2)))
		self.__OnMove()


class MissionRow(ui.Window):
	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = owner
		self.mission = None
		self.SetSize(MISSION_LIST_W, 57)

		self.bg = _Image(self, MISSION_PATH % "bg_mission.png", 0, 0, False, False)
		self.icon = _Image(self, MISSION_PATH % "icon_time.png", 5, 4, False, False)
		self.title = _Text(self, 62, 6, "", "left", TEXT_COLOR)
		self.gaugeEmpty = _Image(self, MISSION_PATH % "gauge_empty.png", 61, 31, False, False)
		# needs SetRenderingRect, so it stays an ExpandedImageBox (not clipped): shown only while the row is fully visible
		self.gaugeFull = _Image(self, MISSION_PATH % "gauge_full.png", 64, 34)
		self.progress = _Text(self, 61 + 142, 34, "", "center", PROGRESS_COLOR)

		self.slot = _Image(self, COMMON_PATH % "slots/1.png", 359, 10, True, False)
		self.slot.OnMouseOverIn = ui.__mem_func__(self.__ShowItem)
		self.slot.OnMouseOverOut = ui.__mem_func__(self.__HideItem)
		self.itemIcon = _Image(self.slot, COMMON_PATH % "slots/1.png", 2, 2, False, False)
		self.itemCount = _Text(self.slot, 33, 21, "", "right", 0xffffffff)

		self.claim = _Button(self, 398, 17, COMMON_PATH % "btn_check_normal.png", COMMON_PATH % "btn_check_hover.png", COMMON_PATH % "btn_check_down.png")
		self.claim.SetEvent(ui.__mem_func__(self.__Claim))
		owner.BindToolTip(self.claim, localeInfo.DUNGEON_STAT_MISSION_CLAIM)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.owner = None

	def SetMask(self, window):
		# same as the dungeon rows of uiDungeonBaerim: the mask of the row clips its children
		if app.ENABLE_CLIP_MASKING:
			self.SetClippingMaskWindow(window)

	def SetMission(self, mission, stats, claimedMask):
		(no, kind, target, vnum, count) = mission
		(fastest, dps, completed) = stats
		self.mission = mission

		self.icon.LoadImage(MISSION_PATH % MISSION_ICONS.get(kind, "icon_time.png"))
		if kind == "t":
			self.title.SetText(localeInfo.DUNGEON_STAT_MISSION_TIME % uiDungeonTrack.FormatTime(target))
			done = fastest > 0 and fastest <= target
			ratio = 1.0 if done else (float(target) / fastest if fastest > 0 else 0.0)
			current = uiDungeonTrack.FormatTime(fastest) if fastest > 0 else "-"
			goal = uiDungeonTrack.FormatTime(target)
		elif kind == "d":
			self.title.SetText(localeInfo.DUNGEON_STAT_MISSION_DPS % uiDungeonTrack.NumberToDecimalString(target))
			done = dps >= target
			ratio = float(dps) / target if target > 0 else 1.0
			current = uiDungeonTrack.NumberToDecimalString(min(dps, target))
			goal = uiDungeonTrack.NumberToDecimalString(target)
		else:
			self.title.SetText(localeInfo.DUNGEON_STAT_MISSION_COMPLETED % uiDungeonTrack.NumberToDecimalString(target))
			done = completed >= target
			ratio = float(completed) / target if target > 0 else 1.0
			current = uiDungeonTrack.NumberToDecimalString(min(completed, target))
			goal = uiDungeonTrack.NumberToDecimalString(target)

		ratio = max(0.0, min(1.0, ratio))
		self.gaugeFull.SetRenderingRect(0.0, 0.0, ratio - 1.0, 0.0)
		self.progress.SetText(localeInfo.DUNGEON_STAT_MISSION_PROGRESS % (current, goal))

		item.SelectItem(vnum)
		self.itemIcon.LoadImage(item.GetIconImageFileName())
		self.itemCount.SetText(str(count) if count > 1 else "")

		claimed = (claimedMask >> no) & 1
		self.bg.LoadImage(MISSION_PATH % ("bg_mission_completed.png" if claimed else "bg_mission.png"))
		if done and not claimed:
			self.claim.Show()
		else:
			self.claim.Hide()

	def SetListPosition(self, y, visible = True):
		self.SetPosition(0, y)
		# a clipped row still takes the mouse: rows scrolled out of the list would cover the page tabs
		if not visible or y + 57 <= 0 or y >= MISSION_LIST_H:
			self.Hide()
			return
		self.Show()
		if y + 34 >= 0 and y + 34 + 14 <= MISSION_LIST_H:
			self.gaugeFull.Show()
		else:
			self.gaugeFull.Hide()

	def __Claim(self):
		if self.owner and self.mission:
			self.owner.ClaimMission(self.mission[0])

	def __ShowItem(self):
		if not self.mission:
			return
		interface = constInfo.GetInterfaceInstance()
		if interface and getattr(interface, "tooltipItem", None):
			interface.tooltipItem.SetItemToolTip(self.mission[3])
			interface.tooltipItem.ShowToolTip()

	def __HideItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface and getattr(interface, "tooltipItem", None):
			interface.tooltipItem.HideToolTip()

	def OnRunMouseWheel(self, nLen):
		if self.owner:
			self.owner.ScrollMissions(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)


class DungeonStatWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.isLoaded = False
		self.decor = []
		self.toolTip = None
		self.bossIdx = 0
		self.pageIdx = PAGE_RANKING
		self.rankIdx = RANK_COMPLETED
		self.raceIdx = 0
		self.missionRows = []
		self.missionScroll = 0
		self.selectItems = []

	def __del__(self):
		ui.Window.__del__(self)

	# ---------------------------------------------------------------- build

	def __Keep(self, window):
		self.decor.append(window)
		return window

	def __Load(self):
		if self.isLoaded:
			return
		self.isLoaded = True

		self.SetSize(WIDTH, HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")

		board = self.__Keep(ui.BoardWithTitleBar())
		board.SetParent(self)
		board.SetSize(WIDTH, HEIGHT)
		board.SetTitleName(localeInfo.DUNGEON_STAT_TITLE)
		board.SetCloseEvent(ui.__mem_func__(self.Close))
		board.AddFlag("not_pick")
		board.Show()

		self.__BuildRanking()
		self.__BuildMissions()
		self.__BuildSelector()

		# created last: the mouse picks the last child first, so a mission row scrolled half out of the list
		# (clipped, but still taking the mouse) can not cover the page tabs (SetTop only works on float windows)
		self.pageTabs = []
		for (i, (x, key)) in enumerate(((125, "DUNGEON_STAT_TAB_RANKING"), (226, "DUNGEON_STAT_TAB_MISSIONS"))):
			tab = _Button(self, x, 33, COMMON_PATH % "button_large_normal.png", COMMON_PATH % "button_large_hover.png", COMMON_PATH % "button_large_down.png", True)
			tab.SetText(getattr(localeInfo, key))
			tab.SetEvent(ui.__mem_func__(self.SelectPage), i)
			self.pageTabs.append(tab)

	def __BuildRanking(self):
		self.rankingWidgets = []
		add = self.rankingWidgets.append

		self.rankTabs = []
		for (i, (x, image, tip, value)) in enumerate(RANK_TABS):
			tab = _Button(self, x, 62, DUNGEON_PATH % (image + "_norm.png"), DUNGEON_PATH % (image + "_hover.png"), DUNGEON_PATH % (image + "_select.png"), True)
			tab.SetEvent(ui.__mem_func__(self.SelectRank), i)
			self.BindToolTip(tab, getattr(localeInfo, tip))
			self.rankTabs.append(tab)
			add(tab)

		add(_Image(self, RANKING_PATH % "background_right.png", 7, 101))

		self.raceButtons = []
		for (i, image) in enumerate(RACE_BUTTONS):
			button = _Button(self, 13 + 86 * i, 108, RANKING_PATH % ("races/%s_norm.png" % image), RANKING_PATH % ("races/%s_hover.png" % image), RANKING_PATH % ("races/%s_select.png" % image), True)
			button.SetEvent(ui.__mem_func__(self.SelectRace), i)
			self.BindToolTip(button, getattr(localeInfo, "DUNGEON_STAT_RACE_%d" % i))
			self.raceButtons.append(button)
			add(button)

		add(_Image(self, RANKING_PATH % "ranking_rows.png", 12, 147))
		add(_Image(self, RANKING_PATH % "crown.png", COLUMNS[0] - 8, 154))
		add(_Text(self, COLUMNS[1], 152, localeInfo.DUNGEON_STAT_COL_NAME))
		add(_Text(self, COLUMNS[2], 152, localeInfo.DUNGEON_STAT_COL_LEVEL))
		add(_Text(self, COLUMNS[3], 152, localeInfo.DUNGEON_STAT_COL_RACE))
		self.valueTitle = _Text(self, COLUMNS[4], 152, "")
		add(self.valueTitle)

		self.rankRows = []
		for i in xrange(ROWS):
			y = ROW_Y + ROW_STEP * i
			line = [_Text(self, x, y, "") for x in COLUMNS]
			self.rankRows.append(line)
			for text in line:
				add(text)

		self.rankEmpty = _Text(self, 226, 302, localeInfo.DUNGEON_STAT_RANK_EMPTY)
		add(self.rankEmpty)

	def __BuildMissions(self):
		self.missionWidgets = []
		self.missionWidgets.append(_Image(self, MISSION_PATH % "bg_list.png", 6, 62))

		self.missionList = ui.Window()
		self.missionList.SetParent(self)
		self.missionList.SetPosition(MISSION_LIST_X, MISSION_LIST_Y)
		self.missionList.SetSize(MISSION_LIST_W, MISSION_LIST_H)
		self.missionList.SetOnRunMouseWheelEvent(ui.__mem_func__(self.ScrollMissions))
		self.missionList.Show()
		self.missionWidgets.append(self.missionList)

		self.missionScrollBar = MissionScrollBar()
		self.missionScrollBar.SetParent(self)
		self.missionScrollBar.SetPosition(438, 70)
		self.missionScrollBar.SetSize(5, MISSION_LIST_H)
		self.missionScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnMissionScrollBar))
		self.missionScrollBar.Hide()

		# shown only by OnMissions when the server sends no missions (not on every page switch: until the answer
		# arrives it would stand on the rows of the last list)
		self.missionEmpty = _Text(self, 226, 252, localeInfo.DUNGEON_STAT_MISSION_EMPTY)
		self.missionEmpty.Hide()

	def __BuildSelector(self):
		self.selectButton = _Button(self, 331, 467, SELECT_PATH % "select_input_norm.png", SELECT_PATH % "select_input_hover.png", SELECT_PATH % "select_input_select.png")
		self.selectButton.SetEvent(ui.__mem_func__(self.__ToggleSelect))
		self.selectText = _Text(self.selectButton, 55, 4, "", "center", SELECT_COLOR)
		self.selectText.AddFlag("not_pick")

		self.selectList = ui.Bar()
		self.selectList.SetParent(self)
		self.selectList.SetColor(0xf0000000)
		self.selectList.Hide()

	# ---------------------------------------------------------------- tooltips

	def BindToolTip(self, window, text):
		window.SetShowToolTipEvent(ui.__mem_func__(self.__ShowToolTip), text)
		window.SetHideToolTipEvent(ui.__mem_func__(self.__HideToolTip))

	def __ShowToolTip(self, text):
		if not self.toolTip:
			self.toolTip = uiToolTip.ToolTip(0)
			self.toolTip.HideToolTip()
		self.toolTip.ClearToolTip()
		self.toolTip.toolTipWidth = 0
		self.toolTip.AutoAppendTextLine(text, 0xffc1c1c1)
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.ShowToolTip()

	def __HideToolTip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	# ---------------------------------------------------------------- dungeon selector

	def __Dungeons(self):
		data = uiDungeonTrack.dungeonData
		return [idx for (listIndex, idx) in sorted([(info["listIndex"], idx) for (idx, info) in data.items()])]

	def __ToggleSelect(self):
		if self.selectList.IsShow():
			self.selectList.Hide()
			return

		for button in self.selectItems:
			button.Hide()
		self.selectItems = []

		dungeons = self.__Dungeons()
		height = SELECT_ITEM_H * len(dungeons)
		self.selectList.SetPosition(331, 467 - height)
		self.selectList.SetSize(111, height)
		for (i, bossIdx) in enumerate(dungeons):
			button = _Button(self.selectList, 0, SELECT_ITEM_H * i, SELECT_PATH % "select_input_norm.png", SELECT_PATH % "select_input_hover.png", SELECT_PATH % "select_input_select.png")
			button.SetEvent(ui.__mem_func__(self.SelectDungeon), bossIdx)
			text = _Text(button, 55, 4, uiDungeonTrack.dungeonData[bossIdx]["name"], "center", SELECTED_COLOR if bossIdx == self.bossIdx else SELECT_COLOR)
			text.AddFlag("not_pick")
			button.text = text
			self.selectItems.append(button)
		self.selectList.Show()
		self.selectList.SetTop()

	# ---------------------------------------------------------------- state

	def SelectPage(self, pageIdx):
		self.pageIdx = pageIdx
		for (i, tab) in enumerate(self.pageTabs):
			_Select(tab, i == pageIdx)

		for widget in self.rankingWidgets:
			if pageIdx == PAGE_RANKING:
				widget.Show()
			else:
				widget.Hide()
		for widget in self.missionWidgets:
			if pageIdx == PAGE_MISSIONS:
				widget.Show()
			else:
				widget.Hide()

		if pageIdx == PAGE_RANKING:
			self.missionEmpty.Hide()
			self.missionScrollBar.Hide()
			self.SelectRank(self.rankIdx)
		else:
			self.__RequestMissions()
		self.selectList.Hide()

	def SelectRank(self, rankIdx):
		self.rankIdx = rankIdx
		for (i, tab) in enumerate(self.rankTabs):
			_Select(tab, i == rankIdx)
		self.valueTitle.SetText(getattr(localeInfo, RANK_TABS[rankIdx][3]))
		self.__RequestRanking()

	def __MarkRace(self):
		for (i, button) in enumerate(self.raceButtons):
			_Select(button, i == self.raceIdx)

	def SelectRace(self, raceIdx):
		self.raceIdx = raceIdx
		self.__MarkRace()
		self.__RequestRanking()

	def __SetDungeon(self, bossIdx):
		self.bossIdx = bossIdx
		self.missionScroll = 0
		data = uiDungeonTrack.dungeonData.get(bossIdx)
		self.selectText.SetText(data["name"] if data else localeInfo.DUNGEON_STAT_SELECT_DUNGEON)
		self.selectList.Hide()

	def SelectDungeon(self, bossIdx):
		self.__SetDungeon(bossIdx)
		if self.pageIdx == PAGE_RANKING:
			self.__RequestRanking()
		else:
			self.__RequestMissions()

	# ---------------------------------------------------------------- ranking

	def __ClearRanking(self):
		for line in self.rankRows:
			for text in line:
				text.SetText("")
		self.rankEmpty.Hide()

	def __RequestRanking(self):
		self.__ClearRanking()
		if self.bossIdx:
			net.SendChatPacket("/dungeon_info stat_rank %d %d %d" % (self.bossIdx, self.rankIdx, self.raceIdx))

	# DungeonStatRank <boss> <rank> <race> <name|value|level|job#... | ->
	def OnRanking(self, bossIdx, rankIdx, raceIdx, data):
		if bossIdx != self.bossIdx or rankIdx != self.rankIdx or raceIdx != self.raceIdx:
			return
		self.__ClearRanking()
		rows = [entry.split("|") for entry in data.split("#") if entry and entry != "-"]
		for (i, row) in enumerate(rows[:ROWS]):
			if len(row) < 4:
				continue
			(name, value, level, job) = (row[0], int(row[1]), int(row[2]), int(row[3]))
			texts = self.rankRows[i]
			texts[0].SetText(str(i + 1))
			texts[1].SetText(name)
			texts[2].SetText(str(level))
			texts[3].SetText(_RaceName(job))
			texts[4].SetText(_FormatValue(rankIdx, value))
		if not rows and self.pageIdx == PAGE_RANKING:
			self.rankEmpty.Show()

	# ---------------------------------------------------------------- missions

	def __RequestMissions(self):
		if self.bossIdx:
			net.SendChatPacket("/dungeon_info missions %d" % self.bossIdx)

	def ClaimMission(self, no):
		if self.bossIdx:
			net.SendChatPacket("/dungeon_info mission_claim %d %d" % (self.bossIdx, no))

	# DungeonMissionData <boss> <fastest> <dps> <completed> <claimed mask> <no:type:target:vnum:count#... | ->
	def OnMissions(self, bossIdx, fastest, dps, completed, claimedMask, data):
		if bossIdx != self.bossIdx:
			return

		missions = []
		for entry in data.split("#"):
			fields = entry.split(":")
			if len(fields) == 5:
				missions.append((int(fields[0]), fields[1], int(fields[2]), int(fields[3]), int(fields[4])))

		while len(self.missionRows) < len(missions):
			row = MissionRow(self)
			row.SetParent(self.missionList)
			row.SetMask(self.missionList)
			self.missionRows.append(row)

		for (i, row) in enumerate(self.missionRows):
			if i < len(missions):
				row.SetMission(missions[i], (fastest, dps, completed), claimedMask)

		self.missionCount = len(missions)
		if self.pageIdx == PAGE_MISSIONS:
			if missions:
				self.missionEmpty.Hide()
			else:
				self.missionEmpty.Show()
		self.__SetMissionScroll(self.missionScroll)

	def __MissionOverflow(self):
		return max(0, getattr(self, "missionCount", 0) * MISSION_STEP - (MISSION_STEP - 57) - MISSION_LIST_H)

	def __SetMissionScroll(self, value):
		overflow = self.__MissionOverflow()
		self.missionScroll = max(0, min(overflow, int(value)))
		for (i, row) in enumerate(self.missionRows):
			row.SetListPosition(MISSION_STEP * i - self.missionScroll, i < getattr(self, "missionCount", 0))
		if overflow > 0 and self.pageIdx == PAGE_MISSIONS:
			self.missionScrollBar.SetScale(MISSION_LIST_H, MISSION_LIST_H + overflow)
			self.missionScrollBar.SetPosScale(float(self.missionScroll) / overflow)
			self.missionScrollBar.Show()
		else:
			self.missionScrollBar.Hide()

	def __OnMissionScrollBar(self, scale):
		overflow = self.__MissionOverflow()
		self.missionScroll = int(scale * overflow)
		for (i, row) in enumerate(self.missionRows):
			row.SetListPosition(MISSION_STEP * i - self.missionScroll, i < getattr(self, "missionCount", 0))

	def ScrollMissions(self, nLen):
		self.__SetMissionScroll(self.missionScroll + (-MISSION_STEP if nLen > 0 else MISSION_STEP))
		return True

	def OnMissionResult(self, result, bossIdx, no):
		text = getattr(localeInfo, "DUNGEON_STAT_MISSION_RESULT_%d" % result, "")
		if text:
			import chat
			chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	# ---------------------------------------------------------------- open / close

	def Open(self, bossIdx, pageIdx = PAGE_RANKING):
		self.__Load()
		self.__SetDungeon(bossIdx)
		self.__MarkRace()
		self.SelectPage(pageIdx)	# one request for the shown page
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def Close(self):
		self.__HideToolTip()
		if self.isLoaded:
			self.selectList.Hide()
		self.Hide()

	def Destroy(self):
		for row in self.missionRows:
			row.Destroy()
		self.missionRows = []
		self.selectItems = []
		self.decor = []

	def OnPressEscapeKey(self):
		self.Close()
		return True
