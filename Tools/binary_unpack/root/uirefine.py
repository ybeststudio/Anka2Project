if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))
chr = __import__(pyapi.GetModuleName("chr"))

import item
import ui
import uiToolTip
import mouseModule
import localeInfo
import uiCommon
import constInfo
import uiScriptLocale
import chat
import dbg

if app.ENABLE_SLOT_MARKING_SYSTEM:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

if app.ENABLE_PITTY_REFINE:
	ITEM_SEAL_VNUM = 94910
	IS_AUTO_REFINE_SEAL = 0

if constInfo.SHOW_REFINE_ITEM_DESC == TRUE:
	TOOLTIP_DATA = {
		"materials" : [],
		"slot_count" : 0
	}

def kformat(n):
	if n >= 1000000000:
		value = n / 1000000000.0
		suffix = 'B'
	elif n >= 1000000:
		value = n / 1000000.0
		suffix = 'M'
	elif n >= 1000:
		value = n / 1000.0
		suffix = 'k'
	else:
		return str(n)
	
	formatted_value = '{:.2f}'.format(value).rstrip('0').rstrip('.')
	return '{}{}'.format(formatted_value, suffix)

class RefineDialog(ui.ScriptWindow):

	makeSocketSuccessPercentage = ( 100, 33, 20, 15, 10, 5, 0 )
	upgradeStoneSuccessPercentage = ( 30, 29, 28, 27, 26, 25, 24, 23, 22 )
	upgradeArmorSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )
	upgradeAccessorySuccessPercentage = ( 99, 88, 77, 66, 33, 33, 33, 33, 33 )
	upgradeSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadScript()

		self.scrollItemPos = 0
		self.targetItemPos = 0

	def __LoadScript(self):

		self.__LoadQuestionDialog()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.successPercentage = self.GetChild("SuccessPercentage")
			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.Close)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		self.successPercentage.Show()

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetPosition(15, 38)
		toolTip.SetFollow(FALSE)
		toolTip.Show()
		self.toolTip = toolTip

		if constInfo.SHOW_REFINE_ITEM_DESC == TRUE:
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadQuestionDialog(self):
		self.dlgQuestion = ui.ScriptWindow()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgQuestion, "uiscript/questiondialog2.py")
		except:
			import exception
			exception.Abort("RefineDialog.__LoadQuestionDialog.LoadScript")

		try:
			GetObject=self.dlgQuestion.GetChild
			GetObject("message1").SetText(localeInfo.REFINE_DESTROY_WARNING)
			GetObject("message2").SetText(localeInfo.REFINE_WARNING2)
			GetObject("accept").SetEvent(ui.__mem_func__(self.Accept))
			GetObject("cancel").SetEvent(ui.__mem_func__(self.dlgQuestion.Hide))
		except:
			import exception
			exception.Abort("SelectCharacterWindow.__LoadQuestionDialog.BindObject")

	def Destroy(self):
		self.ClearDictionary()
		self.board = 0
		self.successPercentage = 0
		self.titleBar = 0
		self.toolTip = 0
		self.dlgQuestion = 0

	def GetRefineSuccessPercentage(self, scrollSlotIndex, itemSlotIndex):
		if -1 != scrollSlotIndex:
			if player.IsRefineGradeScroll(scrollSlotIndex):
				curGrade = player.GetItemGrade(itemSlotIndex)
				itemIndex = player.GetItemIndex(itemSlotIndex)

				item.SelectItem(itemIndex)
				itemType = item.GetItemType()
				itemSubType = item.GetItemSubType()

				if item.ITEM_TYPE_METIN == itemType:
					if curGrade >= len(self.upgradeStoneSuccessPercentage):
						return 0
					return self.upgradeStoneSuccessPercentage[curGrade]
				elif item.ITEM_TYPE_ARMOR == itemType:
					if item.ARMOR_BODY == itemSubType:
						if curGrade >= len(self.upgradeArmorSuccessPercentage):
							return 0
						return self.upgradeArmorSuccessPercentage[curGrade]
					else:
						if curGrade >= len(self.upgradeAccessorySuccessPercentage):
							return 0
						return self.upgradeAccessorySuccessPercentage[curGrade]
				else:
					if curGrade >= len(self.upgradeSuccessPercentage):
						return 0
					return self.upgradeSuccessPercentage[curGrade]

		for i in xrange(player.METIN_SOCKET_MAX_NUM+1):
			if 0 == player.GetItemMetinSocket(itemSlotIndex, i):
				break

		return self.makeSocketSuccessPercentage[i]

	def Open(self, scrollItemPos, targetItemPos):
		self.scrollItemPos = scrollItemPos
		self.targetItemPos = targetItemPos

		percentage = self.GetRefineSuccessPercentage(scrollItemPos, targetItemPos)
		if 0 == percentage:
			return
		self.successPercentage.SetText(localeInfo.REFINE_SUCCESS_PROBALITY)

		itemIndex = player.GetItemIndex(targetItemPos)
		self.toolTip.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))
		self.toolTip.AddItemData(itemIndex, metinSlot)

		self.UpdateDialog()
		self.SetTop()
		self.Show()

	def UpdateDialog(self):
		newWidth = self.toolTip.GetWidth() + 30
		newHeight = self.toolTip.GetHeight() + 98
		self.board.SetSize(newWidth, newHeight)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):
		percentage = self.GetRefineSuccessPercentage(-1, self.targetItemPos)
		if 100 == percentage:
			self.Accept()
			return

		self.dlgQuestion.SetTop()
		self.dlgQuestion.Show()

	def Accept(self):
		net.SendItemUseToItemPacket(self.scrollItemPos, self.targetItemPos)
		self.Close()

	def Close(self):
		self.dlgQuestion.Hide()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class RefineDialogNew(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.isLoaded = FALSE

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None

	def __Initialize(self):
		self.dlgQuestion = None
		self.interface = None
		self.children = []
		self.vnum = 0
		self.targetItemPos = 0
		self.dialogHeight = 0
		self.cost = 0
		self.percentage = 0
		self.type = 0

	def __LoadScript(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")

			self.costText = self.GetChild("Cost")

			if app.ENABLE_PITTY_REFINE:
				self.GetChild("SuccessPercentage").Hide()
			else:
				self.probText = self.GetChild("SuccessPercentage")

			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.CancelRefine)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		# No embedded result tooltip; only left/right icons + materials. Full stats on hover.
		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetFollow(False)
		toolTip.SetPosition(15, 38)
		toolTip.HideToolTip()
		self.toolTip = toolTip

		self.toolTipNext = uiToolTip.ItemToolTip()
		self.toolTipNext.HideToolTip()

		self.toolTipCur = uiToolTip.ItemToolTip()
		self.toolTipCur.HideToolTip()

		self.slotList = []
		for i in xrange(3):
			slot = self.__MakeSlot()
			slot.SetParent(toolTip)
			slot.SetWindowVerticalAlignCenter()
			self.slotList.append(slot)

		itemImage = self.__MakeItemImage()
		itemImage.SetParent(toolTip)
		itemImage.SetWindowVerticalAlignCenter()
		itemImage.SetPosition(-35, 0)
		self.itemImage = itemImage

		self.slotCurrent = ui.MakeImageBox(self, "d:/ymir work/ui/switchbot/switch_slot.tga", 25, 60)
		self.slotCurrent.Show()
		self.slotAfter = ui.MakeImageBox(self, "d:/ymir work/ui/switchbot/switch_slot.tga", 200, 60)
		self.slotAfter.Show()

		self.itemImageCur = ui.MakeImageBox(self, "d:/ymir work/ui/public/Slot_Base.sub", 30, 65)
		self.itemImageCur.Show()
		self.itemImageNext = ui.MakeImageBox(self, "d:/ymir work/ui/public/Slot_Base.sub", 205, 65)
		self.itemImageNext.Show()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.CancelRefine))

		if constInfo.SHOW_REFINE_ITEM_DESC == TRUE:
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

		if app.ENABLE_AUTO_REFINE:
			self.checkBox = ui.CheckBox()
			self.checkBox.SetParent(self)
			self.checkBox.SetPosition(-3, 83+8)
			self.checkBox.SetWindowHorizontalAlignCenter()
			self.checkBox.SetWindowVerticalAlignBottom()
			self.checkBox.SetEvent(ui.__mem_func__(self.AutoRefine), "ON_CHECK", TRUE)
			self.checkBox.SetEvent(ui.__mem_func__(self.AutoRefine), "ON_UNCKECK", False)
			self.checkBox.SetCheckStatus(constInfo.IS_AUTO_REFINE)
			self.checkBox.SetTextInfo(uiScriptLocale.UPGRADE)

			if not app.ENABLE_PITTY_REFINE:
				self.checkBox.Show()

		if app.ENABLE_PITTY_REFINE:
			self.MakePittyInfoWindow()

		self.isLoaded = TRUE

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None

	def __MakeSlot(self):
		slot = ui.ImageBox()
		slot.LoadImage("d:/ymir work/ui/public/slot_base.sub")
		slot.Show()
		self.children.append(slot)
		return slot

	def __MakeItemImage(self):
		itemImage = ui.ImageBox()
		itemImage.Show()
		self.children.append(itemImage)
		return itemImage

	def __MakeThinBoard(self):
		thinBoard = ui.ThinBoard()
		thinBoard.SetParent(self)
		thinBoard.Show()
		self.children.append(thinBoard)
		return thinBoard

	def __RefineNumericCost(self):
		try:
			return long(self.cost)
		except (TypeError, ValueError):
			return 0

	def __RefineNumericPercent(self):
		try:
			return int(self.percentage)
		except (TypeError, ValueError):
			return 0

	def __SetRefineTextLineSafe(self, control, text):
		# Bazi clientlarda SetText icerigi printf gibi isler; '%' -> '%%' (once gercek %% korunur).
		if not control:
			return
		if text is None:
			text = ""
		t = text
		ph = "\x7f\x7fREFINEPCT\x7f\x7f"
		t = t.replace("%%", ph)
		t = t.replace("%", "%%")
		t = t.replace(ph, "%%")
		control.SetText(t)

	def Destroy(self):
		self.dlgQuestion = None
		self.board = 0
		self.probText = 0
		self.costText = 0
		self.titleBar = 0
		self.toolTip = 0
		if hasattr(self, 'toolTipNext'):
			self.toolTipNext = 0
		if hasattr(self, 'toolTipCur'):
			self.toolTipCur = 0
		if hasattr(self, 'itemImageCur'):
			self.itemImageCur = 0
		if hasattr(self, 'itemImageNext'):
			self.itemImageNext = 0
		if hasattr(self, 'slotCurrent'):
			self.slotCurrent = None
		if hasattr(self, 'slotAfter'):
			self.slotAfter = None
		self.successPercentage = None
		self.interface = None
		self.slotList = []
		self.children = []

		self.ClearDictionary()

	if app.ENABLE_AUTO_REFINE:
		def __InitializeOpen(self):
			self.children = []
			self.vnum = 0
			self.targetItemPos = 0
			self.dialogHeight = 0
			self.cost = 0
			self.percentage = 0
			self.type = 0
			self.xRefineStart = 0
			self.yRefineStart = 0

	def Open(self, targetItemPos, nextGradeItemVnum, cost, prob, type):

		if False == self.isLoaded:
			self.__LoadScript()

		if app.ENABLE_AUTO_REFINE:
			self.__InitializeOpen()
		else:
			self.__Initialize()

		self.targetItemPos = targetItemPos
		self.vnum = nextGradeItemVnum
		self.cost = cost
		self.percentage = prob
		self.type = type

		_costVal = kformat(self.__RefineNumericCost())
		_pctVal = self.__RefineNumericPercent()

		if app.ENABLE_PITTY_REFINE:
			if player.GetElk() >= self.__RefineNumericCost():
				self.costText.SetPackedFontColor(0xFF35dDE3D)
			else:
				self.costText.SetPackedFontColor(0xFFFF0033)

			self.costText.SetText(_costVal)

			_spit = localeInfo.REFINE_SUCCESS_PROBALITY
			if "%d" in _spit:
				_spit = _spit.replace("%d", str(_pctVal), 1)
			self.__SetRefineTextLineSafe(self.wndTextPitty, _spit)
		else:
			_fmt = localeInfo.REFINE_COST
			if "%d" in _fmt:
				cost_s = _fmt.replace("%d", _costVal, 1)
			else:
				parts = _fmt.split("0", 1)
				if len(parts) == 2:
					cost_s = parts[0] + _costVal + parts[1]
				else:
					cost_s = _fmt + " " + _costVal
			self.__SetRefineTextLineSafe(self.costText, cost_s)

			_sp = localeInfo.REFINE_SUCCESS_PROBALITY
			if "%d" in _sp:
				_sp = _sp.replace("%d", str(_pctVal), 1)
			self.__SetRefineTextLineSafe(self.probText, _sp)

		self.toolTip.ClearToolTip()
		if hasattr(self, 'toolTipNext'):
			self.toolTipNext.ClearToolTip()
		if hasattr(self, 'toolTipCur'):
			self.toolTipCur.ClearToolTip()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))

		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(targetItemPos, i))
		# AddItemData always ends with ShowToolTip(); hide here so only top icons + materials show.
		if hasattr(self, 'toolTipCur'):
			self.toolTipCur.SetInventoryItem(targetItemPos)
			self.toolTipCur.HideToolTip()
		if hasattr(self, 'toolTipNext'):
			self.toolTipNext.AddRefineItemData(nextGradeItemVnum, metinSlot, attrSlot)
			self.toolTipNext.HideToolTip()

		curItemIndex = player.GetItemIndex(targetItemPos)
		if curItemIndex != 0:
			item.SelectItem(curItemIndex)
			if hasattr(self, 'itemImageCur'):
				try:
					self.itemImageCur.LoadImage(item.GetIconImageFileName())
				except:
					import dbg
					dbg.TraceError("Refine.CurrentItem.LoadImage - Failed to find item data")

		item.SelectItem(nextGradeItemVnum)
		if hasattr(self, 'itemImageNext'):
			self.itemImageNext.LoadImage(item.GetIconImageFileName())

		for slot in self.slotList:
			slot.Hide()
		self.itemImage.Hide()
		if hasattr(self, 'toolTip') and self.toolTip:
			self.toolTip.HideToolTip()

		if app.ENABLE_AUTO_REFINE:
			if constInfo.AUTO_REFINE_TYPE == 2 and chr.GetVirtualNumber(constInfo.AUTO_REFINE_DATA["NPC"][0]) == 20091:
				constInfo.IS_AUTO_REFINE = False
				self.checkBox.Hide()
			else:
				self.checkBox.Show()

		if app.ENABLE_PITTY_REFINE:
			self.boardPittyThin.Hide()
			self.boardPitty.SetSize(self.boardPitty.GetWidth(), 91)
			self.boardPittyBG.SetPosition(self.boardPittyBG.GetLocalPosition()[0], 0)

		self.dialogHeight = 170
		self.UpdateDialog()

		self.SetTop()
		self.Show()

	if app.ENABLE_PITTY_REFINE:
		def MakePittyInfoWindow(self):
			self.boardPitty = ui.Window()
			self.boardPitty.SetParent(self)
			self.boardPitty.SetSize(261, 91 + 44 + 8)
			self.boardPitty.Show()

			self.boardPittyThin = ui.ThinBoard()
			self.boardPittyThin.SetParent(self.boardPitty)
			self.boardPittyThin.SetSize(self.boardPitty.GetWidth() - 6, 44)
			self.boardPittyThin.SetPosition(3, 0)
			self.boardPittyThin.Show()

			self.boardPittyTitle = ui.TextLine()
			self.boardPittyTitle.SetParent(self.boardPittyThin)
			self.boardPittyTitle.SetWindowHorizontalAlignCenter()
			self.boardPittyTitle.SetHorizontalAlignCenter()
			self.boardPittyTitle.SetPosition(10, 8 + 1)
			self.boardPittyTitle.SetText("")
			self.boardPittyTitle.Show()

			self.btnMark = ui.MakeButton(self.boardPittyThin, 0, 0, "", "d:/ymir work/ui/pattern/", "q_mark_01.tga", "q_mark_02.tga", "q_mark_01.tga")
			self.btnMark.SetWindowHorizontalAlignCenter()
			self.btnMark.SetPosition(-40, 8)
			self.btnMark.SetEventOverIn(ui.__mem_func__(self.OverInMarkPitty))
			self.btnMark.SetEventOverOut(ui.__mem_func__(self.OverOutItemPitty))

			self.boardPittyGauge = ui.Gauge()
			self.boardPittyGauge.SetParent(self.boardPittyThin)
			self.boardPittyGauge.SLOT_HEIGHT = 12
			self.boardPittyGauge.MakeGauge(self.boardPittyThin.GetWidth() - 30, "blue")
			self.boardPittyGauge.SetPosition(15, 8 + 20)
			self.boardPittyGauge.Show()

			self.boardPittyBG = ui.ImageBox()
			self.boardPittyBG.SetParent(self.boardPitty)
			self.boardPittyBG.LoadImage("d:/ymir work/ui/refine_ui_new.png")
			self.boardPittyBG.SetPosition(0, 40 + 8)
			self.boardPittyBG.Show()

			self.imageSeal = ui.ImageBox()
			self.imageSeal.SetParent(self.boardPittyBG)
			self.imageSeal.SetPosition(6, 9)
			self.imageSeal.Show()

			if player.GetItemCountByVnum(ITEM_SEAL_VNUM) > 0:
				item.SelectItem(ITEM_SEAL_VNUM)
				self.imageSeal.LoadImage(item.GetIconImageFileName())
			else:
				self.imageSeal.Hide()

			self.wndTextPittyTitle = ui.MakeTextLine(self.boardPittyBG)
			self.wndTextPittyTitle.SetText(localeInfo.REFINE_INCRESE)
			self.wndTextPittyTitle.SetPosition(3, -36)

			self.checkBoxUseSeal = ui.CheckBox()
			self.checkBoxUseSeal.SetParent(self.boardPittyBG)
			self.checkBoxUseSeal.SetPosition(45, 20)
			self.checkBoxUseSeal.SetCheckStatus(0)
			self.checkBoxUseSeal.SetEvent(ui.__mem_func__(self.CheckBoxSeal), "ON_CHECK", TRUE)
			self.checkBoxUseSeal.SetEvent(ui.__mem_func__(self.CheckBoxSeal), "ON_UNCKECK", FALSE)
			self.checkBoxUseSeal.SetTextInfo(localeInfo.REFINE_INCRESE2)
			self.checkBoxUseSeal.Show()

			self.checkBoxUseSealAlways = ui.CheckBox()
			self.checkBoxUseSealAlways.SetParent(self.boardPittyBG)
			self.checkBoxUseSealAlways.SetPosition(45, 35)

			global IS_AUTO_REFINE_SEAL
			self.checkBoxUseSealAlways.SetCheckStatus(IS_AUTO_REFINE_SEAL)

			self.checkBoxUseSealAlways.SetEvent(ui.__mem_func__(self.CheckBoxSealAlways), "ON_CHECK", TRUE)
			self.checkBoxUseSealAlways.SetEvent(ui.__mem_func__(self.CheckBoxSealAlways), "ON_UNCKECK", FALSE)
			self.checkBoxUseSealAlways.SetTextInfo(localeInfo.REFINE_INCRESE3)
			self.checkBoxUseSealAlways.Show()

			self.costText.SetParent(self.boardPittyBG)
			self.costText.SetPosition(95, 19)

			if app.ENABLE_AUTO_REFINE:
				self.checkBox.SetParent(self.boardPittyBG)
				self.checkBox.SetPosition(0, 38)

			self.wndTextPitty = ui.MakeTextLine(self.boardPittyBG)
			self.__SetRefineTextLineSafe(self.wndTextPitty, "100%")
			self.wndTextPitty.SetPosition(-42, 33)
			self.wndTextPitty.SetPackedFontColor(0xffF2E7C1)

		def CheckBoxSeal(self, checkType, autoFlag):
			self.checkBoxUseSealAlways.SetCheckStatus(0)

			global IS_AUTO_REFINE_SEAL
			IS_AUTO_REFINE_SEAL = FALSE

			if autoFlag and player.GetItemCountByVnum(ITEM_SEAL_VNUM) == 0:
				self.checkBoxUseSeal.SetCheckStatus(0)
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_NO_SOULD_OF_GOD_ALERT)

		def CheckBoxSealAlways(self, checkType, autoFlag):
			self.checkBoxUseSeal.SetCheckStatus(0)

			global IS_AUTO_REFINE_SEAL
			IS_AUTO_REFINE_SEAL = autoFlag

			if autoFlag and player.GetItemCountByVnum(ITEM_SEAL_VNUM) == 0:
				self.checkBoxUseSealAlways.SetCheckStatus(0)
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_NO_SOULD_OF_GOD_ALERT)

		def SetPittyInfo(self, forVnum, pittyCurrent, pittyMax):
			if player.GetItemIndex(self.targetItemPos) != forVnum:
				self.boardPitty.Hide()
				self.UpdateDialog()
				return

			if pittyMax == 0:
				self.boardPittyThin.Hide()
				self.boardPitty.SetSize(self.boardPitty.GetWidth(), 91)
				self.boardPittyBG.SetPosition(self.boardPittyBG.GetLocalPosition()[0], 0)
			else:
				try:
					_pt = localeInfo.PITTY_SYSTEM_NAME % (pittyCurrent, pittyMax)
				except TypeError:
					_pt = localeInfo.PITTY_SYSTEM_NAME
				self.__SetRefineTextLineSafe(self.boardPittyTitle, _pt)
				self.boardPittyGauge.SetPercentage(pittyCurrent, pittyMax)

				self.boardPitty.SetSize(261, 91 + 44 + 8)
				self.boardPittyBG.SetPosition(self.boardPittyBG.GetLocalPosition()[0], 40 + 8)
				self.boardPittyThin.Show()

			if pittyCurrent == pittyMax:
				self.__SetRefineTextLineSafe(self.wndTextPitty, "100%")
				self.percentage = 100
			else:
				_spp = localeInfo.REFINE_SUCCESS_PROBALITY
				if "%d" in _spp:
					_spp = _spp.replace("%d", str(self.__RefineNumericPercent()), 1)
				self.__SetRefineTextLineSafe(self.wndTextPitty, _spp)

			self.boardPitty.Show()
			self.UpdateDialog()

		def OverInMarkPitty(self):
			interface = constInfo.GetInterfaceInstance()

			if interface.tooltipItem:
				interface.tooltipItem.ClearToolTip()
				interface.tooltipItem.SetCannotUseItemForceSetDisableColor(FALSE)

				interface.tooltipItem.SetTitle("Pity System")
				interface.tooltipItem.AppendSpace(5)
				interface.tooltipItem.AppendDescription(localeInfo.REFINE_INFO_PITTY_DESC1, 26)
				interface.tooltipItem.AppendHorizontalLine()
				interface.tooltipItem.AppendDescription(localeInfo.REFINE_INFO_PITTY_DESC2, 26)
				interface.tooltipItem.ShowToolTip()

		def OverOutItemPitty(self):
			interface = constInfo.GetInterfaceInstance()
			if not interface:
				return

			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()

	def Close(self):
		if self.dlgQuestion:
			self.dlgQuestion.Close()
		self.dlgQuestion = None

		if hasattr(self, 'toolTipCur') and self.toolTipCur:
			self.toolTipCur.HideToolTip()
		if hasattr(self, 'toolTipNext') and self.toolTipNext:
			self.toolTipNext.HideToolTip()
		if hasattr(self, 'toolTip') and self.toolTip:
			self.toolTip.HideToolTip()
		if hasattr(self, 'tooltipItem') and self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.Hide()

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if hasattr(self, 'inven') and self.inven:
				if hasattr(self, 'targetItemPos') and self.targetItemPos > 0:
					self.SetCanMouseEventSlot(self.targetItemPos)

		if constInfo.SHOW_REFINE_ITEM_DESC:
			TOOLTIP_DATA["materials"] = []

	if constInfo.SHOW_REFINE_ITEM_DESC == TRUE:
		def __MakeItemSlot(self, slotIndex):
			slot = ui.SlotWindow()
			slot.SetParent(self)
			slot.SetSize(32, 32)
			slot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			slot.AppendSlot(slotIndex, 0, 0, 32, 32)
			slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			slot.RefreshSlot()
			slot.Show()
			self.children.append(slot)
			return slot

		def BindInterface(self, interface):
			self.interface = interface
		def OverInItem(self, slotIndex):
			if slotIndex > len(TOOLTIP_DATA['materials']):
				return

			if self.tooltipItem:
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AddItemData(TOOLTIP_DATA['materials'][slotIndex], 0, 0, 0, 0, player.INVENTORY)
				self.tooltipItem.AppendSpace(7)
				self.tooltipItem.ShowToolTip()

		def OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def AppendMaterial(self, vnum, count):
			if constInfo.SHOW_REFINE_ITEM_DESC == TRUE:
				slotIndex = len(TOOLTIP_DATA['materials'])

				slot = self.__MakeItemSlot(slotIndex)
				slot.SetPosition(15, self.dialogHeight)
				slot.SetItemSlot(slotIndex, vnum, 0)#count)

				TOOLTIP_DATA['materials'].append(vnum)
			else:
				slot = self.__MakeSlot()
				slot.SetParent(self)
				slot.SetPosition(15, self.dialogHeight)

				itemImage = self.__MakeItemImage()
				itemImage.SetParent(slot)
				item.SelectItem(vnum)
				itemImage.LoadImage(item.GetIconImageFileName())

			thinBoard = self.__MakeThinBoard()
			thinBoard.SetPosition(50, self.dialogHeight)
			thinBoard.SetSize(191, 20)

			textLine = ui.TextLine()
			textLine.SetParent(thinBoard)
			textLine.SetFontName(localeInfo.UI_DEF_FONT)

			itemCount = player.GetItemCountByVnum(vnum)

			if itemCount >= count:
				textLine.SetPackedFontColor(0xFF35dDE3D)
			else:
				textLine.SetPackedFontColor(0xFFFF0033)

			textLine.SetText("%s x%d  |cFFffce00(%d)" % (item.GetItemName(), count, itemCount))

			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.SetWindowVerticalAlignCenter()
			textLine.SetVerticalAlignCenter()

			if localeInfo.IsARABIC():
				(x,y) = textLine.GetTextSize()
				textLine.SetPosition(x, 0)
			else:
				textLine.SetPosition(15, 0)

			textLine.Show()
			self.children.append(textLine)

			self.dialogHeight += 34
			self.UpdateDialog()

	def UpdateDialog(self):
		newWidth = 280
		if hasattr(self, 'slotAfter') and self.slotAfter:
			newWidth = max(280, self.slotAfter.GetLocalPosition()[0] + self.slotAfter.GetWidth() + 30)
		newHeight = self.dialogHeight + 100 + 10

		newHeight -= 8

		if localeInfo.IsARABIC():
			self.board.SetPosition( newWidth, 0 )

			(x, y) = self.titleBar.GetLocalPosition()
			self.titleBar.SetPosition( newWidth - 15, y )

		if app.ENABLE_PITTY_REFINE:
			if self.boardPitty.IsShow():
				self.boardPitty.SetPosition(6, newHeight - 87 - 6)

				newHeight += self.boardPitty.GetHeight() - 50
				if newWidth < self.boardPitty.GetWidth() + 13:
					newWidth = self.boardPitty.GetWidth() + 13

		self.board.SetSize(newWidth, newHeight)
		if hasattr(self, 'toolTip') and self.toolTip:
			self.toolTip.SetPosition(15 + 35, 38)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):
		if 100 == self.percentage:
			self.Accept()
			return

		if 5 == self.type:
			self.Accept()
			return

		dlgQuestion = uiCommon.QuestionDialog2()
		dlgQuestion.SetText2(localeInfo.REFINE_WARNING2)
		dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.Accept))
		dlgQuestion.SetCancelEvent(ui.__mem_func__(dlgQuestion.Close))

		if 3 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_1)
			dlgQuestion.SetText2(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_2)
		elif 2 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DOWN_GRADE_WARNING)
		else:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING)

		dlgQuestion.Open()
		self.dlgQuestion = dlgQuestion

	def Accept(self):
		if app.ENABLE_PITTY_REFINE:
			bUseSealOfGod = TRUE if (self.checkBoxUseSeal.GetCheckStatus() or self.checkBoxUseSealAlways.GetCheckStatus()) else FALSE
			net.SendRefinePacket(self.targetItemPos, self.type, bUseSealOfGod)
		else:
			net.SendRefinePacket(self.targetItemPos, self.type)

		if not app.ENABLE_AUTO_REFINE:
			self.Close()

	if app.ENABLE_AUTO_REFINE:
		def AutoRefine(self, checkType, autoFlag):
			constInfo.IS_AUTO_REFINE = autoFlag

		def CheckRefine(self, isFail):
			if constInfo.IS_AUTO_REFINE == TRUE:
				if constInfo.AUTO_REFINE_TYPE == 1:
					if constInfo.AUTO_REFINE_DATA["ITEM"][0] != -1 and constInfo.AUTO_REFINE_DATA["ITEM"][1] != -1:
						scrollIndex = player.GetItemIndex(constInfo.AUTO_REFINE_DATA["ITEM"][0])
						itemIndex = player.GetItemIndex(constInfo.AUTO_REFINE_DATA["ITEM"][1])

						#chat.AppendChat(1,"test 2")
						# chat.AppendChat(chat.CHAT_TYPE_INFO, "%d %d" % (itemIndex, int(itemIndex %10)))
						if scrollIndex == 0 or (itemIndex % 10 == 8 and not isFail):
							#chat.AppendChat(1,"test 3")
							self.Close()
						else:
							net.SendItemUseToItemPacket(constInfo.AUTO_REFINE_DATA["ITEM"][0], constInfo.AUTO_REFINE_DATA["ITEM"][1])
				elif constInfo.AUTO_REFINE_TYPE == 2:
					npcData = constInfo.AUTO_REFINE_DATA["NPC"]
					if npcData[0] != 0 and npcData[1] != -1 and npcData[2] != -1 and npcData[3] != 0:
						itemIndex = player.GetItemIndex(npcData[1], npcData[2])
						if (itemIndex % 10 == 8 and not isFail) or isFail:
							self.Close()
						else:
							net.SendGiveItemPacket(npcData[0], npcData[1], npcData[2], npcData[3])
				else:
					#chat.AppendChat(1,"test 4")
					self.Close()
			else:
				#chat.AppendChat(1,"test 5")
				self.Close()

	def OnPressEscapeKey(self):
		self.CancelRefine()
		return TRUE

	def CancelRefine(self):
		if app.ENABLE_PITTY_REFINE:
			net.SendRefinePacket(255, 255, 255, False)
		else:
			net.SendRefinePacket(255, 255, 255)

		self.Close()

		if app.ENABLE_AUTO_REFINE:
			constInfo.AUTO_REFINE_TYPE = 0
			constInfo.AUTO_REFINE_DATA = {
				"ITEM" : [-1, -1],
				"NPC" : [0, -1, -1, 0]
			}

		if constInfo.SHOW_REFINE_ITEM_DESC:
			TOOLTIP_DATA["materials"] = []

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def SetInven(self, inven):
			self.inven = inven

		def SetCanMouseEventSlot(self, idx):
			if not hasattr(self, 'inven') or not self.inven:
				return
			if idx >= INVENTORY_PAGE_SIZE:
				page = self.inven.GetInventoryPageIndex()
				idx -= (page*INVENTORY_PAGE_SIZE)

			self.inven.wndItem.SetCanMouseEventSlot(idx)

		def SetCantMouseEventSlot(self, idx):
			if not hasattr(self, 'inven') or not self.inven:
				return
			if idx >= INVENTORY_PAGE_SIZE:
				page = self.inven.GetInventoryPageIndex()
				idx -= (page*INVENTORY_PAGE_SIZE)

			self.inven.wndItem.SetCantMouseEventSlot(idx)

	def OnUpdate(self):
		if self.IsShow():
			self.SetTop()

		if hasattr(self, 'itemImageCur') and self.itemImageCur:
			if self.itemImageCur.IsIn():
				if hasattr(self, 'toolTipCur') and self.toolTipCur:
					self.toolTipCur.ShowToolTip()
			else:
				if hasattr(self, 'toolTipCur') and self.toolTipCur:
					self.toolTipCur.HideToolTip()

		if hasattr(self, 'itemImageNext') and self.itemImageNext:
			if self.itemImageNext.IsIn():
				if hasattr(self, 'toolTipNext') and self.toolTipNext:
					self.toolTipNext.ShowToolTip()
			else:
				if hasattr(self, 'toolTipNext') and self.toolTipNext:
					self.toolTipNext.HideToolTip()
		
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if not hasattr(self, 'inven') or not self.inven:
				return

			if not hasattr(self, 'targetItemPos'):
				return

			targetItemPos = self.targetItemPos
			if targetItemPos <= 0:
				return

			page = self.inven.GetInventoryPageIndex()

			if (page*INVENTORY_PAGE_SIZE) <= targetItemPos < ((page + 1)*INVENTORY_PAGE_SIZE):
				lock_idx = targetItemPos - (page*INVENTORY_PAGE_SIZE)
				if hasattr(self.inven, 'wndItem'):
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
