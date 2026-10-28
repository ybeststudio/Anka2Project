if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import wndMgr
import localeInfo
import item
import chat
import uiCommon
import exception
import uiScriptLocale
import constInfo

class NameReinforceWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.interface = None
		self.questionDialog = 0
		self.whichWindow = 0

		self.__LoadWindow()
	
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)		
		self.SetTop()
		
		
	def Destory(self):
		self.isLoaded = 0
		self.questionDialog = 0
		self.whichWindow = 0
		self.interface = None

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			self.__LoadScript("UIScript/petHatchingWindow.py")
				
		except:
			import exception
			exception.Abort("petHatchingWindow.LoadWindow.LoadObject")

		try:
			self.GetChild("PetHatching_TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.hatchingSlot = self.GetChild("HatchingItemSlot")
			self.hatchingSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			
			self.hatchingButton = self.GetChild("HatchingButton")
			self.hatchingButton.SetEvent(ui.__mem_func__(self.ClickHatchingButton))
			self.hatchingButton.SetText("Set")

			self.hatchingMoneyText = self.GetChild("HatchingMoney")
			self.hatchingMoneyText.SetText(uiScriptLocale.NAME_REINFORCE_WARNING)
			
			self.petNameEdit = self.GetChild("pet_name")
			self.petNameEdit.SetText("")
			self.petNameEdit.SetReturnEvent(ui.__mem_func__(self.ClickHatchingButton))
			self.petNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))

			self.__MakeQuestionDialog()
		except:
			import exception
			exception.Abort("petHatchingWindow.LoadWindow.BindObject")			
			
		self.hatchingSlot.RefreshSlot()

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)
		
	def __MakeQuestionDialog(self):
		if not self.questionDialog:
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(uiScriptLocale.NAME_REINFORCE_TITLE)
			
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__HatchingQuestionDialogAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__HatchingQuestionDialogCancel))

	def __OpenHatchingQuestionDialog(self):
		if not self.questionDialog:
			self.__MakeQuestionDialog()

		if "" == self.petNameEdit.GetText():
			self.petNameEdit.SetText( self.petName )
			self.petNameEdit.SetEndPosition()
			
		self.questionDialog.SetText(uiScriptLocale.NAME_REINFORCE_1.format(self.petNameEdit.GetText()))
		self.questionDialog.SetTop()
		self.questionDialog.Open()

	def __HatchingQuestionDialogAccept(self):
		self.questionDialog.Close()
		
		if len( self.petNameEdit.GetText() ) < 4:
			self.petNameEdit.SetText("")
			tchat(uiScriptLocale.NAME_REINFORCE_MORE)
			return
			
		if self.whichWindow == 2:
			net.SendChatPacket("/mount_name {}".format(self.petNameEdit.GetText()))
		else:
			net.SendChatPacket("/pet_name {}".format(self.petNameEdit.GetText()))
			
		self.Close()

	def __HatchingQuestionDialogCancel(self):
		self.questionDialog.Close()		

	def OpenHatchWindow(self, arg):
		self.petNameEdit.SetEndPosition()
		self.petNameEdit.SetFocus()
		self.petNameEdit.Show()
		self.whichWindow = arg
		
		if self.whichWindow == 1:
			self.hatchingSlot.SetItemSlot(0, 79714, 0)
		else:
			self.hatchingSlot.SetItemSlot(0, 79715, 0)

		self.Show()

	def BindInterface(self, interface):	
		self.interface = interface

	def ClickHatchingButton(self):				
		self.__OpenHatchingQuestionDialog()

	def Close(self):
		self.petName = 0

		if self.questionDialog:
			self.questionDialog.Close()		
			
		self.Hide()
		if self.petNameEdit:
			self.petNameEdit.KillFocus()
			
