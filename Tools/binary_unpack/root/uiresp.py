if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import math
import os

import background
import dbg
import item
import localeInfo
import nonplayer
import renderTarget
import constInfo
import uiChestDrop
import exception

try:
    import pickle
except ImportError:
    import cPickle as pickle

import ui

ROOT_PATH = "d:/ymir work/ui/game/resp/"
RESP_WAIT_TEXT_COLOR = 0xffbb8585
RESP_AVAILABLE_TEXT_COLOR = 0xff85bb99
TOOLTIP_TITLE_TEXT_COLOR = 0xffd4c39f
CONFIG_FILENAME = "lib/respdialop.pcl"

TOOLTIP_TEXT_DICT = {
    "HEADER_MOB": {
        "title": localeInfo.RESP_MOB_TITLE,
        "text": localeInfo.RESP_MOB_TEXT,
    },
    "HEADER_RESP": {
        "title": localeInfo.RESP_RESP_TITLE,
        "text": localeInfo.RESP_RESP_TEXT,
    },
    "DROP_BUTTON": {
        "title": localeInfo.RESP_DROP_TITLE,
        "text": localeInfo.RESP_DROP_TEXT,
    },
}

#RACE_FLAG_DICT = {
#    nonplayer.RACE_FLAG_ANIMAL: localeInfo.TARGET_RACE_FLAG_ANIMAL,
#    nonplayer.RACE_FLAG_UNDEAD: localeInfo.TARGET_RACE_FLAG_UNDEAD,
#    nonplayer.RACE_FLAG_DEVIL: localeInfo.TARGET_RACE_FLAG_DEVIL,
#    nonplayer.RACE_FLAG_HUMAN: localeInfo.TARGET_RACE_FLAG_HUMAN,
#    nonplayer.RACE_FLAG_ORC: localeInfo.TARGET_RACE_FLAG_ORC,
#    nonplayer.RACE_FLAG_MILGYO: localeInfo.TARGET_RACE_FLAG_MILGYO,
#    nonplayer.RACE_FLAG_INSECT: localeInfo.TARGET_RACE_FLAG_INSECT,
#    nonplayer.RACE_FLAG_FIRE: localeInfo.TARGET_RACE_FLAG_FIRE,
#    nonplayer.RACE_FLAG_ICE: localeInfo.TARGET_RACE_FLAG_ICE,
#    nonplayer.RACE_FLAG_DESERT: localeInfo.TARGET_RACE_FLAG_DESERT,
#    nonplayer.RACE_FLAG_TREE: localeInfo.TARGET_RACE_FLAG_TREE
#}

log = dbg.TraceError


class RespDialog(ui.ScriptWindow):
    class Grid:
        def __init__(self, width, height):
            self.grid = [False] * (width * height)
            self.width = width
            self.height = height

        def __str__(self):
            output = "Grid {}x{} Information\n".format(self.width, self.height)
            for row in range(self.height):
                for col in range(self.width):
                    output += "Status of %d: " % (row * self.width + col)
                    output += "NotEmpty, " if self.grid[row *
                                                        self.width + col] else "Empty, "
                output += "\n"

            return output

        def find_blank(self, width, height):
            if width > self.width or height > self.height:
                return -1

            for row in range(self.height):
                for col in range(self.width):
                    index = row * self.width + col
                    if self.is_empty(index, width, height):
                        return index

            return -1

        def put(self, pos, width, height):
            if not self.is_empty(pos, width, height):
                return False

            for row in range(height):
                start = pos + (row * self.width)
                self.grid[start] = True
                col = 1
                while col < width:
                    self.grid[start + col] = True
                    col += 1

            return True

        def clear(self, pos, width, height):
            if pos < 0 or pos >= (self.width * self.height):
                return

            for row in range(height):
                start = pos + (row * self.width)
                self.grid[start] = True
                col = 1
                while col < width:
                    self.grid[start + col] = False
                    col += 1

        def is_empty(self, pos, width, height):
            if pos < 0:
                return False

            row = pos // self.width
            if (row + height) > self.height:
                return False

            if (pos + width) > ((row * self.width) + self.width):
                return False

            for row in range(height):
                start = pos + (row * self.width)
                if self.grid[start]:
                    return False

                col = 1
                while col < width:
                    if self.grid[start + col]:
                        return False
                    col += 1

            return True

        def get_size(self):
            return self.width * self.height

        def reset(self):
            self.grid = [False] * (self.width * self.height)

    class MobButton(ui.NewListBoxItem, ui.ExpandedButton):
        def __init__(self, vnum):
            ui.NewListBoxItem.__init__(self)
            ui.ExpandedButton.__init__(self)

            self.event = {"over_in": None, "over_out": None}

            self.OnRender = None
            self.RegisterComponent(self)

            self.vnum = vnum

        def __del__(self):
            self.UnregisterComponent(self)

            ui.NewListBoxItem.__del__(self)
            ui.ExpandedButton.__del__(self)

        def OnMouseOverIn(self):
            ui.ExpandedButton.OnMouseOverIn(self)
            self.event["over_in"](self.vnum)

        def OnMouseOverOut(self):
            ui.ExpandedButton.OnMouseOverOut(self)
            self.event["over_out"]()

    def __init__(self):
        self.__wndRenderTargetWnd = None
        self.__wndRenderMapWnd = None
        # self.__wndMinimapDialog = None
        ui.ScriptWindow.__init__(self)
        self.__pageCount = {page: {"max": 0, "now": 0} for page in ("resp", "drop")}
        self.chestdrop = uiChestDrop.ChestDropWindow()
        self.__dropDataDict = {}
        self.__respDataDict = {}
        self.__mobVnum = 0
        self.__itemToolTip = None
        self.__lastUpdate = app.GetTime()
        self.__pendingRequests = {"resp": False, "drop": False}
        self.__LoadWindow()
        self.__LoadData()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def __LoadWindow(self):
        pythonScriptLoader = ui.PythonScriptLoader()
        pythonScriptLoader.LoadScriptFile(self, "uiscript/respdialog.py")

        self.board = self.GetChild("board")
        self.dropButton = self.GetChild("drop_button")
        self.dropSlot = self.GetChild("drop_slot")
        self.slotCount = self.dropSlot.GetSlotCount()

        self.dropWnd = self.GetChild("drop_window")
        self.respWnd = self.GetChild("resp_window")

        self.leftButton = self.GetChild("left_button")
        self.rightButton = self.GetChild("right_button")
        self.pageCountValue = self.GetChild("page_count_value")

        self.headerRespText = self.GetChild("header_resp_text")
        self.badgeTimeValue = self.GetChild("badge_time_value")
        self.badgeLevelValue = self.GetChild("badge_level_value")
        self.badgeAffectValue = self.GetChild("badge_affect_value")

        # UI script may omit these; keep None unless uncommented with matching uiscript children.
        self.headerMobCheckbox = None
        self.headerRespCheckbox = None
        #self.headerMobCheckbox = self.GetChild("header_mob_checkbox")
        #self.headerRespCheckbox = self.GetChild("header_resp_checkbox")

        self.loadingBar = self.GetChild("loading_bar")
        self.OpenWhenTeleportButton = self.GetChild("OpenWhenTeleportButton")

        self.respSlotList = [
            {
                "slot": self.GetChild("resp_slot_%02d" % (i + 1)),
                "count": self.GetChild("count_value_%02d" % (i + 1)),
                "time": self.GetChild("time_value_%02d" % (i + 1)),
                "cord": self.GetChild("cord_value_%02d" % (i + 1)),
                "button": self.GetChild("teleport_button_%02d" % (i + 1)),
                "time_value": 0,
            } for i in xrange(10)
        ]

        self.infoWndDict = {
            "HEADER_MOB": self.GetChild("header_mob_wnd"),
            "HEADER_RESP": self.GetChild("header_resp_wnd"),
            "DROP_BUTTON": self.GetChild("drop_button"),
        }

        self.respErrorText = self.GetChild("resp_error_text")

        self.listBox = ui.NewListBox()
        self.listBox.SetParent(self.GetChild("mob_board"))
        self.listBox.SetPosition(7, 28)
        # mob_board height is 336, header is ~20, so listBox height should be 336 - 28 - 5 = 303
        self.listBox.SetSize(180, 303)
        self.listBox.itemStep = 3
        self.listBox.Show()

        self.board.SetCloseEvent(ui.__mem_func__(self.Hide))
        self.dropButton.SetEvent(ui.__mem_func__(self.__OnClickDropButton))
        self.dropSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnOverInItem))
        self.dropSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OnOverOutItem))
        self.dropSlot.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UseSlotEvent)

        self.leftButton.SetEvent(ui.__mem_func__(self.__OnClickLeftButton))
        self.rightButton.SetEvent(ui.__mem_func__(self.__OnClickRightButton))

        self.OpenWhenTeleportButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableFastOpen))
        self.OpenWhenTeleportButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableFastOpen))

        for i, respSlot in enumerate(self.respSlotList):
            respSlot["button"].SetEvent(ui.__mem_func__(self.OnClickTeleportButton), i)
            respSlot["button"].ShowToolTip = lambda arg=i: self.OnOverInTeleportButton(arg)
            respSlot["button"].HideToolTip = self.OnOverOutTeleportButton

        #for key, wnd in self.infoWndDict.items():
        for key, wnd in self.infoWndDict.items():
            if type(wnd).__name__ == "Button":
                wnd.ShowToolTip = lambda arg=key: self.OnOverInInfo(arg)
                wnd.HideToolTip = self.OnOverOutInfo
            else:
                wnd.OnMouseOverIn = lambda arg=key: self.OnOverInInfo(arg)
                wnd.OnMouseOverOut = self.OnOverOutInfo

        self.dropButton.CallEvent()

        self.__wndRenderMapWnd = RenderMapDialog()
        self.__wndRenderMapWnd.Hide()

        self.__wndRenderTargetWnd = RenderTargetDialog()
        self.__wndRenderTargetWnd.Hide()

        # self.__wndMinimapDialog = MinimapDialog()
        # self.__wndMinimapDialog.SetClickEvent(ui.__mem_func__(self.OpenWindow))
        # self.__wndMinimapDialog.Hide()

        self.OnMoveWindow(*self.GetGlobalPosition())

        if constInfo.ZAPAMIETAJ_OKNO_TPMETKIBOSSY == True:
            self.OpenWhenTeleportButton.Down()

    def __OnClickEnableFastOpen(self):
        if constInfo.ZAPAMIETAJ_OKNO_TPMETKIBOSSY== False:
            constInfo.ZAPAMIETAJ_OKNO_TPMETKIBOSSY = True
        else:
            constInfo.ZAPAMIETAJ_OKNO_TPMETKIBOSSY = False

    def __SetLoading(self, state):
        if state:
            self.loadingBar.Show()
        else:
            self.loadingBar.Hide()

    def UseSlotEvent(self, selectedSlotPos):
        drop_dict = self.__dropDataDict.get(self.__mobVnum, None)

        if not drop_dict:
            return

        item.SelectItem(drop_dict[self.__LocalSlotToGlobal(selectedSlotPos)]["vnum"])
        if item.GetItemType() == item.ITEM_TYPE_GIFTBOX or item.GetItemType() == item.ITEM_TYPE_GACHA:
            if app.IsPressed(app.DIK_LCONTROL):
                isMain = not app.IsPressed(app.DIK_LSHIFT)
                if item.HasDropInfo(drop_dict[self.__LocalSlotToGlobal(selectedSlotPos)]["vnum"], isMain):
                    self.chestdrop.Open(drop_dict[self.__LocalSlotToGlobal(selectedSlotPos)]["vnum"], isMain)
                return

    @property
    def __GetCurrentPage(self):
        return "drop" if self.dropWnd.IsShow() else "resp"

    def __LocalSlotToGlobal(self, slotIndex):
        if self.__GetCurrentPage == "drop":
            return slotIndex + self.slotCount * self.__pageCount["drop"]["now"]
        else:
            return slotIndex + len(self.respSlotList) * self.__pageCount["resp"]["now"]

    def __GlobalSlotToLocal(self, slotIndex):
        if self.__GetCurrentPage == "drop":
            return slotIndex % self.slotCount
        else:
            return slotIndex % len(self.respSlotList)

    def __RefreshDropSlot(self):
        if not self.__GetCurrentPage == "drop":
            return True

        drop_data = self.__dropDataDict.get(self.__mobVnum, None)

        # Check if data is None or empty dict
        if drop_data is None or (isinstance(drop_data, dict) and len(drop_data) == 0):
            for i in xrange(self.slotCount):
                self.dropSlot.ClearSlot(i)
            self.dropSlot.RefreshSlot()
            self.__pageCount["drop"]["now"] = 0
            self.__pageCount["drop"]["max"] = 1
            # Don't send packet here, it will cause infinite loop
            # Packet should be sent only from __OnClickMobButton
            return False

        maxPage = int(math.ceil(float(max(drop_data.keys())) / float(self.slotCount)))
        self.__pageCount["drop"]["max"] = maxPage

        data_sliced = {k: drop_data.get(k, {"vnum": 0, "count": 0}) for k in
                       range(self.__LocalSlotToGlobal(0), self.__LocalSlotToGlobal(self.slotCount))}

        for slotIndex, itemData in data_sliced.items():
            if not itemData["vnum"]:
                self.dropSlot.ClearSlot(self.__GlobalSlotToLocal(slotIndex))
                continue

            self.dropSlot.SetItemSlot(
                self.__GlobalSlotToLocal(slotIndex),
                itemData["vnum"],
                itemData["count"] if itemData["count"] > 1 else 0)

        self.dropSlot.RefreshSlot()
        self.__RefreshPageButton()
        return True

    def __RefreshRespSlot(self):
        if not self.__GetCurrentPage == "resp":
            return True

        resp_data = self.__respDataDict.get(self.__mobVnum, None)

        # Check if data is None or empty list
        if resp_data is None or (isinstance(resp_data, list) and len(resp_data) == 0):
            for respSlot in self.respSlotList:
                respSlot["slot"].Hide()
            self.respErrorText.Show()
            self.__pageCount["resp"]["now"] = 0
            self.__pageCount["resp"]["max"] = 1
            # Don't send packet here, it will cause infinite loop
            # Packet should be sent only from __OnClickMobButton
            return False

        self.respErrorText.Hide()

        maxPage = int(math.ceil(float(len(resp_data)) / float(len(self.respSlotList))))
        self.__pageCount["resp"]["max"] = maxPage

        data_sliced = resp_data[self.__LocalSlotToGlobal(0):self.__LocalSlotToGlobal(len(self.respSlotList))]

        for respSlot in self.respSlotList:
            respSlot["slot"].Hide()

        for i, respData in enumerate(data_sliced):
            xPos, yPos = respData["cord"]
            (_, xBase, yBase) = background.GlobalPositionToMapInfo(xPos, yPos)

            xPoint = str(int(xPos - xBase))[:-2]
            yPoint = str(int(yPos - yBase))[:-2]

            respSlot = self.respSlotList[i]
            respSlot["slot"].Show()
            respSlot["count"].SetText(str(self.__LocalSlotToGlobal(i) + 1))
            respSlot["cord"].SetText("({}, {})".format(xPoint, yPoint))
            respSlot["time_value"] = respData["resp"]

        self.__RefreshPageButton()
        return True

    def __RefreshRespTime(self):
        resp_data = self.__respDataDict.get(self.__mobVnum, None)
        if not resp_data:
            return

        time = [data["time"] for data in resp_data]

        self.badgeTimeValue.SetText(
            self.__FormatTime2(time[0]) if all(val == time[0] for val in time) else "{}-{}".format(
                self.__FormatTime2(min(time)), self.__FormatTime2(max(time))))

    def __RefreshLevel(self):
        level = nonplayer.GetMonsterLevel(self.__mobVnum)
        lower_bound = level - 15
        upper_bound = level + 15
        lower_bound = max(lower_bound, 1)
        if level > 0:
            self.badgeLevelValue.SetText("{}-{}".format(lower_bound, upper_bound))
        else:
            self.badgeLevelValue.SetText("-")

    def __RefreshAffect(self):
        if hasattr(nonplayer, 'GetAttElementFlag'):
            data = nonplayer.GetAttElementFlag(self.__mobVnum)
            if data >= 0:
                self.badgeAffectValue.SetText(
                    ", ".join([text for flag, text in RACE_FLAG_DICT.items() if data & flag]) or "-")
            else:
                self.badgeAffectValue.SetText("-")
        else:
            self.badgeAffectValue.SetText("-")

    # def __RefreshMinimapDialog(self, mobVnum, time):
    #     if time <= app.GetGlobalTimeStamp():
    #         self.__wndMinimapDialog.countDict["metin" if self.__IsStone(mobVnum) else "boss"]["current"] += 1
    #     else:
    #         self.__wndMinimapDialog.countDict["metin" if self.__IsStone(mobVnum) else "boss"]["current"] -= 1

    #     self.__wndMinimapDialog.RefreshMobCount()

    @staticmethod
    def __FormatTime(time):
        (h, r) = divmod(time, 3600)
        (m, s) = divmod(r, 60)
        return "{:02d}:{:02d}:{:02d}".format(h, m, s)

    @staticmethod
    def __FormatTime2(time):
        m, s = divmod(time, 60)
        h, m = divmod(m, 60)
        return '{0}{1}{2}'.format('{0}h '.format(h) if h else '', '{0}m '.format(m) if m or h else '',
                                  '{0}s'.format(s) if not h and not m or s else '')

    @staticmethod
    def __IsStone(vnum):
        return vnum in range(8000, 8300)

    def __LoadData(self):
        if os.path.exists(CONFIG_FILENAME):
            file = open(CONFIG_FILENAME, "r")

            try:
                data = pickle.load(file)
                for key, value in data.items():
                    child = self.GetChild2(key)
                    if child and hasattr(child, "SetCheck"):
                        child.SetCheck(value)
            except ValueError:
                pass

            except EOFError:
                pass

            file.close()

    def __SaveData(self):
        data = {}
        for key, obj in self.ElementDictionary.items():
            if key.find("checkbox") >= 0:
                data[key] = obj.IsChecked()
        file = open(CONFIG_FILENAME, "w")
        file.write(pickle.dumps(data))
        file.write("\n")
        file.close()

    def __RefreshPageButton(self):
        curPage = self.__GetCurrentPage
        nowPage = self.__pageCount[curPage]["now"]
        maxPage = self.__pageCount[curPage]["max"]

        if nowPage < 1:
            self.leftButton.Disable()
        else:
            self.leftButton.Enable()

        if nowPage + 1 >= maxPage:
            self.rightButton.Disable()
        else:
            self.rightButton.Enable()

        self.pageCountValue.SetText(
            localeInfo.RESP_PAGE_COUNT % (nowPage + 1, max(maxPage, 1)))

    def __OnClickDropButton(self):
        # Don't set loading when just switching tabs
        if self.dropWnd.IsShow():
            self.dropWnd.Hide()
            self.respWnd.Show()
            self.__page = "resp"
        else:
            self.dropWnd.Show()
            self.respWnd.Hide()
            self.__page = "drop"

        # Just refresh slots without setting loading
        self.__RefreshRespSlot()
        self.__RefreshDropSlot()

        self.__RefreshRespTime()
        self.__RefreshLevel()
        self.__RefreshAffect()

    def __OnOverInItem(self, slotIndex):
        if self.__itemToolTip:
            drop_dict = self.__dropDataDict.get(self.__mobVnum, None)

            if not drop_dict:
                return

            self.__itemToolTip.SetItemToolTip(drop_dict[self.__LocalSlotToGlobal(slotIndex)]["vnum"])

    def __OnOverOutItem(self):
        if self.__itemToolTip:
            self.__itemToolTip.ClearToolTip()
            self.__itemToolTip.HideToolTip()

    def OnOverInInfo(self, index):
        if self.__itemToolTip:
            self.__itemToolTip.ClearToolTip()
            self.__itemToolTip.SetCannotUseItemForceSetDisableColor(False)
            text = TOOLTIP_TEXT_DICT.get(index, None)
            if text:
                self.__itemToolTip.SetDefaultFontName(localeInfo.UI_DEF_FONT + "b")
                self.__itemToolTip.AppendTextLine(text["title"], TOOLTIP_TITLE_TEXT_COLOR)
                self.__itemToolTip.SetDefaultFontName(localeInfo.UI_DEF_FONT)
                self.__itemToolTip.AppendDescription(text["text"], 26)
            self.__itemToolTip.ShowToolTip()
            self.__itemToolTip.SetCannotUseItemForceSetDisableColor(True)

    def OnOverOutInfo(self):
        if self.__itemToolTip:
            self.__itemToolTip.ClearToolTip()
            self.__itemToolTip.HideToolTip()

    def __OnOverInMobButton(self, mobVnum):
        # Always show render target preview (checkbox is optional)
        if hasattr(self, 'headerMobCheckbox') and self.headerMobCheckbox:
            if self.headerMobCheckbox.IsChecked():
                self.__wndRenderTargetWnd.Open(mobVnum)
        else:
            # If checkbox doesn't exist or is not checked, still show preview
            self.__wndRenderTargetWnd.Open(mobVnum)

    def __OnOverOutMobButton(self):
        if self.__wndRenderTargetWnd:
            self.__wndRenderTargetWnd.Close()

    def OnOverInTeleportButton(self, index):
        # If checkbox exists and is off, skip map; if UI has no checkbox, always show map on hover.
        if self.headerRespCheckbox is not None and not self.headerRespCheckbox.IsChecked():
            return
        resp_data = self.__respDataDict.get(self.__mobVnum, None)
        if not resp_data:
            return

        data = resp_data[self.__LocalSlotToGlobal(index)]

        self.__wndRenderMapWnd.Open(*data["cord"])

    def OnOverOutTeleportButton(self):
        if self.__wndRenderMapWnd:
            self.__wndRenderMapWnd.Hide()

    def __OnClickLeftButton(self):
        curPage = self.__GetCurrentPage
        nowPage = self.__pageCount[curPage]["now"]

        if nowPage < 1:
            return

        self.__pageCount[curPage]["now"] -= 1
        self.__RefreshPageButton()

        self.__SetLoading(True)
        self.__RefreshDropSlot()
        self.__RefreshRespSlot()
        self.__SetLoading(False)

    def __OnClickRightButton(self):
        curPage = self.__GetCurrentPage
        nowPage = self.__pageCount[curPage]["now"]
        maxPage = self.__pageCount[curPage]["max"]

        if nowPage + 1 >= maxPage:
            return

        self.__pageCount[curPage]["now"] += 1
        self.__RefreshPageButton()

        self.__SetLoading(True)
        self.__RefreshDropSlot()
        self.__RefreshRespSlot()
        self.__SetLoading(False)

    def __OnClickMobButton(self, vnum):
        self.__SetLoading(True)
        self.__pendingRequests = {"resp": True, "drop": True}

        self.__mobVnum = vnum

        for item in self.listBox.items:
            if item.vnum == vnum:
                item.Down()
            else:
                item.SetUp()

        name = nonplayer.GetMonsterName(vnum)
        level = nonplayer.GetMonsterLevel(vnum)

        self.headerRespText.SetText("{} (Lv. {})".format(name, level))
        self.__pageCount["drop"]["now"] = 0
        self.__pageCount["resp"]["now"] = 0

        # Clear existing data to force refresh
        if self.__mobVnum in self.__respDataDict:
            del self.__respDataDict[self.__mobVnum]
        if self.__mobVnum in self.__dropDataDict:
            del self.__dropDataDict[self.__mobVnum]

        # Send fetch packets only once here
        if self.__mobVnum:
            net.SendRespFetchRespPacket(self.__mobVnum)
            net.SendRespFetchDropPacket(self.__mobVnum)

        # Refresh slots (they won't send packets, just show current state)
        self.__RefreshRespSlot()
        self.__RefreshDropSlot()
        
        # Loading will be hidden when both SetMobRespData and SetMobDropData are called
        
        self.__RefreshRespTime()
        self.__RefreshLevel()
        self.__RefreshAffect()

    def OnClickTeleportButton(self, index):
        resp_data = self.__respDataDict.get(self.__mobVnum, None)
        if not resp_data:
            return

        data = resp_data[self.__LocalSlotToGlobal(index)]
        net.SendRespTeleportPacket(data["id"])

    def Show(self):
        ui.ScriptWindow.Show(self)
        if self.headerRespCheckbox:
            self.headerRespCheckbox.Hide()
        # Ensure loading is closed when panel is opened
        self.__SetLoading(False)

    def Hide(self):
        ui.ScriptWindow.Hide(self)
        if self.__wndRenderTargetWnd:
            self.__wndRenderTargetWnd.Close()
        if self.__wndRenderMapWnd:
            self.__wndRenderMapWnd.Hide()

    def OpenWindow(self):
        if self.IsShow():
            self.Hide()
        else:
            self.Show()

    def Destroy(self):
        self.__SaveData()

        self.ClearDictionary()

        self.listBox.ClearItems()

        self.board = None
        self.dropButton = None
        self.dropSlot = None

        self.dropWnd = None
        self.respWnd = None

        self.leftButton = None
        self.rightButton = None
        self.pageCountValue = None

        self.headerRespText = None
        self.badgeTimeValue = None
        self.badgeLevelValue = None
        self.badgeAffectValue = None

        self.headerMobCheckbox = None
        self.headerRespCheckbox = None

        self.loadingBar = None

        self.respSlotList = []
        self.infoWndDict = {}
        self.respErrorText = None
        self.listBox = None

        self.__wndRenderMapWnd.Destroy()
        del self.__wndRenderMapWnd

        # self.__wndMinimapDialog.Destroy()
        # del self.__wndMinimapDialog

        self.__wndRenderTargetWnd.Destroy()
        del self.__wndRenderTargetWnd

        self.__pageCount = {}
        self.__dropDataDict = {}
        self.__respDataDict = {}
        self.__itemToolTip = None

    def OnUpdate(self):
        local_time = app.GetTime()

        if local_time - self.__lastUpdate < 0.1:
            return

        if self.__GetCurrentPage == "resp":
            time_now = app.GetGlobalTimeStamp()
            for i, respSlot in enumerate(self.respSlotList):
                time = respSlot["time"]
                delta = respSlot["time_value"] - time_now

                if delta > 0:
                    time.SetPackedFontColor(RESP_WAIT_TEXT_COLOR)
                    time.SetText(self.__FormatTime(delta))
                else:
                    time.SetPackedFontColor(RESP_AVAILABLE_TEXT_COLOR)
                    time.SetText(localeInfo.RESP_AVAILABLE)

        self.__lastUpdate = local_time

    def OnMoveWindow(self, x, y):
        self.__wndRenderTargetWnd.SetPosition(x - self.__wndRenderTargetWnd.GetWidth() - 5,
                                              y + (self.GetHeight() - self.__wndRenderTargetWnd.GetHeight()) / 2)

        self.__wndRenderMapWnd.SetPosition(x + self.GetWidth() + 5,
                                           y + (self.GetHeight() - self.__wndRenderMapWnd.GetHeight()) / 2)

    def OnPressEscapeKey(self):
        self.Hide()
        return True

    def SetItemToolTip(self, itemToolTip):
        self.__itemToolTip = itemToolTip

    def SetMapData(self, data, currentBossCount, maxBossCount, currentMetinCount, maxMetinCount):
        # Sort by level ** grade if GetMonsterGrade exists, otherwise just by level
        if hasattr(nonplayer, 'GetMonsterGrade'):
            sort_key = lambda x: nonplayer.GetMonsterLevel(x) ** nonplayer.GetMonsterGrade(x)
        else:
            sort_key = lambda x: nonplayer.GetMonsterLevel(x)
        
        for vnum in sorted(data, key=sort_key):
            button = self.MobButton(vnum)
            button.SetPosition(0, 0)
            button.SetUpVisual(ROOT_PATH + "mob_button_01.sub")
            button.SetOverVisual(ROOT_PATH + "mob_button_02.sub")
            button.SetDownVisual(ROOT_PATH + "mob_button_03.sub")
            button.SetEvent(ui.__mem_func__(self.__OnClickMobButton), vnum)
            button.SetText(
                "{} |cffc3e0bf(Lv. {})|r".format(nonplayer.GetMonsterName(vnum), nonplayer.GetMonsterLevel(vnum)))
            button.ButtonText.SetFontName("Tahoma:12")
            button.event["over_in"] = ui.__mem_func__(self.__OnOverInMobButton)
            button.event["over_out"] = ui.__mem_func__(self.__OnOverOutMobButton)
            button.Show()

            self.listBox.AppendItem(button)

        # self.__wndMinimapDialog.Show()
        # self.__wndMinimapDialog.UpdateBossCount(currentBossCount, maxBossCount)
        # self.__wndMinimapDialog.UpdateMetinCount(currentMetinCount, maxMetinCount)

    def SetMobDropData(self, vnum, data):
        grid = self.Grid(8, 7)
        page = 0
        drop_list = {}

        for item_data in data:
            item.SelectItem(item_data["vnum"])
            w, h = item.GetItemSize()

            pos = grid.find_blank(w, h)
            if pos >= 0:
                grid.put(pos, w, h)
                drop_list[pos + page * self.slotCount] = item_data
            else:
                page += 1
                grid.reset()
                drop_list[page * self.slotCount] = item_data

        self.__dropDataDict[vnum] = drop_list
        self.__RefreshDropSlot()
        
        # Mark drop request as completed
        self.__pendingRequests["drop"] = False
        
        # Close loading only if both requests are completed
        if not self.__pendingRequests["resp"] and not self.__pendingRequests["drop"]:
            self.__SetLoading(False)

    def SetMobRespData(self, vnum, data):
        self.__respDataDict[vnum] = data
        self.__RefreshRespSlot()
        
        # Mark resp request as completed
        self.__pendingRequests["resp"] = False
        
        # Close loading only if both requests are completed
        if not self.__pendingRequests["resp"] and not self.__pendingRequests["drop"]:
            self.__SetLoading(False)

    def RefreshRest(self, id, mobVnum, time, cord):
        resp_data = self.__respDataDict.get(mobVnum)
        if resp_data:
            for data in resp_data:
                if data["id"] == id:
                    data["resp"] = time
                    data["cord"] = cord
                    break

            self.__RefreshRespSlot()
        # self.__RefreshMinimapDialog(mobVnum, time)


class RenderTargetDialog(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.__LoadWindow()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def __LoadWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "uiscript/resprendertargetdialog.py")

        except:
            import exception
            exception.Abort("RenderTargetDialog.__LoadWindow.LoadObject")

        self.renderBoard = self.GetChild("render_board")
        self.headerText = self.GetChild("header_render_text")
        self.renderTarget = self.GetChild("render_target")

        # Get render target index from UI script (index: 50)
        # Use fixed index from UI script definition
        self.renderTargetNumber = 50
        
        # Ensure render target is properly initialized
        self.renderTarget.SetRenderTarget(self.renderTargetNumber)
        self.renderTarget.Show()

        # Set background for render target
        renderTarget.SetBackground(self.renderTargetNumber, ROOT_PATH + "image_render.sub")
        renderTarget.SetRotation(self.renderTargetNumber, False)
        renderTarget.SetVisibility(self.renderTargetNumber, False)  # Hidden by default

    def Open(self, mobVnum):
        try:
            # Reset and configure render target
            renderTarget.ResetModel(self.renderTargetNumber)
            renderTarget.SelectModel(self.renderTargetNumber, mobVnum)
            # SetScale calls SelectModel internally and resets eye/target; must run before SetModelV3*
            renderTarget.SetScale(self.renderTargetNumber, 0.03)
            tx, ty, tz = 0.0, 0.0, 82.0
            bx, by, bz = 0.0, -1725.0, 750.0
            r = 1.25
            dv = 1.25
            ez0 = (bz - tz) * r + tz
            tz2 = tz * dv
            ez2 = tz2 + (ez0 - tz) * dv
            renderTarget.SetModelV3Target(self.renderTargetNumber, tx, ty, tz2)
            renderTarget.SetModelV3Eye(self.renderTargetNumber,
                (bx - tx) * r + tx, (by - ty) * r + ty, ez2)
            
            self.headerText.SetText(
                "%s  (Lv. %d)" % (nonplayer.GetMonsterName(mobVnum), nonplayer.GetMonsterLevel(mobVnum)))
            
            # Make sure render target UI element is visible
            if self.renderTarget:
                self.renderTarget.Show()
            
            self.Show()
            renderTarget.SetVisibility(self.renderTargetNumber, True)
        except Exception as e:
            import dbg
            dbg.TraceError("RenderTargetDialog.Open error: %s" % str(e))

    def Close(self):
        self.Hide()
        renderTarget.SetVisibility(self.renderTargetNumber, False)

    def Destroy(self):
        self.ClearDictionary()
        if hasattr(self, 'renderTargetNumber'):
            renderTarget.Destroy(self.renderTargetNumber)
        self.renderTarget = None
        self.headerText = None


class RenderMapDialog(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.__LoadWindow()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def __LoadWindow(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "uiscript/resprendermapdialog.py")

        except:
            import exception
            exception.Abort("RenderMapDialog.__LoadWindow.LoadObject")

        self.board = self.GetChild("map_board")
        self.header = self.GetChild("header_map")
        self.mapImage = self.GetChild("map_image")
        self.headerText = self.GetChild("header_map_text")
        self.mapPoint = self.GetChild("map_point")

        self.headerWidth = self.header.GetWidth()

    def Destroy(self):
        self.ClearDictionary()

        self.board = None
        self.header = None
        self.mapImage = None
        self.headerText = None

    def Open(self, xPos, yPos):
        (mapName, xBase, yBase) = background.GlobalPositionToMapInfo(xPos, yPos)

        localeMapName = localeInfo.MINIMAP_ZONE_NAME_DICT.get(mapName, "")
        fileName = "d:/ymir work/ui/atlas/{}/atlas.sub".format(mapName)

        if not localeMapName or not app.IsExistFile(fileName):
            dbg.TraceError("test")
            return False

        self.headerText.SetText(localeMapName)
        self.mapImage.LoadImage(fileName)

        xSize = 8 + self.mapImage.GetWidth()
        ySize = 25 + 3 + self.mapImage.GetHeight()
        self.SetSize(xSize, ySize)
        self.board.SetSize(xSize, ySize)
        self.header.SetScale(float(self.mapImage.GetWidth()) / self.headerWidth, 1.0)

        if hasattr(background, "GlobalPositionToMapSize"):
            xMap, yMap = background.GlobalPositionToMapSize(xPos, yPos)
        else:
            xMap = 0
            yMap = 0

        xPoint = int(xPos - xBase)
        yPoint = int(yPos - yBase)

        if xMap > 0 and yMap > 0:
            xPoint /= float(xMap) / float(self.mapImage.GetWidth())
            yPoint /= float(yMap) / float(self.mapImage.GetHeight())
        else:
            xPoint = self.mapImage.GetWidth() / 2
            yPoint = self.mapImage.GetHeight() / 2

        self.mapPoint.SetPosition(xPoint - (self.mapPoint.GetWidth() / 4), yPoint - self.mapPoint.GetHeight())

        self.mapImage.Show()
        self.headerText.UpdateRect()

        self.Show()
        self.UpdateRect()

        return True


# class MinimapDialog(ui.ScriptWindow):
#     def __init__(self):
#         ui.ScriptWindow.__init__(self)
#         self.countDict = {
#             type_: {
#                 "current": 0,
#                 "max": 0
#             } for type_ in ("boss", "metin")
#         }
#         self.__LoadWindow()

#     def __del__(self):
#         ui.ScriptWindow.__del__(self)

#     def __LoadWindow(self):
#         try:
#             pyScrLoader = ui.PythonScriptLoader()
#             pyScrLoader.LoadScriptFile(self, "uiscript/respminimapdialog.py")
#         except:
#             import exception
#             exception.Abort("MinimapDialog.__LoadWindow.LoadObject")

#         self.bossCountValue = self.GetChild("boss_count_value")
#         self.metinCountValue = self.GetChild("metin_count_value")

#     def Destroy(self):
#         self.ClearDictionary()

#         self.bossCountValue = None
#         self.metinCountValue = None

#     def SetClickEvent(self, event):
#         for element in self.ElementDictionary.values():
#             element.OnMouseLeftButtonUp = event

#     def RefreshMobCount(self):
#         self.UpdateBossCount(*list(self.countDict["boss"].values()))
#         self.UpdateMetinCount(*list(self.countDict["metin"].values()))

#     def UpdateBossCount(self, count, maxCount):
#         self.countDict["boss"]["current"] = count
#         self.countDict["boss"]["max"] = maxCount
#         if not maxCount:
#             self.bossCountValue.SetText("-")
#         else:
#             self.bossCountValue.SetText("{} / {}".format(count, maxCount))

#     def UpdateMetinCount(self, count, maxCount):
#         self.countDict["metin"]["current"] = count
#         self.countDict["metin"]["max"] = maxCount
#         if not maxCount:
#             self.metinCountValue.SetText("-")
#         else:
#             self.metinCountValue.SetText("{} / {}".format(count, maxCount))
