if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))


import ui
import mouseModule
import localeInfo
import math
import uiCommon
import constInfo
import wiki
import uiMiniMap
import uiToolTip

import chat
import skill
import nonplayer
import item
import privateShopSearch
import background
import grp

from _weakref import proxy

HEIGHT_STEP = 20
IMG_EX_DIR = "d:/ymir work/ui/game/privatesearch/"

class PrivateShopSearchDialog(ui.ScriptWindow):
	interface = None
	class SearchResultItem(ui.ExpandedImageBox):
		interface = None
		def __del__(self):
			ui.ExpandedImageBox.__del__(self)

		def Destroy(self):
			self.children = {}
			self.interface = None

		def __init__(self, parent, itemData):
			ui.ExpandedImageBox.__init__(self)
			self.SetParent(parent)
			self.Destroy()
			self.children["parent"] = proxy(parent)
			self.children["itemData"] = itemData

			self.children["renderImages"] = []
			self.children["renderText"] = []

			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.LoadImage(IMG_EX_DIR + ("item_0.tga" if itemData["isSelected"] == FALSE else "item_1.tga"))
			self.SetEvent(ui.__mem_func__(self.OnSelectBox), "mouse_click")
			self.children["renderImages"].append(self)
			self.InitItem()

		def BindInterfaceClass(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetItemName(self, itemName):
			currentItemName = itemName
			itemVnum = self.children["itemData"]["vnum"]
			metinSlot = self.children["itemData"]["metinSlot"]
			try:
				item.SelectItem(itemVnum)

				for i in xrange(item.LIMIT_MAX_NUM):
					(limitType, limitValue) = item.GetLimit(i)
					if item.LIMIT_TIMER_BASED_ON_WEAR == limitType:
						itemName = itemName
					elif item.LIMIT_REAL_TIME == limitType or item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
						itemName = itemName

				if currentItemName == itemName:
					if item.GetItemType() == item.ITEM_TYPE_UNIQUE:
						itemName = itemName
			except:
				return currentItemName

			return itemName

		def InitItem(self):
			itemData = self.children["itemData"]

			selectBox = ui.ExpandedImageBox()
			selectBox.SetParent(self)
			selectBox.SetPosition(31, 8)
			selectBox.LoadImage(IMG_EX_DIR+("not_selected.tga" if self.children["itemData"]["isSelected"] == FALSE else "selected.tga"))
			selectBox.SetEvent(ui.__mem_func__(self.OnSelectBox), "mouse_click")
			selectBox.Show()
			self.children["selectBox"] = selectBox
			self.children["renderImages"].append(selectBox)

			item_icon = ui.ExpandedImageBox()
			item_icon.SetParent(self)
			item_icon.SetPosition(67, 9)
			item_icon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OverInItem)
			item_icon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OverOutItem)
			item_icon.Show()
			self.children["renderImages"].append(item_icon)

			try:
				item.SelectItem(itemData["vnum"])
			except:
				return

			item_name_text = ui.TextLine()
			item_name_text.SetParent(item_icon)
			item_name_text.AddFlag("not_pick")
			item_name_text.SetPosition(0, 0)
			item_name_text.SetHorizontalAlignLeft()
			item_name_text.SetPackedFontColor(0xFFF7BD48)
			if 50300 == itemData["vnum"] or itemData["vnum"] == 70037:
				skillName = ""
				try:
					skillName = skill.GetSkillName(itemData["metinSlot"][0])
				except:
					skillName = ""

				if skillName:
					item_name_text.SetText("%s %s" % (skillName, item.GetItemName()))
				else:
					item_name_text.SetText(self.SetItemName(item.GetItemName()))
			elif 70104 == itemData["vnum"]:
				item_name_text.SetText("%s %s" % (nonplayer.GetMonsterName(itemData["metinSlot"][0]), item.GetItemName()))
			else:
				item_name_text.SetText(self.SetItemName(item.GetItemName()))
			item_name_text.Show()
			self.children["renderText"].append(item_name_text)

			item_icon.SetSize(*item_name_text.GetTextSize())

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelook_icon = ui.ExpandedImageBox()
				changelook_icon.SetParent(item_name_text)
				changelook_icon.SetPosition(-15, -3)
				changelook_icon.LoadImage("icon/item/ingame_convert_Mark.tga")
				changelookvnum = itemData["transmutation"]
				if not changelookvnum == 0:
					changelook_icon.Show()
				else:
					changelook_icon.Hide()
				self.children["renderImages"].append(changelook_icon)

			seller_name = ui.TextLine()
			seller_name.SetParent(self)
			seller_name.SetPosition(240, 9)
			seller_name.SetHorizontalAlignCenter()
			seller_name.SetText(itemData["seller"])
			seller_name.Show()
			self.children["renderText"].append(seller_name)

			item_count = ui.TextLine()
			item_count.SetParent(self)
			item_count.SetPosition(335, 10)
			item_count.SetHorizontalAlignCenter()
			item_count.SetText(localeInfo.NumberToDecimalString(itemData["count"]))
			item_count.Show()
			self.children["renderText"].append(item_count)

			price_text = ui.TextLine()
			price_text.SetParent(self)
			price_text.SetPosition(460, 10)
			price_text.SetHorizontalAlignCenter()
			price_text.SetText(localeInfo.NumberToMoneyString(itemData["price"]))
			price_text.Show()
			self.children["renderText"].append(price_text)

			whisper_button = ui.ExpandedImageBox()
			whisper_button.SetParent(self)
			whisper_button.LoadImage(IMG_EX_DIR + "pm_0.tga")
			whisper_button.SetPosition(555, 11)
			whisper_button.SAFE_SetStringEvent("MOUSE_OVER_IN", self.whisperIn)
			whisper_button.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.whisperOut)
			whisper_button.SetEvent(ui.__mem_func__(self.whisperClick), "mouse_click")
			whisper_button.Show()

			self.children["whisper_button"] = whisper_button
			self.children["renderImages"].append(whisper_button)

		def whisperOut(self):
			self.children["whisper_button"].LoadImage(IMG_EX_DIR + "pm_0.tga")

		def whisperIn(self):
			self.children["whisper_button"].LoadImage(IMG_EX_DIR + "pm_1.tga")

		def whisperClick(self,arg):
			self.interface.OpenWhisperDialog(self.children["itemData"]["seller"])

		def emptyFunc(self, emptyArg):
			pass

		def OnSelectBox(self, emptyArg):
			if self.children["itemData"]["buyed"]:
				self.children["itemData"]["isSelected"] = FALSE
				self.children["selectBox"].Hide()
				self.LoadImage(IMG_EX_DIR + "item_1.tga")
				self.SetEvent(ui.__mem_func__(self.emptyFunc), "mouse_click")
			else:
				newValue = not self.children["itemData"]["isSelected"]
				self.children["itemData"]["isSelected"] = newValue
				self.children["selectBox"].LoadImage(IMG_EX_DIR+("not_selected.tga" if newValue == FALSE else "selected.tga"))
				self.LoadImage(IMG_EX_DIR+("item_0.tga" if newValue == FALSE else "item_1.tga"))

		def __OverOutItem(self):
			if None != self.tooltipItem:
				if self.tooltipItem.IsShow():
					self.tooltipItem.HideToolTip()

		def __OverInItem(self):
			if None == self.tooltipItem:
				return

			itemData = self.children["itemData"]
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.isOfflineShopItem = TRUE
			
			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.tooltipItem.AddItemData(itemData["vnum"], itemData["metinSlot"], itemData["attrSlot"], 0, 0, player.INVENTORY, -1, itemData["transmutation"])
			else:
				self.tooltipItem.AddItemData(itemData["vnum"], itemData["metinSlot"], itemData["attrSlot"])
			self.tooltipItem.AppendPrice(itemData["price"])

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				changelookvnum = itemData["transmutation"]
				if not changelookvnum == 0:
					self.tooltipItem.AppendSpace(5)
					self.tooltipItem.AppendTextLine("[ " + localeInfo.CHANGE_LOOK_TITLE+" ]", self.tooltipItem.CHANGELOOK_TITLE_COLOR)
					itemName = item.GetItemNameByVnum(changelookvnum)

					if item.GetItemType() == item.ITEM_TYPE_COSTUME:
						if item.GetItemSubType() == item.COSTUME_TYPE_BODY:
							malefemale = ""
							if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
								malefemale = localeInfo.FOR_FEMALE

							if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
								malefemale = localeInfo.FOR_MALE
							itemName += " ( " + malefemale +  " )"

					textLine = self.tooltipItem.AppendTextLine(itemName, self.tooltipItem.CHANGELOOK_ITEMNAME_COLOR, TRUE)
					textLine.SetFeather()

			# Tooltip'i göster ve pozisyonunu ayarla
			self.tooltipItem.ShowToolTip()
			
			# Tooltip'in fareyi takip etmesini sa?la (normal davran??)
			try:
				if hasattr(self.tooltipItem, 'SetFollow'):
					self.tooltipItem.SetFollow(True)
			except:
				pass

		def GetShopVid(self):
			return self.shopVid

		def GetShopItemPos(self):
			return self.shopItemPos

		def OnRender(self):
			parent = self.children["parent"] if self.children.has_key("parent") else None
			renderText = self.children["renderText"] if self.children.has_key("renderText") else []
			renderImages = self.children["renderImages"] if self.children.has_key("renderImages") else []

			if parent:
				(pgx, pgy) = parent.GetGlobalPosition()
				parentHeight = parent.GetHeight()

				for selectedItem in renderText:
					(igx,igy) = selectedItem.GetGlobalPosition()
					if igy < pgy:
						selectedItem.Hide()
					elif igy > (pgy+parentHeight - 60):
						selectedItem.Hide()
					else:
						selectedItem.Show()

				for selectedItem in renderImages:
					(igx,igy) = selectedItem.GetGlobalPosition()
					itemHeight = selectedItem.GetHeight()
					if igy < pgy:
						selectedItem.SetRenderingRect(0, ui.calculateRect(itemHeight-abs(igy-pgy), itemHeight), 0, 0)
					elif igy+itemHeight > (pgy+parentHeight - 45):
						calculate = (pgy+parentHeight-45) - (igy+itemHeight)
						if calculate == 0:
							return
						selectedItem.SetRenderingRect(0.0, 0.0, 0.0, ui.calculateRect(itemHeight-abs(calculate), itemHeight))
					else:
						selectedItem.SetRenderingRect(0, 0, 0, 0)

	def __del__(self):
		ui.ScriptWindow.__init__(self)

	def Destroy(self):
		self.childrenEx = {}
		self.perPage = 0
		self.pageCount = 0
		self.interface = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.childrenEx = {}
		self.Destroy()
		self.perPage = 50
		self.pageCount = 1

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PrivateShopSearchDialog.py")
		except:
			import exception
			exception.Abort("PrivateShopSearchDialog.LoadDialog.LoadScript")

		try:
			GetObject = self.GetChild
			self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))

			GetObject("searchClearBtn").SetEvent(ui.__mem_func__(self.ClearEditlineItem))
			GetObject("searchClearBtn").Hide()

			GetObject("searchBtn").SetEvent(ui.__mem_func__(self.__Search))

			searchItemName = GetObject("searchItemName")
			searchItemName.isNeedEmpty = FALSE
			searchItemName.SetOutline()
			searchItemName.OnIMEUpdate = ui.__mem_func__(self.__ItemEditLineUpdate)
			searchItemName.SetReturnEvent(ui.__mem_func__(self.__Search))
			searchItemName.OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)

			GetObject("minLevel").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)
			GetObject("maxLevel").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)
			GetObject("minRefine").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)
			GetObject("maxRefine").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)
			GetObject("minYang").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)
			GetObject("maxYang").OnPressEscapeKey = ui.__mem_func__(self.OnPressEscapeKey)

			self.childrenEx["selectedItem"] = 0

			ListBox = GetObject("ListBox")
			scrollBar = ScrollBar3()
			scrollBar.SetParent(ListBox)
			scrollBar.SetPosition(ListBox.GetWidth()-8, 6)
			scrollBar.SetScrollBarSize(self.GetHeight()-130)
			scrollBar.SetMiddleBarSize(1.0)
			scrollBar.Show()
			ListBox.SetScrollBar(scrollBar)
			self.scrollBar = scrollBar
			self.ListBox = ListBox

			self.pageButtons = []
			for l in xrange(5):
				self.pageButtons.append(GetObject("page%d_button" % (l+1)))
				self.pageButtons[l].Hide()

			GetObject("buyBtn").SetEvent(ui.__mem_func__(self.OnSearchResultItemSelect))
			GetObject("next_button").SetEvent(ui.__mem_func__(self.NextPage))
			GetObject("prev_button").SetEvent(ui.__mem_func__(self.PrevPage))
			GetObject("first_prev_button").SetEvent(ui.__mem_func__(self.FirstPage))
			GetObject("last_next_button").SetEvent(ui.__mem_func__(self.LastPage))

			board = GetObject("board")

			subTypeList = {
				0 : {
					0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_SWORD,
					1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_DAGGER,
					2: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_BOW,
					3: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_TWOHAND,
					4: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_BELL,
					5: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS_FAN,
				},
				1: {
					0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_ARMOUR,
					1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_HELMETS,
					2: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_SHIELD,
					3: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_BRACELETS,
					4: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_SHOES,
					5: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_NECKLACES,
					6: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_EARRINGS,
					7: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_BELTS,
					8: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT_PENDANT,
				},
				2: {
					0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_ARMOUR,
					1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_HAIR,
					2: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_ACCE,
					3: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_WEAPON,
					4: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_MOUNT_SKIN,
					5: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_PET_SKIN,
					6: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES_AURA,
				},
				3: {
					0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_BOOKS_SKILLS,
					1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_BOOKS_COMPANIONS,
				},
				4: {
					0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COMPANIONS_MOUNT,
					1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COMPANIONS_PET,
					2: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COMPANIONS_PET2,
				},
			}

			subType = ui.ComboBoxImage(board, IMG_EX_DIR + "category_input.tga", 21, 166)
			subType.SetParent(board)
			for index, data in subTypeList.iteritems():
				subType.InsertItem(index, data)
			subType.SetEvent(lambda subItemNumber, proxySelf = proxy(self) : proxySelf.__ClickComboList("sub", subItemNumber))
			subType.Show()
			self.childrenEx["subType"] = subType
			self.childrenEx["subTypeList"] = subTypeList

			itemTypeList = {
				-1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_ALL,
				0: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_WEAPONS,
				1: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_EQUIPMENT,
				2: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COSTUMES,
				3: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_BOOKS,
				4: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_COMPANIONS,
				5: localeInfo.OFFLINESHOP_SEARCH_OBJECTS_OTHERS,
			}

			itemType = ui.ComboBoxImage(board, IMG_EX_DIR + "category_input.tga", 21, 137)
			itemType.SetParent(board)

			for j in range(-1, len(itemTypeList)-1):
				itemType.InsertItem(j, itemTypeList[j])

			itemType.SetEvent(lambda subItemNumber, proxySelf = proxy(self) : proxySelf.__ClickComboList("item", subItemNumber))
			itemType.Show()
			self.childrenEx["itemType"] = itemType
			self.childrenEx["itemTypeList"] = itemTypeList
			self.__ClickComboList("item", -1)

			raceTypeList = {
				0: localeInfo.OFFLINESHOP_SEARCH_CLASS_ALL,
				1: localeInfo.OFFLINESHOP_SEARCH_CLASS_WARRIOR,
				2: localeInfo.OFFLINESHOP_SEARCH_CLASS_ASSASSIN,
				3: localeInfo.OFFLINESHOP_SEARCH_CLASS_SHAMAN,
				4: localeInfo.OFFLINESHOP_SEARCH_CLASS_SURA,
			}

			raceType = ui.ComboBoxImage(board, IMG_EX_DIR + "category_input.tga", 21, 70)
			raceType.SetParent(board)

			for index, data in raceTypeList.iteritems():
				raceType.InsertItem(index, data)

			raceType.SetEvent(lambda subItemNumber, proxySelf = proxy(self) : proxySelf.__ClickComboList("race", subItemNumber))
			raceType.Show()
			self.childrenEx["raceType"] = raceType
			self.childrenEx["raceTypeList"] = raceTypeList
			self.__ClickComboList("race", 0)

		except:
			import exception
			exception.Abort("PrivateShopSearchDialog.LoadDialog.BindObject")

		self.searchBuyItem = uiCommon.QuestionDialog2()
		self.searchBuyItem.Hide()

		self.FirstPage()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def OpenWhisperDialog(self, arg):
		self.interface.OpenWhisperDialog(arg)

	def __ClickComboList(self, comboType, comboIndex):
		self.childrenEx["%sTypeIndex" % comboType] = comboIndex
		self.childrenEx["%sType" % comboType].SetCurrentItem(self.childrenEx["%sTypeList" % comboType][self.childrenEx["itemTypeIndex"]][comboIndex] if comboType == "sub" else self.childrenEx["%sTypeList" % comboType][comboIndex])
		self.childrenEx["%sType" % comboType].CloseListBox()

		if comboType == "item":
			subType = self.childrenEx["subType"]
			subTypeList = self.childrenEx["subTypeList"]
			subType.ClearItem()
			if subTypeList.has_key(comboIndex):
				for index, data in subTypeList[comboIndex].iteritems():
					subType.InsertItem(index, data)
				subType.SetCurrentItem(subTypeList[comboIndex][0])
			else:
				subType.SetCurrentItem("-")
			self.childrenEx["subTypeIndex"] = 0

	def __ItemEditLineUpdate(self):
		ui.EditLine.OnIMEUpdate(self.GetChild("searchItemName"))
		if not self.UpdateItemsList():
			self.ClearEditlineItem()

		def OnKeyDown(self, key):
			if app.DIK_RETURN == key:
				self.__Search()
				return TRUE
			return TRUE

	def ClearEditlineItem(self):
		self.childrenEx["selectedItem"] = 0
		searchItemName = self.GetChild("searchItemName")
		searchItemName.SetText("")
		searchItemName.SetInfoMessage(localeInfo.OFFLINESHOP_SEARCH_ITEM_NAME)
		self.GetChild("searchClearBtn").Hide()

	def get_length(self, x):
		return len(x[0])

	def UpdateItemsList(self):
		searchItemName = self.GetChild("searchItemName")
		input_text_real = searchItemName.GetText()
		input_len = len(input_text_real)

		if input_len == 0:
			self.ClearEditlineItem()
			return FALSE
		input_text = input_text_real.lower()

		self.GetChild("searchClearBtn").Show()
		itemList = []
		namesList = []

		items_list = item.GetItemsByName(str(input_text))
		for i, itemVnum in enumerate(items_list, start=1):
			item.SelectItem(itemVnum)
			tempName = list(item.GetItemName().lower())
			for i in xrange(input_len):
				tempName[i]=list(input_text_real)[i]
			itemName = ""
			for x in xrange(len(tempName)):
				itemName+=tempName[x]
			if itemName in namesList:
				continue
			namesList.append(itemName)
			itemList.append([itemName, itemVnum])
		if len(itemList) > 0:
			if len(itemList) > 1:
				itemList = sorted(itemList, key = self.get_length, reverse = FALSE)
			self.childrenEx["selectedItem"] = itemList[0][1]
			searchItemName.SetInfoMessage(itemList[0][0])
		else:
			self.childrenEx["selectedItem"] = 0
			searchItemName.SetInfoMessage("")

		return TRUE

	def OpenForItem(self, itemVnum):
		if not itemVnum:
			return
		self.Show()
		self.SetCenterPosition()
		self.SetTop()
		self.childrenEx['isFirstOpen'] = FALSE
		self.GetChild('searchItemName').SetText(item.GetItemNameByVnum(itemVnum))
		self.__Search()

	def Open(self, type):
		self.Show()
		self.SetCenterPosition()
		self.SetTop()

		isFirstOpen = self.childrenEx["isFirstOpen"] if self.childrenEx.has_key("isFirstOpen") else TRUE
		if isFirstOpen:
			self.__Search()
			self.childrenEx["isFirstOpen"] = FALSE

	def RefreshMe(self):
		self.childrenEx["searchingStatus"] = FALSE
		self.GetChild("listboxText").Hide()

		itemDataList = self.childrenEx["itemDataList"] if self.childrenEx.has_key("itemDataList") else []
		itemDataList[:] = []
		self.ListBox.RemoveAllItems()

		totalItemCount = privateShopSearch.GetItemCount()
		for itemIndex in xrange(privateShopSearch.GetItemCount()):
			dataList = {}
			dataList["isSelected"] = FALSE
			dataList["buyed"] = FALSE
			dataList["id"] = privateShopSearch.GetSearchItemID(itemIndex)
			dataList["vnum"] = privateShopSearch.GetSearchItemVnum(itemIndex)
			dataList["count"] = privateShopSearch.GetSearchItemCount(itemIndex)
			dataList["price"] = privateShopSearch.GetSearchItemPrice(itemIndex)
			dataList["vid"] = privateShopSearch.GetSearchItemShopVID(itemIndex)
			dataList["seller"] = privateShopSearch.GetSearchItemSeller(itemIndex)
			dataList["itemPos"] = privateShopSearch.GetSearchItemPos(itemIndex)
			dataList["metinSlot"] = [ privateShopSearch.GetSearchItemMetinSocket(itemIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM) ]
			dataList["attrSlot"] = [ privateShopSearch.GetSearchItemAttribute(itemIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM) ]
			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				dataList["transmutation"] = privateShopSearch.GetSearchItemTransmutation(itemIndex)
			itemDataList.append(dataList)
		self.childrenEx["itemDataList"] = itemDataList

		self.pageCount = int(math.ceil(float(totalItemCount) / float(self.perPage)))
		self.currentPaginationPage = 1
		self.paginationPageCount = int(math.ceil(float(self.pageCount) / 5.0))
		self.FirstPage()

	def RefreshList(self):
		itemDataList = self.childrenEx["itemDataList"] if self.childrenEx.has_key("itemDataList") else []

		self.ListBox.RemoveAllItems()
		self.RefreshPaginationButtons()

		start = (self.currentPage - 1) * self.perPage
		end = ((self.currentPage - 1) * self.perPage) + self.perPage
		currentPageDict = itemDataList[start:end]
		for j, data in enumerate(currentPageDict):
			resultItem = PrivateShopSearchDialog.SearchResultItem(self.ListBox, data)
			resultItem.BindInterfaceClass(self)
			resultItem.SetPosition(0, 4+(37*j), TRUE)
			resultItem.Show()
			self.ListBox.AppendItem(resultItem)
		self.ListBox.SetBasePos(0)
		self.ListBox.Render(0)

		scrollBar = self.scrollBar
		scrollBar.SetMiddleBarSize(1.0)
		scrollBar.SetScrollStep(0.2)

		if len(currentPageDict) > 10:
			maxHeight = (4+(37*(len(currentPageDict)-1)))
			windowHeight = self.ListBox.GetHeight()

			if maxHeight+80 > windowHeight:
				scrollBar.SetMiddleBarSize(float(windowHeight)/float(maxHeight+80))

	def RefreshPaginationButtons(self):
		self.currentPaginationPage = int(math.ceil(float(self.currentPage) / 5.0 ))
		self.shownPages = min(self.pageCount - (5 * (self.currentPaginationPage - 1)), 5)

		for x in xrange(5):
			currentPage = (x + ((self.currentPaginationPage-1) * 5) + 1)
			self.pageButtons[x].SetUp()
			self.pageButtons[x].SetText("%d" % currentPage)
			self.pageButtons[x].SetEvent(lambda arg=currentPage: self.GotoPage(arg))
		map(ui.Button.Hide, self.pageButtons)
		map(ui.Button.Enable, self.pageButtons)

		for x in xrange(self.shownPages):
			self.pageButtons[x].Show()
		self.pageButtons[(self.currentPage - ((self.currentPaginationPage - 1) * 5)) - 1].Down()
		self.pageButtons[(self.currentPage - ((self.currentPaginationPage - 1) * 5)) - 1].Disable()

	def GotoPage(self, page):
		self.currentPage = page
		self.RefreshList()

	def FirstPage(self):
		self.GotoPage(1)

	def LastPage(self):
		self.GotoPage(self.pageCount)

	def NextPage(self):
		if self.currentPage < self.pageCount:
			self.GotoPage(self.currentPage + 1)

	def PrevPage(self):
		if self.currentPage > 1:
			self.GotoPage(self.currentPage - 1)

	def GetSafeText(self, childName, defaultValue):
		try:
			returnValue = int(self.GetChild(childName).GetText())
		except:
			returnValue = defaultValue
		return returnValue

	def __Search(self):
		privateShopSearch.ClearSearchItems()
		self.RefreshMe()

		(minAbsValue, maxAbsValue) = (0, 30)
		(minAvgValue, maxAvgValue) = (0, 30)
		(minSkillValue, maxSkillValue) = (0, 40)
		sexValue = alchemyValue = bonusValue = 0

		net.SendPrivateShopSearchInfo(self.childrenEx["itemTypeIndex"], self.childrenEx["subTypeIndex"], minAvgValue, maxAvgValue, minSkillValue, maxSkillValue, minAbsValue, maxAbsValue, self.GetSafeText("minLevel", 0), self.GetSafeText("maxLevel", 120), self.GetSafeText("minRefine", 0), self.GetSafeText("maxRefine", 200), bonusValue, alchemyValue, sexValue, self.childrenEx["raceTypeIndex"], self.GetSafeText("minYang", 0), self.GetSafeText("maxYang", 9999999999999999), self.GetChild("searchItemName").GetText().lower())

		self.childrenEx["searchingStatus"] = TRUE
		self.childrenEx["searchingTotal"] = app.GetGlobalTimeStamp() + 10

	def SetItemBuyStatus(self, itemID):
		itemsList = self.ListBox.itemList
		for searchItem in itemsList:
			if searchItem.children["itemData"]["id"] == itemID:
				searchItem.children["itemData"]["buyed"] = TRUE
				searchItem.OnSelectBox("")
				break

	def OnUpdate(self):
		# Tooltip kontrolü: Private shop search'teki item tooltip'lerinin ekranda kalmas?n? önle
		try:
			import uiToolTip
			import wndMgr
			
			# ListBox'taki tüm item'lar? kontrol et
			if hasattr(self, 'ListBox') and self.ListBox:
				itemsList = getattr(self.ListBox, 'itemList', [])
				for searchItem in itemsList:
					if hasattr(searchItem, 'tooltipItem') and searchItem.tooltipItem:
						tooltipItem = searchItem.tooltipItem
						if tooltipItem.IsShow():
							try:
								(x, y) = wndMgr.GetMousePosition()
								(tx, ty) = tooltipItem.GetGlobalPosition()
								tw, th = tooltipItem.GetWidth(), tooltipItem.GetHeight()
								onTooltip = (tx <= x <= tx + tw and ty <= y <= ty + th)
								
								# Item icon'unun üzerinde mi kontrol et
								itemIcon = None
								if hasattr(searchItem, 'children') and searchItem.children.has_key("renderImages"):
									for img in searchItem.children["renderImages"]:
										if img != searchItem:  # Kendisi de?ilse
											try:
												(ix, iy) = img.GetGlobalPosition()
												iw, ih = img.GetWidth(), img.GetHeight()
												if ix <= x <= ix + iw and iy <= y <= iy + ih:
													itemIcon = img
													break
											except:
												pass
								
								# Ne tooltip'in üzerinde ne de item icon'unun üzerinde de?ilse kapat
								if not onTooltip and not itemIcon:
									tooltipItem.HideToolTip()
							except:
								pass
		except:
			pass
		
		searchingStatus = self.childrenEx["searchingStatus"] if self.childrenEx.has_key("searchingStatus") else FALSE
		if searchingStatus:
			searchingTotal = self.childrenEx["searchingTotal"] if self.childrenEx.has_key("searchingTotal") else 0
			if app.GetGlobalTimeStamp() > searchingTotal:
				self.childrenEx["searchingStatus"] = FALSE
				self.GetChild("listboxText").Hide()
				return

			searchingTime = self.childrenEx["searchingTime"] if self.childrenEx.has_key("searchingTime") else 0.0
			if searchingTime > app.GetTime():
				return
			self.childrenEx["searchingTime"] = app.GetTime() + 0.2

			searchingIncrease = self.childrenEx["searchingIncrease"] if self.childrenEx.has_key("searchingIncrease") else 0
			searchingIncrease += 1
			if searchingIncrease > 5:
				searchingIncrease = 1
			self.childrenEx["searchingIncrease"] = searchingIncrease
			self.GetChild("listboxText").SetText(localeInfo.OFFLINESHOP_SEARCH_WAIT+(searchingIncrease * "."))
			self.GetChild("listboxText").Show()

	def OnSearchResultItemSelect(self):
		lastClick = self.childrenEx["lastClick"] if self.childrenEx.has_key("lastClick") else 0
		if lastClick > app.GetGlobalTimeStamp():
			chat.AppendChat(1, localeInfo.OFFLINESHOP_SEARCH_WAIT_SPAM)
			return

		self.childrenEx["lastClick"] = app.GetGlobalTimeStamp()+5
		selectedItems = []
		totalPrice = 0
		for data in self.childrenEx["itemDataList"]:
			if data["isSelected"] == 1:
				totalPrice+=data["price"]
				selectedItems.append([data["vid"], data["itemPos"]])

		if len(selectedItems) == 0:
			chat.AppendChat(1, localeInfo.OFFLINESHOP_SEARCH_SELECT_ITEM)
			return
		elif totalPrice > player.GetElk():
			chat.AppendChat(1, localeInfo.OFFLINESHOP_SEARCH_NOT_ENOUGH_YANG)
			return

		if len(selectedItems) == 1:
			self.searchBuyItem.SetText1(localeInfo.OFFLINESHOP_SEARCH_BUTTON_BUY_QUESTION % (len(selectedItems)))
		else:
			self.searchBuyItem.SetText1(localeInfo.OFFLINESHOP_SEARCH_BUTTON_BUY_QUESTION2 % (len(selectedItems)))
		self.searchBuyItem.SetText2(localeInfo.OFFLINESHOP_SEARCH_BUTTON_BUY_QUESTION_TOTAL % (localeInfo.NumberToMoneyString(totalPrice)))
		self.searchBuyItem.SetAcceptEvent(lambda arg=TRUE: self.__AnswerBuyItem(arg))
		self.searchBuyItem.SetCancelEvent(lambda arg=FALSE: self.__AnswerBuyItem(arg))
		self.searchBuyItem.selectedItems = selectedItems
		self.searchBuyItem.Open()

	def __AnswerBuyItem(self, result):
		if result:
			buyList = self.searchBuyItem.selectedItems
			for data in buyList:
				net.SendOfflineShopBuyPacket(data[0], data[1])
			self.searchBuyItem.selectedItems = []
		self.searchBuyItem.Close()

	def OnKeyDown(self, key):
		if key == 28:
			self.__Search()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class ScrollBar3(ui.Window):
	SCROLLBAR_WIDTH = 10
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
			top.LoadImage(IMG_EX_DIR+"scrollbar_ex/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_EX_DIR+"scrollbar_ex/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_EX_DIR+"scrollbar_ex/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_EX_DIR+"scrollbar_ex/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_EX_DIR+"scrollbar_ex/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle

		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
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
		self.scrollStep = 0.2
		self.SetWindowName("NONAME_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_EX_DIR+"scrollbar_ex/scroll_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_EX_DIR+"scrollbar_ex/scroll_bottom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_EX_DIR+"scrollbar_ex/scroll_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		self.middleBar = middleBar

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

	def SetScrollStep(self, step):
		self.scrollStep = step

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

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
