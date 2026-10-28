if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))

import item, event, constInfo, localeInfo
import ui, grp, uiCommon

IMG_DIR = "d:/ymir work/ui/game/dungeon_info/"
IMG_ICON_DIR = "d:/ymir work/ui/game/dungeon_info/icons/"

dungeonData = {
    66 : {
        "dungeonName":localeInfo.DEVILTOWER,
        "level_range":[40,95],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25163,
        "icon_folder": "demon_tower",
        },
    217 : {
        "dungeonName":localeInfo.SPIDERBARONESA,
        "level_range":[70,90],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25164,
        "icon_folder": "spider_dungeon",
        },
    208 : {
        "dungeonName":localeInfo.BERANSETAOU,
        "level_range":[75,105],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25165,
        "icon_folder": "beran_setaou",
        },
    216 : {
        "dungeonName":localeInfo.CATACUMB,
        "level_range":[80,105],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25166,
        "icon_folder": "azrael",
        },
    351 : {
        "dungeonName":localeInfo.FLAMEDUNGEON,
        "level_range":[90,120],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25167,
        "icon_folder": "razador",
        },
    352 : {
        "dungeonName":localeInfo.SNOWDUNGEON,
        "level_range":[90,120],
        "group_status":False,
        "cooldown": 60*60,
        "ticket":25168,
        "icon_folder": "nemere",
        },
    355 : {
        "dungeonName":localeInfo.HYDRADUNGEON,
        "level_range":[105,120],
        "group_status":True,
        "cooldown": 60*60*3,
        "ticket":25169,
        "icon_folder": "hydra",
        },
    212 : {
        "dungeonName":localeInfo.MELEYDUNGEON,
        "level_range":[110,120],
        "group_status":True,
        "cooldown": 60*60*3,
        "ticket":25170,
        "icon_folder": "meley",
        },
}

class DungeonInfo(ui.ScriptWindow):

    class dungeonItem(ui.RadioButton):
        def __del__(self):
            ui.RadioButton.__del__(self)
        def __init__(self, key):
            ui.RadioButton.__init__(self)
            self.dungeonKey = key
            self.dungeonName=None
            self.dungeonLimit=None
            self.dungeonIcon=None
            self.LoadDungeonInfo()
        def Destroy(self):
            self.dungeonName=None
            self.dungeonLimit=None
            self.dungeonIcon=None
        def LoadDungeonInfo(self):
            self.SetUpVisual(IMG_DIR+"button_dungeon_0.tga")
            self.SetOverVisual(IMG_DIR+"button_dungeon_1.tga")
            self.SetDownVisual(IMG_DIR+"button_dungeon_2.tga")

            if not dungeonData.has_key(self.dungeonKey):
                return
            data = dungeonData[self.dungeonKey]

            dungeonIcon = ui.ImageBox()
            dungeonIcon.SetParent(self)
            dungeonIcon.SetPosition(1, 1)
            dungeonIcon.LoadImage(IMG_ICON_DIR+data["icon_folder"]+".tga")
            dungeonIcon.Show()
            self.dungeonIcon = dungeonIcon

            dungeonName = ui.TextLine()
            dungeonName.SetParent(self)
            dungeonName.SetPosition(70, 16)
            dungeonName.SetHorizontalAlignLeft()
            dungeonName.SetText(data["dungeonName"])
            dungeonName.Show()
            self.dungeonName = dungeonName

            dungeonLimit = ui.TextLine()
            dungeonLimit.SetParent(self)
            dungeonLimit.SetPosition(265, 15)
            dungeonLimit.SetHorizontalAlignRight()
            dungeonLimit.Show()
            self.dungeonLimit = dungeonLimit

        def FormatTime(self, seconds):
            if seconds == 0:
                return "0h 0m 0s"
            m, s = divmod(seconds, 60)
            h, m = divmod(m, 60)
            return "%dh %dm %ds" % (h, m, s)

        def OnRender(self):
            realBack = constInfo.GetFlag("%d_back"%self.dungeonKey)
            leftTimeBack = max(0,realBack-app.GetGlobalTimeStamp())
            if leftTimeBack > 0:
                self.dungeonLimit.SetText("|cffe69138"+self.FormatTime(leftTimeBack))
            else:
                realCooldown = constInfo.GetFlag("%d_cooldown"%self.dungeonKey)
                leftTime = max(0,realCooldown-app.GetGlobalTimeStamp())
                if leftTime > 0:
                    self.dungeonLimit.SetText("|cffff0000"+self.FormatTime(leftTime))
                else:
                    if not dungeonData.has_key(self.dungeonKey):
                        return
                    data = dungeonData[self.dungeonKey]
                    if player.GetStatus(player.LEVEL) >= data["level_range"][0]:
                        self.dungeonLimit.SetText("|cff00ff00"+localeInfo.DUNGEONINFO_CANENTER)
                    else:
                        self.dungeonLimit.SetText("|cffff0000"+localeInfo.DUNGEONINFO_CANTENTER)

    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.dungeonKey = 0
        self.questionDialog = None
        self.scrollBar = None
        self.LoadDialog()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def Destroy(self):
        self.ClearDictionary()
        self.dungeonKey = 0
        self.questionDialog = None

    def OnMouseWheel(self, nLen):
        if self.scrollBar:
            if nLen > 0:
                self.scrollBar.OnUp()
            else:
                self.scrollBar.OnDown()
            return True
        return False

    def LoadDialog(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/dungeoninfowindow.py")

            self.GetChild("title_bar").SetCloseEvent(self.Close)
            self.GetChild("teleport").SetEvent(self.TeleportDungeon)
    
            self.GetChild("ListBox").SetScrollBar(self.GetChild("scrollbar"))
            self.GetChild("ListBox").SetItemStep(51)
            self.GetChild("ListBox").SetItemSize(299,46)


            self.GetChild("ticket_img").SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInItem)
            self.GetChild("ticket_img").SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutItem)
            
        except:
            import exception
            exception.Abort("DungeonInfo.LoadDialog.LoadScript")

        self.SetCenterPosition()
        self.LoadDungeonButtons()

    def OverOutItem(self):
        interface = constInfo.GetInterfaceInstance()
        if interface:
            if interface.tooltipItem:
                interface.tooltipItem.HideToolTip()

    def OverInItem(self):
        interface = constInfo.GetInterfaceInstance()
        if interface:
            if interface.tooltipItem:
                if not dungeonData.has_key(self.dungeonKey):
                    return
                data = dungeonData[self.dungeonKey]
                interface.tooltipItem.SetItemToolTip(data["ticket"])
                interface.tooltipItem.ShowToolTip()

    def FormatTime(self, seconds):
        if seconds == 0:
            return "0s"
        m, s = divmod(seconds, 60)
        h, m = divmod(m, 60)
        timeText = ""
        if h > 0:
            timeText+= "%dh" % h
        if m > 0:
            timeText+= "%dm" % m
        if s > 0:
            timeText+= "%ds" % s
        return timeText
    
    def ClickDungeonItem(self, key, index):
        self.dungeonKey = key
        self.__ClickRadioButton(self.GetChild("ListBox").itemList,index)

        if not dungeonData.has_key(self.dungeonKey):
            return
        data = dungeonData[self.dungeonKey]

        self.GetChild("level_range_value").SetText("%d-%d"%(data["level_range"][0],data["level_range"][1]))
        if data["group_status"]:
            self.GetChild("group_value").SetText(localeInfo.DUNGEONINFO_YES)
        else:
            self.GetChild("group_value").SetText(localeInfo.DUNGEONINFO_NO)
        
        self.GetChild("dungeon_name").SetText(data["dungeonName"])

        self.GetChild("ticket_img").ImageClear()

        if data["ticket"] > 0:
            item.SelectItem(data["ticket"])
            self.GetChild("ticket_img").LoadImage(item.GetIconImageFileName())
            self.GetChild("ticket_img").SetPosition(165-32, 100-10)

        self.GetChild("cooldown_value").SetText(self.FormatTime(data["cooldown"]))

        self.GetChild("complete_value").SetText("0")
        self.GetChild("min_time_value").SetText("00:00:00")
        self.GetChild("max_dmg_value").SetText("0.0")

    def take_second(self, elem):
        return elem[1]

    def LoadDungeonButtons(self):
        self.GetChild("ListBox").RemoveAllItems()

        listforLevel = []
        for key, data in dungeonData.items():
            listforLevel.append([key, data["level_range"][0]])

        newList = constInfo.CalculateDungeonList(listforLevel)
        for c in xrange(len(newList)):
            key = newList[c][0]
            dungeonItem = self.dungeonItem(key)
            dungeonItem.SetEvent(self.ClickDungeonItem, key, c)
            self.GetChild("ListBox").AppendItem(dungeonItem)
            if c == 0:
                self.ClickDungeonItem(key, c)

    def TeleportDungeon(self):
        self.questionDialog = uiCommon.QuestionDialog()
        self.questionDialog.SetText(localeInfo.DUNGEONINFO_WARP)
        self.questionDialog.SetAcceptText(localeInfo.UI_ACCEPT)
        self.questionDialog.SetCancelText(localeInfo.UI_CLOSE)
        self.questionDialog.SetAcceptEvent(lambda arg = 1: self.AnswerTeleport(arg))
        self.questionDialog.SetCancelEvent(lambda arg = 0: self.AnswerTeleport(arg))
        self.questionDialog.Open()

    def AnswerTeleport(self, answer):
        if not self.questionDialog:
            return
        if answer:
            if not dungeonData.has_key(self.dungeonKey):
                self.questionDialog.Close()
                self.questionDialog = None
                return
            data = dungeonData[self.dungeonKey]
            constInfo.DungeonWarp = "WARP#%d#%d" % (data["level_range"][0],self.dungeonKey)
            event.QuestButtonClick(int(constInfo.dungeon_qf_index))
        self.questionDialog.Close()
        self.questionDialog = None

    def __ClickRadioButton(self, buttonList, buttonIndex):
        try:
            btn=buttonList[buttonIndex]
        except IndexError:
            return

        for eachButton in buttonList:
            eachButton.SetUp()

        btn.Down()

    def Close(self):
        self.Hide()

    def OnPressEscapeKey(self):
        self.Close()
        return True

    def Open(self):
        self.SetTop()
        self.Show()
