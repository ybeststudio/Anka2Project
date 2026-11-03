import app
import net
import player
import item
import chat
import ui
import uiToolTip
import localeInfo
import wndMgr

# Brave Cape window (reference Baerim uiAutoCape.BraveCapeWindow). The server pulls the monsters around every
# <time> seconds inside <range> while a Cape of Courage is in the inventory:
# "/brave_cape active <time> <range>" | "/brave_cape deactive", answered with "SetBraveCapeStatus 0|1".
AUTO_CAPE_AFTER_WARP_OPTION = "auto_cape_after_warp"
CAPE_PATH = "d:/ymir work/ui/game/bravery_cape{}"
INVENTORY_PATH = "d:/ymir work/ui/game/inventory/{}"
import userData
SAVE_FILE = userData.GetPath("brave_cape.cfg")

CAPE_VNUMS = (70038, 70057, 76007)
CAPE_COST = 1000	# yang per pull (server BRAVE_CAPE_COST)

TIME_MIN, TIME_MAX, TIME_DEFAULT = 2, 59, 2
RANGE_MIN, RANGE_MAX, RANGE_DEFAULT = 1000, 5000, 1000
CAPE_ICON = "icon/item/mantie.png"

BG_X, BG_Y = 8, 30
WIDTH = 133 + BG_X * 2
HEIGHT = BG_Y + 154 + 8




def IsAutoCapeAfterWarpEnabled():
	try:
		import advanced_options_runtime
		return bool(advanced_options_runtime.Get(AUTO_CAPE_AFTER_WARP_OPTION, False))
	except Exception:
		return False


class SliderBarNew(ui.Window):
	"""input_slider.png bar with the drag.png cursor (reference uiAutoCape.SliderBarNew)."""

	def __init__(self):
		ui.Window.__init__(self)
		self.curPos = 0.0
		self.eventChange = None

		bar = ui.ImageBox()
		bar.SetParent(self)
		bar.LoadImage(CAPE_PATH.format("/input_slider.png"))
		bar.SetPosition(0, 1)
		bar.AddFlag("not_pick")
		bar.Show()
		self.bar = bar

		cursor = ui.DragButton()
		cursor.AddFlag("movable")
		cursor.AddFlag("restrict_y")
		cursor.SetParent(self)
		cursor.SetUpVisual(CAPE_PATH.format("/drag.png"))
		cursor.SetOverVisual(CAPE_PATH.format("/drag.png"))
		cursor.SetDownVisual(CAPE_PATH.format("/drag.png"))
		cursor.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		cursor.Show()
		self.cursor = cursor

		self.SetSize(bar.GetWidth(), cursor.GetHeight())
		self.pageSize = bar.GetWidth() - cursor.GetWidth()
		cursor.SetRestrictMovementArea(0, 0, bar.GetWidth(), 0)

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.eventChange = None
		self.bar = None
		self.cursor = None

	def __OnMove(self):
		(x, y) = self.cursor.GetLocalPosition()
		self.curPos = max(0.0, min(1.0, float(x) / float(max(1, self.pageSize))))
		if self.eventChange:
			self.eventChange()

	def SetSliderPos(self, pos):
		self.curPos = max(0.0, min(1.0, pos))
		self.cursor.SetPosition(int(self.pageSize * self.curPos), 0)

	def GetSliderPos(self):
		return self.curPos

	def SetEvent(self, event):
		self.eventChange = event


class BraveCapeWindow(ui.BoardWithTitleBar):

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.interface = None
		self.keep = []
		self.__children = {}
		self.isActive = False
		self.capeTime = TIME_DEFAULT
		self.capeRange = RANGE_DEFAULT
		self.wasActive = False
		self.__warpSavePending = False
		self.toolTipItem = uiToolTip.ItemToolTip()
		self.toolTipItem.HideToolTip()
		self.CreateWindow()
		self.LoadData()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.SaveData()
		self.Hide()
		for name in ("timeSlider", "rangeSlider"):
			child = self.__children.get(name)
			if child:
				child.Destroy()
		self.__children = {}
		self.keep = []
		self.interface = None
		self.toolTipItem = None

	def BindInterface(self, interface):
		self.interface = interface

	# ------------------------------------------------------------------ layout
	def __Keep(self, wnd):
		self.keep.append(wnd)
		return wnd

	def __Text(self, parent, text, x, y, center = True):
		line = ui.TextLine()
		line.SetParent(parent)
		line.SetPosition(x, y)
		if center:
			line.SetHorizontalAlignCenter()
		line.SetOutline()
		line.SetText(text)
		line.Show()
		return self.__Keep(line)

	def __ValueBox(self, parent, x, y):
		box = ui.ImageBox()
		box.SetParent(parent)
		box.LoadImage(CAPE_PATH.format("/input_output.png"))
		box.SetPosition(x, y)
		box.AddFlag("not_pick")
		box.Show()
		self.__Keep(box)
		text = ui.TextLine()
		text.SetParent(box)
		text.SetPosition(box.GetWidth() / 2, 2)
		text.SetHorizontalAlignCenter()
		text.Show()
		self.__Keep(text)
		return text

	def __Button(self, parent, text, x, y, event):
		btn = ui.Button()
		btn.SetParent(parent)
		btn.SetPosition(x, y)
		btn.SetUpVisual(CAPE_PATH.format("/btn_norm.png"))
		btn.SetOverVisual(CAPE_PATH.format("/btn_hover.png"))
		btn.SetDownVisual(CAPE_PATH.format("/btn_select.png"))
		btn.SetDisableVisual(CAPE_PATH.format("/btn_select.png"))
		btn.SetText(text)
		btn.SetEvent(event)
		btn.Show()
		return self.__Keep(btn)

	def CreateWindow(self):
		self.SetSize(WIDTH, HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetTitleName(localeInfo.BRAVE_CAPE_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))

		bg = ui.ImageBox()
		bg.SetParent(self)
		bg.LoadImage(CAPE_PATH.format("/background.png"))
		bg.SetPosition(BG_X, BG_Y)
		bg.Show()
		self.__Keep(bg)

		# reference: the slot shows icon/item/mantie.png, not the item icon of the cape
		slot = ui.ImageBox()
		slot.SetParent(bg)
		slot.SetPosition(51, 12)
		slot.LoadImage(CAPE_ICON)
		slot.SAFE_SetStringEvent("MOUSE_LEFT_BUTTON_UP", self.__OnItemShortcut, 0)
		slot.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OverInItem, 0)
		slot.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OverOutItem)
		slot.Show()
		self.__children["capeSlot"] = self.__Keep(slot)

		self.__Text(bg, localeInfo.BRAVE_CAPE_TIME, 10, 59, False)
		timeSlider = SliderBarNew()
		timeSlider.SetParent(bg)
		timeSlider.SetPosition(10, 76)
		timeSlider.SetEvent(ui.__mem_func__(self.OnChangeTimeSlider))
		timeSlider.Show()
		self.__children["timeSlider"] = timeSlider
		self.__children["timeText"] = self.__ValueBox(bg, 89, 73)

		self.__Text(bg, localeInfo.BRAVE_CAPE_RANGE, 10, 92, False)
		rangeSlider = SliderBarNew()
		rangeSlider.SetParent(bg)
		rangeSlider.SetPosition(10, 109)
		rangeSlider.SetEvent(ui.__mem_func__(self.OnChangeRangeSlider))
		rangeSlider.Show()
		self.__children["rangeSlider"] = rangeSlider
		self.__children["rangeText"] = self.__ValueBox(bg, 89, 106)

		self.__children["startBtn"] = self.__Button(bg, localeInfo.BRAVE_CAPE_START, 5, 128, lambda: self.__ClickStatusBtn(1))
		self.__children["stopBtn"] = self.__Button(bg, localeInfo.BRAVE_CAPE_STOP, 69, 128, lambda: self.__ClickStatusBtn(0))

		self.SetCenterPosition()

	# ------------------------------------------------------------------ data
	def GetCapeVnum(self):
		for vnum in CAPE_VNUMS:
			if player.GetItemCountByVnum(vnum) > 0:
				return vnum
		return CAPE_VNUMS[0]

	def GetCapeRange(self):
		return self.capeRange

	def __GetCapeCount(self):
		return sum(player.GetItemCountByVnum(vnum) for vnum in CAPE_VNUMS)

	def SaveData(self):
		try:
			f = open(SAVE_FILE, "w")
			f.write("%d %d %d" % (self.capeTime, self.capeRange, int(self.isActive or self.wasActive)))
			f.close()
		except Exception:
			pass

	def LoadData(self):
		try:
			f = open(SAVE_FILE, "r")
			splitList = f.read().split()
			f.close()
			self.capeTime = max(TIME_MIN, min(TIME_MAX, int(splitList[0])))
			self.capeRange = max(RANGE_MIN, min(RANGE_MAX, int(splitList[1])))
			self.wasActive = len(splitList) > 2 and splitList[2] == "1"
		except Exception:
			self.capeTime, self.capeRange, self.wasActive = TIME_DEFAULT, RANGE_DEFAULT, False
		self.Refresh()

	# ------------------------------------------------------------------ view
	def Refresh(self, sliderPosition = True):
		if not self.__children:
			return
		if sliderPosition:
			self.__children["timeSlider"].SetSliderPos(float(self.capeTime - TIME_MIN) / (TIME_MAX - TIME_MIN))
			self.__children["rangeSlider"].SetSliderPos(float(self.capeRange - RANGE_MIN) / (RANGE_MAX - RANGE_MIN))
		self.__children["timeText"].SetText("%d" % self.capeTime)
		self.__children["rangeText"].SetText("%d" % self.capeRange)

		slot = self.__children["capeSlot"]
		slot.SetAlpha(1.0 if player.GetElk() >= CAPE_COST else 0.5)

		if self.isActive:
			self.__children["startBtn"].Disable()
			self.__children["stopBtn"].Enable()
		else:
			self.__children["startBtn"].Enable()
			self.__children["stopBtn"].Disable()

	def OnChangeTimeSlider(self):
		val = self.__children["timeSlider"].GetSliderPos()
		self.capeTime = TIME_MIN + int(round(val * (TIME_MAX - TIME_MIN)))
		self.Refresh(False)

	def OnChangeRangeSlider(self):
		val = self.__children["rangeSlider"].GetSliderPos()
		self.capeRange = RANGE_MIN + int(round(val * (RANGE_MAX - RANGE_MIN) / 100.0)) * 100
		self.Refresh(False)

	def __ClickStatusBtn(self, type):
		if type:
			if player.GetElk() < CAPE_COST:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.BRAVE_CAPE_NO_YANG)
				return
			net.SendChatPacket("/brave_cape active %d %d" % (self.capeTime, self.capeRange))
		else:
			net.SendChatPacket("/brave_cape deactive")
		self.SaveData()

	def ToggleStatusShortcut(self):
		self.__ClickStatusBtn(0 if self.isActive else 1)

	def IsActive(self):
		return self.isActive

	def SetStatus(self, status):
		self.isActive = str(status) == "1"
		self.wasActive = self.isActive
		interface = self.interface
		inventory = interface and getattr(interface, "wndInventory", None)
		if inventory and hasattr(inventory, "SetAutoCapeStatus"):
			inventory.SetAutoCapeStatus(self.isActive)
		self.Refresh(False)
		self.SaveData()

	def OnWarping(self):
		# keep "was active" in the save file, the server stops the loop with the character
		self.wasActive = self.isActive
		self.SaveData()
		self.isActive = False
		interface = self.interface
		inventory = interface and getattr(interface, "wndInventory", None)
		if inventory and hasattr(inventory, "SetAutoCapeStatus"):
			inventory.SetAutoCapeStatus(False)

	def OnMapLoaded(self):
		# AUTO_CAPE_AFTER_WARP: start again with the saved interval / range after a warp or login
		if self.wasActive and not self.isActive and IsAutoCapeAfterWarpEnabled() and player.GetElk() >= CAPE_COST:
			net.SendChatPacket("/brave_cape active %d %d" % (self.capeTime, self.capeRange))

	# ------------------------------------------------------------------ item slot
	def __OnItemShortcut(self, eventName = "left", click = 0, itemIdx = 0):
		# reference tooltip shortcuts: Ctrl + click = shop search, Ctrl + Alt + click = Wikipedia
		interface = self.interface
		if not interface:
			return
		ctrl = app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)
		alt = app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT)
		vnum = self.GetCapeVnum()
		if ctrl and alt and getattr(app, "ENABLE_WIKI_SYSTEM", 0) and hasattr(interface, "SearchWikiForItem"):
			interface.SearchWikiForItem(vnum)
		elif ctrl and not alt and getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0) and hasattr(interface, "SearchShopForItem"):
			interface.SearchShopForItem(vnum)

	def __OverInItem(self, slotIndex = 0):
		tt = self.toolTipItem
		if not tt:
			return
		tt.ClearToolTip()
		tt.toolTipWidth = 300	# room for the 288 px shortcut table (AppendShortcutTable)
		item.SelectItem(self.GetCapeVnum())
		tt.SetTitle(item.GetItemName())
		tt.AppendSpace(5)
		tt.AppendTextLine(localeInfo.BRAVE_CAPE_TOOLTIP_DESC, tt.NORMAL_COLOR)
		cost = "{:,}".format(CAPE_COST).replace(",", ".")
		tt.AppendTextLine(localeInfo.BRAVE_CAPE_TOOLTIP_COST % cost + " |Eemoji/money_icon|e", tt.COLOR_TEXT_SHORTCUT)
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0):
			rows.append((("ctrl", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		if getattr(app, "ENABLE_WIKI_SYSTEM", 0):
			rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
		tt.AppendShortcutTable(rows)
		tt.ShowToolTip()

	def __OverOutItem(self):
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	# ------------------------------------------------------------------ open / close
	def AdjustPosition(self, x, y):
		self.SetPosition(max(0, x), max(0, y))

	def Open(self):
		# reference: opens on the left of the inventory, under its title
		inventory = self.interface and getattr(self.interface, "wndInventory", None)
		if inventory and inventory.IsShow():
			(x, y) = inventory.GetGlobalPosition()
			self.AdjustPosition(x - self.GetWidth() - 38, y + 109)
		self.Refresh()
		self.Show()
		self.SetTop()

	def Close(self):
		self.SaveData()
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()
		self.Hide()

	def Toggle(self):
		if self.IsShow():
			self.Close()
		else:
			self.Open()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		# cape count / slot follow the inventory while the window is open
		if app.GetTime() >= getattr(self, "_nextRefresh", 0.0):
			self._nextRefresh = app.GetTime() + 0.5
			self.Refresh(False)
