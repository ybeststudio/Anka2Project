if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import mouseModule
import snd
import item
import safebox
import chat
import localeInfo
import uiScriptLocale

class PasswordDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()

		self.sendMessage = "/safebox_password "
		self.send_type = 0
		self.__interface = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/passworddialog.py")
		except:
			import exception
			exception.Abort("PasswordDialog.__LoadDialog.LoadObject")

		try:
			self.passwordValue = self.GetChild("password_value")
			self.acceptButton = self.GetChild("accept_button")
			self.cancelButton = self.GetChild("cancel_button")
			self.titleName = self.GetChild("TitleName")
			self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.CloseDialog))
		except:
			import exception
			exception.Abort("PasswordDialog.__LoadDialog.BindObject")

		self.passwordValue.OnIMEReturn = self.OnAccept
		self.passwordValue.OnPressEscapeKey = self.OnCancel
		self.acceptButton.SetEvent(ui.__mem_func__(self.OnAccept))
		self.cancelButton.SetEvent(ui.__mem_func__(self.OnCancel))

	def Destroy(self):
		self.ClearDictionary()
		self.passwordValue = None
		self.acceptButton = None
		self.cancelButton = None
		self.titleName = None

	def SetTitle(self, title):
		self.titleName.SetText(title)

	def SetSendMessage(self, msg):
		self.sendMessage = msg

	def SetSendType(self, type):
		self.send_type = type
		if app.ENABLE_UN_SEAL_SCROLL_PLUS:
			if net.UN_SEAL_SCROLL_PLUS_PASSWORD_INVENTORY == type or net.UN_SEAL_SCROLL_PLUS_PASSWORD_DS_INVENTORY == type:
				self.SetTitle(localeInfo.UN_SEAL_SCROLL_PLUS_PASSWORD_TITLE)
				try:
					self.GetChild("Desc1").SetText(localeInfo.UN_SEAL_SCROLL_PLUS_PASSWORD_DESC1)
					self.GetChild("Desc2").SetText(localeInfo.UN_SEAL_SCROLL_PLUS_PASSWORD_DESC2)
					self.GetChild("Desc3").Hide()
					self.GetChild("Desc4").Hide()
					self.GetChild("Desc5").Hide()
				except:
					pass

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.__interface = proxy(interface)

	def ShowDialog(self):
		if app.ENABLE_GROWTH_PET_SYSTEM:
			if player.IsOpenPetHatchingWindow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_HATCHING_WINDOW_OPEN_CAN_NOT_USE)
				return FALSE

		self.passwordValue.SetText("")
		self.passwordValue.SetFocus()
		self.SetCenterPosition()
		self.Show()

	def CloseDialog(self):
		self.passwordValue.KillFocus()
		self.Hide()

	def OnAccept(self):
		if app.ENABLE_UN_SEAL_SCROLL_PLUS:
			if self.send_type in (net.UN_SEAL_SCROLL_PLUS_PASSWORD_INVENTORY, net.UN_SEAL_SCROLL_PLUS_PASSWORD_DS_INVENTORY):
				if self.__interface and not self.__interface.CheckOpenShopWhenUseUnSealScroll():
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.UN_SEAL_SCROLL_CLOSE_UI)
					self.CloseDialog()
					return True
				player.SendPasswordType(self.send_type, self.passwordValue.GetText())
				self.CloseDialog()
				return True

		net.SendChatPacket(self.sendMessage + self.passwordValue.GetText())
		self.CloseDialog()
		return TRUE

	def OnCancel(self):
		self.CloseDialog()
		return TRUE

class ChangePasswordDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		self.dlgMessage = ui.ScriptWindow()
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgMessage, "uiscript/popupdialog.py")
			self.dlgMessage.GetChild("message").SetText(localeInfo.SAFEBOX_WRONG_PASSWORD)
			self.dlgMessage.GetChild("accept").SetEvent(ui.__mem_func__(self.OnCloseMessageDialog))
		except:
			import exception
			exception.Abort("SafeboxWindow.__LoadDialog.LoadObject")

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/changepassworddialog.py")

		except:
			import exception
			exception.Abort("ChangePasswordDialog.LoadDialog.LoadObject")

		try:
			self.GetChild("accept_button").SetEvent(ui.__mem_func__(self.OnAccept))
			self.GetChild("cancel_button").SetEvent(ui.__mem_func__(self.OnCancel))
			self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.OnCancel))
			oldPassword = self.GetChild("old_password_value")
			newPassword = self.GetChild("new_password_value")
			newPasswordCheck = self.GetChild("new_password_check_value")
		except:
			import exception
			exception.Abort("ChangePasswordDialog.LoadDialog.BindObject")

		oldPassword.SetTabEvent(lambda arg=1: self.OnNextFocus(arg))
		newPassword.SetTabEvent(lambda arg=2: self.OnNextFocus(arg))
		newPasswordCheck.SetTabEvent(lambda arg=3: self.OnNextFocus(arg))
		oldPassword.SetReturnEvent(lambda arg=1: self.OnNextFocus(arg))
		newPassword.SetReturnEvent(lambda arg=2: self.OnNextFocus(arg))
		newPasswordCheck.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		oldPassword.OnPressEscapeKey = self.OnCancel
		newPassword.OnPressEscapeKey = self.OnCancel
		newPasswordCheck.OnPressEscapeKey = self.OnCancel

		self.oldPassword = oldPassword
		self.newPassword = newPassword
		self.newPasswordCheck = newPasswordCheck

	def OnNextFocus(self, arg):
		if 1 == arg:
			self.oldPassword.KillFocus()
			self.newPassword.SetFocus()
		elif 2 == arg:
			self.newPassword.KillFocus()
			self.newPasswordCheck.SetFocus()
		elif 3 == arg:
			self.newPasswordCheck.KillFocus()
			self.oldPassword.SetFocus()

	def Destroy(self):
		self.ClearDictionary()
		self.dlgMessage.ClearDictionary()
		self.oldPassword = None
		self.newPassword = None
		self.newPasswordCheck = None

	def Open(self):
		self.oldPassword.SetText("")
		self.newPassword.SetText("")
		self.newPasswordCheck.SetText("")
		self.oldPassword.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.oldPassword.SetText("")
		self.newPassword.SetText("")
		self.newPasswordCheck.SetText("")
		self.oldPassword.KillFocus()
		self.newPassword.KillFocus()
		self.newPasswordCheck.KillFocus()
		self.Hide()

	def OnAccept(self):
		oldPasswordText = self.oldPassword.GetText()
		newPasswordText = self.newPassword.GetText()
		newPasswordCheckText = self.newPasswordCheck.GetText()
		if newPasswordText != newPasswordCheckText:
			self.dlgMessage.SetCenterPosition()
			self.dlgMessage.SetTop()
			self.dlgMessage.Show()
			return TRUE
		net.SendChatPacket("/safebox_change_password %s %s" % (oldPasswordText, newPasswordText))
		self.Close()
		return TRUE

	def OnCancel(self):
		self.Close()
		return TRUE

	def OnCloseMessageDialog(self):
		self.newPassword.SetText("")
		self.newPasswordCheck.SetText("")
		self.newPassword.SetFocus()
		self.dlgMessage.Hide()

if app.ENABLE_SPECIAL_INVENTORY:
	class SafeboxWindow(ui.ScriptWindow):
		PAGE_SIZE = (160, 333)
		PAGE_START_POS = (20, 43)

		if app.ENABLE_EXTENDED_SAFEBOX:
			BUTTON_PER_LINE = 6

		def __init__(self, wndSpecialInv = None):
			def __LoadPage():
				self.SetSize(*self.PAGE_SIZE)
				self.SetParent(self.wndSpecialInv)
				self.SetPosition(*self.PAGE_START_POS)

			if not wndSpecialInv:
				print("cant load wndSpecialInv (safeboxwindow)")
				return

			ui.ScriptWindow.__init__(self)

			self.tooltipItem = None
			self.sellingSlotNumber = -1
			self.pageButtonList = []
			self.curPageIndex = 0
			self.isLoaded = 0
			self.pageCount = 0
			self.interface = None
			self.wndSpecialInv = wndSpecialInv

			__LoadPage()
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			self.__LoadWindow()

			ui.ScriptWindow.Show(self)

		def Destroy(self):
			self.ClearDictionary()

			self.tooltipItem = None
			self.wndItem = None

			self.pageButtonList = []

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			from _weakref import proxy

			wndItem = ui.GridSlotWindow()
			wndItem.SetParent(self)
			wndItem.SetPosition(0, 0)
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.Show()

			self.wndItem = wndItem

			self.SetTableSize(3)
			self.RefreshSafeboxMoney()

		def OpenPickMoneyDialog(self):
			if mouseModule.mouseController.isAttached():
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

				if player.SLOT_TYPE_INVENTORY == mouseModule.mouseController.GetAttachedType():
					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

				mouseModule.mouseController.DeattachObject()
			else:
				curMoney = safebox.GetMoney()

				if curMoney <= 0:
					return

				self.dlgPickMoney.Open(curMoney)

		def ShowWindow(self, size):
			self.SetTableSize(size)
			self.SetTop()
			self.Show()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenSafeBox(TRUE)

		def __MakePageButton(self, pageCount):
			self.curPageIndex = 0
			self.pageButtonList = []
			self.pageCount = pageCount

			if app.ENABLE_EXTENDED_SAFEBOX:
				page_names = ["I", "II", "III", "IV", "V", "VI"]
				width = 25
				pos = []
			else:
				text = "I"
				pos = -int(float(pageCount-1)/2 * 52)

			for i in xrange(pageCount):
				if app.ENABLE_EXTENDED_SAFEBOX:
					if i > len(page_names) - 1:
						page_names.append("X")
					if not i % self.BUTTON_PER_LINE:
						pos.append(-int((max(1, min(self.BUTTON_PER_LINE, pageCount))-1) * (width / 2)))
				button = ui.RadioButton()
				button.SetParent(self)
				button.SetUpVisual("d:/ymir work/ui/game/pages/buttons/but_pages_norm.tga")
				button.SetOverVisual("d:/ymir work/ui/game/pages/buttons/but_pages_hover.tga")
				button.SetDownVisual("d:/ymir work/ui/game/pages/buttons/but_pages_down.tga")
				button.SetWindowHorizontalAlignCenter()
				if app.ENABLE_EXTENDED_SAFEBOX:
					button.SetPosition(pos[len(pos) - 1], (290 + ((pageCount-self.BUTTON_PER_LINE) + 1 % 3) * 4) - (len(pos) - 1) * 20)
					pos[len(pos) - 1] += width
					button.SetText(page_names[i])
				else:
					button.SetPosition(pos, 298)
					button.SetText(text)
				button.SetEvent(lambda arg=i: self.SelectPage(arg))
				button.Show()
				self.pageButtonList.append(button)

				if not app.ENABLE_EXTENDED_SAFEBOX:
					pos += 52
					text += "I"

			self.pageButtonList[0].Down()

		def SelectPage(self, index):
			self.curPageIndex = index

			for btn in self.pageButtonList:
				btn.SetUp()

			self.pageButtonList[index].Down()
			self.RefreshSafebox()

		def __LocalPosToGlobalPos(self, local):
			return self.curPageIndex*safebox.SAFEBOX_PAGE_SIZE + local

		def SetTableSize(self, size):
			pageCount = max(1, size / safebox.SAFEBOX_SLOT_Y_COUNT)

			if not app.ENABLE_EXTENDED_SAFEBOX:
				pageCount = min(3, pageCount)

			size = safebox.SAFEBOX_SLOT_Y_COUNT

			self.__MakePageButton(pageCount)

			self.wndItem.ArrangeSlot(0, safebox.SAFEBOX_SLOT_X_COUNT, size, 32, 32, 0, 0)
			self.wndItem.RefreshSlot()
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)

			self.SetSize(*self.PAGE_SIZE)
			self.UpdateRect()

		def RefreshSafebox(self):
			getItemID=safebox.GetItemID
			getItemCount=safebox.GetItemCount
			setItemID=self.wndItem.SetItemSlot

			for i in xrange(safebox.SAFEBOX_PAGE_SIZE):
				slotIndex = self.__LocalPosToGlobalPos(i)
				itemCount = getItemCount(slotIndex)
				if itemCount <= 1:
					itemCount = 0
				setItemID(i, getItemID(slotIndex), itemCount)

				if app.ENABLE_GROWTH_PET_SYSTEM:
					self.__SetCoolTimePetItemSlot( slotIndex, getItemID(slotIndex))

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookitemvnum = safebox.GetItemChangeLookVnum(slotIndex)
					if not changelookitemvnum == 0:
						self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndItem.EnableSlotCoverImage(i, FALSE)

			self.wndItem.RefreshSlot()

		def RefreshSafeboxMoney(self):
			pass

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def CommandCloseSafebox(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

			if app.ENABLE_SLOT_MARKING_SYSTEM:
				if self.interface:
					self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
					self.interface.RefreshMarkInventoryBag()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenSafeBox(FALSE)

			self.Hide()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			def __SetCoolTimePetItemSlot(self, slotNumber, itemVnum):
				if 0 == itemVnum:
					return

				item.SelectItem(itemVnum)
				itemSubType = item.GetItemSubType()

				if itemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
					return

				if itemSubType == item.PET_BAG:
					id = safebox.GetItemMetinSocket(slotNumber, 2)
					if id == 0:
						return

				(limitType, limitValue) = item.GetLimit(0)

				if itemSubType == item.PET_UPBRINGING:
					limitValue = safebox.GetItemMetinSocket(slotNumber, 1)

				if limitType in [item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE]:

					sock_time = safebox.GetItemMetinSocket(slotNumber, 0)
					remain_time = max( 0, sock_time - app.GetGlobalTimeStamp() )

					if slotNumber >= safebox.SAFEBOX_PAGE_SIZE:
						slotNumber -= (self.curPageIndex * safebox.SAFEBOX_PAGE_SIZE)

					self.wndItem.SetSlotCoolTimeInverse(slotNumber, limitValue, limitValue - remain_time)

		def SelectEmptySlot(self, selectedSlotPos):
			selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

				if player.SLOT_TYPE_SAFEBOX == attachedSlotType:
					net.SendSafeboxItemMovePacket(attachedSlotPos, selectedSlotPos, 0)
				else:
					attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
					if player.RESERVED_WINDOW == attachedInvenType:
						return

					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")
					else:
						net.SendSafeboxCheckinPacket(attachedInvenType, attachedSlotPos, selectedSlotPos)

				mouseModule.mouseController.DeattachObject()

		def SelectItemSlot(self, selectedSlotPos):
			selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()

				if app.ENABLE_SPECIAL_INVENTORY:
					if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
						player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
						player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
						player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
						player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
						player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType:

						if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
							net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
							snd.PlaySound("sound/ui/money.wav")
						else:
							attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				else:
					if player.SLOT_TYPE_INVENTORY == attachedSlotType:
						if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
							net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
							snd.PlaySound("sound/ui/money.wav")
						else:
							attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

				mouseModule.mouseController.DeattachObject()
			else:
				curCursorNum = app.GetCursor()
				if app.SELL == curCursorNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)

				elif app.BUY == curCursorNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

				else:
					selectedItemID = safebox.GetItemID(selectedSlotPos)
					if app.ENABLE_RENDER_COUNTLINE_ATTACHED_MOUSE:
						itemCount = safebox.GetItemCount(selectedSlotPos)
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SAFEBOX, selectedSlotPos, selectedItemID, itemCount)
					else:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SAFEBOX, selectedSlotPos, selectedItemID)
					snd.PlaySound("sound/ui/pick.wav")

		def __GetCurrentItemGrid(self):
			itemGrid = [[FALSE for cell in xrange(safebox.SAFEBOX_PAGE_SIZE)] for page in xrange(self.pageCount)]

			for page in xrange(self.pageCount):
				for slot in xrange(safebox.SAFEBOX_PAGE_SIZE):
					itemVnum = safebox.GetItemID(slot + page * safebox.SAFEBOX_PAGE_SIZE)
					if itemVnum <> 0:
						item.SelectItem(itemVnum)
						(w, h) = item.GetItemSize()
						for i in xrange(h):
							itemGrid[page][slot + i * safebox.SAFEBOX_SLOT_X_COUNT] = TRUE

			return itemGrid

		def __FindEmptyCellForSize(self, itemGrid, size):
			for page in xrange(self.pageCount):
				for slot in xrange(safebox.SAFEBOX_PAGE_SIZE):
					if itemGrid[page][slot] == FALSE:
						possible = TRUE
						for i in xrange(size):
							p = slot + safebox.SAFEBOX_SLOT_X_COUNT * i
							try:
								if itemGrid[page][p] == TRUE:
									possible = FALSE
									break
							except IndexError:
								possible = FALSE
								break

						if possible:
							return slot + page * safebox.SAFEBOX_PAGE_SIZE

			return -1

		def AttachItemFromInventory(self, slotWindow, slotIndex):
			attachedItemID = player.GetItemIndex(slotWindow, slotIndex)
			item.SelectItem(attachedItemID)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_SAFEBOX):
				return FALSE

			itemGrid = self.__GetCurrentItemGrid()

			item.SelectItem(attachedItemID) 
			emptySafeboxSlot = self.__FindEmptyCellForSize(itemGrid, item.GetItemSize()[1])
			if emptySafeboxSlot == -1 or item.GetItemType() == item.ITEM_TYPE_ELK:
				return FALSE

			net.SendSafeboxCheckinPacket(slotWindow, slotIndex, emptySafeboxSlot)
			return TRUE

		def UseItemSlot(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				mouseModule.mouseController.DeattachObject()

			if self.interface:
				globalSlotIndex = self.__LocalPosToGlobalPos(slotIndex)
				self.interface.AttachItemFromSafebox(globalSlotIndex, safebox.GetItemID(globalSlotIndex))

		def __ShowToolTip(self, slotIndex):
			if self.tooltipItem:
				self.tooltipItem.SetSafeBoxItem(slotIndex)

		def OverInItem(self, slotIndex):
			slotIndex = self.__LocalPosToGlobalPos(slotIndex)
			self.wndItem.SetUsableItem(FALSE)
			self.__ShowToolTip(slotIndex)

		def OverOutItem(self):
			self.wndItem.SetUsableItem(FALSE)
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

	class MallWindow(ui.ScriptWindow):
		PAGE_SIZE = (160, 317)
		PAGE_START_POS = (20, 43)

		def __init__(self, wndSpecialInv = None):
			def __LoadPage():
				self.SetSize(*self.PAGE_SIZE)
				self.SetParent(self.wndSpecialInv)
				self.SetPosition(*self.PAGE_START_POS)

			if not wndSpecialInv:
				print("cant load wndSpecialInv (safeboxwindow)")
				return

			ui.ScriptWindow.__init__(self)

			self.tooltipItem = None
			self.sellingSlotNumber = -1
			self.pageButtonList = []
			self.curPageIndex = 0
			self.isLoaded = 0
			self.wndSpecialInv = wndSpecialInv

			__LoadPage()
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			self.__LoadWindow()

			ui.ScriptWindow.Show(self)

		def Destroy(self):
			self.ClearDictionary()

			self.tooltipItem = None
			self.wndItem = None

			self.pageButtonList = []

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			wndItem = ui.GridSlotWindow()
			wndItem.SetParent(self)
			wndItem.SetPosition(0, 0)
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.Show()

			self.wndItem = wndItem

			self.SetTableSize(3)

		def ShowWindow(self, size):
			self.SetTableSize(size)
			self.Show()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenMall(TRUE)

		def __MakePageButton(self, pageCount):
			self.curPageIndex = 0

		def SetTableSize(self, size):
			pageCount = max(1, size / safebox.SAFEBOX_SLOT_Y_COUNT)

			if not app.ENABLE_EXTENDED_SAFEBOX:
				pageCount = min(3, pageCount)

			size = safebox.SAFEBOX_SLOT_Y_COUNT

			self.__MakePageButton(1)
			self.wndItem.ArrangeSlot(0, safebox.SAFEBOX_SLOT_X_COUNT, size, 32, 32, 0, 0)
			self.wndItem.RefreshSlot()
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			self.SetSize(*self.PAGE_SIZE)
			self.UpdateRect()

		def RefreshMall(self):
			getItemID=safebox.GetMallItemID
			getItemCount=safebox.GetMallItemCount
			setItemID=self.wndItem.SetItemSlot

			for i in xrange(safebox.GetMallSize()):
				itemID = getItemID(i)
				itemCount = getItemCount(i)
				if itemCount <= 1:
					itemCount = 0
				setItemID(i, itemID, itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookitemvnum = safebox.GetMallItemChangeLookVnum(i)
					if not changelookitemvnum == 0:
						self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndItem.EnableSlotCoverImage(i, FALSE)

			self.wndItem.RefreshSlot()

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def Close(self):
			net.SendChatPacket("/mall_close")

		def CommandCloseMall(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenMall(FALSE)

			self.Hide()

		def SelectEmptySlot(self, selectedSlotPos):
			if mouseModule.mouseController.isAttached():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MALL_CANNOT_INSERT)
				mouseModule.mouseController.DeattachObject()

		def SelectItemSlot(self, selectedSlotPos):
			if mouseModule.mouseController.isAttached():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MALL_CANNOT_INSERT)
				mouseModule.mouseController.DeattachObject()
			else:
				curCursorNum = app.GetCursor()
				selectedItemID = safebox.GetMallItemID(selectedSlotPos)
				if app.ENABLE_RENDER_COUNTLINE_ATTACHED_MOUSE:
					itemCount = safebox.GetMallItemCount(selectedSlotPos)
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_MALL, selectedSlotPos, selectedItemID, itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_MALL, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

		def UseItemSlot(self, slotIndex):
			mouseModule.mouseController.DeattachObject()

		def __ShowToolTip(self, slotIndex):
			if self.tooltipItem:
				self.tooltipItem.SetMallItem(slotIndex)

		def OverInItem(self, slotIndex):
			self.__ShowToolTip(slotIndex)

		def OverOutItem(self):
			self.wndItem.SetUsableItem(FALSE)
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()
else:
	class SafeboxWindow(ui.ScriptWindow):

		BOX_WIDTH = 176
		if app.ENABLE_EXTENDED_SAFEBOX:
			BUTTON_PER_LINE = 3

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.tooltipItem = None
			self.sellingSlotNumber = -1
			self.pageButtonList = []
			self.curPageIndex = 0
			self.isLoaded = 0
			self.pageCount = 0
			self.xSafeBoxStart = 0
			self.ySafeBoxStart = 0

			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			self.__LoadWindow()

			ui.ScriptWindow.Show(self)

		def Destroy(self):
			self.ClearDictionary()

			self.dlgPickMoney.Destroy()
			self.dlgPickMoney = None
			self.dlgChangePassword.Destroy()
			self.dlgChangePassword = None

			self.tooltipItem = None
			self.wndMoneySlot = None
			self.wndMoney = None
			self.wndBoard = None
			self.wndItem = None

			self.pageButtonList = []

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/SafeboxWindow.py")

			from _weakref import proxy

			## Item
			wndItem = ui.GridSlotWindow()
			wndItem.SetParent(self)
			wndItem.SetPosition(8, 35)
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.Show()

			## PickMoneyDialog
			import uiPickMoney
			dlgPickMoney = uiPickMoney.PickMoneyDialog()
			dlgPickMoney.LoadDialog()
			dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			dlgPickMoney.Hide()

			## ChangePasswrod
			dlgChangePassword = ChangePasswordDialog()
			dlgChangePassword.LoadDialog()
			dlgChangePassword.Hide()

			## Close Button
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.GetChild("ChangePasswordButton").SetEvent(ui.__mem_func__(self.OnChangePassword))
			self.GetChild("ExitButton").SetEvent(ui.__mem_func__(self.Close))

			self.wndItem = wndItem
			self.dlgPickMoney = dlgPickMoney
			self.dlgChangePassword = dlgChangePassword
			self.wndBoard = self.GetChild("board")

			## Initialize
			self.SetTableSize(3)
			self.RefreshSafeboxMoney()

		def OpenPickMoneyDialog(self):

			if mouseModule.mouseController.isAttached():

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				if player.SLOT_TYPE_INVENTORY == mouseModule.mouseController.GetAttachedType():

					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

				mouseModule.mouseController.DeattachObject()

			else:
				curMoney = safebox.GetMoney()

				if curMoney <= 0:
					return

				self.dlgPickMoney.Open(curMoney)

		def ShowWindow(self, size):

			(self.xSafeBoxStart, self.ySafeBoxStart, z) = player.GetMainCharacterPosition()

			self.SetTableSize(size)
			self.Show()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenSafeBox(TRUE)

		def __MakePageButton(self, pageCount):

			self.curPageIndex = 0
			self.pageButtonList = []
			self.pageCount = pageCount

			if app.ENABLE_EXTENDED_SAFEBOX:
				page_names = ["I", "II", "III", "IV", "V", "VI"]
				width = 52
				pos = []
			else:
				text = "I"
				pos = -int(float(pageCount-1)/2 * 52)

			for i in xrange(pageCount):
				if app.ENABLE_EXTENDED_SAFEBOX:
					if i > len(page_names) - 1:
						page_names.append("X")
					if not i % self.BUTTON_PER_LINE:
						pos.append(-int((max(1, min(self.BUTTON_PER_LINE, pageCount))-1) * (width / 2)))
				button = ui.RadioButton()
				button.SetParent(self)
				button.SetUpVisual("d:/ymir work/ui/game/windows/tab_button_middle_01.sub")
				button.SetOverVisual("d:/ymir work/ui/game/windows/tab_button_middle_02.sub")
				button.SetDownVisual("d:/ymir work/ui/game/windows/tab_button_middle_03.sub")
				button.SetWindowHorizontalAlignCenter()
				button.SetWindowVerticalAlignBottom()
				if app.ENABLE_EXTENDED_SAFEBOX:
					button.SetPosition(pos[len(pos) - 1], (85 + ((pageCount-self.BUTTON_PER_LINE) + 1 % 3) * 4) - (len(pos) - 1) * 20)
					pos[len(pos) - 1] += width
					button.SetText(page_names[i])
				else:
					button.SetPosition(pos, 85)
					button.SetText(text)
				button.SetEvent(lambda arg=i: self.SelectPage(arg))
				button.Show()
				self.pageButtonList.append(button)

				if not app.ENABLE_EXTENDED_SAFEBOX:
					pos += 52
					text += "I"

			self.pageButtonList[0].Down()

		def SelectPage(self, index):

			self.curPageIndex = index

			for btn in self.pageButtonList:
				btn.SetUp()

			self.pageButtonList[index].Down()
			self.RefreshSafebox()

		def __LocalPosToGlobalPos(self, local):
			return self.curPageIndex*safebox.SAFEBOX_PAGE_SIZE + local

		def SetTableSize(self, size):

			pageCount = max(1, size / safebox.SAFEBOX_SLOT_Y_COUNT)

			if not app.ENABLE_EXTENDED_SAFEBOX:
				pageCount = min(3, pageCount)

			size = safebox.SAFEBOX_SLOT_Y_COUNT

			self.__MakePageButton(pageCount)

			self.wndItem.ArrangeSlot(0, safebox.SAFEBOX_SLOT_X_COUNT, size, 32, 32, 0, 0)
			self.wndItem.RefreshSlot()
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)

			wnd_height = 130 + 32 * size
			if app.ENABLE_RENEWAL_QUEST:
				wnd_height += (len(self.pageButtonList) / self.BUTTON_PER_LINE) * 7

			self.wndBoard.SetSize(self.BOX_WIDTH, wnd_height)
			self.SetSize(self.BOX_WIDTH, wnd_height)
			self.UpdateRect()

		def RefreshSafebox(self):
			getItemID=safebox.GetItemID
			getItemCount=safebox.GetItemCount
			setItemID=self.wndItem.SetItemSlot

			for i in xrange(safebox.SAFEBOX_PAGE_SIZE):
				slotIndex = self.__LocalPosToGlobalPos(i)
				itemCount = getItemCount(slotIndex)
				if itemCount <= 1:
					itemCount = 0
				setItemID(i, getItemID(slotIndex), itemCount)

				if app.ENABLE_GROWTH_PET_SYSTEM:
					self.__SetCoolTimePetItemSlot( slotIndex, getItemID(slotIndex))

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookitemvnum = safebox.GetItemChangeLookVnum(slotIndex)
					if not changelookitemvnum == 0:
						self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndItem.EnableSlotCoverImage(i, FALSE)

			self.wndItem.RefreshSlot()

		def RefreshSafeboxMoney(self):
			pass

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def Close(self):
			net.SendChatPacket("/safebox_close")

		def CommandCloseSafebox(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

			if app.ENABLE_SLOT_MARKING_SYSTEM:
				if self.interface:
					self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
					self.interface.RefreshMarkInventoryBag()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenSafeBox(FALSE)

			self.dlgPickMoney.Close()
			self.dlgChangePassword.Close()
			self.Hide()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			def __SetCoolTimePetItemSlot(self, slotNumber, itemVnum):
				if 0 == itemVnum:
					return

				item.SelectItem(itemVnum)
				itemSubType = item.GetItemSubType()

				if itemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
					return

				if itemSubType == item.PET_BAG:
					id = safebox.GetItemMetinSocket(slotNumber, 2)
					if id == 0:
						return

				(limitType, limitValue) = item.GetLimit(0)

				if itemSubType == item.PET_UPBRINGING:
					limitValue = safebox.GetItemMetinSocket(slotNumber, 1)

				if limitType in [item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE]:

					sock_time = safebox.GetItemMetinSocket(slotNumber, 0)
					remain_time = max( 0, sock_time - app.GetGlobalTimeStamp() )

					if slotNumber >= safebox.SAFEBOX_PAGE_SIZE:
						slotNumber -= (self.curPageIndex * safebox.SAFEBOX_PAGE_SIZE)

					self.wndItem.SetSlotCoolTimeInverse(slotNumber, limitValue, limitValue - remain_time)

		## Slot Event
		def SelectEmptySlot(self, selectedSlotPos):

			selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

			if mouseModule.mouseController.isAttached():

				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

				if player.SLOT_TYPE_SAFEBOX == attachedSlotType:

					net.SendSafeboxItemMovePacket(attachedSlotPos, selectedSlotPos, 0)
				else:
					attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
					if player.RESERVED_WINDOW == attachedInvenType:
						return

					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

					else:
						net.SendSafeboxCheckinPacket(attachedInvenType, attachedSlotPos, selectedSlotPos)

				mouseModule.mouseController.DeattachObject()

		def SelectItemSlot(self, selectedSlotPos):

			selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

			if mouseModule.mouseController.isAttached():

				attachedSlotType = mouseModule.mouseController.GetAttachedType()

				if player.SLOT_TYPE_INVENTORY == attachedSlotType:

					if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
						net.SendSafeboxSaveMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

					else:
						attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

				mouseModule.mouseController.DeattachObject()

			else:

				curCursorNum = app.GetCursor()
				if app.SELL == curCursorNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)

				elif app.BUY == curCursorNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

				else:
					selectedItemID = safebox.GetItemID(selectedSlotPos)
					if app.ENABLE_RENDER_COUNTLINE_ATTACHED_MOUSE:
						itemCount = safebox.GetItemCount(selectedSlotPos)
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SAFEBOX, selectedSlotPos, selectedItemID, itemCount)
					else:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SAFEBOX, selectedSlotPos, selectedItemID)
					snd.PlaySound("sound/ui/pick.wav")

		def __GetCurrentItemGrid(self):
			itemGrid = [[FALSE for cell in xrange(safebox.SAFEBOX_PAGE_SIZE)] for page in xrange(self.pageCount)]

			for page in xrange(self.pageCount):
				for slot in xrange(safebox.SAFEBOX_PAGE_SIZE):
					itemVnum = safebox.GetItemID(slot + page * safebox.SAFEBOX_PAGE_SIZE)
					if itemVnum <> 0:
						item.SelectItem(itemVnum)
						(w, h) = item.GetItemSize()
						for i in xrange(h):
							itemGrid[page][slot + i * safebox.SAFEBOX_SLOT_X_COUNT] = TRUE

			return itemGrid

		def __FindEmptyCellForSize(self, itemGrid, size):
			for page in xrange(self.pageCount):
				for slot in xrange(safebox.SAFEBOX_PAGE_SIZE):
					if itemGrid[page][slot] == FALSE:
						possible = TRUE
						for i in xrange(size):
							p = slot + safebox.SAFEBOX_SLOT_X_COUNT * i
							try:
								if itemGrid[page][p] == TRUE:
									possible = FALSE
									break
							except IndexError:
								possible = FALSE
								break

						if possible:
							return slot + page * safebox.SAFEBOX_PAGE_SIZE

			return -1

		def AttachItemFromInventory(self, slotWindow, slotIndex):
			attachedItemID = player.GetItemIndex(slotWindow, slotIndex)
			item.SelectItem(attachedItemID)
			if item.IsAntiFlag(item.ITEM_ANTIFLAG_SAFEBOX):
				return FALSE

			itemGrid = self.__GetCurrentItemGrid()

			item.SelectItem(attachedItemID) 
			emptySafeboxSlot = self.__FindEmptyCellForSize(itemGrid, item.GetItemSize()[1])
			if emptySafeboxSlot == -1 or item.GetItemType() == item.ITEM_TYPE_ELK:
				return FALSE

			net.SendSafeboxCheckinPacket(slotWindow, slotIndex, emptySafeboxSlot)
			return TRUE

		def UseItemSlot(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				mouseModule.mouseController.DeattachObject()

			if self.interface:
				globalSlotIndex = self.__LocalPosToGlobalPos(slotIndex)
				self.interface.AttachItemFromSafebox(globalSlotIndex, safebox.GetItemID(globalSlotIndex))

		def __ShowToolTip(self, slotIndex):
			if self.tooltipItem:
				self.tooltipItem.SetSafeBoxItem(slotIndex)

		def OverInItem(self, slotIndex):
			slotIndex = self.__LocalPosToGlobalPos(slotIndex)
			self.wndItem.SetUsableItem(FALSE)
			self.__ShowToolTip(slotIndex)

		def OverOutItem(self):
			self.wndItem.SetUsableItem(FALSE)
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def OnPickMoney(self, money):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_SAFEBOX, money)

		def OnChangePassword(self):
			self.dlgChangePassword.Open()

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

		def OnUpdate(self):

			USE_SAFEBOX_LIMIT_RANGE = 1000

			(x, y, z) = player.GetMainCharacterPosition()
			if abs(x - self.xSafeBoxStart) > USE_SAFEBOX_LIMIT_RANGE or abs(y - self.ySafeBoxStart) > USE_SAFEBOX_LIMIT_RANGE:
				self.Close()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			def CantCheckInItem(self, slotIndex):
				itemIndex = player.GetItemIndex(slotIndex)

				if itemIndex:
					return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_SAFEBOX)

				return FALSE

			def BindInterface(self, interface):
				from _weakref import proxy
				self.interface = proxy(interface)

			def OnTop(self):
				if not self.interface:
					return

				self.interface.SetOnTopWindow(player.ON_TOP_WND_SAFEBOX)
				self.interface.RefreshMarkInventoryBag()

	class MallWindow(ui.ScriptWindow):

		BOX_WIDTH = 176

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.tooltipItem = None
			self.sellingSlotNumber = -1
			self.pageButtonList = []
			self.curPageIndex = 0
			self.isLoaded = 0
			self.xSafeBoxStart = 0
			self.ySafeBoxStart = 0

			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			self.__LoadWindow()

			ui.ScriptWindow.Show(self)

		def Destroy(self):
			self.ClearDictionary()

			self.tooltipItem = None
			self.wndBoard = None
			self.wndItem = None

			self.pageButtonList = []

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/MallWindow.py")

			from _weakref import proxy

			## Item
			wndItem = ui.GridSlotWindow()
			wndItem.SetParent(self)
			wndItem.SetPosition(8, 35)
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			wndItem.Show()

			## Close Button
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.GetChild("ExitButton").SetEvent(ui.__mem_func__(self.Close))

			self.wndItem = wndItem
			self.wndBoard = self.GetChild("board")

			## Initialize
			self.SetTableSize(3)

		def ShowWindow(self, size):

			(self.xSafeBoxStart, self.ySafeBoxStart, z) = player.GetMainCharacterPosition()

			self.SetTableSize(size)
			self.Show()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenMall(TRUE)

		def SetTableSize(self, size):

			pageCount = max(1, size / safebox.SAFEBOX_SLOT_Y_COUNT)

			if not app.ENABLE_EXTENDED_SAFEBOX:
				pageCount = min(3, pageCount)

			size = safebox.SAFEBOX_SLOT_Y_COUNT

			self.wndItem.ArrangeSlot(0, safebox.SAFEBOX_SLOT_X_COUNT, size, 32, 32, 0, 0)
			self.wndItem.RefreshSlot()
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)

			self.wndBoard.SetSize(self.BOX_WIDTH, 82 + 32*size)
			self.SetSize(self.BOX_WIDTH, 85 + 32*size)
			self.UpdateRect()

		def RefreshMall(self):
			getItemID=safebox.GetMallItemID
			getItemCount=safebox.GetMallItemCount
			setItemID=self.wndItem.SetItemSlot

			for i in xrange(safebox.GetMallSize()):
				itemID = getItemID(i)
				itemCount = getItemCount(i)
				if itemCount <= 1:
					itemCount = 0
				setItemID(i, itemID, itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookitemvnum = safebox.GetMallItemChangeLookVnum(i)
					if not changelookitemvnum == 0:
						self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						self.wndItem.EnableSlotCoverImage(i, FALSE)

			self.wndItem.RefreshSlot()

		def SetItemToolTip(self, tooltip):
			self.tooltipItem = tooltip

		def Close(self):
			net.SendChatPacket("/mall_close")

		def CommandCloseMall(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

			if app.ENABLE_GROWTH_PET_SYSTEM:
				player.SetOpenMall(FALSE)

			self.Hide()

		## Slot Event
		def SelectEmptySlot(self, selectedSlotPos):

			if mouseModule.mouseController.isAttached():

				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MALL_CANNOT_INSERT)
				mouseModule.mouseController.DeattachObject()

		def SelectItemSlot(self, selectedSlotPos):

			if mouseModule.mouseController.isAttached():

				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MALL_CANNOT_INSERT)
				mouseModule.mouseController.DeattachObject()

			else:

				curCursorNum = app.GetCursor()
				selectedItemID = safebox.GetMallItemID(selectedSlotPos)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_MALL, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

		def UseItemSlot(self, slotIndex):
			mouseModule.mouseController.DeattachObject()

		def __ShowToolTip(self, slotIndex):
			if self.tooltipItem:
				self.tooltipItem.SetMallItem(slotIndex)

		def OverInItem(self, slotIndex):
			self.__ShowToolTip(slotIndex)

		def OverOutItem(self):
			self.wndItem.SetUsableItem(FALSE)
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

		def OnUpdate(self):

			USE_SAFEBOX_LIMIT_RANGE = 1000

			(x, y, z) = player.GetMainCharacterPosition()
			if abs(x - self.xSafeBoxStart) > USE_SAFEBOX_LIMIT_RANGE or abs(y - self.ySafeBoxStart) > USE_SAFEBOX_LIMIT_RANGE:
				self.Close()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			def CantCheckInItem(self, slotIndex):
				itemIndex = player.GetItemIndex(slotIndex)

				if itemIndex:
					return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_SAFEBOX)

				return FALSE

			def BindInterface(self, interface):
				from _weakref import proxy
				self.interface = proxy(interface)

			def OnTop(self):
				if not self.interface:
					return

				self.interface.SetOnTopWindow(player.ON_TOP_WND_SAFEBOX)
				self.interface.RefreshMarkInventoryBag()