if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import item
import constInfo
import localeInfo
import wndMgr
import dbg
import uiCommon
import uiToolTip
import uiScriptLocale
from uiToolTip import ItemToolTip
AFFECT_DICT = ItemToolTip.AFFECT_DICT

class Battlepass(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.HideToolTip()

		self.tab = {}
		self.gauge = {}
		self.gauge_f = None
		self.text = {}
		self.reward1 = {}
		self.reward2 = {}
		self.reward3 = {}
		self.icon = {}
		self.list_bg = {}
		self.bitirici = {}
		self.oldugoruntu = None
		self.odulbackground = None
		self.odulbackgroundkapat = None
		self.odul = {}
		self.rewardbonus1 = {}
		self.rewardbonus2 = {}
		self.rewardbonus3 = {}
		self.LoadWindow()

		self.Type_Desc = [0,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_1,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_2,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_3,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_4,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_5,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_6,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_7,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_8,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_9,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_10,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_11,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_12,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_13,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_14,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_15,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_16,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_17,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_18,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_19,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_20,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_21,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_22,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_23,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_24,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_25,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_26,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_27,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_28,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_29,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_30,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_31,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_32,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_33,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_34,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_35,
		uiScriptLocale.BATTLEPASS_QUEST_TYPE_36,
		]

		self.Type_Images = [0,
		"new_battlepas/quest_icon/kill_id_any.png",#1
		"new_battlepas/quest_icon/kill_id_any.png",#2
		"new_battlepas/quest_icon/kill_id_any.png",#3
		"new_battlepas/quest_icon/kill_boss_uni.png",#4
		"new_battlepas/quest_icon/kill_stone_any.png",#5
		"new_battlepas/quest_icon/use_refine_id_uni.png",#6
		"new_battlepas/quest_icon/hatch_id_uni.png",#7
		"new_battlepas/quest_icon/shout.png",#8
		"new_battlepas/quest_icon/kill_boss_uni.png",#9
		"new_battlepas/quest_icon/kill_boss_uni.png",#10
		"new_battlepas/quest_icon/kill_boss_uni.png",#11
		"new_battlepas/quest_icon/kill_boss_uni.png",#12
		"new_battlepas/quest_icon/kill_boss_uni.png",#13
		"new_battlepas/quest_icon/kill_boss_uni.png",#14
		"new_battlepas/quest_icon/kill_boss_uni.png",#15
		"new_battlepas/quest_icon/kill_boss_uni.png",#16
		"new_battlepas/quest_icon/kill_boss_uni.png",#17
		"new_battlepas/quest_icon/kill_boss_uni.png",#18
		"new_battlepas/quest_icon/kill_boss_uni.png",#19
		"new_battlepas/quest_icon/kill_boss_uni.png",#20
		"new_battlepas/quest_icon/kill_boss_uni.png",#21
		"new_battlepas/quest_icon/kill_boss_any.png",#22
		"new_battlepas/quest_icon/kill_boss_any.png",#23
		"new_battlepas/quest_icon/kill_boss_any.png",#24
		"new_battlepas/quest_icon/kill_boss_any.png",#25
		"new_battlepas/quest_icon/kill_boss_any.png",#26
		"new_battlepas/quest_icon/kill_boss_any.png",#27
		"new_battlepas/quest_icon/kill_boss_any.png",#28
		"new_battlepas/quest_icon/kill_boss_any.png",#29
		"new_battlepas/quest_icon/kill_boss_any.png",#30
		"new_battlepas/quest_icon/kill_boss_any.png",#31
		"new_battlepas/quest_icon/kill_boss_any.png",#32
		"new_battlepas/quest_icon/kill_boss_any.png",#33
		"new_battlepas/quest_icon/kill_boss_any.png",#34
		"new_battlepas/quest_icon/kill_boss_any.png",#35
		"new_battlepas/quest_icon/kill_boss_any.png",#36
		]

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.LoadWindow()
		size_safe = max(1, int(constInfo.size_battle_pass))
		self.ScrollBar.SetMiddleBarSize(float(2) / float(size_safe))
		self.final_reward.SetItemSlot(0, constInfo.final_rewards[0], constInfo.final_rewards[3])
		self.final_reward.SetItemSlot(1, constInfo.final_rewards[1], constInfo.final_rewards[4])
		self.final_reward.SetItemSlot(2, constInfo.final_rewards[2], constInfo.final_rewards[5])
		for i in range(9):
			self.MakeButton(
				i,\
				self.board,\
				13, 33 + (50 * i)
			)
		self.SetCenterPosition()
		self.select = None
		ui.ScriptWindow.Show(self)

	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/battlepass.py")
		except:
			import exception
			exception.Abort("battlepass.LoadWindow.LoadObject")
		try:
			self.titleBar = self.GetChild("TitleBar")
			self.board = self.GetChild("board")
			self.ScrollBar = self.GetChild("ScrollBar")
			self.info1 = self.GetChild("Text1Info")
			self.info2 = self.GetChild("Text2Info")
			self.info3 = self.GetChild("Text3Info")
			self.info5 = self.GetChild("Text6Info")
			self.f_button = self.GetChild("FinalReward")
			self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		except:
			import exception
			exception.Abort("battlepass.__LoadWindow.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.f_button.SetEvent(lambda : net.SendChatPacket("/final_reward"))
		self.final_reward = ui.GridSlotWindow()
		self.final_reward.SetParent(self)
		self.final_reward.SetPosition(405, 360)
		self.final_reward.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.final_reward.ArrangeSlot(0, 6, 3, 32, 32, 0, 3)
		self.final_reward.SetSlotBaseImage("d:/ymir work/ui/public/chestdrop_slot.sub", 1.0, 1.0, 1.0, 1.0)

		self.final_reward.SetOverInItemEvent(ui.__mem_func__(self.OverInItemFinal))
		self.final_reward.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.final_reward.RefreshSlot()
		self.final_reward.Show()

	def Close(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
		self.Hide()

	def Destroy(self):
		self.ClearDictionary()
		self.tooltipItem = None

	def OnUpdate(self):
		if self.select != None:
			if self.Get2(self.select, "iStatus") > 0:
				self.info2.SetText(uiScriptLocale.BATTLE_PASSFINISHED)
			else:
				self.info2.SetText(uiScriptLocale.BATTLE_PASS_LOADING)

			self.info3.SetText(uiScriptLocale.BATTLE_PASS_PROGRESS + str(self.Get2(self.select, "iCounts")) + " / " + str(self.Get(self.select, "iCount")))
		self.gauge_f.SetPercentage(self.GetFinishedMission(), max(1, int(constInfo.size_battle_pass)))
		for i in range(len(self.text)):
			self.gauge[i].SetPercentage(self.Get2(i, "iCounts"), max(1, self.Get(i, "iCount")))
			if self.Get2(i, "iStatus") > 0:
				self.list_bg[i].LoadImage("new_battlepas/big_finished.png")
				self.list_bg[i].Show()
		for i in range(len(self.tab)):
			if self.tab[i].IsDown():
				self.select = i
				MissionName = str(constInfo.info_missions_bp[int(i)]["Name"]).replace("Oldur", "Oldur")
				self.info1.SetText(uiScriptLocale.BATTLE_PASS_MISSION_NAME + str(MissionName))

				if self.Get2(i, "iStatus") > 0:
					self.info2.SetText(uiScriptLocale.BATTLE_PASSFINISHED)
				else:
					self.info2.SetText(uiScriptLocale.BATTLE_PASS_LOADING)

				self.info3.SetText(uiScriptLocale.BATTLE_PASS_PROGRESS + str(self.Get2(i, "iCounts")) + " / " + str(self.Get(i, "iCount")))
				idx = self.Get(i, "iType")
				self.info5.SetText("|cffffcc00" + (self.Type_Desc[idx] if 0 <= idx < len(self.Type_Desc) else ""))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def OnScroll(self):
		board_count = 9
		pos = int(self.ScrollBar.GetPos() * max(0, int(constInfo.size_battle_pass) - board_count))

		for i in xrange(board_count):
			realPos = i + pos
			self.MakeButton(
				realPos,\
				self.board,\
				13, 33 + (50 * i)
			)

	def GetFinishedMission(self):
		finished = 0
		for i in range(constInfo.size_battle_pass):
			if int(constInfo.info_missions_bp[int(i)]["iStatus"]) > 0:
				finished = finished + 1
		return finished

	def OnRunMouseWheel(self, nLen):
		if self.IsInPosition():
			if nLen > 0 and self.ScrollBar:
				self.ScrollBar.OnUp()
			else:
				self.ScrollBar.OnDown()

			return True

		return False

	def Get(self,index, var2):
		try:
			return int(constInfo.missions_bp[int(index)][var2])
		except (KeyError, IndexError):
			return 0

	def Get2(self,index, var2):
		try:
			return int(constInfo.info_missions_bp[int(index)][var2])
		except (KeyError, IndexError):
			return 0

	def Get3(self,index, var2):
		try:
			return int(constInfo.rewards_bp[int(index)][var2])
		except (KeyError, IndexError):
			return 0

	def Get4(self, index, var2):
		try:
			return int(constInfo.rewards_bonus_bp[int(index)][var2])
		except (KeyError, IndexError):
			return 0

	def MakeButton(self, index, parent, x, y):
		size_bp = int(constInfo.size_battle_pass)
		if not size_bp or index >= size_bp:
			return

		self.tab[index] = ui.MakeButton(parent, x, y+10-3, False, "new_battlepas/", "mission_btn_0.png", "mission_btn_1.png", "mission_btn_2.png")
		self.gauge[index] = ui.MakeGauge(self.tab[index], 49, 30, 163)
		self.gauge_f = ui.MakeGauge(parent, 392, 163, 220)
		self.text[index] = ui.TextLine()
		self.text[index].SetParent(self.tab[index])
		self.text[index].SetPosition(53, 5)
		MissionName2 = str(constInfo.info_missions_bp[int(index)]["Name"]).replace("Oldur", "Oldur")
		self.text[index].SetText(str(MissionName2))
		self.text[index].Show()

		self.list_bg[index] = ui.MakeImageBox(self.tab[index], "new_battlepas/big_finished.png", 49, 30)
		self.list_bg[index].Hide()

		self.icon[index] = ui.MakeImageBoxNoImg(self.tab[index], 1, 1)

		if self.Get(index, "iType") > 0:
			self.icon[index].LoadImage(self.Type_Images[self.Get(index, "iType")])

		self.oldugoruntu = ui.ThinBoardCircle()
		self.oldugoruntu.SetParent(self)
		self.oldugoruntu.SetSize(self.GetWidth()-10, self.GetHeight()-36)
		self.oldugoruntu.SetPosition(5,30)
		self.oldugoruntu.SetAlpha(0.5)
		self.oldugoruntu.Hide()

		self.bitirici[index] = ui.MakeButton(self.tab[index], 273, 13, False, "", "new_battlepas/bonus.png", "new_battlepas/bonus.png", "new_battlepas/bonus.png")
		self.bitirici[index].SetEvent(ui.__mem_func__(self.__SelectBitirici),index)
		self.bitirici[index].SetToolTipText(localeInfo.BATTLEPASSBITIR)
		self.bitirici[index].Show()

		self.odul[index] = ui.MakeButton(self.tab[index], 308, 13, False, "", "new_battlepas/odul.png", "new_battlepas/odul.png", "new_battlepas/odul.png")
		self.odul[index].SetEvent(ui.__mem_func__(self.__SelectOdul),index)
		self.odul[index].SetToolTipText(localeInfo.BATTLEPASSODUL)
		self.odul[index].Show()

		self.odulbackground = ui.ImageBox()
		self.odulbackground.SetParent(self.oldugoruntu)
		self.odulbackground.SetPosition(205, 145)
		self.odulbackground.LoadImage("new_battlepas/yeni_gorev.png")
		self.odulbackground.Hide()

		self.odulbackgroundkapat = ui.Button()
		self.odulbackgroundkapat.SetParent(self.oldugoruntu)
		self.odulbackgroundkapat.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		self.odulbackgroundkapat.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		self.odulbackgroundkapat.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		self.odulbackgroundkapat.SetToolTipText(localeInfo.UI_CLOSE)
		self.odulbackgroundkapat.SetEvent(ui.__mem_func__(self.OdulPencereKapat))
		self.odulbackgroundkapat.SetPosition(400, 147)
		self.odulbackgroundkapat.Hide()

	def __SelectBitirici(self, index):
		bitiricisorgu = uiCommon.QuestionDialog()
		bitiricisorgu.SetAcceptEvent(lambda arg=True: self.ConfirmBitirici(index))
		bitiricisorgu.SetCancelEvent(lambda arg=False: self.CancelBitirici(index))
		bitiricisorgu.SetText(localeInfo.BATTLEPASSEP % (self.Get(index, "iEp")))
		bitiricisorgu.Open()

		self.bitiricisorgu = bitiricisorgu

	def ConfirmBitirici(self,index):
		net.SendChatPacket("/battlepass_bitirici %d" % int(index))
		self.bitiricisorgu.Close()
		self.bitiricisorgu = None
		return True

	def CancelBitirici(self,index):
		self.bitiricisorgu.Close()
		self.bitiricisorgu = None
		return True

	def __SelectOdul(self, index):
		self.oldugoruntu.Show()
		self.odulbackground.Show()
		self.odulbackgroundkapat.Show()

		self.reward1 = {}
		self.reward1[index] = ui.MakeGridSlot2(self.oldugoruntu, 255, 163, self.Get3(index, "iVnum1"), self.Get3(index, "iCount1"))
		self.reward1[index].SetOverInItemEvent(lambda slotindex = 0, ivnumz = index: self.OverInItem(slotindex, ivnumz, "iVnum1"))
		self.reward1[index].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.reward2 = {}
		self.reward2[index] = ui.MakeGridSlot2(self.oldugoruntu, 295, 163, self.Get3(index, "iVnum2"), self.Get3(index, "iCount2"))
		self.reward2[index].SetOverInItemEvent(lambda slotindex = 0, ivnumz = index: self.OverInItem(slotindex, ivnumz, "iVnum2"))
		self.reward2[index].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.reward3 = {}
		self.reward3[index] = ui.MakeGridSlot2(self.oldugoruntu, 335, 163, self.Get3(index, "iVnum3"), self.Get3(index, "iCount3"))
		self.reward3[index].SetOverInItemEvent(lambda slotindex = 0, ivnumz = index: self.OverInItem(slotindex, ivnumz, "iVnum3"))
		self.reward3[index].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		if self.Get4(index, "iVnum1") > 0 or self.Get4(index, "iVnum2") > 0 or self.Get4(index, "iVnum3") > 0:
			def _affect_text(vnum_key, count_key):
				vnum = int(self.Get4(index, vnum_key))
				cnt = int(self.Get4(index, count_key))
				if vnum in AFFECT_DICT:
					return str(AFFECT_DICT[vnum](cnt))
				return str(cnt) if cnt else ""
			self.rewardbonus1 = {}
			self.rewardbonus1[index] = ui.MakeTextLineNew(self.oldugoruntu, 250, 212, _affect_text("iVnum1", "iCount1"))
			self.rewardbonus1[index].SetPackedFontColor(0xffFFFFFF)
			self.rewardbonus1[index].SetOutline()

			self.rewardbonus2 = {}
			self.rewardbonus2[index] = ui.MakeTextLineNew(self.oldugoruntu, 250, 242, _affect_text("iVnum2", "iCount2"))
			self.rewardbonus2[index].SetPackedFontColor(0xffFFFFFF)
			self.rewardbonus2[index].SetOutline()

			self.rewardbonus3 = {}
			self.rewardbonus3[index] = ui.MakeTextLineNew(self.oldugoruntu, 250, 268, _affect_text("iVnum3", "iCount3"))
			self.rewardbonus3[index].SetPackedFontColor(0xffFFFFFF)
			self.rewardbonus3[index].SetOutline()

	def OdulPencereKapat(self):
		if self.oldugoruntu.IsShow():
			self.oldugoruntu.Hide()
		if self.odulbackground.IsShow():
			self.odulbackground.Hide()
		if self.odulbackgroundkapat.IsShow():
			self.odulbackgroundkapat.Hide()

	def OverInItem(self, slotindex, i, var):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.Get3(i, var))

	def OverInItemFinal(self, slotindex):
		if 0 != self.tooltipItem:
			self.tooltipItem.SetItemToolTip(constInfo.final_rewards[slotindex])

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnPressEscapeKey(self):
		if self.oldugoruntu.IsShow():
			self.oldugoruntu.Hide()

		if self.odulbackground.IsShow():
			self.odulbackground.Hide()

		if self.odulbackgroundkapat.IsShow():
			self.odulbackgroundkapat.Hide()

		self.Close()
		return True

	def OnCancel(self):
		self.Hide()
		return True