if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import mouseModule
import snd
import item
import chat
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder
import localeInfo
import constInfo
import ime
import wndMgr
import uiToolTip

class SelectGems(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.slot_inventario,self.slot_gui,self.slotPos,self.slot_select = [],[],0,0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/selectitemwindow.py")
		except:
			import exception
			exception.Abort("ItemSelectWindow.LoadDialog.LoadObject")

		try:
			GetObject = self.GetChild
			self.board,self.titleBar,self.itemSlot,self.btnExit = GetObject("board"),GetObject("TitleBar"),GetObject("ItemSlot"),GetObject("ExitButton")
		except:
			import exception
			exception.Abort("ItemSelectWindow.LoadDialog.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.btnExit.SetEvent(ui.__mem_func__(self.Close))
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

	def Close(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()
		self.Hide()

	def OnPressEscapeKey(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()
		self.Hide()

	def Open(self):
		self.Clear()
		self.Show()
		self.RefreshSlot(False)

	def Clear(self):
		self.slot_inventario,self.slot_gui,self.slotPos,self.slot_select = [],[],0,0

	def SelectItemSlot(self, slotPos):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()
		SlotInventario,SlotGui,self.slot_select = self.slot_inventario[slotPos],slotPos,slotPos
		net.SendChatPacket("/gem craft %d" %SlotInventario)

	def SetTableSize(self, size):
		SLOT_X_COUNT = 5
		self.itemSlot.ArrangeSlot(0, SLOT_X_COUNT, size, 32, 32, 0, 0)
		self.itemSlot.RefreshSlot()
		self.itemSlot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		self.board.SetSize(self.board.GetWidth(), 76 + 32*size)
		self.SetSize(self.board.GetWidth(), 76 + 32*size)
		self.UpdateRect()

	def RefreshSlot(self,refresh = False):
		getItemVNum,getItemCount,setItemVNum=player.GetItemIndex,player.GetItemCount,self.itemSlot.SetItemSlot

		if not refresh:
			for i in xrange(player.INVENTORY_SLOT_COUNT):
				slotNumber = i
				itemVNum = getItemVNum(slotNumber)

				if 0 == itemVNum or not item.IsMetin(itemVNum):
					continue

				itemGrade = self.CheckGradeStone(item.GetItemName())
				if itemGrade > "3":
					continue

				#if itemVNum != 50926:
				#	continue

				self.slot_inventario.append(slotNumber)
				self.slot_gui.append(self.slotPos)

				self.slotPos += 1
				if self.slotPos > 54:
					break

		itemCount = len(self.slot_inventario)
		if itemCount < 15:
			self.SetTableSize(3)

		else:
			lineCount = 3
			lineCount += (itemCount - 15) / 5
			if itemCount % 5:
				lineCount += 1
			self.SetTableSize(lineCount)

		count = 0
		for inventoryPos in self.slot_inventario:
			itemVNum = getItemVNum(inventoryPos)
			itemCount = getItemCount(inventoryPos)

			if itemCount <= 1:
				itemCount = 0

			setItemVNum(count, itemVNum, itemCount)
			count += 1
	
		self.itemSlot.RefreshSlot()
		if refresh:
			self.tooltipItem.Hide()

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, slotIndex):
		if None != self.tooltipItem:
			inventorySlotPos = self.slot_inventario[slotIndex]
			self.tooltipItem.SetInventoryItem(inventorySlotPos)

	def CheckGradeStone(self,name):
		Search = name.rfind('+')
		Text = name[Search+1]
		return Text

	def SucceedGem(self):
		# del self.slot_inventario[self.slot_select]
		# del self.slot_gui[self.slot_select]
		self.RefreshSlot(True)

	def OnPressEscapeKey(self):
		self.Close()
		return True

class SelectGemsShop(ui.ScriptWindow):
	GEM_SHOP_SLOT_MAX = 9
	GEM_SHOP_REFRESH_ITEM_VNUM = 39063
	GEM_SHOP_WINDOW_LIMIT_RANGE = 500
	GEM_SHOP_ADD_ITEM_VNUM = 39064

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipitem = None
		self.wndSellItemSlot = None
		self.questionDialog = None
		self.RefreshTimeValue = 0
		self.items = []
		self.slots_block = []
		self.xGemWindowStart = 0
		self.yGemWindowStart = 0
		self.gemshoprefreshtime = 0
		self.IsOpen = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.tooltipitem = None
		self.wndSellItemSlot = None
		self.questionDialog = None
		self.RefreshTimeValue = 0
		self.items = []
		self.slots_block = []
		self.xGemWindowStart = 0
		self.yGemWindowStart = 0
		self.gemshoprefreshtime = 0
		self.IsOpen = False

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/GemShopWindow.py")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			## RefreshButton
			self.GetChild("RefreshButton").SetEvent(ui.__mem_func__(self.RefreshItemSlot))

			## RefreshTimeValue
			self.RefreshTimeValue = self.GetChild("BuyRefreshTime")

			## Slot Background
			self.bg = self.GetChild("gemshopbackimg")

		except:
			import exception
			exception.Abort("SelectGems.LoadWindow.UIScript/GemShopWindow.py")

		## SlostSetting
		self.wndSellItemSlot = ui.GridSlotWindow()
		self.wndSellItemSlot.SetParent(self.bg)
		self.wndSellItemSlot.SetPosition(8, 28)
		self.wndSellItemSlot.ArrangeSlot(0, 3, 3, 32, 32, 13, 26)
		self.wndSellItemSlot.SAFE_SetButtonEvent("LEFT", "EXIST", self.UseItemSlot)
		self.wndSellItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.wndSellItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndSellItemSlot.Show()

		positions = [
			[8,28],[8+45,28],[8+(45*2),28],
			[8,28+58],[8+45,28+58],[8+(45*2),28+58],
			[8,28+(58*2)],[8+45,28+(58*2)],[8+(45*2),28+(58*2)]
		]

		self.name,self.icongem = {},{}
		count = 0

		for i in positions:
			self.icongem[count] = ui.ImageBox()
			self.icongem[count].SetParent(self.bg)
			self.icongem[count].SetPosition(positions[count][0]-2,positions[count][1]+40)
			self.icongem[count].LoadImage("d:/ymir work/ui/gemshop/gemshop_gemicon.sub")
			self.icongem[count].Show()

			self.name[count] = ui.TextLine()
			self.name[count].SetParent(self.bg)
			self.name[count].SetPosition(positions[count][0]+14,positions[count][1]+39)
			self.name[count].Show()
			count +=1

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)
		(self.xGemWindowStart, self.yGemWindowStart, z) = player.GetMainCharacterPosition()
		self.IsOpen = True

	def Close(self):
		self.Hide()
		self.IsOpen = False
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()

	def Clear(self):
		self.items = []
		self.slots_block = []

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Destroy(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()

	def Time(self, time):
		self.gemshoprefreshtime = int(time)

	def __FormatTime(self, time):
		m, s = divmod(time, 60)
		h, m = divmod(m, 60)
		return "%1d:%02d:%02d" % (h, m, s)

	def Information(self, vnums, precios, count):
		self.items.append([int(vnums), int(precios), int(count)])

	def SlotsDesblock(self, slot0, slot1, slot2, slot3, slot4, slot5):
		self.slots_block.append([slot0, slot1, slot2, slot3, slot4, slot5])
		count = 0
		for i in xrange(0,6):
			if(self.slots_block[0][i] == "0"):
				self.wndSellItemSlot.DisableSlot(count+3)
			count +=1

	def LoadInformation(self):
		for i in xrange(0,len(self.items)):
			self.name[i].SetText(str(self.items[i][1]))
			self.wndSellItemSlot.SetItemSlot(i, self.items[i][0], self.items[i][2])

	def SetItemToolTip(self, tooltip):
		self.tooltipitem = tooltip

	def __ShowToolTip(self, slotIndex):
		if self.tooltipitem:
			self.tooltipitem.ClearToolTip()
			metinSlot = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				metinSlot.append(0)
			attrSlot = []
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				attrSlot.append((0, 0))

			self.tooltipitem.AddItemData(self.items[slotIndex][0], metinSlot, attrSlot)

	# ¾ÆÀÌÅÛ ÅøÆÞ º¸¿©ÞÖ±â
	def OverInItem(self, slotIndex):
		self.__ShowToolTip(slotIndex)

	## ¾ÆÀÌÅÛ ÅøÆÞ °¨Ãß±â
	def OverOutItem(self):
		if self.tooltipitem:
			self.tooltipitem.HideToolTip()

	## ½½·Ô È®Àå.
	def SlotAddQuestion(self, slotIndex):
		item.SelectItem(self.GEM_SHOP_ADD_ITEM_VNUM)
		gemshopadditemvnum = item.GetItemName()
		openslotitemcount = 1

		self.questionDialog = uiCommon.QuestionDialog2()
		self.questionDialog.SetText1(localeInfo.GEM_SYSTEM_ADD_SLOT % (gemshopadditemvnum))
		self.questionDialog.SetText2(localeInfo.GEM_SYSTEM_ADD_SLOT2 % (openslotitemcount))
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SlotAddQuestionAccept))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.SlotAddQuestionCancle))
		self.questionDialog.SetWidth(350)
		self.questionDialog.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def SlotAddQuestionAccept(self):
		if None == self.questionDialog:
			return

		self.OnCloseQuestionDialog()

	def SlotAddQuestionCancle(self):
		if None == self.questionDialog:
			return

		self.OnCloseQuestionDialog()

	## ¾ÆÀÌÅÛ »ç¿ë ½Ã ±¸¸Å ÞøÇà
	def UseItemSlot(self, slotIndex):
		if self.questionDialog:
			return

		## º¸¼® °¹¼ö È®ÀÞ
		if player.GetGem() < self.items[slotIndex][1]:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_SYSTEM_NOT_ENOUGH_HP_GEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()

		if (slotIndex >= 3):
			if (self.slots_block[0][slotIndex-3] == "0"):
				item.SelectItem(self.GEM_SHOP_ADD_ITEM_VNUM)
				gemshopadditemvnum = item.GetItemName()
				openslotitemcount = 1

				self.questionDialog = uiCommon.QuestionDialog2()
				self.questionDialog.SetText1(localeInfo.GEM_SYSTEM_ADD_SLOT % (gemshopadditemvnum))
				self.questionDialog.SetText2(localeInfo.GEM_SYSTEM_ADD_SLOT2 % (openslotitemcount))
			else:
				self.questionDialog.SetText(localeInfo.GEM_SYSTEM_BUY_ITEM)
		else:
			self.questionDialog.SetText(localeInfo.GEM_SYSTEM_BUY_ITEM)

		self.questionDialog.SetAcceptEvent(lambda arg=slotIndex: self.SendBuyAccept(arg))
		self.questionDialog.SetCancelEvent(self.OnCloseQuestionDialog)
		self.questionDialog.SetWidth(350)
		self.questionDialog.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def SendBuyAccept(self, slotindex):
		if None == self.questionDialog:
			return

		net.SendChatPacket("/gem market %d" % slotindex)

		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## °»½Å ¹öÆ°
	def RefreshItemSlot(self):
		ItemRefreshItemCount = player.GetItemCountByVnum(self.GEM_SHOP_REFRESH_ITEM_VNUM)

		if ItemRefreshItemCount == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_SYSTEM_NOT_ENOUGHT_REFRESHITEM)
			return;

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.GEM_SYSTEM_REFRESH_SHOP_ITEMS)
		self.questionDialog.SetAcceptEvent(self.SendRequestRefreshAccept)
		self.questionDialog.SetCancelEvent(self.OnCloseQuestionDialog)
		self.questionDialog.Open()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def SendRequestRefreshAccept(self):
		if not self.questionDialog:
			return

		net.SendChatPacket("/gem refresh")

		self.OnCloseQuestionDialog()

	def OnUpdate(self):
		if self.IsOpen:
			(x, y, z) = player.GetMainCharacterPosition()
			if abs(x - self.xGemWindowStart) > self.GEM_SHOP_WINDOW_LIMIT_RANGE or abs(y - self.yGemWindowStart) > self.GEM_SHOP_WINDOW_LIMIT_RANGE:
				self.Close()

			## °»½Å ½Ã°£
			leftSec = max(0, self.gemshoprefreshtime - app.GetGlobalTimeStamp())
			if leftSec > 0:
				self.RefreshTimeValue.SetText(str(self.__FormatTime(leftSec)))
			else:
				# self.RefreshTimeValue.SetText(str(localeInfo.SecondToHMGolbal(leftSec)))
				self.RefreshTimeValue.SetText(str("00:00"))