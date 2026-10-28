if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import snd, item, chat, mouseModule, ui, localeInfo, uiScriptLocale, grp
import uiToolTip
import systemSetting
import wndMgr
import uiCommon
import renderTarget

BIYOLOG_RENDER_INDEX = 51
BIYOLOG_NPC_VNUM = 20084
# Render target konum ve boyut (x, y, width, height) - buradan ayarlayin
BIYOLOG_RT_X = 220	#Art?kça sa?a kayar
BIYOLOG_RT_Y = 30	#Art?kça a?a??ya kayar
BIYOLOG_RT_WIDTH = 160	#Art?kça sa?a ve sola geni?ler
BIYOLOG_RT_HEIGHT = 360	#Art?kça yukar?dan a?a?? uzar

ITEM_DATA = {
	1 : "0#30006#|1#30220#|2#50109#",
	2 : "0#30047#|1#30221#|2#50110#",
	3 : "0#30015#|1#30222#|2#50111#",
	4 : "0#30050#|1#30223#|2#50112#",
	5 : "0#30165#|1#30224#|2#50113#",
	6 : "0#30166#|1#30225#|2#50114#",
	7 : "0#30167#|1#30226#|2#50115#",
	8 : "0#30168#|1#30227#|2#50114#",
	9  : "0#30251#",
	10 : "0#30252#|1#30228#",
	11 : "0#50615#",
	12 : "0#50616#",
	13 : "0#50617#",
	14 : "0#50618#"
}

LEVEL_LIST = {
	1 : "30",
	2 : "40",
	3 : "50",
	4 : "60",
	5 : "70",
	6 : "80",
	7 : "85",
	8 : "90",
	9 : "92",
	10: "94",
	11: "96",
	12: "98",
	13: "100",
	14: "102"
}

BONUSES_LIST = {
	1 : "Hareket Hýzý %10|",
	2 : "Saldýrý Hýzý %5|",
	3 : "Defans %60|",
	4 : "Saldýrý Deðeri %50|",
	5 : "Hareket Hýzý %11|Defans %10|",
	6 : "Saldýrý Hýzý %6|Saldýrý Deðeri %10|",
	7 : "Tüm karakter saldýrýlarýna karþý savunma þansý %10|",
	8 : "Yarý insanlara karþý güçlü %10|",
	9 : "Maks. HP +1000|Defans +120|Saldýrý Deðeri +50|",
	10: "Maks. HP +1100|Defans +140|Saldýrý Deðeri +60|",
	11 : "Tüm karakter saldýrýlarýna karþý savunma þansý %2|",
	12 : "Tüm karakter saldýrýlarýna karþý savunma þansý %3|",
	13 : "Tüm karakter saldýrýlarýna karþý savunma þansý %4|",
	14 : "Tüm karakter saldýrýlarýna karþý savunma þansý %6|",
}

TEXT_LIST = [
	"",
	"",
]

class BiyologBonusSelectDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Buttons = {}
		self.selectButton = -1
		self.acceptButton = None


	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.Buttons = {}
		self.selectButton = -1
		self.acceptButton = None


	def LoadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/biyolog_bonusselect_dialog.py")
		except:
			import exception
			exception.Abort("BiyologWindow.LoadWindow.LoadObject")
		try:
			self.GetChild("TitleBar").SetCloseEvent(self.Close)
			self.acceptButton = self.GetChild("AcceptButton")

		except:
			import exception
			exception.Abort("BiyologWindow.LoadWindow.BindObject")

		self.acceptButton.SetEvent(ui.__mem_func__(self.Accept))


	def MakeButton(self, mission):
		if not mission:
			return

		for i in xrange(3):
			if self.Buttons.has_key(i):
				del self.Buttons[i]
				self.Buttons[i] = None

			button = ui.MakeButton(self, 20, 46+28*i , "", "d:/ymir work/ui/game/myshop_deco/", "select_btn_01.sub", "select_btn_02.sub", "select_btn_03.sub")
			button.SetText(BONUSES_LIST[mission].split("|")[i])
			button.SetEvent(ui.__mem_func__(self.__SelectBonus), i)
			button.Show()
			self.Buttons[i] = button
			
		self.selectButton = -1

	def __SelectBonus(self, index):
		for key,item in self.Buttons.iteritems():
			if key != index:
				item.SetUp()
				item.Enable()
			else:
				item.Down()
				item.Disable()

		self.selectButton = (index + 1)

	def Accept(self):
		if self.selectButton == -1:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Bonus seçimi yapmadin.")
			return
		
		if self.selectButton > 0 and self.selectButton <= 3:
			net.SendChatPacket("/biyolog complate %d" % (self.selectButton))

		self.Close()

	def Open(self):
		self.SetTop()
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
		return TRUE

	def __ShowBiyologNPC(self):
		try:
			renderTarget.SelectModel(BIYOLOG_RENDER_INDEX, BIYOLOG_NPC_VNUM)
			renderTarget.SetVisibility(BIYOLOG_RENDER_INDEX, True)
		except Exception, e:
			import dbg
			dbg.TraceError("BiyologWindow.__ShowBiyologNPC: %s" % str(e))

	def __HideBiyologNPC(self):
		try:
			renderTarget.SetVisibility(BIYOLOG_RENDER_INDEX, False)
		except Exception:
			pass

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class BiyologWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.boardElement = {"board" : [], "element" : {}}
		self.ItemData = {"grid" : [], "items" : {}, "slots" : {}}
		self.text = {0 : None, 1 : {}, 2 : {}}
		self.Mission = 0
		self.MissionState = 0
		self.GivenCount = None
		self.bonus = None
		self.NeedCount = None
		self.LeftTime = None
		self.remainingTime = 0
		self.toolTipItem = None
		self.bonusSelect = None
		self.biyologSelect = None
		self.toolTip = None
		self.wndBiyologRenderTarget = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.boardElement = {"boards" : [], "element" : {}}
		self.ItemData = {"grid" : [], "items" : {}, "slots" : {}}
		self.text = {0 : None, 1 : {}, 2 : {}}
		self.Mission = 0
		self.MissionState = 0
		self.GivenCount = None
		self.bonus = None
		self.NeedCount = None
		self.LeftTime = None
		self.remainingTime = 0
		self.toolTipItem = None
		self.bonusSelect = None
		self.biyologSelect = None
		self.toolTip.Hide()
		self.toolTip = None
		self.wndBiyologRenderTarget = None

	def __LoadWindow(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/biyolog_window.py")
		except:
			import exception
			exception.Abort("BiyologWindow.__LoadWindow.LoadObject")
		try:
			self.GetChild("TitleBar").SetCloseEvent(self.Close)

			self.boardElement["board"].append(self.GetChild("board"))
			self.boardElement["board"].append(self.GetChild("BlackBoard"))

			self.GivenCount = self.GetChild("GivenCount")
			self.NeedCount = self.GetChild("NeedCount")
			self.LeftTime = self.GetChild("LeftTime")
			self.bonus = self.GetChild("text")

			self.ItemData["grid"].append(self.GetChild("itemSlot01"))
			self.ItemData["grid"].append(self.GetChild("itemSlot02"))
			self.ItemData["grid"].append(self.GetChild("itemSlotImage04"))
			self.ItemData["grid"].append(self.GetChild("itemSlotImage05"))
			self.ItemData["grid"].append(self.GetChild("itemSlotImage01"))
			self.ItemData["grid"].append(self.GetChild("itemSlotImage03"))

			self.boardElement["element"]["image01"] = self.GetChild("Image01")
			self.boardElement["element"]["image02"] = self.GetChild("Image02")
			self.boardElement["element"]["tab01"] = self.GetChild("tab01")
			#self.boardElement["element"]["line01"] = self.GetChild("line01")
			self.boardElement["element"]["Image01"] = self.GetChild("Image01")
			self.boardElement["element"]["Image02"] = self.GetChild("Image02")
			self.boardElement["element"]["giveButton"] = self.GetChild("GiveButton")
			#self.boardElement["element"]["giveAllButton"] = self.GetChild("GiveAllButton")
			self.boardElement["element"]["giftButton"] = self.GetChild("GiftButton")
		except:
			import exception
			exception.Abort("BiyologWindow.__LoadWindow.BindObject")

		# Render target: gercek NPC (uiremoteshop referansi) - konum/boyut: BIYOLOG_RT_*
		self.wndBiyologRenderTarget = ui.RenderTarget()
		self.wndBiyologRenderTarget.SetParent(self.boardElement["board"][0])
		self.wndBiyologRenderTarget.SetPosition(BIYOLOG_RT_X, BIYOLOG_RT_Y)
		self.wndBiyologRenderTarget.SetSize(BIYOLOG_RT_WIDTH, BIYOLOG_RT_HEIGHT)
		self.wndBiyologRenderTarget.SetRenderTarget(BIYOLOG_RENDER_INDEX)
		self.wndBiyologRenderTarget.Show()
		renderTarget.SetBackground(BIYOLOG_RENDER_INDEX, "d:/ymir work/ui/game/bestwork/render_oval2.tga")
		renderTarget.SetScale(BIYOLOG_RENDER_INDEX, 1.0)
		renderTarget.SetVisibility(BIYOLOG_RENDER_INDEX, True)

		self.boardElement["element"]["tab01"].Hide()
		self.boardElement["element"]["giveButton"].SetEvent(ui.__mem_func__(self.Give), 0)
		#self.boardElement["element"]["giveAllButton"].SetEvent(ui.__mem_func__(self.Give), 1)
		self.boardElement["element"]["giftButton"].SetEvent(ui.__mem_func__(self.GiftButton))
		self.boardElement["element"]["giftButton"].Hide()
		
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.toolTipWidth += 80
		
		#self.boardElement["element"]["giveAllButton"].ShowToolTip = lambda i=0: self.__ShowToolTip()
		#self.boardElement["element"]["giveAllButton"].HideToolTip = lambda i=0: self.__HideToolTip()

		self.ItemData["grid"][0].SetOverInItemEvent(ui.__mem_func__(self.__OverInToolTip))
		self.ItemData["grid"][0].SetOverOutItemEvent(ui.__mem_func__(self.__OverOutToolTip))
		self.ItemData["grid"][1].SetOverInItemEvent(ui.__mem_func__(self.__OverInToolTipInven))
		self.ItemData["grid"][1].SetOverOutItemEvent(ui.__mem_func__(self.__OverOutToolTip))
		self.ItemData["grid"][1].SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.ItemData["grid"][1].SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		
		self.RefreshSize(304+4*13+10+5)
		bonusSelect = BiyologBonusSelectDialog()
		bonusSelect.LoadWindow()
		bonusSelect.Close()
		self.bonusSelect = bonusSelect
		
		biyologSelect = uiCommon.QuestionDialog()
		biyologSelect.Close()
		self.biyologSelect = biyologSelect

	def SelectEmptySlot(self, iSlotIndex):
		if not mouseModule.mouseController.isAttached():
			return

		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		mouseModule.mouseController.DeattachObject()

		if player.SLOT_TYPE_INVENTORY != attachedSlotType:
			return

		for guislotpos, inventorySlotPos in self.ItemData["slots"].items():
			if inventorySlotPos == attachedSlotPos:
				del self.ItemData["slots"][guislotpos]

		if iSlotIndex == 0 and not player.GetItemIndex(attachedSlotPos) in (71035, 76020, 39023):
			return

		if iSlotIndex == 1 and 70022 != player.GetItemIndex(attachedSlotPos):
			return

		self.ItemData["slots"][iSlotIndex] = attachedSlotPos
		snd.PlaySound("sound/ui/drop.wav")
		self.RefreshItemSlot()

	def SelectItemSlot(self, iSlotIndex):
		isAttached = mouseModule.mouseController.isAttached()

		if isAttached:
			snd.PlaySound("sound/ui/loginfail.wav")
			mouseModule.mouseController.DeattachObject()
		else:
			if not iSlotIndex in self.ItemData["slots"]:
				return

			snd.PlaySound("sound/ui/drop.wav")
			del self.ItemData["slots"][iSlotIndex]
			self.RefreshItemSlot()

	def Give(self, arg):
	
		if player.GetStatus(player.LEVEL) < int(LEVEL_LIST[self.Mission]):
			import uiCommon
			self.uyari2 = uiCommon.PopupDialog()
			self.uyari2.SetText("Görev verebilmek icin Seviyen %s olmalý." % LEVEL_LIST[self.Mission])
			self.uyari2.Open()
			return
			
		slot01 = -1; slot02 = -1;

		if self.ItemData["slots"].has_key(0):
			slot01 = self.ItemData["slots"][0]
		if self.ItemData["slots"].has_key(1):
			slot02 = self.ItemData["slots"][1]

		if arg:
			net.SendChatPacket("/biyolog all %d %d" % (slot02, slot01))
		else:
			net.SendChatPacket("/biyolog request %d %d" % (slot02, slot01))

	def GiftButton(self):
		if self.MissionState == 2:
			net.SendChatPacket("/biyolog stone")
		elif self.MissionState == 3:
			net.SendChatPacket("/biyolog complate")

	def Open(self):
		if self.Mission > 10:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Biyolog görevlerini tamamladýn.")
			return
			
		if player.GetStatus(player.LEVEL) < 30:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Biyolog görevini 30 level'den sonra alabilirsin.")
			return
			
		leftTime = max(0, self.remainingTime- app.GetGlobalTimeStamp())
		self.SetMission(self.Mission, self.MissionState, int(self.GivenCount.GetText()), int(self.NeedCount.GetText()), leftTime)
			
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)
		self.__ShowBiyologNPC()

	def SetItemToolTip(self, toolTipItem):
		self.toolTipItem = toolTipItem

	def SetMission(self, mission, missionState, givenCount, needCount, remainingTime):
		self.Mission = int(mission)
		if self.Mission <= 0 or self.Mission not in ITEM_DATA:
			return
		if self.Mission > 10:
			self.Close()
			return

		self.MissionState = int(missionState)
		
		if self.MissionState > 1:
			self.GivenCount.SetText(str(needCount))
		else:
			self.GivenCount.SetText(str(givenCount))
			
		self.NeedCount.SetText(str(needCount))
		self.remainingTime = app.GetGlobalTimeStamp()+int(remainingTime)


		self.ItemData["grid"][4].Show()
		self.ItemData["grid"][5].Show()
		self.boardElement["element"]["image01"].Show()
		self.boardElement["element"]["image02"].Show()

		if self.MissionState == 3 or self.MissionState == 2:
			self.boardElement["element"]["giveButton"].Hide()
			#self.boardElement["element"]["giveAllButton"].Hide()
			self.boardElement["element"]["giftButton"].Show()

			if self.MissionState == 2:
				self.boardElement["element"]["giftButton"].SetText(uiScriptLocale.BIYOLOG_BUTTON_03)
				self.boardElement["element"]["giftButton"].SetEvent(ui.__mem_func__(self.GiftButton))
			elif self.MissionState == 3 and self.Mission in [9, 10]:
				self.boardElement["element"]["giftButton"].SetText("Bonus Seç")
				self.boardElement["element"]["giftButton"].SetEvent(ui.__mem_func__(self.__BonusSelectDialog))
			else:
				self.boardElement["element"]["giftButton"].SetText(uiScriptLocale.BIYOLOG_BUTTON_04)
				self.boardElement["element"]["giftButton"].SetEvent(ui.__mem_func__(self.GiftButton))
		else:
			self.boardElement["element"]["giveButton"].Show()
			#self.boardElement["element"]["giveAllButton"].Show()
			self.boardElement["element"]["giftButton"].Hide()
			self.boardElement["element"]["giftButton"].SetText(uiScriptLocale.BIYOLOG_BUTTON_04)

		self.SetItems(ITEM_DATA[self.Mission])
		
		# if self.Mission in [9, 10]:
			# self.bonus.SetText("Seçilebilir Bonuslar")
		# else:
			# self.bonus.SetText("Bonuslar")

		if not self.Mission in [21, 22]:
			self.SetBonuses(BONUSES_LIST[self.Mission])
		else:
			if self.text[0]:
				del self.text[0]
				self.text[0] = None

			#self.RefreshSize()

		for i in xrange(len(TEXT_LIST)):
			if self.text[1].has_key(i):
				del self.text[1][i]

			if not self.text[1].has_key(i):
				text = MultiLineText()
				text.SetParent(self)
				text.SetText(TEXT_LIST[i], "Tahoma:12")
				text.SetPosition(60, 315)
				text.Show()
				self.text[1][i] = text

	def SetItems(self, data):
		for i in xrange(3):
			self.ItemData["grid"][0].ClearSlot(i)

		data = data.split("|")

		for i in xrange(len(data)):
			itemData = data[i].split("#")
			self.ItemData["grid"][0].SetItemSlot(int(itemData[0].strip()), int(itemData[1].strip()), 0)
			self.ItemData["grid"][0].ActivateSlot(int(itemData[0].strip()), self.MissionState-1)
			self.ItemData["items"][int(itemData[0].strip())] = int(itemData[1].strip())

		self.RefreshItemSlot()

	def SetBonuses(self, data):
		#self.RefreshSize(304+4*13+10)

		text = MultiLineText()
		text.SetParent(self)
		text.SetText(data)
		text.SetPosition(self.GetWidth()-275, 75)
		text.Show()
		self.text[0] = text

	def __BonusSelectDialog(self):
		if not self.bonusSelect:
			return

		self.bonusSelect.MakeButton(self.Mission)
		self.bonusSelect.Open()

	def Close(self):
		self.__HideBiyologNPC()
		if self.toolTipItem:
			self.toolTipItem.Hide()
			
		if self.bonusSelect:
			self.bonusSelect.Close()
			
		if self.biyologSelect:
			self.biyologSelect.Close()

		self.Hide()
		#self.RefreshSize()
		return TRUE

	def __ShowBiyologNPC(self):
		try:
			renderTarget.SelectModel(BIYOLOG_RENDER_INDEX, BIYOLOG_NPC_VNUM)
			renderTarget.SetVisibility(BIYOLOG_RENDER_INDEX, True)
		except Exception, e:
			import dbg
			dbg.TraceError("BiyologWindow.__ShowBiyologNPC: %s" % str(e))

	def __HideBiyologNPC(self):
		try:
			renderTarget.SetVisibility(BIYOLOG_RENDER_INDEX, False)
		except Exception:
			pass

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def __OverInToolTip(self, slotIndex):
		if not self.ItemData["items"].has_key(slotIndex):
			return

		if self.toolTipItem:
			self.toolTipItem.SetItemToolTip(self.ItemData["items"][slotIndex])

	def __OverInToolTipInven(self, slotIndex):
		if not self.ItemData["slots"].has_key(slotIndex):
			return

		if self.toolTipItem:
			self.toolTipItem.SetItemToolTip(player.GetItemIndex(self.ItemData["slots"][slotIndex]))
			
	def __ShowToolTip(self):
		self.toolTip.ClearToolTip()
		self.toolTip.AppendTextLine("Toplu verme iþlemini durdurmak istiyorsanýz", grp.GenerateColor(0.6911, 0.8754, 0.7068, 1.0))
		self.toolTip.AppendTextLine("tekrar butona týklayýn.", grp.GenerateColor(0.6911, 0.8754, 0.7068, 1.0))
		self.toolTip.Show()
		
	def __HideToolTip(self):
		self.toolTip.Hide()
		
	def __OverOutToolTip(self):
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	def RefreshItemSlot(self):
		for i in xrange(2):
			if not self.ItemData["slots"].has_key(i):
				self.ItemData["grid"][1].ClearSlot(i)
				continue

			itemCount = player.GetItemCount(self.ItemData["slots"][i])

			if itemCount <= 0:
				del self.ItemData["slots"][i]
				self.RefreshItemSlot()
				return

			if itemCount <= 1:
				itemCount = 0

			self.ItemData["grid"][1].SetItemSlot(i, player.GetItemIndex(self.ItemData["slots"][i]), itemCount)

		self.ItemData["grid"][1].RefreshSlot()

	def RefreshSize(self, len_=0):
		if len_ == 0:
			len_ = 304
			self.boardElement["element"]["tab01"].Hide()
		else:
			self.boardElement["element"]["tab01"].Show()

		self.SetSize(self.GetWidth(), len_+10)
		Height = self.GetHeight() - 10
		
		self.boardElement["board"][0].SetSize(self.GetWidth()-2, Height+25)
		self.boardElement["board"][1].SetSize(self.GetWidth()-28, Height-48)

		if self.wndBiyologRenderTarget:
			self.wndBiyologRenderTarget.SetPosition(BIYOLOG_RT_X, BIYOLOG_RT_Y)
			self.wndBiyologRenderTarget.SetSize(BIYOLOG_RT_WIDTH, BIYOLOG_RT_HEIGHT)

		#self.boardElement["element"]["line01"].SetPosition(40, Height-139)

		self.ItemData["grid"][1].SetPosition(210, Height-132)
		self.ItemData["grid"][2].SetPosition(210, Height-132)
		self.ItemData["grid"][3].SetPosition(210, Height-86)

		self.boardElement["element"]["giveButton"].SetPosition(15, Height-130)
		#self.boardElement["element"]["giveAllButton"].SetPosition(146, Height-10)
		self.boardElement["element"]["giftButton"].SetPosition(15, Height-130)

	def OnUpdate(self):
		if self.Mission in [9, 10] and self.MissionState == 3:
			self.LeftTime.SetText("Bonus Seç")
			return
		if self.ItemData["slots"].has_key(1):
			if self.LeftTime.GetText() != uiScriptLocale.BIYOLOG_STATE:
				self.LeftTime.SetText(uiScriptLocale.BIYOLOG_STATE)
			return

		leftTime = max(0, self.remainingTime- app.GetGlobalTimeStamp())

		if leftTime == 0:
			self.LeftTime.SetText(uiScriptLocale.BIYOLOG_STATE)
			return

		leftHour = int((leftTime / 60) / 60)
		leftMin = int((leftTime / 60) % 60)
		leftSecond = int(leftTime % 60)

		self.LeftTime.SetText(localeInfo.BIYOLOG_LEFT_TIME % (leftHour, leftMin, leftSecond))

class MultiLineText(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.textLine = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateTextLine(self):
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetOutline()
		textLine.Show()

		self.textLine.append(textLine)

	def SetText(self, text, font = "Tahoma:13"):
		self.textLine = []
		len_ = len(text.split("|"))

		for i in xrange(len_):
			self.__CreateTextLine()
			self.textLine[i].SetFontName(font)
			self.textLine[i].SetText(text.split("|")[i])
			self.textLine[i].SetPosition(self.GetWidth()/2, (i*13))
			self.textLine[i].SetHorizontalAlignCenter()