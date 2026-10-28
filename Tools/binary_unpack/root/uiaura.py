if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import snd
import chat
import item
import wndMgr
import ui
import uiCommon
import localeInfo
import mouseModule

USE_AURA_WINDOW_LIMIT_RANGE = 1000

class AuraWindow(ui.ScriptWindow):
	def __init__(self):
		if not app.ENABLE_AURA_COSTUME_SYSTEM:
			import exception
			exception.Abort("What do you do?")
			return

		ui.ScriptWindow.__init__(self)
		self.Initialize()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()

		if self.questionDialog:
			self.questionDialog.Close()
		self.Initialize()

	def Initialize(self):
		self.auraRefineWindowType = player.AURA_WINDOW_TYPE_MAX
		self.isLoaded = 0
		self.toolTipItem = None
		self.questionDialog = None
		self.xAuraWindowStart = 0
		self.yAuraWindowStart = 0

	def __LoadWindow(self, type):
		pyScriptFileNames = {
			player.AURA_WINDOW_TYPE_ABSORB : "UIScript/aura_absorbwindow.py",
			player.AURA_WINDOW_TYPE_GROWTH : "UIScript/aura_refinewindow.py",
			player.AURA_WINDOW_TYPE_EVOLVE : "UIScript/aura_refinewindow.py"
		}

		titleNames = {
			player.AURA_WINDOW_TYPE_ABSORB : localeInfo.AURA_TITLE_ABSORB,
			player.AURA_WINDOW_TYPE_GROWTH : localeInfo.AURA_TITLE_GROWTH,
			player.AURA_WINDOW_TYPE_EVOLVE : localeInfo.AURA_TITLE_EVOLUTION
		}

		if pyScriptFileNames.has_key(type):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, pyScriptFileNames[type])
			except:
				import exception
				exception.Abort("AuraWindow.__LoadWindow.LoadScript")
		else:
			import exception
			exception.Abort("AuraWindow.__LoadWindow.InvalidWindow[%d]" % (type))
			return

		try:
			wndItem = self.GetChild("AuraSlot")
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Cancel))
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Cancel))
			self.GetChild("TitleName").SetText(titleNames[type])
		except:
			import exception
			exception.Abort("AuraWindow.__LoadWindow.LoadBindGenerals")

		self.AuraToolTip1 = self.GetChild2("AuraToolTip1")
		self.AuraToolTip2 = self.GetChild2("AuraToolTip2")
		self.AuraToolTip3 = self.GetChild2("AuraToolTip3")
		self.AuraToolTip4 = self.GetChild2("AuraToolTip4")
		self.AuraToolTip5 = self.GetChild2("AuraToolTip5")
		self.AuraToolTip6 = self.GetChild2("AuraToolTip6")

		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.Show()
		self.wndItem = wndItem

	def ClearAuraLevelUpWindowToolTip(self):
		if self.auraRefineWindowType == player.AURA_WINDOW_TYPE_GROWTH or self.auraRefineWindowType == player.AURA_WINDOW_TYPE_EVOLVE:
			self.AuraToolTip1.SetText("")
			self.AuraToolTip2.SetText("")
			self.AuraToolTip3.SetText("")
			self.AuraToolTip4.SetText("")
			self.AuraToolTip5.SetText("")
			self.AuraToolTip6.SetText("")

	def CloseWindow(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()

		if self.toolTipItem:
			self.toolTipItem.HideToolTip()
		if self.questionDialog:
			self.questionDialog.Close()

		self.ClearAuraLevelUpWindowToolTip()
		ui.ScriptWindow.Hide(self)
		self.isLoaded = 0
		net.SendAuraRefineCancel()

	def Open(self, type):
		if self.isLoaded == 0:
			self.isLoaded = 1
			self.__LoadWindow(type)
			self.auraRefineWindowType = type
			self.SetCenterPosition()
			self.SetTop()
			ui.ScriptWindow.Show(self)
			(self.xAuraWindowStart, self.yAuraWindowStart, z) = player.GetMainCharacterPosition()

	def Accept(self):
		if player.IsAuraRefineWindowEmpty():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_ITEM_NEED_REGISTER)
			return

		net.SendAuraRefineAccept(self.auraRefineWindowType)

	def Cancel(self):
		net.SendAuraRefineCancel()

	def SetItemToolTip(self, itemTooltip):
		self.toolTipItem = itemTooltip

	def __ShowToolTip(self, slotIndex):
		if self.toolTipItem:
			self.toolTipItem.SetAuraWindowItem(slotIndex)

	def OverInItem(self, slotIndex):
		self.wndItem.SetUsableItem(FALSE)
		self.__ShowToolTip(slotIndex)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(FALSE)
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	def SelectEmptySlot(self, selectedSlotPos):
		if selectedSlotPos == (player.AURA_SLOT_MAX - 1):
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotWindow = player.SlotTypeToInvenType(attachedSlotType)

			itemVnum = player.GetItemIndex(attachedSlotWindow, attachedSlotPos)

			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()
			if attachedSlotPos >= player.EQUIPMENT_SLOT_START:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return

			if player.SLOT_TYPE_AURA != attachedSlotType:
				if player.RESERVED_WINDOW == attachedSlotWindow:
					return

				possablecheckin = FALSE
				if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								if player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
									possablecheckin = TRUE
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						if item.IsWeddingItem(itemVnum) == 1:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

						if itemType == item.ITEM_TYPE_ARMOR:
							if itemSubType in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
								if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
									possablecheckin = TRUE
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_REFINEITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return

				elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return

								possablecheckin = TRUE
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if itemType == item.ITEM_TYPE_RESOURCE:
								if itemSubType == item.RESOURCE_AURA:
									possablecheckin = TRUE
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURARESOURCE)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

				elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
					if selectedSlotPos == player.AURA_SLOT_MAIN:
						if itemType == item.ITEM_TYPE_COSTUME:
							if itemSubType == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(attachedSlotWindow, attachedSlotPos, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curLevel != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return

								possablecheckin = TRUE
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif selectedSlotPos == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell != player.NPOS():
							socketLevelValue = player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
							curLevel = (socketLevelValue / 100000) - 1000
							if itemVnum == player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
								if player.GetItemCount(attachedSlotWindow, attachedSlotPos) >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT):
									possablecheckin = TRUE
								else:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
									return
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
								return

						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

				if possablecheckin:
					if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
						if selectedSlotPos == player.AURA_SLOT_SUB:
							questionDialog = uiCommon.QuestionDialog("thin")
							questionDialog.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
							questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAuraAcceptEvent))
							questionDialog.SetCancelEvent(self.OnAuraCloseEvent)
							questionDialog.Open()
							questionDialog.attachedSlotWindow = attachedSlotWindow
							questionDialog.attachedSlotPos = attachedSlotPos
							questionDialog.selectedSlotPos = selectedSlotPos
							self.questionDialog = questionDialog
						else:
							net.SendAuraRefineCheckIn(attachedSlotWindow, attachedSlotPos, player.AURA_REFINE, selectedSlotPos, self.auraRefineWindowType)

					elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
						net.SendAuraRefineCheckIn(attachedSlotWindow, attachedSlotPos, player.AURA_REFINE, selectedSlotPos, self.auraRefineWindowType)

					elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_EVOLVE:
						net.SendAuraRefineCheckIn(attachedSlotWindow, attachedSlotPos, player.AURA_REFINE, selectedSlotPos, self.auraRefineWindowType)

					snd.PlaySound("sound/ui/drop.wav")

			mouseModule.mouseController.DeattachObject()

	def OnAuraAcceptEvent(self):
		net.SendAuraRefineCheckIn(self.questionDialog.attachedSlotWindow, self.questionDialog.attachedSlotPos, player.AURA_REFINE, self.questionDialog.selectedSlotPos, self.auraRefineWindowType)
		self.questionDialog.Close()
		self.questionDialog = None

	def OnAuraCloseEvent(self):
		self.questionDialog.Close()
		self.questionDialog = None

	def UseItemSlot(self, selectedSlotPos):
		if selectedSlotPos == (player.AURA_SLOT_MAX - 1):
			return

		mouseModule.mouseController.DeattachObject()
		net.SendAuraRefineCheckOut(selectedSlotPos, self.auraRefineWindowType)

	def SelectItemSlot(self, selectedSlotPos):
		if selectedSlotPos == (player.AURA_SLOT_MAX - 1):
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				snd.PlaySound("sound/ui/drop.wav")

			mouseModule.mouseController.DeattachObject()

		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SAFEBOX_SELL_DISABLE_SAFEITEM)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			else:
				selectedItemID = player.GetAuraItemID(selectedSlotPos)
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_AURA, player.AURA_REFINE, selectedSlotPos, selectedItemID)
				snd.PlaySound("sound/ui/pick.wav")

	def RefreshAuraWindow(self):
		getAuraItem = player.GetAuraItemID
		getAuraCount = player.GetAuraItemCount
		setAuraItem = self.wndItem.SetItemSlot
		AuraItemSize = player.GetAuraCurrentItemSlotCount()
		self.ClearAuraLevelUpWindowToolTip()
		[self.wndItem.ClearSlot(i) for i in range(player.AURA_SLOT_MAX)]
		for i in xrange(AuraItemSize):
			itemCount = getAuraCount(i)
			if 1 == itemCount:
				itemCount = 0
			setAuraItem(i, getAuraItem(i), itemCount)
			if self.auraRefineWindowType == player.AURA_WINDOW_TYPE_GROWTH:
				if i == player.AURA_SLOT_MAIN:
					if getAuraItem(i) != 0:
						curLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_CURRENT)
						curStep = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_STEP)
						curDrainPct = 1.0 * curLevel / 10.
						curExpPct = player.GetAuraRefineInfoExpPer(player.AURA_REFINE_INFO_SLOT_CURRENT)

						self.AuraToolTip1.SetText(localeInfo.AURA_TOOLTIP_NOW)
						self.AuraToolTip2.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (curLevel, curStep, curDrainPct))
						self.AuraToolTip3.SetText(localeInfo.AURA_TOOLTIP_EXP % (curExpPct))

				if i == player.AURA_SLOT_SUB:
					if getAuraItem(i) != 0:
						nextLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_NEXT)
						nextStep = player.GetAuraRefineInfo(nextLevel, player.AURA_REFINE_INFO_STEP)
						nextDrainPct = 1.0 * nextLevel / 10.
						nextExpPct = player.GetAuraRefineInfoExpPer(player.AURA_REFINE_INFO_SLOT_NEXT)

						self.AuraToolTip4.SetText(localeInfo.AURA_TOOLTIP_NEXT)
						self.AuraToolTip5.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (nextLevel, curStep, nextDrainPct))
						self.AuraToolTip6.SetText(localeInfo.AURA_TOOLTIP_EXP % (nextExpPct))

			if self.auraRefineWindowType == player.AURA_WINDOW_TYPE_EVOLVE:
				if i == player.AURA_SLOT_MAIN:
					if getAuraItem(i) != 0:
						curLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_CURRENT)
						needMaterialVnum = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM)
						needMaterialCount = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT)
						needGold = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_GOLD)
						evolPer = player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_EVOLVE_PCT)

						nextLevel = player.GetAuraRefineInfoLevel(player.AURA_REFINE_INFO_SLOT_EVOLVED)
						nextStep = player.GetAuraRefineInfo(nextLevel, player.AURA_REFINE_INFO_STEP)
						nextDrainPct = 1.0 * nextLevel / 10.

						self.AuraToolTip1.SetText(localeInfo.AURA_TOOLTIP_NEXT)
						self.AuraToolTip2.SetText(localeInfo.AURA_TOOLTIP_LEVEL % (nextLevel, nextStep, nextDrainPct))
						self.AuraToolTip3.SetText(localeInfo.AURA_TOOLTIP_EVOLUTION_PER % (evolPer))

						self.AuraToolTip4.SetText(localeInfo.AURA_TOOLTIP_RESOURCE)
						self.AuraToolTip5.SetText(localeInfo.AURA_TOOLTIP_RESOURCE_ITEM % (item.GetItemNameByVnum(needMaterialVnum), needMaterialCount))
						self.AuraToolTip6.SetText(localeInfo.AURA_TOOLTIP_NEED_YANG % (needGold))

		self.wndItem.RefreshSlot()

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xAuraWindowStart) > USE_AURA_WINDOW_LIMIT_RANGE or abs(y - self.yAuraWindowStart) > USE_AURA_WINDOW_LIMIT_RANGE:
			self.Cancel()

	def OnPressEscapeKey(self):
		self.Cancel()
		return TRUE
