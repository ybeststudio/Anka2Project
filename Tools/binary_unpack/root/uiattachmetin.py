if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import dbg
import item
import snd
import ui
import uiToolTip
import localeInfo

if app.ENABLE_SLOT_MARKING_SYSTEM:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE

class AttachMetinDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadScript()

		self.metinItemPos = 0
		self.targetItemPos = 0
		
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.interface = None
			self.inven = None

	def __LoadScript(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/attachstonedialog.py")

		except:
			import exception
			exception.Abort("AttachStoneDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.metinImage = self.GetChild("MetinImage")
			self.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.Accept))
			self.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("AttachStoneDialog.__LoadScript.BindObject")

		oldToolTip = uiToolTip.ItemToolTip()
		oldToolTip.SetParent(self)
		oldToolTip.SetPosition(15, 38)
		oldToolTip.SetFollow(FALSE)
		oldToolTip.Show()
		self.oldToolTip = oldToolTip

		newToolTip = uiToolTip.ItemToolTip()
		newToolTip.SetParent(self)
		newToolTip.SetPosition(230 + 20, 38)
		newToolTip.SetFollow(FALSE)
		newToolTip.Show()
		self.newToolTip = newToolTip

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.board = 0
		self.titleBar = 0
		self.metinImage = 0
		self.toolTip = 0
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			self.inven = None
			self.interface = None

	def CanAttachMetin(self, slot, metin):
		if item.METIN_NORMAL == metin:
			if player.METIN_SOCKET_TYPE_SILVER == slot or player.METIN_SOCKET_TYPE_GOLD == slot:
				return True

		elif item.METIN_GOLD == metin:
			if player.METIN_SOCKET_TYPE_GOLD == slot:
				return True

	def Open(self, metinItemPos, targetItemPos):
		self.metinItemPos = metinItemPos
		self.targetItemPos = targetItemPos

		metinIndex = player.GetItemIndex(metinItemPos)
		itemIndex = player.GetItemIndex(targetItemPos)
		self.oldToolTip.ClearToolTip()
		self.newToolTip.ClearToolTip()

		item.SelectItem(metinIndex)

		## Metin Image
		try:
			self.metinImage.LoadImage(item.GetIconImageFileName())
		except:
			dbg.TraceError("AttachMetinDialog.Open.LoadImage - Failed to find item data")

		## Old Item ToolTip
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))
		self.oldToolTip.AddItemData(itemIndex, metinSlot)

		## New Item ToolTip
		item.SelectItem(metinIndex)
		metinSubType = item.GetItemSubType()

		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			slotData = metinSlot[i]
			if self.CanAttachMetin(slotData, metinSubType):
				metinSlot[i] = metinIndex
				break
		self.newToolTip.AddItemData(itemIndex, metinSlot)

		self.UpdateDialog()
		self.SetTop()
		self.Show()
		
		# Tooltip'leri pencere gösterildikten sonra göster ve üstte tut
		if hasattr(self, 'oldToolTip') and self.oldToolTip:
			self.oldToolTip.SetTop()
			self.oldToolTip.Show()
			self.oldToolTip.ShowToolTip()
		if hasattr(self, 'newToolTip') and self.newToolTip:
			self.newToolTip.SetTop()
			self.newToolTip.Show()
			self.newToolTip.ShowToolTip()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if hasattr(self, 'inven') and self.inven:
				self.SetCantMouseEventSlot(self.metinItemPos)
				self.SetCantMouseEventSlot(self.targetItemPos)

	def UpdateDialog(self):
		# Her iki tooltip'in geniþliðini kontrol et
		oldToolTipWidth = 0
		if hasattr(self, 'oldToolTip') and self.oldToolTip:
			oldToolTipWidth = self.oldToolTip.GetWidth()
		
		newToolTipWidth = 0
		if hasattr(self, 'newToolTip') and self.newToolTip:
			newToolTipWidth = self.newToolTip.GetWidth()
		
		# En geniþ tooltip'i baz al
		maxToolTipWidth = max(oldToolTipWidth, newToolTipWidth)
		
		# Yüksekliði de her iki tooltip'ten en yükseðini al
		oldToolTipHeight = 0
		if hasattr(self, 'oldToolTip') and self.oldToolTip:
			oldToolTipHeight = self.oldToolTip.GetHeight()
		
		newToolTipHeight = 0
		if hasattr(self, 'newToolTip') and self.newToolTip:
			newToolTipHeight = self.newToolTip.GetHeight()
		
		maxToolTipHeight = max(oldToolTipHeight, newToolTipHeight)
		
		# Panel geniþliði: sol tooltip (15) + boþluk (230) + sað tooltip (20) + margin (15)
		newWidth = 15 + maxToolTipWidth + 230 + 20 + 15
		newHeight = maxToolTipHeight + 98

		if localeInfo.IsARABIC():
			self.board.SetPosition( newWidth, 0 )

			(x,y) = self.titleBar.GetLocalPosition()
			self.titleBar.SetPosition( newWidth - 15, y )

		self.board.SetSize(newWidth, newHeight)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def Accept(self):
		net.SendItemUseToItemPacket(self.metinItemPos, self.targetItemPos)
		snd.PlaySound("sound/ui/metinstone_insert.wav")
		self.Close()

	def Close(self):
		if hasattr(self, 'oldToolTip') and self.oldToolTip:
			if hasattr(self.oldToolTip, 'HideToolTip'):
				self.oldToolTip.HideToolTip()
			elif hasattr(self.oldToolTip, 'Hide'):
				self.oldToolTip.Hide()
		if hasattr(self, 'newToolTip') and self.newToolTip:
			if hasattr(self.newToolTip, 'HideToolTip'):
				self.newToolTip.HideToolTip()
			elif hasattr(self.newToolTip, 'Hide'):
				self.newToolTip.Hide()
		self.Hide()
		if app.ENABLE_SLOT_MARKING_SYSTEM:
			if hasattr(self, 'inven') and self.inven:
				if hasattr(self, 'metinItemPos') and self.metinItemPos > 0:
					self.SetCanMouseEventSlot(self.metinItemPos)
				if hasattr(self, 'targetItemPos') and self.targetItemPos > 0:
					self.SetCanMouseEventSlot(self.targetItemPos)

	def OnUpdate(self):
		if self.IsShow():
			self.SetTop()
			
			# Tooltip'leri sürekli göster ve üstte tut (baþka item'e hover edilse bile kaybolmasýn)
			try:
				# Önce her iki tooltip'i de Show() ile göster
				if hasattr(self, 'oldToolTip') and self.oldToolTip:
					self.oldToolTip.SetTop()
					self.oldToolTip.Show()
				
				if hasattr(self, 'newToolTip') and self.newToolTip:
					self.newToolTip.SetTop()
					self.newToolTip.Show()
				
				# Sonra tooltip'leri tekrar göster (birbirlerini gizlemesinler diye sýrayla)
				if hasattr(self, 'oldToolTip') and self.oldToolTip:
					if hasattr(self.oldToolTip, 'IsShow'):
						if not self.oldToolTip.IsShow():
							self.oldToolTip.ShowToolTip()
					else:
						self.oldToolTip.ShowToolTip()
				
				if hasattr(self, 'newToolTip') and self.newToolTip:
					if hasattr(self.newToolTip, 'IsShow'):
						if not self.newToolTip.IsShow():
							self.newToolTip.ShowToolTip()
					else:
						self.newToolTip.ShowToolTip()
			except:
				pass

	def SetInven(self, inven):
		self.inven = inven

	def SetInventory(self, inven):
		self.inven = inven

	if app.ENABLE_SLOT_MARKING_SYSTEM:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

		def __GetLocalSlotIndex(self, idx):
			if not hasattr(self, 'inven') or not self.inven:
				return -1
			if idx >= INVENTORY_PAGE_SIZE:
				page = self.inven.GetInventoryPageIndex()
				idx -= (page * INVENTORY_PAGE_SIZE)
			return idx

		def SetCanMouseEventSlot(self, idx):
			if not hasattr(self, 'inven') or not self.inven:
				return
			localIdx = self.__GetLocalSlotIndex(idx)
			if localIdx >= 0:
				self.inven.wndItem.SetCanMouseEventSlot(localIdx)

		def SetCantMouseEventSlot(self, idx):
			if not hasattr(self, 'inven') or not self.inven:
				return
			localIdx = self.__GetLocalSlotIndex(idx)
			if localIdx >= 0:
				self.inven.wndItem.SetCantMouseEventSlot(localIdx)
