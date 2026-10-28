if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import item
import snd
import shop
import chat
import background
import renderTarget
import nonplayer
import skill
import wndMgr

import uisystem
import ui
import uiCommon
import uiToolTip
import mouseModule
import localeInfo
import constInfo
import exception

import uiOfflineShopBuilder

g_isEditingOfflineShop = FALSE

from _weakref import proxy

def IsEditingOfflineShop():
	global g_isEditingOfflineShop
	if (g_isEditingOfflineShop):
		return TRUE
	else:
		return FALSE

class OfflineShopDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.xShopStart = 0
		self.yShopStart = 0
		self.thinboard = None
		self.questionDialog = None
		self.popup = None
		self.vid = 0
		self.shopTitle = 0
		self.shopVnum = 0
		self.itemBuyQuestionDialog = None

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopDialog.py")

			self.GetChild("ItemSlot").SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.GetChild("ItemSlot").SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
			self.GetChild("ItemSlot").SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)
			self.GetChild("ItemSlot").SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GetChild("ItemSlot").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.GetChild("ItemSlot2").SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlotNew)
			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlotNew)
			self.GetChild("ItemSlot2").SetOverInItemEvent(ui.__mem_func__(self.OverInItemNew))
			self.GetChild("ItemSlot2").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.CloseReal))

			renderTarget.SetBackground(1, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetScale(1, 0.7)
		except:
			exception.Abort("OfflineShopDialog.LoadDialog.LoadObject")

	def Open(self, vid):
		self.Refresh()
		self.SetTop()
		self.Show()
		self.vid = vid
		(self.xShopStart, self.yShopStart, z) = player.GetMainCharacterPosition()

	def CloseReal(self):
		self.Close()
		net.SendOfflineShopEndPacket()

	def Close(self):
		renderTarget.SetVisibility(1, FALSE)

		if self.itemBuyQuestionDialog:
			self.itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		self.OnCloseQuestionDialog()
		self.Hide()

	def Destroy(self):
		self.ClearDictionary()
		self.xShopStart = 0
		self.yShopStart = 0
		self.thinboard = None
		self.shopTitle = 0
		self.shopVnum = 0
		self.questionDialog = 0
		self.popup = 0
		self.vid = 0
		self.itemBuyQuestionDialog = 0
		self.interface = None
		self.Hide()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def Refresh(self):
		pointer = [self.GetChild("ItemSlot"),self.GetChild("ItemSlot2")]

		for ptr in pointer:
			index = pointer.index(ptr) + 1
			for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT/2):
				real_index = i
				if index == 2:
					real_index += 40
					if not constInfo.IS_SET(shop.GetShopFlag(), 1 << i):
						ptr.SetItemSlot(i, 50300, 0)
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/lock_0.tga", "d:/ymir work/ui/game/offlineshop/lock_1.tga", "d:/ymir work/ui/game/offlineshop/lock_2.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						continue

				itemCount = shop.GetOfflineShopItemCount(real_index)
				if (itemCount <= 1):
					itemCount = 0

				itemVnum = shop.GetOfflineShopItemID(real_index)

				if shop.GetOfflineShopItemStatus(real_index) == 1:
					item.SelectItem(itemVnum)
					(itemWidth, itemHeight) = item.GetItemSize()
					if itemHeight == 1:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 0, 0)
					elif itemHeight == 2:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 0, 0)
					elif itemHeight == 3:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 0, 0)
				else:
					ptr.SetCoverButton(i, "d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub", "d:/ymir work/ui/game/offlineshop/slot_disable.tga", TRUE, FALSE)

				ptr.SetItemSlot(i, itemVnum, itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookvnum = shop.GetOfflineShopItemTransmutation(real_index)
					if not changelookvnum == 0:
						ptr.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						ptr.EnableSlotCoverImage(i, FALSE)

				wndMgr.RefreshSlot(ptr.GetWindowHandle())

		if len(shop.GetSign()) > 0:
			sign = shop.GetSign()
			if sign[0].isdigit():
				self.GetChild("Board").SetTitleName(sign[1:])
				self.shopTitle = int(sign[0])
				self.ChangeThinboard(self.shopTitle)
			else:
				self.ChangeThinboard(0)

		self.GetChild("Views").SetText(localeInfo.OFFLINESHOP_VIEWS % (shop.GetRealWatcherCount(), shop.GetDisplayedCount()))

		self.shopVnum = shop.GetShopType()
		renderTarget.SelectModel(1, shop.GetShopType())
		renderTarget.SetVisibility(1, TRUE)

	def OnPressEscapeKey(self):
		self.CloseReal()
		return TRUE

	def OnPressExitKey(self):
		self.CloseReal()
		return TRUE

	def ChangeThinboard(self, index):
		if self.thinboard:
			self.thinboard.Destroy()
			del self.thinboard
			self.thinboard = None

		self.thinboard = ui.ThinBoardNorm()
		self.thinboard.__init__("UI_BOTTOM",index)
		self.thinboard.SetParent(self.GetChild("RenderTarget"))

		t_position = [[34,23],[25,10],[25,10],[25,10],[25,10],[25,10]]

		self.thinboard.SetPosition(t_position[index][0], t_position[index][1])
		self.thinboard.SetSize(150)
		self.thinboard.Show()

	def OnCloseQuestionDialog(self):
		if (not self.questionDialog):
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def UnselectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		if app.IsPressed(app.DIK_LCONTROL):
			if self.interface:
				self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))
		else:
			self.AskBuyItem(selectedSlotPos)

	def UnselectItemSlotNew(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		if not constInfo.IS_SET(shop.GetShopFlag(),1<<selectedSlotPos):
			return

		selectedSlotPos += 40

		if app.IsPressed(app.DIK_LCONTROL):
			if self.interface:
				self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))
		else:
			self.AskBuyItem(selectedSlotPos)

	def SelectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		if shop.GetOfflineShopItemStatus(selectedSlotPos) == 1:
			return

		isAttached = mouseModule.mouseController.isAttached()
		if (not isAttached):
			curCursorNum = app.GetCursor()
			if (app.BUY == curCursorNum):
				net.SendOfflineShopBuyPacket(self.vid, selectedSlotPos)
			elif app.IsPressed(app.DIK_LCONTROL):
				if self.interface:
					self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))
			else:
				selectedItemID = shop.GetOfflineShopItemID(selectedSlotPos)
				itemCount = shop.GetOfflineShopItemCount(selectedSlotPos)

				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				mouseModule.mouseController.AttachObject(self, attachedSlotType, selectedSlotPos, selectedItemID, itemCount)
				mouseModule.mouseController.SetCallBack("INVENTORY", ui.__mem_func__(self.DropToInventory))

				snd.PlaySound("sound/ui/pick.wav")

	def SelectItemSlotNew(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		if not constInfo.IS_SET(shop.GetShopFlag(),1<<selectedSlotPos):
			return

		selectedSlotPos += 40

		if app.IsPressed(app.DIK_LCONTROL):
			if self.interface:
				self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))

		if shop.GetOfflineShopItemStatus(selectedSlotPos) == 1:
			return

		isAttached = mouseModule.mouseController.isAttached()
		if (not isAttached):
			curCursorNum = app.GetCursor()
			if (app.BUY == curCursorNum):
				net.SendOfflineShopBuyPacket(self.vid, selectedSlotPos)
			else:
				selectedItemID = shop.GetOfflineShopItemID(selectedSlotPos)
				itemCount = shop.GetOfflineShopItemCount(selectedSlotPos)

				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				mouseModule.mouseController.AttachObject(self, attachedSlotType, selectedSlotPos, selectedItemID, itemCount)
				mouseModule.mouseController.SetCallBack("INVENTORY", ui.__mem_func__(self.DropToInventory))

				snd.PlaySound("sound/ui/pick.wav")

	def DropToInventory(self):
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		self.AskBuyItem(attachedSlotPos)

	def AskBuyItem(self, slotPos):
		if shop.GetOfflineShopItemStatus(slotPos) == 1:
			return

		itemIndex = shop.GetOfflineShopItemID(slotPos)
		itemCount = shop.GetOfflineShopItemCount(slotPos)
		itemPrice = shop.GetOfflineShopItemPrice(slotPos)

		item.SelectItem(itemIndex)

		itemBuyQuestionDialog = uiCommon.QuestionDialog("thin")

		if itemIndex == 70104:
			text = localeInfo.DO_YOU_BUY_ITEM(nonplayer.GetMonsterName(shop.GetOfflineShopItemMetinSocket(slotPos, 0))+ " " + item.GetItemName(), itemCount, localeInfo.NumberToMoneyString(itemPrice))
		elif itemIndex == 50300:
			text = localeInfo.DO_YOU_BUY_ITEM(skill.GetSkillName(shop.GetOfflineShopItemMetinSocket(slotPos, 0))+ " " + item.GetItemName(), itemCount, localeInfo.NumberToMoneyString(itemPrice))
		else:
			text = localeInfo.DO_YOU_BUY_ITEM(item.GetItemName(), itemCount, localeInfo.NumberToMoneyString(itemPrice))

		itemBuyQuestionDialog.SetText(text)
		itemBuyQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.Open()
		itemBuyQuestionDialog.pos = slotPos
		self.itemBuyQuestionDialog = itemBuyQuestionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def AnswerBuyItem(self, flag):
		pos = self.itemBuyQuestionDialog.pos
		if (flag):
			shopid = shop.GetOfflineShopID(pos)
			net.SendOfflineShopBuyPacket(shopid, pos)

		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def OverInItem(self, slotIndex):
		if (mouseModule.mouseController.isAttached()):
			return

		if None == self.tooltipItem:
			return

		self.tooltipItem.SetOfflineShopItem(slotIndex)

	def OverInItemNew(self, slotIndex):
		if (mouseModule.mouseController.isAttached()):
			return

		slotIndex += 40

		if None == self.tooltipItem:
			return

		self.tooltipItem.SetOfflineShopItem(slotIndex)

	def OverOutItem(self):
		if None == self.tooltipItem:
			return

		self.tooltipItem.HideToolTip()

	def OnUpdate(self):
		USE_SHOP_LIMIT_RANGE = 1500

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xShopStart) > USE_SHOP_LIMIT_RANGE or abs(y - self.yShopStart) > USE_SHOP_LIMIT_RANGE:
			self.CloseReal()

class OfflineMyShop(ui.ScriptWindow):
	class LogsItem(ui.ImageBox):
		def Destroy(self):
			self.itemName = None
			self.playerName = None
			self.itemPrice = None

		def __del__(self):
			ui.ImageBox.__del__(self)

		def __init__(self, parent, name, date, itemvnum, itemcount, price):
			ui.ImageBox.__init__(self)
			self.SetParent(parent)
			item.SelectItem(itemvnum)
			self.LoadImage("d:/ymir work/ui/game/offlineshop/logs_table.tga")

			self.itemName = ui.TextLine()
			self.itemName.SetParent(self)
			self.itemName.SetPosition(80, 3)
			self.itemName.SetFontName("Verdana:12")
			self.itemName.SetOutline(1)
			self.itemName.SetHorizontalAlignCenter()
			if int(itemcount) == 1:
				self.itemName.SetText("|cffe5d500%s|r" % (item.GetItemName()))
			else:
				self.itemName.SetText("|cffe5d500%s (x%d)|r" % (item.GetItemName(), int(itemcount)))
			self.itemName.Show()

			self.playerName = ui.TextLine()
			self.playerName.SetParent(self)
			self.playerName.SetPosition(270, 3)
			self.playerName.SetFontName("Verdana:12")
			self.playerName.SetOutline(1)
			self.playerName.SetHorizontalAlignCenter()
			self.playerName.SetText("|cffe5d500%s|r" % (str(name)))
			self.playerName.Show()

			self.itemPrice = ui.TextLine()
			self.itemPrice.SetParent(self)
			self.itemPrice.SetPosition(450, 3)
			self.itemPrice.SetFontName("Verdana:12")
			self.itemPrice.SetOutline(1)
			self.itemPrice.SetHorizontalAlignCenter()
			self.itemPrice.SetText("|cffe5d500%s Yang|r" % (localeInfo.NumberToDecimalString(long(price))))
			self.itemPrice.Show()

			self.Show()

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.priceInputBoard = None
		self.interface = None
		self.shopLogs = FALSE
		self.logIndex = 0
		self.thinboard = 0
		self.title = ""
		self.time = 0
		self.money_time = 0
		self.shopTitle = 0
		self.shopVnum = 0
		self.lastUpdateTime=0
		self.offlineDecoration = None
		self.title_time = 0

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/offlineshoppanel.py")

			self.GetChild("ItemSlot").SAFE_SetButtonEvent("LEFT", "EXIST", self.UnselectItemSlot)
			self.GetChild("ItemSlot").SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)
			self.GetChild("ItemSlot").SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GetChild("ItemSlot").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.GetChild("ItemSlot").SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))

			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("LEFT", "EXIST", self.UnselectItemSlotNew)
			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlotNew)
			self.GetChild("ItemSlot2").SetOverInItemEvent(ui.__mem_func__(self.OverInItemNew))
			self.GetChild("ItemSlot2").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.GetChild("ItemSlot2").SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlotNew))

			self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.CloseReal))
			self.GetChild("SecondButton").SetEvent(ui.__mem_func__(self.OpenLogs))
			self.GetChild("FirstButton").SetEvent(ui.__mem_func__(self.CloseShop))
			self.GetChild("RefreshTitle").SetEvent(ui.__mem_func__(self.RefreshTitle))
			self.GetChild("RefillButton").SetEvent(ui.__mem_func__(self.AddTime))

			self.GetChild("MoneyInput").SetEvent(ui.__mem_func__(self.MoneyCheck))

			self.GetChild("RefreshRender").SetEvent(ui.__mem_func__(self.RefreshDecoration))

			self.GetChild("LogsWindow").Hide()

			self.GetChild("ListBox").SetScrollBar(self.GetChild("ScrollBar"))
			self.GetChild("ListBox").SetViewItemCount(14)
			self.GetChild("ListBox").SetItemStep(22)
			self.GetChild("ListBox").SetItemSize(400, 38)
			self.GetChild("ListBox").RemoveAllItems()

			self.GetChild("TitleBackButton").SetEvent(ui.__mem_func__(self.TitleBack))
			self.GetChild("TitleNextButton").SetEvent(ui.__mem_func__(self.TitleNext))
			self.GetChild("ShopBackButton").SetEvent(ui.__mem_func__(self.ShopBack))
			self.GetChild("ShopNextButton").SetEvent(ui.__mem_func__(self.ShopNext))
			self.GetChild("ShopBackButton").Hide()
			self.GetChild("TitleBackButton").Hide()

			self.GetChild("NameLine").OnPressEscapeKey = ui.__mem_func__(self.CloseReal)

			self.closeQuestionDialog = uiCommon.QuestionDialog("thin")
			self.closeQuestionDialog.Hide()

			renderTarget.SetBackground(1, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetScale(1, 0.7)
		except:
			exception.Abort("OfflineShopChangePriceWindow.LoadWindow.LoadObject")

	def AddTime(self):
		priceInputBoard = uiCommon.QuestionDialog("thin")
		text = localeInfo.OFFLINESHOP_EXTEND_TIME_DIALOG % localeInfo.NumberToMoneyString(20000000)
		priceInputBoard.SetText(text)
		priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptAddTime))
		priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelRemoveItem))
		priceInputBoard.Open()
		self.priceInputBoard = priceInputBoard

	def AcceptAddTime(self):
		net.SendAddTime()
		self.CancelRemoveItem()

	def MoneyCheck(self):
		if shop.GetCurrentOfflineShopMoney() > 0:
			net.SendTakeOfflineMoney()

	def RefreshDecoration(self):
		sign = shop.GetSign()
		first = FALSE
		second = FALSE

		if sign[0].isdigit():
			if self.shopTitle  == int(sign[0]):
				first = TRUE

		if self.shopVnum + 30000 == shop.GetShopType():
			second = TRUE

		if second == TRUE and first == TRUE:
			return

		sign = str(self.shopTitle) + sign[1:]
		shop.ChangeDecoration(sign, self.shopVnum, self.shopTitle)

	def RefreshTitle(self):
		newsign = self.GetChild("NameLine").GetText()
		if newsign.isdigit() or newsign == "" or newsign == " ":
			return

		sign = shop.GetSign()[1:]
		if newsign != sign:
			net.SendOfflineShopChangeTitle(newsign)

	def CloseShop(self):
		self.closeQuestionDialog.SetText(localeInfo.OFFLINESHOP_DO_YOU_WANT_CLOSE)
		self.closeQuestionDialog.SetAcceptEvent(lambda arg = TRUE: self.AnswerCloseOfflineShop(arg))
		self.closeQuestionDialog.SetCancelEvent(lambda arg = FALSE: self.AnswerCloseOfflineShop(arg))
		self.closeQuestionDialog.Open()

	def AnswerCloseOfflineShop(self, flag):
		self.closeQuestionDialog.Hide()
		if flag:
			net.SendDestroyOfflineShop()

	def OpenLogs(self):
		if self.shopLogs == FALSE:
			self.GetChild("MainWindow").Hide()
			self.GetChild("LogsWindow").Show()
			self.shopLogs = TRUE

			self.GetChild("FirstButton").SetEvent(ui.__mem_func__(self.RemoveLogs))
			self.GetChild("FirstButton").SetToolTipText(localeInfo.OFFLINESHOP_BUTTON_HISTORY_CLEAR)
			self.GetChild("FirstButton").SetUpVisual("d:/ymir work/ui/game/offlineshop/remove_history_button_norm.tga")
			self.GetChild("FirstButton").SetOverVisual("d:/ymir work/ui/game/offlineshop/remove_history_button_hover.tga")
			self.GetChild("FirstButton").SetDownVisual("d:/ymir work/ui/game/offlineshop/remove_history_button_down.tga")

		else:
			self.GetChild("MainWindow").Show()
			self.GetChild("LogsWindow").Hide()
			self.shopLogs = FALSE

			self.GetChild("FirstButton").SetEvent(ui.__mem_func__(self.CloseShop))
			self.GetChild("FirstButton").SetToolTipText(localeInfo.OFFLINESHOP_BUTTON_CLOSE)
			self.GetChild("FirstButton").SetUpVisual("d:/ymir work/ui/game/offlineshop/close_button_norm.tga")
			self.GetChild("FirstButton").SetOverVisual("d:/ymir work/ui/game/offlineshop/close_button_hover.tga")
			self.GetChild("FirstButton").SetDownVisual("d:/ymir work/ui/game/offlineshop/close_button_down.tga")

	def RemoveLogs(self):
		self.closeQuestionDialog.SetText(localeInfo.OFFLINESHOP_DO_YOU_REMOVE_HISTORY)
		self.closeQuestionDialog.SetAcceptEvent(lambda arg = TRUE: self.AnswerRemoveLogs(arg))
		self.closeQuestionDialog.SetCancelEvent(lambda arg = FALSE: self.AnswerRemoveLogs(arg))
		self.closeQuestionDialog.Open()

	def AnswerRemoveLogs(self, flag):
		self.closeQuestionDialog.Hide()
		if flag:
			net.SendOfflineShopRemoveLogs()

	def OnRunMouseWheel(self, nLen):
		if self.shopLogs == TRUE:
			if nLen > 0:
				self.GetChild("ScrollBar").OnUp()
			else:
				self.GetChild("ScrollBar").OnDown()

	def Destroy(self):
		self.GetChild("ListBox").RemoveAllItems()

		self.ClearDictionary()
		self.itemSlot = None
		self.itemSlot2 = None
		self.btnOk = None
		self.lastUpdateTime = 0
		self.interface = None
		self.priceInputBoard = None

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def AppendLog(self, name, date, itemvnum, itemcount, price):
		logItem = OfflineMyShop.LogsItem(self, name, date, itemvnum, itemcount, price)
		logItem.SetPosition(5, 20 + (20*self.logIndex))
		logItem.Show()
		self.GetChild("ListBox").AppendItem(logItem)
		self.logIndex += 1

	def AppendLogFirst(self, name, date, itemvnum, itemcount, price):
		logItem = OfflineMyShop.LogsItem(self, name, date, itemvnum, itemcount, price)
		logItem.SetPosition(5, 20 + (20*self.logIndex))
		logItem.Show()
		self.GetChild("ListBox").AppendItemWithIndex(0, logItem)
		self.logIndex += 1

	def Open(self):
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = TRUE
		self.Show()
		self.Refresh()

	def CloseReal(self):
		self.Close()
		net.SendOfflineShopEndPacket()
		return TRUE

	def Close(self):
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = FALSE
		renderTarget.SetVisibility(1, FALSE)
		self.Hide()

	def OnSelectEmptySlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if (isAttached):
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			slot_type_list = [player.SLOT_TYPE_INVENTORY, player.SLOT_TYPE_SKILL_BOOK_INVENTORY, player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY, player.SLOT_TYPE_STONE_INVENTORY, player.SLOT_TYPE_GIFT_BOX_INVENTORY, player.SLOT_TYPE_CHANGERS_INVENTORY, player.SLOT_TYPE_DRAGON_SOUL_INVENTORY]
			if not attachedSlotType in slot_type_list:
				return

			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			itemVNum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			count = player.GetItemCount(attachedInvenType, attachedSlotPos)
			item.SelectItem(itemVNum)

			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_CANNOT_SELL_ITEM)
				return

			priceInputBoard = uiCommon.MoneyInputDialog()
			priceInputBoard.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_PRICE_DIALOG_TITLE)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelRemoveItem))
			priceInputBoard.Open()

			priceInputBoard.SetValue(0)

			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.itemVNum = itemVNum
			self.priceInputBoard.sourceWindowType = attachedInvenType
			self.priceInputBoard.sourceSlotPos = attachedSlotPos
			self.priceInputBoard.targetSlotPos = selectedSlotPos

	def OnSelectEmptySlotNew(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if not constInfo.IS_SET(shop.GetShopFlag(), 1 << selectedSlotPos):
			return

		if (isAttached):
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			slot_type_list = [player.SLOT_TYPE_INVENTORY, player.SLOT_TYPE_SKILL_BOOK_INVENTORY, player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY, player.SLOT_TYPE_STONE_INVENTORY, player.SLOT_TYPE_GIFT_BOX_INVENTORY, player.SLOT_TYPE_CHANGERS_INVENTORY, player.SLOT_TYPE_DRAGON_SOUL_INVENTORY]
			if not attachedSlotType in slot_type_list:
				return

			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
			itemVNum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			count = player.GetItemCount(attachedInvenType, attachedSlotPos)
			item.SelectItem(itemVNum)

			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_CANNOT_SELL_ITEM)
				return

			priceInputBoard = uiCommon.MoneyInputDialog()
			priceInputBoard.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_PRICE_DIALOG_TITLE)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelRemoveItem))
			priceInputBoard.Open()

			priceInputBoard.SetValue(0)

			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.itemVNum = itemVNum
			self.priceInputBoard.sourceWindowType = attachedInvenType
			self.priceInputBoard.sourceSlotPos = attachedSlotPos
			self.priceInputBoard.targetSlotPos = selectedSlotPos + 40

	def AcceptInputPrice(self):
		if (not self.priceInputBoard):
			return TRUE

		price = long(self.priceInputBoard.GetText())
		if not price:
			return TRUE

		if long(price) <= 0:
			return TRUE

		attachedInvenType = self.priceInputBoard.sourceWindowType
		sourceSlotPos = self.priceInputBoard.sourceSlotPos
		targetSlotPos = self.priceInputBoard.targetSlotPos

		itemVnum = player.GetItemIndex(attachedInvenType, sourceSlotPos)
		count = player.GetItemCount(attachedInvenType, sourceSlotPos)

		if itemVnum != self.priceInputBoard.itemVNum:
			self.priceInputBoard.Close()
			self.priceInputBoard = None
			return

		net.SendAddOfflineShopItem(sourceSlotPos, targetSlotPos, price, attachedInvenType)

		snd.PlaySound("sound/ui/drop.wav")
		self.priceInputBoard = None
		return TRUE

	def Refresh(self):
		if not self.IsShow():
			return

		pointer = [self.GetChild("ItemSlot"), self.GetChild("ItemSlot2")]
		for ptr in pointer:
			for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT/2):
				ptr.ClearSlot(i)
				wndMgr.RefreshSlot(ptr.GetWindowHandle())

		for ptr in pointer:
			index = pointer.index(ptr)+1
			for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT/2):
				real_index = i
				if index == 2:
					real_index += 40
					if not constInfo.IS_SET(shop.GetShopFlag(), 1 << i):
						ptr.SetItemSlot(i, 50300, 0)
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/lock_0.tga", "d:/ymir work/ui/game/offlineshop/lock_1.tga", "d:/ymir work/ui/game/offlineshop/lock_2.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						ptr.EnableSlot(i)
						continue

				itemCount = shop.GetOfflineShopItemCount(real_index)
				if (itemCount <= 1):
					itemCount = 0

				itemVnum = shop.GetOfflineShopItemID(real_index)
				ptr.SetItemSlot(i, itemVnum, itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookvnum = shop.GetOfflineShopItemTransmutation(real_index)
					if not changelookvnum == 0:
						ptr.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						ptr.EnableSlotCoverImage(i, FALSE)

				if shop.GetOfflineShopItemStatus(real_index) == 1:
					item.SelectItem(itemVnum)
					(itemWidth, itemHeight) = item.GetItemSize()
					if itemHeight == 1:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/game/offlineshop/negative_frame1.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						ptr.EnableSlot(i)
					elif itemHeight == 2:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/game/offlineshop/negative_frame2.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						ptr.EnableSlot(i)
					elif itemHeight == 3:
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/game/offlineshop/negative_frame3.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						ptr.EnableSlot(i)
				else:
					ptr.SetCoverButton(i, "d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub", "d:/ymir work/ui/game/offlineshop/slot_disable.tga", FALSE, FALSE)
					ptr.EnableSlot(i)

			wndMgr.RefreshSlot(ptr.GetWindowHandle())

		self.GetChild("TitleBackButton").Show()
		self.GetChild("TitleNextButton").Show()
		self.GetChild("ShopBackButton").Show()
		self.GetChild("ShopNextButton").Show()

		if len(shop.GetSign())>0:
			sign = shop.GetSign()
			title = int(sign[0])
			if sign[0].isdigit():
				self.GetChild("NameLine").SetText("")
				self.GetChild("NameLine").SetInfoMessage(sign[1:])
				self.ChangeThinboard(title)
				self.shopTitle = title
				if title <= 0:
					self.GetChild("TitleBackButton").Hide()
				if title+1 >= constInfo.MAX_SHOP_TITLE:
					self.GetChild("TitleNextButton").Hide()
			else:
				self.GetChild("NameLine").SetText("")
				self.GetChild("NameLine").SetInfoMessage(sign)
				self.GetChild("TitleBackButton").Hide()
				self.shopTitle = 0
				self.ChangeThinboard(0)

		self.shopVnum = shop.GetShopType()-30000

		if self.shopVnum <= 0:
			self.GetChild("ShopBackButton").Hide()
		if self.shopVnum + 1 >= constInfo.MAX_SHOP_TYPE:
			self.GetChild("ShopNextButton").Hide()

		renderTarget.SelectModel(1, shop.GetShopType())
		renderTarget.SetVisibility(1, TRUE)

		self.GetChild("Views").SetText(localeInfo.OFFLINESHOP_VIEWS % (shop.GetRealWatcherCount(), shop.GetDisplayedCount()))
		self.GetChild("Money").SetText(localeInfo.NumberToMoneyString(shop.GetCurrentOfflineShopMoney()))
		self.GetChild("Time").SetText(localeInfo.SecondToDHM(shop.GetShopTime() - app.GetGlobalTimeStamp()))

	def SetItemData(self, pos, itemID, itemCount, itemPrice):
		shop.SetOfflineShopItemData(pos, itemID, itemCount, itemPrice)

	def OnPressEscapeKey(self):
		self.CloseReal()
		return TRUE

	def OnPressExitKey(self):
		self.CloseReal()
		return TRUE

	def UnselectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		if app.IsPressed(app.DIK_LCONTROL):
			if self.interface:
				self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))
		else:
			itemIndex = shop.GetOfflineShopItemID(selectedSlotPos)
			item.SelectItem(itemIndex)
			priceInputBoard = uiCommon.QuestionDialog("thin")
			if itemIndex == 70104:
				text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % (nonplayer.GetMonsterName(shop.GetOfflineShopItemMetinSocket(selectedSlotPos, 0)) + " " + item.GetItemName())
			elif itemIndex == 50300:
				text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % (skill.GetSkillName(shop.GetOfflineShopItemMetinSocket(selectedSlotPos, 0)) + " " + item.GetItemName())
			else:
				text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % item.GetItemName()

			priceInputBoard.SetText(text)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptRemoveItem))
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelRemoveItem))
			priceInputBoard.Open()
			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.pos = selectedSlotPos

	def UnselectItemSlotNew(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return

		priceInputBoard = uiCommon.QuestionDialog("thin")
		if not constInfo.IS_SET(shop.GetShopFlag(), 1 << selectedSlotPos):
			priceInputBoard.SetText(localeInfo.OFFLINESHOP_DO_YOU_WANT_SLOT)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptOpenSlot))
			priceInputBoard.pos = selectedSlotPos
		else:
			selectedSlotPos += 40
			if app.IsPressed(app.DIK_LCONTROL):
				if self.interface:
					self.interface.OpenRenderTargetWindow(0, shop.GetOfflineShopItemID(selectedSlotPos))
			else:
				itemIndex = shop.GetOfflineShopItemID(selectedSlotPos)
				item.SelectItem(itemIndex)
				if itemIndex == 70104:
					text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % (nonplayer.GetMonsterName(shop.GetOfflineShopItemMetinSocket(selectedSlotPos, 0)) + " " + item.GetItemName())
				elif itemIndex == 50300:
					text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % (skill.GetSkillName(shop.GetOfflineShopItemMetinSocket(selectedSlotPos, 0)) + " " + item.GetItemName())
				else:
					text = localeInfo.OFFLINESHOP_DO_YOU_GET_BACK % item.GetItemName()
				priceInputBoard.SetText(text)
				priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptRemoveItem))
				priceInputBoard.pos = selectedSlotPos

		priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelRemoveItem))
		priceInputBoard.Open()
		self.priceInputBoard = priceInputBoard

	def AcceptOpenSlot(self):
		if constInfo.IS_SET(shop.GetShopFlag(), 1 << self.priceInputBoard.pos):
			return

		net.SendOpenShopSlot(self.priceInputBoard.pos)
		self.priceInputBoard = None
		self.Refresh()
		return TRUE

	def AcceptRemoveItem(self):
		net.SendRemoveOfflineShopItem(self.priceInputBoard.pos)
		self.priceInputBoard = None
		return TRUE

	def CancelRemoveItem(self):
		self.priceInputBoard = None
		return TRUE

	def OverInItem(self, slotIndex):
		if self.priceInputBoard != None:
			return

		if None == self.tooltipItem:
			return

		self.tooltipItem.SetOfflineShopItem(slotIndex)

	def OverInItemNew(self, slotIndex):
		if self.priceInputBoard != None:
			return

		if None == self.tooltipItem:
			return

		self.tooltipItem.SetOfflineShopItem(slotIndex + 40)

	def OverOutItem(self):
		if None == self.tooltipItem:
			return

		self.tooltipItem.HideToolTip()

	def ChangeThinboard(self, index):
		if self.thinboard:
			self.thinboard.Destroy()
			del self.thinboard
			self.thinboard = None

		self.thinboard = ui.ThinBoardNorm()
		self.thinboard.__init__("UI_BOTTOM", index)
		self.thinboard.SetParent(self.GetChild("RenderTarget"))

		t_position = [[34,23],[25,10],[25,10],[25,10],[25,10],[25,10]]

		self.thinboard.SetPosition(t_position[index][0], t_position[index][1])
		self.thinboard.SetSize(150)
		self.thinboard.Show()

	def TitleBack(self):
		if self.shopTitle <= 0:
			return

		self.shopTitle -= 1
		self.GetChild("TitleNextButton").Show()

		if self.shopTitle == 0:
			self.GetChild("TitleBackButton").Hide()

		self.ChangeThinboard(self.shopTitle)

	def TitleNext(self):
		if self.shopTitle + 1  == constInfo.MAX_SHOP_TITLE:
			return

		self.shopTitle += 1
		self.GetChild("TitleBackButton").Show()

		if self.shopTitle + 1  == constInfo.MAX_SHOP_TITLE:
			self.GetChild("TitleNextButton").Hide()

		self.ChangeThinboard(self.shopTitle)

	def ShopBack(self):
		if self.shopVnum <= 0:
			return

		self.shopVnum -= 1
		self.GetChild("ShopNextButton").Show()

		if self.shopVnum == 0:
			self.GetChild("ShopBackButton").Hide()

		renderTarget.SelectModel(1, (30000 + self.shopVnum))
		renderTarget.SetVisibility(1, TRUE)

	def ShopNext(self):
		if self.shopVnum + 1  == constInfo.MAX_SHOP_TYPE:
			return

		self.shopVnum += 1
		self.GetChild("ShopBackButton").Show()

		if self.shopVnum+1  == constInfo.MAX_SHOP_TYPE:
			self.GetChild("ShopNextButton").Hide()

		renderTarget.SelectModel(1, (30000 + self.shopVnum))
		renderTarget.SetVisibility(1, TRUE)

	def OnUpdate(self):
		if app.GetGlobalTime() - self.lastUpdateTime > 500:
			self.lastUpdateTime = app.GetGlobalTime()
			self.GetChild("Time").SetText(localeInfo.SecondToDHM(shop.GetShopTime() - app.GetGlobalTimeStamp()))
