if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import exchange
import localeInfo
import chat
import item
import playerSettingModule
import constInfo
import time
import ui
import mouseModule
import uiPickMoney
import wndMgr
import uiCommon
import grp

FACE_IMAGE_DICT = {
	playerSettingModule.RACE_WARRIOR_M : "icon/face/warrior_m.tga",
	playerSettingModule.RACE_WARRIOR_W : "icon/face/warrior_w.tga",
	playerSettingModule.RACE_ASSASSIN_M : "icon/face/assassin_m.tga",
	playerSettingModule.RACE_ASSASSIN_W : "icon/face/assassin_w.tga",
	playerSettingModule.RACE_SURA_M : "icon/face/sura_m.tga",
	playerSettingModule.RACE_SURA_W : "icon/face/sura_w.tga",
	playerSettingModule.RACE_SHAMAN_M : "icon/face/shaman_m.tga",
	playerSettingModule.RACE_SHAMAN_W : "icon/face/shaman_w.tga",
}

if app.ENABLE_SLOT_MARKING_SYSTEM:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
	if app.ENABLE_SPECIAL_INVENTORY:
		SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_INVENTORY_PAGE_SIZE

class ExchangeDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.TitleName = 0
		self.tooltipItem = 0
		self.xStart = 0
		self.yStart = 0
		self.usedYang0 = 0
		self.usedYang1 = 0
		self.STOP = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
		self.STOP2 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
		self.interface = None

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

	class Item(ui.ListBoxEx.Item):
		def __init__(self,parent, text, value=0):
			ui.ListBoxEx.Item.__init__(self)
			self.textBox=ui.TextLine()
			self.textBox.SetParent(self)
			self.textBox.SetText(text)
			self.textBox.Show()
			self.value = value

		def GetValue(self):
			return self.value

		def __del__(self):
			ui.ListBoxEx.Item.__del__(self)

	def LoadDialog(self):
		PythonScriptLoader = ui.PythonScriptLoader()
		PythonScriptLoader.LoadScriptFile(self, "UIScript/exchangedialog.py")

		## Owner
		self.OwnerSlot = self.GetChild("Owner_Slot")
		self.OwnerSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectOwnerEmptySlot))
		self.OwnerSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectOwnerItemSlot))
		self.OwnerSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UnselectItemSlotSelf))
		self.OwnerSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInOwnerItem))
		self.OwnerSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		self.OwnerSlotBack = self.GetChild("Owner_Slot_Back")
		self.OwnerSlotBack.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)

		self.OwnerMoney = self.GetChild("Owner_Money_Value")
		self.Owner_Overlay = self.GetChild("Owner_Overlay")
		self.Owner_Overlay.Hide()
		self.OwnerMoneyButton = self.GetChild("Owner_Money")
		self.OwnerMoneyButton.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		## Target
		self.TargetSlot = self.GetChild("Target_Slot")
		self.TargetSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInTargetItem))
		self.TargetSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.TargetMoney = self.GetChild("Target_Money_Value")
		self.Target_Overlay = self.GetChild("Target_Overlay")
		self.Target_Overlay.Hide()

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
		dlgPickMoney.SetTitleName(localeInfo.EXCHANGE_MONEY)
		if app.ENABLE_GOLD_LIMIT:
			dlgPickMoney.SetMax(11)
		else:
			dlgPickMoney.SetMax(9)
		dlgPickMoney.Hide()
		self.dlgPickMoney = dlgPickMoney

		## Button
		self.TitleName = self.GetChild("TitleName")
		self.GetChild("TitleBar").SetCloseEvent(net.SendExchangeExitPacket)
		
		self.Middle_Exchange_Button = self.GetChild("Middle_Exchange_Button")
		self.Middle_Exchange_Button.SetUpVisual("d:/ymir work/ui/game/exchange/none_ready_button.tga")
		self.Middle_Exchange_Button.SetOverVisual("d:/ymir work/ui/game/exchange/none_ready_button_over.tga")
		self.Middle_Exchange_Button.SetDownVisual("d:/ymir work/ui/game/exchange/none_ready_button_down.tga")
		self.Middle_Exchange_Button.SetToggleDownEvent(ui.__mem_func__(self.AcceptExchange))
	
		self.ExchangeLogs = self.GetChild("ExchangeLogs")
		self.LogsScrollBar = ui.ThinScrollBar()
		self.LogsScrollBar.SetParent(self.ExchangeLogs)
		self.LogsScrollBar.SetPosition(442 - 65, 17)
		self.LogsScrollBar.SetScrollBarSize(70)
		self.LogsScrollBar.Show()
		self.LogsDropList = ui.ListBoxEx()
		self.LogsDropList.SetParent(self.ExchangeLogs)
		self.LogsDropList.itemHeight = 12
		self.LogsDropList.itemStep = 13
		self.LogsDropList.append_left = TRUE
		self.LogsDropList.SetPosition(10, 15)
		self.LogsDropList.SetSize(0, 150)
		self.LogsDropList.SetScrollBar(self.LogsScrollBar)
		self.LogsDropList.SetViewItemCount(5)
		self.LogsDropList.SetBasePos(0)
		self.LogsDropList.Show()

	def Destroy(self):
		self.ClearDictionary()
		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0
		self.OwnerSlot = 0
		self.OwnerMoney = 0
		self.Owner_Overlay = 0
		self.OwnerMoneyButton = 0
		self.TargetSlot = 0
		self.TargetMoney = 0
		self.Middle_Exchange_Button = 0
		self.Target_Overlay = 0
		self.TitleName = 0
		self.tooltipItem = 0
		self.LogsDropList.RemoveAllItems()
		self.LogsScrollBar = None
		self.LogsDropList = None
		self.usedYang0 = 0
		self.usedYang1 = 0

	def OpenDialog(self):
		self.TitleName.SetText(localeInfo.EXCHANGE_TITLE_LEVEL % (str(exchange.GetNameFromTarget()), exchange.GetLevelFromTarget()))
		self.Show()
		self.SetTop()

		(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

		self.GetChild("Target_Name").SetText(str(exchange.GetNameFromTarget()))
		self.GetChild("Target_Level").SetText("Lv." + str(exchange.GetLevelFromTarget()))
		self.GetChild("Owner_Name_Value").SetText(str(exchange.GetNameFromSelf()))
		self.GetChild("Owner_Level_Value").SetText("Lv." + str(exchange.GetLevelFromSelf()))

		if exchange.GetRaceFromSelf() not in FACE_IMAGE_DICT:
			self.GetChild("OwnerFaceImage").LoadImage("icon/face/poly_face.tga")
		else:
			self.GetChild("OwnerFaceImage").LoadImage(FACE_IMAGE_DICT[exchange.GetRaceFromSelf()])

		if exchange.GetRaceFromTarget() not in FACE_IMAGE_DICT:
			self.GetChild("TargetFaceImage").LoadImage("icon/face/poly_face.tga")
		else:
			self.GetChild("TargetFaceImage").LoadImage(FACE_IMAGE_DICT[exchange.GetRaceFromTarget()])

		self.LogsDropList.RemoveAllItems()
		self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_START % (str(time.strftime("[%H:%M:%S]"))), 0))

	def CloseDialog(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()

		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.dlgPickMoney.Close()
		self.Hide()
		self.usedYang0 = 0
		self.usedYang1 = 0
		self.STOP = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
		self.STOP2 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def OpenPickMoneyDialog(self):
		if exchange.GetElkFromSelf() > 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANT_EDIT_MONEY)
			return

		self.dlgPickMoney.Open(player.GetElk())

	def OnPickMoney(self, money):
		if money > 0:
			net.SendExchangeElkAddPacket(money)

	def AcceptExchange(self):
		iCountTarget = 0
		iCountSelf = 0
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromTarget(i)
			if (itemIndex != 0):
				iCountTarget = iCountTarget + 1
				
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromSelf(i)
			if (itemIndex != 0):
				iCountSelf = iCountSelf + 1
		
		if iCountTarget == 0 and iCountSelf != 0:
			acceptQuestionDialog = uiCommon.QuestionDialog("thin")
			acceptQuestionDialog.SetText(localeInfo.EXCHANGE_ALERT)
			acceptQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AcceptNegot(arg))
			acceptQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AcceptNegot(arg))
			acceptQuestionDialog.Open()
			self.acceptQuestionDialog = acceptQuestionDialog
		else:
			net.SendExchangeAcceptPacket()
			
	def AcceptNegot(self, flag):
		if (flag):
			net.SendExchangeAcceptPacket()
			
		self.acceptQuestionDialog = None
		self.Middle_Exchange_Button.SetUpVisual("d:/ymir work/ui/game/exchange/none_ready_button.tga")
		self.Middle_Exchange_Button.SetOverVisual("d:/ymir work/ui/game/exchange/none_ready_button_over.tga")
		self.Middle_Exchange_Button.SetDownVisual("d:/ymir work/ui/game/exchange/none_ready_button_down.tga")
		self.Middle_Exchange_Button.Enable()

	def SelectOwnerEmptySlot(self, SlotIndex):
		if FALSE == mouseModule.mouseController.isAttached():
			return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_OPEN_OTHER_WINDOW)
				return

		if mouseModule.mouseController.IsAttachedMoney():
			net.SendExchangeElkAddPacket(mouseModule.mouseController.GetAttachedMoneyAmount())
		else:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			if (player.SLOT_TYPE_INVENTORY == attachedSlotType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType):
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

				SrcSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
				DstSlotNumber = SlotIndex
				itemID = player.GetItemIndex(attachedInvenType, SrcSlotNumber)
				item.SelectItem(itemID)

				if item.IsAntiFlag(item.ANTIFLAG_GIVE):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANNOT_GIVE)
					mouseModule.mouseController.DeattachObject()
					return

				net.SendExchangeItemAddPacket(attachedInvenType, SrcSlotNumber, DstSlotNumber)

				if app.ENABLE_SPECIAL_INVENTORY:
					self.ItemListIdx.append(SrcSlotNumber)

		mouseModule.mouseController.DeattachObject()

	def SelectOwnerItemSlot(self, SlotIndex):
		if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
			money = mouseModule.mouseController.GetAttachedItemCount()
			net.SendExchangeElkAddPacket(money)

	def UnselectItemSlotSelf(self, slotIndex):
		net.SendExchangeItemDelPacket(slotIndex)

	def RefreshOwnerSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromSelf(i)
			itemCount = exchange.GetItemCountFromSelf(i)
			if 1 == itemCount:
				itemCount = 0

			self.OwnerSlot.SetItemSlot(i, itemIndex, itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = exchange.GetChangeLookVnumFromSelf(i)
				if not changelookvnum == 0:
					self.OwnerSlot.SetSlotCoverImage(i,"icon/item/ingame_convert_Mark.tga")
				else:
					self.OwnerSlot.EnableSlotCoverImage(i, FALSE)

			if itemIndex != 0 and self.STOP[i] == 0:
				item.SelectItem(exchange.GetItemVnumFromSelf(i))
				itemName = item.GetItemName(exchange.GetItemVnumFromSelf(i))
				attrSlot = []
				for j in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attrSlot.append(exchange.GetItemAttributeFromSelf(i, j))
				if attrSlot[0][0] != 0:
					if itemCount != 0:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_ADD_COUNT_ATT % (str(time.strftime("[%H:%M:%S]")), str(itemCount), itemName), 0))
					else:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_ADD_ATT % (str(time.strftime("[%H:%M:%S]")), itemName), 0))

				else:
					if itemCount != 0:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_ADD_COUNT % (str(time.strftime("[%H:%M:%S]")), str(itemCount), itemName), 0))
					else:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_ADD % (str(time.strftime("[%H:%M:%S]")), itemName), 0))
				self.STOP[i] = 1

		self.RefreshOwnerBonusedItems()
		self.OwnerSlot.RefreshSlot()

	def RefreshOwnerBonusedItems(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromSelf(i)
			if itemIndex != 0:
				item.SelectItem(exchange.GetItemVnumFromSelf(i))
				itemSize = item.GetItemSize()[1]
				attrSlot = []
				for j in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attrSlot.append(exchange.GetItemAttributeFromSelf(i, j))
				if attrSlot[4][0] != 0 and item.GetItemName()[-2:] == "+9":
					if itemSize == 1:
						self.OwnerSlot.HideSlotBaseImage(i)
					elif itemSize == 2:
						self.OwnerSlot.HideSlotBaseImage(i)
						self.OwnerSlot.HideSlotBaseImage(i+6)	
					elif itemSize == 3:
						self.OwnerSlot.HideSlotBaseImage(i)
						self.OwnerSlot.HideSlotBaseImage(i+6)
						self.OwnerSlot.HideSlotBaseImage(i+12)

	def RefreshTargetSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromTarget(i)
			itemCount = exchange.GetItemCountFromTarget(i)
			if 1 == itemCount:
				itemCount = 0

			self.TargetSlot.SetItemSlot(i, itemIndex, itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = exchange.GetChangeLookVnumFromTarget(i)
				if not changelookvnum == 0:
					self.TargetSlot.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.TargetSlot.EnableSlotCoverImage(i, FALSE)

			if itemIndex != 0 and self.STOP2[i] == 0:
				item.SelectItem(exchange.GetItemVnumFromTarget(i))
				itemName = item.GetItemName(exchange.GetItemVnumFromTarget(i))
				attrSlot = []
				for j in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attrSlot.append(exchange.GetItemAttributeFromTarget(i, j))
				if attrSlot[0][0] != 0:
					if itemCount != 0:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_ADD_COUNT_ATT % (str(time.strftime("[%H:%M:%S]")),exchange.GetNameFromTarget(), str(itemCount), itemName), 0))
					else:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_ADD_ATT % (str(time.strftime("[%H:%M:%S]")),exchange.GetNameFromTarget(), itemName), 0))
				else:
					if itemCount != 0:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_ADD_COUNT % (str(time.strftime("[%H:%M:%S]")),exchange.GetNameFromTarget(), str(itemCount), itemName), 0))
					else:
						self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_ADD % (str(time.strftime("[%H:%M:%S]")),exchange.GetNameFromTarget(), itemName), 0))
				self.STOP2[i] = 1
		self.RefreshTargetBonusedItems()	
		self.TargetSlot.RefreshSlot()
		
	def RefreshTargetBonusedItems(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromTarget(i)
			if itemIndex != 0:
				item.SelectItem(exchange.GetItemVnumFromTarget(i))
				itemSize = item.GetItemSize()[1]
				attrSlot = []
				for j in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
					attrSlot.append(exchange.GetItemAttributeFromTarget(i, j))
				if attrSlot[4][0] != 0 and item.GetItemName()[-2:] == "+9":
					if itemSize == 1:
						self.TargetSlot.HideSlotBaseImage(i)
					elif itemSize == 2:
						self.TargetSlot.HideSlotBaseImage(i)
						self.TargetSlot.HideSlotBaseImage(i+6)	
					elif itemSize == 3:
						self.TargetSlot.HideSlotBaseImage(i)
						self.TargetSlot.HideSlotBaseImage(i+6)
						self.TargetSlot.HideSlotBaseImage(i+12)

	def Refresh(self):

		self.RefreshOwnerSlot()
		self.RefreshTargetSlot()

		self.OwnerMoney.SetText(str(localeInfo.NumberToDecimalString(exchange.GetElkFromSelf())))
		self.TargetMoney.SetText(str(localeInfo.NumberToDecimalString(exchange.GetElkFromTarget())))

		if TRUE == exchange.GetAcceptFromSelf():
			self.Middle_Exchange_Button.SetUpVisual("d:/ymir work/ui/game/exchange/player_ready_button.tga")
			self.Middle_Exchange_Button.SetOverVisual("d:/ymir work/ui/game/exchange/player_ready_button_over.tga")
			self.Middle_Exchange_Button.SetDownVisual("d:/ymir work/ui/game/exchange/player_ready_button_down.tga")
			self.Middle_Exchange_Button.Disable()
			self.Owner_Overlay.Show()
			self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_ACCEPT % (str((time.strftime("[%H:%M:%S]")))), 0))
		elif TRUE == exchange.GetAcceptFromTarget():
			self.Target_Overlay.Show()
			self.Middle_Exchange_Button.SetUpVisual("d:/ymir work/ui/game/exchange/target_ready_button.tga")
			self.Middle_Exchange_Button.SetOverVisual("d:/ymir work/ui/game/exchange/target_ready_button_over.tga")
			self.Middle_Exchange_Button.SetDownVisual("d:/ymir work/ui/game/exchange/target_ready_button_down.tga")	
			self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_ACCEPT % (str((time.strftime("[%H:%M:%S]"))), exchange.GetNameFromTarget()), 0))
		else:
			self.Target_Overlay.Hide()
			self.Middle_Exchange_Button.SetUpVisual("d:/ymir work/ui/game/exchange/none_ready_button.tga")
			self.Middle_Exchange_Button.SetOverVisual("d:/ymir work/ui/game/exchange/none_ready_button_over.tga")
			self.Middle_Exchange_Button.SetDownVisual("d:/ymir work/ui/game/exchange/none_ready_button_down.tga")
			self.Middle_Exchange_Button.Enable()
			self.Owner_Overlay.Hide()

		if exchange.GetElkFromSelf() != 0 and self.usedYang0 == 0:
			self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_OWNER_YANG % (str(time.strftime("[%H:%M:%S]")), localeInfo.NumberToDecimalString(exchange.GetElkFromSelf())), 0))
			self.usedYang0 = 1
		elif exchange.GetElkFromTarget() != 0 and self.usedYang1 == 0:
			self.LogsDropList.AppendItem(self.Item(self, localeInfo.EXCHANGE_TARGET_YANG % (str(time.strftime("[%H:%M:%S]")), str(exchange.GetNameFromTarget()), localeInfo.NumberToDecimalString(exchange.GetElkFromTarget())), 0))
			self.usedYang1 = 1

	def OverInOwnerItem(self, slotIndex):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeOwnerItem(slotIndex)

	def OverInTargetItem(self, slotIndex):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeTargetItem(slotIndex)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if not app.ENABLE_SLOT_MARKING_SYSTEM:
		def OnTop(self):
			self.tooltipItem.SetTop()

	def OnUpdate(self):
		USE_EXCHANGE_LIMIT_RANGE = 1000
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xStart) > USE_EXCHANGE_LIMIT_RANGE or abs(y - self.yStart) > USE_EXCHANGE_LIMIT_RANGE:
			(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()
			net.SendExchangeExitPacket()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if app.ENABLE_SPECIAL_INVENTORY:
				if not self.inven or not self.invenSpecial:
					return
			else:
				if not self.inven:
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

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.LogsScrollBar.OnUp()
		else:
			self.LogsScrollBar.OnDown()

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE)

			return FALSE

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_EXCHANGE)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, inven):
			self.inven = inven

		if app.ENABLE_SPECIAL_INVENTORY:
			def SetSpecialInven(self, invenSpecial):
				self.invenSpecial = invenSpecial

		def AddExchangeItemSlotIndex(self, idx):
			self.ItemListIdx.append(idx)
