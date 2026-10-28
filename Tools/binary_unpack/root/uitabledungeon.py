if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import dungeon_info
import chat
import wndMgr
import uiToolTip
import dungeon_info
import localeInfo
import item
import nonplayer

RUTA_IMGS = "System/Dungeon/new_dungeon/"
RUTA_IMGS_JEFES = "System/Dungeon/new_dungeon/jefes/"

def FReturnInfo(func,index):
    info = [
        "id_dungeon",
        "name",
        "img",
        "lv_min",
        "lv_max",
        "party_max",
        "respawn",
        "time_room",
        "time_respawn_dungeon",
        "time_room_dungeon",
        "entrance",
        "resistance",
        "force",
        "vnum_item",
        "count_item",
        "count_finish",
        "time_finish",
        "difficulty",
        "damage_done",
        "damage_received",
        "id_boss"

    ]
    return dungeon_info.InfoDate(index)[info.index(func)]

def FReturnInfoRanking(func,index):
    info = [
        "vnum_mob",
        "type",
        "name_r",
        "extra_r"
    ]
    return dungeon_info.InfoDateRanking(index)[info.index(func)]

def FReturnInfoMision(func,index):
    info = [
        "vnum_mob_mision",
        "count_mob_mision",
        "count_mob_a_mision",
        "status_mision"
    ]

    return dungeon_info.InfoDateMision(index)[info.index(func)]

def FormatTime(time):
    m, s = divmod(time, 60)
    h, m = divmod(m, 60)
    return "%1d:%02d:%02d" % (h, m, s)

class TableDungeonWindows(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.time_dungeon_info = []
        self.elements_dungeons = {}    

        self.tooltipItem = uiToolTip.ItemToolTip()

        self.LoadWindow()
    
    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def LoadWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "System/Dungeon/script/tabledungeon.py")
            #pyScrLoader.LoadScriptFile(self, "Modulo1/tabledungeon.py")
        except:
            import exception
            exception.Abort("TableDungeonWindows.LoadWindow.LoadObject")
        try:

            self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
            
            self.ImgBase = self.GetChild("ImgBase")
            self.img_banner = self.GetChild("img_banner")
            self.scrollbar_elements = self.GetChild("scrollbar_elements")
            self.title_name_1 = self.GetChild("title_name_1")
            self.title_lvl = self.GetChild("title_lvl")
            self.title_entrance = self.GetChild("title_entrance")
            self.title_party_member = self.GetChild("title_party_member")
            self.title_respawn = self.GetChild("title_respawn")
            self.title_difficulty = self.GetChild("title_difficulty")
            self.title_force = self.GetChild("title_force")
            self.title_resistance = self.GetChild("title_resistance")
            self.title_time_room = self.GetChild("title_time_room")
            self.title_total_finish = self.GetChild("title_total_finish")
            self.title_damage_done = self.GetChild("title_damage_done")
            self.title_time_finish = self.GetChild("title_time_finish")
            self.title_damage_received = self.GetChild("title_damage_received")
            self.item_slot = self.GetChild("item_slot")
            self.teleport = self.GetChild("teleport")

            self.windows_info = self.GetChild("windows_info")
            self.windows_ranking = self.GetChild("windows_ranking")
            self.extra_text_ranking = self.GetChild("extra_text_ranking")

            self.name_top_ranking = self.GetChild("name_top_ranking")
            self.extra_top_ranking = self.GetChild("extra_top_ranking")

            self.buttons_ranking = []
            self.buttons_ranking.append(self.GetChild("button_ranking_time"))
            self.buttons_ranking.append(self.GetChild("button_ranking_kills"))
            self.buttons_ranking.append(self.GetChild("button_ranking_danger"))

            self.top1_ranking = []
            self.top1_ranking.append(self.GetChild("title_top_1_rankings"))
            self.top1_ranking.append(self.GetChild("name_top_ranking"))
            self.top1_ranking.append(self.GetChild("extra_top_ranking"))


            self.name_item_mision = self.GetChild("name_item_mision")
            self.level_mision = self.GetChild("level_mision")
            self.time_mision = self.GetChild("time_mision")
            self.icon_check = self.GetChild("icon_check")
            self.icon_check.Hide()
            self.name_mob_mision = self.GetChild("name_mob_mision")
            self.count_mision = self.GetChild("count_mision")
            self.count_mision.Hide()
            self.empezar_mision = self.GetChild("empezar_mision")

            self.empezar_mision.Hide()
            self.recibir_mision = self.GetChild("recibir_mision")

            self.recibir_mision.Hide()


            self.button_info = self.GetChild("button_info")

            self.buttons_radio = []
            self.buttons_radio.append(self.buttons_ranking[0])
            self.buttons_radio.append(self.buttons_ranking[1])
            self.buttons_radio.append(self.buttons_ranking[2])
            self.buttons_radio.append(self.button_info)

        except:
            import exception
            exception.Abort("TableDungeonWindows.Elements.LoadObject")

        self.ranking_list_info = TableDungeonListWindows(self.windows_ranking)
        self.ranking_list_info.Show()

        self.scrollbar_elements.SetScrollEvent(ui.__mem_func__(self.OnScrollResultList))
        self.windows_info.Hide()
        self.windows_ranking.Hide()

        self.button_info.SetEvent(self.LoadPageInfo,0)

        self.LoadPageInfo(0)

        count_ranking = 0
        for buttons_ranking in self.buttons_ranking:
            buttons_ranking.SetEvent(ui.__mem_func__(self.SetButtonRanking),count_ranking)
            count_ranking += 1

    def __ClickRadioButton(self, buttonIndex):
        selButton=self.buttons_radio[buttonIndex]

        for i in xrange(0,len(self.buttons_radio)):
            self.buttons_radio[i].SetUp()

        selButton.Down()

    def LoadPageInfo(self,index):

        if index == 0:
            if not self.windows_info.IsShow():
                self.windows_info.Show()
                self.windows_ranking.Hide()
                self.Top1RankingFunc(False)
                self.__ClickRadioButton(3)

        if index == 1:
            if self.windows_info.IsShow():
                self.windows_info.Hide()

            if not self.windows_ranking.IsShow():
                self.windows_ranking.Show()
                self.Top1RankingFunc(True)


    def Top1RankingFunc(self,func):
        for top1r in self.top1_ranking:
            if func:
                top1r.Show()
            else:
                top1r.Hide()

    def SetButtonRanking(self,index):

        self.LoadPageInfo(1)
        ranking_title_extra = [localeInfo.TITLE_DUNGEON_INFO_TIME_DONE_RANKING,localeInfo.TITLE_DUNGEON_INFO_MAXIMUM_DEATHS,localeInfo.TITLE_DUNGEON_INFO_MAXIMUM_DAMAGE]
        self.extra_text_ranking.SetText("{}".format(ranking_title_extra[index]))
        self.LoadElementsRanking(self.elements_dungeons["index_select_mob"],index)
        self.__ClickRadioButton(index)

    def LoadDateInfo(self):
        self.LoadElements()

        self.time_dungeon_info = []

        for a in range(self.list_count):
            self.time_dungeon_info.append([FReturnInfo("time_respawn_dungeon",a)+app.GetGlobalTimeStamp(),FReturnInfo("time_room_dungeon",a)+app.GetGlobalTimeStamp()])
            self.elements_dungeons["time_respawn_actual_%d"%a] = FReturnInfo("time_respawn_dungeon",a)+app.GetGlobalTimeStamp()
            self.elements_dungeons["time_room_actual_%d"%a] = FReturnInfo("time_room_dungeon",a)+app.GetGlobalTimeStamp()

        self.FuncSelectDungeon(0,0)


    def LoadElementsRanking(self,index,index_a = 0):
        self.ranking_list_info.ClearRankingDate()
        self.ranking_list_info.SetRankingDate(FReturnInfo("id_boss",index),index_a)

        self.ranking_top_1 = self.ranking_list_info.GetRankingTop1()
        self.Top1RankingFunc(False)
        if len(self.ranking_top_1) > 0:
            self.name_top_ranking.SetText("{}".format(self.ranking_top_1[0][0]))
            self.extra_top_ranking.SetText("{}".format(self.ranking_top_1[0][1]))
            self.Top1RankingFunc(True)

        if self.windows_ranking.IsShow():
            self.__ClickRadioButton(index_a)

    def LoadElements(self):
        self.page_count = dungeon_info.GetPageCount()
        self.pages_view = dungeon_info.GetPageView()
        self.list_count = dungeon_info.CountDate()
        
        for i in xrange(0,self.pages_view):
            self.elements_dungeons["select_boss_%d"%i] = ui.Button()
            self.elements_dungeons["select_boss_%d"%i].SetParent(self.ImgBase)
            self.elements_dungeons["select_boss_%d"%i].SetUpVisual(RUTA_IMGS+"boss_button_1.tga")
            self.elements_dungeons["select_boss_%d"%i].SetOverVisual(RUTA_IMGS+"boss_button_2.tga")
            self.elements_dungeons["select_boss_%d"%i].SetDownVisual(RUTA_IMGS+"boss_button_3.tga")
            self.elements_dungeons["select_boss_%d"%i].SetPosition(16,30*i+15)
            self.elements_dungeons["select_boss_%d"%i].Hide()

        for i in xrange(0,self.pages_view):
            index = i+(self.page_count-1)

            self.elements_dungeons["select_boss_%d"%i].SetEvent(self.FuncSelectDungeon,index,i)
            self.elements_dungeons["select_boss_%d"%i].Show()

            self.elements_dungeons["select_boss_name_%d"%i] = ui.TextLine()
            self.elements_dungeons["select_boss_name_%d"%i].SetParent(self.elements_dungeons["select_boss_%d"%i])
            self.elements_dungeons["select_boss_name_%d"%i].SetPosition(23,5)
            self.elements_dungeons["select_boss_name_%d"%i].SetText(FReturnInfo("name",index))
            self.elements_dungeons["select_boss_name_%d"%i].SetHorizontalAlignLeft()
            self.elements_dungeons["select_boss_name_%d"%i].Show()

            self.elements_dungeons["select_boss_img_%d"%i] = ui.ImageBox()
            self.elements_dungeons["select_boss_img_%d"%i].SetParent(self.elements_dungeons["select_boss_%d"%i])
            self.elements_dungeons["select_boss_img_%d"%i].LoadImage(RUTA_IMGS_JEFES+FReturnInfo("img",index)+"_icon.tga")
            self.elements_dungeons["select_boss_img_%d"%i].SetPosition(-10,0)
            self.elements_dungeons["select_boss_img_%d"%i].Show()

            self.elements_dungeons["select_boss_time_%d"%i] = ui.TextLine()
            self.elements_dungeons["select_boss_time_%d"%i].SetParent(self.elements_dungeons["select_boss_%d"%i])
            self.elements_dungeons["select_boss_time_%d"%i].SetPosition(60+83,5)
            self.elements_dungeons["select_boss_time_%d"%i].SetText("")
            self.elements_dungeons["select_boss_time_%d"%i].SetHorizontalAlignLeft()
            self.elements_dungeons["select_boss_time_%d"%i].Hide()

            self.elements_dungeons["select_boss_status_%d"%i] = ui.TextLine()
            self.elements_dungeons["select_boss_status_%d"%i].SetParent(self.elements_dungeons["select_boss_%d"%i])
            self.elements_dungeons["select_boss_status_%d"%i].SetPosition(170,5)
            self.elements_dungeons["select_boss_status_%d"%i].SetText("")
            self.elements_dungeons["select_boss_status_%d"%i].SetHorizontalAlignCenter()
            self.elements_dungeons["select_boss_status_%d"%i].Show()

            
        self.scrollbar_elements.SetMiddleBarSize(float(3)/float(8))

    def WFunction(self, num):
        if (num + 1) != int(num+1):
            return int(num+1)
        else:
            return int(num)

    def OnScrollResultList(self):
        if self.list_count < self.pages_view+1:
            return

        scrollLineCount = float(self.list_count-self.pages_view)

        startIndex = int(scrollLineCount * self.scrollbar_elements.GetPos()+1)


        if startIndex > (self.list_count - self.pages_view)+1:
            startIndex -= 1


        dungeon_info.SetPageCount(startIndex)

        self.LoadElements()

    def OnRunMouseWheel(self, nLen):
        if nLen > 0:
            self.scrollbar_elements.OnUp()
        else:
            self.scrollbar_elements.OnDown()
            
    def LoadRanking(self):
        self.LoadElementsRanking(self.elements_dungeons["index_select_mob"])

    def LoadMision(self):
        self.CheckMision(self.elements_dungeons["index_select_mob"])
        
    def FuncSelectDungeon(self,index,index_a):
        dungeon_info.Ranking(FReturnInfo("id_boss",index))
        self.LoadInfoExtra(index,index_a)
        self.LoadInfoMision(index)
        self.LoadElementsRanking(index)
        self.elements_dungeons["index_select_mob"] = index


    def OverOutItem(self):
        if None != self.tooltipItem:
            self.tooltipItem.HideToolTip()

    def OverInItem(self, slotIndex):
        if None != self.tooltipItem:
            vnum_item = FReturnInfo("vnum_item",self.elements_dungeons["index_select_mob"])

            self.tooltipItem.ClearToolTip()
            self.tooltipItem.ShowRender(False)
            self.tooltipItem.AddItemData(int(vnum_item),0)

    def LoadInfoExtra(self,index,index_a):
        self.img_banner.LoadImage(RUTA_IMGS_JEFES+FReturnInfo("img",index)+".tga")
        self.title_name_1.SetText(FReturnInfo("name",index))
        self.title_lvl.SetText(localeInfo.TITLE_DUNGEON_INFO_LEVEL%(FReturnInfo("lv_min",index),FReturnInfo("lv_max",index)))
        self.title_entrance.SetText(localeInfo.TITLE_DUNGEON_INFO_LOCATION%FReturnInfo("entrance",index))
        self.title_party_member.SetText(localeInfo.TITLE_DUNGEON_INFO_GROUP_LIMIT%(1,FReturnInfo("party_max",index)))
        self.title_respawn.SetText(localeInfo.TITLE_DUNGEON_INFO_COOLDOWN%(self.FuncConvertTime(FReturnInfo("respawn",index))))
        self.title_force.SetText(localeInfo.TITLE_DUNGEON_INFO_STRENGTH%FReturnInfo("force",index))
        self.title_resistance.SetText(localeInfo.TITLE_DUNGEON_INFO_DEFENSE%FReturnInfo("resistance",index))
        self.title_time_room.SetText(localeInfo.TITLE_DUNGEON_INFO_TIME_LIMIT%(self.FuncConvertTime(FReturnInfo("time_room",index))))
        self.title_difficulty.PercentStartDungeon(FReturnInfo("difficulty",index))
        self.title_total_finish.SetText(localeInfo.TITLE_DUNGEON_INFO_TIME_PERFORMED%FReturnInfo("count_finish",index))
        self.title_damage_done.SetText(localeInfo.TITLE_DUNGEON_INFO_DAMAGE_DONE%FReturnInfo("damage_done",index))
        self.title_damage_received.SetText(localeInfo.TITLE_DUNGEON_INFO_DAMAGE_RECEIVED%FReturnInfo("damage_received",index))
        self.title_time_finish.SetText(localeInfo.TITLE_DUNGEON_INFO_RECORD_TIME%(FormatTime(FReturnInfo("time_finish",index))))
        self.item_slot.SetItemSlot(0, FReturnInfo("vnum_item",index), FReturnInfo("count_item",index))
        self.item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
        self.item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))        
        self.teleport.SetEvent(self.TeleportDungeon,index)
        self.recibir_mision.SetEvent(self.FuncMision,index,0)
        self.empezar_mision.SetEvent(self.FuncMision,index,1)
        
        self.FuncStatusTime(index,index_a)


    def LoadInfoMision(self,index):
        item.SelectItem(FReturnInfo("vnum_item",index))
        self.name_item_mision.SetText("Llave: %s"%(item.GetItemName()))
        self.level_mision.SetText("Nivel Minimo: %s"%(FReturnInfo("lv_min",index)))
        self.name_mob_mision.SetText(nonplayer.GetMonsterName(FReturnInfoMision("vnum_mob_mision",index)))
        
        self.CheckMision(index)

    def CheckMision(self,index):
        status = FReturnInfoMision("status_mision",index)
        vnum_mob = FReturnInfoMision("vnum_mob_mision",index)
        count_total = FReturnInfoMision("count_mob_mision",index)
        count_actual = FReturnInfoMision("count_mob_a_mision",index)

        if vnum_mob <= 0:
            self.recibir_mision.Hide()
            self.count_mision.Hide()
            self.empezar_mision.Hide()
            return

        if status != 0:
            self.count_mision.SetText("%d/%d"%(count_actual,count_total))
            if count_actual >= count_total:
                self.recibir_mision.Show()
                self.count_mision.Hide()
                self.empezar_mision.Hide()
            else:
                self.count_mision.Show()
                self.recibir_mision.Hide()
                self.empezar_mision.Hide()
        else:
            self.empezar_mision.Show()
            self.count_mision.Hide()
            self.recibir_mision.Hide()

    def FuncMision(self,index,func):
        min_level = FReturnInfo("lv_min",index)
        vnum_mob = FReturnInfoMision("vnum_mob_mision",index)

        if player.GetStatus(player.LEVEL) < min_level:
            chat.AppendChat(1,"[DungeonSystem]Lo siento pero no cumple con el nivel necesario.")
            return

        if vnum_mob <= 0:
            return

        dungeon_info.Mision(vnum_mob)

    def FuncStatusTime(self,index,index_a):
        time_respawn = int(self.elements_dungeons["time_respawn_actual_%d"%index])
        time_room = int(self.elements_dungeons["time_room_actual_%d"%index])

        if time_respawn > 0 and time_room <= 0:
            self.elements_dungeons["select_boss_status_%d"%index_a].SetText("Cerrada %s"%(FormatTime(time_respawn)))
            self.elements_dungeons["select_boss_status_%d"%index_a].SetPackedFontColor(0xffff5f5f)

        if time_room > 0:
            self.elements_dungeons["select_boss_status_%d"%index_a].SetText("Activa %s"%(FormatTime(time_room)))
            self.elements_dungeons["select_boss_status_%d"%index_a].SetPackedFontColor(0xfff0ce7d)

        if time_respawn <= 0 and time_room <= 0:
            self.elements_dungeons["select_boss_status_%d"%index_a].SetText("Disponible")
            self.elements_dungeons["select_boss_status_%d"%index_a].SetPackedFontColor(0xffc9f06b)

    def GetLevelPlayerTeleport(self,index):
        level = player.GetStatus(player.LEVEL)
        if level >= FReturnInfo("lv_min",index):
            return True

        chat.AppendChat(1,"[DungeonSystem]Lo siento pero no cumple con el nivel necesario.")
        return False


    def TeleportDungeon(self,index):
        if self.GetLevelPlayerTeleport(index) != True:
            return

        dungeon_info.Teleport(FReturnInfo("id_dungeon",index))
        self.Close()

    def OnUpdate(self):
        if self.list_count > 0:
            for a in range(self.list_count):
                self.UpdateTime(a)

        if len(self.time_dungeon_info) > 0:
            for i in xrange(0,self.pages_view):
                index = i+(self.page_count-1)
                self.FuncStatusTime(index,i)

    def UpdateTime(self,index):
        self.elements_dungeons["time_respawn_actual_%d"%index] = int(self.time_dungeon_info[index][0]) - app.GetGlobalTimeStamp()
        self.elements_dungeons["time_room_actual_%d"%index] = int(self.time_dungeon_info[index][1]) - app.GetGlobalTimeStamp()

    def FuncConvertTime(self,seg):
        horas = int(seg / 3600)
        minutos = int((seg - (horas * 3600)) / 60)
        if horas > 0 and minutos > 0:
            return "%d Horas %d Minutos"%(horas,minutos)
        if horas > 0 and minutos == 0:
            return "%d Horas"%(horas)
        return "%d Minutos"%(minutos)

    def OnPressEscapeKey(self):
        self.Close()
        return True

    def Close(self):
        wndMgr.Hide(self.hWnd)


class TableDungeonListWindows(ui.ScriptWindow):
    MAX_BAR = 4
    MAX_TOP = 8
    def __init__(self,parent):
        ui.ScriptWindow.__init__(self)
        self.parent = parent
        self.LoadWindow()
    
    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def LoadWindow(self):
        self.elements_list = {}
        for i in xrange(0,self.MAX_BAR):
            self.elements_list["img_%d"%i] = ui.ImageBox()
            self.elements_list["img_%d"%i].SetParent(self.parent)
            self.elements_list["img_%d"%i].SetPosition(3,40*i+125)
            self.elements_list["img_%d"%i].LoadImage(RUTA_IMGS+"borde_estadisticas.tga")
            self.elements_list["img_%d"%i].Show()

        for i in xrange(0,self.MAX_TOP):
            self.elements_list["number_%d"%i] = ui.TextLine()
            self.elements_list["number_%d"%i].SetParent(self.parent)
            self.elements_list["number_%d"%i].SetPosition(45,20*i+108)
            self.elements_list["number_%d"%i].SetHorizontalAlignCenter()
            self.elements_list["number_%d"%i].SetText("")
            self.elements_list["number_%d"%i].Show()

            self.elements_list["name_%d"%i] = ui.TextLine()
            self.elements_list["name_%d"%i].SetParent(self.parent)
            self.elements_list["name_%d"%i].SetPosition(45+121,20*i+108)
            self.elements_list["name_%d"%i].SetHorizontalAlignCenter()
            self.elements_list["name_%d"%i].SetText("")
            self.elements_list["name_%d"%i].Show()

            self.elements_list["extra_%d"%i] = ui.TextLine()
            self.elements_list["extra_%d"%i].SetParent(self.parent)
            self.elements_list["extra_%d"%i].SetPosition(45+264,20*i+108)
            self.elements_list["extra_%d"%i].SetHorizontalAlignCenter()
            self.elements_list["extra_%d"%i].SetText("")
            self.elements_list["extra_%d"%i].Show()

    def ClearRankingDate(self):
        for i in xrange(0,self.MAX_TOP):
            self.elements_list["number_%d"%i].SetText("")
            self.elements_list["name_%d"%i].SetText("")
            self.elements_list["extra_%d"%i].SetText("")

    def SetRankingDate(self,vnum_mob,type_ranking_a):
        self.elements_list["info_top_1"] = []

        self.list_ranking_count = dungeon_info.CountDateRanking()
        count_position = 0
        for i in xrange(self.list_ranking_count):
            vnum_mob_ranking = FReturnInfoRanking("vnum_mob",i)
            type_ranking = FReturnInfoRanking("type",i)
            name_ranking = FReturnInfoRanking("name_r",i)
            value_ranking = FReturnInfoRanking("extra_r",i)
            # 0 = time, 1 = kills, 2 = danger
            if vnum_mob_ranking == vnum_mob:
                if type_ranking == type_ranking_a:
                    if type_ranking == 0:
                        value_ranking = FormatTime(value_ranking)
                    if len(self.elements_list["info_top_1"]) == 0:
                        self.elements_list["info_top_1"].append([name_ranking,value_ranking])
                    else:
                        self.elements_list["number_%d"%count_position].SetText("%d"%(count_position+2))
                        self.elements_list["name_%d"%count_position].SetText(name_ranking)
                        self.elements_list["extra_%d"%count_position].SetText("{}".format(value_ranking))
                        count_position += 1
    
    def GetRankingTop1(self):
        return self.elements_list["info_top_1"]
