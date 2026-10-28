# -*- coding: utf-8 -*-
"""Persistent option state and adapters to actual client APIs."""
SETTINGS_FILE = "advanced_options.cfg"
LOCALE_FILE = "locale.cfg"
_values = {}
_modules = {}
_defaults = {'hide_dungeon_announcement': False, 'view_drop_ui': False, 'view_guideline': True, 'view_system_menu': True, 'view_event_icon': True, 'big_font_size': False, 'big_slot_count': False, 'save_status_window': True, 'small_affect_icon': False, 'guild_quest_window': True, 'whisper_sender_letter': True, 'guild_chat_to_whisper': False, 'small_whisper_window': False, 'show_guild_join_notification': True, 'auto_unequip_buffi_pvp': False, 'auto_equip_buffi_pvm': True, 'names_monsters_fov': False, 'show_country_flag': True, 'damage_monster': True, 'colored_damage': True, 'hide_mount': False, 'hide_buffi': False, 'queue_flash': True, 'mining_finish_flash': True, 'hide_effects_except_damage': False, 'hide_effects_from_others_players': False, 'hide_shaman_buff': False, 'hide_stone_effect': False, 'hide_monster_effect': False, 'hide_exp_effect': False, 'show_guild_grade': True, 'view_notification_history': True, 'auto_cape_after_warp': False, 'disable_level_up_confirmation': False, 'hide_metin_spawns': False, 'auto_hide_companion_map1': False, 'anti_aliasing': True, 'shadow_caster_player': True, 'shadow_caster_terrain': True, 'shadow_caster_trees': True, 'sun_rays': True, 'wet_ground': True, 'rain': False, 'storm': False, 'shooting_stars': True, 'water_ripple': True, 'enhanced_water': True, 'underwater': True, 'specular_reflection': True, 'water_reflection': True, 'local_light': True, 'death_cam_effect': True, 'speedtree_dynamic_light': True, 'speedtree_wind': True, 'speedtree_enhanced_foliage': True, 'speedtree_falling_leaves': True, 'high_performance_crowd_mode': False, 'hide_fps_minimap': False, 'font_type_index': 0, 'hide_pet': False, 'queue_effect_index': 0, 'event_stone_index': 0}
SOUND_CATEGORIES = ("hits", "critical", "skills", "movement", "interface", "voices", "monsters")
_defaults.update(dict(("sound_category_" + name, 1.0) for name in SOUND_CATEGORIES))

def RegisterDefault(key, value):
    _defaults[key] = value

def PrepareInterface():
    for key in ("window_scale_pos", "save_status_window", "font_type_index", "big_font_size"):
        if key in _values: Apply(key, _values[key])

def Reset():
    old = dict(_values)
    _values.clear()
    defaults = dict(_defaults)
    defaults.update(dict((key, 0.5) for key in ("damage_spacing_pos", "size_damage_pos", "keyboard_camera_speed_pos", "env_volume_pos", "env_darkness_pos", "env_saturation_pos", "env_contrast_pos", "env_temperature_pos")))
    defaults["fps_limit"] = 60
    defaults.update(dict(("environment_color_"+channel, 1.0) for channel in ("r", "g", "b")))
    for key in old:
        if key.startswith("key_"): continue
        if key in defaults and IsAvailable(key): Apply(key, defaults[key])
    RemapKeys()
    Save()

def Load():
    try:
        stream = open(SETTINGS_FILE, "r")
    except IOError:
        return
    try:
        for line in stream:
            try:
                key, kind, value = line.rstrip("\r\n").split("\t", 2)
                if not key or any(c not in "abcdefghijklmnopqrstuvwxyz0123456789_" for c in key):
                    continue
                if kind == "bool": value = value == "1"
                elif kind == "int": value = int(value)
                elif kind == "float": value = float(value)
                elif kind == "str": value = value.replace("\\t", "\t").replace("\\n", "\n").replace("\\\\", "\\")
                else: continue
                _values[key] = value
            except (ValueError, TypeError):
                continue
    finally:
        stream.close()

def Save():
    stream = open(SETTINGS_FILE, "w")
    try:
        for key, value in sorted(_values.items()):
            if isinstance(value, bool): kind, text = "bool", "1" if value else "0"
            elif isinstance(value, int): kind, text = "int", str(value)
            elif isinstance(value, float): kind, text = "float", repr(value)
            elif isinstance(value, str): kind, text = "str", value.replace("\\", "\\\\").replace("\t", "\\t").replace("\n", "\\n")
            else: continue
            stream.write("%s\t%s\t%s\n" % (key, kind, text))
        stream.flush()
    finally:
        stream.close()

def Configure(**modules):
    global _keyboardDialog, _lastMap
    if "game" in modules and modules["game"] is not _modules.get("game"):
        if _keyboardDialog: _keyboardDialog.Hide()
        _keyboardDialog = None
        _lastMap = None
        import advanced_option_notifications
        advanced_option_notifications.Stop()
        import advanced_metin_queue
        advanced_metin_queue.Stop()
    _modules.update(modules)
    if modules.get("game") is not None:
        import advanced_metin_queue
        advanced_metin_queue.Configure(**_modules)
    if modules.get("game") is not None and "ui" in _modules and "wndMgr" in _modules:
        import advanced_option_notifications
        advanced_option_notifications.Configure(**_modules)
        advanced_option_notifications.Start()

def Get(key, default=False):
    if key not in _values and key in ("hide_pet", "hide_mount") and "systemSetting" in _modules:
        system = _modules["systemSetting"]
        return bool(system.GetAdvancedOption(key, float(system.IsPetStatus())))
    return _values.get(key, default)

def Set(key, value):
    previous = dict(_values)
    _values[key] = value
    try:
        Apply(key, value)
    except Exception:
        _values.clear()
        _values.update(previous)
        raise
    Save()

def _Call(module, name, *args):
    obj = _modules.get(module)
    if obj is None or not hasattr(obj, name):
        raise RuntimeError("Missing client API: %s.%s" % (module, name))
    return getattr(obj, name)(*args)

def _Interface():
    return _modules["constInfo"].GetInterfaceInstance()

def _Font(family=None, enlarged=None):
    family = int(Get("font_type_index", 0)) if family is None else int(family)
    enlarged = Get("big_font_size", False) if enlarged is None else enlarged
    name = "Arial" if family else "Tahoma"
    size = 16 if enlarged else 12
    _Call("app", "SetDefaultFontName", "%s:%d" % (name, size))
    locale = _modules["localeInfo"]
    # Windows are built from UI_DEF_FONT; if it already is this font there is nothing to refresh
    # (the walk below visits every widget -- ~70 ms per call while entering the game).
    unchanged = getattr(locale, "UI_DEF_FONT", None) == "%s:%d" % (name, size)
    for key, suffix in (("UI_DEF_FONT", ""), ("UI_DEF_FONT_LARGE", "b"), ("UI_DEF_FONT_SMALL", "")):
        setattr(locale, key, "%s:%d%s" % (name, size, suffix))
    interface = _Interface()
    if interface and not unchanged:
        # Refresh existing text objects as well as future windows.
        visited = set()
        def visit(widget):
            if widget is None or id(widget) in visited: return
            visited.add(id(widget))
            ui = _modules["ui"]
            if isinstance(widget, ui.TextLine): widget.SetFontName("%s:%d" % (name, size))
            for child in getattr(widget, "Children", ()): visit(child)
            visit(getattr(widget, "ButtonText", None))
        for value in vars(interface).values():
            if isinstance(value, _modules["ui"].Window): visit(value)

NATIVE_TOGGLES = {
    "speedtree_falling_leaves": ("background", "SetFallingLeaves", 1, 0),
    "shadow_caster_terrain": ("systemSetting", "SetTerrainShadow", 1, 0),
    "shadow_caster_trees": ("systemSetting", "SetTreeShadow", 1, 0),
    "rain": ("background", "SetRainLevel", 2, 0),
    "water_reflection": ("background", "SetWaterReflection", 1, 0),
    "speedtree_enhanced_foliage": ("background", "SetEnhancedFoliage", 1, 0),
    "speedtree_dynamic_light": ("background", "SetSpeedTreeDynamicLight", 1, 0),
}

ACTOR_OPTIONS = ("hide_pet", "hide_mount", "damage_monster", "colored_damage", "damage_spacing_pos", "size_damage_pos", "death_cam_effect", "underwater", "env_volume_pos", "shadow_caster_player", "save_status_window", "env_darkness_pos", "env_saturation_pos", "env_contrast_pos", "env_temperature_pos", "anti_aliasing", "names_monsters_fov", "hide_effects_except_damage", "hide_effects_from_others_players", "hide_stone_effect", "hide_monster_effect", "guild_chat_to_whisper", "specular_reflection", "speedtree_wind", "sun_rays", "wet_ground", "local_light", "environment_color_r", "environment_color_g", "environment_color_b", "hide_shaman_buff")
ACTOR_OPTIONS += ("show_country_flag", "shop_range_pos", "auto_hide_companion_map1", "hide_exp_effect", "water_ripple", "enhanced_water", "high_performance_crowd_mode")
_defaults.update({"shop_range_pos": 1.0, "auto_hide_companion_map1": False, "remove_fog": False})
_defaults.update({"window_scale_pos": .5, "damage_color_custom": False})
ACTOR_OPTIONS += ("damage_color_custom", "damage_color_r", "damage_color_g", "damage_color_b")
for _damage_kind in ("normal", "critical", "penetrate", "critical_penetrate", "poison", "received"):
    ACTOR_OPTIONS += tuple("damage_%s_%s" % (_damage_kind, _channel) for _channel in "rgb")
ACTOR_OPTIONS += ("show_guild_grade",)
ACTOR_OPTIONS += ("queue_effect_index", "queue_color_r", "queue_color_g", "queue_color_b")
_defaults.update({"queue_color_r":1.0, "queue_color_g":1.0, "queue_color_b":1.0})
# hide_metin_spawns: suppresses only the minimap marker (CPythonMiniMap::Update, ENABLE_METIN_STONES_MINIMAP);
# the metin stone actor itself keeps rendering in the 3D world, matching reference semantics.
ACTOR_OPTIONS += ("hide_metin_spawns",)
# hide_buffi: CInstanceBase::IsHiddenByOptions hides the Buffi NPC (34900/34901) by this flag.
ACTOR_OPTIONS += ("hide_buffi",)
for _channel in ("r", "g", "b"):
    _defaults["damage_color_" + _channel] = 1.0

# These reference controls require systems absent from this client/server tree.
# Retain their UI and saved state, but never report a storage-only write as success.
UNAVAILABLE_OPTIONS = {
    "guild_quest_window": "Guild quest system",
    "hide_pvp_team_indicators": "PvP team indicator effect",
    "hidden_notification_guideline": "Guideline notification producer",
    "hidden_notification_mission_book": "Mission Book notification producer",
    "hidden_notification_bonus_67_finished": "Bonus 6/7 Finished notification producer",
}

# Gokyuzu on ayarlari -- baerim referansi ile birebir (locale GRAPHICS_SKY_DEFAULT, PRESET_01..03,
# STARRY_NIGHT, PRESET_04..10; dosyalar baerim string tablosundan). 0 = haritanin kendi ortami
# (CMapManager her harita yuklemesinde ortami index 0'a kaydeder).
SKY_FILES = (None, "environment_02.msenv", "environment_03.msenv", "environment_04.msenv", "starry_night.msenv",
             "environment_05.msenv", "environment_06.msenv", "environment_07.msenv", "environment_08.msenv",
             "environment_09.msenv", "environment_10.msenv", "environment_11.msenv")
SKY_STARRY_NIGHT = 4
SKY_ENV_BASE = 100
_defaults["sky_preset"] = 0
_skyActive = False   # bu haritada bir on ayar uygulandi mi
# On ayar haritanin tum ortamini (isik, materyal, sis, skybox) acik hava dosyasiyla degistirir.
# Zindan/magara haritalarinda bu, karanlik ortami silip DungeonBlock zeminlerini parlak gokyuzu isigiyla boyar.
SKY_SKIP_MAP_KEYWORDS = ("dungeon", "cave", "catacomb", "deviltower", "maze", "boss", "temple", "smhgate")

def _IsSkyPresetBlockedMap(background):
    if not hasattr(background, "GetCurrentMapName"):
        return False
    name = (background.GetCurrentMapName() or "").lower()
    for keyword in SKY_SKIP_MAP_KEYWORDS:
        if keyword in name:
            return True
    return False

def ApplySky():
    global _skyActive
    index = int(Get("sky_preset", 0))
    if not 0 <= index < len(SKY_FILES):
        index = 0
    background = _modules.get("background")
    if background is None or not hasattr(background, "ChangeEnvironmentData") or not hasattr(background, "RegisterEnvironmentData"):
        return
    if _IsSkyPresetBlockedMap(background):
        index = 0   # zindanlarda haritanin kendi ortami kalir
    night = hasattr(background, "GetDayMode") and background.GetDayMode() == getattr(background, "DAY_MODE_DARK", -1)
    if not night:   # gece modunda oyunun gece ortami korunur
        name = SKY_FILES[index]
        if name:
            _Call("background", "RegisterEnvironmentData", SKY_ENV_BASE + index, "d:/ymir work/environment/" + name)
            _Call("background", "ChangeEnvironmentData", SKY_ENV_BASE + index)
            # ChangeEnvironmentData sis seviyesini uygulamaz; on ayar dosyasinin kendi sisi (near 1000)
            # uzak objeleri beyaza boyar -> kullanicinin sis seviyesini yeniden uygula
            if hasattr(background, "SetFogLevel") and hasattr(background, "GetFogLevel"):
                background.SetFogLevel(background.GetFogLevel())
            _skyActive = True
        elif _skyActive:   # on ayardan Varsayilan'a donus: haritanin kendi ortami
            _Call("background", "ChangeEnvironmentData", 0)
            _skyActive = False
    try:
        interface = _Interface()
    except Exception:
        interface = None
    layer = getattr(interface, "dlgShootingStars", None) if interface else None
    if layer and hasattr(layer, "SetStarrySky"):
        layer.SetStarrySky(index == SKY_STARRY_NIGHT)

def IsAvailable(key):
    return key not in UNAVAILABLE_OPTIONS

def ExplainUnavailable(key):
    chat = _modules.get("chat")
    if chat:
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Bu secenek henuz bagli degil: %s" % UNAVAILABLE_OPTIONS.get(key, key))

def SendBuffiOptions():
    # The server applies both on damage (CBuffiManager::OnOwnerAttack).
    _Call("net", "SendChatPacket", "/buffi_option %d %d" % (bool(Get("auto_unequip_buffi_pvp", False)), bool(Get("auto_equip_buffi_pvm", True))))

def Apply(key, value):
    if key in UNAVAILABLE_OPTIONS:
        raise RuntimeError("Option system not implemented: " + UNAVAILABLE_OPTIONS[key])
    if key == "rain":
        _Call("systemSetting", "SetAdvancedOption", key, float(value))
        _Call("background", "SetRainLevel", 3 if Get("storm", False) else (2 if value else 0))
    elif key in NATIVE_TOGGLES:
        module, name, on, off = NATIVE_TOGGLES[key]
        _Call(module, name, on if value else off)
    elif key in ACTOR_OPTIONS:
        if key in ("hide_pet", "hide_mount"):
            other = "hide_mount" if key == "hide_pet" else "hide_pet"
            _values[other] = Get(other, False)
            _Call("systemSetting", "SetAdvancedOption", other, float(_values[other]))
            _Call("systemSetting", "SetPetStatusFlag", False)
        _Call("systemSetting", "SetAdvancedOption", key, float(value))
        if key == "show_country_flag":
            interface = _Interface()
            if interface:
                for dialog in interface.whisperDialogDict.values():
                    flag = getattr(dialog, "countryFlag", None)
                    if flag:
                        flag.Show() if value and getattr(dialog, "countryID", 0) else flag.Hide()
    elif key in ("auto_unequip_buffi_pvp", "auto_equip_buffi_pvm"):
        SendBuffiOptions()
    elif key == "auto_cape_after_warp":
        _Call("net", "SendChatPacket", "/auto_cape resume %d" % bool(value))
    elif key == "fps_limit":
        _Call("app", "SetFPS", max(30, min(240, int(value))))
    elif key == "keyboard_camera_speed_pos":
        _Call("app", "SetCameraSpeed", int(max(0., min(1., float(value))) * 100))
    elif key.startswith("sound_category_") and key[len("sound_category_"):] in SOUND_CATEGORIES:
        _Call("snd", "SetCategoryVolume", SOUND_CATEGORIES.index(key[len("sound_category_"):]),
            max(0.0, min(1.0, float(value))))
    elif key == "remove_fog":
        _Call("systemSetting", "SetFogMode", 0 if value else 1)
        background = _modules.get("background")
        if background is not None and hasattr(background, "SetFogLevel"):
            background.SetFogLevel(0 if value else 1)   # native sis motoru (m_iFogLevel kalici)
    elif key == "storm":
        _Call("systemSetting", "SetAdvancedOption", key, float(value))
        _Call("background", "SetRainLevel", 3 if value else (2 if Get("rain", False) else 0))
    elif key == "font_type_index":
        _Font(family=value)
    elif key == "big_font_size":
        _Font(enlarged=value)
    elif key == "event_stone_index":
        _Call("nonplayer", "SetEventStoneAppearance", int(value))
    elif key == "big_slot_count":
        system = _modules["systemSetting"]
        mode = system.SLOT_CHAR_ENLARGE_MODE_ON if value else system.SLOT_CHAR_ENLARGE_MODE_OFF
        _Call("systemSetting", "SetSlotCharEnlargeMode", mode)
    elif key == "window_scale_pos":
        _Call("wndMgr", "SetUIScale", .75 + .5 * max(0., min(1., float(value))))
        if _modules.get("game"):
            import advanced_option_notifications
            advanced_option_notifications.ApplyVisibility()
    elif key in ("hide_dungeon_announcement", "show_guild_join_notification", "view_notification_history") or key.startswith("hidden_notification_"):
        import advanced_option_notifications
        advanced_option_notifications.Configure(**_modules)
        advanced_option_notifications.ApplyVisibility()
    elif key in ("mining_finish_flash", "queue_flash", "disable_level_up_confirmation"):
        # The completion packet receiver reads this preference at event time.
        return
    elif key == "view_guideline":
        # Reference: shows/hides the guide button at the bottom-left (GAME_OPTION_GUIDELINE_BTN).
        interface = _Interface()
        gameButton = getattr(interface, "wndGameButton", None) if interface else None
        if gameButton and "GUIDE" in gameButton.gameButtonDict:
            gameButton.gameButtonDict["GUIDE"].Show() if value else gameButton.gameButtonDict["GUIDE"].Hide()
    elif key == "view_system_menu":
        interface = _Interface()
        if interface and interface.wndTaskBar:
            interface.wndTaskBar.Show() if value else interface.wndTaskBar.Hide()
    elif key == "view_event_icon":
        interface = _Interface()
        if interface:
            window = getattr(interface, "wndMiniGame", None)
            if window:
                window.Show() if value else window.Hide()
    elif key == "shooting_stars":
        interface = _Interface()
        if interface:
            layer = getattr(interface, "dlgShootingStars", None)
            if layer:
                layer.SetEnabled(value)
    elif key == "view_drop_ui":
        interface = _Interface()
        if interface:
            window = getattr(interface, "dlgDropInfo", None)
            if window and not value:
                window.OnWarping()  # clear queued entries and hide
    elif key == "hide_fps_minimap":
        interface = _Interface()
        if interface and interface.wndMiniMap and interface.wndMiniMap.FPSInfo:
            interface.wndMiniMap.FPSInfo.Hide() if value else interface.wndMiniMap.FPSInfo.Show()
    elif key == "small_affect_icon":
        game = _modules.get("game")
        if game and game.affectShower:
            game.affectShower.ApplyIconSize(value)
    elif key == "client_language":
        pages = {"en":1252,"pt":1252,"es":1252,"fr":1252,"de":1252,"ro":1250,"pl":1250,"it":1252,"cz":1250,"hu":1250,"tr":1254,"ae":1256}
        if value not in pages: raise ValueError("Unknown client language")
        stream = open(LOCALE_FILE, "w")
        try: stream.write("%d %s\n" % (pages[value], value))
        finally: stream.close()
    elif key == "whisper_sender_letter":
        interface = _Interface()
        if interface:
            for button in interface.whisperButtonList:
                button.Show() if value else button.Hide()
    elif key == "small_whisper_window":
        interface = _Interface()
        if interface:
            for dialog in interface.whisperDialogDict.values():
                dialog.ApplyOptionSize(value)
    elif key == "sky_preset":
        ApplySky()
    elif key.startswith("key_"):
        RemapKeys(key, int(value))

def RemapKeys(changedKey=None, changedValue=None):
    game = _modules.get("game")
    if game is None: return
    if changedKey is not None:
        app = _modules["app"]
        reserved = [getattr(app, name) for name in ("DIK_ESCAPE", "DIK_RETURN", "DIK_LALT", "DIK_RALT", "DIK_LCONTROL", "DIK_RCONTROL", "DIK_LSHIFT", "DIK_RSHIFT") if hasattr(app, name)]
        if changedValue in reserved: raise ValueError("Reserved key")
    if not hasattr(game, "_advancedOriginalPress"):
        game._advancedOriginalPress = dict(game.onPressKeyDict)
        game._advancedOriginalRelease = dict(game.onClickKeyDict)
    mappings = dict((int(k[4:]), int(v)) for k, v in _values.items() if k.startswith("key_"))
    if changedKey is not None: mappings[int(changedKey[4:])] = changedValue
    destinations = set()
    press, release = {}, {}
    for original, callback in game._advancedOriginalPress.items():
        target = mappings.get(original, original)
        if target in destinations: raise ValueError("This key is already assigned")
        destinations.add(target)
        press[target] = callback
    for original, callback in game._advancedOriginalRelease.items():
        release[mappings.get(original, original)] = callback
    game.onPressKeyDict, game.onClickKeyDict = press, release

_keyboardDialog = None
_colorDialogs = {}

def OpenDamageColor():
    import advanced_damage_colors
    if "damage" not in _colorDialogs:
        _colorDialogs["damage"] = advanced_damage_colors.DamageColorWindow()
    _colorDialogs["damage"].Show()
    _colorDialogs["damage"].SetTop()
def OpenQueueColor():
    import advanced_damage_colors
    if "queue" not in _colorDialogs:
        _colorDialogs["queue"] = advanced_damage_colors.QueueColorWindow()
    _colorDialogs["queue"].Show()
    _colorDialogs["queue"].SetTop()

def OpenEffectColor(kind):
    ui = _modules["ui"]
    if kind in _colorDialogs:
        _colorDialogs[kind].Show(); _colorDialogs[kind].SetTop()
        return
    class DamageColorDialog(ui.BoardWithTitleBar):
        def __init__(self):
            ui.BoardWithTitleBar.__init__(self)
            self.SetSize(310, 180); self.SetTitleName("Hasar rengi" if kind=="damage" else "Metin sirasi rengi")
            self.AddFlag("movable"); self.SetCenterPosition()
            self.controls = []
            for index, channel in enumerate(("r", "g", "b")):
                label = ui.TextLine(); label.SetParent(self); label.SetPosition(15, 48+index*34)
                label.SetText(channel.upper()); label.Show()
                slider = ui.SliderBar(); slider.SetParent(self); slider.SetPosition(45, 50+index*34)
                key = kind+"_color_"+channel
                slider.SetSliderPos(Get(key, _defaults.get(key,1.0)))
                slider.SetEvent(lambda k=key, control=slider: self.Change(k, control.GetSliderPos()))
                slider.Show(); self.controls.append((label, slider))
        def Change(self, key, value):
            Set(key, value)
            if kind=="damage": Set("damage_color_custom", True)
        def OnPressEscapeKey(self): self.Hide(); return True
    _colorDialogs[kind] = DamageColorDialog()
    _colorDialogs[kind].Show()

def OpenEnvironmentColor():
    if "environment" in _colorDialogs:
        _colorDialogs["environment"].Show()
        _colorDialogs["environment"].SetTop()
        return
    import advanced_damage_colors
    _colorDialogs["environment"] = advanced_damage_colors.EnvironmentColorWindow()
    _colorDialogs["environment"].Show()

def OpenKeyboard():
    global _keyboardDialog
    ui = _modules["ui"]
    app = _modules["app"]
    game = _modules.get("game")
    if game is None: raise RuntimeError("Keyboard settings require the game phase")
    if _keyboardDialog:
        _keyboardDialog.Show(); _keyboardDialog.SetTop(); return
    class KeyDialog(ui.BoardWithTitleBar):
        def __init__(self):
            ui.BoardWithTitleBar.__init__(self)
            self.SetSize(330, 405); self.SetTitleName("Keyboard Settings")
            self.AddFlag("movable"); self.SetCenterPosition()
            self.waiting = None; self.page = 0; self.rows = []
            self.keys = sorted(game._advancedOriginalPress if hasattr(game, "_advancedOriginalPress") else game.onPressKeyDict)
            self.keyNames = dict((getattr(app, name), name[4:]) for name in dir(app) if name.startswith("DIK_") and isinstance(getattr(app, name), int))
            self.status = ui.TextLine(); self.status.SetParent(self); self.status.SetPosition(15, 36)
            self.status.SetText("Select an action, then press its new key."); self.status.Show()
            for row in range(10):
                label = ui.TextLine(); label.SetParent(self); label.SetPosition(15, 66 + row*28); label.Show()
                button = ui.Button(); button.SetParent(self); button.SetPosition(145, 59 + row*28)
                button.SetUpVisual("d:/ymir work/ui/game/advanced_game_options/btn_0.png")
                button.SetOverVisual("d:/ymir work/ui/game/advanced_game_options/btn_1.png")
                button.SetDownVisual("d:/ymir work/ui/game/advanced_game_options/btn_2.png")
                button.SetEvent(lambda idx=row: self.Capture(idx)); button.Show()
                self.rows.append((label, button))
            self.navigation = []
            for index, text in enumerate(("Previous", "Next")):
                button = ui.Button(); button.SetParent(self); button.SetPosition(20+index*155, 355)
                button.SetUpVisual("d:/ymir work/ui/game/advanced_game_options/btn_0.png")
                button.SetOverVisual("d:/ymir work/ui/game/advanced_game_options/btn_1.png")
                button.SetDownVisual("d:/ymir work/ui/game/advanced_game_options/btn_2.png")
                button.SetText(text); button.SetEvent(lambda delta=(-1 if index==0 else 1): self.Navigate(delta)); button.Show(); self.navigation.append(button)
            self.Refresh()
        def Refresh(self):
            for index, (label, button) in enumerate(self.rows):
                offset = self.page*10 + index
                if offset >= len(self.keys): label.Hide(); button.Hide(); continue
                original = self.keys[offset]
                label.SetText(self.keyNames.get(original, str(original)))
                assigned = Get("key_%d" % original, original)
                button.SetText(self.keyNames.get(assigned, str(assigned)))
                label.Show(); button.Show()
        def Navigate(self, delta):
            self.page = max(0, min((len(self.keys)-1)//10, self.page+delta)); self.Refresh()
        def Capture(self, index):
            self.waiting = self.keys[self.page*10+index]
            self.status.SetText("Press the new key (Escape cancels)."); self.SetFocus()
        def OnKeyDown(self, key):
            if self.waiting is None: return False
            if key != app.DIK_ESCAPE:
                try:
                    Set("key_%d" % self.waiting, key)
                    self.status.SetText("Key saved.")
                except ValueError:
                    self.status.SetText("Key already assigned. Choose another key.")
                    return True
            self.waiting = None; self.Refresh(); return True
        def OnPressEscapeKey(self):
            self.waiting = None; self.Hide(); return True
    _keyboardDialog = KeyDialog(); _keyboardDialog.Show()

_replayedMap = None
def Replay():
    global _replayedMap
    for key, value in list(_values.items()):
        if IsAvailable(key): Apply(key, value)
    # the frame limit is applied even when it was never saved (it used to stay unlimited until the options were opened)
    if "fps_limit" not in _values and IsAvailable("fps_limit"):
        Apply("fps_limit", 60)
    background = _modules.get("background")
    _replayedMap = background.GetCurrentMapName() if background and hasattr(background, "GetCurrentMapName") else None

_lastMap = None
def UpdateMapSettings():
    global _lastMap, _skyActive, _replayedMap
    import advanced_metin_queue
    background = _modules.get("background")
    if not background or not hasattr(background, "GetCurrentMapName"): return
    current = background.GetCurrentMapName()
    if current == _lastMap:
        advanced_metin_queue.Update()
        return
    advanced_metin_queue.Clear()
    _lastMap = current
    if "net" in _modules:
        _Call("net", "SendChatPacket", "/auto_cape sync %d" % bool(Get("auto_cape_after_warp", False)))
    if "nonplayer" in _modules:
        _Call("nonplayer", "ClearEventStoneAppearance")
    outdoor = current in ("metin2_map_a1", "metin2_map_b1", "metin2_map_c1")
    _Call("systemSetting", "SetAdvancedOption", "in_map1", float(outdoor))
    interface = _Interface()
    if interface:
        layer = getattr(interface, "dlgShootingStars", None)
        if layer:
            layer.SetOutdoor(outdoor)
            layer.OnWarping()
        dropWindow = getattr(interface, "dlgDropInfo", None)
        if dropWindow:
            dropWindow.OnWarping()
    if _replayedMap == current:
        _replayedMap = None   # Replay() already applied these for this map load
        return
    _replayedMap = None
    for key in ("rain", "storm", "speedtree_wind", "water_reflection", "specular_reflection", "shadow_caster_terrain", "shadow_caster_trees"):
        if key in _values: Apply(key, _values[key])
    _skyActive = False   # harita kendi ortamini index 0'a yukledi (SetEnvironmentDataPtr)
    ApplySky()           # secili on ayar varsa yeniden uygula

Load()
