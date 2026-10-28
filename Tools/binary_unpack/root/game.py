if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import os
import dbg
import grp
import item
import background
import event
import snd
import chat
import textTail
import snd
import effect
import wndMgr
import fly
import systemSetting
import nonplayer
import quest
import guild
import skill
import messenger
import serverInfo
import localeInfo
import constInfo
import exchange
import ime

if app.ENABLE_ITEMSHOP:
	import uiItemShopNew

import ui
import uiCommon
import uiPhaseCurtain
import uiMapNameShower
import uiAffectShower
import uiPlayerGauge
import uiCharacter
import uiTarget
import uiPrivateShopBuilder

import mouseModule
import consoleModule
import localeInfo

import playerSettingModule
import interfaceModule

import musicInfo
import debugInfo
import stringCommander
import event

# if app.ENABLE_GUILDRENEWAL_SYSTEM:
	# import uiGuildPopup
	# import uiGuildBank

if app.ENABLE_RENEWAL_OFFLINESHOP:
	import uiOfflineShopBuilder
	import shop

if app.ENABLE_MAINTENANCE_SYSTEM:
	import uiMaintenance

if app.ENABLE_DEFENSAWESHIP:
	import uiShipMastHP

if app.ENABLE_SOCCER_BALL_EVENT:
	import uifutboltopu

if app.ENABLE_SKILL_BOOK_READING:
	import uibkoku

if app.ENABLE_SPIRIT_STONE_READING:
	import uiruhtasi

if app.ENABLE_WORD_GAME_EVENT:
	import uiWordGameSystem

if app.ENABLE_MINI_GAME_CATCH_KING:
	import uiminigamecatchking

if app.ENABLE_STONE_EVENT:
	import uistone_event

if app.ENABLE_REMOTE_SHOP_SYSTEM:
	import uiRemoteShop

import uispeedbutton

if app.ENABLE_FISH_EVENT_SYSTEM:
	import uiMiniGameFishEvent
	#import uiMiniGame

if app.ENABLE_ATTENDANCE_EVENT:
	import uiMiniGameAttendance

from _weakref import proxy

SCREENSHOT_CWDSAVE = TRUE
SCREENSHOT_DIR = None

cameraDistance = 1550.0
cameraPitch = 27.0
cameraRotation = 0.0
cameraHeight = 100.0

testAlignment = 0

class GameWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.ScriptWindow.__init__(self, "GAME")
		self.SetWindowName("game")
		net.SetPhaseWindow(net.PHASE_WINDOW_GAME, self)
		player.SetGameWindow(self)

		if app.ENABLE_WORD_GAME_EVENT:
			self.wordgame = uiWordGameSystem.WordGameWindow()
			self.wordgame.Close()

		if app.ENABLE_SOCCER_BALL_EVENT:
			self.topver = uifutboltopu.TopEventi()
			self.topver.Close()

		if app.ENABLE_STONE_EVENT:
			self.stone_event = uistone_event.StoneEventWindow()
			self.stone_event.Hide()

		self.quickSlotPageIndex = 0
		self.lastPKModeSendedTime = 0
		self.pressNumber = None
		self.guildWarQuestionDialog = None
		self.interface = None
		self.targetBoard = None
		self.console = None
		self.mapNameShower = None
		self.affectShower = None
		self.playerGauge = None

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			constInfo.PREMIUMMODE = [FALSE, 0]

		self.stream=stream
		self.interface = interfaceModule.Interface()
		constInfo.SetInterfaceInstance(self.interface)
		if app.ENABLE_ADVANCED_GAME_OPTIONS:
			import advanced_options_runtime
			advanced_options_runtime.Configure(app=app, systemSetting=systemSetting, background=background, constInfo=constInfo, localeInfo=localeInfo, ui=ui, snd=snd, wndMgr=wndMgr)
			advanced_options_runtime.PrepareInterface()
		self.interface.MakeInterface()
		self.interface.ShowDefaultWindows()

		self.curtain = uiPhaseCurtain.PhaseCurtain()
		self.curtain.speed = 0.03
		self.curtain.Hide()

		self.targetBoard = uiTarget.TargetBoard()
		self.targetBoard.SetWhisperEvent(ui.__mem_func__(self.interface.OpenWhisperDialog))
		self.targetBoard.Hide()

		self.console = consoleModule.ConsoleWindow()
		self.console.BindGameClass(self)
		self.console.SetConsoleSize(wndMgr.GetScreenWidth(), 200)
		self.console.Hide()

		self.speedButton = uispeedbutton.SpeedButtonWindow()
		self.speedButton.Hide()

		self.mapNameShower = uiMapNameShower.MapNameShower()
		self.affectShower = uiAffectShower.AffectShower()


		if app.ENABLE_SPIRIT_STONE_READING:
			self.ruhtasi = uiruhtasi.RuhTasi()
			self.ruhtasi.Hide()

		if app.ENABLE_SKILL_BOOK_READING:
			self.bkoku = uibkoku.BKOku()
			self.bkoku.Hide()

		self.playerGauge = uiPlayerGauge.PlayerGauge(self)
		self.playerGauge.Hide()

		if app.ENABLE_MAINTENANCE_SYSTEM:
			self.wndMaintenance = uiMaintenance.MaintenanceClass()

		if app.ENABLE_DEFENSAWESHIP:
			self.wndShipMastHP = uiShipMastHP.ShipMastHP()
			self.wndShipMastHP.Close()

		self.itemDropQuestionDialog = None

		self.__SetQuickSlotMode()

		self.__ServerCommand_Build()
		self.__ProcessPreservedServerCommand()

		self.interface.SetAffectShower(self.affectShower)

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.interface.PetInfoBindAffectShower(self.affectShower)

		if app.ENABLE_REMOTE_SHOP_SYSTEM:
			self.remoteshop = None
			self.remoteshop = uiRemoteShop.RemoteShop()
			self.remoteshop.Hide()

	def __del__(self):
		player.SetGameWindow(0)
		net.ClearPhaseWindow(net.PHASE_WINDOW_GAME, self)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		app.SetFrameSkip(0)

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		self.quickSlotPageIndex = 0
		self.PickingCharacterIndex = -1
		self.PickingItemIndex = -1
		self.consoleEnable = FALSE
		self.isShowDebugInfo = FALSE
		self.ShowNameFlag = FALSE

		self.enableXMasBoom = FALSE
		self.startTimeXMasBoom = 0.0
		self.indexXMasBoom = 0

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			self.enableXMasSoul = FALSE
			self.startTimeXMasSoul = 0.0
			self.indexXMasSoul = 0

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight

		app.SetCamera(cameraDistance, cameraPitch, cameraRotation, cameraHeight)

		constInfo.SET_DEFAULT_CAMERA_MAX_DISTANCE()
		constInfo.SET_DEFAULT_CHRNAME_COLOR()
		constInfo.SET_DEFAULT_FOG_LEVEL()
		constInfo.SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE()
		constInfo.SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS()
		constInfo.SET_DEFAULT_USE_SKILL_EFFECT_ENABLE()

		# TWO_HANDED_WEAPON_ATTACK_SPEED_UP
		constInfo.SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE()
		# END_OF_TWO_HANDED_WEAPON_ATTACK_SPEED_UP

		import event
		event.SetLeftTimeString(localeInfo.UI_LEFT_TIME)

		textTail.EnablePKTitle(constInfo.PVPMODE_ENABLE)

		if constInfo.PVPMODE_TEST_ENABLE:
			self.testPKMode = ui.TextLine()
			self.testPKMode.SetFontName(localeInfo.UI_DEF_FONT)
			self.testPKMode.SetPosition(0, 15)
			self.testPKMode.SetWindowHorizontalAlignCenter()
			self.testPKMode.SetHorizontalAlignCenter()
			self.testPKMode.SetFeather()
			self.testPKMode.SetOutline()
			self.testPKMode.Show()

			self.testAlignment = ui.TextLine()
			self.testAlignment.SetFontName(localeInfo.UI_DEF_FONT)
			self.testAlignment.SetPosition(0, 35)
			self.testAlignment.SetWindowHorizontalAlignCenter()
			self.testAlignment.SetHorizontalAlignCenter()
			self.testAlignment.SetFeather()
			self.testAlignment.SetOutline()
			self.testAlignment.Show()

		self.__BuildKeyDict()
		self.__BuildDebugInfo()

		# PRIVATE_SHOP_PRICE_LIST
		uiPrivateShopBuilder.Clear()
		# END_OF_PRIVATE_SHOP_PRICE_LIST

		# UNKNOWN_UPDATE
		exchange.InitTrading()
		# END_OF_UNKNOWN_UPDATE

		if app.ENABLE_ULTIMATE_REGEN:
			player.LoadNewRegen()

		if debugInfo.IsDebugMode():
			self.ToggleDebugInfo()

		## Sound
		snd.SetMusicVolume(systemSetting.GetMusicVolume()*net.GetFieldMusicVolume())
		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		if app.ENABLE_FOG_FIX:
			background.SetFogLevel(1 if systemSetting.IsFogMode() else 0)

		netFieldMusicFileName = net.GetFieldMusicFileName()
		if netFieldMusicFileName:
			snd.FadeInMusic("BGM/" + netFieldMusicFileName)
		elif musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

		self.__SetQuickSlotMode()
		self.__SelectQuickPage(self.quickSlotPageIndex)

		self.SetFocus()
		self.Show()
		app.ShowCursor()
		if app.ENABLE_ADVANCED_GAME_OPTIONS:
			import advanced_options_runtime
			advanced_options_runtime.Configure(game=self,player=player,chr=chr,net=net,nonplayer=nonplayer)
			advanced_options_runtime.Replay()
			advanced_options_runtime.RemapKeys()

		net.SendEnterGamePacket()

		if app.ENABLE_DEFENSAWESHIP:
			if background.GetCurrentMapName() == "defensawe_hydra":
				self.__ShipMastHPShow()

		# START_GAME_ERROR_EXIT
		try:
			self.StartGame()
		except:
			import exception
			exception.Abort("GameWindow.Open")
		# END_OF_START_GAME_ERROR_EXIT
		
		self.RecvReadWiki()

		# ex) cubeInformation[20383] = [ {"rewordVNUM": 72723, "rewordCount": 1, "materialInfo": "101,1&102,2", "price": 999 }, ... ]
		self.cubeInformation = {}
		self.currentCubeNPC = 0
		self.isCameraMoving = True
		self.cameraMovementProgress = 0.0

	def Close(self):
		if app.ENABLE_ADVANCED_GAME_OPTIONS:
			import advanced_options_runtime
			advanced_options_runtime.Configure(game=None)
		self.Hide()

		if app.ENABLE_AUTO_CLEAR_WHEN_LOGOUT:
			if app.__AUTO_HUNT__:
				constInfo.autoHuntAutoLoginDict = {
					"status" : 0,
					"leftTime" : 0,
					"id" : "",
					"pwd" : "",
					"addr" : "",
					"port" : 0,
					"account_addr" : "",
					"account_port" : 0,
					"slot" : -1,
					"newOptions" : None,
					"slotStatus" : {},
					"skillDict" : None,
				}

		if app.ENABLE_MULTI_FARM_BLOCK:
			app.SetMultiFarmExeIcon(1)

		if app.ENABLE_REMOTE_SHOP_SYSTEM:
			if self.remoteshop:
				self.remoteshop.Hide()
				self.remoteshop.Destroy()
				self.remoteshop = None

		global cameraDistance, cameraPitch, cameraRotation, cameraHeight
		(cameraDistance, cameraPitch, cameraRotation, cameraHeight) = app.GetCamera()

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)

		self.onPressKeyDict = None
		self.onClickKeyDict = None

		chat.Close()
		snd.StopAllSound()
		grp.InitScreenEffect()
		chr.Destroy()
		textTail.Clear()
		quest.Clear()
		background.Destroy()
		guild.Destroy()
		messenger.Destroy()
		skill.ClearSkillData()
		wndMgr.Unlock()

		if app.ENABLE_RENEWAL_OFFLINESHOP:
			uiOfflineShopBuilder.Clear()
			shop.Clear()

		mouseModule.mouseController.DeattachObject()

		if self.guildWarQuestionDialog:
			self.guildWarQuestionDialog.Close()
		self._CloseLevelUpConfirmation()

		self.partyRequestQuestionDialog = None
		self.partyInviteQuestionDialog = None
		self.guildInviteQuestionDialog = None
		self.guildWarQuestionDialog = None
		self.messengerAddFriendQuestion = None

		# UNKNOWN_UPDATE
		self.itemDropQuestionDialog = None
		# END_OF_UNKNOWN_UPDATE

		# QUEST_CONFIRM
		self.confirmDialog = None
		# END_OF_QUEST_CONFIRM

		self.PrintCoord = None
		self.FrameRate = None
		self.Pitch = None
		self.Splat = None
		self.TextureNum = None
		self.ObjectNum = None
		self.ViewDistance = None
		self.PrintMousePos = None

		self.ClearDictionary()

		self.playerGauge = None
		self.mapNameShower = None
		self.affectShower = None

		if self.console:
			self.console.BindGameClass(0)
			self.console.Close()
			self.console = None

		if self.targetBoard:
			self.targetBoard.Destroy()
			self.targetBoard = None

		if app.ENABLE_WORD_GAME_EVENT:
			if self.wordgame:
				self.wordgame.Close()
				self.wordgame = None

		if app.ENABLE_SOCCER_BALL_EVENT:
			if self.topver:
				self.topver.Close()
				self.topver = None

		if app.ENABLE_STONE_EVENT:
			if self.stone_event:
				self.stone_event.Destroy()
				self.stone_event = None

		if app.ENABLE_SPIRIT_STONE_READING:
			if self.ruhtasi:
				self.ruhtasi.Destroy()
				self.ruhtasi = None

		if app.ENABLE_SKILL_BOOK_READING:
			if self.bkoku:
				self.bkoku.Destroy()
				self.bkoku = None

		if self.speedButton:
			self.speedButton.Destroy()
			self.speedButton = None

		if self.interface:
			self.interface.HideAllWindows()
			self.interface.Close()
			self.interface = None

		if app.ENABLE_MAINTENANCE_SYSTEM:
			if self.wndMaintenance.IsShow():
				self.wndMaintenance.Hide()

		if app.ENABLE_DEFENSAWESHIP:
			if self.wndShipMastHP:
				self.wndShipMastHP.Close()
				self.wndShipMastHP = 0

		player.ClearSkillDict()
		player.ResetCameraRotation()

		self.KillFocus()
		app.HideCursor()

		print "---------------------------------------------------------------------------- CLOSE GAME WINDOW"

	if app.ENABLE_STONE_EVENT:
		def StoneEvent(self, arg):
			if 1 == int(arg):
				self.stone_event.Show()
			else:
				self.stone_event.Hide()

	if app.ENABLE_SPIRIT_STONE_READING:
		def ruhcac(self):
			self.ruhtasi.Show()

	if app.ENABLE_SKILL_BOOK_READING:
		def bkac(self):
			self.bkoku.Show()

	if app.ENABLE_WORD_GAME_EVENT:
		def WordGameWindowShow(self):
			self.wordgame.Show()

	if app.ENABLE_SOCCER_BALL_EVENT:
		def FutbolTopuVer(self):
			self.topver.Show()

	def SpeedButtonWindowShow(self):
		self.speedButton.Show()

	if app.ENABLE_RANKING:
		def OpenRanking(self):
			if self.interface:
				self.interface.OpenRanking()

		def RankingClearData(self):
			if self.interface:
				self.interface.RankingClearData()

		def RankingAddRank(self, position, level, points, name, realPosition):
			if self.interface:
				self.interface.RankingAddRank(position, level, points, name, realPosition)

		def RankingRefresh(self):
			if self.interface:
				self.interface.RankingRefresh()

	def __BuildKeyDict(self):
		onPressKeyDict = {}

		onPressKeyDict[app.DIK_1]	= lambda : self.__PressNumKey(1)
		onPressKeyDict[app.DIK_2]	= lambda : self.__PressNumKey(2)
		onPressKeyDict[app.DIK_3]	= lambda : self.__PressNumKey(3)
		onPressKeyDict[app.DIK_4]	= lambda : self.__PressNumKey(4)
		onPressKeyDict[app.DIK_5]	= lambda : self.__PressNumKey(5)
		onPressKeyDict[app.DIK_6]	= lambda : self.__PressNumKey(6)
		onPressKeyDict[app.DIK_7]	= lambda : self.__PressNumKey(7)
		onPressKeyDict[app.DIK_8]	= lambda : self.__PressNumKey(8)
		onPressKeyDict[app.DIK_9]	= lambda : self.__PressNumKey(9)
		onPressKeyDict[app.DIK_F1]	= lambda : self.__PressQuickSlot(4)
		onPressKeyDict[app.DIK_F2]	= lambda : self.__PressQuickSlot(5)
		onPressKeyDict[app.DIK_F3]	= lambda : self.__PressQuickSlot(6)
		onPressKeyDict[app.DIK_F4]	= lambda : self.__PressQuickSlot(7)

		onPressKeyDict[app.DIK_F5]	= lambda : self.SpeedButtonWindowOpen()

		if app.ENABLE_RENEWAL_SWITCHBOT:
			onPressKeyDict[app.DIK_F6]	= lambda : self.interface.ToggleSwitchbotWindow()

		if app.ENABLE_COLLECTIONS_SYSTEM:
			onPressKeyDict[app.DIK_F7]	= lambda : self.interface.ToggleCollectionsWindow()

		if app.ENABLE_COLLECT_WINDOW:
			onPressKeyDict[app.DIK_F8]	= lambda : self.interface.ToggleCollectWindow()

		if app.ENABLE_RESP_SYSTEM:
			onPressKeyDict[app.DIK_F9]	= lambda : self.interface.OpenRespWindow()

		if app.ENABLE_TRACK_WINDOW:
			#onPressKeyDict[app.DIK_F5]	= lambda : self.interface.OpenTrackWindow()
			onPressKeyDict[app.DIK_F10]	= lambda : self.interface.OpenDungeonInfo()

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			onPressKeyDict[app.DIK_F12]	= lambda : self.interface.OpenPrivateShopSearch(0)

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			onPressKeyDict[app.DIK_X]	= lambda : self.interface.OpenWarpWindow()

		if app.ENABLE_BIOLOGIST_SYSTEM:
			onPressKeyDict[app.DIK_Y] = lambda : self.BiyologOpen()

		#if app.ENABLE_REMOTE_SHOP_SYSTEM:
		#	onPressKeyDict[app.DIK_F10]	= lambda : self.OpenRemoteShop()

		#if app.ENABLE_COLLECTIONS_SYSTEM:
		#	# Collections Window için 'X' tuþu (X harfi Collections için uygun)
		#	onPressKeyDict[app.DIK_X]	= lambda : self.interface.ToggleCollectionsWindow()

		if app.ENABLE_SPECIAL_INVENTORY:
			onPressKeyDict[app.DIK_K]	= lambda : self.interface.ToggleSpecialInventoryWindow()

		if app.__AUTO_HUNT__:
			onPressKeyDict[app.DIK_O]	= lambda : self.interface.OpenAutoHunt()

		if app.ENABLE_AUTO_SELL_SYSTEM:
			onPressKeyDict[app.DIK_U] = lambda : self.interface.ToggleAutoSellWindow()

		onPressKeyDict[app.DIK_LALT]		= lambda : self.ShowName()
		onPressKeyDict[app.DIK_LCONTROL]	= lambda : self.ShowMouseImage()
		onPressKeyDict[app.DIK_SYSRQ]		= lambda : self.SaveScreen()
		onPressKeyDict[app.DIK_SPACE]		= lambda : self.StartAttack()

		onPressKeyDict[app.DIK_UP]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_DOWN]		= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_LEFT]		= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_RIGHT]		= lambda : self.MoveRight()
		onPressKeyDict[app.DIK_W]			= lambda : self.MoveUp()
		onPressKeyDict[app.DIK_S]			= lambda : self.MoveDown()
		onPressKeyDict[app.DIK_A]			= lambda : self.MoveLeft()
		onPressKeyDict[app.DIK_D]			= lambda : self.MoveRight()

		onPressKeyDict[app.DIK_E]			= lambda: app.RotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_R]			= lambda: app.ZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_T]			= lambda: app.PitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_G]			= self.__PressGKey
		onPressKeyDict[app.DIK_Q]			= self.__PressQKey

		onPressKeyDict[app.DIK_NUMPAD9]		= lambda: app.MovieResetCamera()
		onPressKeyDict[app.DIK_NUMPAD4]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD6]		= lambda: app.MovieRotateCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_PGUP]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_PGDN]		= lambda: app.MovieZoomCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_NUMPAD8]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_NEGATIVE)
		onPressKeyDict[app.DIK_NUMPAD2]		= lambda: app.MoviePitchCamera(app.CAMERA_TO_POSITIVE)
		onPressKeyDict[app.DIK_GRAVE]		= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_Z]			= lambda : self.PickUpItem()
		onPressKeyDict[app.DIK_C]			= lambda state = "STATUS": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_V]			= lambda state = "SKILL": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_N]			= lambda state = "QUEST": self.interface.ToggleCharacterWindow(state)
		onPressKeyDict[app.DIK_I]			= lambda : self.interface.ToggleInventoryWindow()
		if app.ENABLE_GROWTH_PET_SYSTEM:
			onPressKeyDict[app.DIK_P]		= lambda : self.interface.TogglePetInformationWindow()
		onPressKeyDict[app.DIK_M]			= lambda : self.interface.PressMKey()
		onPressKeyDict[app.DIK_ADD]			= lambda : self.interface.MiniMapScaleUp()
		onPressKeyDict[app.DIK_SUBTRACT]	= lambda : self.interface.MiniMapScaleDown()
		onPressKeyDict[app.DIK_L]			= lambda : self.interface.ToggleChatLogWindow()
		onPressKeyDict[app.DIK_COMMA]		= lambda : self.ShowConsole()
		onPressKeyDict[app.DIK_LSHIFT]		= lambda : self.__SetQuickPageMode()

		onPressKeyDict[app.DIK_J]			= lambda : self.__PressJKey()
		onPressKeyDict[app.DIK_H]			= lambda : self.__PressHKey()
		onPressKeyDict[app.DIK_B]			= lambda : self.__PressBKey()
		onPressKeyDict[app.DIK_F]			= lambda : self.__PressFKey()

		self.onPressKeyDict = onPressKeyDict

		onClickKeyDict = {}
		onClickKeyDict[app.DIK_UP] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_DOWN] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_LEFT] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_RIGHT] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_SPACE] = lambda : self.EndAttack()

		onClickKeyDict[app.DIK_W] = lambda : self.StopUp()
		onClickKeyDict[app.DIK_S] = lambda : self.StopDown()
		onClickKeyDict[app.DIK_A] = lambda : self.StopLeft()
		onClickKeyDict[app.DIK_D] = lambda : self.StopRight()
		onClickKeyDict[app.DIK_Q] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_E] = lambda: app.RotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_R] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_F] = lambda: app.ZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_T] = lambda: app.PitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_G] = lambda: self.__ReleaseGKey()
		onClickKeyDict[app.DIK_NUMPAD4] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD6] = lambda: app.MovieRotateCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGUP] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_PGDN] = lambda: app.MovieZoomCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD8] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_NUMPAD2] = lambda: app.MoviePitchCamera(app.CAMERA_STOP)
		onClickKeyDict[app.DIK_LALT] = lambda: self.HideName()
		onClickKeyDict[app.DIK_LCONTROL] = lambda: self.HideMouseImage()
		onClickKeyDict[app.DIK_LSHIFT] = lambda: self.__SetQuickSlotMode()

		#if app.ENABLE_GUILDRENEWAL_SYSTEM:
		#	onClickKeyDict[app.DIK_TAB] = lambda: self.__PressTabKey()

		if app.ENABLE_RANKING:
			onPressKeyDict[app.DIK_J] = lambda : self.interface.OpenRanking()

		self.onClickKeyDict=onClickKeyDict

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def __PressTabKey(self):
			self.interface.OpenGuildScoreWindow()

	def __PressNumKey(self,num):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):

			if num >= 1 and num <= 9:
				if(chrmgr.IsPossibleEmoticon(-1)):
					chrmgr.SetEmoticon(-1,int(num)-1)
					net.SendEmoticon(int(num)-1)
		else:
			if num >= 1 and num <= 4:
				self.pressNumber(num-1)

	def __ClickBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			return
		else:
			if constInfo.PVPMODE_ACCELKEY_ENABLE:
				self.ChangePKMode()

	def	__PressJKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if player.IsMountingHorse():
				net.SendChatPacket("/unmount")
			else:
				if not uiPrivateShopBuilder.IsBuildingPrivateShop():
					for i in xrange(player.INVENTORY_PAGE_SIZE):
						if player.GetItemIndex(i) in (71114, 71116, 71118, 71120):
							net.SendItemUsePacket(i)
							break
	def	__PressHKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_ride")
		else:
			if app.ENABLE_WIKI_SYSTEM:
				self.interface.OpenWikiWindow()

	def	__PressBKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_back")
		else:
			state = "EMOTICON"
			self.interface.ToggleCharacterWindow(state)

	def	__PressFKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/user_horse_feed")
		else:
			app.ZoomCamera(app.CAMERA_TO_POSITIVE)

	def __PressGKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			net.SendChatPacket("/ride")
		else:
			if self.ShowNameFlag:
				self.interface.ToggleGuildWindow()
			else:
				app.PitchCamera(app.CAMERA_TO_POSITIVE)

	def	__ReleaseGKey(self):
		app.PitchCamera(app.CAMERA_STOP)

	def __PressQKey(self):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL):
			if 0==interfaceModule.IsQBHide:
				interfaceModule.IsQBHide = 1
				self.interface.HideAllQuestButton()
			else:
				interfaceModule.IsQBHide = 0
				self.interface.ShowAllQuestButton()
		else:
			app.RotateCamera(app.CAMERA_TO_NEGATIVE)

	def __SetQuickSlotMode(self):
		self.pressNumber=ui.__mem_func__(self.__PressQuickSlot)

	def __SetQuickPageMode(self):
		self.pressNumber=ui.__mem_func__(self.__SelectQuickPage)

	def __PressQuickSlot(self, localSlotIndex):
		if app.ENABLE_GROWTH_PET_SYSTEM:
			result = player.CanUseGrowthPetQuickSlot(localSlotIndex)

			if player.QUICK_SLOT_POS_ERROR == result:
				return
			elif result in [player.QUICK_SLOT_ITEM_USE_SUCCESS, player.QUICK_SLOT_IS_NOT_ITEM, player.QUICK_SLOT_PET_ITEM_USE_SUCCESS]:
				player.RequestUseLocalQuickSlot(localSlotIndex)
			elif player.QUICK_SLOT_PET_ITEM_USE_FAILED == result:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PET_CAN_NOT_SUMMON_BECAUSE_LIFE_TIME_END)
			elif player.QUICK_SLOT_CAN_NOT_USE_PET_ITEM == result:
				return
		else:
			player.RequestUseLocalQuickSlot(localSlotIndex)

	if constInfo.ENABLE_EXPANDED_TASKBAR:
		def __PressExpandedTaskbar(self):
			self.interface.PressExpandedTaskbar()

	def __SelectQuickPage(self, pageIndex):
		self.quickSlotPageIndex = pageIndex
		player.SetQuickPage(pageIndex)

	def ToggleDebugInfo(self):
		self.isShowDebugInfo = not self.isShowDebugInfo

		if self.isShowDebugInfo:
			self.PrintCoord.Show()
			self.FrameRate.Show()
			self.Pitch.Show()
			self.Splat.Show()
			self.TextureNum.Show()
			self.ObjectNum.Show()
			self.ViewDistance.Show()
			self.PrintMousePos.Show()
		else:
			self.PrintCoord.Hide()
			self.FrameRate.Hide()
			self.Pitch.Hide()
			self.Splat.Hide()
			self.TextureNum.Hide()
			self.ObjectNum.Hide()
			self.ViewDistance.Hide()
			self.PrintMousePos.Hide()

	def __BuildDebugInfo(self):
		## Character Position Coordinate
		self.PrintCoord = ui.TextLine()
		self.PrintCoord.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintCoord.SetPosition(wndMgr.GetScreenWidth() - 270, 0)

		## Frame Rate
		self.FrameRate = ui.TextLine()
		self.FrameRate.SetFontName(localeInfo.UI_DEF_FONT)
		self.FrameRate.SetPosition(wndMgr.GetScreenWidth() - 270, 20)

		## Camera Pitch
		self.Pitch = ui.TextLine()
		self.Pitch.SetFontName(localeInfo.UI_DEF_FONT)
		self.Pitch.SetPosition(wndMgr.GetScreenWidth() - 270, 40)

		## Splat
		self.Splat = ui.TextLine()
		self.Splat.SetFontName(localeInfo.UI_DEF_FONT)
		self.Splat.SetPosition(wndMgr.GetScreenWidth() - 270, 60)

		##
		self.PrintMousePos = ui.TextLine()
		self.PrintMousePos.SetFontName(localeInfo.UI_DEF_FONT)
		self.PrintMousePos.SetPosition(wndMgr.GetScreenWidth() - 270, 80)

		# TextureNum
		self.TextureNum = ui.TextLine()
		self.TextureNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.TextureNum.SetPosition(wndMgr.GetScreenWidth() - 270, 100)

		self.ObjectNum = ui.TextLine()
		self.ObjectNum.SetFontName(localeInfo.UI_DEF_FONT)
		self.ObjectNum.SetPosition(wndMgr.GetScreenWidth() - 270, 120)

		self.ViewDistance = ui.TextLine()
		self.ViewDistance.SetFontName(localeInfo.UI_DEF_FONT)
		self.ViewDistance.SetPosition(0, 0)

	def __NotifyError(self, msg):
		chat.AppendChat(chat.CHAT_TYPE_INFO, msg)

	def ChangePKMode(self):

		if not app.IsPressed(app.DIK_LCONTROL):
			return

		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return

		curTime = app.GetTime()
		if curTime - self.lastPKModeSendedTime < constInfo.PVPMODE_ACCELKEY_DELAY:
			return

		self.lastPKModeSendedTime = curTime

		curPKMode = player.GetPKMode()
		nextPKMode = curPKMode + 1
		if nextPKMode == player.PK_MODE_PROTECT:
			if 0 == player.GetGuildID():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
				nextPKMode = 0
			else:
				nextPKMode = player.PK_MODE_GUILD

		elif nextPKMode == player.PK_MODE_MAX_NUM:
			nextPKMode = 0

		net.SendChatPacket("/PKMode " + str(nextPKMode))
		print "/PKMode " + str(nextPKMode)

	def OnChangePKMode(self):

		self.interface.OnChangePKMode()

		try:
			self.__NotifyError(localeInfo.OPTION_PVPMODE_MESSAGE_DICT[player.GetPKMode()])
		except KeyError:
			print "UNKNOWN PVPMode[%d]" % (player.GetPKMode())

		if constInfo.PVPMODE_TEST_ENABLE:
			curPKMode = player.GetPKMode()
			alignment, grade = chr.testGetPKData()
			self.pkModeNameDict = { 0 : "PEACE", 1 : "REVENGE", 2 : "FREE", 3 : "PROTECT", }
			self.testPKMode.SetText("Current PK Mode : " + self.pkModeNameDict.get(curPKMode, "UNKNOWN"))
			self.testAlignment.SetText("Current Alignment : " + str(alignment) + " (" + localeInfo.TITLE_NAME_LIST[grade] + ")")

	###############################################################################################
	###############################################################################################
	## Game Callback Functions

	# Start
	def StartGame(self):
		self.RefreshInventory()
		self.RefreshEquipment()
		self.RefreshCharacter()
		self.RefreshSkill()

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			systemSetting.SetSnowModeOption(systemSetting.GetSnowModeOption())

		#if app.ENABLE_ZODIAC_MISSION:
		#	if background.GetCurrentMapName() == "metin2_12zi_stage":
		#		if self.interface.wndMiniMap:
		#			self.interface.wndMiniMap.Hide()

	if app.ENABLE_TRACK_WINDOW:
		def TrackWindowUpdate(self):
			if systemSetting.GetDungeonTrack() or systemSetting.GetBossTrack():
				self.interface.MakeTrackWindow()
				self.interface.TrackWindowCheckPacket()

	# Refresh
	def CheckGameButton(self):
		if self.interface:
			self.interface.CheckGameButton()

	def RefreshAlignment(self):
		self.interface.RefreshAlignment()

	def RefreshStatus(self):
		self.CheckGameButton()

		if self.interface:
			self.interface.RefreshStatus()

		if self.playerGauge:
			self.playerGauge.RefreshGauge()

	def RefreshStamina(self):
		self.interface.RefreshStamina()

	def RefreshSkill(self):
		self.CheckGameButton()
		if self.interface:
			self.interface.RefreshSkill()

	def RefreshSkillCoolTime(self):
		if self.interface:
			self.interface.RefreshSkillCoolTime()

	def RefreshQuest(self):
		self.interface.RefreshQuest()

	def RefreshMessenger(self):
		self.interface.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.interface.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.interface.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.interface.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.interface.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.interface.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.interface.RefreshGuildGradePage()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def RefreshGuildBaseInfoPage(self):
			self.interface.RefreshGuildBaseInfoPage()

		def RefreshGuildBaseInfoPageBankGold(self):
			self.interface.RefreshGuildBaseInfoPageBankGold()

		def RefreshGuildWarInfoPage(self):
			self.interface.RefreshGuildWarInfoPage()

	def OnBlockMode(self, mode):
		self.interface.OnBlockMode(mode)

	def OpenQuestWindow(self, skin, idx):
		if constInfo.INPUT_IGNORE == 1:
			return
		self.interface.OpenQuestWindow(skin, idx)


	## Refine
	def PopupMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, 0, localeInfo.UI_OK)

	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type=0):
		self.interface.OpenRefineDialog(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.interface.AppendMaterialToRefineDialog(vnum, count)

	def RunUseSkillEvent(self, slotIndex, coolTime):
		self.interface.OnUseSkill(slotIndex, coolTime)

	def ClearAffects(self):
		self.affectShower.ClearAffects()

	def SetAffect(self, affect):
		self.affectShower.SetAffect(affect)

	def ResetAffect(self, affect):
		self.affectShower.ResetAffect(affect)

	# UNKNOWN_UPDATE
	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		if self.affectShower:
			self.affectShower.BINARY_NEW_AddAffect(type, pointIdx, value, duration)

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] == -2:
				constInfo.autoHuntAutoLoginDict["leftTime"] = app.GetGlobalTimeStamp() + 2

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			if chr.NEW_AFFECT_AUTO_PICK_UP == type:
				constInfo.PREMIUMMODE = [TRUE, app.GetGlobalTimeStamp() + int(duration)]
				self.interface.OnChangePickUPMode()

		if uiAffectShower.AFF_LEADERSHIP == type:
			self.affectShower.SetLeaderShipStatus(True, pointIdx, value)

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		self.affectShower.BINARY_NEW_RemoveAffect(type, pointIdx)

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			if chr.NEW_AFFECT_AUTO_PICK_UP == type:
				constInfo.PREMIUMMODE = [FALSE, 0]
				self.interface.OnChangePickUPMode()

		elif uiAffectShower.AFF_LEADERSHIP == type:
			self.affectShower.SetLeaderShipStatus(False)

		if app.__AUTO_HUNT__:
			if type == chr.NEW_AFFECT_AUTO_HUNT:
				net.SendChatPacket("/auto_hunt end")

	def BINARY_OpenInGameEvent(self):
		self.interface.OpenInGameEvent()

	def BINARY_CloseInGameEvent(self):
		self.interface.CloseInGameEvent()

	def ActivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnActivateSkill(slotIndex)

	def DeactivateSkillSlot(self, slotIndex):
		if self.interface:
			self.interface.OnDeactivateSkill(slotIndex)

	def RefreshEquipment(self):
		if self.interface:
			self.interface.RefreshInventory()

	def RefreshInventory(self):
		if self.interface:
			self.interface.RefreshInventory()

	def RefreshCharacter(self):
		if self.interface:
			self.interface.RefreshCharacter()

	if app.ENABLE_RENEWAL_DEAD_PACKET:
		def OnGameOver(self, d_time):
			self.CloseTargetBoard()
			self.OpenRestartDialog(d_time)

		def OpenRestartDialog(self, d_time):
			self.interface.OpenRestartDialog(d_time)
	else:
		def OnGameOver(self):
			self.CloseTargetBoard()
			self.OpenRestartDialog()

		def OpenRestartDialog(self):
			self.interface.OpenRestartDialog()

	def ChangeCurrentSkill(self, skillSlotNumber):
		self.interface.OnChangeCurrentSkill(skillSlotNumber)

	if app.ENABLE_STONE_EVENT:
		def BINARY_STONE_EVENT(self, arg1):
			self.stone_event.SetPoint(arg1)

	## TargetBoard
	def SetPCTargetBoard(self, vid, name):
		self.targetBoard.Open(vid, name)

		if app.IsPressed(app.DIK_LCONTROL):

			if not player.IsSameEmpire(vid):
				return

			if player.IsMainCharacterIndex(vid):
				return
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(vid):
				return

			self.interface.OpenWhisperDialog(name)


	def RefreshTargetBoardByVID(self, vid):
		self.targetBoard.RefreshByVID(vid)

	def RefreshTargetBoardByName(self, name):
		self.targetBoard.RefreshByName(name)

	def __RefreshTargetBoard(self):
		self.targetBoard.Refresh()

	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		def SetHPTargetBoard(self, vid, hpPercentage, iMinHP, iMaxHP):
			if vid != self.targetBoard.GetTargetVID():
				self.targetBoard.ResetTargetBoard()
				self.targetBoard.SetEnemyVID(vid)
			
			self.targetBoard.SetHP(hpPercentage, iMinHP, iMaxHP)
			self.targetBoard.Show()
	else:
		def SetHPTargetBoard(self, vid, hpPercentage):
			if vid != self.targetBoard.GetTargetVID():
				self.targetBoard.ResetTargetBoard()
				self.targetBoard.SetEnemyVID(vid)

			self.targetBoard.SetHP(hpPercentage)
			self.targetBoard.Show()

	def CloseTargetBoardIfDifferent(self, vid):
		if vid != self.targetBoard.GetTargetVID():
			self.targetBoard.Close()

	def CloseTargetBoard(self):
		self.targetBoard.Close()

	## View Equipment
	def OpenEquipmentDialog(self, vid):
		self.interface.OpenEquipmentDialog(vid)

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count, dwChangeLookVnum):
			self.interface.SetEquipmentDialogItem(vid, slotIndex, vnum, count, dwChangeLookVnum)
	else:
		def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
			self.interface.SetEquipmentDialogItem(vid, slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		self.interface.SetEquipmentDialogSocket(vid, slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		self.interface.SetEquipmentDialogAttr(vid, slotIndex, attrIndex, type, value)

	if app.ENABLE_SUNGMA_SCORE:
		def SetEquipmentDialogSungmaScore(self, vid, slotIndex, value):
			self.interface.SetEquipmentDialogSungmaScore(vid, slotIndex, value)

	# SHOW_LOCAL_MAP_NAME
	def ShowMapName(self, mapName, x, y):

		if self.mapNameShower:
			self.mapNameShower.ShowMapName(mapName, x, y)

		if self.interface:
			self.interface.SetMapName(mapName)
	# END_OF_SHOW_LOCAL_MAP_NAME
	
		if app.ENABLE_SUNG_MAHI_TOWER:
			if mapName == "metin2_map_smhdungeon_02":
				if self.interface:
					self.interface.SetSungMahiRestartDialog()

	def BINARY_OpenAtlasWindow(self):
		self.interface.BINARY_OpenAtlasWindow()

	def BINARY_AdvancedGuildChat(self, line):
		chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, "[Guild]", line)
		self.interface.RecvWhisper("[Guild]")

	## Chat
	if app.ENABLE_MULTILANGUAGE_CHATTING:
		def OnRecvWhisper(self, mode, name, line, codepage = 0):
			if mode == chat.WHISPER_TYPE_GM:
				self.interface.RegisterGameMasterName(name)
			chat.AppendWhisper(mode, name, line, codepage)
			self.interface.RecvWhisper(name)
	else:
		def OnRecvWhisper(self, mode, name, line):
			if mode == chat.WHISPER_TYPE_GM:
				self.interface.RegisterGameMasterName(name)
			chat.AppendWhisper(mode, name, line)
			self.interface.RecvWhisper(name)

	def OnRecvWhisperSystemMessage(self, mode, name, line):
		chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, line)
		self.interface.RecvWhisper(name)

	def OnRecvWhisperError(self, mode, name, line):
		if localeInfo.WHISPER_ERROR.has_key(mode):
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, localeInfo.WHISPER_ERROR[mode](name))
		else:
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, name, "Whisper Unknown Error(mode=%d, name=%s)" % (mode, name))
		self.interface.RecvWhisper(name)

	def RecvWhisper(self, name):
		self.interface.RecvWhisper(name)

	def OnPickMoney(self, money):
		if app.ENABLE_RENEWAL_SPECIAL_CHAT:
			self.interface.SpecialChatOnPick(money, 0)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_PICK_MONEY % (money))

	if app.ENABLE_SPECIAL_INVENTORY:
		def OpenSpecialInventoryWindow(self, category):
			self.interface.OpenSpecialInventoryWindow(int(category))

	def OnShopError(self, type):
		try:
			self.PopupMessage(localeInfo.SHOP_ERROR_DICT[type])
		except KeyError:
			self.PopupMessage(localeInfo.SHOP_ERROR_UNKNOWN % (type))

	def OnSafeBoxError(self):
		self.PopupMessage(localeInfo.SAFEBOX_ERROR)

	def OnFishingSuccess(self, isFish, fishName):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_SUCCESS(isFish, fishName), 2000)
		if app.ENABLE_ADVANCED_GAME_OPTIONS:
			import advanced_option_notifications
			advanced_option_notifications.Record("fishing", localeInfo.FISHING_SUCCESS(isFish, fishName), popup=True)

	# ADD_FISHING_MESSAGE
	def OnFishingNotifyUnknown(self):
		# chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_UNKNOWN)
		self.noticeBox = ui.NoticeBoxBoard(localeInfo.FISHING_UNKNOWN)

	def OnFishingWrongPlace(self):
		# chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_WRONG_PLACE)
		self.noticeBox = ui.NoticeBoxBoard(localeInfo.FISHING_WRONG_PLACE)
	# END_OF_ADD_FISHING_MESSAGE

	def OnFishingNotify(self, isFish, fishName):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.FISHING_NOTIFY(isFish, fishName))

	def OnFishingFailure(self):
		chat.AppendChatWithDelay(chat.CHAT_TYPE_INFO, localeInfo.FISHING_FAILURE, 2000)

	def OnCannotPickItem(self):
		# chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_PICK_ITEM)
		self.noticeBox = ui.NoticeBoxBoard(localeInfo.GAME_CANNOT_PICK_ITEM)

	# MINING
	if app.ENABLE_CANNOT_MINING_IF_POLY:
		def OnCannotMining(self, fail_type):
			if fail_type == player.MINING_FAIL_ON_MOUNT:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MINING_FAIL_ON_MOUNT)
			elif fail_type == player.MINING_FAIL_WITHOUT_PICK:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MINING_FAIL_WITHOUT_PICK)
			elif fail_type == player.MINING_FAIL_WHEN_POLY:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MINING_FAIL_WHEN_POLY)
	else:
		def OnCannotMining(self):
			# chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GAME_CANNOT_MINING)
			self.noticeBox = ui.NoticeBoxBoard(localeInfo.GAME_CANNOT_MINING)
	# END_OF_MINING

	def OnCannotUseSkill(self, vid, type):
		if localeInfo.USE_SKILL_ERROR_TAIL_DICT.has_key(type):
			# textTail.RegisterInfoTail(vid, localeInfo.USE_SKILL_ERROR_TAIL_DICT[type])
			self.noticeBox = ui.NoticeBoxBoard(localeInfo.USE_SKILL_ERROR_TAIL_DICT[type])
		if localeInfo.USE_SKILL_ERROR_CHAT_DICT.has_key(type):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_SKILL_ERROR_CHAT_DICT[type])
			self.noticeBox = ui.NoticeBoxBoard(localeInfo.USE_SKILL_ERROR_CHAT_DICT[type])

	def	OnCannotShotError(self, vid, type):
		# textTail.RegisterInfoTail(vid, localeInfo.SHOT_ERROR_TAIL_DICT.get(type, localeInfo.SHOT_ERROR_UNKNOWN % (type)))
		msg = localeInfo.SHOT_ERROR_TAIL_DICT.get(type, localeInfo.SHOT_ERROR_UNKNOWN % (type))
		self.noticeBox = ui.NoticeBoxBoard(msg)

	def OnCannotFishingRideMount(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_FISHING_RIDE_MOUNT)

	if app.ENABLE_MSG_FISHING_WHILE_POLY:
		def OnCannotFishingWhilePoly(self):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_FISHING_WHILE_POLY)

	## PointReset
	def StartPointReset(self):
		self.interface.OpenPointResetDialog()

	## Shop
	def StartShop(self, vid):
		self.interface.OpenShopDialog(vid)

	def EndShop(self):
		self.interface.CloseShopDialog()

	def RefreshShop(self):
		self.interface.RefreshShopDialog()

	def SetShopSellingPrice(self, Price):
		pass

	## Exchange
	def StartExchange(self):
		self.interface.StartExchange()

	def EndExchange(self):
		self.interface.EndExchange()

	def RefreshExchange(self):
		self.interface.RefreshExchange()

	## Party
	def RecvPartyInviteQuestion(self, leaderVID, leaderName):
		partyInviteQuestionDialog = uiCommon.QuestionDialogWithTimeLimit2()
		partyInviteQuestionDialog.SetText1(leaderName + localeInfo.PARTY_DO_YOU_JOIN)
		partyInviteQuestionDialog.SetTimeOverMsg(localeInfo.PARTY_ANSWER_TIMEOVER)
		partyInviteQuestionDialog.SetTimeOverEvent(self.AnswerPartyInvite, FALSE)
		partyInviteQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerPartyInvite(arg))
		partyInviteQuestionDialog.Open(10)
		partyInviteQuestionDialog.partyLeaderVID = leaderVID
		self.partyInviteQuestionDialog = partyInviteQuestionDialog

	def AnswerPartyInvite(self, answer):

		if not self.partyInviteQuestionDialog:
			return

		partyLeaderVID = self.partyInviteQuestionDialog.partyLeaderVID

		# Removed the distance limit for joining a party.
		#distance = player.GetCharacterDistance(partyLeaderVID)
		#if distance < 0.0 or distance > 5000:
		#	answer = False

		net.SendPartyInviteAnswerPacket(partyLeaderVID, answer)

		self.partyInviteQuestionDialog.Close()
		self.partyInviteQuestionDialog = None

	def AddPartyMember(self, pid, name):
		self.interface.AddPartyMember(pid, name)

	def UpdatePartyMemberInfo(self, pid):
		self.interface.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.interface.RemovePartyMember(pid)
		self.__RefreshTargetBoard()

	def LinkPartyMember(self, pid, vid):
		self.interface.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.interface.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.interface.UnlinkAllPartyMember()

	def ExitParty(self):
		self.interface.ExitParty()
		self.RefreshTargetBoardByVID(self.targetBoard.GetTargetVID())

	def ChangePartyParameter(self, distributionMode):
		self.interface.ChangePartyParameter(distributionMode)

	## Messenger
	def OnMessengerAddFriendQuestion(self, name):
		messengerAddFriendQuestion = uiCommon.QuestionDialogWithTimeLimit2()
		messengerAddFriendQuestion.SetText1(localeInfo.MESSENGER_DO_YOU_ACCEPT_ADD_FRIEND % (name))
		messengerAddFriendQuestion.SetTimeOverMsg(localeInfo.MESSENGER_ADD_FRIEND_ANSWER_TIMEOVER)
		messengerAddFriendQuestion.SetTimeOverEvent(self.OnDenyAddFriend)
		messengerAddFriendQuestion.SetAcceptEvent(ui.__mem_func__(self.OnAcceptAddFriend))
		messengerAddFriendQuestion.SetCancelEvent(ui.__mem_func__(self.OnDenyAddFriend))
		messengerAddFriendQuestion.Open(10)
		messengerAddFriendQuestion.name = name
		self.messengerAddFriendQuestion = messengerAddFriendQuestion

	def OnAcceptAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth y " + name)
		self.OnCloseAddFriendQuestionDialog()
		return TRUE

	def OnDenyAddFriend(self):
		name = self.messengerAddFriendQuestion.name
		net.SendChatPacket("/messenger_auth n " + name)
		self.OnCloseAddFriendQuestionDialog()
		return TRUE

	def OnCloseAddFriendQuestionDialog(self):
		self.messengerAddFriendQuestion.Close()
		self.messengerAddFriendQuestion = None
		return TRUE

	## SafeBox
	def OpenSafeboxWindow(self, size):
		self.interface.OpenSafeboxWindow(size)

	def RefreshSafebox(self):
		self.interface.RefreshSafebox()

	if app.ENABLE_UN_SEAL_SCROLL_PLUS:
		def UnSealScrollPlusPasswordSuccess(self, type):
			self.interface.UnSealScrollPlusPasswordSuccess(type)

	def RefreshSafeboxMoney(self):
		self.interface.RefreshSafeboxMoney()

	# ITEM_MALL
	def OpenMallWindow(self, size):
		self.interface.OpenMallWindow(size)

	def RefreshMall(self):
		self.interface.RefreshMall()
	# END_OF_ITEM_MALL

	## Guild
	if app.ENABLE_GUILDBANK_LOG:
		def OpenGuildBankInfo(self):
			self.interface.OpenGuildBankInfo()

		def GuildBankLogInfoRefresh(self):
			self.interface.GuildBankLogInfoRefresh()

	if app.ENABLE_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_DONATE_ATTENDANCE:
		def GuildDonateInfoRefresh(self):
			self.interface.GuildDonateInfoRefresh()

	def RecvGuildInviteQuestion(self, guildID, guildName):
		import advanced_option_notifications
		advanced_option_notifications.Record("guild",guildName,popup=True)
		guildInviteQuestionDialog = uiCommon.QuestionDialog()
		guildInviteQuestionDialog.SetText(guildName + localeInfo.GUILD_DO_YOU_JOIN)
		guildInviteQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.AnswerGuildInvite(arg))
		guildInviteQuestionDialog.Open()
		guildInviteQuestionDialog.guildID = guildID
		self.guildInviteQuestionDialog = guildInviteQuestionDialog

	def AnswerGuildInvite(self, answer):

		if not self.guildInviteQuestionDialog:
			return

		guildLeaderVID = self.guildInviteQuestionDialog.guildID
		net.SendGuildInviteAnswerPacket(guildLeaderVID, answer)

		self.guildInviteQuestionDialog.Close()
		self.guildInviteQuestionDialog = None


	def DeleteGuild(self):
		self.interface.DeleteGuild()

	# if app.ENABLE_GUILDRENEWAL_SYSTEM:
		# def SetGuildWarType(self, index):
			# self.interface.SetGuildWarType(index)

		# def OpenGuildBankWindow(self, size):
			# self.interface.OpenGuildBankWindow(size)

		# def OpenGuildGoldInOutWindow(self, inout):
			# self.interface.OpenGuildGoldInOutWindow(inout)

		# def RefreshGuildBank(self):
			# self.interface.RefreshGuildBank()

		# def RefreshGuildBankInfo(self):
			# self.interface.RefreshGuildBankInfo()

		# def OpenGuildBankInfo(self):
			# self.interface.OpenGuildBankInfo()

	## Clock
	def ShowClock(self, second):
		self.interface.ShowClock(second)

	def HideClock(self):
		self.interface.HideClock()

	## Emotion
	def BINARY_ActEmotion(self, emotionIndex):
		if self.interface.wndCharacter:
			self.interface.wndCharacter.ActEmotion(emotionIndex)

	###############################################################################################
	###############################################################################################
	## Keyboard Functions

	def CheckFocus(self):
		if FALSE == self.IsFocus():
			if TRUE == self.interface.IsOpenChat():
				self.interface.ToggleChat()

			self.SetFocus()

	def SpeedButtonWindowOpen(self):
		constInfo.SPEED_BUTTON = 1

	def SaveScreen(self):
		print "save screen"

		# SCREENSHOT_CWDSAVE
		if SCREENSHOT_CWDSAVE:
			if not os.path.exists(os.getcwd()+os.sep+"screenshot"):
				os.mkdir(os.getcwd()+os.sep+"screenshot")

			(succeeded, name) = grp.SaveScreenShotToPath(os.getcwd()+os.sep+"screenshot"+os.sep)
		elif SCREENSHOT_DIR:
			(succeeded, name) = grp.SaveScreenShot(SCREENSHOT_DIR)
		else:
			(succeeded, name) = grp.SaveScreenShot()
		# END_OF_SCREENSHOT_CWDSAVE

		if succeeded:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "%s %s %s" % (name, localeInfo.SCREENSHOT_SAVE1, localeInfo.SCREENSHOT_SAVE2))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SCREENSHOT_SAVE_FAILURE)

	def ShowConsole(self):
		if debugInfo.IsDebugMode() or TRUE == self.consoleEnable:
			player.EndKeyWalkingImmediately()
			self.console.OpenWindow()

	def ShowName(self):
		self.ShowNameFlag = TRUE
		self.playerGauge.EnableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex+1)

	# ADD_ALWAYS_SHOW_NAME
	def __IsShowName(self):

		if systemSetting.IsAlwaysShowName():
			return TRUE

		if self.ShowNameFlag:
			return TRUE

		return FALSE
	# END_OF_ADD_ALWAYS_SHOW_NAME

	def HideName(self):
		self.ShowNameFlag = FALSE
		self.playerGauge.DisableShowAlways()
		player.SetQuickPage(self.quickSlotPageIndex)

	def ShowMouseImage(self):
		self.interface.ShowMouseImage()

	def HideMouseImage(self):
		self.interface.HideMouseImage()

	def StartAttack(self):
		player.SetAttackKeyState(TRUE)

	def EndAttack(self):
		player.SetAttackKeyState(FALSE)

	def MoveUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, TRUE)

	def MoveDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, TRUE)

	def MoveLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, TRUE)

	def MoveRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, TRUE)

	def StopUp(self):
		player.SetSingleDIKKeyState(app.DIK_UP, FALSE)

	def StopDown(self):
		player.SetSingleDIKKeyState(app.DIK_DOWN, FALSE)

	def StopLeft(self):
		player.SetSingleDIKKeyState(app.DIK_LEFT, FALSE)

	def StopRight(self):
		player.SetSingleDIKKeyState(app.DIK_RIGHT, FALSE)

	def PickUpItem(self):
		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			if (constInfo.PICKUPMODE & player.AUTOMATIC_PICK_UP_ACTIVATE):
				player.PickCloseItemAutoVector()
			else:
				player.PickCloseItemVector()
		else:
			if app.ENABLE_INSTANT_PICKUP:
				player.PickCloseItemVector()
			else:
				player.PickCloseItem()

	###############################################################################################
	###############################################################################################
	## Event Handler

	def OnKeyDown(self, key):
		if self.interface and self.interface.wndWeb and self.interface.wndWeb.IsShow():
			return

		if key == app.DIK_ESC:
			self.RequestDropItem(False)
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		if self.onPressKeyDict:
			try:
				self.onPressKeyDict[key]()
			except KeyError:
				pass
			except:
				raise

		return True

	def OnKeyUp(self, key):
		if self.onClickKeyDict:
			try:
				self.onClickKeyDict[key]()
			except KeyError:
				pass
			except:
				raise

		return True

	def OnMouseLeftButtonDown(self):
		import advanced_metin_queue
		if not mouseModule.mouseController.isAttached() and app.IsPressed(app.DIK_LCONTROL):
			if advanced_metin_queue.Toggle(chr.Pick()):
				self._optionQueueClickConsumed=True
				return TRUE
		advanced_metin_queue.Clear()

		if self.interface.BUILD_OnMouseLeftButtonDown():
			return

		if mouseModule.mouseController.isAttached():
			self.CheckFocus()
		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				return
			else:
				self.CheckFocus()
				player.SetMouseState(player.MBT_LEFT, player.MBS_PRESS);

		return TRUE

	def OnMouseLeftButtonUp(self):
		if getattr(self,"_optionQueueClickConsumed",False):
			self._optionQueueClickConsumed=False
			return TRUE

		if self.interface.BUILD_OnMouseLeftButtonUp():
			return

		if mouseModule.mouseController.isAttached():

			attachedType = mouseModule.mouseController.GetAttachedType()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			attachedItemSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()

			## QuickSlot
			if player.SLOT_TYPE_QUICK_SLOT == attachedType:
				player.RequestDeleteGlobalQuickSlot(attachedItemSlotPos)

			## Inventory
			elif player.SLOT_TYPE_INVENTORY == attachedType or\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == attachedType and app.ENABLE_SPECIAL_INVENTORY):

				if player.ITEM_MONEY == attachedItemIndex:
					self.__PutMoney(attachedType, attachedItemCount, self.PickingCharacterIndex)
				else:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			## DragonSoul
			elif app.ENABLE_AURA_COSTUME_SYSTEM and player.SLOT_TYPE_AURA == attachedType:
				self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.SLOT_TYPE_CHANGE_LOOK == attachedType:
					self.__PutItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, self.PickingCharacterIndex)

			mouseModule.mouseController.DeattachObject()

		else:
			hyperlink = ui.GetHyperlink()
			if hyperlink:
				if app.IsPressed(app.DIK_LALT):
					link = chat.GetLinkFromHyperlink(hyperlink)
					ime.PasteString(link)
				else:
					self.interface.MakeHyperlinkTooltip(hyperlink)
				return
			else:
				player.SetMouseState(player.MBT_LEFT, player.MBS_CLICK)

		#player.EndMouseWalking()
		return TRUE

	def __PutItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount, dstChrID):
		if player.SLOT_TYPE_INVENTORY == attachedType or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedType or\
			(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or\
			player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or\
			player.SLOT_TYPE_STONE_INVENTORY == attachedType or\
			player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedType or\
			player.SLOT_TYPE_CHANGERS_INVENTORY == attachedType and app.ENABLE_SPECIAL_INVENTORY):

			attachedInvenType = player.SlotTypeToInvenType(attachedType)
			if TRUE == chr.HasInstance(self.PickingCharacterIndex) and player.GetMainCharacterIndex() != dstChrID:
				if player.IsEquipmentSlot(attachedItemSlotPos) and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedType:
					self.stream.popupWindow.Close()
					self.stream.popupWindow.Open(localeInfo.EXCHANGE_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)
				else:
					if chr.IsNPC(dstChrID):
						if app.ENABLE_AUTO_REFINE:
							constInfo.AUTO_REFINE_TYPE = 2
							constInfo.AUTO_REFINE_DATA["NPC"][0] = dstChrID
							constInfo.AUTO_REFINE_DATA["NPC"][1] = attachedInvenType
							constInfo.AUTO_REFINE_DATA["NPC"][2] = attachedItemSlotPos
							constInfo.AUTO_REFINE_DATA["NPC"][3] = attachedItemCount
						net.SendGiveItemPacket(dstChrID, attachedInvenType, attachedItemSlotPos, attachedItemCount)
					else:
						net.SendExchangeStartPacket(dstChrID)
						net.SendExchangeItemAddPacket(attachedInvenType, attachedItemSlotPos, 0)
			else:
				self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			if player.SLOT_TYPE_AURA == attachedType:
				self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			if player.SLOT_TYPE_CHANGE_LOOK == attachedType:
				self.__DropItem(attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount)

	def __PutMoney(self, attachedType, attachedMoney, dstChrID):
		if TRUE == chr.HasInstance(dstChrID) and player.GetMainCharacterIndex() != dstChrID:
			net.SendExchangeStartPacket(dstChrID)
			net.SendExchangeElkAddPacket(attachedMoney)
		else:
			self.__DropMoney(attachedType, attachedMoney)

	def __DropMoney(self, attachedType, attachedMoney):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if attachedMoney>=1000:
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_MONEY_FAILURE_1000_OVER, 0, localeInfo.UI_OK)
			return

		itemDropQuestionDialog = uiCommon.QuestionDialog("thin")
		itemDropQuestionDialog.SetText(localeInfo.DO_YOU_DROP_MONEY % (attachedMoney))
		itemDropQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.RequestDropItem(arg))
		itemDropQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.RequestDropItem(arg))
		itemDropQuestionDialog.Open()
		itemDropQuestionDialog.dropType = attachedType
		itemDropQuestionDialog.dropCount = attachedMoney
		itemDropQuestionDialog.dropNumber = player.ITEM_MONEY
		self.itemDropQuestionDialog = itemDropQuestionDialog

	def __DropItem(self, attachedType, attachedItemIndex, attachedItemSlotPos, attachedItemCount):
		if app.ENABLE_GAYA_STACK:
			if self.interface and self.interface.wndItemSelectEx and self.interface.wndItemSelectEx.IsShow():
				return

		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if player.SLOT_TYPE_INVENTORY == attachedType and player.IsEquipmentSlot(attachedItemSlotPos):
			self.stream.popupWindow.Close()
			self.stream.popupWindow.Open(localeInfo.DROP_ITEM_FAILURE_EQUIP_ITEM, 0, localeInfo.UI_OK)

		else:
			if player.SLOT_TYPE_INVENTORY == attachedType or\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == attachedType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == attachedType and app.ENABLE_SPECIAL_INVENTORY):

				dropItemIndex = player.GetItemIndex(attachedItemSlotPos)

				item.SelectItem(dropItemIndex)
				dropItemName = item.GetItemName()

				## Question Text
				questionText = localeInfo.HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, attachedItemCount)

				## Dialog
				itemDropQuestionDialog = uiCommon.ItemQuestionDialog()
				itemDropQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.RequestDropItem(arg))
				itemDropQuestionDialog.SetSellEvent(lambda arg=TRUE: self.RequestSellItem(arg))
				if app.ENABLE_DESTROY_DIALOG:
					itemDropQuestionDialog.SetDestroyEvent(lambda arg=TRUE: self.RequestDestroyItem(arg))
				itemDropQuestionDialog.Open(dropItemIndex, attachedItemSlotPos, 0, questionText)
				itemDropQuestionDialog.dropType = attachedType
				itemDropQuestionDialog.dropNumber = attachedItemSlotPos
				itemDropQuestionDialog.dropCount = attachedItemCount
				self.itemDropQuestionDialog = itemDropQuestionDialog

				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				if player.SLOT_TYPE_AURA == attachedType:
					net.SendAuraRefineCheckOut(attachedItemSlotPos, player.GetAuraRefineWindowType())

			if app.ENABLE_CHANGE_LOOK_SYSTEM:
				if player.SLOT_TYPE_CHANGE_LOOK == attachedType:
					net.SendChangeLookCheckOut(attachedItemSlotPos)

	def RequestDropItem(self, answer):
		if not self.itemDropQuestionDialog:
			return

		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropCount = self.itemDropQuestionDialog.dropCount
			dropNumber = self.itemDropQuestionDialog.dropNumber

			if player.SLOT_TYPE_INVENTORY == dropType or\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == dropType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == dropType or\
				player.SLOT_TYPE_STONE_INVENTORY == dropType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == dropType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == dropType and app.ENABLE_SPECIAL_INVENTORY):

				if dropNumber == player.ITEM_MONEY:
					net.SendGoldDropPacketNew(dropCount)
					snd.PlaySound("sound/ui/money.wav")
				else:
					self.__SendDropItemPacket(dropNumber, dropCount)

		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	if app.ENABLE_DESTROY_DIALOG:
		def RequestDestroyItem(self, answer):
			if not self.itemDropQuestionDialog:
				return

			if answer:
				dropType = self.itemDropQuestionDialog.dropType
				dropCount = self.itemDropQuestionDialog.dropCount
				dropNumber = self.itemDropQuestionDialog.dropNumber
				self.itemDropQuestionDialog.Close()
				self.itemDropQuestionDialog = None
				questionText = localeInfo.DESTROY_ITEM_QUESTION
				itemDestroyQuestionDialog = uiCommon.QuestionDialog("thin")
				itemDestroyQuestionDialog.SetText(questionText)
				itemDestroyQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.RequestDestroyItemFinaly(arg))
				itemDestroyQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.RequestDestroyItemFinaly(arg))
				itemDestroyQuestionDialog.dropType = dropType
				itemDestroyQuestionDialog.dropNumber = dropNumber
				itemDestroyQuestionDialog.dropCount = dropCount
				itemDestroyQuestionDialog.Open()
				self.itemDestroyQuestionDialog = itemDestroyQuestionDialog

		def RequestDestroyItemFinaly(self, answer):
			if not self.itemDestroyQuestionDialog:
				return

			if answer:
				dropType = self.itemDestroyQuestionDialog.dropType
				dropCount = self.itemDestroyQuestionDialog.dropCount
				dropNumber = self.itemDestroyQuestionDialog.dropNumber

				if player.SLOT_TYPE_INVENTORY == dropType or\
					(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == dropType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == dropType or\
					player.SLOT_TYPE_STONE_INVENTORY == dropType or\
					player.SLOT_TYPE_GIFT_BOX_INVENTORY == dropType or\
					player.SLOT_TYPE_CHANGERS_INVENTORY == dropType and app.ENABLE_SPECIAL_INVENTORY):

					if dropNumber == player.ITEM_MONEY:
						return
					else:
						self.__SendDestroyItemPacket(dropNumber, dropCount)

			self.itemDestroyQuestionDialog.Close()
			self.itemDestroyQuestionDialog = None

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		def __SendDestroyItemPacket(self, itemVNum, itemCount, itemInvenType = player.INVENTORY):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
				return

			net.SendItemDestroyPacket(itemInvenType, itemVNum, itemCount)

	def RequestSellItem(self, answer):
		if not self.itemDropQuestionDialog:
			return

		if answer:
			dropType = self.itemDropQuestionDialog.dropType
			dropNumber = self.itemDropQuestionDialog.dropNumber
			dropCount = self.itemDropQuestionDialog.dropCount

			if player.SLOT_TYPE_INVENTORY == dropType or\
				(player.SLOT_TYPE_SKILL_BOOK_INVENTORY == dropType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == dropType or\
				player.SLOT_TYPE_STONE_INVENTORY == dropType or\
				player.SLOT_TYPE_GIFT_BOX_INVENTORY == dropType or\
				player.SLOT_TYPE_CHANGERS_INVENTORY == dropType and app.ENABLE_SPECIAL_INVENTORY):

				if dropNumber == player.ITEM_MONEY:
					return
				else:
					self.__SendSellItemPacket(dropNumber, dropCount)

		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __SendSellItemPacket(self, slot, count):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendShopSellPacketNew(slot, count)

	def __SendDropItemPacket(self, itemVNum, itemCount, itemInvenType = player.INVENTORY):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.DROP_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemDropPacketNew(itemInvenType, itemVNum, itemCount)

	def OnMouseRightButtonDown(self):

		self.CheckFocus()

		if TRUE == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			player.SetMouseState(player.MBT_RIGHT, player.MBS_PRESS)

		return TRUE

	def OnMouseRightButtonUp(self):
		if TRUE == mouseModule.mouseController.isAttached():
			return TRUE

		player.SetMouseState(player.MBT_RIGHT, player.MBS_CLICK)
		return TRUE

	def OnMouseMiddleButtonDown(self):
		player.SetMouseMiddleButtonState(player.MBS_PRESS)

	def OnMouseMiddleButtonUp(self):
		player.SetMouseMiddleButtonState(player.MBS_CLICK)

	def OnUpdate(self):
		if getattr(app, "ENABLE_SKILL_UPGRADE", 0):
			import uiSkillUpgrade
			uiSkillUpgrade.CheckAlerts(self.interface)
		if app.ENABLE_ADVANCED_GAME_OPTIONS:
			import advanced_options_runtime
			advanced_options_runtime.UpdateMapSettings()
		app.UpdateGame()

		if self.mapNameShower.IsShow():
			self.mapNameShower.Update()

		if self.isShowDebugInfo:
			self.UpdateDebugInfo()

		if self.enableXMasBoom:
			self.__XMasBoom_Update()

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			if self.enableXMasSoul:
				self.__XMasSoul_Update()

		#if app.ENABLE_BATTLE_PASS:
		#	if constInfo.status_battle_pass == 1:
		#		self.interface.Show_BattlePass()
		#	else:
		#		self.interface.Hide_BattlePass()

		#	if constInfo.status_battle_pass_premium == 1:
		#		self.interface.Show_BattlePass_Premium()
		#	else:
		#		self.interface.Hide_BattlePass_Premium()

		self.interface.BUILD_OnUpdate()

		if self.interface.wndExpandedMoneyTaskBar:
			self.interface.wndExpandedMoneyTaskBar.OnUpdate()

		if constInfo.SPEED_BUTTON == 1:
			self.SpeedButtonWindowShow()
			constInfo.SPEED_BUTTON = 0

		if app.__AUTO_HUNT__:
			if constInfo.autoHuntAutoLoginDict["status"] == 1 and constInfo.autoHuntAutoLoginDict["leftTime"] > 0 and constInfo.autoHuntAutoLoginDict["leftTime"] < app.GetGlobalTimeStamp():
				constInfo.autoHuntAutoLoginDict["leftTime"] = 0
				self.interface.CheckAutoLogin()

		# Tooltip kontrolü: Sadece haritadaki item tooltip'lerinin ekranda kalmasýný önle
		# Envanter, shop gibi pencerelerdeki tooltip'leri etkilemez
		try:
			import uiToolTip
			
			# Interface üzerinden tooltip'e eriþ
			tooltipItem = None
			if self.interface and hasattr(self.interface, 'tooltipItem'):
				tooltipItem = self.interface.tooltipItem
			
			# Eðer interface'den bulamadýysak, global tooltipItem'i kontrol et
			if not tooltipItem and hasattr(uiToolTip, 'tooltipItem'):
				tooltipItem = uiToolTip.tooltipItem
			
			if tooltipItem and tooltipItem.IsShow():
				# Fare pozisyonunu al
				try:
					(x, y) = app.GetCursorPosition()
				except:
					try:
						(x, y) = wndMgr.GetMousePosition()
					except:
						return
				
				# Açýk pencerelerin üzerinde mi kontrol et
				onWindow = False
				onDungeonTimer = False
				if self.interface:
					# Tüm açýk pencereleri kontrol et (dungeon timer hariç - o haritada da açýk olabilir)
					windows_to_check = [
						'wndInventory', 'wndSafebox', 'wndMall', 'wndCube',
						'dlgShop', 'dlgExchange', 'wndItemSelect',
						'wndCharacter', 'wndTaskBar', 'wndGuild', 'wndMessenger',
						'wndMiniMap', 'wndGameButton', 'wndChat',
					]
					
					for attr_name in windows_to_check:
						if hasattr(self.interface, attr_name):
							window = getattr(self.interface, attr_name, None)
							if window and hasattr(window, 'IsShow') and window.IsShow():
								try:
									(wx, wy) = window.GetGlobalPosition()
									ww, wh = window.GetWidth(), window.GetHeight()
									if wx <= x <= wx + ww and wy <= y <= wy + wh:
										onWindow = True
										break
								except:
									pass
					
					# Dungeon timer'ý ayrý kontrol et (haritada da açýk olabilir)
					if hasattr(self.interface, 'wndDungeonTimer'):
						dungeonTimer = getattr(self.interface, 'wndDungeonTimer', None)
						if dungeonTimer and hasattr(dungeonTimer, 'IsShow') and dungeonTimer.IsShow():
							try:
								(dx, dy) = dungeonTimer.GetGlobalPosition()
								dw, dh = dungeonTimer.GetWidth(), dungeonTimer.GetHeight()
								if dx <= x <= dx + dw and dy <= y <= dy + dh:
									onDungeonTimer = True
							except:
								pass
				
				# Eðer hiçbir pencere açýk deðilse (yani haritadayýz) ve tooltip açýksa
				# Veya dungeon timer açýk ama fare üzerinde deðilse
				if not onWindow:
					# Tooltip'in pozisyonunu al
					try:
						(tx, ty) = tooltipItem.GetGlobalPosition()
						tw, th = tooltipItem.GetWidth(), tooltipItem.GetHeight()
						onTooltip = (tx <= x <= tx + tw and ty <= y <= ty + th)
						
						# Fare tooltip'in üzerinde deðilse kapat
						# Dungeon timer açýk olsa bile, fare tooltip'in üzerinde deðilse kapat
						if not onTooltip:
							# Eðer dungeon timer açýksa ve fare üzerindeyse, tooltip'i kapatma
							if onDungeonTimer:
								pass  # Dungeon timer üzerindeyse tooltip'i açýk býrak
							else:
								tooltipItem.HideToolTip()
					except:
						# Hata durumunda tooltip'i kapat
						try:
							tooltipItem.HideToolTip()
						except:
							pass
		except:
			pass

		if app.ENABLE_EVENT_SYSTEM and self.interface and self.interface.wndEvent:
			self.interface.wndEvent.OnUpdate()

	def UpdateDebugInfo(self):
		(x, y, z) = player.GetMainCharacterPosition()
		nUpdateTime = app.GetUpdateTime()
		nUpdateFPS = app.GetUpdateFPS()
		nRenderFPS = app.GetRenderFPS()
		nFaceCount = app.GetFaceCount()
		fFaceSpeed = app.GetFaceSpeed()
		nST=background.GetRenderShadowTime()
		(fAveRT, nCurRT) =  app.GetRenderTime()
		(iNum, fFogStart, fFogEnd, fFarCilp) = background.GetDistanceSetInfo()
		(iPatch, iSplat, fSplatRatio, sTextureNum) = background.GetRenderedSplatNum()
		if iPatch == 0:
			iPatch = 1

		self.PrintCoord.SetText("Coordinate: %.2f %.2f %.2f ATM: %d" % (x, y, z, app.GetAvailableTextureMemory()/(1024*1024)))
		xMouse, yMouse = wndMgr.GetMousePosition()
		self.PrintMousePos.SetText("MousePosition: %d %d" % (xMouse, yMouse))

		self.FrameRate.SetText("UFPS: %3d UT: %3d FS %.2f" % (nUpdateFPS, nUpdateTime, fFaceSpeed))

		if fAveRT>1.0:
			self.Pitch.SetText("RFPS: %3d RT:%.2f(%3d) FC: %d(%.2f) " % (nRenderFPS, fAveRT, nCurRT, nFaceCount, nFaceCount/fAveRT))

		self.Splat.SetText("PATCH: %d SPLAT: %d BAD(%.2f)" % (iPatch, iSplat, fSplatRatio))
		self.ViewDistance.SetText("Num : %d, FS : %f, FE : %f, FC : %f" % (iNum, fFogStart, fFogEnd, fFarCilp))

	def OnRender(self):
		app.RenderGame()

		if self.console.Console.collision:
			background.RenderCollision()
			chr.RenderCollision()

		(x, y) = app.GetCursorPosition()

		########################
		# Picking
		########################
		textTail.UpdateAllTextTail()

		if TRUE == wndMgr.IsPickedWindow(self.hWnd):

			self.PickingCharacterIndex = chr.Pick()

			if -1 != self.PickingCharacterIndex:
				textTail.ShowCharacterTextTail(self.PickingCharacterIndex)
			if 0 != self.targetBoard.GetTargetVID():
				textTail.ShowCharacterTextTail(self.targetBoard.GetTargetVID())

			# ADD_ALWAYS_SHOW_NAME
			if not self.__IsShowName():
				self.PickingItemIndex = item.Pick()
				if -1 != self.PickingItemIndex:
					textTail.ShowItemTextTail(self.PickingItemIndex)
			# END_OF_ADD_ALWAYS_SHOW_NAME

		## Show all name in the range

		# ADD_ALWAYS_SHOW_NAME
		if self.__IsShowName():
			textTail.ShowAllTextTail()
			self.PickingItemIndex = textTail.Pick(x, y)
		# END_OF_ADD_ALWAYS_SHOW_NAME

		if app.ENABLE_GRAPHIC_ON_OFF:
			if systemSetting.IsShowSalesText():
				uiPrivateShopBuilder.UpdateADBoard()

		textTail.UpdateShowingTextTail()
		textTail.ArrangeTextTail()
		if -1 != self.PickingItemIndex:
			textTail.SelectItemName(self.PickingItemIndex)

		grp.PopState()
		grp.SetInterfaceRenderState()

		textTail.Render()
		textTail.HideAllTextTail()

	def OnPressEscapeKey(self):
		import advanced_metin_queue
		advanced_metin_queue.Clear()
		if app.TARGET == app.GetCursor():
			app.SetCursor(app.NORMAL)

		elif TRUE == mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()

		else:
			self.interface.OpenSystemDialog()

		return TRUE

	def OnIMEReturn(self):
		if app.IsPressed(app.DIK_LSHIFT):
			self.interface.OpenWhisperDialogWithoutTarget()
		else:
			self.interface.ToggleChat()
		return TRUE

	def OnPressExitKey(self):
		self.interface.ToggleSystemDialog()
		return TRUE

	## BINARY CALLBACK
	######################################################################################
	if app.ENABLE_CONQUEROR_LEVEL:
		# Sungma
		def BINARY_SungMaAttr(self, str, hp, move, immune):
			if self.affectShower:
				self.affectShower.SetSungMaAffectImage(str, hp, move, immune)
	# WEDDING
	def BINARY_LoverInfo(self, name, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnAddLover(name, lovePoint)
		if self.affectShower:
			self.affectShower.SetLoverInfo(name, lovePoint)

	def BINARY_UpdateLovePoint(self, lovePoint):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnUpdateLovePoint(lovePoint)
		if self.affectShower:
			self.affectShower.OnUpdateLovePoint(lovePoint)
	# END_OF_WEDDING

	if app.ENABLE_MOB_DROP_INFO:
		def BINARY_AddTargetMonsterDropInfo(self, raceNum, itemVnum, itemCount):
			if not raceNum in uiTarget.MONSTER_INFO_DATA:
				uiTarget.MONSTER_INFO_DATA.update({raceNum : {}})
				uiTarget.MONSTER_INFO_DATA[raceNum].update({"items" : []})
			curList = uiTarget.MONSTER_INFO_DATA[raceNum]["items"]

			isUpgradeable = FALSE
			isMetin = FALSE
			item.SelectItem(itemVnum)
			if item.GetItemType() == item.ITEM_TYPE_WEAPON or item.GetItemType() == item.ITEM_TYPE_ARMOR:
				isUpgradeable = TRUE
			elif item.GetItemType() == item.ITEM_TYPE_METIN:
				isMetin = TRUE

			for curItem in curList:
				if isUpgradeable:
					if curItem.has_key("vnum_list") and curItem["vnum_list"][0] / 10 * 10 == itemVnum / 10 * 10:
						if not (itemVnum in curItem["vnum_list"]):
							curItem["vnum_list"].append(itemVnum)
						return
				elif isMetin:
					if curItem.has_key("vnum_list"):
						baseVnum = curItem["vnum_list"][0]
					if curItem.has_key("vnum_list") and (baseVnum - baseVnum%1000) == (itemVnum - itemVnum%1000):
						if not (itemVnum in curItem["vnum_list"]):
							curItem["vnum_list"].append(itemVnum)
						return
				else:
					if curItem.has_key("vnum") and curItem["vnum"] == itemVnum and curItem["count"] == itemCount:
						return

			if isUpgradeable or isMetin:
				curList.append({"vnum_list":[itemVnum], "count":itemCount})
			else:
				curList.append({"vnum":itemVnum, "count":itemCount})

		def BINARY_RefreshTargetMonsterDropInfo(self, raceNum):
			self.targetBoard.RefreshMonsterInfoBoard()

	# QUEST_CONFIRM
	def BINARY_OnQuestConfirm(self, msg, timeout, pid):
		confirmDialog = uiCommon.QuestionDialogWithTimeLimit2()
		confirmDialog.SetText1(msg)
		confirmDialog.SetAcceptEvent(lambda answer=TRUE, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		confirmDialog.SetCancelEvent(lambda answer=FALSE, pid=pid: net.SendQuestConfirmPacket(answer, pid) or self.confirmDialog.Hide())
		confirmDialog.Open(timeout)
		self.confirmDialog = confirmDialog
	# END_OF_QUEST_CONFIRM

	# GIFT command
	def Gift_Show(self):
		self.interface.ShowGift()

	# CUBE
	def BINARY_Cube_Open(self, npcVNUM):
		self.currentCubeNPC = npcVNUM
		self.interface.OpenCubeWindow()

		if npcVNUM not in self.cubeInformation:
			net.SendChatPacket("/cube r_info")
		else:
			cubeInfoList = self.cubeInformation[npcVNUM]

			i = 0
			for cubeInfo in cubeInfoList:
				self.interface.wndCube.AddCubeResultItem(cubeInfo["vnum"], cubeInfo["count"])

				j = 0
				for materialList in cubeInfo["materialList"]:
					for materialInfo in materialList:
						itemVnum, itemCount = materialInfo
						self.interface.wndCube.AddMaterialInfo(i, j, itemVnum, itemCount)
					j = j + 1

				i = i + 1

			self.interface.wndCube.Refresh()

	def BINARY_Cube_Close(self):
		self.interface.CloseCubeWindow()

	def BINARY_Cube_UpdateInfo(self, gold, itemVnum, count):
		self.interface.UpdateCubeInfo(gold, itemVnum, count)

	def BINARY_Cube_Succeed(self, itemVnum, count):
		print "Å¥ºê Á¦ÀÛ ¼º°ø"
		self.interface.SucceedCubeWork(itemVnum, count)
		pass

	def BINARY_Cube_Failed(self):
		print "Å¥ºê Á¦ÀÛ ½ÇÆ?"
		self.interface.FailedCubeWork()
		pass

	def BINARY_Cube_ResultList(self, npcVNUM, listText):
		if npcVNUM == 0:
			npcVNUM = self.currentCubeNPC

		self.cubeInformation[npcVNUM] = []

		try:
			for eachInfoText in listText.split("/"):
				eachInfo = eachInfoText.split(",")
				itemVnum	= int(eachInfo[0])
				itemCount	= int(eachInfo[1])

				self.cubeInformation[npcVNUM].append({"vnum": itemVnum, "count": itemCount})
				self.interface.wndCube.AddCubeResultItem(itemVnum, itemCount)

			resultCount = len(self.cubeInformation[npcVNUM])
			requestCount = 7
			modCount = resultCount % requestCount
			splitCount = resultCount / requestCount
			for i in xrange(splitCount):
				#print("/cube r_info %d %d" % (i * requestCount, requestCount))
				net.SendChatPacket("/cube r_info %d %d" % (i * requestCount, requestCount))

			if 0 < modCount:
				#print("/cube r_info %d %d" % (splitCount * requestCount, modCount))
				net.SendChatPacket("/cube r_info %d %d" % (splitCount * requestCount, modCount))

		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	if app.ENABLE_SUNG_MAHI_TOWER:
		def ClearSungMahiInfo(self):
			constInfo.sungMahiInfo = []
			constInfo.sungMahiLevelInfo = 0
			constInfo.sungMahiQuest = 0
		
		def SetSungMahiQuest(self, questindex):
			constInfo.sungMahiQuest = questindex
			
		def UpdateSungMahiInfo(self, levelInfo, levelIndex, levelRank, levelEndTime, isLevelCompleted):
			levelInfo = int(levelInfo)
			levelIndex = int(levelIndex)
			levelRank = int(levelRank)
			levelEndTime = int(levelEndTime)
			isLevelCompleted = int(isLevelCompleted)
			
			constInfo.sungMahiInfo.append([levelIndex, levelRank, levelEndTime, isLevelCompleted])
			constInfo.sungMahiLevelInfo = levelInfo
		
		def OpenSungMahiWindow(self):
			self.interface.ToggleSungMahiWindow()
		
		def UpdateSungMahiNotice(self, noticeType, noticeText):
			noticeType = int(noticeType)
			noticeText = str(noticeText).replace("_", " ")
			
			self.interface.UpdateSungMahiNotice(noticeType, noticeText)
		
		def SungMahiClearNotice(self, noticeType):
			noticeType = int(noticeType)
			self.interface.SungMahiClearNotice(noticeType)
		
		def UpdateRoomLevel(self, roomLevel):
			self.interface.UpdateRoomLevel(roomLevel)
			
		def UpdateTowerLevel(self, towerLevel):
			self.interface.UpdateTowerLevel(towerLevel)
			
		def UpdateRoomTime(self, roomTime):
			self.interface.UpdateRoomTime(roomTime)

	def BINARY_Cube_MaterialInfo(self, startIndex, listCount, listText):
		# Material Text Format : 125,1|126,2|127,2|123,5&555,5&555,4/120000
		try:
			#print listText

			if 3 > len(listText):
				dbg.TraceError("Wrong Cube Material Infomation")
				return 0

			eachResultList = listText.split("@")

			cubeInfo = self.cubeInformation[self.currentCubeNPC]

			itemIndex = 0
			for eachResultText in eachResultList:
				cubeInfo[startIndex + itemIndex]["materialList"] = [[], [], [], [], []]
				materialList = cubeInfo[startIndex + itemIndex]["materialList"]

				gold = 0
				splitResult = eachResultText.split("/")
				if 1 < len(splitResult):
					gold = int(splitResult[1])

				#print "splitResult : ", splitResult
				eachMaterialList = splitResult[0].split("&")

				i = 0
				for eachMaterialText in eachMaterialList:
					complicatedList = eachMaterialText.split("|")

					if 0 < len(complicatedList):
						for complicatedText in complicatedList:
							(itemVnum, itemCount) = complicatedText.split(",")
							itemVnum = int(itemVnum)
							itemCount = int(itemCount)
							self.interface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

							materialList[i].append((itemVnum, itemCount))

					else:
						itemVnum, itemCount = eachMaterialText.split(",")
						itemVnum = int(itemVnum)
						itemCount = int(itemCount)
						self.interface.wndCube.AddMaterialInfo(itemIndex + startIndex, i, itemVnum, itemCount)

						materialList[i].append((itemVnum, itemCount))

					i = i + 1

				itemIndex = itemIndex + 1

			self.interface.wndCube.Refresh()

		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

		pass

	# END_OF_CUBE

	def BINARY_Highlight_Item(self, inven_type, inven_pos):
		if self.interface:
			self.interface.Highligt_Item(inven_type, inven_pos)

	if app.ENABLE_MINI_GAME_OKEY:
		## MiniGame Rumi
		def BINARY_Cards_UpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points):
			self.interface.UpdateCardsInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, hand_4, hand_4_v, hand_5, hand_5_v, cards_left, points)

		def BINARY_Cards_FieldUpdateInfo(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			self.interface.UpdateCardsFieldInfo(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

		def BINARY_Cards_PutReward(self, hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points):
			self.interface.CardsPutReward(hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points)

		def BINARY_Cards_ShowIcon(self):
			self.interface.CardsShowIcon()

		def BINARY_Cards_Open(self, safemode):
			self.interface.OpenCardsWindow(safemode)

	def OptionAutoCape(self, active, reason):
		import advanced_option_notifications
		if reason not in ("0", "1", "2", "3"): return
		self._optionAutoCapeActive = active == "1"
		if self.interface and self.interface.wndInventory and hasattr(self.interface.wndInventory, "SetAutoCapeStatus"):
			self.interface.wndInventory.SetAutoCapeStatus(self._optionAutoCapeActive)
		if reason == "3": return
		text = {"0":"Otomatik pelerin durduruldu.", "1":"Otomatik pelerin baslatildi.", "2":"Otomatik pelerin durdu: envanterde pelerin yok."}[reason]
		advanced_option_notifications.Record("auto_cape", text, popup=True)

	def ConfirmationLevelup(self, currentLevel):
		import advanced_options_runtime
		try:
			currentLevel = int(currentLevel)
		except (TypeError, ValueError):
			return
		if currentLevel <= 0:
			return
		if advanced_options_runtime.Get("disable_level_up_confirmation", False):
			net.SendChatPacket("/confirm_levelup")
			return
		self._CloseLevelUpConfirmation()
		dialog = uiCommon.QuestionDialogWithMultiTextline()
		dialog.SetText("%d. seviyeye gecmek istiyor musun?\nOnaylamadan seviye atlanmaz." % (currentLevel + 1))
		dialog.SetAcceptEvent(lambda: self.OnAcceptLevelUp())
		dialog.SetCancelEvent(lambda: self.OnDenyLevelUp())
		dialog.Open()
		self.levelUpConfirmDialog = dialog

	def OnAcceptLevelUp(self):
		net.SendChatPacket("/confirm_levelup")
		self._CloseLevelUpConfirmation()

	def OnDenyLevelUp(self):
		self._CloseLevelUpConfirmation()

	def _CloseLevelUpConfirmation(self):
		dialog = getattr(self, "levelUpConfirmDialog", None)
		if dialog:
			dialog.Close()
		self.levelUpConfirmDialog = None

	def OptionItemExpired(self, category, vnum):
		import advanced_option_notifications
		if category not in ("item_expired", "companion_expired", "alchemy_expired"):
			return
		try:
			vnum = int(vnum)
			if vnum <= 0: return
		except (TypeError, ValueError):
			return
		item.SelectItem(vnum)
		name = item.GetItemName() or ("Esya #%d" % vnum)
		advanced_option_notifications.Record(category, "%s: suresi doldu." % name, popup=True)

	def OptionMiningFinished(self):
		import advanced_options_runtime, advanced_option_notifications
		advanced_option_notifications.Record("mining", "Kazma islemi tamamlandi.")
		if advanced_options_runtime.Get("mining_finish_flash", True):
			app.FlashApplication()

	def OptionNoticeNext(self, category):
		import time
		self._optionNoticeCategory = (category, time.time())

	def RecordOptionNotice(self, message):
		import time, advanced_option_notifications
		category, received = getattr(self, "_optionNoticeCategory", ("system", 0))
		self._optionNoticeCategory = ("system", 0)
		if time.time()-received > 2: category = "system"
		return advanced_option_notifications.Record(category,message)

	def BINARY_SetBigMessage(self, message):
		if self.RecordOptionNotice(message):
			self.interface.bigBoard.SetTip(message)

	if app.ENABLE_NEW_DUNGEON_LIB:
		def BINARY_SetMissionMessage(self, message):
			self.interface.missionBoard.SetMission(message)

	def BINARY_SetTipMessage(self, message):
		if self.RecordOptionNotice(message):
			self.interface.tipBoard.SetTip(message)

	if app.ENABLE_RENEWAL_OX_EVENT:
		def BINARY_SetBigControlMessage(self, message):
			self.interface.bigBoardControl.SetTip(message)

	if app.ENABLE_EXTENDED_WHISPER_DETAILS and app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def BINARY_RecieveWhisperDetails(self, name, country):
			if self.interface:
				self.interface.RecieveWhisperDetails(name, country)

	def BINARY_AppendNotifyMessage(self, type):
		if not type in localeInfo.NOTIFY_MESSAGE:
			return
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOTIFY_MESSAGE[type])

	if app.ENABLE_TITLE_SYSTEM:
		def __TitleSystemSyncReset(self):
			if self.interface:
				self.interface.TitleSystemSyncReset()

		def __TitleSystemSyncAdd(self, titleID):
			if self.interface:
				self.interface.TitleSystemSyncAdd(titleID)

		def __TitleSystemSyncActive(self, titleID):
			if self.interface:
				self.interface.TitleSystemSyncActive(titleID)

	def BINARY_Guild_EnterGuildArea(self, areaID):
		self.interface.BULID_EnterGuildArea(areaID)

	def BINARY_Guild_ExitGuildArea(self, areaID):
		self.interface.BULID_ExitGuildArea(areaID)

	def BINARY_GuildWar_OnSendDeclare(self, guildID):
		pass

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def BINARY_GuildWar_OnRecvDeclare(self, guildID, warType, winType, ScoreType, TimeType):
			mainCharacterName = player.GetMainCharacterName()
			masterName = guild.GetGuildMasterName()
			if mainCharacterName == masterName:
				self.__GuildWar_OpenAskDialog(guildID, warType, winType, ScoreType, TimeType)

		def BINARY_GuildWar_OnRecvPoint(self, gainGuildID, opponentGuildID, point, winpoint):
			self.interface.OnRecvGuildWarPoint(gainGuildID, opponentGuildID, point, winpoint)
	else:
		def BINARY_GuildWar_OnRecvDeclare(self, guildID, warType):
			mainCharacterName = player.GetMainCharacterName()
			masterName = guild.GetGuildMasterName()
			if mainCharacterName == masterName:
				self.__GuildWar_OpenAskDialog(guildID, warType)

		def BINARY_GuildWar_OnRecvPoint(self, gainGuildID, opponentGuildID, point):
			self.interface.OnRecvGuildWarPoint(gainGuildID, opponentGuildID, point)

	def BINARY_GuildWar_OnStart(self, guildSelf, guildOpp):
		self.interface.OnStartGuildWar(guildSelf, guildOpp)

	def BINARY_GuildWar_OnEnd(self, guildSelf, guildOpp):
		self.interface.OnEndGuildWar(guildSelf, guildOpp)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.guildWarQuestionDialog:
				self.__GuildWar_CloseAskDialog()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def BINARY_BettingGuildWar_SetObserverMode(self, isEnable, isButtonShow=None):
			# Eðer isButtonShow verilmemiþse varsayýlan olarak True kullan
			if isButtonShow is None:
				isButtonShow = True
			self.interface.BINARY_SetObserverMode(isEnable, isButtonShow)
	else:
		def BINARY_BettingGuildWar_SetObserverMode(self, isEnable):
			self.interface.BINARY_SetObserverMode(isEnable)

	def BINARY_BettingGuildWar_UpdateObserverCount(self, observerCount):
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	def __GuildWar_UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2, observerCount):
		guildID1 = int(guildID1)
		guildID2 = int(guildID2)
		memberCount1 = int(memberCount1)
		memberCount2 = int(memberCount2)
		observerCount = int(observerCount)

		self.interface.UpdateMemberCount(guildID1, memberCount1, guildID2, memberCount2)
		self.interface.wndMiniMap.UpdateObserverCount(observerCount)

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def __GuildWar_OpenAskDialog(self, guildID, warType, winType, ScoreType, TimeType):
			guildName = guild.GetGuildName(guildID)

			# REMOVED_GUILD_BUG_FIX
			if "Noname" == guildName:
				return
			# END_OF_REMOVED_GUILD_BUG_FIX

			import uiGuild
			questionDialog = uiGuild.AcceptGuildWarDialog()
			questionDialog.SAFE_SetAcceptEvent(self.__GuildWar_OnAccept)
			questionDialog.SAFE_SetCancelEvent(self.__GuildWar_OnDecline)
			## [guild_renewal_war]
			questionDialog.Open(guildName, warType, winType, ScoreType, TimeType)

			self.guildWarQuestionDialog = questionDialog
	else:
		def __GuildWar_OpenAskDialog(self, guildID, warType):
			guildName = guild.GetGuildName(guildID)

			# REMOVED_GUILD_BUG_FIX
			if "Noname" == guildName:
				return
			# END_OF_REMOVED_GUILD_BUG_FIX

			import uiGuild
			questionDialog = uiGuild.AcceptGuildWarDialog()
			questionDialog.SAFE_SetAcceptEvent(self.__GuildWar_OnAccept)
			questionDialog.SAFE_SetCancelEvent(self.__GuildWar_OnDecline)
			questionDialog.Open(guildName, warType)

			self.guildWarQuestionDialog = questionDialog

	def __GuildWar_CloseAskDialog(self):
		self.guildWarQuestionDialog.Close()
		self.guildWarQuestionDialog = None

	def __GuildWar_OnAccept(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/war " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	def __GuildWar_OnDecline(self):

		guildName = self.guildWarQuestionDialog.GetGuildName()

		net.SendChatPacket("/nowar " + guildName)
		self.__GuildWar_CloseAskDialog()

		return 1

	# Buffi (reference: game.GameWindow.SetSupport* -> interface.SetBuffi*)
	def SetSupportData(self, level, intel, exp, exp_total, evo, duration, runes_1, runes_2, runes_3):
		if self.interface:
			self.interface.SetBuffiInformation(*map(int, (level, intel, exp, exp_total, evo, duration, runes_1, runes_2, runes_3)))

	def SetSupportSkill(self, pts, amt0, pwr0, amt1, pwr1, amt2, pwr2, amt3, pwr3, amt4, pwr4):
		if self.interface:
			self.interface.SetBuffiSkills(*map(int, (pts, amt0, pwr0, amt1, pwr1, amt2, pwr2, amt3, pwr3, amt4, pwr4)))

	def SetSupportSkillBook(self, book0, book1, book2, book3, book4):
		if self.interface:
			self.interface.SetBuffiSkillsBook(*map(int, (book0, book1, book2, book3, book4)))

	def SetSupportSkillCooldown(self, skill0, skill1, skill2, skill3, skill4):
		if self.interface:
			self.interface.SetBuffiSkillsCooldown(*map(int, (skill0, skill1, skill2, skill3, skill4)))

	def SetSupportName(self, name):
		if self.interface:
			self.interface.SetBuffiName(name)

	def SupportLeave(self):
		if self.interface:
			self.interface.Buffi_SetUnsummon()

	def SetSupportSkillColor(self, idx, c0, c1, c2, c3, c4):
		if self.interface and self.interface.wndCompanion:
			self.interface.wndCompanion.SetBuffiSkillColor(int(idx), map(long, (c0, c1, c2, c3, c4)))

	def SkillUpgradeInfo(self, upgradeType, skillIndex, bookVnum, count, progress, need, remain, maxed):
		if self.interface:
			self.interface.SetSkillUpgradeInfo(*map(int, (upgradeType, skillIndex, bookVnum, count, progress, need, remain, maxed)))

	def SkillUpgradeResult(self, result):
		if self.interface:
			self.interface.SendSkillUpgradeResult(int(result))

	def SkillUpgradeNegativeGradeConfirm(self):
		if self.interface:
			self.interface.SkillUpgradeNegativeGradeConfirm()

	def SupportEvolutionResult(self, success):
		if self.interface and self.interface.wndCompanion:
			self.interface.wndCompanion.OnBuffiEvolutionResult(int(success))

	def SetSupportCastSpeed(self, value):
		if self.interface and self.interface.wndCompanion:
			self.interface.wndCompanion.SetBuffiCastSpeed(int(value))

	def __ServerCommand_Build(self):
		serverCommandList={
			"OptionNoticeNext" : self.OptionNoticeNext,
			"SetSupportData"								: self.SetSupportData,
			"SetSupportSkill"								: self.SetSupportSkill,
			"SetSupportSkillBook"							: self.SetSupportSkillBook,
			"SetSupportSkillCooldown"						: self.SetSupportSkillCooldown,
			"SetSupportName"								: self.SetSupportName,
			"SupportLeave"									: self.SupportLeave,
			"SetSupportSkillColor"							: self.SetSupportSkillColor,
			"SetSupportCastSpeed"							: self.SetSupportCastSpeed,
			"SupportEvolutionResult"						: self.SupportEvolutionResult,
			"SkillUpgradeInfo"								: self.SkillUpgradeInfo,
			"SkillUpgradeResult"							: self.SkillUpgradeResult,
			"SkillUpgradeNegativeGradeConfirm"				: self.SkillUpgradeNegativeGradeConfirm,
			"OptionMiningFinished" : self.OptionMiningFinished,
			"OptionItemExpired" : self.OptionItemExpired,
			"OptionAutoCape" : self.OptionAutoCape,
			"ConfirmationLevelup" : self.ConfirmationLevelup,
			"ConsoleEnable"									: self.__Console_Enable,
			"DayMode"										: self.__DayMode_Update,
			"PRESERVE_DayMode"								: self.__PRESERVE_DayMode_Update,
			"CloseRestartWindow"							: self.__RestartDialog_Close,
			"OpenPrivateShop"								: self.__PrivateShop_Open,
			"PartyHealReady"								: self.PartyHealReady,
			"ShowMeSafeboxPassword"							: self.AskSafeboxPassword,
			"CloseSafebox"									: self.CommandCloseSafebox,

			"CloseMall"										: self.CommandCloseMall,
			"ShowMeMallPassword"							: self.AskMallPassword,
			"item_mall"										: self.__ItemMall_Open,

			"RefineSuceeded"								: self.RefineSuceededMessage,
			"RefineFailed"									: self.RefineFailedMessage,
			"xmas_snow"										: self.__XMasSnow_Enable,
			"xmas_boom"										: self.__XMasBoom_Enable,
			"xmas_song"										: self.__XMasSong_Enable,
			"xmas_tree"										: self.__XMasTree_Enable,
			"newyear_boom"									: self.__XMasBoom_Enable,
			"PartyRequest"									: self.__PartyRequestQuestion,
			"PartyRequestDenied"							: self.__PartyRequestDenied,
			"horse_state"									: self.__Horse_UpdateState,
			"hide_horse_state"								: self.__Horse_HideState,
			"WarUC"											: self.__GuildWar_UpdateMemberCount,
			"test_server"									: self.__EnableTestServerFlag,
			"mall"											: self.__InGameShop_Show,

			"lover_login"									: self.__LoginLover,
			"lover_logout"									: self.__LogoutLover,
			"lover_near"									: self.__LoverNear,
			"lover_far"										: self.__LoverFar,
			"lover_divorce"									: self.__LoverDivorce,
			"PlayMusic"										: self.__PlayMusic,

			"searched_item"				                    : self.SitemFinder,
			"searched_item_count"		                    : self.SitemFinderCounter,

			"open_searched"	                                :self.interface.ShowItemFinder,

			"MyShopPriceList"								: self.__PrivateShop_PriceList,

			"getinputbegin"									: self.GetInputBegin,
			"getinputend"									: self.GetInputEnd,
			"getinput"										: self.GetInput,
		##if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
		#	"OpenWarpWindow"								: self.__OpenWarpWindow,
		##if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:end
		}

		if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
			serverCommandList["OpenWarpWindow"] = self.__OpenWarpWindow

		if app.ENABLE_TITLE_SYSTEM:
			serverCommandList["TitleSyncReset"] = self.__TitleSystemSyncReset
			serverCommandList["TitleSyncAdd"] = self.__TitleSystemSyncAdd
			serverCommandList["TitleSyncActive"] = self.__TitleSystemSyncActive

		if app.ENABLE_COLLECTIONS_SYSTEM:
			serverCommandList["RECV_Collection"] = self.__RecvCollection
			serverCommandList["RECV_CollectionItem"] = self.__RecvCollectionItem
			serverCommandList["RECV_CollectionBuild"] = self.__RecvCollectionBuild
			serverCommandList["RECV_CollectionRefresh"] = self.__RecvCollectionRefresh
			serverCommandList["RECV_CollectionIncrease"] = self.__RecvCollectionIncrease

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
			serverCommandList["clear_guildranking"] = self.__ClearGuildRanking
			serverCommandList["clear_applicant"] = self.__ClearApplicant
			serverCommandList["clear_applicantguild"] = self.__ClearApplicantGuild

		if app.ENABLE_REMOTE_SHOP_SYSTEM:
			serverCommandList["openremoteshop"] = self.OpenRemoteShop

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			serverCommandList["xmas_soul"] = self.__XMasSoul_Enable

		if app.ENABLE_STONE_EVENT:
			serverCommandList["IsStoneEvent"] = self.StoneEvent

		if app.ENABLE_SOCCER_BALL_EVENT:
			serverCommandList["FutbolEventGui"] = self.FutbolTopuVer

		if app.ENABLE_WORD_GAME_EVENT:
			serverCommandList["KelimeEventGui"] = self.WordGameWindowShow

		if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
			serverCommandList["halloween_rewards"] = self.SHalounRewards
			serverCommandList["level_halloween"] = self.SHalounLevel
			serverCommandList["points_halloween"] = self.SPointsHaloun
			serverCommandList["eveniment_haloun"] = self.SHalloweenStatus

		if app.ENABLE_SPIRIT_STONE_READING:
			serverCommandList["ruhtasiekranac"] = self.ruhcac

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			serverCommandList["guild_war"] = self.__Check_Guild_War

		if app.ENABLE_SKILL_BOOK_READING:
			serverCommandList["bkekranac"] = self.bkac

		if app.ENABLE_AUTO_SELL_SYSTEM:
			serverCommandList["addAutoSellInfo"] = self.interface.wndAutoSell.AddItem
			serverCommandList["autosell_status"] = self.interface.wndAutoSell.SetMod

		if app.ENABLE_COLLECT_WINDOW:
			serverCommandList.update({"UpdateTime" : self.UpdateTime})
			serverCommandList.update({"UpdateChance" : self.UpdateChance})

		if app.ENABLE_GAYA_SYSTEM:
			serverCommandList.update({"OpenGuiGem" : self.OpenGuiGem })
			serverCommandList.update({"GemCheck" : self.GemCheck })
			serverCommandList.update({"OpenGuiGemMarket" : self.OpenGuiGemMarket })
			serverCommandList.update({"GemMarketSlotsDesblock" : self.GemMarketSlotsDesblock })
			serverCommandList.update({"GemMarketItems" : self.GemMarketItems })
			serverCommandList.update({"GemMarketClear" : self.GemMarketClear })
			serverCommandList.update({"GemMarketTime" : self.GemTimeMarket })

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			serverCommandList.update({
				"SetBodyCostumeHidden"						: self.SetBodyCostumeHidden,
				"SetHairCostumeHidden"						: self.SetHairCostumeHidden,
			})

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				serverCommandList.update({
					"SetAcceCostumeHidden"					: self.SetAcceCostumeHidden,
				})

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				serverCommandList.update({
					"SetWeaponCostumeHidden"				: self.SetWeaponCostumeHidden,
				})

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				serverCommandList.update({
					"SetAuraCostumeHidden"					: self.SetAuraCostumeHidden,
				})

		if app.ENABLE_SPECIAL_INVENTORY:
			serverCommandList.update({
				"OpenSpecialInventoryWindow"				: self.OpenSpecialInventoryWindow,
				"CloseLoadSafeboxEvent"						: self.__CloseLoadSafeboxEvent,
				"CloseLoadMallEvent"						: self.__CloseLoadMallEvent,
			})

		if app.ENABLE_NEW_DUNGEON_LIB:
			serverCommandList.update({"RefreshDungeonFloor" : self.RefreshDungeonFloor })
			serverCommandList.update({"RefreshDungeonTimer" : self.RefreshDungeonTimer })

		if app.ENABLE_DUNGEON_INFO:
			serverCommandList.update({"getinputbegin" : self.__Inputget1 })
			serverCommandList.update({"DungeonDataClear" : self.DungeonDataClear })
			serverCommandList.update({"getinputend" : self.__Inputget2 })
			serverCommandList.update({"get_index" : self.GetDungeonIndex })
			serverCommandList.update({"dungeon_index" : self.SetDungeonQIndex })
			serverCommandList.update({"DungeonInfoData" : self.DungeonInfoData })
			serverCommandList.update({"DungeonBackData" : self.DungeonBackData })

		if app.ENABLE_ITEMSHOP:
			serverCommandList.update({"SetWheelItemData" : self.interface.SetWheelItemData})
			serverCommandList.update({"OnSetWhell" : self.interface.OnSetWhell})
			serverCommandList.update({"GetWheelGiftData" : self.interface.GetWheelGiftData})
			serverCommandList.update({"SetDragonCoin" : self.ItemShopSetDragonCoin})
			serverCommandList.update({"ItemShopAppendLog" : self.ItemShopAppendLogEx})

		if app.ENABLE_ANTI_EXP:
			serverCommandList.update({
				"SetAntiExp"								: self.SetAntiExp,
			})

		if app.ENABLE_BATTLE_PASS:
			serverCommandList.update({
				"missions_bp"					: self.SMissionsBP,
				"info_missions_bp"				: self.SInfoMissions,
				"size_missions_bp"				: self.SizeMissions,
				"rewards_missions_bp"			: self.SRewardsMissions,
				"rewards_missions_bonus_bp"		: self.SRewardsMissionsBonus,
				"final_reward"					: self.SFinalRewards,
				"show_battlepass"				: self.ShowBoardBpass,
				"battlepass_status"				: self.SBattlePass,
				"missions_bp_premium"				: self.SMissionsBPPremium,
				"info_missions_bp_premium"			: self.SInfoMissionsPremium,
				"size_missions_bp_premium"			: self.SizeMissionsPremium,
				"rewards_missions_bp_premium"		: self.SRewardsMissionsPremium,
				"rewards_missions_bonus_bp_premium"	: self.SRewardsMissionsBonusPremium,
				"final_reward_premium"				: self.SFinalRewardsPremium,
				"show_battlepass_premium"			: self.ShowBoardBpassPremium,
				"battlepass_status_premium"			: self.SBattlePassPremium,
			})

		if app.ENABLE_COLLECT_WINDOW:
			serverCommandList["SetCollectWindowQID"] = self.__SetCollectWindowQID
			serverCommandList["OpenCollectWindow"] = self.OpenCollectWindow

		if app.ENABLE_MULTI_FARM_BLOCK:
			serverCommandList.update({
				"UpdateMultiFarmAffect"						: self.UpdateMultiFarmAffect,
				"UpdateMultiFarmPlayer"						: self.UpdateMultiFarmPlayer,
			})

		if app.ENABLE_ZODIAC_MISSION:
			serverCommandList.update({"zodiac_index" : self.ZodiacLuaIndex})

		if app.ENABLE_TRACK_WINDOW:
			serverCommandList.update({"TrackDungeonInfo" : self.interface.TrackDungeonInfo})
			serverCommandList.update({"TrackBossInfo" : self.interface.TrackBossInfo})

		if app.ENABLE_ITEMSHOP:
			serverCommandList["open_ishop"] = self.BINARY_OpenItemShop

		if app.ENABLE_RENEWAL_OFFLINESHOP:
			serverCommandList.update({
				"OpenBackAllItem"							: self.OpenBackAllItem,
				"OpenOfflineShop"							: self.OpenOfflineShop,
				"RefreshOfflineShop"						: self.RefreshOfflineShop,
				"OpenOfflineShopPanel"						: self.OpenOfflineShopPanel,
				"ClearOfflineShopLog"						: self.ClearOfflineShopLog,
				"AppendShopLog"								: self.AppendShopLog,
				"OfflineShopSellMsg"						: self.interface.AddOfflineShopMessage,
			})

		if app.__DUNGEON_INFO__:
			serverCommandList.update({
				"dungeon_info_qid" : self.interface.DungeonInfoQuestIdx,
				"dungeon_info_cmd" : self.interface.DungeonInfoQuestCMD,
				"dungeon_info_cooldown" : self.interface.DungeonInfoCooldown,
				"dungeon_log_info" : self.interface.LoadServerData,
				})

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			serverCommandList.update({
				"OfflineShopBuyed"							: self.interface.OfflineShopBuyed,
			})

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			serverCommandList.update({
				"PickUPMode"								: self.__PickUPMode,
			})

		if app.ENABLE_MAINTENANCE_SYSTEM:
			serverCommandList.update({
				"BINARY_Update_Maintenance"					: self.BINARY_Update_Maintenance,
			})

		if app.ENABLE_NEW_DUNGEON_LIB:
			serverCommandList.update({
				"RefreshDungeonFloor"						: self.RefreshDungeonFloor,
				"RefreshDungeonTimer"						: self.RefreshDungeonTimer,
			})

		if app.ENABLE_FISH_GAME:
			serverCommandList.update({
				"OpenFishGameWindow"						: self.interface.OpenFishGameWindow,
				"SetFishGameGoal"							: self.interface.SetFishGameGoal,
				"CloseFishGame"								: self.interface.CloseFishGame,
			})

		if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
			serverCommandList.update({
				"ManagerInventoryUnlock"					: self.ManagerInventoryUnlock,
			})

		if app.ENABLE_DEFENSAWESHIP:
			serverCommandList.update({
				"gethydrahp"								: self.__HydraGetHp,
			})

		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			serverCommandList.update({
				"ShowChangeDialog"							: self.__ChangeWindowOpen,
			})

		if app.ENABLE_HUNTING_SYSTEM:
			serverCommandList.update({
				"HuntingButtonFlash"						: self.SetHuntingButtonFlash,
			})

		#PITTY_REFINE
		serverCommandList["RefinePitty"] = self.RecvRefinePitty
		serverCommandList["SkillPowerLeadership"] = self.SetSkillPowerLeadership

		if app.ENABLE_SUNG_MAHI_TOWER:
			serverCommandList["ClearSungMahiInfo"] = self.ClearSungMahiInfo
			serverCommandList["SetSungMahiQuest"] = self.SetSungMahiQuest
			serverCommandList["UpdateSungMahiInfo"] = self.UpdateSungMahiInfo
			serverCommandList["OpenSungMahiWindow"] = self.OpenSungMahiWindow
			serverCommandList["UpdateSungMahiNotice"] = self.UpdateSungMahiNotice
			serverCommandList["SungMahiClearNotice"] = self.SungMahiClearNotice
			serverCommandList["UpdateRoomLevel"] = self.UpdateRoomLevel
			serverCommandList["UpdateTowerLevel"] = self.UpdateTowerLevel
			serverCommandList["UpdateRoomTime"] = self.UpdateRoomTime

		#if app.ENABLE_ITEMSHOP:
		serverCommandList["RefreshAccountMoney"] = self.ItemShopSetDragonCoin

		if app.__AUTO_HUNT__:
			serverCommandList.update({"AutoHuntStatus" : self.interface.AutoHuntStatus})

		if app.ENABLE_BIOLOGIST_SYSTEM:
			serverCommandList["biyologlvl"] = self.BiyologLvBildirim
			serverCommandList["biyolog_update"] = self.__BiyologMission
			serverCommandList["biyolog_open"] = self.BiyologOpen

		if app.ENABLE_YIN_AND_YANG_ATTR:
			serverCommandList["OpenYinAndYangInventory"] = self.interface.OpenYinAndYangInventory
			serverCommandList["OpenYinAndYangHarmony"] = self.interface.OpenYinAndYangHarmony
			serverCommandList["ToggleYinAndYangInventory"] = self.interface.ToggleYinAndYangInventory
			serverCommandList["ToggleYinAndYangHarmony"] = self.interface.ToggleYinAndYangHarmony

		if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
			serverCommandList["ShowItemCombinationDialog"] = self.__ItemCombinationDialog
			serverCommandList["CloseItemCombinationDialog"] = self.__CloseItemCombinationDialog
			serverCommandList["ItemCombinationSuccess"] = self.__ItemCombinationSuccess

		self.serverCommander = stringCommander.Analyzer()
		for serverCommandItem in serverCommandList.items():
			self.serverCommander.SAFE_RegisterCallBack(serverCommandItem[0], serverCommandItem[1])

		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.serverCommander.SAFE_RegisterCallBack("RequestWarpToCharacter", self.RequestWarpToCharacter)

		if app.ENABLE_RENEWAL_SKILL_SELECT:
			self.serverCommander.SAFE_RegisterCallBack("RenewalSkillSelectWindow", self.__RenewalSkillSelectWindow)

		if app.ENABLE_GAYA_TICKET_SYSTEM:
			serverCommandList["OpenGemTicket"] = self.__OpenGemTicket

	if app.ENABLE_DUNGEON_INFO:
		def __Inputget1(self):
			constInfo.INPUT_IGNORE = 1 

		def __Inputget2(self):
			constInfo.INPUT_IGNORE = 0

		def SetDungeonQIndex(self,id):
			constInfo.dungeon_qf_index = int(id)

		def GetDungeonIndex(self):
			net.SendQuestInputStringPacket(constInfo.DungeonWarp)
			constInfo.DungeonWarp = ""

		def DungeonDataClear(self):
			constInfo.py_Flag.clear()

		def DungeonBackData(self, data):
			if len(data) > 0:
				first_list = data[:len(data)-1].split("#")
				for j in xrange(len(first_list)):
					second_list = first_list[j].split("|")
					if len(second_list) == 2:
						constInfo.SetFlag("%d_back"%int(second_list[0]),int(second_list[1])+app.GetGlobalTimeStamp())

		def DungeonInfoData(self, data):
			if len(data) > 0:
				first_list = data[:len(data)-1].split("#")
				for j in xrange(len(first_list)):
					second_list = first_list[j].split("|")
					if len(second_list) == 2:
						constInfo.SetFlag("%d_cooldown"%int(second_list[0]),int(second_list[1])+app.GetGlobalTimeStamp())

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def __Check_Guild_War(self, enable):
			if self.interface:
				if int(enable) == 1:
					self.interface.ShowGuildWarButton()
				else:
					self.interface.HideGuildWarButton()

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
		def __ClearGuildRanking(self):
			guild.ClearGuildRanking()

		def __ClearApplicant(self):
			guild.ClearApplicant()

		def __ClearApplicantGuild(self):
			guild.ClearApplicantGuild()

	if app.ENABLE_DUNGEON_INFO:
		def OpenTableDungeonInfo(self):
			if self.interface:
				self.interface.DUNGEON_INFO_CHECK_SHOW()

		def BINARY_TABLE_DUNGEON_INFO_OPEN(self):
			if self.interface:
				self.interface.BINARY_TABLE_DUNGEON_INFO_OPEN()

		def BINARY_TABLE_DUNGEON_RANKING_LOAD(self):
			if self.interface:
				self.interface.BINARY_TABLE_DUNGEON_RANKING_LOAD()

		def BINARY_TABLE_DUNGEON_MISION_LOAD(self):
			if self.interface:
				self.interface.BINARY_TABLE_DUNGEON_MISION_LOAD()

	if app.ENABLE_COLLECTIONS_SYSTEM:
		def __RecvCollection(self, collectionIdx, name, isComplete):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.AddCollection(collectionIdx, name, isComplete)

		def __RecvCollectionItem(self, collectionIdx, itemIdx, iVnum, iCount, myCount):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.AddItem(collectionIdx, itemIdx, iVnum, iCount, myCount)

		def __RecvCollectionBuild(self):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.Build()

		def __RecvCollectionRefresh(self, collectionIdx, itemIdx, myCount):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.UpdateValue(collectionIdx, itemIdx, myCount)

		def __RecvCollectionIncrease(self, isIncreased):
			if self.interface and self.interface.wndCollections:
				self.interface.wndCollections.SetIncrease(isIncreased)

	if app.ENABLE_RENEWAL_CUBE:
		def BINARY_CUBE_RENEWAL_OPEN(self):
			if self.interface:
				self.interface.BINARY_CUBE_RENEWAL_OPEN()

	def BINARY_BATTLEPASS_OPEN(self):
		if self.interface:
			self.interface.BINARY_BATTLEPASS_OPEN()

	def BINARY_BATTLEPASS_MP(self):
		if self.interface:
			message = "Congratulations! you have finished a battle pass mission."
			visible = True
			if app.ENABLE_ADVANCED_GAME_OPTIONS:
				import advanced_option_notifications
				visible = advanced_option_notifications.Record("battle_pass", message)
			if visible:
				chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, "BattlePass", message)
				self.interface.RecvWhisper("BattlePass")

	if app.ENABLE_GAYA_SYSTEM:
		def OnPickGem(self, gem):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_SYSTEM_PICK_GEM % (gem))

		def OpenGuiGem(self):
			self.interface.OpenGuiGem()

		def GemCheck(self):
			self.interface.GemCheck()

		def OpenGuiGemMarket(self):
			self.interface.OpenGuiGemMarket()

		def GemMarketItems(self, vnums, gem, count):
			self.interface.GemMarketItems(vnums, gem, count)

		def GemMarketSlotsDesblock(self, slot0, slot1, slot2, slot3, slot4, slot5):
			self.interface.GemMarketSlotsDesblock(slot0, slot1, slot2, slot3, slot4, slot5)

		def GemMarketClear(self):
			self.interface.GemMarketClear()

		def GemTimeMarket(self, time):
			self.interface.GemTime(time)

	def BINARY_ServerCommand_Run(self, line):
		#dbg.TraceError(line)
		try:
			#print " BINARY_ServerCommand_Run", line
			return self.serverCommander.Run(line)
		except AttributeError:
			return 0
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def __ProcessPreservedServerCommand(self):
		try:
			command = net.GetPreservedServerCommand()
			while command:
				print " __ProcessPreservedServerCommand", command
				self.serverCommander.Run(command)
				command = net.GetPreservedServerCommand()
		except AttributeError:
			return 0
		except RuntimeError, msg:
			dbg.TraceError(msg)
			return 0

	def GetInputBegin(self):
		constInfo.INPUT_IGNORE = 1

	def GetInputEnd(self):
		constInfo.INPUT_IGNORE = 0

	def GetInput(self):
		net.SendQuestInputStringPacket("1")

	def PartyHealReady(self):
		self.interface.PartyHealReady()

	if app.ENABLE_GUILD_RANK_SYSTEM:
		def BINARY_GUILD_RANK_OPEN(self):
			self.interface.OpenGuildRanking()

	def AskSafeboxPassword(self):
		self.interface.AskSafeboxPassword()

	#PITTY_REFINE
	def RecvRefinePitty(self, itemVnum, pittyValue, pittyMax):
		self.interface.dlgRefineNew.SetPittyInfo(int(itemVnum), int(pittyValue), int(pittyMax))

	def SetSkillPowerLeadership(self, power):
		constInfo.LEADERSHIP_POWER = float(power)

	# ITEM_MALL
	def AskMallPassword(self):
		self.interface.AskMallPassword()

	def __ItemMall_Open(self):
		self.interface.OpenItemMall();

	def CommandCloseMall(self):
		self.interface.CommandCloseMall()
	# END_OF_ITEM_MALL

	def RefineSuceededMessage(self):
		snd.PlaySound("sound/ui/make_soket.wav")
		if app.ENABLE_AUTO_REFINE:
			self.interface.CheckRefineDialog(FALSE)
			if constInfo.IS_AUTO_REFINE:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_SUCCESS)
			else:
				self.PopupMessage(localeInfo.REFINE_SUCCESS)
		else:
			self.PopupMessage(localeInfo.REFINE_SUCCESS)

	def RefineFailedMessage(self):
		snd.PlaySound("sound/ui/jaeryun_fail.wav")
		if app.ENABLE_AUTO_REFINE:
			self.interface.CheckRefineDialog(TRUE)
			if constInfo.IS_AUTO_REFINE:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE)
			else:
				self.PopupMessage(localeInfo.REFINE_FAILURE)
		else:
			self.PopupMessage(localeInfo.REFINE_FAILURE)

	def CommandCloseSafebox(self):
		self.interface.CommandCloseSafebox()

	# PRIVATE_SHOP_PRICE_LIST
	def __PrivateShop_PriceList(self, itemVNum, itemPrice):
		uiPrivateShopBuilder.SetPrivateShopItemPrice(itemVNum, itemPrice)
	# END_OF_PRIVATE_SHOP_PRICE_LIST

	def __Horse_HideState(self):
		self.affectShower.SetHorseState(0, 0, 0)

	def __Horse_UpdateState(self, level, health, battery):
		self.affectShower.SetHorseState(int(level), int(health), int(battery))

	def __IsXMasMap(self):
		mapDict = (
			"metin2_map_n_flame_01",
			"metin2_map_n_desert_01",
			"metin2_map_spiderdungeon_01",
			"metin2_map_spiderdungeon_02",
			"metin2_map_spiderdungeon_03",
			"metin2_map_deviltower1",
			"metin2_map_empirewar03",
			"metin2_map_devilsCatacomb",
			"metin2_map_Mt_Thunder",
			"metin2_map_n_flame_dungeon_01",
			"metin2_map_dawnmist_dungeon_01",
			"metin2_map_Mt_Th_dungeon_01",
		)

		if background.GetCurrentMapName() in mapDict:
			return FALSE

		return TRUE

	def __XMasSnow_Enable(self, mode):
		self.__XMasSong_Enable(mode)

		if "1" == mode:
			#if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			#	background.SetXMasShowEvent(1)

			if not self.__IsXMasMap():
				return

			print "XMAS_SNOW ON"
			background.EnableSnow(1)
		else:
			#if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			#	background.SetXMasShowEvent(0)

			print "XMAS_SNOW OFF"
			background.EnableSnow(0)

	def __XMasBoom_Enable(self, mode):
		if "1" == mode:
			if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
				#if not background.IsBoomMap():
				#	return

				if not self.__IsXMasMap():
					return
			else:
				if not self.__IsXMasMap():
					return

			print "XMAS_BOOM ON"
			self.__DayMode_Update("dark")
			self.enableXMasBoom = TRUE
			self.startTimeXMasBoom = app.GetTime()
		else:
			print "XMAS_BOOM OFF"
			self.__DayMode_Update("light")
			self.enableXMasBoom = FALSE

	def __XMasTree_Enable(self, grade):

		print "XMAS_TREE ", grade
		background.SetXMasTree(int(grade))

	def __XMasSong_Enable(self, mode):
		if "1"==mode:
			print "XMAS_SONG ON"

			XMAS_BGM = "xmas.mp3"

			if app.IsExistFile("BGM/" + XMAS_BGM)==1:
				if musicInfo.fieldMusic != "":
					snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic)

				musicInfo.fieldMusic=XMAS_BGM
				snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

		else:
			print "XMAS_SONG OFF"

			if musicInfo.fieldMusic != "":
				snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic)

			musicInfo.fieldMusic=musicInfo.METIN2THEMA
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

	if app.ENABLE_SOUL_ROULETTE_SYSTEM:
		def __XMasSoul_Enable(self, mode):
			if "1" == mode:
				if not self.__IsXMasMap():
					return
				print "XMAS_SOUL ON"
				self.__DayMode_Update("red")
				self.enableXMasSoul = True
				self.startTimeXMasSoul = app.GetTime()
			else:
				print "XMAS_SOUL OFF"
				self.__DayMode_Update("light")
				self.enableXMasSoul = False

	def __RestartDialog_Close(self):
		self.interface.CloseRestartDialog()

	def __Console_Enable(self):
		constInfo.CONSOLE_ENABLE = TRUE
		self.consoleEnable = TRUE
		app.EnableSpecialCameraMode()
		ui.EnablePaste(TRUE)

	## PrivateShop
	def __PrivateShop_Open(self):
		self.interface.OpenPrivateShopInputNameDialog()

	def BINARY_PrivateShop_Appear(self, vid, text):
		self.interface.AppearPrivateShop(vid, text)

	def BINARY_PrivateShop_Disappear(self, vid):
		self.interface.DisappearPrivateShop(vid)

	## DayMode
	def __PRESERVE_DayMode_Update(self, mode):
		if "light" == mode:
			background.SetEnvironmentData(0)
			import advanced_options_runtime
			advanced_options_runtime.ApplySky()
		elif "dark" == mode:
			if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
				if not background.IsBoomMap():
					return
			else:
				if not self.__IsXMasMap():
					return

			background.RegisterEnvironmentData(background.DAY_MODE_DARK, constInfo.ENVIRONMENT_NIGHT)
			background.SetEnvironmentData(background.DAY_MODE_DARK)

	def __DayMode_Update(self, mode):
		if "light" == mode:
			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)
		elif "dark" == mode:
			if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
				if not background.IsBoomMap():
					return
			else:
				if not self.__IsXMasMap():
					return

			self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)

		if app.ENABLE_SOUL_ROULETTE_SYSTEM:
			if "red" == mode:
				if not self.__IsXMasMap():
					return

				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToRed)

	def __DayMode_OnCompleteChangeToLight(self):
		background.SetEnvironmentData(0)
		import advanced_options_runtime
		advanced_options_runtime.ApplySky()
		self.curtain.FadeIn()

	def __DayMode_OnCompleteChangeToDark(self):
		background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
		background.SetEnvironmentData(1)

		self.curtain.FadeIn()

	if app.ENABLE_SOUL_ROULETTE_SYSTEM:
		def __DayMode_OnCompleteChangeToRed(self):
			background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_RED)
			background.SetEnvironmentData(1)

			self.curtain.FadeIn()

	## XMasBoom
	def __XMasBoom_Update(self):
		self.BOOM_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
		if self.indexXMasBoom >= len(self.BOOM_DATA_LIST):
			return

		boomTime = self.BOOM_DATA_LIST[self.indexXMasBoom][0]
		boomCount = self.BOOM_DATA_LIST[self.indexXMasBoom][1]

		if app.GetTime() - self.startTimeXMasBoom > boomTime:

			self.indexXMasBoom += 1

			for i in xrange(boomCount):
				self.__XMasBoom_Boom()

	def __XMasBoom_Boom(self):
		x, y, z = player.GetMainCharacterPosition()
		randX = app.GetRandom(-150, 150)
		randY = app.GetRandom(-150, 150)

		snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	if app.ENABLE_SOUL_ROULETTE_SYSTEM:
		def __XMasSoul_Update(self):
			self.SOUL_DATA_LIST = ( (2, 5), (5, 2), (7, 3), (10, 3), (20, 5) )
			if self.indexXMasSoul >= len(self.SOUL_DATA_LIST):
				return

			soulTime = self.SOUL_DATA_LIST[self.indexXMasSoul][0]
			soulCount = self.SOUL_DATA_LIST[self.indexXMasSoul][1]

			if app.GetTime() - self.startTimeXMasSoul > soulTime:

				self.indexXMasSoul += 1

				for i in xrange(soulCount):
					self.__XMasSoul_Soul()

		def __XMasSoul_Soul(self):
			x, y, z = player.GetMainCharacterPosition()
			randX = app.GetRandom(-150, 150)
			randY = app.GetRandom(-150, 150)

			snd.PlaySound3D(x+randX, -y+randY, z, "sound/common/etc/salute.mp3")

	def __PartyRequestQuestion(self, vid):
		vid = int(vid)
		partyRequestQuestionDialog = uiCommon.QuestionDialog("thin")
		partyRequestQuestionDialog.SetText(chr.GetNameByVID(vid) + localeInfo.PARTY_DO_YOU_ACCEPT)
		partyRequestQuestionDialog.SetAcceptText(localeInfo.UI_ACCEPT)
		partyRequestQuestionDialog.SetCancelText(localeInfo.UI_DENY)
		partyRequestQuestionDialog.SetAcceptEvent(lambda arg=TRUE: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.SetCancelEvent(lambda arg=FALSE: self.__AnswerPartyRequest(arg))
		partyRequestQuestionDialog.Open()
		partyRequestQuestionDialog.vid = vid
		self.partyRequestQuestionDialog = partyRequestQuestionDialog

	def __AnswerPartyRequest(self, answer):
		if not self.partyRequestQuestionDialog:
			return

		vid = self.partyRequestQuestionDialog.vid

		if answer:
			net.SendChatPacket("/party_request_accept " + str(vid))
		else:
			net.SendChatPacket("/party_request_deny " + str(vid))

		self.partyRequestQuestionDialog.Close()
		self.partyRequestQuestionDialog = None

	def __PartyRequestDenied(self):
		self.PopupMessage(localeInfo.PARTY_REQUEST_DENIED)

	def __EnableTestServerFlag(self):
		app.EnableTestServerFlag()

	def __InGameShop_Show(self, url):
		if constInfo.IN_GAME_SHOP_ENABLE:
			self.interface.OpenWebWindow(url)

	# WEDDING
	def __LoginLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLoginLover()

	def __LogoutLover(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.OnLogoutLover()
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverNear(self):
		if self.affectShower:
			self.affectShower.ShowLoverState()

	def __LoverFar(self):
		if self.affectShower:
			self.affectShower.HideLoverState()

	def __LoverDivorce(self):
		if self.interface.wndMessenger:
			self.interface.wndMessenger.ClearLoverInfo()
		if self.affectShower:
			self.affectShower.ClearLoverState()

	if app.ENABLE_CHANGE_CHANNEL:
		def __GetServerID(self):
			serverID = 1
			for i in serverInfo.REGION_DICT[0].keys():
				if serverInfo.REGION_DICT[0][i]["name"] == net.GetServerInfo().split(",")[0]:
					serverID = int(i)
					break

			return serverID

		def RefreshChannel(self, channel):
			channelName = ""
			serverName = serverInfo.REGION_DICT[0][self.__GetServerID()]["name"]
			if channel in serverInfo.REGION_DICT[0][self.__GetServerID()]["channel"]:
				channelName = serverInfo.REGION_DICT[0][self.__GetServerID()]["channel"][int(channel)]["name"]
			elif channel == 99:
				channelName = "Ortak CH"
			else:
				channelName = "Bilinmeyen CH"

			net.SetServerInfo("%s, %s" % (serverName,channelName))

			if self.interface:
				self.interface.wndMiniMap.serverInfo.SetText(net.GetServerInfo())

			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_CHANNEL_NOTICE % (channel))

	def __PlayMusic(self, flag, filename):
		flag = int(flag)
		if flag:
			snd.FadeOutAllMusic()
			musicInfo.SaveLastPlayFieldMusic()
			snd.FadeInMusic("BGM/" + filename)
		else:
			snd.FadeOutAllMusic()
			musicInfo.LoadLastPlayFieldMusic()
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)
	# END_OF_WEDDING

	def SkillClearCoolTime(self, slotIndex):
		self.interface.SkillClearCoolTime(slotIndex)

	def DeactivateSlot(self, slotindex, type):
		self.interface.DeactivateSlot(slotindex, type)

	def ActivateSlot(self, slotindex, type):
		self.interface.ActivateSlot(slotindex, type)

	if app.ENABLE_RESP_SYSTEM:
		def BINARY_SetMobRespData(self, mobVnum, data):
			self.interface.wndResp.SetMobRespData(mobVnum, data)

		def BINARY_SetMobDropData(self, mobVnum, data):
			self.interface.wndResp.SetMobDropData(mobVnum, data)

		def BINARY_SetMapData(self, data, currentBossCount, maxBossCount, currentMetinCount, maxMetinCount):
			self.interface.wndResp.SetMapData(data, currentBossCount, maxBossCount, currentMetinCount, maxMetinCount)

		def BINARY_RefreshResp(self, id, mobVnum, time, cord):
			self.interface.wndResp.RefreshRest(id, mobVnum, time, cord)

	def SendWeeklyPage(self, page, active, season):
		if active == True:
			if self.interface:
				self.interface.SelectPage(page, season)

	def SendWeeklyInfo(self, pos, name, points, empire, job):
		if self.interface:
			self.interface.SendWeeklyInfo(pos, name, points, empire, job)

	if app.ENABLE_RENEWAL_SWITCHBOT:
		def RefreshSwitchbotWindow(self):
			if self.interface:
				self.interface.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.interface:
				self.interface.RefreshSwitchbotItem(slot)

	if app.ENABLE_KILL_STATISTICS:
		def ReceiveKillStatisticsPacket(self, j, sh, ch, t, td, dw, dl, b, st, mb, top_damage):
			constInfo.KILL_STATISTICS_DATA = [int(j), int(sh), int(ch), int(t), int(td), int(dw), int(dl), int(b), int(st), int(mb), long(top_damage),]

	if app.ENABLE_TELEPORT_TO_A_FRIEND:
		def RequestWarpToCharacter(self, name):
			questionDialogWarp = uiCommon.QuestionDialog("thin")
			questionDialogWarp.SetText(localeInfo.ACCEPT_WARP_FROM_YOUR_FRIEND_REQUEST % (name))
			questionDialogWarp.SetAcceptEvent(lambda: self.WarpAcceptEvent(name))
			questionDialogWarp.SetCancelEvent(lambda: self.WarpCancelEvent())
			questionDialogWarp.Open()
			self.questionDialogWarp = questionDialogWarp
			return

		def WarpAcceptEvent(self, name):
			net.SendMessengerSummonByNamePacket(name)
			self.questionDialogWarp.Close()

		def WarpCancelEvent(self):
			self.questionDialogWarp.Close()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def SetBodyCostumeHidden(self, hidden):
			constInfo.HIDDEN_BODY_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		def SetHairCostumeHidden(self, hidden):
			constInfo.HIDDEN_HAIR_COSTUME = int(hidden)
			self.interface.RefreshVisibleCostume()

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			def SetAcceCostumeHidden(self, hidden):
				constInfo.HIDDEN_ACCE_COSTUME = int(hidden)
				self.interface.RefreshVisibleCostume()

		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			def SetWeaponCostumeHidden(self, hidden):
				constInfo.HIDDEN_WEAPON_COSTUME = int(hidden)
				self.interface.RefreshVisibleCostume()

		if app.ENABLE_AURA_COSTUME_SYSTEM:
			def SetAuraCostumeHidden(self, hidden):
				constInfo.HIDDEN_AURA_COSTUME = int(hidden)
				self.interface.RefreshVisibleCostume()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActAcce(self, iAct, bWindow):
			if self.interface:
				self.interface.ActAcce(iAct, bWindow)

		def AlertAcce(self, bWindow):
			snd.PlaySound("sound/ui/make_soket.wav")
			if bWindow:
				self.PopupMessage(localeInfo.ACCE_DEL_SERVEITEM)
			else:
				self.PopupMessage(localeInfo.ACCE_DEL_ABSORDITEM)

	if app.ENABLE_VIEW_CHEST_DROP:
		def BINARY_AddChestDropInfo(self, chestVnum, pageIndex, slotIndex, itemVnum, itemCount):
			if self.interface:
				self.interface.AddChestDropInfo(chestVnum, pageIndex, slotIndex, itemVnum, itemCount)

		def BINARY_RefreshChestDropInfo(self, chestVnum):
			if self.interface:
				self.interface.RefreshChestDropInfo(chestVnum)

	if app.ENABLE_SPECIAL_INVENTORY:
		def __CloseLoadSafeboxEvent(self):
			self.interface.CloseLoadSafeboxEvent()

		def __CloseLoadMallEvent(self):
			self.interface.CloseLoadMallEvent()

	if app.ENABLE_NEW_DUNGEON_LIB:
		def RefreshDungeonTimer(self, Floor,Time):
			if self.interface:
				if self.interface.wndMiniMap:
					self.interface.wndMiniMap.Hide()
				self.interface.MakeDungeonTimerWindow()
				if self.interface.wndDungeonTimer:
					self.interface.wndDungeonTimer.RefreshDungeonTimer(Time, Floor)

		def RefreshDungeonFloor(self, Floor2):
			if self.interface:
				if self.interface.wndMiniMap:
					self.interface.wndMiniMap.Hide()
				self.interface.MakeDungeonTimerWindow()
				if self.interface.wndDungeonTimer:
					self.interface.wndDungeonTimer.RefreshDungeonFloor(Floor2)

	if app.ENABLE_ITEMSHOP:
		def ItemShopClear(self, updateTime):
			uiItemShopNew.ItemShopClear(int(updateTime))

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopUpdateItem(self, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
			uiItemShopNew.ItemShopUpdateItem(int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount), int(itemMaxSellingCount), long(itemPriceJD))

			if self.interface:
				self.interface.ItemShopUpdateItem(int(itemID), int(itemMaxSellingCount))

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopAppendItem(self, categoryIndex, categorySubIndex, itemID, itemVnum, itemPrice, itemDiscount, itemOffertime, itemTopSelling, itemAddedTime, itemSellingCount, itemMaxSellingCount, itemPriceJD = 0):
			uiItemShopNew.ItemShopAppendItem(int(categoryIndex), int(categorySubIndex), int(itemID), int(itemVnum), long(itemPrice), int(itemDiscount), int(itemOffertime), int(itemTopSelling), int(itemAddedTime), long(itemSellingCount), int(itemMaxSellingCount), long(itemPriceJD))

		def ItemShopHideLoading(self):
			self.interface.ItemShopHideLoading()

		def ItemShopOpenMainPage(self):
			self.interface.OpenItemShopMainWindow()

		def ItemShopLogClear(self):
			uiItemShopNew.ItemShopLogClear()

		#USE_ITEMSHOP_RENEWED: @itemPriceJD
		def ItemShopAppendLog(self, dateText, dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice, itemPriceJD = 0):
			uiItemShopNew.ItemShopAppendLog(str(dateText), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))

		def ItemShopPurchasesWindow(self):
			self.interface.ItemShopPurchasesWindow()

		def ItemShopAppendLogEx(self, dateText, dateText2,dateTime, playerName, ipAdress, itemVnum, itemCount, itemPrice):
			uiItemShopNew.ItemShopAppendLog(str(dateText)+" "+str(dateText2), int(dateTime), str(playerName), str(ipAdress), int(itemVnum), int(itemCount), long(itemPrice))

		def BINARY_OpenItemShop(self):
			if self.interface:
				self.interface.OpenItemShopWindow()

	#USE_ITEMSHOP_RENEWED: @lldJCoins
	def ItemShopSetDragonCoin(self, lldCoins, lldJCoins = 0):
		if app.ENABLE_ITEMSHOP and self.interface:
			self.interface.ItemShopSetDragonCoin(long(lldCoins), long(lldJCoins))

	if app.ENABLE_ANTI_EXP:
		def SetAntiExp(self, flag):
			flag = int(flag)
			if self.interface:
				ptr = [self.interface.wndTaskBar]
				for gui in ptr:
					if gui:
						if flag:
							guiptr = gui.GetChild("AntiExpButton")
							guiptr.SetUpVisual("d:/ymir work/ui/game/taskbar/antiexp_button_01.tga")
							guiptr.SetOverVisual("d:/ymir work/ui/game/taskbar/antiexp_button_02.tga")
							guiptr.SetDownVisual("d:/ymir work/ui/game/taskbar/antiexp_button_03.tga")

							if gui.IsChild("RestExp_Value"):
								gui.GetChild("Exp_Value").Hide()
							if gui.IsChild("Exp_Value"):
								gui.GetChild("RestExp_Value").Hide()

							for j in range(1, 5):
								if gui.IsChild("EXPGauge_0%d" % j):
									gui.GetChild("EXPGauge_0%d" % j).SetDiffuseColor(104.0 / 255.0, 104.0 / 255.0, 104.0 / 255.0, 1)

						else:
							guiptr = gui.GetChild("AntiExpButton")
							guiptr.SetUpVisual("d:/ymir work/ui/game/taskbar/antiexp_button_04.tga")
							guiptr.SetOverVisual("d:/ymir work/ui/game/taskbar/antiexp_button_05.tga")
							guiptr.SetDownVisual("d:/ymir work/ui/game/taskbar/antiexp_button_06.tga")

							if gui.IsChild("RestExp_Value"):
								gui.GetChild("Exp_Value").Show()
							if gui.IsChild("Exp_Value"):
								gui.GetChild("RestExp_Value").Show()

							for j in range(1, 5):
								if gui.IsChild("EXPGauge_0%d" % j):
									gui.GetChild("EXPGauge_0%d" % j).SetDiffuseColor(201, 160, 51, 1)

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
		def RefreshGuildRankingList(self, issearch):
			if self.interface:
				self.interface.RefreshGuildRankingList(issearch)

	if app.ENABLE_RENEWAL_SPECIAL_CHAT:
		def OnPickItem(self, item):
			item_vnum = int(item)
			if self.interface:
				self.interface.SpecialChatOnPick(0, item_vnum)

	if app.ENABLE_MULTI_FARM_BLOCK:
		def UpdateMultiFarmPlayer(self, multiFarmPlayer):
			self.affectShower.SetMultiFarmPlayer(str(multiFarmPlayer))

		def UpdateMultiFarmAffect(self, multiFarmStatus, isNewStatus):
			self.affectShower.SetMultiFarmInfo(int(multiFarmStatus))

			if int(isNewStatus) == 1:
				if int(multiFarmStatus) == 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_ACTIVE_CHAT)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MULTI_FARM_DEACTIVE_CHAT)

			app.SetMultiFarmExeIcon(int(multiFarmStatus))

	def ShowNameWindow(self, arg):
		if self.interface:
			self.interface.ShowNameReinforce(int(arg))

	def RecvReadWiki(self):
		if constInfo.ALREADY_SENT == 0:
			chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, "[WÝKÝ]", localeInfo.READ_WIKI_TEXT)
			self.interface.RecvWhisper("[WÝKÝ]")
			constInfo.ALREADY_SENT = 1

	if app.ENABLE_RENEWAL_OFFLINESHOP:
		def StartOfflineShop(self, vid, isOwner):
			if self.interface:
				self.interface.OpenOfflineShopDialog(vid, isOwner)

		def StartOfflineShopPanel(self):
			if self.interface:
				self.interface.OpenOfflineShopPanel()

		def ClearOfflineShopLog(self):
			if self.interface:
				if self.interface.dlgOfflineShopPanel:
					if self.interface.dlgOfflineShopPanel.GetChild("ListBox"):
						self.interface.dlgOfflineShopPanel.GetChild("ListBox").RemoveAllItems()

		def AppendShopLog(self, name, date, itemvnum, itemcount, price):
			self.interface.AppendLogOfflineShopPanel(name, date, itemvnum, itemcount, price)

		def AppendShopLogFirst(self, name, date, itemvnum, itemcount, price):
			self.interface.AppendLogOfflineShopPanelFirst(str(name), str(date), int(itemvnum), int(itemcount), int(price))

		def EndOfflineShop(self):
			if self.interface:
				if self.interface.dlgOfflineShop:
					if self.interface.dlgOfflineShop.IsShow():
						self.interface.dlgOfflineShop.Close()

				if self.interface.dlgOfflineShopPanel:
					if self.interface.dlgOfflineShopPanel.IsShow():
						self.interface.dlgOfflineShopPanel.Close()

				if self.interface.offlineShopBuilder:
					if self.interface.offlineShopBuilder.IsShow():
						self.interface.offlineShopBuilder.Close()

		def RefreshOfflineShop(self):
			if self.interface:
				if self.interface.dlgOfflineShop:
					self.interface.dlgOfflineShop.Refresh()

				if self.interface.dlgOfflineShopPanel:
					if self.interface.dlgOfflineShopPanel.IsShow():
						self.interface.dlgOfflineShopPanel.Refresh()

				if self.interface.offlineShopBuilder:
					if self.interface.offlineShopBuilder.IsShow():
						self.interface.offlineShopBuilder.Refresh()

		def OpenOfflineShop(self):
			if self.interface:
				self.interface.OpenOfflineShopBuilder()

		def OpenBackAllItem(self):
			backItem = uiCommon.QuestionDialog("thin")
			backItem.SetText(localeInfo.OFFLINESHOP_DO_YOU_WANT_BACK)
			backItem.SetAcceptText(localeInfo.UI_ACCEPT)
			backItem.SetCancelText(localeInfo.UI_DENY)
			backItem.SetAcceptEvent(lambda arg=TRUE: self.__AnswerGetBackItems(arg))
			backItem.SetCancelEvent(lambda arg=FALSE: self.__AnswerGetBackItems(arg))
			backItem.Open()
			self.partyRequestQuestionDialog=backItem

		def __AnswerGetBackItems(self, answer):
			if not self.partyRequestQuestionDialog:
				return

			if answer:
				net.SendGetBackItems()

			self.partyRequestQuestionDialog.Close()
			self.partyRequestQuestionDialog = None

		def OpenOfflineShopPanel(self):
			if self.interface:
				self.interface.OpenOfflineShopPanel()

		def BINARY_OfflineShop_Appear(self, vid, text):
			if (chr.GetInstanceType(vid) == chr.INSTANCE_TYPE_NPC):
				if self.interface:
					self.interface.AppearOfflineShop(vid, text)

		def BINARY_OfflineShop_Disappear(self, vid):
			if (chr.GetInstanceType(vid) == chr.INSTANCE_TYPE_NPC):
				if self.interface:
					self.interface.DisappearOfflineShop(vid)

	if app.ENABLE_GAYA_TICKET_SYSTEM:
		def __OpenGemTicket(self, itemPos):
			self.interface.OpenGemTicket(int(itemPos))

	if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
		def OpenPrivateShopSearch(self, type):
			if self.interface:
				self.interface.OpenPrivateShopSearch(type)

		def RefreshShopSearch(self):
			if self.interface:
				self.interface.RefreshShopSearch()

		def BuyShopSearch(self):
			self.interface.RefreshShopSearch()

		def BuyShopSearch2(self):
			if self.interface:
				if self.interface.wndPrivateShopSearch.IsShow():
					self.interface.wndPrivateShopSearch.StartSearch()

	def BINARY_RANK_APPEND(self, mode, my_pos, pos, name, value, empire, level, guildname):
		if self.interface:
			self.interface.AppendInfoRankGlobal(mode, my_pos, pos, name, value, empire, level, guildname)

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		def __PickUPMode(self, mode):
			constInfo.PICKUPMODE = int(mode)
			self.interface.OnChangePickUPMode()

	if app.ENABLE_MAINTENANCE_SYSTEM:
		def BINARY_Update_Maintenance(self, iTime, iDuration, iReason):
			sTime = int(iTime)
			sDuration = int(iDuration)
			sReason = str(iReason)

			if sTime != 0 and sDuration != 0:
				self.wndMaintenance.OpenMaintenance(int(iTime), int(iDuration), str(iReason))

	if app.ENABLE_RENEWAL_SKILL_SELECT: 
		def __RenewalSkillSelectWindow(self, job):
			self.interface.OpenSkillSelectWindow(job)

	if app.ENABLE_NEW_DUNGEON_LIB:
		def RefreshDungeonTimer(self, Floor, Time):
			if self.interface:
				if self.interface.wndMiniMap:
					self.interface.wndMiniMap.Hide()
				self.interface.MakeDungeonTimerWindow()
				if self.interface.wndDungeonTimer:
					self.interface.wndDungeonTimer.RefreshDungeonTimer(Time, Floor)

		def RefreshDungeonFloor(self, Floor2):
			if self.interface:
				if self.interface.wndMiniMap:
					self.interface.wndMiniMap.Hide()
				self.interface.MakeDungeonTimerWindow()
				if self.interface.wndDungeonTimer:
					self.interface.wndDungeonTimer.RefreshDungeonFloor(Floor2)

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def AddExchangeItemSlotIndex(self, idx):
			self.interface.AddExchangeItemSlotIndex(idx)

	if app.ENABLE_AURA_COSTUME_SYSTEM:
		def AuraWindowOpen(self, type):
			self.interface.AuraWindowOpen(type)

		def AuraWindowClose(self):
			self.interface.AuraWindowClose()

	def BINARY_BattlePassInit(self):
		self.interface.wndBattlePass.RefreshGlobal()

	def BINARY_BattlePassUpdate(self):
		self.interface.wndBattlePass.RefreshLocal()

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def PetHatchingWindowCommand(self, command):
			self.interface.PetHatchingWindowCommand(command)

		def PetNameChangeWindowCommand(self, command):
			self.interface.PetNameChangeWindowCommand(command)

		def PetSkillUpgradeDlgOpen(self, slot, index, gold):
			self.interface.PetSkillUpgradeDlgOpen(slot, index, gold)

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def PetFlashEvent(self, index):
			self.interface.PetFlashEvent(index)

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def PetAffectShowerRefresh(self):
			self.interface.PetAffectShowerRefresh()

		def PetFeedReuslt(self, result):
			self.interface.PetFeedReuslt(result)

		def PetAttrDetermineResult(self, byType):
			self.interface.PetAttrDetermineResult(byType)

		def PetAttrChangeResult(self, slotIndex, byType):
			self.interface.PetAttrChangeResult(slotIndex, byType)

		def PetReviveResult(self, result):
			self.interface.PetReviveResult(result)

		def PetWindowTypeResult(self, result):
			self.interface.PetWindowTypeResult(result)

	if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
		def ManagerInventoryUnlock(self, cmd):
			cmd = cmd.split("|")
			if cmd[0] == "Reload":
				net.SendChatPacket("/reload_inventory")
			elif cmd[0] == "Hide":
				constInfo.Inventory_Locked["Keys_Can_Unlock_%d" % (int(cmd[2]))] = int(cmd[1])
			elif cmd[0] == "Message":
				dbg.LogBox("You need "+str(cmd[1])+" items to unlock.")

	if app.ENABLE_DEFENSAWESHIP:
		def __ShipMastHPShow(self):
			if self.wndShipMastHP:
				self.wndShipMastHP.Open(10000000, 10000000)

		def __HydraGetHp(self, curPoint):
			if self.wndShipMastHP:
				new = int(curPoint)
				self.wndShipMastHP.SetShipMastHP(new, 10000000)

	if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
		def BINARY_OpenWarpWindow(self):
			if self.interface:
				self.interface.OpenWarpWindow()

	if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
		def __OpenWarpWindow(self):
			if app.ENABLE_RENEWAL_TELEPORT_SYSTEM:
				if self.interface:
					self.interface.OpenWarpWindow()

	if app.ENABLE_RESP_SYSTEM:
		def BINARY_OpenRespWindow(self):
			if self.interface:
				self.interface.OpenRespWindow()

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def __ChangeWindowOpen(self, type):
			self.interface.ChangeWindowOpen(type)

	if app.ENABLE_HUNTING_SYSTEM:
		def BINARY_OpenHuntingWindowMain(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.interface:
				self.interface.OpenHuntingWindowMain(level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)

		def BINARY_OpenHuntingWindowSelect(self, level, type, monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.interface:
				self.interface.OpenHuntingWindowSelect(level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)

		def BINARY_OpenHuntingWindowReward(self, level, reward, reward_count, random_reward, random_reward_count, money, exp):
			if self.interface:
				self.interface.OpenHuntingWindowReward(level, reward, reward_count, random_reward, random_reward_count, money, exp)

		def BINARY_UpdateHuntingMission(self, count):
			if self.interface:
				self.interface.UpdateHuntingMission(count)

		def BINARY_HuntingReciveRandomItem(self, window, item_vnum, item_count):
			if self.interface:
				if window == 0:
					self.interface.HuntingSetRandomItemsMain(item_vnum, item_count)
				if window == 1:
					self.interface.HuntingSetRandomItemsSelect(item_vnum, item_count)

		def SetHuntingButtonFlash(self):
			if constInfo.HUNTING_MAIN_UI_SHOW == 0:
				constInfo.HUNTING_BUTTON_FLASH = 1

	if app.ENABLE_RIDING_EXTENDED:
		def OpenMountUpGrade(self):
			if self.interface:
				self.interface.MountUpGradeWindow()

	if app.ENABLE_BIOLOGIST_SYSTEM:
		def __BiyologMission(self, mission, missionState, givenCount, needCount, remainingTime):
			if self.interface:
				self.interface.SetBiyologMission(mission, missionState, givenCount, needCount, remainingTime)

		def BiyologOpen(self):
			if self.interface:
				self.interface.OpenBiyologDialog()

		def BiyologLvBildirim(self, veri):
			self.bildirim = uiCommon.Bildirim()
			self.bildirim.SetUserName(str(veri) + localeInfo.BIYOLOG_BILDIRIM)
			self.bildirim.SlideIn()

	if app.ENABLE_ZODIAC_MISSION:
		def ZodiacLuaIndex(self,p):
			constInfo.ZodiacLua = int(p)

		def ZodiacLuaIndex(self,p):
			constInfo.ZodiacLua = int(p)

		def BINARY_SetMissionMessage(self, message):
			if self.interface.missionBoard:
				self.interface.missionBoard.SetMission(message)

		def ZodiacJumpButtonShow(self):
			if self.interface:
				self.interface.wnd12ziTimer.Show12ziJumpButton()

		def ZodiacJumpButtonClose(self):
			if self.interface:
				self.interface.wnd12ziTimer.CloseJumpButton()

		def Refresh12ziTimer(self, Time, Nextfloor, Floor):
			if self.interface:
				self.interface.wnd12ziTimer.Refresh12ziTimer(Time, Nextfloor, Floor)
				self.interface.wnd12ziTimer.Show()

		def ZodiacDayorNight(self):
			if self.interface:
				self.interface.wnd12ziTimer.DayorNigh12zi()

		def OpenUI12zi(self,yellomark,greenmark,yellowreward,greenreward):
			if self.interface:
				self.interface.wnd12ziReward.Open(yellomark,greenmark,yellowreward,greenreward)

		def OpenZodiac(self):
			if self.interface:
				self.interface.wnd12ziReward.Show()
		def SetBeadCount(self, count):
			if self.interface and getattr(self.interface.wndMiniMap, "beadInfo", None):
				self.interface.wndMiniMap.beadInfo.SetText("%d"%count)
				self.interface.wndMiniMap.beadInfo.Show()

		def NextBeadUpdateTime(self, value):
			if self.interface:
				self.interface.wndMiniMap.beadTime = int(value)

		def OpenReviveDialog(self,count):
			constInfo.RevivePrismaCount = int(count)

		def OpenReviveDialog_Me(self,count_me):
			constInfo.RevivePrismaCount_Me = int(count_me)

	if app.ENABLE_REMOTE_SHOP_SYSTEM:
		def OpenRemoteShop(self):
			self.remoteshop.Show()

	def SitemFinder(self, i, item, mob, i_vnum, count, prob, actives, mob_vnum):
		mob = str(mob).replace("_", " ")
		item = str(item).replace("_", " ")
		self.interface.AppendInfoFinder(int(i), str(mob), int(prob), int(actives), int(i_vnum), int(count), str(item))
		constInfo.finder_items[int(i)]={"iMobVnum":mob_vnum}

	def SitemFinderCounter(self, count):
		constInfo.finder_counts = int(count)

	if app.ENABLE_COLLECT_WINDOW:
		def BINARY_UpdateCollectWindow(self, windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredlevel):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.AddData(windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredlevel)
				
		def UpdateTime(self, val, time):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.SendTime(val, time)

		def UpdateChance(self, val, chance):
			if self.interface.wndCollectWindow:
				self.interface.wndCollectWindow.SendChance(val, chance)

		def __SetCollectWindowQID(self, window, value):
			constInfo.CollectWindowQID[int(window)] = int(value)
			# self.interface.ToggleQuest(str(value))

		def OpenCollectWindow(self):
			self.interface.ToggleCollectWindow()

	if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
		def SHalounRewards(self, index, item_vnum, item_count):
			constInfo.haloun_rewards[int(index)]={"iVnum":int(item_vnum),"iCount":int(item_count)}

		def SHalounLevel(self, nivel):
			constInfo.haloun_lvl = nivel

		def SHalloweenStatus(self, status):
			constInfo.IsHaloun = int(status)

		def SPointsHaloun(self, points):
			constInfo.haloun_points = points

	if app.ENABLE_MINI_GAME_CATCH_KING:
		def MiniGameCatchKingEvent(self, isEnable):
			if self.interface:
				self.interface.SetCatchKingEventStatus(isEnable)

		def MiniGameCatchKingEventStart(self, bigScore):
			self.interface.MiniGameCatchKingEventStart(bigScore)

		def MiniGameCatchKingSetHandCard(self, cardNumber):
			self.interface.MiniGameCatchKingSetHandCard(cardNumber)

		def MiniGameCatchKingResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
			self.interface.MiniGameCatchKingResultField(score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

		def MiniGameCatchKingSetEndCard(self, cardPos, cardNumber):
			self.interface.MiniGameCatchKingSetEndCard(cardPos, cardNumber)

		def MiniGameCatchKingReward(self, rewardCode):
			self.interface.MiniGameCatchKingReward(rewardCode)

	if app.ENABLE_ATTENDANCE_EVENT:
		def MiniGameAttendanceEvent(self, isEnable):
			if self.interface:
				self.interface.SetAttendanceEventStatus(isEnable)

		def MiniGameAttendanceSetData(self, type, value):
			self.interface.MiniGameAttendanceSetData(type, value)
			
		def RefreshHitCount(self, vid):
			if vid == self.targetBoard.GetTargetVID():
				self.targetBoard.RefreshHitCount(vid)

	if app.ENABLE_FISH_EVENT_SYSTEM:
		def MiniGameFishEvent(self, isEnable, lasUseCount):
			if self.interface:
				self.interface.SetFishEventStatus(isEnable)
				self.interface.MiniGameFishCount(lasUseCount)

		def MiniGameFishUse(self, shape, useCount):
			self.interface.MiniGameFishUse(shape, useCount)
			
		def MiniGameFishAdd(self, pos, shape):
			self.interface.MiniGameFishAdd(pos, shape)
			
		def MiniGameFishReward(self, vnum):
			self.interface.MiniGameFishReward(vnum)	

	if app.ENABLE_SOUL_ROULETTE_SYSTEM:
		def BINARY_ROULETTE_OPEN(self, price, soul):
			if self.interface:
				self.interface.Roulette_Open(price, soul)

		def BINARY_ROULETTE_CLOSE(self):
			if self.interface:
				self.interface.Roulette_Close()

		def BINARY_ROULETTE_TURN(self, spin, idx):
			if self.interface:
				self.interface.Roulette_TurnWheel(spin, idx)

		def BINARY_ROULETTE_ICON(self, idx, vnum, count):
			if self.interface:
				self.interface.Roulette_SetIcons(idx, vnum, count)

	if app.ENABLE_HALLOWEEN_EVENT_SYSTEM:
		def ShowHaloun(self):
			self.interface.ShowHaloun()

	if app.ENABLE_EVENT_SYSTEM:
		def OnRecvEventInformation(self):
			if self.interface:
				self.interface.RefreshEventWindowDialog()

	if app.ENABLE_BATTLE_PASS:
		def SMissionsBP(self, i, type, vnum, counts, ep):
			constInfo.missions_bp[int(i)]={"iType":type, "iVnum":vnum, "iCount":counts, "iEp":ep}

		def SInfoMissions(self, i, counts, status, nume):
			nume = str(nume).replace("#", " ")
			constInfo.info_missions_bp[int(i)]={"iCounts":counts, "iStatus":status, "Name":nume}

		def SRewardsMissions(self, i, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.rewards_bp[int(i)]={"iVnum1":vnum1, "iVnum2":vnum2, "iVnum3":vnum3,"iCount1":count1, "iCount2":count2, "iCount3":count3}

		def SRewardsMissionsBonus(self, i, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.rewards_bonus_bp[int(i)]={"iVnum1":vnum1, "iVnum2":vnum2, "iVnum3":vnum3,"iCount1":count1, "iCount2":count2, "iCount3":count3}

		def SizeMissions(self, size):
			constInfo.size_battle_pass = int(size)

		def SBattlePass(self, status):
			constInfo.status_battle_pass = int(status)

		def SFinalRewards(self, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.final_rewards = [int(vnum1),int(vnum2),int(vnum3),int(count1),int(count2),int(count3)]

		def ShowBoardBpass(self):
			self.interface.ShowBoardBpass()

		def SMissionsBPPremium(self, i, type, vnum, counts, ep):
			constInfo.missions_bp_premium[int(i)]={"iType":type, "iVnum":vnum, "iCount":counts, "iEp":ep}

		def SInfoMissionsPremium(self, i, counts, status, nume):
			nume = str(nume).replace("#", " ")
			constInfo.info_missions_bp_premium[int(i)]={"iCounts":counts, "iStatus":status, "Name":nume}

		def SRewardsMissionsPremium(self, i, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.rewards_bp_premium[int(i)]={"iVnum1":vnum1, "iVnum2":vnum2, "iVnum3":vnum3,"iCount1":count1, "iCount2":count2, "iCount3":count3}

		def SRewardsMissionsBonusPremium(self, i, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.rewards_bonus_bp_premium[int(i)]={"iVnum1":vnum1, "iVnum2":vnum2, "iVnum3":vnum3,"iCount1":count1, "iCount2":count2, "iCount3":count3}

		def SizeMissionsPremium(self, size):
			constInfo.size_battle_pass_premium = int(size)

		def SBattlePassPremium(self, status):
			constInfo.status_battle_pass_premium = int(status)

		def SFinalRewardsPremium(self, vnum1, vnum2, vnum3, count1, count2, count3):
			constInfo.final_rewards_premium = [int(vnum1),int(vnum2),int(vnum3),int(count1),int(count2),int(count3)]

		def ShowBoardBpassPremium(self):
			self.interface.ShowBoardBpassPremium()
	if app.ENABLE_MOVE_COSTUME_ACCE_ATTR:
		def __ItemCombinationDialog(self):
			if self.interface:
				self.interface.ItemCombinationDialogOpen()

		def __CloseItemCombinationDialog(self):
			if self.interface:
				self.interface.ItemCombinationDialogClose()

		def __ItemCombinationSuccess(self):
			import snd
			snd.PlaySound("sound/ui/make_soket.wav")

