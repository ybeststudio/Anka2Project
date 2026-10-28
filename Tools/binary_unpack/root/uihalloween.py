if __USE_DYNAMIC_MODULE__:
	import pyapi

net = __import__(pyapi.GetModuleName("net"))

import ui
import snd
import uiToolTip
import item
import grp 
import wndMgr
import constInfo

class Halloween(ui.ScriptWindow):
	POZITII_X = [226,159,93,28,28,28,28,28,28,91,159,227,292,292,292,292,292,]
	POZITII_Y = [326,326,326,326,276,223,168,113,61,61,61,61,61,114,169,222,277,]
	X_LEVELS = [224,157,91,26,26,26,26,26,26,89,157,225,290,290,290,290,290,]
	Y_LEVELS = [325,325,325,325,275,222,167,112,60,60,60,60,60,113,168,221,276,]
	ITEMS = ["Jack'ýn Kabaðý (x1)","Jack'ýn Kabaðý (x1)","Jack'ýn Kabaðý (x1)","Jack'ýn Kabaðý (x2)","Jack'ýn Kabaðý (x2)",
	"Jack'ýn Kabaðý (x2)","Jack'ýn Kabaðý (x2)","Jack'ýn Kabaðý (x2)","Jack'ýn Kabaðý (x3)","Jack'ýn Kabaðý (x3)",
	"Jack'ýn Kabaðý (x3)","Jack'ýn Kabaðý (x3)","Jack'ýn Kabaðý (x4)","Jack'ýn Kabaðý (x4)","Jack'ýn Kabaðý (x4)",
	"Jack'ýn Kabaðý (x4)","Jack'ýn Kabaðý (x4)"
	]

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.toolTip = None
		self.tooltipItem = None
		self.haloun_levels = {}
		self.haloun_minidovleac = {}

		self.toolTip = uiToolTip.ItemToolTip()
		self.toolTip.HideToolTip()

		self.LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Show(self):
		self.LoadWindow()
		self.SetCenterPosition()

		ui.ScriptWindow.Show(self)
		
	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/halloween.py")
		except:
			import exception
			exception.Abort("halloween.LoadWindow.LoadObject")

		try:
			self.board = self.GetChild("board")
			self.exit = self.GetChild("Exit")
			self.wndItem = self.GetChild("grid_rewards")
			self.wndMiniGamePoints = self.GetChild("MiniGamePointsText")
			self.wndMiniGameBoard = self.GetChild("MiniGameBoard")
			self.wndMiniGameRewardImg = self.GetChild("MiniGameRewardImage")
			self.wndMiniGameReward = self.GetChild("MiniGameTakeReward")
			self.wndMiniGameIncrease = self.GetChild("MiniGameIncreaseNivel")
			self.wndMiniGameTextReq = self.GetChild("MiniGameReqText")
		except:
			import exception
			exception.Abort("halloween.__LoadWindow.BindObject")
			
		self.exit.SetEvent(ui.__mem_func__(self.Close))
		self.wndItem.ArrangeSlot(0, 2, 2, 32, 32, 120, 150)
		self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))	
		self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.wndItem.SetSlotBaseImage("d:/ymir work/halloween/slot_r.tga",1.0, 1.0, 1.0, 1.0)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.HideToolTip()
		self.wndMiniGameReward.SetEvent(ui.__mem_func__(self.TakeReward))
		self.wndMiniGameIncrease.SetEvent(ui.__mem_func__(self.IncreaseLv))
		self.SlotsHalounRewars()
		for i in range(17):
			self.haloun_levels[i] = {}
			self.haloun_levels[i]["haloun"] = ui.MakeButtonText(self.board, self.X_LEVELS[i], self.Y_LEVELS[i], "%d" % (i), "Gerekli nesne: %s" % (self.ITEMS[i]),"d:/ymir work/halloween/w_nivel.tga")
		self.haloun_minidovleac[0] = {}
		self.haloun_minidovleac[0]["mini_dovleac"] = ui.MakeButtonText(self.board, self.X_LEVELS[0], self.Y_LEVELS[0], False, "Mevcut seviyen","d:/ymir work/halloween/player_slot.tga")
	
	def Close(self):
		if self.toolTip:
			self.toolTip.HideToolTip()
	
		self.Hide()
		
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def Destroy(self):
		self.ClearDictionary()

		self.tooltipItem = None
		self.toolTip = None
		
	def TakeReward(self):
		net.SendChatPacket("/take_reward_haloun")

	def IncreaseLv(self):
		net.SendChatPacket("/increase_haloun")
	
	def OnUpdate(self):
		self.wndMiniGamePoints.SetText(str(constInfo.haloun_points))
		self.wndMiniGameTextReq.SetText(self.ITEMS[int(constInfo.haloun_lvl)+1])
		self.UpdateLevel()
		self.UpdateReward()
		
	def UpdateReward(self):
		if int(constInfo.haloun_lvl) == 3 or int(constInfo.haloun_lvl) == 8 or int(constInfo.haloun_lvl) == 12 or int(constInfo.haloun_lvl) == 16:
			self.wndMiniGameRewardImg.LoadImage("d:/ymir work/halloween/takereward_image.tga")
		else:
			self.wndMiniGameRewardImg.LoadImage("d:/ymir work/halloween/w_reward.tga")
		
	def UpdateLevel(self):
		self.index = int(constInfo.haloun_lvl)
		if constInfo.haloun_lvl == 0:
			self.haloun_minidovleac[0]["mini_dovleac"].Hide()
		else:
			self.haloun_minidovleac[0]["mini_dovleac"].Show()

		self.haloun_minidovleac[0]["mini_dovleac"].SetPosition(self.POZITII_X[self.index],self.POZITII_Y[self.index])		
	
	def GetVnum(self,index):
		try:
			return int(constInfo.haloun_rewards[int(index)]["iVnum"])
		except KeyError:
			return 0
			
	def GetCount(self,index):
		try:
			return int(constInfo.haloun_rewards[int(index)]["iCount"])
		except KeyError:
			return 0
	
	def SlotsHalounRewars(self):
		for index in range(4):
			self.wndItem.SetItemSlot(index, self.GetVnum(index), self.GetCount(index))

	def OverInItem(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetItemToolTip(self.GetVnum(slotIndex))
			
	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
