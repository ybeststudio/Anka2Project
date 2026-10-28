if __USE_DYNAMIC_MODULE__:
	import pyapi

	app = __import__(pyapi.GetModuleName("app"))
	player = __import__(pyapi.GetModuleName("player"))
	net = __import__(pyapi.GetModuleName("net"))
else:
	import app
	import player
	import net

import ui
import item
import chat
import uiCommon
import localeInfo
import mouseModule
import uiScriptLocale

if not hasattr(player, "COMB_WND_SLOT_MEDIUM"):
	player.COMB_WND_SLOT_MEDIUM = 0
	player.COMB_WND_SLOT_BASE = 1
	player.COMB_WND_SLOT_MATERIAL = 2
	player.COMB_WND_SLOT_RESULT = 3
	player.COMB_WND_SLOT_MAX = 4

class ItemCombinationWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.wndBgImage = None
		self.wndMediumSlot = None
		self.wndCombSlot = None
		self.btnAccept = None
		self.btnCancel = None
		self.titleBar = None
		self.tooltipItem = None
		self.inven = None
		self.pop = None
		self.xCombinationWindow = 0
		self.yCombinationWindow = 0
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.isLoaded = 0
		self.wndBgImage = None
		self.wndMediumSlot = None
		self.wndCombSlot = None
		self.btnAccept = None
		self.btnCancel = None
		self.titleBar = None
		self.tooltipItem = None
		self.inven = None
		self.pop = None

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ItemCombinationWindow.py")
		except:
			import exception
			exception.Abort("ItemCombinationWindow.__LoadWindow.UIScript/ItemCombinationWindow.py")

		try:
			self.wndBgImage = self.GetChild("Item_Comb_Background_Image")
			self.wndMediumSlot = self.GetChild("MediumSlot")
			self.wndCombSlot = self.GetChild("CombSlot")
			self.btnAccept = self.GetChild("AcceptButton")
			self.btnCancel = self.GetChild("CancelButton")
			self.titleBar = self.GetChild("TitleBar")
		except:
			import exception
			exception.Abort("ItemCombinationWindow.__LoadWindow.GetChild")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.btnAccept.SetEvent(ui.__mem_func__(self.Accept))
		self.btnCancel.SetEvent(ui.__mem_func__(self.Close))

		self.wndMediumSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.wndMediumSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.wndMediumSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndMediumSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndMediumSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.wndMediumSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.wndCombSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.wndCombSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.wndCombSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndCombSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.wndCombSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.wndCombSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.ChangeBackgroundImage(player.COMB_WND_SLOT_MEDIUM, False)

	def Open(self):
		self.__LoadWindow()
		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)
		(self.xCombinationWindow, self.yCombinationWindow) = self.GetGlobalPosition()

	def Close(self):
		if self.pop:
			self.pop.Close()
			self.pop = None
		self.ClearSlot()
		net.SendItemCombinationPacketCancel()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnTop(self):
		pass

	def BindInterface(self, interface):
		self.interface = interface

	def SetInven(self, inven):
		self.inven = inven

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetInvenWindow(self, wndInven):
		self.wndInven = wndInven

	def ChangeBackgroundImage(self, slotIndex, isAttached):
		if slotIndex == player.COMB_WND_SLOT_MEDIUM:
			if isAttached:
				self.wndBgImage.LoadImage(uiScriptLocale.LOCALE_UISCRIPT_PATH + "combination/comb2.tga")
			else:
				self.wndBgImage.LoadImage(uiScriptLocale.LOCALE_UISCRIPT_PATH + "combination/comb1.tga")

	def IsFilledAllSlots(self):
		for slot in (player.COMB_WND_SLOT_MEDIUM, player.COMB_WND_SLOT_BASE, player.COMB_WND_SLOT_MATERIAL):
			if player.GetInvenSlotAttachedToConbWindowSlot(slot) < 0:
				return False
		return True

	def CanAttachSlot(self, slotIndex, windowType, invenSlot):
		if slotIndex == player.COMB_WND_SLOT_MEDIUM:
			return player.CanAttachToCombMediumSlot(slotIndex, windowType, invenSlot)
		elif slotIndex in (player.COMB_WND_SLOT_BASE, player.COMB_WND_SLOT_MATERIAL):
			return player.CanAttachToCombItemSlot(slotIndex, windowType, invenSlot)
		return False

	def CantAttachToCombSlot(self, slotIndex, windowType, invenSlot):
		return not self.CanAttachSlot(slotIndex, windowType, invenSlot)

	def AttachToCombinationSlot(self, slotIndex, windowType, invenSlot):
		if not self.CanAttachSlot(slotIndex, windowType, invenSlot):
			return

		itemVnum = player.GetItemIndex(windowType, invenSlot)
		itemCount = player.GetItemCount(windowType, invenSlot)

		if slotIndex == player.COMB_WND_SLOT_MEDIUM:
			self.wndMediumSlot.SetItemSlot(slotIndex, itemVnum, itemCount)
			self.ChangeBackgroundImage(slotIndex, True)
		elif slotIndex in (player.COMB_WND_SLOT_BASE, player.COMB_WND_SLOT_MATERIAL):
			self.wndCombSlot.SetItemSlot(slotIndex, itemVnum, itemCount)

		player.SetItemCombinationWindowActivedItemSlot(slotIndex, invenSlot)
		self.SetResultItem()
		self.RefreshCantMouseEventSlot()

	def AttachItemToSelectSlot(self, slotIndex, windowType, invenSlot):
		self.AttachToCombinationSlot(slotIndex, windowType, invenSlot)

	def SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			self.AttachItemToSelectSlot(selectedSlotPos, attachedInvenType, attachedSlotPos)
			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			self.AttachItemToSelectSlot(selectedSlotPos, attachedInvenType, attachedSlotPos)
			mouseModule.mouseController.DeattachObject()
		else:
			self.__SelectItemSlot(selectedSlotPos)

	def __SelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == player.COMB_WND_SLOT_RESULT:
			return
		self.RemoveItemSlot(selectedSlotPos)

	def UseItemSlot(self, selectedSlotPos):
		if selectedSlotPos == player.COMB_WND_SLOT_RESULT:
			return
		self.RemoveItemSlot(selectedSlotPos)

	def RemoveItemSlot(self, selectedSlotPos):
		if selectedSlotPos == player.COMB_WND_SLOT_MEDIUM:
			self.ClearSlot()
		else:
			player.SetItemCombinationWindowActivedItemSlot(selectedSlotPos, -1)
			self.wndCombSlot.ClearSlot(selectedSlotPos)
			self.SetResultItem()
			self.RefreshCantMouseEventSlot()

	def ClearSlot(self):
		for i in xrange(player.COMB_WND_SLOT_MAX):
			player.SetItemCombinationWindowActivedItemSlot(i, -1)
		self.wndMediumSlot.ClearSlot(player.COMB_WND_SLOT_MEDIUM)
		self.wndCombSlot.ClearSlot(player.COMB_WND_SLOT_BASE)
		self.wndCombSlot.ClearSlot(player.COMB_WND_SLOT_MATERIAL)
		self.wndCombSlot.ClearSlot(player.COMB_WND_SLOT_RESULT)
		self.ChangeBackgroundImage(player.COMB_WND_SLOT_MEDIUM, False)
		self.RefreshCantMouseEventSlot()

	def RefreshCantMouseEventSlot(self):
		if self.inven:
			self.inven.RefreshBagSlotWindow()

	def SetResultItem(self):
		baseInvenSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_BASE)
		materialInvenSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_MATERIAL)

		if baseInvenSlot >= 0 and materialInvenSlot >= 0:
			baseVnum = player.GetItemIndex(player.INVENTORY, baseInvenSlot)
			self.wndCombSlot.SetItemSlot(player.COMB_WND_SLOT_RESULT, baseVnum, 1)
			player.SetItemCombinationWindowActivedItemSlot(player.COMB_WND_SLOT_RESULT, baseInvenSlot)
		else:
			self.wndCombSlot.ClearSlot(player.COMB_WND_SLOT_RESULT)
			player.SetItemCombinationWindowActivedItemSlot(player.COMB_WND_SLOT_RESULT, -1)

	def IsItemTypeAndSubTypeByCombSlot(self, slotIndex, itemType, itemSubType = -1):
		invenSlot = player.GetInvenSlotAttachedToConbWindowSlot(slotIndex)
		if invenSlot < 0:
			return False

		itemVnum = player.GetItemIndex(player.INVENTORY, invenSlot)
		item.SelectItem(itemVnum)
		if item.GetItemType() != itemType:
			return False

		if itemSubType != -1:
			if item.GetItemSubType() != itemSubType:
				return False

		return True

	def GetItemSubTypeByCombSlot(self, slotIndex):
		invenSlot = player.GetInvenSlotAttachedToConbWindowSlot(slotIndex)
		if invenSlot < 0:
			return -1

		itemVnum = player.GetItemIndex(player.INVENTORY, invenSlot)
		item.SelectItem(itemVnum)
		return item.GetItemSubType()

	def OverInItem(self, slotIndex):
		if slotIndex == player.COMB_WND_SLOT_RESULT:
			self.__ShowResultToolTip(slotIndex)
		else:
			self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def __ShowToolTip(self, slotIndex):
		if not self.tooltipItem:
			return
		invenSlot = player.GetInvenSlotAttachedToConbWindowSlot(slotIndex)
		if invenSlot < 0:
			return
		self.tooltipItem.SetInventoryItem(invenSlot)

	def __ShowResultToolTip(self, slotIndex):
		if not self.tooltipItem:
			return
		baseInvenSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_BASE)
		materialInvenSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_MATERIAL)
		if baseInvenSlot < 0 or materialInvenSlot < 0:
			return
		self.tooltipItem.SetResulItemAttrMove(baseInvenSlot, materialInvenSlot)

	def Accept(self):
		if not self.IsFilledAllSlots():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.COMB_NOT_ALL_SLOT_APPEND_ITEM)
			return

		mediumSubType = self.GetItemSubTypeByCombSlot(player.COMB_WND_SLOT_MEDIUM)

		self.pop = uiCommon.QuestionDialog()
		if mediumSubType == item.MEDIUM_MOVE_COSTUME_ACCE_ATTR:
			self.pop.SetText1(localeInfo.COMB_ACCE_WILL_REMOVE_MATERIAL)
			self.pop.SetText2(localeInfo.COMB_ACCE_IS_CONTINUE_PROCESS)
		else:
			self.pop.SetText1(localeInfo.COMB_WILL_REMOVE_MATERIAL)
			self.pop.SetText2(localeInfo.COMB_IS_CONTINUE_PROCESS)

		self.pop.SetAcceptEvent(ui.__mem_func__(self.OnDialogAcceptEvent))
		self.pop.SetCancelEvent(ui.__mem_func__(self.OnDialogCloseEvent))
		self.pop.Open()

	def OnDialogAcceptEvent(self):
		if self.IsFilledAllSlots():
			mediumSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_MEDIUM)
			baseSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_BASE)
			materialSlot = player.GetInvenSlotAttachedToConbWindowSlot(player.COMB_WND_SLOT_MATERIAL)
			net.SendItemCombinationPacket(mediumSlot, baseSlot, materialSlot)

		self.OnDialogCloseEvent()

	def OnDialogCloseEvent(self):
		if self.pop:
			self.pop.Close()
			self.pop = None

	def OnUpdate(self):
		USE_COMB_WINDOW_LIMIT_RANGE = 500
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xCombinationWindow) > USE_COMB_WINDOW_LIMIT_RANGE or abs(y - self.yCombinationWindow) > USE_COMB_WINDOW_LIMIT_RANGE:
			self.Close()
