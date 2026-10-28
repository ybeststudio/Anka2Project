if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import constInfo
import systemSetting
import wndMgr
import chat
import uiTaskBar
import uiCharacter
import uiInventory
import uiChat
import uiMessenger
import guild

import ui
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiSystem
import uiRestart
import uiToolTip
import uidropinfo
import uishootingstars
import uiMiniMap
import uiParty
import uiSafebox
import uiGuild
import uiQuest
import uiPrivateShopBuilder
import uiCommon
import uiRefine
import uiYinAndYangAttr
import uiEquipmentDialog
import uiGameButton
import uiTip
import uiCube
import miniMap
import uiNameReinforce
import uiselectitem
import uiScriptLocale
import shop
import event
import localeInfo

if app.ENABLE_GUILD_RANK_SYSTEM:
	import uiGuildRanking

if app.ENABLE_RENEWAL_SWITCHBOT:
	import uiSwitchbot

if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import uiAcce

if app.ENABLE_VIEW_CHEST_DROP:
	import uiChestDrop

if app.ENABLE_SPECIAL_INVENTORY:
	import uiSpecialInventory

if app.ENABLE_EVENT_SYSTEM:
	import uiEvent

if app.ENABLE_MINI_GAME_OKEY:
	import uiMiniGameRumi

if app.ENABLE_RENDER_TARGET:
	import uiRenderTarget

if app.ENABLE_RENEWAL_OFFLINESHOP:
	import uiOfflineShopBuilder
	import uiOfflineShop

if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
	import uiPrivateShopSearch

if app.ENABLE_WIKI_SYSTEM:
	import uiWiki

if app.ENABLE_RENEWAL_SKILL_SELECT:
	import uiSkillSelect

if app.ENABLE_NEW_DUNGEON_LIB:
	import uiDungeonTimer

if app.ENABLE_TRACK_WINDOW:
	import uiTrack

if app.ENABLE_ITEMSHOP:
	import uiItemShopNew

if app.ENABLE_AURA_COSTUME_SYSTEM:
	import uiAura

if app.ENABLE_GUILDBANK_LOG:
	import uiGuildBank
	
if app.ENABLE_FISH_GAME:
	import uiFishGame

if app.ENABLE_GROWTH_PET_SYSTEM:
	import uiPetInfo
import uiCompanion
import uiSkillUpgrade

if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
	import uiTeleport

if app.ENABLE_SUNG_MAHI_TOWER:
	import uiSungMahi

if app.LINK_IN_CHAT:
	import os

if app.ENABLE_DUNGEON_INFO:
	import uiDungeonInfo
	import uitabledungeon

if app.__DUNGEON_INFO__:
	import uiDungeonTrack

if app.ENABLE_RENEWAL_SPECIAL_CHAT:
	import uiSpecialChat

if app.ENABLE_CHANGE_LOOK_SYSTEM:
	import uiChangeLook

if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
	import uiItemCombination

if app.ENABLE_HUNTING_SYSTEM:
	import uiHunting

if app.ENABLE_RIDING_EXTENDED:
	import uiMountUpGrade

if app.__AUTO_HUNT__:
	import uiAutoHunt

if app.ENABLE_AUTO_SELL_SYSTEM:
	import uiautosell

if app.ENABLE_BIOLOGIST_SYSTEM:
	import uiBiyolog

if app.ENABLE_ZODIAC_MISSION:
	import ui12zi

import uiitemfinder

if app.ENABLE_RESP_SYSTEM:
	import uiresp

if app.ENABLE_COLLECT_WINDOW:
	import uiCollectWindow

if app.ENABLE_COLLECTIONS_SYSTEM:
	import uiCollections

if app.ENABLE_SOUL_ROULETTE_SYSTEM:
	import uiMiniGameRoulette

if app.ENABLE_MINI_GAME_CATCH_KING:
	import uiminigamecatchking

if app.ENABLE_EVENT_SYSTEM:
	import uiMiniGame

if app.ENABLE_ATTENDANCE_EVENT:
	import uiMiniGameAttendance

if app.ENABLE_FISH_EVENT_SYSTEM:
	import uiMiniGameFishEvent

if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
	import uihalloween

if app.ENABLE_GAYA_SYSTEM:
	import uiGemShop
	import uiselectitemEx

if app.ENABLE_GAYA_TICKET_SYSTEM:
	import uiGemTicket

if app.ENABLE_BATTLE_PASS:
	import uibattlepass
	import uibattlepasspremium

if app.ENABLE_RANKING:
	import uiranking

IsQBHide = 0
class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2

	def __init__(self):
		systemSetting.SetInterfaceHandler(self)

		if app.__DUNGEON_INFO__:
			self.wndDungeonTrack = None
			self.wndDungeonManager = None

		if app.ENABLE_TRACK_WINDOW:
			self.wndTrackWindow = None

		self.windowOpenPosition = 0
		self.dlgWhisperWithoutTarget = None
		self.uiAffectshower = None
		self.uitargetBoard = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None

		if app.ENABLE_ZODIAC_MISSION:
			self.wnd12ziReward = None
			self.wnd12ziTimer = None

		if app.ENABLE_DUNGEON_INFO:
			self.wndDungeonInfo = None

		if app.ENABLE_WIKI_SYSTEM:
			self.wndWiki = None

		if app.ENABLE_ITEMSHOP:
			self.wndItemShop=None

		if app.ENABLE_RENEWAL_OX_EVENT:
			self.bigBoardControl = None

		if app.ENABLE_AUTO_SELL_SYSTEM:
			self.wndAutoSell = None

		# ITEM_MALL
		self.mallPageDlg = None
		# END_OF_ITEM_MALL

		self.wndWeb = None
		self.wndTaskBar = None
		self.wndCharacter = None
		self.wndInventory = None
		self.wndExpandedMoney = None
		self.wndExpandedTaskBar = None
		self.wndCompanion = None
		self.wndSkillUpgrade = None
		self.wndChat = None
		self.wndMessenger = None
		self.wndMiniMap = None
		self.wndGuild = None
		self.wndGuildBuilding = None

		if app.ENABLE_EVENT_SYSTEM:
			self.wndMiniGame = None

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			self.wndMiniGameRoulette = None

		if app.ENABLE_COLLECTIONS_SYSTEM:
			self.wndCollections = None

		if app.ENABLE_RESP_SYSTEM:
			self.wndResp = None

		if app.ENABLE_COLLECT_WINDOW:
			self.wndCollectWindow = None

		if app.ENABLE_RENEWAL_SWITCHBOT:
			self.wndSwitchbot = None

		if app.ENABLE_YIN_AND_YANG_ATTR:
			self.wndYinAndYangInventory = None
			self.wndYinAndYangHarmony = None

		if app.ENABLE_SUNG_MAHI_TOWER:
			self.wndSungMahiWindow = None

		if app.ENABLE_RENEWAL_CUBE:
			self.wndCubeRenewal = None

		if app.ENABLE_VIEW_CHEST_DROP:
			self.dlgChestDrop = None

		self.dlgDropInfo = None
		self._dropInfoSnapshot = None
		self.dlgShootingStars = None

		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory = None

		if app.ENABLE_RENDER_TARGET:
			self.wndRenderTarget = None

		if app.ENABLE_RENEWAL_OFFLINESHOP:
			self.dlgOfflineShop = None
			self.dlgOfflineShopPanel = None
			self.offlineShopBuilder = None
			self.dlgShopMessage = None

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			self.wndPrivateShopSearch = None

		if app.ENABLE_NEW_DUNGEON_LIB:
			self.wndDungeonTimer = None

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.OnTopWindow = None
			self.dlgShop = None
			self.dlgExchange = None
			self.privateShopBuilder = None
			self.wndSafebox = None
			self.dlgRefineNew = None

		if app.ENABLE_RIDING_EXTENDED:
			self.wndMountUpGrade = None

		if app.ENABLE_FISH_GAME:
			self.wndFishGame = None

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			self.wndWarpWindow = None

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			self.wndSpecialChat = None

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			self.wndChangeLook = None
		if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
			self.wndItemCombination = None

		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = None
			self.wndHuntingSelect = None
			self.wndHuntingReward = None

		if app.__AUTO_HUNT__:
			self.wndAutoHunt = None

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.wndBiyologWindow = None

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			self.wndImproveGemTicket = None

		if app.ENABLE_RANKING:
			self.wndRanking = None

		self.listGMName = {}
		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		if app.LINK_IN_CHAT:
			self.OpenLinkQuestionDialog = None

		self.wndExpandedMoneyTaskBar = None

		event.SetInterfaceWindow(self)

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	################################
	## Make Windows & Dialogs
	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain

	def __MakeExpandMoney(self):
		# one money bar: the inventory and the taskbar money button share it (two instances overlapped)
		if self.wndExpandedMoneyTaskBar:
			self.wndExpandedMoney = self.wndExpandedMoneyTaskBar
		else:
			self.wndExpandedMoney = uiTaskBar.ExpandedMoneyTaskBar()
	
	def __MakeMessengerWindow(self):
		self.wndMessenger = uiMessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

	def __MakeChatWindow(self):
		wndChat = uiChat.ChatWindow()
		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 37)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		self.wndChat = wndChat
		self.wndChat.BindInterface(self)
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))

	def __MakeTaskBar(self):
		wndTaskBar = uiTaskBar.TaskBar()
		wndTaskBar.BindInterfaceClass(self)
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND_MONEY, ui.__mem_func__(self.ToggleExpandedMoneyButton))
		self.wndExpandedMoneyTaskBar = uiTaskBar.ExpandedMoneyTaskBar()
		if uiTaskBar.TaskBar.IS_EXPANDED:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
			self.wndExpandedTaskBar = uiTaskBar.ExpandedTaskBar()
			self.wndExpandedTaskBar.LoadWindow()
			# Interface class'?n? ba?la
			self.wndExpandedTaskBar.BindInterfaceClass(self)

			if app.ENABLE_GROWTH_PET_SYSTEM:
				self.PetInformationActivate()
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_PET_INFO, ui.__mem_func__(self.TogglePetInformationWindow))

			if app.ENABLE_HUNTING_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_HUNTING, ui.__mem_func__(self.ToggleHuntingWindow))

			if app.__AUTO_HUNT__:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_AUTO_WINDOW, ui.__mem_func__(self.ToggleAutoHuntWindow))

			if app.ENABLE_AUTO_SELL_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_AUTO_SELL, ui.__mem_func__(self.ToggleAutoSellWindow))

			if app.ENABLE_BIOLOGIST_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BIOLOGIST, ui.__mem_func__(self.SetBiyologMission))

			# Sidebar buton event'lerini ExpandedTaskBar'daki metodlara ata
			if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
				self.wndExpandedTaskBar.SetSidebarButtonEvent(uiTaskBar.ExpandedTaskBar.SIDEBAR_TELEPORT, ui.__mem_func__(self.wndExpandedTaskBar.OnClickSidebarTeleport))

			if app.ENABLE_SPECIAL_INVENTORY:
				self.wndExpandedTaskBar.SetSidebarButtonEvent(uiTaskBar.ExpandedTaskBar.SIDEBAR_SPECIAL_DEP, ui.__mem_func__(self.wndExpandedTaskBar.OnClickSidebarSpecialDep))

			if app.ENABLE_RENEWAL_OFFLINESHOP:
				self.wndExpandedTaskBar.SetSidebarButtonEvent(uiTaskBar.ExpandedTaskBar.SIDEBAR_OFFLINE_SHOP, ui.__mem_func__(self.wndExpandedTaskBar.OnClickSidebarOfflineShop))

			if app.ENABLE_RENEWAL_SWITCHBOT:
				self.wndExpandedTaskBar.SetSidebarButtonEvent(uiTaskBar.ExpandedTaskBar.SIDEBAR_SWITCHBOT, ui.__mem_func__(self.wndExpandedTaskBar.OnClickSidebarSwitchbot))

			self.wndExpandedTaskBar.SetSidebarButtonEvent(uiTaskBar.ExpandedTaskBar.SIDEBAR_POTIONS, ui.__mem_func__(self.wndExpandedTaskBar.OnClickSidebarPotions))

			if app.ENABLE_WIKI_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_WIKI, ui.__mem_func__(self.OpenWikiWindow))

			if app.ENABLE_BIOLOGIST_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BIOLOGIST, ui.__mem_func__(self.OpenBiyologDialog))
		else:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))

	def __MakeParty(self):
		wndParty = uiParty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	def __MakeGameButtonWindow(self):
		wndGameButton = uiGameButton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("SKILL", ui.__mem_func__(self.__OnClickSkillPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))
		wndGameButton.SetButtonEvent("GUIDE", ui.__mem_func__(self.__OnClickGuideButton))
		wndGameButton.SetButtonEvent("NOTIFICATION_HISTORY", ui.__mem_func__(self.__OnClickNotificationHistoryButton))
		wndGameButton.SetButtonEvent("BUFFI", ui.__mem_func__(self.ToggleCompanionBuffiWnd))
		import advanced_option_notifications
		advanced_option_notifications.BindButton(wndGameButton.gameButtonDict["NOTIFICATION_HISTORY"])

		self.wndGameButton = wndGameButton

	def __OnClickGuideButton(self):
		if hasattr(self, "OpenWikiWindow"):
			self.OpenWikiWindow()

	def __OnClickNotificationHistoryButton(self):
		import advanced_option_notifications
		advanced_option_notifications.OpenHistory()

	def __IsChatOpen(self):
		return TRUE

	if app.ENABLE_AUTO_REFINE:
		def CheckRefineDialog(self, isFail):
			self.dlgRefineNew.CheckRefine(isFail)

	def __MakeWindows(self):
		wndCharacter = uiCharacter.CharacterWindow()

		wndInventory = uiInventory.InventoryWindow()
		wndInventory.BindInterfaceClass(self)

		# Sidebar butonlar? art?k ExpandedTaskBar'da, bu �a?r?lar kald?r?ld?

		wndMiniMap = uiMiniMap.MiniMap()
		wndMiniMap.BindInterfaceClass(self)

		if app.ENABLE_COLLECT_WINDOW:
			self.wndCollectWindow = uiCollectWindow.CollectWindow()

		if not app.ENABLE_SPECIAL_INVENTORY:
			wndSafebox = uiSafebox.SafeboxWindow()
			if app.ENABLE_SLOT_MARKING_SYSTEM:
				wndSafebox.BindInterface(self)

			wndMall = uiSafebox.MallWindow()
			self.wndMall = wndMall

		wndChatLog = uiChat.ChatLogWindow()
		wndChatLog.BindInterface(self)


		if app.ENABLE_GUILDBANK_LOG:
			self.wndGuildBank = uiGuildBank.GuildBankLogDialog()

		self.wndCharacter = wndCharacter
		self.wndInventory = wndInventory
		self.wndMiniMap = wndMiniMap

		if not app.ENABLE_SPECIAL_INVENTORY:
			self.wndSafebox = wndSafebox

		self.wndChatLog = wndChatLog

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			self.wndMiniGameRoulette = uiMiniGameRoulette.RouletteWindow()

		if app.ENABLE_COLLECTIONS_SYSTEM:
			self.wndCollections = uiCollections.CollectionsWindow()

		if app.ENABLE_SUNG_MAHI_TOWER:
			self.wndSungMahiWindow = uiSungMahi.SungMahiWindow()

		if app.ENABLE_RIDING_EXTENDED:
			self.wndMountUpGrade = uiMountUpGrade.MountUpGradeWindow()

		if app.ENABLE_GUILD_RANK_SYSTEM:
			self.wndGuildRanking = uiGuildRanking.GuildRankingDialog()

		if app.ENABLE_RENEWAL_SWITCHBOT:
			self.wndSwitchbot = uiSwitchbot.SwitchbotWindow()

		if app.ENABLE_VIEW_CHEST_DROP:
			self.dlgChestDrop = uiChestDrop.ChestDropWindow()

		self.dlgDropInfo = uidropinfo.DropInfoWindow()
		self.dlgShootingStars = uishootingstars.ShootingStarLayer()

		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory = uiSpecialInventory.SpecialInventoryWindow()
			self.wndSpecialInventory.BindInterfaceClass(self)
			self.wndSafebox = uiSafebox.SafeboxWindow(self.wndSpecialInventory)
			self.wndMall = uiSafebox.MallWindow(self.wndSpecialInventory)
			self.wndSpecialInventory.BindSafeboxAndMallClass(self.wndSafebox, self.wndMall)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			wndAura = uiAura.AuraWindow()
			self.wndAura = wndAura

		if app.ENABLE_YIN_AND_YANG_ATTR:
			self.wndYinAndYangInventory = uiYinAndYangAttr.YinAndYangInventory()
			self.wndYinAndYangInventory.BindInventory(self.wndInventory)
			self.wndYinAndYangInventory.Hide()

			self.wndYinAndYangHarmony = uiYinAndYangAttr.YinAndYangHarmony()
			self.wndYinAndYangHarmony.BindInventory(self.wndInventory)
			self.wndYinAndYangHarmony.Hide()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.wndPetInfoWindow = uiPetInfo.PetInformationWindow()
			self.wndPetInfoWindow.BindInterfaceClass(self)
			self.wndInventory.SetPetHatchingWindow(self.wndPetInfoWindow.GetPetHatchingWindow())
			self.wndInventory.SetPetFeedWindow(self.wndPetInfoWindow.GetPetFeedWindow())
			self.wndInventory.SetPetNameChangeWindow(self.wndPetInfoWindow.GetPetNameChangeWindow())

		self.wndCompanion = uiCompanion.CompanionWindow()
		self.wndCompanion.BindInterface(self)
		self.wndCompanion.Hide()

		if getattr(app, "ENABLE_SKILL_UPGRADE", 0):
			self.wndSkillUpgrade = uiSkillUpgrade.SkillUpgradeWindow()
			self.wndSkillUpgrade.BindInterface(self)
			self.wndSkillUpgrade.Hide()

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			self.wndWarpWindow = uiTeleport.TeleportWindow()

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			self.wndChangeLook = uiChangeLook.ChangeLookWindow()
		if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
			self.wndItemCombination = uiItemCombination.ItemCombinationWindow()

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.wndBiyologWindow = uiBiyolog.BiyologWindow()

		if app.ENABLE_RANKING:
			self.wndRanking = uiranking.RankingWindow()

	def __MakeDialogs(self):
		self.dlgExchange = uiExchange.ExchangeDialog()
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.dlgExchange.BindInterface(self)
		self.dlgExchange.Hide()

		self.dlgPointReset = uiPointReset.PointResetDialog()
		self.dlgPointReset.LoadDialog()
		self.dlgPointReset.Hide()

		self.dlgShop = uiShop.ShopDialog()
		self.dlgShop.LoadDialog()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.dlgShop.BindInterface(self)
		self.dlgShop.Hide()

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			self.wndImproveGemTicket = uiGemTicket.GemTicketWindow()

		self.dlgRestart = uiRestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uiSystem.SystemDialog()
		self.dlgSystem.BindInterfaceClass(self)
		self.dlgSystem.LoadDialog()
		self.dlgSystem.Hide()

		self.dlgPassword = uiSafebox.PasswordDialog()
		self.dlgPassword.Hide()

		self.hyperlinkItemTooltip = uiToolTip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.Hide()

		self.tooltipItem = uiToolTip.ItemToolTip()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.tooltipItem.BindInterface(self)
		self.tooltipItem.Hide()

		self.tooltipSkill = uiToolTip.SkillToolTip()
		self.tooltipSkill.Hide()

		self.whisper_tooltip = None
		if app.ENABLE_WHISPER_CLOSE_SHORTCUT:
			self.whisper_tooltip = uiToolTip.ToolTip()
			self.whisper_tooltip.AutoAppendTextLine(localeInfo.WHISPER_TOOLTIP_CLOSE_SHORTCUT)
			self.whisper_tooltip.AlignHorizonalCenter()
			self.whisper_tooltip.Hide()

		self.privateShopBuilder = uiPrivateShopBuilder.PrivateShopBuilder()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.privateShopBuilder.BindInterface(self)
		self.privateShopBuilder.Hide()

		self.dlgRefineNew = uiRefine.RefineDialogNew()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.dlgRefineNew.SetInven(self.wndInventory)

		if constInfo.SHOW_REFINE_ITEM_DESC:
			self.dlgRefineNew.BindInterface(self)

		self.dlgRefineNew.Hide()

		if app.ENABLE_RESP_SYSTEM:
			self.wndResp = uiresp.RespDialog()
			self.wndResp.Hide()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.tooltipPetSkill = uiToolTip.PetSkillToolTip()
			self.tooltipPetSkill.Hide()

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			self.wndSpecialChat = uiSpecialChat.SpecialChat()
			self.wndSpecialChat.Hide()

		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = uiHunting.HuntingWindow()
			self.wndHuntingSelect = uiHunting.HuntingSelectWindow()
			self.wndHuntingReward = uiHunting.HuntingRewardWindow()

	def __MakeTipBoard(self):
		self.tipBoard = uiTip.TipBoardNew()
		self.tipBoard.Hide()

		self.bigBoard = uiTip.BigBoard()
		self.bigBoard.Hide()

		if app.ENABLE_RENEWAL_OX_EVENT:
			self.bigBoardControl = uiTip.BigBoardControl()
			self.bigBoardControl.Hide()

	def __MakeWebWindow(self):
		if constInfo.IN_GAME_SHOP_ENABLE:
			import uiWeb
			self.wndWeb = uiWeb.WebWindow()
			self.wndWeb.LoadWindow()
			self.wndWeb.Hide()

	def __MakeCubeWindow(self):
		self.wndCube = uiCube.CubeWindow()
		self.wndCube.LoadWindow()
		self.wndCube.Hide()

	def __MakeCubeResultWindow(self):
		self.wndCubeResult = uiCube.CubeResultWindow()
		self.wndCubeResult.LoadWindow()
		self.wndCubeResult.Hide()

	def __MakeItemFinder(self):
		self.wndItemFinder = uiitemfinder.ItemFinder()
		self.wndItemFinder.LoadWindow()
		self.wndItemFinder.Hide()

	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiselectitem.SelectItemWindow()
		self.wndItemSelect.Hide()

	if app.ENABLE_DUNGEON_INFO:
		def __MakeTableDungeonInfo(self):
			self.wndTableDungeonInfo = uitabledungeon.TableDungeonWindows()
			self.wndTableDungeonInfo.Hide()

	if app.ENABLE_RENEWAL_CUBE:
		def __MakeCubeRenewal(self):
			import uicuberenewal
			self.wndCubeRenewal = uicuberenewal.CubeRenewalWindows()
			self.wndCubeRenewal.Hide()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def __MakeAcceWindow(self):
			self.wndAcceCombine = uiAcce.CombineWindow()
			self.wndAcceCombine.LoadWindow()
			if app.ENABLE_SLOT_MARKING_SYSTEM:
				self.wndAcceCombine.BindInterface(self)
				if self.wndInventory:
					self.wndAcceCombine.SetInven(self.wndInventory)
			self.wndAcceCombine.Hide()

			self.wndAcceAbsorption = uiAcce.AbsorbWindow()
			self.wndAcceAbsorption.LoadWindow()
			if app.ENABLE_SLOT_MARKING_SYSTEM:
				self.wndAcceAbsorption.BindInterface(self)
				if self.wndInventory:
					self.wndAcceAbsorption.SetInven(self.wndInventory)
			self.wndAcceAbsorption.Hide()

			if self.wndInventory:
				self.wndInventory.SetAcceWindow(self.wndAcceCombine, self.wndAcceAbsorption)

	if app.ENABLE_BATTLE_PASS:
		def __BoardBpass(self):
			self.wndbpass = uibattlepass.Battlepass()
			self.wndbpass.LoadWindow()
			self.wndbpass.Hide()

		def __BoardBpasspremium(self):
			self.wndbpasspremium = uibattlepasspremium.Battlepass()
			self.wndbpasspremium.LoadWindow()
			self.wndbpasspremium.Hide()

	if app.ENABLE_EVENT_SYSTEM:
		def __MakeEventWindow(self):
			self.wndEvent = uiEvent.EventWindow(self)
			self.wndEvent.LoadWindow()
			self.wndEvent.Hide()

		def __MakeMiniGameAttendance(self):
			self.wndMiniGameAttendance = uiMiniGameAttendance.MiniGameAttendance()
			self.wndMiniGameAttendance.Hide()

	if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
		def __BoardHalloweenEvent(self):
			self.wndhaloun = uihalloween.Halloween()
			self.wndhaloun.LoadWindow()
			self.wndhaloun.Hide()

	if app.ENABLE_MINI_GAME_OKEY:
		def __MakeCardsInfoWindow(self):
			self.wndCardsInfo = uiMiniGameRumi.CardsInfoWindow()
			self.wndCardsInfo.LoadWindow()
			self.wndCardsInfo.Hide()

		def __MakeCardsWindow(self):
			self.wndCards = uiMiniGameRumi.CardsWindow()
			self.wndCards.LoadWindow()
			self.wndCards.Hide()

		def __MakeCardsIconWindow(self):
			self.wndCardsIcon = uiMiniGameRumi.IngameWindow()
			self.wndCardsIcon.LoadWindow()
			self.wndCardsIcon.Hide()

	if app.ENABLE_RENEWAL_SKILL_SELECT:
		def __MakeSkillSelectWindow(self):
			self.wndSkillSelect = uiSkillSelect.SkillSelectWindow()
			self.wndSkillSelect.LoadWindow()
			self.wndSkillSelect.Hide()

	if app.ENABLE_GAYA_SYSTEM:
		def __MakeItemSelectWindowEx(self):
			self.wndItemSelectEx = uiselectitemEx.SelectItemWindowEx()
			self.wndItemSelectEx.Hide()

		def __MakeGemGui(self):
			self.wndGemG = uiGemShop.SelectGems()
			self.wndGemG.LoadWindow()
			self.wndGemG.Hide()

			self.wndGemM = uiGemShop.SelectGemsShop()
			self.wndGemM.LoadWindow()
			self.wndGemM.Hide()

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		self.__MakeWindows()
		self.__MakeDialogs()

		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeTipBoard()
		self.__MakeWebWindow()
		self.__MakeCubeWindow()
		self.__MakeCubeResultWindow()
		self.__MakeExpandMoney()
		self.__MakeItemFinder()
		self.__MakeItemSelectWindow()

		if app.ENABLE_RENEWAL_CUBE:
			self.__MakeCubeRenewal()

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.__MakeAcceWindow()

		if app.ENABLE_BATTLE_PASS:
			self.__BoardBpass()
			self.__BoardBpasspremium()

		if app.ENABLE_MINI_GAME_OKEY:
			self.__MakeCardsInfoWindow()
			self.__MakeCardsWindow()
			self.__MakeCardsIconWindow()

		if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
			self.__BoardHalloweenEvent()

		if app.ENABLE_GAYA_SYSTEM:
			self.__MakeGemGui()

		if app.ENABLE_RENEWAL_SKILL_SELECT:
			self.__MakeSkillSelectWindow()

		if app.ENABLE_AUTO_SELL_SYSTEM:
			self.__MakeAutoSellWindow()

		if app.ENABLE_DUNGEON_INFO:
			self.__MakeTableDungeonInfo()

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}

		if app.ENABLE_ZODIAC_MISSION:
			self.__Make12ziTimerWindow()
			self.__Make12ziRewardWindow()

		if app.ENABLE_EVENT_SYSTEM:
			self.__MakeEventWindow()
			self.__MakeMiniGameAttendance()

		if app.ENABLE_GAYA_SYSTEM:
			self.__MakeItemSelectWindowEx()

		self.privateShopAdvertisementBoardDict = {}

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.wndBiyologWindow.SetItemToolTip(self.tooltipItem)

		self.wndInventory.SetItemToolTip(self.tooltipItem)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)
		self.wndCube.SetItemToolTip(self.tooltipItem)
		self.wndCubeResult.SetItemToolTip(self.tooltipItem)


		if app.ENABLE_GUILDBANK_LOG:
			self.wndGuildBank.SetItemToolTip(self.tooltipItem)

		# ITEM_MALL
		self.wndMall.SetItemToolTip(self.tooltipItem)
		# END_OF_ITEM_MALL
		if app.ENABLE_SUNG_MAHI_TOWER:
			self.wndSungMahiWindow.SetItemToolTip(self.tooltipItem)
		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(self.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)
		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.wndItemSelect.SetItemToolTip(self.tooltipItem)
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.ENABLE_GAYA_SYSTEM:
			self.wndGemM.SetItemToolTip(self.tooltipItem)

		self.dlgShop.SetItemToolTip(self.tooltipItem)
		self.dlgExchange.SetItemToolTip(self.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			self.wndMiniGameRoulette.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_RESP_SYSTEM:
			self.wndResp.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_RENEWAL_SWITCHBOT:
			self.wndSwitchbot.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine.SetItemToolTip(self.tooltipItem)
			self.wndAcceAbsorption.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_VIEW_CHEST_DROP:
			self.dlgChestDrop.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			self.wndAura.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
			self.privateShopBuilder.SetInven(player.INVENTORY, self.wndInventory)
		elif app.ENABLE_SLOT_MARKING_SYSTEM:
			self.privateShopBuilder.SetInven(self.wndInventory)
			if app.ENABLE_SPECIAL_INVENTORY:
				self.privateShopBuilder.SetSpecialInven(self.wndSpecialInventory)

		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.dlgExchange.SetInven(self.wndInventory)
			if app.ENABLE_SPECIAL_INVENTORY:
				self.dlgExchange.SetSpecialInven(self.wndSpecialInventory)

		if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
			self.wndhaloun.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.wndPetInfoWindow.SetItemToolTip(self.tooltipItem)
			self.wndPetInfoWindow.SetInven(self.wndInventory)
			self.wndPetInfoWindow.SetPetSkillToolTip(self.tooltipPetSkill)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			self.wndChangeLook.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
			self.wndItemCombination.SetItemToolTip(self.tooltipItem)
			self.wndItemCombination.SetInven(self.wndInventory)

		if app.ENABLE_GAYA_SYSTEM:
			self.wndItemSelectEx.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_YIN_AND_YANG_ATTR:
			self.wndYinAndYangInventory.SetItemToolTip(self.tooltipItem)

		self.__InitWhisper()

	if app.LINK_IN_CHAT:
		def AnswerOpenLink(self, answer):
			if not self.OpenLinkQuestionDialog:
				return

			self.OpenLinkQuestionDialog.Close()
			self.OpenLinkQuestionDialog = None

			if not answer:
				return

			link = constInfo.link
			os.system(link)

	if app.ENABLE_AUTO_SELL_SYSTEM:
		def __MakeAutoSellWindow(self):
			if not hasattr(self, "wndAutoSell") or self.wndAutoSell is None:
				self.wndAutoSell = uiautosell.AutoSellWindow()
				self.wndAutoSell.Hide()
	
		def ToggleAutoSellWindow(self):
			self.__MakeAutoSellWindow()
			if self.wndAutoSell.IsShow():
				self.wndAutoSell.Hide()
			else:
				self.wndAutoSell.Show()
				self.wndAutoSell.SetTop()
				net.SendChatPacket("/autosell_info 1")

	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)
			elif "pm" == type and str(tokens[1]) != player.GetMainCharacterName():
				self.OpenWhisperDialog(str(tokens[1]))
			elif "web" == type and tokens[1].startswith("httpXxX") or "web" == type and tokens[1].startswith("httpsXxX"):
					OpenLinkQuestionDialog = uiCommon.QuestionDialog2()
					OpenLinkQuestionDialog.SetText1(localeInfo.CHAT_OPEN_LINK_DANGER)
					OpenLinkQuestionDialog.SetText2(localeInfo.CHAT_OPEN_LINK)
					OpenLinkQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerOpenLink(arg))
					OpenLinkQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerOpenLink(arg))
					constInfo.link = "start " + tokens[1].replace("XxX", "://").replace("&","^&")
					OpenLinkQuestionDialog.Open()
					self.OpenLinkQuestionDialog = OpenLinkQuestionDialog
			elif "sysweb" == type:
				os.system("start " + tokens[1].replace("XxX", "://"))
			elif "petitem" == type and app.ENABLE_GROWTH_PET_SYSTEM:
				self.hyperlinkItemTooltip.SetHyperlinkPetItem(tokens)
			elif "wiki" == type:
				self.OpenWikiWindow()
				self.wndWiki.OpenRules()

	## Make Windows & Dialogs
	################################

	def Close(self):

		if app.__DUNGEON_INFO__:
			if self.wndDungeonTrack:
				self.wndDungeonTrack.Close()
				self.wndDungeonTrack.Destroy()
				self.wndDungeonTrack = None
			if self.wndDungeonManager:
				self.wndDungeonManager.Destroy()
				self.wndDungeonManager = None

		if app.ENABLE_TRACK_WINDOW:
			if self.wndTrackWindow:
				self.wndTrackWindow.Close()
				self.wndTrackWindow.Destroy()
				self.wndTrackWindow=None
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if app.ENABLE_ZODIAC_MISSION:
			if self.wnd12ziTimer:
				self.wnd12ziTimer.Hide()
				self.wnd12ziTimer=None
			if self.wnd12ziReward:
				self.wnd12ziReward.Hide()
				self.wnd12ziReward=None

		if uiQuest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiQuest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow = None
		self.wndQuestWindow = {}

		if app.ENABLE_WIKI_SYSTEM:
			if self.wndWiki:
				self.wndWiki.Close()
				self.wndWiki.Destroy()
				self.wndWiki=None

		if app.ENABLE_ITEMSHOP:
			if self.wndItemShop:
				self.wndItemShop.Hide()
				self.wndItemShop.Destroy()
				self.wndItemShop = None

		if app.ENABLE_DUNGEON_INFO:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Close()
				self.wndDungeonInfo.Destroy()
				self.wndDungeonInfo = None

		if self.uiAffectshower:
			self.uiAffectshower = None

		if self.wndChat:
			self.wndChat.Destroy()

		if self.wndTaskBar:
			self.wndTaskBar.Destroy()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Destroy()

		if self.wndInventory:
			self.wndInventory.Hide()
			self.wndInventory.Destroy()

		if self.wndExpandedMoney:
			self.wndExpandedMoney.Destroy()

		if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
			if self.wndhaloun:
				self.wndhaloun.Destroy()

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			if (self.wndImproveGemTicket):
				del self.wndImproveGemTicket

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgPointReset:
			self.dlgPointReset.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()

		if self.wndWeb:
			self.wndWeb.Destroy()
			self.wndWeb = None

		if self.wndMall:
			self.wndMall.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()

		if app.ENABLE_MINI_GAME_OKEY:
			if self.wndCardsInfo:
				self.wndCardsInfo.Destroy()

			if self.wndCards:
				self.wndCards.Destroy()

			if self.wndCardsIcon:
				self.wndCardsIcon.Destroy()

		if self.wndCube:
			self.wndCube.Destroy()

		if self.wndCubeResult:
			self.wndCubeResult.Destroy()

		if app.ENABLE_DUNGEON_INFO:
			if self.wndTableDungeonInfo:
				self.wndTableDungeonInfo.Hide()
				self.wndTableDungeonInfo.Close()
				self.wndTableDungeonInfo.Destroy()

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndItemFinder:
			self.wndItemFinder.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()

		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()

		if app.ENABLE_COLLECTIONS_SYSTEM:
			if self.wndCollections:
				self.wndCollections.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()

		if app.ENABLE_EVENT_SYSTEM:
			if self.wndMiniGame:
				self.wndMiniGame.Destroy()

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Destroy()
				del self.wndHunting
			if self.wndHuntingSelect:
				self.wndHuntingSelect.Destroy()
				del self.wndHuntingSelect
			if self.wndHuntingReward:
				self.wndHuntingReward.Destroy()
				del self.wndHuntingReward

		if app.__AUTO_HUNT__:
			if self.wndAutoHunt:
				self.wndAutoHunt.Close()
				self.wndAutoHunt.Destroy()
				self.wndAutoHunt = None

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.wndBiyologWindow:
				self.wndBiyologWindow.Destroy()

		# ITEM_MALL
		if self.mallPageDlg:
			self.mallPageDlg.Destroy()
		# END_OF_ITEM_MALL

		# ACCESSORY_REFINE_ADD_METIN_STONE
		if self.wndItemSelect:
			self.wndItemSelect.Destroy()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			if self.wndMiniGameRoulette:
				self.wndMiniGameRoulette.Destroy()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.wndSungMahiWindow:
				self.wndSungMahiWindow.Destroy()

		if app.ENABLE_GUILD_RANK_SYSTEM:
			if self.wndGuildRanking:
				self.wndGuildRanking.Destory()

		if app.ENABLE_RENEWAL_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()
				self.wndSwitchbot.Destroy()

		if app.ENABLE_GAYA_SYSTEM:
			if self.wndItemSelectEx:
				self.wndItemSelectEx.Destroy()

			if self.wndGemG:
				self.wndGemG.Destroy()

			if self.wndGemM:
				self.wndGemM.Destroy()

		if app.ENABLE_BATTLE_PASS:
			if self.wndbpass:
				self.wndbpass.Hide()
				self.wndbpass.Destroy()
				self.wndbpass = None
				del self.wndbpass

			if self.wndbpasspremium:
				self.wndbpasspremium.Hide()
				self.wndbpasspremium.Destroy()
				self.wndbpasspremium = None
				del self.wndbpasspremium

		if app.ENABLE_RENEWAL_CUBE:
			if self.wndCubeRenewal:
				self.wndCubeRenewal.Destroy()
				self.wndCubeRenewal.Close()

		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Destroy()
				del self.wndRanking

		if app.ENABLE_RESP_SYSTEM:
			if self.wndResp:
				self.wndResp.Destroy()

				del self.wndResp

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if self.wndAcceCombine:
				self.wndAcceCombine.Destroy()

			if self.wndAcceAbsorption:
				self.wndAcceAbsorption.Destroy()

		if app.ENABLE_GUILDBANK_LOG:
			if self.wndGuildBank:
				self.wndGuildBank.Destroy()
				self.wndGuildBank.Close()

		if app.ENABLE_VIEW_CHEST_DROP:
			if self.dlgChestDrop:
				self.dlgChestDrop.Destroy()

		if self.dlgDropInfo:
			self.dlgDropInfo.Destroy()

		if self.dlgShootingStars:
			self.dlgShootingStars.Destroy()

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.wndSpecialInventory:
				self.wndSpecialInventory.Hide()
				self.wndSpecialInventory.Destroy()

		if app.ENABLE_RENDER_TARGET:
			if self.wndRenderTarget:
				self.wndRenderTarget.Close()
				self.wndRenderTarget.Destroy()
				self.wndRenderTarget = None

		if app.ENABLE_RENEWAL_OFFLINESHOP:
			if self.dlgOfflineShop:
				self.dlgOfflineShop.Hide()
				self.dlgOfflineShop.Destroy()
				del self.dlgOfflineShop

			if self.dlgOfflineShopPanel:
				self.dlgOfflineShopPanel.Hide()
				self.dlgOfflineShopPanel.Destroy()
				del self.dlgOfflineShopPanel

			if self.offlineShopBuilder:
				self.offlineShopBuilder.Hide()
				self.offlineShopBuilder.Destroy()
				del self.offlineShopBuilder

			if self.dlgShopMessage:
				self.dlgShopMessage.Hide()
				self.dlgShopMessage.Destroy()
				self.dlgShopMessage = None

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Hide()
				self.wndPrivateShopSearch.Destroy()
				del self.wndPrivateShopSearch
				self.wndPrivateShopSearch = 0

		if app.ENABLE_RENEWAL_SKILL_SELECT:
			if self.wndSkillSelect:
				self.wndSkillSelect.Destroy()

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if self.wndAura:
				self.wndAura.Destroy()
			del self.wndAura

		if app.ENABLE_YIN_AND_YANG_ATTR:
			if self.wndYinAndYangInventory:
				self.wndYinAndYangInventory.Destroy()
				self.wndYinAndYangInventory = None

			if self.wndYinAndYangHarmony:
				self.wndYinAndYangHarmony.Destroy()
				self.wndYinAndYangHarmony = None

		if app.ENABLE_FISH_GAME:
			if self.wndFishGame:
				self.wndFishGame.Hide()
				self.wndFishGame.Destroy()
				self.wndFishGame = None

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.Destroy()

		if self.wndCompanion:
			self.wndCompanion.Destroy()
			self.wndCompanion = None

		if self.wndSkillUpgrade:
			self.wndSkillUpgrade.Destroy()
			self.wndSkillUpgrade = None

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			if self.wndWarpWindow:
				self.wndWarpWindow.Destroy()

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			if self.wndSpecialChat:
				self.wndSpecialChat.Destroy()

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if self.wndChangeLook:
				del self.wndChangeLook
		if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
			if self.wndItemCombination:
				self.wndItemCombination.Destroy()
				del self.wndItemCombination
				self.wndItemCombination = None

		if app.ENABLE_RIDING_EXTENDED:
			if self.wndMountUpGrade:
				self.wndMountUpGrade.Destroy()
				del self.wndMountUpGrade

		if app.ENABLE_COLLECT_WINDOW:
			if self.wndCollectWindow:
				self.wndCollectWindow.Destroy()

		if app.ENABLE_EVENT_SYSTEM:
			if self.wndEvent:
				self.wndEvent.Destroy()

			if self.wndMiniGameAttendance:
				self.wndMiniGameAttendance.Destroy()

		self.wndChatLog.Destroy()
		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		for dlg in self.equipmentDialogDict.itervalues():
			dlg.Destroy()

		# ITEM_MALL
		del self.mallPageDlg
		# END_OF_ITEM_MALL

		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		del self.wndChat

		del self.wndTaskBar
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar

		del self.wndCharacter
		del self.wndInventory
		del self.wndExpandedMoney
		del self.dlgExchange
		del self.dlgPointReset
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		del self.tooltipItem
		del self.tooltipSkill
		del self.wndMiniMap
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		del self.wndCube
		del self.wndCubeResult
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndItemFinder
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		del self.wndGameButton
		del self.tipBoard
		del self.bigBoard
		del self.wndItemSelect

		if app.ENABLE_EVENT_SYSTEM:
			del self.wndMiniGame

		if app.ENABLE_COLLECTIONS_SYSTEM:
			del self.wndCollections

		if app.ENABLE_COLLECT_WINDOW:
			del self.wndCollectWindow

		if app.ENABLE_SUNG_MAHI_TOWER:
			del self.wndSungMahiWindow

		if app.ENABLE_GUILD_RANK_SYSTEM:
			del self.wndGuildRanking

		if app.ENABLE_RENEWAL_SWITCHBOT:
			del self.wndSwitchbot

		if app.ENABLE_RENEWAL_CUBE:
			del self.wndCubeRenewal

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			del self.wndAcceCombine
			del self.wndAcceAbsorption

		if app.ENABLE_VIEW_CHEST_DROP:
			if self.dlgChestDrop:
				del self.dlgChestDrop

		if self.dlgDropInfo:
			del self.dlgDropInfo

		if self.dlgShootingStars:
			del self.dlgShootingStars

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.wndSpecialInventory:
				del self.wndSpecialInventory

		if app.ENABLE_GUILDBANK_LOG:
			del self.wndGuildBank

		if app.ENABLE_MINI_GAME_OKEY:
			del self.wndCardsInfo
			del self.wndCards
			del self.wndCardsIcon

		if app.ENABLE_GAYA_SYSTEM:
			del self.wndGemG
			del self.wndGemM

		if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
			del self.wndhaloun

		if app.ENABLE_RENEWAL_OX_EVENT:
			del self.bigBoardControl

		if app.ENABLE_GAYA_SYSTEM:
			del self.wndItemSelectEx

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			del self.wndMiniGameRoulette

		if app.ENABLE_RENEWAL_SKILL_SELECT:
			del self.wndSkillSelect

		if app.ENABLE_NEW_DUNGEON_LIB:
			if self.wndDungeonTimer:
				self.wndDungeonTimer.Hide()
				self.wndDungeonTimer = 0

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.wndPetInfoWindow:
				del self.wndPetInfoWindow
			self.wndPetInfoWindow = None
			del self.tooltipPetSkill

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			del self.wndWarpWindow

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			del self.wndSpecialChat

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.wndBiyologWindow:
				del self.wndBiyologWindow

		if app.ENABLE_DUNGEON_INFO:
			del self.wndTableDungeonInfo

		if app.ENABLE_EVENT_SYSTEM:
			del self.wndEvent
			del self.wndMiniGameAttendance

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		uiChat.DestroyChatInputSetWindow()

	if app.ENABLE_BATTLE_PASS:
		def ShowBoardBpass(self):
			if self.wndbpass.IsShow():
				self.wndbpass.Close()
			else:
				self.wndbpass.Show()

		def ShowBoardBpassPremium(self):
			if self.wndbpasspremium.IsShow():
				self.wndbpasspremium.Close()
			else:
				self.wndbpasspremium.Show()

	## Skill
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SkillClearCoolTime(self, slotIndex):
		self.wndCharacter.SkillClearCoolTime(slotIndex)
		self.wndTaskBar.SkillClearCoolTime(slotIndex)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	## Refresh
	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		self.wndExpandedMoney.RefreshStatus()
		if self.wndExpandedMoneyTaskBar:
			self.wndExpandedMoneyTaskBar.RefreshStatus()
		self.wndInventory.RefreshStatus()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.wndPetInfoWindow.RefreshStatus()

	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()

	def RefreshSkillCoolTime(self):
		if self.wndTaskBar:
			self.wndTaskBar.RefreshSkillCoolTime()
		if self.wndCharacter:
			self.wndCharacter.RefreshSkillCoolTime()

	def __CheckNewDropItems(self):
		if self.dlgDropInfo is None:
			return
		import advanced_options_runtime
		if not advanced_options_runtime.Get("view_drop_ui", False):
			return
		slotCount = player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT
		snapshot = {}
		for slot in xrange(slotCount):
			vnum = player.GetItemIndex(slot)
			if vnum:
				snapshot[slot] = (vnum, player.GetItemCount(slot))
		previous = self._dropInfoSnapshot
		self._dropInfoSnapshot = snapshot
		if previous is None:
			return
		for slot, (vnum, count) in snapshot.items():
			oldEntry = previous.get(slot)
			if oldEntry is None:
				self.dlgDropInfo.AppendDropInfo(vnum, count)
			elif oldEntry[0] == vnum and count > oldEntry[1]:
				self.dlgDropInfo.AppendDropInfo(vnum, count - oldEntry[1])
			elif oldEntry[0] != vnum:
				self.dlgDropInfo.AppendDropInfo(vnum, count)

	def RefreshInventory(self):
		self.__CheckNewDropItems()
		if self.wndTaskBar:
			self.wndTaskBar.RefreshQuickSlot()
		if self.wndInventory:
			self.wndInventory.RefreshItemSlot()

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.wndBiyologWindow:
				self.wndBiyologWindow.RefreshItemSlot()

		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.RefreshItemSlot()

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.IsAuraRefineWindowOpen():
				if self.wndAura and self.wndAura.IsShow():
					self.wndAura.RefreshAuraWindow()

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.GetChangeLookWindowOpen() == 1:
				self.wndChangeLook.RefreshChangeLookWindow()

	def RefreshCharacter(self):
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	if app.ENABLE_GUILD_RANK_SYSTEM:
		def OpenGuildRanking(self):
			self.wndGuildRanking.Open()

	# ITEM_MALL
	def RefreshMall(self):
		self.wndMall.RefreshMall()

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uiShop.MallPageDialog()

		self.mallPageDlg.Open()
	# END_OF_ITEM_MALL

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		## guild_renewal
		def RefreshGuildBaseInfoPage(self):
			self.wndGuild.RefreshGuildBaseInfoPage()

		## GuildBaseInfoBankGold
		def RefreshGuildBaseInfoPageBankGold(self):
			self.wndGuild.RefreshGuildBaseInfoPageBankGold()

		## guild_renewal_war
		def RefreshGuildWarInfoPage(self):
			self.wndGuild.RefreshGuildWarInfoPage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	## Calling Functions
	# PointReset
	def OpenPointResetDialog(self):
		self.dlgPointReset.Show()
		self.dlgPointReset.SetTop()

	def ClosePointResetDialog(self):
		self.dlgPointReset.Close()

	# Shop
	def OpenShopDialog(self, vid):
		self.wndInventory.Show()
		self.wndExpandedMoney.Show()
		self.wndInventory.SetTop()
		self.dlgShop.Open(vid)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	if app.ENABLE_VIEW_CHEST_DROP:
		def AddChestDropInfo(self, chestVnum, pageIndex, slotIndex, itemVnum, itemCount):
			self.dlgChestDrop.AddChestDropItem(int(chestVnum), int(pageIndex), int(slotIndex), int(itemVnum), int(itemCount))

		def RefreshChestDropInfo(self, chestVnum):
			self.dlgChestDrop.RefreshItems(chestVnum)

	## Quest
	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):

		wnds = ()

		q = uiQuest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			# UNKNOWN_UPDATE
			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))
			# END_OF_UNKNOWN_UPDATE

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1

		# END_OF_UNKNOWN_UPDATE
		
	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	## Exchange
	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	def RefreshExchange(self):
		self.dlgExchange.Refresh()

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def AddExchangeItemSlotIndex(self, idx):
			self.dlgExchange.AddExchangeItemSlotIndex(idx)

	## Party
	def AddPartyMember(self, pid, name):
		self.wndParty.AddPartyMember(pid, name)

		self.__ArrangeQuestButton()

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)
		self.__ArrangeQuestButton()

	def LinkPartyMember(self, pid, vid):
		self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()
		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	## Safebox
	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		# SAFEBOX_PASSWORD
		self.dlgPassword.SetTitle(localeInfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")
		# END_OF_SAFEBOX_PASSWORD

		self.dlgPassword.ShowDialog()

	def OpenSafeboxWindow(self, size):
		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.ShowSafeboxWindow(size)

		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def OpenGuildScoreWindow(self):
			import chat
			chat.AppendChat(chat.CHAT_TYPE_INFO, "OpenGuildScoreWindow")
			self.wndGuild.OpenGuildScoreWindow()

	def CommandCloseSafebox(self):
		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.CommandCloseSafebox()

		self.wndSafebox.CommandCloseSafebox()

	# ITEM_MALL
	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeInfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()

	def OpenMallWindow(self, size):
		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.ShowMallWindow(size)

		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		if app.ENABLE_SPECIAL_INVENTORY:
			self.wndSpecialInventory.CommandCloseMall()

		self.wndMall.CommandCloseMall()
	# END_OF_ITEM_MALL

	if app.ENABLE_UN_SEAL_SCROLL_PLUS:
		def OpenUnSealScrollPlusPasswordCheckWindow(self, type):
			if not self.dlgPassword:
				return
			self.dlgPassword.SetSendType(type)
			self.dlgPassword.BindInterfaceClass(self)
			self.dlgPassword.ShowDialog()

		def CheckOpenShopWhenUseUnSealScroll(self):
			if self.privateShopBuilder and self.privateShopBuilder.IsShow():
				return False
			if hasattr(self, "wndShop") and self.wndShop and self.wndShop.IsShow():
				return False
			return True

		def UnSealScrollPlusPasswordSuccess(self, type):
			if net.UN_SEAL_SCROLL_PLUS_PASSWORD_SUCCESS_INVENTORY == type:
				if self.wndInventory:
					self.wndInventory.UnSealScrollPlusPasswordSuccess()
			elif net.UN_SEAL_SCROLL_PLUS_PASSWORD_SUCCESS_DS_INVENTORY == type:
				if hasattr(self, "wndDragonSoul") and self.wndDragonSoul:
					self.wndDragonSoul.UnSealScrollPlusPasswordSuccess()

	## Guild
	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiGuild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.wndGuild.GuildWarOppGuildNameSetting(guildSelf, guildOpp)
		self.guildScoreBoardDict[uiGuild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiGuild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	# GUILDWAR_MEMBER_COUNT
	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiGuild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)
	# END_OF_GUILDWAR_MEMBER_COUNT

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point, winpoint):
			key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
			if not self.guildScoreBoardDict.has_key(key):
				return

			guildBoard = self.guildScoreBoardDict[key]
			guildBoard.SetScore(gainGuildID, opponentGuildID, point)
			import chat
			chat.AppendChat(1, "OnRecvGuildWarPoint - winpoint %d" % (winpoint))
			self.wndGuild.GuildWarScoreSetting(gainGuildID, opponentGuildID, point, winpoint)
	else:
		def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
			key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
			if not self.guildScoreBoardDict.has_key(key):
				return

			guildBoard = self.guildScoreBoardDict[key]
			guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	## PK Mode
	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	## Refine
	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type):
		self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)

	## Show & Hide
	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		self.wndInventory.Show()
		self.wndExpandedMoney.Show()
		self.wndChat.Show()

		self.wndMiniMap.Show()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()

		if app.ENABLE_ZODIAC_MISSION:
			if self.wnd12ziTimer:
				self.wnd12ziTimer.Show()
			if self.wnd12ziReward:
				self.wnd12ziReward.Show()

		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Open()

	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()

		if app.ENABLE_COLLECTIONS_SYSTEM:
			if self.wndCollections:
				self.wndCollections.Hide()

		if app.ENABLE_DETAILS_UI:
			if self.wndCharacter:
				self.wndCharacter.Close()
		else:
			if self.wndCharacter:
				self.wndCharacter.Hide()

		if self.wndInventory:
			self.wndInventory.Hide()

		if self.wndChat:
			self.wndChat.Hide()

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			if (self.wndImproveGemTicket):
				self.wndImproveGemTicket.Hide()

		if self.wndExpandedMoney:
			self.wndExpandedMoney.Hide()

		if app.ENABLE_ZODIAC_MISSION:
			if self.wnd12ziTimer:
				self.wnd12ziTimer.Hide()
			if self.wnd12ziReward:
				self.wnd12ziReward.Hide()

		if self.wndMiniMap:
			self.wndMiniMap.Hide()

		if self.wndMessenger:
			self.wndMessenger.Hide()

		if self.wndGuild:
			self.wndGuild.Hide()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.wndBiyologWindow:
				self.wndBiyologWindow.Hide()

		if self.wndExpandedMoneyTaskBar:
			self.wndExpandedMoneyTaskBar.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.wndSungMahiWindow:
				self.wndSungMahiWindow.Hide()

		if app.ENABLE_RIDING_EXTENDED:
			if self.wndMountUpGrade:
				self.wndMountUpGrade.Hide()

		if app.ENABLE_RESP_SYSTEM:
			if self.wndResp:
				self.wndResp.Hide()

		if app.ENABLE_RENEWAL_SWITCHBOT:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.wndSpecialInventory:
				self.wndSpecialInventory.Hide()

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if self.wndAura:
				self.wndAura.Hide()

		if app.ENABLE_YIN_AND_YANG_ATTR:
			if self.wndYinAndYangInventory and self.wndYinAndYangInventory.IsShow():
				self.wndYinAndYangInventory.Close()

			if self.wndYinAndYangHarmony and self.wndYinAndYangHarmony.IsShow():
				self.wndYinAndYangHarmony.Close()

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.Hide()
				if self.wndPetInfoWindow.wndPetMiniInfo:
					self.wndPetInfoWindow.wndPetMiniInfo.Hide()

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			if self.wndWarpWindow:
				self.wndWarpWindow.Hide()

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			if self.wndSpecialChat:
				self.wndSpecialChat.Hide()

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Hide()

			if self.wndHuntingSelect:
				self.wndHuntingSelect.Hide()

			if self.wndHuntingReward:
				self.wndHuntingReward.Hide()

		if app.ENABLE_COLLECT_WINDOW:
			if self.wndCollectWindow:
				self.wndCollectWindow.Hide()

		if app.ENABLE_RANKING:
			if self.wndRanking:
				self.wndRanking.Close()

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()
		
	def ToggleExpandedMoneyButton(self):
		if self.wndExpandedMoney.IsShow():
			self.wndExpandedMoney.Close()
		else:
			self.wndExpandedMoney.Open()

	def ToggleChat(self):
		if TRUE == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			if self.wndWeb and self.wndWeb.IsShow():
				pass
			else:
				self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	if app.ENABLE_RENEWAL_DEAD_PACKET:
		def OpenRestartDialog(self, d_time):
			self.dlgRestart.OpenDialog(d_time)
			self.dlgRestart.SetTop()
	else:
		def OpenRestartDialog(self):
			self.dlgRestart.OpenDialog()
			self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

	if app.ENABLE_SUNG_MAHI_TOWER:
		def SetSungMahiRestartDialog(self):
			self.dlgRestart.SetSungMahiRestartDialog()

	if app.ENABLE_GAYA_SYSTEM:
		def BINARY_OpenSelectItemWindowEx(self):
			self.wndItemSelectEx.Open()

		def BINARY_RefreshSelectItemWindowEx(self):
			self.wndItemSelectEx.RefreshSlot()

		def OpenGuiGem(self):
			self.wndGemG.Open()

		def GemCheck(self):
			self.wndGemG.SucceedGem()

		def OpenGuiGemMarket(self):
			self.wndGemM.Open()

		def GemMarketItems(self, vnums, gem, count):
			self.wndGemM.Information(vnums, gem, count)
			self.wndGemM.LoadInformation()

		def GemMarketSlotsDesblock(self, slot0, slot1, slot2, slot3, slot4, slot5):
			self.wndGemM.SlotsDesblock(slot0, slot1, slot2, slot3, slot4, slot5)

		def GemMarketClear(self):
			self.wndGemM.Clear()

		def GemTime(self,time):
			self.wndGemM.Time(time)

	def ToggleSystemDialog(self):
		if FALSE == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if FALSE == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def SetMapName(self, mapName):
		self.wndMiniMap.SetMapName(mapName)

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					if app.ENABLE_DETAILS_UI:
						self.wndCharacter.Close()
					else:
						self.wndCharacter.Hide()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if FALSE == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	if app.ENABLE_TITLE_SYSTEM:
		def TitleSystemSyncReset(self):
			if not self.wndCharacter:
				return
			try:
				self.wndCharacter.titleSystemUnlockedIDs = set()
			except:
				pass
			if self.wndCharacter.wndTitleSystem:
				self.wndCharacter.wndTitleSystem.SetUnlockedTitleIDs(set())
	
		def TitleSystemSyncAdd(self, titleID):
			if not self.wndCharacter:
				return
			try:
				titleID = int(titleID)
			except:
				return
			try:
				if not hasattr(self.wndCharacter, "titleSystemUnlockedIDs"):
					self.wndCharacter.titleSystemUnlockedIDs = set()
				self.wndCharacter.titleSystemUnlockedIDs.add(titleID)
			except:
				return
			if self.wndCharacter.wndTitleSystem:
				self.wndCharacter.wndTitleSystem.SetUnlockedTitleIDs(self.wndCharacter.titleSystemUnlockedIDs)
	
		def TitleSystemSyncActive(self, titleID):
			if not self.wndCharacter:
				return
			try:
				titleID = int(titleID)
			except:
				titleID = 0
			try:
				self.wndCharacter.titleSystemActiveID = titleID
			except:
				pass
			if self.wndCharacter.wndTitleSystem:
				self.wndCharacter.wndTitleSystem.SetActiveTitleID(titleID)

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")

	def ToggleInventoryWindow(self):
		if FALSE == player.IsObserverMode():
			if FALSE == self.wndInventory.IsShow():
				self.wndInventory.Show()
				self.wndInventory.SetTop()
				self.wndExpandedMoney.Show()
			else:
				self.wndInventory.OverOutItem()
				self.wndInventory.Close()
				self.wndExpandedMoney.Hide()

	if app.ENABLE_COLLECTIONS_SYSTEM:
		def ToggleCollectionsWindow(self):
			if FALSE == player.IsObserverMode():
				if self.wndCollections:
					if FALSE == self.wndCollections.IsShow():
						self.wndCollections.Show()
						self.wndCollections.SetTop()
					else:
						self.wndCollections.Hide()

	def ToggleExpandedButton(self):
		if False == player.IsObserverMode():
			if False == self.wndExpandedTaskBar.IsShow():
				self.wndExpandedTaskBar.Show()
				self.wndExpandedTaskBar.SetTop()
			else:
				self.wndExpandedTaskBar.Close()

	def ToggleExpandedMoneyButton(self):
		if not self.wndExpandedMoneyTaskBar:
			return
		if not self.wndExpandedMoneyTaskBar.IsShow():
			self.wndExpandedMoneyTaskBar.Show()
			self.wndExpandedMoneyTaskBar.SetTop()
		else:
			self.wndExpandedMoneyTaskBar.Close()

	# if app.ENABLE_COLLECT_WINDOW:
	# 	def ToggleQuest(self, id):
	# 		self.wndCollectWindow.UpdateQuest(str(id))

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def PetInformationActivate(self):
			if uiTaskBar.TaskBar.IS_EXPANDED:
				self.wndExpandedTaskBar.SetToolTipText(uiTaskBar.ExpandedTaskBar.BUTTON_PET_INFO, uiScriptLocale.TASKBAR_PET_INFO)

		def TogglePetInformationWindow(self):
			if False == player.IsObserverMode():
				if not self.wndPetInfoWindow.IsShow():
					self.wndPetInfoWindow.Show()
				else:
					self.wndPetInfoWindow.Close()

	# Skill Upgrader (reference: interfaceModule.Interface.OpenSkillUpgrade / ToggleSkillUpgrade / SendSkillUpgradeResult)
	def OpenSkillUpgrade(self, upgradeType = None, skillIndex = 0):
		if self.wndSkillUpgrade and False == player.IsObserverMode():
			self.wndSkillUpgrade.Open(upgradeType, skillIndex)

	def ToggleSkillUpgrade(self):
		if self.wndSkillUpgrade and False == player.IsObserverMode():
			self.wndSkillUpgrade.Toggle()

	def GetSkillUpgradeWnd(self):
		return self.wndSkillUpgrade

	def SetSkillUpgradeInfo(self, *args):
		if self.wndSkillUpgrade:
			self.wndSkillUpgrade.SetUpgradeInfo(*args)

	def SendSkillUpgradeResult(self, result):
		if self.wndSkillUpgrade:
			self.wndSkillUpgrade.ShowResult(result)

	def SkillUpgradeNegativeGradeConfirm(self):
		if self.wndSkillUpgrade:
			self.wndSkillUpgrade.ShowNegativeGradeConfirmDialog()

	def SkillUpgradeRefresh(self):
		if self.wndSkillUpgrade:
			self.wndSkillUpgrade.Refresh()

	# Buffi / companion (reference: interfaceModule.Interface.*Buffi*)
	def ToggleCompanionBuffiWnd(self):
		if self.wndCompanion and False == player.IsObserverMode():
			self.wndCompanion.Toggle()

	def SetBuffiInformation(self, *args):
		if self.wndCompanion:
			self.wndCompanion.SetBuffiInformation(*args)

	def SetBuffiSkills(self, *args):
		if self.wndCompanion:
			self.wndCompanion.SetBuffiSkills(*args)

	def SetBuffiSkillsBook(self, *args):
		if self.wndCompanion:
			self.wndCompanion.SetBuffiSkillsBook(*args)

	def SetBuffiSkillsCooldown(self, *args):
		if self.wndCompanion:
			self.wndCompanion.SetBuffiSkillsCooldown(*args)

	def SetBuffiName(self, name):
		if self.wndCompanion:
			self.wndCompanion.SetBuffiName(name)

	def Buffi_SetUnsummon(self):
		if self.wndCompanion:
			self.wndCompanion.UnsummonBuffi()

	def GetBuffiSkillPts(self):
		return self.wndCompanion.GetBuffiSkillPts() if self.wndCompanion else 0

	def GetBuffiSkillsPower(self):
		return self.wndCompanion.GetBuffiSkillsPower() if self.wndCompanion else []

	def GetBuffiSkillsBook(self):
		return self.wndCompanion.GetBuffiSkillsBook() if self.wndCompanion else []

	def GetBuffiSkillsCooldown(self):
		return self.wndCompanion.GetBuffiSkillsCooldown() if self.wndCompanion else []

	def ToggleGuildWindow(self):
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
			if not self.wndGuild.CanOpen():
				if self.wndGuild.GuildListDialogIsShow():
					self.wndGuild.CloseGuildListDialog()
				else:
					self.wndGuild.OpenGuildListDialog()
				return

		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	if app.ENABLE_COLLECT_WINDOW:
		def ToggleCollectWindow(self):
			if self.wndCollectWindow.IsShow():
				self.wndCollectWindow.Close()
			else:
				self.wndCollectWindow.Open()

	if app.ENABLE_RENEWAL_SWITCHBOT:
		def ToggleSwitchbotWindow(self):
			if self.wndSwitchbot.IsShow():
				self.wndSwitchbot.Close()
			else:
				self.wndSwitchbot.Open()

		def RefreshSwitchbotWindow(self):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotItem(slot)

	if app.ENABLE_SPECIAL_INVENTORY:
		def ToggleSpecialInventoryWindow(self):
			if FALSE == player.IsObserverMode():
				if FALSE == self.wndSpecialInventory.IsShow():
					self.wndSpecialInventory.Show()
					self.wndSpecialInventory.SetTop()
				else:
					self.wndSpecialInventory.OverOutItem()
					self.wndSpecialInventory.Close()

		def OpenSpecialInventoryWindow(self, category = 0):
			if FALSE == player.IsObserverMode():
				if FALSE == self.wndSpecialInventory.IsShow():
					self.wndSpecialInventory.Show()
					self.wndSpecialInventory.SetTop()
					self.wndSpecialInventory.SetInventoryType(category)

	if app.ENABLE_RENEWAL_SPECIAL_CHAT:
		def OnPickItem(self, item):
			item_vnum = int(item)
			self.SpecialChatOnPick(0, item_vnum)

		def SpecialChatOnPick(self, money = 0, item_vnum = 0):
			if money > 0:
				money_str = localeInfo.NumberToMoneyString(money)
				self.wndSpecialChat.AddSpecialChatLine(localeInfo.SPECIAL_CHAT_PICK % (money_str))

			if item_vnum > 0:
				import item
				item.SelectItem(item_vnum)
				self.wndSpecialChat.AddSpecialChatLine(localeInfo.SPECIAL_CHAT_PICK % (item.GetItemName()))

	def Highligt_Item(self, inven_type, inven_pos):
		if app.ENABLE_PICKUP_ITEM_EFFECT and player.INVENTORY == inven_type:
			self.wndInventory.HighlightSlot(inven_pos)
			if app.ENABLE_SPECIAL_INVENTORY:
				self.wndSpecialInventory.HighlightSlot(inven_pos)

	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	if app.ENABLE_SUNG_MAHI_TOWER:
		def ToggleSungMahiWindow(self):
			if self.wndSungMahiWindow.IsShow():
				self.wndSungMahiWindow.Close()
			else:
				self.wndSungMahiWindow.Open()
				
		def UpdateSungMahiNotice(self, noticeType, noticeText):
			if self.wndMiniMap.sungMahiCover.IsShow():
				if noticeType == 1:
					self.wndMiniMap.sungMahiCover.SetLevelDescriptionText(noticeText)
				else:
					self.wndMiniMap.sungMahiCover.SetLevelCurseText(noticeText, noticeType)
			
		def SungMahiClearNotice(self, noticeType):
			if self.wndMiniMap.sungMahiCover.IsShow():
				self.wndMiniMap.sungMahiCover.SungMahiClearNotice(noticeType)
			
		def UpdateRoomLevel(self, roomLevel):
			if self.wndMiniMap.sungMahiCover.IsShow():
				self.wndMiniMap.sungMahiCover.SetRoomLevel(roomLevel)
			
		def UpdateTowerLevel(self, towerLevel):
			if self.wndMiniMap.sungMahiCover.IsShow():
				self.wndMiniMap.sungMahiCover.SetLevelInfo(towerLevel)
			
		def UpdateRoomTime(self, roomTime):
			if self.wndMiniMap.sungMahiCover.IsShow():
				self.wndMiniMap.sungMahiCover.UpdateRoomTime(roomTime)

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")

	def __OnClickSkillPlusButton(self):
		self.ToggleCharacterWindow("SKILL")

	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	def OpenWebWindow(self, url):
		pass
		#self.wndWeb.Open(url)

	if app.ENABLE_DUNGEON_INFO:
		def DUNGEON_INFO_CHECK_SHOW(self):
			if self.wndTableDungeonInfo:
				if self.wndTableDungeonInfo.IsShow():
					self.wndTableDungeonInfo.Close()
				else:
					import dungeon_info
					dungeon_info.Open()
		def BINARY_TABLE_DUNGEON_INFO_OPEN(self):
			if self.wndTableDungeonInfo:
				self.wndTableDungeonInfo.LoadDateInfo()
				self.wndTableDungeonInfo.Show()

		def BINARY_TABLE_DUNGEON_RANKING_LOAD(self):
			if self.wndTableDungeonInfo:
				self.wndTableDungeonInfo.LoadRanking()

		def BINARY_TABLE_DUNGEON_MISION_LOAD(self):
			if self.wndTableDungeonInfo:
				self.wndTableDungeonInfo.LoadMision()

	# show GIFT
	def ShowGift(self):
		self.wndTaskBar.ShowGift()

	def CloseWbWindow(self):
		self.wndWeb.Close()

	if app.ENABLE_MINI_GAME_OKEY:
		def OpenCardsInfoWindow(self):
			self.wndCardsInfo.Open()

		def OpenCardsWindow(self, safemode):
			self.wndCards.Open(safemode)

		def UpdateCardsInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
			self.wndCards.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)

		def UpdateCardsFieldInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			self.wndCards.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

		def CardsPutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			self.wndCards.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

		def CardsShowIcon(self):
			self.wndCardsIcon.Show()

	if app.ENABLE_RENEWAL_CUBE:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			self.wndCubeRenewal.Show()

	#if app.ENABLE_BATTLE_PASS:
	#	def Show_BattlePass(self):
	#		self.wndGameButton.ShowPassButton()
	#
	#	def Hide_BattlePass(self):
	#		self.wndGameButton.HidePassButton()
	#
	#	def Show_BattlePass_Premium(self):
	#		self.wndGameButton.ShowPassButtonPremium()
	#
	#	def Hide_BattlePass_Premium(self):
	#		self.wndGameButton.HidePassButtonPremium()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if FALSE == self.wndInventory.IsShow():
			self.wndInventory.Show()
			self.wndExpandedMoney.Show()

	def ShowItemFinder(self):
		self.wndItemFinder.Show()

	def AppendInfoFinder(self, index, name_monster, prob, activi, vnum, count, name_item):
		self.wndItemFinder.AppendInfo(index, name_monster, prob, activi, vnum, count, name_item)

	if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
		def ShowHaloun(self):
			self.wndhaloun.Show()

	def UpdateCubeInfo(self, gold, itemVnum, count):
		self.wndCube.UpdateInfo(gold, itemVnum, count)

	def CloseCubeWindow(self):
		self.wndCube.Close()

	def FailedCubeWork(self):
		self.wndCube.Refresh()

	def SucceedCubeWork(self, itemVnum, count):
		self.wndCube.Clear()

		if 0:
			self.wndCubeResult.SetPosition(*self.wndCube.GetGlobalPosition())
			self.wndCubeResult.SetCubeResultItem(itemVnum, count)
			self.wndCubeResult.Open()
			self.wndCubeResult.SetTop()

	if constInfo.ENABLE_EXPANDED_TASKBAR:
		def PressExpandedTaskbar(self):
			if self.wndExpandedTaskBar.IsShow():
				self.wndExpandedTaskBar.Hide()
			else:
				self.wndExpandedTaskBar.Show()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			board = (self.wndAcceAbsorption,self.wndAcceCombine)[int(bWindow)]
			if iAct == 1:
				self.ActAcceOpen(board)
			elif iAct == 2:
				self.ActAcceClose(board)
			elif iAct == 3 or iAct == 4:
				self.ActAcceRefresh(board, iAct)

		def ActAcceOpen(self,board):
			if not board.IsOpened():
				board.Open()
			if not self.wndInventory.IsShow():
				self.wndInventory.Show()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceClose(self,board):
			if board.IsOpened():
				board.Close()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceRefresh(self,board,iAct):
			if board.IsOpened():
				board.Refresh(iAct)
			self.wndInventory.RefreshBagSlotWindow()

	if app.ENABLE_GUILDBANK_LOG:
		def OpenGuildBankInfo(self):
			self.wndGuildBank.Show()

		def GuildBankLogInfoRefresh(self):
			self.wndGuildBank.RefreshGuildBankInfo()

	if app.ENABLE_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_DONATE_ATTENDANCE:
		def GuildDonateInfoRefresh(self):
			guildDonateBoard = uiGuild.DonateGuildDialog()
			guildDonateBoard.RefreshGuildDonateInfo()

	if app.ENABLE_RESP_SYSTEM:
		def OpenRespWindow(self):
			if self.wndResp.IsShow():
				self.wndResp.Hide()
			else:
				self.wndResp.Show()

		def ClearLocationWindow(self):
			self.wndLocationWindow.ClearData()

		def UpdateLocationWindowPos(self, position, index, posx, posy):
			self.wndLocationWindow.AppendPosition(position, index, posx, posy)

		def UpdateLocationWindowName(self, name):
			self.wndLocationWindow.AppendName(name)

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndExpandedMoney,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.wndGameButton,

		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			if (self.wndImproveGemTicket):
				hideWindows += self.wndImproveGemTicket,

		if app.ENABLE_ZODIAC_MISSION:
			if self.wnd12ziTimer:
				hideWindows += self.wnd12ziTimer,
			if self.wnd12ziReward:
				hideWindows += self.wnd12ziReward,

		if app.ENABLE_COLLECTIONS_SYSTEM:
			hideWindows += self.wndCollections,

		if app.ENABLE_RIDING_EXTENDED and self.wndMountUpGrade:
			hideWindows += self.wndMountUpGrade,

		if app.ENABLE_RESP_SYSTEM and self.wndResp:
			hideWindows += self.wndResp,

		if app.ENABLE_RENEWAL_SWITCHBOT:
			if self.wndSwitchbot:
				hideWindows += self.wndSwitchbot,

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.wndSpecialInventory:
				hideWindows += self.wndSpecialInventory,

		if app.ENABLE_GROWTH_PET_SYSTEM:
			if self.wndPetInfoWindow:
				hideWindows += self.wndPetInfoWindow,

		if self.wndCompanion:
			hideWindows += self.wndCompanion,

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			if self.wndWarpWindow:
				hideWindows += self.wndWarpWindow,

		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			if self.wndSpecialChat:
				hideWindows += self.wndSpecialChat,

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				hideWindows += self.wndHunting,

			if self.wndHuntingSelect:
				hideWindows += self.wndHuntingSelect,

			if self.wndHuntingReward:
				hideWindows += self.wndHuntingReward,

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.wndBiyologWindow:
				hideWindows += self.wndBiyologWindow,

		if app.ENABLE_RANKING:
			if self.wndRanking:
				hideWindows += self.wndRanking,

		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)
		import sys

		self.HideAllQuestButton()
		self.HideAllWhisperButton()

		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def __ShowWindows(self, wnds):
		import sys
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	if app.ENABLE_COLLECTIONS_SYSTEM:
		def ToggleCollectionWindow(self):
			if self.wndCollections.IsShow():
				self.wndCollections.Hide()
			else:
				self.wndCollections.Show()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def BINARY_SetObserverMode(self, flag, isbuttonshow):
			self.wndGameButton.SetObserverMode(flag,isbuttonshow)
	else:
		def BINARY_SetObserverMode(self, flag):
			self.wndGameButton.SetObserverMode(flag)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	def OpenPrivateShopInputNameDialog(self):
		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if self.wndAura and self.wndAura.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_OPEN_OTHER_WINDOW)
				return

		if app.ENABLE_GROWTH_PET_SYSTEM:
			pet_id = player.GetActivePetItemId()
			if pet_id:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_SHOP_BECAUSE_SUMMON)
				return

		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return TRUE

	def OpenPrivateShopBuilder(self):
		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if self.wndAura and self.wndAura.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AURA_OPEN_OTHER_WINDOW)
				return

		if app.ENABLE_GROWTH_PET_SYSTEM:
			pet_id = player.GetActivePetItemId()
			if pet_id:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_OPEN_SHOP_BECAUSE_SUMMON)
				return

		if not self.inputDialog:
			return TRUE

		if not len(self.inputDialog.GetText()):
			return TRUE

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return TRUE

	def AppearPrivateShop(self, vid, text):

		board = uiPrivateShopBuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	def DisappearPrivateShop(self, vid):
		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiPrivateShopBuilder.DeleteADBoard(vid)

	### Equipment ###

	def OpenEquipmentDialog(self, vid):
		dlg = uiEquipmentDialog.EquipmentDialog()
		dlg.SetItemToolTip(self.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)

		self.equipmentDialogDict[vid] = dlg

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count, dwChangeLookVnum):
			if not vid in self.equipmentDialogDict:
				return
			self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count, dwChangeLookVnum)
	else:
		def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
			if not vid in self.equipmentDialogDict:
				return
			self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	if app.ENABLE_SUNGMA_SCORE:
		def SetEquipmentDialogSungmaScore(self, vid, slotIndex, value):
			if not vid in self.equipmentDialogDict:
				return
			self.equipmentDialogDict[vid].SetEquipmentDialogSungmaScore(slotIndex, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		del self.equipmentDialogDict[vid]

	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

	def RecvQuest(self, index, name):
		self.BINARY_RecvQuest(index, name, "file", localeInfo.GetLetterImageName())

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiWhisper.WhisperButton()

		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName
			btn.SetUpVisual(localeInfo.GetLetterCloseImageName())
			btn.SetOverVisual(localeInfo.GetLetterOpenImageName())
			btn.SetDownVisual(localeInfo.GetLetterOpenImageName())

		if not app.ENABLE_RENEWAL_QUEST:
			btn.SetToolTipText(name, -20, 35)
			btn.ToolTipText.SetHorizontalAlignLeft()

			btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
			btn.Show()
		else:
			btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)

		btn.index = index
		btn.name = name

		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20

		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:
			if app.ENABLE_RENEWAL_QUEST:
				btn.SetToolTipText(str(len(self.questButtonList)))
				btn.ToolTipText.SetHorizontalAlignCenter()

			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1

			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				if app.ENABLE_RENEWAL_QUEST and count > 0:
					btn.Hide()
				else:
					btn.Show()

	def __StartQuest(self, btn):
		if app.ENABLE_RENEWAL_QUEST:
			self.__OnClickQuestButton()
			self.HideAllQuestButton()
		else:
			event.QuestButtonClick(btn.index)
			self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()
			if app.ENABLE_RENEWAL_QUEST:
				break
	### Whisper ###

	def __InitWhisper(self):
		chat.InitWhisper(self)

	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(self.windowOpenPosition * 30,self.windowOpenPosition * 30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition + 1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		btn = self.__FindWhisperButton(name)
		if 0 != btn:
			self.__DestroyWhisperButton(btn)

		elif self.whisperDialogDict.has_key(name):
			oldDialog = self.whisperDialogDict[name]
			oldDialog.Destroy()
			del self.whisperDialogDict[name]

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.dlgWhisperWithoutTarget = None
		self.__CheckGameMaster(name)

	if app.ENABLE_EXTENDED_WHISPER_DETAILS and app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def RecieveWhisperDetails(self, name, country):
			if self.whisperDialogDict.has_key(name):
				self.whisperDialogDict[name].SetCountryFlag(country)
			else:
				btn = self.__FindWhisperButton(name)
				if btn != 0:
					btn.countryID = country

	def OpenWhisperDialog(self, name):
		if not self.whisperDialogDict.has_key(name):
			dlg = self.__MakeWhisperDialog(name)
			dlg.OpenWithTarget(name)
			dlg.chatLine.SetFocus()
			dlg.Show()

			self.__CheckGameMaster(name)
			btn = self.__FindWhisperButton(name)
			if 0 != btn:
				self.__DestroyWhisperButton(btn)

	def RecvWhisper(self, name):
		if not self.whisperDialogDict.has_key(name):
			btn = self.__FindWhisperButton(name)
			if 0 == btn:
				btn = self.__MakeWhisperButton(name)
				btn.Flash()

				if app.ENABLE_BLINK_ALERT:
					app.FlashApplication()

				chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))

			else:
				btn.Flash()

				if app.ENABLE_BLINK_ALERT:
					app.FlashApplication()

		elif self.IsGameMasterName(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def MakeWhisperButton(self, name):
		self.__MakeWhisperButton(name)

	def ShowWhisperDialog(self, btn):
		if app.ENABLE_WIKI_SYSTEM and btn.name == "[W?K?]":
			self.__DestroyWhisperButton(btn)
			self.OpenWikiWindow()
			return

		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		self.__DestroyWhisperButton(btn)

	def MinimizeWhisperDialog(self, name):

		if 0 != name:
			self.__MakeWhisperButton(name)

		self.CloseWhisperDialog(name)

	def CloseWhisperDialog(self, name):

		if 0 == name:

			if self.dlgWhisperWithoutTarget:
				self.dlgWhisperWithoutTarget.Destroy()
				self.dlgWhisperWithoutTarget = None

			return

		try:
			dlgWhisper = self.whisperDialogDict[name]
			dlgWhisper.Destroy()
			del self.whisperDialogDict[name]
		except:
			import dbg
			dbg.TraceError("interface.CloseWhisperDialog - Failed to find key")

	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	def ShowWhisperToolTip(self):
		if app.ENABLE_WHISPER_CLOSE_SHORTCUT and self.whisper_tooltip:
			self.whisper_tooltip.SetTop()
			self.whisper_tooltip.Show()

	def HideWhisperTooltip(self):
		if app.ENABLE_WHISPER_CLOSE_SHORTCUT and self.whisper_tooltip:
			self.whisper_tooltip.Hide()

	def __MakeWhisperButton(self, name):
		whisperButton = uiWhisper.WhisperButton()
		if app.ENABLE_WIKI_SYSTEM and name == "[W?K?]":
			whisperButton.SetUpVisual("d:/ymir work/ui/game/whisper/wikipedia.tga")
			whisperButton.SetOverVisual("d:/ymir work/ui/game/whisper/wikipedia_over.tga")
			whisperButton.SetDownVisual("d:/ymir work/ui/game/whisper/wikipedia.tga")
			whisperButton.SetToolTipTextWithColor(name, 0xffe7a83b)
		else:
			whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			if self.IsGameMasterName(name):
				whisperButton.SetToolTipTextWithColor(name, 0xffffa200)
			else:
				whisperButton.SetToolTipText(name)
		whisperButton.ToolTipText.SetHorizontalAlignCenter()
		whisperButton.SetEvent(ui.__mem_func__(self.ShowWhisperDialog), whisperButton)
		if app.ENABLE_WHISPER_CLOSE_SHORTCUT:
			whisperButton.SetOverInEvent(ui.__mem_func__(self.ShowWhisperToolTip))
			whisperButton.SetOverOutEvent(ui.__mem_func__(self.HideWhisperTooltip))
		import advanced_options_runtime
		if advanced_options_runtime.Get("whisper_sender_letter", True): whisperButton.Show()
		whisperButton.name = name

		self.whisperButtonList.insert(0, whisperButton)
		self.__ArrangeWhisperButton()

		return whisperButton

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()
		if app.ENABLE_WHISPER_CLOSE_SHORTCUT:
			self.HideWhisperTooltip()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			self.wndInventory.RefreshVisibleCostume()

	if app.ENABLE_SPECIAL_INVENTORY:
		def CloseLoadSafeboxEvent(self):
			if not self.wndSpecialInventory:
				return

			self.wndSpecialInventory.CloseLoadSafeboxEvent()

		def CloseLoadMallEvent(self):
			if not self.wndSpecialInventory:
				return

			self.wndSpecialInventory.CloseLoadMallEvent()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return TRUE
		else:
			return FALSE

	### Guild Building ###

	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiGuild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		# GUILD_BUILDING
		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return TRUE
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return TRUE
		# END_OF_GUILD_BUILDING
		return FALSE

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return TRUE

		return FALSE

	def BULID_EnterGuildArea(self, areaID):
		# GUILD_BUILDING
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return
		# END_OF_GUILD_BUILDING

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()

	if app.ENABLE_EVENT_SYSTEM:
		def ShowEventWindowDialog(self):
			self.wndEvent.Refresh()
			self.wndEvent.OpenEventInfoDialog()
			
		def RefreshEventWindowDialog(self):
			self.wndEvent.Refresh()
			
		def BossHuntersTimeStamp(self, day):
			self.wndMiniGameAttendance.MiniGameAttendanceSetDay(day)
			
		def BossHuntersClearMission(self, value):
			self.wndMiniGameAttendance.MiniGameAttendanceSetMissionClear(value)
			
		def OpenMiniGameAttendance(self):
			self.wndMiniGameAttendance.MiniGameAttendanceSetShowMax(8)
			self.wndMiniGameAttendance.Open()

	if app.ENABLE_FISH_EVENT_SYSTEM:
		def MiniGameFishUse(self, shape, useCount):
			if self.wndEvent:
				self.wndEvent.MiniGameFishUse(shape, useCount)
			
		def MiniGameFishAdd(self, pos, shape):
			if self.wndEvent:
				self.wndEvent.MiniGameFishAdd(pos, shape)
			
		def MiniGameFishReward(self, vnum):
			if self.wndEvent:
				self.wndEvent.MiniGameFishReward(vnum)
				
		def MiniGameFishCount(self, count):
			if self.wndEvent:
				self.wndEvent.MiniGameFishCount(count)
	
		def SetFishEventStatus(self, isEnable):
			if isEnable:
				if not self.wndEvent:
					self.wndEvent = uiMiniGame.MiniGameWindow()
	
					if self.tooltipItem:
						if self.wndEvent:
							self.wndEvent.SetItemToolTip(self.tooltipItem)
	
				self.wndEvent.IntegrationMiniGame(True)
			else:
				if self.wndEvent:
					self.wndEvent.IntegrationMiniGame(False)

	if app.ENABLE_MINI_GAME_CATCH_KING:
		def SetCatchKingEventStatus(self, isEnable):
			if isEnable:
				if not self.wndEvent:
					self.wndEvent = uiMiniGame.MiniGameWindow()
	
					if self.tooltipItem:
						if self.wndEvent:
							self.wndEvent.SetItemToolTip(self.tooltipItem)
	
				self.wndEvent.IntegrationMiniGame(True)
			else:
				if self.wndEvent:
					self.wndEvent.IntegrationMiniGame(False)

		def MiniGameCatchKingEventStart(self, bigScore):
			if self.wndEvent:
				self.wndEvent.MiniGameCatchKingEventStart(bigScore)

		def MiniGameCatchKingSetHandCard(self, cardNumber):
			if self.wndEvent:
				self.wndEvent.MiniGameCatchKingSetHandCard(cardNumber)

		def MiniGameCatchKingResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
			if self.wndEvent:
				self.wndEvent.MiniGameCatchKingResultField(score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

		def MiniGameCatchKingSetEndCard(self, cardPos, cardNumber):
			if self.wndEvent:
				self.wndEvent.MiniGameCatchKingSetEndCard(cardPos, cardNumber)

		def MiniGameCatchKingReward(self, rewardCode):
			if self.wndEvent:
				self.wndEvent.MiniGameCatchKingReward(rewardCode)

	if app.ENABLE_SOUL_ROULETTE_SYSTEM:
		def Roulette_Open(self, price, soul):
			if self.wndMiniGameRoulette:
				self.wndMiniGameRoulette.Show(price, soul)
		def Roulette_Close(self):
			if self.wndMiniGameRoulette:
				self.wndMiniGameRoulette.Hide()
		def Roulette_TurnWheel(self, spin, i):
			if self.wndMiniGameRoulette:
				self.wndMiniGameRoulette.TurnWheel(spin, i)
		def Roulette_SetIcons(self, i, vnum, count):
			if self.wndMiniGameRoulette:
				self.wndMiniGameRoulette.SetSlotItem(i, vnum, count)

	if app.ENABLE_ATTENDANCE_EVENT:
		def SetAttendanceEventStatus(self, isEnable):
			if isEnable:
				if not self.wndEvent:
					self.wndEvent = uiMiniGame.MiniGameWindow()
	
					if self.tooltipItem:
						if self.wndEvent:
							self.wndEvent.SetItemToolTip(self.tooltipItem)
	
				self.wndEvent.IntegrationMiniGame(True)
			else:
				if self.wndEvent:
					self.wndEvent.IntegrationMiniGame(False)
					
		def MiniGameAttendanceSetData(self, type, value):
			if self.wndMiniGame:
				self.wndMiniGame.MiniGameAttendanceSetData(type, value)

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	def AttachItemFromSafebox(self, slotIndex, itemIndex):
		if self.wndInventory and self.wndInventory.IsShow():
			self.wndInventory.AttachItemFromSafebox(slotIndex, itemIndex)

		return TRUE

	def AttachInvenItemToOtherWindowSlot(self, slotWindow, slotIndex):
		if self.wndSafebox and self.wndSafebox.IsShow():
			return self.wndSafebox.AttachItemFromInventory(slotWindow, slotIndex)

		return FALSE

	def EmptyFunction(self):
		pass

	if app.ENABLE_RANKING:
		def OpenRanking(self):
			if self.wndRanking:
				self.wndRanking.Open()

		def RankingClearData(self):
			if self.wndRanking:
				self.wndRanking.ClearData()

		def RankingAddRank(self, position, level, points, name, realPosition):
			if self.wndRanking:
				self.wndRanking.AddRank(position, name, points, level, realPosition)

		def RankingRefresh(self):
			if self.wndRanking:
				self.wndRanking.RefreshList()
				self.wndRanking.OnScroll()

	def IsShowDlgQuestionWindow(self):
		if self.wndInventory.IsDlgQuestionShow():
			return TRUE
		else:
			return FALSE

	def CloseDlgQuestionWindow(self):
		if self.wndInventory.IsDlgQuestionShow():
			self.wndInventory.CancelDlgQuestion()

	def DeactivateSlot(self, slotindex, type):
		self.wndInventory.DeactivateSlot(slotindex, type)

	def ActivateSlot(self, slotindex, type):
		self.wndInventory.ActivateSlot(slotindex, type)

	if app.ENABLE_RENDER_TARGET:
		def MakeRenderTargetWindow(self):
			if self.wndRenderTarget == None:
				self.wndRenderTarget = uiRenderTarget.RenderTargetWindow()

		def OpenRenderTargetWindow(self, renderType = 0, renderVnum = 11299):
			self.MakeRenderTargetWindow()
			self.wndRenderTarget.Open(renderType, renderVnum)

	if app.ENABLE_RENEWAL_OFFLINESHOP:
		def MakeOfflineShopMessage(self):
			if self.dlgShopMessage == None:
				self.dlgShopMessage = ui.MessageWindow()

		def AddOfflineShopMessage(self, itemVnum, itemCount, itemPrice):
			itemVnum, itemCount, itemPrice = (int(itemVnum), int(itemCount), int(itemPrice))
			self.MakeOfflineShopMessage()
			item.SelectItem(itemVnum)

			if itemCount > 1:
				message = localeInfo.OFFLINESHOP_ITEM_SOLD_MESSAGE_WITH_COUNT % (itemCount, item.GetItemName(), localeInfo.NumberToMoneyString(itemPrice))
			else:
				message = localeInfo.OFFLINESHOP_ITEM_SOLD_MESSAGE % (item.GetItemName(), localeInfo.NumberToMoneyString(itemPrice))
			visible = True
			if app.ENABLE_ADVANCED_GAME_OPTIONS:
				import advanced_option_notifications
				visible = advanced_option_notifications.Record("market_sales", message)
			if visible:
				self.dlgShopMessage.AddMessage(message, 5)

		def OpenOfflineShopDialog(self, vid, isOwner):
			self.wndInventory.Show()
			self.wndInventory.SetTop()

			if isOwner:
				self.OpenOfflineShopPanel()
			else:
				if self.dlgOfflineShop == None:
					self.MakeOfflineShop()

				if self.dlgOfflineShop:
					self.dlgOfflineShop.Open(vid)
					self.dlgOfflineShop.SetCenterPosition()
					self.dlgOfflineShop.SetTop()

		def OpenOfflineShopPanel(self):
			if self.dlgOfflineShopPanel == None:
				self.MakeOfflineShopPanel()

			self.dlgOfflineShopPanel.Open()
			self.dlgOfflineShopPanel.SetTop()
			self.dlgOfflineShopPanel.SetCenterPosition()

		def OpenOfflineShopBuilder(self):
			if self.offlineShopBuilder == None:
				self.MakeOfflineShopBuilder()

			self.offlineShopBuilder.Open(".....")
			self.offlineShopBuilder.SetTop()

		def DisappearOfflineShop(self, vid):
			uiOfflineShopBuilder.HideADBoardWithKey(vid)

		def AppearOfflineShop(self, vid, text):
			type = 0
			if text[0].isdigit():
				type = int(text[0])
				text = text[1:]
			if uiOfflineShopBuilder.ShowADBoardWithKey(vid) == FALSE:
				board = uiOfflineShopBuilder.OfflineShopAdvertisementBoard(type)
				board.Open(vid, text)
			else:
				uiOfflineShopBuilder.UpdateADText(vid, type, text)

		def MakeOfflineShopBuilder(self):
			self.offlineShopBuilder = uiOfflineShopBuilder.OfflineShopBuilder()
			self.offlineShopBuilder.BindInterfaceClass(self)
			self.offlineShopBuilder.Hide()

		def MakeOfflineShop(self):
			self.dlgOfflineShop = uiOfflineShop.OfflineShopDialog()
			self.dlgOfflineShop.BindInterfaceClass(self)
			self.dlgOfflineShop.LoadDialog()
			self.dlgOfflineShop.Hide()

		def MakeOfflineShopPanel(self):
			self.dlgOfflineShopPanel = uiOfflineShop.OfflineMyShop()
			self.dlgOfflineShopPanel.BindInterfaceClass(self)
			self.dlgOfflineShopPanel.LoadWindow()

		def AppendLogOfflineShopPanel(self, name, date, itemvnum, itemcount, price):
			if self.dlgOfflineShopPanel == None:
				self.MakeOfflineShopPanel()

			self.dlgOfflineShopPanel.AppendLog(name, date, itemvnum, itemcount, price)

		def AppendLogOfflineShopPanelFirst(self, name, date, itemvnum, itemcount, price):
			if self.dlgOfflineShopPanel == None:
				self.MakeOfflineShopPanel()

			self.dlgOfflineShopPanel.AppendLogFirst(name, date, itemvnum, itemcount, price)

		def ToggleOfflineShopWindow(self):
			if self.dlgOfflineShopPanel:
				if self.dlgOfflineShopPanel.IsShow():
					self.dlgOfflineShopPanel.CloseReal()
					return

			if self.offlineShopBuilder:
				if self.offlineShopBuilder.IsShow():
					self.offlineShopBuilder.CloseReal()
					return

			net.SendOfflineShopButton()

	if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
		def __MakePrivateShopSearchWindow(self):
			self.wndPrivateShopSearch = uiPrivateShopSearch.PrivateShopSearchDialog()
			self.wndPrivateShopSearch.BindInterfaceClass(self)
			self.wndPrivateShopSearch.LoadWindow()

		def OfflineShopBuyed(self, itemID):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.SetItemBuyStatus(int(itemID))

		def SearchShopForItem(self, itemVnum):
			if not itemVnum:
				return
			if self.wndPrivateShopSearch == None:
				self.__MakePrivateShopSearchWindow()
			self.wndPrivateShopSearch.OpenForItem(itemVnum)

		def OpenPrivateShopSearch(self, type = 0):
			if self.wndPrivateShopSearch == None:
				self.__MakePrivateShopSearchWindow()
				self.wndPrivateShopSearch.Open(type)
				return

			if self.wndPrivateShopSearch.IsShow():
				self.wndPrivateShopSearch.Hide()
			else:
				self.wndPrivateShopSearch.Open(type)

		def RefreshShopSearch(self):
			if self.wndPrivateShopSearch == None:
				self.__MakePrivateShopSearchWindow()

			self.wndPrivateShopSearch.RefreshMe()
			self.wndPrivateShopSearch.RefreshList()

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		def OnChangePickUPMode(self):
			self.dlgSystem.OnChangePickUPMode()

	if app.ENABLE_RENEWAL_SKILL_SELECT:
		def OpenSkillSelectWindow(self, job):
			self.wndSkillSelect.Open(job)

	if app.ENABLE_NEW_DUNGEON_LIB:
		def MakeDungeonTimerWindow(self):
			self.wndDungeonTimer = uiDungeonTimer.Cooldown()
			self.wndDungeonTimer.Hide()

	if app.ENABLE_DUNGEON_INFO:
		def MakeDungeonInfo(self):
			if self.wndDungeonInfo == None:
				self.wndDungeonInfo = uiDungeonInfo.DungeonInfo()

		def OpenDungeonInfo(self):
			self.MakeDungeonInfo()
			if self.wndDungeonInfo.IsShow():
				self.wndDungeonInfo.Close()
			else:
				self.wndDungeonInfo.Open()

	if app.ENABLE_ITEMSHOP:
		def MakeItemShopWindow(self):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

		def OpenItemShopWindow(self):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

			if self.wndItemShop.IsShow():
				self.wndItemShop.Close()
			else:
				self.wndItemShop.Open()

		def OpenItemShopMainWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.LoadFirstOpening()

		def ItemShopHideLoading(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.CloseLoading()

		def ItemShopPurchasesWindow(self):
			self.MakeItemShopWindow()
			self.wndItemShop.Open()
			self.wndItemShop.OpenPurchasesWindow()

		def ItemShopUpdateItem(self, itemID, itemMaxSellingCount):
			self.MakeItemShopWindow()
			self.wndItemShop.UpdateItem(itemID, itemMaxSellingCount)

		#USE_ITEMSHOP_RENEWED: @lldJCoins
		def ItemShopSetDragonCoin(self, lldCoins, lldJCoins = 0):
			if not self.wndItemShop:
				self.wndItemShop = uiItemShopNew.ItemShopWindow()

			self.wndItemShop.SetDragonCoin(lldCoins)

			if app.USE_ITEMSHOP_RENEWED:
				self.wndItemShop.SetDragonJeton(lldJCoins)

		def SetWheelItemData(self, cmd):
			self.MakeItemShopWindow()
			self.wndItemShop.SetWheelItemData(str(cmd))

		def OnSetWhell(self, giftIndex):
			self.MakeItemShopWindow()
			self.wndItemShop.OnSetWhell(int(giftIndex))

		def GetWheelGiftData(self, itemVnum, itemCount):
			self.MakeItemShopWindow()
			self.wndItemShop.GetWheelGiftData(int(itemVnum), int(itemCount))

	if app.ENABLE_WIKI_SYSTEM:
		def SearchWikiForItem(self, itemVnum):
			if not itemVnum:
				return
			if self.wndWiki == None:
				self.__MakeWiki()
			if self.wndWiki:
				if not self.wndWiki.IsShow():
					self.wndWiki.Open()
				self.wndWiki.SearchItem(itemVnum)

		def OpenWikiWindow(self):
			if self.wndWiki == None:
				self.__MakeWiki()
			if self.wndWiki and self.wndWiki.IsShow():
				self.wndWiki.Close()
			elif self.wndWiki:
				self.wndWiki.Open()

		def __MakeWiki(self):
			if self.wndWiki == None:
				self.wndWiki = uiWiki.EncyclopediaofGame()
				self.wndInventory.BindWikiWindow(self.wndWiki)
				if hasattr(self, 'wndInventoryNew') and self.wndInventoryNew:
					self.wndInventoryNew.BindWikiWindow(self.wndWiki)

		def RecvReadWiki(self):
			name = "[W?K?]"
			if not self.whisperDialogDict.has_key(name):
				btn = self.__FindWhisperButton(name)
				if 0 == btn:
					btn = self.__MakeWhisperButton(name)
					btn.Flash()
					if app.ENABLE_BLINK_ALERT:
						app.FlashApplication()
					chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))
				else:
					btn.Flash()
					if app.ENABLE_BLINK_ALERT:
						app.FlashApplication()


	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def MarkUnusableInvenSlotOnTopWnd(self, onTopWnd, InvenSlot):
			if app.ENABLE_SLOT_MARKING_SYSTEM:
				if onTopWnd == player.ON_TOP_WND_SHOP and self.dlgShop and self.dlgShop.CantSellInvenItem(InvenSlot):
					return TRUE
				elif onTopWnd == player.ON_TOP_WND_SAFEBOX and self.wndSafebox and self.wndSafebox.CantCheckInItem(InvenSlot):
					return TRUE
				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP and self.privateShopBuilder:
					if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
						if self.privateShopBuilder.CantTradableItem(InvenSlot, player.INVENTORY):
							return TRUE
					else:
						if self.privateShopBuilder.CantTradableItem(InvenSlot):
							return TRUE
				elif onTopWnd == player.ON_TOP_WND_EXCHANGE and self.dlgExchange and self.dlgExchange.CantTradableItem(InvenSlot):
					return TRUE

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if onTopWnd == player.ON_TOP_WND_ACCE_COMBINE and self.wndAcceCombine and self.wndAcceCombine.CantTradableItem(InvenSlot):
					return TRUE
				elif onTopWnd == player.ON_TOP_WND_ACCE_ABSORB and self.wndAcceAbsorption and self.wndAcceAbsorption.CantTradableItem(InvenSlot):
					return TRUE

			return FALSE

		def SetOnTopWindow(self, onTopWnd):
			self.OnTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.OnTopWindow

		def MarkUnusableDSInvenSlotOnTopWnd(self, onTopWnd, slotIndex):
			if onTopWnd == player.ON_TOP_WND_SHOP and self.dlgShop and self.dlgShop.CantSellDSItem(slotIndex):
				return TRUE
			elif onTopWnd == player.ON_TOP_WND_SAFEBOX and self.wndSafebox and self.wndSafebox.CantCheckInDSItem(slotIndex):
				return TRUE
			elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP and self.privateShopBuilder:
				if app.ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING:
					if self.privateShopBuilder.CantTradableItem(slotIndex, player.DRAGON_SOUL_INVENTORY):
						return TRUE
			elif onTopWnd == player.ON_TOP_WND_EXCHANGE and self.dlgExchange:
				if self.dlgExchange.CantTradableItem(slotIndex, player.DRAGON_SOUL_INVENTORY):
					return TRUE
			return FALSE

		def RefreshMarkInventoryBag(self):
			if self.wndInventory and self.wndInventory.IsShow():
				self.wndInventory.RefreshBagSlotWindow()

			if app.ENABLE_SPECIAL_INVENTORY:
				if self.wndSpecialInventory and self.wndSpecialInventory.IsShow():
					self.wndSpecialInventory.RefreshBagSlotWindow()

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def AuraWindowOpen(self, type):
			if self.wndAura.IsShow():
				return

			if self.inputDialog or self.privateShopBuilder.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_OPEN_PRIVATE_SHOP)
				return

			if self.dlgRefineNew and self.dlgRefineNew.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_NOT_OPEN_REFINE)
				return

			self.wndAura.Open(type)

			if not self.wndInventory.IsShow():
				self.wndInventory.Show()

		def AuraWindowClose(self):
			if not self.wndAura.IsShow():
				return

			self.wndAura.CloseWindow()

	if app.ENABLE_ZODIAC_MISSION:
		def __Make12ziRewardWindow(self):
			self.wnd12ziReward = ui12zi.Reward12ziWindow()
			self.wnd12ziReward.SetItemToolTip(self.tooltipItem)
			self.wnd12ziReward.Hide()
		def __Make12ziTimerWindow(self):
			self.wnd12ziTimer = ui12zi.FloorLimitTimeWindow()
			self.wnd12ziTimer.Hide()

	if app.ENABLE_TRACK_WINDOW:
		def TrackWindowCheckPacket(self):
			self.wndTrackWindow.CheckPacket()
		def MakeTrackWindow(self):
			if self.wndTrackWindow == None:
				self.wndTrackWindow = uiTrack.TrackWindow()
		def OpenTrackWindow(self):
			self.MakeTrackWindow()
			if self.wndTrackWindow.IsShow():
				self.wndTrackWindow.Close()
			else:
				self.wndTrackWindow.Open()
		def TrackDungeonInfo(self, cmdData):
			self.MakeTrackWindow()
			self.wndTrackWindow.TrackDungeonInfo(cmdData)
		def TrackBossInfo(self, bossID, bossLeftTime, bossChannel):
			self.MakeTrackWindow()
			self.wndTrackWindow.TrackBossInfo(int(bossID), int(bossLeftTime), int(bossChannel))

	if app.ENABLE_FISH_GAME:
		def MakeFishGameWindow(self):
			if self.wndFishGame == None:
				self.wndFishGame = uiFishGame.FishGameWindow()

		def OpenFishGameWindow(self, gameKey):
			self.MakeFishGameWindow()
			self.wndFishGame.Open(int(gameKey))

		def SetFishGameGoal(self, goalCount):
			self.MakeFishGameWindow()
			self.wndFishGame.SetFishScore(int(goalCount))

		def CloseFishGame(self):
			self.MakeFishGameWindow()
			self.wndFishGame.Hide()

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def PetHatchingWindowCommand(self, command):
			if self.wndPetInfoWindow:
				if self.wndPetInfoWindow.wndPetHatching:
					self.wndPetInfoWindow.wndPetHatching.PetHatchingWindowCommand(command)

		def PetNameChangeWindowCommand(self, command):
			if self.wndPetInfoWindow:
				if self.wndPetInfoWindow.wndPetNameChange:
					self.wndPetInfoWindow.wndPetNameChange.PetNameChangeWindowCommand(command)

		def PetSkillUpgradeDlgOpen(self, slot, index, gold):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.OpenPetSkillUpGradeQuestionDialog(slot, index, gold)

		def PetFlashEvent(self, index):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetFlashEvent(index)

		def PetInfoBindAffectShower(self, affect_shower):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetInfoBindAffectShower(affect_shower)

		def PetAffectShowerRefresh(self):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetAffectShowerRefresh()

		def PetFeedReuslt(self, result):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetFeedReuslt(result)

		def PetAttrDetermineResult(self, byType):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetAttrDetermineResult(byType)

		def PetAttrChangeResult(self, slotIndex, byType):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetAttrChangeResult(slotIndex, byType)

		def PetReviveResult(self, result):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetReviveResult(result)

		def PetWindowTypeResult(self, result):
			if self.wndPetInfoWindow:
				self.wndPetInfoWindow.PetWindowTypeResult(result)

		def TogglePetInformationWindow(self):
			if FALSE == player.IsObserverMode():
				if not self.wndPetInfoWindow.IsShow():
					self.wndPetInfoWindow.Show()
				else:
					self.wndPetInfoWindow.Close()

	if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
		def OpenWarpWindow(self):
			if self.wndWarpWindow.IsShow():
				self.wndWarpWindow.Close()
			else:
				self.wndWarpWindow.Open()

	if app.ENABLE_TRACK_WINDOW:
		def TrackWindowCheckPacket(self):
			self.wndTrackWindow.CheckPacket()

		def MakeTrackWindow(self):
			if self.wndTrackWindow == None:
				self.wndTrackWindow = uiTrack.TrackWindow()

		def OpenTrackWindow(self):
			self.MakeTrackWindow()
			if self.wndTrackWindow.IsShow():
				self.wndTrackWindow.Close()
			else:
				self.wndTrackWindow.Open()

		def TrackDungeonInfo(self, cmdData):
			self.MakeTrackWindow()
			self.wndTrackWindow.TrackDungeonInfo(cmdData)

		def TrackBossInfo(self, bossID, bossLeftTime, bossChannel):
			self.MakeTrackWindow()
			self.wndTrackWindow.TrackBossInfo(int(bossID), int(bossLeftTime), int(bossChannel))

		def TrackRecvInfo(self, content):
			content = content.replace("$", " ")
			self.RegisterGameMasterName(localeInfo.SYSTEM_WHISPER)
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, localeInfo.SYSTEM_WHISPER, content)
			self.RecvWhisper(localeInfo.SYSTEM_WHISPER)


	if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
		def ItemCombinationDialogOpen(self):
			if self.wndItemCombination:
				self.wndItemCombination.Open()

		def ItemCombinationDialogClose(self):
			if self.wndItemCombination and self.wndItemCombination.IsShow():
				self.wndItemCombination.Close()
	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def ChangeWindowOpen(self, type):
			if self.wndChangeLook:
				self.wndChangeLook.Open(type)

		def IsShowDlgQuestionWindow(self):
			if self.wndInventory.IsDlgQuestionShow():
				return TRUE
			else:
				return FALSE

		def CloseDlgQuestionWindow(self):
			if self.wndInventory.IsDlgQuestionShow():
				self.wndInventory.CancelDlgQuestion()

	def SetAffectShower(self, shower):
		import _weakref
		self.uiAffectshower = _weakref.proxy(shower)
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.wndMiniMap:
				self.uiAffectshower.SetMiniMapWidth(self.wndMiniMap.GetWidth())
			if self.wndParty:
				self.uiAffectshower.SetPartyWindow(self.wndParty)
		if app.ENABLE_STATUS_EFFECT_SHOWER:
			try:
				if self.uitargetBoard:
					self.uitargetBoard.BindAffectShower(shower)
			except (AttributeError, TypeError):
				pass

	def GetAffectData(self, affect_type):
		if self.uiAffectshower == None:
			return None
		return self.uiAffectshower.GetAffectData(affect_type)

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		if app.ENABLE_GUILD_REQUEST:
			def RefreshGuildRankingList(self, issearch):
				if self.wndGuild:
					self.wndGuild.RefreshGuildRankingList(issearch)

			def CloseGuildRankWindow(self):
				if self.wndGuild:
					self.wndGuild.CloseGuildListDialog()

		def ShowGuildWarButton(self):
			if self.wndGameButton:
				self.wndGameButton.ShowGuildWarButton()

		def HideGuildWarButton(self):
			if self.wndGameButton:
				self.wndGameButton.HideGuildWarButton()

	if app.ENABLE_HUNTING_SYSTEM:
		def ToggleHuntingWindow(self):
			if self.wndHunting.IsShow():
				self.wndHunting.Close()
			elif self.wndHuntingSelect.IsShow():
				self.wndHuntingSelect.Close()
			else:
				net.SendHuntingAction(1, 0)

		def OpenHuntingWindowMain(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting:
				self.wndHunting.OpenMain(level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
				self.wndHunting.SetTop()
				constInfo.HUNTING_BUTTON_FLASH = 0

		def OpenHuntingWindowSelect(self, level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting and self.wndHunting.IsShow():
				self.wndHunting.CloseWithMini()
			self.wndHuntingSelect.OpenSelect(level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
			constInfo.HUNTING_BUTTON_FLASH = 0

		def OpenHuntingWindowReward(self, level, reward, reward_count, random_reward, random_reward_count, money, exp):
			if False == self.wndHuntingReward.IsShow():
				self.wndHuntingReward.OpenReward(level, reward, reward_count, random_reward, random_reward_count, money, exp)
				self.wndHuntingReward.SetTop()

		def UpdateHuntingMission(self, count):
			if self.wndHunting:
				self.wndHunting.UpdateMission(count)

		def HuntingSetRandomItemsMain(self, item_vnum, item_count):
			if self.wndHunting:
				self.wndHunting.SetRandomItemTable(item_vnum, item_count)

		def HuntingSetRandomItemsSelect(self, item_vnum, item_count):
			if self.wndHuntingSelect:
				self.wndHuntingSelect.SetRandomItemTable(item_vnum, item_count)

	if app.ENABLE_RIDING_EXTENDED:
		def MountUpGradeWindow(self):
			if not player.IsObserverMode():
				if not self.wndMountUpGrade.IsShow():
					self.wndMountUpGrade.Open()
				else:
					self.wndMountUpGrade.Close()

	if app.__DUNGEON_INFO__:
		def DungeonInfoQuestIdx(self, questIdx):
			constInfo.dungeonInfoQuestIdx = int(questIdx)

		def DungeonInfoQuestCMD(self):
			net.SendQuestInputStringPacket(constInfo.dungeonInfoCMD)
			constInfo.dungeonInfoCMD = ""

		def OpenDungeonInfo(self):
			try:
				if self.wndDungeonTrack == None:
					self.wndDungeonTrack = uiDungeonTrack.DungeonTrack()
				if self.wndDungeonTrack.IsShow():
					self.wndDungeonTrack.Close()
				else:
					self.wndDungeonTrack.Open()
			except Exception, e:
				import dbg
				dbg.TraceError("OpenDungeonInfo Error: %s" % str(e))

		def DungeonInfoCooldown(self, clean, data):
			dungeonData = uiDungeonTrack.dungeonData
			if int(clean):
				for mobIdx, dungeonInfo in dungeonData.items():
					dungeonInfo["server_cooldown"] = 0
					dungeonInfo["server_completed"] = 0
					dungeonInfo["server_fastest"] = 0
					dungeonInfo["server_damage"] = 0
					dungeonInfo["whisper_info"] = False
			if data != "" and data != "-":
				dungeonList = data[:len(data)-1].split("#")
				for dungeonInfo in dungeonList:
					dungeonSplitData = dungeonInfo.split("|")
					if len(dungeonSplitData) == 5:
						bossIdx = int(dungeonSplitData[0])
						if dungeonData.has_key(bossIdx):
							if dungeonData[bossIdx]["server_cooldown"] != app.GetGlobalTimeStamp() + int(dungeonSplitData[1]):
								dungeonData[bossIdx]["whisper_info"] = False
							dungeonData[bossIdx]["server_cooldown"] = 0 if int(dungeonSplitData[1]) <= 0 else app.GetGlobalTimeStamp() + int(dungeonSplitData[1])
							dungeonData[bossIdx]["server_completed"] = int(dungeonSplitData[2])
							dungeonData[bossIdx]["server_fastest"] = int(dungeonSplitData[3])
							dungeonData[bossIdx]["server_damage"] = int(dungeonSplitData[4])
			if self.wndDungeonManager == None:
				self.wndDungeonManager = uiDungeonTrack.DungeonManager()
			if self.wndDungeonTrack == None:
				self.wndDungeonTrack = uiDungeonTrack.DungeonTrack()
			self.wndDungeonTrack.Refresh()

		def LoadServerData(self, mobIdx, rankIdx, cmd):
			if self.wndDungeonTrack != None:
				self.wndDungeonTrack.LoadServerData(int(mobIdx), int(rankIdx), cmd)

	if app.__AUTO_HUNT__:
		def AutoHuntStatus(self, status):
			if self.wndAutoHunt:
				self.wndAutoHunt.SetStatus(True if int(status) else False)
		def CheckAutoLogin(self):
			if self.wndAutoHunt == None:
				self.wndAutoHunt = uiAutoHunt.Window()
			self.wndAutoHunt.CheckAutoLogin()
		def OpenAutoHunt(self):
			if self.wndAutoHunt == None:
				self.wndAutoHunt = uiAutoHunt.Window()
			if self.wndAutoHunt.IsShow():
				self.wndAutoHunt.Close()
			else:
				self.wndAutoHunt.Open()
		def ToggleAutoHuntWindow(self):
			if self.wndAutoHunt == None:
				self.wndAutoHunt = uiAutoHunt.Window()
			if self.wndAutoHunt.IsShow():
				self.wndAutoHunt.Close()
			else:
				self.wndAutoHunt.Open()

	if app.ENABLE_BIOLOGIST_SYSTEM:
		def SetBiyologMission(self, mission, missionState, givenCount, needCount, remainingTime):
			if self.wndBiyologWindow:
				self.wndBiyologWindow.SetMission(mission, missionState, givenCount, needCount, remainingTime)

		def OpenBiyologDialog(self):
			if self.wndBiyologWindow:
				self.wndBiyologWindow.Open()


	if app.ENABLE_GAYA_TICKET_SYSTEM:
		def OpenGemTicket(self, itemPos):
			self.wndImproveGemTicket.Open(itemPos)

	if app.ENABLE_YIN_AND_YANG_ATTR:
		def OpenYinAndYangInventory(self):
			if self.wndYinAndYangInventory:
				self.wndYinAndYangInventory.Open()

		def CloseYinAndYangInventory(self):
			if self.wndYinAndYangInventory:
				self.wndYinAndYangInventory.Close()

		def ToggleYinAndYangInventory(self):
			if self.wndYinAndYangInventory:
				if self.wndYinAndYangInventory.IsShow():
					self.wndYinAndYangInventory.Close()
				else:
					self.wndYinAndYangInventory.Open()

		def OpenYinAndYangHarmony(self):
			if self.wndYinAndYangHarmony:
				self.wndYinAndYangHarmony.Open()

		def CloseYinAndYangHarmony(self):
			if self.wndYinAndYangHarmony:
				self.wndYinAndYangHarmony.Close()

		def ToggleYinAndYangHarmony(self):
			if self.wndYinAndYangHarmony:
				if self.wndYinAndYangHarmony.IsShow():
					self.wndYinAndYangHarmony.Close()
				else:
					self.wndYinAndYangHarmony.Open()

if __name__ == "__main__":

	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui
	import localeInfo

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(TRUE)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create(localeInfo.APP_TITLE, systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	class TestGame(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)

			localeInfo.LoadLocaleData()
			player.SetItemData(0, 27001, 10)
			player.SetItemData(1, 27004, 10)

			self.interface = Interface()
			self.interface.MakeInterface()
			self.interface.ShowDefaultWindows()
			self.interface.RefreshInventory()

		def __del__(self):
			ui.Window.__del__(self)

		def OnUpdate(self):
			app.UpdateGame()

		def OnRender(self):
			app.RenderGame()
			grp.PopState()
			grp.SetInterfaceRenderState()

	game = TestGame()
	game.SetSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	game.Show()

	app.Loop()
