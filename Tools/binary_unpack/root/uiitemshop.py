if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import item
import os
import constInfo
import localeInfo

import uiToolTip

from _weakref import proxy

itemshopData = {}
playerLog = []

UPDATE_TIME = -1
ITEM_COLUMNS = 3

def ItemShopLogClear():
	global playerLog
	playerLog = []

def ItemShopAppendLog(dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
	dateTimeReal = "None"
	dateSplit = dateText.split(" ")
	if len(dateSplit) > 1:
		dateFirst = dateSplit[0].split("-")
		if len(dateFirst) == 3:
			dateTimeReal = dateFirst[2]+"/"+dateFirst[1]+"/"+dateFirst[0]
			dateTimeReal+=" "
			dateSecond = dateSplit[1].split(":")
			if len(dateSecond) == 3:
				dateTimeReal += dateSecond[0]+":"+dateSecond[1]

	global playerLog
	playerLog.append([dateTimeReal,dateTime,playerName,ipAdress,itemVnum,itemCount,itemPrice])

def ItemShopClear(time):
	global itemshopData
	global UPDATE_TIME
	if time != UPDATE_TIME:
		itemshopData.clear()
		UPDATE_TIME = time

def ItemShopAppendItem(categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount):
	if not itemshopData.has_key(categoryIndex):
		subCategoryList=[]
		for x in xrange(10):
			subCategoryList.append([])
		itemshopData[categoryIndex] = subCategoryList
	itemshopData[categoryIndex][categorySubIndex].append([itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount])

def ItemShopUpdateItem(itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount):
	global itemshopData
	for key, items in itemshopData.items():
		for subCategory in items:
			for shopItem in subCategory:
				if shopItem[0] == itemID:
					shopItem = [itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount]
					return

buyQuestionDialog = None

LIST_ITEM_ID = 0
LIST_ITEM_VNUM = 1
LIST_ITEM_PRICE = 2
LIST_ITEM_DISCOUNT = 3
LIST_ITEM_TIME = 4
LIST_ITEM_TOP_SELLING_INDEX = 5
LIST_ITEM_ADDED_TIME = 6
LIST_ITEM_SELLING_COUNT = 7
LIST_ITEM_MAX_SELLING_COUNT = 8

LOG_DATE_TEXT = 0
LOG_DATE = 1
LOG_ACCOUNT = 2
LOG_IP = 3
LOG_ITEM = 4
LOG_COUNT = 5
LOG_PRICE = 6


HOT_ITEM = 1
NEW_ITEM = 2
OFFER_ITEM = 3

IMG_DIR ="d:/ymir work/ui/game/ishop/"

WHEEL_DIR ="d:/ymir work/ui/game/ishop/wheel/default/"
wheel_random_items = [\
	[71084, 5], [71084, 10], [71084, 10], [71084, 20], [71084, 20], [71084, 100], [70005, 1], [25041, 1], [25041, 2], [72319, 1], [72319, 2]\
	,[71035, 1], [70043, 1], [72320, 1], [72320, 2], [71155, 1], [85001, 1], [80003, 1], [70063, 1], [70064, 1], [84014, 1]
]

itemShopCategoryData = {
	0:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_0_NAME,
		"subCategory" : [localeInfo.ITEMSHOP_CATEGORY_0_SUB_0_NAME, localeInfo.ITEMSHOP_CATEGORY_0_SUB_1_NAME],
		},
	1:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_1_NAME,
		"subCategory" : [],
		},
	2:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_2_NAME,
		"subCategory" : [localeInfo.ITEMSHOP_CATEGORY_2_SUB_0_NAME, localeInfo.ITEMSHOP_CATEGORY_2_SUB_1_NAME, localeInfo.ITEMSHOP_CATEGORY_2_SUB_2_NAME, localeInfo.ITEMSHOP_CATEGORY_2_SUB_3_NAME, localeInfo.ITEMSHOP_CATEGORY_2_SUB_4_NAME],
		},
	3:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_3_NAME,
		"subCategory" : [localeInfo.ITEMSHOP_CATEGORY_3_SUB_0_NAME, localeInfo.ITEMSHOP_CATEGORY_3_SUB_1_NAME, localeInfo.ITEMSHOP_CATEGORY_3_SUB_2_NAME],
		},
	4:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_4_NAME,
		"subCategory" : [],
		},
	5:{
		"name" : localeInfo.ITEMSHOP_CATEGORY_5_NAME,
		"subCategory" : [],
		},
}

class ItemShopWindow(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		ItemShopLogClear()
		if self.children.has_key("categoryListBox"):
			self.children["categoryListBox"].RemoveAllItems()

		if self.children.has_key("itemListBox"):
			self.children["itemListBox"].RemoveAllItems()

		if self.children.has_key("PurchasesListBox"):
			self.children["PurchasesListBox"].RemoveAllItems()

		self.CloseWeb()

		global buyQuestionDialog
		if buyQuestionDialog != None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None

		self.RealMoney = 0
		self.NewMoney = 0
		self.MoneyRange = 0
		self.MoneyFlag = FALSE
		self.moneySize = 0

		self.interface = None

		self.children = {}

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.children = {}
		self.Destroy()
		
		# Nesne market genel item tooltip penceresi (tek, paylaþýmlý) - LoadWindow'dan önce oluþturulmalý
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.HideToolTip()
		# Tooltip'in fareyi takip etmemesini saðla
		self.tooltipItem.SetFollow(FALSE)
		
		self.LoadWindow()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def LoadWindow(self):
		self.SetTitleName(localeInfo.ITEMSHOP_WINDOW_TITLE)
		self.SetCloseEvent(self.Close)
		self.SetSize(995, 560)
		self.AddFlag("attach")
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetCenterPosition()

		boardImage = ui.ImageBox()
		boardImage.SetParent(self)
		boardImage.AddFlag("attach")
		boardImage.LoadImage(IMG_DIR + "background.tga")
		boardImage.SetPosition(10, 35)
		boardImage.Show()
		self.children["boardImage"] = boardImage

		leftDragonCoinImage = ui.ImageBox()
		leftDragonCoinImage.SetParent(boardImage)
		leftDragonCoinImage.LoadImage(IMG_DIR + "coin.tga")
		leftDragonCoinImage.SetPosition(25, 17)
		leftDragonCoinImage.Show()
		self.children["leftDragonCoinImage"] = leftDragonCoinImage

		dragonCoinText = MultiTextLine()
		dragonCoinText.SetParent(boardImage)
		dragonCoinText.SetPosition(35+leftDragonCoinImage.GetWidth(), 10)
		dragonCoinText.SetTextRange(17)
		dragonCoinText.SetTextType("horizontal#left")
		dragonCoinText.SetFontName("Tahoma:14")
		dragonCoinText.SetOutline(TRUE)
		dragonCoinText.Show()
		self.children["dragonCoinText"] = dragonCoinText

		buyCoinBtn = ui.Button()
		buyCoinBtn.SetParent(boardImage)
		buyCoinBtn.SetUpVisual(IMG_DIR + "button1_0.tga")
		buyCoinBtn.SetOverVisual(IMG_DIR + "button1_1.tga")
		buyCoinBtn.SetDownVisual(IMG_DIR + "button1_2.tga")
		buyCoinBtn.SetEvent(self.BuyDragonCoin)
		buyCoinBtn.SetPosition(180, 8)
		buyCoinBtn.Show()
		self.children["buyCoinBtn"] = buyCoinBtn

		buyCoinBtnText = MultiTextLine()
		buyCoinBtnText.SetParent(buyCoinBtn)
		buyCoinBtnText.SetTextRange(17)
		buyCoinBtnText.SetPosition(buyCoinBtn.GetWidth()/2, 10)
		buyCoinBtnText.SetTextType("horizontal#center")
		buyCoinBtnText.SetFontName("Tahoma:16")
		buyCoinBtnText.SetText(localeInfo.ITEMSHOP_MENU_TITLE_HOME)
		buyCoinBtnText.Show()
		buyCoinBtn.multiText = buyCoinBtnText

		wheelBtn = ui.Button()
		wheelBtn.SetParent(boardImage)
		wheelBtn.SetUpVisual(IMG_DIR + "button1_0.tga")
		wheelBtn.SetOverVisual(IMG_DIR + "button1_1.tga")
		wheelBtn.SetDownVisual(IMG_DIR + "button1_2.tga")
		wheelBtn.SetEvent(self.WheelBtn)
		wheelBtn.SetPosition(180+buyCoinBtn.GetWidth()+7, 8)
		wheelBtn.Show()
		self.children["wheelBtn"] = wheelBtn

		wheelBtnText = MultiTextLine()
		wheelBtnText.SetParent(wheelBtn)
		wheelBtnText.SetTextRange(17)
		wheelBtnText.SetPosition(wheelBtn.GetWidth()/2, 10)
		wheelBtnText.SetTextType("horizontal#center")
		wheelBtnText.SetFontName("Tahoma:16")
		wheelBtnText.SetText(localeInfo.ITEMSHOP_MENU_TITLE_WHEEL)
		wheelBtnText.Show()
		wheelBtn.multiText = wheelBtnText

		purchasesBtn = ui.Button()
		purchasesBtn.SetParent(boardImage)
		purchasesBtn.SetUpVisual(IMG_DIR + "button2_0.tga")
		purchasesBtn.SetOverVisual(IMG_DIR + "button2_1.tga")
		purchasesBtn.SetDownVisual(IMG_DIR + "button2_2.tga")
		purchasesBtn.SetEvent(self.PurchaseBtn)
		purchasesBtn.SetPosition(180+((buyCoinBtn.GetWidth()+7)*2), 8)
		purchasesBtn.Show()
		self.children["purchasesBtn"] = purchasesBtn

		purchasesBtnText = MultiTextLine()
		purchasesBtnText.SetParent(purchasesBtn)
		purchasesBtnText.SetTextRange(17)
		purchasesBtnText.SetPosition(purchasesBtn.GetWidth()/2, 10)
		purchasesBtnText.SetTextType("horizontal#center")
		purchasesBtnText.SetFontName("Tahoma:16")
		purchasesBtnText.SetText(localeInfo.ITEMSHOP_PURCHASES)
		purchasesBtnText.Show()
		purchasesBtn.multiText = purchasesBtnText

		hotItemsBtn = ui.Button()
		hotItemsBtn.SetParent(boardImage)
		hotItemsBtn.SetUpVisual(IMG_DIR + "button2_0.tga")
		hotItemsBtn.SetOverVisual(IMG_DIR + "button2_1.tga")
		hotItemsBtn.SetDownVisual(IMG_DIR + "button2_2.tga")
		hotItemsBtn.SetEvent(self.HotItems)
		hotItemsBtn.SetPosition(180+((buyCoinBtn.GetWidth()+7)*3), 8)
		hotItemsBtn.Show()
		self.children["hotItemsBtn"] = hotItemsBtn

		hotItemsBtnText = MultiTextLine()
		hotItemsBtnText.SetParent(hotItemsBtn)
		hotItemsBtnText.SetTextRange(17)
		hotItemsBtnText.SetPosition(hotItemsBtn.GetWidth()/2, 10)
		hotItemsBtnText.SetTextType("horizontal#center")
		hotItemsBtnText.SetFontName("Tahoma:16")
		hotItemsBtnText.SetText(localeInfo.ITEMSHOP_MENU_TITLE_HOTITEMS)
		hotItemsBtnText.Show()
		hotItemsBtn.multiText = hotItemsBtnText

		offersBtn = ui.Button()
		offersBtn.SetParent(boardImage)
		offersBtn.SetUpVisual(IMG_DIR + "button2_0.tga")
		offersBtn.SetOverVisual(IMG_DIR + "button2_1.tga")
		offersBtn.SetDownVisual(IMG_DIR + "button2_2.tga")
		offersBtn.SetEvent(self.OfferItems)
		offersBtn.SetPosition(180+((buyCoinBtn.GetWidth()+7)*4), 8)
		offersBtn.Show()
		self.children["offersBtn"] = offersBtn

		offersBtnText = MultiTextLine()
		offersBtnText.SetParent(offersBtn)
		offersBtnText.SetTextRange(17)
		offersBtnText.SetPosition(offersBtn.GetWidth()/2, 10)
		offersBtnText.SetTextType("horizontal#center")
		offersBtnText.SetFontName("Tahoma:16")
		offersBtnText.SetText(localeInfo.ITEMSHOP_MENU_TITLE_OFFERS)
		offersBtnText.Show()
		offersBtn.multiText = offersBtnText

		itemSearchBg = ui.ImageBox()
		itemSearchBg.SetParent(boardImage)
		itemSearchBg.LoadImage(IMG_DIR + "input_search.tga")
		itemSearchBg.SetPosition(730, 8)
		itemSearchBg.Show()
		self.children["itemSearchBg"] = itemSearchBg

		itemSearch = ui.EditLine()
		itemSearch.SetParent(itemSearchBg)
		itemSearch.SetMax(40)
		itemSearch.SetPosition(16, 10)
		itemSearch.SetSize(itemSearchBg.GetWidth()-16, itemSearchBg.GetHeight()-10)
		itemSearch.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		itemSearch.OnPressEscapeKey = ui.__mem_func__(self.Close)
		itemSearch.Show()
		self.children["itemSearch"] = itemSearch

		itemSearchText = MultiTextLine()
		itemSearchText.SetParent(itemSearch)
		itemSearchText.SetTextRange(15)
		itemSearchText.SetPosition(0, -2)
		itemSearchText.SetFontColor(128, 128, 128)
		itemSearchText.SetTextType("horizontal#left")
		itemSearchText.SetFontName("Tahoma:16")
		itemSearchText.SetText(localeInfo.ITEMSHOP_ITEMSEARCH_DEFAULT_TEXT)
		itemSearchText.Show()
		itemSearch.backText = itemSearchText

		categoryScrollBar = ScrollBar()
		categoryScrollBar.SetParent(boardImage)
		categoryScrollBar.SetPosition(169, 66)
		categoryScrollBar.SetScrollBarSize(435)
		categoryScrollBar.SetScrollStep(0.07)
		categoryScrollBar.Hide()
		self.children["categoryScrollBar"] = categoryScrollBar

		categoryListBox = ListBox()
		categoryListBox.SetParent(boardImage)
		categoryListBox.SetPosition(10, 66)
		categoryListBox.SetSize(150, 55*8)
		categoryListBox.SetItemStep(2)
		categoryListBox.SetItemSize(150, 55)
		categoryListBox.SetScrollBar(categoryScrollBar)
		categoryListBox.Show()
		self.children["categoryListBox"] = categoryListBox

		itemScrollBar = ScrollBar()
		itemScrollBar.SetParent(boardImage)
		itemScrollBar.SetPosition(200+755, 66)
		itemScrollBar.SetScrollBarSize(435)
		itemScrollBar.SetScrollStep(0.07)
		itemScrollBar.Show()
		self.children["itemScrollBar"] = itemScrollBar

		itemListBox = ListBox()
		itemListBox.SetParent(boardImage)
		itemListBox.SetPosition(190, 66)
		itemListBox.SetSize(750, 445)
		itemListBox.SetItemStep(8)
		itemListBox.SetItemSize(750, 122)
		itemListBox.SetScrollBar(itemScrollBar)
		itemListBox.Show()
		self.children["itemListBox"] = itemListBox

		PurchasesScrollBar = ScrollBar()
		PurchasesScrollBar.SetParent(boardImage)
		PurchasesScrollBar.SetPosition(200+750+5, 66)
		PurchasesScrollBar.SetScrollBarSize(435)
		PurchasesScrollBar.SetScrollStep(0.07)
		self.children["PurchasesScrollBar"] = PurchasesScrollBar

		PurchasesListBox = ListBox()
		PurchasesListBox.SetParent(boardImage)
		PurchasesListBox.SetPosition(16, 65)
		PurchasesListBox.SetSize(927, 440)
		PurchasesListBox.SetItemStep(8)
		PurchasesListBox.SetItemSize(927, 28)
		PurchasesListBox.SetScrollBar(PurchasesScrollBar)
		self.children["PurchasesListBox"] = PurchasesListBox

		LoadingImage = ui.ExpandedImageBox()
		LoadingImage.SetParent(boardImage)
		LoadingImage.SetPosition(boardImage.GetWidth()/2,boardImage.GetHeight()/2)
		LoadingImage.LoadImage(IMG_DIR + "load_.tga")
		self.children["LoadingImage"] = LoadingImage

		self.children["LoadingImageRotation"] = 0

		wheelBg = ui.ImageBox()
		wheelBg.SetParent(self)
		wheelBg.AddFlag("attach")
		wheelBg.LoadImage(WHEEL_DIR + "wallpaper.tga")
		wheelBg.SetPosition(10, 86)
		wheelBg.Hide()
		self.children["wheelBg"] = wheelBg

		wheelBgDesign = ui.ImageBox()
		wheelBgDesign.AddFlag("attach")
		wheelBgDesign.SetParent(wheelBg)
		wheelBgDesign.LoadImage(WHEEL_DIR + "wheel_bg.tga")
		wheelBgDesign.SetAlpha(0.5)
		wheelBgDesign.Show()
		self.children["wheelBgDesign"] = wheelBgDesign

		wheelSpinCheck = ui.ToggleButton()
		wheelSpinCheck.SetParent(wheelBg)
		wheelSpinCheck.SetUpVisual(WHEEL_DIR + "no_selected.tga")
		wheelSpinCheck.SetOverVisual(WHEEL_DIR + "no_selected.tga")
		wheelSpinCheck.SetDownVisual(WHEEL_DIR + "selected.tga")
		wheelSpinCheck.SetPosition(27, 24)
		wheelSpinCheck.SetToggleUpEvent(self.SetSpintAnimation, FALSE)
		wheelSpinCheck.SetToggleDownEvent(self.SetSpintAnimation, TRUE)
		wheelSpinCheck.Show()
		self.children["wheelSpinCheck"] = wheelSpinCheck

		wheelSpinCheckText = MultiTextLine()
		wheelSpinCheckText.SetParent(wheelBg)
		wheelSpinCheckText.SetPosition(52, 25)
		wheelSpinCheckText.SetTextRange(17)
		wheelSpinCheckText.SetTextType("horizontal#left")
		wheelSpinCheckText.SetFontName("Tahoma:14")
		wheelSpinCheckText.SetOutline(TRUE)
		wheelSpinCheckText.Show()
		self.children["wheelSpinCheckText"] = wheelSpinCheckText

		whellTicketBtn = ui.RadioButton()
		whellTicketBtn.SetParent(wheelBg)
		whellTicketBtn.SetUpVisual(WHEEL_DIR + "no_selected.tga")
		whellTicketBtn.SetOverVisual(WHEEL_DIR + "no_selected.tga")
		whellTicketBtn.SetDownVisual(WHEEL_DIR + "selected.tga")
		whellTicketBtn.SetPosition(797, 400)
		whellTicketBtn.SetEvent(self.SetTicketMode, 0)
		whellTicketBtn.Show()
		self.children["whellTicketBtn"] = whellTicketBtn

		whellTicketBtnText = MultiTextLine()
		whellTicketBtnText.SetParent(whellTicketBtn)
		whellTicketBtnText.SetPosition(29, -1)
		whellTicketBtnText.SetTextRange(17)
		whellTicketBtnText.SetTextType("horizontal#left")
		whellTicketBtnText.SetFontName("Tahoma:14")
		whellTicketBtnText.SetOutline(TRUE)
		whellTicketBtnText.SetText(localeInfo.ITEMSHOP_WHEEL_TICKET_TEXT)
		whellTicketBtnText.Show()
		whellTicketBtn.whellTicketBtnText = whellTicketBtnText

		whellCoinBtn = ui.RadioButton()
		whellCoinBtn.SetParent(wheelBg)
		whellCoinBtn.SetUpVisual(WHEEL_DIR + "no_selected.tga")
		whellCoinBtn.SetOverVisual(WHEEL_DIR + "no_selected.tga")
		whellCoinBtn.SetDownVisual(WHEEL_DIR + "selected.tga")
		whellCoinBtn.SetPosition(797, 430)
		whellCoinBtn.SetEvent(self.SetTicketMode, 1)
		whellCoinBtn.Show()
		self.children["whellCoinBtn"] = whellCoinBtn

		whellCoinBtnText = MultiTextLine()
		whellCoinBtnText.SetParent(whellCoinBtn)
		whellCoinBtnText.SetPosition(29, -1)
		whellCoinBtnText.SetTextRange(17)
		whellCoinBtnText.SetTextType("horizontal#left")
		whellCoinBtnText.SetFontName("Tahoma:14")
		whellCoinBtnText.SetOutline(TRUE)
		whellCoinBtnText.SetText(localeInfo.ITEMSHOP_WHEEL_COIN_TEXT)
		whellCoinBtnText.Show()
		whellTicketBtn.whellCoinBtnText = whellCoinBtnText

		fortuneImg = ui.ImageBox()
		fortuneImg.SetParent(wheelBg)
		fortuneImg.AddFlag("not_pick")
		fortuneImg.LoadImage(WHEEL_DIR + "fortune.tga")
		fortuneImg.SetPosition(308, 43)
		fortuneImg.Show()
		self.children["fortuneImg"] = fortuneImg

		fortuneSingle = ui.ExpandedImageBox()
		fortuneSingle.SetParent(fortuneImg)
		fortuneSingle.AddFlag("not_pick")
		fortuneSingle.LoadImage(WHEEL_DIR+"single.tga")
		fortuneSingle.SetPosition(125, 6)
		fortuneSingle.Show()
		self.children["fortuneSingle"] = fortuneSingle

		wheelCircle = ui.ExpandedImageBox()
		wheelCircle.SetParent(fortuneImg)
		wheelCircle.LoadImage(WHEEL_DIR+"circle.tga")
		wheelCircle.AddFlag("not_pick")
		wheelCircle.SetPosition(125, 111)
		wheelCircle.Show()
		self.children["wheelCircle"] = wheelCircle

		wheelStartBtn = ui.Button()
		wheelStartBtn.SetParent(fortuneImg)
		wheelStartBtn.SetUpVisual(WHEEL_DIR + "wheel_0.tga")
		wheelStartBtn.SetOverVisual(WHEEL_DIR + "wheel_1.tga")
		wheelStartBtn.SetDownVisual(WHEEL_DIR + "wheel_2.tga")
		wheelStartBtn.SetPosition(130, 130)
		wheelStartBtn.SetEvent(ui.__mem_func__(self.StartWheel))
		wheelStartBtn.Show()
		self.children["wheelStartBtn"] = wheelStartBtn

		itemPosition = [[175,40], [245, 65], [290, 135], [290, 210], [245, 265], [175, 295], [100, 270], [60, 210], [55, 135], [100, 65]]
		for j in xrange(len(itemPosition)):
			wheelItem = ImageBoxSpecial()
			wheelItem.SetParent(fortuneImg)
			wheelItem.AddFlag("movable")
			# Paylaþýmlý tooltip kullan
			wheelItem.SetItemToolTip(self.tooltipItem)
			wheelItem.SetPosition(itemPosition[j][0], itemPosition[j][1])
			wheelItem.SetSize(32, 32)
			wheelItem.Show()
			wheelItem.index = j
			self.children["wheelItem%d" % j] = wheelItem

		self.SetTrailerMode()
		self.SetTicketMode(0)

		self.SetDragonCoin(0)
		self.SetCenterPosition()
		self.LoadShopCategory()

	def SetTrailerMode(self):
		self.children["wheelStep"] = 0
		self.children["circleRotation"] = 0
		self.children["wheelRotation"] = 0
		for j in xrange(10):
			for x in xrange(10):
				itemData = wheel_random_items[app.GetRandom(0, len(wheel_random_items)-1)]
				item.SelectItem(itemData[0])
				self.children["wheelItem%d" % j].LoadImage(item.GetIconImageFileName(), itemData[0], itemData[1])

	def UpdateSingleFortune(self):
		single_pos = [[125,6,0], [193,33,35], [230,92,70], [228,161,110], [185,215,142], [117,237,180], [54,216,215], [11,159,250], [11,86,285], [53,26,324]]
		index = self.GetRotationToIndex(self.children["circleRotation"])
		self.children["fortuneSingle"].SetRotation(single_pos[index][2])
		self.children["fortuneSingle"].SetPosition(single_pos[index][0], single_pos[index][1])

	def OnSetWhell(self, giftIndex):
		self.giftIndex = giftIndex
		self.children["wheelRotation"] = self.GetGiftRotation(giftIndex)

		if self.children["wheelSpinCheckFlag"] == FALSE:
			self.children["wheelStep"] = app.GetRandom(10, 15)
		else:
			self.children["wheelStep"] = 0
			self.children["circleRotation"] = self.children["wheelRotation"]
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			self.SendServerDone()

	def SetWheelItemData(self, data):
		for j in xrange(10):
			self.children["wheelItem%d" % j].Clear()

		index = 0
		splitData = data.split("#")
		for splitText in splitData:
			splitItem = splitText.split("|")
			if len(splitItem) == 2:
				item.SelectItem(int(splitItem[0]))
				self.children["wheelItem%d" % index].LoadImage(item.GetIconImageFileName(), int(splitItem[0]), int(splitItem[1]))
				index += 1

	def StartWheel(self):
		net.SendChatPacket("/ishop wheel start %d" % self.children["wheelCointType"])

	def SendServerDone(self):
		net.SendChatPacket("/ishop wheel done")

	def CloseMessageWindow(self):
		if self.children.has_key("message_window"):
			msgWindow = self.children["message_window"]
			if msgWindow != None:
				msgWindow.Hide()
				msgWindow = None

	def GetWheelGiftData(self, itemVnum, itemCount):
		self.CloseMessageWindow()
		self.children["message_window"] = MessageWindow()
		self.children["message_window"].LoadMessage(itemVnum, itemCount)

	def SetTicketMode(self, index):
		if not self.children.has_key("wheelSpinCheckFlag"):
			self.children["wheelSpinCheckFlag"] = FALSE
		self.children["wheelCointType"] = index
		self.__ClickRadioButton([self.children["whellTicketBtn"], self.children["whellCoinBtn"]], index)
		self.SetSpintAnimation(self.children["wheelSpinCheckFlag"])

	def WheelBtn(self):
		self.children["wheelBg"].Show()

	def BackToItemShop(self):
		self.children["wheelBg"].Hide()

	def SetSpintAnimation(self, flag):
		self.children["wheelSpinCheckFlag"] = flag
		text = localeInfo.ITEMSHOP_WHEEL_SKIP_ANIMATION + "\n"
		if self.children["wheelCointType"] == 0:
			text += localeInfo.ITEMSHOP_WHEEL_TICKET_TEXT_INFO
		else:
			text += localeInfo.ITEMSHOP_WHEEL_COIN_TEXT_INFO
		self.children["wheelSpinCheckText"].SetText(text)

	def GetGiftRotation(self, giftIndex):
		if giftIndex == 0:
			if app.GetRandom(0,1) == 0:
				return 341 + app.GetRandom(4, 18)
			else:
				return app.GetRandom(4, 19)
		else:
			listofIndex = [0, 20, 58, 94, 126, 160, 196, 232, 268, 304, 341]
			return listofIndex[giftIndex]+app.GetRandom(4,28)

	def GetRotationToIndex(self, rotation):
		if rotation >= 341 and rotation <= 360 or rotation <= 20:
			return 0
		elif rotation >= 305 and rotation <= 305+38:
			return 9
		elif rotation >= 269 and rotation <= 269+38:
			return 8
		elif rotation >= 233 and rotation <= 233+38:
			return 7
		elif rotation >= 197 and rotation <= 197+38:
			return 6
		elif rotation >= 161 and rotation <= 161+38:
			return 5
		elif rotation >= 127 and rotation <= 127+38:
			return 4
		elif rotation >= 95 and rotation <= 95+38:
			return 3
		elif rotation >= 59 and rotation <= 59+38:
			return 2
		elif rotation >= 21 and rotation <= 21+38:
			return 1
		return 0

	def OnUpdate(self):
		if self.children["wheelStep"] > 0:
			increaseValue = 0.5 * float((self.children["wheelStep"]*2))
			self.children["circleRotation"] += increaseValue
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			if self.children["circleRotation"] >= 360:
				if self.children["wheelStep"] > 1:
					self.children["wheelStep"]-= 1
				self.children["circleRotation"]= 0
			if self.children["wheelStep"] == 1 and self.children["wheelRotation"] == self.children["circleRotation"]:
				self.children["wheelStep"] -= 1
				self.SendServerDone()
				return

		if self.children.has_key("itemListBox"):
			listboxData = self.children["itemListBox"].GetItems()
			for itemPointer in listboxData:
				itemPointer.OnUpdate()
		if self.children.has_key("LoadingImage"):
			LoadingImage = self.children["LoadingImage"]
			if LoadingImage.IsShow():
				self.children["LoadingImageRotation"] += 15
				LoadingImage.SetRotation(self.children["LoadingImageRotation"])

		if self.MoneyFlag == TRUE:
			if self.moneySize>=0 and self.moneySize<=15:
				if self.NewMoney > self.RealMoney:
					self.RealMoney += ((self.MoneyRange)/15)
					self.SetDragonCoinReal(self.RealMoney)
				else:
					self.RealMoney -= ((self.MoneyRange)/15)
					self.SetDragonCoinReal(self.RealMoney)
				self.moneySize+=1
				if self.moneySize ==15:
					self.RealMoney = self.NewMoney
					self.SetDragonCoinReal(self.RealMoney)
					self.MoneyFlag = FALSE
		
		# Tooltip kontrolü: Eðer tooltip açýksa ve fare item'lerin üzerinde deðilse kapat
		if self.tooltipItem and self.tooltipItem.IsShow():
			(x, y) = app.GetCursorPosition()
			# Tooltip'in kendisinin üzerinde mi kontrol et
			(tx, ty) = self.tooltipItem.GetGlobalPosition()
			tw, th = self.tooltipItem.GetWidth(), self.tooltipItem.GetHeight()
			onTooltip = (tx <= x <= tx + tw and ty <= y <= ty + th)
			
			# Item listesi üzerinde mi kontrol et
			onItemList = FALSE
			itemListBox = self.children.get("itemListBox")
			if itemListBox and itemListBox.IsShow():
				(sx, sy) = itemListBox.GetGlobalPosition()
				sw, sh = itemListBox.GetWidth(), itemListBox.GetHeight()
				if sx <= x <= sx + sw and sy <= y <= sy + sh:
					# Item listesi içinde, þimdi her bir item'in üzerinde mi kontrol et
					listboxData = itemListBox.GetItems()
					for itemWindow in listboxData:
						for shopItem in itemWindow.children:
							if hasattr(shopItem, 'childrenImages') and shopItem.childrenImages.has_key("itemImage"):
								itemImage = shopItem.childrenImages["itemImage"]
								(ix, iy) = itemImage.GetGlobalPosition()
								iw, ih = itemImage.GetWidth(), itemImage.GetHeight()
								if ix <= x <= ix + iw and iy <= y <= iy + ih:
									onItemList = TRUE
									break
						if onItemList:
							break
			
			# Ne item üzerinde ne de tooltip üzerinde deðilse kapat
			if not onItemList and not onTooltip:
				self.tooltipItem.HideToolTip()

	def __OnValueUpdate(self):
		itemSearch = self.children["itemSearch"]
		ui.EditLine.OnIMEUpdate(itemSearch)

		self.ClearItemList()
		searchItemList = []
		searchText = itemSearch.GetText().lower()
		if len(searchText) > 0:
			for key, data in itemshopData.items():
				for itemList in data:
					for itemData in itemList:
						item.SelectItem(itemData[LIST_ITEM_VNUM])
						if item.GetItemName().lower().find(searchText) != -1:
							searchItemList.append(itemData)
			self.RefreshItemList(searchItemList)
		else:
			self.LoadFirstOpening()

	def LoadShopCategory(self):
		self.categoryIndex = -1
		self.categorySubIndex = -1
		categoryListBox = self.children["categoryListBox"]
		for key, data in itemShopCategoryData.items():
			categoryItem = CategoryItem()
			categoryItem.SetParent(categoryListBox)
			categoryItem.SetPosition(0, len(categoryListBox.GetItems())*57, TRUE)
			categoryItem.SetData(key, data["name"],categoryListBox)
			categoryItem.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInCategory,key, proxy(categoryItem))
			categoryItem.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutCategory, key, proxy(categoryItem))
			categoryItem.SetEvent(self.ClickCategory, "mouse_click", key, -1)
			categoryItem.Show()
			categoryListBox.AppendItem(categoryItem)
		categoryListBox.SetBasePos(1)

	def ClickCategory(self, emptyArg, categoryIndex, categorySubIndex = -1):
		if self.categoryIndex == categoryIndex:
			if categorySubIndex == self.categorySubIndex:
				return

		oldIndex = self.categoryIndex
		self.categoryIndex = categoryIndex
		self.categorySubIndex = categorySubIndex

		categoryItems = self.children["categoryListBox"].GetItems()
		for category in categoryItems:
			if category.categoryIndex == oldIndex:
				self.OverOutCategory(oldIndex, category)
			elif category.categoryIndex == categoryIndex:
				self.OverInCategory(categoryIndex, category)

		self.UpdateSubCategoryWindow()
		self.RefreshItem()

	def OverOutCategory(self, categoryIndex, categoryItem):
		self.ClearSubCategoryWindow()
		if categoryIndex==self.categoryIndex:
			return
		categoryItem.LoadImage(IMG_DIR + "category_no_selected.tga")
		categoryItem.OnRender()

	def OverInCategory(self, categoryIndex, categoryItem):
		self.ClearSubCategoryWindow()
		subCategoryBtns = self.CreateSubCategoryWindow(categoryIndex)
		if subCategoryBtns != None:
			subCategoryBtns.Show()
			(px,py) = categoryItem.parent.GetLocalPosition()
			maxYPos = py+categoryItem.parent.GetHeight()
			(x, y) = categoryItem.GetLocalPosition()

			if y <= 0:
				subCategoryBtns.SetPosition(px+categoryItem.GetWidth()+2, py)
			elif (py+y+(len(subCategoryBtns.children)*39)) <= maxYPos:
				subCategoryBtns.SetPosition(px+categoryItem.GetWidth()+2, py+y)
			else:
				subCategoryBtns.SetPosition(px+categoryItem.GetWidth()+2, maxYPos-(len(subCategoryBtns.children)*39)-1)
			self.children["subCategoryBtns"]=subCategoryBtns

		categoryItem.LoadImage(IMG_DIR + "category_selected.tga")
		categoryItem.OnRender()

	def UpdateSubCategoryWindow(self):
		if self.children.has_key("subCategoryBtns"):
			subCategoryBtns = self.children["subCategoryBtns"]
			if subCategoryBtns != None:
				if subCategoryBtns.categoryIndex == self.categoryIndex:
					if self.categorySubIndex != -1:
						self.__ClickRadioButton(subCategoryBtns.children, self.categorySubIndex)

	def ClearSubCategoryWindow(self):
		if self.children.has_key("subCategoryBtns"):
			subCategoryBtns = self.children["subCategoryBtns"]
			if subCategoryBtns != None:
				(x, y) = app.GetCursorPosition()
				(sx,sy) = subCategoryBtns.GetGlobalPosition()
				if x >= sx and x <= sx+115:
					if y >= sy and y <= sy+(len(subCategoryBtns.children)*39):
						return

				if subCategoryBtns:
					for children in subCategoryBtns.children:
						children.backText = None
					subCategoryBtns.children = None
					subCategoryBtns.Hide()

				self.children["subCategoryBtns"] = None

	def CreateSubCategoryWindow(self, categoryIndex):
		if not itemShopCategoryData.has_key(categoryIndex):
			return None
		categoryData = itemShopCategoryData[categoryIndex]["subCategory"]
		if len(categoryData) == 0:
			return None

		subCategoryBtns = ui.ImageBox()
		subCategoryBtns.SetParent(self)
		children = []

		for j in xrange(len(categoryData)):
			categoryBtn = ui.RadioButton()
			categoryBtn.SetParent(subCategoryBtns)
			categoryBtn.SetUpVisual(IMG_DIR + "submenu_selected.tga")
			categoryBtn.SetOverVisual(IMG_DIR + "submenu_selected2.tga")
			categoryBtn.SetDownVisual(IMG_DIR + "submenu_selected2.tga")
			categoryBtn.SetHideToolTipEvent(ui.__mem_func__(self.ClearSubCategoryWindow))
			categoryBtn.SetEvent(self.ClickCategory, "", categoryIndex, j)
			categoryBtn.SetPosition(0, 39*j)
			categoryBtn.Show()

			categoryBtnText = MultiTextLine()
			categoryBtnText.SetParent(categoryBtn)
			categoryBtnText.SetTextRange(15)
			categoryBtnText.SetPosition(categoryBtn.GetWidth()/2,8)
			categoryBtnText.SetTextType("horizontal#center")
			categoryBtnText.SetFontName("Tahoma:16")
			categoryBtnText.SetText(categoryData[j])
			categoryBtnText.Show()
			categoryBtn.backText = categoryBtnText
			children.append(categoryBtn)
		subCategoryBtns.categoryIndex = categoryIndex
		subCategoryBtns.children = children
		if self.categoryIndex == categoryIndex:
			if self.categorySubIndex != -1:
				self.__ClickRadioButton(subCategoryBtns.children, self.categorySubIndex)
		subCategoryBtns.SetSize(115, len(children)*39)
		return subCategoryBtns

	def RefreshItem(self):
		self.ClearItemList()
		itemListBox = self.children["itemListBox"]
		if itemshopData.has_key(self.categoryIndex):
			itemData = itemshopData[self.categoryIndex]
			if self.categorySubIndex == -1:
				allList = []
				for itemList in itemData:
					for shopItem in itemList:
						allList.append(shopItem)
				self.RefreshItemList(allList)
			else:
				if self.categorySubIndex >= len(itemData):
					return
				itemList = itemData[self.categorySubIndex]
				self.RefreshItemList(itemList)

	def SortList(self, itemDataList):
		sortData = []
		for itemData in itemDataList:
			sortItem = SortClass(itemData)
			sortData.append(sortItem)
		sortData.sort()
		itemDataListEx = []
		for shortItem in sortData:
			itemDataListEx.append(shortItem.arg)
		return itemDataListEx

	def ClearItemList(self):
		itemListBox = self.children["itemListBox"]
		itemListBox.RemoveAllItems()

	def RefreshItemList(self, itemDataList):
		self.ClosePurchasesWindow()
		if len(itemDataList) == 0:
			return
		itemListBox = self.children["itemListBox"]

		itemDataList = self.SortList(itemDataList)
		itemWindowPtr = ItemWindow(itemListBox)
		itemWindowPtr.SetParent(itemListBox)
		itemWindowPtr.SetPosition(0, len(itemListBox.GetItems())*132, TRUE)
		itemWindowPtr.Show()
		for shopItem in itemDataList:
			xItem = ItemShopItem()
			xItem.SetParent(itemWindowPtr)
			xItem.SetPosition(len(itemWindowPtr.children)*250, 0)
			# Paylaþýmlý tooltip kullan
			xItem.SetItemToolTip(self.tooltipItem)
			# ItemShopWindow referansýný ekle (ejderha parasý kontrolü için)
			xItem.itemShopWindow = self
			xItem.LoadData(shopItem)
			xItem.Show()
			itemWindowPtr.AppendWindow(xItem)
			if len(itemWindowPtr.children) == ITEM_COLUMNS:
				itemListBox.AppendItem(itemWindowPtr)
				itemWindowPtr = ItemWindow(itemListBox)
				itemWindowPtr.SetParent(itemListBox)
				itemWindowPtr.SetPosition(0, len(itemListBox.GetItems())*132, TRUE)
				itemWindowPtr.Show()
		if itemWindowPtr != None:
			if len(itemWindowPtr.children) > 0:
				itemListBox.AppendItem(itemWindowPtr)

	def InsertItemWithType(self, itemDataList, insertType):
		currentTime = app.GetGlobalTimeStamp()
		for key, data in itemshopData.items():
			for categoryItems in data:
				for itemData in categoryItems:
					if insertType == LIST_ITEM_DISCOUNT:
						if itemData[LIST_ITEM_DISCOUNT] > 0 or itemData[LIST_ITEM_TIME] > 0:
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					elif insertType == LIST_ITEM_TOP_SELLING_INDEX:
						if itemData[LIST_ITEM_TOP_SELLING_INDEX] >= 0 and itemData[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					elif insertType == LIST_ITEM_ADDED_TIME:
						if itemData[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3):
							if not itemData in itemDataList:
								itemDataList.append(itemData)
					else:
						itemDataList.append(itemData)

		return itemDataList

	def OfferItems(self):
		self.ClearItemList()
		itemDataList = []
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_DISCOUNT)
		self.RefreshItemList(itemDataList)

	def HotItems(self):
		self.ClearItemList()

		itemDataList = []
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_TOP_SELLING_INDEX)
		self.RefreshItemList(itemDataList)

	def LoadFirstOpening(self):
		self.categoryIndex = -1
		self.categorySubIndex = -1
		self.ClearItemList()
		itemListBox = self.children["itemListBox"]

		itemDataList = []
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_TOP_SELLING_INDEX)
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_DISCOUNT)
		itemDataList = self.InsertItemWithType(itemDataList, LIST_ITEM_ADDED_TIME)
		if len(itemDataList) == 0:
			itemDataList = self.InsertItemWithType(itemDataList, 99)
		itemDataList = self.SortList(itemDataList)
		self.RefreshItemList(itemDataList)

	def BuyDragonCoin(self):
		self.LoadFirstOpening()

	def OpenWeb(self, url):
		self.CloseWeb()
		webWnd = WebWindow(self)
		webWnd.Open(url)
		self.children["WebWindow"] = webWnd

	def CloseWeb(self):
		if self.children.has_key("WebWindow"):
			webWnd = self.children["WebWindow"]
			webWnd.Close()
			webWnd.Destroy()
			del self.children["WebWindow"]

	def PurchaseBtn(self):
		self.OpenPurchasesWindow()

	def SetDragonCoinReal(self, dragonCoinValue):
		text = NumberToMoneyFormat(dragonCoinValue)+"\n|cffF8BE47" + localeInfo.ITEMSHOP_DRAGONCOINT
		self.children["dragonCoinText"].SetText(text)

	def UpdateItem(self, itemID, itemMaxSellingCount):
		itemList = self.children["itemListBox"].GetItems()
		for listBox in itemList:
			for shopItem in listBox.children:
				if isinstance(shopItem, ItemShopItem):
					if shopItem.data[LIST_ITEM_ID] == itemID:
						shopItem.data[LIST_ITEM_MAX_SELLING_COUNT] = itemMaxSellingCount
						shopItem.LoadData(shopItem.data)

	def SetDragonCoin(self, dragonCoinValue):
		if self.RealMoney == 0:
			self.RealMoney = dragonCoinValue
			self.SetDragonCoinReal(self.RealMoney)
		else:
			self.NewMoney = dragonCoinValue
			if self.NewMoney == self.RealMoney:
				self.MoneyFlag = FALSE
			else:
				self.MoneyFlag = TRUE
				self.MoneyRange = self.NewMoney - self.RealMoney
				self.moneySize = 0

	def Open(self):
		if not self.children.has_key("sendPacketData"):
			self.children["sendPacketData"] = 1
			self.children["LoadingImage"].Show()
			self.LoadFirstOpening()
			global UPDATE_TIME
			net.SendChatPacket("/ishop data %d" % UPDATE_TIME)

		self.Show()
		self.SetTop()

	def CloseLoading(self):
		self.children["LoadingImage"].Hide()

	def OpenPurchasesWindow(self):
		self.BackToItemShop()
		self.children["itemListBox"].Hide()
		self.children["itemScrollBar"].Hide()
		self.children["categoryListBox"].Hide()
		self.children["categoryScrollBar"].Hide()

		self.children["PurchasesScrollBar"].Show()
		self.children["PurchasesListBox"].Show()

		if not self.children.has_key("sendPacketDataLog"):
			self.children["sendPacketDataLog"] = 1
			self.children["LoadingImage"].Show()
			net.SendChatPacket("/ishop log")
		else:
			self.RefreshPurchasesWindow()

	def SortPurchasesWindow(self):
		global playerLog
		sortData = []
		for logData in playerLog:
			sortLog = SortClass(logData,TRUE)
			sortData.append(sortLog)
		sortData.sort()
		logDataList = []
		for sortLog in sortData:
			logDataList.append(sortLog.arg)
		return logDataList

	def RefreshPurchasesWindow(self):

		PurchasesListBox = self.children["PurchasesListBox"]
		PurchasesListBox.Show()
		PurchasesListBox.RemoveAllItems()

		global playerLog
		playerLog = self.SortPurchasesWindow()

		logTitle = PurchasesLog(PurchasesListBox)
		logTitle.SetParent(PurchasesListBox)
		logTitle.LoadData(-1)
		logTitle.SetPosition(0,0, TRUE)
		logTitle.Show()
		PurchasesListBox.AppendItem(logTitle)

		for j in xrange(len(playerLog)):
			logItem = PurchasesLog(PurchasesListBox)
			logItem.SetParent(PurchasesListBox)
			logItem.LoadData(j)
			logItem.SetPosition(0,len(PurchasesListBox.GetItems())*35, TRUE)
			logItem.Show()
			PurchasesListBox.AppendItem(logItem)
		PurchasesListBox.Render(0)

	def ClosePurchasesWindow(self):
		self.BackToItemShop()
		self.children["PurchasesScrollBar"].Hide()
		self.children["PurchasesListBox"].Hide()

		self.children["itemListBox"].Show()
		self.children["itemScrollBar"].Show()
		self.children["categoryListBox"].Show()
		self.children["categoryScrollBar"].Hide()

	def Close(self):
		# Kapatýldýðýnda tüm tooltip'leri kapat
		from uiToolTip import ToolTip
		for tooltip in ToolTip._allToolTips:
			if tooltip and hasattr(tooltip, 'IsShow') and tooltip.IsShow():
				if hasattr(tooltip, 'HideToolTip'):
					tooltip.HideToolTip()
				elif hasattr(tooltip, 'Hide'):
					tooltip.Hide()
		
		# Paylaþýmlý tooltip'i de kapat
		if self.tooltipItem and self.tooltipItem.IsShow():
			self.tooltipItem.HideToolTip()
		
		self.CloseMessageWindow()
		if self.children["wheelStep"] > 0:
			self.children["wheelStep"] = 0
			self.children["circleRotation"] = self.children["wheelRotation"]
			self.children["wheelCircle"].SetRotation(self.children["circleRotation"])
			self.UpdateSingleFortune()
			self.SendServerDone()

		self.CloseWeb()

		global buyQuestionDialog
		if buyQuestionDialog != None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None

		self.Hide()
	def OnPressEscapeKey(self):
		# ESC ile kapatýldýðýnda tüm tooltip'leri kapat
		from uiToolTip import ToolTip
		for tooltip in ToolTip._allToolTips:
			if tooltip and hasattr(tooltip, 'IsShow') and tooltip.IsShow():
				if hasattr(tooltip, 'HideToolTip'):
					tooltip.HideToolTip()
				elif hasattr(tooltip, 'Hide'):
					tooltip.Hide()
		
		# Paylaþýmlý tooltip'i de kapat
		if self.tooltipItem and self.tooltipItem.IsShow():
			self.tooltipItem.HideToolTip()
		
		self.Close()
		return TRUE
	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			btn = buttonList[buttonIndex]
		except IndexError:
			return
		for eachButton in buttonList:
			eachButton.SetUp()
		btn.Down()

class PurchasesLog(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def Destroy(self):
		self.childrenImages = {}
		self.childrenTextLine = {}
		self.parent = None

	def __init__(self, parent):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.parent = proxy(parent)

	def LoadData(self, index):
		if index >= 0:
			self.childrenImages["self"] = self
			self.LoadImage(IMG_DIR + "purch_img.tga")

		try:
			logData = playerLog[index]
		except:
			return

		if index >= 0:
			dateText = logData[LOG_DATE_TEXT]
			accountText = logData[LOG_ACCOUNT]
			ipText = logData[LOG_IP]
			if logData[LOG_ITEM] == 1:
				itemText = localeInfo.ITEMSHOP_WHEEL_PURCHASE
			else:
				item.SelectItem(logData[LOG_ITEM])
				if logData[LOG_COUNT] > 1:
					itemText = "|cffF7BD48%sx%d|r" % (item.GetItemName(), logData[LOG_COUNT])
				else:
					itemText = "|cffF7BD48%s|r" % item.GetItemName()
			priceText = localeInfo.ITEMSHOP_PURCHASES_PRICE_TEXT % NumberToMoneyFormat(logData[LOG_PRICE])
		else:
			dateText = localeInfo.ITEMSHOP_PURCHASES_DATE
			accountText = localeInfo.ITEMSHOP_PURCHASES_ACCOUNTID
			ipText = localeInfo.ITEMSHOP_PURCHASES_IP
			itemText = localeInfo.ITEMSHOP_PURCHASES_ITEMBOUGHT
			priceText = localeInfo.ITEMSHOP_PURCHASES_PRICE

		textList = [
			[70, 7, dateText],
			[260, 7, accountText],
			[400, 7, ipText],
			[600, 7, itemText],
			[800, 7, priceText],
		]

		for j in xrange(len(textList)):
			text = textList[j]
			logText = ui.TextLine()
			logText.SetParent(self)
			logText.SetFontName("Tahoma:13")
			logText.SetText(text[2])
			logText.SetHorizontalAlignCenter()
			if index >= 0:
				logText.SetPosition(text[0],text[1])
			else:
				logText.SetPosition(text[0],0)
			logText.Show()
			self.childrenTextLine["text%d" % j] = logText

	def OnRender(self):
		listboxHeight = self.parent.GetHeight()
		(pgx,pgy) = self.parent.GetGlobalPosition()
		for key, textline in self.childrenTextLine.items():
			(igx,igy) = textline.GetGlobalPosition()
			if igy < pgy:
				textline.Hide()
			elif igy > (pgy+listboxHeight-25):
				textline.Hide()
			else:
				textline.Show()
		for key, image in self.childrenImages.items():
			(jgx,jgy) = image.GetGlobalPosition()
			imageHeight = image.GetHeight()
			if jgy < pgy:
				image.SetRenderingRect(0,ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight),0,0)
			elif jgy+imageHeight > (pgy+listboxHeight-8):
				calculate = (pgy+listboxHeight-8) - (jgy+imageHeight)
				if calculate == 0:
					return
				image.SetRenderingRect(0.0, 0.0, 0.0, ui.calculateRect(imageHeight-abs(calculate), imageHeight))
			else:
				image.SetRenderingRect(0, 0, 0, 0)

class ItemShopBoard(ui.Window):
	CORNER_WIDTH = 7
	CORNER_HEIGHT = 7
	LINE_WIDTH = 7
	LINE_HEIGHT = 7
	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self):
		ui.Window.__init__(self)
		self.MakeBoard()
		self.MakeBase()

	def MakeBoard(self):
		cornerPath = IMG_DIR + "board/corner_"
		linePath = IMG_DIR + "board/"
		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("left", "right", "top", "bottom") ]
		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ui.ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)
		self.Lines = []
		for fileName in LineFileNames:
			Line = ui.ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)
		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def MakeBase(self):
		self.Base = ui.ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage(IMG_DIR + "board/base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.Base=0
		self.Corners=0
		self.Lines=0
		self.CORNER_WIDTH = 0
		self.CORNER_HEIGHT = 0
		self.LINE_WIDTH = 0
		self.LINE_HEIGHT = 0
		self.LT = 0
		self.LB = 0
		self.RT = 0
		self.RB = 0
		self.L = 0
		self.R = 0
		self.T = 0
		self.B = 0

	def SetSize(self, width, height):
		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		ui.Window.SetSize(self, width, height)
		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)
		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)

class WebWindow(ItemShopBoard):
	def __del__(self):
		ItemShopBoard.__del__(self)

	def Destroy(self):
		self.oldPos = (0,0)
		self.closeBtn = None
		self.parent = None

	def __init__(self, parent):
		ItemShopBoard.__init__(self)
		self.Destroy()
		self.parent = proxy(parent)
		self.LoadWindow()

	def LoadWindow(self):
		self.AddFlag("attach")
		self.AddFlag("float")
		self.SetSize(450*2, 450)

		closeBtn = ui.Button()
		closeBtn.SetParent(self)
		closeBtn.SetUpVisual(IMG_DIR + "close_0.tga")
		closeBtn.SetOverVisual(IMG_DIR + "close_1.tga")
		closeBtn.SetDownVisual(IMG_DIR + "close_2.tga")
		closeBtn.SetPosition(self.GetWidth()-closeBtn.GetWidth(), 0)
		closeBtn.SetEvent(self.parent.CloseWeb)
		closeBtn.Show()
		self.closeBtn = closeBtn

	def Close(self):
		app.HideWebPage()
		self.Hide()

	def Open(self, url):
		self.SetSize(450*2, 450)
		self.Show()
		self.SetTop()
		self.SetCenterPosition()

		x, y = self.GetGlobalPosition()
		sx, sy = x + 10, y + 30
		ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40
		app.ShowWebPage(url, (sx, sy, ex, ey))

	def OnUpdate(self):
		newPos = self.GetGlobalPosition()
		if newPos == self.oldPos:
			return
		self.oldPos = newPos
		x, y = newPos
		sx, sy = x + 10, y + 30
		ex, ey = sx + self.GetWidth() - 20, sy + self.GetHeight() - 40
		app.MoveWebPage((sx, sy, ex, ey))

class BuyWindow(ItemShopBoard):
	def __del__(self):
		ItemShopBoard.__del__(self)

	def __init__(self):
		ItemShopBoard.__init__(self)
		self.AddFlag("attach")
		self.AddFlag("float")
		self.Destroy()

		buyText = MultiTextLine()
		buyText.SetParent(self)
		buyText.SetTextRange(25)
		buyText.SetPosition(self.GetWidth()/2, 28)
		buyText.SetTextType("horizontal#center")
		buyText.SetFontName("Tahoma:16")
		buyText.Show()
		self.children["buyText"] = buyText

		yesBtn = ui.Button()
		yesBtn.SetParent(self)
		yesBtn.SetUpVisual(IMG_DIR + "buy_0.tga")
		yesBtn.SetOverVisual(IMG_DIR + "buy_1.tga")
		yesBtn.SetDownVisual(IMG_DIR + "buy_2.tga")
		yesBtn.SetDownVisual(IMG_DIR + "buy_2.tga")
		yesBtn.SetText(localeInfo.UI_ACCEPT)
		yesBtn.Show()
		self.children["yesBtn"] = yesBtn

		cancelBtn = ui.Button()
		cancelBtn.SetParent(self)
		cancelBtn.SetUpVisual(IMG_DIR + "buy_0.tga")
		cancelBtn.SetOverVisual(IMG_DIR + "buy_1.tga")
		cancelBtn.SetDownVisual(IMG_DIR + "buy_2.tga")
		cancelBtn.SetText(localeInfo.UI_CANCEL)
		cancelBtn.Show()
		self.children["cancelBtn"] = cancelBtn

	def Destroy(self):
		self.children = {}

	def SetText(self, text):
		buyText = self.children["buyText"]
		buyText.SetText(text)
		(x, y) = buyText.GetTextSize()
		(minWidth,minHeight) = (256, 142)
		if x+40 > minWidth:
			minWidth = x+40
		if not ((minWidth%2) == 0):
			minWidth+=1
		self.SetSize(minWidth,minHeight)
		buyText.SetPosition(self.GetWidth()/2, 28)
		self.children["yesBtn"].SetPosition((self.GetWidth()/2)+15, 94)
		self.children["cancelBtn"].SetPosition((self.GetWidth()/2)-self.children["cancelBtn"].GetWidth()-15, 94)

	def SetCancelEvent(self, func):
		self.children["cancelBtn"].SetEvent(func)

	def SetAcceptEvent(self, func):
		self.children["yesBtn"].SetEvent(func)

	def OnPressEscapeKey(self):
		self.children["cancelBtn"].CallEvent()
		return TRUE

class SortClass:
	def __init__(self, arg, isLog = FALSE):
		self.arg = arg
		self.isLog = isLog

	def __del__(self):
		self.arg = []

	def __lt__(self, other):
		if self.isLog:
			return self.arg[LOG_DATE] >= other.arg[LOG_DATE]
		else:
			currentTime = app.GetGlobalTimeStamp()

			if self.arg[LIST_ITEM_TOP_SELLING_INDEX] <= 10 or other.arg[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
				return self.arg[LIST_ITEM_TOP_SELLING_INDEX] <= other.arg[LIST_ITEM_TOP_SELLING_INDEX]

			if self.arg[LIST_ITEM_TIME] != 0 or other.arg[LIST_ITEM_TIME] != 0:
				if self.arg[LIST_ITEM_TIME] > 0 and other.arg[LIST_ITEM_TIME] > 0:
					return ((self.arg[LIST_ITEM_TIME]-currentTime) <= (other.arg[LIST_ITEM_TIME]-currentTime))
				else:
					return self.arg[LIST_ITEM_TIME] >= other.arg[LIST_ITEM_TIME]

			if self.arg[LIST_ITEM_DISCOUNT] != 0 or other.arg[LIST_ITEM_DISCOUNT] != 0:
				return self.arg[LIST_ITEM_DISCOUNT] >= other.arg[LIST_ITEM_DISCOUNT]

			if self.arg[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3) or other.arg[LIST_ITEM_ADDED_TIME] > currentTime-(60*60*24*3):
				return self.arg[LIST_ITEM_ADDED_TIME] >= other.arg[LIST_ITEM_ADDED_TIME]
			return self.arg[LIST_ITEM_ID] >= other.arg[LIST_ITEM_ID]

class ItemShopItem(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.interface = None
		self.buyCount = 1
		self.LoadImage(IMG_DIR + "item_box.tga")
		self.childrenImages["self"] = self

		# Tooltip dýþarýdan verilecek, böylece tüm itemler ayný tooltip penceresini kullanýr
		self.tooltipItem = None

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def Destroy(self):
		self.data = []
		self.buyCount = 0
		self.butButtonSet = FALSE
		self.childrenImages = {}
		self.childrenTextLine = {}
		self.interface = None
		self.itemShopWindow = None

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def LoadData(self, itemData):
		self.data = itemData
		self.SetItem(itemData[LIST_ITEM_VNUM])
		self.SetCount()
		self.SetItemName()
		self.SetBuyButton(itemData[LIST_ITEM_TIME], itemData[LIST_ITEM_MAX_SELLING_COUNT])
		self.SetPrice(itemData[LIST_ITEM_PRICE],itemData[LIST_ITEM_DISCOUNT])
		self.SetTime(itemData[LIST_ITEM_TIME])
		self.SetLastSellCount(itemData[LIST_ITEM_MAX_SELLING_COUNT])

		if itemData[LIST_ITEM_TOP_SELLING_INDEX] >= 0 and itemData[LIST_ITEM_TOP_SELLING_INDEX] <= 10:
			self.SetItemType(1)
		elif itemData[LIST_ITEM_TIME] > 0 or itemData[LIST_ITEM_DISCOUNT] > 0:
			self.SetItemType(3)
		elif itemData[LIST_ITEM_ADDED_TIME] > app.GetGlobalTimeStamp()-(60*60*24*3):
			self.SetItemType(2)

	def SetLastSellCount(self, maxSellingCount):
		if maxSellingCount != -1:
			self.ClearChildren("maxSelling")
			maxSelling = MultiTextLine()
			maxSelling.SetParent(self)
			maxSelling.SetTextRange(20)
			maxSelling.SetTextType("horizontal#right")
			maxSelling.SetFontName("Tahoma:12")
			maxSelling.SetText(localeInfo.ITEMSHOP_LAST_COUNT % maxSellingCount)
			maxSelling.SetPosition(self.GetWidth()-5, 4)
			maxSelling.Show()
			self.childrenTextLine["maxSelling"] = maxSelling

	def ClearChildren(self, childrenName):
		if self.childrenTextLine.has_key(childrenName):
			self.childrenTextLine[childrenName].Hide()
			self.childrenTextLine[childrenName].Destroy()
			del self.childrenTextLine[childrenName]

		if self.childrenImages.has_key(childrenName):
			self.childrenImages[childrenName].Hide()
			self.childrenImages[childrenName].Destroy()
			del self.childrenImages[childrenName]

	def FormatTime(self, seconds):
		if seconds <= 0:
			if self.butButtonSet == FALSE:
				self.SetBuyButton(self.data[LIST_ITEM_TIME])
			return "|cffFF7B7B0h 0m 0s"
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		d, h = divmod(h, 24)
		if d > 0:
			return "|cffFF7B7B%dd %dh %dm %ds"%(d,h,m,s)
		return "|cffFF7B7B%dh %dm %ds" % (h,m,s)

	def OnUpdate(self):
		if self.childrenTextLine.has_key("timeText"):
			self.childrenTextLine["timeText"].SetText(self.FormatTime(self.data[LIST_ITEM_TIME]-app.GetGlobalTimeStamp()))

	def SetTime(self, time):
		self.ClearChildren("timeText")
		if time != 0:
			self.butButtonSet = FALSE

			timeText = ui.TextLine()
			timeText.SetParent(self)
			timeText.AddFlag("not_pick")
			timeText.SetPosition(108, 85)
			timeText.SetWindowHorizontalAlignLeft()
			timeText.SetFontName("Tahoma:13")
			timeText.Show()
			self.childrenTextLine["timeText"] = timeText

	def SetBuyButton(self, time, maxSellingCount = -1):
		self.ClearChildren("buyButton")
		self.butButtonSet = FALSE
		buyButton = ui.ExpandedImageBox()
		buyButton.SetParent(self)
		if((time != 0 and app.GetGlobalTimeStamp() > time) or maxSellingCount == 0):
			buyButton.LoadImage(IMG_DIR + "button_buy_disabled.tga")
			self.butButtonSet = TRUE

			buyBtnText = ui.TextLine()
			buyBtnText.SetParent(buyButton)
			buyBtnText.AddFlag("not_pick")
			buyBtnText.SetHorizontalAlignCenter()
			buyBtnText.SetVerticalAlignCenter()
			buyBtnText.SetWindowHorizontalAlignCenter()
			buyBtnText.SetWindowVerticalAlignCenter()
			buyBtnText.SetText(localeInfo.ITEMSHOP_EXPIRED)
			buyBtnText.Show()
			self.childrenTextLine["buyBtnText"] = buyBtnText

			self.childrenImages["CountLeft"].Hide()
			self.childrenImages["CountBg"].Hide()
			self.childrenImages["CountRight"].Hide()

		else:
			buyButton.LoadImage(IMG_DIR + "buy_0.tga")
			buyButton.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInBuyBtn)
			buyButton.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutBuyBtn)
			buyButton.SetEvent(ui.__mem_func__(self.ClickBuyBtn),"mouse_click")
		buyButton.SetPosition(98, 45)
		buyButton.Show()
		self.childrenImages["buyButton"] = buyButton

	def ClickBuyBtn(self, emptyArg):
		time = self.data[LIST_ITEM_TIME]
		if time != 0 and app.GetGlobalTimeStamp() > time:
			return

		global buyQuestionDialog
		if buyQuestionDialog == None:
			buyQuestionDialog = BuyWindow()

		text = ""

		price = self.data[LIST_ITEM_PRICE]*self.buyCount
		discount = self.data[LIST_ITEM_DISCOUNT]
		if discount > 0:
			price = int((float(price)/100.0)*float(100-discount))

		item.SelectItem(self.data[LIST_ITEM_VNUM])
		if self.buyCount > 1:
			text+=localeInfo.ITEMSHOP_BUY_TEXT_COUNT % (item.GetItemName(), self.buyCount)
		else:
			text+=localeInfo.ITEMSHOP_BUY_TEXT % item.GetItemName()

		text+="\n"
		text+=localeInfo.ITEMSHOP_PRICE_NEW % price

		buyQuestionDialog.SetText(text)
		buyQuestionDialog.SetAcceptEvent(self.AcceptBuy)
		buyQuestionDialog.SetCancelEvent(self.CloseQuestionDialog)
		buyQuestionDialog.SetCenterPosition()
		buyQuestionDialog.SetTop()
		buyQuestionDialog.Show()

	def CloseQuestionDialog(self):
		global buyQuestionDialog
		if buyQuestionDialog!= None:
			buyQuestionDialog.Hide()
			buyQuestionDialog.Destroy()
			buyQuestionDialog=None

	def AcceptBuy(self):
		# Server-side kontrolüne güveniyoruz, client-side kontrolü kaldýrýldý
		# Çünkü client-side'da para miktarý animasyonlu güncelleniyor ve yanlýþ deðer okunabilir
		net.SendChatPacket("/ishop buy %d %d"%(self.data[LIST_ITEM_ID],self.buyCount))
		self.CloseQuestionDialog()

	def EmptyFunction(self):
		pass

	def OverOutBuyBtn(self):
		if self.childrenImages.has_key("buyButton"):
			self.childrenImages["buyButton"].LoadImage(IMG_DIR + "buy_0.tga")

	def OverInBuyBtn(self):
		if self.childrenImages.has_key("buyButton"):
			self.childrenImages["buyButton"].LoadImage(IMG_DIR + "buy_1.tga")

	def SetPrice(self, price, discount):
		self.ClearChildren("lineImage")
		self.ClearChildren("itemPrice")

		itemPrice = MultiTextLine()
		itemPrice.AddFlag("not_pick")
		itemPrice.SetParent(self.childrenImages["buyButton"])
		itemPrice.SetTextRange(20)
		itemPrice.SetPosition(35, 6)
		itemPrice.SetTextType("horizontal#center")
		itemPrice.SetFontName("Tahoma:13")

		price = price*self.buyCount

		text= ""
		if discount > 0:
			newPrice = int((float(price)/100.0)*float(100-discount))

			text = localeInfo.ITEMSHOP_PRICE_ONLY_TEXT % NumberToMoneyFormat(price)
			itemPriceEx = ui.TextLine()
			itemPriceEx.SetParent(self)
			itemPriceEx.SetText(text)
			(xSize,ySize) = itemPriceEx.GetTextSize()

			text = localeInfo.ITEMSHOP_PRICE_WITH_DISCOUNT % (NumberToMoneyFormat(price),discount)
			text += "\n"
			text += "     "
			text += localeInfo.ITEMSHOP_PRICE_ONLY_TEXT % NumberToMoneyFormat(newPrice)

			lineImage = ui.ExpandedImageBox()
			lineImage.SetParent(itemPrice)
			lineImage.AddFlag("not_pick")
			lineImage.LoadImage(IMG_DIR + "line.tga")
			lineImage.SetDiffuseColor(1.0, 0.0, 0.0, 0.9)
			scaleCalc = float(xSize)*(1.0/10.0)
			lineImage.SetScale(scaleCalc, 1.0)
			lineImage.SetPosition(17, (ySize/2)+1)
			lineImage.Show()
			self.childrenImages["lineImage"] = lineImage

			self.SetItemType(3)
		else:
			text = localeInfo.ITEMSHOP_PRICE_NEW % NumberToMoneyFormat(price)

		itemPrice.SetText(text)
		itemPrice.Show()
		self.childrenTextLine["itemPrice"] = itemPrice

	def SetItemName(self):
		self.ClearChildren("itemName")
		itemName = ui.TextLine()
		itemName.SetParent(self)
		itemName.AddFlag("not_pick")
		itemName.SetWindowHorizontalAlignLeft()
		itemName.SetFontName("Tahoma:13")
		itemName.SetPosition(95, 15)
		itemName.SetText("|cffF7BD48"+item.GetItemName())
		itemName.Show()
		self.childrenTextLine["itemName"] = itemName

	def SetCount(self):
		self.ClearChildren("CountBg")
		self.ClearChildren("CountText")
		self.ClearChildren("CountLeft")
		self.ClearChildren("CountRight")
		if not item.IsFlag(item.ITEM_FLAG_STACKABLE):
			return

		CountLeft = ui.ExpandedImageBox()
		CountLeft.SetParent(self)
		CountLeft.LoadImage(IMG_DIR + "count_left.tga")
		CountLeft.SetPosition(20, 90)
		CountLeft.SetEvent(ui.__mem_func__(self.ClickCount),"mouse_click", 0)
		CountLeft.Show()
		self.childrenImages["CountLeft"] = CountLeft

		CountBg = ui.ExpandedImageBox()
		CountBg.SetParent(self)
		CountBg.AddFlag("not_pick")
		CountBg.LoadImage(IMG_DIR + "count_back.tga")
		CountBg.SetPosition(20+CountLeft.GetWidth(), 90)
		CountBg.Show()
		self.childrenImages["CountBg"] = CountBg

		CountText = ui.TextLine()
		CountText.SetParent(CountBg)
		CountText.AddFlag("not_pick")
		CountText.SetHorizontalAlignCenter()
		CountText.SetVerticalAlignCenter()
		CountText.SetWindowHorizontalAlignCenter()
		CountText.SetWindowVerticalAlignCenter()
		CountText.SetText("1")
		CountText.Show()
		self.childrenTextLine["CountText"] = CountText

		CountRight = ui.ExpandedImageBox()
		CountRight.SetParent(self)
		CountRight.LoadImage(IMG_DIR + "count_right.tga")
		CountRight.SetPosition(20+CountLeft.GetWidth()+CountBg.GetWidth(), 90)
		CountRight.SetEvent(ui.__mem_func__(self.ClickCount),"mouse_click", 1)
		CountRight.Show()
		self.childrenImages["CountRight"] = CountRight

	def ClickCount(self, emptyArg, btnType):
		value = 1
		if app.IsPressed(app.DIK_LCONTROL):
			value += 4
		buyCount = self.buyCount
		if btnType == 0:
			buyCount -= value
			if buyCount <= 0:
				buyCount = 1
		else:
			buyCount += value
			if buyCount <= 0:
				buyCount = 1
			elif buyCount > 20:
				buyCount = 20
		self.buyCount = buyCount

		if self.childrenTextLine.has_key("CountText"):
			self.childrenTextLine["CountText"].SetText(str(self.buyCount))

		itemData = self.data
		self.SetPrice(itemData[LIST_ITEM_PRICE],itemData[LIST_ITEM_DISCOUNT])

	def SetItem(self, itemVnum):
		self.ClearChildren("itemImage")
		self.ClearChildren("itemCount")
		item.SelectItem(itemVnum)

		(width,height) = item.GetItemSize()
		(xPos, yPos) = (36, 46)
		if height == 1:
			xPos = 30
		elif height == 2:
			yPos -= 20
		elif height == 3:
			yPos -= 30
			xPos -= 5

		itemImage = ui.ExpandedImageBox()
		itemImage.SetParent(self)
		itemImage.LoadImage(item.GetIconImageFileName())
		itemImage.SetPosition(xPos,yPos)
		itemImage.SAFE_SetStringEvent("MOUSE_OVER_IN", self.OverInItem,itemVnum)
		itemImage.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.OverOutItem)
		itemImage.Show()
		self.childrenImages["itemImage"] = itemImage

	def OverInItem(self, itemVnum):
		if None == self.tooltipItem:
			return

		self.tooltipItem.SetItemToolTip(itemVnum)
		# Tooltip pozisyonunu item'in yanýna sabitle
		(x, y) = self.GetGlobalPosition()
		itemImage = self.childrenImages.get("itemImage")
		if itemImage:
			(ix, iy) = itemImage.GetGlobalPosition()
			self.tooltipItem.SetPosition(ix + itemImage.GetWidth() + 10, iy)
		self.tooltipItem.ShowToolTip()

	def OverOutItem(self):
		if None == self.tooltipItem:
			return
		
		# Tooltip'i kesinlikle kapat
		if self.tooltipItem.IsShow():
			self.tooltipItem.HideToolTip()

	def SetItemType(self, itemType):
		self.ClearChildren("itemType")
		typeImages = ["hot.tga", "new.tga", "offer.tga"]
		if itemType == 0:
			return

		itemType-=1
		if itemType >= len(typeImages):
			return

		typeImage = ui.ExpandedImageBox()
		typeImage.AddFlag("not_pick")
		typeImage.SetParent(self)
		typeImage.LoadImage(IMG_DIR + typeImages[itemType])
		typeImage.SetPosition(0, 0)
		typeImage.Show()
		self.childrenImages["typeImage"] = typeImage

	def OnRender(self, parent):
		listboxHeight = parent.GetHeight()
		(pgx,pgy) = parent.GetGlobalPosition()
		for key, textline in self.childrenTextLine.items():
			(igx,igy) = textline.GetGlobalPosition()
			if igy < pgy:
				textline.Hide()
			elif igy > (pgy+listboxHeight-25):
				textline.Hide()
			else:
				textline.Show()
		for key, image in self.childrenImages.items():
			(jgx,jgy) = image.GetGlobalPosition()
			imageHeight = image.GetHeight()
			if jgy < pgy:
				image.SetRenderingRect(0, ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight), 0, 0)
			elif jgy+imageHeight > (pgy+listboxHeight-8):
				calculate = (pgy+listboxHeight-8) - (jgy+imageHeight)
				if calculate == 0:
					return
				image.SetRenderingRect(0.0, 0.0, 0.0, ui.calculateRect(imageHeight-abs(calculate), imageHeight))
			else:
				image.SetRenderingRect(0, 0, 0, 0)

class ItemWindow(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)

	def __init__(self, parent):
		ui.Window.__init__(self)
		self.children = []
		self.Destroy()
		self.parent = proxy(parent)
		self.SetSize(750, 122)

	def OnUpdate(self):
		for children in self.children:
			children.OnUpdate()

	def Destroy(self):
		for children in self.children:
			children.Destroy()
			children = None
		self.children = []
		self.parent = None

	def AppendWindow(self, window):
		self.children.append(window)
		self.OnRender()

	def OnRender(self):
		for children in self.children:
			children.OnRender(self.parent)

class CategoryItem(ui.ExpandedImageBox):
	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.Destroy()
		self.categoryIndex=-1
		self.LoadImage(IMG_DIR + "category_no_selected.tga")

	def Destroy(self):
		self.parent = None
		self.categoryIndex = 0
		self.categoryText = None

	def SetData(self, categoryIndex, categoryName, categoryListBox):
		self.categoryIndex = categoryIndex
		self.parent = proxy(categoryListBox)
		categoryText = MultiTextLine()
		categoryText.SetParent(self)
		categoryText.SetPosition(self.GetWidth()/2, 11)
		categoryText.SetTextRange(15)
		categoryText.SetTextType("horizontal#center")
		categoryText.SetFontName("Tahoma:16")
		categoryText.SetText(categoryName)
		categoryText.Show()
		self.categoryText = categoryText

	def OnRender(self):
		parent = self.parent
		if parent == None:
			return
		listboxHeight = parent.GetHeight()
		(pgx,pgy) = parent.GetGlobalPosition()
		(igx,igy) = self.categoryText.GetGlobalPosition()
		if igy < pgy:
			self.categoryText.Hide()
		elif igy > (pgy+listboxHeight-25):
			self.categoryText.Hide()
		else:
			self.categoryText.Show()
		(jgx,jgy) = self.GetGlobalPosition()
		imageHeight = self.GetHeight()
		if jgy < pgy:
			self.SetRenderingRect(0, ui.calculateRect(imageHeight-abs(jgy-pgy),imageHeight), 0, 0)
		elif jgy+imageHeight > (pgy+listboxHeight-4):
			calculate = (pgy+listboxHeight-4) - (jgy+imageHeight)
			if calculate == 0:
				return
			self.SetRenderingRect(0.0, 0.0, 0.0, ui.calculateRect(imageHeight-abs(calculate), imageHeight))
		else:
			self.SetRenderingRect(0, 0, 0, 0)

class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 13
	SCROLLBAR_MIDDLE_HEIGHT = 1
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	SCROLL_BTN_XDIST = 2
	SCROLL_BTN_YDIST = 2

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")

		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR + "scroll_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR + "scroll_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR + "scroll_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.bottom = bottom
			self.middle = middle

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())

	def __init__(self):
		ui.Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = FALSE

		self.CreateScrollBar()
		self.SetScrollBarSize(0)

		self.scrollStep = 0.03
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR + "scrollbar_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR + "scrollbar_bottom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR + "scrollbar_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(0)
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - self.SCROLL_BTN_YDIST*2)))
		realHeight = self.GetHeight() - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def SetPos(self, pos, moveEvent = TRUE):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == TRUE:
			self.OnMove()

	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = TRUE

	def UnlockScroll(self):
		self.lockFlag = FALSE

def NumberToMoneyFormat(n):
	if n <= 0 :
		return "0"
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

class MultiTextLine(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.children = []
		self.rangeText = 0

	def __init__(self):
		ui.Window.__init__(self)
		self.children = []
		self.rangeText = 15
		self.textType = ""
		self.fontName = ""
		self.outline = FALSE
		self.rgb = (0, 0, 0)

	def SetFontColor(self, r, g, b):
		self.rgb = (r, g, b)
		for text in self.children:
			if self.rgb[0] != 0:
				text.SetFontColor(self.rgb[0], self.rgb[1], self.rgb[2])

	def SetOutline(self, flag):
		self.outline = flag
		for text in self.children:
			if self.outline:
				text.SetOutline()

	def SetTextType(self, textType):
		self.textType = textType
		for text in self.children:
			self.AddTextType(self.textType.split("#"),text)

	def SetFontName(self, fontName):
		self.fontName = fontName
		for text in self.children:
			text.SetFontName(fontName)

	def GetTextSize(self, index = 0):
		if len(self.children) == 0:
			return (0,0)
		elif index >= len(self.children):
			return (0,0)
		return self.children[index].GetTextSize()

	def SetTextRange(self, range):
		self.rangeText = range
		yPosition = 0
		for text in self.children:
			text.SetPosition(0, yPosition)
			yPosition += self.rangeText

	def AddTextType(self, typeArg, text):
		if len(typeArg) > 1:
			if typeArg[0] == "vertical":
				if typeArg[1] == "top":
					text.SetVerticalAlignTop()
				elif typeArg[1] == "bottom":
					text.SetVerticalAlignBottom()
				elif typeArg[1] == "center":
					text.SetVerticalAlignCenter()
			elif typeArg[0] == "horizontal":
				if typeArg[1] == "left":
					text.SetHorizontalAlignLeft()
				elif typeArg[1] == "right":
					text.SetHorizontalAlignRight()
				elif typeArg[1] == "center":
					text.SetHorizontalAlignCenter()

	def SetText(self, cmd):
		if len(self.children) > 1:
			self.children=[]
		multi_arg = cmd.split("\n")
		yPosition = 0
		for text in multi_arg:
			childText = ui.TextLine()
			childText.SetParent(self)
			childText.SetPosition(0,yPosition)
			if self.rgb[0] != 0:
				childText.SetFontColor(self.rgb[0],self.rgb[1],self.rgb[2])
			if self.textType != "":
				self.AddTextType(self.textType.split("#"),childText)
			if self.fontName != "":
				childText.SetFontName(self.fontName)
			if self.outline:
				childText.SetOutline()
			childText.SetText(str(text))
			childText.Show()
			self.children.append(childText)
			yPosition += self.rangeText

class ListBox(ui.Window):
	def __init__(self, isHorizontal = FALSE):
		ui.Window.__init__(self)
		self.viewItemCount = 10
		self.basePos = 0
		self.itemHeight = 16
		self.itemStep = 20
		self.itemList = []
		self.itemWidth = 100
		self.scrollBar = None
		self.isHorizontal = isHorizontal

	def __del__(self):
		ui.Window.__del__(self)

	def __GetItemCount(self):
		return len(self.itemList)

	def SetItemStep(self, itemStep):
		self.itemStep = itemStep

	def SetItemSize(self, itemWidth, itemHeight):
		self.itemWidth = itemWidth
		self.itemHeight = itemHeight

	def SetViewItemCount(self, viewItemCount):
		self.viewItemCount = viewItemCount

	def RemoveAllItems(self):
		for item in self.itemList:
			item.Destroy()
		self.itemList = []
		if self.scrollBar:
			self.scrollBar.SetPos(0)

	def GetItems(self):
		return self.itemList

	def RemoveItem(self, delItem):
		self.itemList.remove(delItem)

	def AppendItem(self, newItem):
		self.itemList.append(newItem)

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar = scrollBar

	def OnMouseWheelScroll_ScrollBar(self,mode):
		if self.scrollBar:
			if mode == "UP":
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar:
			if self.scrollBar.IsShow():
				if nLen > 0:
					self.scrollBar.OnUp()
				else:
					self.scrollBar.OnDown()
				return TRUE
		return FALSE

	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos()*self.__GetScrollLen()))

	def __GetScrollLen(self):
		if self.__GetItemCount() == 0:
			return 0
		(lx,ly) = self.itemList[len(self.itemList)-1].exPos
		return ly

	def Render(self,basePos):
		for item in self.itemList:
			(ex,ey) = item.exPos
			if self.isHorizontal:
				item.SetPosition(ex-(basePos),ey)
			else:
				item.SetPosition(ex,ey-(basePos))
			item.OnRender()

	def SetBasePos(self, basePos):
		if self.basePos == basePos:
			return
		self.Render(basePos)
		self.basePos=basePos

class ImageBoxSpecial(ui.Window):
	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.imageList = []
		self.waitingTime = 0.0
		self.sleepTime = 0.0
		self.alphaValue = 0.0
		self.increaseValue = 0.0
		self.minAlpha = 0.0
		self.maxAlpha = 0.0
		self.alphaStatus = FALSE
		self.imageIndex = 0
		self.img = None
		self.numberText = None

	def __init__(self):
		ui.Window.__init__(self)
		self.Destroy()
		self.waitingTime = 2.0
		self.alphaValue = 0.3
		self.increaseValue = 0.05
		self.minAlpha = 0.3
		self.maxAlpha = 1.0

		# Tooltip dýþarýdan set edilecek (ItemShopWindow üzerinden paylaþýmlý)
		self.tooltipItem = None

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	def SetImage(self, folder, itemVnum, itemCount):
		(x, y) = (0, 0)
		if self.img != None:
			(x, y) = self.img.GetLocalPosition()

		if self.img == None:
			img = ui.ImageBox()
			img.SetParent(self)
			img.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.OverOutItem)
			img.SAFE_SetStringEvent("MOUSE_OVER_IN", self.OverInItem)
			self.img = img

		if self.numberText == None:
			numberText = ui.NumberLine()
			numberText.SetParent(self.img)
			numberText.SetPosition(20, 20)
			self.numberText = numberText

		self.numberText.SetNumber(str(itemCount))
		self.numberText.Show()

		self.img.LoadImage(folder)
		self.img.SetPosition(x, y)
		self.img.Show()

	def OverOutItem(self):
		if None == self.tooltipItem:
			return
		
		# Tooltip'i kesinlikle kapat
		if self.tooltipItem.IsShow():
			self.tooltipItem.HideToolTip()

	def OverInItem(self):
		if None == self.tooltipItem:
			return

		self.tooltipItem.SetItemToolTip(self.imageList[self.imageIndex][1])
		# Tooltip pozisyonunu item'in yanýna sabitle
		if self.img:
			(ix, iy) = self.img.GetGlobalPosition()
			self.tooltipItem.SetPosition(ix + self.img.GetWidth() + 10, iy)
		self.tooltipItem.ShowToolTip()

	def Clear(self):
		self.img = None
		self.numberText = None
		self.imageList = []

	def LoadImage(self, imageFolder, itemVnum, itemCount):
		item_list = [imageFolder, itemVnum, itemCount]
		if item_list in self.imageList:
			return
		self.imageList.append(item_list)
		if self.img == None:
			self.SetImage(imageFolder, itemVnum, itemCount)

	def GetNextImage(self, listIndex):
		if listIndex >= len(self.imageList):
			if len(self.imageList) > 0:
				return (0,self.imageList[0][0], self.imageList[0][1], self.imageList[0][2])
			return (0,"",0,0)
		return (listIndex, self.imageList[listIndex][0], self.imageList[listIndex][0], self.imageList[listIndex][2])

	def OnUpdate(self):
		if len(self.imageList) <= 1:
			self.imageIndex=0
			return
		elif self.sleepTime > app.GetTime():
			return
		if self.alphaStatus == TRUE:
			self.alphaValue -= self.increaseValue
			if self.alphaValue < self.minAlpha:
				self.alphaValue = self.minAlpha
				self.alphaStatus = FALSE
				(imageIndex, imageFolder, itemVnum, itemCount) = self.GetNextImage(self.imageIndex+1)
				if imageFolder != "":
					self.SetImage(imageFolder, itemVnum, itemCount)
				self.imageIndex = imageIndex
		else:
			self.alphaValue += self.increaseValue
			if self.alphaValue > self.maxAlpha:
				self.alphaStatus = TRUE
				self.sleepTime = app.GetTime()+self.waitingTime
		if self.img != None:
			self.img.SetAlpha(self.alphaValue)

class MessageWindow(ui.BoardWithTitleBar):
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()

	def Destroy(self):
		self.children = {}

	def LoadMessage(self, itemVnum, itemCount):
		self.SetTitleName(localeInfo.ITEMSHOP_WHEEL_REWARD_TITLE)
		self.SetCloseEvent(self.Close)
		self.SetSize(210, 180)
		self.AddFlag("attach")
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetCenterPosition()

		item.SelectItem(itemVnum)

		multiText = MultiTextLine()
		multiText.SetParent(self)
		multiText.SetTextType("horizontal#center")
		multiText.SetPosition(105, 45)
		if itemCount > 1:
			text = "%s\nx%d %s" % (localeInfo.ITEMSHOP_WHEEL_REWARD_TEXT, itemCount,item.GetItemName())
		else:
			text = "%s\n%s" % (localeInfo.ITEMSHOP_WHEEL_REWARD_TEXT, item.GetItemName())
		multiText.SetText(text)
		multiText.Show()
		self.children["multiText"] = multiText

		itemIcon = ui.ImageBox()
		itemIcon.SetParent(self)
		itemIcon.LoadImage(item.GetIconImageFileName())
		itemIcon.SetPosition(105-16, 95)
		itemIcon.Show()
		self.children["itemIcon"] = itemIcon

		itemCountNumber = ui.NumberLine()
		itemCountNumber.SetParent(itemIcon)
		itemCountNumber.SetNumber(str(itemCount))
		itemCountNumber.SetPosition(20, 20)
		itemCountNumber.Show()
		self.children["itemCount"] = itemCountNumber

		okBtn = ui.Button()
		okBtn.SetParent(self)
		okBtn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		okBtn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		okBtn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		okBtn.SetText(localeInfo.UI_OK)
		okBtn.SetEvent(ui.__mem_func__(self.Close))
		okBtn.SetPosition(105-(okBtn.GetWidth()/2), 135)
		okBtn.Show()
		self.children["okBtn"] = okBtn

		self.Show()

	def Close(self):
		self.Destroy()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
