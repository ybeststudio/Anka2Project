if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import mouseModule
import snd
import uiScriptLocale
import uiCommon
import uiPrivateShopBuilder
import localeInfo
import constInfo
import time
import wndMgr
import interfaceModule
import dbg
import uiToolTip
import grp
import translate
import chat


class SpeedButtonWindow(ui.ScriptWindow):
	Button1_Timer = 0
	Button2_Timer = 0
	Button3_Timer = 0
	Button4_Timer = 0
	Button5_Timer = 0
	Button6_Timer = 0
	Button7_Timer = 0
	Button8_Timer = 0
	Button9_Timer = 0
	Button10_Timer = 0
	Button11_Timer = 0
	Button12_Timer = 0
	Button13_Timer = 0
	Button14_Timer = 0
	Button15_Timer = 0
	Button16_Timer = 0
	Button17_Timer = 0
	Button18_Timer = 0
	Button19_Timer = 0
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()
#		self.average_price = uiaverage_price.AveragePrice()
#		self.average_price.Hide()
		tooltip = uiToolTip.ToolTip()
		self.toolTip = tooltip
		# Interface instance'ý al (constInfo'dan)
		try:
			self.interface = constInfo.GetInterfaceInstance()
		except:
			self.interface = None
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print " -------------------------------------- DELETE GAME OPTION DIALOG"

	def __Initialize(self):
		self.titleBar = 0

	def Destroy(self):
		self.ClearDictionary()

		self.__Initialize()
		print " -------------------------------------- DESTROY GAME OPTION DIALOG"
	
	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			self.titleBar = self.GetChild("board")

			self.Button1 = self.GetChild("Button1")
			self.Button2 = self.GetChild("Button2")
			self.Button3 = self.GetChild("Button3")
			self.Button4 = self.GetChild("Button4")
			self.Button5 = self.GetChild("Button5")
			self.Button6 = self.GetChild("Button6")
			self.Button7 = self.GetChild("Button7")
			self.Button8 = self.GetChild("Button8")
			self.Button9 = self.GetChild("Button9")
			self.Button10 = self.GetChild("Button10")
			self.Button11 = self.GetChild("Button11")
			self.Button12 = self.GetChild("Button12")
			self.Button13 = self.GetChild("Button13")
			self.Button14 = self.GetChild("Button14")
			self.Button15 = self.GetChild("Button15")
			self.Button16 = self.GetChild("Button16")
			self.Button17 = self.GetChild("Button17")
			self.Button18 = self.GetChild("Button18")
			self.Button19 = self.GetChild("Button19")

			self.Button1.SAFE_SetEvent(self.ButtonEvent,1)
			self.Button2.SAFE_SetEvent(self.ButtonEvent,2)
			self.Button3.SAFE_SetEvent(self.ButtonEvent,3)
			self.Button4.SAFE_SetEvent(self.ButtonEvent,4)
			self.Button5.SAFE_SetEvent(self.ButtonEvent,5)
			self.Button6.SAFE_SetEvent(self.ButtonEvent,6)
			self.Button7.SAFE_SetEvent(self.ButtonEvent,7)
			self.Button8.SAFE_SetEvent(self.ButtonEvent,8)
			self.Button9.SAFE_SetEvent(self.ButtonEvent,9)
			self.Button10.SAFE_SetEvent(self.ButtonEvent,10)
			self.Button11.SAFE_SetEvent(self.ButtonEvent,11)
			self.Button12.SAFE_SetEvent(self.ButtonEvent,12)
			self.Button13.SAFE_SetEvent(self.ButtonEvent,13)
			self.Button14.SAFE_SetEvent(self.ButtonEvent,14)
			self.Button15.SAFE_SetEvent(self.ButtonEvent,15)
			self.Button16.SAFE_SetEvent(self.ButtonEvent,16)
			self.Button17.SAFE_SetEvent(self.ButtonEvent,17)
			self.Button18.SAFE_SetEvent(self.ButtonEvent,18)
			self.Button19.SAFE_SetEvent(self.ButtonEvent,19)

		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

			
	def __ShowMenuToolTip(self, statDesc):
		self.toolTip.ClearToolTip()
		self.toolTip.AppendTextLine(statDesc)
		self.toolTip.Show()

	def __OverInButtonMenu(self, num):	
		try:
			self.__ShowMenuToolTip(translate.SystemMenuText[num])
		except KeyError:
			pass
			
	def __OverOutButtonMenu(self):
		self.toolTip.Hide()
		
	def ButtonEvent(self,arg):
		if int(arg) == 1:
			if app.GetTime() > self.Button1_Timer:
				if app.ENABLE_RENEWAL_SWITCHBOT and self.interface:
					self.interface.ToggleSwitchbotWindow()
				self.Button1_Timer = app.GetTime() + 2
			else:
				Button1_TimerInfo = self.Button1_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button1_TimerInfo))
		elif int(arg) == 2:
			if app.GetTime() > self.Button2_Timer:
				if self.interface and hasattr(self.interface, 'wndMiniMap'):
					self.interface.wndMiniMap.OpenBattlePassMenu()
				self.Button2_Timer = app.GetTime() + 2
			else:
				Button2_TimerInfo = self.Button2_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button2_TimerInfo))
		elif int(arg) == 3:
			if app.GetTime() > self.Button3_Timer:
				if app.ENABLE_TRACK_WINDOW and self.interface:
					self.interface.OpenDungeonInfo()
				self.Button3_Timer = app.GetTime() + 2
			else:
				Button3_TimerInfo = self.Button3_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button3_TimerInfo))
		elif int(arg) == 4:
			if app.GetTime() > self.Button4_Timer:
				if app.ENABLE_RENEWAL_TELEPORT_SYSTEM and self.interface:
					self.interface.OpenWarpWindow()
				self.Button4_Timer = app.GetTime() + 2
			else:
				Button4_TimerInfo = self.Button4_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button4_TimerInfo))
		elif int(arg) == 5:
			if app.GetTime() > self.Button5_Timer:
				if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM and self.interface:
					self.interface.OpenPrivateShopSearch(0)
				self.Button5_Timer = app.GetTime() + 2
			else:
				Button5_TimerInfo = self.Button5_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button5_TimerInfo))
		elif int(arg) == 6:
			if app.GetTime() > self.Button6_Timer:
				if app.ENABLE_COLLECTIONS_SYSTEM and self.interface:
					self.interface.ToggleCollectionsWindow()
				self.Button6_Timer = app.GetTime() + 2
			else:
				Button6_TimerInfo = self.Button6_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button6_TimerInfo))
		elif int(arg) == 7:
			if app.GetTime() > self.Button7_Timer:
				if app.ENABLE_RESP_SYSTEM and self.interface:
					self.interface.OpenRespWindow()
				self.Button7_Timer = app.GetTime() + 2
			else:
				Button7_TimerInfo = self.Button7_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button7_TimerInfo))
		elif int(arg) == 8:
			if app.GetTime() > self.Button8_Timer:
				if app.ENABLE_COLLECT_WINDOW and self.interface:
					self.interface.ToggleCollectWindow()
				self.Button8_Timer = app.GetTime() + 2
			else:
				Button8_TimerInfo = self.Button8_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button8_TimerInfo))
		elif int(arg) == 9:
			if app.GetTime() > self.Button9_Timer:
				if app.ENABLE_SPECIAL_INVENTORY and self.interface:
					self.interface.ToggleSpecialInventoryWindow()
				self.Button9_Timer = app.GetTime() + 2
			else:
				Button9_TimerInfo = self.Button9_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button9_TimerInfo))
		elif int(arg) == 10:
			if app.GetTime() > self.Button10_Timer:
				if app.__AUTO_HUNT__ and self.interface:
					self.interface.OpenAutoHunt()
				self.Button10_Timer = app.GetTime() + 2
			else:
				Button10_TimerInfo = self.Button10_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button10_TimerInfo))
		elif int(arg) == 11:
			if app.GetTime() > self.Button11_Timer:
				if app.ENABLE_AUTO_SELL_SYSTEM and self.interface:
					self.interface.ToggleAutoSellWindow()
				self.Button11_Timer = app.GetTime() + 2
			else:
				Button11_TimerInfo = self.Button11_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button11_TimerInfo))
		elif int(arg) == 12:
			if app.GetTime() > self.Button12_Timer:
				if app.ENABLE_GROWTH_PET_SYSTEM and self.interface:
					self.interface.TogglePetInformationWindow()
				self.Button12_Timer = app.GetTime() + 2
			else:
				Button12_TimerInfo = self.Button12_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button12_TimerInfo))
		elif int(arg) == 13:
			if app.GetTime() > self.Button13_Timer:
				if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM and self.interface:
					self.interface.OpenPrivateShopSearch(0)
				self.Button13_Timer = app.GetTime() + 2
			else:
				Button13_TimerInfo = self.Button13_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button13_TimerInfo))
		elif int(arg) == 14:
			if app.GetTime() > self.Button14_Timer:
				if self.interface:
					if hasattr(self.interface, 'OpenRanking'):
						self.interface.OpenRanking()
				self.Button14_Timer = app.GetTime() + 2
			else:
				Button14_TimerInfo = self.Button14_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button14_TimerInfo))
		elif int(arg) == 15:
			if app.GetTime() > self.Button15_Timer:
				if app.ENABLE_RENEWAL_OFFLINESHOP and self.interface:
					if hasattr(self.interface, 'ToggleOfflineShopWindow'):
						self.interface.ToggleOfflineShopWindow()
				self.Button15_Timer = app.GetTime() + 2
			else:
				Button15_TimerInfo = self.Button15_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button15_TimerInfo))
		elif int(arg) == 16:
			if app.GetTime() > self.Button16_Timer:
				if self.interface and self.interface.wndInventory:
					if hasattr(self.interface.wndInventory, 'TogglePotionsWindow'):
						self.interface.wndInventory.TogglePotionsWindow()
				self.Button16_Timer = app.GetTime() + 2
			else:
				Button16_TimerInfo = self.Button16_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button16_TimerInfo))
		elif int(arg) == 17:
			if app.GetTime() > self.Button17_Timer:
				if app.ENABLE_WIKI_SYSTEM and self.interface:
					if hasattr(self.interface, 'OpenWikiWindow'):
						self.interface.OpenWikiWindow()
				self.Button17_Timer = app.GetTime() + 2
			else:
				Button17_TimerInfo = self.Button17_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button17_TimerInfo))
		elif int(arg) == 18:
			if app.GetTime() > self.Button18_Timer:
				if app.ENABLE_HUNTING_SYSTEM and self.interface:
					if hasattr(self.interface, 'ToggleHuntingWindow'):
						self.interface.ToggleHuntingWindow()
				self.Button18_Timer = app.GetTime() + 2
			else:
				Button18_TimerInfo = self.Button18_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button18_TimerInfo))
		elif int(arg) == 19:
			if app.GetTime() > self.Button19_Timer:
				if app.ENABLE_BIOLOGIST_SYSTEM and self.interface:
					if hasattr(self.interface, 'OpenBiyologDialog'):
						self.interface.OpenBiyologDialog()
				self.Button19_Timer = app.GetTime() + 2
			else:
				Button19_TimerInfo = self.Button19_Timer - app.GetTime()
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.INFORMATION_TIME_CLICK % (Button19_TimerInfo))

		self.Close()

	def __Load(self):
		self.__Load_LoadScript("uiscript/speedbutton.py")

		self.__Load_BindObject()

		self.SetCenterPosition()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		
	def OnUpdate(self):
		pass

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

