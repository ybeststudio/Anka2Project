# -*- coding: utf-8 -*-
if __USE_DYNAMIC_MODULE__:
	import pyapi

player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))
pack = __import__(pyapi.GetModuleName("pack"))
app = __import__(pyapi.GetModuleName("app"))

# static
import ui, constInfo, uiSelectMusic, musicInfo, localeInfo, uiScriptLocale

# dynamic
import dbg, chat, wndMgr, systemSetting, snd, background
import time
import advanced_options_reference
import advanced_options_runtime

MUSIC_FILENAME_MAX_LEN = 25
blockMode = 0
IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"
TEXT_MAIN_COLOR = "|cffcab5a4"
# Baerim referansi: GRAPHICS_SKY_DEFAULT, PRESET_01..03, STARRY_NIGHT, PRESET_04..10 (sira ayni).
# Dosyalar advanced_options_runtime.SKY_FILES ile ayni indekste.
SKY_PRESETS = (
	("Varsay\xfdlan", None),
	("Sabah Sisi", "environment_02.msenv"),
	("Hafif Bulutlar", "environment_03.msenv"),
	("Mor Alacakaranl\xfdk", "environment_04.msenv"),
	("Y\xfdld\xfdzl\xfd Gece", "starry_night.msenv"),
	("A\xe7\xfdk G\xf6ky\xfcz\xfc", "environment_05.msenv"),
	("Yaz G\xf6ky\xfcz\xfc", "environment_06.msenv"),
	("Pembe \xdeafak", "environment_07.msenv"),
	("Alt\xfdn Alacakaranl\xfdk", "environment_08.msenv"),
	("Kehribar Ufuk", "environment_09.msenv"),
	("Derin Mavi", "environment_10.msenv"),
	("Alt\xfdn I\xfe\xfdk", "environment_11.msenv"),
)

advanced_options_runtime.Configure(app=app, background=background, systemSetting=systemSetting, snd=snd, constInfo=constInfo, localeInfo=localeInfo, ui=ui, chat=chat, wndMgr=wndMgr)

if not hasattr(constInfo, "ADVANCED_OPTIONS_SETTINGS"):
	constInfo.ADVANCED_OPTIONS_SETTINGS = {}

def GetOptSetting(key, defaultVal=False):
	return advanced_options_runtime.Get(key, constInfo.ADVANCED_OPTIONS_SETTINGS.get(key, defaultVal))

def SetOptSetting(key, val):
	advanced_options_runtime.Set(key, val)
	constInfo.ADVANCED_OPTIONS_SETTINGS[key] = val

def _SetClipboardText(text):
	try:
		if hasattr(app, "SetClipboardText"):
			app.SetClipboardText(str(text))
			return True
	except Exception:
		pass
	try:
		import ctypes
		if ctypes.windll.user32.OpenClipboard(0):
			ctypes.windll.user32.EmptyClipboard()
			hCd = ctypes.windll.kernel32.GlobalAlloc(2, len(str(text)) + 1)
			pCd = ctypes.windll.kernel32.GlobalLock(hCd)
			ctypes.cdll.msvcrt.strcpy(ctypes.c_char_p(pCd), str(text))
			ctypes.windll.kernel32.GlobalUnlock(hCd)
			ctypes.windll.user32.SetClipboardData(1, hCd)
			ctypes.windll.user32.CloseClipboard()
			return True
	except Exception:
		pass
	return False

option_data = [
	{"category": "Arayüz", "icon": "interface"},
	{"category": "Oyun", "icon": "game"},
	{"category": "Çevre", "icon": "environment"},
	{"category": "Ses", "icon": "sound"},
	{"category": "Dil", "icon": "language"},
	{"category": "Performance", "icon": "performance"}
]

CATEGORY_CONFIG = {
	0: {
		"title": "Arayüz",
		"groups": [
			{"id": "keyboard", "name": "Keyboard Settings", "windows": ["keyboard_window"], "hide_titles": ["keyboard_title_img"]},
			{"id": "chat", "name": "Chat", "windows": ["chat_window"], "hide_titles": ["chat_mode_title_img"]},
			{"id": "show_hide_interface", "name": "Show/Hide Interface", "windows": ["drop_ui_window", "guideline_window", "system_menu_window", "event_icon_window", "guild_quest_window", "show_guild_join_notification_window"], "hide_titles": ["drop_interface_title_img", "guideline_title_img", "system_menu_title_img", "event_icon_title_img", "guild_quest_title_img", "show_guild_join_notification_title_img"]},
			{"id": "font_settings", "name": "Font Settings", "windows": ["window_scale_window", "font_change_window"], "hide_titles": ["window_scale_title_img", "font_change_title_img"]},
			{"id": "whisper_settings", "name": "Whisper Settings", "windows": ["whisper_sender_letter_window"], "hide_titles": ["whisper_sender_letter_title_img"]},
			{"id": "others", "name": "Others", "windows": ["save_status_window", "small_affect_icon_window"], "hide_titles": ["save_status_title_img", "small_affect_icon_title_img"]},
		]
	},
	1: {
		"title": "Oyun",
		"groups": [
			{"id": "pvp_mode", "name": "PvP Mode", "windows": ["pvp_mode_window"], "hide_titles": ["pvp_mode_title_img"]},
			{"id": "block", "name": "Block", "windows": ["block_mode_window"], "hide_titles": ["block_mode_title_img"]},
			{"id": "name_settings", "name": "Name Settings", "windows": ["names_window", "show_guild_grade_window"], "hide_titles": ["names_title_img", "show_guild_grade_title_img"]},
			{"id": "damage_settings", "name": "Damage Settings", "windows": ["damage_window"], "hide_titles": ["show_damage_title_img", "damage_title_img"]},
			{"id": "metinstone_settings", "name": "Metinstone Settings", "windows": ["stone_scale_window", "queue_metin_window", "event_stone_window", "hide_metin_spawns_window"], "hide_titles": ["stone_scale_title_img", "queue_metin_title_img", "event_stone_title_img", "hide_metin_spawns_img"]},
			{"id": "hide_entities_effects", "name": "Hide Entities / Effects", "windows": ["hide_companion_window", "hide_effects_except_damage_window", "auto_hide_companion_map1_window", "shop_range_window"], "hide_titles": ["hide_companion", "hide_effects_except_damage_title_img", "auto_hide_companion_map1_img", "shop_range_title_img"]},
			{"id": "notification_settings", "name": "Notification Settings", "windows": ["notification_settings_window", "notification_history_button_window"], "hide_titles": ["notification_settings_title_img", "notification_history_button_title_img"]},
			{"id": "buffi_settings", "name": "Buffi Ayarları", "windows": ["buffi_settings_window"], "hide_titles": ["buffi_settings"]},
			{"id": "others", "name": "Others", "windows": ["mining_flash_window", "auto_cape_after_warp_window", "disable_level_up_confirmation_window"], "hide_titles": ["mining_flash_title_img", "auto_cape_after_warp_title_img", "disable_level_up_confirmation_title_img"]},
		]
	},
	2: {
		"title": "Çevre",
		"groups": [
			{"id": "graphics_background", "name": "Arka Plan Ayarlar\xfd", "windows": ["anti_aliasing_window", "sharpness_windows", "local_light_window", "specular_reflection_window", "remove_fog_window", "rain_window", "storm_window", "wet_ground_window"], "hide_titles": ["anti_aliasing_title_img", "sharpness_window_img", "local_light_window_img", "specular_reflection_window_img", "remove_fog_window_img", "rain_window_img", "storm_window_img", "wet_ground_window_img"]},
			{"id": "graphics_shadow", "name": "G\xf6lge Ayarlar\xfd", "windows": ["graphics_level_window", "shadow_options_window", "tree_shadow_window"], "hide_titles": ["graphics_level_img", "shadow_options_title_img", "tree_shadow_window_img"]},
			{"id": "graphics_trees", "name": "A\xf0a\xe7 Ayarlar\xfd", "windows": ["speedtree_fx_window"], "hide_titles": ["speedtree_fx_window_img"]},
			{"id": "graphics_water", "name": "Su Ayarlar\xfd", "windows": ["water_reflection_window", "enhanced_water_window", "water_ripple_window", "underwater_window"], "hide_titles": ["water_reflection_window_img", "enhanced_water_window_img", "water_ripple_window_img", "underwater_window_img"]},
			{"id": "graphics_camera", "name": "Kamera Ayarlar\xfd", "windows": ["fov_windows", "camera_window", "death_cam_window"], "hide_titles": ["death_cam_window_img"]},
			{"id": "graphics_sky", "name": "G\xf6ky\xfcz\xfc Ayarlar\xfd", "windows": ["sun_rays_window", "shooting_stars_window", "environment_window", "skybox_window"], "hide_titles": ["sun_rays_window_img", "shooting_stars_window_img", "environment_window_img", "skybox_window_img"]},
			{"id": "graphics_environment", "name": "\xc7evre Ayarlar\xfd", "windows": ["environment_darkness_window", "environment_saturation_window", "environment_color_window", "environment_temperature_window", "environment_contrast_window"], "hide_titles": ["environment_darkness_window_img", "environment_saturation_window_img", "environment_color_window_img", "environment_temperature_window_img", "environment_contrast_window_img"]},
		]
	},
	3: {
		"title": "Ses",
		"groups": [
			{"id": "music_selection", "name": "M\xfczik Se\xe7imi", "windows": ["bgm_window"], "hide_titles": ["bgm_title_img"]},
			{"id": "sound_volume", "name": "Ses d\xfczeyi", "windows": ["volume_window"], "hide_titles": ["volume_title_img"]},
			{"id": "sound_categories", "name": "Ses efekti ayr\xfdnt\xfdlar\xfd", "windows": ["sound_categories_window"], "hide_titles": ["sound_categories_title_img"]},
		]
	},
	4: {
		"title": "Dil",
		"groups": [
			{"id": "language_settings", "name": "Client Language", "windows": ["language_window"], "hide_titles": ["lang_mode", "language_title_img"]},
		]
	},
	5: {
		"title": "Performance",
		"groups": [
			{"id": "crowd_performance", "name": "Performans", "windows": ["crowd_performance_window"], "hide_titles": ["crowd_performance_img", "crowd_performance_title_img"]},
			{"id": "fps_limit", "name": "FPS", "windows": ["fps_limit_window"], "hide_titles": ["fps_limit_img", "fps_limit_title_img"]},
		]
	}
}

class AdvancedGameOptionsWindow(ui.BoardWithTitleBar):
	# Keep the reference skin local to this panel.
	BASE_PATH = IMG_DIR + "panel_skin"

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.pvpModeButtonDict = {}
		self.blockButtonList = []
		self.viewChatButtonList = []
		self.nameColorModeButtonList = []
		self.viewTargetBoardButtonList = []
		self.alwaysShowNameButtonList = []
		self.showDamageButtonList = []
		self.showsalesTextButtonList = []
		self.cameraModeButtonList = []
		self.skyboxModeButtonList = []
		self.fogModeButtonList = []
		self.waterModeButtonList = []
		self.rainPuddleButtonList = []
		self.grassModeButtonList = []
		self.nightModeButtonList = []
		self.snowModeButtonList = []
		self.nameOutlineModeButtonList = []
		self.enbModeButtonList = []
		self.graphicsLevelButtonList = []
		self.shadowQualityButtonList = []
		self.sharpnessController = None
		self.sharpnessResetButton = None
		self.effectLevelButtonList = []
		self.effectLevel = 0
		self.effectLevelApplyButton = None
		self.privateShopLevelButtonList = []
		self.privateShopLevel = 0
		self.privateShopLevelApplyButton = None
		self.dropItemLevelButtonList = []
		self.dropItemLevel = 0
		self.dropItemLevelApplyButton = None
		self.petStatusButtonList = []
		self.npcNameStatusButtonList = []
		self.showMobLevelButtonList = []
		self.showMobAIFlagButtonList = []
		self.fovController = None
		self.fovResetButton = None
		self.stoneScaleController = None
		self.stoneScaleResetButton = None
		self.mainPickModeButtonList = []
		self.mainPickUPButtonList = []
		self.pickupPremiumStatusText = None
		self.scrollbar_is_show = False

		self.ctrlSoundVolume = None
		self.ctrlMusicVolume = None
		self.ctrlEnvVolume = None
		self.selectMusicFile = None
		self.selectMusicButton = None
		self.musicListDlg = None

		self.toggleControls = {}
		self.sliderControls = {}
		self.radioControls = {}
		self.languageButtonList = {}
		self.fpsLimitButtonList = {}
		self.skyboxComboBox = None
		self.cameraFarButton = None
		self.nightModeButton = None
		self.removeFogButton = None

		for j in xrange(10):
			if self.children.has_key("optionWindow%d" % j):
				window = self.children["optionWindow%d" % j]
				if window:
					window.Hide()
					if getattr(window, "scrollBar", None):
						window.scrollBar.Hide()
						window.scrollBar.Destroy()
					window.scrollBar = None
					window.ClearDictionary()
					window.Destroy()
		self.children = {}
		self.Hide()

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.children = {}
		self.toggleControls = {}
		self.sliderControls = {}
		self.radioControls = {}
		self.languageButtonList = {}
		self.fpsLimitButtonList = {}
		self.skyboxComboBox = None
		self.cameraFarButton = None
		self.nightModeButton = None
		self.removeFogButton = None
		self.Destroy()
		self.scrollbar_is_show = False
		advanced_options_reference.ApplyLocale(app.GetLocalePath().replace("\\", "/").split("/")[-1].lower(), app.GetDefaultCodePage())
		if app.GetLocalePath().replace("\\", "/").split("/")[-1].lower() == "tr":
			localeInfo.GAME_OPTION_HIDE_METIN_SPAWNS = "Do\xf0an canavarlar\xfd gizle"
			localeInfo.GAME_OPTION_HIDE_METIN_SPAWNS_DESC = "Metin ta\xfelar\xfdndan do\xf0an canavarlar\xfd gizle. H\xe2l\xe2 sald\xfdrabilirler."
		self.__LoadWindow()
		self.SetTitleName(uiScriptLocale.GAMEOPTION_TITLE)
		self.last_update_time = time.time()
		self.animation_interval = 0.01
		self.text_directions = {}
		self.original_texts = {}
		self.expected_ending_texts = {}
		self.default_texts = {}
		self.text_iteration = {}
		self.text_length = {}
		self.text_nr_of_letters_hidden = {}
		self.text_nr_of_letters_revealed = {}

	def __LoadWindow(self):
		self.AddFlag("movable")
		self.AddFlag("attach")

		bg = ui.ImageBox()
		bg.SetParent(self)
		bg.AddFlag("attach")
		bg.SetPosition(10, 33)
		bg.LoadImage(IMG_DIR + "background.png")
		bg.Show()
		self.children["bg"] = bg

		self.SetSize(bg.GetWidth() + 21, bg.GetHeight() + 44)
		self.SetCenterPosition()
		self.titleBar.imgLeft.LoadImage(IMG_DIR + "panel_skin/titlebar_left.tga")
		self.titleBar.imgCenter.LoadImage(IMG_DIR + "panel_skin/titlebar_center.tga")
		self.titleBar.imgRight.LoadImage(IMG_DIR + "panel_skin/titlebar_right.tga")
		self.titleBar.btnClose.SetUpVisual(IMG_DIR + "panel_skin/close_button_01.sub")
		self.titleBar.btnClose.SetOverVisual(IMG_DIR + "panel_skin/close_button_02.sub")
		self.titleBar.btnClose.SetDownVisual(IMG_DIR + "panel_skin/close_button_03.sub")
		self.titleBar.SetWidth(self.GetWidth() - 15)

		searchBoard = ui.Window()
		searchBoard.SetParent(bg)
		searchBoard.SetPosition(6, 4)
		searchBoard.SetSize(208, 28)
		searchBoard.Show()
		self.children["searchBoard"] = searchBoard

		self.search_bg = ui.ImageBox()
		self.search_bg.SetParent(self.children["searchBoard"])
		self.search_bg.SetPosition(0, 0)
		self.search_bg.LoadImage(IMG_DIR + "search_bg.png")
		self.search_bg.Show()

		self.searchButton = ui.Button()
		self.searchButton.SetParent(self.search_bg)
		self.searchButton.SetUpVisual(IMG_DIR + "panel_skin/search_norm.png")
		self.searchButton.SetOverVisual(IMG_DIR + "panel_skin/search_hover.png")
		self.searchButton.SetDownVisual(IMG_DIR + "panel_skin/search_select.png")
		self.searchButton.SetPosition(self.search_bg.GetWidth() - self.searchButton.GetWidth() - 3, 2)
		self.searchButton.SAFE_SetEvent(self.StartSearch)
		self.searchButton.Show()

		self.CancelSearchButton = ui.Button()
		self.CancelSearchButton.SetParent(self.search_bg)
		self.CancelSearchButton.SetUpVisual(IMG_DIR + "search_btn_x_0.png")
		self.CancelSearchButton.SetOverVisual(IMG_DIR + "search_btn_x_1.png")
		self.CancelSearchButton.SetDownVisual(IMG_DIR + "search_btn_x_2.png")
		self.CancelSearchButton.SetPosition(self.search_bg.GetWidth() - 56, 2)
		self.CancelSearchButton.SAFE_SetEvent(self.OnClickClearSearchButton)
		self.CancelSearchButton.Hide()

		self.searchEdit = ui.EditLine()
		self.searchEdit.SetParent(self.search_bg)
		self.searchEdit.SetMax(50)
		self.searchEdit.SetSize(self.search_bg.GetWidth() - self.searchButton.GetWidth() - self.CancelSearchButton.GetWidth() - 10, 15)
		self.searchEdit.SetPosition(10, (self.search_bg.GetHeight() - 15) / 2)
		self.searchEdit.SetLimitWidth(self.searchEdit.GetWidth())
		self.searchEdit.SetEscapeEvent(ui.__mem_func__(self.OnPressNameEscapeKey))
		self.searchEdit.SetReturnEvent(ui.__mem_func__(self.StartSearch))
		self.searchEdit.SetFontName("Tahoma:12")
		self.searchEdit.SetPackedFontColor(0xff9f9892)
		self.searchEdit.SetOutline()
		self.searchEdit.Show()
		self.searchHint = ui.TextLine()
		self.searchHint.SetParent(self.search_bg)
		self.searchHint.AddFlag("not_pick")
		self.searchHint.SetPosition(10, 6)
		self.searchHint.SetFontName("Tahoma:12")
		self.searchHint.SetPackedFontColor(0xff9f9892)
		self.searchHint.SetText(getattr(localeInfo, "GAME_OPTION_SEARCH", "Ara.."))
		self.searchHint.Show()

		self.children["filteringEnabled"] = False

		categoryScrollBoard = ui.Window()
		categoryScrollBoard.SetParent(bg)
		categoryScrollBoard.SetPosition(6, 36)
		categoryScrollBoard.SetSize(200, 356)
		categoryScrollBoard.SetInsideRender(True)
		categoryScrollBoard.Show()
		self.children["categoryScrollBoard"] = categoryScrollBoard

		self.scrollBar2 = ScrollBarNew()
		self.scrollBar2.SetParent(bg)
		self.scrollBar2.SetPosition(206, 40)
		self.scrollBar2.SetScrollBarSize(356)
		self.scrollBar2.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.scrollBar2.Hide()
		# Reset Button on Title Bar
		self.resetButton = ui.Button()
		self.resetButton.SetParent(self)
		self.resetButton.SetPosition(12, 7)
		self.resetButton.SetUpVisual(IMG_DIR + "panel_skin/button_titlebar_refresh_norm.png")
		self.resetButton.SetOverVisual(IMG_DIR + "panel_skin/button_titlebar_refresh_hover.png")
		self.resetButton.SetDownVisual(IMG_DIR + "panel_skin/button_titlebar_refresh_down.png")
		self.resetButton.SAFE_SetEvent(self.__ClickResetConfig)
		self.resetButton.Show()

		self.children["categoryScrollBoard"].OnRunMouseWheel = self.OnRunMouseWheel
		self.children["categoryScrollBoard"].OnMouseWheel = self.OnRunMouseWheel
		bg.OnRunMouseWheel = self.OnRunMouseWheel
		bg.OnMouseWheel = self.OnRunMouseWheel

		self.categoryButtons = []
		category_icons = ["interface", "game", "environment", "sound", "language", "performance"]

		for optionIndex, category_data in enumerate(option_data):
			key = category_data["category"]
			icon_name = category_data.get("icon", category_icons[optionIndex] if optionIndex < len(category_icons) else "game")

			categoryButton = ui.Button()
			categoryButton.SetParent(self.children["categoryScrollBoard"])
			categoryButton.SetUpVisual(IMG_DIR + "btn_cat_1.png")
			categoryButton.SetOverVisual(IMG_DIR + "btn_cat_2.png")
			categoryButton.SetDownVisual(IMG_DIR + "btn_cat_3.png")
			categoryButton.SetPosition(0, optionIndex * 43)
			categoryButton.SetText(TEXT_MAIN_COLOR + key)
			if categoryButton.ButtonText:
				categoryButton.ButtonText.SetFontName("Tahoma:12")
				categoryButton.ButtonText.SetPosition(53, 12)
				categoryButton.ButtonText.SetHorizontalAlignLeft()
			categoryButton.SAFE_SetEvent(self.ClickCategory, optionIndex)

			category_icon = ui.Button()
			category_icon.SetParent(categoryButton)
			category_icon.SetUpVisual(IMG_DIR + "category_icons/" + icon_name + ".png")
			category_icon.SetOverVisual(IMG_DIR + "category_icons/" + icon_name + ".png")
			category_icon.SetDownVisual(IMG_DIR + "category_icons/" + icon_name + ".png")
			category_icon.SetPosition(0, 0)
			category_icon.SAFE_SetEvent(self.ClickCategory, optionIndex)
			category_icon.Show()

			categoryButton.Show()
			self.children["titleBtn%d" % optionIndex] = categoryButton
			self.children["category_icon%d" % optionIndex] = category_icon
			self.categoryButtons.append(categoryButton)

			optionWindow = self.CreateOptionWindow(bg, optionIndex)
			optionWindow.Hide()
			self.children["optionWindow%d" % optionIndex] = optionWindow

		self.ClickCategory(0)

	def ShowHideCancelSearchButton(self):
		if not self.searchEdit:
			return
		if self.searchEdit.GetText() == "":
			self.CancelSearchButton.Hide()
			self.searchHint.Show()
		else:
			self.CancelSearchButton.Show()
			self.searchHint.Hide()

	def _clear_previous_filters(self):
		if self.searchEdit:
			self.searchEdit.SetText("")
		self.ShowHideCancelSearchButton()
		self.children["filteringEnabled"] = False
		if self.children.has_key("filterWindow"):
			self.children["filterWindow"].Hide()
		if self.children.has_key("filteredWindow"):
			self.children["filteredWindow"].Hide()
		curCat = self.children.get("optionType", 0)
		key = "optionWindow%d" % curCat
		curWin = self.children.get(key, None)
		if curWin and hasattr(curWin, "optionGroups"):
			for idx, group in enumerate(curWin.optionGroups):
				group.Show()
				is_first = (curCat == 3 and idx < 2)
				group.is_expanded = is_first
				if is_first:
					group.SetSize(352, getattr(group, "originalHeight", 80))
					for c in getattr(group, "contentControls", []):
						if c:
							c.Show()
					if getattr(group, "arrowBtn", None):
						group.arrowBtn.SetUpVisual(IMG_DIR + "arrow_up.png")
						group.arrowBtn.SetOverVisual(IMG_DIR + "arrow_up.png")
						group.arrowBtn.SetDownVisual(IMG_DIR + "arrow_up.png")
				else:
					group.SetSize(352, 29)
					for c in getattr(group, "contentControls", []):
						if c:
							c.Hide()
					if getattr(group, "arrowBtn", None):
						group.arrowBtn.SetUpVisual(IMG_DIR + "arrow_down.png")
						group.arrowBtn.SetOverVisual(IMG_DIR + "arrow_down.png")
						group.arrowBtn.SetDownVisual(IMG_DIR + "arrow_down.png")
			self.__LayoutOptionGroups(curWin)
			scrollBar = getattr(curWin, "scrollBar", None)
			if scrollBar:
				scrollBar.SetPos(0.0)
			self.UpdateScrollBar()
		self.ClickCategory(curCat)

	def OnClickClearSearchButton(self):
		self._clear_previous_filters()

	def OnPressNameEscapeKey(self):
		if not self.searchEdit:
			return
		if not self.searchEdit.IsShowCursor() and self.searchEdit.GetText() == "":
			self.OnPressEscapeKey()
		else:
			self._clear_previous_filters()

	def _text_matches_search(self, text, query):
		if not text or not query:
			return False
		try:
			return str(query).lower() in str(text).lower()
		except Exception:
			return False

	def _widget_matches_search(self, widget, query):
		if not widget:
			return False
		wName = getattr(widget, "GetWindowName", None)
		if wName:
			try:
				if self._text_matches_search(widget.GetWindowName(), query):
					return True
			except Exception:
				pass
		if isinstance(widget, ui.TextLine):
			try:
				if self._text_matches_search(widget.GetText(), query):
					return True
			except Exception:
				pass
		if isinstance(widget, (ui.Button, ui.RadioButton, ui.ToggleButton)):
			for fn in ("GetText", "GetButtonText"):
				func = getattr(widget, fn, None)
				if func:
					try:
						t = func()
						if t and self._text_matches_search(t, query):
							return True
					except Exception:
						pass
		for child in getattr(widget, "Children", []):
			if child and self._widget_matches_search(child, query):
				return True
		return False

	def _group_matches_search(self, group, query):
		if not group or not query:
			return False
		gName = getattr(group, "group_name", "")
		if self._text_matches_search(gName, query):
			return True
		gId = getattr(group, "group_id", "")
		if self._text_matches_search(gId, query):
			return True
		for ctrl in getattr(group, "contentControls", []):
			if self._widget_matches_search(ctrl, query):
				return True
		return False

	def StartSearch(self):
		searchText = self.searchEdit.GetText().strip()
		if not searchText:
			self._clear_previous_filters()
			return

		self.ShowHideCancelSearchButton()
		searchLower = searchText.lower()

		curCat = self.children.get("optionType", 0)
		curWin = self.children.get("optionWindow%d" % curCat, None)

		matchedCat = None
		firstMatchedGroup = None

		# 1. Önce aktif kategoride eşleşme var mı kontrol et
		if curWin and hasattr(curWin, "optionGroups"):
			for group in curWin.optionGroups:
				if self._group_matches_search(group, searchLower):
					matchedCat = curCat
					firstMatchedGroup = group
					break

		# 2. Aktif kategoride yoksa diğer kategorileri tara
		if matchedCat is None:
			for catIdx in xrange(len(option_data)):
				if catIdx == curCat:
					continue
				key = "optionWindow%d" % catIdx
				if not self.children.has_key(key):
					self.children[key] = self.CreateOptionWindow(self.children["bg"], catIdx)
				checkWin = self.children[key]
				if checkWin and hasattr(checkWin, "optionGroups"):
					for group in checkWin.optionGroups:
						if self._group_matches_search(group, searchLower):
							matchedCat = catIdx
							firstMatchedGroup = group
							break
				if matchedCat is not None:
					break

		if matchedCat is not None:
			if matchedCat != curCat:
				self.ClickCategory(matchedCat)
			curWin = self.children.get("optionWindow%d" % matchedCat, None)
			if curWin and hasattr(curWin, "optionGroups"):
				self.children["filteringEnabled"] = True
				for group in curWin.optionGroups:
					if self._group_matches_search(group, searchLower):
						group._matches_filter = True
						group.Show()
						group.is_expanded = True
						group.SetSize(352, getattr(group, "originalHeight", 80))
						for c in getattr(group, "contentControls", []):
							if c:
								c.Show()
						if getattr(group, "arrowBtn", None):
							group.arrowBtn.SetUpVisual(IMG_DIR + "arrow_up.png")
							group.arrowBtn.SetOverVisual(IMG_DIR + "arrow_up.png")
							group.arrowBtn.SetDownVisual(IMG_DIR + "arrow_up.png")
					else:
						group._matches_filter = False
						group.Hide()

				self.__LayoutOptionGroups(curWin)
				self.UpdateScrollBar()
				if firstMatchedGroup and hasattr(firstMatchedGroup, "exPos"):
					scrollBar = getattr(curWin, "scrollBar", None)
					if scrollBar and scrollBar.IsShow():
						totalH = getattr(curWin, "totalContentHeight", 0)
						winH = curWin.GetHeight()
						if totalH > winH:
							scrollBar.SetPos(float(max(0, firstMatchedGroup.exPos[1] - 5)) / float(totalH - winH))
							self.UpdateScrollBar()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Aramanıza uygun seçenek bulunamadı: %s" % searchText)

	def Open(self):
		self.RefreshBlock()
		self.Show()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			if not buttonList or not isinstance(buttonList, (list, tuple)):
				return
			if buttonIndex < 0 or buttonIndex >= len(buttonList):
				return
			btn = buttonList[buttonIndex]
			if btn is None:
				return
			for eachButton in buttonList:
				if eachButton:
					eachButton.SetUp()
			btn.Down()
		except Exception as e:
			dbg.TraceError("__ClickRadioButton: %s" % str(e))

	def IsCanContinue(self, dictKey):
		if dictKey == "index":
			return True
		return False

	def GetTitleTextLines(self):
		pass

	def ClickCategory(self, optionType):
		if optionType < 0 or optionType >= len(option_data):
			return

		if self.children.get("filteringEnabled", False):
			self.children["filteringEnabled"] = False
			if self.children.has_key("filterWindow"):
				self.children["filterWindow"].Hide()

		for j in xrange(len(option_data)):
			if self.children.has_key("titleBtn%d" % j):
				btn = self.children["titleBtn%d" % j]
				if j == optionType:
					btn.SetUpVisual(IMG_DIR + "btn_cat_3.png")
					btn.SetOverVisual(IMG_DIR + "btn_cat_3.png")
					btn.SetDownVisual(IMG_DIR + "btn_cat_3.png")
					btn.Down()
				else:
					btn.SetUpVisual(IMG_DIR + "btn_cat_1.png")
					btn.SetOverVisual(IMG_DIR + "btn_cat_2.png")
					btn.SetDownVisual(IMG_DIR + "btn_cat_3.png")
					btn.SetUp()

			if self.children.has_key("optionWindow%d" % j):
				self.children["optionWindow%d" % j].Hide()
				previousScrollBar = getattr(self.children["optionWindow%d" % j], "scrollBar", None)
				if previousScrollBar:
					previousScrollBar.Hide()

		self.children["optionType"] = optionType
		key = "optionWindow%d" % optionType
		if not self.children.has_key(key):
			self.children[key] = self.CreateOptionWindow(self.children["bg"], optionType)

		curWin = self.children[key]
		if curWin:
			curWin.Show()
			scrollBar = getattr(curWin, "scrollBar", None)
			if scrollBar:
				scrollBar.SetPos(0.0, moveEvent=False)
			self.UpdateScrollBar()

	def ClickSubcategory(self, optionType, categoryType):
		self.ClickCategory(optionType)
		self.GoToGroupIndex(categoryType)

	def OnUpdate(self):
		self.ShowHideCancelSearchButton()

	def __CollapseSubwindowTitle(self, subWin, hideTitleNames, compactGame=False):
		if not subWin:
			return
		foundTitle = None
		for tName in hideTitleNames:
			for c in getattr(subWin, "Children", []):
				if c and c.GetWindowName() == tName:
					foundTitle = c
					break
			if foundTitle:
				break

		if not foundTitle:
			children = getattr(subWin, "Children", [])
			if len(children) > 0:
				firstChild = children[0]
				cName = firstChild.GetWindowName().lower()
				if "title" in cName or "img" in cName or "mode" in cName:
					foundTitle = firstChild

		if foundTitle and not getattr(subWin, "_title_collapsed", False):
			subWin._title_collapsed = True
			foundTitle.Hide()
			contentRows = [c.GetLocalPosition()[1] for c in getattr(subWin, "Children", []) if c and c != foundTitle]
			# Already title-free reference layouts start at y=5.
			shiftY = 28 if contentRows and min(contentRows) >= 28 else 0
			for c in getattr(subWin, "Children", []):
				if c and c != foundTitle:
					(cx, cy) = c.GetLocalPosition()
					c.SetPosition(cx, max(0, cy - shiftY))
		if compactGame and subWin.GetWindowName() == "hide_effects_except_damage_window":
			for child in getattr(subWin, "Children", []):
				if child and child.GetWindowName() == "hide_effects_dropdown_anchor":
					child.SetPosition(17, 0)

		def contentBottom(widget):
			bottom = max(0, widget.GetHeight())
			if isinstance(widget, ui.TextLine):
				bottom = max(bottom, 14 if compactGame else widget.GetTextSize()[1])
			for child in getattr(widget, "Children", []):
				if child and child != foundTitle:
					(cx, cy) = child.GetLocalPosition()
					bottom = max(bottom, cy + contentBottom(child))
			return bottom

		# Preserve reserved row spacing and include every nested description.
		maxBottom = 0
		for child in getattr(subWin, "Children", []):
			if child and child != foundTitle:
				(cx, cy) = child.GetLocalPosition()
				maxBottom = max(maxBottom, cy + contentBottom(child))
		subWin.SetSize(352, max(32, maxBottom + 6))

	def __CreateOptionGroups(self, scriptWindow, optionIndex):
		if not scriptWindow:
			return
		scriptWindow.optionGroups = []
		cfg = CATEGORY_CONFIG.get(optionIndex, None)
		if not cfg:
			return

		elementDict = getattr(scriptWindow, "ElementDictionary", {})
		allChildren = list(getattr(scriptWindow, "Children", []))

		for groupIndex, groupDef in enumerate(cfg["groups"]):
			groupName = groupDef["name"]
			groupId = groupDef["id"]
			targetWindowNames = groupDef["windows"]
			hideTitleNames = groupDef.get("hide_titles", [])

			groupSubWindows = []
			for wName in targetWindowNames:
				w = elementDict.get(wName, None)
				if not w:
					for c in allChildren:
						if c and c.GetWindowName() == wName:
							w = c
							break
				if w and w not in groupSubWindows:
					# Zero-height sections are disabled in the reference script.
					if w.GetHeight() <= 0:
						w.Hide()
						continue
					groupSubWindows.append(w)

			if len(groupSubWindows) == 0:
				continue

			container = ui.Window()
			container.SetParent(scriptWindow)
			container.AddFlag("attach")

			headerBar = ui.ExpandedImageBox()
			headerBar.SetParent(container)
			headerBar.AddFlag("attach")
			headerBar.LoadImage(IMG_DIR + "option_category.png")
			headerBar.SetPosition(0, 0)
			headerBar.Show()

			headerClick = ui.Button()
			headerClick.SetParent(container)
			headerClick.SetPosition(0, 0)
			headerClick.SetSize(352 - 50, 29)
			headerClick.SAFE_SetEvent(self.__ToggleOptionGroup, scriptWindow, container)
			headerClick.Show()

			headerTitle = ui.TextLine()
			headerTitle.SetParent(container)
			headerTitle.SetPosition(15, 6)
			headerTitle.SetFontName("Tahoma:12b")
			headerTitle.SetPackedFontColor(0xffcebb98)
			headerTitle.SetOutline()
			headerTitle.SetText(groupName)
			headerTitle.Show()

			copyBtn = ui.Button()
			copyBtn.SetParent(container)
			copyBtn.SetUpVisual(IMG_DIR + "button_copylink_norm.png")
			copyBtn.SetOverVisual(IMG_DIR + "button_copylink_over.png")
			copyBtn.SetDownVisual(IMG_DIR + "button_copylink_down.png")
			copyBtn.SetPosition(352 - 45, 7)
			copyBtn.SAFE_SetEvent(self.__CopyOptionGroupHyperlink, optionIndex, groupId, groupName)
			copyBtn.Show()

			arrowBtn = ui.Button()
			arrowBtn.SetParent(container)
			arrowBtn.SetUpVisual(IMG_DIR + "arrow_down.png")
			arrowBtn.SetOverVisual(IMG_DIR + "arrow_down.png")
			arrowBtn.SetDownVisual(IMG_DIR + "arrow_down.png")
			arrowBtn.SetPosition(352 - 25, 9)
			arrowBtn.SAFE_SetEvent(self.__ToggleOptionGroup, scriptWindow, container)
			arrowBtn.Show()

			curY = 29
			contentControls = []
			for subWin in groupSubWindows:
				self.__CollapseSubwindowTitle(subWin, hideTitleNames, optionIndex in (0, 1, 2, 5))
				if optionIndex == 1:
					# Baerim's Game pane keeps one-line descriptions and fixed rows.
					fixedHeights = {"damage_window": 234, "stone_scale_window": 63,
						"hide_companion_window": 61, "hide_effects_except_damage_window": 63,
						"auto_hide_companion_map1_window": 44, "shop_range_window": 61,
						"notification_settings_window": 63,
						"notification_history_button_window": 43}
					subWinName = subWin.GetWindowName()
					if subWinName in fixedHeights:
						subWin.SetSize(352, fixedHeights[subWinName])
				if optionIndex == 5 and subWin.GetWindowName() == "crowd_performance_window":
					subWin.SetSize(352, 65)
				if optionIndex == 2 and groupId == "graphics_background":
					if subWin.GetWindowName() == "sharpness_windows":
						subWin.SetSize(352, subWin.GetHeight() + 13)
					elif subWin.GetWindowName() in ("rain_window", "storm_window"):
						subWin.SetSize(352, 44)
					elif subWin.GetWindowName() == "wet_ground_window":
						subWin.SetSize(352, subWin.GetHeight() + 3)
				if optionIndex == 3:
					soundHeights = {"bgm_window": 47, "volume_window": 156,
						"sound_categories_window": 360}
					if subWin.GetWindowName() in soundHeights:
						subWin.SetSize(352, soundHeights[subWin.GetWindowName()])
				subWin.SetParent(container)
				subWin.SetPosition(-7, curY)
				curY += subWin.GetHeight() + 1
				subWin.OnRunMouseWheel = self.OnRunMouseWheel
				subWin.OnMouseWheel = self.OnRunMouseWheel
				contentControls.append(subWin)

			headerBar.OnRunMouseWheel = self.OnRunMouseWheel
			headerBar.OnMouseWheel = self.OnRunMouseWheel
			headerClick.OnRunMouseWheel = self.OnRunMouseWheel
			headerClick.OnMouseWheel = self.OnRunMouseWheel

			container.group_id = groupId
			container.group_name = groupName
			container.headerBar = headerBar
			container.headerTitle = headerTitle
			container.copyBtn = copyBtn
			container.arrowBtn = arrowBtn
			container.headerClick = headerClick
			container.contentControls = contentControls
			container.originalHeight = curY
			container.OnRunMouseWheel = self.OnRunMouseWheel
			container.OnMouseWheel = self.OnRunMouseWheel

			is_first_group = (optionIndex == 3 and groupIndex < 2)
			container.is_expanded = is_first_group

			if is_first_group:
				container.SetSize(352, container.originalHeight)
				for w in contentControls:
					w.Show()
				arrowBtn.SetUpVisual(IMG_DIR + "arrow_up.png")
				arrowBtn.SetOverVisual(IMG_DIR + "arrow_up.png")
				arrowBtn.SetDownVisual(IMG_DIR + "arrow_up.png")
			else:
				container.SetSize(352, 29)
				for w in contentControls:
					w.Hide()
				arrowBtn.SetUpVisual(IMG_DIR + "arrow_down.png")
				arrowBtn.SetOverVisual(IMG_DIR + "arrow_down.png")
				arrowBtn.SetDownVisual(IMG_DIR + "arrow_down.png")

			container.Show()
			scriptWindow.optionGroups.append(container)

	def __ToggleOptionGroup(self, scriptWindow, targetGroup):
		if not targetGroup:
			return
		is_expanded = getattr(targetGroup, "is_expanded", False)
		new_state = not is_expanded
		targetGroup.is_expanded = new_state

		arrowBtn = getattr(targetGroup, "arrowBtn", None)
		controls = getattr(targetGroup, "contentControls", [])

		if new_state:
			targetGroup.SetSize(352, getattr(targetGroup, "originalHeight", 80))
			for c in controls:
				if c:
					c.Show()
			if arrowBtn:
				arrowBtn.SetUpVisual(IMG_DIR + "arrow_up.png")
				arrowBtn.SetOverVisual(IMG_DIR + "arrow_up.png")
				arrowBtn.SetDownVisual(IMG_DIR + "arrow_up.png")
		else:
			targetGroup.SetSize(352, 29)
			for c in controls:
				if c:
					c.Hide()
			if arrowBtn:
				arrowBtn.SetUpVisual(IMG_DIR + "arrow_down.png")
				arrowBtn.SetOverVisual(IMG_DIR + "arrow_down.png")
				arrowBtn.SetDownVisual(IMG_DIR + "arrow_down.png")

		self.__LayoutOptionGroups(scriptWindow)
		self.UpdateScrollBar()

	def __LayoutOptionGroups(self, scriptWindow):
		groups = getattr(scriptWindow, "optionGroups", None)
		if not groups:
			groups = [c for c in getattr(scriptWindow, "Children", []) if c and not isinstance(c, ScrollBarNew) and c != getattr(scriptWindow, "scrollBar", None) and hasattr(c, "IsShow")]

		yPos = 5
		filtering = self.children.get("filteringEnabled", False)
		for group in groups:
			if not group:
				continue
			if filtering and not getattr(group, "_matches_filter", group.IsShow()):
				continue
			group.SetPosition(0, yPos)
			group.exPos = (0, yPos)
			yPos += group.GetHeight()

		scriptWindow.totalContentHeight = yPos
		return yPos

	def CreateOptionWindow(self, windowBG, optionIndex):
		scriptWindow = ui.ScriptWindow()
		scriptWindow._referenceCompact = (optionIndex in (0, 1, 2, 5))
		scriptWindow.SetParent(windowBG)
		scriptWindow.SetInsideRender(True)
		try:
			fileName = "UIScript/advanced_game_options/game_option_%d.py" % (optionIndex)
			pyScrLoader = ui.PythonScriptLoader()
			if pack.Exist(fileName):
				pyScrLoader.LoadScriptFile(scriptWindow, fileName)
				self.LoadCategoryOptions(scriptWindow, optionIndex)
				self.__AdjustButtonTextPositionsOnce(scriptWindow)
				self.__CreateOptionGroups(scriptWindow, optionIndex)
				if optionIndex == 1:
					for legacyName in ("hide_mount", "hide_pet", "hide_buffi",
						"hide_effects_except_damage_btn", "hide_effects_from_others_players_btn",
						"hide_shaman_buff_btn", "hide_stone_effect_btn",
						"hide_monster_effect_btn", "hide_exp_effect_btn", "hide_effects_label"):
						legacy = scriptWindow.ElementDictionary.get(legacyName)
						if legacy:
							legacy.Hide()
			else:
				dbg.TraceError("[AdvancedGameOptions] Can't find script window for categoryIndex: %d" % (optionIndex))
		except Exception as msg:
			dbg.TraceError("AdvancedGameOptions.LoadScript - %s" % (msg))

		windowX, windowY = scriptWindow.GetLocalPosition()
		scriptWindow.SetSize(scriptWindow.GetWidth(), max(1, windowBG.GetHeight() - windowY - 5))
		if hasattr(scriptWindow, "SetClippingMaskWindow"):
			scriptWindow.SetClippingMaskWindow(scriptWindow)
		totalHeight = self.__LayoutOptionGroups(scriptWindow)
		scriptWindow.totalContentHeight = totalHeight

		if scriptWindow.GetWidth() > 0:
			scrollBar = ScrollBarNew()
			# Keep the stationary track outside the scrolling content's clip tree.
			scrollBar.SetParent(windowBG)
			windowX, windowY = scriptWindow.GetLocalPosition()
			scrollBar.SetPosition(windowX + scriptWindow.GetWidth() - scrollBar.GetWidth() - 6, windowY)
			scrollBar.SetScrollBarSize(scriptWindow.GetHeight())
			scrollBar.SetScrollEvent(ui.__mem_func__(self.UpdateScrollBar))
			scrollBar.Hide()
			scriptWindow.scrollBar = scrollBar
			scriptWindow.OnRunMouseWheel = self.OnRunMouseWheel
			scriptWindow.OnMouseWheel = self.OnRunMouseWheel
		else:
			scriptWindow.scrollBar = None

		return scriptWindow

	def __CopyOptionGroupHyperlink(self, optionIndex, groupId, groupName):
		link = "|cffffc700|Hgameoption:%d:%s|h[Game Option: %s]|h|r" % (optionIndex, groupId, groupName)
		_SetClipboardText(link)
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Ayar linki panoya kopyalandı.")

	def OpenOptionGroup(self, groupIdOrName):
		curWin = self.GetCurrentWindow()
		if not curWin:
			return
		groups = getattr(curWin, "optionGroups", [])
		for g in groups:
			if getattr(g, "group_id", "") == groupIdOrName or getattr(g, "group_name", "") == groupIdOrName:
				if not getattr(g, "is_expanded", False):
					self.__ToggleOptionGroup(curWin, g)
				scrollBar = getattr(curWin, "scrollBar", None)
				exPos = getattr(g, "exPos", None)
				if scrollBar and scrollBar.IsShow() and exPos:
					totalH = getattr(curWin, "totalContentHeight", 0)
					winH = curWin.GetHeight()
					if totalH > winH:
						scrollBar.SetPos(float(max(0, exPos[1] - 5)) / float(totalH - winH))
						self.UpdateScrollBar()
				break

	def __ClickResetConfig(self):
		import uiCommon
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText("Tüm oyun seçeneklerini varsayılana sıfırlamak istiyor musunuz?")
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__AcceptResetConfig))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

	def __CloseQuestionDialog(self):
		if hasattr(self, "questionDialog") and self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

	def __AcceptResetConfig(self):
		advanced_options_runtime.Reset()
		constInfo.ADVANCED_OPTIONS_SETTINGS = {}
		self.__RefreshAllOptionStates()
		self.__CloseQuestionDialog()
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Oyun seçenekleri varsayılan değerlere sıfırlandı.")

	def __RefreshAllOptionStates(self):
		for j in xrange(len(option_data)):
			key = "optionWindow%d" % j
			if self.children.has_key(key):
				self.LoadCategoryOptions(self.children[key], j)

	def __BindToggle(self, btn, settingKey, defaultVal=False, callback=None):
		if not btn:
			return
		self.toggleControls[settingKey] = btn
		advanced_options_runtime.RegisterDefault(settingKey, defaultVal)
		if not advanced_options_runtime.IsAvailable(settingKey):
			btn.SetUp()
			btn.SetToolTipText("Hazir degil: " + advanced_options_runtime.UNAVAILABLE_OPTIONS[settingKey])
			btn.Disable()
			return
		btn.Enable()
		if settingKey == "auto_cape_after_warp":
			btn.SetToolTipText("Baslat: /auto_cape start | Durdur: /auto_cape stop")
		val = GetOptSetting(settingKey, defaultVal)
		if val:
			btn.Down()
		else:
			btn.SetUp()
		def OnToggle():
			if not advanced_options_runtime.IsAvailable(settingKey):
				advanced_options_runtime.ExplainUnavailable(settingKey)
				btn.Down() if GetOptSetting(settingKey, defaultVal) else btn.SetUp()
				return
			current = GetOptSetting(settingKey, defaultVal)
			newVal = not current
			try:
				SetOptSetting(settingKey, newVal)
			except Exception as error:
				dbg.TraceError("Option %s failed: %s" % (settingKey, error))
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Se\xe7enek uygulanamad\xfd: %s" % settingKey)
				btn.Down() if current else btn.SetUp()
				return
			if newVal:
				btn.Down()
			else:
				btn.SetUp()
			if callback:
				try:
					callback(newVal)
				except Exception as e:
					dbg.TraceError("__BindToggle callback %s: %s" % (settingKey, str(e)))
		btn.SetToggleUpEvent(OnToggle)
		btn.SetToggleDownEvent(OnToggle)

	def __BindRadio(self, btnList, settingKey, defaultIdx=0, callback=None):
		if not btnList:
			return
		advanced_options_runtime.RegisterDefault(settingKey, defaultIdx)
		curIdx = GetOptSetting(settingKey, defaultIdx)
		if curIdx >= len(btnList):
			curIdx = 0
		for idx, btn in enumerate(btnList):
			if not btn:
				continue
			if idx == curIdx:
				btn.Down()
			else:
				btn.SetUp()
			def MakeHandler(i):
				def OnClick():
					if not advanced_options_runtime.IsAvailable(settingKey):
						advanced_options_runtime.ExplainUnavailable(settingKey)
						return
					try:
						SetOptSetting(settingKey, i)
					except Exception as error:
						dbg.TraceError("Option %s failed: %s" % (settingKey, error))
						chat.AppendChat(chat.CHAT_TYPE_INFO, "Se\xe7enek uygulanamad\xfd: %s" % settingKey)
						return
					for j, b in enumerate(btnList):
						if b:
							if j == i:
								b.Down()
							else:
								b.SetUp()
					if callback:
						try:
							callback(i)
						except Exception as e:
							dbg.TraceError("__BindRadio callback %s: %s" % (settingKey, str(e)))
				return OnClick
			btn.SetEvent(MakeHandler(idx))

	def LoadCategoryOptions(self, scriptWindow, optionIndex):
		try:
			if not hasattr(scriptWindow, '_text_positions_adjusted'):
				self.__AdjustButtonTextPositionsOnce(scriptWindow)
				scriptWindow._text_positions_adjusted = True

			elementDict = getattr(scriptWindow, "ElementDictionary", None)
			def getOpt(name):
				try:
					if elementDict and name in elementDict:
						return elementDict[name]
					if hasattr(scriptWindow, "GetChild2"):
						return scriptWindow.GetChild2(name)
					if hasattr(scriptWindow, "ElementDictionary") and name in scriptWindow.ElementDictionary:
						return scriptWindow.ElementDictionary[name]
				except Exception:
					pass
				return None

			filtering = self.children.get("filteringEnabled", False)

			# -------------------------------------------------------------
			# CATEGORY 0: ARAYUZ / INTERFACE (game_option_0.py)
			# -------------------------------------------------------------
			if optionIndex == 0 or filtering:
				kb_btn = getOpt("keyboard_btn")
				if kb_btn:
					kb_btn.SAFE_SetEvent(self.__OnClickKeyboardSettings)

				view_chat = getOpt("view_chat_off_button")
				if view_chat:
					self.viewChatButtonList = view_chat
					view_chat.SetToggleUpEvent(self.__OnClickViewChat)
					view_chat.SetToggleDownEvent(self.__OnClickViewChat)
					self.RefreshViewChat()

				self.__BindToggle(getOpt("hide_dungeon_announcement_on_off_btn"), "hide_dungeon_announcement", False)
				self.__BindToggle(getOpt("view_drop_ui_off_button"), "view_drop_ui", False)
				self.__BindToggle(getOpt("view_guideline_btn_off_button"), "view_guideline", True)
				self.__BindToggle(getOpt("view_system_menu_off_button"), "view_system_menu", True)
				self.__BindToggle(getOpt("view_event_icon_off_button"), "view_event_icon", True)

				ws_ctrl = getOpt("window_scale_controller")
				if ws_ctrl:
					ws_ctrl.SetSliderPos(GetOptSetting("window_scale_pos", 0.5))
					ws_ctrl.SetEvent(ui.__mem_func__(self.OnChangeWindowScale))
				ws_apply = getOpt("window_scale_apply_button")
				if ws_apply:
					ws_apply.SAFE_SetEvent(self.OnClickWindowScaleApply)

				f0 = getOpt("font_btn_0")
				f1 = getOpt("font_btn_1")
				if f0 and f1:
					self.__BindRadio([f0, f1], "font_type_index", 0, self.__OnChangeFont)

				self.__BindToggle(getOpt("big_font_size_button"), "big_font_size", False)
				self.__BindToggle(getOpt("big_slot_count_button"), "big_slot_count", False)
				self.__BindToggle(getOpt("save_status_off_button"), "save_status_window", True)
				self.__BindToggle(getOpt("small_affect_icon_off_button"), "small_affect_icon", False)
				self.__BindToggle(getOpt("guild_quest_off_button"), "guild_quest_window", True)
				self.__BindToggle(getOpt("view_whisper_sender_letter_off_button"), "whisper_sender_letter", True)
				self.__BindToggle(getOpt("guild_chat_to_whisper_window_off_button"), "guild_chat_to_whisper", False)
				self.__BindToggle(getOpt("small_whisper_window_off_button"), "small_whisper_window", False)
				self.__BindToggle(getOpt("show_guild_join_notification_on_off_btn"), "show_guild_join_notification", True)

			# -------------------------------------------------------------
			# CATEGORY 1: OYUN / GAME (game_option_1.py)
			# -------------------------------------------------------------
			if optionIndex == 1 or filtering:
				self.pvpModeButtonDict = {}
				for pkMode, btnName in [(player.PK_MODE_PEACE, "pvp_peace"),
				                        (player.PK_MODE_REVENGE, "pvp_revenge"),
				                        (player.PK_MODE_GUILD, "pvp_guild"),
				                        (player.PK_MODE_FREE, "pvp_free")]:
					btn = getOpt(btnName)
					if btn:
						self.pvpModeButtonDict[pkMode] = btn
				if player.PK_MODE_PEACE in self.pvpModeButtonDict:
					self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
				if player.PK_MODE_REVENGE in self.pvpModeButtonDict:
					self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
				if player.PK_MODE_GUILD in self.pvpModeButtonDict:
					self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
				if player.PK_MODE_FREE in self.pvpModeButtonDict:
					self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)
				self.__RefreshPVPButtonList()

				self.__BindToggle(getOpt("auto_unequip_buffi_pvp_on_off_btn"), "auto_unequip_buffi_pvp", False)
				self.__BindToggle(getOpt("auto_equip_buffi_pvm_on_off_btn"), "auto_equip_buffi_pvm", True)

				sr_slider = getOpt("shop_range_slider")
				if sr_slider:
					sr_slider.SetSliderPos(GetOptSetting("shop_range_pos", 1.0))
					sr_slider.SetEvent(ui.__mem_func__(self.OnChangeShopRange))

				self.blockButtonList = []
				for bName, bHandler in [("block_exchange_button", self.__OnClickBlockExchangeButton),
				                        ("block_party_button", self.__OnClickBlockPartyButton),
				                        ("block_guild_button", self.__OnClickBlockGuildButton),
				                        ("block_whisper_button", self.__OnClickBlockWhisperButton),
				                        ("block_friend_button", self.__OnClickBlockFriendButton),
				                        ("block_party_request_button", self.__OnClickBlockPartyRequest)]:
					bBtn = getOpt(bName)
					if bBtn:
						bBtn.SetToggleUpEvent(bHandler)
						bBtn.SetToggleDownEvent(bHandler)
						self.blockButtonList.append(bBtn)
				self.RefreshBlock()

				names_far = getOpt("names_far_on_off_btn")
				if names_far:
					self.alwaysShowNameButtonList = [names_far]
					names_far.SetToggleUpEvent(self.__OnClickAlwaysShowNameToggle)
					names_far.SetToggleDownEvent(self.__OnClickAlwaysShowNameToggle)
					self.RefreshAlwaysShowNameToggle()

				self.__BindToggle(getOpt("names_monsters_fov_on_off_btn"), "names_monsters_fov", False)
				self.__BindToggle(getOpt("show_country_flag_on_off_btn"), "show_country_flag", True)

				dmg_btn = getOpt("damage_on_off_btn")
				if dmg_btn:
					self.showDamageButtonList = dmg_btn
					dmg_btn.SetToggleUpEvent(self.__OnClickShowDamage)
					dmg_btn.SetToggleDownEvent(self.__OnClickShowDamage)
					self.RefreshShowDamage()

				self.__BindToggle(getOpt("damage_monster_on_off_btn"), "damage_monster", True)
				self.__BindToggle(getOpt("colored_damage_on_off_btn"), "colored_damage", True)

				cd_color_btn = getOpt("colored_damage_color_button")
				if cd_color_btn:
					# The original 139px button clips the final Turkish glyph at the default font size.
					cd_color_btn.SetText(localeInfo.GAME_OPTION_DAMAGE_COLORS_OPEN)
					if cd_color_btn.ButtonText:
						cd_color_btn.ButtonText.SetFontName("Tahoma:11")
					cd_color_btn.SAFE_SetEvent(self.__OnClickColoredDamageColor)

				sd_slider = getOpt("size_damage")
				if sd_slider:
					sd_slider.SetSliderPos(GetOptSetting("size_damage_pos", 0.5))
					sd_slider.SetEvent(ui.__mem_func__(self.OnChangeDamageSize))
				ds_slider = getOpt("damage_spacing")
				if ds_slider:
					ds_slider.SetSliderPos(GetOptSetting("damage_spacing_pos", 0.5))
					ds_slider.SetEvent(ui.__mem_func__(self.OnChangeDamageSpacing))

				stone_scale_ctrl = getOpt("stones_scale_controller")
				if stone_scale_ctrl:
					self.stoneScaleController = stone_scale_ctrl
					stone_scale_ctrl.SetEvent(ui.__mem_func__(self.OnChangeStoneScale))
					if hasattr(systemSetting, 'GetStoneScale'):
						stone_scale_ctrl.SetSliderPos(float(systemSetting.GetStoneScale()))
					else:
						stone_scale_ctrl.SetSliderPos(GetOptSetting("stone_scale_pos", 0.0))

				self.__BindToggle(getOpt("hide_mount"), "hide_mount", False)
				pet_toggle = getOpt("hide_pet")
				if pet_toggle:
					self.__BindToggle(pet_toggle, "hide_pet", False)
				self.__BindToggle(getOpt("hide_buffi"), "hide_buffi", False)

				qm_color = getOpt("queue_color_button")
				if qm_color:
					qm_color.SAFE_SetEvent(self.__OnClickQueueColor)
				queueWindow = getOpt("queue_metin_window")
				if queueWindow and not getattr(scriptWindow, "numberedEffectButton", None):
					for name, y in (("queue_color_label", 102), ("queue_color_button", 125),
						("queue_flash_toggle", 160)):
						control = getOpt(name)
						if control:
							control.SetPosition(control.GetLocalPosition()[0], y)
					numbered = ui.RadioButton()
					numbered.SetParent(queueWindow)
					numbered.SetWindowName("queue_numbered_effect")
					numbered.SetPosition(17, 77)
					numbered.SetUpVisual(IMG_DIR + "radio_unselected.png")
					numbered.SetOverVisual(IMG_DIR + "radio_unselected.png")
					numbered.SetDownVisual(IMG_DIR + "radio_selected.png")
					numbered.SetSize(110, 12)
					numbered.SetTextAlignLeft("Numaral\xfd efekt", 25)
					if numbered.ButtonText:
						numbered.ButtonText.SetPackedFontColor(0xffcebb98)
					numbered.Show()
					queueWindow.Children.append(numbered)
					scriptWindow.numberedEffectButton = numbered
				effects = [getOpt(name) for name in ("effect_1", "effect_2", "effect_3")]
				effects.append(getattr(scriptWindow, "numberedEffectButton", None))
				if all(effects):
					self.__BindRadio(effects, "queue_effect_index", 0)
				self.__BindToggle(getOpt("queue_flash_toggle"), "queue_flash", True)

				self.__BindToggle(getOpt("mining_finish_flash_toggle"), "mining_finish_flash", True)

				es0, es1, es2 = getOpt("event_stone_original"), getOpt("event_stone_model_1"), getOpt("event_stone_model_2")
				if es0 and es1 and es2:
					self.__BindRadio([es0, es1, es2], "event_stone_index", 0)

				self.__BindToggle(getOpt("hide_effects_except_damage_btn"), "hide_effects_except_damage", False)
				self.__BindToggle(getOpt("hide_effects_from_others_players_btn"), "hide_effects_from_others_players", False)
				self.__BindToggle(getOpt("hide_shaman_buff_btn"), "hide_shaman_buff", False)
				self.__BindToggle(getOpt("hide_stone_effect_btn"), "hide_stone_effect", False)
				self.__BindToggle(getOpt("hide_monster_effect_btn"), "hide_monster_effect", False)
				self.__BindToggle(getOpt("hide_exp_effect_btn"), "hide_exp_effect", False)

				self.__BindToggle(getOpt("show_guild_grade_on_off_btn"), "show_guild_grade", True)
				self.__BindToggle(getOpt("view_notification_history_button_off_button"), "view_notification_history", True)
				self.__BindToggle(getOpt("auto_cape_after_warp_on_off_btn"), "auto_cape_after_warp", False)
				self.__BindToggle(getOpt("disable_level_up_confirmation_on_off_btn"), "disable_level_up_confirmation", False)
				self.__BindToggle(getOpt("hide_metin_spawns"), "hide_metin_spawns", False)
				self.__BindToggle(getOpt("auto_hide_companion_map1"), "auto_hide_companion_map1", False)
				self.__SetupGameReferenceDropdowns(scriptWindow, getOpt)

			# -------------------------------------------------------------
			# CATEGORY 2: GRAFIK & CEVRE (game_option_2.py)
			# -------------------------------------------------------------
			if optionIndex == 2 or filtering:
				self.__BindToggle(getOpt("anti_aliasing_toggle"), "anti_aliasing", True)

				self.graphicsLevelButtonList = []
				for i in xrange(3):
					gl_btn = getOpt("graphics_level_%d" % i)
					if gl_btn:
						self.graphicsLevelButtonList.append(gl_btn)
						gl_btn.SAFE_SetEvent(self.__OnClickGraphicsLevel, i)
				if len(self.graphicsLevelButtonList) > 0:
					self.RefreshGraphicsLevel()

				self.shadowQualityButtonList = []
				for i in xrange(4):
					sq_btn = getOpt("shadow_quality_%d" % i)
					if sq_btn:
						self.shadowQualityButtonList.append(sq_btn)
						sq_btn.SAFE_SetEvent(self.__OnClickShadowQuality, i)
				if len(self.shadowQualityButtonList) > 0:
					self.RefreshShadowQuality()

				self.__BindToggle(getOpt("shadow_caster_player"), "shadow_caster_player", True)
				self.__BindToggle(getOpt("shadow_caster_terrain"), "shadow_caster_terrain", True)
				self.__BindToggle(getOpt("shadow_caster_trees"), "shadow_caster_trees", True)
				self.__BindToggle(getOpt("sun_rays_toggle"), "sun_rays", True)
				self.__BindToggle(getOpt("wet_ground_toggle"), "wet_ground", True)
				self.__BindToggle(getOpt("rain_toggle"), "rain", False)
				self.__BindToggle(getOpt("storm_toggle"), "storm", False)
				self.__BindToggle(getOpt("shooting_stars_toggle"), "shooting_stars", True)
				self.__BindToggle(getOpt("water_ripple_toggle"), "water_ripple", True)
				self.__BindToggle(getOpt("enhanced_water_toggle"), "enhanced_water", True)
				self.__BindToggle(getOpt("underwater_toggle"), "underwater", True)
				self.__BindToggle(getOpt("specular_reflection_toggle"), "specular_reflection", True)
				self.__BindToggle(getOpt("water_reflection_toggle"), "water_reflection", True)
				self.__BindToggle(getOpt("local_light_toggle"), "local_light", True)
				self.__BindToggle(getOpt("death_cam_effect"), "death_cam_effect", True)

				self.__BindToggle(getOpt("speedtree_dynamic_light"), "speedtree_dynamic_light", True)
				self.__BindToggle(getOpt("speedtree_wind"), "speedtree_wind", True)
				self.__BindToggle(getOpt("speedtree_enhanced_foliage"), "speedtree_enhanced_foliage", True)
				self.__BindToggle(getOpt("speedtree_falling_leaves"), "speedtree_falling_leaves", True)

				sharp_ctrl = getOpt("sharpness_controller")
				if sharp_ctrl:
					self.sharpnessController = sharp_ctrl
					sharp_ctrl.SetEvent(ui.__mem_func__(self.OnChangeSharpness))
					if hasattr(systemSetting, 'GetSharpnessPower'):
						sharpness_power = systemSetting.GetSharpnessPower()
						sharp_ctrl.SetSliderPos(max(0.0, min(sharpness_power, 5.0)) / 5.0)
					else:
						sharp_ctrl.SetSliderPos(GetOptSetting("sharpness_pos", 0.0))

				fov_ctrl = getOpt("camera_fov_controller")
				if fov_ctrl:
					self.fovController = fov_ctrl
					fov_ctrl.SetEvent(ui.__mem_func__(self.OnChangeFOV))
					if hasattr(systemSetting, 'GetFOV'):
						if hasattr(app, 'MAX_CAMERA_PERSPECTIVE'):
							fov_ctrl.SetSliderPos(float(systemSetting.GetFOV()) / float(app.MAX_CAMERA_PERSPECTIVE))
						else:
							fov_ctrl.SetSliderPos(float(systemSetting.GetFOV()) / 100.0)
					else:
						fov_ctrl.SetSliderPos(GetOptSetting("fov_pos", 0.5))

				cam_far = getOpt("camera_far_on_off_btn")
				if cam_far:
					self.cameraFarButton = cam_far
					cam_far.SetToggleUpEvent(self.__OnClickCameraFarToggle)
					cam_far.SetToggleDownEvent(self.__OnClickCameraFarToggle)
					self.RefreshCameraFarToggle()

				kcs_ctrl = getOpt("keyboard_camera_speed_controller")
				if kcs_ctrl:
					kcs_ctrl.SetSliderPos(GetOptSetting("keyboard_camera_speed_pos", 0.5))
					kcs_ctrl.SetEvent(ui.__mem_func__(self.OnChangeCameraSpeed))

				sky_holder = getOpt("skybox_combo_holder")
				if sky_holder:
					self.__SetupSkyboxComboBox(sky_holder)

				night_btn = getOpt("night_mode_button")
				if night_btn:
					self.nightModeButton = night_btn
					night_btn.SetToggleUpEvent(self.__OnClickNightModeToggle)
					night_btn.SetToggleDownEvent(self.__OnClickNightModeToggle)
					self.RefreshNightModeToggle()

				fog_btn = getOpt("remove_fog_button")
				if fog_btn:
					self.removeFogButton = fog_btn
					fog_btn.SetToggleUpEvent(self.__OnClickRemoveFogToggle)
					fog_btn.SetToggleDownEvent(self.__OnClickRemoveFogToggle)
					self.RefreshRemoveFogToggle()

				for sName, sKey in [("environment_darkness_controller", "env_darkness"),
				                    ("environment_saturation_controller", "env_saturation"),
				                    ("environment_contrast_controller", "env_contrast"),
				                    ("environment_temperature_controller", "env_temperature")]:
					sCtrl = getOpt(sName)
					if sCtrl:
						sCtrl.SetSliderPos(GetOptSetting(sKey + "_pos", 0.5))
						def MakeSliderHandler(k, ctrlRef):
							return lambda: SetOptSetting(k + "_pos", ctrlRef.GetSliderPos())
						sCtrl.SetEvent(MakeSliderHandler(sKey, sCtrl))

				env_color_btn = getOpt("environment_color_button")
				if env_color_btn:
					env_color_btn.SAFE_SetEvent(self.__OnClickEnvironmentColor)

			# -------------------------------------------------------------
			# CATEGORY 3: SES / SOUND (game_option_3.py)
			# -------------------------------------------------------------
			if optionIndex == 3 or filtering:
				snd_ctrl = getOpt("sound_volume_controller")
				if snd_ctrl:
					self.ctrlSoundVolume = snd_ctrl
					snd_ctrl.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
					snd_ctrl.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))

				mus_ctrl = getOpt("music_volume_controller")
				if mus_ctrl:
					self.ctrlMusicVolume = mus_ctrl
					mus_ctrl.SetSliderPos(float(systemSetting.GetMusicVolume()))
					mus_ctrl.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))

				env_ctrl = getOpt("environment_volume_controller")
				if env_ctrl:
					self.ctrlEnvVolume = env_ctrl
					env_ctrl.SetSliderPos(GetOptSetting("env_volume_pos", 0.5))
					env_ctrl.SetEvent(ui.__mem_func__(self.OnChangeEnvVolume))

				bgm_btn = getOpt("bgm_button")
				if bgm_btn:
					self.selectMusicButton = bgm_btn
					bgm_btn.SetToggleDownEvent(self.__OnClickChangeMusicButton)

				bgm_file = getOpt("bgm_file")
				if bgm_file:
					self.selectMusicFile = bgm_file
					if musicInfo.fieldMusic != "":
						self.selectMusicFile.SetText("Metin2 Standart M\xfcz\xfd\xf0i" if musicInfo.fieldMusic == musicInfo.METIN2THEMA else musicInfo.fieldMusic[:MUSIC_FILENAME_MAX_LEN])

				for category in ("hits", "critical", "skills", "movement", "interface", "voices", "monsters"):
					categorySlider = getOpt("sound_%s_volume_controller" % category)
					if categorySlider:
						optionKey = "sound_category_" + category
						categorySlider.SetSliderPos(GetOptSetting(optionKey, 1.0))
						def MakeSoundHandler(slider, key):
							def OnChange():
								SetOptSetting(key, slider.GetSliderPos())
							return OnChange
						categorySlider.SetEvent(MakeSoundHandler(categorySlider, optionKey))

			# -------------------------------------------------------------
			# CATEGORY 4: DIL / LANGUAGE (game_option_4.py)
			# -------------------------------------------------------------
			if optionIndex == 4 or filtering:
				self.languageButtonList = {}
				langCodes = ["en", "pt", "es", "fr", "de", "ro", "pl", "it", "cz", "HU", "tr", "ae"]
				for code in langCodes:
					lBtn = getOpt("language_%s" % code)
					if lBtn:
						self.languageButtonList[code.lower()] = lBtn
						lBtn.SAFE_SetEvent(self.__OnClickLanguage, code.lower())
				self.RefreshLanguage()

			# -------------------------------------------------------------
			# CATEGORY 5: PERFORMANS (game_option_5.py)
			# -------------------------------------------------------------
			if optionIndex == 5 or filtering:
				self.__BindToggle(getOpt("high_performance_crowd_mode"), "high_performance_crowd_mode", False, self.__OnToggleCrowdPerformance)

				self.fpsLimitButtonList = {}
				for fps in [60, 120, 144, 200]:
					fpsBtn = getOpt("fps_limit_%d" % fps)
					if fpsBtn:
						self.fpsLimitButtonList[fps] = fpsBtn
						fpsBtn.SAFE_SetEvent(self.__OnClickFPSLimit, fps)
				self.RefreshFPSLimit()

				self.__BindToggle(getOpt("hide_fps_minimap"), "hide_fps_minimap", False, self.__OnToggleHideFPSMinimap)

		except Exception as msg:
			dbg.TraceError("LoadCategoryOptions : %s" % (msg))

	def __SetupGameReferenceDropdowns(self, scriptWindow, getOpt):
		# The Baerim script ships anchors; its game code supplies these menus.
		if getattr(scriptWindow, "referenceGameCombos", None):
			return
		for name in ("hide_mount", "hide_pet", "hide_buffi",
			"hide_effects_except_damage_btn", "hide_effects_from_others_players_btn",
			"hide_shaman_buff_btn", "hide_stone_effect_btn", "hide_monster_effect_btn",
			"hide_exp_effect_btn", "hide_effects_label"):
			control = getOpt(name)
			if control:
				control.Hide()

		class OverlayComboBox(ui.ComboBox):
			def __init__(self, overlayParent):
				ui.ComboBox.__init__(self)
				self.overlayParent = overlayParent
				# The option rows are clipped to their fixed height. Keep the
				# expanded list on the dialog so it can cover following rows.
				self.listBox.SetParent(overlayParent)

			def OnMouseLeftButtonUp(self):
				ui.ComboBox.OnMouseLeftButtonUp(self)
				if self.isListOpened:
					cx, cy = self.GetGlobalPosition()
					px, py = self.overlayParent.GetGlobalPosition()
					listY = cy - py + self.GetHeight() + 5
					if listY + self.listBox.GetHeight() > self.overlayParent.GetHeight() - 8:
						listY = cy - py - self.listBox.GetHeight() - 5
					self.listBox.SetPosition(cx - px, max(25, listY))
					self.listBox.SetTop()

		def makeCombo(anchor, names, callback, selected):
			if not anchor:
				return None
			anchor.SetSize(190, 22)
			combo = OverlayComboBox(self)
			combo.SetParent(anchor)
			combo.SetPosition(0, 0)
			combo.SetSize(190, 22)
			for index, name in enumerate(names):
				combo.InsertItem(index, name)
			combo.SetEvent(callback)
			combo.SetCurrentItem(names[selected])
			combo.Show()
			arrow = ui.ImageBox()
			arrow.SetParent(combo)
			arrow.AddFlag("not_pick")
			arrow.LoadImage(IMG_DIR + "arrow_down.png")
			arrow.SetPosition(175, 7)
			arrow.Show()
			combo.optionArrow = arrow
			return combo

		companionKeys = ("hide_mount", "hide_pet", "hide_buffi")
		companionNames = ("Binekleri Gizle", "Petleri Gizle", "Buffileri gizle")
		companionCombo = [None]
		def refreshCompanion():
			combo = companionCombo[0]
			if not combo: return
			states = [bool(GetOptSetting(key, False)) for key in companionKeys]
			combo.ClearItem()
			for index, name in enumerate(companionNames):
				combo.InsertItem(index, ("[x] " if states[index] else "[ ] ") + name)
			combo.SetCurrentItem("Nothing hidden" if not any(states) else "%d gizli" % sum(states))
		def selectCompanion(index):
			if index < 0 or index >= len(companionKeys): return
			key = companionKeys[index]
			if advanced_options_runtime.IsAvailable(key):
				SetOptSetting(key, not GetOptSetting(key, False))
			else:
				advanced_options_runtime.ExplainUnavailable(key)
			refreshCompanion()
		companionCombo[0] = makeCombo(getOpt("hide_companion_dropdown_anchor"), companionNames,
			selectCompanion, 0)
		refreshCompanion()

		effectKeys = ("hide_effects_except_damage", "hide_effects_from_others_players",
			"hide_shaman_buff", "hide_stone_effect", "hide_monster_effect", "hide_exp_effect",
			"hide_pvp_team_indicators")
		effectNames = ("Tüm Efektler", "Diðer Oyuncularýn Efektleri",
			"Þaman Buff Gizle", "Metin Efektlerini Gizle",
			"Canavar Efektlerini Gizle", "EXP Efektini Gizle", "PvP Takým Göstergeleri")
		effectCombo = [None]
		def refreshEffect():
			combo = effectCombo[0]
			if not combo: return
			states = [bool(GetOptSetting(key, False)) for key in effectKeys]
			combo.ClearItem()
			for index, name in enumerate(effectNames):
				combo.InsertItem(index, ("[x] " if states[index] else "[ ] ") + name)
			combo.SetCurrentItem("No Effect Hidden" if not any(states) else "%d efekt gizli" % sum(states))
		def selectEffect(index):
			if index < 0 or index >= len(effectKeys): return
			key = effectKeys[index]
			if advanced_options_runtime.IsAvailable(key):
				SetOptSetting(key, not GetOptSetting(key, False))
			else:
				advanced_options_runtime.ExplainUnavailable(key)
			refreshEffect()
		effectCombo[0] = makeCombo(getOpt("hide_effects_dropdown_anchor"), effectNames,
			selectEffect, 0)
		refreshEffect()

		notificationPanel = getOpt("notification_settings_window")
		notificationAnchor = None
		if notificationPanel:
			notificationAnchor = ui.Window()
			notificationAnchor.SetParent(notificationPanel)
			notificationAnchor.SetWindowName("notification_dropdown_anchor")
			notificationAnchor.SetPosition(17, 55)
			notificationAnchor.SetSize(190, 22)
			notificationAnchor.Show()
			notificationPanel.Children.append(notificationAnchor)
		notificationNames = ("Guideline", "Mission Book", "Battle Pass", "Item Expired",
			"Companion Expired", "Alchemy Expired", "Bonus 6/7 Finished",
			"Fishing", "Market Sales", "Otomatik Pelerin")
		notificationKeys = ("guideline", "mission_book", "battle_pass", "item_expired",
			"companion_expired", "alchemy_expired", "bonus_67_finished",
			"fishing", "market_sales", "auto_cape")
		notificationCombo = [None]
		def selectNotification(index):
			if index < 0 or index >= len(notificationNames): return
			key = "hidden_notification_"+notificationKeys[index]
			if advanced_options_runtime.IsAvailable(key):
				SetOptSetting(key, not GetOptSetting(key, False))
			else:
				advanced_options_runtime.ExplainUnavailable(key)
			notificationCombo[0].SetCurrentItem("All Notifications")
		notificationCombo[0] = makeCombo(notificationAnchor, notificationNames,
			selectNotification, 0)
		if notificationCombo[0]:
			notificationCombo[0].SetCurrentItem("All Notifications")
		scriptWindow.referenceGameCombos = (companionCombo[0], effectCombo[0], notificationCombo[0])

	def __AdjustButtonTextPositionsOnce(self, scriptWindow):
		try:
			elementDictionary = getattr(scriptWindow, "ElementDictionary", {})
			compactGame = getattr(scriptWindow, "_referenceCompact", False)
			processed = set()

			def processElement(elem):
				if not elem or id(elem) in processed:
					return
				processed.add(id(elem))

				# Text windows need real bounds for INSIDE_RENDER clipping.
				if isinstance(elem, ui.TextLine):
					textWidth, textHeight = elem.GetTextSize()
					if compactGame and ("description" in elem.GetWindowName() or elem.GetWindowName().endswith("_desc")):
						# Reference rows use one-line clipped descriptions; UIScript's
						# adjust_text loader enables wrapping by default.
						wndMgr.SetMultiLine(elem.hWnd, 0)
						elem.SetSize(max(elem.GetWidth(), textWidth), 14)
					else:
						elem.SetSize(max(elem.GetWidth(), textWidth), max(14, elem.GetHeight(), textHeight))

				# 1. SliderBar_AdvancedGameOptions
				if isinstance(elem, ui.SliderBar_AdvancedGameOptions):
					if hasattr(elem, "backGroundImage") and elem.backGroundImage:
						elem.SetSize(elem.backGroundImage.GetWidth() + 60, max(20, elem.backGroundImage.GetHeight()))
					if hasattr(elem, "percentage") and elem.percentage:
						elem.percentage.Show()
						elem.percentage.SetOutline()

				# 2. ToggleButton
				elif isinstance(elem, ui.ToggleButton):
					children = getattr(elem, "Children", [])
					if len(children) > 0:
						elem.SetSize(330, max(30 if compactGame else 26, elem.GetHeight()))
						for ch in children:
							if ch:
								ch.Show()
								(chx, chy) = ch.GetLocalPosition()
								if chy < 0:
									ch.SetPosition(chx, 0)
								if isinstance(ch, ui.TextLine):
									ch.SetOutline()
									ch.Show()
						# Descriptions also need height inside their button's clip bounds.
						labelBottom = elem.GetHeight()
						for ch in children:
							if ch and isinstance(ch, ui.TextLine):
								labelBottom = max(labelBottom, ch.GetLocalPosition()[1] + max(14, ch.GetTextSize()[1]))
						elem.SetSize(330, 30 if compactGame else labelBottom)
					elif getattr(elem, "ButtonText", None):
						elem.ButtonText.Show()
						elem.ButtonText.SetOutline()
						(bx, by) = elem.ButtonText.GetLocalPosition()
						tw = elem.ButtonText.GetTextWidth() if hasattr(elem.ButtonText, "GetTextWidth") else len(elem.ButtonText.GetText()) * 7
						elem.SetSize(max(elem.GetWidth(), bx + tw + 10, 78), max(20, elem.GetHeight()))

				# 3. RadioButton
				elif isinstance(elem, ui.RadioButton):
					if getattr(elem, "ButtonText", None):
						elem.ButtonText.Show()
						elem.ButtonText.SetOutline()
						(bx, by) = elem.ButtonText.GetLocalPosition()
						tw = elem.ButtonText.GetTextWidth() if hasattr(elem.ButtonText, "GetTextWidth") else len(elem.ButtonText.GetText()) * 7
						elem.SetSize(max(elem.GetWidth(), bx + tw + 10, 78), max(20, elem.GetHeight()))

				# 4. Standard Button
				elif isinstance(elem, ui.Button):
					if getattr(elem, "ButtonText", None):
						elem.ButtonText.Show()
						elem.ButtonText.SetOutline()

				# Recurse children
				if hasattr(elem, 'Children'):
					for child in elem.Children:
						processElement(child)

			for child in getattr(scriptWindow, "Children", []):
				processElement(child)

			for childName, child in elementDictionary.items():
				processElement(child)

		except Exception as msg:
			dbg.TraceError("__AdjustButtonTextPositionsOnce : %s" % (msg))

	def GetCurrentWindow(self):
		if self.children.has_key("optionType"):
			optionType = self.children["optionType"]
			if self.children.has_key("filteringEnabled"):
				if self.children["filteringEnabled"]:
					return self.children.get("filteredWindow", self.children.get("optionWindow%d" % optionType))
				else:
					if self.children.has_key("optionWindow%d" % (optionType)):
						return self.children["optionWindow%d" % (optionType)]
		return None

	def GetCurrentScrollBar(self):
		window = self.GetCurrentWindow()
		if window != None:
			return window.scrollBar
		return None

	def OnRunMouseWheel(self, nLen):
		scrollBar = self.GetCurrentScrollBar()
		if scrollBar and scrollBar.IsShow():
			if nLen > 0:
				scrollBar.OnUp()
			else:
				scrollBar.OnDown()
			return True
		return False

	def OnScroll(self):
		pass

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)

	def OnMouseWheelCategories(self, nLen):
		return self.OnRunMouseWheel(nLen)

	def GoToGroupIndex(self, groupIndex):
		window = self.GetCurrentWindow()
		if window is None:
			return
		groups = getattr(window, "optionGroups", [])
		if groupIndex < 0 or groupIndex >= len(groups):
			return
		targetGroup = groups[groupIndex]
		if not getattr(targetGroup, "is_expanded", False):
			self.__ToggleOptionGroup(window, targetGroup)
		scrollBar = getattr(window, "scrollBar", None)
		exPos = getattr(targetGroup, "exPos", None)
		if scrollBar and scrollBar.IsShow() and exPos:
			totalH = getattr(window, "totalContentHeight", 0)
			winH = window.GetHeight()
			if totalH > winH:
				scrollBar.SetPos(float(max(0, exPos[1] - 5)) / float(totalH - winH))
				self.UpdateScrollBar()

	def GoToSubcategory(self, subcategoryName):
		window = self.GetCurrentWindow()
		if window is None:
			return
		groups = getattr(window, "optionGroups", [])
		for idx, g in enumerate(groups):
			if getattr(g, "group_name", "") == subcategoryName or getattr(g, "group_id", "") == subcategoryName:
				self.GoToGroupIndex(idx)
				return

	def UpdateScrollBar(self):
		window = self.GetCurrentWindow()
		if window is None:
			return
		scrollBar = getattr(window, "scrollBar", None)
		if scrollBar is None:
			return
		groups = getattr(window, "optionGroups", None)
		if not groups:
			groups = [c for c in getattr(window, "Children", []) if c and c != scrollBar and not isinstance(c, ScrollBarNew) and hasattr(c, "IsShow")]

		if len(groups) == 0:
			scrollBar.Hide()
			self.scrollbar_is_show = False
			return

		if hasattr(window, "SetClippingMaskWindow"):
			window.SetClippingMaskWindow(window)
		windowHeight = window.GetHeight()
		screenSize = getattr(window, "totalContentHeight", 0)

		if self.children.get("filteringEnabled", False):
			if self.children.get("filteredWindowHeight", 0) > 0:
				screenSize = self.children["filteredWindowHeight"]

		if screenSize <= 0:
			for g in groups:
				exPos = getattr(g, "exPos", None)
				if exPos is not None:
					bottom = exPos[1] + g.GetHeight()
					if bottom > screenSize:
						screenSize = bottom
			window.totalContentHeight = screenSize

		filtering = self.children.get("filteringEnabled", False)

		if screenSize > windowHeight:
			scrollLen = screenSize - windowHeight
			basePos = int(scrollBar.GetPos() * scrollLen)
			for g in groups:
				if filtering and not getattr(g, "_matches_filter", g.IsShow()):
					continue
				exPos = getattr(g, "exPos", None)
				if exPos is not None:
					(_x, _y) = exPos
					newY = _y - basePos
					g.SetPosition(_x, newY)
					g.Show()

			if not getattr(scrollBar, "lockFlag", False):
				scrollBar.SetMiddleBarSize(float(windowHeight) / float(screenSize))
			scrollBar.Show()
			scrollBar.SetTop()
			self.scrollbar_is_show = True
		else:
			for g in groups:
				if filtering and not getattr(g, "_matches_filter", g.IsShow()):
					continue
				exPos = getattr(g, "exPos", None)
				if exPos is not None:
					g.SetPosition(exPos[0], exPos[1])
					g.Show()
			scrollBar.Hide()
			self.scrollbar_is_show = False

	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)

	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()

	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL) < constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return True
		return False

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

	# -------------------------------------------------------------
	# BLOCK MODE
	# -------------------------------------------------------------
	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()

	def OnBlockMode(self, mode):
		global blockMode
		blockMode = mode
		self.RefreshBlock()

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

	def __OnClickBlockWarpRequest(self):
		self.RefreshBlock()
		global blockMode
		if hasattr(player, 'BLOCK_WARP_REQUEST'):
			net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WARP_REQUEST))
		elif hasattr(player, 'BLOCK_WARP'):
			net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WARP))

	# -------------------------------------------------------------
	# CHAT & NAMES & DAMAGE
	# -------------------------------------------------------------
	def RefreshViewChat(self):
		if not self.viewChatButtonList:
			return
		if hasattr(systemSetting, 'IsViewChat'):
			if systemSetting.IsViewChat():
				self.viewChatButtonList.Down()
			else:
				self.viewChatButtonList.SetUp()

	def __OnClickViewChat(self):
		if hasattr(systemSetting, 'SetViewChatFlag') and hasattr(systemSetting, 'IsViewChat'):
			systemSetting.SetViewChatFlag(not systemSetting.IsViewChat())
		self.RefreshViewChat()

	def RefreshAlwaysShowNameToggle(self):
		if hasattr(systemSetting, 'IsAlwaysShowName'):
			val = systemSetting.IsAlwaysShowName()
			for btn in self.alwaysShowNameButtonList:
				if btn:
					if val:
						btn.Down()
					else:
						btn.SetUp()

	def __OnClickAlwaysShowNameToggle(self):
		if hasattr(systemSetting, 'SetAlwaysShowNameFlag') and hasattr(systemSetting, 'IsAlwaysShowName'):
			systemSetting.SetAlwaysShowNameFlag(not systemSetting.IsAlwaysShowName())
		self.RefreshAlwaysShowNameToggle()

	def RefreshAlwaysShowName(self):
		self.RefreshAlwaysShowNameToggle()

	def __OnClickAlwaysShowName(self):
		self.__OnClickAlwaysShowNameToggle()

	def RefreshShowDamage(self):
		if self.showDamageButtonList:
			if hasattr(systemSetting, 'IsShowDamage'):
				if systemSetting.IsShowDamage():
					self.showDamageButtonList.Down()
				else:
					self.showDamageButtonList.SetUp()

	def __OnClickShowDamage(self):
		if hasattr(systemSetting, 'SetShowDamageFlag') and hasattr(systemSetting, 'IsShowDamage'):
			systemSetting.SetShowDamageFlag(not systemSetting.IsShowDamage())
		self.RefreshShowDamage()

	def __OnClickColoredDamageColor(self):
		advanced_options_runtime.OpenDamageColor()

	def OnChangeDamageSize(self):
		window = self.GetCurrentWindow()
		if window and window.GetChild2("size_damage"):
			SetOptSetting("size_damage_pos", window.GetChild2("size_damage").GetSliderPos())

	def OnChangeDamageSpacing(self):
		window = self.GetCurrentWindow()
		if window and window.GetChild2("damage_spacing"):
			SetOptSetting("damage_spacing_pos", window.GetChild2("damage_spacing").GetSliderPos())

	def OnChangeShopRange(self):
		window = self.GetCurrentWindow()
		ctrl = window.GetChild2("shop_range_slider") if window else None
		if ctrl:
			SetOptSetting("shop_range_pos", ctrl.GetSliderPos())

	# -------------------------------------------------------------
	# KEYBOARD & INTERFACE HELPERS
	# -------------------------------------------------------------
	def __OnClickKeyboardSettings(self):
		advanced_options_runtime.OpenKeyboard()

	def OnChangeWindowScale(self):
		pass

	def OnClickWindowScaleApply(self):
		window = self.GetCurrentWindow()
		ctrl = window.GetChild2("window_scale_controller") if window else None
		if ctrl:
			SetOptSetting("window_scale_pos", ctrl.GetSliderPos())
			self.SetCenterPosition()

	def __OnChangeFont(self, fontIdx):
		SetOptSetting("font_type_index", fontIdx)

	# -------------------------------------------------------------
	# GRAPHICS & CAMERA & ENVIRONMENT
	# -------------------------------------------------------------
	def RefreshCameraFarToggle(self):
		if self.cameraFarButton:
			val = constInfo.GET_CAMERA_MAX_DISTANCE_INDEX()
			if val:
				self.cameraFarButton.Down()
			else:
				self.cameraFarButton.SetUp()

	def __OnClickCameraFarToggle(self):
		constInfo.SET_CAMERA_MAX_DISTANCE_INDEX(not constInfo.GET_CAMERA_MAX_DISTANCE_INDEX())
		self.RefreshCameraFarToggle()

	def RefreshCameraMode(self):
		self.RefreshCameraFarToggle()

	def __OnClickCameraMode(self):
		self.__OnClickCameraFarToggle()

	def OnChangeCameraSpeed(self):
		window = self.GetCurrentWindow()
		ctrl = window.GetChild2("keyboard_camera_speed_controller") if window else None
		if ctrl:
			SetOptSetting("keyboard_camera_speed_pos", ctrl.GetSliderPos())

	def RefreshNightModeToggle(self):
		if self.nightModeButton:
			val = False
			if hasattr(systemSetting, 'GetNightModeOption'):
				val = systemSetting.GetNightModeOption()
			if val:
				self.nightModeButton.Down()
			else:
				self.nightModeButton.SetUp()

	def __OnClickNightModeToggle(self):
		if hasattr(systemSetting, 'SetNightModeOption') and hasattr(systemSetting, 'GetNightModeOption'):
			val = not systemSetting.GetNightModeOption()
			systemSetting.SetNightModeOption(val)
			if hasattr(background, 'SetNightModeOption'):
				background.SetNightModeOption(val)
		self.RefreshNightModeToggle()

	def RefreshNightMode(self):
		self.RefreshNightModeToggle()

	def __OnClickNightMode(self):
		self.__OnClickNightModeToggle()

	def RefreshRemoveFogToggle(self):
		if self.removeFogButton:
			val = GetOptSetting("remove_fog", False)
			if val:
				self.removeFogButton.Down()
			else:
				self.removeFogButton.SetUp()

	def __OnClickRemoveFogToggle(self):
		val = not GetOptSetting("remove_fog", False)
		SetOptSetting("remove_fog", val)   # advanced_options_runtime: SetFogMode + background.SetFogLevel
		self.RefreshRemoveFogToggle()

	def __OnToggleRain(self, val):
		if hasattr(background, 'SetRainLevel'):
			background.SetRainLevel(2 if val else 0)

	def __OnToggleWaterReflection(self, val):
		if hasattr(background, 'SetWaterReflection'):
			background.SetWaterReflection(1 if val else 0)

	def __OnToggleHidePet(self, val):
		if hasattr(systemSetting, 'SetPetStatusFlag'):
			systemSetting.SetPetStatusFlag(val)

	def __OnClickEnvironmentColor(self):
		advanced_options_runtime.OpenEnvironmentColor()

	# -------------------------------------------------------------
	# SHADOW QUALITY
	# -------------------------------------------------------------
	def RefreshShadowQuality(self):
		if hasattr(systemSetting, 'GetShadowQualityLevel'):
			level = systemSetting.GetShadowQualityLevel()
			if level < len(self.shadowQualityButtonList):
				self.__ClickRadioButton(self.shadowQualityButtonList, level)

	def __OnClickShadowQuality(self, index):
		if hasattr(systemSetting, 'SetShadowQualityLevel'):
			systemSetting.SetShadowQualityLevel(index)
		if hasattr(background, 'SetShadowQualityLevel'):
			background.SetShadowQualityLevel(index)
		self.RefreshShadowQuality()

	# -------------------------------------------------------------
	# SHARPNESS
	# -------------------------------------------------------------
	def OnChangeSharpness(self):
		if not self.sharpnessController:
			return
		pos = self.sharpnessController.GetSliderPos()
		if hasattr(systemSetting, 'SetSharpnessPower'):
			systemSetting.SetSharpnessPower(pos * 5.0)

	def OnClickSharpnessReset(self):
		if hasattr(systemSetting, 'SetSharpnessPower'):
			systemSetting.SetSharpnessPower(0.0)
		if self.sharpnessController:
			self.sharpnessController.SetSliderPos(0.0)

	# -------------------------------------------------------------
	# FOV & STONE SCALE
	# -------------------------------------------------------------
	def OnChangeFOV(self):
		if self.fovController:
			pos = self.fovController.GetSliderPos()
			if hasattr(app, 'MAX_CAMERA_PERSPECTIVE'):
				fovValue = pos * float(app.MAX_CAMERA_PERSPECTIVE)
			else:
				fovValue = int(pos * 100.0)
			if hasattr(systemSetting, 'SetFOV'):
				systemSetting.SetFOV(fovValue)

	def OnClickFOVReset(self):
		if hasattr(systemSetting, 'SetFOV'):
			if hasattr(app, 'DEFAULT_CAMERA_PERSPECTIVE'):
				defaultFOV = float(app.DEFAULT_CAMERA_PERSPECTIVE)
			else:
				defaultFOV = 90.0
			systemSetting.SetFOV(defaultFOV)
			if self.fovController:
				if hasattr(app, 'MAX_CAMERA_PERSPECTIVE'):
					self.fovController.SetSliderPos(defaultFOV / float(app.MAX_CAMERA_PERSPECTIVE))
				else:
					self.fovController.SetSliderPos(defaultFOV / 100.0)

	def OnChangeStoneScale(self):
		if self.stoneScaleController:
			value = self.stoneScaleController.GetSliderPos()
			if hasattr(systemSetting, 'SetStoneScale'):
				systemSetting.SetStoneScale(value)
			try:
				if hasattr(background, 'ApplyStoneScale'):
					background.ApplyStoneScale(value)
			except:
				pass

	def OnClickStoneScaleReset(self):
		if hasattr(systemSetting, 'SetStoneScale'):
			systemSetting.SetStoneScale(0)
			if self.stoneScaleController:
				self.stoneScaleController.SetSliderPos(0)
			try:
				if hasattr(background, 'ApplyStoneScale'):
					background.ApplyStoneScale(0)
			except:
				pass

	# -------------------------------------------------------------
	# SKYBOX
	# -------------------------------------------------------------
	def __SetupSkyboxComboBox(self, parent):
		try:
			class SkyboxComboBox(ui.ComboBox):
				def __init__(self, overlayParent):
					ui.ComboBox.__init__(self)
					self.overlayParent = overlayParent
					self.listBox.SetParent(overlayParent)

				def OnMouseLeftButtonUp(self):
					ui.ComboBox.OnMouseLeftButtonUp(self)
					if self.isListOpened:
						cx, cy = self.GetGlobalPosition()
						px, py = self.overlayParent.GetGlobalPosition()
						listY = cy - py + self.GetHeight() + 5
						if listY + self.listBox.GetHeight() > self.overlayParent.GetHeight() - 8:
							listY = cy - py - self.listBox.GetHeight() - 5
						self.listBox.SetPosition(cx - px, max(25, listY))
						self.listBox.SetTop()

			self.skyboxComboBox = SkyboxComboBox(self)
			self.skyboxComboBox.SetParent(parent)
			self.skyboxComboBox.SetPosition(0, 0)
			self.skyboxComboBox.SetSize(190, 20)
			for idx, (name, envFile) in enumerate(SKY_PRESETS):
				self.skyboxComboBox.InsertItem(idx, name)
			self.skyboxComboBox.SetEvent(self.__OnClickSkybox)
			curIdx = max(0, min(len(SKY_PRESETS)-1, int(GetOptSetting("sky_preset", 0))))
			constInfo.CURRENT_SKYBOX_INDEX = curIdx
			self.skyboxComboBox.SelectItem(curIdx)
			self.skyboxComboBox.SetCurrentItem(SKY_PRESETS[curIdx][0])
			self.skyboxComboBox.Show()
			arrow = ui.ImageBox()
			arrow.SetParent(self.skyboxComboBox)
			arrow.AddFlag("not_pick")
			arrow.LoadImage(IMG_DIR + "arrow_down.png")
			arrow.SetPosition(175, 7)
			arrow.Show()
			self.skyboxComboBox.optionArrow = arrow
		except Exception as e:
			dbg.TraceError("__SetupSkyboxComboBox: %s" % str(e))

	def RefreshSkybox(self):
		constInfo.CURRENT_SKYBOX_INDEX = max(0, min(len(SKY_PRESETS)-1, int(GetOptSetting("sky_preset", 0))))
		if self.skyboxComboBox:
			index = constInfo.CURRENT_SKYBOX_INDEX
			self.skyboxComboBox.SelectItem(index)
			self.skyboxComboBox.SetCurrentItem(SKY_PRESETS[index][0])

	def __OnClickSkybox(self, index):
		if index < 0 or index >= len(SKY_PRESETS):
			return
		constInfo.CURRENT_SKYBOX_INDEX = index
		SetOptSetting("sky_preset", index)   # kaydeder + advanced_options_runtime.ApplySky()
		self.RefreshSkybox()

	# -------------------------------------------------------------
	# SOUND & MUSIC
	# -------------------------------------------------------------
	def OnChangeSoundVolume(self):
		if self.ctrlSoundVolume:
			pos = self.ctrlSoundVolume.GetSliderPos()
			snd.SetSoundVolumef(pos)
			systemSetting.SetSoundVolumef(pos)

	def OnChangeMusicVolume(self):
		if self.ctrlMusicVolume:
			pos = self.ctrlMusicVolume.GetSliderPos()
			snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
			systemSetting.SetMusicVolume(pos)

	def OnChangeEnvVolume(self):
		if self.ctrlEnvVolume:
			SetOptSetting("env_volume_pos", self.ctrlEnvVolume.GetSliderPos())

	def __OnChangeMusic(self, fileName):
		if self.selectMusicFile:
			self.selectMusicFile.SetText("Metin2 Standart M\xfcz\xfd\xf0i" if fileName == uiSelectMusic.DEFAULT_THEMA else fileName[:MUSIC_FILENAME_MAX_LEN])
		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.fieldMusic)
		if fileName == uiSelectMusic.DEFAULT_THEMA:
			musicInfo.fieldMusic = musicInfo.METIN2THEMA
		else:
			musicInfo.fieldMusic = fileName
		musicInfo.SaveLastPlayFieldMusic()
		if musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

	def __OnClickChangeMusicButton(self):
		if self.selectMusicButton:
			self.selectMusicButton.SetUp()
		if not self.musicListDlg:
			self.musicListDlg = uiSelectMusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)
		self.musicListDlg.Open()

	# -------------------------------------------------------------
	# LANGUAGE (Category 4)
	# -------------------------------------------------------------
	def RefreshLanguage(self):
		curLang = GetOptSetting("client_language", app.GetLocale().lower())
		for code, btn in self.languageButtonList.items():
			if btn:
				if code == curLang:
					btn.Down()
				else:
					btn.SetUp()

	def __OnClickLanguage(self, langCode):
		SetOptSetting("client_language", langCode)
		self.RefreshLanguage()
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Dil kaydedildi: %s. Uygulamak icin oyunu yeniden baslatin." % langCode.upper())

	# -------------------------------------------------------------
	# PERFORMANCE (Category 5)
	# -------------------------------------------------------------
	def __OnToggleCrowdPerformance(self, val):
		pass

	def RefreshCrowdPerformance(self):
		pass

	def RefreshFPSLimit(self):
		curFPS = GetOptSetting("fps_limit", 144)
		for fps, btn in self.fpsLimitButtonList.items():
			if btn:
				if fps == curFPS:
					btn.Down()
				else:
					btn.SetUp()

	def __OnClickFPSLimit(self, fps):
		SetOptSetting("fps_limit", fps)
		if hasattr(app, 'SetFPS'):
			app.SetFPS(fps)
		self.RefreshFPSLimit()

	def __OnToggleHideFPSMinimap(self, val):
		advanced_options_runtime.Apply("hide_fps_minimap", val)

	def RefreshHideFPSMinimap(self):
		pass

	# -------------------------------------------------------------
	# QUEUE & MISC HANDLERS
	# -------------------------------------------------------------
	def __OnClickQueueColor(self):
		advanced_options_runtime.OpenQueueColor()

	# -------------------------------------------------------------
	# LEGACY INTERFACES FOR EXTERNAL CALLERS (game.py, uisystem.py)
	# -------------------------------------------------------------
	def RefreshNameColor(self):
		pass

	def __OnClickNameColorMode(self):
		pass

	def RefreshTargetBoard(self):
		pass

	def __SetTargetBoardViewMode(self):
		pass

	def RefreshNameOutline(self):
		pass

	def __OnClickNameOutlineMode(self):
		pass

	def RefreshShowSalesText(self):
		pass

	def __OnClickSalesTextGizle(self):
		pass

	def __OnClickSalesTextGoster(self):
		pass

	def RefreshFog(self):
		pass

	def __OnClickFog(self, index):
		pass

	def RefreshWaterReflection(self):
		pass

	def __OnClickWaterReflection(self, index):
		pass

	def RefreshRainPuddle(self):
		pass

	def __OnClickRainPuddle(self, index):
		pass

	def RefreshGrassMode(self):
		pass

	def __OnClickGrassMode(self, index):
		pass

	def RefreshSnowMode(self):
		pass

	def __OnClickSnowMode(self):
		pass

	def RefreshGraphicsLevel(self):
		buttons = getattr(self, "graphicsLevelButtonList", None)
		if not buttons or not hasattr(systemSetting, 'GetGraphicsLevel'):
			return
		level = max(0, min(systemSetting.GetGraphicsLevel(), len(buttons) - 1))
		self.__ClickRadioButton(buttons, level)

	def __OnClickGraphicsLevel(self, index):
		# 0 = Dusuk, 1 = Orta, 2 = Ultra (tree shadows, water reflection). Applied live:
		# CMapOutdoor::SkyUtil creates/releases the matching shaders on the next frame.
		if hasattr(systemSetting, 'SetGraphicsLevel'):
			systemSetting.SetGraphicsLevel(index)
		self.RefreshGraphicsLevel()
		if getattr(self, "shadowQualityButtonList", None):
			self.RefreshShadowQuality()

	def RefreshENBMode(self):
		pass

	def __OnClickENBMode(self):
		pass

	def RefreshEffectLevel(self):
		pass

	def __OnClickEffectLevel(self, index):
		pass

	def __OnClickEffectApplyButton(self):
		pass

	def RefreshPrivateShopLevel(self):
		pass

	def __OnClickPrivateShopLevel(self, index):
		pass

	def __OnClickPrivateShopApplyButton(self):
		pass

	def RefreshDropItemLevel(self):
		pass

	def __OnClickDropItemLevel(self, index):
		pass

	def __OnClickDropItemApplyButton(self):
		pass

	def RefreshPetStatus(self):
		pass

	def __OnClickPetStatus(self):
		pass

	def RefreshNpcNameStatus(self):
		pass

	def __OnClickNpcNameStatus(self):
		pass

	def RefreshShowMobLevel(self):
		pass

	def __OnClickShowMobLevel(self):
		pass

	def RefreshShowMobAIFlag(self):
		pass

	def __OnClickShowMobAIFlag(self):
		pass

	def RefreshPickUP(self):
		pass

	def __OnClickPickupModeButton(self):
		pass

	def OnChangePickUPMode(self):
		pass

	def __OnClickPickupIgnoreButton(self, type, ignore):
		pass

	def __SendPickupModeCommand(self):
		pass


class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 9

	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.AddFlag("restrict_x")
			self.parts = []

		def MakeImage(self):
			for name in ("scroll_top.tga", "scroll_mid.tga", "scroll_bottom.tga"):
				part = ui.ExpandedImageBox()
				part.SetParent(self)
				part.AddFlag("not_pick")
				part.LoadImage(IMG_DIR + "scrollbar/" + name)
				part.Show()
				self.parts.append(part)
			self.SetSize(16)

		def SetSize(self, height):
			height = max(12, int(height))
			ui.DragButton.SetSize(self, 7, height)
			if self.parts:
				self.parts[0].SetPosition(0, 0)
				self.parts[1].SetPosition(0, 2)
				self.parts[1].SetScale(1.0, float(height - 4))
				self.parts[2].SetPosition(0, height - 2)

		def Destroy(self):
			self.parts = []

		def __del__(self):
			ui.DragButton.__del__(self)

	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = ()
		self.lockFlag = False
		self.scrollStep = 0.08
		self.upButton = self.downButton = None
		self.trackParts = []
		for name in ("scrollbar_top.tga", "scrollbar_scale.tga", "scrollbar_bottom.tga"):
			part = ui.ExpandedImageBox()
			part.SetParent(self)
			part.AddFlag("not_pick")
			part.LoadImage(IMG_DIR + "scrollbar/" + name)
			part.Show()
			self.trackParts.append(part)
		self.middleBar = self.MiddleBar()
		self.middleBar.SetParent(self)
		self.middleBar.MakeImage()
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		self.middleBar.Show()
		self.SetScrollBarSize(16)
		self.SetWindowName("AdvancedOptions_ScrollBar")

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = ()
		self.middleBar.Destroy()
		self.middleBar = None
		self.trackParts = []

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	def SetScrollBarSize(self, height):
		height = max(16, int(height))
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.trackParts[0].SetPosition(2, 0)
		self.trackParts[1].SetPosition(2, 7)
		self.trackParts[1].SetScale(1.0, float(height - 13))
		self.trackParts[2].SetPosition(2, height - 6)
		self.SetMiddleBarSize(1.0)

	def SetMiddleBarSize(self, pageScale):
		if self.lockFlag:
			return
		self.lockFlag = True
		try:
			height = self.GetHeight()
			middleHeight = min(height, max(16, int(height * max(0.0, min(1.0, float(pageScale))))))
			self.middleBar.SetSize(middleHeight)
			self.pageSize = max(0, height - middleHeight)
			self.middleBar.SetRestrictMovementArea(1, 0, 7, height)
			self.middleBar.SetPosition(1, int(self.pageSize * self.curPos))
		finally:
			self.lockFlag = False

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos, moveEvent=True):
		self.curPos = max(0.0, min(1.0, float(pos)))
		self.lockFlag = True
		try:
			self.middleBar.SetPosition(1, int(self.pageSize * self.curPos))
		finally:
			self.lockFlag = False
		if moveEvent and self.eventScroll:
			self.eventScroll(*self.eventArgs)

	def OnMove(self):
		if self.lockFlag:
			return
		self.curPos = max(0.0, min(1.0, float(self.middleBar.GetLocalPosition()[1]) / self.pageSize)) if self.pageSize else 0.0
		if self.eventScroll:
			self.eventScroll(*self.eventArgs)

	def OnUp(self):
		self.SetPos(self.curPos - self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos + self.scrollStep)

	def OnMouseLeftButtonDown(self):
		x, y = self.GetMouseLocalPosition()
		self.SetPos(float(y - self.middleBar.GetHeight() / 2) / self.pageSize if self.pageSize else 0.0)
		return True

	def OnMouseWheel(self, delta):
		if delta:
			self.OnUp() if delta > 0 else self.OnDown()
			return True
		return False

	def OnRunMouseWheel(self, delta):
		return self.OnMouseWheel(delta)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False
