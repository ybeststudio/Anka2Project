if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))

import ui

class EquipmentDialog(ui.ScriptWindow):

	def __init__(self):
		print "NEW EQUIPMENT DIALOG ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()

		self.vid = None
		self.eventClose = None
		self.itemDataDict = {}
		if app.ENABLE_SUNGMA_SCORE:
			self.sungmaScoreDict = {}
		self.tooltipItem = None

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE EQUIPMENT DIALOG "
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/EquipmentDialog.py")

			getObject = self.GetChild
			self.board = getObject("Board")
			self.slotWindow = getObject("EquipmentSlot")

		except:
			import exception
			exception.Abort("EquipmentDialog.LoadDialog.BindObject")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.slotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.slotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

	def Open(self, vid):

		self.vid = vid
		self.itemDataDict = {}
		if app.ENABLE_SUNGMA_SCORE:
			self.sungmaScoreDict = {}

		name = chr.GetNameByVID(vid)
		self.board.SetTitleName(name)

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.itemDataDict = {}
		if app.ENABLE_SUNGMA_SCORE:
			self.sungmaScoreDict = {}
		self.tooltipItem = None
		self.Hide()

		if self.eventClose:
			self.eventClose(self.vid)

	def Destroy(self):
		self.eventClose = None

		self.Close()
		self.ClearDictionary()

		self.board = None
		self.slotWindow = None

	if app.ENABLE_CHANGE_LOOK_SYSTEM:
		def SetEquipmentDialogItem(self, slotIndex, vnum, count, dwChangeLookVnum):
			if count <= 1:
				count = 0
			self.slotWindow.SetItemSlot(slotIndex, vnum, count)

			emptySocketList = []
			emptyAttrList = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				emptySocketList.append(0)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				emptyAttrList.append((0, 0))

			self.itemDataDict[slotIndex] = (vnum, count, emptySocketList, emptyAttrList, dwChangeLookVnum)

			if not dwChangeLookVnum == 0:
				self.slotWindow.SetSlotCoverImage(slotIndex, "icon/item/ingame_convert_Mark.tga")
			else:
				self.slotWindow.EnableSlotCoverImage(slotIndex, FALSE)
	else:
		def SetEquipmentDialogItem(self, slotIndex, vnum, count):
			if count <= 1:
				count = 0
			self.slotWindow.SetItemSlot(slotIndex, vnum, count)

			emptySocketList = []
			emptyAttrList = []
			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				emptySocketList.append(0)
			for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
				emptyAttrList.append((0, 0))

			self.itemDataDict[slotIndex] = (vnum, count, emptySocketList, emptyAttrList)

	def SetEquipmentDialogSocket(self, slotIndex, socketIndex, value):
		if not slotIndex in self.itemDataDict:
			return
		if socketIndex < 0 or socketIndex > player.METIN_SOCKET_MAX_NUM:
			return
		self.itemDataDict[slotIndex][2][socketIndex] = value

	def SetEquipmentDialogAttr(self, slotIndex, attrIndex, type, value):
		if not slotIndex in self.itemDataDict:
			return
		if attrIndex < 0 or attrIndex > player.ATTRIBUTE_SLOT_MAX_NUM:
			return
		self.itemDataDict[slotIndex][3][attrIndex] = (type, value)

	if app.ENABLE_SUNGMA_SCORE:
		def SetEquipmentDialogSungmaScore(self, slotIndex, value):
			if not slotIndex in self.itemDataDict:
				return
			self.sungmaScoreDict[slotIndex] = value

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetCloseEvent(self, event):
		self.eventClose = event

	def OverInItem(self, slotIndex):

		if None == self.tooltipItem:
			return

		if not slotIndex in self.itemDataDict:
			return

		itemVnum = self.itemDataDict[slotIndex][0]
		if 0 == itemVnum:
			return

		self.tooltipItem.ClearToolTip()
		metinSlot = self.itemDataDict[slotIndex][2]
		attrSlot = self.itemDataDict[slotIndex][3]
		self.tooltipItem.AddItemData(itemVnum, metinSlot, attrSlot)
		if app.ENABLE_CHANGE_LOOK_SYSTEM:
			self.tooltipItem.AppendChangeLookInfoItemVnum(self.itemDataDict[slotIndex][4])
		self.tooltipItem.ShowToolTip()

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
