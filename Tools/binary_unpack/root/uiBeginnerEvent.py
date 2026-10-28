# -*- coding: utf-8 -*-
if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
import ui
import localeInfo
import uiScriptLocale
import dbg
import wikicontent

TRUE = True
FALSE = False

BEGINNER_QUEST_IMAGE_DIR = "d:/ymir work/ui/game/beginner_quest/"
BEGINNER_REWARD_IMAGE_DIR = "d:/ymir work/ui/game/beginner/"
BEGINNER_SHOP_IMAGE = "d:/ymir work/ui/game/common/shop_btn.png"
BEGINNER_REWARD_CONFIG = "config_json/beginner.json"

class BeginnerRewardScrollBar(ui.Window):
	WIDTH = 5
	HEIGHT = 326

	def __init__(self, itemCount):
		ui.Window.__init__(self)
		self.SetSize(self.WIDTH, self.HEIGHT)
		self.scrollPos = 0.0
		self.step = 0.2
		self.onScroll = lambda: None
		self.thumbHeight = min(self.HEIGHT, int(float(self.HEIGHT) * 6 / max(6, itemCount)) + 10)

		self.track = ui.Bar()
		self.track.SetParent(self)
		self.track.AddFlag("not_pick")
		self.track.SetSize(self.WIDTH, self.HEIGHT)
		self.track.SetColor(0xff6b655a)
		self.track.Show()

		self.trackInside = ui.Bar()
		self.trackInside.SetParent(self)
		self.trackInside.AddFlag("not_pick")
		self.trackInside.SetPosition(1, 0)
		self.trackInside.SetSize(3, self.HEIGHT)
		self.trackInside.SetColor(0xff17120f)
		self.trackInside.Show()

		self.thumb = ui.DragButton()
		self.thumb.SetParent(self)
		self.thumb.SetPosition(1, 0)
		self.thumb.SetSize(3, self.thumbHeight)
		self.thumb.SetRestrictMovementArea(1, 0, 1, self.HEIGHT)
		self.thumb.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.thumb.Show()

		self.thumbFill = ui.Bar()
		self.thumbFill.SetParent(self.thumb)
		self.thumbFill.AddFlag("not_pick")
		self.thumbFill.SetSize(3, self.thumbHeight)
		self.thumbFill.SetColor(0xff6b655a)
		self.thumbFill.Show()

	def __OnMove(self):
		travel = max(1, self.HEIGHT - self.thumbHeight)
		self.scrollPos = min(1.0, max(0.0, float(self.thumb.GetLocalPosition()[1]) / travel))
		self.onScroll()

	def SetScrollEvent(self, event):
		self.onScroll = event

	def SetScrollStep(self, step):
		self.step = step

	def GetPos(self):
		return self.scrollPos

	def SetPos(self, position):
		self.scrollPos = min(1.0, max(0.0, position))
		self.thumb.SetPosition(1, int((self.HEIGHT - self.thumbHeight) * self.scrollPos))
		self.onScroll()

	def OnMouseWheel(self, delta):
		if delta:
			self.SetPos(self.scrollPos - self.step if delta > 0 else self.scrollPos + self.step)
			return True
		return False

class BeginnerEventWindow(ui.ScriptWindow):
	QUESTS = (
		(localeInfo.BEGINNER_EVENT_METINSTONES, "metinstones", 100),
		(localeInfo.BEGINNER_EVENT_BOSSES, "bosses", 10),
		(localeInfo.BEGINNER_EVENT_DUNGEONS, "dungeons", 10),
		(localeInfo.BEGINNER_EVENT_UPGRADE_ITEM, "upgrade_item", 3),
		(localeInfo.BEGINNER_EVENT_EXPLORATION, "exploration", 1),
		(localeInfo.BEGINNER_EVENT_SPIRIT, "spirit", 20),
	)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.board = None
		self.widgets = []
		self.questWidgets = []
		self.rewardWidgets = []
		self.rewardRows = []
		self.rewardData = []
		self.claimedLevels = set()
		self.activeWidgets = self.questWidgets
		self.currentTab = 0
		self.__BuildWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __AddImage(self, parent, path, x, y):
		image = ui.ImageBox()
		image.SetParent(parent)
		image.AddFlag("not_pick")
		image.SetPosition(x, y)
		image.LoadImage(path if path.startswith("d:/") else BEGINNER_QUEST_IMAGE_DIR + path)
		image.Show()
		self.widgets.append(image)
		self.activeWidgets.append(image)
		return image

	def __AddText(self, parent, text, x, y, color=0xffe8d8b5, centered=FALSE):
		line = ui.TextLine()
		line.SetParent(parent)
		line.AddFlag("not_pick")
		line.SetPackedFontColor(color)
		line.SetOutline()
		if centered:
			line.SetHorizontalAlignCenter()
		line.SetPosition(x, y)
		line.SetText(text)
		line.Show()
		self.widgets.append(line)
		self.activeWidgets.append(line)
		return line

	def __AddTab(self, name, x, y, tabIndex):
		button = ui.Button()
		button.SetParent(self)
		button.SetPosition(x, y)
		button.SetUpVisual(BEGINNER_QUEST_IMAGE_DIR + "tab_" + name + "_norm.png")
		button.SetOverVisual(BEGINNER_QUEST_IMAGE_DIR + "tab_" + name + "_hover.png")
		button.SetDownVisual(BEGINNER_QUEST_IMAGE_DIR + "tab_" + name + "_select.png")
		if tabIndex < 2:
			button.SetEvent(ui.__mem_func__(self.__SelectTab), tabIndex)
		button.Show()
		self.widgets.append(button)
		return button

	def __LoadRewardData(self):
		try:
			config = wikicontent.LoadCommonJson(BEGINNER_REWARD_CONFIG)
			if not config:
				raise ValueError("reward config unavailable")
			for entry in config["rewards"]:
				level = int(entry["level"])
				items = [(int(item["vnum"]), int(item["count"])) for item in entry["items"]]
				if level <= 0 or len(items) != 3 or not all(vnum > 0 and count > 0 for vnum, count in items):
					raise ValueError("invalid reward level %d" % level)
				self.rewardData.append((level, items))
			self.rewardData.sort()
		except (IOError, ValueError, KeyError, TypeError) as error:
			dbg.TraceError("BeginnerEvent reward config: %s" % error)
			self.rewardData = []

	def __BuildRewardPage(self):
		self.activeWidgets = self.rewardWidgets
		self.__LoadRewardData()
		self.__AddImage(self.board, BEGINNER_REWARD_IMAGE_DIR + "bg.png", 9, 32)
		for rowIndex in xrange(6):
			y = 33 + rowIndex * 55
			self.__AddImage(self.board, BEGINNER_REWARD_IMAGE_DIR + "box.png", 9, y)
			levelImage = self.__AddImage(self.board, BEGINNER_REWARD_IMAGE_DIR + "level_10.png", 29, y + 14)
			slots = ui.SlotWindow()
			slots.SetParent(self.board)
			slots.SetPosition(210, y + 7)
			slots.SetSize(96, 32)
			slots.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			slots.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnRewardWheel))
			for slotIndex in xrange(3):
				slots.AppendSlot(slotIndex, slotIndex * 32, 0, 32, 32)
			slots.Show()
			self.widgets.append(slots)
			self.rewardWidgets.append(slots)
			claimed = self.__AddImage(self.board, BEGINNER_REWARD_IMAGE_DIR + "claimed.png", 9, y)
			self.rewardRows.append((levelImage, slots, claimed))
		self.rewardScroll = BeginnerRewardScrollBar(len(self.rewardData))
		self.rewardScroll.SetParent(self.board)
		self.rewardScroll.SetPosition(337, 35)
		self.rewardScroll.SetScrollStep(1.0 / max(1, len(self.rewardData) - 6))
		self.rewardScroll.SetScrollEvent(ui.__mem_func__(self.__RefreshRewardRows))
		self.rewardScroll.Show()
		self.widgets.append(self.rewardScroll)
		self.rewardWidgets.append(self.rewardScroll)
		self.__RefreshRewardRows()
		for widget in self.rewardWidgets:
			widget.Hide()
		self.activeWidgets = self.questWidgets

	def __RefreshRewardRows(self):
		maxOffset = max(0, len(self.rewardData) - 6)
		offset = int(round(self.rewardScroll.GetPos() * maxOffset)) if maxOffset else 0
		for rowIndex, (levelImage, slots, claimed) in enumerate(self.rewardRows):
			dataIndex = offset + rowIndex
			if dataIndex >= len(self.rewardData):
				levelImage.Hide()
				slots.Hide()
				claimed.Hide()
				continue
			level, items = self.rewardData[dataIndex]
			levelImage.LoadImage(BEGINNER_REWARD_IMAGE_DIR + "level_%d.png" % level)
			if self.currentTab == 1:
				levelImage.Show()
			else:
				levelImage.Hide()
			for slotIndex, (vnum, count) in enumerate(items):
				slots.SetItemSlot(slotIndex, vnum, count)
			slots.RefreshSlot()
			if self.currentTab == 1:
				slots.Show()
			else:
				slots.Hide()
			if self.currentTab == 1 and level in self.claimedLevels:
				claimed.Show()
			else:
				claimed.Hide()

	def SetClaimedLevels(self, levels):
		self.claimedLevels = set(int(level) for level in levels)
		if self.rewardRows:
			self.__RefreshRewardRows()

	def __SelectTab(self, tabIndex):
		if tabIndex == self.currentTab:
			return
		self.currentTab = tabIndex
		for widget in self.questWidgets:
			widget.Show() if tabIndex == 0 else widget.Hide()
		for widget in self.rewardWidgets:
			widget.Show() if tabIndex == 1 else widget.Hide()
		if tabIndex == 1:
			self.__RefreshRewardRows()
		for index, button in enumerate(self.tabButtons):
			name = ("quest", "reward", "dungeon")[index]
			button.SetUpVisual(BEGINNER_QUEST_IMAGE_DIR + "tab_%s_%s.png" % (name, "select" if index == tabIndex else "norm"))

	def OnRunMouseWheel(self, delta):
		if self.currentTab == 1:
			return self.rewardScroll.OnMouseWheel(delta)
		return False

	def __OnRewardWheel(self, down):
		if self.currentTab == 1:
			self.rewardScroll.OnMouseWheel(-1 if down else 1)

	def __BuildWindow(self):
		# Move the outer ScriptWindow so the board and every child stay together.
		self.AddFlag("movable")
		self.board = ui.BoardWithTitleBar()
		# Include the left tab strip in the parent's hit-test bounds.
		self.SetSize(388, 385)
		self.SetCenterPosition(-14, 0)
		self.board.SetParent(self)
		self.board.SetPosition(28, 0)
		self.board.SetSize(360, 385)
		self.board.SetTitleName(uiScriptLocale.BEGINNER_EVENT_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnRewardWheel))
		self.board.Show()

		self.tabButtons = (
			self.__AddTab("quest", 0, 39, 0),
			self.__AddTab("reward", 0, 78, 1),
			self.__AddTab("dungeon", 0, 117, 2),
		)
		self.tabButtons[0].SetUpVisual(BEGINNER_QUEST_IMAGE_DIR + "tab_quest_select.png")
		self.__AddImage(self.board, "bg_header.png", 8, 31)
		self.__AddImage(self.board, "bg_coins.png", 13, 38)
		self.__AddImage(self.board, BEGINNER_SHOP_IMAGE, 108, 36)
		self.__AddImage(self.board, "bg_time.png", 247, 36)
		self.__AddText(self.board, "0", 73, 47, centered=True)
		self.__AddText(self.board, "--:--:--", 307, 47, centered=True)

		self.__AddImage(self.board, "bg_quest.png", 8, 77)
		self.__AddText(self.board, uiScriptLocale.BEGINNER_EVENT_DAILY_QUEST, 180, 82, 0xfff0dcae, True)

		positions = ((10, 96), (120, 96), (230, 96),
			(10, 211), (120, 211), (230, 211))
		iconOffsets = ((38, 39), (38, 39), (36, 39),
			(38, 37), (38, 39), (38, 39))
		for index, (definition, position) in enumerate(zip(self.QUESTS, positions)):
			label, icon, goal = definition
			x, y = position
			self.__AddImage(self.board, "quest_bg.png", x, y)
			self.__AddText(self.board, label, x + 54, y + 7, 0xffe8d8b5, True)
			self.__AddText(self.board, "10", x + 12, y + 27, 0xfff2d887, True)
			iconX, iconY = iconOffsets[index]
			self.__AddImage(self.board, "icon/" + icon + ".png", x + iconX, y + iconY)
			self.__AddImage(self.board, "gauge_empty.png", x + 6, y + 88)
			self.__AddText(self.board, "0 / %d" % goal, x + 54, y + 89, 0xffe8e0d0, True)

		self.__AddImage(self.board, "bg_skin.png", 8, 326)
		self.__AddText(self.board, uiScriptLocale.BEGINNER_EVENT_REWARD_HINT, 180, 333, centered=True)
		self.__AddImage(self.board, "big_gauge_empty.png", 42, 350)
		self.__AddText(self.board, "0 / 6", 184, 352, 0xffe8e0d0, True)
		self.__BuildRewardPage()

	def Open(self):
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.widgets = []
		self.questWidgets = []
		self.rewardWidgets = []
		self.rewardRows = []
		self.tabButtons = None
		self.rewardScroll = None
		self.board = None
		self.ClearDictionary()
