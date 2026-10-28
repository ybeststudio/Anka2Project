if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import snd
import systemSetting
import chat
import localeInfo
import constInfo
import interfaceModule
import musicInfo
import background
import uiCommon
import uiSelectMusic
import uiPhaseCurtain
import uiPrivateShopBuilder

from _weakref import proxy

if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	import sys
	import uiScriptLocale

	def ReloadModule(name):
		if name in sys.modules:
			del sys.modules[name]

			if name in locals():
				del locals()[name]

			if name in globals():
				del globals()[name]

			module = __import__(name)

			if not module in locals():
				locals()[name] = module

			if not module in globals():
				globals()[name] = module

	LOCALE_LANG_DICT = {
		app.LOCALE_CZ : { "name" : uiScriptLocale.LANGUAGE_CZ, "locale" : "cz", "code_page" : 65001 },
		app.LOCALE_DE : { "name" : uiScriptLocale.LANGUAGE_DE, "locale" : "de", "code_page" : 65001 },
		app.LOCALE_EN : { "name" : uiScriptLocale.LANGUAGE_EN, "locale" : "en", "code_page" : 65001 },
		app.LOCALE_ES : { "name" : uiScriptLocale.LANGUAGE_ES, "locale" : "es", "code_page" : 65001 },
		app.LOCALE_FR : { "name" : uiScriptLocale.LANGUAGE_FR, "locale" : "fr", "code_page" : 65001 },
		app.LOCALE_HU : { "name" : uiScriptLocale.LANGUAGE_HU, "locale" : "hu", "code_page" : 65001 },
		app.LOCALE_IT : { "name" : uiScriptLocale.LANGUAGE_IT, "locale" : "it", "code_page" : 65001 },
		app.LOCALE_PL : { "name" : uiScriptLocale.LANGUAGE_PL, "locale" : "pl", "code_page" : 65001 },
		app.LOCALE_PT : { "name" : uiScriptLocale.LANGUAGE_PT, "locale" : "pt", "code_page" : 65001 },
		app.LOCALE_RO : { "name" : uiScriptLocale.LANGUAGE_RO, "locale" : "ro", "code_page" : 65001 },
		app.LOCALE_TR : { "name" : uiScriptLocale.LANGUAGE_TR, "locale" : "tr", "code_page" : 65001 },
	}

MUSIC_FILENAME_MAX_LEN = 25

blockMode = 0
viewChatMode = 0

if app.ENABLE_GRAPHIC_ON_OFF:
	GRAPHIC_LEVEL_MAX_NUM = 5

class OptionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__LoadWindow()

		self.RefreshViewChat()
		self.RefreshAlwaysShowName()
		self.RefreshShowDamage()
		self.RefreshShowSalesText()

		if app.ENABLE_FOG_FIX:
			self.RefreshFogMode()

		if app.ENABLE_GRAPHIC_ON_OFF:
			self.RefreshPetStatus()
			self.RefreshNpcNameStatus()

		if app.ENABLE_ENB_MODE:
			self.RefreshENBModeStatus()
			self.questionDialog = None

		if app.ENABLE_SHOW_MOB_INFO:
			self.RefreshShowMobInfo()

		if app.ENABLE_RENEWAL_TEXT_SHADOW:
			self.RefreshOutline()

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			self.RefreshPickUP()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.__CreateLanguageSelectWindow()

		self.curtain = uiPhaseCurtain.PhaseCurtain()
		self.curtain.speed = 0.03
		self.curtain.Hide()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.titleBar = []

		self.gameOptionButton = []
		self.gameOptionBackground = []
		self.gamecharButton = []
		self.gameOptions = []
		self.gameSoundButton = []
		self.graphicsButton = []
		self.graphicsBg = []

		self.changeMusicButton = 0
		self.selectMusicFile = 0
		self.ctrlMusicVolume = 0
		self.ctrlSoundVolume = 0
		self.musicListDlg = 0

		self.cameraModeButtonList = []
		self.fogModeButtonList = []
		self.nameColorModeButtonList = []
		self.viewTargetBoardButtonList = []
		self.pvpModeButtonDict = {}
		self.blockButtonList = []
		self.viewChatButtonList = []
		self.alwaysShowNameButtonList = []
		self.showDamageButtonList = []
		self.showsalesTextButtonList = []

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			self.nightModeButtonList = []
			self.snowModeButtonList = []

		if app.ENABLE_GRAPHIC_ON_OFF:
			self.effectLevel = 0
			self.effectLevelApplyButton = 0
			self.effectLevelButtonList = []

			self.privateShopLevel = 0
			self.privateShopLevelApplyButton = 0
			self.privateShopLevelButtonList = []

			self.dropItemLevel = 0
			self.dropItemLevelApplyButton = 0
			self.dropItemLevelButtonList = []

			self.petStatusButtonList = []
			self.npcNameStatusButtonList = []

		if app.ENABLE_ENB_MODE:
			self.enbModeStatusButtonList = []

		if app.ENABLE_SHOW_MOB_INFO:
			self.showMobInfoButtonList = []

		if app.ENABLE_FOV_OPTION:
			self.fovController = None
			self.fovResetButton = None
			self.fovValueText = None

		if app.ENABLE_RENEWAL_TEXT_SHADOW:
			self.showOutlineButtonList = []

		if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
			self.mainPickModeButtonList = []
			self.mainPickUPButtonList = []

		if app.ENABLE_STONE_SCALE_OPTION:
			self.stoneScaleController = None
			self.stoneScaleResetButton = None

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.language_change_window = None
			self.language_select_button = None
			self.language_change_button = None

			self.cur_language_text = None
			self.cur_language_text_window = None

			self.language_button_dict = {}
			self.language_select_list_open = False
			self.language_select_window_height = 0

			self.language_select_window_bar = None

			self.mouse_over_image = None

			self.selected_language = 0

	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()

	def __LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/gameoptiondialog.py")
		except:
			import exception
			exception.Abort("OptionDialog.__LoadWindow.LoadObject")

		try:
			GetObject = self.GetChild

			self.titleBar.append(GetObject("titlebar"))

			self.selectMusicFile = GetObject("bgm_file")
			self.changeMusicButton = GetObject("bgm_button")
			self.ctrlMusicVolume = GetObject("music_volume_controller")
			self.ctrlSoundVolume = GetObject("sound_volume_controller")

			options_button = ["category_game_option", "category_graphics_option", "category_sound_option"]
			self.gameOptionButton.extend([GetObject(button_status) for button_status in options_button])

			options_background = ["background_game", "background_sound", "background_graphics"]
			self.gameOptionBackground.extend([GetObject(button_status) for button_status in options_background])

			game_options = ["char_opt_board", "ui_opt_board", "ambient_opt_board", "pickup_opt_board"]
			self.gameOptions.extend([GetObject(button_status) for button_status in game_options])

			self.gamecharButton = [GetObject("character{}".format(i)) for i in range(4)]

			self.gameSoundButton.append(GetObject("general_button_sound"))

			self.graphicsButton.append(GetObject("display_settings_graphics"))
			self.graphicsBg.append(GetObject("display_settings_option_board"))

			camera_mode = ["camera_short", "camera_long"]
			self.cameraModeButtonList.extend([GetObject(button_status) for button_status in camera_mode])

			if app.ENABLE_FOG_FIX:
				fog_mode = ["fog_on", "fog_off"]
				self.fogModeButtonList.extend([GetObject(button_status) for button_status in fog_mode])
			else:
				fog_mode = ["fog_level0", "fog_level1", "fog_level2"]
				self.fogModeButtonList.extend([GetObject(button_status) for button_status in fog_mode])

			if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
				night_mode = ["night_mode_off", "night_mode_on"]
				self.nightModeButtonList.extend([GetObject(button_status) for button_status in night_mode])

				snow_mode = ["snow_mode_off", "snow_mode_on"]
				self.snowModeButtonList.extend([GetObject(button_status) for button_status in snow_mode])

			if app.ENABLE_GRAPHIC_ON_OFF:
				for i in xrange(1, GRAPHIC_LEVEL_MAX_NUM + 1):
					self.effectLevelButtonList.append(GetObject("effect_level%d" % i))
					self.privateShopLevelButtonList.append(GetObject("privateShop_level%d" % i))
					self.dropItemLevelButtonList.append(GetObject("dropItem_level%d" % i))

				self.effectLevelApplyButton = GetObject("effect_apply")
				self.privateShopLevelApplyButton = GetObject("privateShop_apply")
				self.dropItemLevelApplyButton = GetObject("dropItem_apply")

				pet_name = ["pet_on", "pet_off"]
				self.petStatusButtonList.extend([GetObject(button_status) for button_status in pet_name])

				npc_name = ["npcName_on", "npcName_off"]
				self.npcNameStatusButtonList.extend([GetObject(button_status) for button_status in npc_name])

			if app.ENABLE_ENB_MODE:
				enb_mode = ["enbMode_on", "enbMode_off"]
				self.enbModeStatusButtonList.extend([GetObject(button_status) for button_status in enb_mode])

			if app.ENABLE_SHOW_MOB_INFO:
				mob_info = ["show_mob_level_button", "show_mob_AI_flag_button"]
				self.showMobInfoButtonList.extend([GetObject(button_status) for button_status in mob_info])

			if app.ENABLE_FOV_OPTION:
				self.fovController = GetObject("fov_controller")
				self.fovResetButton = GetObject("fov_reset_button")
				self.fovValueText = GetObject("fov_value_text")

			if app.ENABLE_RENEWAL_TEXT_SHADOW:
				name_outline = ["name_outline_on", "name_outline_off"]
				self.showOutlineButtonList.extend([GetObject(button_status) for button_status in name_outline])

			if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
				pickup_filter_child_names = [
					"pick_up_weapons",
					"pick_up_armors",
					"pick_up_shield",
					"pick_up_helmets",
					"pick_up_bracelets",
					"pick_up_necklace",
					"pick_up_earrings",
					"pick_up_shoes",
					"pick_up_others",
					"pick_up_yang",
					"pick_up_chests"
				]

				i = 1
				for names in pickup_filter_child_names:
					objectPointer = GetObject(names)
					objectPointer.SetToggleUpEvent(lambda type = 1 << i, ignore = FALSE : self.__OnClickPickupIgnoreButton(type, ignore))
					objectPointer.SetToggleDownEvent(lambda type = 1 << i, ignore = TRUE : self.__OnClickPickupIgnoreButton(type, ignore))
					self.mainPickUPButtonList.append(objectPointer)
					i += 1

				pick_up_button_activate = GetObject("pick_up_button_activate")
				pick_up_button_activate.SetToggleUpEvent(self.__OnClickPickupModeButton)
				pick_up_button_activate.SetToggleDownEvent(self.__OnClickPickupModeButton)
				self.mainPickModeButtonList.append(pick_up_button_activate)

				pick_up_button_deactivate = GetObject("pick_up_button_deactivate")
				pick_up_button_deactivate.SetToggleUpEvent(self.__OnClickPickupModeButton)
				pick_up_button_deactivate.SetToggleDownEvent(self.__OnClickPickupModeButton)
				self.mainPickModeButtonList.append(pick_up_button_deactivate)

			if app.ENABLE_STONE_SCALE_OPTION:
				self.stoneScaleController = GetObject("stone_scale_controller")
				self.stoneScaleResetButton = GetObject("stone_scale_reset_button")

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.language_select_button = self.GetChild("language_select_button")
				self.language_change_window = self.GetChild("language_change_window")
				self.cur_language_text = self.GetChild("cur_language_text")
				self.cur_language_text_window = self.GetChild("cur_language_text_window")
				self.language_change_button = self.GetChild("language_change_button")

			name_color = ["name_color_normal", "name_color_empire"]
			self.nameColorModeButtonList.extend([GetObject(button_status) for button_status in name_color])

			target_board = ["target_board_no_view", "target_board_view"]
			self.viewTargetBoardButtonList.extend([GetObject(button_status) for button_status in target_board])

			pvp_modes = {
				player.PK_MODE_PEACE: "pvp_peace",
				player.PK_MODE_REVENGE: "pvp_revenge",
				player.PK_MODE_GUILD: "pvp_guild",
				player.PK_MODE_FREE: "pvp_free"
			}

			self.pvpModeButtonDict = {mode: GetObject(button) for mode, button in pvp_modes.items()}

			block_buttons = [
				"block_exchange_button",
				"block_party_button",
				"block_guild_button",
				"block_whisper_button",
				"block_friend_button",
				"block_party_request_button",
			]

			if app.ENABLE_TELEPORT_TO_A_FRIEND:
				block_buttons.append("block_warp_request_button")

			self.blockButtonList.extend(GetObject(block_button) for block_button in block_buttons)

			view_chat = ["view_chat_on_button", "view_chat_off_button"]
			self.viewChatButtonList.extend([GetObject(button_status) for button_status in view_chat])

			show_name = ["always_show_name_on_button", "always_show_name_off_button"]
			self.alwaysShowNameButtonList.extend([GetObject(button_status) for button_status in show_name])

			show_damage = ["show_damage_on_button", "show_damage_off_button"]
			self.showDamageButtonList.extend([GetObject(button_status) for button_status in show_damage])

			sales_text = ["salestext_on_button", "salestext_off_button"]
			self.showsalesTextButtonList.extend([GetObject(button_status) for button_status in sales_text])

		except:
			import exception
			exception.Abort("OptionDialog.__LoadWindow.LoadObject")

		self.SetCenterPosition()

		self.titleBar[0].SetCloseEvent(ui.__mem_func__(self.Close))

		board_options = [self.__SwitchGameOption, self.__SwitchGraphicsOption, self.__SwitchSoundOption]
		for i, button in enumerate(self.gameOptionButton):
			button.SAFE_SetEvent(board_options[i])

		for button in [self.gameOptionButton[0], self.gamecharButton[0]]:
			button.Down()

		game_buttons = [self.__ShowCharOptions, self.__ShowUiOptions, self.__ShowAmbientOptions, self.__ShowPickupOptions]
		for i, button in enumerate(self.gamecharButton):
			if i < len(game_buttons):
				button.SAFE_SetEvent(game_buttons[i])

		for button in [self.gameSoundButton[0], self.graphicsButton[0]]:
			button.Down()

		for i, option in enumerate(self.gameOptions):
			if i == 0:
				option.Show()
			else:
				option.Hide()

		self.graphicsBg[0].Show()

		for i, background in enumerate(self.gameOptionBackground):
			if i == 0:
				background.Show()
			else:
				background.Hide()

		self.ctrlMusicVolume.SetSliderPos(float(systemSetting.GetMusicVolume()))
		self.ctrlMusicVolume.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))

		self.ctrlSoundVolume.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
		self.ctrlSoundVolume.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))

		self.changeMusicButton.SAFE_SetEvent(self.__OnClickChangeMusicButton)

		self.cameraModeButtonList[0].SAFE_SetEvent(self.__OnClickCameraModeShortButton)
		self.cameraModeButtonList[1].SAFE_SetEvent(self.__OnClickCameraModeLongButton)

		if app.ENABLE_FOG_FIX:
			self.fogModeButtonList[0].SAFE_SetEvent(self.__OnClickFogModeOn)
			self.fogModeButtonList[1].SAFE_SetEvent(self.__OnClickFogModeOff)
		else:
			self.fogModeButtonList[0].SAFE_SetEvent(self.__OnClickFogModeLevel0Button)
			self.fogModeButtonList[1].SAFE_SetEvent(self.__OnClickFogModeLevel1Button)
			self.fogModeButtonList[2].SAFE_SetEvent(self.__OnClickFogModeLevel2Button)

		if app.ENABLE_GRAPHIC_ON_OFF:
			self.__ClickRadioButton(self.effectLevelButtonList, systemSetting.GetEffectLevel())
			self.__ClickRadioButton(self.privateShopLevelButtonList, systemSetting.GetPrivateShopLevel())
			self.__ClickRadioButton(self.dropItemLevelButtonList, systemSetting.GetDropItemLevel())
			self.__ClickRadioButton(self.petStatusButtonList, systemSetting.IsPetStatus())
			self.__ClickRadioButton(self.npcNameStatusButtonList, systemSetting.IsNpcNameStatus())

		if app.ENABLE_ENB_MODE:
			self.enbModeStatusButtonList[0].SAFE_SetEvent(self.__OnClickENBModeStatusButton, 1)
			self.enbModeStatusButtonList[1].SAFE_SetEvent(self.__OnClickENBModeStatusButton, 0)

		if not app.ENABLE_FOG_FIX:
			self.__ClickRadioButton(self.fogModeButtonList, constInfo.GET_FOG_LEVEL_INDEX())

		self.__ClickRadioButton(self.cameraModeButtonList, constInfo.GET_CAMERA_MAX_DISTANCE_INDEX())

		if musicInfo.fieldMusic == musicInfo.METIN2THEMA:
			self.selectMusicFile.SetText(uiSelectMusic.DEFAULT_THEMA)
		else:
			self.selectMusicFile.SetText(musicInfo.fieldMusic[:MUSIC_FILENAME_MAX_LEN])

		if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
			self.nightModeButtonList[0].SAFE_SetEvent(self.__OnClickNightModeOffButton)
			self.nightModeButtonList[1].SAFE_SetEvent(self.__OnClickNightModeOnButton)
			self.__InitNightModeOption()

			self.snowModeButtonList[0].SAFE_SetEvent(self.__OnClickSnowModeOffButton)
			self.snowModeButtonList[1].SAFE_SetEvent(self.__OnClickSnowModeOnButton)
			self.__InitSnowModeOption()

		if app.ENABLE_GRAPHIC_ON_OFF:
			for i in xrange(GRAPHIC_LEVEL_MAX_NUM):
				self.effectLevelButtonList[i].SAFE_SetEvent(self.__OnClickEffectLevelButton, i)
				self.privateShopLevelButtonList[i].SAFE_SetEvent(self.__OnClickPrivateShopLevelButton, i)
				self.dropItemLevelButtonList[i].SAFE_SetEvent(self.__OnClickDropItemLevelButton, i)

			self.petStatusButtonList[0].SAFE_SetEvent(self.__OnClickPetStatusButton, 0)
			self.petStatusButtonList[1].SAFE_SetEvent(self.__OnClickPetStatusButton, 1)

			self.npcNameStatusButtonList[0].SAFE_SetEvent(self.__OnClickNpcNameStatusButton, 0)
			self.npcNameStatusButtonList[1].SAFE_SetEvent(self.__OnClickNpcNameStatusButton, 1)

			self.effectLevelApplyButton.SAFE_SetEvent(self.__OnClickEffectApplyButton)
			self.privateShopLevelApplyButton.SAFE_SetEvent(self.__OnClickPrivateShopApplyButton)
			self.dropItemLevelApplyButton.SAFE_SetEvent(self.__OnClickDropItemApplyButton)

		if app.ENABLE_ENB_MODE:
			self.__ClickRadioButton(self.enbModeStatusButtonList, systemSetting.IsENBModeStatus())

		if app.ENABLE_SHOW_MOB_INFO:
			self.showMobInfoButtonList[0].SetToggleUpEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[1].SetToggleUpEvent(self.__OnClickShowMobAIFlagButton)

			self.showMobInfoButtonList[0].SetToggleDownEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[1].SetToggleDownEvent(self.__OnClickShowMobAIFlagButton)

		if app.ENABLE_FOV_OPTION:
			if self.fovController:
				self.fovController.SetSliderPos(float(systemSetting.GetFOV()) / float(app.MAX_CAMERA_PERSPECTIVE))
				self.fovController.SetEvent(ui.__mem_func__(self.__OnChangeFOV))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

			if self.fovResetButton:
				self.fovResetButton.SetEvent(ui.__mem_func__(self.__OnClickFOVResetButton))

		if app.ENABLE_RENEWAL_TEXT_SHADOW:
			self.showOutlineButtonList[0].SAFE_SetEvent(self.__OnClickShowOutlineButton)
			self.showOutlineButtonList[1].SAFE_SetEvent(self.__OnClickShowOutlineButton)

		if app.ENABLE_STONE_SCALE_OPTION:
			if self.stoneScaleController:
				self.stoneScaleController.SetSliderPos(float(systemSetting.GetStoneScale()))
				self.stoneScaleController.SetEvent(ui.__mem_func__(self.__OnChangeStoneScale))

			if self.stoneScaleResetButton:
				self.stoneScaleResetButton.SetEvent(ui.__mem_func__(self.__OnClickStoneScaleResetButton))

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			if self.language_select_button:
				self.language_select_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))
				self.cur_language_text_window.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))

			if self.language_change_button:
				self.language_change_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageChangeButton))

			self.mouse_over_image = self.GetChild("mouse_over_image")
			self.mouse_over_image.Hide()

		self.nameColorModeButtonList[0].SAFE_SetEvent(self.__OnClickNameColorModeNormalButton)
		self.nameColorModeButtonList[1].SAFE_SetEvent(self.__OnClickNameColorModeEmpireButton)

		self.viewTargetBoardButtonList[0].SAFE_SetEvent(self.__OnClickTargetBoardViewButton)
		self.viewTargetBoardButtonList[1].SAFE_SetEvent(self.__OnClickTargetBoardNoViewButton)

		self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
		self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
		self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
		self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)

		self.blockButtonList[0].SetToggleUpEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleUpEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleUpEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleUpEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleUpEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleUpEvent(self.__OnClickBlockPartyRequest)
		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.blockButtonList[6].SetToggleUpEvent(self.__OnClickBlockWarpRequest)

		self.blockButtonList[0].SetToggleDownEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleDownEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleDownEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleDownEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleDownEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleDownEvent(self.__OnClickBlockPartyRequest)
		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.blockButtonList[6].SetToggleDownEvent(self.__OnClickBlockWarpRequest)

		self.viewChatButtonList[0].SAFE_SetEvent(self.__OnClickViewChatOnButton)
		self.viewChatButtonList[1].SAFE_SetEvent(self.__OnClickViewChatOffButton)

		self.alwaysShowNameButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowNameOnButton)
		self.alwaysShowNameButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowNameOffButton)

		self.showDamageButtonList[0].SAFE_SetEvent(self.__OnClickShowDamageOnButton)
		self.showDamageButtonList[1].SAFE_SetEvent(self.__OnClickShowDamageOffButton)

		self.showsalesTextButtonList[0].SAFE_SetEvent(self.__OnClickSalesTextOnButton)
		self.showsalesTextButtonList[1].SAFE_SetEvent(self.__OnClickSalesTextOffButton)

		self.__ClickRadioButton(self.nameColorModeButtonList, constInfo.GET_CHRNAME_COLOR_INDEX())
		self.__ClickRadioButton(self.viewTargetBoardButtonList, constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD())
		self.__SetPeacePKMode()

	def __ShowDisplaySettings(self):
		if self.graphicsBg[0].IsShow():
			return

	def __ShowCharOptions(self):
		if not self.gameOptions[0].IsShow():
			self.gameOptions[1].Hide()
			self.gameOptions[2].Hide()
			self.gameOptions[3].Hide()
			self.gameOptions[0].Show()
			self.UpdateGameOpt()

	def __ShowUiOptions(self):
		for i in range(len(self.gameOptions)):
			if i == 1:
				self.gameOptions[i].Show()
			else:
				self.gameOptions[i].Hide()

		self.UpdateGameOpt()

	def __ShowAmbientOptions(self):
		for i in range(len(self.gameOptions)):
			if i == 2:
				self.gameOptions[i].Show()
			else:
				self.gameOptions[i].Hide()

		self.UpdateGameOpt()

	def __ShowPickupOptions(self):
		for i in range(len(self.gameOptions)):
			if i == 3:
				self.gameOptions[i].Show()
			else:
				self.gameOptions[i].Hide()

		self.UpdateGameOpt()

	def __SwitchGameOption(self):
		if not self.gameOptionBackground[0].IsShow():
			self.gameOptionBackground[1].Hide()
			self.gameOptionBackground[2].Hide()
			self.gameOptionBackground[0].Show()
			self.UpdateOptionButton()

	def __SwitchGraphicsOption(self):
		if not self.gameOptionBackground[2].IsShow():
			self.gameOptionBackground[0].Hide()
			self.gameOptionBackground[1].Hide()
			self.gameOptionBackground[2].Show()
			self.UpdateOptionButton()

	def __SwitchSoundOption(self):
		if not self.gameOptionBackground[1].IsShow():
			self.gameOptionBackground[0].Hide()
			self.gameOptionBackground[2].Hide()
			self.gameOptionBackground[1].Show()

			self.UpdateOptionButton()

	def UpdateGameOpt(self):
		for i in range(len(self.gameOptions)):
			if self.gameOptions[i].IsShow():
				self.gamecharButton[i].Down()
			else:
				self.gamecharButton[i].SetUp()

	def UpdateOptionButton(self):
		button_order = [
			(0, [2, 1, 0]),
			(1, [0, 1, 2]),
			(2, [0, 2, 1]),
			(None, [2, 1, 0])
		]

		for background_index, button_indices in button_order:
			if background_index is None or self.gameOptionBackground[background_index].IsShow():
				for button_index in button_indices:
					self.gameOptionButton[button_index].SetUp() if button_index != button_indices[2] else self.gameOptionButton[button_index].Down()
				break

	def __OnClickChangeMusicButton(self):
		if not self.musicListDlg:
			self.musicListDlg=uiSelectMusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)

		self.musicListDlg.Open()

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton = buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def __SetCameraMode(self, index):
		constInfo.SET_CAMERA_MAX_DISTANCE_INDEX(index)
		self.__ClickRadioButton(self.cameraModeButtonList, index)

	def __SetFogLevel(self, index):
		constInfo.SET_FOG_LEVEL_INDEX(index)
		self.__ClickRadioButton(self.fogModeButtonList, index)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	if app.ENABLE_FOG_FIX:
		def __OnClickFogModeOn(self):
			systemSetting.SetFogMode(TRUE)
			background.SetFogLevel(1)
			self.RefreshFogMode()

		def __OnClickFogModeOff(self):
			systemSetting.SetFogMode(FALSE)
			background.SetFogLevel(0)
			self.RefreshFogMode()

		def RefreshFogMode(self):
			if systemSetting.IsFogMode():
				self.fogModeButtonList[0].Down()
				self.fogModeButtonList[1].SetUp()
			else:
				self.fogModeButtonList[0].SetUp()
				self.fogModeButtonList[1].Down()

	if app.ENABLE_ENVIRONMENT_EFFECT_OPTION:
		def __InitNightModeOption(self):
			self.NightModeOn = systemSetting.GetNightModeOption()
			self.__ClickRadioButton(self.nightModeButtonList, self.NightModeOn)
			background.SetNightModeOption(self.NightModeOn)

		def __InitSnowModeOption(self):
			SnowModeOn = systemSetting.GetSnowModeOption()
			if SnowModeOn:
				self.__ClickRadioButton(self.snowModeButtonList, 1)
			else:
				self.__ClickRadioButton(self.snowModeButtonList, 0)

		def __OnClickNightModeOffButton(self):
			self.__ClickRadioButton(self.nightModeButtonList, 0)
			self.__SetNightMode(0)

		def __OnClickNightModeOnButton(self):
			self.__ClickRadioButton(self.nightModeButtonList, 1)
			self.__SetNightMode(1)

		def __OnClickSnowModeOffButton(self):
			systemSetting.SetSnowModeOption(FALSE)
			self.__ClickRadioButton(self.snowModeButtonList, 0)

		def __OnClickSnowModeOnButton(self):
			systemSetting.SetSnowModeOption(TRUE)
			self.__ClickRadioButton(self.snowModeButtonList, 1)

		def __SetNightMode(self, index):
			systemSetting.SetNightModeOption(index)	
			background.SetNightModeOption(index)

			if not background.GetDayMode():
				return

			if not background.IsBoomMap():
				return

			if 1 == index:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToDark)
			else:
				self.curtain.SAFE_FadeOut(self.__DayMode_OnCompleteChangeToLight)

		def __DayMode_OnCompleteChangeToLight(self):
			background.ChangeEnvironmentData(0)
			self.curtain.FadeIn()

		def __DayMode_OnCompleteChangeToDark(self):
			background.RegisterEnvironmentData(1, constInfo.ENVIRONMENT_NIGHT)
			background.ChangeEnvironmentData(1)
			self.curtain.FadeIn()

	if app.ENABLE_GRAPHIC_ON_OFF:
		def __OnClickEffectLevelButton(self, index):
			self.__ClickRadioButton(self.effectLevelButtonList, index)
			self.effectLevel = index

		def __OnClickEffectApplyButton(self):
			systemSetting.SetEffectLevel(self.effectLevel)

		def __OnClickPrivateShopLevelButton(self, privateShopLevelIdx):
			self.__ClickRadioButton(self.privateShopLevelButtonList, privateShopLevelIdx)
			self.privateShopLevel = privateShopLevelIdx

		def __OnClickPrivateShopApplyButton(self):
			systemSetting.SetPrivateShopLevel(self.privateShopLevel)

		def __OnClickDropItemLevelButton(self, index):
			self.__ClickRadioButton(self.dropItemLevelButtonList, index)
			self.dropItemLevel = index

		def __OnClickDropItemApplyButton(self):
			systemSetting.SetDropItemLevel(self.dropItemLevel)

		def __OnClickPetStatusButton(self, flag):
			self.__ClickRadioButton(self.petStatusButtonList, flag)
			systemSetting.SetPetStatusFlag(flag)
			self.RefreshPetStatus()

		def RefreshPetStatus(self):
			if systemSetting.IsPetStatus():
				self.petStatusButtonList[0].SetUp()
				self.petStatusButtonList[1].Down()
			else:
				self.petStatusButtonList[0].Down()
				self.petStatusButtonList[1].SetUp()

		def __OnClickNpcNameStatusButton(self, flag):
			self.__ClickRadioButton(self.npcNameStatusButtonList, flag)
			systemSetting.SetNpcNameStatusFlag(flag)
			self.RefreshNpcNameStatus()

		def RefreshNpcNameStatus(self):
			if systemSetting.IsNpcNameStatus():
				self.npcNameStatusButtonList[0].SetUp()
				self.npcNameStatusButtonList[1].Down()
			else:
				self.npcNameStatusButtonList[0].Down()
				self.npcNameStatusButtonList[1].SetUp()

	if app.ENABLE_ENB_MODE:
		def __OnClickENBModeStatusButton(self, flag):
			self.__ClickRadioButton(self.enbModeStatusButtonList, flag)
			self.ConfirmENBSelect(flag)
			self.RefreshENBModeStatus()

		def RefreshENBModeStatus(self):
			if systemSetting.IsENBModeStatus():
				self.enbModeStatusButtonList[1].SetUp()
				self.enbModeStatusButtonList[0].Down()
			else:
				self.enbModeStatusButtonList[1].Down()
				self.enbModeStatusButtonList[0].SetUp()

		def ConfirmENBSelect(self, flag):
			questionDialog = uiCommon.QuestionDialog2()
			questionDialog.SetText1(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_1)
			questionDialog.SetText2(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_2)
			questionDialog.SetAcceptEvent(lambda arg = flag: self.OnAcceptENBQuestionDialog(arg))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			questionDialog.SetWidth(450)
			questionDialog.Open()
			self.questionDialog = questionDialog

		def OnAcceptENBQuestionDialog(self, flag):
			self.OnCloseQuestionDialog()

			systemSetting.SetENBModeStatusFlag(flag)

			if flag == 1:
				value = 1
			else:
				value = 0

			idxCurMode = "EnableProxyLibrary=%d" % (value)
			f = []
			getLine = 2

			import os
			if os.path.exists("enbconvertor.ini"):
				idx = open("enbconvertor.ini", "r")

				for it in idx:
					f.append(it)

				while len(f) < int(getLine):
					f.append("")

				f[int(getLine)-1] = str(idxCurMode)
				idx = open("enbconvertor.ini", "w")

				for it in f:
					idx.write(it)
					if (len(it) > 0 and it[-1:] != "\n") or len(it) == 0:
						idx.write("\n")
			else:
				return

			app.Exit()

	if app.ENABLE_SHOW_MOB_INFO:
		def __OnClickShowMobLevelButton(self):
			if systemSetting.IsShowMobLevel():
				systemSetting.SetShowMobLevel(FALSE)
			else:
				systemSetting.SetShowMobLevel(TRUE)

			self.RefreshShowMobInfo()

		def __OnClickShowMobAIFlagButton(self):
			if systemSetting.IsShowMobAIFlag():
				systemSetting.SetShowMobAIFlag(FALSE)
			else:
				systemSetting.SetShowMobAIFlag(TRUE)

			self.RefreshShowMobInfo()

	if app.ENABLE_FOV_OPTION:
		def __OnChangeFOV(self):
			pos = self.fovController.GetSliderPos()
			systemSetting.SetFOV(pos * float(app.MAX_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

		def __OnClickFOVResetButton(self):
			self.fovController.SetSliderPos(float(app.DEFAULT_CAMERA_PERSPECTIVE) / float(app.MAX_CAMERA_PERSPECTIVE))
			systemSetting.SetFOV(float(app.DEFAULT_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

	if app.ENABLE_RENEWAL_TEXT_SHADOW:
		def __OnClickShowOutlineButton(self):
			systemSetting.SetShowOutlineFlag(not systemSetting.IsShowOutline())
			self.RefreshOutline()

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		def __SendPickupModeCommand(self):
			net.SendChatPacket("/setpickupmode {}".format(constInfo.PICKUPMODE))

		def __OnClickPickupModeButton(self):
			if 0 != (constInfo.PICKUPMODE & (1 << 0)):
				constInfo.PICKUPMODE -= (1 << 0)
			else:
				constInfo.PICKUPMODE += (1 << 0)

			self.__SendPickupModeCommand()
			self.RefreshPickUP()

		def __OnClickPickupIgnoreButton(self, type, ignore):
			if ignore:
				constInfo.PICKUPMODE += type
			else:
				constInfo.PICKUPMODE -= type

			self.__SendPickupModeCommand()
			self.RefreshPickUP()

	if app.ENABLE_STONE_SCALE_OPTION:
		def __OnChangeStoneScale(self):
			value = self.stoneScaleController.GetSliderPos()
			systemSetting.SetStoneScale(value)

		def __OnClickStoneScaleResetButton(self):
			self.stoneScaleController.SetSliderPos(0)
			systemSetting.SetStoneScale(0)

	def __OnClickFogModeLevel0Button(self):
		self.__SetFogLevel(0)

	def __OnClickFogModeLevel1Button(self):
		self.__SetFogLevel(1)

	def __OnClickFogModeLevel2Button(self):
		self.__SetFogLevel(2)

	def __OnChangeMusic(self, fileName):
		self.selectMusicFile.SetText(fileName[:MUSIC_FILENAME_MAX_LEN])

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)

		if fileName==uiSelectMusic.DEFAULT_THEMA:
			musicInfo.fieldMusic=musicInfo.METIN2THEMA
		else:
			musicInfo.fieldMusic=fileName

		musicInfo.SaveLastPlayFieldMusic()

		if musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

	def OnChangeMusicVolume(self):
		pos = self.ctrlMusicVolume.GetSliderPos()
		snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
		systemSetting.SetMusicVolume(pos)

	def OnChangeSoundVolume(self):
		pos = self.ctrlSoundVolume.GetSliderPos()
		snd.SetSoundVolumef(pos)
		systemSetting.SetSoundVolumef(pos)

	def __SetNameColorMode(self, index):
		constInfo.SET_CHRNAME_COLOR_INDEX(index)
		self.__ClickRadioButton(self.nameColorModeButtonList, index)

	def __SetTargetBoardViewMode(self, flag):
		constInfo.SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(flag)
		self.__ClickRadioButton(self.viewTargetBoardButtonList, flag)

	def __OnClickNameColorModeNormalButton(self):
		self.__SetNameColorMode(0)

	def __OnClickNameColorModeEmpireButton(self):
		self.__SetNameColorMode(1)

	def __OnClickTargetBoardViewButton(self):
		self.__SetTargetBoardViewMode(0)

	def __OnClickTargetBoardNoViewButton(self):
		self.__SetTargetBoardViewMode(1)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	def __OnClickFogModeLevel0Button(self):
		self.__SetFogLevel(0)

	def __OnClickFogModeLevel1Button(self):
		self.__SetFogLevel(1)

	def __OnClickFogModeLevel2Button(self):
		self.__SetFogLevel(2)

	def __OnClickBlockExchangeButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EXCHANGE))

	def __OnClickBlockPartyButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY))

	def __OnClickBlockGuildButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_GUILD))

	def __OnClickBlockWhisperButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WHISPER))

	def __OnClickBlockFriendButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_FRIEND))

	def __OnClickBlockPartyRequest(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY_REQUEST))

	if app.ENABLE_TELEPORT_TO_A_FRIEND:
		def __OnClickBlockWarpRequest(self):
			self.RefreshBlock()
			global blockMode
			net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WARP_REQUEST))

	def __OnClickViewChatOnButton(self):
		global viewChatMode
		viewChatMode = 1
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()

	def __OnClickViewChatOffButton(self):
		global viewChatMode
		viewChatMode = 0
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()

	def __OnClickAlwaysShowNameOnButton(self):
		systemSetting.SetAlwaysShowNameFlag(TRUE)
		self.RefreshAlwaysShowName()

	def __OnClickAlwaysShowNameOffButton(self):
		systemSetting.SetAlwaysShowNameFlag(FALSE)
		self.RefreshAlwaysShowName()

	def __OnClickShowDamageOnButton(self):
		systemSetting.SetShowDamageFlag(TRUE)
		self.RefreshShowDamage()

	def __OnClickShowDamageOffButton(self):
		systemSetting.SetShowDamageFlag(FALSE)
		self.RefreshShowDamage()

	def __OnClickSalesTextOnButton(self):
		systemSetting.SetShowSalesTextFlag(TRUE)
		self.RefreshShowSalesText()
		uiPrivateShopBuilder.UpdateADBoard()

	def __OnClickSalesTextOffButton(self):
		systemSetting.SetShowSalesTextFlag(FALSE)
		self.RefreshShowSalesText()

	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return 1
		return 0

	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()

	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)

	def __RefreshPVPButtonList(self):
		self.__SetPKMode(player.GetPKMode())

	def __OnClickPvPModePeaceButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 0", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeRevengeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 1", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeFreeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 2", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeGuildButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if 0 == player.GetGuildID():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
			return

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 4", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def OnChangePKMode(self):
		self.__RefreshPVPButtonList()

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return TRUE

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return TRUE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()

	def RefreshViewChat(self):
		if systemSetting.IsViewChat():
			self.viewChatButtonList[0].Down()
			self.viewChatButtonList[1].SetUp()
		else:
			self.viewChatButtonList[0].SetUp()
			self.viewChatButtonList[1].Down()

	def RefreshAlwaysShowName(self):
		if systemSetting.IsAlwaysShowName():
			self.alwaysShowNameButtonList[0].Down()
			self.alwaysShowNameButtonList[1].SetUp()
		else:
			self.alwaysShowNameButtonList[0].SetUp()
			self.alwaysShowNameButtonList[1].Down()

	def RefreshShowDamage(self):
		if systemSetting.IsShowDamage():
			self.showDamageButtonList[0].Down()
			self.showDamageButtonList[1].SetUp()
		else:
			self.showDamageButtonList[0].SetUp()
			self.showDamageButtonList[1].Down()

	def RefreshShowSalesText(self):
		if systemSetting.IsShowSalesText():
			self.showsalesTextButtonList[0].Down()
			self.showsalesTextButtonList[1].SetUp()
		else:
			self.showsalesTextButtonList[0].SetUp()
			self.showsalesTextButtonList[1].Down()

	if app.ENABLE_SHOW_MOB_INFO:
		def RefreshShowMobInfo(self):
			if systemSetting.IsShowMobLevel():
				self.showMobInfoButtonList[0].Down()
			else:
				self.showMobInfoButtonList[0].SetUp()

			if systemSetting.IsShowMobAIFlag():
				self.showMobInfoButtonList[1].Down()
			else:
				self.showMobInfoButtonList[1].SetUp()

	if app.ENABLE_RENEWAL_TEXT_SHADOW:
		def RefreshOutline(self):
			if systemSetting.IsShowOutline():
				self.showOutlineButtonList[0].Down()
				self.showOutlineButtonList[1].SetUp()
			else:
				self.showOutlineButtonList[0].SetUp()
				self.showOutlineButtonList[1].Down()

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		def OnChangePickUPMode(self):
			self.RefreshPickUP()

		def RefreshPickUP(self):
			if 0 != (constInfo.PICKUPMODE & (1 << 0)):
				self.mainPickModeButtonList[0].Down()
				self.mainPickModeButtonList[1].SetUp()
			else:
				self.mainPickModeButtonList[1].Down()
				self.mainPickModeButtonList[0].SetUp()

			for i in range(1, 11):
				if 0 != (constInfo.PICKUPMODE & (1 << i)):
					self.mainPickUPButtonList[i-1].Down()
				else:
					self.mainPickUPButtonList[i-1].SetUp()

			if constInfo.PREMIUMMODE[0] == FALSE:
				self.GetChild("pickup_premium_status_text").SetText(uiScriptLocale.AUTO_PICKUP_PREMIUM_STATUS_OFF)

		def FormatTime(self, seconds):
			if seconds <= 0:
				return "|cffFF7B7B0h 0m 0s"

			m, s = divmod(seconds, 60)
			h, m = divmod(m, 60)
			d, h = divmod(h, 24)

			if d > 0:
				return "%dd %dh %dm %ds"%(d, h, m, s)

			return "%dh %dm %ds" % (h, m, s)

		def OnUpdate(self):
			if constInfo.PREMIUMMODE[0] == TRUE:
				self.GetChild("pickup_premium_status_text").SetText(uiScriptLocale.AUTO_PICKUP_PREMIUM_STATUS_ON % self.FormatTime(constInfo.PREMIUMMODE[1] - app.GetGlobalTimeStamp()))

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def __AdjustLanguageSelectWindowPosition(self):
			x, y = self.GetGlobalPosition()

			(lx, ly) = self.language_change_window.GetLocalPosition()
			if self.language_select_window_bar:
				self.language_select_window_bar.SetPosition(x + lx + 245, y + ly + 113)

		def __CreateLanguageSelectWindow(self):
			if self.language_button_dict:
				return

			languageList = LOCALE_LANG_DICT
			if not LOCALE_LANG_DICT:
				return

			curLanguageKey = self.__GetCurLanguageKey()
			if curLanguageKey is None or curLanguageKey not in LOCALE_LANG_DICT:
				curLanguageKey = LOCALE_LANG_DICT.keys()[0] if LOCALE_LANG_DICT else None
				if curLanguageKey is None:
					return

			self.cur_language_text.SetText(LOCALE_LANG_DICT[curLanguageKey]["name"])

			button_height = 16
			dict_len = len(languageList)
			self.language_select_window_height = dict_len * button_height

			self.language_select_window_bar = ui.Bar("TOP_MOST")
			self.language_select_window_bar.SetSize(210, self.language_select_window_height)
			self.language_select_window_bar.Hide()

			for index, key in enumerate(LOCALE_LANG_DICT):
				button = ui.Button()
				button.SetParent(self.language_select_window_bar)
				button.SetPosition(0, button_height * index)

				if 1 == dict_len:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")
				elif index == 0:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")
				elif index == dict_len - 1:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
				else:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")

				button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelect), key)
				button.SetOverEvent(ui.__mem_func__(self.__OnClickLanguageButtonOver), key)
				button.SetOverOutEvent(ui.__mem_func__(self.__OnClickLanguageButtonOverOut), key)
				button.SetText(LOCALE_LANG_DICT[key]["name"])
				button.Hide()

				self.language_button_dict[key] = button

			self.mouse_over_image.SetParent(self.language_select_window_bar)

			self.__AdjustLanguageSelectWindowPosition()

		def __GetCurLanguageKey(self):
			for key in LOCALE_LANG_DICT.keys():
				localeName = LOCALE_LANG_DICT[key]["locale"]
				if app.GetLocaleName() == localeName:
					return key

		def __GetStringCurLanguage(self):
			for key in LOCALE_LANG_DICT.keys():
				localeName = LOCALE_LANG_DICT[key]["locale"]
				if app.GetLocaleName() == localeName:
					return localeName

		def __LanguageSelectShowHide(self, is_show):
			if TRUE == is_show:
				self.language_select_list_open = TRUE

				if self.language_select_window_bar:
					self.language_select_window_bar.SetSize(210, self.language_select_window_height)
					self.language_select_window_bar.Show()

				for button in self.language_button_dict.values():
					button.Show()
			else:
				self.language_select_list_open = FALSE

				if self.language_select_window_bar:
					self.language_select_window_bar.SetSize(210, 0)

				for button in self.language_button_dict.values():
					button.Hide()

		def __OnClickLanguageButtonOver(self, index):
			if not self.mouse_over_image:
				return

			button = self.language_button_dict.get(index, 0)
			if 0 == button:
				return

			(button_x, button_y) = button.GetLocalPosition()
			self.mouse_over_image.SetPosition(button_x, button_y)
			self.mouse_over_image.Show()

		def __OnClickLanguageButtonOverOut(self, index):
			if not self.mouse_over_image:
				return

			self.mouse_over_image.Hide()

		def __OnClickLanguageChangeButton(self):
			if self.__GetCurLanguageKey() == self.selected_language:
				return

			if self.selected_language != 0:
				self.ConfirmLanguageChange()

		def __OnClickLanguageSelect(self, index):
			for button in self.language_button_dict.values():
				button.Hide()

			self.__LanguageSelectShowHide(FALSE)

			self.selected_language = index

			if self.cur_language_text:
				self.cur_language_text.SetText(LOCALE_LANG_DICT[index]["name"])

		def __OnClickLanguageSelectButton(self):
			self.__CreateLanguageSelectWindow()

			if self.language_select_list_open:
				self.__LanguageSelectShowHide(FALSE)
			else:
				self.__LanguageSelectShowHide(TRUE)

		def __OnLanguageSelectScroll(self):
			pass

		def __SaveLoca(self, code_page, locale):
			if app.SetLocale(code_page, locale):
				return TRUE

			return FALSE

		def OnTop(self):
			if self.language_select_window_bar:
				self.language_select_window_bar.SetTop()

		def OnMoveWindow(self, x, y):
			self.__AdjustLanguageSelectWindowPosition()

		def ConfirmLanguageChange(self):
			questionDialog = uiCommon.QuestionDialog2()
			questionDialog.SetText1(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_1)
			questionDialog.SetText2(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_2)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAcceptLanguageChange))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			questionDialog.SetWidth(450)
			questionDialog.Open()
			self.questionDialog = questionDialog

		def OnEndCountDown(self):
			net.Disconnect()
			app.ShellExecute(app.GetName())
			app.Exit()

		def OnPressExitKey(self):
			pass

		def OnAcceptLanguageChange(self):
			self.OnCloseQuestionDialog()

			if self.selected_language != 0:
				locale = LOCALE_LANG_DICT[self.selected_language]["locale"]
				code_page = LOCALE_LANG_DICT[self.selected_language]["code_page"]

				if self.__SaveLoca(code_page, locale):
					net.SetLanguage(self.selected_language)

					import introLogin
					self.popUpTimer = introLogin.ConnectingDialog()
					self.popUpTimer.Open(3.0)
					self.popUpTimer.SetText(localeInfo.LEFT_TIME)
					self.popUpTimer.SAFE_SetTimeOverEvent(self.OnEndCountDown)
					self.popUpTimer.SAFE_SetExitEvent(self.OnPressExitKey)
			else:
				pass

	def OnBlockMode(self, mode):
		global blockMode
		blockMode = mode
		self.RefreshBlock()

	def Show(self):
		self.RefreshBlock()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

		if app.ENABLE_ENB_MODE:
			if self.questionDialog:
				self.OnCloseQuestionDialog()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.__LanguageSelectShowHide(FALSE)

	def __NotifyChatLine(self, text):
		chat.AppendChat(chat.CHAT_TYPE_INFO, text)
