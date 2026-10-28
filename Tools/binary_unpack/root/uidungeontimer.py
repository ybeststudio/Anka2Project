if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui, localeInfo, background

class Cooldown(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.LeftTime = None
        self.CurrentFloor = None
        self.CoolTimeImage = None
        self.Time = 0
        self.NextFloor = 0
        self.Floor = 0
        self.Floor2 = 0
        self.CoolTime = 0.
        self.startTime = 0.
        self.__LoadWindow()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def __LoadWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/map_info.py")
        except:
            import exception
            exception.Abort("Cooldown.__LoadWindow.UIScript/map_info.py")

        self.LeftTime = self.GetChild("LeftTime")
        self.CurrentFloor = self.GetChild("CurrentFloor")
        self.CoolTimeImage = self.GetChild("CoolTime")

        ROOT_PATH = "d:/ymir work/ui/game/dungeontimer/"
        MOB_NAME = "devil_"
        FILE = "sub"

        if background.GetCurrentMapName() == "metin2_map_n_flame_dungeon_01":
            MOB_NAME = "razador_"
        elif background.GetCurrentMapName() == "metin2_map_n_snow_dungeon_01":
            MOB_NAME = "nemere_"
        elif background.GetCurrentMapName() == "defensawe_hydra":
            MOB_NAME = "hydra_"
        elif background.GetCurrentMapName() == "crystal_dungeon":
            MOB_NAME = "crystal_"
        elif background.GetCurrentMapName() == "magic_cave_dungeon":
            MOB_NAME = "vespik_"
        elif background.GetCurrentMapName() == "metin2_map_orclabyrinth":
            MOB_NAME = "orc_"
        elif background.GetCurrentMapName() == "sahmeran_boss":
            MOB_NAME = "sahmeran_"
        elif background.GetCurrentMapName() == "water_dungeon":
            MOB_NAME = "basilisk_"
        elif background.GetCurrentMapName() == "metin2_map_spiderdungeon_03":
            MOB_NAME = "barones_"
        elif background.GetCurrentMapName() == "metin2_map_dawnmist_dungeon_01":
            MOB_NAME = "jotun_"
        elif background.GetCurrentMapName() == "metin2_map_skipia_dungeon_boss":
            MOB_NAME = "beran_"
        elif background.GetCurrentMapName() == "metin2_map_mushroom_dungeon":
            MOB_NAME = "mushroom_"
        elif background.GetCurrentMapName() == "metin2_map_demon_dungeon":
            MOB_NAME = "demon_"
        elif background.GetCurrentMapName() == "plechi_greendragon_map":
            MOB_NAME = "trex_"
        elif background.GetCurrentMapName() == "metin2_map_nephrite_cave":
            MOB_NAME = "green_lion_"
        elif background.GetCurrentMapName() == "metin2_map_n_flame_dragon":
            MOB_NAME = "meley_"
        elif background.GetCurrentMapName() == "metin2_map_n_ice_dragon":
            MOB_NAME = "viserion_"
        elif background.GetCurrentMapName() == "metin2_map_devilsCatacomb":
            MOB_NAME = "orc_"
        elif background.GetCurrentMapName() == "moon_cave":
            MOB_NAME = "fenrir_"
            FILE = "png"

        self.GetChild("FloorInfoBG").LoadImage(ROOT_PATH+MOB_NAME+"back."+FILE)
        self.GetChild("FloorInfoBG").Show()
        self.CoolTimeImage.LoadImage(ROOT_PATH+MOB_NAME+"timer."+FILE)
        self.CoolTimeImage.SetPosition(9,18)

    def RefreshDungeonTimer(self, Time, Floor):
        self.CoolTimeImage.Hide()
        #self.Time = float(Time)
        self.CoolTime = float(Time)
        self.Floor = int(Floor)
        self.CoolTimeImage.Show()
        self.startTime = app.GetTime() + 0.5
        self.CoolTimeImage.SetCoolTime(self.CoolTime)
        self.CoolTimeImage.SetStartCoolTime(self.startTime)
        if background.GetCurrentMapName() == "defensawe_hydra":
            self.CurrentFloor.SetText(localeInfo.DUNGEONTIMER_WAVE % int(self.Floor))
        else:
            self.CurrentFloor.SetText(localeInfo.DUNGEONTIMER_FLOOR % int(self.Floor))
        self.Show()

    def RefreshDungeonFloor(self, Floor):
        self.Floor = int(Floor)
        if background.GetCurrentMapName() == "defensawe_hydra":
            self.CurrentFloor.SetText(localeInfo.DUNGEONTIMER_WAVE % int(self.Floor))
        else:
            self.CurrentFloor.SetText(localeInfo.DUNGEONTIMER_FLOOR % int(self.Floor))
        self.Show()

    def OnUpdate(self):
        leftTime = max(0, self.startTime + self.CoolTime - app.GetTime() + 0.5)
        leftMin = int(leftTime/60)
        leftSecond = int(leftTime%60)
        if leftSecond == 0:
            self.LeftTime.SetText("00:00")
        else:
            self.LeftTime.SetText("%02d:%02d" % (leftMin, leftSecond))

    def Close(self):
        self.Hide()

    def Destroy(self):
        self.ClearDictionary()
        self.LeftTime = 0
        self.CurrentFloor = 0
        self.CoolTimeImage = 0
        self.Time = 0
        self.NextFloor = 0
        self.Floor = 0
        self.Floor2 = 0
        self.CoolTime = 0
        self.startTime = 0
        self.Hide()
