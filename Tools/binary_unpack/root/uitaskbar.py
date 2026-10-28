if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import item
import skill
import localeInfo
import wndMgr
import constInfo
import mouseModule
import uiScriptLocale
import uiToolTip

MOUSE_SETTINGS = [0, 0]

def InitMouseButtonSettings(left, right):
	global MOUSE_SETTINGS
	MOUSE_SETTINGS = [left, right]

def SetMouseButtonSetting(dir, event):
	global MOUSE_SETTINGS
	MOUSE_SETTINGS[dir] = event

def GetMouseButtonSettings():
	global MOUSE_SETTINGS
	return MOUSE_SETTINGS

def SaveMouseButtonSettings():
	global MOUSE_SETTINGS
	open("mouse.cfg", "w").write("%s\t%s" % tuple(MOUSE_SETTINGS))

def LoadMouseButtonSettings():
	global MOUSE_SETTINGS
	tokens = open("mouse.cfg", "r").read().split()

	if len(tokens) != 2:
		raise RuntimeError, "MOUSE_SETTINGS_FILE_ERROR"

	MOUSE_SETTINGS[0] = int(tokens[0])
	MOUSE_SETTINGS[1] = int(tokens[1])

def unsigned32(n):
	return n & 0xFFFFFFFFL

#-------------------Giftbox Begin------------------------------

class GiftBox(ui.ScriptWindow):
	class TextToolTip(ui.Window):
		def __init__(self):
			ui.Window.__init__(self, "TOP_MOST")
			self.SetWindowName("GiftBox")
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetOutline()
			textLine.Show()
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 15)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipGift = self.TextToolTip()
		self.tooltipGift.Show()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/giftbox.py")
		except:
			import exception
			exception.Abort("GiftBox.LoadWindow.LoadObject")

		self.giftBoxIcon = self.GetChild("GiftBox_Icon")
		self.giftBoxToolTip = self.GetChild("GiftBox_ToolTip")

	def Destroy(self):
		self.giftBoxIcon = 0
		self.giftBoxToolTip = 0

#-------------------Giftbox End------------------------------

class ExpandedMoneyTaskBar(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.toolTip = None
		self.wndMoney = None
		self.wndMoneySlot = None
		self.wndMoneyIcon = None
		
		# Gem sistemi (mevcut sistem)
		self.Gem = None
		self.wndGemSlotIcon = None
		self.wndGemSlot = None
		
		if app.ENABLE_COINS_INVENTORY:
			self.wndCoins = None
			self.wndCoinsIcon = None
			self.wndCoinsSlot = None
		
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.toolTip = None
		self.wndMoney = None
		self.wndMoneySlot = None
		self.wndMoneyIcon = None
		
		# Gem sistemi (mevcut sistem)
		self.Gem = None
		self.wndGemSlotIcon = None
		self.wndGemSlot = None
		
		if app.ENABLE_COINS_INVENTORY:
			self.wndCoins = None
			self.wndCoinsIcon = None
			self.wndCoinsSlot = None

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ExpandedMoneyTaskbar.py")
		except:
			import exception
			exception.Abort("ExpandedMoneyTaskBar.LoadWindow.LoadObject")
		
		try:
			self.wndMoney = self.GetChild("Money")
			self.wndMoneyIcon = self.GetChild("Money_Icon")
			self.wndMoneySlot = self.GetChild("Money_Slot")
			
			# Gem sistemi (mevcut sistem)
			try:
				self.Gem = self.GetChild("Gem")
				self.wndGemSlotIcon = self.GetChild("Gem_Icon")
				self.wndGemSlot = self.GetChild("Gem_Slot")
			except:
				self.Gem = None
				self.wndGemSlotIcon = None
				self.wndGemSlot = None
			
			if app.ENABLE_COINS_INVENTORY:
				self.wndCoins = self.GetChild("Coins")
				self.wndCoinsIcon = self.GetChild("Coins_Icon")
				self.wndCoinsSlot = self.GetChild("Coins_Slot")
		except:
			import exception
			exception.Abort("ExpandedMoneyTaskBar.LoadWindow.BindObject")

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()
		
		self.wndMoneyIcon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowMoneyTitleToolTip)
		self.wndMoneyIcon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideTitleToolTip)
		
		# Gem sistemi event'leri (mevcut sistem)
		if self.wndGemSlotIcon:
			self.wndGemSlotIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GAYA_NEW)
			self.wndGemSlotIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
		
		if app.ENABLE_COINS_INVENTORY:
			self.wndCoinsIcon.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowCoinsTitleToolTip)
			self.wndCoinsIcon.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideTitleToolTip)

		self.RefreshStatus()

	def __ShowMoneyTitleToolTip(self):
		if not self.toolTip:
			return
		
		self.toolTip.ClearToolTip()
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.AutoAppendNewTextLine(localeInfo.TOOLTIP_EXPANDED_YANG, grp.GenerateColor((255.0/255.0), (255.0/255.0), (102.0/255.0), 1.0))
		self.toolTip.Show()

	if app.ENABLE_COINS_INVENTORY:
		def __ShowCoinsTitleToolTip(self):
			if not self.toolTip:
				return
			
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(localeInfo.TOOLTIP_EXPANDED_COINS, grp.GenerateColor((255.0/255.0), (255.0/255.0), (102.0/255.0), 1.0))
			self.toolTip.Show()

	def __HideTitleToolTip(self):
		if not self.toolTip:
			return
		
		self.toolTip.HideToolTip()

	def EventProgress(self, event_type, arg):
		if "mouse_over_in" == str(event_type):
			if not self.toolTip:
				return
			self.toolTip.ClearToolTip()
			self.toolTip.AlignHorizonalCenter()
			self.toolTip.AutoAppendNewTextLine(arg, grp.GenerateColor((255.0/255.0), (255.0/255.0), (102.0/255.0), 1.0))
			self.toolTip.Show()
		elif "mouse_over_out" == str(event_type):
			if not self.toolTip:
				return
			self.toolTip.HideToolTip()
		else:
			return

	def GetMoneySlot(self):
		if self.wndMoneySlot:
			return self.wndMoneySlot
		else:
			return None

	def GetMoney(self):
		if self.wndMoney:
			return self.wndMoney
		else:
			return None

	def GetMoneyIcon(self):
		if self.wndMoneyIcon:
			return self.wndMoneyIcon
		else:
			return None

	if app.ENABLE_COINS_INVENTORY:
		def GetCoins(self):
			if self.wndCoins:
				return self.wndCoins
			else:
				return None

		def GetCoinsSlot(self):
			if self.wndCoinsSlot:
				return self.wndCoinsSlot
			else:
				return None

		def GetCoinsIcon(self):
			if self.wndCoinsIcon:
				return self.wndCoinsIcon
			else:
				return None

	def Open(self):
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def Close(self):
		self.__HideTitleToolTip()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshStatus(self):
		if self.wndMoney:
			money = player.GetElk()
			self.wndMoney.SetText(localeInfo.NumberToMoneyString(money))
		
		# Gem sistemi (mevcut sistem)
		if self.Gem:
			self.Gem.SetText(localeInfo.NumberToString(player.GetGem()))
		
		if app.ENABLE_COINS_INVENTORY:
			if self.wndCoins:
				coins = player.GetCoins()
				self.wndCoins.SetText(localeInfo.NumberToSecondaryCoinString(coins))

	if app.ENABLE_COINS_INVENTORY:
		def OnUpdate(self):
			# EP text canlý güncelle
			coins = player.GetCoins()
			self.wndCoins.SetText(localeInfo.NumberToSecondaryCoinString(coins))

			# Tooltip, sadece ikonlarýn üzerindeyken görünür olsun.
			if not self.toolTip:
				return

			overMoney = self.wndMoneyIcon and self.wndMoneyIcon.IsIn()
			overCoins = self.wndCoinsIcon and self.wndCoinsIcon.IsIn()

			if not (overMoney or overCoins):
				self.toolTip.HideToolTip()
	else:
		def OnUpdate(self):
			# Gem sistemi güncellemesi (mevcut sistem)
			if self.Gem:
				self.Gem.SetText(localeInfo.NumberToString(player.GetGem()))

class ExpandedTaskBar(ui.ScriptWindow):
	SIDEBAR_TELEPORT = 0
	SIDEBAR_SPECIAL_DEP = 1
	SIDEBAR_OFFLINE_SHOP = 2
	SIDEBAR_SWITCHBOT = 3
	SIDEBAR_POTIONS = 4
	BUTTON_WIKI = 5
	if app.ENABLE_GROWTH_PET_SYSTEM:
		BUTTON_PET_INFO = 6
	if app.ENABLE_HUNTING_SYSTEM:
		BUTTON_HUNTING = 7
	if app.__AUTO_HUNT__:
		BUTTON_AUTO_WINDOW = 8
	if app.ENABLE_AUTO_SELL_SYSTEM:
		BUTTON_AUTO_SELL = 9
	if app.ENABLE_BIOLOGIST_SYSTEM:
		BUTTON_BIOLOGIST = 10

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ExpandedTaskBar")

		self.exclusion_list	= []
		self.sidebarButtonDict = {}
		self.interface = None

	def __del__(self):
		ui.Window.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ExpandedTaskBar.py")
		except:
			import exception
			exception.Abort("ExpandedTaskBar.LoadWindow.LoadObject")

		self.expandedTaskBarBoard = self.GetChild("ExpanedTaskBar_Board")

		self.toggleButtonDict = {}

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_PET_INFO] = self.GetChild("PetInfoButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_PET_INFO].SetParent(self)

		if app.ENABLE_HUNTING_SYSTEM:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_HUNTING] = self.GetChild("HuntingButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_HUNTING].SetParent(self)

		# Sidebar butonlarýný oluþtur
		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_TELEPORT] = self.GetChild("SidebarTeleportButton")
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_TELEPORT].SetParent(self)

		if app.ENABLE_SPECIAL_INVENTORY:
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_SPECIAL_DEP] = self.GetChild("SidebarSpecialDepButton")
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_SPECIAL_DEP].SetParent(self)

		if app.ENABLE_RENEWAL_OFFLINESHOP:
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_OFFLINE_SHOP] = self.GetChild("SidebarOfflineShopButton")
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_OFFLINE_SHOP].SetParent(self)

		if app.ENABLE_RENEWAL_SWITCHBOT:
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_SWITCHBOT] = self.GetChild("SidebarSwitchbotButton")
			self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_SWITCHBOT].SetParent(self)

		self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_POTIONS] = self.GetChild("SidebarPotionsButton")
		self.sidebarButtonDict[ExpandedTaskBar.SIDEBAR_POTIONS].SetParent(self)

		if app.ENABLE_WIKI_SYSTEM:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_WIKI] = self.GetChild("WikiButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_WIKI].SetParent(self)

		if app.__AUTO_HUNT__:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_AUTO_WINDOW] = self.GetChild("AutoButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_AUTO_WINDOW].SetParent(self)

		if app.ENABLE_AUTO_SELL_SYSTEM:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_AUTO_SELL] = self.GetChild("AutoSell")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_AUTO_SELL].SetParent(self)

		if app.ENABLE_BIOLOGIST_SYSTEM:
			try:
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOGIST] = self.GetChild("BiologistButton")
				if self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOGIST]:
					self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOGIST].SetParent(self)
			except:
				import dbg
				dbg.TraceError("BiologistButton not found in ExpandedTaskBar")
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOGIST] = None

		self.RePositionButton()

	def RePositionButton(self):
		button_order = []
		# expandedtaskbar.py'deki sýraya göre: PetInfo, Hunting, Wiki, AutoButton, AutoSell, Biologist
		if app.ENABLE_GROWTH_PET_SYSTEM:
			if not ExpandedTaskBar.BUTTON_PET_INFO in self.exclusion_list:
				button_order.append( ExpandedTaskBar.BUTTON_PET_INFO )

		if app.ENABLE_HUNTING_SYSTEM:
			if not ExpandedTaskBar.BUTTON_HUNTING in self.exclusion_list:
				button_order.append( ExpandedTaskBar.BUTTON_HUNTING )

		if app.ENABLE_WIKI_SYSTEM:
			if not ExpandedTaskBar.BUTTON_WIKI in self.exclusion_list:
				if ExpandedTaskBar.BUTTON_WIKI in self.toggleButtonDict:
					button_order.append( ExpandedTaskBar.BUTTON_WIKI )

		if app.__AUTO_HUNT__:
			if not ExpandedTaskBar.BUTTON_AUTO_WINDOW in self.exclusion_list:
				button_order.append( ExpandedTaskBar.BUTTON_AUTO_WINDOW )

		if app.ENABLE_AUTO_SELL_SYSTEM:
			if not ExpandedTaskBar.BUTTON_AUTO_SELL in self.exclusion_list:
				button_order.append( ExpandedTaskBar.BUTTON_AUTO_SELL )
		if app.ENABLE_BIOLOGIST_SYSTEM:
			if not ExpandedTaskBar.BUTTON_BIOLOGIST in self.exclusion_list:
				if ExpandedTaskBar.BUTTON_BIOLOGIST in self.toggleButtonDict:
					button_order.append( ExpandedTaskBar.BUTTON_BIOLOGIST )

		## Sidebar butonlarýný sýraya ekle
		sidebar_order = []
		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			if ExpandedTaskBar.SIDEBAR_TELEPORT in self.sidebarButtonDict:
				sidebar_order.append( ExpandedTaskBar.SIDEBAR_TELEPORT )
		if app.ENABLE_SPECIAL_INVENTORY:
			if ExpandedTaskBar.SIDEBAR_SPECIAL_DEP in self.sidebarButtonDict:
				sidebar_order.append( ExpandedTaskBar.SIDEBAR_SPECIAL_DEP )
		if app.ENABLE_RENEWAL_OFFLINESHOP:
			if ExpandedTaskBar.SIDEBAR_OFFLINE_SHOP in self.sidebarButtonDict:
				sidebar_order.append( ExpandedTaskBar.SIDEBAR_OFFLINE_SHOP )
		if app.ENABLE_RENEWAL_SWITCHBOT:
			if ExpandedTaskBar.SIDEBAR_SWITCHBOT in self.sidebarButtonDict:
				sidebar_order.append( ExpandedTaskBar.SIDEBAR_SWITCHBOT )
		if ExpandedTaskBar.SIDEBAR_POTIONS in self.sidebarButtonDict:
			sidebar_order.append( ExpandedTaskBar.SIDEBAR_POTIONS )

		## Toplam buton sayýsý (DragonSoul + toggle + sidebar)
		total_count = 1 + len(button_order) + len(sidebar_order)
		quotient = total_count / 2
		remainder = total_count % 2

		## Window pozisyonunu hesapla (eski mantýk)
		window_x = (wndMgr.GetScreenWidth()/2 + 33) + (-36 * quotient)
		if 0 == remainder:
			window_x = window_x + 18

		window_y = wndMgr.GetScreenHeight() - 74
		self.SetPosition( window_x, window_y )

		## Toggle butonlarýn pozisyonunu hesapla
		pos_x = 0
		pos_y = 0
		for key in button_order:
			if not self.toggleButtonDict.has_key(key):
				continue
			if not self.toggleButtonDict[key]:
				continue

			if key in self.exclusion_list:
				continue

			self.toggleButtonDict[key].SetPosition( pos_x, pos_y)
			pos_x = pos_x + 36

		## Sidebar butonlarýn pozisyonunu hesapla (toggle butonlardan sonra)
		for key in sidebar_order:
			if not self.sidebarButtonDict.has_key(key):
				continue
			if not self.sidebarButtonDict[key]:
				continue
			self.sidebarButtonDict[key].SetPosition( pos_x, pos_y)
			pos_x = pos_x + 36

	def SetTop(self):
		super(ExpandedTaskBar, self).SetTop()
		for button in self.toggleButtonDict.values():
			button.SetTop()
		for btn in self.sidebarButtonDict.values():
			btn.SetTop()

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def SetToolTipText(self, eButton, text):
		if eButton in self.toggleButtonDict and self.toggleButtonDict[eButton]:
			self.toggleButtonDict[eButton].SetToolTipText(text)

	def SetToggleButtonEvent(self, eButton, kEventFunc):
		if eButton in self.toggleButtonDict and self.toggleButtonDict[eButton]:
			self.toggleButtonDict[eButton].SetEvent(kEventFunc)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	## Sidebar Buttons
	def SetSidebarButtonEvent(self, eButton, kEventFunc):
		if eButton in self.sidebarButtonDict:
			self.sidebarButtonDict[eButton].SetEvent(kEventFunc)

	def BindInterfaceClass(self, interface):
		"""Interface class'ýný baðla - sidebar butonlarýnýn açýlma fonksiyonlarý için gerekli"""
		self.interface = interface

	## Sidebar Button Click Handlers
	def OnClickSidebarTeleport(self):
		"""Teleport butonuna týklandýðýnda çaðrýlýr"""
		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			if self.interface:
				self.interface.OpenWarpWindow()

	def OnClickSidebarSpecialDep(self):
		"""Special Depository butonuna týklandýðýnda çaðrýlýr"""
		if app.ENABLE_SPECIAL_INVENTORY:
			if self.interface:
				self.interface.ToggleSpecialInventoryWindow()

	def OnClickSidebarOfflineShop(self):
		"""Offline Shop butonuna týklandýðýnda çaðrýlýr"""
		if app.ENABLE_RENEWAL_OFFLINESHOP:
			if self.interface:
				self.interface.ToggleOfflineShopWindow()

	def OnClickSidebarSwitchbot(self):
		"""Switchbot butonuna týklandýðýnda çaðrýlýr"""
		if app.ENABLE_RENEWAL_SWITCHBOT:
			if self.interface:
				self.interface.ToggleSwitchbotWindow()

	def OnClickSidebarPotions(self):
		"""Potions butonuna týklandýðýnda çaðrýlýr"""
		if self.interface and self.interface.wndInventory:
			self.interface.wndInventory.TogglePotionsWindow()

	def OnClickSidebarWikipedia(self):
		"""Wikipedia butonuna týklandýðýnda çaðrýlýr"""
		if app.ENABLE_WIKI_SYSTEM:
			if self.interface:
				self.interface.OpenWikiWindow()

class TaskBar(ui.ScriptWindow):

	BUTTON_CHARACTER = 0
	BUTTON_INVENTORY = 1
	BUTTON_MESSENGER = 2
	BUTTON_SYSTEM = 3
	BUTTON_CHAT = 4
	BUTTON_EXPAND = 5
	BUTTON_EXPAND_MONEY = 6
	IS_EXPANDED = FALSE

	MOUSE_BUTTON_LEFT = 0
	MOUSE_BUTTON_RIGHT = 1
	NONE = 255

	EVENT_MOVE = 0
	EVENT_ATTACK = 1
	EVENT_MOVE_AND_ATTACK = 2
	EVENT_CAMERA = 3
	EVENT_SKILL = 4
	EVENT_AUTO = 5

	GAUGE_WIDTH = 95
	GAUGE_HEIGHT = 13

	QUICKPAGE_NUMBER_FILENAME = [
		"d:/ymir work/ui/game/taskbar/1.sub",
		"d:/ymir work/ui/game/taskbar/2.sub",
		"d:/ymir work/ui/game/taskbar/3.sub",
		"d:/ymir work/ui/game/taskbar/4.sub",
	]

	# Gift icon show and hide
	def ShowGift(self):
		self.wndGiftBox.Show()

	def HideGift(self):
		self.wndGiftBox.Hide()

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
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 15)

	class SkillButton(ui.SlotWindow):
		def __init__(self):
			ui.SlotWindow.__init__(self)

			self.event = 0
			self.arg = 0

			self.slotIndex = 0
			self.skillIndex = 0

			slotIndex = 0
			wndMgr.SetSlotBaseImage(self.hWnd, "d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
			wndMgr.AppendSlot(self.hWnd, slotIndex, 0, 0, 32, 32)
			self.SetCoverButton(slotIndex,	"d:/ymir work/ui/public/slot_cover_button_01.sub",\
											"d:/ymir work/ui/public/slot_cover_button_02.sub",\
											"d:/ymir work/ui/public/slot_cover_button_03.sub",\
											"d:/ymir work/ui/public/slot_cover_button_04.sub", TRUE, FALSE)
			self.SetSize(32, 32)

		def __del__(self):
			ui.SlotWindow.__del__(self)

		def Destroy(self):
			if 0 != self.tooltipSkill:
				self.tooltipSkill.HideToolTip()

		def RefreshSkill(self):
			if 0 != self.slotIndex:
				self.SetSkill(self.slotIndex)

		def SetSkillToolTip(self, tooltip):
			self.tooltipSkill = tooltip

		def SetSkill(self, skillSlotNumber):
			slotNumber = 0
			skillIndex = player.GetSkillIndex(skillSlotNumber)
			skillGrade = player.GetSkillGrade(skillSlotNumber)
			skillLevel = player.GetSkillLevel(skillSlotNumber)
			skillType = skill.GetSkillType(skillIndex)

			self.skillIndex = skillIndex
			if 0 == self.skillIndex:
				self.ClearSlot(slotNumber)
				return

			self.slotIndex = skillSlotNumber

			self.SetSkillSlotNew(slotNumber, skillIndex, skillGrade, skillLevel)
			self.SetSlotCountNew(slotNumber, skillGrade, skillLevel)

			if player.IsSkillCoolTime(skillSlotNumber):
				(coolTime, elapsedTime) = player.GetSkillCoolTime(skillSlotNumber)
				self.SetSlotCoolTime(slotNumber, coolTime, elapsedTime)

			if player.IsSkillActive(skillSlotNumber):
				self.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)

		def SetSkillEvent(self, event, arg=0):
			self.event = event
			self.arg = arg

		def GetSkillIndex(self):
			return self.skillIndex

		def GetSlotIndex(self):
			return self.slotIndex

		def Activate(self, coolTime):
			self.SetSlotCoolTime(0, coolTime)

			if skill.IsToggleSkill(self.skillIndex):
				self.ActivateSlot(0, wndMgr.COLOR_TYPE_WHITE)

		def Deactivate(self):
			if skill.IsToggleSkill(self.skillIndex):
				self.DeactivateSlot(0)

		def OnOverInItem(self, dummy):
			self.tooltipSkill.SetSkill(self.skillIndex)

		def OnOverOutItem(self):
			self.tooltipSkill.HideToolTip()

		def OnSelectItemSlot(self, dummy):
			if 0 != self.event:
				if 0 != self.arg:
					self.event(self.arg)
				else:
					self.event()

	interface = None

	def __init__(self):
		ui.ScriptWindow.__init__(self, "TOP_MOST")

		self.quickPageNumImageBox = None
		self.tooltipItem = 0
		self.tooltipSkill = 0
		self.mouseModeButtonList = [ ui.ScriptWindow("TOP_MOST"), ui.ScriptWindow("TOP_MOST") ]

		self.tooltipHP = self.TextToolTip()
		self.tooltipHP.Show()
		self.tooltipSP = self.TextToolTip()
		self.tooltipSP.Show()
		self.tooltipST = self.TextToolTip()
		self.tooltipST.Show()
		self.tooltipEXP = self.TextToolTip()
		self.tooltipEXP.Show()

		self.skillCategoryNameList = [ "ACTIVE_1", "ACTIVE_2", "ACTIVE_3" ]
		self.skillPageStartSlotIndexDict = {
			"ACTIVE_1" : 1,
			"ACTIVE_2" : 21,
			"ACTIVE_3" : 41,
		}

		self.selectSkillButtonList = []

		self.lastUpdateQuickSlot = 0
		self.SetWindowName("TaskBar")

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/TaskBar.py")
			pyScrLoader.LoadScriptFile(self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT], "UIScript/MouseButtonWindow.py")
			pyScrLoader.LoadScriptFile(self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT], "UIScript/RightMouseButtonWindow.py")
		except:
			import exception
			exception.Abort("TaskBar.LoadWindow.LoadObject")

		self.quickslot = []
		self.quickslot.append(self.GetChild("quick_slot_1"))
		self.quickslot.append(self.GetChild("quick_slot_2"))
		for slot in self.quickslot:
			slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyQuickSlot))
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemQuickSlot))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UnselectItemQuickSlot))
			slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		toggleButtonDict = {}
		toggleButtonDict[TaskBar.BUTTON_CHARACTER] = self.GetChild("CharacterButton")
		toggleButtonDict[TaskBar.BUTTON_INVENTORY] = self.GetChild("InventoryButton")
		toggleButtonDict[TaskBar.BUTTON_MESSENGER] = self.GetChild("MessengerButton")
		toggleButtonDict[TaskBar.BUTTON_SYSTEM] = self.GetChild("SystemButton")
		toggleButtonDict[TaskBar.BUTTON_EXPAND_MONEY]=self.GetChild("ExpandMoneyButton")

		# ChatButton, ExpandButton.
		try:
			toggleButtonDict[TaskBar.BUTTON_CHAT]=self.GetChild("ChatButton")
		except:
			toggleButtonDict[TaskBar.BUTTON_EXPAND]=self.GetChild("ExpandButton")
			TaskBar.IS_EXPANDED = True

		if localeInfo.IsARABIC():
			systemButton = toggleButtonDict[TaskBar.BUTTON_SYSTEM]
			if systemButton.ToolTipText:
				tx, ty = systemButton.ToolTipText.GetLocalPosition()
				tw = systemButton.ToolTipText.GetWidth()
				systemButton.ToolTipText.SetPosition(-tw/2, ty)


		expGauge = []
		expGauge.append(self.GetChild("EXPGauge_01"))
		expGauge.append(self.GetChild("EXPGauge_02"))
		expGauge.append(self.GetChild("EXPGauge_03"))
		expGauge.append(self.GetChild("EXPGauge_04"))

		for exp in expGauge:
			exp.SetSize(0, 0)

		if app.ENABLE_CONQUEROR_LEVEL:
			expConquerorGauge = []
			expConquerorGauge.append(self.GetChild("NewExpGauge_01"))
			expConquerorGauge.append(self.GetChild("NewExpGauge_02"))
			expConquerorGauge.append(self.GetChild("NewExpGauge_03"))
			expConquerorGauge.append(self.GetChild("NewExpGauge_04"))
			
			for expConqueror in expConquerorGauge:
				expConqueror.SetSize(0, 0)

		self.quickPageNumImageBox=self.GetChild("QuickPageNumber")

		self.GetChild("QuickPageUpButton").SetEvent(ui.__mem_func__(self.__OnClickQuickPageUpButton))
		self.GetChild("QuickPageDownButton").SetEvent(ui.__mem_func__(self.__OnClickQuickPageDownButton))

		mouseLeftButtonModeButton = self.GetChild("LeftMouseButton")
		mouseRightButtonModeButton = self.GetChild("RightMouseButton")
		mouseLeftButtonModeButton.SetEvent(ui.__mem_func__(self.ToggleLeftMouseButtonModeWindow))
		mouseRightButtonModeButton.SetEvent(ui.__mem_func__(self.ToggleRightMouseButtonModeWindow))
		self.curMouseModeButton = [ mouseLeftButtonModeButton, mouseRightButtonModeButton ]

		(xLocalRight, yLocalRight) = mouseRightButtonModeButton.GetLocalPosition()
		self.curSkillButton = self.SkillButton()
		self.curSkillButton.SetParent(self)
		self.curSkillButton.SetPosition(xLocalRight, 3)
		self.curSkillButton.SetSkillEvent(ui.__mem_func__(self.ToggleRightMouseButtonModeWindow))
		self.curSkillButton.Hide()

		(xLeft, yLeft) = mouseLeftButtonModeButton.GetGlobalPosition()
		(xRight, yRight) = mouseRightButtonModeButton.GetGlobalPosition()
		leftModeButtonList = self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT]
		leftModeButtonList.SetPosition(xLeft, yLeft - leftModeButtonList.GetHeight()-5)
		rightModeButtonList = self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT]
		rightModeButtonList.SetPosition(xRight - rightModeButtonList.GetWidth() + 32, yRight - rightModeButtonList.GetHeight()-5)
		rightModeButtonList.GetChild("button_skill").SetEvent(lambda adir=self.MOUSE_BUTTON_RIGHT, aevent=self.EVENT_SKILL: self.SelectMouseButtonEvent(adir, aevent))
		rightModeButtonList.GetChild("button_skill").Hide()

		mouseImage = ui.ImageBox("TOP_MOST")
		mouseImage.AddFlag("float")
		mouseImage.LoadImage("d:/ymir work/ui/game/taskbar/mouse_button_camera_01.sub")
		mouseImage.SetPosition(xRight, wndMgr.GetScreenHeight() - 34)
		mouseImage.Hide()
		self.mouseImage = mouseImage

		dir = self.MOUSE_BUTTON_LEFT
		wnd = self.mouseModeButtonList[dir]
		wnd.GetChild("button_move_and_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_MOVE_AND_ATTACK: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_auto_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_AUTO: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_camera").SetEvent(lambda adir=dir, aevent=self.EVENT_CAMERA: self.SelectMouseButtonEvent(adir, aevent))

		dir = self.MOUSE_BUTTON_RIGHT
		wnd = self.mouseModeButtonList[dir]
		wnd.GetChild("button_move_and_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_MOVE_AND_ATTACK: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_camera").SetEvent(lambda adir=dir, aevent=self.EVENT_CAMERA: self.SelectMouseButtonEvent(adir, aevent))

		self.toggleButtonDict = toggleButtonDict
		self.expGauge = expGauge
		if app.ENABLE_CONQUEROR_LEVEL:
			self.expConquerorGauge = expConquerorGauge
		if constInfo.IN_GAME_SHOP_ENABLE:
			self.rampageGauge1  = self.GetChild("RampageGauge")
			self.rampageGauge1.OnMouseOverIn = ui.__mem_func__(self.__RampageGauge_OverIn)
			self.rampageGauge2 = self.GetChild("RampageGauge2")
			self.rampageGauge2.OnMouseOverOut = ui.__mem_func__(self.__RampageGauge_OverOut)
			self.rampageGauge2.OnMouseLeftButtonUp = ui.__mem_func__(self.__RampageGauge_Click)
			print "[DEBUG]: constInfo.IN_GAME_SHOP_ENABLE / self.rampageGauge1",constInfo.IN_GAME_SHOP_ENABLE, self.rampageGauge1
			self.__RampageGauge_OverOut()

		self.hpGauge = self.GetChild("HPGauge")
		self.mpGauge = self.GetChild("SPGauge")
		self.stGauge = self.GetChild("STGauge")
		self.hpRecoveryGaugeBar = self.GetChild("HPRecoveryGaugeBar")
		self.spRecoveryGaugeBar = self.GetChild("SPRecoveryGaugeBar")

		self.hpGaugeBoard = self.GetChild("HPGauge_Board")
		self.mpGaugeBoard = self.GetChild("SPGauge_Board")
		self.stGaugeBoard = self.GetChild("STGauge_Board")
		self.expGaugeBoard = self.GetChild("EXP_Gauge_Board")

		if app.ENABLE_ANTI_EXP:
			self.GetChild("AntiExpButton").SetEvent(ui.__mem_func__(self.__AntiExpButton_Click))

		#giftbox object
		wndGiftBox = GiftBox()
		wndGiftBox.LoadWindow()
		self.wndGiftBox = wndGiftBox

		self.__LoadMouseSettings()
		self.RefreshStatus()
		self.RefreshQuickSlot()

	def __RampageGauge_OverIn(self):
		# print "rampage_over_in"
		self.rampageGauge2.Show()
		self.rampageGauge1.Hide()

	def __RampageGauge_OverOut(self):
		# print "rampage_over_out"
		self.rampageGauge2.Hide()
		self.rampageGauge1.Show()

	def __RampageGauge_Click(self):
		print "rampage_up"

		if app.ENABLE_ITEMSHOP:
			net.SendChatPacket("/in_game_mall")
		else:
			# net.SendChatPacket("/in_game_mall")
			# gift icon hide when click mall icon
			self.wndGiftBox.Hide()
			import event
			constInfo.ITEMSHOP["questCMD"] = 'LOAD#'+str(constInfo.ITEMSHOP['tableUpdate'])
			event.QuestButtonClick(int(constInfo.ITEMSHOP["qid"]))

	if app.ENABLE_ANTI_EXP:
		def __AntiExpButton_Click(self):
			net.SendChatPacket("/anti_exp")

	def __LoadMouseSettings(self):
		try:
			LoadMouseButtonSettings()
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()
			if not self.__IsInSafeMouseButtonSettingRange(mouseLeftButtonEvent) or not self.__IsInSafeMouseButtonSettingRange(mouseRightButtonEvent):
					raise RuntimeError, "INVALID_MOUSE_BUTTON_SETTINGS"
		except:
			InitMouseButtonSettings(self.EVENT_MOVE_AND_ATTACK, self.EVENT_CAMERA)
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()

		try:
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_LEFT, mouseLeftButtonEvent)
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_RIGHT, mouseRightButtonEvent)
		except:
			InitMouseButtonSettings(self.EVENT_MOVE_AND_ATTACK, self.EVENT_CAMERA)
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()

			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_LEFT, mouseLeftButtonEvent)
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_RIGHT, mouseRightButtonEvent)

	def __IsInSafeMouseButtonSettingRange(self, arg):
		return arg >= self.EVENT_MOVE and arg <= self.EVENT_AUTO

	def Destroy(self):
		SaveMouseButtonSettings()

		self.ClearDictionary()
		self.mouseModeButtonList[0].ClearDictionary()
		self.mouseModeButtonList[1].ClearDictionary()
		self.mouseModeButtonList = 0
		self.curMouseModeButton = 0
		self.curSkillButton = 0
		self.selectSkillButtonList = 0

		self.expGauge = None
		if app.ENABLE_CONQUEROR_LEVEL:
			self.expConquerorGauge = None
		self.hpGauge = None
		self.mpGauge = None
		self.stGauge = None
		self.hpRecoveryGaugeBar = None
		self.spRecoveryGaugeBar = None

		self.tooltipItem = 0
		self.tooltipSkill = 0
		self.quickslot = 0
		self.toggleButtonDict = 0

		self.hpGaugeBoard = 0
		self.mpGaugeBoard = 0
		self.stGaugeBoard = 0

		self.expGaugeBoard = 0

		self.tooltipHP = 0
		self.tooltipSP = 0
		self.tooltipST = 0
		self.tooltipEXP = 0

		self.mouseImage = None

	def __OnClickQuickPageUpButton(self):
		player.SetQuickPage(player.GetQuickPage()-1)

	def __OnClickQuickPageDownButton(self):
		player.SetQuickPage(player.GetQuickPage()+1)

	def SetToggleButtonEvent(self, eButton, kEventFunc):
		if eButton in self.toggleButtonDict and self.toggleButtonDict[eButton]:
			self.toggleButtonDict[eButton].SetEvent(kEventFunc)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetSkillToolTip(self, tooltipSkill):
		self.tooltipSkill = tooltipSkill
		self.curSkillButton.SetSkillToolTip(self.tooltipSkill)

	## Mouse Image
	def ShowMouseImage(self):
		self.mouseImage.SetTop()
		self.mouseImage.Show()

	def HideMouseImage(self):
		player.SetQuickCameraMode(FALSE)
		self.mouseImage.Hide()

	## Gauge
	def RefreshStatus(self):
		curHP = player.GetStatus(player.HP)
		maxHP = player.GetStatus(player.MAX_HP)
		curSP = player.GetStatus(player.SP)
		maxSP = player.GetStatus(player.MAX_SP)
		if app.ENABLE_CONQUEROR_LEVEL:
			if player.GetStatus(player.CONQUEROR_LEVEL) >= 1:
				curEXP = unsigned32(player.GetStatus(player.CONQUEROR_EXP))
				nextEXP = unsigned32(player.GetStatus(player.CONQUEROR_NEXT_EXP))
			else:
				curEXP = unsigned32(player.GetStatus(player.EXP))
				nextEXP = unsigned32(player.GetStatus(player.NEXT_EXP))			
		else:
			curEXP = unsigned32(player.GetStatus(player.EXP))
			nextEXP = unsigned32(player.GetStatus(player.NEXT_EXP))
		recoveryHP = player.GetStatus(player.HP_RECOVERY)
		recoverySP = player.GetStatus(player.SP_RECOVERY)

		self.RefreshStamina()

		self.SetHP(curHP, recoveryHP, maxHP)
		self.SetSP(curSP, recoverySP, maxSP)
		if app.ENABLE_CONQUEROR_LEVEL:
			if player.GetStatus(player.CONQUEROR_LEVEL) >= 1:
				self.SetConquerorExperience(curEXP, nextEXP)
			else:
				self.SetExperience(curEXP, nextEXP)
		else:
			self.SetExperience(curEXP, nextEXP)

	def RefreshStamina(self):
		curST = player.GetStatus(player.STAMINA)
		maxST = player.GetStatus(player.MAX_STAMINA)
		self.SetST(curST, maxST)

	def RefreshSkill(self):
		self.curSkillButton.RefreshSkill()
		for button in self.selectSkillButtonList:
			button.RefreshSkill()

	def SkillClearCoolTime(self, usedSlotIndex):
		QUICK_SLOT_SLOT_COUNT = 4
		slotIndex = 0
		for slotWindow in self.quickslot:
			for i in xrange(QUICK_SLOT_SLOT_COUNT):
				(Type, Position) = player.GetLocalQuickSlot(slotIndex)
				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.SetSlotCoolTime(slotIndex, 0)
						return
				slotIndex += 1

	def SetHP(self, curPoint, recoveryPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.hpGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipHP.SetText("%s : %d / %d" % (localeInfo.TASKBAR_HP, curPoint, maxPoint))

			if 0 == recoveryPoint:
				self.hpRecoveryGaugeBar.Hide()
			else:
				destPoint = min(maxPoint, curPoint + recoveryPoint)
				newWidth = int(self.GAUGE_WIDTH * (float(destPoint) / float(maxPoint)))
				self.hpRecoveryGaugeBar.SetSize(newWidth, self.GAUGE_HEIGHT)
				self.hpRecoveryGaugeBar.Show()

	def SetSP(self, curPoint, recoveryPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.mpGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipSP.SetText("%s : %d / %d" % (localeInfo.TASKBAR_SP, curPoint, maxPoint))

			if 0 == recoveryPoint:
				self.spRecoveryGaugeBar.Hide()
			else:
				destPoint = min(maxPoint, curPoint + recoveryPoint)
				newWidth = int(self.GAUGE_WIDTH * (float(destPoint) / float(maxPoint)))
				self.spRecoveryGaugeBar.SetSize(newWidth, self.GAUGE_HEIGHT)
				self.spRecoveryGaugeBar.Show()

	def SetST(self, curPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.stGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipST.SetText("%s : %d / %d" % (localeInfo.TASKBAR_ST, curPoint, maxPoint))

	def SetExperience(self, curPoint, maxPoint):
		if app.ENABLE_CONQUEROR_LEVEL:
			for i in xrange(4):
				self.expConquerorGauge[i].Hide()
		curPoint = min(curPoint, maxPoint)
		curPoint = max(curPoint, 0)
		maxPoint = max(maxPoint, 0)

		quarterPoint = maxPoint / 4
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)

		for i in xrange(4):
			self.expGauge[i].Hide()

		for i in xrange(FullCount):
			self.expGauge[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.expGauge[i].Show()

		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.expGauge[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.expGauge[FullCount].Show()

		#####
		self.tooltipEXP.SetText("%s : %.2f%%" % (localeInfo.TASKBAR_EXP, float(curPoint) / max(1, float(maxPoint)) * 100))

	if app.ENABLE_CONQUEROR_LEVEL:
		def SetConquerorExperience(self, curPoint, maxPoint):
		
			for i in xrange(4):
				self.expGauge[i].Hide()

			curPoint = min(curPoint, maxPoint)
			curPoint = max(curPoint, 0)
			maxPoint = max(maxPoint, 0)

			quarterPoint = maxPoint / 4
			FullCount = 0

			if 0 != quarterPoint:
				FullCount = min(4, curPoint / quarterPoint)

			for i in xrange(4):
				self.expConquerorGauge[i].Hide()

			for i in xrange(FullCount):
				self.expConquerorGauge[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
				self.expConquerorGauge[i].Show()

			if 0 != quarterPoint:
				if FullCount < 4:
					Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
					self.expConquerorGauge[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
					self.expConquerorGauge[FullCount].Show()

			#####
			self.tooltipEXP.SetText("%s : %.2f%%" % (localeInfo.TASKBAR_EXP, float(curPoint) / max(1, float(maxPoint)) * 100))

	## QuickSlot
	def RefreshQuickSlot(self):
		pageNum = player.GetQuickPage()

		try:
			self.quickPageNumImageBox.LoadImage(TaskBar.QUICKPAGE_NUMBER_FILENAME[pageNum])
		except:
			pass

		startNumber = 0
		for slot in self.quickslot:
			for i in xrange(4):

				slotNumber = i+startNumber

				(Type, Position) = player.GetLocalQuickSlot(slotNumber)

				if player.SLOT_TYPE_NONE == Type:
					slot.ClearSlot(slotNumber)
					continue

				if player.SLOT_TYPE_INVENTORY == Type:

					itemIndex = player.GetItemIndex(Position)
					itemCount = player.GetItemCount(Position)
					if itemCount <= 1:
						itemCount = 0

					if constInfo.IS_AUTO_POTION(itemIndex):
						metinSocket = [player.GetItemMetinSocket(Position, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

						if 0 != int(metinSocket[0]):
							slot.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)
						else:
							slot.DeactivateSlot(slotNumber)

					if app.ENABLE_GROWTH_PET_SYSTEM:
						if constInfo.IS_GROWTH_PET_ITEM(itemIndex):
							self.__SetCoolTimePetItemSlot(slot, Position, slotNumber, itemIndex)

							slot.DeactivateSlot(slotNumber)

							active_id = player.GetActivePetItemId()
							if active_id and active_id == player.GetItemMetinSocket(Position, 2):
								slot.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)

					slot.SetItemSlot(slotNumber, itemIndex, itemCount)

				elif player.SLOT_TYPE_SKILL == Type:

					skillIndex = player.GetSkillIndex(Position)
					if 0 == skillIndex:
						slot.ClearSlot(slotNumber)
						continue

					skillType = skill.GetSkillType(skillIndex)
					if skill.SKILL_TYPE_GUILD == skillType:
						import guild
						skillGrade = 0
						skillLevel = guild.GetSkillLevel(Position)

					else:
						skillGrade = player.GetSkillGrade(Position)
						skillLevel = player.GetSkillLevel(Position)

					slot.SetSkillSlotNew(slotNumber, skillIndex, skillGrade, skillLevel)
					slot.SetSlotCountNew(slotNumber, skillGrade, skillLevel)
					slot.SetCoverButton(slotNumber)

					## NOTE : CoolTime
					if player.IsSkillCoolTime(Position):
						(coolTime, elapsedTime) = player.GetSkillCoolTime(Position)
						slot.SetSlotCoolTime(slotNumber, coolTime, elapsedTime)

					## NOTE : Activate
					if player.IsSkillActive(Position):
						slot.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)

				elif player.SLOT_TYPE_EMOTION == Type:

					emotionIndex = Position
					slot.SetEmotionSlot(slotNumber, emotionIndex)
					slot.SetCoverButton(slotNumber)
					slot.SetSlotCount(slotNumber, 0)

			slot.RefreshSlot()
			startNumber += 4

	def canAddQuickSlot(self, Type, slotNumber):
		if app.ENABLE_SPECIAL_INVENTORY:
			if player.SLOT_TYPE_INVENTORY == Type or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == Type or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == Type or\
				player.SLOT_TYPE_STONE_INVENTORY == Type or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == Type or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == Type:

				itemIndex = player.GetItemIndex(slotNumber)
				return item.CanAddToQuickSlotItem(itemIndex)
		else:
			if player.SLOT_TYPE_INVENTORY == Type:
				itemIndex = player.GetItemIndex(slotNumber)
				return item.CanAddToQuickSlotItem(itemIndex)

		return TRUE

	def AddQuickSlot(self, localSlotIndex):
		AttachedSlotType = mouseModule.mouseController.GetAttachedType()
		AttachedSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
		AttachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

		if player.SLOT_TYPE_QUICK_SLOT == AttachedSlotType:
			player.RequestMoveGlobalQuickSlotToLocalQuickSlot(AttachedSlotNumber, localSlotIndex)

		elif player.SLOT_TYPE_EMOTION == AttachedSlotType:
			player.RequestAddLocalQuickSlot(localSlotIndex, AttachedSlotType, AttachedItemIndex)

		elif TRUE == self.canAddQuickSlot(AttachedSlotType, AttachedSlotNumber):

			## Online Code
			player.RequestAddLocalQuickSlot(localSlotIndex, AttachedSlotType, AttachedSlotNumber)

		mouseModule.mouseController.DeattachObject()
		self.RefreshQuickSlot()

	def SelectEmptyQuickSlot(self, slotIndex):
		if TRUE == mouseModule.mouseController.isAttached():
			self.AddQuickSlot(slotIndex)

	def SelectItemQuickSlot(self, localQuickSlotIndex):
		if TRUE == mouseModule.mouseController.isAttached():
			self.AddQuickSlot(localQuickSlotIndex)

		else:
			globalQuickSlotIndex=player.LocalQuickSlotIndexToGlobalQuickSlotIndex(localQuickSlotIndex)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_QUICK_SLOT, globalQuickSlotIndex, globalQuickSlotIndex)

	def UnselectItemQuickSlot(self, localSlotIndex):
		if FALSE == mouseModule.mouseController.isAttached():
			player.RequestUseLocalQuickSlot(localSlotIndex)
			return

		elif mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()
			return

	def OnUseSkill(self, usedSlotIndex, coolTime):
		QUICK_SLOT_SLOT_COUNT = 4
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Activate(coolTime)

		## Quick Slot
		for slotWindow in self.quickslot:
			for i in xrange(QUICK_SLOT_SLOT_COUNT):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.SetSlotCoolTime(slotIndex, coolTime)
						return

				slotIndex += 1

	def OnActivateSkill(self, usedSlotIndex):
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Deactivate()

		## Quick Slot
		for slotWindow in self.quickslot:
			for i in xrange(4):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.ActivateSlot(slotIndex, wndMgr.COLOR_TYPE_WHITE)
						return

				slotIndex += 1

	def OnDeactivateSkill(self, usedSlotIndex):
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Deactivate()

		## Quick Slot
		for slotWindow in self.quickslot:
			for i in xrange(4):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.DeactivateSlot(slotIndex)
						return

				slotIndex += 1

	## ToolTip
	def OverInItem(self, slotNumber):
		if mouseModule.mouseController.isAttached():
			return

		(Type, Position) = player.GetLocalQuickSlot(slotNumber)

		if player.SLOT_TYPE_INVENTORY == Type:
			self.tooltipItem.SetInventoryItem(Position)
			self.tooltipSkill.HideToolTip()

		elif player.SLOT_TYPE_SKILL == Type:

			skillIndex = player.GetSkillIndex(Position)
			skillType = skill.GetSkillType(skillIndex)

			if skill.SKILL_TYPE_GUILD == skillType:
				import guild
				skillGrade = 0
				skillLevel = guild.GetSkillLevel(Position)

			else:
				skillGrade = player.GetSkillGrade(Position)
				skillLevel = player.GetSkillLevel(Position)

			self.tooltipSkill.SetSkillNew(Position, skillIndex, skillGrade, skillLevel)
			self.tooltipItem.HideToolTip()

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		if 0 != self.tooltipSkill:
			self.tooltipSkill.HideToolTip()

	def OnUpdate(self):
		if app.GetGlobalTime() - self.lastUpdateQuickSlot > 500:
			self.lastUpdateQuickSlot = app.GetGlobalTime()
			self.RefreshQuickSlot()

		if TRUE == self.hpGaugeBoard.IsIn():
			self.tooltipHP.Show()
		else:
			self.tooltipHP.Hide()

		if TRUE == self.mpGaugeBoard.IsIn():
			self.tooltipSP.Show()
		else:
			self.tooltipSP.Hide()

		if TRUE == self.stGaugeBoard.IsIn():
			self.tooltipST.Show()
		else:
			self.tooltipST.Hide()

		if TRUE == self.expGaugeBoard.IsIn():
			self.tooltipEXP.Show()
		else:
			self.tooltipEXP.Hide()

	## Skill
	def ToggleLeftMouseButtonModeWindow(self):

		wndMouseButtonMode = self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT]

		if TRUE == wndMouseButtonMode.IsShow():

			wndMouseButtonMode.Hide()

		else:
			wndMouseButtonMode.Show()

	def ToggleRightMouseButtonModeWindow(self):

		wndMouseButtonMode = self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT]

		if TRUE == wndMouseButtonMode.IsShow():

			wndMouseButtonMode.Hide()
			self.CloseSelectSkill()

		else:
			wndMouseButtonMode.Show()
			self.OpenSelectSkill()

	def OpenSelectSkill(self):

		PAGE_SLOT_COUNT = 6

		(xSkillButton, y) = self.curSkillButton.GetGlobalPosition()
		y -= (37 + 32 + 1)

		for key in self.skillCategoryNameList:

			appendCount = 0
			startNumber = self.skillPageStartSlotIndexDict[key]
			x = xSkillButton

			getSkillIndex=player.GetSkillIndex
			getSkillLevel=player.GetSkillLevel
			for i in range(PAGE_SLOT_COUNT):

				skillIndex = getSkillIndex(startNumber+i)
				skillLevel = getSkillLevel(startNumber+i)

				if 0 == skillIndex:
					continue
				if 0 == skillLevel:
					continue
				if skill.IsStandingSkill(skillIndex):
					continue

				skillButton = self.SkillButton()
				skillButton.SetSkill(startNumber+i)
				skillButton.SetPosition(x, y)
				skillButton.SetSkillEvent(ui.__mem_func__(self.CloseSelectSkill), startNumber+i+1)
				skillButton.SetSkillToolTip(self.tooltipSkill)
				skillButton.SetTop()
				skillButton.Show()
				self.selectSkillButtonList.append(skillButton)

				appendCount += 1
				x -= 32

			if appendCount > 0:
				y -= 32

	def CloseSelectSkill(self, slotIndex=-1):
		self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT].Hide()
		for button in self.selectSkillButtonList:
			button.Destroy()

		self.selectSkillButtonList = []

		if -1 != slotIndex:
			self.curSkillButton.Show()
			self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Hide()
			player.SetMouseFunc(player.MBT_RIGHT, player.MBF_SKILL)
			player.ChangeCurrentSkillNumberOnly(slotIndex-1)
		else:
			self.curSkillButton.Hide()
			self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Show()

	def SelectMouseButtonEvent(self, dir, event):
		SetMouseButtonSetting(dir, event)

		self.CloseSelectSkill()
		self.mouseModeButtonList[dir].Hide()

		btn = 0
		type = self.NONE
		func = self.NONE
		tooltip_text = ""

		if self.MOUSE_BUTTON_LEFT == dir:
			type = player.MBT_LEFT

		elif self.MOUSE_BUTTON_RIGHT == dir:
			type = player.MBT_RIGHT

		if self.EVENT_MOVE == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_move")
			func = player.MBF_MOVE
			tooltip_text = localeInfo.TASKBAR_MOVE
		elif self.EVENT_ATTACK == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_attack")
			func = player.MBF_ATTACK
			tooltip_text = localeInfo.TASKBAR_ATTACK
		elif self.EVENT_AUTO == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_auto_attack")
			func = player.MBF_AUTO
			tooltip_text = localeInfo.TASKBAR_AUTO
		elif self.EVENT_MOVE_AND_ATTACK == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_move_and_attack")
			func = player.MBF_SMART
			player.ClearAutoAttackTargetActorID()
			tooltip_text = localeInfo.TASKBAR_ATTACK
		elif self.EVENT_CAMERA == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_camera")
			func = player.MBF_CAMERA
			tooltip_text = localeInfo.TASKBAR_CAMERA
		elif self.EVENT_SKILL == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_skill")
			func = player.MBF_SKILL
			tooltip_text = localeInfo.TASKBAR_SKILL

		if 0 != btn:
			self.curMouseModeButton[dir].SetToolTipText(tooltip_text, 0, -18)
			self.curMouseModeButton[dir].SetUpVisual(btn.GetUpVisualFileName())
			self.curMouseModeButton[dir].SetOverVisual(btn.GetOverVisualFileName())
			self.curMouseModeButton[dir].SetDownVisual(btn.GetDownVisualFileName())
			self.curMouseModeButton[dir].Show()

		player.SetMouseFunc(type, func)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.curSkillButton.SetSkill(skillSlotNumber)
		self.curSkillButton.Show()
		self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Hide()

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def __SetCoolTimePetItemSlot(self, slot, Position, slotNumber, itemVnum):
			item.SelectItem(itemVnum)
			itemSubType = item.GetItemSubType()

			if itemSubType not in [item.PET_UPBRINGING, item.PET_BAG]:
				return

			if itemSubType == item.PET_BAG:
				id = player.GetItemMetinSocket(Position, 2)
				if id == 0:
					return

			(limitType, limitValue) = item.GetLimit(0)

			if itemSubType == item.PET_UPBRINGING:
				limitValue = player.GetItemMetinSocket(Position, 1)

			if limitType in [item.LIMIT_REAL_TIME, item.LIMIT_REAL_TIME_START_FIRST_USE]:
				sock_time = player.GetItemMetinSocket(Position, 0)
				remain_time = max(0, sock_time - app.GetGlobalTimeStamp())

				slot.SetSlotCoolTimeInverse(slotNumber, limitValue, limitValue - remain_time)
