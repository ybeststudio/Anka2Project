if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))

import ui, localeInfo, uiToolTip, chat, item
from _weakref import proxy

MAIN_PATH = "d:/ymir work/ui/collections/"
OF_PATH = "d:/ymir work/ui/new_weekly_rank/"

BONUS_LIST = {
	0 : { 
		0: "Canavarlara Kar˛˝ G¸Ál¸ +%35", 1: "Yar˝ ›nsanlara Kar˛˝ G¸Ál¸ +%10", 2: "Ortalama Hasar: %15", 3: "Beceri Hasar +%5"
	},
	1 : { 
		0: "Ortalama Hasar: %10"
	},
	2 : { 
		0: "Canavarlara Kar˛˝ G¸Ál¸ +%15"
	},
	3 : { 
		0: "Patronlara Kar˛˝ G¸Ál¸ +%10", 1: "Yar˝ ›nsanlara Kar˛˝ G¸Ál¸ +%25"
	},
	4 : { 
		0: "Ortalama Hasar: %10", 1: "Canavarlara Kar˛˝ G¸Ál¸ +%10"
	},
	5 : { 
		0: "Yar˝ ›nsanlara Kar˛˝ G¸Ál¸ +%30", 1: "Ortalama Hasar: %15", 2: "Canavarlara Kar˛˝ G¸Ál¸ +%15"
	},
}
CATEGORY = ["Sava˛Á˝", "Birle˛tirici (I)", "Birle˛tirici (II)", "Birle˛tirici (III)", "Birle˛tirici (IV)", "Birle˛tirici (V)"]

class CollectionsWindow(ui.ScriptWindow):
	class CategoryButton(ui.RadioButton):
		BUTTON_SIZE = (135, 22)
		BUTTON_IMAGES = (OF_PATH+"button_new1.tga", OF_PATH+"button_new2.tga", OF_PATH+"button_new3.tga")
		def __init__(self, index, name, percent):
			super(CollectionsWindow.CategoryButton, self).__init__()
			self.__Index = index
			self.__Name = eval("localeInfo.%s" % (name))
			self.__Perecent = percent
			self.__Items = list()
			
			self.__NeedCount = 0
			self.__MyCount = 0
			
			self.SetUpVisual(self.BUTTON_IMAGES[0])
			self.SetOverVisual(self.BUTTON_IMAGES[1])
			self.SetDownVisual(self.BUTTON_IMAGES[2])
			self.SetText(self.__Name)
			self.SetOutline(True)
			
		def __del__(self):
			super(CollectionsWindow.CategoryButton, self).__del__()
			self.__Index, self.__NeedCount, self.__Perecent, self.__MyCount = 0,0,0,0
			self.__Name = ""
			self.__Items = list()
		
		def GetIndex(self):
			return self.__Index
			
		def AddItem(self, index, vnum, count, myCount):
			dictItem = { "index" : index, "vnum" : vnum, "count" : count, "mycount" : myCount }
			self.__Items.append(dictItem)
			self.__NeedCount += count
			self.__MyCount += myCount
		
		def GetPercent(self):
			return self.__Perecent
			
		def IsComplete(self):
			return self.__MyCount >= self.__NeedCount
			
		def UpdateItem(self, index, myCount):
			for it in self.__Items:
				if it["index"] == index:
					countDiff = myCount - it["mycount"]
					it["mycount"] = myCount
					self.__MyCount += countDiff
		
		def GetProgress(self):
			if self.__NeedCount == 0 or self.__MyCount > self.__NeedCount: #Fix: Div/0 / Higher Percent Thank 100 -> didnt happend but chuj wie kto se tam co kliknie w bazie dla test√≥w...
				return 100
			
			return int(100 * float(self.__MyCount)/float(self.__NeedCount))
			
		def GetItems(self):
			return self.__Items
		
		def GetItemLength(self):
			return len(self.__Items)
	
	def __init__(self):
		super(CollectionsWindow, self).__init__()
		self.__ActualCategory = 0
		self.base = {}
		self.itemslot = {}
		self.totalcount = {}
		self.needcount = {}
		self.bonus_text = {}
		self.__ToolTip = uiToolTip.ItemToolTip()
		self.__LoadWindow()
		
	def __del__(self):
		super(CollectionsWindow, self).__del__()
		self.__ToolTip = None
		
	def __LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/collectionswindow.py")
			
		self.__TitleBar = self.GetChild("TitleBar")
		self.__TitleBar.SetCloseEvent(ui.__mem_func__(self.Hide))

		self.items_board = self.GetChild("ItemsBox")
		self.category_text = self.GetChild("category_text")

		for i in xrange(6):
			self.bonus_text[i] = self.GetChild("bonustext_{}".format(i))

		self.__CategoryListBox = self.GetChild("CategoryBox")
		self.__CategoryListBox.SetItemSize(162, 22)
		self.__CategoryListBox.SetViewItemCount(7)
		self.__CategoryListBox.SetItemStep(25)

		self.startpos = 30
		for i in range(15):
			self.base[i]= ui.ImageBox()
			self.base[i].SetParent(self.items_board)

			if i < 5: 
				x_position = 10
				y_position = self.startpos + (i * 40)
			elif i < 10:
				x_position = 188
				y_position = self.startpos + ((i - 5) * 40)
			else:
				x_position = 366
				y_position = self.startpos + ((i - 10) * 40)

			self.base[i].LoadImage("d:/ymir work/ui/zlomiarz_window/itembox.png")
			self.base[i].SetPosition(x_position, y_position)
			self.base[i].Show()

			self.itemslot[i] = ui.SlotWindow()
			self.itemslot[i].SetParent(self.base[i])
			self.itemslot[i].SetPosition(2, 2)
			self.itemslot[i].SetSize(32, 32)
			self.itemslot[i].AppendSlot(i, 0, 0, 32, 32)
			self.itemslot[i].SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			self.itemslot[i].SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
			self.itemslot[i].SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
			self.itemslot[i].SetSelectItemSlotEvent(ui.__mem_func__(self.__SendItem))
			self.itemslot[i].RefreshSlot()
			self.itemslot[i].Show()

			self.totalcount[i] = ui.TextLine()
			self.totalcount[i].SetParent(self.base[i])
			self.totalcount[i].SetWindowHorizontalAlignCenter()
			self.totalcount[i].SetHorizontalAlignCenter()
			self.totalcount[i].SetPosition(8, 9)
			self.totalcount[i].SetFontName("Tahoma:14")
			self.totalcount[i].SetOutline(True)
			self.totalcount[i].SetText("")
			self.totalcount[i].Show()

			# self.collectimage[i] = ui.ImageBox()
			# self.collectimage[i].SetParent(self.base[i])
			# self.collectimage[i].SetPosition(50, 9)
			# self.collectimage[i].LoadImage("d:/ymir work/ui/zlomiarz_window/")
			# self.collectimage[i].Show()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def UpdateValue(self, collectionIdx, itemIndex, myCount):
		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		it = actualCategory.GetItems()[int(itemIndex)]
		actualCategory.UpdateItem(itemIndex, myCount)
		self.__RefreshSingleItem(int(collectionIdx), int(itemIndex), int(myCount))

		if (it['mycount'] >= it['count']):
			# self.totalcount[int(itemIndex)].SetText(emoji.AppendEmoji("icon/emoji/collected.png")+"|cFF66d678 - Oddane")
			self.totalcount[int(itemIndex)].SetText("|cFF66d678Teslim Edildi")
		else:
			self.totalcount[int(itemIndex)].SetText("{} / {}".format(myCount, it['count']))

	def __RefreshSingleItem(self, collectionIdx, itemIndex, myCount):
		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		if actualCategory:
			actualCategory.UpdateItem(itemIndex, myCount)
			
	def Build(self):
		self.__SelectCategory(0)

	def IsCollected(self, index):
		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		if not actualCategory:
			return False
		it = actualCategory.GetItems()[index]
		if it['mycount'] >= it['count']:
			self.itemslot[index].DisableSlot(0)
			return True
			
		return False
	
	def AddCollection(self, collIdx, name, percent):
		collectButton = self.CategoryButton(int(collIdx), name, int(percent))
		collectButton.SetEvent(ui.__mem_func__(self.__SelectCategory), self.__CategoryListBox.GetItemCount())
		self.__CategoryListBox.AppendItem(collectButton)

	def __SelectCategory(self, index):
		self.category_text.SetText(CATEGORY[index])
		# Eski kategoriyi SetUp yap (eer varsa)
		if self.__ActualCategory >= 0:
			oldCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
			if oldCategory:
				oldCategory.SetUp()
		# Yeni kategoriyi seÁ
		self.__ActualCategory = index
		newCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		if newCategory:
			newCategory.Down()
		
		self.__RefreshItems()

	def SetIncrease(self, isIncreased):
		self.__IsScrollUsed = int(isIncreased)
		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		# actualCategory kullan˝lm˝yor, sadece kontrol iÁin
	
	def AddItem(self, collectionIdx, itemIndex, itemVnum, itemCount, myCount):
		for it in self.__CategoryListBox.GetItems():
			if it.GetIndex() == int(collectionIdx):
				it.AddItem(int(itemIndex), int(itemVnum), int(itemCount), int(myCount))

	def __RefreshItems(self):
		for i in range(15):
			self.itemslot[i].ClearSlot(i)
			self.totalcount[i].SetText("-")
		for i in range(6):
			self.bonus_text[i].SetText("-")

		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		if not actualCategory:
			return
		
		for index in range(len(actualCategory.GetItems())):
			it = actualCategory.GetItems()[index]
			
			# Debug output to confirm values
			#chat.AppendChat(chat.CHAT_TYPE_INFO, "Setting item slot %d with values: index=%d, vnum=%d, count=%d" % (index, it['index'], it['vnum'], it['count']))

			self.itemslot[index].SetItemSlot(index, it["vnum"], it["count"])
			self.itemslot[index].RefreshSlot()

			if it['mycount'] >= it['count']:
				# self.totalcount[index].SetText(emoji.AppendEmoji("icon/emoji/collected.png")+"|cFF66d678 - Oddane")
				self.totalcount[index].SetText("|cFF66d678Teslim Edildi")
			else:
				self.totalcount[index].SetText("{} / {}".format(it['mycount'], it['count']))


			for i in range(len(BONUS_LIST[self.__ActualCategory])):
				self.bonus_text[i].SetText(BONUS_LIST[self.__ActualCategory][i])
			
	def	__OverInItem(self, slotpos):
		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		if slotpos < len(actualCategory.GetItems()):
			it = actualCategory.GetItems()[slotpos]

		if self.__ToolTip:
			self.__ToolTip.ClearToolTip()
			self.__ToolTip.SetItemToolTip(it["vnum"])
			# Add your Emoji or just simply add: CTRL + RMB | RMB - Send All | Send
			self.__ToolTip.AppendSpace(8)
			try:
				import emoji
				self.__ToolTip.AppendTextLine("{}".format(emoji.AppendEmoji("icon/emoji/key_lclick.tga"))+localeInfo.COLLECITON_SEND_TOOLTIP)
				self.__ToolTip.AppendSpace(5)
				self.__ToolTip.AppendTextLine("{}".format(emoji.AppendEmoji("icon/emoji/key_ctrl.tga"))+" + {}".format(emoji.AppendEmoji("icon/emoji/key_lclick.tga"))+localeInfo.COLLECTION_SEND_ALL_TOOLTIP)
			except:
				# Emoji mod¸l¸ yoksa sadece metin gˆster
				self.__ToolTip.AppendTextLine(localeInfo.COLLECITON_SEND_TOOLTIP)
				self.__ToolTip.AppendSpace(5)
				self.__ToolTip.AppendTextLine("CTRL + " + localeInfo.COLLECTION_SEND_ALL_TOOLTIP)
			self.__ToolTip.ShowToolTip()
			
	def	__OverOutItem(self):
		if self.__ToolTip:
			self.__ToolTip.HideToolTip()
			
	def __SendItem(self, selectedSlotPos):
		if self.IsCollected(selectedSlotPos):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.COLLECTION_THIS_ITEM_IS_COLLECTED)
			return

		actualCategory = self.__CategoryListBox.GetItem(self.__ActualCategory)
		
		if app.IsPressed(app.DIK_LCONTROL):
			net.SendChatPacket("/do_add_collect_item {} {} {}".format(actualCategory.GetIndex(), selectedSlotPos, 1))
		else:
			net.SendChatPacket("/do_add_collect_item {} {} {}".format(actualCategory.GetIndex(), selectedSlotPos, 0))

	def Destroy(self):
		self.__TitleBar = None
		self.__CategoryListBox = None
		self.__ActualCategory = 0
		
		self.ClearDictionary()
		self.Hide()