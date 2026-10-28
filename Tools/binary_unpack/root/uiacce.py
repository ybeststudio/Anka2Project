if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import snd
import item
import acce
import uiToolTip
import localeInfo
import uiInventory
import mouseModule
import uiScriptLocale

if app.ENABLE_SLOT_MARKING_SYSTEM:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
	if app.ENABLE_SPECIAL_INVENTORY:
		SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_INVENTORY_PAGE_SIZE

class CombineWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.PositionOut = 0
		self.PositionStartX = 0
		self.PositionStartY = 0

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

			self.ItemListIdx = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

			self.ItemListIdx = []

	def Destroy(self):
		self.ClearAllSlots()
		self.ClearDictionary()
		self.titleBar = None
		self.btnAccept = None
		self.btnCancel = None
		self.acceSlot = None
		self.needMoney = None
		self.Result = None
		self.PositionOut = 0
		self.PositionStartX = 0
		self.PositionStartY = 0

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

	def LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/acce_combinewindow.py")

		except:
			import exception
			exception.Abort("Acce_CombineWindow.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.btnAccept = self.GetChild("AcceptButton")
			self.btnCancel = self.GetChild("CancelButton")
			self.needMoney = self.GetChild("NeedMoney")
			self.Result = self.GetChild("Result")
			self.acceSlot = self.GetChild("AcceSlot")
		except:
			import exception
			exception.Abort("Acce_CombineWindow.LoadDialog.BindObject")

		self.acceSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
		self.acceSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.acceSlot.SetUseSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.acceSlot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
		self.acceSlot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.OnClose))
		self.btnCancel.SetEvent(ui.__mem_func__(self.OnClose))
		self.btnAccept.SetEvent(ui.__mem_func__(self.OnAccept))
		self.tooltipItem = None

	def SetItemToolTip(self, itemTooltip):
		self.tooltipItem = itemTooltip

	def IsOpened(self):
		if self.IsShow() and self.isLoaded:
			return TRUE

		return FALSE

	def ClearAllSlots(self):
		acce.Clear()
		if self.acceSlot:
			for i in xrange(acce.WINDOW_MAX_MATERIALS + 1):
				self.acceSlot.ClearSlot(i)

	def Open(self):
		self.PositionOut = 0
		(self.PositionStartX, self.PositionStartY, z) = player.GetMainCharacterPosition()
		self.needMoney.SetText(localeInfo.ACCE_REFINE_COST % (acce.GetPrice()))
		self.ClearAllSlots()
		self.SetCenterPosition()
		self.Show()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_ACCE_COMBINE)
				self.interface.RefreshMarkInventoryBag()

	def Close(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.Hide()

	def OnClose(self):
		acce.SendCloseRequest()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

	def OnPressEscapeKey(self):
		self.OnClose()
		return TRUE

	def OnAccept(self):
		acce.SendRefineRequest()

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetInven(self, inven):
			self.inven = inven

		if app.ENABLE_SPECIAL_INVENTORY:
			def SetSpecialInven(self, invenSpecial):
				self.invenSpecial = invenSpecial

		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				item.SelectItem(itemIndex)

				if (item.GetItemType() != item.ITEM_TYPE_COSTUME or item.GetItemSubType() != item.COSTUME_TYPE_ACCE):
					return TRUE

			return FALSE

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_ACCE_COMBINE)
			self.interface.RefreshMarkInventoryBag()

		def AcceItemMouseEvent(self):
			if app.ENABLE_SPECIAL_INVENTORY:
				if not self.inven or not self.invenSpecial:
					return
			else:
				if not self.inven:
					return

			if not self.ItemListIdx:
				return

			page = self.inven.GetInventoryPageIndex()
			if app.ENABLE_SPECIAL_INVENTORY:
				special_page = self.invenSpecial.GetInventoryPageIndex()

			for i in self.ItemListIdx:
				if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE):
					lock_idx = i - (page * INVENTORY_PAGE_SIZE)
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

				if app.ENABLE_SPECIAL_INVENTORY:
					if self.invenSpecial.GetInventoryType() == 0:
						if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 1:
						if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 2:
						if (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 3:
						if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 4:
						if (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.CHANGERS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

	def OnUpdate(self):
		LIMIT_RANGE = acce.LIMIT_RANGE
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.PositionStartX) >= LIMIT_RANGE or abs(y - self.PositionStartY) >= LIMIT_RANGE:
			if not self.PositionOut:
				self.PositionOut += 1
				self.OnClose()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.AcceItemMouseEvent()

	def OnSelectEmptySlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if not isAttached or selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
			return

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
		mouseModule.mouseController.DeattachObject()
		if attachedSlotType == player.SLOT_TYPE_INVENTORY and attachedInvenType == player.INVENTORY:
			acce.Add(attachedInvenType, attachedSlotPos, selectedSlotPos)

	def OnSelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
			return

		mouseModule.mouseController.DeattachObject()
		acce.Remove(selectedSlotPos)

	def OnOverInItem(self, selectedSlotPos):
		if self.tooltipItem:
			if selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
				(isHere, iCell) = acce.GetAttachedItem(0)
				if isHere:
					self.tooltipItem.ClearToolTip()
					self.tooltipItem.SetInventoryItem(iCell)
					self.tooltipItem.SetAcceResultItem(iCell)
			else:
				(isHere, iCell) = acce.GetAttachedItem(selectedSlotPos)
				if isHere:
					self.tooltipItem.SetInventoryItem(iCell)

	def OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Refresh(self, iAct):
		self.needMoney.SetText(localeInfo.ACCE_REFINE_COST % (acce.GetPrice()))
		self.acceSlot.ClearSlot(acce.WINDOW_MAX_MATERIALS)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

		for i in xrange(acce.WINDOW_MAX_MATERIALS):
			self.acceSlot.ClearSlot(i)
			(isHere, iCell) = acce.GetAttachedItem(i)
			if isHere:
				self.acceSlot.SetItemSlot(i, player.GetItemIndex(iCell), 0)

				if app.ENABLE_SLOT_MARKING_SYSTEM:
					self.ItemListIdx.append(iCell)

				if i == int(acce.WINDOW_MAX_MATERIALS - 1):
					(itemVnum, MinAbs, MaxAbs) = acce.GetResultItem()
					if not itemVnum:
						break

					self.acceSlot.SetItemSlot(i + 1, itemVnum, 0)
					break

class AbsorbWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.PositionOut = 0
		self.PositionStartX = 0
		self.PositionStartY = 0

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

			self.ItemListIdx = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

			self.ItemListIdx = []

	def Destroy(self):
		self.ClearDictionary()
		self.titleBar = None
		self.btnAccept = None
		self.btnCancel = None
		self.acceSlot = None
		self.PositionOut = 0
		self.PositionStartX = 0
		self.PositionStartY = 0

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

	def LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/acce_absorbwindow.py")

		except:
			import exception
			exception.Abort("Acce_AbsorbtionWindow.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.btnAccept = self.GetChild("AcceptButton")
			self.btnCancel = self.GetChild("CancelButton")
			self.acceSlot = self.GetChild("AcceSlot")
		except:
			import exception
			exception.Abort("Acce_AbsorbtionWindow.LoadDialog.BindObject")

		self.acceSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
		self.acceSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.acceSlot.SetUseSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.acceSlot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
		self.acceSlot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.OnClose))
		self.btnCancel.SetEvent(ui.__mem_func__(self.OnClose))
		self.btnAccept.SetEvent(ui.__mem_func__(self.OnAccept))

		self.tooltipItem = None

	def SetItemToolTip(self, itemTooltip):
		self.tooltipItem = itemTooltip

	def IsOpened(self):
		if self.IsShow() and self.isLoaded:
			return TRUE

		return FALSE

	def Open(self):
		self.PositionOut = 0
		(self.PositionStartX, self.PositionStartY, z) = player.GetMainCharacterPosition()
		for i in xrange(acce.WINDOW_MAX_MATERIALS + 1):
			self.acceSlot.ClearSlot(i)
		self.SetCenterPosition()
		self.Show()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_ACCE_ABSORB)
				self.interface.RefreshMarkInventoryBag()

	def Close(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.Hide()

	def OnClose(self):
		acce.SendCloseRequest()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

	def OnPressEscapeKey(self):
		self.OnClose()
		return TRUE

	def OnAccept(self):
		acce.SendRefineRequest()

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetInven(self, inven):
			self.inven = inven

		if app.ENABLE_SPECIAL_INVENTORY:
			def SetSpecialInven(self, invenSpecial):
				self.invenSpecial = invenSpecial

		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				item.SelectItem(itemIndex)

				if ((item.GetItemType() != item.ITEM_TYPE_ARMOR or item.GetItemSubType() != item.ARMOR_BODY) and item.GetItemType() != item.ITEM_TYPE_WEAPON and (item.GetItemType() != item.ITEM_TYPE_COSTUME or item.GetItemSubType() != item.COSTUME_TYPE_ACCE)):
					return TRUE

			return FALSE

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_ACCE_ABSORB)
			self.interface.RefreshMarkInventoryBag()

		def AcceItemMouseEvent(self):
			if app.ENABLE_SPECIAL_INVENTORY:
				if not self.inven or not self.invenSpecial:
					return
			else:
				if not self.inven:
					return

			if not self.ItemListIdx:
				return

			page = self.inven.GetInventoryPageIndex()
			if app.ENABLE_SPECIAL_INVENTORY:
				special_page = self.invenSpecial.GetInventoryPageIndex()

			for i in self.ItemListIdx:
				if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE):
					lock_idx = i - (page * INVENTORY_PAGE_SIZE)
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

				if app.ENABLE_SPECIAL_INVENTORY:
					if self.invenSpecial.GetInventoryType() == 0:
						if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 1:
						if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 2:
						if (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 3:
						if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

					if self.invenSpecial.GetInventoryType() == 4:
						if (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (item.CHANGERS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							lock_idx = i - (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCantMouseEventSlot(lock_idx)

	def OnUpdate(self):
		LIMIT_RANGE = acce.LIMIT_RANGE
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.PositionStartX) >= LIMIT_RANGE or abs(y - self.PositionStartY) >= LIMIT_RANGE:
			if not self.PositionOut:
				self.PositionOut += 1
				self.OnClose()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.AcceItemMouseEvent()

	def OnSelectEmptySlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if not isAttached or selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
			return

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
		mouseModule.mouseController.DeattachObject()
		if attachedSlotType == player.SLOT_TYPE_INVENTORY and attachedInvenType == player.INVENTORY:
			acce.Add(attachedInvenType, attachedSlotPos, selectedSlotPos)

	def OnSelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
			return

		mouseModule.mouseController.DeattachObject()
		acce.Remove(selectedSlotPos)

	def OnOverInItem(self, selectedSlotPos):
		if self.tooltipItem:
			if selectedSlotPos == acce.WINDOW_MAX_MATERIALS:
				(isHere1, iCell1) = acce.GetAttachedItem(0)
				(isHere2, iCell2) = acce.GetAttachedItem(1)
				if isHere1 and isHere2:
					self.tooltipItem.ClearToolTip()
					self.tooltipItem.SetInventoryItem(iCell1)
					self.tooltipItem.SetAcceResultAbsItem(iCell1, iCell2)
			else:
				(isHere, iCell) = acce.GetAttachedItem(selectedSlotPos)
				if isHere:
					self.tooltipItem.SetInventoryItem(iCell)

	def OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def Refresh(self, iAct):
		self.acceSlot.ClearSlot(acce.WINDOW_MAX_MATERIALS)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

		for i in xrange(acce.WINDOW_MAX_MATERIALS):
			self.acceSlot.ClearSlot(i)
			(isHere, iCell) = acce.GetAttachedItem(i)
			if isHere:
				self.acceSlot.SetItemSlot(i, player.GetItemIndex(iCell), 0)

				if app.ENABLE_SLOT_MARKING_SYSTEM:
					self.ItemListIdx.append(iCell)

				if i == int(acce.WINDOW_MAX_MATERIALS - 1):
					(itemVnum, MinAbs, MaxAbs) = acce.GetResultItem()
					if not itemVnum:
						break

					self.acceSlot.SetItemSlot(i + 1, itemVnum, 0)
					break

