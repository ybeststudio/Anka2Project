if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import localeInfo
import uiScriptLocale
import mouseModule
import snd
import guild
import chat
import uiCommon
import uiToolTip
import wndMgr

class GuildListDialog(ui.ScriptWindow):
	MAX_LINE_COUNT = 8
	PAGEBUTTON_MAX_SIZE = 9
	SPECIAL_TITLE_COLOR  = 0xff4E3D30
	PAGEBUTTON_NUMBER_SIZE = 5
	PAGEONE_MAX_SIZE = 50
	CLICK_LIMIT_TIME = 3
	EMPIRE_ALL = 0
	APPLICANT = 4

	M2JOBLIST = {
		0	:	localeInfo.JOB_WARRIOR,
		1	:	localeInfo.JOB_ASSASSIN,
		2	:	localeInfo.JOB_SURA,
		3	:	localeInfo.JOB_SHAMAN,
		4	:	localeInfo.JOB_WOLFMAN,
	}

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.bigpagecount = 1
		self.nowpagenumber = 1
		self.pagecount = 0
		self.nowempire = 0
		self.nowtype = 0
		self.searchguildclicktime = 0.0
		self.applicantclicktime = 0.0
		self.selectslotindex = -1
		self.isshowpromoteguild = False
		self.isShow = False
		self.isGuildMember = False
		self.isSearchResult = False
		self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.GuildNameValue = None
		self.SearchGuildNameButton = None
		self.ApplicantGuildButton = None
		self.GuildNameImg = None
		self.ResultNameRanking = None
		self.ResultNameGuildOrPlayer = None
		self.ResultNameLevel = None
		self.ResultNameLadderOrJob = None
		self.ResultNameMemberOrSKill = None
		self.ResultNamePromote = None
		self.popup = None
		self.ResultCheckBoxList = {}
		self.ResutlSlotButtonList = {}
		self.ResultApplicantSlotButtonList = {}
		self.ResultSlotList = {}
		self.pagebuttonList = {}
		self.tooltipbutton = None
		self.ShowButtonToolTip = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.isLoaded = 0
		self.bigpagecount = 1
		self.nowpagenumber = 1
		self.pagecount = 0
		self.nowempire = 0
		self.nowtype = 0
		self.selectslotindex = -1
		self.searchguildclicktime = 0.0
		self.applicantclicktime = 0.0
		self.isshowpromoteguild = False
		self.isShow = False
		self.isGuildMember = False
		self.isSearchResult = False
		self.board = None
		self.pageName = None
		self.tabDict = None
		self.tabButtonDict = None
		self.GuildNameValue = None
		self.SearchGuildNameButton = None
		self.ApplicantGuildButton = None
		self.GuildNameImg = None
		self.ResultNameRanking = None
		self.ResultNameGuildOrPlayer = None
		self.ResultNameLevel = None
		self.ResultNameLadderOrJob = None
		self.ResultNameMemberOrSKill = None
		self.ResultNamePromote = None
		self.popup = None
		self.ResultCheckBoxList = {}
		self.ResutlSlotButtonList = {}
		self.ResultApplicantSlotButtonList = {}
		self.ResultSlotList = {}
		self.pagebuttonList = {}
		self.buttontooltip = None
		self.ShowButtonToolTip = False
		self.Close()

	def Open(self):
		if self.isLoaded==0:
			self.isLoaded = 1
			self.__LoadWindow()
			self.__MakeResultUI()
		self.SetCenterPosition()
		self.SetTop()
		ui.ScriptWindow.Show(self)
		self.isShow = True
		self.isGuildMember = guild.IsGuildEnable()
		self.SelectPage("EMPIRE_ALL")

	def IsShow(self):
		return self.isShow

	def Close(self):
		self.isShow = False
		self.Hide()
		if self.buttontooltip:
			self.buttontooltip.Hide()
			self.ShowButtonToolTip = False

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/GuildWindow_GuildListWindow.py")
		except:
			import exception
			exception.Abort("GuildWindow_GuildListWindow.__LoadWindow.LoadScript")

		self.buttontooltip = uiToolTip.ToolTip()
		self.buttontooltip.ClearToolTip()

		try:
			getObject = self.GetChild

			self.board = getObject("Board")
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

			self.tabDict = {
				"EMPIRE_ALL"	    : getObject("Tab_01"),
				"EMPIRE_A"		: getObject("Tab_02"),
				"EMPIRE_B"		: getObject("Tab_03"),
				"EMPIRE_C"		: getObject("Tab_04"),
				"APPLICANT"      : getObject("Tab_05"),
			}
			self.tabButtonDict = {
				"EMPIRE_ALL" 	: getObject("Tab_Button_01"),
				"EMPIRE_A"		: getObject("Tab_Button_02"),
				"EMPIRE_B"		: getObject("Tab_Button_03"),
				"EMPIRE_C"		: getObject("Tab_Button_04"),
				"APPLICANT"      : getObject("Tab_Button_05"),
			}
			self.tabEmpireDict = {
				"EMPIRE_ALL" 	: self.EMPIRE_ALL,
				"EMPIRE_A"		: net.EMPIRE_A,
				"EMPIRE_B"		: net.EMPIRE_B,
				"EMPIRE_C"		: net.EMPIRE_C,
				"APPLICANT"      : self.APPLICANT,
			}

			self.GuildNameValue = getObject("GuildNameValue")
			self.GuildNameValue.SetEscapeEvent(ui.__mem_func__(self.Close))

			self.GuildNameImg = getObject("GuildNameSlot")

			self.SearchGuildNameButton = getObject("SearchGuildButton")
			self.SearchGuildNameButton .SetEvent(ui.__mem_func__(self.SearchGuildForName))

			self.ShowPromoteGuildButton = getObject("ShowPromoteGuildButton")
			self.ShowPromoteGuildButton.SetEvent(ui.__mem_func__(self.ShowPromoteGuild))

			self.ApplicantGuildButton = getObject("ApplicantGuildButton")
			self.ApplicantGuildButton.SetEvent(ui.__mem_func__(self.ApplicantGuild))

			self.prev_button = self.GetChild("prev_button")
			self.prev_button.SetEvent(ui.__mem_func__(self.prevbutton))

			self.ResultNameRanking = getObject("ResultNameRanking")
			self.ResultNameGuildOrPlayer = getObject("ResultNameGuildOrPlayer")
			self.ResultNameLevel = getObject("ResultNameLevel")
			self.ResultNameLadderOrJob = getObject("ResultNameLadderOrJob")
			self.ResultNameMemberOrSKill = getObject("ResultNameMemberOrSKill")
			self.ResultNamePromote = getObject("ResultNamePromote")
			if localeInfo.IsARABIC():
				self.ResultNameRanking.SetPosition(15, 4)
				self.ResultNameGuildOrPlayer.SetPosition(95, 4)
				self.ResultNameLevel.SetPosition(190, 4)
				self.ResultNameLadderOrJob.SetPosition(250, 4)
				self.ResultNameMemberOrSKill.SetPosition(345, 4)
				self.ResultNamePromote.SetPosition(390, 4)
			self.SetRankingResultNameText()

			self.next_button = self.GetChild("next_button")
			self.next_button.SetEvent(ui.__mem_func__(self.nextbutton))

			self.first_prev_button = self.GetChild("first_prev_button")
			self.first_prev_button.SetEvent(ui.__mem_func__(self.firstprevbutton))

			self.last_next_button = self.GetChild("last_next_button")
			self.last_next_button.SetEvent(ui.__mem_func__(self.lastnextbutton))

			if localeInfo.IsARABIC():
				self.prev_button.LeftRightReverse()
				self.next_button.LeftRightReverse()
				self.first_prev_button.LeftRightReverse()
				self.last_next_button.LeftRightReverse()
				self.GetChild("leftcenterImg").LeftRightReverse()
				self.GetChild("rightcenterImg").LeftRightReverse()
				self.GetChild("LeftTop").LeftRightReverse()
				self.GetChild("RightTop").LeftRightReverse()
				self.GetChild("LeftBottom").LeftRightReverse()
				self.GetChild("RightBottom").LeftRightReverse()

				self.topcenterimg = self.GetChild("topcenterImg")
				self.topcenterimg.SetPosition(self.GetWidth() - (self.topcenterimg.GetWidth()*2)+10,57)

				self.bottomcenterImg = self.GetChild("bottomcenterImg")
				self.bottomcenterImg.SetPosition(self.GetWidth() - (self.bottomcenterImg.GetWidth()*2)+10,290)

				self.centerImg = self.GetChild("centerImg")
				self.centerImg.SetPosition(self.GetWidth() - (self.centerImg.GetWidth()*2)+10,57+15)

				for key, img in self.tabDict.items():
					img.LeftRightReverse()

			self.page1_button = self.GetChild("page1_button")
			self.page1_button.SetEvent(ui.__mem_func__(self.Pagebutton), 1)

			self.page2_button = self.GetChild("page2_button")
			self.page2_button.SetEvent(ui.__mem_func__(self.Pagebutton), 2)

			self.page3_button = self.GetChild("page3_button")
			self.page3_button.SetEvent(ui.__mem_func__(self.Pagebutton), 3)

			self.page4_button = self.GetChild("page4_button")
			self.page4_button.SetEvent(ui.__mem_func__(self.Pagebutton), 4)

			self.page5_button = self.GetChild("page5_button")
			self.page5_button.SetEvent(ui.__mem_func__(self.Pagebutton), 5)

			TemppageSlotButton = []
			TemppageSlotButton.append(self.page1_button)
			TemppageSlotButton.append(self.page2_button)
			TemppageSlotButton.append(self.page3_button)
			TemppageSlotButton.append(self.page4_button)
			TemppageSlotButton.append(self.page5_button)
			TemppageSlotButton.append(self.prev_button)
			TemppageSlotButton.append(self.next_button)
			TemppageSlotButton.append(self.first_prev_button)
			TemppageSlotButton.append(self.last_next_button)
			self.pagebuttonList[0] = TemppageSlotButton
			self.HidePageButton()

			self.tabButtonDict["EMPIRE_ALL"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_ALL)
			self.tabButtonDict["EMPIRE_ALL"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.tabButtonDict["EMPIRE_A"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_EMPIRE_A)
			self.tabButtonDict["EMPIRE_A"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.tabButtonDict["EMPIRE_B"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_EMPIRE_B)
			self.tabButtonDict["EMPIRE_B"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.tabButtonDict["EMPIRE_C"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_EMPIRE_C)
			self.tabButtonDict["EMPIRE_C"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.tabButtonDict["APPLICANT"].SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_REGISTER)
			self.tabButtonDict["APPLICANT"].SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.SearchGuildNameButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_SEARCH)
			self.SearchGuildNameButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.ShowPromoteGuildButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_PROMOTE_GUILD)
			self.ShowPromoteGuildButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))
			self.ApplicantGuildButton.SetShowToolTipEvent(ui.__mem_func__(self.OverInToolTipButton), localeInfo.GUILDWINDOW_LIST_REGISTER_GUILD)
			self.ApplicantGuildButton.SetHideToolTipEvent(ui.__mem_func__(self.OverOutToolTipButton))

		except:
			import exception
			exception.Abort("GuildWindow_GuildListWindow.__LoadWindow.SetObject")

		for key, btn in self.tabButtonDict.items():
			btn.SetEvent(ui.__mem_func__(self.SelectPage), key)

		self.SelectPage("EMPIRE_ALL")

	def OverInToolTipButton(self, arg):
		arglen = len(str(arg))
		pos_x, pos_y = wndMgr.GetMousePosition()

		self.buttontooltip.ClearToolTip()
		self.buttontooltip.SetThinBoardSize(11 * arglen)
		self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
		self.buttontooltip.AppendTextLine(arg, 0xffffffff)
		self.buttontooltip.Show()
		self.ShowButtonToolTip = True

	def OverOutToolTipButton(self):
		self.buttontooltip.Hide()
		self.ShowButtonToolTip = False

	def ButtonToolTipProgress(self) :
		if self.ShowButtonToolTip :
			pos_x, pos_y = wndMgr.GetMousePosition()
			self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)

	def SetRankingResultNameText(self):
		self.ResultNameRanking.SetText(localeInfo.GUILDWINDOW_LIST_RANKING)
		self.ResultNameGuildOrPlayer.SetText(localeInfo.GUILDWINDOW_LIST_GUILD_NAME)
		self.ResultNameLevel.SetText(localeInfo.GUILDWINDOW_LIST_GUILD_LV)
		self.ResultNameLadderOrJob.SetText(localeInfo.GUILDWINDOW_LIST_LADDER)
		(x,y) = self.ResultNameLadderOrJob.GetLocalPosition()
		self.ResultNameLadderOrJob.SetPosition(250,y)
		self.ResultNameMemberOrSKill.SetText(localeInfo.GUILDWINDOW_LIST_GUILDMEMBER)
		(x,y) = self.ResultNameMemberOrSKill.GetLocalPosition()
		self.ResultNameMemberOrSKill.SetPosition(325,y)
		self.ResultNamePromote.SetText(localeInfo.GUILDWINDOW_LIST_PROMOTE_GUILD)

	def SetApplicantResultNameText(self):
		self.ResultNameRanking.SetText(localeInfo.GUILDWINDOW_LIST_RANKING_COUNT)
		self.ResultNameGuildOrPlayer.SetText(localeInfo.GUILDWINDOW_LIST_CHRACTER_NAME)
		self.ResultNameLevel.SetText(localeInfo.GUILDWINDOW_LIST_CHRACTER_LV)
		self.ResultNameLadderOrJob.SetText(localeInfo.GUILDWINDOW_LIST_CHRACTER_JOB)
		(x,y) = self.ResultNameLadderOrJob.GetLocalPosition()
		self.ResultNameLadderOrJob.SetPosition(275,y)
		self.ResultNameMemberOrSKill.SetText(localeInfo.GUILDWINDOW_LIST_CHRACTER_SUB_JOB)
		(x,y) = self.ResultNameMemberOrSKill.GetLocalPosition()
		self.ResultNameMemberOrSKill.SetPosition(365,y)
		self.ResultNamePromote.SetText("")

	def __MakeResultUI(self):
		yPos = 0
		for i in range(0,self.MAX_LINE_COUNT):
			yPos = 84 + i * 25

			RankingSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 23, yPos)
			RankingSlotImage.SetAlpha(0)
			RankingSlot = ui.MakeTextLine(RankingSlotImage)
			self.Children.append(RankingSlotImage)
			self.Children.append(RankingSlot)

			GuildNameImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_04.sub", 77, yPos)
			GuildNameImage.SetAlpha(0)
			GuildNameSlot = ui.MakeTextLine(GuildNameImage)
			self.Children.append(GuildNameImage)
			self.Children.append(GuildNameSlot)

			GuildLevelSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 205, yPos)
			GuildLevelSlotImage.SetAlpha(0)
			GuildLevelSlot = ui.MakeTextLine(GuildLevelSlotImage)
			self.Children.append(GuildLevelSlotImage)
			self.Children.append(GuildLevelSlot)

			LadderSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 270, yPos)
			LadderSlotImage.SetAlpha(0)
			LadderSlot = ui.MakeTextLine(LadderSlotImage)
			self.Children.append(LadderSlotImage)
			self.Children.append(LadderSlot)

			GuildMemberCountImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_01.sub", 337, yPos)
			GuildMemberCountImage.SetAlpha(0)
			GuildMemberCountSlot = ui.MakeTextLine(GuildMemberCountImage)
			self.Children.append(GuildMemberCountImage)
			self.Children.append(GuildMemberCountSlot)

			if localeInfo.IsARABIC():
				RankingSlotImage.SetPosition(self.GetWidth() - (40 + 25), yPos)
				GuildNameImage.SetPosition(self.GetWidth() - (160 + 25), yPos)
				GuildLevelSlotImage.SetPosition(self.GetWidth() - (224 + 25), yPos)
				LadderSlotImage.SetPosition(self.GetWidth() - (290 + 25), yPos)
				GuildMemberCountImage.SetPosition(self.GetWidth() - (360 + 25), yPos)

			tempguildlankingslotlist = []
			tempguildlankingslotlist.append(RankingSlot)
			tempguildlankingslotlist.append(GuildNameSlot)
			tempguildlankingslotlist.append(GuildLevelSlot)
			tempguildlankingslotlist.append(LadderSlot)
			tempguildlankingslotlist.append(GuildMemberCountSlot)

			self.ResultSlotList[i] = tempguildlankingslotlist

			itemSlotButtonImage = ui.MakeButton(self, 22, yPos, "", "d:/ymir work/ui/game/guild/guildRankingList/", "ranking_list_button01.sub", "ranking_list_button02.sub", "ranking_list_button02.sub")
			itemSlotButtonImage.Hide()
			itemSlotButtonImage.SetEvent(ui.__mem_func__(self.__SelectItem),i)
			self.Children.append(itemSlotButtonImage)

			if localeInfo.IsARABIC():
				itemSlotButtonImage.LeftRightReverse()

			TempitemSlotButtonImage = []
			TempitemSlotButtonImage.append(itemSlotButtonImage)
			self.ResutlSlotButtonList[i] = TempitemSlotButtonImage

			applicantslotbuttonimage = ui.MakeButton(self, 22, yPos, "", "d:/ymir work/ui/game/guild/guildRankingList/", "applicant_list_button01.sub", "applicant_list_button02.sub", "applicant_list_button02.sub")
			applicantslotbuttonimage.Hide()
			applicantslotbuttonimage.SetEvent(ui.__mem_func__(self.__SelectItem),i)
			self.Children.append(applicantslotbuttonimage)

			if localeInfo.IsARABIC():
				applicantslotbuttonimage.LeftRightReverse()

			TempapplicantbuttonImage = []
			TempapplicantbuttonImage.append(applicantslotbuttonimage)
			self.ResultApplicantSlotButtonList[i] = TempapplicantbuttonImage

			PromoteCheckBoxImg = ui.MakeImageBox(self, "d:/ymir work/ui/public/check_image.sub", 414, yPos)
			PromoteCheckBoxImg.Hide()
			self.Children.append(PromoteCheckBoxImg)
			TempCheckBoxImg = []
			TempCheckBoxImg.append(PromoteCheckBoxImg)
			self.ResultCheckBoxList[i] = TempCheckBoxImg

			if localeInfo.IsARABIC():	
				PromoteCheckBoxImg.SetPosition(self.GetWidth() - (414 + 25),yPos)

	def OnUpdate(self):
		if (app.GetTime() - self.searchguildclicktime) > self.CLICK_LIMIT_TIME and self.SearchGuildNameButton.IsDIsable() == 0:
			self.SearchGuildNameButton.Enable()

		if (app.GetTime() - self.applicantclicktime) > self.CLICK_LIMIT_TIME and self.ApplicantGuildButton.IsDIsable() == 0:
			self.ApplicantGuildButton.Enable()

		self.ButtonToolTipProgress()

	def ApplicantGuild(self):
		if self.selectslotindex == -1:
			chat.AppendChat(chat.CHAT_TYPE_INFO,localeInfo.GUILDWINDOW_LIST_SELECT_GUILD)
		else:
			if not self.isGuildMember:
				self.applicantclicktime = app.GetTime()
				self.ApplicantGuildButton.Disable()
				self.ApplicantGuildButton.SetUp()

				self.popup = uiCommon.QuestionDialog()
				self.popup.SetText(localeInfo.GUILDLIST_APPLICANT)
				self.popup.SetAcceptEvent(lambda arg=True: self.ApplicantGuildDialog(arg))
				self.popup.SetCancelEvent(lambda arg=False: self.ApplicantGuildDialog(arg))
				self.popup.Open()

	def ApplicantGuildDialog(self, arg):
		if arg:
			type = self.nowtype
			if self.isSearchResult:
				type = guild.RANKING_INFO_SEARCH
			(allpage, nowpage, nowpagecount) = guild.GetRankingPageInfo(type, self.nowempire)
			(guildname, level, ladderpoint, minmember, maxmember, promote, Ranking) = guild.GetRankingInfo(nowpage, type, self.nowempire, self.selectslotindex)
			if net.SendRequestApplicant(guildname) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO,localeInfo.GUILDWINDOW_LIST_ALREADY_REGISTER)

			self.AllResultSlotButtonUp()
			self.popup.Close()
			self.popup = None
		else:
			self.popup.Close()
			self.popup = None

	def SearchGuildForName(self):
		name = self.GuildNameValue.GetText()
		if name=="":
			chat.AppendChat(chat.CHAT_TYPE_INFO,localeInfo.GUILDWINDOW_LIST_INPUT_GUILDNAME)
			return

		self.searchguildclicktime = app.GetTime()
		self.SearchGuildNameButton.Disable()
		self.SearchGuildNameButton.SetUp()

		self.ClearBase()
		self.isSearchResult = True

		for key, btn in self.tabButtonDict.items():
			btn.SetUp()

		net.SendRequestSearchGuild(name, self.nowtype, self.nowempire)

	def ShowPromoteGuild(self):
		self.GuildNameValue.SetText("")
		self.isshowpromoteguild = True
		self.isSearchResult = False
		self.ClearBase()

		if self.nowempire == 0:
			self.nowtype = guild.RANKING_INFO_PROMOTE_ALL
			net.SendRequestGuildList(0, guild.RANKING_INFO_PROMOTE_ALL, self.nowempire)
		else:
			self.nowtype = guild.RANKING_INFO_PROMOTE_EMPIRE
			net.SendRequestGuildList(0, guild.RANKING_INFO_PROMOTE_EMPIRE, self.nowempire)

	def SelectPage(self,arg):
		self.GuildNameValue.SetText("")
		for key, btn in self.tabButtonDict.items():
			if arg != key:
				btn.SetUp()

		for key, img in self.tabDict.items():
			if arg == key:
				img.Show()
			else:
				img.Hide()

		self.nowempire = self.tabEmpireDict[arg]
		self.ClearBase()

		if self.isGuildMember:
			self.ShowPromoteGuildButton.Show()
		else:
			self.ShowPromoteGuildButton.Show()

		if self.nowempire == net.GetEmpireID():
			if not self.isGuildMember:
				self.ApplicantGuildButton.Show()
			else:
				self.ApplicantGuildButton.Hide()
		else:
			self.ApplicantGuildButton.Hide()

		self.GuildNameValue.Show()
		self.SearchGuildNameButton.Show()
		self.GuildNameImg.Show()

		self.board.SetTitleName(localeInfo.GUILDWINDOW_LIST_GUILD_LIST)

		self.SetRankingResultNameText()

		if arg == "EMPIRE_ALL":
			self.HidePageButton()
			self.nowtype = guild.RANKING_INFO_ALL
			net.SendRequestGuildList(0, guild.RANKING_INFO_ALL, self.tabEmpireDict[arg])
		elif arg == "APPLICANT":
			self.HidePageButton()
			self.ApplicantGuildButton.Hide()
			self.ShowPromoteGuildButton.Hide()
			self.GuildNameValue.Hide()
			self.SearchGuildNameButton.Hide()
			self.GuildNameImg.Hide()

			if self.isGuildMember:
				self.board.SetTitleName(localeInfo.GUILDWINDOW_LIST_APPLICANT_LIST)
				self.SetApplicantResultNameText()
				self.nowtype = guild.RANKING_INFO_APPLICANT
				net.SendRequestApplicantList(0)
			else:
				self.board.SetTitleName(localeInfo.GUILDWINDOW_LIST_PROMOTE_GUILD_LIST)
				self.nowtype = guild.RANKING_INFO_APPLICANT_GUILD
				net.SendRequestApplicantGuildList(0)
		else:
			self.HidePageButton()
			self.nowtype = guild.RANKING_INFO_EMPIRE
			net.SendRequestGuildList(0, guild.RANKING_INFO_EMPIRE, self.tabEmpireDict[arg])

		self.isshowpromoteguild = False
		self.isSearchResult = False

		for key, btn in self.tabButtonDict.items():
			btn.SetUp()

	def __SelectItem(self,arg):
		if not self.isGuildMember and self.nowtype != guild.RANKING_INFO_APPLICANT_GUILD:
			type = self.nowtype
			if self.isSearchResult:
				type = guild.RANKING_INFO_SEARCH
			(allpage, nowpage, nowpagecount) = guild.GetRankingPageInfo(type, self.nowempire)
			(guildname, level, ladderpoint, minmember, maxmember, promote, Ranking) = guild.GetRankingInfo(nowpage, type, self.nowempire, arg)
			if promote:
				self.AllResultSlotButtonUp()
				self.ResutlSlotButtonList[arg][0].Disable()
				self.ResutlSlotButtonList[arg][0].Down()
				self.selectslotindex = arg
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO,localeInfo.GUILDWINDOW_LIST_NOTPROMOTEGUILD)

	def RefreshGuildRankingList(self, issearch):
		self.AllResultSlotButtonUp()
		type = self.nowtype
		if issearch:
			type = guild.RANKING_INFO_SEARCH

		(allpage, nowpage, nowpagecount) = guild.GetRankingPageInfo(type, self.nowempire)

		if type == guild.RANKING_INFO_APPLICANT:
			for line, ResultSlotList in self.ResultSlotList.items():
				(charname, level, job, skillgroup) = guild.GetApplicantInfo(nowpage,line)
				if "" == charname:
					ResultSlotList[0].SetText("")
					ResultSlotList[1].SetText("")
					ResultSlotList[2].SetText("")
					ResultSlotList[3].SetText("")
					ResultSlotList[4].SetText("")
					self.ResultApplicantSlotButtonList[line][0].Hide()
				else:
					ResultSlotList[0].SetText( str( (line +1) + (8 * nowpage) ))
					ResultSlotList[1].SetText(charname)
					ResultSlotList[2].SetText(str(level))
					
					ResultSlotList[3].SetText(self.M2JOBLIST[job])
					(x,y) = ResultSlotList[3].GetLocalPosition()
					ResultSlotList[3].SetPosition(20,y)
					
					ResultSlotList[4].SetText(localeInfo.JOBINFO_TITLE[job][skillgroup])
					(x,y) = ResultSlotList[4].GetLocalPosition()
					ResultSlotList[4].SetPosition(40,y)

					self.ResultApplicantSlotButtonList[line][0].Show()
		else:
			for line, ResultSlotList in self.ResultSlotList.items():
				(guildname, level, ladderpoint, minmember, maxmember, promote, Ranking) = guild.GetRankingInfo(nowpage, type, self.nowempire, line)
				if "" == guildname:
					ResultSlotList[0].SetText("")
					ResultSlotList[1].SetText("")
					ResultSlotList[2].SetText("")
					ResultSlotList[3].SetText("")
					ResultSlotList[4].SetText("")
					self.ResutlSlotButtonList[line][0].Hide()
					self.ResultCheckBoxList[line][0].Hide()
				else:
					ResultSlotList[0].SetText(str( Ranking ))
					ResultSlotList[1].SetText(guildname)
					ResultSlotList[2].SetText(str(level))

					ResultSlotList[3].SetText(str(ladderpoint))
					(x,y) = ResultSlotList[3].GetLocalPosition()
					ResultSlotList[3].SetPosition(0,y)

					ResultSlotList[4].SetText(str(minmember) + "/" + str(maxmember))
					(x,y) = ResultSlotList[4].GetLocalPosition()
					ResultSlotList[4].SetPosition(0,y)

					self.ResutlSlotButtonList[line][0].Show()
					if promote:
						self.ResultCheckBoxList[line][0].Show()
					else:
						self.ResultCheckBoxList[line][0].Hide()
				if issearch:
					break

		if issearch:
			if "" == guildname:
				self.HidePageButton()
				self.HideItemButton()
				#chat.AppendChat(chat.CHAT_TYPE_INFO,localeInfo.GUILDWINDOW_LIST_GUILD_NOT)
				return

		self.SetPageButton(allpage, nowpage)

		if self.bigpagecount == 1:
			self.prev_button.Disable()
			self.prev_button.Down()
		else:
			self.prev_button.Enable()

		if self.bigpagecount - 1 <= 1:
			self.first_prev_button.Disable()
			self.first_prev_button.Down()
		else:
			self.first_prev_button.Enable()

		if allpage > (self.PAGEBUTTON_NUMBER_SIZE * self.bigpagecount):
			self.next_button.Enable()
		else:
			self.next_button.Disable()
			self.next_button.Down()

		if allpage > (self.PAGEBUTTON_NUMBER_SIZE * (self.bigpagecount+1)):
			self.last_next_button.Enable()
		else:
			self.last_next_button.Disable()
			self.last_next_button.Down()

	def SetPageButton(self, maxsize, page):
		pagebuttonindex = 0
		pagebuttonindex = page - (self.bigpagecount-1) * 5
		self.pagecount = maxsize

		if self.pagecount > 5:
			if (5 * self.bigpagecount - self.pagecount) < 0:
				self.pagecount = 5
			else:
				self.pagecount = 5 - (5 * self.bigpagecount - self.pagecount)

		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(self.pagecount):
				pagebutton[i].Show()

		self.pagebuttonList[0][5].Show()
		self.pagebuttonList[0][6].Show()
		self.pagebuttonList[0][7].Show()
		self.pagebuttonList[0][8].Show()

		self.clearPagebuttoncolor()
		self.pagebuttonList[0][pagebuttonindex].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pagebuttonList[0][pagebuttonindex].Down()
		self.pagebuttonList[0][pagebuttonindex].Disable()
		self.nowpagenumber = pagebuttonindex

	def HidePageButton(self):
		for line, pagebutton in self.pagebuttonList.items():
			for i in range(0,self.PAGEBUTTON_MAX_SIZE):
				pagebutton[i].Hide()

	def Pagebutton(self,number):
		if number == self.nowpagenumber+1:
			return
		if self.bigpagecount > 1:
			if number == self.nowpagenumber - (self.bigpagecount-1) * 5:
				return

		self.clearPagebuttoncolor()
		self.pagebuttonList[0][number-1].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pagebuttonList[0][number-1].Down()
		self.pagebuttonList[0][number-1].Disable()
		self.nowpagenumber = int(self.pagebuttonList[0][number-1].GetText())-1

		if self.nowempire == self.EMPIRE_ALL:
			if self.isshowpromoteguild:
				net.SendRequestGuildList(self.nowpagenumber, guild.RANKING_INFO_PROMOTE_ALL,self.nowempire)
				self.nowtype = guild.RANKING_INFO_PROMOTE_ALL
			else:
				net.SendRequestGuildList(self.nowpagenumber, guild.RANKING_INFO_ALL,self.nowempire)
				self.nowtype = guild.RANKING_INFO_ALL

		elif self.nowempire == self.APPLICANT:
				if self.nowtype == guild.RANKING_INFO_APPLICANT_GUILD:
					net.SendRequestApplicantGuildList(self.nowpagenumber)
				if self.nowtype == guild.RANKING_INFO_APPLICANT:
					net.SendRequestApplicantList(self.nowpagenumber)

		else:
			if self.isshowpromoteguild:
				net.SendRequestGuildList(self.nowpagenumber, guild.RANKING_INFO_PROMOTE_EMPIRE,self.nowempire)
				self.nowtype = guild.RANKING_INFO_PROMOTE_EMPIRE
			else:
				net.SendRequestGuildList(self.nowpagenumber, guild.RANKING_INFO_EMPIRE,self.nowempire)
				self.nowtype = guild.RANKING_INFO_EMPIRE

	def SendRequestList(self):
		if self.nowtype == guild.RANKING_INFO_APPLICANT:
			net.SendRequestApplicantList(self.nowpagenumber)
		elif self.nowtype == guild.RANKING_INFO_APPLICANT_GUILD:
			net.SendRequestApplicantGuildList(self.nowpagenumber)
		else:
			net.SendRequestGuildList(self.nowpagenumber, self.nowtype, self.nowempire)

	def firstprevbutton(self):
		if self.bigpagecount - 1 <= 1:
			return

		self.clearPagebuttoncolor()
		self.bigpagecount = 1
		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(5):
				pagebutton[i].SetText(str(i+1))

		self.nowpagenumber = int(self.pagebuttonList[0][0].GetText())-1
		self.pagebuttonList[0][0].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pagebuttonList[0][0].Down()
		self.pagebuttonList[0][0].Disable()

		self.SendRequestList()

	def lastnextbutton(self):
		(allpage, nowpage, nowpagecount) = guild.GetRankingPageInfo(self.nowtype, self.nowempire)
		self.pagecount = allpage
		self.HidePageButton()
		self.clearPagebuttoncolor()

		if self.pagecount%5 == 0:
			self.bigpagecount = (self.pagecount/5)
		else:
			self.bigpagecount = (self.pagecount/5) + 1

		pagenumber = 5 * (self.pagecount/5)
		if pagenumber == self.pagecount:
			pagenumber -= 5
		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(5):
				pagebutton[i].SetText(str(i+pagenumber+1))

		self.nowpagenumber = self.pagecount-1
		self.SendRequestList()

	def prevbutton(self):
		if self.bigpagecount == 1:
			return

		self.clearPagebuttoncolor()
		self.bigpagecount -= 1

		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(5):
				pagenumber = int(pagebutton[i].GetText()) - 5
				pagebutton[i].SetText(str(pagenumber))

		self.nowpagenumber = int(self.pagebuttonList[0][0].GetText())-1
		self.pagebuttonList[0][0].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pagebuttonList[0][0].Down()
		self.pagebuttonList[0][0].Disable()

		self.SendRequestList()

	def nextbutton(self):
		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(5):
				pagenumber = int(pagebutton[i].GetText()) + 5
				pagebutton[i].SetText(str(pagenumber))

		self.nowpagenumber = int(self.pagebuttonList[0][0].GetText())-1
		self.bigpagecount += 1
		self.HidePageButton()
		self.clearPagebuttoncolor()
		self.pagebuttonList[0][0].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pagebuttonList[0][0].Down()
		self.pagebuttonList[0][0].Disable()

		self.SendRequestList()

	def clearPagebuttoncolor(self):
		for line, pagebutton in self.pagebuttonList.items():
			for i in range(0,self.PAGEBUTTON_NUMBER_SIZE):
				pagebutton[i].SetTextColor(0xffffffff)
				pagebutton[i].SetUp()
				pagebutton[i].Enable()

	def HidePageButton(self):
		for line, pagebutton in self.pagebuttonList.items():
			for i in range(0,self.PAGEBUTTON_MAX_SIZE):
				pagebutton[i].Hide()

	def HideItemButton(self):
		for line, ResultSlotList in self.ResultSlotList.items():
			ResultSlotList[0].SetText("")
			ResultSlotList[1].SetText("")
			ResultSlotList[2].SetText("")
			ResultSlotList[3].SetText("")
			ResultSlotList[4].SetText("")
			self.ResutlSlotButtonList[line][0].Hide()
			self.ResultCheckBoxList[line][0].Hide()
			self.ResultApplicantSlotButtonList[line][0].Hide()

	def AllResultSlotButtonUp(self):
		for line in range(0,self.MAX_LINE_COUNT):
			self.ResutlSlotButtonList[line][0].SetUp()
			self.ResutlSlotButtonList[line][0].Enable()
			self.ResultApplicantSlotButtonList[line][0].SetUp()
			self.ResultApplicantSlotButtonList[line][0].Enable()

		self.selectslotindex = -1

	def ClearBase(self):
		self.bigpagecount = 1
		self.HidePageButton()
		self.HideItemButton()
		for line, pagebutton in self.pagebuttonList.items():
			for i in xrange(5):
				pagebutton[i].SetText(str(i+1))

	def OnPressEscapeKey(self):
		self.Close()
		return True
