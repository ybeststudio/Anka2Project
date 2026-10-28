if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import os
import ui
import mouseModule
import snd
import item
import chat
import grp
import uiScriptLocale
import localeInfo
import constInfo
import ime
import wndMgr

class TopEventi(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.arryfeed = [-1, -1, -1, -1, -1, -1, -1, -1, -1]
		self.__LoadWindow()
		self.itemVnum = 0
		

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)
		constInfo.itemtop1 = 0
			
	def Close(self):
		for x in range(len(self.arryfeed)):
			self.arryfeed[x] = -1
			self.TopSlot.ClearSlot(x)
			self.TopSlot.RefreshSlot()
		self.Hide()
		constInfo.FEEDWIND = 0
		constInfo.itemtop1 = 0
	
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/topeventi.py")
		except:
			import exception
			exception.Abort("topeventi.LoadWindow.LoadObject")
			
		try:
			self.WordWindow = self.GetChild("board")
			self.CloseButton = self.GetChild("Top_TitleBar")
			self.TopSlot = self.GetChild("TopItemSlot")
			self.TopButton = self.GetChild("TopButton")
			
			self.TopSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.TopSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
	
			self.TopButton.SetEvent(ui.__mem_func__(self.SendTopButton))
			self.CloseButton.SetCloseEvent(self.Close)
			
		except:
			import exception
			exception.Abort("topeventi.LoadWindow.BindObject")
	
	def SelectEmptySlot(self, selectedSlotPos):

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			if attachedItemCount > 200:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Non e' possibile inserire oggetti sovrapposti")
				return
			if attachedItemCount < 20:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sistem> En az 20 Futbol Topu yerleþtirebilirsin.")
				return
			if attachedItemCount == 1:
				attachedItemCount = 0
			
			if 50096 != attachedItemIndex:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sistem> Sadece Futbol Topu yerlestirilebilir.")
				return
				
			if 50096 != attachedItemIndex and 0 == selectedSlotPos:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sistem> Sadece Futbol Topu'nu bu slota ekleyebilirsiniz.")
				return

			if 50096 == attachedItemIndex and 0 == selectedSlotPos:
				constInfo.itemtop1 = 1
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sistem> Futbol Topu slota baþarýlý þekilde yerleþtirildi.")
		
			if player.SLOT_TYPE_INVENTORY == attachedSlotType and not attachedSlotPos in self.arryfeed:
				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				self.arryfeed[selectedSlotPos] = attachedSlotPos
				self.TopSlot.SetItemSlot(selectedSlotPos, attachedItemIndex, attachedItemCount)

			mouseModule.mouseController.DeattachObject()
	
	def SelectItemSlot(self, itemSlotIndex):
		self.arryfeed[itemSlotIndex] = -1
		self.TopSlot.ClearSlot(itemSlotIndex)
		self.TopSlot.RefreshSlot()
	
	def UseItemSlot(self, slotIndex):
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Select"+str(slotIndex))
		
	def SendTopButton(self):
		if constInfo.itemtop1 == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sistem> Futbol Topu slot üzerinde bulunmuyor.")
			return
			

		net.SendChatPacket("/topverirmisin")
		constInfo.itemtop1 = 0
		self.TopSlot.ClearSlot(0)
		self.TopSlot.RefreshSlot()
		self.Close()

	def OnPressEscapeKey(self):
		constInfo.itemtop1 = 0
		self.Close()
		return True
