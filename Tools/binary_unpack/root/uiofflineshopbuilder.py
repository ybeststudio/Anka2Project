if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import localeInfo
import constInfo
import exception
import mouseModule
import systemSetting
import item
import snd
import shop
import chat
import renderTarget

import ui
import uiCommon
import uiToolTip

from _weakref import proxy

g_itemPriceDict = []
g_offlineShopAdvertisementBoardDict = {}
shop_all_Money = long(0)

def SetPrivateShopItemPrice(itemVNum, itemPrice, count):
	global g_itemPriceDict

	pcs_price = long(itemPrice)/count
	for_q = FALSE

	for i in xrange(len(g_itemPriceDict)):
		if g_itemPriceDict[i]["vnum"] == itemVNum and pcs_price != g_itemPriceDict[i]["price"]:
			g_itemPriceDict[i].update({"vnum":int(itemVNum),"price":long(pcs_price)})
			for_q = TRUE
			break

	if for_q == FALSE:
		g_itemPriceDict.append({"vnum":int(itemVNum),"price":long(pcs_price)})

def GetPrivateShopItemPrice(itemVNum, count):
	global g_itemPriceDict

	for i in xrange(len(g_itemPriceDict)):
		item = g_itemPriceDict[i]

		if item["vnum"] == itemVNum:
			return long(item["price"]*count)
	return 0

def Clear():
	global g_itemPriceDict
	global shop_all_Money
	global g_offlineShopAdvertisementBoardDict

	g_itemPriceDict = []
	shop_all_Money = long(0)

	for key in g_offlineShopAdvertisementBoardDict.keys():
		g_offlineShopAdvertisementBoardDict[key].Hide()
		del g_offlineShopAdvertisementBoardDict[key]

	g_offlineShopAdvertisementBoardDict = {}

def UpdateADText(vid,type,text):
	if g_offlineShopAdvertisementBoardDict.has_key(vid):
		item = g_offlineShopAdvertisementBoardDict[vid]

		if item == None:
			del g_offlineShopAdvertisementBoardDict[vid]
			return

		oldtext = item.textLine.GetText()
		oldType = item.type

		if oldType != type:
			DeleteADBoardwithKey(vid)
			board = OfflineShopAdvertisementBoard(type)
			board.Open(vid, text)
		elif oldtext != text:
			item.textLine.SetText(text)
			item.SetSize(len(text) * 6 + 80 * 2)
			item.Show()

def UpdateADBoard():
	global g_offlineShopAdvertisementBoardDict

	if not systemSetting.IsShowSalesText():
		for key in g_offlineShopAdvertisementBoardDict.keys():
			g_offlineShopAdvertisementBoardDict[key].Hide()
		return
	for key in g_offlineShopAdvertisementBoardDict.keys():
		g_offlineShopAdvertisementBoardDict[key].Show()

def HideADBoard():
	global g_offlineShopAdvertisementBoardDict

	for key in g_offlineShopAdvertisementBoardDict.keys():
		g_offlineShopAdvertisementBoardDict[key].Hide()

def HideADBoardWithKey(vid):
	if g_offlineShopAdvertisementBoardDict.has_key(vid):
		g_offlineShopAdvertisementBoardDict[vid].Hide()

def ShowADBoardWithKey(vid):
	if g_offlineShopAdvertisementBoardDict.has_key(vid):
		g_offlineShopAdvertisementBoardDict[vid].Show()
		return TRUE
	return FALSE

def DeleteADBoard():
	global g_offlineShopAdvertisementBoardDict

	for key in g_offlineShopAdvertisementBoardDict.keys():
		g_offlineShopAdvertisementBoardDict[key].Hide()
		del g_offlineShopAdvertisementBoardDict[key]

def DeleteADBoardwithKey(vid):
	if g_offlineShopAdvertisementBoardDict.has_key(vid):
		g_offlineShopAdvertisementBoardDict[vid].Hide()
		g_offlineShopAdvertisementBoardDict[vid].Destroy()
		g_offlineShopAdvertisementBoardDict[vid]=0
		del g_offlineShopAdvertisementBoardDict[vid]

class OfflineShopAdvertisementBoard(ui.ThinBoardNorm):
	def __init__(self, type):
		ui.ThinBoardNorm.__init__(self, "UI_BOTTOM", type)
		self.vid = None
		self.title = None
		self.type = int(type)
		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetWindowHorizontalAlignCenter()
		self.textLine.SetWindowVerticalAlignCenter()
		self.textLine.SetHorizontalAlignCenter()
		self.textLine.SetVerticalAlignCenter()

		if(self.type == 0):
			self.textLine.SetPosition(0, 0)
		else:
			self.textLine.SetPosition(-10, 5)

		self.textLine.Show()

	def __del__(self):
		ui.ThinBoardNorm.__del__(self)

	def SetBoard(self, a):
		ui.ThinBoardNorm.SetBoard(self, a)

	def Destroy(self):
		self.vid = 0
		self.title = 0
		self.type = 0
		self.textLine = 0
		self.Hide()

	def Open(self, vid, text):
		self.vid = vid
		self.title = text
		self.SetSize(len(text) * 6 + 80 * 2)
		self.textLine.SetText(text)
		self.textLine.Show()
		self.textLine.UpdateRect()
		DeleteADBoardwithKey(vid)
		g_offlineShopAdvertisementBoardDict[vid] = self
		self.Show()

	def OnMouseLeftButtonUp(self):
		if (not self.vid):
			return

		if chr.GetNameByVID(self.vid) == player.GetName():
			net.SendOfflineShopButton()
		else:
			net.SendOnClickPacket(self.vid)

		return TRUE

	def OnUpdate(self):
		if (not self.vid):
			DeleteADBoardwithKey(self.vid)
			return

		if not systemSetting.IsShowSalesText():
			self.SetPosition(-800, -300)
			return

		shopRange = [10000, 6000, 4000, 2000, 1000]
		try:
			LIMIT_RANGE = shopRange[systemSetting.GetShopNamesRange()]
		except:
			LIMIT_RANGE = shopRange[0]

		if chr.GetPixelPosition(self.vid) == None:
			DeleteADBoardwithKey(self.vid)
			return

		(to_x, to_y, to_z) = chr.GetPixelPosition(self.vid)
		(my_x, my_y, my_z) = player.GetMainCharacterPosition()

		if abs(my_x - to_x) <= LIMIT_RANGE and abs(my_y - to_y) <= LIMIT_RANGE:
			(x, y) = chr.GetProjectPosition(self.vid, 220)
			x_new = (x-self.GetWidth()/2)
			y_new = (y-self.GetHeight()/2)
			self.SetPosition(x_new, y_new)
			self.Show()
		else:
			self.SetPosition(-800, -300)

class OfflineShopBuilder(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.itemStock = {}
		self.priceInputBoard = None
		self.title = ""
		self.thinboard = None
		self.playerChoose = None

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.shopVnum = 0
		self.shopTitle = 0
		self.isLoaded = 0
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/OfflineShopBuilder.py")

			self.GetChild("FirstButton").SetEvent(ui.__mem_func__(self.CheckPlayerAccept))
			self.GetChild("SecondButton").SetEvent(ui.__mem_func__(self.CloseReal))

			self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.CloseReal))

			self.GetChild("ItemSlot").SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
			self.GetChild("ItemSlot").SAFE_SetButtonEvent("LEFT", "EXIST", self.OnSelectItemSlot)
			self.GetChild("ItemSlot").SAFE_SetButtonEvent("RIGHT", "EXIST", self.OnSelectItemSlot)
			self.GetChild("ItemSlot").SetOverInItemEvent(ui.__mem_func__(self.OnOverInItem))
			self.GetChild("ItemSlot").SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))

			self.GetChild("ItemSlot2").SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlotNew))
			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("LEFT", "EXIST", self.OnSelectItemSlotNew)
			self.GetChild("ItemSlot2").SAFE_SetButtonEvent("RIGHT", "EXIST", self.OnSelectItemSlotNew)
			self.GetChild("ItemSlot2").SetOverInItemEvent(ui.__mem_func__(self.OnOverInItemNew))
			self.GetChild("ItemSlot2").SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))

			self.GetChild("title_back_btn").SetEvent(ui.__mem_func__(self.TitleBack))
			self.GetChild("title_next_btn").SetEvent(ui.__mem_func__(self.TitleNext))
			self.GetChild("shop_back_btn").SetEvent(ui.__mem_func__(self.ShopBack))
			self.GetChild("shop_next_btn").SetEvent(ui.__mem_func__(self.ShopNext))
			self.GetChild("shop_back_btn").Hide()
			self.GetChild("title_back_btn").Hide()

			self.GetChild("NameLine").OnPressEscapeKey = ui.__mem_func__(self.CloseReal)

			renderTarget.SetBackground(1, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetScale(1, 0.7)
			renderTarget.SetVisibility(1, TRUE)
		except:
			exception.Abort("OfflineShopBuilderWindow.LoadWindow.LoadObject")

	def Destroy(self):
		self.ClearDictionary()

		if self.thinboard:
			self.thinboard.Destroy()

		self.thinboard = 0
		self.itemStock.clear()
		self.itemStock = 0
		self.priceInputBoard = 0
		self.title = 0
		self.shopVnum = 0
		self.shopTitle = 0
		self.playerChoose =None
		self.isLoaded = 0
		self.interface = None
		self.Hide()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def Open(self, title):
		self.title = title
		self.itemStock = {}

		shop.ClearOfflineShopStock()
		self.GetChild("NameLine").SetText(title)

		self.shopVnum = 0
		self.shopTitle = 0
		self.GetChild("shop_back_btn").Hide()
		self.GetChild("shop_next_btn").Show()
		self.GetChild("title_back_btn").Hide()
		self.GetChild("title_next_btn").Show()

		self.SetYang(0, TRUE)
		self.Refresh()

		self.Show()
		self.SetCenterPosition()
		self.SetTop()

	def CloseReal(self):
		self.Close()
		net.SendOfflineShopEndPacket()
		return TRUE

	def Close(self):
		global shop_all_Money

		shop_all_Money = 0
		renderTarget.SetVisibility(1,FALSE)
		shop.ClearOfflineShopStock()

		if self.priceInputBoard:
			self.priceInputBoard.Close()
			self.priceInputBoard = None

		if self.playerChoose:
			self.playerChoose.Close()
			self.playerChoose = None

		self.Hide()

	def Refresh(self):
		pointer = [self.GetChild("ItemSlot"), self.GetChild("ItemSlot2")]
		for ptr in pointer:
			for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT/2):
				ptr.SetCoverButton(i, "d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub","d:/ymir work/ui/game/quest/slot_button_01.sub", "d:/ymir work/ui/pattern/slot_disable.tga", FALSE, FALSE)

		for ptr in pointer:
			index = pointer.index(ptr) + 1
			for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT/2):
				real_index = i
				if index == 2:
					real_index += 40

					if not constInfo.IS_SET(shop.GetShopFlag(), 1 << i):
						ptr.SetItemSlot(i, 50300, 0)
						ptr.SetCoverButton(i, "d:/ymir work/ui/game/offlineshop/lock_0.tga", "d:/ymir work/ui/game/offlineshop/lock_1.tga", "d:/ymir work/ui/game/offlineshop/lock_2.tga", "d:/ymir work/ui/public/slot_cover_button_04.sub", 1, 0)
						ptr.EnableSlot(i)
						continue

				if (not self.itemStock.has_key(real_index)):
					ptr.ClearSlot(i)
					continue

				pos = self.itemStock[real_index]
				itemCount = player.GetItemCount(*pos)
				if (itemCount <= 1):
					itemCount = 0

				ptr.SetItemSlot(i, player.GetItemIndex(*pos), itemCount)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					changelookvnum = player.GetChangeLookVnum(*pos)
					if not changelookvnum == 0:
						ptr.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
					else:
						ptr.EnableSlotCoverImage(i, FALSE)

				ptr.EnableCoverButton(i)

			ptr.RefreshSlot()

		self.ChangeThinboard(self.shopTitle)
		renderTarget.SelectModel(1, (30000 + self.shopVnum))
		renderTarget.SetVisibility(1, TRUE)

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
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
			priceInputBoard.Open()

			(itemPrice) = GetPrivateShopItemPrice(itemVNum,count)

			priceInputBoard.SetValue(itemPrice)

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
			priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
			priceInputBoard.Open()

			(itemPrice) = GetPrivateShopItemPrice(itemVNum,count)

			priceInputBoard.SetValue(itemPrice)

			self.priceInputBoard = priceInputBoard
			self.priceInputBoard.itemVNum = itemVNum
			self.priceInputBoard.sourceWindowType = attachedInvenType
			self.priceInputBoard.sourceSlotPos = attachedSlotPos
			self.priceInputBoard.targetSlotPos = selectedSlotPos + 40

	def OnSelectItemSlot(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if (isAttached):
			snd.PlaySound("sound/ui/loginfail.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			if (not selectedSlotPos in self.itemStock):
				return

			if app.IsPressed(app.DIK_LCONTROL):
				if self.interface:
					self.interface.OpenRenderTargetWindow(0, player.GetItemIndex(*self.itemStock[selectedSlotPos]))

			invenType, invenPos = self.itemStock[selectedSlotPos]
			self.SetYang(0, FALSE, invenType, invenPos)
			shop.DelOfflineShopItemStock(invenType, invenPos)
			snd.PlaySound("sound/ui/drop.wav")
			del self.itemStock[selectedSlotPos]
			self.Refresh()

	def OnSelectItemSlotNew(self, selectedSlotPos):
		isAttached = mouseModule.mouseController.isAttached()
		if (isAttached):
			snd.PlaySound("sound/ui/loginfail.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			if not constInfo.IS_SET(shop.GetShopFlag(), 1 << selectedSlotPos):
				priceInputBoard = uiCommon.QuestionDialog("thin")
				priceInputBoard.SetText(localeInfo.OFFLINESHOP_DO_YOU_WANT_SLOT)
				priceInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptOpenSlot))
				priceInputBoard.pos = selectedSlotPos
				priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
				priceInputBoard.Open()
				self.priceInputBoard = priceInputBoard
				return

			selectedSlotPos += 40
			if (not selectedSlotPos in self.itemStock):
				return

			if app.IsPressed(app.DIK_LCONTROL):
				if self.interface:
					self.interface.OpenRenderTargetWindow(0, player.GetItemIndex(*self.itemStock[selectedSlotPos]))

			invenType, invenPos = self.itemStock[selectedSlotPos]
			self.SetYang(0, FALSE, invenType, invenPos)
			shop.DelOfflineShopItemStock(invenType, invenPos)
			snd.PlaySound("sound/ui/drop.wav")
			del self.itemStock[selectedSlotPos]
			self.Refresh()

	def AcceptOpenSlot(self):
		if constInfo.IS_SET(shop.GetShopFlag(), 1 << self.priceInputBoard.pos):
			return

		net.SendOpenShopSlot(self.priceInputBoard.pos)
		self.priceInputBoard = None
		self.Refresh()

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

		itemVNum = player.GetItemIndex(attachedInvenType, sourceSlotPos)
		count = player.GetItemCount(attachedInvenType, sourceSlotPos)

		if itemVNum != self.priceInputBoard.itemVNum:
			self.priceInputBoard.Close()
			self.priceInputBoard=None
			return

		SetPrivateShopItemPrice(self.priceInputBoard.itemVNum, price, count)
		self.SetYang(price, TRUE)

		for privatePos, (itemWindowType, itemSlotIndex) in self.itemStock.items():
			if (itemWindowType == attachedInvenType and itemSlotIndex == sourceSlotPos):
				self.SetYang(0, FALSE, itemWindowType, itemSlotIndex)
				shop.DelOfflineShopItemStock(itemWindowType, itemSlotIndex)
				del self.itemStock[privatePos]

		shop.AddOfflineShopItemStock(attachedInvenType, sourceSlotPos, targetSlotPos, price)
		self.itemStock[targetSlotPos] = (attachedInvenType, sourceSlotPos)
		snd.PlaySound("sound/ui/drop.wav")

		if self.priceInputBoard:
			self.priceInputBoard.Close()
			self.priceInputBoard = None

		self.Refresh()
		return TRUE

	def CancelInputPrice(self):
		if self.priceInputBoard:
			self.priceInputBoard.Close()

		self.priceInputBoard = None
		return 1

	def CancelPlayerChoose(self):
		if self.playerChoose:
			self.playerChoose.Close()

		self.playerChoose = None

	def CheckPlayerAccept(self):
		if self.playerChoose != None:
			self.CancelPlayerChoose()

		playerChoose = uiCommon.QuestionDialog("thin")
		playerChoose.SetText(localeInfo.OFFLINESHOP_DO_YOU_WANT_OPEN % localeInfo.NumberToDecimalString(2000000))
		playerChoose.SetAcceptEvent(ui.__mem_func__(self.AcceptPlayerChoose))
		playerChoose.SetCancelEvent(ui.__mem_func__(self.CancelPlayerChoose))
		playerChoose.Open()
		self.playerChoose = playerChoose

	def AcceptPlayerChoose(self):
		self.OnOk()
		self.CancelPlayerChoose()

	def OnOk(self):
		self.title = self.GetChild("NameLine").GetText()
		if (not self.title):
			return

		if (len(self.itemStock) == 0):
			return
		elif net.IsChatInsultIn(self.title):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHAT_INSULT_STRING)
			return
		elif constInfo.getInjectCheck(self.title):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHAT_INSULT_STRING)
			return

		shop.BuildOfflineShop(self.title, 30000 + self.shopVnum, self.shopTitle)
		self.Close()

	def OnPressEscapeKey(self):
		self.CloseReal()
		return TRUE

	def OnPressExitKey(self):
		self.CloseReal()
		return TRUE

	def OnOverInItem(self, slotIndex):
		if None == self.tooltipItem:
			return

		if self.itemStock.has_key(slotIndex):
			self.tooltipItem.SetOfflineShopBuilderItem(*self.itemStock[slotIndex] + (slotIndex,))

	def OnOverInItemNew(self, slotIndex):
		if None == self.tooltipItem:
			return

		slotIndex += 40
		if self.itemStock.has_key(slotIndex):
			self.tooltipItem.SetOfflineShopBuilderItem(*self.itemStock[slotIndex] + (slotIndex,))

	def OnOverOutItem(self):
		if None == self.tooltipItem:
			return

		self.tooltipItem.HideToolTip()

	def SetYang(self, value, flag, invenType = 0, invenPos = 0):
		global shop_all_Money
		long_value = long(value)

		if flag == TRUE:
			shop_all_Money += long_value
			self.GetChild("Money").SetText("%s" % localeInfo.NumberToMoneyString(shop_all_Money))
		else:
			p = shop.GetOfflineShopItemPriceReal(invenType, invenPos)
			shop_all_Money -= long(p)
			self.GetChild("Money").SetText("%s" % localeInfo.NumberToMoneyString(shop_all_Money))

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

	def TitleBack(self):
		if self.shopTitle <= 0:
			return

		self.shopTitle -= 1
		self.GetChild("title_next_btn").Show()

		if self.shopTitle == 0:
			self.GetChild("title_back_btn").Hide()

		self.ChangeThinboard(self.shopTitle)

	def TitleNext(self):
		if self.shopTitle +1 == constInfo.MAX_SHOP_TITLE:
			return

		self.shopTitle += 1
		self.GetChild("title_back_btn").Show()

		if self.shopTitle + 1 == constInfo.MAX_SHOP_TITLE:
			self.GetChild("title_next_btn").Hide()

		self.ChangeThinboard(self.shopTitle)

	def ShopBack(self):
		if self.shopVnum <= 0:
			return

		self.shopVnum -= 1
		self.GetChild("shop_next_btn").Show()

		if self.shopVnum == 0:
			self.GetChild("shop_back_btn").Hide()

		renderTarget.SelectModel(1, (30000 + self.shopVnum))
		renderTarget.SetVisibility(1, TRUE)

	def ShopNext(self):
		if self.shopVnum +1 == constInfo.MAX_SHOP_TYPE:
			return

		self.shopVnum += 1
		self.GetChild("shop_back_btn").Show()

		if self.shopVnum +1 == constInfo.MAX_SHOP_TYPE:
			self.GetChild("shop_next_btn").Hide()

		renderTarget.SelectModel(1, (30000 + self.shopVnum))
		renderTarget.SetVisibility(1, TRUE)
