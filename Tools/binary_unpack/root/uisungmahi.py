if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
import constInfo
import grp
import ui
import mouseModule
import localeInfo
import wndMgr
import event
import chat
import uiScriptLocale

BOARD_WIDTH = 348
BOARD_HEIGHT = 232

PATTERN_WINDOW_WIDTH	= 326
PATTERN_WINDOW_HEIGHT	= 162

TOP_BOARD_SPACE = 30
SIDE_BOARD_SPACE = 8

SCREEN_WIDTH = wndMgr.GetScreenWidth()
SCREEN_HEIGHT = wndMgr.GetScreenHeight()

MAX_LEVEL_COUNT = 50
MAX_REWARD_COUNT = 5

class SungMahiListItem(ui.RadioButton):
	showEvent = None
	
	def __init__(self):
		ui.RadioButton.__init__(self)
		
	def __del__(self):
		ui.RadioButton.__del__(self)
		
	def Destroy(self):
		self.isDungeonCompleted = None
		self.showEvent = None
		
	def MakeButton(self, dungeonLevel):
		self.SetUpVisual("d:/ymir work/ui/game/sungmahee_tower/floor_button_default.sub")
		self.SetOverVisual("d:/ymir work/ui/game/sungmahee_tower/floor_button_over.sub")
		self.SetDownVisual("d:/ymir work/ui/game/sungmahee_tower/floor_button_down.sub")
		
		self.SetText(localeInfo.SUNGMAHEE_TOWER_FLOOR_TEXT.format(dungeonLevel))
		self.SetEvent(ui.__mem_func__(self.__OnClickShowButton))
		
		self.isDungeonCompleted = ui.ImageBoxSungMahi()
		self.isDungeonCompleted.SetParent(self)
		self.isDungeonCompleted.LoadImage("d:/ymir work/ui/game/sungmahee_tower/clear_icon.sub")
		self.isDungeonCompleted.SetPosition(107, 7)
		self.isDungeonCompleted.Hide()
		
	def SetShowEvent(self, event):
		self.showEvent = event
	
	def __OnClickShowButton(self):
		if self.showEvent:
			self.showEvent()
	
	def MarkAsCompleted(self):
		self.isDungeonCompleted.Show()

class SungMahiWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)

		try:
			self.__Initialize()
			self.__LoadWindow()
			self.__BindEvents()
		
		except:
			import exception
			exception.Abort("SungMahiWindow.__init__")
	
	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def __Initialize(self):
		self.tooltipItem = None
		self.dlgQuestion = None
		
		self.patternWindow = None
		self.enterButton = None
		self.elementWindow = None
		self.elementText = None
		
		self.scrollbar = None
		
		self.levelListBox = None
		self.levelListItems = []
		self.previousListItem = 0
		
		self.divisonImage = None
		
		self.rewardInfoBG = None
		self.rewardInfoText = None
		self.rewardSlotWindow = None
		self.rewardItemVnums = []
		
		self.rankTitleBG = None
		self.rankTitleText = None
		self.rankInfoBG = None
		self.rankInfoText = None
		self.rankTimeText = None
		
	def __LoadWindow(self):
		self.AddFlag("movable")
		self.SetSize(BOARD_WIDTH, BOARD_HEIGHT)
		self.SetTitleName(uiScriptLocale.SUNGMAHEE_TOWER_ENTER_TITLE)
		
		self.patternWindow = ui.BoarderA()
		self.patternWindow.SetParent(self)
		self.patternWindow.SetSize(PATTERN_WINDOW_WIDTH, PATTERN_WINDOW_HEIGHT)
		self.patternWindow.SetPosition(10, 33)
		self.patternWindow.Show()
		
		self.enterButton = ui.Button()
		self.enterButton.SetParent(self)
		self.enterButton.SetPosition(248, 199)
		self.enterButton.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.enterButton.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.enterButton.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.enterButton.SetEvent(self.OpenQuestionDialog)
		self.enterButton.SetText(uiScriptLocale.SUNGMAHEE_TOWER_ENTER_BUTTON)
		self.enterButton.Show()
		
		self.scrollbar = ui.ScrollBar()
		self.scrollbar.SetParent(self)
		self.scrollbar.SetScrollBarSize(156)
		self.scrollbar.SetPosition(144, 37)
		self.scrollbar.Show()
		
		self.levelListBox = ui.ListBoxEx()
		self.levelListBox.SetParent(self)
		self.levelListBox.SetPosition(15, 38)
		# Liste kutusu geniþliði: scrollbar pozisyonuna kadar (scrollbar ile arasýnda küçük bir boþluk)
		listBoxWidth = 144 - 15 - 1  # scrollbar pozisyonu (144) - liste pozisyonu (15) - boþluk (1)
		listBoxHeight = 156 - 2  # scrollbar yüksekliði (156) - boþluk (2)
		self.levelListBox.SetSize(listBoxWidth, listBoxHeight)
		# Clipping mask ayarla: patternWindow'u kullanarak liste öðelerinin board sýnýrlarýný aþmasýný önle
		if app.ENABLE_CLIP_MASKING:
			self.levelListBox.SetClippingMaskWindow(self.patternWindow)
		# SetInsideRender ile içeride render et (board sýnýrlarýný aþmasýn)
		self.levelListBox.SetInsideRender(True)
		self.levelListBox.SetScrollBar(self.scrollbar)
		# Öðe geniþliði: liste kutusu geniþliðinden küçük (taþmayý önlemek için)
		# Yýldýz ikonu için saðdan yeterli boþluk býrakýlmalý (yýldýz ikonu geniþliði + boþluk)
		itemWidth = listBoxWidth - 6  # Liste kutusu geniþliðinden 6 piksel küçük (yýldýz ikonu için güvenli alan)
		self.levelListBox.SetItemSize(126, 28)
		self.levelListBox.SetItemStep(31)
		self.levelListBox.SetViewItemCount(5)
		self.levelListBox.Show()
		
		for levelCount in range(MAX_LEVEL_COUNT, 0, -1):
			button = SungMahiListItem()
			button.SetParent(self)
			button.MakeButton(levelCount)
			button.SetShowEvent(lambda arg = levelCount : self.__LoadLevelInfo(arg))
			button.Show()
			
			self.levelListBox.AppendItem(button)
			# Her öðeye patternWindow'u clipping mask olarak ayarla (board sýnýrlarýný aþmasýn)
			if app.ENABLE_CLIP_MASKING:
				button.SetClippingMaskWindow(self.patternWindow)
			self.levelListItems.append(button)
		
		self.levelListItems.reverse()
		self.scrollbar.SetPos(1.0)
		
		self.divisonImage = ui.ImageBoxSungMahi()
		self.divisonImage.SetParent(self)
		self.divisonImage.LoadImage("d:/ymir work/ui/game/sungmahee_tower/division_line.sub")
		self.divisonImage.SetPosition(158, 35)
		self.divisonImage.Show()
		
		self.rewardInfoBG = ui.ImageBoxSungMahi()
		self.rewardInfoBG.SetParent(self)
		self.rewardInfoBG.LoadImage("d:/ymir work/ui/game/sungmahee_tower/menu_bg.sub")
		self.rewardInfoBG.SetPosition(161, 36)
		self.rewardInfoBG.Show()
		
		self.rewardInfoText = ui.TextLine()
		self.rewardInfoText.SetParent(self)
		self.rewardInfoText.SetText(uiScriptLocale.SUNGMAHEE_TOWER_REWARD_INFO_TITLE)
		self.rewardInfoText.SetPosition(220, 39)
		self.rewardInfoText.Show()
		
		self.rewardSlotWindow = ui.SlotWindow()
		self.rewardSlotWindow.SetParent(self)
		self.rewardSlotWindow.SetPosition(167, 67)
		self.rewardSlotWindow.SetSize(160, 32)
		for slotCount in xrange(MAX_REWARD_COUNT):
			self.rewardSlotWindow.AppendSlot(slotCount, (32*slotCount), 0, 32, 32)
		self.rewardSlotWindow.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		self.rewardSlotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.rewardSlotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.rewardSlotWindow.Show()
		
		self.rankTitleBG = ui.ImageBoxSungMahi()
		self.rankTitleBG.SetParent(self)
		self.rankTitleBG.LoadImage("d:/ymir work/ui/game/sungmahee_tower/menu_bg.sub")
		self.rankTitleBG.SetPosition(161, 108)
		self.rankTitleBG.Show()
		
		self.rankTitleText = ui.TextLine()
		self.rankTitleText.SetParent(self)
		self.rankTitleText.SetText(uiScriptLocale.SUNGMAHEE_TOWER_MY_RANK_TITLE)
		self.rankTitleText.SetPosition(235, 111)
		self.rankTitleText.Show()
		
		self.rankInfoBG = ui.ImageBoxSungMahi()
		self.rankInfoBG.SetParent(self)
		self.rankInfoBG.LoadImage("d:/ymir work/ui/game/sungmahee_tower/my_rank_bg.sub")
		self.rankInfoBG.SetPosition(168, 136)
		self.rankInfoBG.Show()
		
		self.rankInfoText = ui.TextLine()
		self.rankInfoText.SetParent(self)
		self.rankInfoText.SetPosition(245, 141)
		self.rankInfoText.SetHorizontalAlignCenter()
		self.rankInfoText.Show()
		
		self.rankTimeText = ui.TextLine()
		self.rankTimeText.SetParent(self)
		self.rankTimeText.SetHorizontalAlignCenter()
		self.rankTimeText.SetPosition(245, 165)
		self.rankTimeText.Show()
		
		self.elementWindow = ui.ImageBoxSungMahi()
		self.elementWindow.SetParent(self)
		self.elementWindow.LoadImage("d:/ymir work/ui/game/sungmahee_tower/element_text_bg.sub")
		self.elementWindow.SetPosition(11, 200)
		self.elementWindow.SAFE_SetEvent(self.OverInElement, "MOUSE_OVER_IN")
		self.elementWindow.SAFE_SetEvent(self.OverOutItem, "MOUSE_OVER_OUT")
		self.elementWindow.Show()
		
		self.elementText = ui.TextLine()
		self.elementText.SetParent(self)
		self.elementText.SetPosition(63, 202)
		self.elementText.SetHorizontalAlignCenter()
		self.elementText.Show()
		
		self.__LoadQuestionDialog()
	
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
	
	def __BindEvents(self):
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		
	def __LoadQuestionDialog(self):
		self.dlgQuestion = ui.ScriptWindow()
		
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgQuestion, "uiscript/questiondialog.py")
		except:
			import exception
			exception.Abort("RefineDialog.__LoadQuestionDialog.LoadScript")
			
		try:
			GetObject=self.dlgQuestion.GetChild
			self.dialogMessage = GetObject("message")
			
			GetObject("accept").SetEvent(ui.__mem_func__(self.Accept))
			GetObject("cancel").SetEvent(ui.__mem_func__(self.dlgQuestion.Hide))
		except:
			import exception
			exception.Abort("SungMahiWindow.__LoadQuestionDialog.BindObject")
			
	def OpenQuestionDialog(self):
		self.dialogMessage.SetText(uiScriptLocale.SUNGMAHEE_TOWER_DUNGEON_ENTER_QUESTION % (self.previousListItem + 1))
		self.dlgQuestion.SetTop()
		self.dlgQuestion.Show()

	def Accept(self):
		if (constInfo.sungMahiLevelInfo != self.previousListItem):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SUNGMAHEE_TOWER_ENTER_FAIL_FLOOR)
			return
		
		event.QuestButtonClick(int(constInfo.sungMahiQuest))
		self.Close()
		
	def Destroy(self):
		self.__Initialize()
	
	def __LoadLevelInfo(self, levelIndex):
		# try:
		levelIndex = (levelIndex - 1)
		if levelIndex > MAX_LEVEL_COUNT:
			return
		
		if levelIndex < 0:
			levelIndex = 0
			
		self.levelListItems[self.previousListItem].SetUp()
		self.levelListItems[levelIndex].Down()
		self.previousListItem = levelIndex
		
		self.rankInfoText.SetText("-")
		levelInfoRank = constInfo.sungMahiInfo[levelIndex][1]
		if levelInfoRank > 0:
			self.rankInfoText.SetText(localeInfo.SUNGMAHEE_TOWER_MY_RANK_TEXT.format(levelInfoRank))
		
		self.rankTimeText.SetText("-")
		levelInfoTime = constInfo.sungMahiInfo[levelIndex][2]
		if levelInfoTime > 0:
			self.rankTimeText.SetText("{}".format(localeInfo.SecondToMS(levelInfoTime)))
		
		self.SetLevelElement(levelIndex)
		self.SetLevelRewards(levelIndex)
		# except:
			# dbg.TraceError("Error at LoadLevelInfo - SungMahi! Loading level %d" % levelIndex)
	
	def UpdateScrollPos(self, dungeonsCompleted):
		completedCount = (1.0 - ((0.1090 * (dungeonsCompleted / 5))))
		if dungeonsCompleted <= 5:
			completedCount = 1.0
			
		self.scrollbar.SetPos(completedCount)
		
	def SetCompletedDungeons(self, dungeonsCompleted):
		for index in xrange(dungeonsCompleted):
			self.levelListItems[index].MarkAsCompleted()
			
		self.UpdateScrollPos(dungeonsCompleted)
		self.__LoadLevelInfo(dungeonsCompleted)
		
	def SetLevelElement(self, levelIndex):
		if len(constInfo.sungMahiElementInfo) < levelIndex:
			return
		
		elementNameDict = {'FIRE': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_FIRE, 'ICE': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_ICE, 'ELEC': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_ELEC, 'WIND': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_WIND, 'EARTH': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_EARTH, 'DARK': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_DARK, 'ALL': uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_ALL}
		self.elementText.SetText(elementNameDict[constInfo.sungMahiElementInfo[levelIndex][1]])
	
	def SetLevelRewards(self, levelIndex):
		if len(constInfo.sungMahiRewardInfo) < levelIndex:
			return
		
		indexContor = 0
		itemSlotContor = 0
		
		self.rewardItemVnums = []
		for slot in xrange(self.rewardSlotWindow.GetSlotCount()):
			self.rewardSlotWindow.ClearSlot(slot)
		
		for itemInfo in constInfo.sungMahiRewardInfo[levelIndex]:
			if (indexContor < 1):
				indexContor += 1
				continue
			
			if int(itemInfo[0]) > 0:
				self.rewardSlotWindow.SetItemSlot(itemSlotContor, int(itemInfo[0]), int(itemInfo[1]))
				self.rewardItemVnums.append(itemInfo[0])
				itemSlotContor += 1
		
		self.rewardSlotWindow.RefreshSlot()
		
	def OverInItem(self, overSlotPos):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			
			itemIndex = int(self.rewardItemVnums[overSlotPos])
			if itemIndex:
				self.tooltipItem.AddItemData(itemIndex, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
				self.tooltipItem.ShowToolTip()
	
	def OverInElement(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			
			self.tooltipItem.AppendTextLine(uiScriptLocale.SUNGMAHEE_TOWER_ELEMENT_TITLE)
			self.tooltipItem.ShowToolTip()
			
	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
			
	def Open(self):
		self.SetCompletedDungeons(constInfo.sungMahiLevelInfo)
		
		self.Show()
		self.SetTop()
		self.SetCenterPosition()
	
	def Close(self):
		self.dlgQuestion.Hide()
		self.Hide()
		
		return True
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
