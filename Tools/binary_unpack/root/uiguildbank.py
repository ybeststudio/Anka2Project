if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import localeInfo
import ime
import uiScriptLocale
import guild
import mouseModule
import snd
#import guildbank
if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
	import uiToolTip
	import wndMgr

class GuildBankLogDialog(ui.ScriptWindow):
	SLOTLINECOUNTER = 12
	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		GUILD_GOLD_TYPE_IN = 0
		GUILD_GOLD_TYPE_OUT = 1
		GUILD_GOLD_TYPE_PROMOTE = 2
		GUILD_GOLD_TYPE_PROMOTE_UPDATE = 3
		GUILD_GOLD_TYPE_OBJECT_CHANGE = 4
		GUILD_GOLD_TYPE_OBJECT_CREATE = 5
		GUILD_GOLD_TYPE_DRAGON_SP_RECOVER = 6
		GUILD_GOLD_TYPE_BUY_GUILD_LAND = 7
		GUILD_GOLD_TYPE_CHANGE_SMELTER = 8
		GUILD_GOLD_TYPE_UPGRADE_ALTEROFPOWER = 9
		GUILD_GOLD_TYPE_UPGRADE_GUILDBANK = 10
		GUILD_GOLD_TYPE_DEAL_GUILD_LAND = 11

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.buttontooltip = None
			self.ShowButtonToolTip = False
			self.closebutton = None

		self.__LoadWindow()
		self.open = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.buttontooltip = None
			self.ShowButtonToolTip = False

	def Show(self):
		if self.open == True:
			# self.MakeInfoList()
			return
		self.open = True;
		self.SetCenterPosition()
		self.SetTop()
		self.BankScrollBar.SetPos(0.0)
		ui.ScriptWindow.Show(self)
		net.SendGuildBankInfoOpen()

	def __LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/guildwindow_guildbanklog.py")

		self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.closebutton = self.GetChild("ExitButton")
			self.closebutton.SetEvent(ui.__mem_func__(self.Close))
		else:
			self.GetChild("ExitButton").SetEvent(ui.__mem_func__(self.Close))

		scrollBar = self.GetChild("ScrollBar")
		scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollInfoLine))
		self.BankScrollBar = scrollBar
		self.MakeInfoList()

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.buttontooltip = uiToolTip.ToolTip()
			self.buttontooltip.ClearToolTip()
			self.closebutton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), "mouse_over_in", localeInfo.GUILD_BANKINFO_CLOSE)
			self.closebutton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton), "mouse_over_out")

			self.IndexName = self.GetChild("IndexName")
			self.IndexName.SetEvent(ui.__mem_func__(self.OverInToolTipButton), "mouse_over_in", localeInfo.GUILD_BANKINFO_NAME)
			self.IndexName.SetEvent(ui.__mem_func__(self.OverOutToolTipButton), "mouse_over_out")

			self.IndexItemGuildMoney = self.GetChild("IndexItemGuildMoney")
			self.IndexItemGuildMoney.SetEvent(ui.__mem_func__(self.OverInToolTipButton), "mouse_over_in", localeInfo.GUILD_BANKINFO_TYPE)
			self.IndexItemGuildMoney.SetEvent(ui.__mem_func__(self.OverOutToolTipButton), "mouse_over_out")

			self.IndexUseing = self.GetChild("IndexUseing")
			self.IndexUseing.SetEvent(ui.__mem_func__(self.OverInToolTipButton), "mouse_over_in", localeInfo.GUILD_BANKINFO_USETYPE)
			self.IndexUseing.SetEvent(ui.__mem_func__(self.OverOutToolTipButton), "mouse_over_out")

	def OnScrollInfoLine(self):
		scrollBar = self.BankScrollBar
		pos = scrollBar.GetPos()

		count = guild.GetGuildBankInfoSize()
		newLinePos = int(float(count - self.SLOTLINECOUNTER) * pos)

		if newLinePos != self.InfoDataPos:
			self.InfoDataPos = newLinePos
			# import chatm2g
			# chatm2g.AppendChat(7, "pos %d " % self.InfoDataPos)
			self.RefreshGuildBankInfo()
	
	# def OnUpdate(self):
		# self.RefreshGuildBankInfo()

	def MakeInfoList(self):
		self.InfoDict = {}
		self.InfoDataPos = 0

		for i in xrange(self.SLOTLINECOUNTER):
			inverseLineIndex = self.SLOTLINECOUNTER - i - 1
			yPos = 60 + inverseLineIndex * 20

			NameImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 15, yPos)
			NameSlot = ui.MakeTextLine(NameImage)
			self.Children.append(NameSlot)
			self.Children.append(NameImage)

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				UseImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 220+95, yPos)
			else:
				UseImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_01.sub", 220, yPos)
			UseSlot = ui.MakeTextLine(UseImage)
			self.Children.append(UseImage)
			self.Children.append(UseSlot)

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				ItemImage = ui.MakeSlotBar(self, 108, yPos, 207, 17)
			else:
				ItemImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_04.sub", 105, yPos)
			ItemSlot = ui.MakeTextLine(ItemImage)
			self.Children.append(ItemImage)
			self.Children.append(ItemSlot)

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				UseImage.SetPosition(15+12,yPos)
				ItemImage.SetPosition(105+12,yPos)
				NameImage.SetPosition(220+95+12,yPos)

			InfoSlotList = []
			InfoSlotList.append(NameSlot)
			InfoSlotList.append(ItemSlot)
			InfoSlotList.append(UseSlot)
			self.InfoDict[inverseLineIndex] = InfoSlotList

	def RefreshGuildBankInfo(self):
		count = guild.GetGuildBankInfoSize()
		if count > self.SLOTLINECOUNTER:
			self.BankScrollBar.SetMiddleBarSize(float(self.SLOTLINECOUNTER) /float(count))
			self.BankScrollBar.Show()
		else:
			self.BankScrollBar.Hide()
		
		slotCnt = guild.GetGuildBankInfoSize()
		if slotCnt >= self.SLOTLINECOUNTER:
			slotCnt = self.SLOTLINECOUNTER
		
		for i in xrange(slotCnt):
			line, slotList = i, self.InfoDict[i]
			chrname, itemname, iteminout, itemcount, datatype = self.GetInfoData(line)

			slotList[0].SetText(chrname)

			if datatype == 0:
				if itemcount <= 1:
					slotList[1].SetText(itemname)
				else:
					itemText = itemname + " (%d)" % itemcount
					slotList[1].SetText(itemText)
				if iteminout == 0:
					slotList[2].SetText(localeInfo.GUILDBANK_ITEM_IN)
				elif iteminout == 1:
					slotList[2].SetText(localeInfo.GUILDBANK_ITEM_OUT)
				else:
					slotList[2].SetText("")
			else:
				slotList[1].SetText(localeInfo.NumberToMoneyString(itemcount))
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					if iteminout == self.GUILD_GOLD_TYPE_IN:
						slotList[2].SetText(localeInfo.GUILDBANK_GOLD_IN)
					elif iteminout == self.GUILD_GOLD_TYPE_OUT:
						slotList[2].SetText(localeInfo.GUILDBANK_GOLD_OUT)
					elif iteminout == self.GUILD_GOLD_TYPE_PROMOTE:
						slotList[2].SetText(localeInfo.GUILDWINDOW_PROMOTE_REGISTER)
					elif iteminout == self.GUILD_GOLD_TYPE_PROMOTE_UPDATE:
						slotList[2].SetText(localeInfo.GUILDWINDOW_PROMOTE_REFRESH)
					elif iteminout == self.GUILD_GOLD_TYPE_OBJECT_CHANGE:
						slotList[2].SetText(localeInfo.GUILDWINDOW_CHANGE_OBJECT)
					elif iteminout == self.GUILD_GOLD_TYPE_OBJECT_CREATE:
						slotList[2].SetText(localeInfo.GUILDWINDOW_CREATE_OBJECT)
					elif iteminout == self.GUILD_GOLD_TYPE_DRAGON_SP_RECOVER:
						slotList[2].SetText(localeInfo.GUILDWINDOW_SP_RECOVER)
					elif iteminout == self.GUILD_GOLD_TYPE_BUY_GUILD_LAND:
						slotList[2].SetText(localeInfo.GUILDWINDOW_BUY_GUILD_LAND)
					elif iteminout == self.GUILD_GOLD_TYPE_CHANGE_SMELTER:
						slotList[2].SetText(localeInfo.GUILDWINDOW_CHANGE_SMELTER)
					elif iteminout == self.GUILD_GOLD_TYPE_UPGRADE_ALTEROFPOWER:
						slotList[2].SetText(localeInfo.GUILDWINDOW_UPGRADE_ALTEROFPOWER)
					elif iteminout == self.GUILD_GOLD_TYPE_UPGRADE_GUILDBANK:
						slotList[2].SetText(localeInfo.GUILDWINDOW_UPGRADE_GUILDBANK)
					elif iteminout == self.GUILD_GOLD_TYPE_DEAL_GUILD_LAND:
						slotList[2].SetText(localeInfo.GUILDWINDOW_DEAL_GUILD_LAND)
				else:
					if iteminout == 0:
						slotList[2].SetText(localeInfo.GUILDBANK_GOLD_IN)
					else:
						slotList[2].SetText(localeInfo.GUILDBANK_GOLD_OUT)

	def GetInfoData(self, localPos):
		if localPos > guild.GetGuildBankInfoSize():
			localPos = guild.GetGuildBankInfoSize()

		return guild.GetGuildBankInfoData(self.InfoDataPos+localPos)

	def Close(self):
		self.Hide()
		self.open = False

	def SetItemToolTip(self, tooltip):
		self.tooltipitem = tooltip

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def OverInToolTipButton(self, event_type, arg):
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.buttontooltip.ClearToolTip()
			self.buttontooltip.SetThinBoardSize(11 * arglen)
			self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.buttontooltip.AppendTextLine(arg, 0xffffffff)
			self.buttontooltip.Show()
			self.ShowButtonToolTip = True

		def OverOutToolTipButton(self, event_type):
			self.buttontooltip.Hide()
			self.ShowButtonToolTip = False

		def ButtonToolTipProgress(self) :
			if self.ShowButtonToolTip :
				pos_x, pos_y = wndMgr.GetMousePosition()
				self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

		def OnUpdate(self):
			self.ButtonToolTipProgress()

class GuildstorageWindow(ui.ScriptWindow):
	BOX_WIDTH = 176

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.xBankStart = 0
		self.yBankStart = 0
		self.tooltipItem = None

		self.sellingSlotNumber = -1
		self.pageButtonList = []
		self.curPageIndex = 0
		self.interface = None
		self.tryCloseAgain = 0

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

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/GuildstorageWindow.py")

		except:
			import exception
			exception.Abort("uiGuildBankDialog.__LoadWindow.UIScript/GuildstorageWindow.py")

		try:
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

		except:
			import exception
			exception.Abort("uiGuildBankDialog.__LoadWindow. Settings")

	def SetTableSize(self, size):
		pageCount = max(1, size / guildbank.GUILDBANK_SLOT_Y_COUNT)
		pageCount = min(3, pageCount)
		size = guildbank.GUILDBANK_SLOT_Y_COUNT

		self.__MakePageButton(pageCount)

		self.wndItem.ArrangeSlot(0, guildbank.GUILDBANK_SLOT_X_COUNT, size, 32, 32, 0, 0)
		self.wndItem.RefreshSlot()
		self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)

		wnd_height = 130 + 32 * size
		self.wndBoard.SetSize(self.BOX_WIDTH, wnd_height)
		self.SetSize(self.BOX_WIDTH, wnd_height)
		self.UpdateRect()

	def ShowWindow(self, size):
		(self.xBankStart, self.yBankStart, z) = player.GetMainCharacterPosition()

		self.SetTableSize(size)
		self.SetTop()
		self.Show()

	def __MakePageButton(self, pageCount):
		self.curPageIndex = 0
		self.pageButtonList = []

		text = "I"
		pos = -int(float(pageCount-1)/2 * 52)
		for i in xrange(pageCount):
			button = ui.RadioButton()
			button.SetParent(self)
			button.SetUpVisual("d:/ymir work/ui/game/windows/tab_button_middle_01.sub")
			button.SetOverVisual("d:/ymir work/ui/game/windows/tab_button_middle_02.sub")
			button.SetDownVisual("d:/ymir work/ui/game/windows/tab_button_middle_03.sub")
			button.SetWindowHorizontalAlignCenter()
			button.SetWindowVerticalAlignBottom()
			button.SetPosition(pos, 85)
			button.SetText(text)
			button.SetEvent(lambda arg=i: self.SelectPage(arg))
			button.Show()
			self.pageButtonList.append(button)

			pos += 52
			text += "I"

		self.pageButtonList[0].Down()

	def SelectPage(self, index):
		self.curPageIndex = index

		for btn in self.pageButtonList:
			btn.SetUp()

		self.pageButtonList[index].Down()
		self.RefreshBank()

	def __LocalPosToGlobalPos(self, local):
		return self.curPageIndex*guildbank.GUILDBANK_PAGE_SIZE + local

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

			if player.SLOT_TYPE_GUILDBANK == attachedSlotType:
				net.SendGuildBankMove(attachedSlotPos, selectedSlotPos, 2)
				#snd.PlaySound("sound/ui/drop.wav")
			else:
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.RESERVED_WINDOW == attachedInvenType:
					return

				net.SendGuildstorageCheckinPacket(attachedInvenType, attachedSlotPos, selectedSlotPos)
				#snd.PlaySound("sound/ui/drop.wav")

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, selectedSlotPos):
		selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				#net.SendGuildstorageCheckinPacket(attachedSlotPos, selectedSlotPos)
				#snd.PlaySound("sound/ui/drop.wav")

			mouseModule.mouseController.DeattachObject()
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				chatm2g.AppendChat(chatm2g.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)

			elif app.BUY == curCursorNum:
				chatm2g.AppendChat(chatm2g.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			else:
				selectedItemID = guildbank.GetItemID(selectedSlotPos)
				if app.ENABLE_RENDER_COUNTLINE_ATTACHED_MOUSE:
					itemCount = guildbank.GetItemCount(selectedSlotPos)
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_GUILDBANK, selectedSlotPos, selectedItemID, itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_GUILDBANK, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

	def UseItemSlot(self, slotIndex):
		mouseModule.mouseController.DeattachObject()

	def OverInItem(self, slotIndex):
		slotIndex = self.__LocalPosToGlobalPos(slotIndex)
		self.wndItem.SetUsableItem(False)
		self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnUpdate(self):
		USE_GUILDBANK_LIMIT_RANGE = 1000

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xBankStart) > USE_GUILDBANK_LIMIT_RANGE or abs(y - self.yBankStart) > USE_GUILDBANK_LIMIT_RANGE:
			self.Close()

	def RefreshBank(self):
		getItemID=guildbank.GetItemID
		getItemCount=guildbank.GetGuildBankItemCount
		setItemID=self.wndItem.SetItemSlot

		for i in xrange(guildbank.GUILDBANK_PAGE_SIZE):
			slotIndex = self.__LocalPosToGlobalPos(i)
			itemCount = getItemCount(slotIndex)
			if itemCount <= 1:
				itemCount = 0
			setItemID(i, getItemID(slotIndex), itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not guildbank.GetItemChangeLookVnum(slotIndex) == 0:
					self.wndItem.SetSlotCoverImage(i,"icon/item/ingame_convert_Mark.tga")
				else:
					self.wndItem.EnableSlotCoverImage(i,False)

		self.wndItem.RefreshSlot()

	def Close(self):
		if self.tryCloseAgain < app.GetTime():
			self.tryCloseAgain = app.GetTime() + 1
			net.SendChatPacket("/guildstorage_close")
			self.Hide() # @fixme009

	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetGuildBankItem(slotIndex)

	def CommandCloseGuildstorage(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
