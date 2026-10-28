if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import item
import snd
import shop
import wndMgr
import chat

import ui
import uiCommon
import mouseModule
import localeInfo
import constInfo

if app.ENABLE_SHOP_REPURCHASE:
	import uiShopRepurchase

class ShopDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = 0
		self.xShopStart = 0
		self.yShopStart = 0
		self.questionDialog = None
		self.popup = None
		self.itemBuyQuestionDialog = None

		if app.ENABLE_SHOP_REPURCHASE:
			self.__repurchase_shop_button = None
			self.__repurchase_shop = uiShopRepurchase.RepurchaseShopBoard()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __GetRealIndex(self, i):
		return self.tabIdx * shop.SHOP_SLOT_COUNT + i

	def Refresh(self):
		getItemID = shop.GetItemID
		getItemCount = shop.GetItemCount
		setItemID = self.itemSlotWindow.SetItemSlot

		for i in xrange(shop.SHOP_SLOT_COUNT):
			idx = self.__GetRealIndex(i)
			itemCount = getItemCount(idx)

			if not app.ENABLE_SLOT_CHAR_ENLARGEMENT:
				if itemCount <= 1:
					itemCount = 0
			setItemID(i, getItemID(idx), itemCount)

			# if app.ENABLE_CHANGE_LOOK_SYSTEM:
				# changelookvnum = shop.GetItemChangeLookVnum(idx)
				# if not changelookvnum == 0:
					# self.itemSlotWindow.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				# else:
					# self.itemSlotWindow.EnableSlotCoverImage(i, FALSE)

		wndMgr.RefreshSlot(self.itemSlotWindow.GetWindowHandle())

	def SetItemData(self, pos, itemID, itemCount, itemPrice):
		shop.SetItemData(pos, itemID, itemCount, itemPrice)

	def LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/shopdialog.py")
		except:
			import exception
			exception.Abort("ShopDialog.LoadDialog.LoadObject")

		smallTab1 = None
		smallTab2 = None
		smallTab3 = None
		middleTab1 = None
		middleTab2 = None

		try:
			GetObject = self.GetChild
			self.itemSlotWindow = GetObject("ItemSlot")
			self.btnBuy = GetObject("BuyButton")
			self.btnSell = GetObject("SellButton")
			self.btnClose = GetObject("CloseButton")
			self.titleBar = GetObject("TitleBar")
			middleTab1 = GetObject("MiddleTab1")
			middleTab2 = GetObject("MiddleTab2")
			smallTab1 = GetObject("SmallTab1")
			smallTab2 = GetObject("SmallTab2")
			smallTab3 = GetObject("SmallTab3")

			if app.ENABLE_SHOP_REPURCHASE:
				self.__repurchase_shop_button = GetObject("repurchase_button")

		except:
			import exception
			exception.Abort("ShopDialog.LoadDialog.BindObject")

		self.itemSlotWindow.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlotWindow.SAFE_SetButtonEvent("LEFT", "EMPTY", self.SelectEmptySlot)
		self.itemSlotWindow.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
		self.itemSlotWindow.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)

		self.itemSlotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.btnBuy.SetToggleUpEvent(ui.__mem_func__(self.CancelShopping))
		self.btnBuy.SetToggleDownEvent(ui.__mem_func__(self.OnBuy))

		self.btnSell.SetToggleUpEvent(ui.__mem_func__(self.CancelShopping))
		self.btnSell.SetToggleDownEvent(ui.__mem_func__(self.OnSell))

		self.btnClose.SetEvent(ui.__mem_func__(self.AskClosePrivateShop))

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop_button:
				self.__repurchase_shop_button.SetEvent(ui.__mem_func__(self.OnClickRepurchaseButton))

		if app.ENABLE_RENEWAL_SHOPEX:
			self.smallRadioButtonGroup = ui.RadioButtonGroup.Create([[smallTab1, ui.__mem_func__(self.OnClickTabButton), None], [smallTab2, ui.__mem_func__(self.OnClickTabButton), None], [smallTab3, ui.__mem_func__(self.OnClickTabButton), None]])
			self.middleRadioButtonGroup = ui.RadioButtonGroup.Create([[middleTab1, ui.__mem_func__(self.OnClickTabButton), None], [middleTab2, ui.__mem_func__(self.OnClickTabButton), None]])
			self.reworkedRadioButtonGroup = None
		else:
			self.smallRadioButtonGroup = ui.RadioButtonGroup.Create([[smallTab1, lambda : self.OnClickTabButton(0), None], [smallTab2, lambda : self.OnClickTabButton(1), None], [smallTab3, lambda : self.OnClickTabButton(2), None]])
			self.middleRadioButtonGroup = ui.RadioButtonGroup.Create([[middleTab1, lambda : self.OnClickTabButton(0), None], [middleTab2, lambda : self.OnClickTabButton(1), None]])

		self.__HideMiddleTabs()
		self.__HideSmallTabs()
		
		self.tabIdx = 0
		self.coinType = shop.SHOP_COIN_TYPE_GOLD

		self.Refresh()

	def __ShowBuySellButton(self):
		self.btnBuy.Hide()
		self.btnSell.Hide()
		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop_button:
				self.__repurchase_shop_button.Show()
		
	def __ShowMiddleTabs(self):
		self.middleRadioButtonGroup.Show()

	def __ShowSmallTabs(self):
		self.smallRadioButtonGroup.Show()

	if app.ENABLE_RENEWAL_SHOPEX:
		def __ShowReworkedShopTabs(self):
			if self.reworkedRadioButtonGroup:
				self.reworkedRadioButtonGroup.Show()

		def __HideReworkedShopTabs(self):
			if self.reworkedRadioButtonGroup:
				self.reworkedRadioButtonGroup.Hide()

		def __CreateReworkedShopButtons(self, groupSize):
			def __GetButtonX(buttonIndex, width):
				CENTER_X = (self.GetWidth()/2) + (2 if groupSize != 1 else 0)
				realgroupSize = groupSize-1
				return CENTER_X - (int(width/2) - (int(width/2 * realgroupSize)) + (((realgroupSize) - buttonIndex) * width))

			self.reworkedRadioButtonGroup = ui.RadioButtonGroup()
			for buttonNumb in xrange(groupSize):
				tmpRB = ui.RadioButton()
				tmpRB.SetParent(self)
				tmpRB.SetUpVisual("d:/ymir work/ui/game/pages/buttons/but_pages_norm.tga")
				tmpRB.SetOverVisual("d:/ymir work/ui/game/pages/buttons/but_pages_hover.tga")
				tmpRB.SetDownVisual("d:/ymir work/ui/game/pages/buttons/but_pages_down.tga")
				tmpRB.SetDisableVisual("d:/ymir work/ui/game/pages/buttons/but_pages_disable.tga")
				tmpRB.SetPosition(__GetButtonX(buttonNumb, tmpRB.GetWidth()), 298)
				tmpRB.Show()
				self.reworkedRadioButtonGroup.AddButton(tmpRB, ui.__mem_func__(self.OnClickTabButton), None)

		def __SetReworkedTabNames(self, groupSize):
			for buttonNumb in xrange(groupSize):
				self.reworkedRadioButtonGroup.SetText(buttonNumb, constInfo.NumberToStrRomanNumerals((buttonNumb + 1)))
				self.reworkedRadioButtonGroup.SetTextPosition(buttonNumb, 0, 0)
				tabName = shop.GetTabName(buttonNumb)
				if tabName:
					self.reworkedRadioButtonGroup.SetToolTipText(buttonNumb, tabName.replace("_", " "))
				else:
					self.reworkedRadioButtonGroup.SetToolTipText(buttonNumb, "")

	def __HideBuySellButton(self):
		self.btnBuy.Hide()
		self.btnSell.Hide()
		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop_button:
				self.__repurchase_shop_button.Hide()

	def __HideMiddleTabs(self):
		self.middleRadioButtonGroup.Hide()

	def __HideSmallTabs(self):
		self.smallRadioButtonGroup.Hide()

	def __SetTabNames(self):
		if shop.GetTabCount() == 2:
			self.middleRadioButtonGroup.SetText(0, shop.GetTabName(0))
			self.middleRadioButtonGroup.SetText(1, shop.GetTabName(1))
		elif shop.GetTabCount() == 3:
			self.smallRadioButtonGroup.SetText(0, shop.GetTabName(0))
			self.smallRadioButtonGroup.SetText(1, shop.GetTabName(1))
			self.smallRadioButtonGroup.SetText(2, shop.GetTabName(2))

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop:
				self.__repurchase_shop.Destroy()
				self.__repurchase_shop = None
			self.__repurchase_shop_button = None

		self.tooltipItem = 0
		self.itemSlotWindow = 0
		self.btnBuy = 0
		self.btnSell = 0
		self.btnClose = 0
		self.titleBar = 0
		self.questionDialog = None
		self.popup = None

	def Open(self, vid):
		isPrivateShop = FALSE
		isMainPlayerPrivateShop = FALSE

		if chr.IsNPC(vid):
			isPrivateShop = FALSE
		else:
			isPrivateShop = TRUE

		if player.IsMainCharacterIndex(vid):
			isMainPlayerPrivateShop = TRUE

			self.btnBuy.Hide()
			self.btnSell.Hide()
			self.btnClose.Show()
		else:
			isMainPlayerPrivateShop = FALSE

			self.btnBuy.Hide()
			self.btnSell.Hide()
			self.btnClose.Hide()

		shop.Open(isPrivateShop, isMainPlayerPrivateShop)

		self.tabIdx = 0

		if app.ENABLE_RENEWAL_SHOPEX:
			if isPrivateShop:
				self.__HideMiddleTabs()
				self.__HideSmallTabs()
			else:
				tabCount = shop.GetTabCount()
				if tabCount > 0:
					self.__CreateReworkedShopButtons(tabCount)
					self.__SetReworkedTabNames(tabCount)
					self.reworkedRadioButtonGroup.OnClick(0)
				else:
					self.__ShowBuySellButton()
					self.__HideMiddleTabs()
					self.__HideSmallTabs()
		else:
			if isPrivateShop:
				self.__HideMiddleTabs()
				self.__HideSmallTabs()
			else:
				if shop.GetTabCount() == 1:
					self.__ShowBuySellButton()
					self.__HideMiddleTabs()
					self.__HideSmallTabs()
				elif shop.GetTabCount() == 2:
					self.__HideBuySellButton()
					self.__ShowMiddleTabs()
					self.__HideSmallTabs()
					self.__SetTabNames()
					self.middleRadioButtonGroup.OnClick(0)
				elif shop.GetTabCount() == 3:
					self.__HideBuySellButton()
					self.__HideMiddleTabs()
					self.__ShowSmallTabs()
					self.__SetTabNames()
					self.smallRadioButtonGroup.OnClick(0)

		self.Refresh()
		self.SetTop()

		if app.ENABLE_SHOP_REPURCHASE:
			if isPrivateShop:
				if self.__repurchase_shop_button:
					self.__repurchase_shop_button.Hide()
			else:
				if self.__repurchase_shop_button:
					self.__repurchase_shop_button.Show()
				if self.__repurchase_shop and self.__repurchase_shop.IsShow():
					self.__repurchase_shop.Close()

		self.Show()

		(self.xShopStart, self.yShopStart, z) = player.GetMainCharacterPosition()

	def Close(self):
		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop and self.__repurchase_shop.IsShow():
				self.__repurchase_shop.Close()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

		if self.itemBuyQuestionDialog:
			self.itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		if self.questionDialog:
			self.OnCloseQuestionDialog()

		shop.Close()

		net.SendShopEndPacket()
		self.CancelShopping()
		self.tooltipItem.HideToolTip()
		self.Hide()

	def GetIndexFromSlotPos(self, slotPos):
		return self.tabIdx * shop.SHOP_SLOT_COUNT + slotPos

	def OnClickTabButton(self, idx):
		self.tabIdx = idx
		self.Refresh()

	def AskClosePrivateShop(self):
		questionDialog = uiCommon.QuestionDialog("thin")
		questionDialog.SetText(localeInfo.PRIVATE_SHOP_CLOSE_QUESTION)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnClosePrivateShop))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		return TRUE

	def OnClosePrivateShop(self):
		net.SendChatPacket("/close_shop")
		self.OnCloseQuestionDialog()
		return TRUE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnPressExitKey(self):
		self.Close()
		return TRUE

	def OnBuy(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
		app.SetCursor(app.BUY)
		self.btnSell.SetUp()

	def OnSell(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_SELL_INFO)
		app.SetCursor(app.SELL)
		self.btnBuy.SetUp()

	def CancelShopping(self):
		self.btnBuy.SetUp()
		self.btnSell.SetUp()
		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop_button:
				self.__repurchase_shop_button.SetUp()
		app.SetCursor(app.NORMAL)

	def __OnClosePopupDialog(self):
		self.pop = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SellAttachedItem(self):

		if shop.IsPrivateShop():
			mouseModule.mouseController.DeattachObject()
			return

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedCount = mouseModule.mouseController.GetAttachedItemCount()

		if player.SLOT_TYPE_INVENTORY == attachedSlotType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType or\
			(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
			player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
			player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
			player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
			player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType and app.ENABLE_SPECIAL_INVENTORY):

			itemIndex = player.GetItemIndex(attachedSlotPos)
			item.SelectItem(itemIndex)

			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemtype = player.INVENTORY

			if player.IsValuableItem(attachedSlotPos):

				itemPrice = item.GetISellItemPrice()

				if item.Is1GoldItem():
					itemPrice = attachedCount / itemPrice / 1
				else:
					itemPrice = itemPrice * max(1, attachedCount) / 1

				itemName = item.GetItemName()

				questionDialog = uiCommon.QuestionDialog("thin")
				questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, attachedCount, itemPrice))

				questionDialog.SetAcceptEvent(lambda arg1=attachedSlotPos, arg2=attachedCount, arg3 = itemtype: self.OnSellItem(arg1, arg2, arg3))
				questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				questionDialog.Open()
				self.questionDialog = questionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
			else:
				self.OnSellItem(attachedSlotPos, attachedCount, itemtype)
		else:
			snd.PlaySound("sound/ui/loginfail.wav")

		mouseModule.mouseController.DeattachObject()

	def OnSellItem(self, slotPos, count, itemtype):
		net.SendShopSellPacketNew(slotPos, count, itemtype)
		snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SelectEmptySlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			self.SellAttachedItem()

	def UnselectItemSlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if shop.IsPrivateShop():
			self.AskBuyItem(selectedSlotPos)
		else:
			net.SendShopBuyPacket(self.__GetRealIndex(selectedSlotPos))

	def SelectItemSlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		isAttached = mouseModule.mouseController.isAttached()
		selectedSlotPos = self.__GetRealIndex(selectedSlotPos)

		if isAttached:
			self.SellAttachedItem()

		else:
			if TRUE == shop.IsMainPlayerPrivateShop():
				return

			curCursorNum = app.GetCursor()
			if app.BUY == curCursorNum:
				self.AskBuyItem(selectedSlotPos)

			elif app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_SELL_INFO)

			else:
				selectedItemID = shop.GetItemID(selectedSlotPos)
				itemCount = shop.GetItemCount(selectedSlotPos)

				type = player.SLOT_TYPE_SHOP
				if shop.IsPrivateShop():
					type = player.SLOT_TYPE_PRIVATE_SHOP

				mouseModule.mouseController.AttachObject(self, type, selectedSlotPos, selectedItemID, itemCount)
				mouseModule.mouseController.SetCallBack("INVENTORY", ui.__mem_func__(self.DropToInventory))
				snd.PlaySound("sound/ui/pick.wav")

	def DropToInventory(self):
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		self.AskBuyItem(attachedSlotPos)

	def AskBuyItem(self, slotPos):
		slotPos = self.__GetRealIndex(slotPos)

		itemIndex = shop.GetItemID(slotPos)
		itemPrice = shop.GetItemPrice(slotPos)
		itemCount = shop.GetItemCount(slotPos)

		item.SelectItem(itemIndex)
		itemName = item.GetItemName()

		itemBuyQuestionDialog = uiCommon.QuestionDialog("thin")
		itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyString(itemPrice)))
		itemBuyQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.Open()
		itemBuyQuestionDialog.pos = slotPos
		self.itemBuyQuestionDialog = itemBuyQuestionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def AnswerBuyItem(self, flag):
		if flag:
			pos = self.itemBuyQuestionDialog.pos
			net.SendShopBuyPacket(pos)

		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		if app.ENABLE_SHOP_REPURCHASE:
			if self.__repurchase_shop:
				self.__repurchase_shop.SetItemToolTip(tooltipItem)

	def OverInItem(self, slotIndex):
		slotIndex = self.__GetRealIndex(slotIndex)
		if mouseModule.mouseController.isAttached():
			return

		if self.tooltipItem:
			if app.ENABLE_RENEWAL_SHOPEX:
				PriceType = shop.GetItemPriceType(slotIndex)
				if PriceType == shop.SHOPEX_GOLD:
					self.tooltipItem.SetShopItem(slotIndex)
				elif PriceType == shop.SHOPEX_COINS:
					self.tooltipItem.SetShopItemBySecondaryCoin(slotIndex)
				else:
					self.tooltipItem.SetShopItemByShopEx(slotIndex, PriceType)
			else:
				if shop.SHOP_COIN_TYPE_GOLD == shop.GetTabCoinType(self.tabIdx):
					self.tooltipItem.SetShopItem(slotIndex)
				else: 
					self.tooltipItem.SetShopItemBySecondaryCoin(slotIndex)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnUpdate(self):
		USE_SHOP_LIMIT_RANGE = 1000

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xShopStart) > USE_SHOP_LIMIT_RANGE or abs(y - self.yShopStart) > USE_SHOP_LIMIT_RANGE:
			self.Close()

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def CantSellInvenItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_SELL)
			return FALSE

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)
			if app.ENABLE_SHOP_REPURCHASE:
				if self.__repurchase_shop:
					self.__repurchase_shop.BindInterface(interface)

		def OnTop(self):
			if not self.interface:
				return

			if shop.IsPrivateShop():
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()
			else:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_SHOP)
				self.interface.RefreshMarkInventoryBag()

	if app.ENABLE_SHOP_REPURCHASE:
		def OnClickRepurchaseButton(self):
			if not self.__repurchase_shop:
				return

			if not self.__repurchase_shop.IsShow():
				self.__repurchase_shop.SendShopOpen()
			else:
				self.__repurchase_shop.SendShopClose()

class MallPageDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()

	def Open(self):
		scriptLoader = ui.PythonScriptLoader()
		scriptLoader.LoadScriptFile(self, "uiscript/mallpagedialog.py")

		self.GetChild("titlebar").SetCloseEvent(ui.__mem_func__(self.Close))

		(x, y) = self.GetGlobalPosition()
		x += 10
		y += 30

		MALL_PAGE_WIDTH = 600
		MALL_PAGE_HEIGHT = 480

		app.ShowWebPage(
			"http://metin2.co.kr/08_mall/game_mall/login_fail.htm", 
			(x, y, x + MALL_PAGE_WIDTH, y + MALL_PAGE_HEIGHT))

		self.Lock()
		self.Show()

	def Close(self):
		app.HideWebPage()
		self.Unlock()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
