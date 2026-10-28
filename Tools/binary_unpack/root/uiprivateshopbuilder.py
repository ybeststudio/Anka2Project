if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import snd
import shop
import mouseModule
import uiCommon
import localeInfo
import chat
import item
import systemSetting

g_isBuildingPrivateShop = FALSE

g_itemPriceDict={}

g_privateShopAdvertisementBoardDict={}

def Clear():
	global g_itemPriceDict
	global g_isBuildingPrivateShop
	g_itemPriceDict={}
	g_isBuildingPrivateShop = FALSE

def IsPrivateShopItemPriceList():
	global g_itemPriceDict
	if g_itemPriceDict:
		return TRUE
	else:
		return FALSE

def IsBuildingPrivateShop():
	global g_isBuildingPrivateShop
	if player.IsOpenPrivateShop() or g_isBuildingPrivateShop:
		return TRUE
	else:
		return FALSE

def SetPrivateShopItemPrice(itemVNum, itemPrice):
	global g_itemPriceDict
	g_itemPriceDict[int(itemVNum)]=itemPrice

def GetPrivateShopItemPrice(itemVNum):
	try:
		global g_itemPriceDict
		return g_itemPriceDict[itemVNum]
	except KeyError:
		return 0

def UpdateADBoard():
	for key in g_privateShopAdvertisementBoardDict.keys():
		if app.ENABLE_GRAPHIC_ON_OFF:
			if IsShowSalesRange(key):
				g_privateShopAdvertisementBoardDict[key].Show()
			else:
				g_privateShopAdvertisementBoardDict[key].Hide()
		else:
			g_privateShopAdvertisementBoardDict[key].Show()

def DeleteADBoard(vid):
	if not g_privateShopAdvertisementBoardDict.has_key(vid):
		return

	del g_privateShopAdvertisementBoardDict[vid]

if app.ENABLE_GRAPHIC_ON_OFF:
	def IsShowSalesRange(vid):
		if not vid:
			return FALSE

		try:
			VIEW_RANGE = 0

			(xShop, yShop, zShop) = chr.GetPixelPosition(vid)
			(x, y, z) = player.GetMainCharacterPosition()

			if systemSetting.GetPrivateShopLevel() == 1: VIEW_RANGE = 5000
			elif systemSetting.GetPrivateShopLevel() == 2: VIEW_RANGE = 2500
			elif systemSetting.GetPrivateShopLevel() == 3: VIEW_RANGE = 1000
			elif systemSetting.GetPrivateShopLevel() == 4: VIEW_RANGE = 500

			if (abs(x - xShop) < VIEW_RANGE and abs(y - yShop) < VIEW_RANGE) or systemSetting.GetPrivateShopLevel() < 1:
				return TRUE
			else:
				return FALSE
		except:
			return FALSE

if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
elif app.ENABLE_SLOT_MARKING_SYSTEM:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
	if app.ENABLE_SPECIAL_INVENTORY:
		SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_INVENTORY_PAGE_SIZE

class PrivateShopAdvertisementBoard(ui.ThinBoard):
	def __init__(self):
		ui.ThinBoard.__init__(self, "UI_BOTTOM")
		self.vid = None
		self.__MakeTextLine()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def __MakeTextLine(self):
		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetWindowHorizontalAlignCenter()
		self.textLine.SetWindowVerticalAlignCenter()
		self.textLine.SetHorizontalAlignCenter()
		self.textLine.SetVerticalAlignCenter()
		self.textLine.Show()

	def Open(self, vid, text):
		self.vid = vid

		self.textLine.SetText(text)
		self.textLine.UpdateRect()
		self.SetSize(len(text)*6 + 10*2, 20)
		self.Show()

		g_privateShopAdvertisementBoardDict[vid] = self

	def OnMouseLeftButtonUp(self):
		if not self.vid:
			return

		net.SendOnClickPacket(self.vid)

		return TRUE

	def OnUpdate(self):
		if not self.vid:
			return

		if systemSetting.IsShowSalesText():
			if app.ENABLE_GRAPHIC_ON_OFF:
				if IsShowSalesRange(self.vid):
					self.Show()
				else:
					self.Hide()

				x, y = chr.GetProjectPosition(self.vid, 220)
				self.SetPosition(x - self.GetWidth()/2, y - self.GetHeight()/2)
			else:
				self.Show()
				x, y = chr.GetProjectPosition(self.vid, 220)
				self.SetPosition(x - self.GetWidth()/2, y - self.GetHeight()/2)
		else:
			for key in g_privateShopAdvertisementBoardDict.keys():
				if  player.GetMainCharacterIndex() == key:
					g_privateShopAdvertisementBoardDict[key].Show()
					x, y = chr.GetProjectPosition(player.GetMainCharacterIndex(), 220)
					g_privateShopAdvertisementBoardDict[key].SetPosition(x - self.GetWidth()/2, y - self.GetHeight()/2)
				else:
					g_privateShopAdvertisementBoardDict[key].Hide()

class PrivateShopBuilder(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__LoadWindow()
		self.itemStock = {}
		self.tooltipItem = None
		self.priceInputBoard = None
		self.title = ""

		if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
			self.inven = {}
			self.ItemListIdx = {}
		elif app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
			self.inven = {}
			self.ItemListIdx = None
		elif app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None
			if app.ENABLE_SPECIAL_INVENTORY:
				self.invenSpecial = None

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PrivateShopBuilder.py")
		except:
			import exception
			exception.Abort("PrivateShopBuilderWindow.LoadWindow.LoadObject")

		try:
			GetObject = self.GetChild
			self.nameLine = GetObject("NameLine")
			self.itemSlot = GetObject("ItemSlot")
			self.btnOk = GetObject("OkButton")
			self.btnClose = GetObject("CloseButton")
			self.titleBar = GetObject("TitleBar")
		except:
			import exception
			exception.Abort("PrivateShopBuilderWindow.LoadWindow.BindObject")

		self.btnOk.SetEvent(ui.__mem_func__(self.OnOk))
		self.btnClose.SetEvent(ui.__mem_func__(self.OnClose))
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.OnClose))

		self.itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
		self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))

	def Destroy(self):
		self.ClearDictionary()

		self.nameLine = None
		self.itemSlot = None
		self.btnOk = None
		self.btnClose = None
		self.titleBar = None
		self.priceInputBoard = None

	def Open(self, title):

		self.title = title

		if len(title) > 25:
			title = title[:22] + "..."

		self.itemStock = {}
		shop.ClearPrivateShopStock()
		self.nameLine.SetText(title)
		self.SetCenterPosition()
		self.Refresh()
		self.Show()

		global g_isBuildingPrivateShop
		g_isBuildingPrivateShop = TRUE

		if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
			self.ItemListIdx = {}
			for type in self.inven.keys():
				self.ItemListIdx[type] = []
		elif app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = []

	def Close(self):
		global g_isBuildingPrivateShop
		g_isBuildingPrivateShop = FALSE

		if self.priceInputBoard:
			self.priceInputBoard.Close()

		self.title = ""
		self.itemStock = {}
		shop.ClearPrivateShopStock()
		self.priceInputBoard = None;
		self.Hide()

		if self.interface:
			self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			self.interface.RefreshMarkInventoryBag()

		if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
			self.ItemListIdx = None
		elif app.ENABLE_SLOT_MARKING_SYSTEM:
			self.ItemListIdx = None

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def Refresh(self):
		getitemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setitemVNum=self.itemSlot.SetItemSlot
		delItem=self.itemSlot.ClearSlot

		for i in xrange(shop.SHOP_SLOT_COUNT):

			if not self.itemStock.has_key(i):
				delItem(i)
				continue

			pos = self.itemStock[i]

			itemCount = getItemCount(*pos)
			if itemCount <= 1:
				itemCount = 0
			setitemVNum(i, getitemVNum(*pos), itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				inventyp, pos = self.itemStock[i]
				if not player.GetChangeLookVnum(inventyp, pos) == 0:
					self.itemSlot.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.itemSlot.EnableSlotCoverImage(i, FALSE)

		self.itemSlot.RefreshSlot()

	def OnSelectEmptySlot(self, selectedSlotPos):

		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()

			if player.SLOT_TYPE_INVENTORY != attachedSlotType and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedSlotType and\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_STONE_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY != attachedSlotType and\
				player.SLOT_TYPE_CHANGERS_INVENTORY != attachedSlotType and app.ENABLE_SPECIAL_INVENTORY):
				return

			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)

			itemVNum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			item.SelectItem(itemVNum)

			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_CANNOT_SELL_ITEM)
				return

			priceInputBoard = uiCommon.MoneyInputDialog()
			priceInputBoard.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_PRICE_DIALOG_TITLE)
			priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
			priceInputBoard.Open()

			itemPrice=GetPrivateShopItemPrice(itemVNum)

			if itemPrice>0:
				priceInputBoard.SetValue(itemPrice)

			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.itemVNum = itemVNum
			self.priceInputBoard.sourceWindowType = attachedInvenType
			self.priceInputBoard.sourceSlotPos = attachedSlotPos
			self.priceInputBoard.targetSlotPos = selectedSlotPos

			if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
				if attachedInvenType in self.ItemListIdx:
					self.ItemListIdx[attachedInvenType].append(attachedSlotPos)
				self.priceInputBoard.Socket = self.GetItemDataSocketValue(attachedInvenType, attachedSlotPos)
			elif app.ENABLE_SLOT_MARKING_SYSTEM:
				self.ItemListIdx.append(attachedSlotPos)
				self.priceInputBoard.Socket = self.GetItemDataSocketValue(attachedInvenType, attachedSlotPos)

	def OnSelectItemSlot(self, selectedSlotPos):

		isAttached = mouseModule.mouseController.isAttached()
		if isAttached:
			snd.PlaySound("sound/ui/loginfail.wav")
			mouseModule.mouseController.DeattachObject()

		else:
			if not selectedSlotPos in self.itemStock:
				return

			invenType, invenPos = self.itemStock[selectedSlotPos]
			shop.DelPrivateShopItemStock(invenType, invenPos)
			snd.PlaySound("sound/ui/drop.wav")

			del self.itemStock[selectedSlotPos]

			if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
				if invenPos in self.ItemListIdx[invenType]:
					self.ItemListIdx[invenType].remove(invenPos)
			elif app.ENABLE_SLOT_MARKING_SYSTEM:
				for i in xrange(len(self.ItemListIdx)):
					if self.ItemListIdx[i] == invenPos:
						del self.ItemListIdx[i]
						break

			self.Refresh()

	def AcceptInputPrice(self):

		if not self.priceInputBoard:
			return TRUE

		text = self.priceInputBoard.GetText()

		if not text:
			return TRUE

		if not text.isdigit():
			return TRUE

		if app.ENABLE_GOLD_LIMIT:
			if long(text) <= 0:
				return TRUE
		else:
			if int(text) <= 0:
				return TRUE

		attachedInvenType = self.priceInputBoard.sourceWindowType
		sourceSlotPos = self.priceInputBoard.sourceSlotPos
		targetSlotPos = self.priceInputBoard.targetSlotPos

		for privatePos, (itemWindowType, itemSlotIndex) in self.itemStock.items():
			if itemWindowType == attachedInvenType and itemSlotIndex == sourceSlotPos:
				shop.DelPrivateShopItemStock(itemWindowType, itemSlotIndex)
				del self.itemStock[privatePos]

		price = int(self.priceInputBoard.GetText())

		if IsPrivateShopItemPriceList():
			SetPrivateShopItemPrice(self.priceInputBoard.itemVNum, price)

		shop.AddPrivateShopItemStock(attachedInvenType, sourceSlotPos, targetSlotPos, price)
		self.itemStock[targetSlotPos] = (attachedInvenType, sourceSlotPos)
		snd.PlaySound("sound/ui/drop.wav")

		self.Refresh()

		self.priceInputBoard = None
		return TRUE

	if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
		def CancelInputPrice(self):
			if self.inven and self.priceInputBoard:
				type = self.priceInputBoard.sourceWindowType
				pos = self.priceInputBoard.sourceSlotPos

				if not (pos in self.ItemListIdx[type]):
					return

				if not self.inven[type].wndItem:
					return

				self.ItemListIdx[type].remove(pos)

				page = self.inven[type].GetInventoryPageIndex()
				if type == player.INVENTORY:
					lock_idx = pos - (page * INVENTORY_PAGE_SIZE)
					self.inven[type].wndItem.SetCanMouseEventSlot(lock_idx)
				elif type == player.DRAGON_SOUL_INVENTORY:
					self.inven[type].SetCanMouseEventSlot(pos)

			self.priceInputBoard = None
	else:
		def CancelInputPrice(self):
			if app.ENABLE_SLOT_MARKING_SYSTEM:
				pos = self.priceInputBoard.sourceSlotPos
			for i in xrange(len(self.ItemListIdx)):
				if self.ItemListIdx[i] == pos:
					del self.ItemListIdx[i]
					break

			if app.ENABLE_SPECIAL_INVENTORY:
				if self.inven or self.invenSpecial:
					page = self.inven.GetInventoryPageIndex()
					special_page = self.invenSpecial.GetInventoryPageIndex()

					if (page * INVENTORY_PAGE_SIZE) <= pos < ((page + 1) * INVENTORY_PAGE_SIZE):
						idx = pos - (page * INVENTORY_PAGE_SIZE)
						self.inven.wndItem.SetCanMouseEventSlot(idx)

					if self.invenSpecial.GetInventoryType() == 0:
						if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= pos < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							idx = pos - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCanMouseEventSlot(idx)

					if self.invenSpecial.GetInventoryType() == 1:
						if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= pos < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							idx = pos - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCanMouseEventSlot(idx)

					if self.invenSpecial.GetInventoryType() == 2:
						if (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= pos < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							idx = pos - (item.STONE_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCanMouseEventSlot(idx)

					if self.invenSpecial.GetInventoryType() == 3:
						if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= pos < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							idx = pos - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCanMouseEventSlot(idx)

					if self.invenSpecial.GetInventoryType() == 4:
						if (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE)) <= pos < (item.CHANGERS_INVENTORY_SLOT_START + (((special_page + 1) * SPECIAL_INVENTORY_PAGE_SIZE))):
							idx = pos - (item.CHANGERS_INVENTORY_SLOT_START + (special_page * SPECIAL_INVENTORY_PAGE_SIZE))
							self.invenSpecial.wndItem.SetCanMouseEventSlot(idx)
			else:
				if self.inven:
					page = self.inven.GetInventoryPageIndex()

					if (page * INVENTORY_PAGE_SIZE) <= pos < ((page + 1) * INVENTORY_PAGE_SIZE):
						idx = pos - (page * INVENTORY_PAGE_SIZE)
						self.inven.wndItem.SetCanMouseEventSlot(idx)

			self.priceInputBoard = None
			return TRUE

	def OnOk(self):

		if not self.title:
			return

		if 0 == len(self.itemStock):
			return

		shop.BuildPrivateShop(self.title)
		self.Close()

	def OnClose(self):
		self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnOverInItem(self, slotIndex):
		if self.tooltipItem:
			if self.itemStock.has_key(slotIndex):
				self.tooltipItem.SetPrivateShopBuilderItem(*self.itemStock[slotIndex] + (slotIndex,))

	def OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
		def CantTradableItem(self, slotIndex, type=player.INVENTORY):
			if not (type in self.inven):
				return FALSE

			if not player.GetItemIndex(type, slotIndex):
				return FALSE

			if app.ENABLE_SEALBIND_SYSTEM:
				if player.GetItemSealDate(type, slotIndex) != -1:
					return TRUE

			if player.IsAntiFlagBySlot(type, slotIndex, item.ANTIFLAG_GIVE):
				return TRUE

			if player.IsAntiFlagBySlot(type, slotIndex, item.ANTIFLAG_MYSHOP):
				return TRUE

			return FALSE

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_PRIVATE_SHOP)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, type, inven):
			if not self.inven:
				self.inven[type] = inven
			elif not (type in self.inven):
				self.inven[type] = inven

		def OnUpdate(self):
			if self.ItemListIdx:
				for type in self.inven.keys():
					if not self.inven[type].wndItem:
						continue

					if type == player.INVENTORY:
						page = self.inven[type].GetInventoryPageIndex()
						for pos in self.ItemListIdx[type]:
							if (page * INVENTORY_PAGE_SIZE) <= pos < ((page + 1) * INVENTORY_PAGE_SIZE):
								lock_idx = pos - (page * INVENTORY_PAGE_SIZE)
								self.inven[type].wndItem.SetCantMouseEventSlot(lock_idx)
					elif type == player.DRAGON_SOUL_INVENTORY:
						for pos in self.ItemListIdx[type]:
							self.inven[type].SetCantMouseEventSlot(pos)

	elif app.ENABLE_SLOT_MARKING_SYSTEM:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				if itemIndex == 50200:
					return TRUE

				if player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE) or player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_MYSHOP):
					return TRUE

			return FALSE

		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def OnTop(self):
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_PRIVATE_SHOP)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, inven):
			self.inven = inven

		if app.ENABLE_SPECIAL_INVENTORY:
			def SetSpecialInven(self, invenSpecial):
				self.invenSpecial = invenSpecial

		def OnUpdate(self):
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

		def GetItemDataSocketValue(self, winType, SlotPos):
			for i in xrange(item.LIMIT_MAX_NUM):

				(limitType, limitValue) = item.GetLimit(i)
				if limitType == item.LIMIT_REAL_TIME or limitType == item.LIMIT_REAL_TIME_START_FIRST_USE or limitType == item.LIMIT_TIMER_BASED_ON_WEAR:
					return 0

			return player.GetItemMetinSocket(winType, SlotPos, 0)
