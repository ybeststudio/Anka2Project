if __USE_DYNAMIC_MODULE__:
	import pyapi

	app = __import__(pyapi.GetModuleName("app"))
	player = __import__(pyapi.GetModuleName("player"))
else:
	import app
	import player

import ui
import item
import chat
import shop
import wndMgr
import uiCommon
import uiToolTip
import constInfo
import localeInfo
import mouseModule
import repurchaseShop

class RepurchaseShopBoard(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.__interface = None
		self.__tooltip = None
		self.__has_requested_open = False
		self.__has_requested_close = False
		self.__has_requested_repurchase = False
		self.__item_slots = None
		self.__item_buy_question_dialog = None
		self.__item_expired_check_time = 0
		self.__last_repurchase_time_ms = 0
		self.__repurchase_count = 0

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ShopRepurchaseWindow.py")
		except:
			import exception
			exception.Abort("ShopRepurchaseBoard.__LoadWindow.UIScript/ShopRepurchaseWindow.py LoadScriptFile")

		try:
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
			self.GetChild("help_button").SetOverEvent(ui.__mem_func__(self.ShowHelpButtonTooltip))
			self.GetChild("help_button").SetOverOutEvent(ui.__mem_func__(self.HideTooltip))

			item_slots = self.GetChild("repurchase_slot_list")
			item_slots.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			item_slots.SetSlotType(player.SLOT_TYPE_SHOP_REPURCHASE)
			item_slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			item_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			item_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			item_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInItemSlot))
			item_slots.SetOverOutItemEvent(ui.__mem_func__(self.HideTooltip))

			self.__item_slots = item_slots
		except:
			import exception
			exception.Abort("ShopRepurchaseBoard.__LoadWindow.UIScript/ShopRepurchaseWindow.py Load Children")

		repurchaseShop.SetInterfaceHandler(self)

	def SetItemToolTip(self, tooltip):
		import _weakref
		self.__tooltip = _weakref.proxy(tooltip)

	def BindInterface(self, interface):
		import _weakref
		self.__interface = _weakref.proxy(interface)

	def Open(self):
		if self.__interface and self.__interface.dlgShop and self.__interface.dlgShop.IsShow():
			self.SetTop()
			self.Show()
			self.Refresh()

	def Close(self):
		self.CloseRepurchaseQuestionDialog()
		self.Hide()

	def OnPressEscapeKey(self):
		self.SendShopClose()
		return True

	def OnGameOver(self):
		self.Close()

	def ShowHelpButtonTooltip(self):
		if not self.__tooltip:
			return

		self.__tooltip.ClearToolTip()
		if self.__tooltip:
			self.__tooltip.SetCannotUseItemForceSetDisableColor(True)
		self.__tooltip.SetTitle(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_TITLE)
		self.__tooltip.AppendSpace(5)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_1)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_2)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_3)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_4)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_5)
		self.__tooltip.AutoAppendTextLine(localeInfo.REPURCHASE_SHOP_HELP_TOOLTIP_DESC_6)
		self.__tooltip.OptimizeTooltipWindowSize()
		self.__tooltip.ShowToolTip()

	def HideTooltip(self):
		if self.__tooltip:
			self.__tooltip.HideToolTip()

	def SelectEmptySlot(self, slot_pos):
		pass

	def SelectItemSlot(self, slot_pos):
		if mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_LSHIFT):
			return

		if self.__has_requested_repurchase:
			return

		item_vnum = player.GetItemOriIndex(player.SHOP_REPURCHASE_WINDOW, slot_pos)
		if item_vnum == 0:
			return

		item.SelectItem(item_vnum)
		expire_time = repurchaseShop.GetItemExpireTime(slot_pos)
		if expire_time > 0 and expire_time <= app.GetGlobalTimeStamp():
			return

		item_count = player.GetItemCount(player.SHOP_REPURCHASE_WINDOW, slot_pos)
		item_price = repurchaseShop.GetItemPrice(slot_pos)
		set_value = player.GetItemSetValue(player.SHOP_REPURCHASE_WINDOW, slot_pos)
		item_name = uiToolTip.GetItemNameBySetValue(item_vnum, set_value)

		self.__item_buy_question_dialog = uiCommon.MultilineQuestionDialog()
		self.__item_buy_question_dialog.ClearText()
		self.__item_buy_question_dialog.AppendTextToList(localeInfo.REPURCHASE_SHOP_DO_YOU_BUY_ITEM_1 % item_name)
		self.__item_buy_question_dialog.AppendTextToList(localeInfo.REPURCHASE_SHOP_DO_YOU_BUY_ITEM_2 % (item_name, item_count))
		self.__item_buy_question_dialog.AppendTextToList(localeInfo.REPURCHASE_SHOP_DO_YOU_BUY_ITEM_3 % localeInfo.NumberToMoneyString(item_price))
		self.__item_buy_question_dialog.SetAcceptEvent(ui.__mem_func__(self.OnRepurchaseItem))
		self.__item_buy_question_dialog.SetCancelEvent(ui.__mem_func__(self.CloseRepurchaseQuestionDialog))
		self.__item_buy_question_dialog.pos = slot_pos
		self.__item_buy_question_dialog.vnum = item_vnum
		self.__item_buy_question_dialog.count = item_count
		self.__item_buy_question_dialog.price = item_price
		self.__item_buy_question_dialog.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def OverInItemSlot(self, slot_pos):
		if not self.__tooltip:
			return

		self.__tooltip.ClearToolTip()
		self.__tooltip.SetInventoryItem(slot_pos, player.SHOP_REPURCHASE_WINDOW)
		self.__tooltip.AppendSpace(5)
		self.__tooltip.AppendPrice(repurchaseShop.GetItemPrice(slot_pos))
		self.__tooltip.ShowToolTip()

	def SendShopOpen(self):
		if not self.__interface:
			return
		if not self.__interface.dlgShop or not self.__interface.dlgShop.IsShow():
			return
		if self.IsShow():
			return
		if self.__has_requested_open:
			return

		if repurchaseShop.SendShopOpen():
			self.__has_requested_open = True

	def ResetOpenSendFlag(self):
		self.__has_requested_open = False

	def SendShopClose(self):
		if not self.IsShow():
			return
		if self.__has_requested_close:
			return

		if repurchaseShop.SendShopClose():
			self.__has_requested_close = True

	def ResetCloseSendFlag(self):
		self.__has_requested_close = False

	def Refresh(self):
		cur_time = app.GetGlobalTimeStamp()
		self.__item_expired_check_time = 0

		for i in xrange(repurchaseShop.SLOT_MAX_COUNT):
			item_vnum = player.GetItemOriIndex(player.SHOP_REPURCHASE_WINDOW, i)
			item_count = player.GetItemCount(player.SHOP_REPURCHASE_WINDOW, i)

			if item_vnum == 0:
				self.__item_slots.SetItemSlot(i, 0, 0)
				self.__item_slots.ResetSlotCoolTime(i)
				self.__item_slots.EnableSlotCoverImage(i, False)
				continue

			item.SelectItem(item_vnum)
			self.__item_slots.SetItemSlot(i, item_vnum, item_count)
			self.__item_slots.ResetSlotCoolTime(i)

			change_look_vnum = player.GetChangeLookVnum(player.SHOP_REPURCHASE_WINDOW, i)
			if change_look_vnum:
				self.__item_slots.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				self.__item_slots.EnableSlotCoverImage(i, True)
			else:
				self.__item_slots.EnableSlotCoverImage(i, False)

			expire_time = repurchaseShop.GetItemExpireTime(i)
			if expire_time > 0:
				if self.__item_expired_check_time == 0 or expire_time < self.__item_expired_check_time:
					self.__item_expired_check_time = expire_time

			item_type = item.GetItemType()
			item_sub_type = item.GetItemSubType()

			# Real time / pet cool time handling
			if item_type == item.ITEM_TYPE_PET and item_sub_type in (item.PET_UPBRINGING, item.PET_BAG):
				sock_time = player.GetItemMetinSocket(player.SHOP_REPURCHASE_WINDOW, i, 0)
				if sock_time > cur_time:
					self.__item_slots.SetSlotCoolTimeInverse(i, sock_time - cur_time, sock_time - cur_time)
			else:
				for j in xrange(item.LIMIT_MAX_NUM):
					limit_type, limit_value = item.GetLimit(j)
					if limit_type in (item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE):
						sock_time = player.GetItemMetinSocket(player.SHOP_REPURCHASE_WINDOW, i, 0)
						if sock_time > cur_time:
							self.__item_slots.SetSlotCoolTimeInverse(i, sock_time - cur_time, limit_value)
						break

	def OnRepurchaseItem(self):
		if not self.__item_buy_question_dialog:
			return

		slot_pos = self.__item_buy_question_dialog.pos
		vnum = self.__item_buy_question_dialog.vnum
		count = self.__item_buy_question_dialog.count
		price = self.__item_buy_question_dialog.price

		self.CloseRepurchaseQuestionDialog()

		if player.GetItemOriIndex(player.SHOP_REPURCHASE_WINDOW, slot_pos) != vnum:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_INVALID_ITEM)
			return

		if player.GetItemCount(player.SHOP_REPURCHASE_WINDOW, slot_pos) != count:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_INVALID_ITEM)
			return

		item.SelectItem(vnum)
		expire_time = repurchaseShop.GetItemExpireTime(slot_pos)
		if expire_time > 0 and expire_time <= app.GetGlobalTimeStamp():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_INVALID_ITEM)
			return

		if player.GetMoney() < price:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_NOT_ENOUGH_YANG)
			return

		item_type = item.GetItemType()
		if item_type != item.ITEM_TYPE_DS:
			item_size = item.GetItemSize()
			size_y = item_size[1] if isinstance(item_size, (tuple, list)) else 1
			if player.GetEmptyCell(player.INVENTORY, size_y) == -1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_NOT_ENOUGH_INVENTORY)
				return

		cur_time_ms = app.GetGlobalTime()
		cooldown_ms = repurchaseShop.REPURCHASE_SHORT_COOLDOWN_MS if self.__repurchase_count < repurchaseShop.MAX_SHORT_COOLDOWN_REPURCHASE_COUNT else repurchaseShop.REPURCHASE_LONG_COOLDOWN_MS

		if cur_time_ms - self.__last_repurchase_time_ms < cooldown_ms:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_NOT_ENOUGH_COOLDOWN)
			return

		if repurchaseShop.SendItemRepurchase(slot_pos):
			self.__has_requested_repurchase = True
			if self.__repurchase_count < repurchaseShop.MAX_SHORT_COOLDOWN_REPURCHASE_COUNT:
				self.__repurchase_count += 1
			else:
				self.__repurchase_count = 0
			self.__last_repurchase_time_ms = cur_time_ms

	def CloseRepurchaseQuestionDialog(self):
		if self.__item_buy_question_dialog:
			self.__item_buy_question_dialog.Close()
			self.__item_buy_question_dialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def ProcessRepurchaseItemResult(self, result):
		self.__has_requested_repurchase = False

		if result == repurchaseShop.REPURCHASE_RESULT_FAIL_NOT_ENOUGH_INVENTORY:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_NOT_ENOUGH_INVENTORY)
		elif result == repurchaseShop.REPURCHASE_RESULT_FAIL_INVALID_ITEM:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_INVALID_ITEM)
		elif result == repurchaseShop.REPURCHASE_RESULT_FAIL_COOLDOWN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REPURCHASE_SHOP_NOT_ENOUGH_COOLDOWN)

	def OnUpdate(self):
		if self.__item_expired_check_time > 0 and app.GetGlobalTimeStamp() >= self.__item_expired_check_time:
			repurchaseShop.RemoveExpiredItems()
			self.Refresh()
