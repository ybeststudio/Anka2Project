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
import uiPrivateShopBuilder
import localeInfo
import constInfo
import ime
import wndMgr
import uiToolTip

class ChangeLookWindow(ui.ScriptWindow):

	USE_CHANGELOOKWINDOW_LIMIT_RANGE = 500
	CHANGELOOK_SLOT_LEFT = 0
	CHANGELOOK_SLOT_RIGHT = 1
	CHANGELOOK_SLOT_MAX = 2
	CHANGELOOK_COST = 50000000

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isloded = 0
		self.tooltipitem = None
		self.xChangeLookWindowStart = 0
		self.yChangeLookWindowStart = 0
		self.ChangeLookToolTIpButton = None
		self.ChangeLookToolTip = None
		self.wnditem = None
		self.pop = None
		self.ChangeLookCost = None
		self.wndpassitem = None
		self.Title = None

		self.ChangeLookToolTipList = [
			localeInfo.CHANGE_TOOLTIP_LINE1, 
			localeInfo.CHANGE_TOOLTIP_LINE2,
			localeInfo.CHANGE_TOOLTIP_LINE3,
			localeInfo.CHANGE_TOOLTIP_LINE4,
			localeInfo.CHANGE_TOOLTIP_LINE5,
			localeInfo.CHANGE_TOOLTIP_LINE6
		]

		self.ChangeLookMountToolTipList = [
			localeInfo.CHANGE_TOOLTIP2_LINE1, 
			localeInfo.CHANGE_TOOLTIP2_LINE2,
			localeInfo.CHANGE_TOOLTIP2_LINE3,
			localeInfo.CHANGE_TOOLTIP2_LINE4,
			localeInfo.CHANGE_TOOLTIP2_LINE5
		]

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		player.SetChangeLookWindow(FALSE)
		self.isloded = 0
		self.tooltipitem = None
		self.pop = None
		self.ChangeLookToolTIpButton = None
		self.ChangeLookToolTip = None
		self.wnditem = None
		self.ChangeLookCost = None
		self.wndpassitem = None
		self.Title = None

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ChangeLookWindow.py")
		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.UIScript/ChangeLookWindow.py")
		try:
			wnditem = self.GetChild("ChangeLookSlot")
			wndpassitem = self.GetChild("ChangeLookSlot_PassYangItem")
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			self.ChangeLookCost = self.GetChild("Cost")
			self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (self.NumberToMoneyString(self.CHANGELOOK_COST)))

			self.Title = self.GetChild("TitleName")
			self.Title.SetText(localeInfo.CHANGE_LOOK_TITLE)

		except:
			import exception
			exception.Abort("ChangeLookWindow.__LoadWindow.ChangeLookSlot")

		wnditem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wnditem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wnditem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wnditem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wnditem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wnditem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wnditem.Show()

		wndpassitem.SetOverInItemEvent(ui.__mem_func__(self.OverInItemFreeYang))
		wndpassitem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndpassitem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlotFreepass))
		wndpassitem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlotFreepass))
		wndpassitem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlotFreepass))
		wndpassitem.Show()
		self.wndpassitem = wndpassitem

		self.wnditem = wnditem

		self.ChangeLookToolTIpButton = self.GetChild("ChangeLookToolTIpButton")

	def NumberToMoneyString(self, n) :
		if n <= 0 :
			return "0"

		return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uiToolTip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(7)

		for desc in descList:
			toolTip.AutoAppendTextLine(desc)

		toolTip.AlignHorizonalCenter()
		toolTip.SetTop()
		return toolTip

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xChangeLookWindowStart) > self.USE_CHANGELOOKWINDOW_LIMIT_RANGE or abs(y - self.yChangeLookWindowStart) > self.USE_CHANGELOOKWINDOW_LIMIT_RANGE:
			self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def Open(self, type):
		if self.isloded == 0:
			self.isloded = 1
			self.__LoadWindow()

		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)
		(self.xChangeLookWindowStart, self.yChangeLookWindowStart, z) = player.GetMainCharacterPosition()
		player.SetChangeLookWindow(TRUE)
		player.SetChangeLookWindowType(int(type))

		self.ChangeLookToolTip = None

		if int(type) == player.TRANSMUTATION_TYPE_MOUNT:
			self.Title.SetText(localeInfo.CHANGE_LOOK_TITLE2)
			self.ChangeLookToolTip = self.__CreateGameTypeToolTip(localeInfo.CHANGE_TOOLTIP_TITLE, self.ChangeLookMountToolTipList)
		else:
			self.Title.SetText(localeInfo.CHANGE_LOOK_TITLE)
			self.ChangeLookToolTip = self.__CreateGameTypeToolTip(localeInfo.CHANGE_TOOLTIP_TITLE, self.ChangeLookToolTipList)

		self.ChangeLookToolTip.SetTop()
		self.ChangeLookToolTIpButton.SetToolTipWindow(self.ChangeLookToolTip)

		self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (self.NumberToMoneyString(player.GetChangeChangeLookPrice())))
		self.RefreshChangeLookWindow()

	def Close(self):
		self.Hide()
		net.SendChangeLookCanCle()
		player.SetChangeLookWindow(FALSE)

	def Accept(self):
		leftvnum = 	player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT)
		rightvnum = player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT)
		if  leftvnum == 0 or rightvnum == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_INSERT_ITEM)
		else:
			popup = uiCommon.QuestionDialog("thin")
			popup.SetText(localeInfo.CHANGE_LOOK_CHANGE_ITEM)
			popup.SetAcceptEvent(self.SendAccept)
			popup.SetCancelEvent(self.OnCloseEvent)
			popup.Open()
			self.pop = popup

	def SendAccept(self):
		self.pop.Close()
		self.pop = None
		net.SendChangeLookAccept()

	def SetItemToolTip(self, tooltip):
		self.tooltipitem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.tooltipitem:
			self.tooltipitem.SetChangeLookWindowItem(slotIndex)

	def OverInItem(self, slotIndex):
		self.wnditem.SetUsableItem(FALSE)
		self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		self.wnditem.SetUsableItem(FALSE)
		if self.tooltipitem:
			self.tooltipitem.HideToolTip()

	def OverInItemFreeYang(self, slotIndex):
		self.wnditem.SetUsableItem(FALSE)
		self.__ShowToolTip_FreeItem(player.GetChangeLookFreeYangInvenSlotPos())	

	def __ShowToolTip_FreeItem(self, slotIndex):
		if self.tooltipitem:
			self.tooltipitem.SetInventoryItem(slotIndex, player.INVENTORY)

	def UseItemSlotFreepass(self, slotIndex):
		mouseModule.mouseController.DeattachObject()
		net.SendChangeLookCheckOutFreeYangItem()
		self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (self.NumberToMoneyString(player.GetChangeChangeLookPrice())))

	def SelectEmptySlotFreepass(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			ItemVNum = player.GetItemIndex(attachedSlotPos)

			if item.IsChangeLookFreePassYangItem(ItemVNum) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_REGISTER_ITEM)
				return

			net.SendChangeLookCheckInFreeYangItem(attachedInvenType, attachedSlotPos)
			mouseModule.mouseController.DeattachObject()
			self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (self.NumberToMoneyString(0)))

	def SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			ItemVNum = player.GetItemIndex(attachedSlotPos)

			if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
				if item.IsPossibleChangeLookLeft(ItemVNum) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_CHANGE_LOOK_ITEM)
					return
			else:
				if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_INSERT_CHANGE_LOOK_ITEM)
					return
				if item.IsPossibleChangeLookRight(player.GetChangeLookItemID(self.CHANGELOOK_SLOT_LEFT), ItemVNum) == 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_REGISTER_ITEM)
					return

			item.SelectItem(ItemVNum)

			window = player.SlotTypeToInvenType(attachedSlotType)
			if window == player.EQUIPMENT:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_DO_NOT_EQUIP_ITEM)
				return

			if player.SLOT_TYPE_CHANGE_LOOK != attachedSlotType:
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.RESERVED_WINDOW == attachedInvenType:
					return

				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					net.SendChangeLookCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos)
				else:
					if player.GetChangeLookItemInvenSlot(self.CHANGELOOK_SLOT_LEFT) == attachedSlotPos:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_ALREADY_REGISTER)
						return

					net.SendChangeLookCheckIn(attachedInvenType, attachedSlotPos, selectedSlotPos)
					popup = uiCommon.PopupDialog()
					popup.SetText(localeInfo.CHANGE_LOOK_DEL_ITEM)
					popup.SetAcceptEvent(self.__OnClosePopupDialog)
					popup.Open()
					self.pop = popup

			mouseModule.mouseController.DeattachObject()

	def UseItemSlot(self, slotIndex):
		if slotIndex == self.CHANGELOOK_SLOT_LEFT:
			if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT) == 0:
				mouseModule.mouseController.DeattachObject()
				net.SendChangeLookCheckOut(slotIndex)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
		else:
			mouseModule.mouseController.DeattachObject()
			net.SendChangeLookCheckOut(slotIndex)

	def SelectItemSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				snd.PlaySound("sound/ui/drop.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			else:
				if selectedSlotPos == self.CHANGELOOK_SLOT_LEFT:
					if player.GetChangeLookItemID(self.CHANGELOOK_SLOT_RIGHT) == 0:
						selectedItemID = player.GetChangeLookItemID(selectedSlotPos)
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CHANGE_LOOK, selectedSlotPos, selectedItemID)
						snd.PlaySound("sound/ui/pick.wav")
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHANGE_LOOK_CHECK_OUT_REGISTER_ITEM)
				else:
					selectedItemID = player.GetChangeLookItemID(selectedSlotPos)
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CHANGE_LOOK, selectedSlotPos, selectedItemID)
					snd.PlaySound("sound/ui/pick.wav")

	def RefreshChangeLookWindow(self):
		if not player.GetChangeLookWindowOpen():
			return

		if not self.wnditem:
			return

		setChangeLookItem = self.wnditem.SetItemSlot
		getChangeLookItem = player.GetChangeLookItemID

		for i in xrange(self.CHANGELOOK_SLOT_MAX):
			ChangeLookSlotVnum = getChangeLookItem(i)
			if not ChangeLookSlotVnum == player.ITEM_SLOT_COUNT:
				setChangeLookItem(i, ChangeLookSlotVnum, 1)
			else:
				setChangeLookItem(i, 0, 1)

			changelookvnum = player.GetChangeWIndowChangeLookVnum(i)
			if not changelookvnum == 0:
				self.wnditem.SetSlotCoverImage(i,"icon/item/ingame_convert_Mark.tga")
			else:
				self.wnditem.EnableSlotCoverImage(i, FALSE)

		self.wnditem.RefreshSlot()

		ChangeLookFreeItemVnum = player.GetChangeLookFreeYangItemID()

		if not ChangeLookFreeItemVnum == 0:
			self.wndpassitem.SetItemSlot(0, ChangeLookFreeItemVnum, 1)
		else:
			self.ChangeLookCost.SetText(localeInfo.CHANGE_LOOK_COST % (self.NumberToMoneyString(player.GetChangeChangeLookPrice())))
			self.wndpassitem.SetItemSlot(0, 0, 1)

	def OnCloseEvent(self):
		self.pop.Close()
		self.pop = None

	def __OnClosePopupDialog(self):
		self.popup = None