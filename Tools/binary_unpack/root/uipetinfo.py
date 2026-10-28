if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiScriptLocale
import dbg
import snd
import mouseModule
import wndMgr
import skill
import localeInfo
import uiToolTip
import constInfo
import item
import chat
import uiCommon
import uiAffectShower
import functools
import event

SKILL_SLOT_ENABLE = "d:/ymir work/ui/game/pet/skill_button/skill_enable_button.sub"
SKILL_SLOT_MAX		= 3

TOTAL_EXP_GAUGE_COUNT = 5
BATTLE_EXP_GAUGE_MAX = 4
ITEM_EXP_GAUGE_POS = 4

FEED_WINDOW_X_SIZE = 3
FEED_WINDOW_Y_SIZE = 3

VISIBLE_LINE_COUNT = 4
DEFAULT_DESC_Y = 7

def unsigned32(n):
	return n & 0xFFFFFFFFL

"""
		PetMiniInfomationWindow appears in the bottom left corner.
		It shows experience bars, skill slots and their cooldowns
		and icon flash effect which indicates that pet is ready to
		evolve.
"""
class PetMiniInfomationWindow(ui.ScriptWindow):
	def __init__(self, wndPetInformation):
		import exception

		if not wndPetInformation:
			exception.Abort("wndPetInformation parameter must be set to PetInformationWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.skillSlot = []
		self.isLoaded = 0
		self.wndPetInformation = wndPetInformation
		self.petSlot = 0
		self.petSlotAniImg  = None
		self.expGauge		= None
		self.expGaugeBoard	= None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		if self.petSlot:
			self.petSlot.SetItemSlot(0, 0)

		if self.tooltipEXP:
			self.tooltipEXP.Hide()

		self.Hide()

	def Destroy(self):
		self.isLoaded = 0
		self.wndPetInformation = 0
		self.lifeTimeGauge	= None
		self.petSlot = 0
		self.petSlotAniImg  = None
		self.expGauge		= None
		self.expGaugeBoard	= None
		self.tooltipEXP		= None

		if self.skillSlot:
			del self.skillSlot[:]

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petMiniInformationWindow.py")
		except:
			import exception
			exception.Abort("PetMiniInfomationWindow.LoadWindow.LoadObject")

		try:
			## Pet Icon Slot
			self.petSlot = self.GetChild("pet_icon_slot")
			self.petSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.petSlot.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)

			# Pet Icon Slot Animation Image - Flash
			self.petSlotAniImg = self.GetChild("pet_icon_slot_ani_img")
			if hasattr(self.petSlotAniImg, "SetAlpha"):
				self.petSlotAniImg.SetAlpha(0.45)
			if hasattr(self.petSlotAniImg, "SetRenderingMode") and hasattr(wndMgr, "RENDERING_MODE_MODULATE"):
				self.petSlotAniImg.SetRenderingMode(wndMgr.RENDERING_MODE_MODULATE)
			self.petSlotAniImg.Hide()

			# EXP Gauges
			expGauge = []
			self.expGaugeBoard = self.GetChild("pet_mini_info_exp_gauge_board")
			expGauge.append(self.GetChild("pet_mini_EXPGauge_01"))
			expGauge.append(self.GetChild("pet_mini_EXPGauge_02"))
			expGauge.append(self.GetChild("pet_mini_EXPGauge_03"))
			expGauge.append(self.GetChild("pet_mini_EXPGauge_04"))
			expGauge.append(self.GetChild("pet_mini_EXPGauge_05"))

			for exp in expGauge:
				exp.SetSize(0, 0)

			self.expGauge	= expGauge
			self.tooltipEXP = TextToolTip()
			self.tooltipEXP.Hide()

			# Mini Info Skill Slot Scale
			for value in range(SKILL_SLOT_MAX):
				self.skillSlot.append(self.GetChild("mini_skill_slot"+str(value)))
				self.skillSlot[value].SetCoverButton(0, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, FALSE, FALSE)
				self.skillSlot[value].SetAlwaysRenderCoverButton(0)

			# This client renders the mini pet HP gauge wider than the
			# official TR client with the stock script values, so keep a
			# local override here to match the expected compact width.
			self.lifeTimeGauge = self.GetChild("LifeGauge")
			self.lifeTimeGauge.SetWindowHorizontalAlignLeft()

		except:
			import exception
			exception.Abort("PetMiniInfomationWindow.LoadWindow.BindObject")

		self.Hide()

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def OnUpdate(self):
		if self.expGaugeBoard.IsIn():
			self.tooltipEXP.Show()
		else:
			self.tooltipEXP.Hide()

		return

	def SetItemSlot( self, CurPetItemVNum ):
		self.petSlot.SetItemSlot( 0, CurPetItemVNum )
		self.petSlot.RefreshSlot()

	def SetSkillSlot( self, slotNumber, slotIndex, skillVnum ):
		if 0 > slotNumber or slotNumber >= SKILL_SLOT_MAX:
			return

		self.skillSlot[slotNumber].SetPetSkillSlotNew(slotIndex, skillVnum)
		self.skillSlot[slotNumber].SetCoverButton(slotIndex)

	def SetSkillCoolTime(self, slotNumber, slotIndex, max_cool_time, cur_cool_time):
		self.skillSlot[slotNumber].SetSlotCoolTime(slotIndex, max_cool_time, cur_cool_time)
		self.skillSlot[slotNumber].SetSlotCoolTimeColor(slotIndex, 0.0, 1.0, 0.0, 0.5)

	def ClearSkillSlot(self):
		for value in range(SKILL_SLOT_MAX):
			self.skillSlot[value].ClearSlot(0)
			self.skillSlot[value].SetCoverButton(0, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, FALSE, FALSE)
			self.skillSlot[value].SetAlwaysRenderCoverButton(0)

	def SetAlwaysRenderCoverButton(self, slotNumber):
		self.skillSlot[slotNumber].SetAlwaysRenderCoverButton(0, FALSE)

	def SelectItemSlot(self):
		if not self.wndPetInformation:
			return

		if self.wndPetInformation.IsShow():
			self.wndPetInformation.Close()
		else:
			self.wndPetInformation.Show()

	def SetLifeTime(self, curPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.lifeTimeGauge.SetPercentageWithScale(curPoint, maxPoint)

	def SetExperience(self, curPoint, maxPoint, itemExp, itemExpMax):
		curPoint = min(curPoint, maxPoint)
		curPoint = max(curPoint, 0)
		maxPoint = max(maxPoint, 0)

		itemExp = min(itemExp, itemExpMax)
		itemExp = max(itemExp, 0)
		itemExpMax = max(itemExpMax, 0)

		# Battle exp is divided into BATTLE_EXP_GAUGE_MAX parts
		quarterPoint = maxPoint / BATTLE_EXP_GAUGE_MAX
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(BATTLE_EXP_GAUGE_MAX, curPoint / quarterPoint)

		for i in xrange(TOTAL_EXP_GAUGE_COUNT):
			self.expGauge[i].Hide()

		for i in xrange(FullCount):
			self.expGauge[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.expGauge[i].Show()

		if 0 != quarterPoint:
			if FullCount < BATTLE_EXP_GAUGE_MAX:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.expGauge[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.expGauge[FullCount].Show()

		## Calculate the experience obtained with the item.
		## The last value of self.expGauge is the item exp marble.
		## If the Top value is 0, a full ball
		## An empty marble if Top value is -1
		if 0 != itemExpMax:
			itemExpGauge = self.expGauge[ITEM_EXP_GAUGE_POS]
			Percentage = float(itemExp) / float(itemExpMax) - float(1.0)
			itemExpGauge.SetRenderingRect(0.0, Percentage, 0.0, 0.0)
			itemExpGauge.Show()

		output_cur_exp = curPoint + itemExp
		output_max_exp = maxPoint + itemExpMax

		## TEXT output is based on hunting experience + item experience.
		expPercent = 0
		if output_max_exp:
			expPercent = float(output_cur_exp) / float(output_max_exp) * 100

		if app.ENABLE_MULTI_TEXTLINE:
			tooltip_text = str(localeInfo.PET_INFO_EXP) + ': '+ str(curPoint) + '\n' + str(localeInfo.PET_INFO_NEXT_EXP) + ': ' + str(maxPoint - curPoint) + '\n'	\
						 + str(localeInfo.PET_INFO_ITEM_EXP) + ': '+ str(itemExp) + '\n' + str(localeInfo.PET_INFO_NEXT_ITEM_EXP) + ': ' + str(itemExpMax - itemExp)

			self.tooltipEXP.SetText(tooltip_text)
		else:
			self.tooltipEXP.SetText("%s : %.2f%%" % (localeInfo.TASKBAR_EXP, expPercent))

	def OnFlashEvent(self):
		if self.petSlotAniImg:
			self.petSlotAniImg.Show()

	def OffFlashEvent(self):
		if self.petSlotAniImg:
			self.petSlotAniImg.Hide()

"""
		PetHatchingWindow will appear when hatching an egg.
		It can be closed manually or after successful egg hatch.
"""
class PetHatchingWindow(ui.ScriptWindow):
	def __init__(self, wndPetInformation):
		import exception

		if not wndPetInformation:
			exception.Abort("wndPetInformation parameter must be set to PetInformationWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.hatchingSlot = 0
		self.eggItemSlotIndex = -1
		self.eggItemSlotWindow = player.INVENTORY
		self.wndPetInformation = wndPetInformation
		self.hatchingButton = 0
		self.petNameEdit = 0
		self.petName = 0
		self.questionDialog = 0
		self.popupDialog = 0
		self.hatchingMoneyText = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def Close(self):
		self.ClearMouseEventEggItem()
		self.hatchingSlot.SetItemSlot(0, 0)
		self.hatchingSlot.RefreshSlot()
		self.petName = 0

		self.Hide()
		player.SetOpenPetHatchingWindow(FALSE)
		net.SendPetHatchingWindowPacket(FALSE)

		if self.questionDialog:
			self.questionDialog.Close()

		if self.popupDialog:
			self.popupDialog.Close()

		if self.petNameEdit:
			self.petNameEdit.KillFocus()

	def Destroy(self):
		self.isLoaded = 0
		self.hatchingSlot = 0
		self.wndPetInformation = 0
		self.hatchingButton = 0
		self.petName = 0
		self.hatchingMoneyText = None

		if self.popupDialog:
			self.popupDialog.Destroy()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petHatchingWindow.py")
		except:
			import exception
			exception.Abort("petHatchingWindow.LoadWindow.LoadObject")

		try:
			self.GetChild("PetHatching_TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.hatchingSlot = self.GetChild("HatchingItemSlot")
			self.hatchingSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.hatchingSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.hatchingSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			# Hatch button
			self.hatchingButton = self.GetChild("HatchingButton")
			self.hatchingButton.SetEvent(ui.__mem_func__(self.ClickHatchingButton))

			## Hatching Gold TEXT
			self.hatchingMoneyText = self.GetChild("HatchingMoney");
			self.hatchingMoneyText.SetText(localeInfo.PET_HATCHING_MONEY % localeInfo.NumberToMoneyString(0) )

			# Pet name editline
			self.petNameEdit = self.GetChild("pet_name")
			self.petNameEdit.SetText("")
			self.petNameEdit.SetReturnEvent(ui.__mem_func__(self.ClickHatchingButton))
			self.petNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
			self.petNameEdit.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.OnMouseLeftButtonUpEvent))
			self.petNameEdit.SetFocus()
			self.petNameEdit.Show()

			# Initialize question and popup dialogs
			self.__MakeQuestionDialog()
			self.__MakePopupDialog()

		except:
			import exception
			exception.Abort("petHatchingWindow.LoadWindow.BindObject")

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __MakeQuestionDialog(self):
		if not self.questionDialog:
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText("")
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__HatchingQuestionDialogAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__HatchingQuestionDialogCancel))

	def __MakePopupDialog(self):
		self.popupDialog = uiCommon.PopupDialog()
		self.popupDialog.SetText("")

	def ClickHatchingButton(self):
		if self.popupDialog:
			if self.popupDialog.IsShow():
				return

		self.__OpenHatchingQuestionDialog()

	def OnMouseLeftButtonUpEvent(self):
		if self.petName == self.petNameEdit.GetText():
			self.petNameEdit.SetText("")
			self.petNameEdit.SetEndPosition()

	def OverInItem(self, slotIndex):
		return

	def OverOutItem(self):
		return

	def ClearMouseEventEggItem(self):
		if self.eggItemSlotIndex == -1:
			return

		# Unlock egg item
		inven_slot_pos = self.eggItemSlotIndex

		if inven_slot_pos >= player.INVENTORY_PAGE_SIZE:
			inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
			inven_slot_pos -= (inven_page * player.INVENTORY_PAGE_SIZE)

		self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(inven_slot_pos)

		# Reset egg item position
		self.eggItemSlotIndex  = -1
		self.eggItemSlotWindow = player.INVENTORY

	def OnUpdate(self):
		if not self.wndPetInformation.inven:
			return

		if self.eggItemSlotIndex == -1:
			return

		if not self.hatchingSlot:
			return

		try:
			inven = self.wndPetInformation.inven
			invenPage = inven.GetInventoryPageIndex() ## 0 or 1

			min_range = invenPage * player.INVENTORY_PAGE_SIZE ## 0 or 45
			max_range = (invenPage + 1) * player.INVENTORY_PAGE_SIZE ## 45 or 90

			# Lock egg item
			inven_slot_pos = self.eggItemSlotIndex

			if min_range <= inven_slot_pos < max_range:
				inven_slot_pos = inven_slot_pos - min_range
				inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)

		except:
			pass

		return

	def HatchingWindowOpen(self, slotWindow, slotIndex):
		# Check if player has no other trading windows opened
		checkMsg = net.CheckUsePetItem()

		if checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_TRADING:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		if checkMsg != item.PET_EGG_USE_TRUE:
			return

		if not self.hatchingSlot:
			return

		ItemVNum = player.GetItemIndex(slotWindow, slotIndex)

		if ItemVNum == 0:
			return

		item.SelectItem(ItemVNum)

		growthPetVnum = item.GetValue(0)

		if growthPetVnum == 0:
			return

		hatching_money_value = item.GetValue(3)
		if self.hatchingMoneyText:
			self.hatchingMoneyText.SetText(localeInfo.PET_HATCHING_MONEY % localeInfo.NumberToMoneyString(hatching_money_value))

		self.Close()
		self.eggItemSlotIndex = slotIndex
		self.eggItemSlotWindow = slotWindow
		self.hatchingSlot.SetItemSlot(0, growthPetVnum)
		self.hatchingSlot.RefreshSlot()
		item_string = item.GetItemName()
		self.petName = item_string[:item.PET_NAME_MAX_SIZE]
		self.petNameEdit.SetText(self.petName)
		self.petNameEdit.SetEndPosition()
		self.petNameEdit.SetFocus()
		self.petNameEdit.Show()
		self.Show()
		net.SendPetHatchingWindowPacket(TRUE)
		player.SetOpenPetHatchingWindow(TRUE)

	def __OpenHatchingQuestionDialog(self):
		interface = self.wndPetInformation.interface

		if interface.IsShowDlgQuestionWindow():
			interface.CloseDlgQuestionWindow()

		if not self.questionDialog:
			self.__MakeQuestionDialog()

		# Reset position if name has no characters inside editline
		if "" == self.petNameEdit.GetText():
			self.petNameEdit.SetText(self.petName)
			self.petNameEdit.SetEndPosition()

		self.questionDialog.SetText(localeInfo.PET_HATCHING_ACCEPT % (self.petNameEdit.GetText()))
		self.questionDialog.SetTop()
		self.questionDialog.Open()

	def __HatchingQuestionDialogAccept(self):
		self.questionDialog.Close()

		# Check if pet's name is smaller than defined minimal size (item.PET_NAME_MIN_SIZE)
		if len(self.petNameEdit.GetText()) < item.PET_NAME_MIN_SIZE:
			self.petNameEdit.SetText("")
			self.__OpenPopupDialog(localeInfo.PET_NAME_MIN)
			return

		itemVnum = player.GetItemIndex(self.eggItemSlotWindow, self.eggItemSlotIndex)
		if itemVnum == 0:
			return

		item.SelectItem(itemVnum)
		hatching_money = item.GetValue(3)

		# Check if player has enough money to hatch an egg
		if player.GetMoney() < hatching_money:
			self.__OpenPopupDialog(localeInfo.PET_MSG_NOT_ENOUGH_MONEY)
			return

		net.SendPetHatchingPacket(self.petNameEdit.GetText(), self.eggItemSlotWindow, self.eggItemSlotIndex)

	def __HatchingQuestionDialogCancel(self):
		self.questionDialog.Close()

	def __OpenPopupDialog(self, str):
		interface = self.wndPetInformation.interface

		if interface.IsShowDlgQuestionWindow():
			interface.CloseDlgQuestionWindow()

		if not self.popupDialog:
			self.__MakePopupDialog()

		self.popupDialog.SetText(str)
		self.popupDialog.SetTop()
		self.popupDialog.Open()

	def PetHatchingWindowCommand(self, command):
		# Egg hatch succeeded, close the window
		if command == item.EGG_USE_SUCCESS:
			self.Close()

		# Egg hatch failed, clear the name
		elif command == item.EGG_USE_FAILED_BECAUSE_NAME:
			self.petNameEdit.SetText("")
			self.petNameEdit.SetEndPosition()

		# Egg hatch hit an error, close the window
		elif command == item.EGG_USE_FAILED_TIMEOVER:
				self.Close()

"""
		PetNameChangeWindow will appear when change name item is
		dragged to upbringing item. It can be closed manually or 
		after successful name change.
"""

class PetNameChangeWindow(ui.ScriptWindow):
	def __init__(self, wndPetInformation):
		import exception

		if not wndPetInformation:
			exception.Abort("wndPetInformation parameter must be set to PetInformationWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndPetInformation = wndPetInformation

		self.nameChangeItemSlotIndex = -1
		self.petItemSlotIndex = -1

		self.petItemSlot = None
		self.nameChangeButton = None
		self.petNameEdit = None
		self.petName = None
		self.questionDialog = None
		self.popupDialog = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def Close(self):
		self.ClearMouseEventItem()

		if self.petItemSlot:
			self.petItemSlot.SetItemSlot(0, 0)
			self.petItemSlot.RefreshSlot()

		self.petName = None
		self.Hide()

		player.SetOpenPetNameChangeWindow(FALSE)
		net.SendPetNameChangeWindowPacket(FALSE)

		if self.questionDialog:
			self.questionDialog.Close()

		if self.popupDialog:
			self.popupDialog.Close()

		if self.petNameEdit:
			self.petNameEdit.KillFocus()

	def Destroy(self):
		self.isLoaded			= 0
		self.wndPetInformation	= None
		self.petItemSlot		= None
		self.nameChangeButton	= None
		self.petName			= None

		if self.popupDialog:
			self.popupDialog.Destroy()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petNameChangeWindow.py")
		except:
			import exception
			exception.Abort("petNameChangeWindow.LoadWindow.LoadObject")

		try:
			self.GetChild("PetNameChangeTitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.petItemSlot = self.GetChild("PetItemSlot")
			self.petItemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)

			# Name change button
			self.nameChangeButton = self.GetChild("NameChangeButton")
			self.nameChangeButton.SetEvent(ui.__mem_func__(self.ClickNameChangeButton))

			# Pet change name price
			MoneyText = self.GetChild("NameChangeMoney");
			MoneyText.SetText(localeInfo.PET_HATCHING_MONEY % (localeInfo.NumberToMoneyString(item.PET_HATCHING_MONEY)))

			# Pet name editline
			self.petNameEdit = self.GetChild("pet_name")
			self.petNameEdit.SetText("")
			self.petNameEdit.SetReturnEvent(ui.__mem_func__(self.ClickNameChangeButton))
			self.petNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
			self.petNameEdit.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.OnMouseLeftButtonUpEvent))
			self.petNameEdit.SetFocus()
			self.petNameEdit.Show()

			# Initialize question and popup dialogs
			self.__MakeQuestionDialog()
			self.__MakePopupDialog()

		except:
			import exception
			exception.Abort("petNameChangeWindow.LoadWindow.BindObject")

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __MakeQuestionDialog(self):
		if not self.questionDialog:
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText("")
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__NameChangeQuestionDialogAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__NameChangeQuestionDialogCancel))

	def __MakePopupDialog(self):
		self.popupDialog = uiCommon.PopupDialog()
		self.popupDialog.SetText("")

	def ClickNameChangeButton(self):
		if self.popupDialog:
			if self.popupDialog.IsShow():
				return

		self.__OpenNameChangeQuestionDialog()

	def OnMouseLeftButtonUpEvent(self):
		if self.petName == self.petNameEdit.GetText():
			self.petNameEdit.SetText("")
			self.petNameEdit.SetEndPosition()

	def ClearMouseEventItem(self):
		if self.nameChangeItemSlotIndex == -1:
			return

		if self.petItemSlotIndex == -1:
			return

		# Unlock name change item
		if self.nameChangeItemSlotIndex >= player.INVENTORY_PAGE_SIZE:
			inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
			self.nameChangeItemSlotIndex -= (inven_page * player.INVENTORY_PAGE_SIZE)

		self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(self.nameChangeItemSlotIndex)

		# Unlock pet upbringing item
		if self.petItemSlotIndex >= player.INVENTORY_PAGE_SIZE:
			inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
			self.petItemSlotIndex -= (inven_page * player.INVENTORY_PAGE_SIZE)

		self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(self.petItemSlotIndex)

		# Reset item positions
		self.nameChangeItemSlotIndex	= -1
		self.petItemSlotIndex			= -1

	def OnUpdate(self):
		if not self.wndPetInformation.inven:
			return

		if self.nameChangeItemSlotIndex == -1:
			return

		if self.petItemSlotIndex == -1:
			return

		if not self.petItemSlot:
			return

		try:
			inven		= self.wndPetInformation.inven
			invenPage	= inven.GetInventoryPageIndex() ## 0 or 1

			min_range = invenPage * player.INVENTORY_PAGE_SIZE
			max_range = (invenPage + 1) * player.INVENTORY_PAGE_SIZE

			# Name change
			inven_slot_pos = self.nameChangeItemSlotIndex
			if min_range <= inven_slot_pos < max_range:
				inven_slot_pos = inven_slot_pos - min_range
				inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)

			# Lock pet upbringing item
			inven_slot_pos = self.petItemSlotIndex
			if min_range <= inven_slot_pos < max_range:
				inven_slot_pos = inven_slot_pos - min_range
				inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)
		except:
			pass

		return

	def NameChangeWindowOpen(self, srcSlotIndex, dstSlotIndex):
		checkMsg = net.CheckUsePetItem()

		if checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_TRADING:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		elif checkMsg == item.PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_EGG_ITEM_USE)

		if checkMsg != item.PET_EGG_USE_TRUE:
			return

		if not self.petItemSlot:
			return

		petItemVnum = player.GetItemIndex(dstSlotIndex)
		if 0 == petItemVnum:
			return

		metinSlot = [player.GetItemMetinSocket(dstSlotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		pet_id = metinSlot[2]
		if 0 == pet_id:
			return

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)

		self.Close()
		self.nameChangeItemSlotIndex = srcSlotIndex
		self.petItemSlotIndex = dstSlotIndex

		self.petItemSlot.SetItemSlot(0, petItemVnum)
		self.petItemSlot.RefreshSlot()
		self.petName = pet_nick
		self.petNameEdit.SetText(self.petName)
		self.petNameEdit.SetEndPosition()
		self.petNameEdit.SetFocus()
		self.petNameEdit.Show()
		self.Show()

		net.SendPetNameChangeWindowPacket(TRUE)
		player.SetOpenPetNameChangeWindow(TRUE)

	def __OpenNameChangeQuestionDialog(self):
		interface = self.wndPetInformation.interface

		if interface.IsShowDlgQuestionWindow():
			interface.CloseDlgQuestionWindow()

		if not self.questionDialog:
			self.__MakeQuestionDialog()

		# Reset position if name has no characters inside editline
		if "" == self.petNameEdit.GetText():
			self.petNameEdit.SetText(self.petName)
			self.petNameEdit.SetEndPosition()

		self.questionDialog.SetText(localeInfo.PET_NAME_CHANGE_ACCEPT % (self.petNameEdit.GetText()))
		self.questionDialog.SetTop()
		self.questionDialog.Open()

	def __NameChangeQuestionDialogAccept(self):
		self.questionDialog.Close()

		# Check if pet's name is smaller than defined minimal size (item.PET_NAME_MIN_SIZE)
		if len(self.petNameEdit.GetText()) < item.PET_NAME_MIN_SIZE:
			self.petNameEdit.SetText("")
			self.__OpenPopupDialog(localeInfo.PET_NAME_MIN)
			return

		# Check if player has enough money to change the name
		if player.GetMoney() < item.PET_HATCHING_MONEY:
			self.__OpenPopupDialog(localeInfo.PET_MSG_NOT_ENOUGH_MONEY)
			return

		net.SendPetNameChangePacket(self.petNameEdit.GetText(), self.nameChangeItemSlotIndex, self.petItemSlotIndex)

	def __NameChangeQuestionDialogCancel(self):
		self.questionDialog.Close()

	def __OpenPopupDialog(self, str):
		interface = self.wndPetInformation.interface

		if interface.IsShowDlgQuestionWindow():
			interface.CloseDlgQuestionWindow()

		if not self.popupDialog:
			self.__MakePopupDialog()

		self.popupDialog.SetText(str)
		self.popupDialog.SetTop()
		self.popupDialog.Open()

	def PetNameChangeWindowCommand(self, command):
		if command == item.NAME_CHANGE_USE_SUCCESS:
			self.Close()

		# Name change has failed, clear the name
		elif command == item.NAME_CHANGE_USE_FAILED_BECAUSE_NAME:
			self.petNameEdit.SetText("")
			self.petNameEdit.SetEndPosition()

"""
		PetFeedWindow is opened through main pet interface.
		It consists of item slot grid on which player can
		drag items. Used for pet lifetime, evolve & item exp.
"""

class PetFeedWindow(ui.ScriptWindow):
	def __init__(self, wndPetInformation):
		import exception

		if not wndPetInformation:
			exception.Abort("wndPetInformation parameter must be set to PetInformationWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.feedButtonClickTime = 0
		self.backupFeedItems = []
		self.wndPetInformation = wndPetInformation

		self.questionDialogEmptyPos = -1
		self.questionDialogItemPos = -1

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		player.SetOpenPetFeedWindow(TRUE)

		if self.wndPetInformation and self.wndPetInformation.inven:
			self.wndPetInformation.inven.Show()

	def ClearFeedItems(self):
		self.ClearMouseEventFeedItems()

		if self.FeedItemSlot:
			for slotPos in xrange(self.FeedItemSlot.GetSlotCount()):
				self.FeedItemSlot.ClearSlot(slotPos)

			self.FeedItemSlot.RefreshSlot()

	def SetOnTopWindowNone(self):
		if not self.wndPetInformation:
			return

		if not self.wndPetInformation.interface:
			return

		interface = self.wndPetInformation.interface
		interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
		interface.RefreshMarkInventoryBag()

	def Close(self):
		self.SetOnTopWindowNone()
		self.wndPetInformation.PetFeedToggleButtonUpAll()
		self.wndPetInformation.feedIndex = player.FEED_BUTTON_MAX
		self.ClearFeedItems()

		if self.questionDialog:
			self.questionDialog.Close()

		self.Hide()

		player.SetOpenPetFeedWindow(FALSE)

	def Destroy(self):
		del self.FeedItems[:]
		del	self.FeedItemsCount[:]
		del self.FeedItemDummy[:]
		self.FeedItems		= None
		self.FeedItemsCount	= None
		self.FeedItemSlot	= None
		self.FeedItemDummy  = None
		self.questionDialog = None

		self.questionDialogEmptyPos = -1
		self.questionDialogItemPos = -1

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petFeedWindow.py")
		except:
			import exception
			exception.Abort("petFeedWindow.LoadWindow.LoadObject")

		try:
			self.GetChild("PetFeed_TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			# Item slot grid
			FeedItemSlot = self.GetChild("FeedItemSlot")
			FeedItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			FeedItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			FeedItemSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UnselectItemSlot))
			FeedItemSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			FeedItemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			FeedItemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.FeedItemSlot = FeedItemSlot

			self.FeedItems		= []
			self.FeedItemsCount = []

			# Used as a sub-container for items to check their positions
			self.FeedItemDummy	= []

			self.ClearMouseEventFeedItems()

			self.feedButton = self.GetChild("FeedButton")
			if self.feedButton:
				self.feedButton.SetEvent(ui.__mem_func__(self.ClickPetFeedButton))

			self.questionDialog = uiCommon.QuestionDialog2()
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__ItemMoveQuestionDialogAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__ItemMoveQuestionDialogCancel))
			self.questionDialog.Close()

		except:
			import exception
			exception.Abort("petFeedWindow.LoadWindow.BindObject")

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def OverInItem(self, slotIndex):
		if None != self.wndPetInformation.tooltipItem:
			invenPos = self.FeedItems[slotIndex]
			if invenPos != -1:
				self.wndPetInformation.tooltipItem.SetInventoryItem(invenPos, player.INVENTORY)

		return

	def OverOutItem(self):
		if None != self.wndPetInformation.tooltipItem:
			self.wndPetInformation.tooltipItem.HideToolTip()
		return

	def UseItemSlot(self, slotIndex):
		self.RemoveItemSlot(slotIndex)
		return

	def UnselectItemSlot(self, slotIndex):
		self.RemoveItemSlot(slotIndex)
		return

	def SelectItemSlot(self, slotIndex):
		self.RemoveItemSlot(slotIndex)
		return

	def RemoveItemSlot(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			return

		# Unlock item that was inside feed window
		inven_slot_pos = self.FeedItems[slotIndex]
		if inven_slot_pos != -1:
			if inven_slot_pos >= player.INVENTORY_PAGE_SIZE:
				inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
				inven_slot_pos -= (inven_page * player.INVENTORY_PAGE_SIZE)

			self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(inven_slot_pos)

		self.DeleteDataDummySlot(slotIndex, self.FeedItems[slotIndex])
		self.FeedItems[slotIndex] = -1
		self.FeedItemsCount[slotIndex] = 0
		self.FeedItemSlot.ClearSlot(slotIndex)
		self.FeedItemSlot.RefreshSlot()

	# Search for empty slot space for item
	# If not found, return -1
	def SearchEmptySlot(self, size):
		for value in range(player.PET_FEED_SLOT_MAX):

			if 0 == self.FeedItemDummy[value]:	# Check if value inside this slot is empty

				if 1 == size:
					return value

				emptySlotIndex = value
				searchSucceed = TRUE

				for i in range(size - 1):
					emptySlotIndex = emptySlotIndex + FEED_WINDOW_X_SIZE

					if emptySlotIndex >= player.PET_FEED_SLOT_MAX:
						searchSucceed = FALSE
						continue

					if 1 == self.FeedItemDummy[emptySlotIndex]:
						searchSucceed = FALSE

				if TRUE == searchSucceed:
					return value

		return -1

	## When using an inventory item with a right-click
	## Inventory -> Feed window
	def ItemMoveFeedWindow(self, slotWindow, slotIndex):
		if player.INVENTORY == slotWindow:
			attachSlotType = player.SLOT_TYPE_INVENTORY
		else:
			return FALSE

		checkTime = app.GetGlobalTimeStamp() - self.feedButtonClickTime
		if checkTime < 2:
			if slotIndex in self.backupFeedItems:
				return FALSE
		else:
			self.backupFeedItems = []

		mouseModule.mouseController.DeattachObject()

		selectedItemVNum = player.GetItemIndex(slotWindow, slotIndex)
		count			 = player.GetItemCount(slotWindow, slotIndex)

		mouseModule.mouseController.AttachObject(self, attachSlotType, slotIndex, selectedItemVNum, count)

		item.SelectItem(selectedItemVNum)
		itemSize = item.GetItemSize()

		emptySlotPos = self.SearchEmptySlot(itemSize[1])

		if -1 != emptySlotPos:
			self.ProcessItemMove(slotIndex, emptySlotPos)
			return TRUE

		return FALSE

	def ProcessItemMove(self, slotIndex, emptySlotIndex):
		itemVnum = player.GetItemIndex(player.INVENTORY, slotIndex)
		item.SelectItem(itemVnum)

		# If item has one or more attributes, display a question dialog to continue
		if item.GetItemType() in [item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR]:
			metinSlot = [player.GetItemMetinSocket(player.INVENTORY, slotIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
			attrSlot = [player.GetItemAttribute(player.INVENTORY, slotIndex, i) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]

			attributeCount = 0
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				type = attrSlot[i][0]
				value = attrSlot[i][1]

				if value:
					attributeCount += 1

			if attributeCount:
				self.questionDialogEmptyPos = emptySlotIndex
				self.questionDialogItemPos = slotIndex

				self.questionDialog.SetText1(localeInfo.SECOND_CONFIRM_PET_FEED_QUESTION_1 % attributeCount)
				self.questionDialog.SetText2(localeInfo.SECOND_CONFIRM_PET_FEED_QUESTION_2)
				self.questionDialog.SetTop()
				self.questionDialog.Open()

				if mouseModule.mouseController.isAttached():
					mouseModule.mouseController.DeattachObject()
				return

		# Item did not have any attributes, move it to the feed window
		self.InsertItemToSlot(emptySlotIndex)

	def __ItemMoveQuestionDialogAccept(self):
		attachSlotType = player.SLOT_TYPE_INVENTORY
		selectedItemVNum = player.GetItemIndex(player.INVENTORY, self.questionDialogItemPos)
		count			 = player.GetItemCount(player.INVENTORY, self.questionDialogItemPos)

		mouseModule.mouseController.AttachObject(self, attachSlotType, self.questionDialogItemPos, selectedItemVNum, count)

		self.InsertItemToSlot(self.questionDialogEmptyPos)

		# Clear question dialog data for further use
		self.__ClearItemMoveQuestionDialog()

	def __ItemMoveQuestionDialogCancel(self):
		self.__ClearItemMoveQuestionDialog()

	def __ClearItemMoveQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()

		# Clear the locked item (it will stay locked if it was put into the feed window)
		inven_slot_pos = self.questionDialogItemPos
		if inven_slot_pos != -1:
			if inven_slot_pos >= player.INVENTORY_PAGE_SIZE:
				inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
				inven_slot_pos -= (inven_page * player.INVENTORY_PAGE_SIZE)

			self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(inven_slot_pos)

		self.questionDialogEmptyPos = -1
		self.questionDialogItemPos = -1

	# Inventory -> Feed Window
	def SelectEmptySlot(self, slotIndex):
		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()

		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return FALSE

		self.ItemMoveFeedWindow(player.INVENTORY, attachedSlotPos)

	def InsertItemToSlot(self, slotIndex):
		checkTime = app.GetGlobalTimeStamp() - self.feedButtonClickTime
		if checkTime < 2:
			if slotIndex in self.backupFeedItems:
				return FALSE
		else:
			self.backupFeedItems = []

		if not mouseModule.mouseController.isAttached():
			return FALSE

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
		attachedItemVNum = player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType	= item.GetItemType()
		itemSubType = item.GetItemSubType()

		# Return if item is beging attached from not inventory window
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return FALSE

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT:
			return FALSE

		# Return if there is no pet summoned
		petVNum = player.GetActivePetItemVNum()
		if 0 == petVNum:
			return FALSE

		# Return if active pet is being dragged to the window
		if item.ITEM_TYPE_GROWTH_PET == itemType and itemSubType == item.PET_UPBRINGING:
			activePetId = player.GetActivePetItemId()
			petId = player.GetItemMetinSocket(attachedSlotPos, 2)
			if petId == activePetId:
				return FALSE

		# Display a msg if item type does not correspond to condtions
		if self.wndPetInformation.CantFeedItem(attachedSlotPos):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_FEED_TYPE)
			return FALSE

		if -1 != self.FeedItems[slotIndex]:
			return FALSE

		if attachedSlotPos not in self.FeedItems:
			mouseModule.mouseController.DeattachObject()

			invenItemCount = player.GetItemCount(attachedSlotPos)
			if attachedItemCount != invenItemCount:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_FEED_SPLIT_ITEM)
				return FALSE

			self.FeedItems[slotIndex] = attachedSlotPos
			self.FeedItemsCount[slotIndex] = attachedItemCount
			self.InsertDataDummySlot(slotIndex, attachedItemVNum)
			self.FeedItemSlot.SetItemSlot(slotIndex, attachedItemVNum, attachedItemCount)
			self.FeedItemSlot.RefreshSlot()

		return TRUE

	def InsertDataDummySlot(self, slotIndex, vnum):
		self.FeedItemDummy[slotIndex] = 1

		item.SelectItem(vnum)
		itemSize = item.GetItemSize(vnum)	# Item size to put

		if 1 == itemSize[1]:
			return

		addSlotIndex = slotIndex

		for value in range(itemSize[1] - 1):
			addSlotIndex = addSlotIndex + FEED_WINDOW_X_SIZE

			if addSlotIndex >= player.PET_FEED_SLOT_MAX:
				return

			self.FeedItemDummy[addSlotIndex] = 1

	def DeleteDataDummySlot(self, slotIndex, InvenPos):
		vnum = player.GetItemIndex(InvenPos)
		item.SelectItem(vnum)
		itemSize = item.GetItemSize(vnum)	# Item size to be subtracted

		self.FeedItemDummy[slotIndex] = 0

		if 1 == itemSize[1]:
			return

		delSlotIndex = slotIndex

		for value in range(itemSize[1] - 1):
			delSlotIndex = delSlotIndex + FEED_WINDOW_X_SIZE

			if delSlotIndex >= player.PET_FEED_SLOT_MAX:
				return

			self.FeedItemDummy[delSlotIndex] = 0

	# Send feed packet if there are items inside the window
	def ClickPetFeedButton(self):
		resultFeedItems = [value for value in self.FeedItems if value != -1]
		resultFeedItemCounts = [value for value in self.FeedItemsCount if value != 0]

		if resultFeedItems:
			if net.SendPetFeedPacket(self.wndPetInformation.feedIndex, resultFeedItems, resultFeedItemCounts):
				self.feedButtonClickTime = app.GetGlobalTimeStamp()

	def ClearMouseEventFeedItems(self):
		# Unlock items that are inside the feed window
		for inven_slot_pos in self.FeedItems:
			if inven_slot_pos != -1:
				if inven_slot_pos >= player.INVENTORY_PAGE_SIZE:
					inven_page = self.wndPetInformation.inven.GetInventoryPageIndex()
					inven_slot_pos -= (inven_page * player.INVENTORY_PAGE_SIZE)

				self.wndPetInformation.inven.wndItem.SetCanMouseEventSlot(inven_slot_pos)

		del self.FeedItems[:]
		del self.FeedItemsCount[:]
		del self.FeedItemDummy[:]

		for value in range(0, player.PET_FEED_SLOT_MAX):
			self.FeedItems.append(-1)
			self.FeedItemsCount.append(0)
			self.FeedItemDummy.append(0)

	def BackUpSucceedFeedItems(self):
		self.backupFeedItems = self.FeedItems[:]

	def OnUpdate(self):
		if self.wndPetInformation.inven == 0:
			return

		inven = self.wndPetInformation.inven
		invenPage = inven.GetInventoryPageIndex() ## 0 or 1

		min_range = invenPage * player.INVENTORY_PAGE_SIZE ## 0 or 45
		max_range = (invenPage + 1) * player.INVENTORY_PAGE_SIZE ## 45 or 90

		# Lock items that are inside the feed window
		for inven_slot_pos in self.FeedItems:
			if inven_slot_pos == -1:
				continue

			if min_range <= inven_slot_pos < max_range:
				inven_slot_pos = inven_slot_pos - min_range
				inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)

		if self.questionDialogItemPos != -1:
			inven_slot_pos = self.questionDialogItemPos
			if min_range <= inven_slot_pos < max_range:
				inven_slot_pos = inven_slot_pos - min_range
				inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)

		return

	def OnTop(self):
		if not self.wndPetInformation:
			return

		if not self.wndPetInformation.interface:
			return

		interface = self.wndPetInformation.interface
		interface.SetOnTopWindow(player.ON_TOP_WND_PET_FEED)
		interface.RefreshMarkInventoryBag()


"""
		PetInformationWindow is opened through taskbar or 'P' key.
		Shows pet statistics and windows for attr change/pet revive.
"""

class PetInformationWindow(ui.ScriptWindow):
	wndPetFeed		= None
	tooltipItem		= None
	inven			= None
	wndPetHatching	= None
	wndPetNameChange= None
	wndPetMiniInfo	= None
	feedIndex		= player.FEED_BUTTON_MAX
	skillSlot		= []
	feedButton		= []

	SkillBookSlotIndex	= -1
	SkillBookInvenIndex = -1

	SkillBookDelSlotIndex	= -1
	SkillBookDelInvenIndex	= -1

	typeInfo = {
		1	:	localeInfo.PET_ATTR_DETERMINE_TYPE1,
		2	:	localeInfo.PET_ATTR_DETERMINE_TYPE2,
		3	:	localeInfo.PET_ATTR_DETERMINE_TYPE3,
		4	:	localeInfo.PET_ATTR_DETERMINE_TYPE4,
		5	:	localeInfo.PET_ATTR_DETERMINE_TYPE5,
		6	:	localeInfo.PET_ATTR_DETERMINE_TYPE6,
		7	:	localeInfo.PET_ATTR_DETERMINE_TYPE7,
		8	:	localeInfo.PET_ATTR_DETERMINE_TYPE8,
	}

	class DescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.descIndex = 0

		def __del__(self):
			ui.Window.__del__(self)

		def SetIndex(self, index):
			self.descIndex = index

		def OnRender(self):
			event.RenderEventSet(self.descIndex)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.interface = None
		self.isLoaded = 0

		# Tab - Pages
		self.state = player.PET_WINDOW_INFO
		self.tabDict		= None
		self.tabButtonDict	= None
		self.pageDict		= None
		self.AffectShower	= None
		self.popupDialog	= None
		self.questionDialog	= None
		self.skillUpgradeGold	= 0
		self.skillUpgradeSlot	= -1
		self.skillUpgradeIndex	= -1
		self.tooptipPetSkill	= None
		self.attrChangeIndex	= [-1, -1, -1]
		self.petReviveIndex		= -1
		self.petReviveItems	= [-1 for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX)]
		self.petReviveItemsCount	= [0 for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX)]
		self.isPetReviveResultTooltip = FALSE
		self.descIndex = 0
		self.desc_Y = DEFAULT_DESC_Y
		self.SetWindowName("PetInformationWindow")
		self.__LoadWindow()
		self.wndPetHatching		= PetHatchingWindow(self)
		self.wndPetNameChange	= PetNameChangeWindow(self)
		self.wndPetMiniInfo 	= PetMiniInfomationWindow(self)
		self.wndPetFeed			= PetFeedWindow(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def Hide(self):
		if self.wndPetFeed:
			self.wndPetFeed.Close()

		wndMgr.Hide(self.hWnd)

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petInformationWindow.py")

		except:
			import exception
			exception.Abort("petInformationWindow.LoadWindow.__LoadScript")

		try:
			# Close Button Event
			self.GetChild("PetWindowCloseButton").SetEvent(ui.__mem_func__(self.Close))

			# Page Tabs
			self.tabDict = {
				player.PET_WINDOW_INFO					: self.GetChild("Tab_01"),
				player.PET_WINDOW_ATTR_CHANGE			: self.GetChild("Tab_02"),
				player.PET_WINDOW_PRIMIUM_FEEDSTUFF		: self.GetChild("Tab_03"),
			}

			self.tabButtonDict = {
				player.PET_WINDOW_INFO					: self.GetChild("Tab_Button_01"),
				player.PET_WINDOW_ATTR_CHANGE			: self.GetChild("Tab_Button_02"),
				player.PET_WINDOW_PRIMIUM_FEEDSTUFF		: self.GetChild("Tab_Button_03"),
			}

			self.pageDict = {
				player.PET_WINDOW_INFO					: self.GetChild("PetInfo_Page"),
				player.PET_WINDOW_ATTR_CHANGE			: self.GetChild("PetAttrChange_Page"),
				player.PET_WINDOW_PRIMIUM_FEEDSTUFF		: self.GetChild("PetPrimiumFeefstuff_Page"),
			}

			for (tabKey, tabButton) in self.tabButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)

			# UpBringing Pet Slot
			wndUpBringingPetSlot = self.GetChild("UpBringing_Pet_Slot")
			wndUpBringingPetSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)

			self.wndUpBringingPetSlot = wndUpBringingPetSlot

			feedLifeTimeButton = self.GetChild("FeedLifeTimeButton")
			if feedLifeTimeButton:
				feedLifeTimeButton.SetToggleDownEvent(ui.__mem_func__(self.ClickFeedLifeTimeButtonDown))
				feedLifeTimeButton.SetToggleUpEvent(ui.__mem_func__(self.ClickFeedLifeTimeButtonUp))
			self.feedButton.append(feedLifeTimeButton)

			# Feed Evolution Button
			feedEvolButton = self.GetChild("FeedEvolButton")
			if feedEvolButton:
				feedEvolButton.SetToggleDownEvent(ui.__mem_func__(self.ClickFeedEvolButtonDown))
				feedEvolButton.SetToggleUpEvent(ui.__mem_func__(self.ClickFeedEvolButtonUp))
			self.feedButton.append(feedEvolButton)

			# Feed EXP Button
			feedExpButton = self.GetChild("FeedExpButton")
			if feedExpButton:
				feedExpButton.SetToggleDownEvent(ui.__mem_func__(self.ClickFeedExpButtonDown))
				feedExpButton.SetToggleUpEvent(ui.__mem_func__(self.ClickFeedExpButtonUp))
			self.feedButton.append(feedExpButton)

			for value in range(player.FEED_BUTTON_MAX):
				self.feedButton[value].DisableFlash()

			##		Life Time Gauge
			self.lifeTimeGauge = self.GetChild("LifeGauge")
			self.lifeTimeGauge.SetScale(1.0, 1.0)
			self.lifeTimeGauge.SetWindowHorizontalAlignLeft()

			##		EXP GAUGE
			expGauge = []
			self.expGaugeBoard = self.GetChild("UpBringing_Pet_EXP_Gauge_Board")
			expGauge.append(self.GetChild("UpBringing_Pet_EXPGauge_01"))
			expGauge.append(self.GetChild("UpBringing_Pet_EXPGauge_02"))
			expGauge.append(self.GetChild("UpBringing_Pet_EXPGauge_03"))
			expGauge.append(self.GetChild("UpBringing_Pet_EXPGauge_04"))
			expGauge.append(self.GetChild("UpBringing_Pet_EXPGauge_05"))

			for exp in expGauge:
				exp.SetSize(0, 0)

			self.expGauge	= expGauge
			self.tooltipEXP = TextToolTip()
			self.tooltipEXP.Hide()

			for value in range(SKILL_SLOT_MAX):
				self.skillSlot.append(self.GetChild("PetSkillSlot"+str(value)))
				self.skillSlot[value].SetCoverButton(0, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, FALSE, FALSE)
				self.skillSlot[value].SetAlwaysRenderCoverButton(0)
				self.skillSlot[value].AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")

			# Skill slot empty event
			self.skillSlot[0].SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySkillSlot1))
			self.skillSlot[1].SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySkillSlot2))
			self.skillSlot[2].SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySkillSlot3))

			# Skill slot select Item event
			self.skillSlot[0].SetSelectItemSlotEvent(ui.__mem_func__(self.SetSelectItemSlotEvent1))
			self.skillSlot[1].SetSelectItemSlotEvent(ui.__mem_func__(self.SetSelectItemSlotEvent2))
			self.skillSlot[2].SetSelectItemSlotEvent(ui.__mem_func__(self.SetSelectItemSlotEvent3))

			# Skill slot over in event
			self.skillSlot[0].SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot1))
			self.skillSlot[1].SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot2))
			self.skillSlot[2].SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot3))

			# Skill slot over out event
			self.skillSlot[0].SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot1))
			self.skillSlot[1].SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot2))
			self.skillSlot[2].SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot3))

			self.skillSlot[0].SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSkill1SlotButton))
			self.skillSlot[1].SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSkill2SlotButton))
			self.skillSlot[2].SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSkill3SlotButton))

			# Initialize skill delete question dialog
			self.questionSkillDelDlg = uiCommon.QuestionDialog2()
			self.questionSkillDelDlg.SetAcceptEvent(ui.__mem_func__(self.__SkillDeleteQuestionDialogAccept))
			self.questionSkillDelDlg.SetCancelEvent(ui.__mem_func__(self.__SkillDeleteQuestionDialogCancel))
			self.questionSkillDelDlg.Close()

			# Initialize skill learn question dialog
			self.questionDialog1 = uiCommon.QuestionDialog("thin")
			self.questionDialog1.SetAcceptEvent(ui.__mem_func__(self.__SkillLearnQuestionDialogAccept))
			self.questionDialog1.SetCancelEvent(ui.__mem_func__(self.__SkillLearnQuestionDialogCancel))
			self.questionDialog1.Close()

			# Initialize skill upgrade question dialog
			self.questionDialog2 = uiCommon.QuestionDialog2()
			self.questionDialog2.SetText1(localeInfo.PET_SKILL_UPGRADE_QUESTION_DLG_MSG1)
			self.questionDialog2.SetText2("")
			self.questionDialog2.SetAcceptEvent(ui.__mem_func__(self.__SkillUpgradeQuestionDialogAccept))
			self.questionDialog2.SetCancelEvent(ui.__mem_func__(self.__SkillUpgradeQuestionDialogCancel))
			self.questionDialog2.Close()

			# Determine Button
			self.GetChild("DetermineButton").SetEvent(ui.__mem_func__(self.ClickDetermineButton))

			# Attr Change Slot
			attrChangeSlot = self.GetChild("Change_Pet_Slot")
			attrChangeSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyAttrChangeSlot))
			attrChangeSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectAttrChangeItemSlot))
			attrChangeSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInAttrChangeItem))
			attrChangeSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutAttrChangeItem))
			self.attrChangeSlot = attrChangeSlot

			# Attr Change Button
			self.attrChangeButton = self.GetChild("Pet_Change_Button")
			self.attrChangeButton.SetEvent(ui.__mem_func__(self.ClickAttrChangeButton))
			self.attrChangeButton.Hide()

			# Attr Change Close Button
			self.attrCloseButton = self.GetChild("Pet_OK_Button")
			self.attrCloseButton.SetEvent(ui.__mem_func__(self.ClickAttrClearButton))

			# Attr Change Type Text
			self.attrChangeText = self.GetChild("PetDetermineInfoText")

			# Revive description window
			self.reviveDescWindow = self.GetChild("desc_window")

			# Revive description box
			self.descriptionBox = self.DescriptionBox()
			self.descriptionBox.Show()
			self.descriptionBox.SetParent(self.reviveDescWindow)

			# Revive description prev/next buttons
			self.petReviveDescPrevButton = self.GetChild("PetReviveDescPrevButton")
			self.petReviveDescNextButton = self.GetChild("PetReviveDescNextButton")
			self.petReviveDescPrevButton.SetEvent(ui.__mem_func__(self.PetPremiumFeedStuffPrevDescriptionPage))
			self.petReviveDescNextButton.SetEvent(ui.__mem_func__(self.PetPremiumFeedStuffNextDescriptionPage))

			# Revive UpBringing Pet Slot
			petReviveSlot = self.GetChild("PetReviveSlot")
			petReviveSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectPetReviveEmptySlot))
			petReviveSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectPetReviveItemSlot))
			petReviveSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInPetReviveSlot))
			petReviveSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutPetReviveSlot))
			self.petReviveSlot = petReviveSlot

			# Revived UpBringing Pet Slot
			petReviveResultSlot = self.GetChild("PetReviveResultSlot")
			petReviveResultSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInPetReviveResultSlot))
			petReviveResultSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutPetReviveResultSlot))
			self.petReviveResultSlot = petReviveResultSlot

			# Revive Material Slot Grid
			petReviveMaterialSlot = self.GetChild("PetReviveMaterialSlot")
			petReviveMaterialSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectPetReviveMaterialEmptySlot))
			petReviveMaterialSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectPetReviveMaterialItemSlot))
			self.petReviveMaterialSlot = petReviveMaterialSlot

			# Revive Age/Material Count Texts 
			self.petReviveAgeText = self.GetChild("PetReviveAgeNumberText")
			self.petReviveResultAgeText = self.GetChild("PetReviveResultAgeNumerText")
			self.petReviveItemsCountText = self.GetChild("PetReviveMaterialCountText")

			# Revive Button
			self.petReviveButton = self.GetChild("PetReviveButton")
			self.petReviveButton.SetEvent(ui.__mem_func__(self.ClickPetReviveButton))

			# Revive Cancel Button
			self.petReviveCancelButton = self.GetChild("PetReviveCancelButton")
			self.petReviveCancelButton.SetEvent(ui.__mem_func__(self.ClickPetReviveCancelButton))

			# Initialize question dialog
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.Close()

			# Initialize info page and revive description
			self.SetState(player.PET_WINDOW_INFO)
			self.__DescPetPremiumFeedstuff()
		except:
			import exception
			exception.Abort("petInformationWindow.LoadWindow.BindObject")

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def Destroy(self):
		self.isLoaded = 0

		if self.wndPetFeed:
			self.wndPetFeed.Destroy()
			self.wndPetFeed = None

		self.interface		= None
		self.inven			= None
		self.tooltipItem	= None

		self.ClearDictionary()
		self.wndUpBringingPetSlot = None

		self.lifeTimeGauge	= None
		self.expGauge		= None
		self.expGaugeBoard	= None
		self.tooltipEXP		= None

		if self.wndPetHatching:
			self.wndPetHatching.Destroy()
			self.wndPetHatching = None

		if self.wndPetNameChange:
			self.wndPetNameChange.Destroy()
			self.wndPetNameChange = None

		if self.wndPetMiniInfo:
			self.wndPetMiniInfo.Destroy()
			self.wndPetMiniInfo = None

		if self.skillSlot:
			del self.skillSlot[:]

		if self.feedButton:
			del self.feedButton[:]

		self.feedIndex				= player.FEED_BUTTON_MAX
		self.SkillBookSlotIndex  = -1
		self.SkillBookInvenIndex = -1

		SkillBookDelSlotIndex	= -1
		SkillBookDelInvenIndex	= -1

		self.skillUpgradeGold	 = 0
		self.skillUpgradeSlot	 = -1
		self.skillUpgradeIndex	 = -1
		self.attrChangeIndex	 = None
		self.petReviveIndex		 = -1
		self.petReviveItems	= None
		self.petReviveItemsCount = None
		self.isPetReviveResultTooltip	= FALSE

		self.AffectShower		= None
		self.tooptipPetSkill	= None

		if self.questionDialog1:
			self.questionDialog1.Destroy()

		if self.questionDialog2:
			self.questionDialog2.Destroy()

		if self.questionSkillDelDlg:
			self.questionSkillDelDlg.Destroy()

		if self.popupDialog:
			self.popupDialog.Destroy()

		if self.questionDialog:
			self.questionDialog.Destroy()

		self.questionDialog1		= None
		self.questionDialog2		= None
		self.questionSkillDelDlg	= None
		self.popupDialog			= None
		self.questionDialog		= None

		self.tabDict		= None
		self.tabButtonDict	= None
		self.pageDict		= None

		self.descIndex = 0
		self.desc_Y = DEFAULT_DESC_Y

	def Close(self):
		if self.tooltipEXP:
			self.tooltipEXP.Hide()

		if self.wndPetFeed:
			self.wndPetFeed.Close()

		self.PetFeedToggleButtonUpAll()

		self.__ClearPetSkillSlot()

		self.__ClearSkillBookLearnEvent()
		self.__ClearSkillUpgradeEvent()
		self.__ClearSkillDeleteBookEvent()

		self.__ClearAttrChangeWindow()
		self.__ClearPetReviveWindow()

		if self.popupDialog:
			self.popupDialog.Close()

		if self.questionDialog:
			self.questionDialog.Close()
		self.Hide()

		net.SendPetWindowType(player.PET_WINDOW_INFO)

	def __OnClickTabButton(self, stateKey):
		if stateKey == self.state:
			return

		# Update server with current opened window
		if stateKey == player.PET_WINDOW_INFO:
			net.SendPetWindowType(player.PET_WINDOW_INFO)

		elif stateKey == player.PET_WINDOW_ATTR_CHANGE:
			net.SendPetWindowType(player.PET_WINDOW_ATTR_CHANGE)
	
		elif stateKey == player.PET_WINDOW_PRIMIUM_FEEDSTUFF:
			net.SendPetWindowType(player.PET_WINDOW_PRIMIUM_FEEDSTUFF)

	def PetWindowTypeResult(self, result):
		if result == player.PET_WINDOW_INFO:
			self.SetState(player.PET_WINDOW_INFO)

		elif result == player.PET_WINDOW_ATTR_CHANGE:
			self.SetState(player.PET_WINDOW_ATTR_CHANGE)

		elif result == player.PET_WINDOW_PRIMIUM_FEEDSTUFF:
			self.SetState(player.PET_WINDOW_PRIMIUM_FEEDSTUFF)

	def SetState(self, stateKey):
		self.state = stateKey

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()

		if self.interface and self.IsShow():
			if stateKey == player.PET_WINDOW_INFO:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			elif stateKey == player.PET_WINDOW_ATTR_CHANGE:
				if self.wndPetFeed.IsShow():
					self.wndPetFeed.Close()

				self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_ATTR_CHANGE)
				self.interface.RefreshMarkInventoryBag()

			elif stateKey == player.PET_WINDOW_PRIMIUM_FEEDSTUFF:
				if self.wndPetFeed.IsShow():
					self.wndPetFeed.Close()

				self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF)
				self.interface.RefreshMarkInventoryBag()

			# Clear attr change/revive windows
			self.__ClearAttrChangeWindow()
			self.__ClearPetReviveWindow()

	def OnTop(self):
		if self.state == player.PET_WINDOW_INFO:
			self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			self.interface.RefreshMarkInventoryBag()
		elif self.state == player.PET_WINDOW_ATTR_CHANGE:
			self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_ATTR_CHANGE)
			self.interface.RefreshMarkInventoryBag()
		elif self.state == player.PET_WINDOW_PRIMIUM_FEEDSTUFF:
			self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF)
			self.interface.RefreshMarkInventoryBag()

	def GetState(self):
		return self.state

	def __ClearPetSkillSlot(self):
		for value in range(SKILL_SLOT_MAX):
			self.skillSlot[value].ClearSlot(0)
			self.skillSlot[value].SetCoverButton(0, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, SKILL_SLOT_ENABLE, FALSE, FALSE)
			self.skillSlot[value].SetAlwaysRenderCoverButton(0)

	# Skill upgrade button events
	def OnPressedSkill1SlotButton(self, slotIndex):
		self.OnPressedSkillSlotButton(0, slotIndex)

	def OnPressedSkill2SlotButton(self, slotIndex):
		self.OnPressedSkillSlotButton(1, slotIndex)

	def OnPressedSkill3SlotButton(self, slotIndex):
		self.OnPressedSkillSlotButton(2, slotIndex)

	# Open skill upgrade question dialog, triggered from OnPressedSkillXSlotButton
	def OnPressedSkillSlotButton(self, slotPos, slotIndex):
		self.OpenPetSkillUpGradeQuestionDialog(slotPos, slotIndex)

	def __MakePopupDialog(self):
		self.popupDialog = uiCommon.PopupDialog()
		self.popupDialog.SetText("")

	def __OpenPopupDialog(self, str):
		if self.interface.IsShowDlgQuestionWindow():
			self.interface.CloseDlgQuestionWindow()

		if not self.popupDialog:
			self.__MakePopupDialog()

		self.popupDialog.SetText(str)
		self.popupDialog.SetTop()
		self.popupDialog.Open()

	def OpenPetSkillUpGradeQuestionDialog(self, slot, index):
		if not self.questionDialog2:
			self.questionDialog2 = uiCommon.QuestionDialog2()
			self.questionDialog2.SetText1(localeInfo.PET_SKILL_UPGRADE_QUESTION_DLG_MSG1)
			self.questionDialog2.SetAcceptEvent(ui.__mem_func__(self.__SkillUpgradeQuestionDialogAccept))
			self.questionDialog2.SetCancelEvent(ui.__mem_func__(self.__SkillUpgradeQuestionDialogCancel))

		self.skillUpgradeGold	= 2000000
		self.skillUpgradeSlot	= slot
		self.skillUpgradeIndex	= index

		self.questionDialog2.SetText2(localeInfo.PET_SKILL_UPGRADE_QUESTION_DLG_MSG2 % (localeInfo.NumberToMoneyString(self.skillUpgradeGold)))
		self.questionDialog2.SetTop()
		self.questionDialog2.Open()

	def __SkillUpgradeQuestionDialogAccept(self):

		slot  = self.skillUpgradeSlot
		gold  = self.skillUpgradeGold
		index = self.skillUpgradeIndex

		self.__ClearSkillUpgradeEvent()

		# Check if player has enough money to upgrade skill
		if player.GetMoney() < gold:
			self.__OpenPopupDialog(localeInfo.PET_MSG_NOT_ENOUGH_MONEY)
			return

		# Send skill upgrade request packet
		net.SendPetSkillUpgradeRequest(slot, index)

	def __SkillUpgradeQuestionDialogCancel(self):
		self.__ClearSkillUpgradeEvent()

	def __ClearSkillUpgradeEvent(self):
		if self.questionDialog2:
			self.questionDialog2.Close()

		self.skillUpgradeGold	= 0
		self.skillUpgradeSlot	= -1
		self.skillUpgradeIndex	= -1

	# Skill slot select events
	def SetSelectItemSlotEvent1(self, slotIndex):
		self.SetSelectItemSlotEvent(0)

	def SetSelectItemSlotEvent2(self, slotIndex):
		self.SetSelectItemSlotEvent(1)

	def SetSelectItemSlotEvent3(self, slotIndex):
		self.SetSelectItemSlotEvent(2)

	# Delete skill dialog, triggered from SetSelectItemSlotEventX
	def SetSelectItemSlotEvent(self, skillSlotIndex):
		# Return if there is no pet summoned	
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			return

		if not mouseModule.mouseController.isAttached():
			return

		# Return if skill slot index is not between 0~2
		if skillSlotIndex < 0 or skillSlotIndex >= SKILL_SLOT_MAX:
			return

		# Skill data
		(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)

		if 0 == skill_count:
			return

		if skillSlotIndex >= skill_count:
			return

		if 0 == skillSlotIndex:
			if not pet_skill1:
				return
		elif 1 == skillSlotIndex:
			if not pet_skill2:
				return
		elif 2 == skillSlotIndex:
			if not pet_skill3:
				return

		attachedSlotType	= mouseModule.mouseController.GetAttachedType()
		attachedSlotPos		= mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount	= mouseModule.mouseController.GetAttachedItemCount()
		attachedItemVNum	= player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType	= item.GetItemType()
		itemSubType = item.GetItemSubType()

		# Return if item is not from inventory
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT: 
			return

		# Return if it is not a skill delete item
		if item.ITEM_TYPE_GROWTH_PET != itemType or item.PET_SKILL_DEL_BOOK != itemSubType:
			return

		# Initialize skill delete question dialog if it hasn't been yet
		if not self.questionSkillDelDlg:
			self.questionSkillDelDlg = uiCommon.QuestionDialog2()
			self.questionSkillDelDlg.SetAcceptEvent(ui.__mem_func__(self.__SkillDeleteQuestionDialogAccept))
			self.questionSkillDelDlg.SetCancelEvent(ui.__mem_func__(self.__SkillDeleteQuestionDialogCancel))

		self.questionSkillDelDlg.SetText1(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG1)
		self.questionSkillDelDlg.SetText2(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG2)
		(w,h) = self.questionSkillDelDlg.GetTextSize1()
		self.questionSkillDelDlg.SetWidth(w+100)

		mouseModule.mouseController.DeattachObject()
		self.SkillBookDelSlotIndex  = skillSlotIndex
		self.SkillBookDelInvenIndex = attachedSlotPos
		self.questionSkillDelDlg.SetTop()
		self.questionSkillDelDlg.Open()

	def __SkillDeleteQuestionDialogAccept(self):
		pet_id = player.GetActivePetItemId()
		if pet_id:
			net.SendPetDeleteSkill(self.SkillBookDelSlotIndex, self.SkillBookDelInvenIndex)	

		self.__ClearSkillDeleteBookEvent()
		return

	def __SkillDeleteQuestionDialogCancel(self):
		self.__ClearSkillDeleteBookEvent()
		return

	def __ClearSkillDeleteBookEvent(self):
		# Lock skill delete item
		self.CanInvenSlot(self.SkillBookDelInvenIndex)

		self.SkillBookDelSlotIndex  = -1
		self.SkillBookDelInvenIndex = -1

		if self.questionSkillDelDlg:
			self.questionSkillDelDlg.Close()

	# Skill slot OverIn events
	def OverInSkillSlot1(self, slotIndex):
		self.OverInPetSkillSlot(0, slotIndex)

	def OverInSkillSlot2(self, slotIndex):
		self.OverInPetSkillSlot(1, slotIndex)

	def OverInSkillSlot3(self, slotIndex):
		self.OverInPetSkillSlot(2, slotIndex)

	# Show pet tooltip
	def OverInPetSkillSlot(self, slot, index):
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			return

		if self.tooptipPetSkill:
			self.tooptipPetSkill.SetPetSkill(pet_id, slot, index)

	# Skill slot OverOut events
	def OverOutSkillSlot1(self):
		self.tooptipPetSkill.HideToolTip()

	def OverOutSkillSlot2(self):
		self.tooptipPetSkill.HideToolTip()

	def OverOutSkillSlot3(self):
		self.tooptipPetSkill.HideToolTip()

	# Skill empty slot events
	def SelectEmptySkillSlot1(self, slotIndex):
		self.SelectEmptySkillSlot(0)

	def SelectEmptySkillSlot2(self, slotIndex):
		self.SelectEmptySkillSlot(1)

	def SelectEmptySkillSlot3(self, slotIndex):
		self.SelectEmptySkillSlot(2)

	# Triggered from SelectEmptySkillSlotX, opens learn skill dialog
	def SelectEmptySkillSlot(self, skillSlotIndex):
		# Return if there is no summoned pet		
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			return

		if not mouseModule.mouseController.isAttached():
			return

		if skillSlotIndex < 0 or skillSlotIndex >= SKILL_SLOT_MAX:
			return

		# Pet data
		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)

		# Skill data
		(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)

		attachedSlotType	= mouseModule.mouseController.GetAttachedType()
		attachedSlotPos		= mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount	= mouseModule.mouseController.GetAttachedItemCount()
		attachedItemVNum	= player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType	= item.GetItemType()
		itemSubType = item.GetItemSubType()

		# Return if item is not from inventory
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT: 
			return

		# Check if item is item.ITEM_TYPE_PET
		if item.ITEM_TYPE_GROWTH_PET != itemType:
			return

		# Return if skill delete item is dragged to an empty slot
		if item.PET_SKILL_DEL_BOOK == itemSubType:
			self.__OpenPopupDialog(localeInfo.PET_EMPTY_SKILL_SLOT_USE_ITEM)
			return

		# Return if it is not item.PET_SKILL subtype
		if item.PET_SKILL != itemSubType:
			return

		# Pet has no unlocked skills, return
		if 0 == skill_count:
			return

		if skillSlotIndex >= skill_count:
			return

		if 0 == skillSlotIndex:
			if pet_skill1:
				return
		elif 1 == skillSlotIndex:
			if pet_skill2:
				return
		elif 2 == skillSlotIndex:
			if pet_skill3:
				return

		# Return if pet's evolution is not player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL
		if evol_level < player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL:
			return

		if not self.questionDialog1:
			self.questionDialog1 = uiCommon.QuestionDialog("thin")
			self.questionDialog1.SetAcceptEvent(ui.__mem_func__(self.__SkillLearnQuestionDialogAccept))
			self.questionDialog1.SetCancelEvent(ui.__mem_func__(self.__SkillLearnQuestionDialogCancel))

		self.questionDialog1.SetText(localeInfo.PET_SKILL_LEARN_QUESTION_DLG_MSG % (item.GetItemName()))
		mouseModule.mouseController.DeattachObject()
		self.SkillBookSlotIndex  = skillSlotIndex
		self.SkillBookInvenIndex = attachedSlotPos
		self.questionDialog1.SetTop()
		self.questionDialog1.Open()

	def __SkillLearnQuestionDialogAccept(self):
		pet_id = player.GetActivePetItemId()
		if pet_id:
			net.SendPetLearnSkill(self.SkillBookSlotIndex, self.SkillBookInvenIndex)	

		self.__ClearSkillBookLearnEvent()
		return

	def __SkillLearnQuestionDialogCancel(self):
		self.__ClearSkillBookLearnEvent()
		return

	def __ClearSkillBookLearnEvent(self):
		self.CanInvenSlot(self.SkillBookInvenIndex)

		self.SkillBookSlotIndex = -1
		self.SkillBookInvenIndex = -1

		if self.questionDialog1:
			self.questionDialog1.Close()

	def PetFeedToggleButtonUpAll(self, exclusion_index = player.FEED_BUTTON_MAX):
		for value in range(player.FEED_BUTTON_MAX):
			if exclusion_index == value:
				continue
			self.feedButton[value].SetUp()

	def ClickFeedLifeTimeButtonDown(self):
		self.ClickPetFeedButton(player.FEED_LIFE_TIME_EVENT)

	def ClickFeedLifeTimeButtonUp(self):
		if self.feedIndex == player.FEED_LIFE_TIME_EVENT:
			self.feedIndex = player.FEED_BUTTON_MAX
			self.wndPetFeed.Close()

	def ClickFeedEvolButtonDown(self):
		self.ClickPetFeedButton(player.FEED_EVOL_EVENT)

	def ClickFeedEvolButtonUp(self):
		if self.feedIndex == player.FEED_EVOL_EVENT:
			self.feedIndex = player.FEED_BUTTON_MAX
			self.wndPetFeed.Close()

	def ClickFeedExpButtonDown(self):
		self.ClickPetFeedButton(player.FEED_EXP_EVENT)

	def ClickFeedExpButtonUp(self):
		if self.feedIndex == player.FEED_EXP_EVENT:
			self.feedIndex = player.FEED_BUTTON_MAX
			self.wndPetFeed.Close()

	def IsActivateEvolButton(self, pet_id):
		# Return if there is no summoned pet
		if 0 == pet_id:
			return FALSE

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)

		if evol_level == player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_EVOL_BUTTON_LEVEL_MAX)
			return FALSE

		evol_require = self.GetEvolInfo(evol_level)
		if 0 == evol_require:
			return FALSE

		# Check level and experience limit for the first two evolutions
		if evol_level < player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL-1:
			# Level Check
			if pet_level < evol_require:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_EVOL_BUTTON % evol_require)
				return FALSE
			else:
				# EXP Check
				(curEXP, nextEXP, itemEXP, itemMaxEXP) = player.GetPetExpPoints(pet_id)

				if curEXP != nextEXP or itemEXP != itemMaxEXP:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_EVOL_BUTTON_EXP_LACK)
					return FALSE

		# Check level and age limit for the last evolution
		elif evol_level == player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL - 1:
			# Level Check
			if pet_level < evol_require:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_EVOL_BUTTON % evol_require)
				return FALSE
			else:
				# Age Check
				curTime = app.GetGlobalTimeStamp()
				birthSec = max(0, curTime - birthday)

				if birthSec < player.SPECIAL_EVOL_MIN_AGE:
					day = localeInfo.SecondToDayNumber(player.SPECIAL_EVOL_MIN_AGE)
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_SPECIAL_EVOL_BUTTON % day)
					return FALSE

		return TRUE

	def ClickPetFeedButton(self, index):
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			self.PetFeedToggleButtonUpAll()
			return

		if player.FEED_EVOL_EVENT == index:
			if FALSE == self.IsActivateEvolButton(pet_id):
				self.PetFeedToggleButtonUpAll(self.feedIndex)
				return

		if not self.wndPetFeed:
			self.wndPetFeed = PetFeedWindow(self)

		self.feedIndex = index
		self.wndPetFeed.ClearFeedItems()
		self.wndPetFeed.Show()
		self.wndPetFeed.SetTop()

		self.PetFeedToggleButtonUpAll(self.feedIndex)

	def OnUpdate(self):
		self.RefreshStatus()

		# Lock used items 
		self.CantInvenSlot(self.SkillBookInvenIndex)
		self.CantInvenSlot(self.SkillBookDelInvenIndex)
		self.CantInvenSlot(self.petReviveIndex)

		for i in xrange(player.PET_WND_SLOT_ATTR_CHANGE_MAX):
			self.CantInvenSlot(self.attrChangeIndex[i])

		for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
			self.CantInvenSlot(self.petReviveItems[i])

		if self.expGaugeBoard.IsIn():
			self.tooltipEXP.Show()
		else:
			self.tooltipEXP.Hide()

		(xposEventSet, yposEventSet) = self.reviveDescWindow.GetGlobalPosition()
		event.UpdateEventSet(self.descIndex, xposEventSet+7, -(yposEventSet+self.desc_Y))
		self.descriptionBox.SetIndex(self.descIndex)
		self.descriptionBox.SetTop()

	def RefreshStatus(self):
		if self.isLoaded==0:
			return

		# If there is no summoned pet, close pet mini window and clear the main window
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			self.ClearStatus()
			if self.wndPetMiniInfo:
				self.wndPetMiniInfo.Close()
			return

		if not self.wndPetMiniInfo:
			return

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)
		curTime = app.GetGlobalTimeStamp()

		# UpBringing Pet Slot Image
		CurPetItemVNum = player.GetActivePetItemVNum()
		self.wndUpBringingPetSlot.SetItemSlot(0, CurPetItemVNum)
		self.wndPetMiniInfo.SetItemSlot(CurPetItemVNum)

		# Pet Name
		self.GetChild("PetName").SetText(pet_nick);

		# Evolution Name
		self.GetChild("EvolName").SetText(self.__GetEvolName(evol_level))

		# Level
		self.GetChild("LevelValue").SetText(str(pet_level))

		# Age
		birthSec = max(0, curTime - birthday)
		self.GetChild("AgeValue").SetText(localeInfo.SecondToDay(birthSec))

		# Life Time Text
		(endTime, maxTime) = player.GetPetLifeTime(pet_id)
		lifeTime = max(0, endTime - curTime)
		self.GetChild("LifeTextValue").SetText( localeInfo.SecondToH(lifeTime) + " / " + localeInfo.SecondToH(maxTime) + " " +uiScriptLocale.PET_INFORMATION_LIFE_TIME)

		# Life Time Gauge
		self.SetLifeTime(lifeTime, maxTime)
		self.wndPetMiniInfo.SetLifeTime(lifeTime, maxTime)

		# HP, Def, SP Bonus Text
		self.GetChild("HpValue").SetText("+" + str("%0.1f" % pet_hp) + "%")
		self.GetChild("DefValue").SetText("+" + str("%0.1f" % pet_def) + "%")
		self.GetChild("SpValue").SetText("+" + str("%0.1f" % pet_sp) + "%")

		# EXP
		(curEXP, nextEXP, itemEXP, itemMaxEXP) = player.GetPetExpPoints(pet_id)

		curEXP		= unsigned32(curEXP)
		nextEXP		= unsigned32(nextEXP)
		itemEXP		= unsigned32(itemEXP)
		itemMaxEXP	= unsigned32(itemMaxEXP)
		self.SetExperience(curEXP, nextEXP, itemEXP, itemMaxEXP)
		self.wndPetMiniInfo.SetExperience(curEXP, nextEXP, itemEXP, itemMaxEXP)

		# Clear skill slots
		self.__ClearPetSkillSlot()

		# Clear skill slots on mini window
		self.wndPetMiniInfo.ClearSkillSlot()

		bMiniWindowFlash = FALSE

		if self.PetEvolFlashEventCheck(pet_level, evol_level, birthday, curEXP, nextEXP, itemEXP, itemMaxEXP):
			bMiniWindowFlash = TRUE

		if self.PetLifeTimeFlashEventCheck(lifeTime):
			bMiniWindowFlash = TRUE

		if self.PetItemExpFlashEventCheck(curEXP, nextEXP, itemEXP, itemMaxEXP):
			bMiniWindowFlash = TRUE

		if not bMiniWindowFlash:
			self.wndPetMiniInfo.OffFlashEvent()
		else:
			self.wndPetMiniInfo.OnFlashEvent()

		(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)

		if skill_count:
			for value in range(skill_count):
				self.skillSlot[value].SetAlwaysRenderCoverButton(0, FALSE)
				self.wndPetMiniInfo.SetAlwaysRenderCoverButton(value)

		if pet_skill1:
			self.skillSlot[0].SetPetSkillSlotNew(0, pet_skill1)
			self.skillSlot[0].SetSlotCount(0, pet_skill_level1)
			self.skillSlot[0].SetCoverButton(0)
			self.wndPetMiniInfo.SetSkillSlot(0, 0, pet_skill1)

			if player.PET_GROWTH_SKILL_LEVEL_MAX > pet_skill_level1:
				self.skillSlot[0].ShowSlotButton(0)

			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill1)
			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool1:
					curCoolTime = pet_skill_cool1 - curTime
					curCoolTime = pet_skill_cool_time - curCoolTime
					self.skillSlot[0].SetSlotCoolTime(0, pet_skill_cool_time, curCoolTime)
					self.wndPetMiniInfo.SetSkillCoolTime(0, 0, pet_skill_cool_time, curCoolTime)
				else:
					if self.AffectShower:
						self.AffectShower.SetPetSkillAffect(1, pet_skill1)
			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				if self.AffectShower:
					self.AffectShower.SetPetSkillAffect(1, pet_skill1)

		if pet_skill2:
			self.skillSlot[1].SetPetSkillSlotNew(0, pet_skill2)
			self.skillSlot[1].SetSlotCount(0, pet_skill_level2)
			self.skillSlot[1].SetCoverButton(0)
			self.wndPetMiniInfo.SetSkillSlot(1, 0, pet_skill2)

			if player.PET_GROWTH_SKILL_LEVEL_MAX > pet_skill_level2:
				self.skillSlot[1].ShowSlotButton(0)

			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill2)
			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool2:
					curCoolTime = pet_skill_cool2 - curTime
					curCoolTime = pet_skill_cool_time - curCoolTime
					self.skillSlot[1].SetSlotCoolTime(0, pet_skill_cool_time, curCoolTime)
					self.wndPetMiniInfo.SetSkillCoolTime(1, 0, pet_skill_cool_time, curCoolTime)
				else:
					if self.AffectShower:
						self.AffectShower.SetPetSkillAffect(2, pet_skill2)

			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				if self.AffectShower:
					self.AffectShower.SetPetSkillAffect(2, pet_skill2)

		if pet_skill3:
			self.skillSlot[2].SetPetSkillSlotNew(0, pet_skill3)
			self.skillSlot[2].SetSlotCount(0, pet_skill_level3)
			self.skillSlot[2].SetCoverButton(0)
			self.wndPetMiniInfo.SetSkillSlot(2, 0, pet_skill3)

			if player.PET_GROWTH_SKILL_LEVEL_MAX > pet_skill_level3:
				self.skillSlot[2].ShowSlotButton(0)

			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill3)
			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool3:
					curCoolTime = pet_skill_cool3 - curTime
					curCoolTime = pet_skill_cool_time - curCoolTime
					self.skillSlot[2].SetSlotCoolTime(0, pet_skill_cool_time, curCoolTime)
					self.wndPetMiniInfo.SetSkillCoolTime(2, 0, pet_skill_cool_time, curCoolTime)
				else:
					if self.AffectShower:
						self.AffectShower.SetPetSkillAffect(3, pet_skill3)

			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				if self.AffectShower:
					self.AffectShower.SetPetSkillAffect(3, pet_skill3)

		self.wndPetMiniInfo.Show()

	def ClearStatus(self):
		self.wndUpBringingPetSlot.SetItemSlot(0, 0)
		self.GetChild("PetName").SetText("")
		self.GetChild("EvolName").SetText("")
		self.GetChild("LevelValue").SetText("")
		self.GetChild("AgeValue").SetText("")
		self.GetChild("LifeTextValue").SetText("")
		self.GetChild("DefValue").SetText("")
		self.GetChild("SpValue").SetText("")
		self.GetChild("HpValue").SetText("")
		self.SetExperience(0, 0, 0, 0)
		self.SetLifeTime(100, 100)
		self.__ClearPetSkillSlot()	##Skill clear

		if self.wndPetFeed:
			if self.wndPetFeed.IsShow():
				self.wndPetFeed.Close()

		self.__ClearSkillBookLearnEvent()
		self.__ClearSkillDeleteBookEvent()
		self.__ClearSkillUpgradeEvent()

		if self.AffectShower:
			self.AffectShower.ClearPetSkillAffect()

		self.AllOffPetInfoFlashEvent()

	# Lock item at position invenIndex
	def CantInvenSlot(self, invenIndex):
		if invenIndex == -1:
			return

		inven = self.inven
		invenPage = inven.GetInventoryPageIndex()

		min_range = invenPage * player.INVENTORY_PAGE_SIZE
		max_range = (invenPage + 1) * player.INVENTORY_PAGE_SIZE

		inven_slot_pos = invenIndex

		if min_range <= inven_slot_pos < max_range:
			inven_slot_pos = inven_slot_pos - min_range
			inven.wndItem.SetCantMouseEventSlot(inven_slot_pos)

	# Unlock item at position invenIndex
	def CanInvenSlot(self, invenIndex):
		if invenIndex == -1:
			return

		inven = self.inven
		invenPage = inven.GetInventoryPageIndex()

		min_range = invenPage * player.INVENTORY_PAGE_SIZE
		max_range = (invenPage + 1) * player.INVENTORY_PAGE_SIZE

		inven_slot_pos = invenIndex

		if min_range <= inven_slot_pos < max_range:
			inven_slot_pos = inven_slot_pos - min_range
			inven.wndItem.SetCanMouseEventSlot(inven_slot_pos)

	def __GetEvolName(self, evol_level):
		if 1 == evol_level:
			return localeInfo.PET_INFORMATION_STAGE1
		elif 2 == evol_level:
			return localeInfo.PET_INFORMATION_STAGE2
		elif 3 == evol_level:
			return localeInfo.PET_INFORMATION_STAGE3
		elif 4 == evol_level:
			return localeInfo.PET_INFORMATION_STAGE4

		return localeInfo.PET_INFORMATION_STAGE1

	def PetAffectShowerRefresh(self):
		# Return if affectShower class was not bound
		if not self.AffectShower:
			return

		# Clear pet affects
		self.AffectShower.ClearPetSkillAffect()

		# Return if there is no summoned pet
		pet_id = player.GetActivePetItemId()
		if 0 == pet_id:
			return

		curTime = app.GetGlobalTimeStamp()

		# Skill data
		(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)

		if pet_skill1:
			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill1)

			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool1:
					pass
				else:
					self.AffectShower.SetPetSkillAffect(1, pet_skill1)
			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				self.AffectShower.SetPetSkillAffect(1, pet_skill1)

		if pet_skill2:
			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill2)

			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool2:
					pass
				else:
					self.AffectShower.SetPetSkillAffect(2, pet_skill2)
			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				self.AffectShower.SetPetSkillAffect(2, pet_skill2)

		if pet_skill3:
			(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill3)

			if skill.PET_SKILL_USE_TYPE_AUTO == pet_skill_use_type:
				if curTime <= pet_skill_cool3:
					pass
				else:
					self.AffectShower.SetPetSkillAffect(3, pet_skill3)

			elif skill.PET_SKILL_USE_TYPE_PASSIVE == pet_skill_use_type:
				self.AffectShower.SetPetSkillAffect(3, pet_skill3)

	def PetEvolFlashEventCheck(self, pet_level, evol_level, birthday, curEXP, nextEXP, itemEXP, itemMaxEXP):
		if evol_level == player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL:
			self.DisableFlashButtonEvent(player.FEED_EVOL_EVENT)
			return FALSE

		evol_require = self.GetEvolInfo(evol_level)
		if 0 == evol_require:
			self.DisableFlashButtonEvent(player.FEED_EVOL_EVENT)
			return FALSE

		if evol_level < player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL-1:
			if pet_level == evol_require:
				if curEXP == nextEXP and itemEXP == itemMaxEXP:
					self.EnableFlashButtonEvent(player.FEED_EVOL_EVENT)

					return TRUE

		elif evol_level == player.PET_GROWTH_SKILL_OPEN_EVOL_LEVEL - 1:
			if pet_level >= evol_require:

				curTime = app.GetGlobalTimeStamp()
				birthSec = max(0, curTime - birthday)

				if birthSec > player.SPECIAL_EVOL_MIN_AGE:
					self.EnableFlashButtonEvent(player.FEED_EVOL_EVENT)

					return TRUE

		self.DisableFlashButtonEvent(player.FEED_EVOL_EVENT)
		return FALSE

	def PetLifeTimeFlashEventCheck(self, lifeTime):
		if lifeTime < player.LIFE_TIME_FLASH_MIN_TIME:
			self.EnableFlashButtonEvent(player.FEED_LIFE_TIME_EVENT)

			return TRUE

		self.DisableFlashButtonEvent(player.FEED_LIFE_TIME_EVENT)
		return FALSE

	def PetItemExpFlashEventCheck(self, curEXP, nextEXP, itemEXP, itemMaxEXP):
		if (curEXP >= nextEXP) and (itemEXP < itemMaxEXP):
			self.EnableFlashButtonEvent(player.FEED_EXP_EVENT)
			return TRUE

		self.DisableFlashButtonEvent(player.FEED_EXP_EVENT)
		return FALSE

	def AllOffPetInfoFlashEvent(self):
	
		if self.wndPetMiniInfo:
			self.wndPetMiniInfo.OffFlashEvent()

		for i in xrange(player.FEED_BUTTON_MAX):
			self.DisableFlashButtonEvent(i)

	def EnableFlashButtonEvent(self, index):

		if index < 0 or index >= player.FEED_BUTTON_MAX:
			return

		if self.feedButton[index]:
			self.feedButton[index].EnableFlash()

	def DisableFlashButtonEvent(self, index):

		if index < 0 or index >= player.FEED_BUTTON_MAX:
			return

		if self.feedButton[index]:
			self.feedButton[index].DisableFlash()

	def SetExperience(self, curPoint, maxPoint, itemExp, itemExpMax):

		curPoint = min(curPoint, maxPoint)
		curPoint = max(curPoint, 0)
		maxPoint = max(maxPoint, 0)

		itemExp = min(itemExp, itemExpMax)
		itemExp = max(itemExp, 0)
		itemExpMax = max(itemExpMax, 0)

		# Battle exp is divided into BATTLE_EXP_GAUGE_MAX parts
		quarterPoint = maxPoint / BATTLE_EXP_GAUGE_MAX
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(BATTLE_EXP_GAUGE_MAX, curPoint / quarterPoint)

		for i in xrange(TOTAL_EXP_GAUGE_COUNT):
			self.expGauge[i].Hide()

		for i in xrange(FullCount):
			self.expGauge[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.expGauge[i].Show()

		if 0 != quarterPoint:
			if FullCount < BATTLE_EXP_GAUGE_MAX:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.expGauge[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.expGauge[FullCount].Show()

		if 0 != itemExpMax:
			itemExpGauge = self.expGauge[ITEM_EXP_GAUGE_POS]
			Percentage = float(itemExp) / float(itemExpMax) - float(1.0)
			itemExpGauge.SetRenderingRect(0.0, Percentage, 0.0, 0.0)
			itemExpGauge.Show()

		output_cur_exp = curPoint + itemExp
		output_max_exp = maxPoint + itemExpMax

		expPercent = 0
		if output_max_exp:
			expPercent = float(output_cur_exp) / float(output_max_exp) * 100

		# If ENABLE_MULTI_TEXTLINE is active, experience points will be
		# shown seperately based on type.
		if app.ENABLE_MULTI_TEXTLINE:
			tooltip_text = str(localeInfo.PET_INFO_EXP) + ': '+ str(curPoint) + '\n' + str(localeInfo.PET_INFO_NEXT_EXP) + ': ' + str(maxPoint - curPoint) + '\n'	\
						 + str(localeInfo.PET_INFO_ITEM_EXP) + ': '+ str(itemExp) + '\n' + str(localeInfo.PET_INFO_NEXT_ITEM_EXP) + ': ' + str(itemExpMax - itemExp)

			self.tooltipEXP.SetText(tooltip_text)
		else:
			self.tooltipEXP.SetText("%s : %.2f%%" % (localeInfo.TASKBAR_EXP, expPercent))

	def SetLifeTime(self, curPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.lifeTimeGauge.SetPercentageWithScale(curPoint, maxPoint)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetInven(self, inven):
		self.inven = inven

	def IsFeedWindowOpen(self):
		if self.wndPetFeed:
			if self.wndPetFeed.IsShow():
				return TRUE

		return FALSE

	def GetPetHatchingWindow(self):
		return self.wndPetHatching

	def GetPetNameChangeWindow(self):
		return self.wndPetNameChange

	def GetPetFeedWindow(self):
		return self.wndPetFeed

	def CantFeedItem(self, InvenSlot):
		
		if self.feedIndex == player.FEED_LIFE_TIME_EVENT:
			return self.__CantLifeTimeFeedItem(InvenSlot)
			
		elif self.feedIndex == player.FEED_EVOL_EVENT:
			return self.__CantEvolFeedItem(InvenSlot)
			
		elif self.feedIndex == player.FEED_EXP_EVENT:
			return self.__CantExpFeedItem(InvenSlot)

		return FALSE

	def __CantLifeTimeFeedItem(self, InvenSlot):
		ItemVNum = player.GetItemIndex(InvenSlot)

		if ItemVNum == 0:
			return TRUE

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_PETFEED):
			return TRUE

		item.SelectItem(ItemVNum)
		if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
			if item.GetItemSubType() in [item.PET_UPBRINGING, item.PET_EGG]:
				return FALSE

		return TRUE

	def __CantEvolFeedItem(self, InvenSlot):
		ItemVNum = player.GetItemIndex(InvenSlot)

		if ItemVNum == 0:
			return TRUE

		item.SelectItem(ItemVNum)

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_PETFEED):
			return TRUE

	def __CantExpFeedItem(self, InvenSlot):
		ItemVNum = player.GetItemIndex(InvenSlot)

		if ItemVNum == 0:
			return TRUE

		item.SelectItem(ItemVNum)

		if item.IsAntiFlag(item.ITEM_ANTIFLAG_PETFEED):
			return TRUE

		if item.GetItemType() in [item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_GROWTH_PET]:
			if item.ITEM_TYPE_GROWTH_PET == item.GetItemType():
				if item.GetItemSubType() in [item.PET_EXPFOOD, item.PET_EXPFOOD_PER]:
					return FALSE
				else:
					return TRUE
			else:
				return FALSE

		return TRUE

	def CantAttrChangeItem(self, InvenSlot):
		ItemVNum = player.GetItemIndex(InvenSlot)

		if ItemVNum == 0:
			return TRUE

		item.SelectItem(ItemVNum)

		if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
			if item.GetItemSubType() in [item.PET_UPBRINGING, item.PET_ATTR_CHANGE]:
				return FALSE

		return TRUE

	def CantPremiumFeedItem(self, InvenSlot):
		ItemVNum = player.GetItemIndex(InvenSlot)

		if ItemVNum == 0:
			return TRUE

		item.SelectItem(ItemVNum)

		if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
			if item.GetItemSubType() in [item.PET_UPBRINGING, item.PET_PRIMIUM_FEEDSTUFF]:
				return FALSE

		return TRUE

	def PetInfoBindAffectShower(self, affect_shower):
		self.AffectShower = affect_shower

	def SetPetSkillToolTip(self, tooltipPetSkill):
		self.tooptipPetSkill = tooltipPetSkill

	def GetEvolInfo(self, evol_level):
		# Return if evolution level in invalid
		if evol_level < 1 or evol_level >= player.PET_GROWTH_EVOL_MAX:
			return 0

		if 1 == evol_level:
			return 40
		elif 2 == evol_level:
			return 80
		elif 3 == evol_level:
			return 81

		return 0

	def PetFeedReuslt(self, result):
		if not self.wndPetFeed:
			return

		if TRUE == result:
			self.wndPetFeed.BackUpSucceedFeedItems()

			if self.feedIndex == player.FEED_EVOL_EVENT:
				self.wndPetFeed.Close()

		self.wndPetFeed.ClearFeedItems()

	def OverInAttrChangeItem(self, slotIndex):
		if None != self.tooltipItem:
			invenPos = self.attrChangeIndex[slotIndex]
			if invenPos != -1:
				self.tooltipItem.SetInventoryItem(invenPos, player.INVENTORY)

		return

	def OverOutAttrChangeItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		return

	def SelectAttrChangeItemSlot(self, slotIndex):
		self.__ClearAttrChangeWindow()

	def __ClearAttrChangeSlot(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			return

		#Unlock item in slotIndex slot
		self.CanInvenSlot(self.attrChangeIndex[slotIndex])
		self.attrChangeIndex[slotIndex] = -1
		self.attrChangeSlot.ClearSlot(slotIndex)
		self.attrChangeSlot.RefreshSlot()

		if self.attrChangeButton.IsShow():
			self.attrChangeButton.Hide()

	def SelectEmptyAttrChangeSlot(self, slotIndex):
		if not mouseModule.mouseController.isAttached():
			return FALSE

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemVNum = player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		# Return if item is beging attached from not inventory window
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return FALSE

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT: 
			return FALSE

		if slotIndex == player.PET_WND_SLOT_ATTR_CHANGE:

			# Return if upbringing item is already in place
			if -1 != self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE]:
				return FALSE

			if item.ITEM_TYPE_GROWTH_PET != itemType or itemSubType != item.PET_UPBRINGING:
				return FALSE

			# Return if there is an active pet
			activePetId = player.GetActivePetItemId()
			if activePetId != 0:
				return FALSE

			# Select attr change item if it wasn't selected yet
			if -1 == self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_ITEM]:

				attrChangePos = player.GetItemSlotIndex(player.PET_ATTR_CHANGE_ITEM)
				if attrChangePos != -1:
					self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_ITEM] = attrChangePos
					self.attrChangeSlot.SetItemSlot(player.PET_WND_SLOT_ATTR_CHANGE_ITEM, player.PET_ATTR_CHANGE_ITEM)

		elif slotIndex == player.PET_WND_SLOT_ATTR_CHANGE_ITEM:

			# Return if attr change item is already in place
			if -1 != self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_ITEM]:
				return FALSE

			if item.ITEM_TYPE_GROWTH_PET != itemType or itemSubType != item.PET_ATTR_CHANGE:
				return FALSE

		elif slotIndex == player.PET_WND_SLOT_ATTR_CHANGE_RESULT:
			return FALSE

		self.attrChangeIndex[slotIndex] = attachedSlotPos
		self.attrChangeSlot.SetItemSlot(slotIndex, attachedItemVNum)
		self.attrChangeSlot.RefreshSlot()

		if self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_ITEM] != -1 and self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE] != -1:
			self.attrChangeButton.Show()

		mouseModule.mouseController.DeattachObject()
		return TRUE

	def ClickAttrChangeButton(self):
		if self.questionDialog.IsShow():
			return

		self.questionDialog.SetText(localeInfo.PET_ATTR_CHANGE_DIALOG_TEXT)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__PetAttrChangeAccept))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__PetAttrChangeCancel))
		self.questionDialog.Open()

	def ClickAttrClearButton(self):
		self.__ClearAttrChangeWindow()

	def __ClearAttrChangeWindow(self):
		if self.questionDialog.IsShow():
			self.questionDialog.Close()

		self.attrChangeText.SetText("")
		for i in xrange(player.PET_WND_SLOT_ATTR_CHANGE_MAX):
			self.__ClearAttrChangeSlot(i)

	def __PetAttrChangeAccept(self):
		upBringingPos = self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE]
		attrChangePos = self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_ITEM]

		# Send packet if positions are valid and close the question dialog
		if upBringingPos != -1 and attrChangePos != -1:
			net.SendPetAttrChange(upBringingPos, attrChangePos)

		self.questionDialog.Close()

	def __PetAttrChangeCancel(self):
		self.questionDialog.Close()

	def PetAttrChangeResult(self, slotIndex, byType):
		self.attrChangeButton.Hide()

		text = self.typeInfo[byType]
		self.attrChangeText.SetText(text)

		# Clear slots 
		for i in xrange(player.PET_WND_SLOT_ATTR_CHANGE_MAX):
			self.__ClearAttrChangeSlot(i)

		# Append new upbringing pet item in result slot
		itemVnum = player.GetItemIndex(slotIndex)
		self.attrChangeIndex[player.PET_WND_SLOT_ATTR_CHANGE_RESULT] = slotIndex
		self.attrChangeSlot.SetItemSlot(player.PET_WND_SLOT_ATTR_CHANGE_RESULT, itemVnum)
		self.attrChangeSlot.RefreshSlot()

	def ItemMoveAttrChangeWindow(self, slotWindow, slotIndex):
		if player.INVENTORY == slotWindow:
			attachSlotType = player.SLOT_TYPE_INVENTORY
		else:
			return FALSE

		mouseModule.mouseController.DeattachObject()

		selectedItemVNum = player.GetItemIndex(slotWindow, slotIndex)
		count = player.GetItemCount(slotWindow, slotIndex)

		mouseModule.mouseController.AttachObject(self, attachSlotType, slotIndex, selectedItemVNum, count)

		item.SelectItem(selectedItemVNum)

		if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
			if item.GetItemSubType() == item.PET_UPBRINGING:
				slotPos = player.PET_WND_SLOT_ATTR_CHANGE
			elif item.GetItemSubType() == item.PET_ATTR_CHANGE:
				slotPos = player.PET_WND_SLOT_ATTR_CHANGE_ITEM
			else:
				return FALSE

			self.SelectEmptyAttrChangeSlot(slotPos)

			mouseModule.mouseController.DeattachObject()
			return TRUE

		return FALSE

	def ClickDetermineButton(self):
		# If there is an active pet and player has player.PET_ATTR_DETERMINE_ITEM 
		# item, send the packet
		activePetId = player.GetActivePetItemId()
		if activePetId != 0:
			sourcePos = player.GetItemSlotIndex(player.PET_ATTR_DETERMINE_ITEM)
			if sourcePos != -1:
				net.SendPetAttrDetermine(sourcePos)

	def PetAttrDetermineResult(self, byType):
		# Display determine result in a popup dialog
		text = self.typeInfo[byType]
		self.__OpenPopupDialog(text)

	def __DescPetPremiumFeedstuff(self):
		# Initialize pet premium feedstuff description
		event.ClearEventSet(self.descIndex)

		self.descIndex = event.RegisterEventSet(uiScriptLocale.PET_PRIMIUM_FEEDSTUFF_DESC)
		event.SetVisibleLineCount(self.descIndex, VISIBLE_LINE_COUNT)

		event.SetVisibleLineCount(self.descIndex, VISIBLE_LINE_COUNT)
		event.SetRestrictedCount(self.descIndex, 70)

	def PetPremiumFeedStuffPrevDescriptionPage(self):
		line_height	= event.GetLineHeight(self.descIndex) + 4

		cur_start_line = event.GetVisibleStartLine(self.descIndex)

		decrease_count = VISIBLE_LINE_COUNT

		if cur_start_line - decrease_count < 0:
			return;

		event.SetVisibleStartLine(self.descIndex, cur_start_line - decrease_count)
		self.desc_Y += (line_height * decrease_count)

	def PetPremiumFeedStuffNextDescriptionPage(self):
		line_height = event.GetLineHeight(self.descIndex) + 4

		total_line_count = event.GetProcessedLineCount(self.descIndex)
		cur_start_line = event.GetVisibleStartLine(self.descIndex)

		increase_count = VISIBLE_LINE_COUNT

		if cur_start_line + increase_count >= total_line_count:
			increase_count = total_line_count - cur_start_line

		if increase_count < 0 or cur_start_line + increase_count >= total_line_count:
			return

		event.SetVisibleStartLine(self.descIndex, cur_start_line + increase_count)
		self.desc_Y -= (line_height * increase_count)

	def OverInPetReviveResultSlot(self):
		if None != self.tooltipItem:
			invenPos = self.petReviveIndex
			if invenPos != -1:
				if self.isPetReviveResultTooltip:	# Real pet tooltip after being revived
					self.tooltipItem.SetInventoryItem(invenPos, player.INVENTORY)
				else:	# To be revived pet dummy tooltip
					metinSlot = [player.GetItemMetinSocket(invenPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
					pet_id = metinSlot[2]

					if pet_id:
						self.tooltipItem.ClearToolTip()

						itemVnum = player.GetItemIndex(invenPos)
						item.SelectItem(itemVnum)

						(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)

						(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)

						# Item Title
						self.tooltipItem.AppendTextLine(pet_nick, self.tooltipItem.TITLE_COLOR)

						# Item Description
						itemDesc = item.GetItemDescription()
						self.tooltipItem.AppendDescription(itemDesc, 26)

						# Pet Level/Age
						self.tooltipItem.AppendSpace(5)
						birthSec = max(0, metinSlot[0] - birthday)
						self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTIP_LEVEL + " " + str(pet_level) + " (" + localeInfo.SecondToDay(birthSec * 0.8) + ")")

						# Pet Evolution / Skill Count
						self.tooltipItem.AppendSpace(5)
						if skill_count:
							self.tooltipItem.AppendTextLine(self.tooltipItem.GetEvolName(evol_level) + "(" + str(skill_count) + ")")
						else:
							self.tooltipItem.AppendTextLine(self.tooltipItem.GetEvolName(evol_level))

						# Pet HP/SP/DEF
						self.tooltipItem.AppendSpace(5)
						self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTIP_HP + " +" + str("%0.1f" % pet_hp) + "%", self.tooltipItem.SPECIAL_POSITIVE_COLOR)
						self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTIP_DEF + " +" + str("%0.1f" % pet_def) + "%", self.tooltipItem.SPECIAL_POSITIVE_COLOR)
						self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTIP_SP + " +" + str("%0.1f" % pet_sp) + "%", self.tooltipItem.SPECIAL_POSITIVE_COLOR)

						# Pet Skills
						if pet_skill1:
							self.tooltipItem.AppendSpace(5)
							(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill1)
							self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTUP_SKILL % (pet_skill_name, pet_skill_level1) , self.tooltipItem.SPECIAL_POSITIVE_COLOR)

						if pet_skill2:
							self.tooltipItem.AppendSpace(5)
							(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill2)
							self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTUP_SKILL % (pet_skill_name, pet_skill_level2) , self.tooltipItem.SPECIAL_POSITIVE_COLOR)

						if pet_skill3:
							self.tooltipItem.AppendSpace(5)
							(pet_skill_name, pet_skill_desc, pet_skill_use_type , pet_skill_cool_time) = skill.GetPetSkillInfo(pet_skill3)
							self.tooltipItem.AppendTextLine(localeInfo.PET_TOOLTUP_SKILL % (pet_skill_name, pet_skill_level3) , self.tooltipItem.SPECIAL_POSITIVE_COLOR)

						# Pet Lifetime
						for i in xrange(item.LIMIT_MAX_NUM):
							(limitType, limitValue) = item.GetLimit(i)
							if item.LIMIT_REAL_TIME == limitType:
								(endTime, maxTime) = player.GetPetLifeTime(pet_id)
								self.tooltipItem.AppendPetItemLastTime(app.GetGlobalTimeStamp() + maxTime)

						self.tooltipItem.Show()

		return

	def OverOutPetReviveResultSlot(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		return

	def OverInPetReviveSlot(self):
		if None != self.tooltipItem:
			invenPos = self.petReviveIndex
			if invenPos != -1:
				self.tooltipItem.SetInventoryItem(invenPos, player.INVENTORY)

		return

	def OverOutPetReviveSlot(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		return

	def __ClearPetReviveWindow(self):
		self.CanInvenSlot(self.petReviveIndex)

		self.isPetReviveResultTooltip = FALSE
		self.petReviveIndex = -1

		self.petReviveSlot.SetItemSlot(0, 0)
		self.petReviveSlot.RefreshSlot()

		self.petReviveResultSlot.SetItemSlot(0, 0)
		self.petReviveResultSlot.RefreshSlot()

		for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
			self.petReviveMaterialSlot.SetItemSlot(i, 0)
			self.petReviveMaterialSlot.RefreshSlot()

			if self.petReviveItems[i]:
				self.CanInvenSlot(self.petReviveItems[i])
			self.petReviveItems[i] = -1
			self.petReviveItemsCount[i] = 0

		self.petReviveAgeText.SetText("")
		self.petReviveResultAgeText.SetText("")
		self.petReviveItemsCountText.SetText(localeInfo.PET_REVIVE_UI_MATERIAL_COUNT % (0, 0))

	def SelectPetReviveEmptySlot(self, slotIndex):
		if not mouseModule.mouseController.isAttached():
			return FALSE

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemVNum = player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		# Return if item is beging attached from not inventory window
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return FALSE

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT: 
			return FALSE

		curTime = app.GetGlobalTimeStamp()
		metinSlot = [player.GetItemMetinSocket(attachedSlotPos, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]

		# Return if there is an active pet or target pet is still alive
		if item.ITEM_TYPE_GROWTH_PET == itemType and itemSubType == item.PET_UPBRINGING:
			activePetId = player.GetActivePetItemId()
			if activePetId != 0:
				return FALSE

			if metinSlot[0] > curTime:
				return FALSE
		else:
			return FALSE

		# Return if there is already pet in place
		if self.petReviveIndex != -1:
			return FALSE

		# Return if no pet_id in the socket
		pet_id = metinSlot[2]
		if 0 == pet_id:
			return FALSE

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)
		birthSec = max(0, metinSlot[0] - birthday)

		# Upbringing Slot
		self.petReviveIndex = attachedSlotPos
		self.petReviveSlot.SetItemSlot(slotIndex, attachedItemVNum)
		self.petReviveSlot.RefreshSlot()

		# Revived Upbringing Slot
		self.petReviveResultSlot.SetItemSlot(slotIndex, attachedItemVNum)
		self.petReviveResultSlot.RefreshSlot()

		# Age Texts
		self.petReviveAgeText.SetText("%s" % localeInfo.SecondToDay(birthSec))
		self.petReviveResultAgeText.SetText("%s" % localeInfo.SecondToDay(birthSec * 0.8))
		self.__RefreshPetReviveMaterialCount()

		mouseModule.mouseController.DeattachObject()
		return TRUE

	def SelectPetReviveItemSlot(self):
		self.__ClearPetReviveWindow()

	def SelectPetReviveMaterialEmptySlot(self, slotIndex):
		if not mouseModule.mouseController.isAttached():
			return FALSE

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
		attachedItemVNum = player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVNum)

		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()	

		# Return if item is beging attached from not inventory window
		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return FALSE

		# Return if item position is bigger than player.ITEM_SLOT_COUNT
		if attachedSlotPos >= player.ITEM_SLOT_COUNT:
			return FALSE

		# Return if there is no to-be-revived pet in slot
		if self.petReviveIndex == -1:
			return FALSE

		# Return if material is not item.PET_PRIMIUM_FEEDSTUFF
		if itemType != item.ITEM_TYPE_GROWTH_PET or itemSubType != item.PET_PRIMIUM_FEEDSTUFF:
			return FALSE

		# Return if the slot is already occupied or item is in another slot
		if self.petReviveItems[slotIndex] != -1:
			return FALSE

		if attachedSlotPos in self.petReviveItems:
			return FALSE

		# Return if enough material has been appended
		if self.HasEnoughPetReviveMaterial():
			return FALSE

		self.petReviveItems[slotIndex] = attachedSlotPos
		self.petReviveItemsCount[slotIndex] = attachedItemCount
		self.petReviveMaterialSlot.SetItemSlot(slotIndex, attachedItemVNum, attachedItemCount)
		self.petReviveMaterialSlot.RefreshSlot()

		# Refresh required material text
		self.__RefreshPetReviveMaterialCount()

		mouseModule.mouseController.DeattachObject()
		return TRUE

	def SelectPetReviveMaterialItemSlot(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			return

		self.CanInvenSlot(self.petReviveItems[slotIndex])
		self.petReviveItems[slotIndex] = -1
		self.petReviveItemsCount[slotIndex] = 0
		self.petReviveMaterialSlot.ClearSlot(slotIndex)
		self.petReviveMaterialSlot.RefreshSlot()

		self.__RefreshPetReviveMaterialCount()

	def __RefreshPetReviveMaterialCount(self):
		if -1 == self.petReviveIndex:
			self.petReviveItemsCountText.SetText(localeInfo.PET_REVIVE_UI_MATERIAL_COUNT % (0, 0))
			return

		metinSlot = [player.GetItemMetinSocket(self.petReviveIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		pet_id = metinSlot[2]

		# Pet data
		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)
		birthSec = max(0, metinSlot[0] - birthday)

		# Material Count
		petMaxReviveMaterialCount = localeInfo.SecondToDayNumber(birthSec) / 10
		if petMaxReviveMaterialCount < 1:
			petMaxReviveMaterialCount = 1

		petReviveMaterialCount = 0
		for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
			if self.petReviveItems[i] != -1:
				petReviveMaterialCount += self.petReviveItemsCount[i]

		# Set added mateiral count to required material count if higher
		if petReviveMaterialCount > petMaxReviveMaterialCount:
			petReviveMaterialCount = petMaxReviveMaterialCount

		self.petReviveItemsCountText.SetText(localeInfo.PET_REVIVE_UI_MATERIAL_COUNT % (petReviveMaterialCount, petMaxReviveMaterialCount))

	def HasEnoughPetReviveMaterial(self):
		if -1 == self.petReviveIndex:
			return FALSE

		metinSlot = [player.GetItemMetinSocket(self.petReviveIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		pet_id = metinSlot[2]

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)
		birthSec = max(0, metinSlot[0] - birthday)

		# Material Count
		petMaxReviveMaterialCount = localeInfo.SecondToDayNumber(birthSec) / 10
		if petMaxReviveMaterialCount < 1:
			petMaxReviveMaterialCount = 1

		itemCount = 0
		for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
			if self.petReviveItems[i] != -1:
				itemCount += self.petReviveItemsCount[i]

			if itemCount >= petMaxReviveMaterialCount:
				return TRUE

		return FALSE

	def ClickPetReviveButton(self):
		# Return if there is no pet upbringing item in slot
		if self.petReviveIndex == -1:
			self.__OpenPopupDialog(localeInfo.PET_REVIVE_FAIL_EMPTY_PET_SLOT)
			return

		# Return if question dialog is already opened
		if self.questionDialog.IsShow():
			return

		# Return if there is not enough material
		if not self.HasEnoughPetReviveMaterial():
			self.__OpenPopupDialog(localeInfo.PET_REVIVE_FAIL_NOT_ENOUGHT_MATERIAL)
			return

		metinSlot = [player.GetItemMetinSocket(self.petReviveIndex, i) for i in xrange(player.METIN_SOCKET_MAX_NUM)]
		pet_id = metinSlot[2]

		(pet_level, evol_level, birthday, pet_nick, pet_hp, pet_def, pet_sp) = player.GetPetItem(pet_id)
		birthSec = max(0, metinSlot[0] - birthday)

		petMaxReviveMaterialCount = localeInfo.SecondToDayNumber(birthSec) / 10
		if petMaxReviveMaterialCount < 1:
			petMaxReviveMaterialCount = 1

		self.questionDialog.SetText(localeInfo.PET_REVIVE_QUESTION % (pet_nick, petMaxReviveMaterialCount)) 
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__PetReviveAccept))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__PetReviveCancel))
		self.questionDialog.Open()

	def __PetReviveAccept(self):
		resultReviveItems = [value for value in self.petReviveItems if value != -1]
		resultReviveItemsCount = [value for value in self.petReviveItemsCount if value != 0]

		# Send revive packet if there are items in list and close the question dialog
		if resultReviveItems:
			net.SendPetRevive(self.petReviveIndex, resultReviveItems, resultReviveItemsCount)

		self.questionDialog.Close()

	def __PetReviveCancel(self):
		self.questionDialog.Close()

	def ClickPetReviveCancelButton(self):
		self.__ClearPetReviveWindow()

	def PetReviveResult(self, result):
		# If succeeded set revived pet to result item slot,
		# else clear the revive window.
		if result:
			self.isPetReviveResultTooltip = TRUE
			self.petReviveSlot.SetItemSlot(0, 0)
			self.petReviveSlot.RefreshSlot()

			for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
				self.petReviveMaterialSlot.SetItemSlot(i, 0)
				self.petReviveMaterialSlot.RefreshSlot()

				if self.petReviveItems[i]:
					self.CanInvenSlot(self.petReviveItems[i])
				self.petReviveItems[i] = -1
				self.petReviveItemsCount[i] = 0

			self.petReviveAgeText.SetText("")
			self.petReviveItemsCountText.SetText(localeInfo.PET_REVIVE_UI_MATERIAL_COUNT % (0, 0))
		else:
			self.__ClearPetReviveWindow()

	def ItemMovePremiumFeedWindow(self, slotWindow, slotIndex):
		if player.INVENTORY == slotWindow:
			attachSlotType = player.SLOT_TYPE_INVENTORY
		else:
			return FALSE

		mouseModule.mouseController.DeattachObject()

		selectedItemVNum = player.GetItemIndex(slotWindow, slotIndex)
		count = player.GetItemCount(slotWindow, slotIndex)

		mouseModule.mouseController.AttachObject(self, attachSlotType, slotIndex, selectedItemVNum, count)

		item.SelectItem(selectedItemVNum)

		if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
			if item.GetItemSubType() == item.PET_UPBRINGING:
				return self.SelectPetReviveEmptySlot(0)
			elif item.GetItemSubType() == item.PET_PRIMIUM_FEEDSTUFF:
				# Return the first available slot
				for i in xrange(player.PET_REVIVE_MATERIAL_SLOT_MAX):
					if self.petReviveItems[i] == -1:
						return self.SelectPetReviveMaterialEmptySlot(i)

		return FALSE

	def AttachItemToPetWindow(self, windowType, slotIndex):
		if self.IsShow():
			if self.GetState() == player.PET_WINDOW_INFO and self.feedIndex != player.FEED_BUTTON_MAX:
				if not self.CantFeedItem(slotIndex):
					if self.wndPetFeed.ItemMoveFeedWindow(windowType, slotIndex):
						return TRUE
			elif self.GetState() == player.PET_WINDOW_ATTR_CHANGE:
				if self.ItemMoveAttrChangeWindow(windowType, slotIndex):
					return TRUE
			elif  self.GetState() == player.PET_WINDOW_PRIMIUM_FEEDSTUFF:
				if self.ItemMovePremiumFeedWindow(windowType, slotIndex):
					return TRUE

		return FALSE

class TextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.Show()
		self.textLine = textLine

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetHorizontalAlignLeft()
		self.textLine.SetText(text)

	def OnRender(self):
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		mouseY = mouseY - 50

		if self.textLine.GetText():
			# Move upwards if text is a multiline text
			mouseY = mouseY - (self.textLine.GetTextLineCount() - 1) * 6

		self.textLine.SetPosition(mouseX, mouseY)
