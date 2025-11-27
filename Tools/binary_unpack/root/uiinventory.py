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
import uiToolTip
import uiPrivateShopBuilder
import localeInfo
import constInfo
import ime
import wndMgr
import exchange

if app.ENABLE_QUICK_SELL_ITEM:
	import shop

if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import acce

if app.ENABLE_AUTO_SELL_SYSTEM:
	import uiautosell

if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
	import uiAttachBonus

ITEM_FLAG_APPLICABLE = 1 << 14

class InventoryWindowPotions(ui.ScriptWindow):
	def __init__(self, wndInventory):
		import exception
		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.UseInventoryPotions = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindowPotions.py")
		except:
			import exception
			exception.Abort("InventoryWindowPotions.LoadWindow.LoadObject")

		try:
			wndEquip = self.GetChild("GridTable")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.UseInventoryPotions = self.GetChild("UseInventoryPotions")

		except:
			import exception
			exception.Abort("InventoryWindowPotions.LoadWindow.BindObject")

		self.UseInventoryPotions.SetEvent(self.ActivateAll)

		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))
		self.wndEquip = wndEquip

	def RefreshSlot(self):
		if not self.wndEquip:
			return
			
		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndEquip.SetItemSlot

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i

			itemCount = getItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			# GridTable start_index kullanýyor, bu yüzden global slot numarasýný (slotNumber) kullan
			# GridTable otomatik olarak start_index'i çýkararak local indeksi hesaplar
			# SetItemSlot fonksiyonu ItemIndex 0 olduðunda otomatik olarak ClearSlot çaðýrýyor
			# Bu yüzden ClearSlot'u doðrudan çaðýrmaya gerek yok
			setItemVNum(slotNumber, itemVnum, itemCount)

		self.wndEquip.RefreshSlot()

	def ActivateAll(self):
		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			net.SendItemUsePacket(slotNumber)

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class InventoryWindow(ui.ScriptWindow):

	USE_TYPE_TUPLE = (
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE2",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET"
	)

	if app.ENABLE_BONUS_COSTUME_SYSTEM:
		USE_TYPE_TUPLE += ("USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR")

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		USE_TYPE_TUPLE += ("USE_PUT_INTO_AURA_SOCKET",)

	questionDialog = None
	tooltipItem = None
	dlgPickMoney = None
	interface = None

	if app.ENABLE_GROWTH_PET_SYSTEM:
		petHatchingWindow = None
		petFeedWindow = None
		petNameChangeWindow = None

	sellingSlotNumber = -1
	isLoaded = 0

	equip_info = [
		[item.EQUIPMENT_BODY, 39, 37, 32, 64, "Armor"],
		[item.EQUIPMENT_HEAD, 39, 2, 32, 32, "Head"],
		[item.EQUIPMENT_SHOES, 39, 145, 32, 32, "Shoes"],
		[item.EQUIPMENT_WRIST, 75, 67, 32, 32, "Wrist"],
		[item.EQUIPMENT_WEAPON, 3, 3, 32, 96, "Weapon"],
		[item.EQUIPMENT_NECK, 114, 67, 32, 32, "Necklace"],
		[item.EQUIPMENT_EAR, 114, 35, 32, 32, "Earrings"],
		[item.EQUIPMENT_UNIQUE1, 2, 145, 32, 32, "Unique"],
		[item.EQUIPMENT_UNIQUE2, 75, 145, 32, 32, "Unique"],
		[item.EQUIPMENT_ARROW, 114, 2, 32, 32, "Arrow"],
		[item.EQUIPMENT_SHIELD, 75, 35, 32, 32, "Shield"],
		[item.EQUIPMENT_BELT, 39, 106, 32, 32, "Belt"],
	]

	costume_info = [
		[item.COSTUME_SLOT_HAIR, 56, 28, 32, 32, "Hairstyle"],
		[item.COSTUME_SLOT_BODY, 56, 74, 32, 64, "Costume"],
		[item.COSTUME_SLOT_ACCE, 107, 9, 32, 32, "Acce"],
		[item.COSTUME_SLOT_WEAPON, 8, 13, 32, 96, "Weapon"],
		[item.COSTUME_SLOT_MOUNT, 107, 53, 32, 32, "Mount Skin"],
		[item.COSTUME_SLOT_PET, 106, 95, 32, 32, "Pet Skin"],
		[item.COSTUME_SLOT_AURA, 106, 137, 32, 32, "Aura"],
	]

	rings_info = [
		[item.EQUIPMENT_RING1, 9, 45, 32, 32, "Ring 1"],
		[item.EQUIPMENT_RING2, 59, 45, 32, 32, "Ring 2"],
		[item.EQUIPMENT_PENDANT, 109, 45, 32, 32, "Pendant"],
		[item.EQUIPMENT_MOUNT, 9, 103, 32, 32, "Mount"],
		[item.EQUIPMENT_PET, 59, 103, 32, 32, "Pet"],
		[item.EQUIPMENT_TITLE, 109, 103, 32, 32, "Title"],
	]

	def __init__(self):
		self.wndExpandedMoneyBar = None
		self.wndPotions = None

		ui.ScriptWindow.__init__(self)

		self.inventoryPageIndex = 0
		self.equipmentPageIndex = 0

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList = []

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.wndExpandedMoneyBar = None
		self.wndPotions = None

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

		if self.wndExpandedMoneyBar:
			self.wndExpandedMoneyBar.Open()

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.RefreshVisibleCostume()

		# Sidebar butonlarý artýk ExpandedTaskBar'da, __ShowSidebar() kaldýrýldý

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def TogglePotionsWindow(self):
		"""Potions window'unu aç/kapa"""
		if not self.wndPotions:
			self.wndPotions = InventoryWindowPotions(self)
		
		if self.wndPotions.IsShow():
			self.wndPotions.Close()
		else:
			self.wndPotions.Show()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			wndBase = self.GetChild("Equipment_Base")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			if app.ENABLE_SORT_INVENTORY:
				self.sortInventory = self.GetChild2("SortInventoryButton")

			self.inventoryTab = []
			for i in xrange(player.INVENTORY_PAGE_COUNT):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d" % (i+1)))

			self.equipmentTab = []
			for i in xrange(3):
				self.equipmentTab.append(self.GetChild("Equipment_Tab_%02d" % (i+1)))

			self.equipmentTabText = []
			for i in xrange(3):
				self.equipmentTabText.append(self.GetChild("Equipment_Tab_%02d_Print" % (i+1)))

			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleButtonList.append(self.GetChild("CostumeBodyButton"))
				self.visibleButtonList.append(self.GetChild("CostumeHairButton"))

				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					self.visibleButtonList.append(self.GetChild("CostumeAcceButton"))

				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					self.visibleButtonList.append(self.GetChild("CostumeWeaponButton"))

				if app.ENABLE_AURA_COSTUME_SYSTEM:
					self.visibleButtonList.append(self.GetChild("CostumeAuraButton"))

			if app.ENABLE_PICKUP_ITEM_EFFECT:
				self.listHighlightedSlot = []

			if app.ENABLE_GROWTH_PET_SYSTEM:
				self.PetItemQuestionDlg = uiCommon.QuestionDialog("thin")
				self.PetItemQuestionDlg.Close()

				self.PetSkillDelDlg = uiCommon.QuestionDialog2()
				self.PetSkillDelDlg.Close()
				self.PetSkillAllDelBookIndex = -1

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.listHighlightedChangeLookSlot = []

			self.dlgQuestion = uiCommon.QuestionDialog2()
			self.dlgQuestion.Close()

		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		wndEquip = ui.SlotWindow()
		wndEquip.SetParent(wndBase)
		wndEquip.SetPosition(3, 3)
		wndEquip.SetSize(150, 182)

		for j in xrange(len(self.equip_info)):
			wndEquip.AppendSlot(self.equip_info[j][0], self.equip_info[j][1], self.equip_info[j][2], self.equip_info[j][3], self.equip_info[j][4])

		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndEquip.Show()
		self.wndEquip = wndEquip

		wndCostume = ui.SlotWindow()
		wndCostume.SetParent(wndBase)
		wndCostume.SetPosition(3, 3)
		wndCostume.SetSize(150, 182)

		for j in xrange(len(self.costume_info)):
			wndCostume.AppendSlot(self.costume_info[j][0], self.costume_info[j][1], self.costume_info[j][2], self.costume_info[j][3], self.costume_info[j][4])

		wndCostume.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndCostume.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndCostume.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndCostume.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndCostume.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndCostume.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndCostume.Show()
		self.wndCostume = wndCostume

		wndRings = ui.SlotWindow()
		wndRings.SetParent(wndBase)
		wndRings.SetPosition(3, 3)
		wndRings.SetSize(150, 182)

		for j in xrange(len(self.rings_info)):
			wndRings.AppendSlot(self.rings_info[j][0], self.rings_info[j][1], self.rings_info[j][2], self.rings_info[j][3], self.rings_info[j][4])

		wndRings.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndRings.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndRings.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndRings.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndRings.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndRings.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		wndRings.Show()
		self.wndRings = wndRings

		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		if app.ENABLE_SORT_INVENTORY:
			if self.sortInventory:
				self.sortInventory.SetEvent(ui.__mem_func__(self.ClickSortInventory))

		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()

		if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
			self.attachBonusDialog = uiAttachBonus.AttachBonusDialog()
			self.attachBonusDialog.Hide()

		for i in xrange(player.INVENTORY_PAGE_COUNT):
			self.inventoryTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		for i in xrange(3):
			self.equipmentTab[i].SetEvent(lambda arg=i: self.SetEquipmentPage(arg))
		self.equipmentTab[0].Down()

		self.wndItem = wndItem
		self.wndBase = wndBase
		self.dlgPickMoney = dlgPickMoney

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList[0].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 1, 0)
			self.visibleButtonList[1].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 2, 0)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.visibleButtonList[2].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 3, 0)

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visibleButtonList[3].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 4, 0)

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.visibleButtonList[4].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 5, 0)

			self.visibleButtonList[0].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 1, 1)
			self.visibleButtonList[1].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 2, 1)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.visibleButtonList[2].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 3, 1)

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.visibleButtonList[3].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 4, 1)

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.visibleButtonList[4].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 5, 1)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.listAttachedAcces = []

		if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
			self.lock, self.lock2, u, e = {}, {}, 0, 0
			for i in xrange(18):
				self.lock[i] = ui.Button()
				self.lock[i].SetParent(self.wndItem)
				if i >= 9:
					self.lock[i].SetPosition(0, 0+e)
					e+=32
				else:
					self.lock[i].SetPosition(0, 0+u)
				self.lock[i].SetUpVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_close.sub")
				self.lock[i].SetOverVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_close.sub")
				self.lock[i].SetDownVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_close.sub")
				self.lock[i].SetEvent(lambda x=i: self.ClickUnlockInventory(x))
				self.lock[i].Hide()

				u+=32

		self.SetInventoryPage(0)
		self.SetEquipmentPage(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
			self.attachBonusDialog.Hide()
			self.attachBonusDialog.Destroy()
			self.attachBonusDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		self.wndEquip = None
		self.wndCostume = None
		self.wndRings = None

		self.dlgPickMoney = 0
		self.wndMoneySlot = 0
		self.questionDialog = None
		self.interface = None

		self.dlgQuestion = None

		self.wndExpandedMoneyBar = None

		if app.ENABLE_SORT_INVENTORY:
			self.sortInventory = None

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList = []

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.petHatchingWindow:
				self.petHatchingWindow = None

			if self.petFeedWindow:
				self.petFeedWindow = None

			if self.petNameChangeWindow:
				self.petNameChangeWindow = None

		# Sidebar butonlarý artýk ExpandedTaskBar'da, __HideSidebar() kaldýrýldý

		self.inventoryTab = []
		self.equipmentTab = []
		self.equipmentTabText = []

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return

		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		if self.wndExpandedMoneyBar:
			self.wndExpandedMoneyBar.Close()

		# Sidebar butonlarý artýk ExpandedTaskBar'da, __HideSidebar() kaldýrýldý

		wndMgr.Hide(self.hWnd)

	def Close(self):
		self.Hide()

	def SetExpandedMoneyBar(self, wndBar):
		self.wndExpandedMoneyBar = wndBar
		if self.wndExpandedMoneyBar:
			self.wndMoney = self.wndExpandedMoneyBar.GetMoney()
			self.wndMoneySlot = self.wndExpandedMoneyBar.GetMoneySlot()

	def SetInventoryPage(self, page):
		self.inventoryTab[self.inventoryPageIndex].SetUp()
		self.inventoryPageIndex = page
		self.inventoryTab[self.inventoryPageIndex].Down()
		self.RefreshBagSlotWindow()

	def SetEquipmentPage(self, page):
		self.equipmentTab[self.equipmentPageIndex].SetUp()
		self.equipmentPageIndex = page
		self.equipmentTab[self.equipmentPageIndex].Down()

		for i in xrange(3):
			if page == i:
				self.equipmentTabText[i].SetPackedFontColor(0xffffffff)
			else:
				self.equipmentTabText[i].SetPackedFontColor(0xff919291)

		self.wndEquip.Hide()
		self.wndCostume.Hide()
		self.wndRings.Hide()

		if page == 1:
			self.wndCostume.Show()
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				for item in self.visibleButtonList:
					item.Show()
		elif page == 2:
			self.wndRings.Show()
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				for item in self.visibleButtonList:
					item.Hide()
		else:
			self.wndEquip.Show()
			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				for item in self.visibleButtonList:
					item.Hide()

		self.wndBase.LoadImage("d:/ymir work/ui/game/inventory/%s.jpg" % (page+1))
		self.RefreshEquipSlotWindow()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			body = constInfo.HIDDEN_BODY_COSTUME
			hair = constInfo.HIDDEN_HAIR_COSTUME

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				acce = constInfo.HIDDEN_ACCE_COSTUME

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				weapon = constInfo.HIDDEN_WEAPON_COSTUME

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				aura = constInfo.HIDDEN_AURA_COSTUME

			if body == 1:
				self.visibleButtonList[0].Down()
			else:
				self.visibleButtonList[0].SetUp()

			if hair == 1:
				self.visibleButtonList[1].Down()
			else:
				self.visibleButtonList[1].SetUp()

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if acce == 1:
					self.visibleButtonList[2].Down()
				else:
					self.visibleButtonList[2].SetUp()

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				if weapon == 1:
					self.visibleButtonList[3].Down()
				else:
					self.visibleButtonList[3].SetUp()

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if aura == 1:
					self.visibleButtonList[4].Down()
				else:
					self.visibleButtonList[4].SetUp()

		def VisibleCostume(self, part, hidden):
			if not player.CanInteract():
				elementID = {1 : 0, 2 : 1, 3 : 2, 4 : 3, 5 : 4}[part]
				self.visibleButtonList[elementID].SetUp() if hidden == 1 else self.visibleButtonList[elementID].Down()
				return

			net.SendChatPacket("/hide_costume %d %d" % (part, hidden))

	if app.ENABLE_SORT_INVENTORY:
		def ClickSortInventory(self):
			self.__ClickStartButton()

		def __ClickStartButton(self):
			startQuestionDialog = uiCommon.QuestionDialog2()
			startQuestionDialog.SetText1(uiScriptLocale.SORT_INVENTORY_TEXT1)
			startQuestionDialog.SetText2(uiScriptLocale.SORT_INVENTORY_TEXT2)
			startQuestionDialog.SetAcceptEvent(ui.__mem_func__(self.__StartAccept))
			startQuestionDialog.SetCancelEvent(ui.__mem_func__(self.__StartCancel))
			startQuestionDialog.Open()
			self.startQuestionDialog = startQuestionDialog

		def __StartAccept(self):
			net.SendChatPacket("/click_sort_items")
			self.__StartCancel()

		def __StartCancel(self):
			self.startQuestionDialog.Close()

	if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
		def ClickUnlockInventory(self, index):
			if index > constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]:
				pass
			else:
				if constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)] <= 18:
					self.questionDialog = uiCommon.QuestionDialog("thin")
					self.questionDialog.SetCenterPosition()
					self.questionDialog.SetText(uiScriptLocale.INVENTORY_UNLOCK % int((constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]*2)+2))
					self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OpenInventory))
					self.questionDialog.SetCancelEvent(ui.__mem_func__(self.NoOpenInventory))
					self.questionDialog.Open()

		def OpenInventory(self):
			if constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)] <= 18:
				net.SendChatPacket("/unlock_inventory %d" % (0))

			self.questionDialog.Close()

		def NoOpenInventory(self):
			self.questionDialog.Close()

	def OpenPickMoneyDialog(self):

		if mouseModule.mouseController.isAttached():

			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():

				if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mouseModule.mouseController.DeattachObject()

		else:
			curMoney = player.GetElk()

			if curMoney <= 0:
				return

			self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
			self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			self.dlgPickMoney.Open(curMoney)
			self.dlgPickMoney.SetMax(9) 

	def OnPickMoney(self, money):
		mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or (app.ENABLE_NEW_EQUIPMENT_SYSTEM and player.IsBeltInventorySlot(local)):
			return local

		if app.ENABLE_SPECIAL_INVENTORY:
			if player.IsSkillBookInventorySlot(local) or player.IsUpgradeItemsInventorySlot(local) or\
				player.IsStoneInventorySlot(local) or player.IsGiftBoxInventorySlot(local) or\
				player.IsChangersInventorySlot(local):
				return local

		return self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	def RefreshBagSlotWindow(self):
		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndItem.SetItemSlot

		if app.ENABLE_PICKUP_ITEM_EFFECT:
			for i in xrange(self.wndItem.GetSlotCount()):
				self.wndItem.DeactivateSlot(i)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if self.interface:
				onTopWindow = self.interface.GetOnTopWindow()

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
			self.wndItem.SetRealSlotNumber(i, slotNumber)
			itemCount = getItemCount(slotNumber)

			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)

			if constInfo.IS_AUTO_POTION(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

				if slotNumber >= player.INVENTORY_PAGE_SIZE*self.inventoryPageIndex:
					slotNumber -= player.INVENTORY_PAGE_SIZE*self.inventoryPageIndex

				isActivated = 0 != metinSocket[0]

				if isActivated:
					if constInfo.IS_AUTO_POTION_HP(itemVnum):
						self.wndItem.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_RED)
					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
						self.wndItem.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_BLUE)

					potionType = 0;
					if constInfo.IS_AUTO_POTION_HP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_HP
					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_SP

					usedAmount = int(metinSocket[1])
					totalAmount = int(metinSocket[2])
					player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))

				else:
					self.wndItem.DeactivateSlot(slotNumber)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				slotNumberChecked = 0
				if not constInfo.IS_AUTO_POTION(itemVnum):
					self.wndItem.DeactivateSlot(i)

				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = acce.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							self.wndItem.ActivateSlot(i, wndMgr.COLOR_TYPE_GREEN)
							if not slotNumber in self.listAttachedAcces:
								self.listAttachedAcces.append(slotNumber)

							slotNumberChecked = 1
					else:
						if slotNumber in self.listAttachedAcces and not slotNumberChecked:
							self.wndItem.DeactivateSlot(i)
							self.listAttachedAcces.remove(slotNumber)

			if app.ENABLE_RENEWAL_AFFECT:
				if constInfo.IS_AFFECT_PLUS(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated = 0 != metinSocket[1]

					if isActivated:
						self.wndItem.ActivateSlot(i, wndMgr.COLOR_TYPE_SKY)
					else:
						self.wndItem.DeactivateSlot(i)

			if app.ENABLE_SLOT_MARKING_SYSTEM:
				if itemVnum and self.interface and onTopWindow:
					if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow, slotNumber):
						if itemVnum == 50200 and uiPrivateShopBuilder.IsBuildingPrivateShop():
							self.wndItem.SetCantMouseEventSlot(i)
						else:
							self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				else:
					self.wndItem.SetUsableSlotOnTopWnd(i)

			if app.ENABLE_GROWTH_PET_SYSTEM:
				if constInfo.IS_GROWTH_PET_ITEM(itemVnum):
					self.__ActivePetHighlightSlot(slotNumber)
					self.__SetCoolTimePetItemSlot(slotNumber, itemVnum)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.INVENTORY, slotNumber) == 0:
					self.wndItem.SetSlotCoverImage(i, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndItem.EnableSlotCoverImage(i, FALSE)

		if app.ENABLE_PICKUP_ITEM_EFFECT:
			self.__HighlightSlot_Refresh()

		self.wndItem.RefreshSlot()

	def RefreshEquipSlotWindow(self):
		getItemVNum = player.GetItemIndex
		getItemCount = player.GetItemCount
		setItemVNum = self.wndEquip.SetItemSlot
		setItemVNum2 = self.wndCostume.SetItemSlot
		setItemVNum3 = self.wndRings.SetItemSlot

		for i in xrange(player.EQUIPMENT_PAGE_COUNT):
			slotNumber = player.EQUIPMENT_SLOT_START + i
			itemCount = getItemCount(slotNumber)

			if itemCount <= 1:
				itemCount = 0

			setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, slotNumber) == 0:
					self.wndEquip.SetSlotCoverImage(slotNumber, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndEquip.EnableSlotCoverImage(slotNumber, FALSE)

		if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			for i in xrange(player.NEW_EQUIPMENT_SLOT_COUNT):
				slotNumber = player.NEW_EQUIPMENT_SLOT_START + i
				itemCount = getItemCount(slotNumber)

				if itemCount <= 1:
					itemCount = 0

				setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
				setItemVNum2(slotNumber, getItemVNum(slotNumber), itemCount)
				setItemVNum3(slotNumber, getItemVNum(slotNumber), itemCount)

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i

			setItemVNum2(slotNumber, getItemVNum(slotNumber), 0)
			setItemVNum3(slotNumber, getItemVNum(slotNumber), 0)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if not player.GetChangeLookVnum(player.EQUIPMENT, slotNumber) == 0:
					self.wndCostume.SetSlotCoverImage(slotNumber, "icon/item/ingame_convert_Mark.tga")
				else:
					self.wndCostume.EnableSlotCoverImage(slotNumber, FALSE)

		self.wndEquip.RefreshSlot()
		self.wndCostume.RefreshSlot()
		self.wndRings.RefreshSlot()

	def RefreshItemSlot(self):
		self.RefreshEquipSlotWindow()
		self.RefreshBagSlotWindow()
		
		# Ýksir penceresi açýksa refresh et
		if self.wndPotions and self.wndPotions.IsShow():
			self.wndPotions.RefreshSlot()

	def RefreshStatus(self):
		if self.wndExpandedMoneyBar:
			self.wndExpandedMoneyBar.RefreshStatus()

	if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
		def OnUpdate(self):
			if constInfo.Inventory_Locked["Active"]:
				if constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)] >= 18:
					for i in xrange(18):
						self.lock[i].Hide()
				else:
					if self.inventoryPageIndex >= 2:
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetUpVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetOverVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetDownVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")

						for i in xrange(9):
							self.lock[i].Show()
							if constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)] > i:
								self.lock[i].Hide()
					else:
						for i in xrange(9):
							self.lock[i].Hide()

					if self.inventoryPageIndex == 3:
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetUpVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetOverVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")
						self.lock[constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)]].SetDownVisual("d:/ymir work/ui/game/windows/ex_inven_cover_button_open.sub")

						for i in xrange(9, 18):
							self.lock[i].Show()
							if constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (0)] > i:
								self.lock[i].Hide()
					else:
						for i in xrange(9, 18):
							self.lock[i].Hide()

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if app.ENABLE_SPECIAL_INVENTORY:
				if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType:

					itemCount = player.GetItemCount(attachedSlotPos)
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()

					self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

					if item.IsRefineScroll(attachedItemIndex):
						self.wndItem.SetUseMode(FALSE)

				elif app.ENABLE_RENEWAL_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()
					net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

				elif app.ENABLE_AURA_COSTUME_SYSTEM and player.SLOT_TYPE_AURA == attachedSlotType:
					net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())

				elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
					mouseModule.mouseController.RunCallBack("INVENTORY")

				elif player.SLOT_TYPE_SHOP == attachedSlotType:
					net.SendShopBuyPacket(attachedSlotPos)

				elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:
					if player.ITEM_MONEY == attachedItemIndex:
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

					else:
						net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

				elif player.SLOT_TYPE_MALL == attachedSlotType:
					net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)
			else:
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					itemCount = player.GetItemCount(attachedSlotPos)
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()
					self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

					if item.IsRefineScroll(attachedItemIndex):
						self.wndItem.SetUseMode(FALSE)

				elif app.ENABLE_RENEWAL_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()
					net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

				elif app.ENABLE_AURA_COSTUME_SYSTEM and player.SLOT_TYPE_AURA == attachedSlotType:
					net.SendAuraRefineCheckOut(attachedSlotPos, player.GetAuraRefineWindowType())

				elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
					mouseModule.mouseController.RunCallBack("INVENTORY")

				elif player.SLOT_TYPE_SHOP == attachedSlotType:
					net.SendShopBuyPacket(attachedSlotPos)

				elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

					if player.ITEM_MONEY == attachedItemIndex:
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

					else:
						net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

				elif player.SLOT_TYPE_MALL == attachedSlotType:
					net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			mouseModule.mouseController.DeattachObject()
			
			# Ýksir slotlarýna item eklendiðinde refresh server'dan güncelleme geldiðinde yapýlacak
			# RefreshItemSlot fonksiyonu zaten server'dan güncelleme geldiðinde çaðrýlýyor

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if app.ENABLE_SPECIAL_INVENTORY:
				if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_CHANGERS_INVENTORY == attachedSlotType:

					self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)
			else:
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()
			
			# Ýksir slotlarýna item eklendiðinde refresh server'dan güncelleme geldiðinde yapýlacak
			# RefreshItemSlot fonksiyonu zaten server'dan güncelleme geldiðinde çaðrýlýyor
		else:
			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)
			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)
			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)
			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)
				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickMoney.Open(itemCount)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if TRUE == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)
			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				if app.ENABLE_RENEWAL_AFFECT:
					if self.__CanAttachThisItem(selectedItemVNum, itemSlotIndex):
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				elif app.ENABLE_GROWTH_PET_SYSTEM:
					if self.__CanAttachGrowthPetItem(selectedItemVNum, itemSlotIndex):
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(TRUE)
				else:
					self.wndItem.SetUseMode(FALSE)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos:
			return

		if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
			if player.GetItemIndex(srcItemSlotPos) == 49999:
				self.AttachBonusToItem(srcItemSlotPos, dstItemSlotPos)
				return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		if item.IsRefineScroll(srcItemVID):
			self.RefineItem(srcItemSlotPos, dstItemSlotPos)
			self.wndItem.SetUseMode(FALSE)

		elif item.IsMetin(srcItemVID) and not item.IsMetin(player.GetItemIndex(dstItemSlotPos)):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif app.ENABLE_EXTEND_TIME_COSTUME_SYSTEM and player.GetItemIndex(srcItemSlotPos) >= 84014 and player.GetItemIndex(srcItemSlotPos) <= 84016:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		else:
			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__IsPetItem(srcItemVID):
					if self.__SendUsePetItemToItemPacket(srcItemVID, srcItemSlotPos, dstItemSlotPos):
						return

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVID):
					if player.CanChangeLookClearItem(srcItemVID, player.INVENTORY, dstItemSlotPos):
						self.__OpenQuestionDialog(srcItemSlotPos, dstItemSlotPos)
						return

			if player.IsEquipmentSlot(dstItemSlotPos):
				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)
			else:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)

			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)

			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice / 5
			else:
				itemPrice = itemPrice * itemCount / 5

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()

			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		if app.ENABLE_AUTO_REFINE:
			constInfo.AUTO_REFINE_TYPE = 1
			constInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
			constInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		return

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_FAILURE_CLEAN)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog("thin")
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			item.SelectItem(targetIndex)
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					self.questionDialog.SetText(localeInfo.ACCE_DO_YOU_CLEAN)
		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)

	def OnUpdate(self):
		if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
			if self.attachBonusDialog:
				if self.attachBonusDialog.IsShow():
					self.attachBonusDialog.Update()

	if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
		def AttachBonusToItem(self, sourceSlotPos, targetSlotPos):
			targetIndex = player.GetItemIndex(targetSlotPos)
			item.SelectItem(targetIndex)

			if item.GetItemType() not in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
				return False

			self.attachBonusDialog.Open(sourceSlotPos, targetSlotPos)
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(FALSE)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(FALSE)

		if app.ENABLE_PICKUP_ITEM_EFFECT:
			self.DelHighlightSlot(overSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()

			if app.ENABLE_SPECIAL_INVENTORY:
				if player.SLOT_TYPE_INVENTORY == attachedItemType or\
					player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedItemType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedItemType or\
					player.SLOT_TYPE_STONE_INVENTORY == attachedItemType or\
					player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedItemType or\
					player.SLOT_TYPE_CHANGERS_INVENTORY == attachedItemType:

					attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

					if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(TRUE)
						self.wndItem.SetUseMode(TRUE)
						self.ShowToolTip(overSlotPos)
						return
			else:
				if player.SLOT_TYPE_INVENTORY == attachedItemType:

					attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
					attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

					if self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPos):
						self.wndItem.SetUsableItem(TRUE)
						self.wndItem.SetUseMode(TRUE)
						self.ShowToolTip(overSlotPos)
						return

		self.ShowToolTip(overSlotPos)

	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if item.IsRefineScroll(srcItemVNum):
			return TRUE
		elif item.IsMetin(srcItemVNum):
			return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			return TRUE
		elif item.IsKey(srcItemVNum):
			return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		elif app.ENABLE_STYLE_ATTRIBUTE_SYSTEM and player.GetItemIndex(srcSlotPos) == 71051:
			if self.__CanPutNewAttribute(dstSlotPos):
				return TRUE
		else:
			if app.ENABLE_EXTEND_TIME_COSTUME_SYSTEM:
				if player.GetItemIndex(srcSlotPos) >= 84014 and player.GetItemIndex(srcSlotPos) <= 84016:
					return TRUE

			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__IsUsablePetItem(srcItemVNum):
					return TRUE

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if item.IsChangeLookClearScroll(srcItemVNum):
					return TRUE

			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return TRUE

		return FALSE

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if srcSlotPos == dstSlotPos and not item.IsMetin(srcItemVNum):
			return FALSE

		elif item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos) or (item.IsMetin(player.GetItemIndex(dstSlotPos)) and player.GetItemIndex(dstSlotPos) == srcItemVNum):
				return TRUE
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return TRUE
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return TRUE
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return TRUE
		elif app.ENABLE_STYLE_ATTRIBUTE_SYSTEM and player.GetItemIndex(srcSlotPos) == 49999:
			return TRUE
		else:
			if app.ENABLE_EXTEND_TIME_COSTUME_SYSTEM:
				if player.GetItemIndex(srcSlotPos) >= 84014 and player.GetItemIndex(srcSlotPos) <= 84016:
					item.SelectItem(player.GetItemIndex(dstSlotPos))
					if item.GetItemType() == item.ITEM_TYPE_COSTUME:
						return TRUE

			if app.ENABLE_GROWTH_PET_SYSTEM:
				if self.__CanUseSrcPetItemToDstPetItem(srcItemVNum, srcSlotPos, dstSlotPos):
					return TRUE

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.CanChangeLookClearItem(srcItemVNum, player.INVENTORY, dstSlotPos):
					return TRUE

			useType = item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return TRUE
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return TRUE
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return TRUE
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return TRUE;
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVNum)
				if item.ITEM_TYPE_ARMOR == item.GetItemType() and item.ARMOR_BELT == item.GetItemSubType():
					return TRUE
			elif "USE_CHANGE_COSTUME_ATTR" == useType and app.ENABLE_BONUS_COSTUME_SYSTEM:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return TRUE;
			elif "USE_RESET_COSTUME_ATTR" == useType and app.ENABLE_BONUS_COSTUME_SYSTEM:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return TRUE;
			elif "USE_PUT_INTO_AURA_SOCKET" == useType and app.ENABLE_AURA_COSTUME_SYSTEM:
				dstItemVnum = player.GetItemIndex(dstSlotPos)
				item.SelectItem(dstItemVnum)

				if item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_AURA == item.GetItemSubType():
					if player.GetItemMetinSocket(dstSlotPos, player.ITEM_SOCKET_AURA_BOOST) == 0:
						return TRUE

		return FALSE

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return FALSE

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return TRUE

		return FALSE

	def __CanChangeItemAttrList(self, dstSlotPos):
		if app.ENABLE_STYLE_ATTRIBUTE_SYSTEM:
			def __CanPutNewAttribute(self, dstSlotPos):
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				if dstItemVNum == 0:
					return FALSE
				item.SelectItem(dstItemVNum)
				if item.GetItemType() not in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
					return FALSE
				return TRUE
			
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return FALSE

			item.SelectItem(dstItemVNum)

			if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
				return FALSE

			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i) != 0:
					return TRUE

			return FALSE

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return FALSE

		if curCount>=maxCount:
			return FALSE

		return TRUE

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return FALSE

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return FALSE

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return FALSE

		return TRUE

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return FALSE

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return FALSE

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i) != 0:
				attrCount += 1

		if attrCount<4:
			return TRUE

		return FALSE

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def UseItemSlot(self, slotIndex):
		if app.ENABLE_AUTO_SELL_SYSTEM and app.IsPressed(app.DIK_LCONTROL) and app.IsPressed(app.DIK_X):
			globalSlot = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
			itemVnum = player.GetItemIndex(globalSlot)
			itemCount = player.GetItemCount(globalSlot)
			item.SelectItem(itemVnum)
			itemType = item.GetItemType()
		
			if itemType == item.ITEM_TYPE_GIFTBOX:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OTOMATIK_SAT_UYARI_SANDIK)
			else:
				if itemCount > 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OTOMATIK_SAT_UYARI)
				else:
					if app.ENABLE_AUTO_SELL_SYSTEM:
						if self.interface and hasattr(self.interface, 'wndAutoSell') and self.interface.wndAutoSell:
							self.interface.wndAutoSell.SendAutoSellAdd(itemVnum)
						else:
							net.SendChatPacket("/autosell_add %d" % itemVnum)
			return

		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if self.isShowAcceWindow():
				acce.Add(player.INVENTORY, slotIndex, 255)
				return

		if exchange.isTrading() and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)):
			item.SelectItem(player.GetItemIndex(slotIndex))
			emptyExchangeSlots = self.GetExchangeEmptyItemPos(item.GetItemSize()[1])
			if emptyExchangeSlots == -1:
				return

			if item.IsAntiFlag(item.ANTIFLAG_GIVE):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANNOT_GIVE)
				return

			net.SendExchangeItemAddPacket(player.SLOT_TYPE_INVENTORY, slotIndex, emptyExchangeSlots[0])
		else:
			self.__UseItem(slotIndex)
			mouseModule.mouseController.DeattachObject()
			self.OverOutItem()

	def GetExchangeEmptyItemPos(self, itemHeight):
		inventorySize = exchange.EXCHANGE_ITEM_MAX_NUM
		inventoryWidth = 4
		GetBlockedSlots = lambda slot, size: [slot+(round*inventoryWidth) for round in xrange(size)] 
		blockedSlots = [element for sublist in [GetBlockedSlots(slot, item.GetItemSize(item.SelectItem(exchange.GetItemVnumFromSelf(slot)))[1]) for slot in xrange(inventorySize) if exchange.GetItemVnumFromSelf(slot) != 0] for element in sublist] 
		freeSlots = [slot for slot in xrange(inventorySize) if not slot in blockedSlots and not TRUE in [e in blockedSlots for e in [slot+(round*inventoryWidth) for round in xrange(itemHeight)]]] 
		return [freeSlots, -1][len(freeSlots) == 0]

	def __GetCurrentItemGrid(self):
		itemGrid = [[FALSE for slot in xrange(player.INVENTORY_PAGE_SIZE)] for page in xrange(player.INVENTORY_PAGE_COUNT)]

		for page in xrange(player.INVENTORY_PAGE_COUNT):
			for slot in xrange(player.INVENTORY_PAGE_SIZE):
				itemVnum = player.GetItemIndex(slot + page * player.INVENTORY_PAGE_SIZE)
				if itemVnum <> 0:
					(w, h) = item.GetItemSize(item.SelectItem(itemVnum))
					for i in xrange(h):
						itemGrid[page][slot + i * 5] = TRUE

		return itemGrid

	def __FindEmptyCellForSize(self, itemGrid, size):
		for page in xrange(player.INVENTORY_PAGE_COUNT):
			for slot in xrange(player.INVENTORY_PAGE_SIZE):
				if itemGrid[page][slot] == FALSE:
					possible = TRUE
					for i in xrange(size):
						p = slot + (i * 5)

						try:
							if itemGrid[page][p] == TRUE:
								possible = FALSE
								break
						except IndexError:
							possible = FALSE
							break

					if possible:
						return slot + page * player.INVENTORY_PAGE_SIZE

		return -1

	def AttachItemFromSafebox(self, slotIndex, itemIndex):
		itemGrid = self.__GetCurrentItemGrid()

		if item.GetItemType(item.SelectItem(itemIndex)) == item.ITEM_TYPE_DS:
			return

		emptySlotIndex = self.__FindEmptyCellForSize(itemGrid, item.GetItemSize()[1])
		if emptySlotIndex <> -1:
			net.SendSafeboxCheckoutPacket(slotIndex, player.INVENTORY, emptySlotIndex)

		return TRUE

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

		if self.interface and (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)) and self.interface.AttachInvenItemToOtherWindowSlot(player.INVENTORY, slotIndex):
			return

		if app.ENABLE_QUICK_SELL_ITEM:
			if app.IsPressed(app.DIK_LCONTROL) and self.IsSellItems(slotIndex):
				if not shop.IsPrivateShop():
					self.__SendSellItemPacket(slotIndex)
					return

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				self.__UseItemAura(slotIndex)
				return

		if app.ENABLE_GROWTH_PET_SYSTEM:
			itemType = item.GetItemType()

			if self.interface.wndPetInfoWindow.AttachItemToPetWindow(player.INVENTORY, slotIndex):
				return

			if self.PetSkillDelDlg and self.PetSkillDelDlg.IsShow():
				self.__PetSkillDeleteQuestionDialogCancel()

			if item.ITEM_TYPE_GROWTH_PET == itemType:
				self.__UseItemPet(slotIndex)
				return

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		elif item.GetItemType() == item.ITEM_TYPE_GIFTBOX and app.ENABLE_VIEW_CHEST_DROP:
			if self.interface:
				if self.interface.dlgChestDrop:
					if not self.interface.dlgChestDrop.IsShow():
						self.interface.dlgChestDrop.Open(slotIndex)
						net.SendChestDropInfo(slotIndex)

		else:
			self.__SendUseItemPacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):

		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):

		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):

		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	if app.ENABLE_QUICK_SELL_ITEM:
		def IsSellItems(self, slotIndex):
			itemVnum = player.GetItemIndex(slotIndex)
			item.SelectItem(itemVnum)
			itemPrice = item.GetISellItemPrice()

			if itemPrice > 1:
				return TRUE

			return FALSE

		def __SendSellItemPacket(self, itemVNum):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
				return

			net.SendItemSellPacket(itemVNum)

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine = wndAcceCombine
			self.wndAcceAbsorption = wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1
			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1
			return 0

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def __AddHighlightSlotChangeLook(self, slotIndex):
			if not slotIndex in self.listHighlightedChangeLookSlot:
				self.listHighlightedChangeLookSlot.append(slotIndex)

		def __DelHighlightSlotChangeLook(self, slotIndex):
			if slotIndex in self.listHighlightedChangeLookSlot:
				if slotIndex >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(slotIndex - (self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(slotIndex)
				self.listHighlightedChangeLookSlot.remove(slotIndex)

	if app.ENABLE_PICKUP_ITEM_EFFECT:
		def ActivateSlot(self, slotindex, type):
			if type == wndMgr.HILIGHTSLOT_MAX:
				return

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.__AddHighlightSlotChangeLook(slotindex)

		def DeactivateSlot(self, slotindex, type):
			if type == wndMgr.HILIGHTSLOT_MAX:
				return

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				self.__DelHighlightSlotChangeLook(slotindex)

		def __HighlightSlot_Refresh(self):
			for i in xrange(self.wndItem.GetSlotCount()):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.ActivateSlot(i, wndMgr.COLOR_TYPE_ORANGE)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					if slotNumber in self.listHighlightedChangeLookSlot:
						self.wndItem.ActivateSlot(i, wndMgr.COLOR_TYPE_RED)

		def __HighlightSlot_Clear(self):
			for i in xrange(self.wndItem.GetSlotCount()):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in self.listHighlightedSlot:
					self.wndItem.DeactivateSlot(i)
					self.listHighlightedSlot.remove(slotNumber)

				if app.ENABLE_CHANGE_LOOK_SYSTEM:
					if slotNumber in self.listHighlightedChangeLookSlot:
						self.wndItem.DeactivateSlot(i)
						self.listHighlightedChangeLookSlot.remove(slotNumber)

		def HighlightSlot(self, slot):
			if slot>player.INVENTORY_PAGE_SIZE*player.INVENTORY_PAGE_COUNT:
				return

			if not slot in self.listHighlightedSlot:
				self.listHighlightedSlot.append(slot)

		def DelHighlightSlot(self, inventorylocalslot):
			if inventorylocalslot in self.listHighlightedSlot:
				if inventorylocalslot >= player.INVENTORY_PAGE_SIZE:
					self.wndItem.DeactivateSlot(inventorylocalslot - (self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE))
				else:
					self.wndItem.DeactivateSlot(inventorylocalslot)

				self.listHighlightedSlot.remove(inventorylocalslot)

	if app.ENABLE_BONUS_COSTUME_SYSTEM:
		def __CanChangeCostumeAttrList(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return FALSE

			item.SelectItem(dstItemVNum)

			if item.ITEM_TYPE_COSTUME != item.GetItemType() and not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON):
				return FALSE

			for i in xrange(player.COSTUME_ATTRIBUTE_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return TRUE

			return FALSE

		def __CanResetCostumeAttr(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return FALSE

			item.SelectItem(dstItemVNum)
			if item.ITEM_TYPE_COSTUME != item.GetItemType() and not item.GetItemSubType() in (item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON):
				return FALSE

			return TRUE

	if app.ENABLE_RENEWAL_AFFECT:
		def __CanAttachThisItem(self, itemVNum, itemSlotIndex):
			if constInfo.IS_AFFECT_PLUS(itemVNum):
				isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
				if isActivated == 1:
					return FALSE
			return TRUE


	def IsDlgQuestionShow(self):
		if self.dlgQuestion.IsShow():
			return TRUE
		else:
			return FALSE

	def CancelDlgQuestion(self):
		self.__Cancel()

	def __OpenQuestionDialog(self, srcItemPos, dstItemPos):
		if self.interface.IsShowDlgQuestionWindow():
			self.interface.CloseDlgQuestionWindow()

		getItemVNum = player.GetItemIndex
		self.srcItemPos = srcItemPos
		self.dstItemPos = dstItemPos

		self.dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.__Accept))
		self.dlgQuestion.SetCancelEvent(ui.__mem_func__(self.__Cancel))

		self.dlgQuestion.SetText1("%s" % item.GetItemName(getItemVNum(srcItemPos)))
		self.dlgQuestion.SetText2(localeInfo.INVENTORY_REALLY_USE_ITEM)

		self.dlgQuestion.Open()

	def __Accept(self):
		self.dlgQuestion.Close()
		self.__SendUseItemToItemPacket(self.srcItemPos, self.dstItemPos)
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)

	def __Cancel(self):
		self.srcItemPos = (0, 0)
		self.dstItemPos = (0, 0)
		self.dlgQuestion.Close()

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def __UseItemAuraQuestionDialog_OnAccept(self):
			self.questionDialog.Close()
			net.SendAuraRefineCheckIn(*(self.questionDialog.srcItem + self.questionDialog.dstItem + (player.GetAuraRefineWindowType(),)))

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAuraQuestionDialog_Close(self):
			self.questionDialog.Close()

			self.questionDialog.srcItem = (0, 0)
			self.questionDialog.dstItem = (0, 0)

		def __UseItemAura(self, slotIndex):
			AuraSlot = player.FineMoveAuraItemSlot()
			UsingAuraSlot = player.FindActivatedAuraSlot(player.INVENTORY, slotIndex)

			AuraVnum = player.GetItemIndex(slotIndex)
			item.SelectItem(AuraVnum)

			if player.GetAuraCurrentItemSlotCount() >= player.AURA_SLOT_MAX <= UsingAuraSlot:
				return

			if player.IsEquipmentSlot(slotIndex):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EQUIPITEM)
				return

			if player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_ABSORB:
				isAbsorbItem = FALSE
				if item.GetItemType() == item.ITEM_TYPE_COSTUME:
					if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
						if player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0:
							if UsingAuraSlot == player.AURA_SLOT_MAX:
								if AuraSlot != player.AURA_SLOT_MAIN:
									return
								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
							return
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return

				elif item.GetItemType() == item.ITEM_TYPE_ARMOR:
					if item.GetItemSubType() in [item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR]:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						isAbsorbItem = TRUE
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
						return
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_ABSORBITEM)
					return

				if isAbsorbItem:
					if UsingAuraSlot == player.AURA_SLOT_MAX:
						if AuraSlot != player.AURA_SLOT_SUB:
							if player.FindUsingAuraSlot(player.AURA_SLOT_SUB) == player.NPOS():
								AuraSlot = player.AURA_SLOT_SUB
							else:
								return

						self.questionDialog = uiCommon.QuestionDialog("thin")
						self.questionDialog.SetText(localeInfo.AURA_NOTICE_DEL_ABSORDITEM)
						self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_OnAccept))
						self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemAuraQuestionDialog_Close))
						self.questionDialog.srcItem = (player.INVENTORY, slotIndex)
						self.questionDialog.dstItem = (player.AURA_REFINE, AuraSlot)
						self.questionDialog.Open()

			elif player.GetAuraRefineWindowType() == player.AURA_WINDOW_TYPE_GROWTH:
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curExp >= player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_GROWTHITEM)
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						if player.FindUsingAuraSlot(player.AURA_SLOT_MAIN) != player.NPOS():
							if item.GetItemType() == item.ITEM_TYPE_RESOURCE:
								if item.GetItemSubType() == item.RESOURCE_AURA:
									if UsingAuraSlot == player.AURA_SLOT_MAX:
										if AuraSlot != player.AURA_SLOT_SUB:
											return

										net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
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
				if UsingAuraSlot == player.AURA_SLOT_MAX:
					if AuraSlot == player.AURA_SLOT_MAIN:
						if item.GetItemType() == item.ITEM_TYPE_COSTUME:
							if item.GetItemSubType() == item.COSTUME_TYPE_AURA:
								socketLevelValue = player.GetItemMetinSocket(slotIndex, player.ITEM_SOCKET_AURA_CURRENT_LEVEL)
								curLevel = (socketLevelValue / 100000) - 1000
								curExp = socketLevelValue % 100000;
								if curLevel >= player.AURA_MAX_LEVEL:
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
									return

								if curLevel != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_LEVEL_MAX) or curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
									chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
									return

								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

								net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())
							else:
								chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
								return
						else:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_AURAITEM)
							return

					elif AuraSlot == player.AURA_SLOT_SUB:
						Cell = player.FindUsingAuraSlot(player.AURA_SLOT_MAIN)
						if Cell == player.NPOS():
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_POSSIBLE_REGISTER_AURAITEM)
							return

						socketLevelValue = player.GetItemMetinSocket(*(Cell + (player.ITEM_SOCKET_AURA_CURRENT_LEVEL,)))
						curLevel = (socketLevelValue / 100000) - 1000
						curExp = socketLevelValue % 100000;

						if curLevel >= player.AURA_MAX_LEVEL:
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_MAX_LEVEL)
							return

						if curExp < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_NEED_EXP):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if AuraVnum != player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_VNUM):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEM)
							return

						if player.GetItemCount(slotIndex) < player.GetAuraRefineInfo(curLevel, player.AURA_REFINE_INFO_MATERIAL_COUNT):
							chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_IMPOSSIBLE_EVOLUTION_ITEMCOUNT)
							return

						if UsingAuraSlot == player.AURA_SLOT_MAX:
							if AuraSlot != player.AURA_SLOT_MAX:
								if player.FindUsingAuraSlot(AuraSlot) != player.NPOS():
									return

							net.SendAuraRefineCheckIn(player.INVENTORY, slotIndex, player.AURA_REFINE, AuraSlot, player.GetAuraRefineWindowType())

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def __UseItemPet(self, slotIndex):
			itemSubType = item.GetItemSubType()
			if item.PET_EGG == itemSubType:
				self.petHatchingWindow.HatchingWindowOpen(player.INVENTORY, slotIndex)

			elif item.PET_UPBRINGING == itemSubType:
				if self.__CanUseGrowthPet(slotIndex):
					self.__SendUseItemPacket(slotIndex)

			elif item.PET_BAG == itemSubType:
				if self.__CanUsePetBagItem(slotIndex):
					if self.questionDialog:
						self.questionDialog.Close()

					self.questionDialog = uiCommon.QuestionDialog("thin")
					self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_PET_BAG_TAKE_OUT)
					self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
					self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
					self.questionDialog.slotIndex = slotIndex
					self.questionDialog.Open()

			if item.PET_SKILL_ALL_DEL_BOOK == itemSubType:
				pet_id = player.GetActivePetItemId()
				if 0 == pet_id:
					return
				(skill_count, pet_skill1, pet_skill_level1, pet_skill_cool1, pet_skill2, pet_skill_level2, pet_skill_cool2, pet_skill3, pet_skill_level3, pet_skill_cool3) = player.GetPetSkill(pet_id)
				if 0 == pet_skill1 and 0 == pet_skill2 and 0 == pet_skill3:
					popup = uiCommon.PopupDialog()
					popup.SetText(localeInfo.PET_EMPTY_SKILL_SLOT_USE_ITEM)
					popup.SetAcceptEvent(self.__OnClosePopupDialog)
					popup.Open()
					self.pop = popup
					return

				self.PetSkillAllDelBookIndex = slotIndex
				self.SetCantMouseEventSlot( self.PetSkillAllDelBookIndex )

				self.PetSkillDelDlg.SetAcceptEvent(ui.__mem_func__(self.__PetSkillDeleteQuestionDialogAccept))
				self.PetSkillDelDlg.SetCancelEvent(ui.__mem_func__(self.__PetSkillDeleteQuestionDialogCancel))

				self.PetSkillDelDlg.SetText1(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG1)
				self.PetSkillDelDlg.SetText2(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG2)
				(w,h) = self.PetSkillDelDlg.GetTextSize1()
				self.PetSkillDelDlg.SetWidth(w+100)
				self.PetSkillDelDlg.Open()

		def __OnClosePopupDialog(self):
			self.pop = None

		def __ActivePetHighlightSlot(self, slotNumber):
			active_id = player.GetActivePetItemId()

			if active_id == player.GetItemMetinSocket(player.INVENTORY, slotNumber, 2):

				if slotNumber >= player.INVENTORY_PAGE_SIZE:
					slotNumber -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				self.wndItem.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)
			else:
				if slotNumber >= player.INVENTORY_PAGE_SIZE:
					slotNumber -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				self.wndItem.DeactivateSlot(slotNumber)

		def __SetCoolTimePetItemSlot(self, slotNumber, itemVnum):

			item.SelectItem(itemVnum)
			itemSubType = item.GetItemSubType()

			if itemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
				return

			if itemSubType == item.PET_BAG:
				id = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 2)
				if id == 0:
					return

			(limitType, limitValue) = item.GetLimit(0)

			if itemSubType == item.PET_UPBRINGING:
				limitValue = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 1)

			if limitType in [item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE]:

				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotNumber, 0)

				remain_time = max(0, sock_time - app.GetGlobalTimeStamp())

				if slotNumber >= player.INVENTORY_PAGE_SIZE:
					slotNumber -= (self.inventoryPageIndex * player.INVENTORY_PAGE_SIZE)

				self.wndItem.SetSlotCoolTimeInverse(slotNumber, limitValue, limitValue - remain_time)

		def __IsPetItem(self, srcItemVID):
			item.SelectItem(srcItemVID)

			if item.GetItemType() == item.ITEM_TYPE_GROWTH_PET:
				return TRUE

			return FALSE

		def __SendUsePetItemToItemPacket(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
			if self.__CanUseSrcPetItemToDstPetItem(srcItemVID, srcItemSlotPos, dstItemSlotPos):
				srcItemVnum = player.GetItemIndex(srcItemSlotPos)
				item.SelectItem( srcItemVnum )
				srcItemType = item.GetItemType()
				srcItemSubType = item.GetItemSubType()

				if item.ITEM_TYPE_GROWTH_PET == srcItemType:
					if srcItemSubType in [item.PET_FEEDSTUFF, item.PET_BAG]:
						self.__OpenPetItemQuestionDialog(srcItemSlotPos, dstItemSlotPos)
					elif item.PET_NAME_CHANGE == srcItemSubType:
						self.__UseItemPetNameChange(srcItemSlotPos, dstItemSlotPos)
				return TRUE

			return FALSE

		def __UseItemPetNameChange(self, srcSlotPos, dstSlotPos):
			if self.petNameChangeWindow:
				self.petNameChangeWindow.NameChangeWindowOpen(srcSlotPos, dstSlotPos)

		def __IsUsablePetItem(self, srcItemVNum):
			item.SelectItem(srcItemVNum)
			srcItemType = item.GetItemType()
			srcItemSubType = item.GetItemSubType()

			if srcItemType != item.ITEM_TYPE_GROWTH_PET:
				return FALSE

			if srcItemSubType not in [item.PET_UPBRINGING, item.PET_FEEDSTUFF, item.PET_BAG, item.PET_NAME_CHANGE]:
				return FALSE

			return TRUE

		def __CanUseSrcPetItemToDstPetItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
			item.SelectItem(srcItemVNum)
			srcItemType = item.GetItemType()
			srcItemSubType = item.GetItemSubType()

			if srcItemType != item.ITEM_TYPE_GROWTH_PET:
				return FALSE

			if srcItemSubType == item.PET_FEEDSTUFF:
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_GROWTH_PET:
					return FALSE

				if dstItemSubType not in [item.PET_UPBRINGING]:
					return FALSE

				if dstItemSubType == item.PET_BAG:
					incaseTime = player.GetItemMetinSocket(dstSlotPos, 1)
					if incaseTime == 0:
						return FALSE

			elif srcItemSubType == item.PET_BAG:
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_GROWTH_PET:
					return FALSE

				if dstItemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
					return FALSE

				lifeTime = player.GetItemMetinSocket(dstSlotPos, 0)

				if dstItemSubType == item.PET_UPBRINGING:
					if lifeTime < app.GetGlobalTimeStamp():
						return FALSE

					srcIncase = player.GetItemMetinSocket(srcSlotPos, 1)
					if srcIncase != 0:
						return FALSE

				elif dstItemSubType == item.PET_BAG:
					if lifeTime > app.GetGlobalTimeStamp():
						return FALSE

					srcIncase = player.GetItemMetinSocket(srcSlotPos, 1)
					if srcIncase != 0:
						return FALSE

					destIncase = player.GetItemMetinSocket(dstSlotPos, 1)
					if destIncase == 0:
						return FALSE

			elif srcItemSubType == item.PET_NAME_CHANGE:
				detIndex = player.GetItemIndex(dstSlotPos)
				item.SelectItem(detIndex)

				dstItemType = item.GetItemType()
				dstItemSubType = item.GetItemSubType()

				if dstItemType != item.ITEM_TYPE_GROWTH_PET:
					return FALSE

				if dstItemSubType not in [item.PET_UPBRINGING]:
					return FALSE

			else:
				return FALSE

			return TRUE

		def __CanUseGrowthPet(self, slotIndex):
			if not player.GetItemMetinSocket(player.INVENTORY, slotIndex, 2):
				return FALSE

			(limitType, limitValue) = item.GetLimit(0)
			remain_time = 999
			if item.LIMIT_REAL_TIME == limitType:
				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 0)
				if app.GetGlobalTimeStamp() > sock_time:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_SUMMON_BECAUSE_LIFE_TIME_END)
					return FALSE

			return TRUE

		def __CanUsePetBagItem(self, slotIndex):
			if not player.GetItemMetinSocket(player.INVENTORY, slotIndex, 2):
				return FALSE

			(limitType, limitValue) = item.GetLimit(0)
			remain_time = 999
			if item.LIMIT_REAL_TIME_START_FIRST_USE == limitType:
				sock_time = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 0)
				use_cnt = player.GetItemMetinSocket(player.INVENTORY, slotIndex, 1)

				if use_cnt:
					if app.GetGlobalTimeStamp() > sock_time:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_USE_BAG)
						return FALSE;

			return TRUE

		def __CanAttachGrowthPetItem(self, itemVNum, itemSlotIndex):
			activePetId = player.GetActivePetItemId()
			if activePetId == 0:
				return TRUE

			item.SelectItem(itemVNum)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()

			if item.ITEM_TYPE_GROWTH_PET == itemType and itemSubType == item.PET_UPBRINGING:
				petId = player.GetItemMetinSocket(itemSlotIndex, 2)
				if petId == activePetId:
					return FALSE

			return TRUE

		def SetPetHatchingWindow(self, window):
			self.petHatchingWindow = window

		def SetPetNameChangeWindow(self, window):
			self.petNameChangeWindow = window

		def SetPetFeedWindow(self, window):
			self.petFeedWindow = window

		def ItemMoveFeedWindow(self, slotWindow, slotIndex):
			if not self.petFeedWindow:
				return

			self.petFeedWindow.ItemMoveFeedWindow(slotWindow, slotIndex)

		def __OpenPetBagQuestionDialog(self, srcItemSlotPos, dstItemSlotPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

		def __OpenPetItemQuestionDialog(self, srcItemPos, dstItemPos):
			if self.interface.IsShowDlgQuestionWindow():
				self.interface.CloseDlgQuestionWindow()

			getItemVNum = player.GetItemIndex
			self.srcItemPos = srcItemPos
			self.dstItemPos = dstItemPos

			srcItemVnum = getItemVNum(srcItemPos)
			dstItemVnum = getItemVNum(dstItemPos)

			item.SelectItem(srcItemVnum)
			src_item_name = item.GetItemName(srcItemVnum)
			srcItemType = item.GetItemType()
			srcItemSubType = item.GetItemSubType()

			item.SelectItem(dstItemVnum)
			dst_item_name = item.GetItemName(getItemVNum(dstItemPos))

			self.PetItemQuestionDlg.SetAcceptEvent(ui.__mem_func__(self.__PetItemAccept))
			self.PetItemQuestionDlg.SetCancelEvent(ui.__mem_func__(self.__PetItemCancel))

			if item.ITEM_TYPE_GROWTH_PET == srcItemType:
				if item.PET_FEEDSTUFF == srcItemSubType:
					self.PetItemQuestionDlg.SetText(localeInfo.INVENTORY_REALLY_USE_PET_FEEDSTUFF_ITEM % (src_item_name, dst_item_name))
					self.PetItemQuestionDlg.Open()

				elif item.PET_BAG == srcItemSubType:
					self.PetItemQuestionDlg.SetText(localeInfo.INVENTORY_REALLY_USE_PET_BAG_ITEM)
					self.PetItemQuestionDlg.Open()

		def __PetItemAccept(self):
			self.PetItemQuestionDlg.Close()
			self.__SendUseItemToItemPacket(self.srcItemPos, self.dstItemPos)
			self.srcItemPos = (0, 0)
			self.dstItemPos = (0, 0)
			self.srcItemWindow = None
			self.dstItemWindow = None

		def __PetItemCancel(self):
			self.srcItemPos = (0, 0)
			self.dstItemPos = (0, 0)
			self.PetItemQuestionDlg.Close()

		def __PetSkillDeleteQuestionDialogAccept(self):
			net.SendPetDeleteAllSkill(self.PetSkillAllDelBookIndex)
			self.__PetSkillDeleteQuestionDialogCancel()

		def __PetSkillDeleteQuestionDialogCancel(self):
			self.PetSkillDelDlg.Close()
			self.SetCanMouseEventSlot(self.PetSkillAllDelBookIndex)
			self.PetSkillAllDelBookIndex = -1

		def SetCanMouseEventSlot(self, inven_slot_index):
			if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
				page = self.GetInventoryPageIndex()
				inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
			self.wndItem.SetCanMouseEventSlot(inven_slot_index)

		def SetCantMouseEventSlot(self, inven_slot_index):
			if inven_slot_index >= player.INVENTORY_PAGE_SIZE:
				page = self.GetInventoryPageIndex()
				inven_slot_index -= (page * player.INVENTORY_PAGE_SIZE)
			self.wndItem.SetCantMouseEventSlot(inven_slot_index)
