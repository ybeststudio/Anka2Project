if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import item
import wndMgr
import renderTarget
import localeInfo
import uiToolTip
import uiCommon
import chat
import time

# Kostum Magazasi (reference Baerim uiCostumeShop.MainWindow, layout measured from the live client).
PATH = "d:/ymir work/ui/game/shop_costume/"
RENDER_PATH = "d:/ymir work/ui/game/render/"
OLD_PATH = "d:/ymir work/ui/game/costumeshop/"	# reset / remove buttons of the older Baerim costume shop (same sizes as the live client)

COUPON_VNUMS = (90020, 90021, 90022)	# Kostum / Binek / Evcil Hayvan Kuponu

# category id (server costume_shop.txt) -> button image name; order = Baerim button order
CATEGORY_LIST = (
	(1, "armor"),
	(2, "hair"),
	(3, "weapon"),
	(4, "wing"),
	(5, "mount"),
	(6, "pet"),
	(7, "shining"),
)

# preview slots: (slot index, x, y, width, height, key) - Baerim _costumeSlotLayout
SLOT_LAYOUT = (
	(199, 61, 72, 32, 64, "body"),
	(200, 61, 38, 32, 32, "hair"),
	(204, 17, 46, 32, 96, "weapon"),
	(2, 108, 94, 32, 32, "mount"),
	(6, 110, 135, 32, 32, "pet"),
	(215, 110, 54, 32, 32, "wings"),
	(216, 18, 148, 32, 32, "weapon_shining"),
	(217, 61, 148, 32, 32, "armor_shining"),
)
SLOT_KEYS = tuple(s[5] for s in SLOT_LAYOUT)
CATEGORY_SLOT_KEY = { 1 : "body", 2 : "hair", 3 : "weapon", 4 : "wings", 5 : "mount", 6 : "pet" }

GRID_X, GRID_Y = 43, 65
GRID_COLUMNS, GRID_ROWS = 7, 10
SLOT_SIZE = 32
EMOJI_WIDTH = 20
PART_WEAPON, PART_HAIR, PART_ACCE = 1, 4, 5	# CRaceData::EParts
ACCE_BASE_VNUM = 85000
SKILL_PREVIEW_INTERVAL = 0.5	# seconds between two syncs of the active skill buffs
SHINING_WEAPON, SHINING_ARMOR = 0, 1	# CInstanceBase::SetShining slots
PREVIEW_SCALE = 0.85

PRICE_OK_COLOR = 0xffcabdab
LIMITED_OFFER_COLOR = 0xffffc800	# measured from the Baerim tooltip (PNG)
LIMITED_PERIOD_COLOR = 0xffb0f2b6
TOOLTIP_WIDTH = 310
TEMP_DESC_COLOR = 0xffc1c1c1	# Baerim time icon tooltip (PNG)	# like the inventory tooltip with the shortcut table
PRICE_LACK_COLOR = 0xffff5050

GENDER_MALE, GENDER_FEMALE = 0, 1
RACE_COUNT = 4

def GetRealRace(gender, race):
	return ([0, 5, 2, 7] if gender == GENDER_MALE else [4, 1, 6, 3])[race]

RACE_ANTIFLAG = None
def GetRaceAntiFlags():
	global RACE_ANTIFLAG
	if RACE_ANTIFLAG is None:
		RACE_ANTIFLAG = (item.ITEM_ANTIFLAG_WARRIOR, item.ITEM_ANTIFLAG_ASSASSIN, item.ITEM_ANTIFLAG_SURA, item.ITEM_ANTIFLAG_SHAMAN)
	return RACE_ANTIFLAG


class CostumeShopEntry:
	def __init__(self, data):
		(self.id, self.category, self.vnum, c1, c2, c3, self.startTime, self.endTime) = data
		self.cost = (c1, c2, c3)
		item.SelectItem(self.vnum)
		self.name = item.GetItemName()
		self.lowerName = self.name.lower()
		(self.width, self.height) = item.GetItemSize()
		self.itemType = item.GetItemType()
		self.itemSubType = item.GetItemSubType()
		self.antiMale = item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE)
		self.antiFemale = item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE)
		self.antiRace = [item.IsAntiFlag(f) for f in GetRaceAntiFlags()]
		self.value0 = item.GetValue(0)
		self.mountVnum = 0
		for i in xrange(item.ITEM_APPLY_MAX_NUM):
			(applyType, applyValue) = item.GetAffect(i)
			if applyType == item.APPLY_MOUNT:
				self.mountVnum = applyValue

	def GetSlotKey(self):
		if self.category == 7:
			if self.itemSubType == item.SHINING_ARMOR:
				return "armor_shining"
			return "weapon_shining"
		return CATEGORY_SLOT_KEY.get(self.category, "body")

	def FitsGender(self, gender):
		if gender == GENDER_MALE and self.antiMale:
			return False
		if gender == GENDER_FEMALE and self.antiFemale:
			return False
		return True

	def FitsRace(self, race):
		return not self.antiRace[race]

	def IsTemporary(self):
		return self.endTime != 0


class BuySetQuestionDialog(ui.BoardWithTitleBar):
	# question with the list of item names (Baerim BuySetQuestionDialog)
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.AddFlag("float")
		self.AddFlag("movable")
		self.SetTitleName(localeInfo.COSTUME_SHOP_TITLE)
		self.SetCloseEvent(self.Close)
		self.acceptEvent = None
		self.lines = []

		self.mainText = ui.TextLine()
		self.mainText.SetParent(self)
		self.mainText.SetHorizontalAlignCenter()
		self.mainText.Show()

		self.acceptButton = ui.Button()
		self.acceptButton.SetParent(self)
		self.acceptButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.acceptButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.acceptButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.acceptButton.SetText(localeInfo.UI_ACCEPT)
		self.acceptButton.SetEvent(ui.__mem_func__(self.__OnAccept))
		self.acceptButton.Show()

		self.cancelButton = ui.Button()
		self.cancelButton.SetParent(self)
		self.cancelButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.cancelButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.cancelButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.cancelButton.SetText(localeInfo.UI_CANCEL)
		self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.cancelButton.Show()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.acceptEvent = None
		self.lines = []

	def SetAcceptEvent(self, event):
		self.acceptEvent = event

	def Open(self, mainText, itemNames):
		self.mainText.SetText(mainText)
		self.lines = []
		for name in itemNames:
			line = ui.TextLine()
			line.SetParent(self)
			line.SetHorizontalAlignCenter()
			line.SetPackedFontColor(PRICE_OK_COLOR)
			line.SetText(name)
			line.Show()
			self.lines.append(line)

		width = max(260, self.mainText.GetTextSize()[0] + 40)
		for line in self.lines:
			width = max(width, line.GetTextSize()[0] + 40)
		height = 40 + 18 + len(self.lines) * 16 + 45
		self.SetSize(width, height)

		self.mainText.SetPosition(width / 2, 38)
		for (i, line) in enumerate(self.lines):
			line.SetPosition(width / 2, 58 + i * 16)
		self.acceptButton.SetPosition(width / 2 - 66, height - 35)
		self.cancelButton.SetPosition(width / 2 + 5, height - 35)

		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def __OnAccept(self):
		event = self.acceptEvent
		self.Close()
		if event:
			event()

	def Close(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True


class CostumeRenderTarget(ui.RenderTarget):
	# character preview: right drag rotates, wheel zooms (same handling as uiRenderTarget)
	def __init__(self):
		ui.RenderTarget.__init__(self)
		self.isDrag = False
		self.lastPos = (0, 0)
		self.totalPos = [0, 0]
		self.renderIndex = -1

	def __del__(self):
		ui.RenderTarget.__del__(self)

	def SetIndex(self, renderIndex):
		self.renderIndex = renderIndex
		self.SetRenderTarget(renderIndex)
		self.SetMouseRightButtonDownEvent(ui.__mem_func__(self.RenderMouseRightDown))
		self.SetMouseRightButtonUpEvent(ui.__mem_func__(self.RenderMouseRightUp))

	def RenderMouseRightDown(self):
		app.SetCursor(app.CAMERA_ROTATE)
		self.isDrag = True
		self.lastPos = app.GetCursorPosition()
		return True

	def RenderMouseRightUp(self):
		app.SetCursor(app.NORMAL)
		self.isDrag = False
		return True

	def OnRunMouseWheel(self, nLen):
		if renderTarget.IsShow(self.renderIndex) != 1:
			return False
		renderTarget.Zoom(self.renderIndex, app.CAMERA_TO_NEGATIVE if nLen > 0 else app.CAMERA_TO_POSITIVE)
		return True

	def OnUpdate(self):
		if not self.isDrag:
			return
		if renderTarget.IsShow(self.renderIndex) != 1:
			return
		currentPos = app.GetCursorPosition()
		x = (currentPos[0] - self.lastPos[0]) + self.totalPos[0]
		y = (currentPos[1] - self.lastPos[1]) + self.totalPos[1]
		renderTarget.RotateEyeAroundTarget(self.renderIndex, y * 0.3, x * 0.3)
		self.totalPos = [x, y]
		self.lastPos = currentPos


class CostumeShopWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.entries = {}
		self.entryOrder = []
		self.serverTime = 0
		self.clientTimeAtOpen = 0
		self.category = 1
		self.gender = GENDER_MALE
		self.race = 0
		self.scrollOffset = 0
		self.shownEntries = []
		self.placedEntries = []
		self.slotEntry = {}
		self.totalRows = 0
		self.selected = dict((key, 0) for key in SLOT_KEYS)
		self.tempIcons = []
		self.renderIndex = -1
		self.petRenderIndex = -1
		self.isBuilt = False
		self.showSkills = False
		self.nextSkillTime = 0
		self.pendingSingle = 0
		self.tooltipItem = None
		self.tooltipText = None
		self.buyQuestionDlg = None
		self.singleQuestionDlg = None
		self.interface = None

	def __del__(self):
		ui.Window.__del__(self)

	def SetInterface(self, interface):
		self.interface = interface

	def Destroy(self):
		self.Hide()
		self.interface = None
		if self.renderIndex >= 0:
			renderTarget.SetVisibility(self.renderIndex, False)
		if self.petRenderIndex >= 0:
			renderTarget.SetVisibility(self.petRenderIndex, False)
		if self.buyQuestionDlg:
			self.buyQuestionDlg.Destroy()
			self.buyQuestionDlg = None
		if self.singleQuestionDlg:
			self.singleQuestionDlg.Close()
			self.singleQuestionDlg = None
		self.tooltipItem = None
		self.tooltipText = None
		self.entries = {}
		self.entryOrder = []
		self.tempIcons = []
		self.isBuilt = False

	# --- build ---------------------------------------------------------------------------------
	def __MakeImage(self, parent, x, y, path, cls = ui.ImageBox):
		image = cls()
		image.SetParent(parent)
		image.LoadImage(path)
		image.SetPosition(x, y)
		image.Show()
		return image

	def __MakeButton(self, parent, x, y, normal, hover, down, cls = ui.Button):
		button = cls()
		button.SetParent(parent)
		button.SetUpVisual(normal)
		button.SetOverVisual(hover)
		button.SetDownVisual(down)
		button.SetPosition(x, y)
		button.Show()
		return button

	def __MakeText(self, parent, x, y, text, center = False):
		line = ui.TextLine()
		line.SetParent(parent)
		line.SetPosition(x, y)
		if center:
			line.SetHorizontalAlignCenter()
		line.SetText(text)
		line.Show()
		return line

	def BuildWindow(self):
		if self.isBuilt:
			return
		self.isBuilt = True

		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetSize(705, 397)
		self.SetCenterPosition()

		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.SetSize(675, 397)
		self.board.SetPosition(30, 0)
		self.board.SetTitleName(localeInfo.COSTUME_SHOP_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.Show()

		# category buttons (left of the board)
		self.categoryTab = []
		for (i, (category, name)) in enumerate(CATEGORY_LIST):
			button = self.__MakeButton(self, 0, 30 + 39 * i,
				PATH + "category/btn_%s_norm.png" % name, PATH + "category/btn_%s_hover.png" % name, PATH + "category/btn_%s_select.png" % name, ui.RadioButton)
			button.SAFE_SetEvent(self.SetCategoryShop, category)
			self.categoryTab.append(button)

		# search line + gender filter
		self.searchBackground = self.__MakeImage(self, 37, 29, PATH + "bg_search.png", ui.ExpandedImageBox)
		self.searchBar = self.__MakeImage(self, 42, 34, PATH + "text_search.png")
		self.itemSearch = ui.EditLine()
		self.itemSearch.SetParent(self)
		self.itemSearch.SetPosition(47, 39)
		self.itemSearch.SetSize(132, 16)
		self.itemSearch.SetMax(24)
		self.itemSearch.SetInfoMessage(localeInfo.COSTUME_SHOP_SEARCH)
		self.itemSearch.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.itemSearch.OnIMEUpdate = ui.__mem_func__(self.__OnSearchIMEUpdate)
		self.itemSearch.Show()
		self.searchButton = self.__MakeButton(self, 184, 34, PATH + "btn_search_norm.png", PATH + "btn_search_hover.png", PATH + "btn_search_select.png")
		self.searchButton.SAFE_SetEvent(self.__StartSearchItem)

		self.genderButtons = []
		for gender in (GENDER_MALE, GENDER_FEMALE):
			button = self.__MakeButton(self, 216 + 26 * gender, 34,
				PATH + "gender/%d_0.png" % gender, PATH + "gender/%d_1.png" % gender, PATH + "gender/%d_2.png" % gender, ui.RadioButton)
			button.SAFE_SetEvent(self.__SelectGender, gender)
			self.genderButtons.append(button)

		# item list
		self.slotBackground = self.__MakeImage(self, 37, 60, PATH + "bg_slot.png", ui.ExpandedImageBox)
		self.itemGrid = ui.GridSlotWindow()
		self.itemGrid.SetParent(self)
		self.itemGrid.SetPosition(GRID_X, GRID_Y)
		self.itemGrid.ArrangeSlot(0, GRID_COLUMNS, GRID_ROWS, SLOT_SIZE, SLOT_SIZE, 0, 0)
		self.itemGrid.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		self.itemGrid.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnClickItemSlot))
		self.itemGrid.SetUnselectItemSlotEvent(ui.__mem_func__(self.__OnRightClickItemSlot))
		self.itemGrid.SetUseSlotEvent(ui.__mem_func__(self.__OnRightClickItemSlot))
		self.itemGrid.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
		self.itemGrid.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
		self.itemGrid.Show()

		for i in xrange(GRID_COLUMNS * GRID_ROWS):
			icon = ui.ExpandedImageBox()
			icon.SetParent(self)
			icon.LoadImage("d:/ymir work/ui/game/common/time_small.png")
			icon.SetPosition(GRID_X + (i % GRID_COLUMNS) * SLOT_SIZE + 3, GRID_Y + (i / GRID_COLUMNS) * SLOT_SIZE + 3)
			icon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OnTempTimeOverIn, i)
			icon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OnTempTimeOverOut)
			icon.Hide()
			self.tempIcons.append(icon)

		# preview
		self.previewWindow = self.__MakeImage(self, 274, 31, PATH + "bg_rendertarget.png")
		self.renderIndex = renderTarget.GetFreeIndex(50, 85)
		self.characterRenderTarget = CostumeRenderTarget()
		self.characterRenderTarget.SetParent(self)
		self.characterRenderTarget.SetSize(265, 356)
		self.characterRenderTarget.SetPosition(274, 31)
		self.characterRenderTarget.SetIndex(self.renderIndex)
		self.characterRenderTarget.Show()
		renderTarget.SetBackground(self.renderIndex, PATH + "bg_rendertarget.png")
		renderTarget.SetRotation(self.renderIndex, False)

		self.petRenderIndex = renderTarget.GetFreeIndex(50, 85)
		self.companionRenderTarget = CostumeRenderTarget()
		self.companionRenderTarget.SetParent(self)
		self.companionRenderTarget.SetSize(75, 75)
		self.companionRenderTarget.SetPosition(276, 311)
		self.companionRenderTarget.SetIndex(self.petRenderIndex)
		self.companionRenderTarget.Hide()
		renderTarget.SetTransparent(self.petRenderIndex)	# see-through: the preview background stays visible behind the pet
		renderTarget.SetRotation(self.petRenderIndex, True)

		# right aligned at x 529 like Baerim; the emoji (|E...|e, ~20 px) is not part of the measured text width
		self.previewSkillsCheckBox = ui.CheckBox()
		self.previewSkillsCheckBox.SetParent(self)
		self.previewSkillsCheckBox.SetPosition(282, 41)
		self.previewSkillsCheckBox.SetTextInfo(localeInfo.COSTUME_SHOP_SHOW_SKILLS)
		self.previewSkillsCheckBox.SetEvent(ui.__mem_func__(self.__OnCheckShowSkills), "ON_CHECK")
		self.previewSkillsCheckBox.SetEvent(ui.__mem_func__(self.__OnUncheckShowSkills), "ON_UNCKECK")
		self.previewSkillsCheckBox.Show()

		self.zoomText = self.__MakeText(self, 529 - EMOJI_WIDTH, 41, localeInfo.COSTUME_SHOP_ZOOM)
		self.zoomText.SetHorizontalAlignRight()
		self.moveText = self.__MakeText(self, 529 - EMOJI_WIDTH, 71, localeInfo.COSTUME_SHOP_MOVE)
		self.moveText.SetHorizontalAlignRight()

		self.raceButtons = []
		for race in xrange(RACE_COUNT):
			button = self.__MakeButton(self, 359 + 34 * race, 346,
				RENDER_PATH + "face/race_%d_0.png" % race, RENDER_PATH + "face/race_%d_1.png" % race, RENDER_PATH + "face/race_%d_1.png" % race, ui.RadioButton)
			button.SetDisableVisual(RENDER_PATH + "face/race_%d_2.png" % race)
			button.SAFE_SetEvent(self.__SelectRace, race)
			self.raceButtons.append(button)

		self.resetButton = self.__MakeButton(self, 500, 351, OLD_PATH + "back_button_0.tga", OLD_PATH + "back_button_1.tga", OLD_PATH + "back_button_2.tga")
		self.resetButton.SetToolTipText(localeInfo.COSTUME_SHOP_RESET)
		self.resetButton.SAFE_SetEvent(self.__OnResetEquipment)

		# preview slots
		self.costumeBackground = self.__MakeImage(self, 541, 31, PATH + "costume_bg.png")
		self.costumeSlot = ui.SlotWindow()
		self.costumeSlot.SetParent(self)
		self.costumeSlot.SetPosition(541, 31)
		self.costumeSlot.SetSize(155, 187)
		for (index, x, y, w, h, key) in SLOT_LAYOUT:
			self.costumeSlot.AppendSlot(index, x, y, w, h)
		self.costumeSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnEquipSlotOverIn))
		self.costumeSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
		self.costumeSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__OnEquipSlotRightClick))
		self.costumeSlot.SetUseSlotEvent(ui.__mem_func__(self.__OnEquipSlotRightClick))
		self.costumeSlot.Show()

		self.clearSlotButtons = {}
		for (index, x, y, w, h, key) in SLOT_LAYOUT:
			button = self.__MakeButton(self, 541 + x + w - 9, 31 + y - 1, OLD_PATH + "delete_btn_0.tga", OLD_PATH + "delete_btn_1.tga", OLD_PATH + "delete_btn_2.tga")
			button.SetToolTipText(localeInfo.COSTUME_SHOP_REMOVE)
			button.SAFE_SetEvent(self.__OnClearCostumeSlot, key)
			button.Hide()
			self.clearSlotButtons[key] = button

		# buy box
		self.titleBackground = self.__MakeImage(self, 539, 218, PATH + "title_buy.png")
		self.titleText = self.__MakeText(self, 618, 226, localeInfo.COSTUME_SHOP_BUY_ALL, True)
		self.buyBackground = self.__MakeImage(self, 539, 245, PATH + "bg_buy.png")
		self.descText = self.__MakeText(self, 618, 257, localeInfo.COSTUME_SHOP_TOTAL_PRICE, True)

		# coupon slots: one grid, slot index = coupon index (37 px step like Baerim)
		self.buyCostSlot = ui.GridSlotWindow()
		self.buyCostSlot.SetParent(self)
		self.buyCostSlot.SetPosition(563, 283)
		self.buyCostSlot.ArrangeSlot(0, len(COUPON_VNUMS), 1, SLOT_SIZE, SLOT_SIZE, 5, 0)
		for i in xrange(len(COUPON_VNUMS)):
			self.buyCostSlot.SetItemSlot(i, COUPON_VNUMS[i], 0)
		self.buyCostSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnBuyCostSlotOverIn))
		self.buyCostSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
		self.buyCostSlot.RefreshSlot()
		self.buyCostSlot.Show()
		self.buySlotRow = []
		for i in xrange(len(COUPON_VNUMS)):
			image = self.__MakeImage(self, 562 + 37 * i, 325, PATH + "price.png")
			text = self.__MakeText(self, 580 + 37 * i, 327, "0", True)
			self.buySlotRow.append({ "image" : image, "text" : text })

		self.buySetButton = self.__MakeButton(self, 563, 355, PATH + "btn_buy_norm.png", PATH + "btn_buy_hover.png", PATH + "btn_buy_select.png")
		self.buySetButton.SetText(localeInfo.COSTUME_SHOP_BUY)
		self.buySetButton.SAFE_SetEvent(self.__OnClickBuySet)

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.tooltipText = uiToolTip.ToolTip()
		self.tooltipText.Hide()

		self.buyQuestionDlg = BuySetQuestionDialog()
		self.buyQuestionDlg.Hide()

		self.__SelectGender(GENDER_FEMALE if chr.RaceToSex(player.GetRace()) == 0 else GENDER_MALE, False)
		self.__SelectRace(chr.RaceToJob(player.GetRace()), False)

	# --- protocol ------------------------------------------------------------------------------
	def ClearEntries(self):
		self.entries = {}
		self.entryOrder = []

	def AddEntries(self, data):
		for part in data.split(";"):
			values = part.split(",")
			if len(values) != 8:
				continue
			try:
				entry = CostumeShopEntry(tuple(int(v) for v in values))
			except ValueError:
				continue
			self.entries[entry.id] = entry
			self.entryOrder.append(entry.id)

	def Open(self, serverTime):
		self.BuildWindow()
		self.serverTime = serverTime
		self.clientTimeAtOpen = app.GetGlobalTimeStamp()

		for key in SLOT_KEYS:
			if self.selected[key] and self.selected[key] not in self.entries:
				self.selected[key] = 0

		self.SetCategoryShop(self.category)
		self.__RefreshCostumeSlots()
		self.__UpdatePreviewModel()
		self.Show()
		self.SetTop()

	def OnResult(self, result):
		text = getattr(localeInfo, "COSTUME_SHOP_RESULT_%d" % result, "")
		if text:
			chat.AppendChat(chat.CHAT_TYPE_INFO, text)
		if result == 0:
			self.__RecomputeTotals()

	def GetServerNow(self):
		now = app.GetGlobalTimeStamp()
		return self.serverTime + max(0, now - self.clientTimeAtOpen)

	# --- list ----------------------------------------------------------------------------------
	def SetCategoryShop(self, category):
		self.category = category
		for (i, (cat, name)) in enumerate(CATEGORY_LIST):
			if cat == category:
				self.categoryTab[i].Down()
			else:
				self.categoryTab[i].SetUp()
		self.scrollOffset = 0
		self.__ApplyFilter()

	def __SearchText(self):
		return self.itemSearch.GetText().strip().lower()

	def __OnSearchIMEUpdate(self):
		ui.EditLine.OnIMEUpdate(self.itemSearch)
		self.scrollOffset = 0
		self.__ApplyFilter()

	def __StartSearchItem(self):
		self.scrollOffset = 0
		self.__ApplyFilter()

	def __CanEntryFitFilter(self, entry):
		if entry.category in (1, 2, 3, 4, 7):
			if not entry.FitsGender(self.gender):
				return False
		if entry.category in (1, 2, 3):
			if not entry.FitsRace(self.race):
				return False
		return True

	def __ApplyFilter(self):
		search = self.__SearchText()
		now = self.GetServerNow()
		shown = []
		for entryId in self.entryOrder:
			entry = self.entries[entryId]
			if search:
				if entry.lowerName.find(search) < 0:
					continue
			elif entry.category != self.category:
				continue
			if entry.startTime and now < entry.startTime:
				continue
			if entry.endTime and now > entry.endTime:
				continue
			if not self.__CanEntryFitFilter(entry):
				continue
			shown.append(entry)
		# temporary entries first (Baerim _GetShopDisplaySortKey), then shop order
		shown.sort(key = lambda e : (0 if e.IsTemporary() else 1, self.entryOrder.index(e.id)))
		self.shownEntries = shown
		self.__LayoutEntries()
		self.scrollOffset = min(self.scrollOffset, self.__MaxScrollOffset())
		self.__RefreshSlotItem()

	def __LayoutEntries(self):
		# every item takes its own height (costumes 2 cells, weapons up to 3) like an inventory grid:
		# first free cell in row order where the whole column span is free
		occupied = set()
		self.placedEntries = []
		self.totalRows = 0
		for entry in self.shownEntries:
			height = max(1, entry.height)
			cell = 0
			while True:
				(row, column) = (cell / GRID_COLUMNS, cell % GRID_COLUMNS)
				if not [k for k in xrange(height) if (row + k, column) in occupied]:
					break
				cell += 1
			for k in xrange(height):
				occupied.add((row + k, column))
			self.placedEntries.append((entry, row, column, height))
			self.totalRows = max(self.totalRows, row + height)

	def __MaxScrollOffset(self):
		return max(0, self.totalRows - GRID_ROWS)

	def __RefreshSlotItem(self):
		self.slotEntry = {}
		for i in xrange(GRID_COLUMNS * GRID_ROWS):
			self.itemGrid.ClearSlot(i)
			self.itemGrid.DeactivateSlot(i)
			self.tempIcons[i].Hide()

		for (entry, row, column, height) in self.placedEntries:
			row -= self.scrollOffset
			if row < 0 or row + height > GRID_ROWS:
				continue	# only whole items are shown
			slotIndex = row * GRID_COLUMNS + column
			self.slotEntry[slotIndex] = entry
			self.itemGrid.SetItemSlot(slotIndex, entry.vnum, 0)
			if self.selected.get(entry.GetSlotKey()) == entry.id:
				self.itemGrid.ActivateSlot(slotIndex, 0)
			if entry.IsTemporary():
				self.tempIcons[slotIndex].Show()
		self.itemGrid.RefreshSlot()

	def __GetEntryBySlot(self, slotIndex):
		return self.slotEntry.get(slotIndex)

	def OnRunMouseWheel(self, nLen):
		(x, y) = self.GetMouseLocalPosition()
		if GRID_X <= x < GRID_X + GRID_COLUMNS * SLOT_SIZE and GRID_Y <= y < GRID_Y + GRID_ROWS * SLOT_SIZE:
			if nLen > 0:
				self.scrollOffset = max(0, self.scrollOffset - 1)
			else:
				self.scrollOffset = min(self.__MaxScrollOffset(), self.scrollOffset + 1)
			self.__RefreshSlotItem()
			return True
		return False

	# --- selection / preview -------------------------------------------------------------------
	def __IsCtrl(self):
		return app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)

	def __IsAlt(self):
		return app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT)

	def __OnClickItemSlot(self, slotIndex):
		entry = self.__GetEntryBySlot(slotIndex)
		if not entry:
			return
		# Ctrl + Alt + left click: wiki (same shortcut as the inventory)
		if getattr(app, "ENABLE_WIKI_SYSTEM", 0) and self.interface and self.__IsCtrl() and self.__IsAlt():
			self.interface.SearchWikiForItem(entry.vnum)
			return
		key = entry.GetSlotKey()
		if self.selected[key] == entry.id:
			self.selected[key] = 0
		else:
			self.selected[key] = entry.id
		self.__RefreshSlotItem()
		self.__RefreshCostumeSlots()
		self.__UpdatePreviewModel()

	def __OnRightClickItemSlot(self, slotIndex):
		entry = self.__GetEntryBySlot(slotIndex)
		if not entry:
			return
		# Ctrl + right click: search the item in the player shops (same shortcut as the inventory)
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0) and self.interface and self.__IsCtrl() and not self.__IsAlt():
			self.interface.SearchShopForItem(entry.vnum)
			return
		self.pendingSingle = entry.id
		dlg = uiCommon.QuestionDialog()
		dlg.SetText(localeInfo.COSTUME_SHOP_BUY_QUESTION % entry.name)
		dlg.SetAcceptEvent(ui.__mem_func__(self.__OnConfirmBuySingle))
		dlg.SetCancelEvent(ui.__mem_func__(self.__OnCancelBuySingle))
		dlg.Open()
		self.singleQuestionDlg = dlg

	def __OnConfirmBuySingle(self):
		if self.pendingSingle:
			net.SendChatPacket("/costume_shop _buy %d" % self.pendingSingle)
		self.__OnCancelBuySingle()

	def __OnCancelBuySingle(self):
		self.pendingSingle = 0
		if self.singleQuestionDlg:
			self.singleQuestionDlg.Close()
			self.singleQuestionDlg = None

	def __OnClearCostumeSlot(self, key):
		self.selected[key] = 0
		self.__RefreshSlotItem()
		self.__RefreshCostumeSlots()
		self.__UpdatePreviewModel()

	def __OnResetEquipment(self):
		for key in SLOT_KEYS:
			self.selected[key] = 0
		self.__RefreshSlotItem()
		self.__RefreshCostumeSlots()
		self.__UpdatePreviewModel()

	def __OnEquipSlotRightClick(self, slotIndex):
		for (index, x, y, w, h, key) in SLOT_LAYOUT:
			if index == slotIndex:
				self.__OnClearCostumeSlot(key)
				return

	def __SlotIndexToKey(self, slotIndex):
		for (index, x, y, w, h, key) in SLOT_LAYOUT:
			if index == slotIndex:
				return key
		return None

	def __RefreshCostumeSlots(self):
		for (index, x, y, w, h, key) in SLOT_LAYOUT:
			entryId = self.selected[key]
			if entryId and entryId in self.entries:
				self.costumeSlot.SetItemSlot(index, self.entries[entryId].vnum, 0)
				self.clearSlotButtons[key].Show()
			else:
				self.costumeSlot.ClearSlot(index)
				self.clearSlotButtons[key].Hide()
		self.costumeSlot.RefreshSlot()
		self.__RecomputeTotals()

	def __SelectedEntry(self, key):
		entryId = self.selected.get(key, 0)
		return self.entries.get(entryId) if entryId else None

	def __UpdatePreviewModel(self):
		if self.renderIndex < 0:
			return
		# rebuild the model every time: MountHorse(0) would attach race 0 as a "mount" and a mount can not be removed
		race = GetRealRace(self.gender, self.race)
		renderTarget.ResetModel(self.renderIndex)
		renderTarget.SelectModel(self.renderIndex, race)
		renderTarget.SetScale(self.renderIndex, PREVIEW_SCALE)
		renderTarget.SetVisibility(self.renderIndex, True)

		body = self.__SelectedEntry("body")
		hair = self.__SelectedEntry("hair")
		weapon = self.__SelectedEntry("weapon")
		mount = self.__SelectedEntry("mount")

		# slots without a selection show what the character wears now (only on its own race / gender)
		# same order as uiRenderTarget: weapon, hair, sash, armour last (ChangeArmor rebuilds the body with the other parts)
		isOwnRace = (race == player.GetRace())
		renderTarget.SetWeapon(self.renderIndex, weapon.vnum if weapon else (player.GetMainCharacterPart(PART_WEAPON) if isOwnRace else 0))
		if hair:
			renderTarget.SetHair(self.renderIndex, hair.vnum, False)
		else:
			renderTarget.SetHair(self.renderIndex, player.GetMainCharacterPart(PART_HAIR) if isOwnRace else 0, True)
		renderTarget.SetArmor(self.renderIndex, body.vnum if body else (self.__GetWornArmor() if isOwnRace else 0))
		# sash after the armour: ChangeArmor re-applies the sash from the part data (vnum) and would lose it.
		# The selected wing / sash skin, else the worn sash (SetAcce takes vnum - ACCE_BASE_VNUM, the part data holds the vnum)
		wings = self.__SelectedEntry("wings")
		if wings:
			renderTarget.SetAcce(self.renderIndex, wings.vnum - ACCE_BASE_VNUM)
		elif isOwnRace:
			acce = player.GetMainCharacterPart(PART_ACCE)
			renderTarget.SetAcce(self.renderIndex, acce - ACCE_BASE_VNUM if acce >= ACCE_BASE_VNUM else acce)

		# shining glows after the armour (ChangeArmor rebuilds the model): selected one, else the worn one
		weaponShining = self.__SelectedEntry("weapon_shining")
		armorShining = self.__SelectedEntry("armor_shining")
		renderTarget.SetShining(self.renderIndex, SHINING_WEAPON, weaponShining.vnum if weaponShining else (player.GetItemIndex(item.SHINING_SLOT_WEAPON) if isOwnRace else 0))
		renderTarget.SetShining(self.renderIndex, SHINING_ARMOR, armorShining.vnum if armorShining else (player.GetItemIndex(item.SHINING_SLOT_ARMOR) if isOwnRace else 0))
		if self.showSkills and isOwnRace:
			renderTarget.CopyMainAffects(self.renderIndex)
		if mount and mount.mountVnum:
			renderTarget.SetMount(self.renderIndex, mount.mountVnum)

		pet = self.__SelectedEntry("pet")
		if pet and pet.value0:
			renderTarget.ResetModel(self.petRenderIndex)
			renderTarget.SelectModel(self.petRenderIndex, pet.value0)
			renderTarget.SetVisibility(self.petRenderIndex, True)
			self.companionRenderTarget.Show()
		else:
			renderTarget.SetVisibility(self.petRenderIndex, False)
			self.companionRenderTarget.Hide()

	def __GetWornArmor(self):
		# PART_MAIN is never filled on the instance, so read the wear slots: costume body first, then the armour
		costume = player.GetItemIndex(item.COSTUME_SLOT_BODY)
		if costume:
			return costume
		return player.GetItemIndex(item.EQUIPMENT_BODY)

	# --- "Becerilerimi goster": the skill buffs that are active on the character right now are shown on the preview
	def __OnCheckShowSkills(self):
		self.showSkills = True
		self.nextSkillTime = 0

	def __OnUncheckShowSkills(self):
		self.showSkills = False
		self.__UpdatePreviewModel()

	def OnUpdate(self):
		if not self.showSkills or self.renderIndex < 0:
			return
		now = app.GetTime()
		if now < self.nextSkillTime:
			return
		self.nextSkillTime = now + SKILL_PREVIEW_INTERVAL
		if GetRealRace(self.gender, self.race) == player.GetRace():
			renderTarget.CopyMainAffects(self.renderIndex)

	def __SelectGender(self, gender, refresh = True):
		self.gender = gender
		for (i, button) in enumerate(self.genderButtons):
			if i == gender:
				button.Down()
			else:
				button.SetUp()
		if refresh:
			self.__DropUnfitSelections()
			self.scrollOffset = 0
			self.__ApplyFilter()
			self.__RefreshCostumeSlots()
			self.__UpdatePreviewModel()

	def __SelectRace(self, race, refresh = True):
		self.race = race
		for (i, button) in enumerate(self.raceButtons):
			if i == race:
				button.Down()
			else:
				button.SetUp()
		if refresh:
			self.__DropUnfitSelections()
			self.scrollOffset = 0
			self.__ApplyFilter()
			self.__RefreshCostumeSlots()
			self.__UpdatePreviewModel()

	def __DropUnfitSelections(self):
		for key in SLOT_KEYS:
			entry = self.__SelectedEntry(key)
			if entry and not self.__CanEntryFitFilter(entry):
				self.selected[key] = 0

	# --- price / buy ---------------------------------------------------------------------------
	def __SelectedIds(self):
		return [self.selected[key] for key in SLOT_KEYS if self.selected[key] and self.selected[key] in self.entries]

	def __RecomputeTotals(self):
		totals = [0] * len(COUPON_VNUMS)
		for entryId in self.__SelectedIds():
			entry = self.entries[entryId]
			for i in xrange(len(COUPON_VNUMS)):
				totals[i] += entry.cost[i]
		for i in xrange(len(COUPON_VNUMS)):
			text = self.buySlotRow[i]["text"]
			text.SetText(str(totals[i]))
			if totals[i] > player.GetItemCountByVnum(COUPON_VNUMS[i]):
				text.SetPackedFontColor(PRICE_LACK_COLOR)
			else:
				text.SetPackedFontColor(PRICE_OK_COLOR)
		if self.__SelectedIds():
			self.buySetButton.Enable()
		else:
			self.buySetButton.Disable()

	def __OnClickBuySet(self):
		ids = self.__SelectedIds()
		if not ids:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.COSTUME_SHOP_NOTHING_SELECTED)
			return
		names = [self.entries[entryId].name for entryId in ids]
		self.buyQuestionDlg.SetAcceptEvent(ui.__mem_func__(self.__OnAcceptBuySet))
		self.buyQuestionDlg.Open(localeInfo.COSTUME_SHOP_BUY_SET_QUESTION, names)

	def __OnAcceptBuySet(self):
		ids = self.__SelectedIds()
		if ids:
			net.SendChatPacket("/costume_shop _buy %s" % ",".join(str(i) for i in ids))

	# --- tooltips ------------------------------------------------------------------------------
	def __OverInItem(self, slotIndex):
		entry = self.__GetEntryBySlot(slotIndex)
		if not entry or not self.tooltipItem:
			return
		# Baerim order: name, empty bonus lines, wearable, offer period, model preview, shortcuts
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.deferModelPreview = True
		self.tooltipItem.showEmptyCostumeBonus = True
		self.tooltipItem.SetItemToolTip(entry.vnum, TOOLTIP_WIDTH)
		self.tooltipItem.showEmptyCostumeBonus = False
		if entry.IsTemporary():
			self.__AppendTempPeriod(self.tooltipItem, entry)
		if hasattr(self.tooltipItem, "AppendPendingModelPreview"):
			self.tooltipItem.AppendPendingModelPreview()
		self.tooltipItem.deferModelPreview = False
		self.__AppendShortcuts(self.tooltipItem)
		self.tooltipItem.ShowToolTip()

	def __OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		if self.tooltipText:
			self.tooltipText.HideToolTip()

	def __OnEquipSlotOverIn(self, slotIndex):
		key = self.__SlotIndexToKey(slotIndex)
		entry = self.__SelectedEntry(key) if key else None
		if not entry or not self.tooltipItem:
			return
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.SetItemToolTip(entry.vnum)
		self.tooltipItem.ShowToolTip()

	def __OnBuyCostSlotOverIn(self, couponIndex):
		if not self.tooltipItem:
			return
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.SetItemToolTip(COUPON_VNUMS[couponIndex])
		self.tooltipItem.ShowToolTip()

	def __FormatTimestamp(self, timestamp):
		return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(timestamp))

	def __AppendTempPeriod(self, tooltip, entry):
		# Baerim: "Sinirli sureli teklif" + "<start> - <end> tarihleri arasinda gecerli"
		tooltip.AppendSpace(5)
		tooltip.AppendTextLine(localeInfo.COSTUME_SHOP_LIMITED_OFFER, LIMITED_OFFER_COLOR)
		tooltip.AppendTextLine(localeInfo.COSTUME_SHOP_TEMP_PERIOD % (self.__FormatTimestamp(entry.startTime) if entry.startTime else "-", self.__FormatTimestamp(entry.endTime)), LIMITED_PERIOD_COLOR)

	def __AppendShortcuts(self, tooltip):
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0):
			rows.append((("ctrl", "mouse_right"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		if getattr(app, "ENABLE_WIKI_SYSTEM", 0):
			rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
		if rows and hasattr(tooltip, "AppendShortcutTable"):
			tooltip.AppendShortcutTable(rows)

	def __OnTempTimeOverIn(self, slotIndex):
		# Baerim time icon tooltip: description, empty line, "Available from <start> to <end>"
		entry = self.__GetEntryBySlot(slotIndex)
		if not entry or not entry.IsTemporary() or not self.tooltipText:
			return
		self.tooltipText.ClearToolTip()
		self.tooltipText.toolTipWidth = 0	# AutoAppendTextLine widens it to the longest line
		self.tooltipText.AutoAppendTextLine(localeInfo.COSTUME_SHOP_TEMP_DESC, TEMP_DESC_COLOR)
		self.tooltipText.AppendSpace(12)
		self.tooltipText.AutoAppendTextLine(localeInfo.COSTUME_SHOP_TEMP_AVAILABLE % (self.__FormatTimestamp(entry.startTime) if entry.startTime else "-", self.__FormatTimestamp(entry.endTime)), LIMITED_PERIOD_COLOR)
		self.tooltipText.AlignHorizonalCenter()
		self.tooltipText.ShowToolTip()

	def __OnTempTimeOverOut(self):
		if self.tooltipText:
			self.tooltipText.HideToolTip()

	# --- window --------------------------------------------------------------------------------
	def Close(self):
		self.__OverOutItem()
		if self.renderIndex >= 0:
			renderTarget.SetVisibility(self.renderIndex, False)
		if self.petRenderIndex >= 0:
			renderTarget.SetVisibility(self.petRenderIndex, False)
		if self.buyQuestionDlg:
			self.buyQuestionDlg.Close()
		self.__OnCancelBuySingle()
		if self.isBuilt:
			self.itemSearch.KillFocus()
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True
