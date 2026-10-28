if __USE_DYNAMIC_MODULE__:
	import pyapi

	app = __import__(pyapi.GetModuleName("app"))
	player = __import__(pyapi.GetModuleName("player"))
else:
	import app
	import player

import ui
import item
import localeInfo
import bonus_settings
import wndMgr
from uitooltip import ItemToolTip

FILE_NAME_LEN = 28
DEFAULT_THEMA = localeInfo.MUSIC_METIN2_DEFAULT_THEMA

class Item(ui.ListBoxEx.Item):

	def __init__(self, fileName, id):
		ui.ListBoxEx.Item.__init__(self)
		self.canLoad = 0
		self.text = fileName
		self.id = id
		self.textLine = self.__CreateTextLine(fileName[:FILE_NAME_LEN])

	def __del__(self):
		ui.ListBoxEx.Item.__del__(self)

	def GetText(self):
		return self.text

	def GetID(self):
		return self.id

	def SetSize(self, width, height):
		ui.ListBoxEx.Item.SetSize(self, 6 * len(self.textLine.GetText()) + 4, height)

	def __CreateTextLine(self, fileName):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetPosition(0, 0)
		textLine.SetText(fileName)
		textLine.Show()
		return textLine

class PopupDialog(ui.ScriptWindow):

	def __init__(self, parent):
		print "NEW POPUP WINDOW   ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.__Load()
		self.__Bind()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print "---------------------------------------------------------------------------- DELETE POPUP WINDOW"

	def __Load(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")
		except:
			import exception
			exception.Abort("PopupDialog.__Load")

	def __Bind(self):
		try:
			self.textLine = self.GetChild("message")
			self.okButton = self.GetChild("accept")
		except:
			import exception
			exception.Abort("PopupDialog.__Bind")

		self.okButton.SAFE_SetEvent(self.__OnOK)

	def Open(self, msg):
		self.textLine.SetText(msg)
		self.SetCenterPosition()
		self.Show()
		self.SetTop()

	def __OnOK(self):
		self.Hide()


class AttachBonusList(ui.ScriptWindow):

	def __init__(self):
		print "NEW LIST DIALOG   ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.curSlotID = 0
		self.selectEvent = None
		self.fileListBox = None
		return

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print "---------------------------------------------------------------------------- DELETE LIST DIALOG"

	def Show(self, curItemPos):
		self.__Load(curItemPos)
		ui.ScriptWindow.Show(self)

	def Open(self, curSlotID, itemPos):
		self.Show(itemPos)
		self.SetTop()
		self.curSlotID = curSlotID
		self.__RefreshFileList(itemPos)

	def __Close(self):
		self.popupDialog.Hide()
		self.Hide()

	def OnPressEscapeKey(self):
		self.__Close()
		return True

	def SAFE_SetSelectEvent(self, event):
		self.selectEvent = ui.__mem_func__(event)

	def __CreateFileListBox(self):
		fileListBox = ui.ListBoxEx()
		fileListBox.SetParent(self)
		fileListBox.SetPosition(15, 50)
		fileListBox.Show()
		return fileListBox

	def __Load(self, curItemPos):
		self.popupDialog = PopupDialog(self)
		self.__Load_LoadScript("UIScript/SelectBonusListWindow.py")
		self.__Load_BindObject()
		self.cancelButton.SAFE_SetEvent(self.__Close)
		self.okButton.SAFE_SetEvent(self.__OnOK)
		self.board.SetCloseEvent(ui.__mem_func__(self.__Close))
		self.UpdateRect()
		self.__RefreshFileList(curItemPos)

	def __Load_LoadScript(self, fileName):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("MusicListBox.__Load")

	def __Load_BindObject(self):
		try:
			self.fileListBox = self.__CreateFileListBox()
			self.fileListBox.SetScrollBar(self.GetChild("ScrollBar"))
			self.board = self.GetChild("board")
			self.okButton = self.GetChild("ok")
			self.cancelButton = self.GetChild("cancel")
			self.popupText = self.popupDialog.GetChild("message")
		except:
			import exception
			exception.Abort("MusicListBox.__Bind")

	def __PopupMessage(self, msg):
		self.popupDialog.Open(msg)

	def __OnOK(self):
		selItem = self.fileListBox.GetSelectedItem()
		if selItem:
			if self.selectEvent:
				self.selectEvent(selItem.GetID(), selItem.GetText(), self.curSlotID)
			self.Hide()
		else:
			self.__PopupMessage("Efsun se\xe7medin.")

	def __RefreshFileList(self, curItemPos):
		self.__ClearFileList()
		self.__AppendFileList(curItemPos)

	def __ClearFileList(self):
		self.fileListBox.RemoveAllItems()

	def __AppendFileList(self, curItemPos):
		itemIndex = player.GetItemIndex(curItemPos)
		item.SelectItem(itemIndex)
		affect_names = bonus_settings.bonusNameList
		avail = bonus_settings.GetFitBonusList(item.GetItemType(), item.GetItemSubType())
		for x in avail:
			if x in affect_names:
				self.__AppendFile(affect_names[x], x)

	def __AppendFile(self, fileName, x):
		self.fileListBox.AppendItem(Item(fileName, x))
