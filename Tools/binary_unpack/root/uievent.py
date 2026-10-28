if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))
chr = __import__(pyapi.GetModuleName("chr"))

import gameEvents
import chat
import localeInfo
import ui
import uiScriptLocale
import uiCommon
import item
import time
import constInfo
if app.ENABLE_FISH_EVENT_SYSTEM:
	import uiMiniGameFishEvent
if app.ENABLE_ATTENDANCE_EVENT:
	import uiMiniGameAttendance
if app.ENABLE_MINI_GAME_CATCH_KING:
	import uiMiniGameCatchKing

button_gap		= 10
button_height	= 42

class EventWindow(ui.ScriptWindow):

	def __init__(self, interface):
		ui.ScriptWindow.__init__(self)
		self.interface = interface
		if app.ENABLE_FISH_EVENT_SYSTEM:
			self.fishGame = None
			self.wndInventory = None
		if app.ENABLE_ATTENDANCE_EVENT:
			self.attendanceGame = None
		if app.ENABLE_MINI_GAME_CATCH_KING:
			self.catch_king_game	= None

		self.tooltipItem = None
		self.lastRefreshTime = 0
		self.refreshInterval = 5  # 5 saniyede bir yenile

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def BindInventory(self, inventory):
		self.wndInventory = inventory
			
	def SetItemToolTip(self, toolTip):
		self.tooltipItem = toolTip
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/eventwindow.py")
			self.gameButton = self.GetChild("game_event_button")
		except:
			import exception
			exception.Abort("eventwindow.LoadDialog.LoadScript")

		try:
			self.eventinfoDialog = EventInfoDialog()
			self.eventinfoDialog.Hide()
		except:
			import exception
			exception.Abort("eventwindow.LoadDialog.BindObject")
			
		self.gameButton.SetEvent(ui.__mem_func__(self.OpenEventInfoDialog))
			
	def OpenEventInfoDialog(self):
		self.eventinfoDialog.Show()

	def __OpenEventItemInformationFootball(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
		informationDialog.height = 32
		informationDialog.Open(50096, uiScriptLocale.EVENT_FOOTBALL_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, uiScriptLocale.GAME_EVENTS_INFORMATION_12)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationAyisigi(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
		informationDialog.height = 32
		informationDialog.Open(50011, uiScriptLocale.EVENT_AYISIGI_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, "")
		self.informationDialog = informationDialog

	def __OpenEventItemInformationOkeyCard(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		if self.interface:
			self.interface.OpenCardsInfoWindow()

	def __OpenEventItemInformationAttendance(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		if self.interface:
			self.interface.OpenCardsInfoWindow()

	def __OpenEventItemInformationCatchKing(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		if self.interface:
			self.interface.OpenCardsInfoWindow()

	def __OpenEventItemInformationWord(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
		informationDialog.height = 32
		informationDialog.Open(50128, uiScriptLocale.EVENT_WORD_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, uiScriptLocale.GAME_EVENTS_INFORMATION_4)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationHollowen(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		if constInfo.IsHaloun == 1:
			if self.interface:
				self.interface.ShowHaloun()
		else:
			informationDialog = uiCommon.EventInformationDialog()
			informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
			informationDialog.height = 32
			informationDialog.Open(70604, uiScriptLocale.EVENT_HALLOWEEN_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_19, uiScriptLocale.GAME_EVENTS_INFORMATION_20)
			self.informationDialog = informationDialog

	def __OpenEventItemInformationRituelSoul(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
		informationDialog.height = 32
		informationDialog.Open(30370, uiScriptLocale.EVENT_RITUELSOUL_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, uiScriptLocale.GAME_EVENTS_INFORMATION_5)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationEaster(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_15)
		informationDialog.height = 32
		informationDialog.Open(71150, uiScriptLocale.EVENT_EASTER_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, uiScriptLocale.GAME_EVENTS_INFORMATION_6)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationStoneKill(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_14)
		informationDialog.height = 32
		informationDialog.Open(32142, uiScriptLocale.EVENT_STONEKILL_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_13, uiScriptLocale.GAME_EVENTS_INFORMATION_7)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationXmas(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_1)
		informationDialog.height = 32
		informationDialog.Open(50010, uiScriptLocale.EVENT_XMAS_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_2, uiScriptLocale.GAME_EVENTS_INFORMATION_8)
		self.informationDialog = informationDialog


	def __OpenEventItemInformationRamadan(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_9)
		informationDialog.height = 32
		informationDialog.Open(30318, uiScriptLocale.EVENT_RAMADAN_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_10, uiScriptLocale.GAME_EVENTS_INFORMATION_11)
		self.informationDialog = informationDialog

	def __OpenEventItemInformationChequeDesk(self):
		if constInfo.IS_OPEN_EVENT_INFORMATION == 1:
			return
		informationDialog = uiCommon.EventInformationDialog()
		informationDialog.SetText(uiScriptLocale.GAME_EVENTS_INFORMATION_16)
		informationDialog.height = 32
		informationDialog.Open(80020, uiScriptLocale.EVENT_CHEQUEDESK_TEXT, uiScriptLocale.GAME_EVENTS_INFORMATION_17, uiScriptLocale.GAME_EVENTS_INFORMATION_18)
		self.informationDialog = informationDialog

	def Refresh(self):
		activate_event_count = 0

		if self.eventinfoDialog == None:
			return

		self.eventinfoDialog.DeleteAllButton()
		
		currentTime = app.GetGlobalTimeStamp()
		
		for i in xrange(gameEvents.EVENT_MAX_NUM):
			isActivate = gameEvents.IsActivateEvent(i)
			if isActivate == True:
				eventTime = gameEvents.GetEventTime(i)
				
				# Etkinlik aktifse göster
				# Eðer eventTime > 0 ise zaman kontrolü yap (eventTime gelecekteki bitiþ zamanýdýr)
				# Eðer eventTime == 0 ise süresiz etkinlik olarak kabul et veya zaman henüz set edilmemiþtir
				# Zaman kontrolü: eventTime == 0 (henüz set edilmemiþ veya süresiz) veya eventTime > currentTime (henüz dolmamýþ)
				if eventTime == 0 or eventTime > currentTime:
					activate_event_count += 1
					if gameEvents.EVENT_FOOTBALL == i:
						self.eventinfoDialog.AppendButton(50096, 0, 0, uiScriptLocale.EVENT_FOOTBALL_TEXT, uiScriptLocale.EVENT_FOOTBALL_TEXT, gameEvents.GetEventTime(0), self.__OpenEventItemInformationFootball)
					elif gameEvents.EVENT_AYISIGI == i:
						self.eventinfoDialog.AppendButton(50011, 0, 0, uiScriptLocale.EVENT_AYISIGI_TEXT, uiScriptLocale.EVENT_AYISIGI_TEXT, gameEvents.GetEventTime(1), self.__OpenEventItemInformationAyisigi)
					elif gameEvents.EVENT_OKEYCARD == i:
						self.eventinfoDialog.AppendButton(71196, 71195, 71194, uiScriptLocale.EVENT_OKEYCARD_TEXT, uiScriptLocale.EVENT_OKEYCARD_TEXT, gameEvents.GetEventTime(2), self.__OpenEventItemInformationOkeyCard)
					elif gameEvents.EVENT_FISH == i:
						self.eventinfoDialog.AppendButton(83001, 83002, 83003, uiScriptLocale.EVENT_FISH_TEXT, uiScriptLocale.EVENT_FISH_TEXT, gameEvents.GetEventTime(3), self.__ClickFishEventButton)
					elif gameEvents.EVENT_ATTENDANCE == i:
						self.eventinfoDialog.AppendButton(50274, 50273, 50272, uiScriptLocale.EVENT_ATTENDANCE_TEXT, uiScriptLocale.EVENT_ATTENDANCE_TEXT, gameEvents.GetEventTime(4), self.__ClickAttendanceButton)
					elif gameEvents.EVENT_CATCHKING == i:
						self.eventinfoDialog.AppendButton(50930, 50929, 50928, uiScriptLocale.EVENT_CATCHKING_TEXT, uiScriptLocale.EVENT_CATCHKING_TEXT, gameEvents.GetEventTime(5), self.__ClickCatchKingButton)
					elif gameEvents.EVENT_WORD == i:
						self.eventinfoDialog.AppendButton(50128, 0, 0, uiScriptLocale.EVENT_WORD_TEXT, uiScriptLocale.EVENT_WORD_TEXT, gameEvents.GetEventTime(6), self.__OpenEventItemInformationWord)
					elif gameEvents.EVENT_HALLOWEEN == i:
						self.eventinfoDialog.AppendButton(50215, 0, 0, uiScriptLocale.EVENT_HALLOWEEN_TEXT, uiScriptLocale.EVENT_HALLOWEEN_TEXT, gameEvents.GetEventTime(7), self.__OpenEventItemInformationHollowen)
					elif gameEvents.EVENT_RITUELSOUL == i:
						self.eventinfoDialog.AppendButton(30370, 0, 0, uiScriptLocale.EVENT_RITUELSOUL_TEXT, uiScriptLocale.EVENT_RITUELSOUL_TEXT, gameEvents.GetEventTime(8), self.__OpenEventItemInformationRituelSoul)
					elif gameEvents.EVENT_EASTER == i:
						self.eventinfoDialog.AppendButton(71150, 0, 0, uiScriptLocale.EVENT_EASTER_TEXT, uiScriptLocale.EVENT_EASTER_TEXT, gameEvents.GetEventTime(9), self.__OpenEventItemInformationEaster)
					elif gameEvents.EVENT_STONEKILL == i:
						self.eventinfoDialog.AppendButton(32142, 0, 0, uiScriptLocale.EVENT_STONEKILL_TEXT, uiScriptLocale.EVENT_STONEKILL_TEXT, gameEvents.GetEventTime(10), self.__OpenEventItemInformationStoneKill)
					elif gameEvents.EVENT_XMAS == i:
						self.eventinfoDialog.AppendButton(50010, 0, 0, uiScriptLocale.EVENT_XMAS_TEXT, uiScriptLocale.EVENT_XMAS_TEXT, gameEvents.GetEventTime(11), self.__OpenEventItemInformationXmas)
					elif gameEvents.EVENT_RAMADAN == i:
						self.eventinfoDialog.AppendButton(30318, 0, 0, uiScriptLocale.EVENT_RAMADAN_TEXT, uiScriptLocale.EVENT_RAMADAN_TEXT, gameEvents.GetEventTime(12), self.__OpenEventItemInformationRamadan)
					elif gameEvents.EVENT_CHEQUEDESK == i:
						self.eventinfoDialog.AppendButton(80020, 0, 0, uiScriptLocale.EVENT_CHEQUEDESK_TEXT, uiScriptLocale.EVENT_CHEQUEDESK_TEXT, gameEvents.GetEventTime(13), self.__OpenEventItemInformationChequeDesk)
			else:
				self.eventinfoDialog.DeleteButton(i)

		if activate_event_count > 0:
			self.Show()
		else:
			self.Hide()

		self.eventinfoDialog.RefreshDialog()
		self.lastRefreshTime = app.GetGlobalTimeStamp()
		
	def OnUpdate(self):
		# Periyodik olarak etkinlik durumunu kontrol et ve zamaný dolmuþ etkinlikleri kaldýr
		# Sadece zamaný dolmuþ etkinlikleri kontrol et, etkinlik açýldýðýnda Refresh() zaten çaðrýlýyor
		currentTime = app.GetGlobalTimeStamp()
		if self.lastRefreshTime > 0 and currentTime - self.lastRefreshTime >= self.refreshInterval:
			# Zamaný dolmuþ etkinlikleri kontrol et
			hasExpiredEvent = False
			for i in xrange(gameEvents.EVENT_MAX_NUM):
				isActivate = gameEvents.IsActivateEvent(i)
				if isActivate == True:
					eventTime = gameEvents.GetEventTime(i)
					# Eðer eventTime > 0 ise ve zamaný dolmuþsa Refresh() çaðýr
					if eventTime > 0 and eventTime <= currentTime:
						hasExpiredEvent = True
						break
			
			# Eðer zamaný dolmuþ etkinlik varsa Refresh() çaðýr
			if hasExpiredEvent:
				self.Refresh()
		
	def IntegrationMiniGame(self, enable):
		if app.ENABLE_FISH_EVENT_SYSTEM:
			if self.fishGame:
				self.fishGame.Destroy()
				self.fishGame = None

		if app.ENABLE_FISH_EVENT_SYSTEM:
			if not self.fishGame:
				self.fishGame = uiMiniGameFishEvent.MiniGameFish()
				
				if self.tooltipItem:
					self.fishGame.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ATTENDANCE_EVENT:
			if self.attendanceGame:
				self.attendanceGame.Destroy()
				self.attendanceGame = None

		if app.ENABLE_ATTENDANCE_EVENT:
			if not self.attendanceGame:
				self.attendanceGame = uiMiniGameAttendance.MiniGameAttendance()
				
				if self.tooltipItem:
					self.attendanceGame.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_MINI_GAME_CATCH_KING:
			if self.catch_king_game:
				self.catch_king_game.Destroy()
				self.catch_king_game = None

		if app.ENABLE_MINI_GAME_CATCH_KING:
			if not self.catch_king_game:
				self.catch_king_game = uiMiniGameCatchKing.MiniGameCatchKing()
				
				if self.tooltipItem:
					self.catch_king_game.SetItemToolTip(self.tooltipItem)

		self.Refresh()

	if app.ENABLE_MINI_GAME_CATCH_KING:
		def __ClickCatchKingButton(self):
			if self.catch_king_game:
				self.catch_king_game.Open()

		def MiniGameCatchKingEventStart(self, bigScore):
			if self.catch_king_game:
				self.catch_king_game.GameStart(bigScore)

		def MiniGameCatchKingSetHandCard(self, cardNumber):
			if self.catch_king_game:
				self.catch_king_game.CatchKingSetHandCard(cardNumber)

		def MiniGameCatchKingResultField(self, score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear):
			if self.catch_king_game:
				self.catch_king_game.CatchKingResultField(score, rowType, cardPos, cardValue, keepFieldCard, destroyHandCard, getReward, isFiveNear)

		def MiniGameCatchKingSetEndCard(self, cardPos, cardNumber):
			if self.catch_king_game:
				self.catch_king_game.CatchKingSetEndCard(cardPos, cardNumber)

		def MiniGameCatchKingReward(self, rewardCode):
			if self.catch_king_game:
				self.catch_king_game.CatchKingReward(rewardCode)

	if app.ENABLE_ATTENDANCE_EVENT:
		def __ClickAttendanceButton(self):
			if self.attendanceGame:
				self.attendanceGame.Open()

		def MiniGameAttendanceSetData(self, type, value):
			if not self.attendanceGame:
				return
				
			if type == 1:
				self.attendanceGame.MiniGameAttendanceSetDay(value)
			elif type == 2:
				self.attendanceGame.MiniGameAttendanceSetMissionClear(value)
			elif type == 3:
				self.attendanceGame.MiniGameAttendanceSetReward(value)
				
			self.attendanceGame.RefreshAttendanceBoard()

	if app.ENABLE_FISH_EVENT_SYSTEM:
		def MiniGameFishUse(self, shape, useCount):
			if self.fishGame:
				self.fishGame.MiniGameFishUse(shape, useCount)
				
		def MiniGameFishAdd(self, pos, shape):
			if self.fishGame:
				self.fishGame.MiniGameFishAdd(pos, shape)
				
		def MiniGameFishReward(self, vnum):
			if self.fishGame:
				self.fishGame.MiniGameFishReward(vnum)
				
		def MiniGameFishCount(self, count):
			if self.fishGame:
				self.fishGame.MiniGameFishCount(count)
			
		def __ClickFishEventButton(self):
			if self.fishGame:
				self.fishGame.Open()
			
	def Destroy(self):
		if app.ENABLE_MINI_GAME_CATCH_KING:
			if self.catch_king_game:
				self.catch_king_game.Destroy()
				self.catch_king_game = None

		if app.ENABLE_ATTENDANCE_EVENT:
			if self.attendanceGame:
				self.attendanceGame.Destroy()
				self.attendanceGame = None

		if app.ENABLE_FISH_EVENT_SYSTEM:
			if self.fishGame:
				self.fishGame.Destroy()
				self.fishGame = None
		self.ClearDictionary()
		
class EventInfoDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.board			= None
		self.button_dict	= {}
		self.button_slot_1	= {}
		self.button_slot_2	= {}
		self.button_slot_3	= {}
		self.button_slot_4	= {}
		self.button_text	= {}
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Destroy()

	def LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/eventinfodialog.py")
		except:
			import exception
			exception.Abort("eventinfodialog.LoadDialog.LoadScript")

		try:
			GetObject = self.GetChild
			self.board = GetObject("board")
			self.TitleBar = self.GetChild("TitleBar")
			self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("eventinfodialog.LoadDialog.BindObject")

	def AppendButton(self, vnum, vnum1, vnum2, text, name, time, func):
		if self.button_dict.has_key(name):
			return

		button = ui.Button()
		button.SetParent( self )
		button_count = len(self.button_dict)
		pos_y = (button_gap * (button_count + 1)) + button_count * button_height+20
		button.SetPosition( 15, pos_y+30 )
		button.SetUpVisual( "d:/ymir work/ui/event/button/event_button_board.tga" )
		button.SetOverVisual( "d:/ymir work/ui/event/button/event_button_board_up.tga" )
		button.SetDownVisual( "d:/ymir work/ui/event/button/event_button_board_down.tga" )

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetPosition( 60, pos_y+35 )
		textLine.SetText(text)
		self.textLine = textLine

		slotGrid = ui.SlotWindow()
		slotGrid.SetParent(self)
		slotGrid.SetPosition( 120+82, pos_y+37 )
		slotGrid.AppendSlot(0, 0, 0, 32, 32)
		self.slotGrid = slotGrid

		slotGrid2 = ui.SlotWindow()
		slotGrid2.SetParent(self)
		slotGrid2.SetPosition( 120+82+32, pos_y+37 )
		slotGrid2.AppendSlot(0, 0, 0, 32, 32)
		self.slotGrid2 = slotGrid

		slotGrid3 = ui.SlotWindow()
		slotGrid3.SetParent(self)
		slotGrid3.SetPosition( 120+82+32+32, pos_y+37 )
		slotGrid3.AppendSlot(0, 0, 0, 32, 32)
		self.slotGrid3 = slotGrid

		slotGrid4 = ui.TextLine()
		slotGrid4.SetParent(self)
		slotGrid4.SetPosition( 120+82+32+32-245, pos_y+37+56-6-30 )
		self.slotGrid4 = slotGrid4
		self.time = time

		if vnum:
			slotGrid.SetItemSlot(0, vnum)

		if vnum1:
			slotGrid2.SetItemSlot(0, vnum1)

		if vnum2:
			slotGrid3.SetItemSlot(0, vnum2)

		if time:
			leftTime = max(0, self.time - app.GetGlobalTimeStamp())
			slotGrid4.SetText(localeInfo.TOOLTIP_TIME % (localeInfo.FormatTime(leftTime)))

		if func:
			button.SetEvent( ui.__mem_func__(func) )

		button.Show()
		slotGrid.Show()
		slotGrid2.Show()
		slotGrid3.Show()
		slotGrid4.Show()
		textLine.Show()
		self.button_dict[name] = button
		self.button_slot_1[vnum] = slotGrid
		self.button_slot_2[vnum1] = slotGrid2
		self.button_slot_3[vnum2] = slotGrid3
		self.button_slot_4[time] = slotGrid4
		self.button_text[text] = textLine

	def DeleteButton(self, name):

		if not self.button_dict.has_key(name):
			return

		if not self.button_slot_1.has_key(vnum):
			return

		if not self.button_slot_2.has_key(vnum1):
			return

		if not self.button_slot_3.has_key(vnum2):
			return

		if not self.button_slot_4.has_key(time):
			return

		if not self.button_text.has_key(text):
			return

		self.button_dict[name].Hide()
		del self.button_dict[vnum]

		self.button_slot_1[vnum].Hide()
		del self.button_slot_1[vnum]

		self.button_slot_2[vnum1].Hide()
		del self.button_slot_2[vnum1]

		self.button_slot_3[vnum2].Hide()
		del self.button_slot_3[vnum2]

		self.button_slot_4[time].Hide()
		del self.button_slot_4[time]

		self.button_text[text].Hide()
		del self.button_text[text]

	def DeleteAllButton(self):

		for button in self.button_dict.values():
			button.Hide()
			del button

		self.button_dict.clear()

		for slotGrid in self.button_slot_1.values():
			slotGrid.Hide()
			del slotGrid

		self.button_slot_2.clear()

		for slotGrid2 in self.button_slot_2.values():
			slotGrid2.Hide()
			del slotGrid2

		self.button_slot_2.clear()

		for slotGrid3 in self.button_slot_3.values():
			slotGrid3.Hide()
			del slotGrid3

		self.button_slot_3.clear()

		for slotGrid4 in self.button_slot_4.values():
			slotGrid4.Hide()
			del slotGrid4

		self.button_slot_4.clear()

		for textLine in self.button_text.values():
			textLine.Hide()
			del textLine

		self.button_text.clear()

	def Show(self):
		self.LoadWindow()
		ui.ScriptWindow.Show(self)

	def Destroy(self):
		self.isLoaded = 0
		self.board			= None
		self.button_dict	= {}
		self.button_slot_1	= {}
		self.button_slot_2	= {}
		self.button_slot_3	= {}
		self.button_slot_4	= {}
		self.button_text	= {}

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		if self.time > 0:
			try:
				leftTime = max(0, self.time - app.GetGlobalTimeStamp())
				self.slotGrid4.SetText(localeInfo.TOOLTIP_TIME % (localeInfo.FormatTime(leftTime)))
			except:
				pass

	def RefreshDialog(self):
		total_len = len(self.button_dict)
		board_height = (button_height * total_len) + (button_gap * (total_len + 1))
		self.board.SetSize(325, board_height+55) #Pencere

		self.SetSize(325, board_height+45)


