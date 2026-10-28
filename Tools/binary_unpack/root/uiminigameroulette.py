if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui, wndMgr, localeInfo, uiCommon

ROULETTE_SLOT_MAX = player.ROULETTE_ITEM_MAX
spin_time = player.GetRouletteData(0)
edge_effect_pos_tuple = player.GetRouletteData(1)

class RouletteWindow(ui.ScriptWindow):		
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		
		self.spin_button					= None
		self.slot_edge_effect				= None
		self.items_slot						= None
		self.item_vnums						= [0 for col in range(0, ROULETTE_SLOT_MAX)]
		self.tooltipItem					= None
		self.IsTurning						= False
		self.Price							= None
		self.Soul							= None
		self.PopupDialog					= None
		self.CurrentPos						= 0
		self.NextPos						= -1
		self.SpinTourCount 					= 1
		self.lastUpdate 					= 0
		self.__LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Show(self, price, soul):
		self.__LoadWindow()
		self.Reset()
		self.Price.SetText(localeInfo.NumberToMoneyString(price))
		ui.ScriptWindow.Show(self)
		self.SetCenterPosition()
		self.SetTop()

	def Hide(self):
		wndMgr.Hide(self.hWnd)
		
	def Close(self):
		net.SoulRoulettePacket(player.ROULETTE_PACKET_CLOSE)
		
	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)
	
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def __LoadWindow(self):
		if self.isLoaded:
			return
		self.isLoaded = True
		try:
			self.__LoadScript("UIScript/minigameroulettewindow.py")
		except:
			import exception
			exception.Abort("RouletteWindow.LoadWindow.__LoadScript")
		try:
			self.spin_button				= self.GetChild("spin_button")
			self.slot_edge_effect			= self.GetChild("slot_edge_effect")			
			self.items_slot					= self.GetChild("item_slot")
			self.slot_edge_effect			= self.GetChild("slot_edge_effect")
			self.Soul						= self.GetChild("spin_button_text2")
			self.Price						= self.GetChild("spin_button_text3")
		except:
			import exception
			exception.Abort("RouletteWindow.LoadWindow.__BindObject")
			
		try:			
			self.GetChild("board").SetCloseEvent( ui.__mem_func__(self.Close) )	
			if self.items_slot:
				self.items_slot.SetOverInItemEvent(ui.__mem_func__(self.__SlotOverInItem))
				self.items_slot.SetOverOutItemEvent(ui.__mem_func__(self.__SlotOverOutItem))
			if self.spin_button:
				self.spin_button.SetEvent( ui.__mem_func__(self.TurnPacket) )
				
			self.PopupDialog = uiCommon.PopupDialog()
			self.PopupDialog.SetText(localeInfo.ROULETTE_REWARD_TEXT)
			
		except:
			import exception
			exception.Abort("RouletteWindow.LoadWindow.__BindEvent")
			
	def TurnPacket(self):
		net.SoulRoulettePacket(player.ROULETTE_PACKET_TURN)
		self.PopupDialog.Close()
		
	def EdgeEffect(self, i):
		self.slot_edge_effect.SetPosition(edge_effect_pos_tuple[i][0], edge_effect_pos_tuple[i][1])
	
	def TurnWheel(self, spin, i):
		self.spin_button.Disable()
		self.SpinTourCount = spin
		self.NextPos = i
		self.IsTurning = True
			
	def OnUpdate(self):		
		if not self.IsTurning:
			return
			
		if app.GetGlobalTime() - self.lastUpdate < spin_time[self.SpinTourCount]:
			return
			
		self.lastUpdate = app.GetGlobalTime()
		
		if self.CurrentPos == self.NextPos and self.SpinTourCount <= 0:
			net.SoulRoulettePacket(player.ROULETTE_PACKET_GIFT)
			self.IsTurning = False
			self.NextPos = -1
			self.SpinTourCount = 1
			self.spin_button.Enable()
			self.PopupDialog.Open()
		else:
			self.CurrentPos += 1
			if self.CurrentPos >= ROULETTE_SLOT_MAX:
				self.CurrentPos = 0
				if self.SpinTourCount > 0:
					self.SpinTourCount -= 1
			self.EdgeEffect(self.CurrentPos)
			
	def Reset(self):		
		self.IsTurning = False
		self.EdgeEffect(0)
		self.lastUpdate = 0	
		self.CurrentPos = 0
		self.NextPos = -1
		self.SpinTourCount = 1
		self.PopupDialog.Close()
		
	def Destroy(self):
		self.isLoaded = 0
		self.Reset()
		self.item_vnums	= [0 for col in range(0, ROULETTE_SLOT_MAX)]
		self.tooltipItem = None
		if self.Price:
			del self.Price
		if self.Soul:
			del self.Soul
		if self.spin_button:
			del self.spin_button
		if self.slot_edge_effect:
			del self.slot_edge_effect
		if self.items_slot:
			del self.items_slot
		if self.PopupDialog:
			del self.PopupDialog
		
	def SetSlotItem(self, slotIndex, item_vnum, item_count):
		if not self.items_slot or not self.item_vnums:
			return
		
		self.items_slot.SetItemSlot( slotIndex, item_vnum, item_count )
		self.item_vnums[slotIndex] = item_vnum
		
	def __SlotOverInItem(self, slotIndex):
		if self.tooltipItem:
			if self.item_vnums and self.item_vnums[slotIndex]:
				self.tooltipItem.SetItemToolTip( self.item_vnums[slotIndex] )
	
	def __SlotOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
			
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem	
		