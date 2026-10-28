if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import mouseModule
import snd
import item
import chat
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiToolTip
import uiPrivateShopBuilder
import localeInfo
import constInfo
import ime
import wndMgr
import exchange

from _weakref import proxy

if app.ENABLE_QUICK_SELL_ITEM:
	import shop

if app.ENABLE_AUTO_SELL_SYSTEM:
    import uiautosell

ITEM_MALL_BUTTON_ENABLE = TRUE
ITEM_FLAG_APPLICABLE = 1 << 14

SKILL_BOOK_NUM = 0
UPGRADE_ITEMS_NUM = 1
STONE_NUM = 2
GIFT_BOX_NUM = 3
CHANGERS_NUM = 4
SAFEBOX_BUTTON_NUM = 5
MALL_BUTTON_NUM = 6

class SpecialInventoryWindow(ui.ScriptWindow):
	USE_TYPE_TUPLE = (
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE2",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET"
	)

	if app.ENABLE_BONUS_COSTUME_SYSTEM:
		USE_TYPE_TUPLE += ("USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR")

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		USE_TYPE_TUPLE += ("USE_PUT_INTO_AURA_SOCKET",)

	SLOT_WINDOW_TYPE = {
		SKILL_BOOK_NUM : {"window" : player.INVENTORY, "slot" : player.SLOT_TYPE_SKILL_BOOK_INVENTORY},
		UPGRADE_ITEMS_NUM : {"window" : player.INVENTORY, "slot" : player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY},
		STONE_NUM : {"window" : player.INVENTORY, "slot" : player.SLOT_TYPE_STONE_INVENTORY},
		GIFT_BOX_NUM : {"window" : player.INVENTORY, "slot" : player.SLOT_TYPE_GIFT_BOX_INVENTORY},
		CHANGERS_NUM : {"window" : player.INVENTORY, "slot" : player.SLOT_TYPE_CHANGERS_INVENTORY}
	}

	questionDialog = None
	tooltipItem = None
	dlgPickMoney = None

	sellingSlotNumber = -1
	isLoaded = 0

	interface = None
	safebox = None
	mall = None
	toolTip = None

	inventoryPageIndex = 0
	inventoryType = 0

	tooltipInfo = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.categoryPageIndex = 0

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, category = 0):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

		self.RefreshItemSlot()

	def BindInterfaceClass(self, interface):
		self.interface = interface
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if hasattr(self, 'attachMetinDialog') and self.attachMetinDialog:
				self.attachMetinDialog.BindInterface(interface)

	def BindSafeboxAndMallClass(self, safebox, mall):
		self.safebox = proxy(safebox)
		self.mall = proxy(mall)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UiScript/SpecialInventoryWindow.py")
		except:
			import exception
			exception.Abort("SpecialInventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.titleName = self.GetChild("TitleName")

			if app.ENABLE_SORT_INVENTORY:
				self.sortInventoryButton = self.GetChild2("SortInventoryButton")

			self.SkillBookButton = self.GetChild("SkillBookButton")
			self.UpgradeItemsButton = self.GetChild("UpgradeItemsButton")
			self.stoneButton = self.GetChild("StoneButton")
			self.GiftBoxButton = self.GetChild("GiftBoxButton")
			self.ChangersButton = self.GetChild("ChangersButton")
			self.SafeboxButton = self.GetChild("SafeboxButton")
			self.MallButton = self.GetChild("MallButton")
			self.ScrollBar = self.GetChild("ScrollBar")
			self.ScrollButton = self.GetChild("ScrollButton")

			self.inventoryTab = []

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()

			self.loading_image = self.GetChild("loading_image")
			self.loading_image.Hide()

			if app.ENABLE_PICKUP_ITEM_EFFECT:
				self.listHighlightedSlot = []

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()
		except:
			import exception
			exception.Abort("SpecialInventoryWindow.LoadWindow.BindObject")

		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.SkillBookButton.SetEvent(lambda arg=0: self.SetInventoryType(arg))
		self.SkillBookButton.ShowToolTip = lambda arg=0: self.OverInButton(arg)
		self.SkillBookButton.HideToolTip = lambda arg=0: self.OverOutButton()

		self.UpgradeItemsButton.SetEvent(lambda arg=1: self.SetInventoryType(arg))
		self.UpgradeItemsButton.ShowToolTip = lambda arg=1: self.OverInButton(arg)
		self.UpgradeItemsButton.HideToolTip = lambda arg=1: self.OverOutButton()

		self.stoneButton.SetEvent(lambda arg=2: self.SetInventoryType(arg))
		self.stoneButton.ShowToolTip = lambda arg=2: self.OverInButton(arg)
		self.stoneButton.HideToolTip = lambda arg=2: self.OverOutButton()

		self.GiftBoxButton.SetEvent(lambda arg=3: self.SetInventoryType(arg))
		self.GiftBoxButton.ShowToolTip = lambda arg=3: self.OverInButton(arg)
		self.GiftBoxButton.HideToolTip = lambda arg=3: self.OverOutButton()

		self.ChangersButton.SetEvent(lambda arg=4: self.SetInventoryType(arg))
		self.ChangersButton.ShowToolTip = lambda arg=4: self.OverInButton(arg)
		self.ChangersButton.HideToolTip = lambda arg=4: self.OverOutButton()

		self.SafeboxButton.SetEvent(lambda arg=5: self.SetInventoryType(arg))
		self.SafeboxButton.ShowToolTip = lambda arg=5: self.OverInButton(arg)
		self.SafeboxButton.HideToolTip = lambda arg=5: self.OverOutButton()

		self.MallButton.SetEvent(lambda arg=6: self.SetInventoryType(arg))
		self.MallButton.ShowToolTip = lambda arg=6: self.OverInButton(arg)
		self.MallButton.HideToolTip = lambda arg=6: self.OverOutButton()

		self.SkillBookButton.Down()

		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.attachMetinDialog.SetInven(self)
		self.attachMetinDialog.Hide()

		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.Diff = (9 * player.SPECIAL_INVENTORY_PAGE_COUNT) - 9
		stepSize = 1.0 / self.Diff
		self.ScrollBar.SetScrollStep(stepSize)
		self.ScollPos = 0

		self.wndItem = wndItem
		self.dlgPickMoney = dlgPickMoney

		if app.ENABLE_SORT_INVENTORY:
			if self.sortInventoryButton:
				self.sortInventoryButton.SetEvent(ui.__mem_func__(self.ClickSortInventory))

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()

		self.SetInventoryType(0)
		self.RefreshBagSlotWindow()

		self.RefreshItemSlot()

	def OnScroll(self):

		ScollPos = int(self.ScrollBar.GetPos() * self.Diff)
		self.ScollPos = ScollPos

		if ScollPos <= 9:
			self.ScrollButton.LoadImage("d:/ymir work/ui/game/special_inventory/scroll_down.png")
		else:
			self.ScrollButton.LoadImage("d:/ymir work/ui/game/special_inventory/scroll_up.png")
		self.ScrollButton.SetScale(1, 1)

		self.RefreshBagSlotWindow()

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.ScrollBar.OnUp()
		else:
			self.ScrollBar.OnDown()

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		self.dlgPickMoney = 0
		self.questionDialog = None
		self.interface = None
		self.safebox = None
		self.mall = None
		self.toolTip = None

		if app.ENABLE_SORT_INVENTORY:
			self.sortInventoryButton = None

		self.inventoryTab = []
		self.inventoryPageIndex = 0

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.toolTip:
			self.toolTip.HideToolTip()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		if self.GetInventoryType() == SAFEBOX_BUTTON_NUM:
			self.__CloseSafebox()
			self.SetInventoryType(0)
		elif self.GetInventoryType() == MALL_BUTTON_NUM:
			self.__CloseMall()
			self.SetInventoryType(0)

		if self.tooltipInfo:
			for i in xrange(4): self.tooltipInfo[i].Hide()

		wndMgr.Hide(self.hWnd)

	def Close(self):
		self.Hide()

	def SetInventoryType(self, type, canPassSafeBoxAndMallEvent = FALSE):
		if self.GetInventoryType() == SAFEBOX_BUTTON_NUM:
			self.__CloseSafebox()
		elif self.GetInventoryType() == MALL_BUTTON_NUM:
			self.__CloseMall()

		self.inventoryType = int(type)

		m_buttons = [
			(self.SkillBookButton, localeInfo.SPECIAL_INVENTORY_SKILL_BOOK),
			(self.UpgradeItemsButton, localeInfo.SPECIAL_INVENTORY_MATERIAL),
			(self.stoneButton, localeInfo.SPECIAL_INVENTORY_STONE),
			(self.GiftBoxButton, localeInfo.SPECIAL_INVENTORY_CHEST),
			(self.ChangersButton, localeInfo.SPECIAL_INVENTORY_CHANGERS),
			(self.SafeboxButton, localeInfo.SPECIAL_INVENTORY_SAFEBOX),
			(self.MallButton, localeInfo.SPECIAL_INVENTORY_MALL)]

		for obj, _ in m_buttons:
			obj.SetUp()

		obeject, title = m_buttons[type]
		obeject.Down()
		self.titleName.SetText(title)

		if not canPassSafeBoxAndMallEvent:
			if type == SAFEBOX_BUTTON_NUM:
				self.ScrollBar.Hide()
				self.ScrollButton.Hide()
				if app.ENABLE_SORT_INVENTORY:
					self.sortInventoryButton.Disable()
				self.__OpenSafebox()
			elif type == MALL_BUTTON_NUM:
				self.ScrollBar.Hide()
				self.ScrollButton.Hide()
				if app.ENABLE_SORT_INVENTORY:
					self.sortInventoryButton.Disable()
				self.__OpenMall()
			else:
				self.ScrollBar.Hide()
				self.ScrollButton.Show()
				if app.ENABLE_SORT_INVENTORY:
					self.sortInventoryButton.Enable()

		if self.CanRefreshSpecialInventoryPage() is TRUE:
			self.RefreshBagSlotWindow()

	def OverInButton(self, stat):
		m_buttonsTooltip = [
			uiScriptLocale.SPECIAL_INVENTORY_SKILL_BOOK_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_MATERIAL_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_STONE_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_CHEST_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_CHANGERS_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_SAFEBOX_TOOLTIP,
			uiScriptLocale.SPECIAL_INVENTORY_MALL_TOOLTIP]

		self.toolTip.ClearToolTip()
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.AutoAppendNewTextLine(m_buttonsTooltip[stat])
		self.toolTip.Show()

	def OverOutButton(self):
		self.toolTip.Hide()

	if app.ENABLE_SORT_INVENTORY:
		def ClickSortInventory(self):
			self.__ClickStartButton()

		def __ClickStartButton(self):
			startQuestionDialog = uiCommon.QuestionDialog2()
			startQuestionDialog.SetText1(uiScriptLocale.SORT_INVENTORY_TEXT1)
			startQuestionDialog.SetText2(uiScriptLocale.SORT_INVENTORY_TEXT2)
			startQuestionDialog.SetAcceptEvent(ui.__mem_func__(self.__StartAccept))
			startQuestionDialog.SetCancelEvent(ui.__mem_func__(self.__StartCancel))
			startQuestionDialog.Open()
			self.startQuestionDialog = startQuestionDialog

		def __StartAccept(self):
			if self.inventoryType == 0:
				net.SendChatPacket("/click_sort_special_storage 0")
			elif self.inventoryType == 1:
				net.SendChatPacket("/click_sort_special_storage 1")
			elif self.inventoryType == 2:
				net.SendChatPacket("/click_sort_special_storage 2")
			elif self.inventoryType == 3:
				net.SendChatPacket("/click_sort_special_storage 3")
			elif self.inventoryType == 4:
				net.SendChatPacket("/click_sort_special_storage 4")
			elif self.inventoryType >= 5:
				return

			self.__StartCancel()

		def __StartCancel(self):
			self.startQuestionDialog.Close()

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex

		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		self.ScollPos = int(self.ScrollBar.GetPos() * self.Diff)

		if self.inventoryType == 0:
			return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.SKILL_BOOK_INVENTORY_SLOT_START + (self.ScollPos * 5)
		elif self.inventoryType == 1:
			return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (self.ScollPos * 5)
		elif self.inventoryType == 2:
			return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.STONE_INVENTORY_SLOT_START + (self.ScollPos * 5)
		elif self.inventoryType == 3:
			return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.GIFT_BOX_INVENTORY_SLOT_START + (self.ScollPos * 5)
		elif self.inventoryType == 4:
			return self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE + local + item.CHANGERS_INVENTORY_SLOT_START + (self.ScollPos * 5)

	def RefreshBagSlotWindow(self):
		if not self.wndItem:
			return

		if self.CanRefreshSpecialInventoryPage() is FALSE:
			return

		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndItem.SetItemSlot

		self.ScollPos = int(self.ScrollBar.GetPos() * self.Diff)

		for i in xrange(self.wndItem.GetSlotCount()):
			self.wndItem.DeactivateSlot(i)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				onTopWindow = self.interface.GetOnTopWindow()

		for i in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE):
			self.wndItem.EnableSlot(i)
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			self.wndItem.SetRealSlotNumber(i, slotNumber)

			itemCount = getItemCount(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)
			itemVnum = getItemVNum(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotNumber)

			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			setItemVNum(i, itemVnum, itemCount)

			if app.ENABLE_SLOT_MARKING_SYSTEM:
				if itemVnum and self.interface and onTopWindow:
					if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow, slotNumber):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				else:
					self.wndItem.SetUsableSlotOnTopWnd(i)

		if app.ENABLE_PICKUP_ITEM_EFFECT:
			self.__HighlightSlot_Refresh()

		self.wndItem.RefreshSlot()

	def RefreshItemSlot(self):
		if self.CanRefreshSpecialInventoryPage() is FALSE:
			return

		self.RefreshBagSlotWindow()

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")

		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType:

				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()

				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(FALSE)

			elif app.ENABLE_RENEWAL_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			elif app.ENABLE_AURA_COSTUME_SYSTEM and player.SLOT_TYPE_AURA == attachedSlotType:
				net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				mouseModule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")
				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		localSlotIndex = itemSlotIndex
		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType:

				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()

			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex, localSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)

				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if TRUE == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)
			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)

				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(TRUE)
				else:
					self.wndItem.SetUseMode(FALSE)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos:
			return

		if app.ENABLE_AURA_COSTUME_SYSTEM and player.IsAuraRefineWindowOpen():
			return

		if item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(FALSE)

		elif item.IsMetin(srcItemVID) and not item.IsMetin(player.GetItemIndex(dstItemSlotPos)):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		else:
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos, localSlotIndex):
		self.sellingSlotNumber = itemSlotPos
		itemIndex = player.GetItemIndex(itemSlotPos)
		itemCount = player.GetItemCount(itemSlotPos)

		self.sellingSlotitemIndex = itemIndex
		self.sellingSlotitemCount = itemCount

		item.SelectItem(itemIndex)

		if item.IsAntiFlag(item.ANTIFLAG_SELL):
			popup = uiCommon.PopupDialog()
			popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup
			return

		itemPrice = item.GetISellItemPrice()

		if itemPrice != 0:
			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice
			else:
				itemPrice = itemPrice * itemCount

		item.GetItemName(itemIndex)
		itemName = item.GetItemName()

		self.questionDialog = uiCommon.QuestionDialog("thin")
		self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.count = itemCount

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		if app.ENABLE_AUTO_REFINE:
			constInfo.AUTO_REFINE_TYPE = 1
			constInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
			constInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		return

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog("thin")
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(FALSE)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(TRUE)

		if app.ENABLE_PICKUP_ITEM_EFFECT:
			self.DelHighlightSlot(overSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()

			if player.SLOT_TYPE_INVENTORY == attachedItemType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedItemType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedItemType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedItemType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedItemType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == attachedItemType:

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
					self.wndItem.SetUseMode(TRUE)
					self.ShowToolTip(overSlotPos)
					return

		self.ShowToolTip(overSlotPos)

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if item.IsRefineScroll(srcItemVNum):
			return TRUE
		elif item.IsMetin(srcItemVNum):
			return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			return TRUE
		elif item.IsKey(srcItemVNum):
			return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return TRUE

		return FALSE

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if srcSlotPos == dstSlotPos and not item.IsMetin(srcItemVNum):
			return FALSE

		elif item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos) or (item.IsMetin(player.GetItemIndex(dstSlotPos)) and player.GetItemIndex(dstSlotPos) == srcItemVNum):
				return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		else:
			useType = item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return TRUE
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return TRUE
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return TRUE;
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_ARMOR == item.GetItemType() and item.ARMOR_BELT == item.GetItemSubType():
					return TRUE
			elif "USE_CHANGE_COSTUME_ATTR" == useType and app.ENABLE_BONUS_COSTUME_SYSTEM:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return TRUE;
			elif "USE_RESET_COSTUME_ATTR" == useType and app.ENABLE_BONUS_COSTUME_SYSTEM:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return TRUE;
			elif "USE_PUT_INTO_AURA_SOCKET" == useType and app.ENABLE_AURA_COSTUME_SYSTEM:
				dstItemVnum = player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVnum)

				if item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_AURA == item.GetItemSubType():
					if player.GetItemMetinSocket(dstSlotPos, player.ITEM_SOCKET_AURA_BOOST) == 0:
						return TRUE

			else:
				pass

		return FALSE

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType() and item.ITEM_TYPE_ARMOR != item.GetItemType():
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return TRUE

		return FALSE

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				return TRUE

		return FALSE

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return FALSE

		if curCount>=maxCount:
			return FALSE

		return TRUE

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return FALSE

		return TRUE

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return FALSE

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				attrCount += 1

		if attrCount<4:
			return TRUE

		return FALSE

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		# if (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)) and\
			# (app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT)):
			# item.SelectItem(player.GetItemIndex(slotIndex))
			# self.interface.SearchNewPrivateShopSearchItem(item.GetItemName())
			# return

		if self.interface.dlgExchange.IsShow() and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)):
			item.SelectItem(player.GetItemIndex(slotIndex))
			emptyExchangeSlots = self.GetExchangeEmptyItemPos(item.GetItemSize()[1])
			if emptyExchangeSlots == -1:
				return

			if item.IsAntiFlag(item.ANTIFLAG_GIVE):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANNOT_GIVE)
				return


			net.SendExchangeItemAddPacket(self.SLOT_WINDOW_TYPE[self.categoryPageIndex]["window"], slotIndex, emptyExchangeSlots[0])
		else:
			if app.ENABLE_AUTO_SELL_SYSTEM:
				if app.IsPressed(app.DIK_LCONTROL) and app.IsPressed(app.DIK_X):
					itemVnum = player.GetItemIndex(slotIndex)
					itemCount = player.GetItemCount(slotIndex)
					item.SelectItem(itemVnum)
					itemType = item.GetItemType()
					if itemType == item.ITEM_TYPE_GIFTBOX:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OTOMATIK_SAT_UYARI_SANDIK)
					else:
						if itemCount > 1:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OTOMATIK_SAT_UYARI)
						else: 
							deleteItemInstance = uiautosell.AutoSellWindow()
							deleteItemInstance.SendAutoSellAdd(itemVnum)
					return  
			self.__UseItem(slotIndex)
			mouseModule.mouseController.DeattachObject()
			self.OverOutItem()

	def GetExchangeEmptyItemPos(self, itemHeight):
		inventorySize = exchange.EXCHANGE_ITEM_MAX_NUM
		inventoryWidth = player.SPECIAL_INVENTORY_PAGE_SIZE
		GetBlockedSlots = lambda slot, size: [slot+(round*inventoryWidth) for round in xrange(size)] 
		blockedSlots = [element for sublist in [GetBlockedSlots(slot, item.GetItemSize(item.SelectItem(exchange.GetItemVnumFromSelf(slot)))[1]) for slot in xrange(inventorySize) if exchange.GetItemVnumFromSelf(slot) != 0] for element in sublist] 
		freeSlots = [slot for slot in xrange(inventorySize) if not slot in blockedSlots and not TRUE in [e in blockedSlots for e in [slot+(round*inventoryWidth) for round in xrange(itemHeight)]]] 
		return [freeSlots, -1][len(freeSlots) == 0]

	def __GetCurrentItemGrid(self):
		itemGrid = [[FALSE for slot in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE)] for page in xrange(player.SPECIAL_INVENTORY_PAGE_COUNT)]

		for page in xrange(player.SPECIAL_INVENTORY_PAGE_COUNT):
			for slot in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE):
				itemVnum = player.GetItemIndex(slot + page * player.SPECIAL_INVENTORY_PAGE_SIZE)
				if itemVnum <> 0:
					(w, h) = item.GetItemSize(item.SelectItem(itemVnum))
					for i in xrange(h):
						itemGrid[page][slot + i * 5] = TRUE

		return itemGrid

	def __FindEmptyCellForSize(self, itemGrid, size):
		for page in xrange(player.SPECIAL_INVENTORY_PAGE_COUNT):
			for slot in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE):
				if itemGrid[page][slot] == FALSE:
					possible = TRUE
					for i in xrange(size):
						p = slot + (i * 5)

						try:
							if itemGrid[page][p] == TRUE:
								possible = FALSE
								break
						except IndexError:
							possible = FALSE
							break

					if possible:
						return slot + page * player.SPECIAL_INVENTORY_PAGE_SIZE

		return -1

	def AttachItemFromSafebox(self, slotIndex, itemIndex):
		itemGrid = self.__GetCurrentItemGrid()

		if item.GetItemType(item.SelectItem(itemIndex)) == item.ITEM_TYPE_DS:
			return

		emptySlotIndex = self.__FindEmptyCellForSize(itemGrid, item.GetItemSize()[1])
		if emptySlotIndex <> -1:
			net.SendSafeboxCheckoutPacket(slotIndex, player.INVENTORY, emptySlotIndex)

		return TRUE

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

		if self.interface and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)) and self.interface.AttachInvenItemToOtherWindowSlot(player.INVENTORY, slotIndex):
			return

		if app.ENABLE_QUICK_SELL_ITEM:
			if app.IsPressed(app.DIK_LCONTROL) and self.IsSellItems(slotIndex):
				if not shop.IsPrivateShop():
					self.__SendSellItemPacket(slotIndex)
					return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				self.__UseItemAura(slotIndex)
				return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		elif app.IsPressed(app.DIK_LSHIFT):
			if player.GetItemTypeBySlot(slotIndex) == item.ITEM_TYPE_GACHA or\
				player.GetItemTypeBySlot(slotIndex) == item.ITEM_TYPE_GIFTBOX and\
				ItemVNum != 31374 and ItemVNum != 50255 and\
				ItemVNum != 50187 and ItemVNum != 50197 and\
				ItemVNum != 50188 and ItemVNum != 50189 and\
				ItemVNum != 50190 and ItemVNum != 50191 and\
				ItemVNum != 50192 and ItemVNum != 50193 and\
				ItemVNum != 50194 and ItemVNum != 50195:
				if app.ENABLE_VIEW_CHEST_DROP:
					if self.interface:
						if self.interface.dlgChestDrop:
							if not self.interface.dlgChestDrop.IsShow():
								self.interface.dlgChestDrop.Open(slotIndex)
								net.SendChestDropInfo(slotIndex)
		else:
			self.__SendUseItemPacket(slotIndex)

	def BindWikiWindow(self, wndWiki):
		self.wndWiki = wndWiki

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	if app.ENABLE_QUICK_SELL_ITEM:
		def IsSellItems(self, slotIndex):
			itemVnum = player.GetItemIndex(slotIndex)
			item.SelectItem(itemVnum)
			itemPrice = item.GetISellItemPrice()

			if itemPrice > 1:
				return TRUE

			return FALSE

		def __SendSellItemPacket(self, itemVNum):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
				return

			net.SendItemSellPacket(itemVNum)

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	def GetInventoryType(self):
		return self.inventoryType

	def CanManageSafeboxWindow(self):
		return bool(self.safebox)

	def CanManageMallWindow(self):
		return bool(self.mall)

	def CanRefreshSpecialInventoryPage(self):
		if self.GetInventoryType() == SAFEBOX_BUTTON_NUM or self.GetInventoryType() == MALL_BUTTON_NUM:
			return FALSE
		return TRUE

	def __OpenSafebox(self):
		net.SendChatPacket("/new_safebox_open")
		self.wndItem.Hide()
		for button in self.inventoryTab: button.Hide()
		self.loading_image.Show()

	def __CloseSafebox(self):
		net.SendChatPacket("/safebox_close")
		self.wndItem.Show()
		for button in self.inventoryTab: button.Show()
		if self.CanManageSafeboxWindow is TRUE: self.safebox.Hide()
		self.loading_image.Hide()

	def ShowSafeboxWindow(self, size):
		if self.GetInventoryType() != SAFEBOX_BUTTON_NUM or not self.IsShow():
			net.SendChatPacket("/safebox_close")
			return

		if self.CanManageSafeboxWindow() is FALSE: return
		self.safebox.ShowWindow(size)
		self.loading_image.Hide()

	def CommandCloseSafebox(self):
		if self.CanManageSafeboxWindow() is FALSE: return
		self.safebox.CommandCloseSafebox()

	def __OpenMall(self):
		net.SendChatPacket("/new_mall_open")
		self.wndItem.Hide()
		for button in self.inventoryTab: button.Hide()
		self.loading_image.Show()

	def __CloseMall(self):
		net.SendChatPacket("/mall_close")
		self.wndItem.Show()
		for button in self.inventoryTab: button.Show()
		if self.CanManageMallWindow() is TRUE: self.mall.Hide()
		self.loading_image.Hide()

	def ShowMallWindow(self, size):
		if self.GetInventoryType() != MALL_BUTTON_NUM or not self.IsShow():
			net.SendChatPacket("/mall_close")
			return

		if self.CanManageMallWindow() is FALSE: return
		self.mall.ShowWindow(size)
		self.loading_image.Hide()

	def CommandCloseMall(self):
		if self.CanManageMallWindow() is FALSE: return
		self.mall.CommandCloseMall()

	def CloseLoadSafeboxEvent(self):
		self.__CloseSafebox()
		self.SetInventoryType(0)

	def CloseLoadMallEvent(self):
		self.__CloseMall()
		self.SetInventoryType(0)

	def IsDlgQuestionShow(self):
		if self.dlgQuestion.IsShow():
			return TRUE
		else:
			return FALSE

	def CancelDlgQuestion(self):
		self.__Cancel()

	if app.ENABLE_PICKUP_ITEM_EFFECT:
		def __HighlightSlot_Refresh(self):
			for i in xrange(self.wndItem.GetSlotCount()):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlot(i, wndMgr.COLOR_TYPE_ORANGE)

		def __HighlightSlot_Clear(self):
			for i in xrange(self.wndItem.GetSlotCount()):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedSlot.remove(slotNumber)

		def HighlightSlot(self, slot):
			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

		def DelHighlightSlot(self, inventorylocalslot):
			if inventorylocalslot in self.listHighlightedSlot:
				if inventorylocalslot >= player.SPECIAL_INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(inventorylocalslot - (self.inventoryPageIndex * player.SPECIAL_INVENTORY_PAGE_SIZE) )
				else:
					self.wndItem.DeactivateSlot(inventorylocalslot)

				self.listHighlightedSlot.remove(inventorylocalslot)

				self.RefreshBagSlotWindow()

	if app.ENABLE_BONUS_COSTUME_SYSTEM:
		def __CanChangeCostumeAttrList(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return FALSE

			item.SelectItem(dstItemVNum)

			if item.ITEM_TYPE_COSTUME != item.GetItemType() and not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON):
				return FALSE

			for i in xrange(player.COSTUME_ATTRIBUTE_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return TRUE

			return FALSE

		def __CanResetCostumeAttr(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return FALSE

			item.SelectItem(dstItemVNum)
			if item.ITEM_TYPE_COSTUME != item.GetItemType() and not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON):
				return FALSE

			return TRUE

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def __UseItemAuraQuestionDialog_OnAccept(self):
			self.questionDialog.Close()
			net.SendAuraRefineCheckIn(*(self.questionDialog.srcItem + self.questionDialog.dstItem + (player.GetAuraRefineWindowType(),)))

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAuraQuestionDialog_Close(self):
			self.questionDialog.Close()

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAura(self, slotIndex):
			AuraSlot = player.FineMoveAuraItemSlot()
			UsingAuraSlot = player.FindActivatedAuraSlot(player.INVENTORY, slotIndex)

			AuraVnum = player.GetItemIndex(slotIndex)
			item.SelectItem(AuraVnum)

			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return

			if player.IsEquipmentSlot(slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return

			if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem = FALSE
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
						if player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
							if UsingAuraSlot == player.AURA_SLOT_MAX:
								if AuraSlot != player.AURA_SLOT_MAIN:
									return
								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return

				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						isAbsorbItem = TRUE
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
					return

				if isAbsorbItem:
					if UsingAuraSlot == player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB) == player.NPOS():
								AuraSlot = player.AURA_SLOT_SUB
							else:
								return

						self.questionDialog = uiCommon.QuestionDialog("thin")
						self.questionDialog.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_Close))
						self.questionDialog.srcItem = (player.INVENTORY, slotIndex)
						self.questionDialog.dstItem = (player.AURA_REFINE, AuraSlot)
						self.questionDialog.Open()

			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType() == item.ITEM_TYPE_RESOURCE:
								if item.GetItemSubType() == item.RESOURCE_AURA:
									if UsingAuraSlot == player.AURA_SLOT_MAX:
										if AuraSlot != player.AURA_SLOT_SUB:
											return

										net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curLevel != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return

								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						socketLevelValue = player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
						curLevel = (socketLevelValue / 100000) - 1000
						curExp = socketLevelValue % 100000;

						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
							return

						if curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if AuraVnum != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if player.GetItemCount(slotIndex) < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return

						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
