if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import os
import ui
import nonplayer
import renderTarget
import mouseModule
import snd
import time
import item
import chat
import grp
import uiScriptLocale
import localeInfo
import constInfo
import ime
import wndMgr
import uiToolTip
import uiHuntingMapNames

from _weakref import proxy

IMG_PATH = "d:/ymir work/ui/public/huntsystem/"

MAX_MISSION_LEVEL = 90

TARGET_RENDER_INDEX_MAIN = 10

TARGET_RENDER_INDEX_SEL_1 = 11
TARGET_RENDER_INDEX_SEL_2 = 12

RACE_IMAGES = {
	0 : IMG_PATH + "face_warrior_m.sub",
	1 : IMG_PATH + "face_assassine_w.sub",
	2 : IMG_PATH + "face_sura_m.sub",
	3 : IMG_PATH + "face_shaman_w.sub",
	4 : IMG_PATH + "face_warrior_w.sub",
	5 : IMG_PATH + "face_assassine_m.sub",
	6 : IMG_PATH + "face_sura_w.sub",
	7 : IMG_PATH + "face_shaman_m.sub",
}

class HuntingWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.level = 0
		self.info_type = 0
		self.dest_count = 0
		self.race_item = 0
		self.isMouseWheel = FALSE
		self.ScrollBarStep = 0.2
		self.curScrollbarPos = 0.0
		self.destScrollbarPos = 0.0
		self.targetMonster = 0
		self.AniImageObjects = []

		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		constInfo.HUNTING_MAIN_UI_SHOW = 0
		self.level = 0
		self.info_type = 0
		self.dest_count = 0
		self.race_item = 0
		self.isMouseWheel = FALSE
		self.ScrollBarStep = 0.2
		self.curScrollbarPos = 0.0
		self.destScrollbarPos = 0.0
		self.AnimationImage = None
		self.AniImageObjects = []

	def Show(self):
		ui.ScriptWindow.Show(self)

	def OpenMain(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
		self.ResetBoard()
		self.level = level
		self.GetChild("board").SetTitleName(uiScriptLocale.HUNTING_SYSTEM_TITLE % level)
		if level > MAX_MISSION_LEVEL:
			self.SetCompledetAllMissionsInfo()
			self.info_type = 1
			if self.HuntingMiniUI:
				self.HuntingMiniUI.Hide()
			self.Show()
			return

		if player.GetStatus(player.LEVEL) < level:
			self.SetOverLevelInfo(level)
			self.info_type = 2
			if self.HuntingMiniUI:
				self.HuntingMiniUI.Hide()
			self.Show()
			return

		self.dest_count = dest_count
		self.SetRenderTarget(monster)
		self.SetInfo(level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max)
		self.SetRaceItem(race_item, race_item_count)
		if self.HuntingMiniUI:
			self.HuntingMiniUI.SetInfo(level, monster, cur_count, dest_count)
			if constInfo.HUNTING_MINI_UI_SHOW == 1:
				self.GetChild("mini_ui_button").SetText(uiScriptLocale.HUNTING_SYSTEM_BUTTON_MINI_UI_OFF)
				self.HuntingMiniUI.Show()

		constInfo.HUNTING_MAIN_UI_SHOW = 1
		self.info_type = 0
		self.Show()

	def Close(self):
		constInfo.HUNTING_MAIN_UI_SHOW = 0
		if self.randomItemList:
			self.randomItemList.ClearList()
		self.Hide()

	def CloseWithMini(self):
		self.GetChild("mini_ui_button").SetText(uiScriptLocale.HUNTING_SYSTEM_BUTTON_MINI_UI_ON)
		self.HuntingMiniUI.Hide()
		self.Close()

	def Destroy(self):
		constInfo.HUNTING_MAIN_UI_SHOW = 0
		self.HuntingMiniUI.Hide()
		del self.HuntingMiniUI
		self.Hide()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/hunting.py")
		except:
			import exception
			exception.Abort("HuntingWindow.LoadWindow.LoadObject")

		try:
			self.randomItemList = ListBoxItems()
			self.randomItemList.SetParent(self.GetChild("item_area"))
			self.randomItemList.SetGlobalParent(self)
			self.randomItemList.SetPosition(0, 0)
			self.randomItemList.SetSize(32, 154)

			self.tooltip = uiToolTip.ToolTip()
			self.tooltip.Hide()
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.GetChild("board").SetCloseEvent(self.Close)

			self.GetChild("mini_ui_button").SetEvent(self.ButtonMiniUI)
			self.GetChild("close_button").SetEvent(self.Close)
			self.GetChild("reward_button").SetEvent(self.ButtonReciveReward)

			self.ModelPreview = ui.RenderTarget()
			self.ModelPreview.SetParent(self.GetChild("thinboard"))
			self.ModelPreview.SetSize(140, 180)
			self.ModelPreview.SetPosition(5, 5)
			self.ModelPreview.SetRenderTarget(TARGET_RENDER_INDEX_MAIN)

			self.HuntingMiniUI = HuntingMiniWindow()
			self.HuntingMiniUI.Hide()

			self.scrollBar = ScrollBar()
			self.scrollBar.SetParent(self.GetChild("scrollbar_area"))
			self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
			self.scrollBar.SetTexture(IMG_PATH + "scrollbar.sub")
			self.scrollBar.SetMovementArea(0, 0, 4, 150)
			self.scrollBar.SetPosition(0, 0)
			self.scrollBar.Show()

			race = net.GetMainActorRace()
			self.GetChild("face_icon").LoadImage(RACE_IMAGES[race])
			self.GetChild("face_icon").SetPosition(224, 27)
			self.GetChild("face_icon").SetEvent(ui.__mem_func__(self.MouseOverInRaceDesc), "mouse_over_in")
			self.GetChild("face_icon").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("face_icon").Show()

			self.GetChild("reward_button").Disable()

		except:
			import exception
			exception.Abort("HuntingWindow.LoadWindow.BindObject")

	def ResetBoard(self):
		self.GetChild("info_layer").Hide()
		self.GetChild("mini_ui_button").Hide()
		self.GetChild("close_button").Hide()
		self.GetChild("reward_button").Hide()
		self.ModelPreview.Hide()

	def SetCompledetAllMissionsInfo(self):
		self.GetChild("close_button").Show()
		self.GetChild("info_text_over_level").Hide()
		self.GetChild("info_text_complete_all").Show()
		self.GetChild("info_layer").Show()

	def SetOverLevelInfo(self, level):
		self.GetChild("close_button").Show()
		self.GetChild("info_text_over_level").SetText(uiScriptLocale.HUNTING_SYSTEM_INFO_OVER_LEVEL % level)
		self.GetChild("info_text_over_level").Show()
		self.GetChild("info_text_complete_all").Hide()
		self.GetChild("info_layer").Show()

	def SetRenderTarget(self, monster):
		renderTarget.SetBackground(TARGET_RENDER_INDEX_MAIN, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
		renderTarget.SetVisibility(TARGET_RENDER_INDEX_MAIN, TRUE)
		if monster != 0:
			renderTarget.SelectModel(TARGET_RENDER_INDEX_MAIN, monster)
		self.ModelPreview.Show()

	def SetInfo(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max):
		mob_name = str(nonplayer.GetMonsterName(monster))
		self.GetChild("mobname_text").SetText(mob_name)
		self.GetChild("dest_text").SetText(self.GetMapNameByMobVnum(monster))
		self.GetChild("amount_text").SetText(uiScriptLocale.HUNTING_SYSTEM_AMOUNT_TEXT % (cur_count, dest_count))
		self.GetChild("money_text").SetText("%s - %s" % (localeInfo.NumberToDecimalString(money_min), localeInfo.NumberToDecimalString(money_max)))
		self.GetChild("exp_text").SetText("%d%% - %d%%" % (exp_min, exp_max))
		self.GetChild("mini_ui_button").Show()
		self.GetChild("reward_button").Show()
		if cur_count >= dest_count:
			self.GetChild("gauge").LoadImage("d:/ymir work/ui/public/huntsystem/gauge_green.sub")
			self.GetChild("reward_button").Enable()
			self.GetChild("reward_button").EnableFlash()
		else:
			self.GetChild("gauge").LoadImage("d:/ymir work/ui/public/huntsystem/gauge_blue.sub")
			self.GetChild("reward_button").Disable()
		self.GetChild("gauge").SetPercentage(cur_count, dest_count)

	def SetRaceItem(self, race_item, race_item_count):
		if race_item != 0:
			self.race_item = race_item
			item.SelectItem(race_item)
			self.GetChild("item_icon").LoadImage(item.GetIconImageFileName())
			self.GetChild("item_icon").SetEvent(ui.__mem_func__(self.MouseOverInRaceItem), "mouse_over_in")
			self.GetChild("item_icon").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("item_icon").Show()
			for i in xrange(3):
				self.GetChild("empty_slot_%d" % i).Hide()
		else:
			self.current_race_item = 0
			self.GetChild("item_icon").Hide()
			for i in xrange(3):
				self.GetChild("empty_slot_%d" % i).Show()

	def UpdateMission(self, count):
		self.GetChild("amount_text").SetText(uiScriptLocale.HUNTING_SYSTEM_AMOUNT_TEXT % (count, self.dest_count))
		self.GetChild("gauge").SetPercentage(count, self.dest_count)
		if count >= self.dest_count:
			self.GetChild("gauge").LoadImage("d:/ymir work/ui/public/huntsystem/gauge_green.sub")
			self.GetChild("reward_button").Enable()
			self.GetChild("reward_button").EnableFlash()
		if self.HuntingMiniUI:
			self.HuntingMiniUI.UpdateCount(count)

	def SetRandomItemTable(self, item_vnum, item_count):
		self.randomItemList.AppendObject(item_vnum, item_count, 3)
		self.randomItemList.Show()
		if self.randomItemList.GetObjectCount() >= 5:
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def ButtonReciveReward(self):
		net.SendHuntingAction(3, 0)

	def ButtonMiniUI(self):
		if constInfo.HUNTING_MINI_UI_SHOW == 0:
			constInfo.HUNTING_MINI_UI_SHOW = 1
			self.GetChild("mini_ui_button").SetText(uiScriptLocale.HUNTING_SYSTEM_BUTTON_MINI_UI_OFF)
			self.HuntingMiniUI.Show()
		elif constInfo.HUNTING_MINI_UI_SHOW == 1:
			constInfo.HUNTING_MINI_UI_SHOW = 0
			self.GetChild("mini_ui_button").SetText(uiScriptLocale.HUNTING_SYSTEM_BUTTON_MINI_UI_ON)
			self.HuntingMiniUI.Hide()

	def GetMapNameByMobVnum(self, mobvnum):
		for key in uiHuntingMapNames.MAP_NAMES_FROM_MOB:
			for mob in uiHuntingMapNames.MAP_NAMES_FROM_MOB[key]:
				if mob == mobvnum:
					return str(key)
		return "Unknown Map"

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar.IsShow():
			if nLen > 0:
				pos = self.destScrollbarPos - self.ScrollBarStep
			else:
				pos = self.destScrollbarPos + self.ScrollBarStep
			pos = max(0.0, pos)
			pos = min(1.0, pos)

			self.isMouseWheel = TRUE
			self.destScrollbarPos = pos

	def MouseOverInRaceDesc(self):
		if self.tooltip:
			self.tooltip.ClearToolTip()
			self.tooltip.AppendDescription(uiScriptLocale.HUNTING_SYSTEM_RACE_TOOLTIP, 36)
			self.tooltip.ShowToolTip()

	def MouseOverInRaceItem(self):
		if self.tooltipItem and self.race_item != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.race_item, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverOut(self):
		if self.tooltip:
			self.tooltip.Hide()
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def OnUpdate(self):
		if self.info_type == 2 and player.GetStatus(player.LEVEL) >= self.level:
			net.SendHuntingAction(1, 0)
			self.info_type = 0

		if self.isMouseWheel:
			self.curScrollbarPos += (self.destScrollbarPos - self.curScrollbarPos) / 10.0
			if abs(self.curScrollbarPos - self.destScrollbarPos) < 0.005:
				self.curScrollbarPos = self.destScrollbarPos
				self.isMouseWheel = FALSE
			self.scrollBar.SetPos(self.curScrollbarPos)
		else:
			self.curScrollbarPos = self.scrollBar.GetPos()

	def OnScroll(self):
		self.randomItemList.OnScroll(self.scrollBar.GetPos())

class HuntingSelectWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.RenderTargetList = []
		self.RaceItem = [[],[]]
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)

	def OpenSelect(self, level, type, monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
		if level != 0:
			self.RaceItem[type].append(race_item)
			self.RaceItem[type].append(race_item_count)
			self.SetRenderTargetModel(type, monster)
			self.SetInformation(level, type, monster, count, money_min, money_max, exp_min, exp_max)
			self.SetRaceItem(type, race_item, race_item_count)
			if type == 1:
				self.scrollBar1.SetPosition(0, 0)
				self.scrollBar2.SetPosition(0, 0)
				self.Show()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Failure to open Huntin Select Window" % type)

	def Close(self):
		self.RaceItem = [[],[]]
		if self.randomItemList1:
			self.randomItemList1.ClearList()
		if self.randomItemList2:
			self.randomItemList2.ClearList()
		self.Hide()

	def Destroy(self):
		self.Hide()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/huntingselect.py")
		except:
			import exception
			exception.Abort("HuntingWindow.LoadWindow.LoadObject")

		try:
			self.main_race = net.GetMainActorRace()

			self.tooltip = uiToolTip.ToolTip()
			self.tooltip.Hide()
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.board = self.GetChild("board")
			self.board.SetCloseEvent(self.Close)

			self.randomItemList1 = ListBoxItems()
			self.randomItemList1.SetParent(self.GetChild("item_area_0"))
			self.randomItemList1.SetGlobalParent(self)
			self.randomItemList1.SetPosition(0, 0)
			self.randomItemList1.SetSize(32, 154)

			self.randomItemList2 = ListBoxItems()
			self.randomItemList2.SetParent(self.GetChild("item_area_1"))
			self.randomItemList2.SetGlobalParent(self)
			self.randomItemList2.SetPosition(0, 0)
			self.randomItemList2.SetSize(32, 154)

			self.scrollBar1 = ScrollBar()
			self.scrollBar1.SetParent(self.GetChild("scrollbar_area_0"))
			self.scrollBar1.SetScrollEvent(ui.__mem_func__(self.OnScrollList1))
			self.scrollBar1.SetTexture(IMG_PATH + "scrollbar.sub")
			self.scrollBar1.SetMovementArea(0, 0, 4, 150)
			self.scrollBar1.SetPosition(0, 0)
			self.scrollBar1.Show()

			self.scrollBar2 = ScrollBar()
			self.scrollBar2.SetParent(self.GetChild("scrollbar_area_1"))
			self.scrollBar2.SetScrollEvent(ui.__mem_func__(self.OnScrollList2))
			self.scrollBar2.SetTexture(IMG_PATH + "scrollbar.sub")
			self.scrollBar2.SetMovementArea(0, 0, 4, 150)
			self.scrollBar2.SetPosition(0, 0)
			self.scrollBar2.Show()

			self.GetChild("face_icon_0").LoadImage(RACE_IMAGES[self.main_race])
			self.GetChild("face_icon_0").SetPosition(224, 27)
			self.GetChild("face_icon_0").SetEvent(ui.__mem_func__(self.MouseOverInRaceDesc), "mouse_over_in")
			self.GetChild("face_icon_0").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("face_icon_0").Show()

			self.GetChild("face_icon_1").LoadImage(RACE_IMAGES[self.main_race])
			self.GetChild("face_icon_1").SetPosition(224, 27)
			self.GetChild("face_icon_1").SetEvent(ui.__mem_func__(self.MouseOverInRaceDesc), "mouse_over_in")
			self.GetChild("face_icon_1").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("face_icon_1").Show()

			self.GetChild("select_button_0").EnableFlash()
			self.GetChild("select_button_1").EnableFlash()
			self.GetChild("select_button_0").SetEvent(ui.__mem_func__(self.OnButtonSelect), 0)
			self.GetChild("select_button_1").SetEvent(ui.__mem_func__(self.OnButtonSelect), 1)

			for i in xrange(2):
				if i == 0:
					index = TARGET_RENDER_INDEX_SEL_1
				else:
					index = TARGET_RENDER_INDEX_SEL_2
				self.ModelPreview = ui.RenderTarget()
				self.ModelPreview.SetParent(self.GetChild("board_%d" % i))
				self.ModelPreview.SetSize(140, 180)
				self.ModelPreview.SetPosition(5, 5)
				self.ModelPreview.SetRenderTarget(index)
				self.ModelPreview.Show()
				renderTarget.SetBackground(index, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
				renderTarget.SetVisibility(index, TRUE)
				self.RenderTargetList.append(self.ModelPreview)

		except:
			import exception
			exception.Abort("HuntingSelectWindow.LoadWindow.BindObject")

	def SetRenderTargetModel(self, type, monster):
		index = TARGET_RENDER_INDEX_SEL_1
		if type == 1:
			index = TARGET_RENDER_INDEX_SEL_2
		renderTarget.SelectModel(index, monster)

	def SetInformation(self, level, type, monster, count, money_min, money_max, exp_min, exp_max):
		self.board.SetTitleName(uiScriptLocale.HUNTING_SYSTEM_TITLE_SELECT % level)
		mob_name = str(nonplayer.GetMonsterName(monster))
		self.GetChild("mobname_text_%d" % type).SetText(mob_name)
		self.GetChild("dest_text_%d" % type).SetText(self.GetMapNameByMobVnum(monster))
		self.GetChild("amount_text_%d" % type).SetText("%d" % count)
		self.GetChild("money_text_%d" % type).SetText("%s - %s" % (localeInfo.NumberToDecimalString(money_min), localeInfo.NumberToDecimalString(money_max)))
		self.GetChild("exp_text_%d" % type).SetText("%d%% - %d%%" % (exp_min, exp_max))

	def SetRaceItem(self, type, race_item, race_item_count):
		if race_item != 0:
			item.SelectItem(race_item)
			self.GetChild("item_icon_%d" % type).LoadImage(item.GetIconImageFileName())
			if type == 0:
				self.GetChild("item_icon_%d" % type).SetEvent(ui.__mem_func__(self.MouseOverInRaceItem1), "mouse_over_in")
			if type == 1:
				self.GetChild("item_icon_%d" % type).SetEvent(ui.__mem_func__(self.MouseOverInRaceItem2), "mouse_over_in")
			self.GetChild("item_icon_%d" % type).SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("item_icon_%d" % type).Show()
			for i in xrange(3):
				self.GetChild("empty_slot_%d_%d" % (i, type)).Hide()
		else:
			self.GetChild("item_icon_%d" % type).Hide()
			for i in xrange(3):
				self.GetChild("empty_slot_%d_%d" % (i, type)).Show()

	def SetRandomItemTable(self, item_vnum, item_count):
		self.randomItemList1.AppendObject(item_vnum, item_count, 3)
		self.randomItemList2.AppendObject(item_vnum, item_count, 3)
		self.randomItemList1.Show()
		self.randomItemList2.Show()
		if self.randomItemList1.GetObjectCount() >= 5:
			self.scrollBar1.Show()
			self.scrollBar2.Show()
		else:
			self.scrollBar1.Hide()
			self.scrollBar2.Hide()

	def OnButtonSelect(self, type):
		net.SendHuntingAction(2, type)
		self.Close()

	def GetMapNameByMobVnum(self, mobvnum):
		for key in uiHuntingMapNames.MAP_NAMES_FROM_MOB:
			for mob in uiHuntingMapNames.MAP_NAMES_FROM_MOB[key]:
				if mob == mobvnum:
					return str(key)
		return "Unknown Map"

	def MouseOverInRaceDesc(self):
		if self.tooltip:
			self.tooltip.ClearToolTip()
			self.tooltip.AppendDescription(uiScriptLocale.HUNTING_SYSTEM_RACE_TOOLTIP, 36)
			self.tooltip.ShowToolTip()

	def MouseOverInRaceItem1(self):
		if self.tooltipItem and self.RaceItem[0][0] != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.RaceItem[0][0], metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverInRaceItem2(self):
		if self.tooltipItem and self.RaceItem[1][0] != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.RaceItem[1][0], metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverOut(self):
		if self.tooltip:
			self.tooltip.Hide()
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def OnRunMouseWheel(self, nLen):
		return

	def OnScrollList1(self):
		self.randomItemList1.OnScroll(self.scrollBar1.GetPos())

	def OnScrollList2(self):
		self.randomItemList2.OnScroll(self.scrollBar2.GetPos())

class HuntingRewardWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.startetAni = 1
		self.startNextAni = 0
		self.startingAni = []
		self.cur = [0,0,0]
		self.dest = [0,0,0]

		self.clickOpenButton = FALSE
		self.showRandomItemName = FALSE

		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.startNextAni = 0
		self.startingAni = []

	def Show(self, level = 0):
		ui.ScriptWindow.Show(self)

	def OpenReward(self, level, reward, reward_count, rand_reward, rand_reward_count, money, exp):
		self.clickOpenButton = FALSE
		self.showRandomItemName = FALSE
		self.level = level
		self.reward = reward
		self.reward_count = reward_count
		self.rand_reward = rand_reward
		self.rand_reward_count = rand_reward_count
		self.money = money
		self.exp = exp
		self.dest = [rand_reward_count,money,exp]

		self.startNextAni = 0
		self.startetAni = 1

		for i in xrange(4):
			self.GetChild("empty_slot_%d" % i).Show()
			self.GetChild("ani_slot_%d" % i).Show()

		self.GetChild("random_item_name").SetText("")
		self.GetChild("money_text").SetText("")
		self.GetChild("exp_text").SetText("")

		self.clickOpenButton = FALSE
		self.showRandomItemName = FALSE
		self.cur = [0,0,0]
		self.GetChild("ButtonRecive").SetText(uiScriptLocale.HUNTING_SYSTEM_BUTTON_REWARD_OPEN)

		self.GetChild("board").SetTitleName(uiScriptLocale.HUNTING_SYSTEM_TITLE_REWARD % self.level)
		self.GetChild("ButtonRecive").EnableFlash()

		self.CreateBrokeAnimation()

		ui.ScriptWindow.Show(self)

	def Close(self):
		if self.clickOpenButton == FALSE:
			chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.HUNTING_SYSTEM_BUTTON_REWARD_CLOSE)
			return
		else:
			self.GetChild("item_icon").Hide()
			self.GetChild("item_icon_random").Hide()
			net.SendHuntingAction(1, 0)
		self.Hide()

	def Destroy(self):
		self.Hide()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/huntingreward.py")
		except:
			import exception
			exception.Abort("HuntingWindow.LoadWindow.LoadObject")
			
		try:
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.GetChild("board").SetCloseEvent(self.Close)
			self.GetChild("ButtonRecive").SetEvent(self.ButtonReciveReward)

		except:
			import exception
			exception.Abort("HuntingWindow.LoadWindow.BindObject")

	def CreateBrokeAnimation(self):
		FRAMES_COUNT = 29
		self.AniImageObjects = []
		self.startingAni = []
		if self.reward != 0:
			item.SelectItem(self.reward)
			self.GetChild("item_icon").LoadImage(item.GetIconImageFileName())
			self.GetChild("item_icon").SetEvent(ui.__mem_func__(self.MouseOverInRaceItem), "mouse_over_in")
			self.GetChild("item_icon").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("item_icon").Show()
			for i in xrange(3):
				self.startingAni.append([0,0])
				self.AnimationImage = ui.AniImageBox()
				self.AnimationImage.SetParent(self.GetChild("ani_slot_%d" % i))
				self.AnimationImage.SetDelay(4)
				self.AnimationImage.SetEndFrameEvent(ui.__mem_func__(self.HideBrokeSlotAni), i)
				for s in xrange(FRAMES_COUNT+1):
					self.AnimationImage.AppendImage("d:/ymir work/ui/effect/slot_broke_key/frame_%d.sub" % (s))
					self.AnimationImage.Show()
				self.AnimationImage.SetPosition(-24, -24)
				self.AnimationImage.ResetFrame()
				self.AnimationImage.Hide()
				self.AniImageObjects.append(self.AnimationImage)
		else:
			for i in xrange(3):
				self.GetChild("empty_slot_%d" % i).LoadImage("d:/ymir work/ui/pet/skill_button/skill_enable_button.sub")
				self.GetChild("ani_slot_%d" % i).Hide()

		for obj in xrange(len(self.AniImageObjects)):
			self.AniImageObjects[obj].Hide()

		if self.rand_reward != 0:
			item.SelectItem(self.rand_reward)
			self.GetChild("item_icon_random").LoadImage(item.GetIconImageFileName())
			self.GetChild("item_icon_random").SetEvent(ui.__mem_func__(self.MouseOverInRandomItem), "mouse_over_in")
			self.GetChild("item_icon_random").SetEvent(ui.__mem_func__(self.MouseOverOut), "mouse_over_out")
			self.GetChild("item_icon_random").Show()

			self.AnimationImage = ui.AniImageBox()
			self.AnimationImage.SetParent(self.GetChild("ani_slot_3"))
			self.AnimationImage.SetDelay(4)
			self.AnimationImage.SetEndFrameEvent(ui.__mem_func__(self.HideBrokeSlotAni), 3)
			for x in xrange(FRAMES_COUNT+1):
				self.AnimationImage.AppendImage("d:/ymir work/ui/effect/slot_broke_key/frame_%d.sub" % (x))
				self.AnimationImage.Show()
			self.AnimationImage.SetPosition(-24, -24)
			self.AnimationImage.ResetFrame()
			self.AnimationImage.Hide()
			self.AniImageObjects.append(self.AnimationImage)
			self.startingAni.append([0,0])
		else:
			self.GetChild("empty_slot_3").LoadImage("d:/ymir work/ui/pet/skill_button/skill_enable_button.sub")
			self.GetChild("ani_slot_3").Hide()

	def ShowBrokeSlotAni(self, object = 0):
		if len(self.startingAni) != 0:
			self.startingAni[0][0] = 1
			self.startetAni = 1
			self.startNextAni = time.clock() + 0.3

	def HideBrokeSlotAni(self, object):
		if len(self.startingAni) == 1:
			self.AniImageObjects[0].Hide()
			self.GetChild("empty_slot_3").Hide()
			self.GetChild("ani_slot_3").Hide()
		else:
			self.AniImageObjects[object].Hide()
			self.GetChild("empty_slot_%d" % object).Hide()
			self.GetChild("ani_slot_%d" % object).Hide()

	def ButtonReciveReward(self):
		if self.clickOpenButton == FALSE:
			self.clickOpenButton = TRUE
			self.ShowBrokeSlotAni()
			self.GetChild("ButtonRecive").SetText(uiScriptLocale.CLOSE)
			self.GetChild("ButtonRecive").DisableFlash()
			net.SendHuntingAction(4, 0)
		else:
			self.Close()

	def MouseOverInRaceItem(self):
		if self.tooltipItem and self.reward != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.reward, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverInRandomItem(self):
		if self.tooltipItem and self.rand_reward != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.rand_reward, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()

	def OnUpdate(self):
		if len(self.startingAni) == 1:
			if self.startNextAni != 0 and self.startNextAni < time.clock() and self.startingAni[0][1] == 0:
				self.startingAni[0][1] = 1
				self.AniImageObjects[0].ResetFrame()
				self.AniImageObjects[0].Show()
				self.GetChild("empty_slot_3").Hide()
				snd.PlaySound("sound/ui/broken_glass.wav")
				self.showRandomItemName = TRUE
		else:
			if self.startetAni < len(self.startingAni):
				if self.startNextAni != 0 and self.startNextAni < time.clock() and self.startingAni[self.startetAni][1] == 0:
					self.startNextAni = time.clock() + 0.3
					self.startingAni[self.startetAni][0] = 1
					self.startetAni += 1

				if self.startetAni-1 < 4 and self.startingAni[self.startetAni-1][0] == 1 and self.startingAni[self.startetAni-1][1] == 0:
					self.startingAni[self.startetAni-1][1] = 1
					self.AniImageObjects[self.startetAni-1].ResetFrame()
					self.AniImageObjects[self.startetAni-1].Show()
					self.GetChild("empty_slot_%d" % int(self.startetAni-1)).Hide()
					snd.PlaySound("sound/ui/broken_glass.wav")
					if self.startetAni-1 == 3:
						self.showRandomItemName = TRUE

		if self.clickOpenButton == TRUE:
			if self.showRandomItemName == TRUE:
				self.cur[0] += (self.dest[0] - self.cur[0]) / 50.0
				if abs(self.cur[0] - self.dest[0]) < 0.5:
					self.cur[0] = self.dest[0]
				item.SelectItem(self.rand_reward)
				self.GetChild("random_item_name").SetText("%dx %s" %(self.cur[0], item.GetItemName()))

			self.cur[1] += (self.dest[1] - self.cur[1]) / 15.0
			if abs(self.cur[1] - self.dest[1]) < 0.8:
				self.cur[1] = self.dest[1]
			self.GetChild("money_text").SetText("%s" % (localeInfo.NumberToDecimalString(int(self.cur[1]))))

			self.cur[2] += (self.dest[2] - self.cur[2]) / 50.0
			if abs(self.cur[2] - self.dest[2]) < 0.5:
				self.cur[2] = self.dest[2]
			self.GetChild("exp_text").SetText("%d%%" %(self.cur[2]))

class HuntingMiniWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.level = 0
		self.mob = 0
		self.cur_count = 0
		self.dest_count = 0

		self.LoadWindow()

	def __del__(self):
		self.level = 0
		self.mob = 0
		self.cur_count = 0
		self.dest_count = 0
		ui.Window.__del__(self)

	def Show(self):
		ui.Window.Show(self)

	def Close(self):
		constInfo.HUNTING_MINI_UI_SHOW = 0
		self.Hide()

	def Destroy(self):
		self.Hide()

	def LoadWindow(self):
		try:
			self.AddFlag("movable")
			self.AddFlag("float")
			self.SetSize(152, 62)
			self.SetPosition(200, wndMgr.GetScreenHeight() - self.GetHeight() - 110)

			self.background_image = ui.MakeExpandedImageBox(self, "d:/ymir work/ui/public/huntsystem/mini_ui_board.tga", 0, 0, "attach")
			self.background_image.SetParent(self)
			self.background_image.Show()

			self.close_button = ui.Button()
			self.close_button.SetParent(self.background_image)
			self.close_button.SetUpVisual("d:/ymir work/ui/public/huntsystem/mini_button_close_normal.sub")
			self.close_button.SetOverVisual("d:/ymir work/ui/public/huntsystem/mini_button_close_hover.sub")
			self.close_button.SetDownVisual("d:/ymir work/ui/public/huntsystem/mini_button_close_down.sub")
			self.close_button.SetEvent(self.Close)
			self.close_button.SetPosition(136, 8)
			self.close_button.Show()

			self.title_text = ui.TextLine()
			self.title_text.SetParent(self.background_image)
			self.title_text.SetWindowHorizontalAlignCenter()
			self.title_text.SetHorizontalAlignCenter()
			self.title_text.SetFontName(localeInfo.UI_DEF_FONT_SLARGE)
			self.title_text.SetPosition(0, 8)
			self.title_text.SetOutline()
			self.title_text.SetText("Hunting Mission - Lv.10")
			self.title_text.Show()

			self.mob_text = ui.TextLine()
			self.mob_text.SetParent(self.background_image)
			self.mob_text.SetWindowHorizontalAlignCenter()
			self.mob_text.SetHorizontalAlignCenter()
			self.mob_text.SetPosition(0, 26)
			self.mob_text.SetOutline()
			self.mob_text.SetText("White Oath Commander")
			self.mob_text.Show()

			self.count_text = ui.TextLine()
			self.count_text.SetParent(self.background_image)
			self.count_text.SetHorizontalAlignCenter()
			self.count_text.SetPosition(32, 39)
			self.count_text.SetOutline()
			self.count_text.SetText("17 / 45")
			self.count_text.Show()

			self.gauge_image = ui.MakeExpandedImageBox(self, "d:/ymir work/ui/public/huntsystem/mini_gauge_blue.sub", 56, 44, "not_pick")
			self.gauge_image.SetParent(self.background_image)
			self.gauge_image.Show()

		except:
			import exception
			exception.Abort("HuntingMiniWindow.LoadWindow.BindObject")

	def SetInfo(self, level, monster, cur_count, dest_count):
		self.level = level
		self.mob = monster
		self.dest_count = dest_count
		self.UpdateCount(cur_count)
		self.title_text.SetText(uiScriptLocale.HUNTING_SYSTEM_TITLE % self.level)
		mob_name = str(nonplayer.GetMonsterName(monster))
		self.mob_text.SetText(mob_name)

	def UpdateCount(self, count):
		self.cur_count = count
		self.count_text.SetText("%d / %d" % (self.cur_count, self.dest_count))
		if self.cur_count >= self.dest_count:
			self.gauge_image.LoadImage("d:/ymir work/ui/public/huntsystem/mini_gauge_green.sub")
		else:
			self.gauge_image.LoadImage("d:/ymir work/ui/public/huntsystem/mini_gauge_blue.sub")
		self.gauge_image.SetPercentage(self.cur_count, self.dest_count)

class ListBoxItems(ui.Window):
	class Item(ui.Window):
		def __init__(self, parent, index, vnum, count):
			ui.Window.__init__(self)
			ui.Window.SetParent(self, parent)

			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.parent = proxy(parent)
			self.SetWindowName("ListBox_Item")
			self.xBase, self.yBase = 0, 0

			self.index = index
			self.vnum = vnum
			self.count = count

			self.background_image = ui.MakeExpandedImageBox(self, "d:/ymir work/ui/public/slot_base.sub", 0, 0, "not_pick")
			self.background_image.SetParent(self)
			self.background_image.Show()

			item.SelectItem(self.vnum)
			self.item_image = ui.MakeExpandedImageBox(self, item.GetIconImageFileName(), 0, 0)
			self.item_image.SetEvent(ui.__mem_func__(self.OverInItem), "mouse_over_in")
			self.item_image.SetEvent(ui.__mem_func__(self.OverOutItem), "mouse_over_out")
			self.item_image.Show()

			self.item_count = ui.NumberLine()
			self.item_count.SetParent(self.item_image)
			self.item_count.SetWindowName("itemCount")
			self.item_count.SetHorizontalAlignRight()
			self.item_count.SetNumber(str(self.count))
			self.item_count.SetPosition(32 - 4, 32 - 10)
			self.item_count.Show()

		def __del__(self):
			ui.Window.__del__(self)
			self.xBase, self.yBase = 0, 0
			self.index = 0
			self.vnum = 0
			self.count = 0
			self.background_image = None
			self.item_image = None
			self.item_count = None

		def SetBasePosition(self, x, y):
			self.xBase = x
			self.yBase = y

		def GetBasePosition(self):
			return (self.xBase, self.yBase)

		def OverInItem(self):
			if self.tooltipItem:
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AddItemData(self.vnum, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
				self.tooltipItem.ShowToolTip()

		def OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def GetIndex(self):
			return self.index

		def Show(self):
			ui.Window.Show(self)

		def OnRender(self):
			xList, yList = self.parent.GetGlobalPosition()

			if self.background_image:
				self.background_image.SetClipRect(xList, yList, xList + self.parent.GetWidth(), yList + self.parent.GetHeight())
				self.item_image.SetClipRect(xList, yList, xList + self.parent.GetWidth(), yList + self.parent.GetHeight())

			if self.item_count:
				xText, yText = self.item_count.GetGlobalPosition()
				wText, hText = self.item_count.GetWidth(), 7

				if yText < yList or (yText + hText > yList + self.parent.GetHeight()):
					self.item_count.Hide()
				else:
					self.item_count.Show()

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ListBox")
		self.globalParent = None
		self.index_list = 0
		self.object_list = []

	def __del__(self):
		ui.Window.__del__(self)
		self.globalParent = None
		self.index_list = 0
		self.object_list = []

	def GetObjectCount(self):
		count = 0
		for i in xrange(len(self.object_list)):
			count += 1
		return count

	def SetGlobalParent(self, parent):
		self.globalParent = proxy(parent)

	def OnScroll(self, scrollPos):
		totalHeight = 0
		for item in self.object_list:
			totalHeight += item.GetHeight() 

		totalHeight -= self.GetHeight()

		for i in xrange(len(self.object_list)):
			x, y = self.object_list[i].GetLocalPosition()
			xB, yB = self.object_list[i].GetBasePosition()
			setPos = yB - int(scrollPos * totalHeight)
			self.object_list[i].SetPosition(xB, setPos)

	def AppendObject(self, item_vnum, item_count, space_y):
		self.index_list += 1
		item = self.Item(self, self.index_list, item_vnum, item_count)
		item.SetSize(32, 32 + space_y)

		if len(self.object_list) == 0:
			item.SetPosition(0, 0)
			item.SetBasePosition(0, 0)
		else:
			x, y = self.object_list[-1].GetLocalPosition()
			item.SetPosition(0, y + self.object_list[-1].GetHeight())
			item.SetBasePosition(0, y + self.object_list[-1].GetHeight())

		item.Show()
		self.object_list.append(item)

	def ClearList(self):
		self.index_list = 0
		self.object_list = []

class ScrollBar(ui.DragButton):
	def __init__(self):
		ui.DragButton.__init__(self)
		self.AddFlag("float")
		self.AddFlag("movable")
		self.AddFlag("restrict_x")

		self.eventScroll = lambda *arg: None
		self.movearea = 0
		self.currentPos = 0.0

	def __del__(self):
		ui.DragButton.__del__(self)
		self.movearea = 0
		self.currentPos = 0.0
		self.eventScroll = lambda *arg: None

	def SetMovementArea(self, x, y, width, height):
		self.movearea = height - y - self.GetHeight()
		self.SetRestrictMovementArea(x, y, width, height)

	def SetTexture(self, image):
		self.SetUpVisual(image)
		self.SetOverVisual(image)
		self.SetDownVisual(image)

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		yPos = float(pos * self.movearea)

		self.SetPosition(12, yPos)
		self.OnMove()

	def GetPos(self):
		return self.currentPos

	def OnMove(self):
		(xLocal, yLocal) = self.GetLocalPosition()
		self.currentPos = float(yLocal) / float(self.movearea) 

		self.eventScroll()
