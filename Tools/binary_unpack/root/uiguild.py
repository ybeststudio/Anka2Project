if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import guild
import wndMgr
import grp
import grpText
import uiPickMoney
import localeInfo
import skill
import mouseModule
import uiUploadMark
import uiCommon
import uiToolTip
import playerSettingModule
import constInfo
import background
import miniMap
import uiScriptLocale
import interfaceModule
if app.ENABLE_GUILDRENEWAL_SYSTEM:
	import uiGuildPopup

if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
	import uiGuildList

from _weakref import proxy

if app.ENABLE_GUILD_DONATE_ATTENDANCE:
	import item

DISABLE_GUILD_SKILL = FALSE
DISABLE_DECLARE_WAR = FALSE

def NumberToMoneyString(n):
	return localeInfo.NumberToMoneyString(n)

MATERIAL_STONE_INDEX = 0
MATERIAL_LOG_INDEX = 1
MATERIAL_PLYWOOD_INDEX = 2
if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
	MATERIAL_STONE_PLUS_INDEX = 0
	MATERIAL_LOG_PLUS_INDEX = 1
	MATERIAL_PLYWOOD_PLUS_INDEX = 2
	MATERIAL_SMELT_STONE_INDEX = 3

MATERIAL_STONE_ID = 90010
MATERIAL_LOG_ID = 90011
MATERIAL_PLYWOOD_ID = 90012
if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
	MATERIAL_STONE_PLUS_ID = 90015
	MATERIAL_LOG_PLUS_ID = 90016
	MATERIAL_PLYWOOD_PLUS_ID = 90017
	MATERIAL_SMELT_STONE_ID = 31225

BUILDING_DATA_LIST = []

def GetGVGKey(srcGuildID, dstGuildID):
	minID = min(srcGuildID, dstGuildID)
	maxID = max(srcGuildID, dstGuildID)
	return minID*1000 + maxID

def unsigned32(n):
	return n & 0xFFFFFFFFL

if app.ENABLE_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_DONATE_ATTENDANCE:
	class DonateGuildDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)

			self.toolTip = uiToolTip.ItemToolTip()
			self.type=0
			self.__CreateDialog()
			self.questionDialog = None
			self.popup = None
			self.open = FALSE
			self.state = TRUE
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.ShowButtonToolTip = FALSE

		def __del__(self):
			ui.ScriptWindow.__del__(self)
			self.questionDialog = None
			self.popup = None
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.ShowButtonToolTip = FALSE

		def Open(self):
			if self.open == TRUE:
				return
			self.open = TRUE;
			self.SetCenterPosition()
			self.SetTop()
			ui.ScriptWindow.Show(self)
			net.GuildDonateInfoOpen()
			self.RefreshGuildDonateInfo()
			#self.Show()

		def Close(self):
			self.ClearDictionary()
			self.board = None
			self.questionDialog = None
			self.popup = None

			self.normal_donate = None
			self.normal_exp = None
			self.middle_donate = None
			self.middle_exp = None
			self.high_donate = None
			self.high_exp = None

			self.Hide()
			self.open = FALSE
			self.state = TRUE
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if self.toolTip:
					self.toolTip.Hide()
					self.ShowButtonToolTip = FALSE

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/guilddonatedialog.py")

			getObject = self.GetChild
			self.board = getObject("board")

			self.typeButtonList=[]
			self.typeButtonList.append(getObject("NoramlDonateButton"))
			self.typeButtonList.append(getObject("MiddleDonateButton"))
			self.typeButtonList.append(getObject("HighDonateButton"))

			self.normal_donate = getObject("normal_donate_medal_of_honor")
			self.normal_exp = getObject("normal_donate_guild_exp")
			self.middle_donate = getObject("middle_donate_medal_of_honor")
			self.middle_exp = getObject("middle_donate_guild_exp")
			self.high_donate = getObject("high_donate_medal_of_honor")
			self.high_exp = getObject("high_donate_guild_exp")
			self.medal_of_honor_text = getObject("cur_medal_of_honor_text")
			self.donate_count_text = getObject("donate_count_text")

			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

			self.normal_donate_get_item_slot = getObject("normal_donate_get_item_slot")
			self.middle_donate_get_item_slot = getObject("middle_donate_get_item_slot")
			self.high_donate_get_item_slot = getObject("high_donate_get_item_slot")
			for i in xrange(3):
				self.normal_donate_get_item_slot.SetItemSlot(i, MATERIAL_STONE_ID+i, 3)
				self.middle_donate_get_item_slot.SetItemSlot(i, MATERIAL_STONE_ID+i, 5)
				self.high_donate_get_item_slot.SetItemSlot(i, MATERIAL_STONE_ID+i, 15)

			self.RefreshGuildDonateInfo()

			self.typeButtonList[0].SAFE_SetEvent(self.__OnClickTypeButtonNormal)
			self.typeButtonList[1].SAFE_SetEvent(self.__OnClickTypeButtonMiddle)
			self.typeButtonList[2].SAFE_SetEvent(self.__OnClickTypeButtonHgih)

			self.normal_donate_get_item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItemNormal))
			self.middle_donate_get_item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItemNormal))
			self.high_donate_get_item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItemNormal))

			self.normal_donate_get_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItemNormal))
			self.middle_donate_get_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItemNormal))
			self.high_donate_get_item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItemNormal))

			self.typeButtonList[0].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), uiScriptLocale.GUILD_DONATE_TAB_NORMAL)
			self.typeButtonList[0].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.typeButtonList[1].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), uiScriptLocale.GUILD_DONATE_TAB_MIDDLE)
			self.typeButtonList[1].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.typeButtonList[2].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), uiScriptLocale.GUILD_DONATE_TAB_HIGH)
			self.typeButtonList[2].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

			self.GetChild("normal_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_NORMAL_MATERIAL)
			self.GetChild("normal_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
			self.GetChild("middle_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_MIDDLE_MATERIAL)
			self.GetChild("middle_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
			self.GetChild("high_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_HIGH_MATERIAL)
			self.GetChild("high_donate_medal_of_honor_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
			self.GetChild("cur_medal_of_honor_img").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_MEDAL_OF_HONOR)
			self.GetChild("cur_medal_of_honor_img").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")

			self.GetChild("normal_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_GUILD_EXP)
			self.GetChild("normal_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
			self.GetChild("middle_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_GUILD_EXP)
			self.GetChild("middle_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")
			self.GetChild("high_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", uiScriptLocale.GUILD_DONATE_GUILD_EXP)
			self.GetChild("high_donate_guild_exp_icon").SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", "")

		def RefreshGuildDonateInfo(self):
			if guild.GetGuildLevel() < 20:
				return

			norm_donate, norm_exp, mid_d, mid_exp, high_d, high_exp  = guild.GetGuildDonateStats()
			medals = player.GetMedalOfHonor()
			donate_count_left, donate_count_max = guild.GetGuildDonateCount()

			self.normal_donate.SetText(localeInfo.NumberToMoneyString(norm_donate))
			self.normal_exp.SetText(localeInfo.NumberToMoneyString(norm_exp))
			self.middle_donate.SetText(localeInfo.NumberToMoneyString(mid_d))
			self.middle_exp.SetText(localeInfo.NumberToMoneyString(mid_exp))
			self.high_donate.SetText(localeInfo.NumberToMoneyString(high_d))
			self.high_exp.SetText(localeInfo.NumberToMoneyString(high_exp))

			self.medal_of_honor_text.SetText(str(medals))	#Meine Tapferkeitsmedaillien
			self.donate_count_text.SetText(uiScriptLocale.GUILD_DONATE_COUNT % (donate_count_left, donate_count_max))	#Verfuegbare Spenden
			if donate_count_max == 3:
				self.CheckCountLimit()

		# Block the Buttons if max donations per day reached
		def CheckCountLimit(self):
			donate_count_left, donate_count_max = guild.GetGuildDonateCount()

			if donate_count_left >= 3:
				self.typeButtonList[0].Down()
				self.typeButtonList[1].Down()
				self.typeButtonList[2].Down()
				self.state = FALSE

		def __OnClickTypeButtonNormal(self):
			if self.state == FALSE:
				return

			self.__ClickTypeRadioButton(0)
			questionDialog = uiCommon.QuestionDialog()
			questionDialog.SetText(localeInfo.GUILD_DONATE_QUESTION_NORMAL)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.__DonateNormal))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.ClosePopUpDialog))
			questionDialog.Open()
			self.popup = questionDialog

		def __OnClickTypeButtonMiddle(self):
			if self.state == FALSE:
				return

			self.__ClickTypeRadioButton(1)
			questionDialog = uiCommon.QuestionDialog()
			questionDialog.SetText(localeInfo.GUILD_DONATE_QUESTION_MIDDLE)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.__DonateMiddle))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.ClosePopUpDialog))
			questionDialog.Open()
			self.popup = questionDialog

		def __OnClickTypeButtonHgih(self):
			if self.state == FALSE:
				return

			self.__ClickTypeRadioButton(2)
			questionDialog = uiCommon.QuestionDialog()
			questionDialog.SetText(localeInfo.GUILD_DONATE_QUESTION_HIGH)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.__DonateHigh))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.ClosePopUpDialog))
			questionDialog.Open()
			self.popup = questionDialog

		def __ClickTypeRadioButton(self, type):
			self.__ClickRadioButton(self.typeButtonList, type)
			self.type=type

		def __ClickRadioButton(self, buttonList, buttonIndex):
			try:
				selButton=buttonList[buttonIndex]
			except IndexError:
				return

			for eachButton in buttonList:
				eachButton.SetUp()

			selButton.Down()

		def __DonateNormal(self):
			type = self.GetType()

			net.SendGuildOfferPacket(type)
			net.GuildDonateInfoOpen()
			self.__OnCloseQuestionDialog()
			self.ClosePopUpDialog()
			#self.Close()

		def __DonateMiddle(self):
			type = self.GetType()

			net.SendGuildOfferPacket(type)
			net.GuildDonateInfoOpen()
			self.__OnCloseQuestionDialog()
			self.ClosePopUpDialog()
			#self.Close()

		def __DonateHigh(self):
			type = self.GetType()

			net.SendGuildOfferPacket(type)
			net.GuildDonateInfoOpen()
			self.__OnCloseQuestionDialog()
			self.ClosePopUpDialog()
			#self.Close()

		def ClosePopUpDialog(self):
			if self.popup:
				self.popup.Close()
			self.popup = None

		def __OnCloseQuestionDialog(self):
			if self.questionDialog:
				self.questionDialog.Close()
			self.questionDialog = None

		def OverInItemNormal(self, slotIndex):
			pos_x, pos_y = wndMgr.GetMousePosition()	
		
			self.toolTip.ClearToolTip()
			self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 90)
			self.toolTip.AddItemData(MATERIAL_STONE_ID+slotIndex,(0,0,0))

		def OverOutItemNormal(self):
			self.toolTip.Hide()

		def OnUpdate(self):
			self.RefreshGuildDonateInfo()
			#net.GuildDonateInfoOpen()

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetBoardWidth(self, width):
			self.board.SetSize(max(width + 50, 160), self.GetHeight())
			self.SetSize(max(width + 50, 160), self.GetHeight())
			self.UpdateRect()

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.inputValue.OnIMEReturn = event

		def SetCancelEvent(self, event):
			self.board.SetCloseEvent(event)
			self.cancelButton.SetEvent(event)
			self.inputValue.OnPressEscapeKey = event

		def GetType(self):
			return self.type

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			def OverInToolTipButton(self, arg):
				arglen = len(str(arg))
				pos_x, pos_y = wndMgr.GetMousePosition()

				self.toolTip.ClearToolTip()
				self.toolTip.SetThinBoardSize(11 * arglen)
				self.toolTip.SetToolTipPosition(pos_x + 50, pos_y + 50)
				self.toolTip.AppendTextLine(arg, 0xffffffff)
				self.toolTip.Show()
				self.ShowButtonToolTip = TRUE

			def OverOutToolTipButton(self):
				self.toolTip.Hide()
				self.ShowButtonToolTip = FALSE

		def EventProgress(self, event_type, arg) :
			if "mouse_click" == event_type :
				self.OverInToolTipButton(arg)
			elif "mouse_over_in" == event_type :
				self.OverInToolTipButton(arg)
			elif "mouse_over_out" == event_type :
				self.OverOutToolTipButton()
			else :
				print " uiGuild.py ::EventProgress : FALSE"

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

if app.ENABLE_GUILDRENEWAL_SYSTEM:
	class GuildWarScoreDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.isLoaded = 0
			self.__CreateDialog()
			self.MyGuildId = 0
			self.OppGuildId = 0
			self.isOpen = 0

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Open(self):
			self.__CreateDialog()
			self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2, 2)
			self.SetTop()
			self.Show()
			guildID = guild.GetGuildID()
			self.Guild1Mark.SetIndex(guildID)
			self.Guild1Mark.SetScale(1)
			self.MyGuildName.SetText(guild.GetGuildName())
			self.MyGuildId = guildID
			self.isOpen = 1

		def Close(self):
			self.Hide()
			self.isOpen = 0

		def __CreateDialog(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/GuildWindow_WarScoreBoard.py")

			except:
				import exception
				exception.Abort("GuildWindow_WarScoreBoard.__CreateDialog - LoadScript")

			#self.GetChild("Mainboard").SetCloseEvent(ui.__mem_func__(self.Close))
			self.Guild1Mark = self.GetChild("Guild1Mark")
			self.Guild2Mark = self.GetChild("Guild2Mark")

			self.Guild1SetButtonList = []
			self.Guild1SetButtonList.append(self.GetChild("Guild1SetButton1"))
			self.Guild1SetButtonList.append(self.GetChild("Guild1SetButton2"))
			self.Guild1SetButtonList.append(self.GetChild("Guild1SetButton3"))
			self.Guild1SetButtonList.append(self.GetChild("Guild1SetButton4"))
			self.Guild1SetButtonList.append(self.GetChild("Guild1SetButton5"))

			for i in xrange(5):
				self.Guild1SetButtonList[i].Disable()
				self.Guild1SetButtonList[i].Down()

			self.Guild2SetButtonList = []
			self.Guild2SetButtonList.append(self.GetChild("Guild2SetButton1"))
			self.Guild2SetButtonList.append(self.GetChild("Guild2SetButton2"))
			self.Guild2SetButtonList.append(self.GetChild("Guild2SetButton3"))
			self.Guild2SetButtonList.append(self.GetChild("Guild2SetButton4"))
			self.Guild2SetButtonList.append(self.GetChild("Guild2SetButton5"))

			for i in xrange(5):
				self.Guild2SetButtonList[i].Disable()
				self.Guild2SetButtonList[i].Down()

			self.MyGuildName = self.GetChild("GuildName1")
			self.OppGuildName = self.GetChild("GuildName2")
			self.MyGuildScore = self.GetChild("Guild1Score")
			self.OppGuildScore = self.GetChild("Guild2Score")

		def SetOppGuildName(self, OppGuildId):
			self.OppGuildName.SetText(guild.GetGuildName(OppGuildId))
			self.Guild2Mark.SetIndex(OppGuildId)
			self.Guild2Mark.SetScale(1)
			self.OppGuildId = OppGuildId
			self.MyGuildId = guild.GetGuildID()

		def SetWarPoint(self, gainGuildId, point, winpoint):
			if self.MyGuildId == gainGuildId:
				self.MyGuildScore.SetText(str(point))
			elif self.OppGuildId == gainGuildId:
				self.OppGuildScore.SetText(str(point))

			for i in xrange(winpoint):
				if self.MyGuildId == gainGuildId:
					self.Guild1SetButtonList[i].SetUp()
				elif self.OppGuildId == gainGuildId:
					self.Guild2SetButtonList[i].SetUp()

		def GuildWarEnd(self):
			self.MyGuildScore.SetText("")
			self.OppGuildScore.SetText("")
			self.OppGuildName.SetText("")

			for i in xrange(5):
				self.Guild2SetButtonList[i].SetUp()
				self.Guild2SetButtonList[i].Down()

			for i in xrange(5):
				self.Guild1SetButtonList[i].SetUp()
				self.Guild1SetButtonList[i].Down()
		
		def GetOpend(self):
			return self.isOpen

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

	class DeclearGuildWarSelectDialog(ui.ScriptWindow):
		def __init__(self, func):
			ui.ScriptWindow.__init__(self)
			self.isLoaded = 0
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buttontooltip = None
				self.ShowButtonToolTip = FALSE

			self.__CreateDialog()
			self.func = func

		def __del__(self):
			ui.ScriptWindow.__del__(self)
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buttontooltip = None
				self.ShowButtonToolTip = FALSE

		def Open(self):
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.Hide()
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if self.buttontooltip:
					self.buttontooltip.Hide()
					self.ShowButtonToolTip = FALSE

		def __CreateDialog(self):
			if self.isLoaded == 1:
				return
			self.isLoaded = 1

			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/declareguildwarselectdialog.py")

			except:
				import exception
				exception.Abort("DeclareGuildWarSelectDialog.__CreateDialog - LoadScript")

			self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.Close))

			getObject = self.GetChild
			self.WarTypeButtonList = []
			self.WarTypeButtonList.append(getObject("King_Button"))
			self.WarTypeButtonList.append(getObject("Die_Button"))
			self.WarTypeButtonList.append(getObject("Protect_Button"))
			self.WarTypeButtonList.append(getObject("Tiger_Button"))
			self.WarTypeButtonList.append(getObject("Defense_Button"))
			self.WarTypeButtonList.append(getObject("Time_Button"))

			for i in xrange(6):
				self.WarTypeButtonList[i].SetEvent(ui.__mem_func__(self.__OnTypeButtonClick),i)

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buttontooltip = uiToolTip.ToolTip()
				self.buttontooltip.ClearToolTip()
				self.WarTypeButtonList[0].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_NORMAL)
				self.WarTypeButtonList[0].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.WarTypeButtonList[1].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_DIE)
				self.WarTypeButtonList[1].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.WarTypeButtonList[2].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_FLAG)
				self.WarTypeButtonList[2].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.WarTypeButtonList[3].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_TIGER)
				self.WarTypeButtonList[3].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.WarTypeButtonList[4].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_DEFENSE)
				self.WarTypeButtonList[4].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.WarTypeButtonList[5].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_TYPE_TIME)
				self.WarTypeButtonList[5].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

		def __OnTypeButtonClick(self, index):
			self.func(index)
			self.Close()

		def __CreateGameTypeToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(5)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			return toolTip

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			def OverInToolTipButton(self, arg):
				arglen = len(str(arg))
				pos_x, pos_y = wndMgr.GetMousePosition()

				self.buttontooltip.ClearToolTip()
				self.buttontooltip.SetThinBoardSize(11 * arglen)
				self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
				self.buttontooltip.AppendTextLine(arg, 0xffffffff)
				self.buttontooltip.Show()
				self.ShowButtonToolTip = TRUE

			def OverOutToolTipButton(self):
				self.buttontooltip.Hide()
				self.ShowButtonToolTip = FALSE

			def ButtonToolTipProgress(self) :
				if self.ShowButtonToolTip :
					pos_x, pos_y = wndMgr.GetMousePosition()
					self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

			def OnUpdate(self):
				self.ButtonToolTipProgress()

class DeclareGuildWarDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.type=0
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.buttontooltip = None
			self.ShowButtonToolTip = FALSE

		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.buttontooltip = None
			self.ShowButtonToolTip = FALSE

	def Open(self):
		self.inputValue.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.buttontooltip:
				self.buttontooltip.Hide()
				self.ShowButtonToolTip = FALSE

		self.Hide()

	def __CreateDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/declareguildwardialog.py")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")

			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				self.board.SetCloseEvent(ui.__mem_func__(self.Close))

				self.warwinButtonList = []
				self.warwinButtonList.append(getObject("WarWinbutton1"))
				self.warwinButtonList.append(getObject("WarWinbutton2"))
				self.warwinButtonList.append(getObject("WarWinbutton3"))

				self.warscoreButtonList = []
				self.warscoreButtonList.append(getObject("WarScorebutton1"))
				self.warscoreButtonList.append(getObject("WarScorebutton2"))
				self.warscoreButtonList.append(getObject("WarScorebutton3"))

				self.wartimeButtonList = []
				self.wartimeButtonList.append(getObject("WarTimebutton1"))
				self.wartimeButtonList.append(getObject("WarTimebutton2"))
				self.wartimeButtonList.append(getObject("WarTimebutton3"))

				self.acceptButton = getObject("AcceptButton")
				self.cancelButton = getObject("CancelButton")

				self.inputSlot = getObject("InputSlot")
				self.inputValue = getObject("InputValue")

				self.wartypename = getObject("WarTypeName")
				self.warselectButton = getObject("WarTypeButton")

				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					self.buttontooltip = uiToolTip.ToolTip()
					self.buttontooltip.ClearToolTip()

					self.warselectButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_SELECT_WAR)
					self.warselectButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

					self.warwinButtonList[0].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_WIN_ONE)
					self.warwinButtonList[0].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.warwinButtonList[1].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_WIN_THREE)
					self.warwinButtonList[1].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.warwinButtonList[2].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_WIN_FIVE)
					self.warwinButtonList[2].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

					self.warscoreButtonList[0].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAINSCORE_THIRTY)
					self.warscoreButtonList[0].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.warscoreButtonList[1].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAINSCORE_FIFTY)
					self.warscoreButtonList[1].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.warscoreButtonList[2].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAINSCORE_HUNDRED)
					self.warscoreButtonList[2].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

					self.wartimeButtonList[0].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAMETIME_TEN)
					self.wartimeButtonList[0].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.wartimeButtonList[1].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAMETIME_THIRTY)
					self.wartimeButtonList[1].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
					self.wartimeButtonList[2].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILD_WAR_GAMETIME_SIXTY)
					self.wartimeButtonList[2].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			else:
				self.typeButtonList=[]
				self.typeButtonList.append(getObject("NormalButton"))
				self.typeButtonList.append(getObject("WarpButton"))
				self.typeButtonList.append(getObject("CTFButton"))

				self.acceptButton = getObject("AcceptButton")
				self.cancelButton = getObject("CancelButton")
				self.inputSlot = getObject("InputSlot")
				self.inputValue = getObject("InputValue")

				gameType=getObject("GameType")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - BindObject")

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			for i in xrange(3):
				self.warwinButtonList[i].SAFE_SetEvent(self.__OnClickTypeWarWin,i+1,self.warwinButtonList)
				self.warscoreButtonList[i].SAFE_SetEvent(self.__OnClickTypeWarWin,i+1,self.warscoreButtonList)
				self.wartimeButtonList[i].SAFE_SetEvent(self.__OnClickTypeWarWin,i+1,self.wartimeButtonList)

			self.SetAcceptEvent(ui.__mem_func__(self.__OnOK))
			self.SetCancelEvent(ui.__mem_func__(self.__OnCancel))

			self.warselectButton.SetEvent(ui.__mem_func__(self.__OnClickWarTypeSelectButton))

			self.__AllWarButtonSetUp()
			self.__NotUseWarButtonList(self.warscoreButtonList)
			self.__OnClickTypeWarWin(1,self.wartimeButtonList)
			self.__OnClickTypeWarWin(1,self.warwinButtonList)
		else:
			if constInfo.GUILD_WAR_TYPE_SELECT_ENABLE==0:
				gameType.Hide()

			self.typeButtonList[0].SAFE_SetEvent(self.__OnClickTypeButtonNormal)
			self.typeButtonList[1].SAFE_SetEvent(self.__OnClickTypeButtonWarp)
			self.typeButtonList[2].SAFE_SetEvent(self.__OnClickTypeButtonCTF)

			self.typeButtonList[0].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_NORMAL_TITLE, localeInfo.GUILDWAR_NORMAL_DESCLIST))
			self.typeButtonList[1].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_WARP_TITLE, localeInfo.GUILDWAR_WARP_DESCLIST))
			self.typeButtonList[2].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_CTF_TITLE, localeInfo.GUILDWAR_CTF_DESCLIST))

			self.__ClickRadioButton(self.typeButtonList, 0)

			self.SetAcceptEvent(ui.__mem_func__(self.__OnOK))
			self.SetCancelEvent(ui.__mem_func__(self.__OnCancel))

	def __OnOK(self):
		text = self.GetText()
		type = self.GetType()
		flag = 1

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			ErrorText = ""
			EnablePopup = 0

			if ""==text:
				ErrorText = localeInfo.GUILDWAR_INPUT_OTHERGUILD
				EnablePopup = 1

			guildmembercount, guildmembercountmax = guild.GetGuildMemberCount()

			if EnablePopup == 1:
				popup = uiCommon.PopupDialog()
				popup.SetText(ErrorText)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			net.SendChatPacket("/war %s %d %d %d %d %d" % (text, type, flag, self.wintype, self.scoretype, self.timetype))
		else:
			if ""==text:
				return

			net.SendChatPacket("/war %s %d" % (text, type))

		self.Close()
		return 1

	def __OnCancel(self):
		self.Close()
		return 1

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def __OnClickTypeWarWin(self, wartype, buttonlist):
			if buttonlist == self.warwinButtonList:
				self.wintype = wartype
			elif buttonlist == self.warscoreButtonList:
				self.scoretype = wartype
			elif buttonlist == self.wartimeButtonList:
				self.timetype = wartype

			try:
				selectButton = buttonlist[wartype-1]
			except IndexError:
				return

			for eachButton in buttonlist:
				eachButton.SetUp()

			selectButton.Down()

		def __NotUseWarButtonList(self, buttonlist):
			for eachButton in buttonlist:
				eachButton.Down()
				eachButton.SetTextColor(0)

		def __AllWarButtonSetUp(self):
			self.wintype = 0
			self.scoretype = 0
			self.timetype = 0

			for eachButton in self.warwinButtonList:
				eachButton.SetUp()
				eachButton.SetTextColor(0xffffffff)
			for eachButton in self.warscoreButtonList:
				eachButton.SetUp()
				eachButton.SetTextColor(0xffffffff)
			for eachButton in self.wartimeButtonList:
				eachButton.SetUp()
				eachButton.SetTextColor(0xffffffff)

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.inputValue.OnIMEReturn = event

		def SetCancelEvent(self, event):
			self.board.SetCloseEvent(event)
			self.cancelButton.SetEvent(event)
			self.inputValue.OnPressEscapeKey = event

		def GetType(self):
			return self.type

		def GetText(self):
			return self.inputValue.GetText()

		def SetGuildWarType(self, index):
			self.WAR_NAME = { 
				0 : localeInfo.GUILDWAR_NORMAL_TITLE, 
				1 : localeInfo.GUILDWAR_WARP_TITLE, 
				2 : localeInfo.GUILDWAR_CTF_TITLE, 
				3 : localeInfo.GUILDWAR_TYPE_TIGER,
				4 : localeInfo.GUILDWAR_TYPE_DEFENSE,
				5 : localeInfo.GUILDWAR_TYPE_TIME,
			}
			self.wartypename.SetText(self.WAR_NAME.get(index))
			self.type = index

			if index == 0 or index == 2 or index == 3 or index == 4:
				self.__AllWarButtonSetUp()
				self.__NotUseWarButtonList(self.warscoreButtonList)
				self.__OnClickTypeWarWin(1,self.warwinButtonList)
				self.__OnClickTypeWarWin(1,self.wartimeButtonList)
			elif index == 1:
				self.__AllWarButtonSetUp()
				self.__OnClickTypeWarWin(1,self.wartimeButtonList)
				self.__OnClickTypeWarWin(1,self.warwinButtonList)
				self.__OnClickTypeWarWin(1,self.warscoreButtonList)
			elif index == 5:
				self.__AllWarButtonSetUp()
				self.__NotUseWarButtonList(self.warscoreButtonList)
				self.__NotUseWarButtonList(self.wartimeButtonList)
				self.__OnClickTypeWarWin(1,self.warwinButtonList)
			else:
				self.__AllWarButtonSetUp()
				self.__OnClickTypeWarWin(1,self.warwinButtonList)
				self.__OnClickTypeWarWin(1,self.warscoreButtonList)
				self.__OnClickTypeWarWin(1,self.wartimeButtonList)

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

		def __OnClickWarTypeSelectButton(self):
			self.WarTpyeSelect = DeclearGuildWarSelectDialog(self.SetGuildWarType)
			self.WarTpyeSelect.Open()

		def __OnClosePopupDialog(self):
			self.popup = None	
	else:
		def __OnClickTypeButtonNormal(self):
			self.__ClickTypeRadioButton(0)

		def __OnClickTypeButtonWarp(self):
			self.__ClickTypeRadioButton(1)

		def __OnClickTypeButtonCTF(self):
			self.__ClickTypeRadioButton(2)

		def __ClickTypeRadioButton(self, type):
			self.__ClickRadioButton(self.typeButtonList, type)
			self.type=type

		def __ClickRadioButton(self, buttonList, buttonIndex):
			try:
				selButton=buttonList[buttonIndex]
			except IndexError:
				return

			for eachButton in buttonList:
				eachButton.SetUp()

			selButton.Down()

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetNumberMode(self):
			self.inputValue.SetNumberMode()

		def SetSecretMode(self):
			self.inputValue.SetSecret()

		def SetFocus(self):
			self.inputValue.SetFocus()

		def SetMaxLength(self, length):
			width = length * 6 + 10
			self.inputValue.SetMax(length)
			self.SetSlotWidth(width)
			self.SetBoardWidth(max(width + 50, 160))

		def SetSlotWidth(self, width):
			self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
			self.inputValue.SetSize(width, self.inputValue.GetHeight())

		def SetBoardWidth(self, width):
			self.board.SetSize(max(width + 50, 160), self.GetHeight())
			self.SetSize(max(width + 50, 160), self.GetHeight())
			self.UpdateRect()

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.inputValue.OnIMEReturn = event

		def SetCancelEvent(self, event):
			self.board.SetCloseEvent(event)
			self.cancelButton.SetEvent(event)
			self.inputValue.OnPressEscapeKey = event

		def GetType(self):
			return self.type

		def GetText(self):
			return self.inputValue.GetText()

		def __CreateGameTypeToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(5)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			return toolTip

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def OverInToolTipButton(self, arg):
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.buttontooltip.ClearToolTip()
			self.buttontooltip.SetThinBoardSize(11 * arglen)
			self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.buttontooltip.AppendTextLine(arg, 0xffffffff)
			self.buttontooltip.Show()
			self.ShowButtonToolTip = TRUE

		def OverOutToolTipButton(self):
			self.buttontooltip.Hide()
			self.ShowButtonToolTip = FALSE

		def ButtonToolTipProgress(self) :
			if self.ShowButtonToolTip :
				pos_x, pos_y = wndMgr.GetMousePosition()
				self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

		def OnUpdate(self):
			self.ButtonToolTipProgress()

class AcceptGuildWarDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.type=0
		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def Open(self, guildName, warType, winType, ScoreType, TimeType):
			self.guildName=guildName
			self.SetGuildWarTypeName(warType)
			self.SetGuildWarWinName(winType)
			self.SetGuildWarScoreName(ScoreType)
			self.SetGuildWarTimeName(TimeType)
			self.inputValue.SetText(guildName)
			self.SetCenterPosition()
			self.SetTop()
			self.Show()
			self.GuildWarTypes = { 0 : warType, 1 : winType, 2 : ScoreType, 3 : TimeType}
	else:
		def Open(self, guildName, warType):
			self.guildName=guildName
			self.warType=warType
			self.__ClickSelectedTypeRadioButton()
			self.inputValue.SetText(guildName)
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

	def GetGuildName(self):
		return self.guildName

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
			self.inputSlot = None
			self.inputValue = None
		self.Hide()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def GetGuildWarTypes(self):
			return self.GuildWarTypes
	else:
		def __ClickSelectedTypeRadioButton(self):
			self.__ClickTypeRadioButton(self.warType)

	def __CreateDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/acceptguildwardialog.py")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")

			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				self.acceptButton = getObject("AcceptButton")
				self.cancelButton = getObject("CancelButton")
				self.inputValue = getObject("InputValue")
				self.WarTypeName = getObject("WarTypeName")
				self.WarWinTypeName = getObject("WarWinTypeName")
				self.WarScoreName = getObject("WarScoreName")
				self.WarTimeName = getObject("WarTimeName")
			else:
				self.typeButtonList=[]
				self.typeButtonList.append(getObject("NormalButton"))
				self.typeButtonList.append(getObject("WarpButton"))
				self.typeButtonList.append(getObject("CTFButton"))

				self.acceptButton = getObject("AcceptButton")
				self.cancelButton = getObject("CancelButton")
				self.inputSlot = getObject("InputSlot")
				self.inputValue = getObject("InputValue")

				gameType=getObject("GameType")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - BindObject")

		if constInfo.GUILD_WAR_TYPE_SELECT_ENABLE==0:
			gameType.Hide()

		if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
			self.typeButtonList[0].SAFE_SetEvent(self.__OnClickTypeButtonNormal)
			self.typeButtonList[1].SAFE_SetEvent(self.__OnClickTypeButtonWarp)
			self.typeButtonList[2].SAFE_SetEvent(self.__OnClickTypeButtonCTF)

			self.typeButtonList[0].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_NORMAL_TITLE, localeInfo.GUILDWAR_NORMAL_DESCLIST))
			self.typeButtonList[1].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_WARP_TITLE, localeInfo.GUILDWAR_WARP_DESCLIST))
			self.typeButtonList[2].SetToolTipWindow(self.__CreateGameTypeToolTip(localeInfo.GUILDWAR_CTF_TITLE, localeInfo.GUILDWAR_CTF_DESCLIST))

			self.__ClickRadioButton(self.typeButtonList, 0)

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def SetGuildWarTypeName(self, type):
			self.WAR_NAME = { 
				0 : localeInfo.GUILDWAR_NORMAL_TITLE, 
				1 : localeInfo.GUILDWAR_WARP_TITLE, 
				2 : localeInfo.GUILDWAR_CTF_TITLE, 
				3 : localeInfo.GUILDWAR_TYPE_TIGER,
				4 : localeInfo.GUILDWAR_TYPE_DEFENSE,
				5 : localeInfo.GUILDWAR_TYPE_TIME,
			}
			self.WarTypeName.SetText(self.WAR_NAME.get(type))

		def SetGuildWarWinName(self, type):
			self.WIN_NAME = { 0 : "", 1 : localeInfo.GUILDWAR_WIN_ONE, 2 : localeInfo.GUILDWAR_WIN_THREE, 3 : localeInfo.GUILDWAR_WIN_FIVE,}
			self.WarWinTypeName.SetText(self.WIN_NAME.get(type))

		def SetGuildWarScoreName(self, type):
			self.SCORE_NAME = { 0 : "", 1 : localeInfo.GUILDWAR_GAINSCORE_THIRTY, 2 : localeInfo.GUILDWAR_GAINSCORE_FIFTY, 3 : localeInfo.GUILDWAR_GAINSCORE_HUNDRED, }
			self.WarScoreName.SetText(self.SCORE_NAME.get(type))

		def SetGuildWarTimeName(self, type):
			self.TIME_NAME = { 0 : "", 1 : localeInfo.GUILDWAR_GAMETIME_TEN, 2 : localeInfo.GUILDWAR_GAMETIME_THIRTY, 3 : localeInfo.GUILDWAR_GAMETIME_SIXTY, }
			self.WarTimeName.SetText(self.TIME_NAME.get(type))
	else:
		def __OnClickTypeButtonNormal(self):
			self.__ClickSelectedTypeRadioButton()

		def __OnClickTypeButtonWarp(self):
			self.__ClickSelectedTypeRadioButton()

		def __OnClickTypeButtonCTF(self):
			self.__ClickSelectedTypeRadioButton()

		def __ClickTypeRadioButton(self, type):
			self.__ClickRadioButton(self.typeButtonList, type)
			self.type=type

		def __ClickRadioButton(self, buttonList, buttonIndex):
			try:
				selButton=buttonList[buttonIndex]
			except IndexError:
				return

			for eachButton in buttonList:
				eachButton.SetUp()

			selButton.Down()

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetNumberMode(self):
			self.inputValue.SetNumberMode()

		def SetSecretMode(self):
			self.inputValue.SetSecret()

		def SetFocus(self):
			self.inputValue.SetFocus()

		def SetMaxLength(self, length):
			width = length * 6 + 10
			self.inputValue.SetMax(length)
			self.SetSlotWidth(width)
			self.SetBoardWidth(max(width + 50, 160))

		def SetSlotWidth(self, width):
			self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
			self.inputValue.SetSize(width, self.inputValue.GetHeight())

		def SetBoardWidth(self, width):
			self.board.SetSize(max(width + 50, 160), self.GetHeight())
			self.SetSize(max(width + 50, 160), self.GetHeight())
			self.UpdateRect()

		def GetType(self):
			return self.type

		def GetText(self):
			return self.inputValue.GetText()

		def __CreateGameTypeToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(5)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			return toolTip

	def SAFE_SetAcceptEvent(self, event):
		self.SetAcceptEvent(ui.__mem_func__(event))

	def SAFE_SetCancelEvent(self, event):
		self.SetCancelEvent(ui.__mem_func__(event))

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetType(self):
		return self.type

	def GetText(self):
		return self.inputValue.GetText()

	def __CreateGameTypeToolTip(self, title, descList):
		toolTip = uiToolTip.ToolTip()
		toolTip.SetTitle(title)
		toolTip.AppendSpace(5)

		for desc in descList:
			toolTip.AutoAppendTextLine(desc)

		toolTip.AlignHorizonalCenter()
		return toolTip

class GuildWarScoreBoard(ui.ThinBoard):
	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.Initialize()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def Initialize(self):
		self.allyGuildID = 0
		self.enemyGuildID = 0
		self.allyDataDict = {}
		self.enemyDataDict = {}

	def Open(self, allyGuildID, enemyGuildID):
		self.allyGuildID = allyGuildID
		self.enemyGuildID = enemyGuildID

		self.SetPosition(10, wndMgr.GetScreenHeight() - 100)

		mark = ui.MarkBox()
		mark.SetParent(self)
		mark.SetIndex(allyGuildID)
		mark.SetPosition(10, 10 + 18*0)
		mark.Show()
		scoreText = ui.TextLine()
		scoreText.SetParent(self)
		scoreText.SetPosition(30, 10 + 18*0)
		scoreText.SetHorizontalAlignLeft()
		scoreText.Show()
		self.allyDataDict["NAME"] = guild.GetGuildName(allyGuildID)
		self.allyDataDict["SCORE"] = 0
		self.allyDataDict["MEMBER_COUNT"] = -1
		self.allyDataDict["MARK"] = mark
		self.allyDataDict["TEXT"] = scoreText

		mark = ui.MarkBox()
		mark.SetParent(self)
		mark.SetIndex(enemyGuildID)
		mark.SetPosition(10, 10 + 18*1)
		mark.Show()
		scoreText = ui.TextLine()
		scoreText.SetParent(self)
		scoreText.SetPosition(30, 10 + 18*1)
		scoreText.SetHorizontalAlignLeft()
		scoreText.Show()
		self.enemyDataDict["NAME"] = guild.GetGuildName(enemyGuildID)
		self.enemyDataDict["SCORE"] = 0
		self.enemyDataDict["MEMBER_COUNT"] = -1
		self.enemyDataDict["MARK"] = mark
		self.enemyDataDict["TEXT"] = scoreText

		self.__RefreshName()
		self.Show()

	def __GetDataDict(self, ID):
		if self.allyGuildID == ID:
			return self.allyDataDict
		if self.enemyGuildID == ID:
			return self.enemyDataDict

		return None

	def SetScore(self, gainGuildID, opponetGuildID, point):
		dataDict = self.__GetDataDict(gainGuildID)
		if not dataDict:
			return
		dataDict["SCORE"] = point
		self.__RefreshName()

	def UpdateMemberCount(self, guildID1, memberCount1, guildID2, memberCount2):
		dataDict1 = self.__GetDataDict(guildID1)
		dataDict2 = self.__GetDataDict(guildID2)
		if dataDict1:
			dataDict1["MEMBER_COUNT"] = memberCount1
		if dataDict2:
			dataDict2["MEMBER_COUNT"] = memberCount2
		self.__RefreshName()

	def __RefreshName(self):
		nameMaxLen = max(len(self.allyDataDict["NAME"]), len(self.enemyDataDict["NAME"]))

		if -1 == self.allyDataDict["MEMBER_COUNT"] or -1 == self.enemyDataDict["MEMBER_COUNT"]:
			self.SetSize(30+nameMaxLen*6+8*5, 50)
			self.allyDataDict["TEXT"].SetText("%s %d" % (self.allyDataDict["NAME"], self.allyDataDict["SCORE"]))
			self.enemyDataDict["TEXT"].SetText("%s %d" % (self.enemyDataDict["NAME"], self.enemyDataDict["SCORE"]))

		else:
			self.SetSize(30+nameMaxLen*6+8*5+15, 50)
			self.allyDataDict["TEXT"].SetText("%s(%d) %d" % (self.allyDataDict["NAME"], self.allyDataDict["MEMBER_COUNT"], self.allyDataDict["SCORE"]))
			self.enemyDataDict["TEXT"].SetText("%s(%d) %d" % (self.enemyDataDict["NAME"], self.enemyDataDict["MEMBER_COUNT"], self.enemyDataDict["SCORE"]))

class MouseReflector(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.width = self.height = 0
		self.isDown = FALSE

	def __del__(self):
		ui.Window.__del__(self)

	def Down(self):
		self.isDown = TRUE

	def Up(self):
		self.isDown = FALSE

	def OnRender(self):
		if self.isDown:
			grp.SetColor(ui.WHITE_COLOR)
		else:
			grp.SetColor(ui.HALF_WHITE_COLOR)

		x, y = self.GetGlobalPosition()
		grp.RenderBar(x+2, y+2, self.GetWidth()-4, self.GetHeight()-4)

if app.ENABLE_GUILDRENEWAL_SYSTEM:
	class SelectTextSlot(ui.ImageBox):
		def __init__(self, parent, x, y, page):
			ui.ImageBox.__init__(self)
			self.SetParent(parent)
			self.SetPosition(x, y)
			self.LoadImage("d:/ymir work/ui/public/Parameter_Slot_100x18.sub")

			self.mouseReflector = MouseReflector(self)
			self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

			self.Enable = TRUE
			self.textLine = ui.MakeTextLine(self)
			self.event = lambda *arg: None
			self.arg = 0
			self.page = page
			self.Show()

			self.mouseReflector.UpdateRect()

		def __del__(self):
			ui.ImageBox.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def GetText(self):
			self.textLine.GetText()

		def SetEvent(self, event, arg):
			self.event = event
			self.arg = arg

		def Disable(self):
			self.Enable = FALSE

		def OnMouseOverIn(self):
			if not self.Enable:
				return
			self.mouseReflector.Show()

		def OnMouseOverOut(self):
			if not self.Enable:
				return
			self.mouseReflector.Hide()

		def OnMouseLeftButtonDown(self):
			if not self.Enable:
				return

			for line, slotList in self.page.memberDict.items():
				if self != slotList[0]:
					slotList[0].mouseReflector.Hide()
					slotList[0].Enable = TRUE

			self.mouseReflector.Down()
			self.Enable = FALSE
			self.page.Name = self.textLine.GetText()

		def OnMouseLeftButtonUp(self):
			if not self.Enable:
				return
			self.mouseReflector.Up()

class EditableTextSlot(ui.ImageBox):
	def __init__(self, parent, x, y):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.LoadImage("d:/ymir work/ui/public/Parameter_Slot_03.sub")
		else:
			self.LoadImage("d:/ymir work/ui/public/Parameter_Slot_02.sub")

		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

		self.Enable = TRUE
		self.textLine = ui.MakeTextLine(self)
		self.event = lambda *arg: None
		self.arg = 0
		self.Show()

		self.mouseReflector.UpdateRect()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetEvent(self, event, arg):
		self.event = event
		self.arg = arg

	def Disable(self):
		self.Enable = FALSE

	def OnMouseOverIn(self):
		if not self.Enable:
			return
		self.mouseReflector.Show()

	def OnMouseOverOut(self):
		if not self.Enable:
			return
		self.mouseReflector.Hide()

	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return
		self.mouseReflector.Down()

	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.mouseReflector.Up()
		self.event(self.arg)

class CheckBox(ui.ImageBox):
	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/public/Parameter_Slot_07.sub"):
			ui.ImageBox.__init__(self)
			self.SetParent(parent)
			self.SetPosition(x, y)
			self.LoadImage(filename)

			self.mouseReflector = MouseReflector(self)
			self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

			image = ui.MakeImageBox(self, "d:/ymir work/ui/public/check_image.sub", 0, 0)
			image.AddFlag("not_pick")
			image.SetWindowHorizontalAlignCenter()
			image.SetWindowVerticalAlignCenter()
			image.Hide()
			self.Enable = TRUE
			self.image = image
			self.event = event
			self.Show()
			self.mouseReflector.UpdateRect()
	else:
		def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/public/Parameter_Slot_01.sub"):
			ui.ImageBox.__init__(self)
			self.SetParent(parent)
			self.SetPosition(x, y)
			self.LoadImage(filename)

			self.mouseReflector = MouseReflector(self)
			self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

			image = ui.MakeImageBox(self, "d:/ymir work/ui/public/check_image.sub", 0, 0)
			image.AddFlag("not_pick")
			image.SetWindowHorizontalAlignCenter()
			image.SetWindowVerticalAlignCenter()
			image.Hide()
			self.Enable = TRUE
			self.image = image
			self.event = event
			self.Show()
			self.mouseReflector.UpdateRect()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetCheck(self, flag):
		if flag:
			self.image.Show()
		else:
			self.image.Hide()

	def Disable(self):
		self.Enable = FALSE

	def OnMouseOverIn(self):
		if not self.Enable:
			return
		self.mouseReflector.Show()

	def OnMouseOverOut(self):
		if not self.Enable:
			return
		self.mouseReflector.Hide()

	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return
		self.mouseReflector.Down()

	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.mouseReflector.Up()
		self.event()

class ChangeGradeNameDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self):
		self.gradeNameSlot.SetText("")
		self.gradeNameSlot.SetFocus()
		xMouse, yMouse = wndMgr.GetMousePosition()
		self.SetPosition(xMouse - self.GetWidth()/2, yMouse + 50)
		self.SetTop()
		self.Show()
	def Close(self):
		self.gradeNameSlot.KillFocus()
		self.Hide()
		return TRUE

	def SetGradeNumber(self, gradeNumber):
		self.gradeNumber = gradeNumber
	def GetGradeNumber(self):
		return self.gradeNumber
	def GetGradeName(self):
		return self.gradeNameSlot.GetText()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class CommentSlot(ui.Window):
	TEXT_LIMIT = 35

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def __init__(self, width, height):
			ui.Window.__init__(self)

			self.slotImage = ui.MakeSlotBar(self, 0, 0, width, height)
			self.slotImage.AddFlag("not_pick")

			self.slotSimpleText = ui.MakeTextLine(self)
			self.slotSimpleText.SetPosition(2, 0)
			if localeInfo.IsARABIC():
				self.slotSimpleText.SetWindowHorizontalAlignCenter()
				self.slotSimpleText.SetHorizontalAlignCenter()
			else:
				self.slotSimpleText.SetWindowHorizontalAlignLeft()
				self.slotSimpleText.SetHorizontalAlignLeft()

			self.bar = ui.SlotBar()
			self.bar.SetParent(self)
			self.bar.AddFlag("not_pick")
			self.bar.Hide()

			self.slotFullText = ui.MakeTextLine(self)
			self.slotFullText.SetPosition(2, 0)
			self.slotFullText.SetWindowHorizontalAlignLeft()
			self.slotFullText.SetHorizontalAlignLeft()

			if localeInfo.IsARABIC():
				self.slotFullText.SetParent(self.bar)
				self.slotFullText.SetWindowHorizontalAlignCenter()
				self.slotFullText.SetHorizontalAlignCenter()

			self.SetSize(self.slotImage.GetWidth(), self.slotImage.GetHeight())
			self.len = 0

			self.textwidthlimit = width
	else:
		def __init__(self):
			ui.Window.__init__(self)

			self.slotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/Parameter_Slot_06.sub", 0, 0)
			self.slotImage.AddFlag("not_pick")

			self.slotSimpleText = ui.MakeTextLine(self)
			self.slotSimpleText.SetPosition(2, 0)
			if localeInfo.IsARABIC():
				self.slotSimpleText.SetWindowHorizontalAlignCenter()
				self.slotSimpleText.SetHorizontalAlignCenter()
			else:
				self.slotSimpleText.SetWindowHorizontalAlignLeft()
				self.slotSimpleText.SetHorizontalAlignLeft()

			self.bar = ui.SlotBar()
			self.bar.SetParent(self)
			self.bar.AddFlag("not_pick")
			self.bar.Hide()

			self.slotFullText = ui.MakeTextLine(self)
			self.slotFullText.SetPosition(2, 0)
			self.slotFullText.SetWindowHorizontalAlignLeft()
			self.slotFullText.SetHorizontalAlignLeft()

			self.SetSize(self.slotImage.GetWidth(), self.slotImage.GetHeight())
			self.len = 0

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.slotFullText.SetText(text)
			width, height = self.slotFullText.GetTextSize()
			self.len = width
			self.slotFullText.SetText("")
			if self.len > self.textwidthlimit:
				lineCount = grpText.GetSplitingTextLineCount(text, 1)
				self.SplitingTextCount = 0
				for i in xrange(lineCount):
					temptext = grpText.GetSplitingTextLine(text, i, 0)
					self.slotFullText.SetText(temptext)
					width, height = self.slotFullText.GetTextSize()
					if width > self.textwidthlimit:
						self.SplitingTextCount = i
						break
				self.tempsize = len(text)
				limitText = grpText.GetSplitingTextLine(text, self.SplitingTextCount-3, 0)
				self.slotSimpleText.SetText(limitText + "...")
				self.bar.SetSize(self.len+10, 17)
			else:
				self.slotSimpleText.SetText(text)
		else:
			self.len = len(text)
			if len(text) > self.TEXT_LIMIT:
				limitText = grpText.GetSplitingTextLine(text, self.TEXT_LIMIT-3, 0)
				self.slotSimpleText.SetText(limitText + "...")
				self.bar.SetSize(self.len * 6 + 5, 17)
			else:
				self.slotSimpleText.SetText(text)

		self.slotFullText.SetText(text)
		self.slotFullText.SetPosition(2, 0)
		self.slotFullText.Hide()

	def OnMouseOverIn(self):
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.len > self.textwidthlimit:
				self.bar.Show()
				self.slotFullText.Show()
		else:
			if self.len > self.TEXT_LIMIT:
				self.bar.Show()
				self.slotFullText.Show()

	def OnMouseOverOut(self):
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.len > self.textwidthlimit:
				self.bar.Hide()
				self.slotFullText.Hide()
		else:
			if self.len > self.TEXT_LIMIT:
				self.bar.Hide()
				self.slotFullText.Hide()

class GuildWindow(ui.ScriptWindow):
	JOB_NAME = {	0 : localeInfo.JOB_WARRIOR,
			1 : localeInfo.JOB_ASSASSIN,
			2 : localeInfo.JOB_SURA,
			3 : localeInfo.JOB_SHAMAN, }

	GUILD_SKILL_PASSIVE_SLOT = 0
	GUILD_SKILL_ACTIVE_SLOT = 1
	GUILD_SKILL_AFFECT_SLOT = 2

	GRADE_SLOT_NAME = 0
	GRADE_ADD_MEMBER_AUTHORITY = 1
	GRADE_REMOVE_MEMBER_AUTHORITY = 2
	GRADE_NOTICE_AUTHORITY = 3
	GRADE_SKILL_AUTHORITY = 4

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		MEMBER_LINE_COUNT = 11
	else:
		MEMBER_LINE_COUNT = 13

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		GRADE_WAR_AUTHORITY = 5
		GRADE_BANK_AUTHORITY = 6

		EMPIRE_NAME = {
			net.EMPIRE_A : localeInfo.EMPIRE_A,
			net.EMPIRE_B : localeInfo.EMPIRE_B,
			net.EMPIRE_C : localeInfo.EMPIRE_C
		}

		EMPIRE_NAME_TO_MAPINDEX = {
			1 : localeInfo.EMPIRE_A,
			3 : localeInfo.EMPIRE_A,
			4 : localeInfo.EMPIRE_A,
			6 : localeInfo.EMPIRE_A,
			21 : localeInfo.EMPIRE_B,
			23 : localeInfo.EMPIRE_B,
			24 : localeInfo.EMPIRE_B,
			26 : localeInfo.EMPIRE_B,
			41 : localeInfo.EMPIRE_C,
			43 : localeInfo.EMPIRE_C,
			44 : localeInfo.EMPIRE_C,
			46 : localeInfo.EMPIRE_C
		}		

		MAP_NAME = {
			0 : "",
			1 : localeInfo.MAP_A1,
			3 : localeInfo.MAP_A3,
			4 : localeInfo.MAP_AG,
			6 : localeInfo.GUILD_VILLAGE_01,
			21 : localeInfo.MAP_B1,
			23 : localeInfo.MAP_B3,
			24 : localeInfo.MAP_BG,
			26 : localeInfo.GUILD_VILLAGE_02,
			41 : localeInfo.MAP_C1,
			43 : localeInfo.MAP_C3,
			44 : localeInfo.MAP_CG,
			46 : localeInfo.GUILD_VILLAGE_03
		}

		WAR_NAME = {
			0 : localeInfo.GUILDWAR_NORMAL_TITLE,
			1 : localeInfo.GUILDWAR_WARP_TITLE,
			2 : localeInfo.GUILDWAR_CTF_TITLE,
			3 : localeInfo.GUILDWAR_TYPE_TIGER,
			4 : localeInfo.GUILDWAR_TYPE_DEFENSE,
			5 : localeInfo.GUILDWAR_TYPE_TIME,
		}

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			SMELTER_NAME = {
				0 : "",
				1 : localeInfo.GUILD_DIAMANT_OVEN,
				2 : localeInfo.GUILD_FOSSIL_OVEN,
				3 : localeInfo.GUILD_COPPER_OVEN,
				4 : localeInfo.GUILD_SILVER_OVEN,
				5 : localeInfo.GUILD_GOLD_OVEN,
				6 : localeInfo.GUILD_JADE_OVEN,
				7 : localeInfo.GUILD_EBONY_OVEN,
				8 : localeInfo.GUILD_PEARL_OVEN,
				9 : localeInfo.GUILD_WHITEGOLD_OVEN,
				10 : localeInfo.GUILD_KRISTALL_OVEN,
				11 : localeInfo.GUILD_QUARZ_OVEN,
				12 : localeInfo.GUILD_TEARS_OVEN,
				13 : localeInfo.GUILD_SOULCRISTAL_OVEN,
				14 : localeInfo.GUILD_RUBIN_OVEN,
				15 : localeInfo.GUILD_GRANAT_OVEN,
				16 : localeInfo.GUILD_SMARAGD_OVEN,
				17 : localeInfo.GUILD_SAPHIR_OVEN,
				18 : localeInfo.GUILD_TURMALIN_OVEN,
			}

			GBLACKSMITH_NAME = {
				0 : "",
				1 : localeInfo.GUILD_WEAPON_BLACKSMITH,
				2 : localeInfo.GUILD_ARMOR_BLACKSMITH,
				3 : localeInfo.GUILD_JUWELER_BLACKSMITH,
			}

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		PLUS_WIDTH = 80
		PLUS_RIGHT_WIDTH = 40
		PLUS_LEFT_WIDTH = 40

	class PageWindow(ui.ScriptWindow):
		def __init__(self, parent, filename):
			ui.ScriptWindow.__init__(self)
			self.SetParent(parent)
			self.filename = filename

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def GetScriptFileName(self):
			return self.filename

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded=0
		self.__Initialize()
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pickDialog = None
		self.questionDialog = None
		self.offerDialog = None
		self.popupDialog = None
		self.moneyDialog = None
		self.changeGradeNameDialog = None
		self.popup = None

		self.popupMessage = None
		self.commentSlot = None
		self.currentPage = ""

		self.pageWindow = None
		self.tooltipSkill = None

		self.memberLinePos = 0
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.enemyGuildName = None
			self.GuildBonusList = []
			if app.ENABLE_GUILD_REQUEST:
				self.GuildLIstDialog = uiGuildList.GuildListDialog()
			self.buttontooltip = None
			self.ShowButtonToolTip = FALSE
		else:
			self.enemyGuildNameList = []

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.isGuildWarStart = 0
			self.warScoreDialog = GuildWarScoreDialog()

		if app.ENABLE_GUILD_DONATE_ATTENDANCE:
			self.DonateButtonToolTipList = [
				uiScriptLocale.GUILD_ATTENDANCE_LOCA1,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA2,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA3,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA4,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA5,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA6,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA7,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA8,
				uiScriptLocale.GUILD_ATTENDANCE_LOCA9
			]

	def Open(self):
		self.Show()
		self.SetTop()

		guildID = guild.GetGuildID()
		self.largeMarkBox.SetIndex(guildID)
		self.largeMarkBox.SetScale(3)

	def Close(self):
		self.__CloseAllGuildMemberPageGradeComboBox()
		self.offerDialog.Close()
		self.popupDialog.Hide()
		self.changeGradeNameDialog.Hide()
		self.tooltipSkill.Hide()
		self.Hide()

		self.pickDialog = None
		self.questionDialog = None
		self.popup = None
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.warScoreDialog = None

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.buttontooltip:
				self.buttontooltip.Hide()
				self.ShowButtonToolTip = FALSE

	def Destroy(self):
		self.ClearDictionary()

		if self.offerDialog:
			self.offerDialog.Destroy()

		if self.popupDialog:
			self.popupDialog.ClearDictionary()

		if self.changeGradeNameDialog:
			self.changeGradeNameDialog.ClearDictionary()

		if self.pageWindow:
			for window in self.pageWindow.values():
				window.ClearDictionary()

		self.__Initialize()

	def Show(self):
		if self.isLoaded==0:
			self.isLoaded=1
			if not app.ENABLE_GUILDRENEWAL_SYSTEM:
				self.__LoadWindow()

		self.RefreshGuildInfoPage()
		self.RefreshGuildBoardPage()
		self.RefreshGuildMemberPage()
		self.RefreshGuildSkillPage()
		self.RefreshGuildGradePage()
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.RefreshGuildBaseInfoPage()
			self.RefreshGuildBaseInfoPageBankGold()
			self.RefreshGuildWarInfoPage()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		global DISABLE_GUILD_SKILL
		try:
			pyScrLoader = ui.PythonScriptLoader()
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buttontooltip = uiToolTip.ToolTip()
				self.buttontooltip.ClearToolTip()

			pyScrLoader.LoadScriptFile(self, "uiscript/guildwindow.py")

			self.popupDialog = ui.ScriptWindow()
			pyScrLoader.LoadScriptFile(self.popupDialog, "UIScript/PopupDialog.py")

			self.changeGradeNameDialog = ChangeGradeNameDialog()
			pyScrLoader.LoadScriptFile(self.changeGradeNameDialog, "uiscript/changegradenamedialog.py")

			self.pageWindow = {
				"GUILD_INFO"	: self.PageWindow(self, "uiscript/guildwindow_guildinfopage.py"),
				"BOARD"			: self.PageWindow(self, "uiscript/guildwindow_boardpage.py"),
				"MEMBER"		: self.PageWindow(self, "uiscript/guildwindow_memberpage.py"),
				"BASE_INFO"		: self.PageWindow(self, "uiscript/guildwindow_baseinfopage.py"),
				"SKILL"			: self.PageWindow(self, "uiscript/guildwindow_guildskillpage.py"),
				"GRADE"			: self.PageWindow(self, "uiscript/guildwindow_gradepage.py"),
			}

			for window in self.pageWindow.values():
				pyScrLoader.LoadScriptFile(window, window.GetScriptFileName())

		except:
			import exception
			exception.Abort("GuildWindow.__LoadWindow.LoadScript")

		try:
			getObject = self.GetChild

			self.board = getObject("Board")
			self.pageName = {
				"GUILD_INFO"	: localeInfo.GUILD_TILE_INFO,		# Gildeninformationen
				"BOARD"			: localeInfo.GUILD_TILE_BOARD,		# Gildennotizen
				"MEMBER"		: localeInfo.GUILD_TILE_MEMBER,		# Mitglieder
				"BASE_INFO"		: localeInfo.GUILD_TILE_BASEINFO,	# Landinformationen
				"SKILL"			: localeInfo.GUILD_TILE_SKILL,		# Gildenfertigkeiten
				"GRADE"			: localeInfo.GUILD_TILE_GRADE,		# Rechteverwaltung
			}

			self.tabDict = {
				"GUILD_INFO"	: getObject("Tab_01"),
				"BOARD"			: getObject("Tab_02"),
				"MEMBER"		: getObject("Tab_03"),
				"BASE_INFO"		: getObject("Tab_04"),
				"SKILL"			: getObject("Tab_05"),
				"GRADE"			: getObject("Tab_06"),
			}
			self.tabButtonDict = {
				"GUILD_INFO"	: getObject("Tab_Button_01"),
				"BOARD"			: getObject("Tab_Button_02"),
				"MEMBER"		: getObject("Tab_Button_03"),
				"BASE_INFO"		: getObject("Tab_Button_04"),
				"SKILL"			: getObject("Tab_Button_05"),
				"GRADE"			: getObject("Tab_Button_06"),
			}

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.tabButtonDict["GUILD_INFO"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_INFO)
				self.tabButtonDict["GUILD_INFO"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.tabButtonDict["BOARD"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_BOARD)
				self.tabButtonDict["BOARD"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.tabButtonDict["MEMBER"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_MEMBER)
				self.tabButtonDict["MEMBER"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.tabButtonDict["BASE_INFO"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_LANDINFO)
				self.tabButtonDict["BASE_INFO"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.tabButtonDict["SKILL"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_SKILL)
				self.tabButtonDict["SKILL"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				self.tabButtonDict["GRADE"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_GRADE)
				self.tabButtonDict["GRADE"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))					

			## QuestionDialog
			self.popupMessage = self.popupDialog.GetChild("message")
			self.popupDialog.GetChild("accept").SetEvent(ui.__mem_func__(self.popupDialog.Hide))

			## ChangeGradeName
			self.changeGradeNameDialog.GetChild("AcceptButton").SetEvent(ui.__mem_func__(self.OnChangeGradeName))
			self.changeGradeNameDialog.GetChild("CancelButton").SetEvent(ui.__mem_func__(self.changeGradeNameDialog.Hide))
			self.changeGradeNameDialog.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.changeGradeNameDialog.Hide))
			self.changeGradeNameDialog.gradeNameSlot = self.changeGradeNameDialog.GetChild("GradeNameValue")
			self.changeGradeNameDialog.gradeNameSlot.OnIMEReturn = ui.__mem_func__(self.OnChangeGradeName)
			self.changeGradeNameDialog.gradeNameSlot.OnPressEscapeKey = ui.__mem_func__(self.changeGradeNameDialog.Close)

			## Comment
			self.commentSlot = self.pageWindow["BOARD"].GetChild("CommentValue")
			self.commentSlot.OnIMEReturn = ui.__mem_func__(self.OnPostComment)
			self.commentSlot.OnKeyDown = ui.__mem_func__(self.OnKeyDownInBoardPage)

			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				self.commentSlot.SetEscapeEvent(ui.__mem_func__(self.Close))

			## RefreshButton
			self.pageWindow["BOARD"].GetChild("RefreshButton").SetEvent(ui.__mem_func__(self.OnRefreshComments))

			## ScrollBar
			scrollBar = self.pageWindow["MEMBER"].GetChild("ScrollBar")
			scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollMemberLine))
			self.pageWindow["MEMBER"].scrollBar = scrollBar

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					for key, img in self.tabDict.items():
							img.LeftRightReverse()

		except:
			import exception
			exception.Abort("GuildWindow.__LoadWindow.BindObject")

		self.__MakeInfoPage()
		self.__MakeBoardPage()
		self.__MakeMemberPage()
		self.__MakeBaseInfoPage()
		self.__MakeSkillPage()
		self.__MakeGradePage()

		for page in self.pageWindow.values():
			page.UpdateRect()

		for key, btn in self.tabButtonDict.items():
			btn.SetEvent(ui.__mem_func__(self.SelectPage), key)

		if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
			self.tabButtonDict["BASE_INFO"].Disable()

		if DISABLE_GUILD_SKILL:
			self.tabButtonDict["SKILL"].Disable()

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.SetTitleColor(0xffffffff)
		self.SelectPage("GUILD_INFO")

		self.offerDialog = uiPickMoney.PickMoneyDialog()
		self.offerDialog.LoadDialog()
		self.offerDialog.SetMax(9)
		self.offerDialog.SetTitleName(localeInfo.GUILD_OFFER_EXP)
		self.offerDialog.SetAcceptEvent(ui.__mem_func__(self.OnOffer))

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def OverInToolTipButton(self, arg):
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()

			self.buttontooltip.ClearToolTip()
			self.buttontooltip.SetThinBoardSize(11 * arglen)
			self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
			self.buttontooltip.AppendTextLine(arg, 0xffffffff)
			self.buttontooltip.Show()
			self.ShowButtonToolTip = TRUE

		def OverOutToolTipButton(self):
			self.buttontooltip.Hide()
			self.ShowButtonToolTip = FALSE

		def ButtonToolTipProgress(self) :
			if self.ShowButtonToolTip :
				pos_x, pos_y = wndMgr.GetMousePosition()
				self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

	if app.ENABLE_GUILD_DONATE_ATTENDANCE:
		def __CreateGameTypeToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(5)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			return toolTip

		def OnUpdate(self):
			self.ButtonToolTipProgress()

	def __MakeInfoPage(self):
		page = self.pageWindow["GUILD_INFO"]

		try:
			page.nameSlot = page.GetChild("GuildNameValue")
			page.masterNameSlot = page.GetChild("GuildMasterNameValue")
			page.guildLevelSlot = page.GetChild("GuildLevelValue")
			page.curExpSlot = page.GetChild("CurrentExperienceValue")
			page.lastExpSlot = page.GetChild("LastExperienceValue")
			page.memberCountSlot = page.GetChild("GuildMemberCountValue")
			page.levelAverageSlot = page.GetChild("GuildMemberLevelAverageValue")
			page.uploadMarkButton = page.GetChild("UploadGuildMarkButton")
			page.uploadSymbolButton = page.GetChild("UploadGuildSymbolButton")
			page.declareWarButton = page.GetChild("DeclareWarButton")
			page.OfferButton = page.GetChild("OfferButton")
			page.guildListButton = page.GetChild("GuildListButton")
			if app.ENABLE_GUILD_DONATE_ATTENDANCE:
				page.DonateButton = page.GetChild("DonateButton")

			try:
				page.guildMoneySlot = page.GetChild("GuildMoneyValue")
			except KeyError:
				page.guildMoneySlot = None

			try:
				page.GetChild("DepositButton").SetEvent(ui.__mem_func__(self.__OnClickDepositButton))
				page.GetChild("WithdrawButton").SetEvent(ui.__mem_func__(self.__OnClickWithdrawButton))
			except KeyError:
				pass

			page.uploadMarkButton.SetEvent(ui.__mem_func__(self.__OnClickSelectGuildMarkButton))
			page.uploadSymbolButton.SetEvent(ui.__mem_func__(self.__OnClickSelectGuildSymbolButton))
			page.declareWarButton.SetEvent(ui.__mem_func__(self.__OnClickDeclareWarButton))
			page.GetChild("OfferButton").SetEvent(ui.__mem_func__(self.__OnClickOfferButton))

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.GetChild("GuildListButton").SetEvent(ui.__mem_func__(self.__OnClickGuildListButton))
				if app.ENABLE_GUILD_DONATE_ATTENDANCE:
					page.DonateButton.SetEvent(ui.__mem_func__(self.__OnClickDonateButton))
				self.enemyGuildName = page.GetChild("EnemyGuildName")

				yPos = 158
				for i in xrange(4):
					expbonusSlotImage = ui.MakeSlotBar(page, 188+self.PLUS_WIDTH/2, yPos, 167+self.PLUS_WIDTH/2, 17)
					expbonusSlot = ui.MakeTextLine(expbonusSlotImage)
					page.Children.append(expbonusSlotImage)
					page.Children.append(expbonusSlot)
					expbonusSlot.SetText(localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY)
					self.GuildBonusList.append(expbonusSlot)
					if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
						if localeInfo.IsARABIC():
							expbonusSlotImage.SetPosition(5,yPos)
					yPos = 165 + 20 + (26 * i)
			else:
				page.GetChild("EnemyGuildCancel1").Hide()
				page.GetChild("EnemyGuildCancel2").Hide()
				page.GetChild("EnemyGuildCancel3").Hide()
				page.GetChild("EnemyGuildCancel4").Hide()
				page.GetChild("EnemyGuildCancel5").Hide()
				page.GetChild("EnemyGuildCancel6").Hide()
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName1"))
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName2"))
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName3"))
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName4"))
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName5"))
				self.enemyGuildNameList.append(page.GetChild("EnemyGuildName6"))

			self.largeMarkBox = page.GetChild("LargeGuildMark")

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.declareWarButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_DECLARE)
				page.declareWarButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				page.uploadSymbolButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_SYMBOL)
				page.uploadSymbolButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				page.uploadMarkButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_MARK)
				page.uploadMarkButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				page.GetChild("OfferButton").SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_OFFER)
				page.GetChild("OfferButton").SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				page.GetChild("GuildListButton").SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_GUILDLIST)
				page.GetChild("GuildListButton").SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
				if app.ENABLE_GUILD_DONATE_ATTENDANCE:
					page.GetChild("DonateButton").SetToolTipWindow(self.__CreateGameTypeToolTip(uiScriptLocale.GUILD_ATTENDANCE_TITLE, self.DonateButtonToolTipList))

		except:
			import exception
			exception.Abort("GuildWindow.__MakeInfoPage")

		self.largeMarkBox.AddFlag("not_pick")

		self.markSelectDialog=uiUploadMark.MarkSelectDialog()
		self.markSelectDialog.SAFE_SetSelectEvent(self.__OnSelectMark)

		self.symbolSelectDialog=uiUploadMark.SymbolSelectDialog()
		self.symbolSelectDialog.SAFE_SetSelectEvent(self.__OnSelectSymbol)


	def __MakeBoardPage(self):
		i = 0
		lineStep = 20
		page = self.pageWindow["BOARD"]

		page.boardDict = {}

		for i in xrange(12):

			yPos = 25 + i * lineStep

			## NoticeMark
			noticeMarkImage = ui.MakeImageBox(page, "d:/ymir work/ui/game/guild/notice_mark.sub", 5, yPos+3)
			noticeMarkImage.Hide()
			page.Children.append(noticeMarkImage)

			## Name
			if localeInfo.IsARABIC():
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					nameSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 335, yPos)
				else:
					nameSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 255, yPos)
			else:
				nameSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 15, yPos)
			nameSlot = ui.MakeTextLine(nameSlotImage)
			page.Children.append(nameSlotImage)
			page.Children.append(nameSlot)

			## Delete Button
			if localeInfo.IsARABIC():
				deleteButton = ui.MakeButton(page, 3, yPos + 3, localeInfo.GUILD_DELETE, "d:/ymir work/ui/public/", "close_button_01.sub", "close_button_02.sub", "close_button_03.sub")
			else:
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					deleteButton = ui.MakeButton(page, 340 + self.PLUS_WIDTH, yPos + 3, localeInfo.GUILD_DELETE, "d:/ymir work/ui/public/", "close_button_01.sub", "close_button_02.sub", "close_button_03.sub")
				else:
					deleteButton = ui.MakeButton(page, 340, yPos + 3, localeInfo.GUILD_DELETE, "d:/ymir work/ui/public/", "close_button_01.sub", "close_button_02.sub", "close_button_03.sub")
			deleteButton.SetEvent(ui.__mem_func__(self.OnDeleteComment), i)
			page.Children.append(deleteButton)

			## Comment
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				commentSlot = CommentSlot(300, 17)
			else:
				commentSlot = CommentSlot()
			commentSlot.SetParent(page)
			if localeInfo.IsARABIC():
				commentSlot.SetPosition(25, yPos)
			else:
				commentSlot.SetPosition(114, yPos)
			commentSlot.Show()
			page.Children.append(commentSlot)

			boardSlotList = []
			boardSlotList.append(noticeMarkImage)
			boardSlotList.append(nameSlot)
			boardSlotList.append(commentSlot)
			page.boardDict[i] = boardSlotList

		## PostComment - Have to make this here for that fit tooltip's position.
		if localeInfo.IsARABIC():
			postCommentButton = ui.MakeButton(page, 3, 273, localeInfo.GUILD_COMMENT, "d:/ymir work/ui/game/taskbar/", "Send_Chat_Button_01.sub", "Send_Chat_Button_02.sub", "Send_Chat_Button_03.sub")
		else:
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				postCommentButton = ui.MakeButton(page, 337+self.PLUS_WIDTH, 273, localeInfo.GUILD_COMMENT, "d:/ymir work/ui/game/taskbar/", "Send_Chat_Button_01.sub", "Send_Chat_Button_02.sub", "Send_Chat_Button_03.sub")
			else:
				postCommentButton = ui.MakeButton(page, 337, 273, localeInfo.GUILD_COMMENT, "d:/ymir work/ui/game/taskbar/", "Send_Chat_Button_01.sub", "Send_Chat_Button_02.sub", "Send_Chat_Button_03.sub")
		postCommentButton.SetEvent(ui.__mem_func__(self.OnPostComment))
		page.Children.append(postCommentButton)

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			page.IndexID = page.GetChild("IndexID")
			page.IndexID.OnMouseOverIn = lambda arg = localeInfo.GUILD_BOARD_ID : self.OverInToolTipButton(arg)
			page.IndexID.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.IndexMessages = page.GetChild("IndexMessages")
			page.IndexMessages.OnMouseOverIn = lambda arg = localeInfo.GUILD_BOARD_TEXT : self.OverInToolTipButton(arg)
			page.IndexMessages.OnMouseOverOut = lambda : self.OverOutToolTipButton()

	def __MakeMemberPage(self):
		page = self.pageWindow["MEMBER"]

		from _weakref import proxy
		lineStep = 20
		page.memberDict = {}

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			page.Name = ""
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.GetChild("MemberOutButton").SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_OUTMEMBER)
				page.GetChild("MemberOutButton").SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			page.GetChild("MemberOutButton").SetEvent(ui.__mem_func__(self.__OnOutMember), proxy(page))	

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.GetChild("MasterChangeButton").SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_CHANGEMASTER)
				page.GetChild("MasterChangeButton").SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			page.GetChild("MasterChangeButton").SetEvent(ui.__mem_func__(self.__OnChangeMaster), proxy(page))	

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			page.GetChild("IndexName").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_NAME : self.OverInToolTipButton(arg)
			page.GetChild("IndexName").OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GetChild("IndexGrade").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_RANK : self.OverInToolTipButton(arg)
			page.GetChild("IndexGrade").OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GetChild("IndexJob").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_JOB : self.OverInToolTipButton(arg)
			page.GetChild("IndexJob").OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GetChild("IndexLevel").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_LEVEL : self.OverInToolTipButton(arg)
			page.GetChild("IndexLevel").OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GetChild("IndexOffer").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_SPECIFIC_GRAVITY : self.OverInToolTipButton(arg)
			page.GetChild("IndexOffer").OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GetChild("IndexGeneral").OnMouseOverIn = lambda arg = localeInfo.GUILD_MEMBER_KNIGHT : self.OverInToolTipButton(arg)
			page.GetChild("IndexGeneral").OnMouseOverOut = lambda : self.OverOutToolTipButton()

		for i in xrange(self.MEMBER_LINE_COUNT):
			inverseLineIndex = self.MEMBER_LINE_COUNT - i - 1
			yPos = 28 + inverseLineIndex*lineStep
			## Name
			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				nameSlot = SelectTextSlot(page, 10, yPos, proxy(page))
			else:
				nameSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 10, yPos)
				nameSlot = ui.MakeTextLine(nameSlotImage)
				page.Children.append(nameSlotImage)
			page.Children.append(nameSlot)

			## Grade
			gradeSlot = ui.ComboBox()
			gradeSlot.SetParent(page)
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				gradeSlot.SetPosition(101+10, yPos-1)
				gradeSlot.SetSize(61+self.PLUS_WIDTH/3, 18)
			else:
				gradeSlot.SetPosition(101, yPos-1)
				gradeSlot.SetSize(61, 18)
			gradeSlot.SetEvent(lambda gradeNumber, lineIndex=inverseLineIndex, argSelf=proxy(self): argSelf.OnChangeMemberGrade(lineIndex, gradeNumber))
			gradeSlot.Show()
			page.Children.append(gradeSlot)

			## Job
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				jobSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_03.sub", 170+self.PLUS_WIDTH/2-10, yPos)
			else:
				jobSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 170, yPos)
			jobSlot = ui.MakeTextLine(jobSlotImage)
			page.Children.append(jobSlotImage)
			page.Children.append(jobSlot)

			## Level
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				levelSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 210+self.PLUS_WIDTH, yPos)
			else:
				levelSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 210, yPos)
			levelSlot = ui.MakeTextLine(levelSlotImage)
			page.Children.append(levelSlotImage)
			page.Children.append(levelSlot)

			## Offer
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				offerSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 250+self.PLUS_WIDTH, yPos)
			else:
				offerSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 250, yPos)
			offerSlot = ui.MakeTextLine(offerSlotImage)
			page.Children.append(offerSlotImage)
			page.Children.append(offerSlot)

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					offerSlotImage.SetPosition(62+10,yPos)
					levelSlotImage.SetPosition(102+10,yPos)
					jobSlotImage.SetPosition(152,yPos)
					gradeSlot.SetPosition(242,yPos)
					nameSlot.SetPosition(332,yPos)

			## General Enable
			event = lambda argSelf=proxy(self), argIndex=inverseLineIndex: apply(argSelf.OnEnableGeneral, (argIndex,))
			if localeInfo.IsARABIC():
				generalEnableCheckBox = CheckBox(page, 22, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
			else:
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					generalEnableCheckBox = CheckBox(page, 297+self.PLUS_WIDTH, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
				else:
					generalEnableCheckBox = CheckBox(page, 297, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
			page.Children.append(generalEnableCheckBox)

			memberSlotList = []
			memberSlotList.append(nameSlot)
			memberSlotList.append(gradeSlot)
			memberSlotList.append(jobSlot)
			memberSlotList.append(levelSlot)
			memberSlotList.append(offerSlot)
			memberSlotList.append(generalEnableCheckBox)
			page.memberDict[inverseLineIndex] = memberSlotList

	def __MakeBaseInfoPage(self):
		page = self.pageWindow["BASE_INFO"]
		page.buildingDataDict = {}

		lineStep = 20
		GUILD_BUILDING_MAX_NUM = 7

		yPos = 95 + 35

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			page.worldSlot = page.GetChild("Base_world")
			page.townSlot = page.GetChild("Base_town")
			page.powerlevelSlot = page.GetChild("PowerLevel")
			page.smelterSlot = page.GetChild("smeltertype")
			page.factorySlot = page.GetChild("factorytype")
			page.banklevelSlot = page.GetChild("banklevel")
			page.banklistUseSlot = page.GetChild("banklistusechr")
			page.goldSlot = page.GetChild("guildgold")
			#page.todaygoldSlot = page.GetChild("todaygold")
			page.outgoldmember = page.GetChild("outgoldmember")
			page.categoryList = page.GetChild("LandscapeList")
			page.categoryScrollbar = page.GetChild("LandscapeScrollBar")
			page.goldoutbutton = page.GetChild("GuildGoldOutButton")
			page.goldinbutton = page.GetChild("GuildGoldinButton")
			#page.goldoutbutton.Hide()
			#page.goldinbutton.Hide()
			page.bankgoldinfobutton = page.GetChild("GuildBankGoldInfoButton")
			page.guildbaseabandonbutton = page.GetChild("GuildBaseAbandonButton")
			page.guildbaseauctionTaxInfo = page.GetChild("GuildBaseAuctionTaxInfo")

			page.categoryScrollbar.SetScrollEvent(ui.__mem_func__(self.__OnScrollsmelterList))
			page.goldinbutton.SetEvent(ui.__mem_func__(self.__OnGuildGoldInOutButton), 1)
			page.goldoutbutton.SetEvent(ui.__mem_func__(self.__OnGuildGoldInOutButton), 0)
			page.bankgoldinfobutton.SetEvent(ui.__mem_func__(self.__OnGuildBankGolInfoButton))
			page.guildbaseabandonbutton.SetEvent(ui.__mem_func__(self.__OnGuildBaseAbandonButton))

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					page.categoryList.SetPosition(0,1)
					page.categoryList.SetWindowHorizontalAlignCenter()
		else:
			for i in xrange(GUILD_BUILDING_MAX_NUM):
				nameSlotImage = ui.MakeSlotBar(page, 15, yPos, 78, 17)
				nameSlot = ui.MakeTextLine(nameSlotImage)
				page.Children.append(nameSlotImage)
				page.Children.append(nameSlot)
				nameSlot.SetText(localeInfo.GUILD_BUILDING_NAME)

				gradeSlotImage = ui.MakeSlotBar(page, 99, yPos, 26, 17)
				gradeSlot = ui.MakeTextLine(gradeSlotImage)
				page.Children.append(gradeSlotImage)
				page.Children.append(gradeSlot)
				gradeSlot.SetText(localeInfo.GUILD_BUILDING_GRADE)

				RESOURCE_MAX_NUM = 6
				for j in xrange(RESOURCE_MAX_NUM):
					resourceSlotImage = ui.MakeSlotBar(page, 131 + 29*j, yPos, 26, 17)
					resourceSlot = ui.MakeTextLine(resourceSlotImage)
					page.Children.append(resourceSlotImage)
					page.Children.append(resourceSlot)
					resourceSlot.SetText(localeInfo.GUILD_GEM)

				event = lambda *arg: None
				powerSlot = CheckBox(page, 308, yPos, event, "d:/ymir work/ui/public/Parameter_Slot_00.sub")
				page.Children.append(powerSlot)

				yPos += lineStep

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			page.bankgoldinfobutton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_BANKGOLDINFO)
			page.bankgoldinfobutton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			page.guildbaseabandonbutton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_BASEABANDON)
			page.guildbaseabandonbutton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			page.goldoutbutton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_GOLDOUT)
			page.goldoutbutton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			page.goldinbutton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_BUTTON_GOLDIN)
			page.goldinbutton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

	def __MakeSkillPage(self):
		page = self.pageWindow["SKILL"]

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			page.skillPoint = page.GetChild("Skill_Plus_Value")
			page.activeSlot = page.GetChild("Active_Skill_Slot_Table")
			page.gpGauge = page.GetChild("Dragon_God_Power_Gauge")
			page.gpValue = page.GetChild("Dragon_God_Power_Value")
			page.btnHealGSP = page.GetChild("Heal_GSP_Button")
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.guildwar_all_score_slot = page.GetChild("guildwar_all_score_slot")
				page.guildwar_all_score_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_ALL : self.OverInToolTipButton(arg)
				page.guildwar_all_score_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()

				page.guildwar_win_score_slot = page.GetChild("guildwar_win_score_slot")
				page.guildwar_win_score_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_WIN : self.OverInToolTipButton(arg)
				page.guildwar_win_score_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()

				page.guildwar_lose_score_slot = page.GetChild("guildwar_lose_score_slot")
				page.guildwar_lose_score_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_LOSE : self.OverInToolTipButton(arg)
				page.guildwar_lose_score_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()

				page.guildwar_draw_score_slot = page.GetChild("guildwar_draw_score_slot")
				page.guildwar_draw_score_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_DRAW : self.OverInToolTipButton(arg)
				page.guildwar_draw_score_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()

				page.guildwar_RadderPoint_slot = page.GetChild("guildwar_RadderPoint_slot")
				page.guildwar_RadderPoint_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_LADDER : self.OverInToolTipButton(arg)
				page.guildwar_RadderPoint_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()

				page.guildwar_Ranking_slot = page.GetChild("guildwar_Ranking_slot")
				page.guildwar_Ranking_slot.OnMouseOverIn = lambda arg = localeInfo.GUILDWAR_RANKING : self.OverInToolTipButton(arg)
				page.guildwar_Ranking_slot.OnMouseOverOut = lambda : self.OverOutToolTipButton()				

				page.btnHealGSP.ShowToolTip = lambda arg = localeInfo.GUILDWINDOW_BUTTON_HEALGSP : self.OverInToolTipButton(arg)
				page.btnHealGSP.HideToolTip = lambda : self.OverOutToolTipButton()

			page.activeSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			page.activeSlot.SetOverInItemEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OverInItem(slotNumber, type))
			page.activeSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			page.activeSlot.SetSelectItemSlotEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnPickUpGuildSkill(slotNumber, type))
			page.activeSlot.SetUnselectItemSlotEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnUseGuildSkill(slotNumber, type))
			page.activeSlot.SetPressedSlotButtonEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnUpGuildSkill(slotNumber, type))
			page.activeSlot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")
			page.btnHealGSP.SetEvent(ui.__mem_func__(self.__OnOpenHealGSPBoard))

			page.GuildWarList = page.GetChild("GuildWarList")
			page.GuildWarListName = page.GetChild("GuildWarListName")
			page.GuildWarAllRecode = page.GetChild("guildwar_all_score")
			page.GuildWarWinRecode = page.GetChild("guildwar_win_score")
			page.GuildWarLoseRecode = page.GetChild("guildwar_lose_score")
			page.GuildWarDrawRecode = page.GetChild("guildwar_draw_score")
			page.GuildWarScrollBar = page.GetChild("GuildWarScrollBar")
			page.GuildWarScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnGuildWarScroll))
			page.GuildWarRederPoint = page.GetChild("guildwar_RadderPoint")
			page.GuildWarRanking = page.GetChild("guildwar_Ranking")

			for i in xrange(6):
				page.GuildWarListName.InsertItem(i, "%s" % (self.WAR_NAME.get(i)))

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					page.skillPoint.SetPosition(48,0)
					page.gpGauge.SetPosition(280,73+7+5)
					page.GuildWarList.SetPosition(0,1)
					page.GuildWarList.SetWindowHorizontalAlignCenter()
		else:
			page.skillPoint = page.GetChild("Skill_Plus_Value")
			page.passiveSlot = page.GetChild("Passive_Skill_Slot_Table")
			page.activeSlot = page.GetChild("Active_Skill_Slot_Table")
			page.affectSlot = page.GetChild("Affect_Slot_Table")
			page.gpGauge = page.GetChild("Dragon_God_Power_Gauge")
			page.gpValue = page.GetChild("Dragon_God_Power_Value")
			page.btnHealGSP = page.GetChild("Heal_GSP_Button")

			page.activeSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			page.activeSlot.SetOverInItemEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OverInItem(slotNumber, type))
			page.activeSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			page.activeSlot.SetSelectItemSlotEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnPickUpGuildSkill(slotNumber, type))
			page.activeSlot.SetUnselectItemSlotEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnUseGuildSkill(slotNumber, type))
			page.activeSlot.SetPressedSlotButtonEvent(lambda slotNumber, type=self.GUILD_SKILL_ACTIVE_SLOT: self.OnUpGuildSkill(slotNumber, type))
			page.activeSlot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")
			page.passiveSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			page.passiveSlot.SetOverInItemEvent(lambda slotNumber, type=self.GUILD_SKILL_PASSIVE_SLOT: self.OverInItem(slotNumber, type))
			page.passiveSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			page.passiveSlot.SetPressedSlotButtonEvent(lambda slotNumber, type=self.GUILD_SKILL_PASSIVE_SLOT: self.OnUpGuildSkill(slotNumber, type))
			page.passiveSlot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")
			page.affectSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			page.affectSlot.SetOverInItemEvent(lambda slotNumber, type=self.GUILD_SKILL_AFFECT_SLOT: self.OverInItem(slotNumber, type))
			page.affectSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			page.btnHealGSP.SetEvent(ui.__mem_func__(self.__OnOpenHealGSPBoard))

		## Active
		for i in xrange(len(playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST)):
			slotIndex = page.activeSlot.GetStartIndex()+i
			skillIndex = playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST[i]
			page.activeSlot.SetSkillSlot(slotIndex, skillIndex, 0)
			page.activeSlot.SetCoverButton(slotIndex)
			page.activeSlot.RefreshSlot()
			guild.SetSkillIndex(slotIndex, len(playerSettingModule.PASSIVE_GUILD_SKILL_INDEX_LIST)+i)

	def __MakeGradePage(self):
		lineStep = 18
		page = self.pageWindow["GRADE"]

		page.gradeDict = {}

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			page.GradeNumber = page.GetChild("GradeNumber")
			page.GradeNumber.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_GRADENUMBER : self.OverInToolTipButton(arg)
			page.GradeNumber.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GradeName = page.GetChild("GradeName")
			page.GradeName.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_GRADENAME : self.OverInToolTipButton(arg)
			page.GradeName.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.InviteAuthority = page.GetChild("InviteAuthority")
			page.InviteAuthority.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_INVITEAUTHORITY : self.OverInToolTipButton(arg)
			page.InviteAuthority.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.DriveOutAuthority = page.GetChild("DriveOutAuthority")
			page.DriveOutAuthority.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_DRIVEOUTAUTHORITY : self.OverInToolTipButton(arg)
			page.DriveOutAuthority.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.NoticeAuthority = page.GetChild("NoticeAuthority")
			page.NoticeAuthority.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_NOTICEAUTHORITY : self.OverInToolTipButton(arg)
			page.NoticeAuthority.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.SkillAuthority = page.GetChild("SkillAuthority")
			page.SkillAuthority.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_SKILLAUTHORITY : self.OverInToolTipButton(arg)
			page.SkillAuthority.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.GuildWar = page.GetChild("GuildWar")
			page.GuildWar.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_GUILDWAR : self.OverInToolTipButton(arg)
			page.GuildWar.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			page.Bank = page.GetChild("Bank")
			page.Bank.OnMouseOverIn = lambda arg = localeInfo.GUILDWINDOW_GRADE_BANK : self.OverInToolTipButton(arg)
			page.Bank.OnMouseOverOut = lambda : self.OverOutToolTipButton()

			if localeInfo.IsARABIC():
				page.GetChild("GuildGradeTItle").SetPosition(self.GetWidth(),0)
				page.Bank.SetPosition(self.GetWidth()-4,4)
				page.GuildWar.SetPosition(self.GetWidth()-43,4)
				page.SkillAuthority.SetPosition(self.GetWidth()-82,4)
				page.NoticeAuthority.SetPosition(self.GetWidth()-121,4)
				page.DriveOutAuthority.SetPosition(self.GetWidth()-160,4)
				page.InviteAuthority.SetPosition(self.GetWidth()-199,4)
				page.GradeName.SetPosition(self.GetWidth()-265,4)
				page.GradeNumber.SetPosition(self.GetWidth()-343,4)

		for i in xrange(15):
			yPos = 22 + i*lineStep
			index = i+1

			## GradeNumber
			if localeInfo.IsARABIC():
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					gradeNumberSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 320, yPos)
				else:
					gradeNumberSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 310, yPos)
			else:
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					gradeNumberSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 14-20, yPos)
				else:
					gradeNumberSlotImage = ui.MakeImageBox(page, "d:/ymir work/ui/public/Parameter_Slot_00.sub", 14, yPos)
			gradeNumberSlot = ui.MakeTextLine(gradeNumberSlotImage)
			gradeNumberSlot.SetText(str(i+1))
			page.Children.append(gradeNumberSlotImage)
			page.Children.append(gradeNumberSlot)

			## GradeName
			if localeInfo.IsARABIC():
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					gradeNameSlot = EditableTextSlot(page, 221, yPos)
				else:
					gradeNameSlot = EditableTextSlot(page, 242, yPos)
			else:
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					gradeNameSlot = EditableTextSlot(page, 58-20, yPos)
				else:
					gradeNameSlot = EditableTextSlot(page, 58, yPos)
			gradeNameSlot.SetEvent(ui.__mem_func__(self.OnOpenChangeGradeName), index)
			page.Children.append(gradeNameSlot)

			## Invite Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<0: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					inviteAuthorityCheckBox = CheckBox(page, 180, yPos, event)
				else:
					inviteAuthorityCheckBox = CheckBox(page, 122+10, yPos, event)
			else:
				inviteAuthorityCheckBox = CheckBox(page, 122, yPos, event)
			page.Children.append(inviteAuthorityCheckBox)

			## DriveOut Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<1: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					driveoutAuthorityCheckBox = CheckBox(page, 141, yPos, event)
				else:
					driveoutAuthorityCheckBox = CheckBox(page, 162+10, yPos, event)
			else:
				driveoutAuthorityCheckBox = CheckBox(page, 162, yPos, event)
			page.Children.append(driveoutAuthorityCheckBox)

			## Notice Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<2: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					noticeAuthorityCheckBox = CheckBox(page, 102, yPos, event)
				else:
					noticeAuthorityCheckBox = CheckBox(page, 202+10, yPos, event)
			else:
				noticeAuthorityCheckBox = CheckBox(page, 205, yPos, event)
			page.Children.append(noticeAuthorityCheckBox)

			## Skill Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<3: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					skillAuthorityCheckBox = CheckBox(page, 63, yPos, event)
				else:
					skillAuthorityCheckBox = CheckBox(page, 242+10, yPos, event)
			else:
				skillAuthorityCheckBox = CheckBox(page, 245, yPos, event)
			page.Children.append(skillAuthorityCheckBox)

			## War Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<4: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					warAuthorityCheckBox = CheckBox(page, 24, yPos, event)
				else:
					warAuthorityCheckBox = CheckBox(page, 282+10, yPos, event)
			else:
				warAuthorityCheckBox = CheckBox(page, 280, yPos, event)
			page.Children.append(warAuthorityCheckBox)

			## Bank Authority
			event = lambda argSelf=proxy(self), argIndex=index, argAuthority=1<<5: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if localeInfo.IsARABIC():
					bankAuthorityCheckBox = CheckBox(page, -15, yPos, event)
				else:
					bankAuthorityCheckBox = CheckBox(page, 322+10, yPos, event)
			else:
				bankAuthorityCheckBox = CheckBox(page, 320, yPos, event)
			page.Children.append(bankAuthorityCheckBox)

			gradeSlotList = []
			gradeSlotList.append(gradeNameSlot)
			gradeSlotList.append(inviteAuthorityCheckBox)
			gradeSlotList.append(driveoutAuthorityCheckBox)
			gradeSlotList.append(noticeAuthorityCheckBox)
			gradeSlotList.append(skillAuthorityCheckBox)
			gradeSlotList.append(warAuthorityCheckBox)
			gradeSlotList.append(bankAuthorityCheckBox)
			page.gradeDict[index] = gradeSlotList

		masterSlotList = page.gradeDict[1]
		for slot in masterSlotList:
			slot.Disable()

	def CanOpen(self):
		return guild.IsGuildEnable()

	def Open(self):
		self.Show()
		self.SetTop()

		guildID = guild.GetGuildID()
		self.largeMarkBox.SetIndex(guildID)
		self.largeMarkBox.SetScale(3)

		if localeInfo.IsARABIC():
			self.largeMarkBox.SetPosition(self.largeMarkBox.GetWidth()+32,1)

		#if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
		#	net.SendRequestGuildInfos(guild.REQUEST_TYPE_OPEN)

		if "BOARD" == self.currentPage:
			if self.commentSlot:
				self.commentSlot.SetFocus()
				self.commentSlot.Show()

	def Close(self):
		self.__CloseAllGuildMemberPageGradeComboBox()
		self.offerDialog.Close()
		self.popupDialog.Hide()
		self.changeGradeNameDialog.Hide()
		self.Hide()

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.tooltipSkill.Hide()
		else:
			if self.tooltipSkill:
				self.tooltipSkill.Hide()

		self.pickDialog = None
		self.questionDialog = None
		self.moneyDialog = None

		if self.commentSlot:
			self.commentSlot.KillFocus()
			self.commentSlot.Hide()

	def Destroy(self):
		self.ClearDictionary()
		self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pickDialog = None
		self.questionDialog = None
		self.markSelectDialog = None
		self.symbolSelectDialog = None

		if self.offerDialog:
			self.offerDialog.Destroy()
			self.offerDialog = None

		if self.popupDialog:
			self.popupDialog.ClearDictionary()
			self.popupDialog = None

		if self.changeGradeNameDialog:
			self.changeGradeNameDialog.ClearDictionary()
			self.changeGradeNameDialog = None

		self.popupMessage = None
		self.commentSlot = None
		self.currentPage = ""

		if self.pageWindow:
			for window in self.pageWindow.values():
				window.ClearDictionary()

		self.pageWindow = None
		self.tooltipSkill = None
		self.moneyDialog = None

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.enemyGuildName = None
			if app.ENABLE_GUILD_REQUEST:
				if self.GuildLIstDialog:
					self.GuildLIstDialog.Destroy()
					del self.GuildLIstDialog
					self.GuildListDialog = None
		else:
			self.enemyGuildNameList = []

	def DeleteGuild(self):
		self.RefreshGuildInfoPage()
		self.RefreshGuildBoardPage()
		self.RefreshGuildMemberPage()
		self.RefreshGuildSkillPage()
		self.RefreshGuildGradePage()
		self.Hide()

	def SetSkillToolTip(self, tooltipSkill):
		self.tooltipSkill = tooltipSkill

	def SelectPage(self, arg):
		if "BOARD" == arg:
			self.OnRefreshComments()
			if self.commentSlot:
				self.commentSlot.SetFocus()
				self.commentSlot.Show()
		else:
			if self.commentSlot:
				self.commentSlot.KillFocus()
				self.commentSlot.Hide()

		self.currentPage = arg

		for key, btn in self.tabButtonDict.items():
			if arg != key:
				btn.SetUp()
		for key, img in self.tabDict.items():
			if arg == key:
				img.Show()
			else:
				img.Hide()
		for key, page in self.pageWindow.items():
			if arg == key:
				page.Show()
			else:
				page.Hide()
		self.board.SetTitleName(self.pageName[arg])
		self.__CloseAllGuildMemberPageGradeComboBox()

	def __CloseAllGuildMemberPageGradeComboBox(self):
		page = self.pageWindow["MEMBER"]

		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			page.Name =""

		for key, slotList in page.memberDict.items():
			slotList[1].CloseListBox()

	def RefreshGuildInfoPage(self):
		if self.isLoaded==0:
			return

		global DISABLE_DECLARE_WAR
		page = self.pageWindow["GUILD_INFO"]
		page.nameSlot.SetText(guild.GetGuildName())
		page.masterNameSlot.SetText(guild.GetGuildMasterName())
		page.guildLevelSlot.SetText(str(guild.GetGuildLevel()))
		if page.guildMoneySlot:
			page.guildMoneySlot.SetText(str(guild.GetGuildMoney()))

		curExp, lastExp = guild.GetGuildExperience()
		curExp *= 100
		lastExp *= 100
		page.curExpSlot.SetText(str(curExp))
		page.lastExpSlot.SetText(str(lastExp))

		curMemberCount, maxMemberCount = guild.GetGuildMemberCount()
		if maxMemberCount== 0xffff:
			page.memberCountSlot.SetText("%d / %s " % (curMemberCount, localeInfo.GUILD_MEMBER_COUNT_INFINITY))
		else:
			page.memberCountSlot.SetText("%d / %d" % (curMemberCount, maxMemberCount))

		page.levelAverageSlot.SetText(str(guild.GetGuildMemberLevelAverage()))

		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName == masterName:
			page.uploadMarkButton.Show()

			if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
				if DISABLE_DECLARE_WAR:
					page.declareWarButton.Hide()
				else:
					page.declareWarButton.Show()

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.uploadSymbolButton.Hide()
			else:
				if guild.HasGuildLand():
					page.uploadSymbolButton.Show()
				else:
					page.uploadSymbolButton.Hide()
		else:
			page.uploadMarkButton.Hide()
			if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
				page.declareWarButton.Hide()
			page.uploadSymbolButton.Hide()

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			name = guild.GetEnemyGuildName()
			if name:
				self.enemyGuildName.SetText(name)
			else:
				self.enemyGuildName.SetText(localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY)
		else:
			for i in xrange(guild.ENEMY_GUILD_SLOT_MAX_COUNT):
				name = guild.GetEnemyGuildName(i)
				nameTextLine = self.enemyGuildNameList[i]
				if name:
					nameTextLine.SetText(name)
				else:
					nameTextLine.SetText(localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY)

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			guildlevel = guild.GetGuildLevel()
			if guildlevel >= 5:
				self.GuildBonusList[0].SetText(localeInfo.GUILDWINDOW_INFO_LEVEL5)
			if guildlevel >= 10:
				self.GuildBonusList[1].SetText(localeInfo.GUILDWINDOW_INFO_LEVEL10)
			if guildlevel >= 15:
				self.GuildBonusList[2].SetText(localeInfo.GUILDWINDOW_INFO_LEVEL15)
			if guildlevel >= 20:
				self.GuildBonusList[3].SetText(localeInfo.GUILDWINDOW_INFO_LEVEL20)

	def __GetGuildBoardCommentData(self, index):
		commentID, chrName, comment = guild.GetGuildBoardCommentData(index)
		if 0==commentID:
			if ""==chrName:
				chrName=localeInfo.UI_NONAME
			if ""==comment:
				comment=localeInfo.UI_NOCONTENTS

		return commentID, chrName, comment

	def RefreshGuildBoardPage(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["BOARD"]

		self.BOARD_LINE_MAX_NUM = 12
		lineIndex = 0

		commentCount = guild.GetGuildBoardCommentCount()
		for i in xrange(commentCount):
			commentID, chrName, comment = self.__GetGuildBoardCommentData(i)

			if not comment:
				continue

			slotList = page.boardDict[lineIndex]

			if "!" == comment[0]:
				slotList[0].Show()
				slotList[1].SetText(chrName)
				slotList[2].SetText(comment[1:])
			else:
				slotList[0].Hide()
				slotList[1].SetText(chrName)
				slotList[2].SetText(comment)

			lineIndex += 1

		for i in xrange(self.BOARD_LINE_MAX_NUM - lineIndex):
			slotList = page.boardDict[lineIndex+i]
			slotList[0].Hide()
			slotList[1].SetText("")
			slotList[2].SetText("")

	def RefreshGuildMemberPage(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["MEMBER"]

		## ScrollBar
		count = guild.GetMemberCount()
		if count > self.MEMBER_LINE_COUNT:
			page.scrollBar.SetMiddleBarSize(float(self.MEMBER_LINE_COUNT) / float(count))
			page.scrollBar.Show()
		else:
			page.scrollBar.Hide()
		self.RefreshGuildMemberPageGradeComboBox()
		self.RefreshGuildMemberPageMemberList()
		if app.ENABLE_GUILDRENEWAL_SYSTEM:
			self.RefreshGuildMemberSelectBox()

	def RefreshGuildMemberPageMemberList(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["MEMBER"]

		for line, slotList in page.memberDict.items():
			gradeComboBox = slotList[1]
			gradeComboBox.Disable()

			if not guild.IsMember(line):
				slotList[0].SetText("")
				slotList[2].SetText("")
				slotList[3].SetText("")
				slotList[4].SetText("")
				slotList[5].SetCheck(FALSE)
				continue

			pid, name, grade, race, level, offer, general = self.GetMemberData(line)
			if pid < 0:
				continue

			job = chr.RaceToJob(race)

			guildExperienceSummary = guild.GetGuildExperienceSummary()

			offerPercentage = 0
			if guildExperienceSummary > 0:
				offerPercentage = int(float(offer) / float(guildExperienceSummary) * 100.0)

			slotList[0].SetText(name)
			slotList[2].SetText(self.JOB_NAME.get(job, "?"))
			slotList[3].SetText(str(level))
			slotList[4].SetText(str(offerPercentage) + "%")
			slotList[5].SetCheck(general)

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if line > 0:
					gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
				else:
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_INFO_MASTER_VALUE)
			else:
				gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if guild.GetGradeName(grade) == "Member":
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_BANKINFO_NAME)

			if 1 != grade:
				gradeComboBox.Enable()

	def RefreshGuildMemberPageGradeComboBox(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["MEMBER"]

		self.CAN_CHANGE_GRADE_COUNT = 15 - 1
		for key, slotList in page.memberDict.items():
			gradeComboBox = slotList[1]
			gradeComboBox.Disable()

			if not guild.IsMember(key):
				continue

			pid, name, grade, job, level, offer, general = self.GetMemberData(key)
			if pid < 0:
				continue

			gradeComboBox.ClearItem()
			for i in xrange(self.CAN_CHANGE_GRADE_COUNT):
				if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
					if guild.GetGradeName(i + 2) == "Member":
						gradeComboBox.InsertItem(i + 2, uiScriptLocale.GUILD_BANKINFO_NAME)
					else:
						gradeComboBox.InsertItem(i + 2, guild.GetGradeName(i + 2))
				else:
					gradeComboBox.InsertItem(i + 2, guild.GetGradeName(i + 2))

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if guild.GetGradeName(grade) == "Member":
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_BANKINFO_NAME)
				else:
					gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
			else:
				gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))

			if 1 != grade:
				gradeComboBox.Enable()

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def RefreshGuildMemberSelectBox(self):
			if self.isLoaded==0:
				return

			page = self.pageWindow["MEMBER"]
			page.Name = ""
			for line, slotList in page.memberDict.items():
				slotList[0].mouseReflector.Hide()
				slotList[0].Enable = TRUE

	def RefreshGuildSkillPage(self):
		if self.isLoaded==0:
			return

		page = self.pageWindow["SKILL"]

		curPoint, maxPoint = guild.GetDragonPowerPoint()
		maxPoint = max(maxPoint, 1)
		page.gpValue.SetText(str(curPoint) + " / " + str(maxPoint))

		percentage = (float(curPoint) / float(maxPoint) * 100) * (float(173) / float(95))
		page.gpGauge.SetPercentage(int(percentage), 100)

		skillPoint = guild.GetGuildSkillPoint()
		page.skillPoint.SetText(str(skillPoint))

		if app.ENABLE_GUILDRENEWAL_SYSTEM==0:
			page.passiveSlot.HideAllSlotButton()
		page.activeSlot.HideAllSlotButton()

		## Active
		for i in xrange(len(playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST)):
			slotIndex = page.activeSlot.GetStartIndex()+i
			skillIndex = playerSettingModule.ACTIVE_GUILD_SKILL_INDEX_LIST[i]
			skillLevel = guild.GetSkillLevel(slotIndex)
			skillMaxLevel = skill.GetSkillMaxLevel(skillIndex)

			page.activeSlot.SetSlotCount(slotIndex, skillLevel)

			if skillLevel <= 0:
				page.activeSlot.DisableCoverButton(slotIndex)
			else:
				page.activeSlot.EnableCoverButton(slotIndex)

			if skillPoint > 0:
				if skillLevel < skillMaxLevel:
					page.activeSlot.ShowSlotButton(slotIndex)

	def RefreshGuildGradePage(self):

		if self.isLoaded==0:
			return

		page = self.pageWindow["GRADE"]

		for key, slotList in page.gradeDict.items():
			name, authority = guild.GetGradeData(int(key))

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if key > 1:
					if name == "Member":
						slotList[self.GRADE_SLOT_NAME].SetText(uiScriptLocale.GUILD_BANKINFO_NAME)
					else:
						slotList[self.GRADE_SLOT_NAME].SetText(name)
				else:
					slotList[self.GRADE_SLOT_NAME].SetText(uiScriptLocale.GUILD_INFO_MASTER_VALUE)
			else:
				slotList[self.GRADE_SLOT_NAME].SetText(name)

			slotList[self.GRADE_ADD_MEMBER_AUTHORITY].SetCheck(authority & guild.AUTH_ADD_MEMBER)
			slotList[self.GRADE_REMOVE_MEMBER_AUTHORITY].SetCheck(authority & guild.AUTH_REMOVE_MEMBER)
			slotList[self.GRADE_NOTICE_AUTHORITY].SetCheck(authority & guild.AUTH_NOTICE)
			slotList[self.GRADE_SKILL_AUTHORITY].SetCheck(authority & guild.AUTH_SKILL)
			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				slotList[self.GRADE_WAR_AUTHORITY].SetCheck(authority & guild.AUTH_WAR)
				slotList[self.GRADE_BANK_AUTHORITY].SetCheck(authority & guild.AUTH_BANK)

	if app.ENABLE_GUILDRENEWAL_SYSTEM:
		def RefreshGuildBaseInfoPageBankGold(self):
			if self.isLoaded==0:
				return

			page = self.pageWindow["BASE_INFO"]
			gold, lastgolduseuser, lastbankuseuser = guild.GetBaseInfoBankGold()
			page.goldSlot.SetText(NumberToMoneyString(gold))
			page.banklistUseSlot.SetText(lastbankuseuser)
			page.outgoldmember.SetText(lastgolduseuser)

		def RefreshGuildWarInfoPage(self):
			if self.isLoaded == 0:
				return

			page = self.pageWindow["SKILL"]
			page.GuildWarList.ClearItem()

			winall = 0
			loseall = 0
			allall = 0
			drawall = 0

			for index in xrange(3):	#6
				win, lose, draw, all = guild.GetWarRecode(index)
				page.GuildWarList.InsertItem(index, localeInfo.GUILDWAR_RECODE_ALL % (all, win, lose, draw))

				winall = winall + win
				loseall = loseall + lose
				allall = allall + all
				drawall = drawall + draw

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.GuildWarAllRecode.SetText("%d" % (allall))
				page.GuildWarWinRecode.SetText("%d" % (winall))
				page.GuildWarLoseRecode.SetText("%d" % (loseall))
				page.GuildWarDrawRecode.SetText("%d" % (drawall))
			else:
				page.GuildWarAllRecode.SetText(localeInfo.GUILDWAR_ALL % (allall))
				page.GuildWarWinRecode.SetText(localeInfo.GUILDWAR_WIN % (winall))
				page.GuildWarLoseRecode.SetText(localeInfo.GUILDWAR_LOSE % (loseall))
				page.GuildWarDrawRecode.SetText(localeInfo.GUILDWAR_DRAW % (drawall))

			if page.GuildWarList.GetItemCount() <= 4:
				page.GuildWarScrollBar.Hide()
			else:
				page.GuildWarScrollBar.Show()

			ladder = 0
			ranking = 0

			ladder, ranking = guild.GetGuildLadderRanking()

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				page.GuildWarRederPoint.SetText("%d" % (ladder))
				page.GuildWarRanking.SetText("%d" % (ranking))
			else:
				page.GuildWarRederPoint.SetText(localeInfo.GUILDWAR_LADDER % (ladder))
				page.GuildWarRanking.SetText(localeInfo.GUILDWAR_RANKING % (ranking))

		def __OnGuildWarScroll(self):
			page = self.pageWindow["SKILL"]
			viewItemCount = page.GuildWarList.GetViewItemCount()
			itemCount = page.GuildWarList.GetItemCount()
			pos = page.GuildWarScrollBar.GetPos() * (itemCount-viewItemCount)
			page.GuildWarList.SetBasePos(int(pos))
			page.GuildWarListName.SetBasePos(int(pos))

		def RefreshGuildBaseInfoPage(self):
			if self.isLoaded==0:
				return

			page = self.pageWindow["BASE_INFO"]

			local = guild.GetBaseInfo()
			page.townSlot.SetText(self.MAP_NAME[local])

			if local == 0:
				page.worldSlot.SetText("")
			else:
				page.worldSlot.SetText(self.EMPIRE_NAME_TO_MAPINDEX[local])

			altarLv, alchemist, blacksmith, storageLv = guild.GetbuildingInfo()

			if altarLv == 0:	#Altar der Kraft LV.
				page.powerlevelSlot.SetText("")
			else:
				page.powerlevelSlot.SetText('%d' % altarLv)

			page.smelterSlot.SetText(self.SMELTER_NAME.get(alchemist,""))
			page.factorySlot.SetText(self.GBLACKSMITH_NAME.get(blacksmith,""))

			if storageLv == 0:
				page.banklevelSlot.SetText("")
			else:
				page.banklevelSlot.SetText('%d' % storageLv)

		def __OnScrollsmelterList(self):
			page = self.pageWindow["BASE_INFO"]
			viewItemCount = page.categoryList.GetViewItemCount()
			itemCount = page.categoryList.GetItemCount()
			pos = page.categoryScrollbar.GetPos() * (itemCount-viewItemCount)
			page.categoryList.SetBasePos(int(pos))

		def __OnGuildGoldInOutButton(self, inout):
			if guild.MainPlayerHasAuthority(guild.AUTH_BANK) == 0:
				self.OnCreatePopUp(localeInfo.GUILDBANK_NOTUSE)
				return

			if inout == 1:
				if guild.GetGuildMoney() > 2000000000:
					self.OnCreatePopUp(localeInfo.GUILDBANK_OVER_INCOME)
					return
			else:
				if guild.GetGuildMoney() < 0:
					self.OnCreatePopUp(localeInfo.GUILDBANK_DONT_MONEY_OUT)
					return

			if app.ENABLE_CHEQUE_SYSTEM:
				inoutDialog = uiPickETC.PickETCDialog()
			else:
				inoutDialog = uiPickMoney.PickMoneyDialog()

			inoutDialog.LoadDialog()
			inoutDialog.SetMax(9)
			if inout == 1:
				inoutDialog.SetTitleName(localeInfo.GUILDBANK_MONEYIN)
				inoutDialog.SetAcceptEvent(ui.__mem_func__(self.__OnGuildGoldInDialog))
				inoutDialog.Open(2000000000 - guild.GetGuildMoney(), 1)
				#inoutDialog.SetCloseEvent(ui.__mem_func__(self.__OnCloseGoldInOutWindow))
			else:
				inoutDialog.SetTitleName(localeInfo.GUILDBANK_MONEYOUT)
				inoutDialog.SetAcceptEvent(ui.__mem_func__(self.__OnGuildGoldOutDialog))
				inoutDialog.Open(guild.GetGuildMoney(), 1)
				#inoutDialog.SetCloseEvent(ui.__mem_func__(self.__OnCloseGoldInOutWindow))
			self.inoutDialog = inoutDialog

		def __OnGuildGoldInDialog(self, money):
			if player.GetMoney() < money:
				self.OnCreatePopUp(localeInfo.GUILDBANK_NOT_ENOUGH_MONEY)
				return

			net.SendGuildDepositMoneyPacket(money)

		def __OnGuildGoldOutDialog(self, money):
			if money > guild.GetGuildMoney():
				self.OnCreatePopUp(localeInfo.GUILDBANK_OVER_MONEY)
				return

			net.SendGuildWithdrawMoneyPacket(money)

		def __OnGuildBankGolInfoButton(self):
			if app.ENABLE_GUILDBANK_LOG:
				net.GuildBankInfoOpen()
			else:
				pass

		def __OnGuildBaseAbandonButton(self):
			if player.GetMainCharacterName() != guild.GetGuildMasterName():
				self.OnCreatePopUp(localeInfo.GUILD_USE_MASTER_ONLY)
				return

			if guild.HasGuildLand() == 0:
				self.OnCreatePopUp(localeInfo.GUILDLAND_DONTHAVE_LAND)
				return

			popupText1 = localeInfo.GUILDLAND_ABANDON_TEXT1
			popupText2 = localeInfo.GUILDLAND_ABANDON_TEXT2
			popupText3 = localeInfo.GUILDLAND_ABANDON_TEXT3
			self.OnCreateGuildPopUp2(popupText1, popupText2, popupText3, localeInfo.GUILDLAND_ABANDON, self.__OnGuildBaseAbandon)

		def __OnGuildBaseAbandon(self):
			net.SendGuildVoteLandAbndon(guild.GetGuildID())
			if self.popup:
				self.popup.Close()
				self.popup = None

		def OnCreatePopUp(self, Text):
			popup = uiCommon.PopupDialog()
			popup.SetText(Text)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup
		## GuildBaseInfo End

		def SetGuildWarType(self, index):
			self.inputDialog.SetGuildWarType(index)

		def OpenGuildScoreWindow(self):
			# if self.isGuildWarStart == 0:
				# return

			import chat
			chat.AppendChat(chat.CHAT_TYPE_INFO, "OpenGuildScoreWindow")

			if self.warScoreDialog.GetOpend() == 1:
				self.warScoreDialog.Close()
				return

			self.warScoreDialog.Open()

		def GuildWarOppGuildNameSetting(self, guildSelf, Oppguild):
			if guild.GetGuildID() != guildSelf:
				return

			self.warScoreDialog.SetOppGuildName(Oppguild)
			self.isGuildWarStart = 1

		def GuildWarScoreSetting(self, gainGuildID, guildOpp, point, winpoint):
			self.warScoreDialog.SetWarPoint(gainGuildID, point, winpoint)

		def GuildWarEnd(self):
			self.warScoreDialog.GuildWarEnd()
			self.isGuildWarStart = 0
			self.warScoreDialog.Close()

		def __OnOutMember(self, page):
			if guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER) == 0:
				self.OnCreatePopUp(localeInfo.GUILD_MEMBEROUT_AUTH)
				return
			if page.Name != "" :
				if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
					questionDialog = uiCommon.QuestionDialog()
					questionDialog.SetText(localeInfo.GUILD_MEMBEROUT_NOMAL)
					questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnOutMemberStart))
					questionDialog.SetCancelEvent(ui.__mem_func__(self.ClosePopUpDialog))
					questionDialog.Open()
					self.popup = questionDialog	
				else:
					popupText1 = localeInfo.GUILDVOTE_MEMBEROUT_TEXT1 % (page.Name)
					popupText2 = localeInfo.GUILDVOTE_MEMBEROUT_TEXT2
					self.OnCreateGuildPopUp(popupText1, popupText2, uiScriptLocale.GUILD_MEMBER_OUT_TITLEBAR, self.__OnOutMemberStart)
			elif page.Name == "" :
				self.OnCreatePopUp(localeInfo.GUILD_SELECT_ONE)

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			def ClosePopUpDialog(self):
				if self.popup:
					self.popup.Close()
				self.popup = None

		def __OnOutMemberStart(self):
			page = self.pageWindow["MEMBER"]
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if page.Name != "":
					net.SendGuildVoteMemberOut(page.Name)
					if self.popup:
						self.popup.Close()
						self.popup = None
				elif page.Name == "" :
					self.OnCreatePopUp(localeInfo.GUILD_SELECT_ONE)
			else:
				net.SendGuildVoteMemberOut(page.Name)
				if self.popup:
					self.popup.Close()
					self.popup = None

		def __OnChangeMaster(self, page):
			mainCharacterName = player.GetMainCharacterName()
			masterName = guild.GetGuildMasterName()
			
			if mainCharacterName != masterName:
				self.OnCreatePopUp(localeInfo.GUILD_USE_MASTER_ONLY)
				return

			if page.Name =="" :
				self.OnCreatePopUp(localeInfo.GUILD_SELECT_ONE)
				return

			popupText1 = localeInfo.GUILD_CHANGEMASTER_TEXT1 % (page.Name)
			popupText2 = localeInfo.GUILD_CHANGEMASTER_TEXT2
			self.OnCreateGuildPopUp(popupText1, popupText2, uiScriptLocale.GUILD_MASTER_CHANGE_TITLEBAR, self.__OnAcceptEventChangeMaster)

		def __OnAcceptEventChangeMaster(self):
			popupText1 = localeInfo.GUILD_CHANGEMASTER_TEXT3
			popupText2 = localeInfo.GUILD_CHANGEMASTER_TEXT4
			self.OnCreateGuildPopUp(popupText1, popupText2, uiScriptLocale.GUILD_MASTER_CHANGE_TITLEBAR, self.__OnReQuestionEventChangeMaster)

		def __OnReQuestionEventChangeMaster(self):
			page = self.pageWindow["MEMBER"]
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				if page.Name != "":
					net.SendGuildVoteChangeMaster(page.Name)
					if self.popup:
						self.popup.Close()
						self.popup = None
				elif page.Name == "" :
					self.OnCreatePopUp(localeInfo.GUILD_SELECT_ONE)
			else:
				net.SendGuildVoteChangeMaster(page.Name)
				if self.popup:
					self.popup.Close()
					self.popup = None

		def OnCreateGuildPopUp(self, Text1, Text2, TitleBar, Event):
			if self.popup:
				self.popup.Close()
				self.popup = None
			popup = uiGuildPopup.GuildPopupDialog()
			popup.SetText1(Text1)
			popup.SetText2(Text2)
			popup.SetTitleBarText(TitleBar)
			popup.SetNormalButton()
			popup.SetAcceptEvent(Event)
			popup.Open()
			self.popup = popup

		def OnCreateGuildPopUp2(self, Text1, Text2, Text3, TitleBar, Event):
			if self.popup:
				self.popup.Close()
				self.popup = None
			popup = uiGuildPopup.GuildVoteDialog2()
			popup.SetText1(Text1)
			popup.SetText2(Text2)
			popup.SetText3(Text3)
			popup.SetTitleBarText(TitleBar)
			popup.SetNormalButton()
			popup.SetAcceptEvent(Event)
			popup.Open()
			self.popup = popup

		def OnCreatePopUp(self, Text):
			popup = uiCommon.PopupDialog()
			popup.SetText(Text)
			popup.SetAcceptEvent(self.__OnClosePopupDialog)
			popup.Open()
			self.popup = popup

	## GuildInfo
	def __PopupMessage(self, msg):
		self.popupMessage.SetText(msg)
		self.popupDialog.SetTop()
		self.popupDialog.Show()

	def __OnClickSelectGuildMarkButton(self):
		if guild.GetGuildLevel() < int(localeInfo.GUILD_MARK_MIN_LEVEL):
			self.__PopupMessage(localeInfo.GUILD_MARK_NOT_ENOUGH_LEVEL)
		elif not guild.MainPlayerHasAuthority(guild.AUTH_NOTICE):
			self.__PopupMessage(localeInfo.GUILD_NO_NOTICE_PERMISSION)
		else:
			self.markSelectDialog.Open()
			if app.ENABLE_GUILD_MARK_RENEWAL:
				self.__PopupMessage(localeInfo.GUILD_MARK_UPLOAD_NOTICE)

	def __OnClickSelectGuildSymbolButton(self):
		if guild.MainPlayerHasAuthority(guild.AUTH_NOTICE):
			self.symbolSelectDialog.Open()
		else:
			self.__PopupMessage(localeInfo.GUILD_NO_NOTICE_PERMISSION)

	def __OnClickDeclareWarButton(self):
		inputDialog = DeclareGuildWarDialog()
		inputDialog.Open()
		self.inputDialog = inputDialog

	def __OnSelectMark(self, markFileName):
		ret = net.UploadMark("upload/"+markFileName)

		# MARK_BUG_FIX
		if net.ERROR_MARK_UPLOAD_NEED_RECONNECT == ret:
			self.__PopupMessage(localeInfo.UPLOAD_MARK_UPLOAD_NEED_RECONNECT)

		return ret
		# END_OF_MARK_BUG_FIX

	def __OnSelectSymbol(self, symbolFileName):
		net.UploadSymbol("upload/"+symbolFileName)

	def __OnClickOfferButton(self):
		if app.ENABLE_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_DONATE_ATTENDANCE:
			if guild.GetGuildLevel() >= 20:
				self.offerDialogDonate = DonateGuildDialog()
				self.offerDialogDonate.Open()
			else:
				curEXP = unsigned32(player.GetStatus(player.EXP))
				if curEXP <= 100:
					self.__PopupMessage(localeInfo.GUILD_SHORT_EXP);
					return
				self.offerDialog.Open(curEXP, 100)
		else:
			curEXP = unsigned32(player.GetStatus(player.EXP))

			if curEXP <= 100:
				self.__PopupMessage(localeInfo.GUILD_SHORT_EXP);
				return

			self.offerDialog.Open(curEXP, 100)

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM and app.ENABLE_GUILD_REQUEST:
		def __OnClickGuildListButton(self):
			if self.GuildLIstDialog:
				self.GuildLIstDialog.Open()

		def OpenGuildListDialog(self):
			if self.GuildLIstDialog:
				self.GuildLIstDialog.Open()

		def CloseGuildListDialog(self):
			if self.GuildLIstDialog:
				self.GuildLIstDialog.Close()

		def GuildListDialogIsShow(self):
			if self.GuildLIstDialog:
				return self.GuildLIstDialog.IsShow()
	else:
		def __OnClickGuildListButton(self):
			pass

	if app.ENABLE_GUILD_DONATE_ATTENDANCE:
		def __OnClickDonateButton(self):
			net.SendChatPacket("/gdonate")

	def __OnClickDepositButton(self):
		moneyDialog = uiPickMoney.PickMoneyDialog()
		moneyDialog.LoadDialog()
		moneyDialog.SetMax(6)
		moneyDialog.SetTitleName(localeInfo.GUILD_DEPOSIT)
		moneyDialog.SetAcceptEvent(ui.__mem_func__(self.OnDeposit))
		moneyDialog.Open(player.GetMoney())
		self.moneyDialog = moneyDialog

	def __OnClickWithdrawButton(self):
		moneyDialog = uiPickMoney.PickMoneyDialog()
		moneyDialog.LoadDialog()
		moneyDialog.SetMax(6)
		moneyDialog.SetTitleName(localeInfo.GUILD_WITHDRAW)
		moneyDialog.SetAcceptEvent(ui.__mem_func__(self.OnWithdraw))
		moneyDialog.Open(guild.GetGuildMoney())
		self.moneyDialog = moneyDialog

	def __OnBlock(self):
		popup = uiCommon.PopupDialog()
		popup.SetText(localeInfo.NOT_YET_SUPPORT)
		popup.SetAcceptEvent(self.__OnClosePopupDialog)
		popup.Open()
		self.popup = popup

	def __OnClosePopupDialog(self):
		self.popup = None

	def OnDeposit(self, money):
		net.SendGuildDepositMoneyPacket(money)

	def OnWithdraw(self, money):
		net.SendGuildWithdrawMoneyPacket(money)

	def OnOffer(self, exp):
		net.SendGuildOfferPacket(exp)

	## Board
	def OnPostComment(self):
		text = self.commentSlot.GetText()
		if not text:
			return FALSE

		net.SendGuildPostCommentPacket(text[:50])
		self.commentSlot.SetText("")
		return TRUE

	def OnDeleteComment(self, index):
		commentID, chrName, comment = self.__GetGuildBoardCommentData(index)
		net.SendGuildDeleteCommentPacket(commentID)

	def OnRefreshComments(self):
		net.SendGuildRefreshCommentsPacket(0)

	def OnKeyDownInBoardPage(self, key):
		if key == 63:
			self.OnRefreshComments()
		return TRUE

	## Member
	## OnEnableGeneral
	def OnChangeMemberGrade(self, lineIndex, gradeNumber):
		PID = guild.MemberIndexToPID(lineIndex + self.memberLinePos)
		net.SendGuildChangeMemberGradePacket(PID, gradeNumber)

	def OnEnableGeneral(self, lineIndex):
		if not guild.IsMember(lineIndex):
			return

		pid, name, grade, job, level, offer, general = self.GetMemberData(lineIndex)
		if pid < 0:
			return

		net.SendGuildChangeMemberGeneralPacket(pid, 1 - general)

	## Grade
	def OnOpenChangeGradeName(self, arg):
		self.changeGradeNameDialog.SetGradeNumber(arg)
		self.changeGradeNameDialog.Open()

	def OnChangeGradeName(self):
		self.changeGradeNameDialog.Hide()
		gradeNumber = self.changeGradeNameDialog.GetGradeNumber()
		gradeName = self.changeGradeNameDialog.GetGradeName()

		if len(gradeName) == 0:
			gradeName = localeInfo.GUILD_DEFAULT_GRADE

		net.SendGuildChangeGradeNamePacket(gradeNumber, gradeName)
		return TRUE

	def OnCheckAuthority(self, argIndex, argAuthority):
		name, authority = guild.GetGradeData(argIndex)
		net.SendGuildChangeGradeAuthorityPacket(argIndex, authority ^ argAuthority)

	def OnScrollMemberLine(self):
		scrollBar = self.pageWindow["MEMBER"].scrollBar
		pos = scrollBar.GetPos()

		count = guild.GetMemberCount()
		newLinePos = int(float(count - self.MEMBER_LINE_COUNT) * pos)

		if newLinePos != self.memberLinePos:
			self.memberLinePos = newLinePos
			self.RefreshGuildMemberPageMemberList()
			self.__CloseAllGuildMemberPageGradeComboBox()
			if app.ENABLE_GUILDRENEWAL_SYSTEM:
				self.RefreshGuildMemberSelectBox()

	def GetMemberData(self, localPos):
		return guild.GetMemberData(localPos + self.memberLinePos)

	## Guild Skill
	def __OnOpenHealGSPBoard(self):
		curPoint, maxPoint = guild.GetDragonPowerPoint()

		if maxPoint - curPoint <= 0:
			self.__PopupMessage(localeInfo.GUILD_CANNOT_HEAL_GSP_ANYMORE)
			return

		pickDialog = uiPickMoney.PickMoneyDialog()
		pickDialog.LoadDialog()
		pickDialog.SetMax(9)
		pickDialog.SetTitleName(localeInfo.GUILD_HEAL_GSP)
		pickDialog.SetAcceptEvent(ui.__mem_func__(self.__OnOpenHealGSPQuestionDialog))
		pickDialog.Open(maxPoint - curPoint, 1)
		self.pickDialog = pickDialog

	def __OnOpenHealGSPQuestionDialog(self, healGSP):
		money = healGSP * constInfo.GUILD_MONEY_PER_GSP

		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.GUILD_DO_YOU_HEAL_GSP % (money, healGSP))
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.__OnHealGSP))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.__OnCloseQuestionDialog))
		questionDialog.SetWidth(400)
		questionDialog.Open()
		questionDialog.healGSP = healGSP
		self.questionDialog = questionDialog

	def __OnHealGSP(self):
		net.SendGuildChargeGSPPacket(self.questionDialog.healGSP)
		self.__OnCloseQuestionDialog()

	def __OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None

	def OnPickUpGuildSkill(self, skillSlotIndex, type):
		mouseController = mouseModule.mouseController

		if FALSE == mouseController.isAttached():
			skillIndex = player.GetSkillIndex(skillSlotIndex)
			skillLevel = guild.GetSkillLevel(skillSlotIndex)

			if skill.CanUseSkill(skillIndex) and skillLevel > 0:
				if app.IsPressed(app.DIK_LCONTROL):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_SKILL, skillSlotIndex)
					return

				mouseController.AttachObject(self, player.SLOT_TYPE_SKILL, skillSlotIndex, skillIndex)
		else:
			mouseController.DeattachObject()

	def OnUseGuildSkill(self, slotNumber, type):
		skillIndex = player.GetSkillIndex(slotNumber)
		skillLevel = guild.GetSkillLevel(slotNumber)

		if skillLevel <= 0:
			return

		player.UseGuildSkill(slotNumber)

	def OnUpGuildSkill(self, slotNumber, type):
		skillIndex = player.GetSkillIndex(slotNumber)
		net.SendChatPacket("/gskillup " + str(skillIndex))

	def OnUseSkill(self, slotNumber, coolTime):
		if not app.ENABLE_GUILDRENEWAL_SYSTEM:
			if self.isLoaded==0:
				return

		page = self.pageWindow["SKILL"]

		if page.activeSlot.HasSlot(slotNumber):
			page.activeSlot.SetSlotCoolTime(slotNumber, coolTime)

	def OnStartGuildWar(self, guildSelf, guildOpp):
		if self.isLoaded==0:
			return

		if guild.GetGuildID() != guildSelf:
			return

		guildName = guild.GetGuildName(guildOpp)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY == self.enemyGuildName.GetText():
				self.enemyGuildName.SetText(guildName)
		else:
			for guildNameTextLine in self.enemyGuildNameList:
				if localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY == guildNameTextLine.GetText():
					guildNameTextLine.SetText(guildName)
					return

	def OnEndGuildWar(self, guildSelf, guildOpp):
		if self.isLoaded==0:
			return

		if guild.GetGuildID() != guildSelf:
			return

		guildName = guild.GetGuildName(guildOpp)
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if guildName == self.enemyGuildName.GetText():
				self.enemyGuildName.SetText(localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY)
		else:
			for guildNameTextLine in self.enemyGuildNameList:
				if guildName == guildNameTextLine.GetText():
					guildNameTextLine.SetText(localeInfo.GUILD_INFO_ENEMY_GUILD_EMPTY)
					return

	## ToolTip
	def OverInItem(self, slotNumber, type):
		if mouseModule.mouseController.isAttached():
			return

		if None != self.tooltipSkill:
			skillIndex = player.GetSkillIndex(slotNumber)
			skillLevel = guild.GetSkillLevel(slotNumber)

			self.tooltipSkill.SetSkill(skillIndex, skillLevel)

	def OverOutItem(self):
		if None != self.tooltipSkill:
			self.tooltipSkill.HideToolTip()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def RefreshGuildRankingList(self, issearch):
			if self.GuildLIstDialog:
				self.GuildLIstDialog.RefreshGuildRankingList(issearch)
				
if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
	class BuildGuildBuildingChangeWindow(ui.ScriptWindow):
		SHOW_LIST_MAX = 10
		LIST_BOX_BASE_WIDHT = 165
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.IsOpen = FALSE
			self.ChangeBuildingList = None
			self.Scrollbar = None
			self.PositionButton = None
			self.DeleteButton = None
			self.AcceptButton = None
			self.CancelButton = None
			self.ctrlRotationZ = None
			self.PositionChangeMode = FALSE
			self.ChangePositionBaseX = 0
			self.ChangePositionBaseY = 0
			self.SelectObjectIndex = 0
			self.x = 0
			self.y = 0
			self.z = 0
			self.rot_x = 0
			self.rot_y = 0
			self.rot_z = 0
			self.rot_x_limit = 0
			self.rot_y_limit = 0
			self.rot_z_limit = 0
			self.BuildGuildBuildingWindow = None
			self.questionDialog = None
			self.emptyindex = 0
			self.selectindexlist = 0
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)
			self.IsOpen = FALSE
			self.ChangeBuildingList = None
			self.Scrollbar = None
			self.PositionButton = None
			self.DeleteButton = None
			self.AcceptButton = None
			self.CancelButton = None
			self.ctrlRotationZ = None
			self.PositionChangeMode = FALSE
			self.ChangePositionBaseX = 0
			self.ChangePositionBaseY = 0
			self.SelectObjectIndex = 0
			self.x = 0
			self.y = 0
			self.z = 0
			self.rot_x = 0
			self.rot_y = 0
			self.rot_z = 0
			self.rot_z_limit = 0
			self.questionDialog = None
			self.emptyindex = 0
			self.selectindexlist = 0

		def __LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/BuildGuildBuildingChangeWindow.py")

			except:
				import exception
				exception.Abort("BuildGuildBuildingChangeWindow.__LoadWindow - LoadScript")

			try:
				self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.Close))
				self.ChangeBuildingList = self.GetChild("ChangeBuildingList")
				self.ChangeBuildingList.SetEvent(ui.__mem_func__(self.SelectList))
				self.Scrollbar = self.GetChild("ChangeListScrollBar")
				self.Scrollbar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBuildingList))
				self.Scrollbar.Hide()
				self.PositionButton = self.GetChild("PositionButton")
				self.PositionButton.SetEvent(ui.__mem_func__(self.SelectObject))
				self.DeleteButton = self.GetChild("DeleteButton")
				self.DeleteButton.SetEvent(ui.__mem_func__(self.MakeQuestionDialog), localeInfo.GUILD_BUILDING_FIX_DELETE ,self.Delete)
				self.AcceptButton = self.GetChild("AcceptButton")
				self.AcceptButton.SetEvent(ui.__mem_func__(self.MakeQuestionDialog2), localeInfo.GUILD_BUILDING_FIX_CHANGE_GOLD, localeInfo.GUILD_BUILDING_FIX_CHANGE,self.Accept)
				self.CancelButton = self.GetChild("CancelButton")
				self.CancelButton.SetEvent(ui.__mem_func__(self.Close))
				self.ctrlRotationZ = self.GetChild("BuildingRotationZ")
				self.ctrlRotationZ.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
				self.ctrlRotationZ.SetSliderPos(0.5)
				if localeInfo.IsARABIC():
					self.ChangeBuildingList.SetPosition(-9,1)
					self.ChangeBuildingList.SetWindowHorizontalAlignCenter()

			except:
				import exception
				exception.Abort("BuildGuildBuildingWindow.__LoadWindow - BindObject")

		def Open(self):
			self.SetTop()
			self.Show()
			self.IsOpen = TRUE
			self.SetChangeBuildingList()
			if self.BuildGuildBuildingWindow:
				self.BuildGuildBuildingWindow.Hide()

		def Close(self):
			if not self.SelectObjectIndex == 0:
				objectx, objecty = guild.GetObjectXY(self.ChangeBuildingList.GetSelectedItem())
				rot_z = guild.GetObjectzRot(self.ChangeBuildingList.GetSelectedItem())
				chr.SelectInstance(int(self.SelectObjectIndex))
				chr.SetPixelPosition(int(objectx), int(objecty), int(self.z))
				chr.SetRotationAll(0, 0, rot_z)
			chr.DeleteInstance(self.emptyindex)
			self.RealClose()

		def DestoryWindow(self):
			self.BuildGuildBuildingWindow = None
			self.Close()
			self.ClearDictionary()
			self.IsOpen = FALSE
			self.ChangeBuildingList = None
			self.Scrollbar = None
			self.PositionButton = None
			self.DeleteButton = None
			self.AcceptButton = None
			self.CancelButton = None
			self.ctrlRotationZ = None
			self.PositionChangeMode = FALSE
			self.ChangePositionBaseX = 0
			self.ChangePositionBaseY = 0
			self.SelectObjectIndex = 0
			self.x = 0
			self.y = 0
			self.z = 0
			self.rot_x = 0
			self.rot_y = 0
			self.rot_z = 0
			self.rot_z_limit = 0
			self.questionDialog = None
			self.emptyindex = 0
			self.selectindexlist = 0

		def RealClose(self):
			self.Hide()
			self.IsOpen = FALSE
			if self.BuildGuildBuildingWindow:
				self.BuildGuildBuildingWindow.ChangeShow()
				self.BuildGuildBuildingWindow = None

		def AllClose(self):
			if not self.SelectObjectIndex == 0:
				objectx, objecty = guild.GetObjectXY(self.ChangeBuildingList.GetSelectedItem())
				rot_z = guild.GetObjectzRot(self.ChangeBuildingList.GetSelectedItem())
				chr.SelectInstance(int(self.SelectObjectIndex))
				chr.SetPixelPosition(int(objectx), int(objecty), int(self.z))
				chr.SetRotationAll(0, 0, rot_z)
			chr.DeleteInstance(self.emptyindex)
			self.Hide()
			self.IsOpen = FALSE
			if self.BuildGuildBuildingWindow:
				self.BuildGuildBuildingWindow.Close()

		def SetBuildGuildBuildingWindow(self, window):
			self.BuildGuildBuildingWindow = window
			self.emptyindex = self.BuildGuildBuildingWindow.START_INSTANCE_INDEX

		def __OnChangeRotation(self):
			self.rot_z = (self.ctrlRotationZ.GetSliderPos() * 360 + 180) % 360
			chr.SelectInstance(int(self.SelectObjectIndex))
			chr.SetRotationAll(self.rot_x, self.rot_y, self.rot_z)

		def Accept(self):
			net.SendChatPacket("/build u %d %d %d %d %d %d" % (self.SelectObjectIndex, int(self.x), int(self.y), self.rot_x, self.rot_y, self.rot_z))
			self.SelectObjectIndex = 0 
			self.CloseQuestionDialog()
			self.AllClose()

		def Delete(self):
			net.SendChatPacket("/build d %d" % self.SelectObjectIndex )
			self.SelectObjectIndex = 0
			self.CloseQuestionDialog()
			self.AllClose()

		def Update(self):
			if self.PositionChangeMode == TRUE:
				x, y, z = background.GetPickingPoint()
				self.x = x
				self.y = y
				self.z = z
				chr.SelectInstance(int(self.SelectObjectIndex))
				chr.SetPixelPosition(int(x), int(y), int(z))

		def CreateEmptyObject(self,race,objectx,objecty,rot_z):
			idx = self.emptyindex

			chr.DeleteInstance(idx)
			chr.CreateInstance(idx)
			chr.SelectInstance(idx)
			chr.SetVirtualID(idx)
			chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)
			chr.SetRace(race)
			chr.SetArmor(0)
			chr.Refresh()
			chr.SetLoopMotion(chr.MOTION_WAIT)
			chr.SetBlendRenderMode(idx, 0.55)
			chr.SetPixelPosition(int(objectx), int(objecty), int(self.z))
			chr.SetRotationAll(0.0, 0.0, rot_z)

		def SelectObject(self):
			ChangeBuilding = self.ChangeBuildingList.GetSelectedItem()

			objectx, objecty = guild.GetObjectXY(ChangeBuilding)
			rot_z = guild.GetObjectzRot(int(ChangeBuilding))
			self.CreateEmptyObject(guild.GetbuildingInfoChangeWIndow(ChangeBuilding),objectx,objecty,rot_z)

			idx = guild.GetObjectVid(int(ChangeBuilding))
			self.SelectObjectIndex = idx
			self.PositionChangeMode = TRUE
			self.Hide()

		def SetChangeBuildingList(self):
			self.ChangeBuildingList.ClearItem()
			self.ChangeBuildingList.SetBasePos(0)
			##scrollbar
			if guild.GetbuildingSize() > self.SHOW_LIST_MAX:
				self.ChangeBuildingList.SetSize(self.LIST_BOX_BASE_WIDHT, self.ChangeBuildingList.GetHeight())
				if localeInfo.IsARABIC():
					self.ChangeBuildingList.SetPosition(9,1)
				self.Scrollbar.SetMiddleBarSize(float(self.SHOW_LIST_MAX) /float(guild.GetbuildingSize()))
				self.Scrollbar.SetPos(0.0)
				self.Scrollbar.Show()
			else:
				self.ChangeBuildingList.SetSize(self.LIST_BOX_BASE_WIDHT +15, self.ChangeBuildingList.GetHeight())
				self.Scrollbar.Hide()

			index = 0
			for line in xrange(guild.GetbuildingSize()):
				for data in BUILDING_DATA_LIST:
					if data["VNUM"] == guild.GetbuildingInfoChangeWIndow(line):
						self.ChangeBuildingList.InsertItem(index, data["LOCAL_NAME"])
						index += 1
			self.ChangeBuildingList.SelectItem(0)
			self.SetCameraSetting(0)
			idx = guild.GetObjectVid(int(0))
			self.SelectObjectIndex = idx

		def __OnScrollBuildingList(self):
			itemCount = guild.GetbuildingSize()
			pos = self.Scrollbar.GetPos() * (itemCount-self.SHOW_LIST_MAX)
			self.ChangeBuildingList.SetBasePos(int(pos))

		def SelectList(self):
			if not self.selectindexlist == self.ChangeBuildingList.GetSelectedItem():
				objectx, objecty = guild.GetObjectXY(self.selectindexlist)
				rot_z = guild.GetObjectzRot(self.selectindexlist)
				chr.SelectInstance(int(self.SelectObjectIndex))
				chr.SetPixelPosition(int(objectx), int(objecty), int(self.z))
				chr.SetRotationAll(0, 0, rot_z)

			ChangeBuilding = self.ChangeBuildingList.GetSelectedItem()
			if ChangeBuilding >= len(BUILDING_DATA_LIST):
				return

			index = 0
			for data in BUILDING_DATA_LIST:
				if data["VNUM"] == guild.GetbuildingInfo(ChangeBuilding):
					self.__SetBuildingData(data)
				index += 1

			self.SetCameraSetting(int(ChangeBuilding))
			self.PositionChangeMode = FALSE
			idx = guild.GetObjectVid(int(ChangeBuilding))
			self.SelectObjectIndex = idx
			self.selectindexlist = ChangeBuilding
			self.rot_z = guild.GetObjectzRot(int(ChangeBuilding))

			if self.rot_z == 180:
				self.ctrlRotationZ.SetSliderPos(0.0)
			elif self.rot_z > 180:
				sliderpos = (self.rot_z - 180.0) / 360.0
				self.ctrlRotationZ.SetSliderPos(sliderpos)
			elif self.rot_z < 180:
				sliderpos = (360.0+self.rot_z - 180.0)  / 360.0
				self.ctrlRotationZ.SetSliderPos(sliderpos)
			else:
				self.ctrlRotationZ.SetSliderPos(0.5)

		def __SetBuildingData(self, data):
			self.rot_z_limit = data["Z_ROT_LIMIT"]
			self.ctrlRotationZ.Enable()
			if 0 == self.rot_z_limit:
				self.ctrlRotationZ.Disable()
			self.ctrlRotationZ.SetSliderPos(0.5)

		def SetCameraSetting(self, index):
			objectx, objecty = guild.GetObjectXY(index)
			x, y, z = player.GetMainCharacterPosition()
			self.x = objectx
			self.y = objecty
			self.z = z
			app.SetCameraSetting(int(objectx), int(-objecty), int(z), 7000, 0, 30)

		def IsPositionChangeMode(self):
			return self.PositionChangeMode

		def EndPositionChangeMode(self):
			app.SetCameraSetting(int(self.x), int(-self.y), int(self.z), 7000, 0, 30)
			self.PositionChangeMode = FALSE
			self.Show()

		def IsOpen(self):
			return self.IsOpen

		def MakeQuestionDialog(self, str, acceptevent):
			questionDialog = uiCommon.QuestionDialog()
			questionDialog.SetText(str)
			questionDialog.SetAcceptEvent(ui.__mem_func__(acceptevent))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.CloseQuestionDialog))
			questionDialog.Open()
			self.questionDialog = questionDialog

		def MakeQuestionDialog2(self, str1, str2, acceptevent):
			questionDialog = uiCommon.QuestionDialog2()
			questionDialog.SetText1(str1)
			questionDialog.SetText2(str2)
			questionDialog.SetAcceptEvent(ui.__mem_func__(acceptevent))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.CloseQuestionDialog))
			questionDialog.Open()
			self.questionDialog = questionDialog

		def CloseQuestionDialog(self):
			if self.questionDialog:
				self.questionDialog.Close()
			self.questionDialog = None

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

class BuildGuildBuildingWindow(ui.ScriptWindow):
	GUILD_CATEGORY_LIST = (
			("HEADQUARTER", localeInfo.GUILD_HEADQUARTER),
			("FACILITY", 	localeInfo.GUILD_FACILITY),
			("OBJECT", 	localeInfo.GUILD_OBJECT),
		)

	MODE_VIEW = 0
	MODE_POSITIONING = 1
	MODE_PREVIEW = 2

	BUILDING_ALPHA = 0.55

	ENABLE_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	DISABLE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)
	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		BUILDINGLIST_BASE_WIDTH = 135

	START_INSTANCE_INDEX = 123450
	#WALL_SET_INSTANCE = 14105

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

		self.closeEvent = None
		self.popup = None
		self.mode = self.MODE_VIEW
		self.race = 0
		self.type = None
		self.x = 0
		self.y = 0
		self.z = 0
		self.rot_x = 0
		self.rot_y = 0
		self.rot_z = 0
		self.rot_x_limit = 0
		self.rot_y_limit = 0
		self.rot_z_limit = 0
		self.needMoney = 0
		self.needStoneCount = 0
		self.needLogCount = 0
		self.needPlywoodCount = 0
		if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
			self.needSmeltStoneCount = 0

		self.indexList = []
		self.raceList = []
		self.posList = []
		self.rotList = []

		index = 0
		for category in self.GUILD_CATEGORY_LIST:
			self.categoryList.InsertItem(index, category[1])
			index += 1

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.wndBuildingChangeWindow = None
			self.wndBuildingChangeWindow = BuildGuildBuildingChangeWindow()
			self.ChangeButton = None

	def __del__(self):
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.wndBuildingChangeWindow:
				self.wndBuildingChangeWindow.DestoryWindow()
				self.wndBuildingChangeWindow = None
			del self.wndBuildingChangeWindow
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buildguildbuildingwindow.py")

		except:
			import exception
			exception.Abort("DeclareGuildWarWindow.__CreateDialog - LoadScript")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.categoryList = getObject("CategoryList")
			self.buildingList = getObject("BuildingList")
			self.listScrollBar = getObject("ListScrollBar")
			self.positionButton = getObject("PositionButton")
			self.previewButton = getObject("PreviewButton")
			self.posValueX = getObject("BuildingPositionXValue")
			self.posValueY = getObject("BuildingPositionYValue")
			self.ctrlRotationX = getObject("BuildingRotationX")
			self.ctrlRotationY = getObject("BuildingRotationY")
			self.ctrlRotationZ = getObject("BuildingRotationZ")
			self.buildingPriceValue = getObject("BuildingPriceValue")
			self.buildingMaterialStoneValue = getObject("BuildingMaterialStoneValue")
			self.buildingMaterialLogValue = getObject("BuildingMaterialLogValue")
			self.buildingMaterialPlywoodValue = getObject("BuildingMaterialPlywoodValue")
			if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
				self.buildingMaterialSmeltStoneValue = getObject("BuildingMaterialSmeltStoneValue")

			self.positionButton.SetEvent(ui.__mem_func__(self.__OnSelectPositioningMode))
			self.previewButton.SetToggleDownEvent(ui.__mem_func__(self.__OnEnterPreviewMode))
			self.previewButton.SetToggleUpEvent(ui.__mem_func__(self.__OnLeavePreviewMode))
			self.ctrlRotationX.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.ctrlRotationY.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.ctrlRotationZ.SetEvent(ui.__mem_func__(self.__OnChangeRotation))
			self.listScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBuildingList))

			getObject("CategoryList").SetEvent(ui.__mem_func__(self.__OnSelectCategory))
			getObject("BuildingList").SetEvent(ui.__mem_func__(self.__OnSelectBuilding))
			getObject("AcceptButton").SetEvent(ui.__mem_func__(self.Build))
			getObject("CancelButton").SetEvent(ui.__mem_func__(self.Close))
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.ChangeButton = getObject("ChangeButton")
				self.ChangeButton.SetEvent(ui.__mem_func__(self.__ChangeWindowOpen))

		except:
			import exception
			exception.Abort("BuildGuildBuildingWindow.__LoadWindow - BindObject")

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def __ChangeWindowOpen(self):
			pass
			# if self.wndBuildingChangeWindow:
				# if not self.IsPreviewMode():
					# if guild.GetbuildingSize() != 0:
						# self.__DeleteInstance()
						# self.wndBuildingChangeWindow.SetBuildGuildBuildingWindow(self)
						# self.wndBuildingChangeWindow.Open()
					# else:
						# self.__PopupDialog(localeInfo.GUILD_BUILDING_FIX_NEED_BUILDING)

	def __CreateWallBlock(self, race, x, y, rot=0.0 ):
		idx = self.START_INSTANCE_INDEX + len(self.indexList)
		self.indexList.append(idx)
		self.raceList.append(race)
		self.posList.append((x, y))
		self.rotList.append(rot)
		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)
		chr.SetRotationAll(0.0, 0.0, rot)

		self.ctrlRotationX.SetSliderPos(0.5)
		self.ctrlRotationY.SetSliderPos(0.5)
		self.ctrlRotationZ.SetSliderPos(0.5)

	def __GetObjectSize(self, race):
		idx = self.START_INSTANCE_INDEX + 1000
		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		sx, sy, ex, ey = chr.GetBoundBoxOnlyXY(idx)
		chr.DeleteInstance(idx)
		return sx, sy, ex, ey

	def __GetBuildInPosition(self):
		zList = []
		zList.append( background.GetHeight(self.x+self.sxPos, self.y+self.syPos) )
		zList.append( background.GetHeight(self.x+self.sxPos, self.y+self.eyPos) )
		zList.append( background.GetHeight(self.x+self.exPos, self.y+self.syPos) )
		zList.append( background.GetHeight(self.x+self.exPos, self.y+self.eyPos) )
		zList.append( background.GetHeight(self.x+(self.exPos+self.sxPos)/2, self.y+(self.eyPos+self.syPos)/2) )
		zList.sort()
		return zList[3]

	def __CreateBuildInInstance(self,race):
		self.__DeleteInstance()

		object_base = race - race%10

		door_minX, door_minY, door_maxX, door_maxY = self.__GetObjectSize(object_base+4)
		corner_minX, corner_minY, corner_maxX, corner_maxY = self.__GetObjectSize(object_base+1)
		line_minX, line_minY, line_maxX, line_maxY = self.__GetObjectSize(object_base+2)
		line_width = line_maxX - line_minX
		line_width_half = line_width / 2

		X_SIZE_STEP = 2 * 2
		Y_SIZE_STEP = 8
		sxPos = door_maxX - corner_minX + (line_width_half*X_SIZE_STEP)
		exPos = -sxPos
		syPos = 0
		eyPos = -(corner_maxY*2 + line_width*Y_SIZE_STEP)

		self.sxPos = sxPos
		self.syPos = syPos
		self.exPos = exPos
		self.eyPos = eyPos

		z = self.__GetBuildInPosition()

		## Door
		self.__CreateWallBlock(object_base+4, 0.0, syPos)

		## Corner
		self.__CreateWallBlock(object_base+1, sxPos, syPos)
		self.__CreateWallBlock(object_base+1, exPos, syPos, 270.0)
		self.__CreateWallBlock(object_base+1, sxPos, eyPos, 90.0)
		self.__CreateWallBlock(object_base+1, exPos, eyPos,180.0 )

		## Line
		lineBlock = object_base+2
		line_startX = -door_maxX - line_minX - (line_width_half*X_SIZE_STEP)
		self.__CreateWallBlock(lineBlock, line_startX, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*1, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*2, eyPos)
		self.__CreateWallBlock(lineBlock, line_startX+line_width*3, eyPos)
		for i in xrange(X_SIZE_STEP):
			self.__CreateWallBlock(lineBlock, line_startX+line_width*(3+i+1), eyPos)
		for i in xrange(X_SIZE_STEP/2):
			self.__CreateWallBlock(lineBlock, door_minX - line_maxX - line_width*i, syPos)
			self.__CreateWallBlock(lineBlock, door_maxX - line_minX + line_width*i, syPos)
		for i in xrange(Y_SIZE_STEP):
			self.__CreateWallBlock(lineBlock, sxPos, line_minX + corner_minX - line_width*i, 90.0)
			self.__CreateWallBlock(lineBlock, exPos, line_minX + corner_minX - line_width*i, 90.0)

		self.SetBuildingPosition(int(self.x), int(self.y), self.__GetBuildInPosition())

	def __DeleteInstance(self):
		if not self.indexList:
			return

		for index in self.indexList:
			chr.DeleteInstance(index)

		self.indexList = []
		self.raceList = []
		self.posList = []
		self.rotList = []

	def __CreateInstance(self, race):
		self.__DeleteInstance()

		self.race = race

		idx = self.START_INSTANCE_INDEX
		self.indexList.append(idx)
		self.posList.append((0, 0))
		self.rotList.append(0)

		chr.CreateInstance(idx)
		chr.SelectInstance(idx)
		chr.SetVirtualID(idx)
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.Refresh()
		chr.SetLoopMotion(chr.MOTION_WAIT)
		chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)

		self.SetBuildingPosition(int(self.x), int(self.y), 0)
		self.ctrlRotationX.SetSliderPos(0.5)
		self.ctrlRotationY.SetSliderPos(0.5)
		self.ctrlRotationZ.SetSliderPos(0.5)

	def Build(self):

		if not self.__IsEnoughMoney():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MONEY)
			return
		if not self.__IsEnoughMaterialStone():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return
		if not self.__IsEnoughMaterialLog():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return
		if not self.__IsEnoughMaterialPlywood():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return
		if not self.__IsEnoughMaterialSmeltStone():
			self.__PopupDialog(localeInfo.GUILD_NOT_ENOUGH_MATERIAL)
			return

		## /build c vnum x y x_rot y_rot z_rot
		## /build d vnum
		if "BUILDIN" == self.type:
			for i in xrange(len(self.raceList)):
				race = self.raceList[i]
				xPos, yPos = self.posList[i]
				rot = self.rotList[i]
				net.SendChatPacket("/build c %d %d %d %d %d %d" % (race, int(self.x+xPos), int(self.y+yPos), self.rot_x, self.rot_y, rot))
		else:
			net.SendChatPacket("/build c %d %d %d %d %d %d" % (self.race, int(self.x), int(self.y), self.rot_x, self.rot_y, self.rot_z))

		self.Close()

	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		def ChangeShow(self):
			x, y, z = player.GetMainCharacterPosition()
			app.SetCameraSetting(int(x), int(-y), int(z), 3000, 0, 30)
			self.x = x
			self.y = y-500
			self.z = z
			self.categoryList.SelectItem(0)
			self.buildingList.SelectItem(0)
			self.SetTop()
			self.Show()

	def Open(self):
		x, y, z = player.GetMainCharacterPosition()
		app.SetCameraSetting(int(x), int(-y), int(z), 3000, 0, 30)

		background.VisibleGuildArea()

		self.x = x
		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			self.y = y-500
		else:
			self.y = y
		self.z = z
		self.categoryList.SelectItem(0)
		self.buildingList.SelectItem(0)
		self.SetTop()
		self.Show()
		self.__DisablePCBlocker()

		import debugInfo
		if debugInfo.IsDebugMode():
			self.categoryList.SelectItem(2)
			self.buildingList.SelectItem(0)

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			player.SetParalysis(True)

	def Close(self):
		self.__DeleteInstance()

		background.DisableGuildArea()

		self.Hide()
		self.__OnClosePopupDialog()
		self.__EnablePCBlocker()
		self.__UnlockCameraMoving()
		if self.closeEvent:
			self.closeEvent()

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			player.SetParalysis(False)

	def Destory(self):
		self.Close()

		self.ClearDictionary()
		self.board = None
		self.categoryList = None
		self.buildingList = None
		self.listScrollBar = None
		self.positionButton = None
		self.previewButton = None
		self.posValueX = None
		self.posValueY = None
		self.ctrlRotationX = None
		self.ctrlRotationY = None
		self.ctrlRotationZ = None
		self.buildingPriceValue = None
		self.buildingMaterialStoneValue = None
		self.buildingMaterialLogValue = None
		self.buildingMaterialPlywoodValue = None
		if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
			self.buildingMaterialSmeltStoneValue = None
		self.closeEvent = None

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			if self.wndBuildingChangeWindow:
				self.wndBuildingChangeWindow.Destory()

	def SetCloseEvent(self, event):
		self.closeEvent = event

	def __PopupDialog(self, text):
		popup = uiCommon.PopupDialog()
		popup.SetText(text)
		popup.SetAcceptEvent(self.__OnClosePopupDialog)
		popup.Open()
		self.popup = popup

	def __OnClosePopupDialog(self):
		self.popup = None

	def __EnablePCBlocker(self):
		chr.SetInstanceType(chr.INSTANCE_TYPE_BUILDING)

		for idx in self.indexList:
			chr.SetBlendRenderMode(idx, 1.0)

	def __DisablePCBlocker(self):
		chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)

		for idx in self.indexList:
			chr.SetBlendRenderMode(idx, self.BUILDING_ALPHA)

	def __OnSelectPositioningMode(self):
		if self.MODE_PREVIEW == self.mode:
			self.positionButton.SetUp()
			return

		self.mode = self.MODE_POSITIONING
		self.Hide()

	def __OnEnterPreviewMode(self):
		if self.MODE_POSITIONING == self.mode:
			self.previewButton.SetUp()
			return

		self.mode = self.MODE_PREVIEW
		self.positionButton.SetUp()
		self.__UnlockCameraMoving()
		self.__EnablePCBlocker()

	def __OnLeavePreviewMode(self):
		self.__RestoreViewMode()

	def __RestoreViewMode(self):
		self.__DisablePCBlocker()
		self.__LockCameraMoving()
		self.mode = self.MODE_VIEW
		self.positionButton.SetUp()
		self.previewButton.SetUp()

	def __IsEnoughMoney(self):
		if app.IsEnableTestServerFlag():
			return TRUE

		if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
			curMoney = guild.GetGuildMoney()
		else:
			curMoney = player.GetMoney()
		if curMoney < self.needMoney:
			return FALSE
		return TRUE

	def __IsEnoughMaterialStone(self):
		if app.IsEnableTestServerFlag():
			return TRUE

		curStoneCount = player.GetItemCountByVnum(MATERIAL_STONE_ID)
		if curStoneCount < self.needStoneCount:
			return FALSE
		return TRUE

	def __IsEnoughMaterialLog(self):
		if app.IsEnableTestServerFlag():
			return TRUE

		curLogCount = player.GetItemCountByVnum(MATERIAL_LOG_ID)
		if curLogCount < self.needLogCount:
			return FALSE
		return TRUE

	def __IsEnoughMaterialPlywood(self):
		if app.IsEnableTestServerFlag():
			return TRUE

		curPlywoodCount = player.GetItemCountByVnum(MATERIAL_PLYWOOD_ID)
		if curPlywoodCount < self.needPlywoodCount:
			return FALSE
		return TRUE

	if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
		def __IsEnoughMaterialSmeltStone(self):
			if app.IsEnableTestServerFlag():
				return TRUE

			curSmeltStoneCount = player.GetItemCountByVnum(MATERIAL_SMELT_STONE_ID)
			if curSmeltStoneCount < self.needSmeltStoneCount:
				return FALSE
			return TRUE

	def __OnSelectCategory(self):
		self.listScrollBar.SetPos(0.0)
		self.__RefreshItem()

	def __SetBuildingData(self, data):
		self.buildingPriceValue.SetText(NumberToMoneyString(data["PRICE"]))

		self.needMoney = int(data["PRICE"])

		materialList = data["MATERIAL"]
		self.needStoneCount = int(materialList[MATERIAL_STONE_INDEX])
		self.needLogCount = int(materialList[MATERIAL_LOG_INDEX])
		self.needPlywoodCount = int(materialList[MATERIAL_PLYWOOD_INDEX])
		# if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
			# if data["VNUM"] == 14085:
				# self.needSmeltStoneCount = int(materialList[MATERIAL_SMELT_STONE_INDEX])

		self.buildingMaterialStoneValue.SetText(materialList[MATERIAL_STONE_INDEX])
		self.buildingMaterialLogValue.SetText(materialList[MATERIAL_LOG_INDEX] )
		self.buildingMaterialPlywoodValue.SetText(materialList[MATERIAL_PLYWOOD_INDEX])
		# if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
			# if data["VNUM"] == 14085:
				# self.buildingMaterialSmeltStoneValue.SetText(materialList[MATERIAL_SMELT_STONE_INDEX])

		if self.__IsEnoughMoney():
			self.buildingPriceValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingPriceValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialStone():
			self.buildingMaterialStoneValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialStoneValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialLog():
			self.buildingMaterialLogValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialLogValue.SetPackedFontColor(self.DISABLE_COLOR)

		if self.__IsEnoughMaterialPlywood():
			self.buildingMaterialPlywoodValue.SetPackedFontColor(self.ENABLE_COLOR)
		else:
			self.buildingMaterialPlywoodValue.SetPackedFontColor(self.DISABLE_COLOR)

		if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
			if self.__IsEnoughMaterialSmeltStone():
				self.buildingMaterialSmeltStoneValue.SetPackedFontColor(self.ENABLE_COLOR)
			else:
				self.buildingMaterialSmeltStoneValue.SetPackedFontColor(self.DISABLE_COLOR)

		self.rot_x_limit = data["X_ROT_LIMIT"]
		self.rot_y_limit = data["Y_ROT_LIMIT"]
		self.rot_z_limit = data["Z_ROT_LIMIT"]
		self.ctrlRotationX.Enable()
		self.ctrlRotationY.Enable()
		self.ctrlRotationZ.Enable()
		if 0 == self.rot_x_limit:
			self.ctrlRotationX.Disable()
		if 0 == self.rot_y_limit:
			self.ctrlRotationY.Disable()
		if 0 == self.rot_z_limit:
			self.ctrlRotationZ.Disable()

	def __OnSelectBuilding(self):
		buildingIndex = self.buildingList.GetSelectedItem()
		if buildingIndex >= len(BUILDING_DATA_LIST):
			return

		categoryIndex = self.categoryList.GetSelectedItem()
		if categoryIndex >= len(self.GUILD_CATEGORY_LIST):
			return
		selectedType = self.GUILD_CATEGORY_LIST[categoryIndex][0]

		index = 0
		for data in BUILDING_DATA_LIST:
			type = data["TYPE"]
			vnum = data["VNUM"]
			if selectedType != type:
				continue

			if index == buildingIndex:
				self.type = type
				if "BUILDIN" == self.type:
					self.__CreateBuildInInstance(vnum)
				else:
					self.__CreateInstance(vnum)

				self.__SetBuildingData(data)

			index += 1

	def __OnScrollBuildingList(self):
		viewItemCount = self.buildingList.GetViewItemCount()
		itemCount = self.buildingList.GetItemCount()
		pos = self.listScrollBar.GetPos() * (itemCount-viewItemCount)
		self.buildingList.SetBasePos(int(pos))

	def __OnChangeRotation(self):
		self.rot_x = self.ctrlRotationX.GetSliderPos() * self.rot_x_limit - self.rot_x_limit/2
		self.rot_y = self.ctrlRotationY.GetSliderPos() * self.rot_y_limit - self.rot_y_limit/2
		self.rot_z = (self.ctrlRotationZ.GetSliderPos() * 360 + 180) % 360
		if "BUILDIN" == self.type:
			chr.SetRotationAll(self.rot_x, self.rot_y, self.rot_z)
		else:
			chr.SetRotationAll(self.rot_x, self.rot_y, self.rot_z)

	def __LockCameraMoving(self):
		app.SetCameraSetting(int(self.x), int(-self.y), int(self.z), 3000, 0, 30)

	def __UnlockCameraMoving(self):
		app.SetDefaultCamera()

	def __RefreshItem(self):
		self.buildingList.ClearItem()

		categoryIndex = self.categoryList.GetSelectedItem()
		if categoryIndex >= len(self.GUILD_CATEGORY_LIST):
			return
		selectedType = self.GUILD_CATEGORY_LIST[categoryIndex][0]

		index = 0
		for data in BUILDING_DATA_LIST:
			if selectedType != data["TYPE"]:
				continue

			if data["SHOW"]:
				self.buildingList.InsertItem(index, data["LOCAL_NAME"])

			index += 1

		self.buildingList.SelectItem(0)

		if self.buildingList.GetItemCount() < self.buildingList.GetViewItemCount():
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buildingList.SetSize(self.BUILDINGLIST_BASE_WIDTH +15, self.buildingList.GetHeight())
			else:
				self.buildingList.SetSize(120, self.buildingList.GetHeight())
			self.buildingList.LocateItem()
			self.listScrollBar.Hide()
		else:
			if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
				self.buildingList.SetSize(self.BUILDINGLIST_BASE_WIDTH, self.buildingList.GetHeight())
			else:
				self.buildingList.SetSize(105, self.buildingList.GetHeight())
			self.buildingList.LocateItem()
			self.listScrollBar.Show()

	def SettleCurrentPosition(self):
		guildID = miniMap.GetGuildAreaID(self.x, self.y)

		import debugInfo
		if debugInfo.IsDebugMode():
			guildID = player.GetGuildID()

		if guildID != player.GetGuildID():
			return

		self.__RestoreViewMode()
		self.__LockCameraMoving()
		self.Show()

	def SetBuildingPosition(self, x, y, z):
		self.x = x
		self.y = y
		self.posValueX.SetText(str(int(x)))
		self.posValueY.SetText(str(int(y)))

		for i in xrange(len(self.indexList)):
			idx = self.indexList[i]
			xPos, yPos = self.posList[i]

			chr.SelectInstance(idx)
			if 0 != z:
				self.z = z
				chr.SetPixelPosition(int(x+xPos), int(y+yPos), int(z))
			else:
				chr.SetPixelPosition(int(x+xPos), int(y+yPos))

	def IsPositioningMode(self):
		if self.MODE_POSITIONING == self.mode:
			return TRUE
		return FALSE

	def IsPreviewMode(self):
		if self.MODE_PREVIEW == self.mode:
			return TRUE
		return FALSE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
