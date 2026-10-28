if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))
pack = __import__(pyapi.GetModuleName("pack"))

import ui
import uiScriptLocale
import dbg
import snd
import mouseModule
import wndMgr
import skill
import item
import playerSettingModule
import quest
import localeInfo
import uiToolTip
import uiCommon
import constInfo
import emotion
import interfaceModule
import chat
if app.ENABLE_TITLE_SYSTEM:
	import uicharactertitle

if app.ENABLE_RENEWAL_QUEST:
	import math
	import uiQuest

if app.ENABLE_SKILL_COLOR_SYSTEM:
	import uiSkillColor

if app.ENABLE_DETAILS_UI:
	import uiCharacterDetails


PARTY_AFFECT_ATTACKER			= 1
PARTY_AFFECT_TANKER				= 2
PARTY_AFFECT_BUFFER				= 3
PARTY_AFFECT_SKILL_MASTER		= 4
PARTY_AFFECT_BERSERKER			= 5
PARTY_AFFECT_DEFENDER			= 6

SHOW_ONLY_ACTIVE_SKILL = False
SHOW_LIMIT_SUPPORT_SKILL_LIST = []
HIDE_SUPPORT_SKILL_POINT = False

if app.ENABLE_CONQUEROR_LEVEL:
	HIDE_SUPPORT_SKILL_POINT = TRUE
	SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140, 132, 133, 134, 246]
else:
	if localeInfo.IsYMIR():
		SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140,141,142]
		[121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140, 0, 0, 0, 0]
		if not localeInfo.IsCHEONMA():
			HIDE_SUPPORT_SKILL_POINT = TRUE 
			SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140,141,142]
	elif localeInfo.IsJAPAN() or   (localeInfo.IsEUROPE() and app.GetLocalePath() != "locale/ca") and (localeInfo.IsEUROPE() and app.GetLocalePath() != "locale/br"):
		HIDE_SUPPORT_SKILL_POINT = TRUE	
		SHOW_LIMIT_SUPPORT_SKILL_LIST = [121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140]

	else:
		HIDE_SUPPORT_SKILL_POINT = TRUE

FACE_IMAGE_DICT = {
	playerSettingModule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
	playerSettingModule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
	playerSettingModule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
	playerSettingModule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
	playerSettingModule.RACE_SURA_M		: "icon/face/sura_m.tga",
	playerSettingModule.RACE_SURA_W		: "icon/face/sura_w.tga",
	playerSettingModule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
	playerSettingModule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
}

def unsigned32(n):
	return n & 0xFFFFFFFFL

if app.ENABLE_RENEWAL_QUEST:
	quest_slot_listbar = {
		"name" : "Quest_Slot",
		"type" : "listbar",

		"x" : 0,
		"y" : 0,

		"width" : 210,
		"height" : 20,

		"text" : "Quest title",
		"align" : "left",

		"horizontal_align" : "left",
		"vertical_align" : "left",
		"text_horizontal_align" : "left",
		"all_align" : "left",

		"text_height": 40
	}

	quest_lable_expend_img_path_dict = {
		0: "d:/ymir work/ui/quest_re/tabcolor_1_main.tga",
		1: "d:/ymir work/ui/quest_re/tabcolor_2_sub.tga",
		2: "d:/ymir work/ui/quest_re/tabcolor_3_levelup.tga",
		3: "d:/ymir work/ui/quest_re/tabcolor_4_event.tga",
		4: "d:/ymir work/ui/quest_re/tabcolor_5_collection.tga",
		5: "d:/ymir work/ui/quest_re/tabcolor_6_system.tga",
		6: "d:/ymir work/ui/quest_re/tabcolor_7_scroll.tga",
		7: "d:/ymir work/ui/quest_re/tabcolor_8_daily.tga"
	}

	quest_label_dict = {
		0 : localeInfo.QUEST_CATEGORY_00,
		1 : localeInfo.QUEST_CATEGORY_01,
		2 : localeInfo.QUEST_CATEGORY_02,
		3 : localeInfo.QUEST_CATEGORY_03,
		4 : localeInfo.QUEST_CATEGORY_04,
		5 : localeInfo.QUEST_CATEGORY_05,
		6 : localeInfo.QUEST_CATEGORY_06,
		7 : localeInfo.QUEST_CATEGORY_07,
	}

class CharacterWindow(ui.ScriptWindow):

	if app.ENABLE_NINETH_SKILL:
		ACTIVE_PAGE_SLOT_COUNT = 9
		SUPPORT_PAGE_SLOT_COUNT = 18
	else:
		ACTIVE_PAGE_SLOT_COUNT = 8
		SUPPORT_PAGE_SLOT_COUNT = 18

	PAGE_SLOT_COUNT = 12
	PAGE_HORSE = 2

	SKILL_GROUP_NAME_DICT = {
		playerSettingModule.JOB_WARRIOR		: { 1 : localeInfo.SKILL_GROUP_WARRIOR_1,	2 : localeInfo.SKILL_GROUP_WARRIOR_2, },
		playerSettingModule.JOB_ASSASSIN	: { 1 : localeInfo.SKILL_GROUP_ASSASSIN_1,	2 : localeInfo.SKILL_GROUP_ASSASSIN_2, },
		playerSettingModule.JOB_SURA		: { 1 : localeInfo.SKILL_GROUP_SURA_1,		2 : localeInfo.SKILL_GROUP_SURA_2, },
		playerSettingModule.JOB_SHAMAN		: { 1 : localeInfo.SKILL_GROUP_SHAMAN_1,	2 : localeInfo.SKILL_GROUP_SHAMAN_2, },
	}

	STAT_DESCRIPTION = {
		"HTH" : localeInfo.STAT_TOOLTIP_CON,
		"INT" : localeInfo.STAT_TOOLTIP_INT,
		"STR" : localeInfo.STAT_TOOLTIP_STR,
		"DEX" : localeInfo.STAT_TOOLTIP_DEX,
		"HTH_QUICK" : localeInfo.STAT_TOOLTIP_CON_QUICK,
		"INT_QUICK" : localeInfo.STAT_TOOLTIP_INT_QUICK,
		"STR_QUICK" : localeInfo.STAT_TOOLTIP_STR_QUICK,
		"DEX_QUICK" : localeInfo.STAT_TOOLTIP_DEX_QUICK,
	}

	STAT_MINUS_DESCRIPTION = localeInfo.STAT_MINUS_DESCRIPTION

	if app.ENABLE_PASSIVE_SYSTEM:
		wndPassive = None

	if app.ENABLE_RENEWAL_QUEST:
		MAX_QUEST_PAGE_HEIGHT = 336.5

	def __init__(self):
		if app.ENABLE_DETAILS_UI:
			self.chDetailsWnd = None
			self.isOpenedDetailsWnd = False

		if app.ENABLE_RENEWAL_QUEST:
			self.isQuestCategoryLoad = FALSE

		ui.ScriptWindow.__init__(self)
		self.state = "STATUS"
		if app.ENABLE_CONQUEROR_LEVEL:
			self.substate = "BASE"

			self.statusConquerorPlusCommandDict={
				"SMH_STR" : "/conqueror_stat smh_str",
				"SMH_HP" : "/conqueror_stat smh_hp",
				"SMH_MOVE" : "/conqueror_stat smh_move",
				"SMH_IMMUNE" : "/conqueror_stat smh_immune",
			}

		self.isLoaded = 0

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			self.skillColorWnd = None
			self.skillColorButton = []

		self.toolTipSkill = 0

		self.__Initialize()
		self.__LoadWindow()

		self.statusPlusCommandDict = {
			"HTH" : "/stat ht",
			"INT" : "/stat iq",
			"STR" : "/stat st",
			"DEX" : "/stat dx",
		}

		if app.ENABLE_STATUS_UP_RENEWAL:
			self.faststatusPlusCommandDict={
				"HTH" : "/stat ht 10",
				"INT" : "/stat iq 10",
				"STR" : "/stat st 10",
				"DEX" : "/stat dx 10",
			}

		self.statusMinusCommandDict={
			"HTH-" : "/stat- ht",
			"INT-" : "/stat- iq",
			"STR-" : "/stat- st",
			"DEX-" : "/stat- dx",
		}

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.refreshToolTip = 0
		self.curSelectedSkillGroup = 0
		self.canUseHorseSkill = -1

		self.toolTip = None
		self.toolTipJob = None
		self.toolTipAlignment = None
		self.toolTipSkill = None

		self.faceImage = None
		self.statusPlusLabel = None
		self.statusPlusValue = None
		self.activeSlot = None
		self.tabDict = None
		self.tabButtonDict = None
		self.pageDict = None
		self.titleBarDict = None
		self.statusPlusButtonDict = None
		self.statusMinusButtonDict = None
		if app.ENABLE_CONQUEROR_LEVEL:
			self.statusConquerorPlusButtonDict = None

		self.skillPageDict = None
		if app.ENABLE_RENEWAL_QUEST:
			self.questScrollBar = None
			self.questLastScrollPosition = 0
			self.questPage = None
			self.questTitleBar = None
			self.questSlotList = []
			self.questCategory = {}
			self.questCategoryList = []

			self.questColorList = {
				"green" : 0xFF83C055,
				"blue": 0xFF45678D,
				"golden": 0xFFCAB62F,
				"default_title": 0xFFCEC6B5
			}

			self.questOpenedCategories = []
			self.questMaxOpenedCategories = 1

			self.questClicked = []
			self.questIndexMap = {}
			self.questCounterList = []
			self.questClockList = []
			self.questSeparatorList = []

			self.displayY = 0
			self.baseCutY = 0
			self.questCategoryRenderPos = []

			self.questSlideWnd = {}
			self.questSlideWndNewKey = 0
		else:
			self.questShowingStartIndex = 0
			self.questScrollBar = None
			self.questSlot = None
			self.questNameList = None
			self.questLastTimeList = None
			self.questLastCountList = None
		self.skillGroupButton = ()

		self.activeSlot = None
		self.activeSkillPointValue = None
		self.supportSkillPointValue = None
		self.skillGroupButton1 = None
		self.skillGroupButton2 = None
		self.activeSkillGroupName = None

		self.guildNameSlot = None
		self.guildNameValue = None
		self.characterNameSlot = None
		self.characterNameValue = None

		self.emotionToolTip = None
		self.soloEmotionSlot = None
		self.dualEmotionSlot = None
		self.specialEmotionSlot = None
		self.specialEmotionSlot2 = None
		self.prevButton = None
		self.nextButton = None
		self.currentPageBack = None
		self.currentPageText = None
		self.currentPage = 1
		if app.ENABLE_TITLE_SYSTEM:
			self.wndTitleSystem = None
			self.titleSystemButton = None
			self.titleSystemUnlockedIDs = set()
			self.titleSystemActiveID = 0

		if app.ENABLE_CONQUEROR_LEVEL:
			self.toolTipConquerorInfoButton = None
			self.tabSungmaButtonDict = None
			self.SungmaButton = None

	if app.ENABLE_DETAILS_UI:
		def OnTop(self):
			if self.chDetailsWnd:
				self.chDetailsWnd.SetTop()

	if app.ENABLE_DETAILS_UI:
		def Hide(self):
			if self.chDetailsWnd:
				self.isOpenedDetailsWnd = self.chDetailsWnd.IsShow()
				self.chDetailsWnd.Close()
			wndMgr.Hide(self.hWnd)

	def Show(self):
		self.__LoadWindow()

		if app.ENABLE_DETAILS_UI:
			self.__InitCharacterDetailsUIButton()
			if self.chDetailsWnd and self.isOpenedDetailsWnd:
				self.chDetailsWnd.Show()

		ui.ScriptWindow.Show(self)

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __BindObject(self):
		self.toolTip = uiToolTip.ToolTip()
		self.toolTipJob = uiToolTip.ToolTip()
		self.toolTipAlignment = uiToolTip.ToolTip(130)

		if app.ENABLE_CONQUEROR_LEVEL:
			self.toolTipConquerorInfoButton = uiToolTip.ToolTip()

		self.faceImage = self.GetChild("Face_Image")
		GetObject = self.GetChild

		faceSlot = self.GetChild("Face_Slot")
		if 949 == app.GetDefaultCodePage():
			faceSlot.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowJobToolTip)
			faceSlot.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideJobToolTip)

		self.boardLeader = self.GetChild("leader_board")
		self.boardLeader.Hide()  # Panel ba?lang?çta kapal?, skill 121'e t?kland???nda aç?lacak
		self.selectedLeadership = None  # Seçili leadership bonus'u (None = seçilmemi?)
		
		list_leader = {
			"LeaderAttacker" : PARTY_AFFECT_ATTACKER, 
			"LeaderBerserker" : PARTY_AFFECT_BERSERKER, 
			"LeaderTanker" : PARTY_AFFECT_TANKER, 
			"LeaderDefender" : PARTY_AFFECT_DEFENDER, 
			"LeaderBuffer" : PARTY_AFFECT_BUFFER, 
			"LeaderSkillMaster" : PARTY_AFFECT_SKILL_MASTER
		}

		for key in list_leader:
			self.GetChild(key).SetEvent(ui.__mem_func__(self.SelectLeaderBonus), list_leader[key])
			
			self.GetChild(key).SetEventOverIn(ui.__mem_func__(self.OverInLeaderBonus), list_leader[key])
			self.GetChild(key).SetEventOverOut(ui.__mem_func__(self.OverOutLeaderBonus), list_leader[key])

		self.statusPlusLabel = self.GetChild("Status_Plus_Label")
		self.statusPlusValue = self.GetChild("Status_Plus_Value")

		self.characterNameSlot = self.GetChild("Character_Name_Slot")
		self.characterNameValue = self.GetChild("Character_Name")
		self.guildNameSlot = self.GetChild("Guild_Name_Slot")
		self.guildNameValue = self.GetChild("Guild_Name")
		self.characterNameSlot.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowAlignmentToolTip)
		self.characterNameSlot.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideAlignmentToolTip)

		self.activeSlot = self.GetChild("Skill_Active_Slot")
		self.activeSkillPointValue = self.GetChild("Active_Skill_Point_Value")
		self.supportSkillPointValue = self.GetChild("Support_Skill_Point_Value")
		self.skillGroupButton1 = self.GetChild("Skill_Group_Button_1")
		self.skillGroupButton2 = self.GetChild("Skill_Group_Button_2")
		self.activeSkillGroupName = self.GetChild("Active_Skill_Group_Name")

		if app.ENABLE_TITLE_SYSTEM:
			try:
				self.titleSystemButton = self.GetChild("Face_Button")
				self.titleSystemButton.SetEvent(ui.__mem_func__(self.__ToggleTitleSystemWindow))
			except:
				self.titleSystemButton = None

		if app.ENABLE_PASSIVE_SYSTEM:
			self.passiveexpandedbtn = self.GetChild("passive_expanded_btn")
			self.passiveexpandedbtn.SetEvent(ui.__mem_func__(self.__ShowPassiveButton))

		if app.ENABLE_RENEWAL_QUEST:
			self.questScrollBar = self.GetChild("Quest_ScrollBar")
			self.questPage = self.GetChild("Quest_Page")
			self.questTitleBar = self.GetChild("Quest_TitleBar")
			self.quest_page_board_window = self.GetChild("quest_page_board_window")
			self.quest_object_board_window = self.GetChild("quest_object_board_window")

		self.tabDict = {
			"STATUS"	: self.GetChild("Tab_01"),
			"SKILL"		: self.GetChild("Tab_02"),
			"EMOTICON"	: self.GetChild("Tab_03"),
			"QUEST"		: self.GetChild("Tab_04"),
		}

		self.tabButtonDict = {
			"STATUS"	: self.GetChild("Tab_Button_01"),
			"SKILL"		: self.GetChild("Tab_Button_02"),
			"EMOTICON"	: self.GetChild("Tab_Button_03"),
			"QUEST"		: self.GetChild("Tab_Button_04")
		}

		self.pageDict = {
			"STATUS"	: self.GetChild("Character_Page"),
			"SKILL"		: self.GetChild("Skill_Page"),
			"EMOTICON"	: self.GetChild("Emoticon_Page"),
			"QUEST"		: self.GetChild("Quest_Page")
		}

		self.titleBarDict = {
			"STATUS"	: self.GetChild("Character_TitleBar"),
			"SKILL"		: self.GetChild("Skill_TitleBar"),
			"EMOTICON"	: self.GetChild("Emoticon_TitleBar"),
			"QUEST"		: self.GetChild("Quest_TitleBar")
		}

		self.statusPlusButtonDict = {
			"HTH"		: self.GetChild("HTH_Plus"),
			"INT"		: self.GetChild("INT_Plus"),
			"STR"		: self.GetChild("STR_Plus"),
			"DEX"		: self.GetChild("DEX_Plus"),
		}

		self.statusMinusButtonDict = {
			"HTH-"		: self.GetChild("HTH_Minus"),
			"INT-"		: self.GetChild("INT_Minus"),
			"STR-"		: self.GetChild("STR_Minus"),
			"DEX-"		: self.GetChild("DEX_Minus"),
		}

		self.skillPageDict = {
			"ACTIVE" : self.GetChild("Skill_Active_Slot"),
			"SUPPORT" : self.GetChild("Skill_ETC_Slot"),
			"HORSE" : self.GetChild("Skill_Active_Slot"),
		}

		self.skillPageStatDict = {
			"SUPPORT"	: player.SKILL_SUPPORT,
			"ACTIVE"	: player.SKILL_ACTIVE,
			"HORSE"		: player.SKILL_HORSE,
		}

		self.skillGroupButton = (
			self.GetChild("Skill_Group_Button_1"),
			self.GetChild("Skill_Group_Button_2"),
		)

		if app.ENABLE_CONQUEROR_LEVEL:
			self.tabSungmaButtonDict = {
				"BASE"		: self.GetChild("change_base_button"),
				"SUNGMA"	: self.GetChild("change_conqueror_button")
			}

			self.SungmaPageDict = {
				"BASE" : self.GetChild("base_info"),
				"SUNGMA" : self.GetChild("sungma_info"),
			}

			self.statusConquerorPlusButtonDict = {
				"SMH_STR"		: self.GetChild("sungma_str_plus"),
				"SMH_HP"		: self.GetChild("sungma_hp_plus"),
				"SMH_MOVE"		: self.GetChild("sungma_move_plus"),
				"SMH_IMMUNE"		: self.GetChild("sungma_immune_plus"),
			}
			
			self.HTH_IMG = self.GetChild("HTH_IMG")
			self.INT_IMG = self.GetChild("INT_IMG")
			self.STR_IMG = self.GetChild("STR_IMG")
			self.DEX_IMG = self.GetChild("DEX_IMG")

			if app.ENABLE_CONQUEROR_LEVEL:
				self.SUNGMA_STR_IMG = self.GetChild("SUNGMA_STR_IMG")
				self.SUNGMA_HP_IMG = self.GetChild("SUNGMA_HP_IMG")
				self.SUNGMA_MOVE_IMG = self.GetChild("SUNGMA_MOVE_IMG")
				self.SUNGMA_IMMUNE_IMG = self.GetChild("SUNGMA_IMMUNE_IMG")

			self.HTH_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowHTHToolTip)
			self.HTH_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideHTHToolTip)
			
			self.INT_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowINTToolTip)
			self.INT_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideINTToolTip)
			
			self.STR_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSTRToolTip)
			self.STR_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSTRToolTip)
			
			self.DEX_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowDEXToolTip)
			self.DEX_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideDEXToolTip)
			
			self.MSPD_IMG = self.GetChild("MSPD_IMG")
			self.ASPD_IMG = self.GetChild("ASPD_IMG")
			self.CSPD_IMG = self.GetChild("CSPD_IMG")
			self.MATT_IMG = self.GetChild("MATT_IMG")
			self.MDEF_IMG = self.GetChild("MDEF_IMG")
			
			self.HEL_IMG = self.GetChild("HEL_IMG")
			self.SP_IMG = self.GetChild("SP_IMG")
			self.ATT_IMG = self.GetChild("ATT_IMG")
			self.DEF_IMG = self.GetChild("DEF_IMG")
			
			if app.ENABLE_CONQUEROR_LEVEL:
				self.ER_IMG = self.GetChild("ER_IMG")
			
			self.MSPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowMSPDToolTip)
			self.MSPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideMSPDToolTip)
			
			self.ASPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowASPDToolTip)
			self.ASPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideASPDToolTip)
			
			self.CSPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowCSPDToolTip)
			self.CSPD_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideCSPDToolTip)
			
			self.MATT_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowMATTToolTip)
			self.MATT_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideMATTToolTip)

			self.MDEF_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowMDEFToolTip)
			self.MDEF_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideMDEFToolTip)

			self.HEL_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowHELToolTip)
			self.HEL_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideHELToolTip)

			self.SP_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSPToolTip)
			self.SP_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSPToolTip)

			self.ATT_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowATTToolTip)
			self.ATT_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideATTToolTip)

			self.DEF_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowDEFolTip)
			self.DEF_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideDEFToolTip)

			if app.ENABLE_CONQUEROR_LEVEL:
				self.ER_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowMERToolTip)
				self.ER_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideMERToolTip)
				
				self.SUNGMA_STR_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSungmaStrToolTip)
				self.SUNGMA_STR_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSungmaStrToolTip)

				self.SUNGMA_HP_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSungmaHpToolTip)
				self.SUNGMA_HP_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSungmaHpToolTip)

				self.SUNGMA_MOVE_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSunmaMoveToolTip)
				self.SUNGMA_MOVE_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSunmaMoveToolTip)

				self.SUNGMA_IMMUNE_IMG.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__ShowSungmaImmuneToolTip)
				self.SUNGMA_IMMUNE_IMG.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__HideSungmaImmuneToolTip)

		global SHOW_ONLY_ACTIVE_SKILL
		global HIDE_SUPPORT_SKILL_POINT
		if SHOW_ONLY_ACTIVE_SKILL or HIDE_SUPPORT_SKILL_POINT:
			self.GetChild("Support_Skill_Point_Label").Hide()

		self.soloEmotionSlot = self.GetChild("SoloEmotionSlot")
		self.dualEmotionSlot = self.GetChild("DualEmotionSlot")
		self.specialEmotionSlot = self.GetChild("SpecialEmotionSlot")
		self.specialEmotionSlot2 = self.GetChild("SpecialEmotionSlot2")
		self.prevButton = self.GetChild("prev_button")
		self.nextButton = self.GetChild("next_button")
		self.currentPageBack = self.GetChild("CurrentPageBack")
		self.currentPageText = self.GetChild("CurrentPage")
		self.__SetEmotionSlot()
		self.RefreshEmotionPage()

		if app.ENABLE_RENEWAL_QUEST:
			self.questScrollBar.SetParent(self.quest_page_board_window)
			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				self.questCategory = ui.SubTitleBar()
				self.questCategory.SetParent(self.questPage)
				self.questCategory.MakeSubTitleBar(210, "red")
				self.questCategory.SetText(quest_label_dict[i])
				self.questCategory.SetSize(210, 16)
				self.questCategory.SetPosition(13, 0)
				self.questCategoryList.append(self.questCategory)

				self.questCategoryRenderPos.append(0)

			self.questScrollBar.SetParent(self.questPage)
			self.RearrangeQuestCategories(xrange(quest.QUEST_CATEGORY_MAX_NUM))
		else:
			self.questShowingStartIndex = 0
			self.questScrollBar = self.GetChild("Quest_ScrollBar")
			self.questScrollBar.SetScrollEvent(ui.__mem_func__(self.OnQuestScroll))
			self.questSlot = self.GetChild("Quest_Slot")
			for i in xrange(quest.QUEST_MAX_NUM):
				self.questSlot.HideSlotBaseImage(i)
				self.questSlot.SetCoverButton(i,\
					"d:/ymir work/ui/game/quest/slot_button_01.sub",\
					"d:/ymir work/ui/game/quest/slot_button_02.sub",\
					"d:/ymir work/ui/game/quest/slot_button_03.sub",\
					"d:/ymir work/ui/game/quest/slot_button_03.sub", True)

			self.questNameList = []
			self.questLastTimeList = []
			self.questLastCountList = []
			for i in xrange(quest.QUEST_MAX_NUM):
				self.questNameList.append(self.GetChild("Quest_Name_0" + str(i)))
				self.questLastTimeList.append(self.GetChild("Quest_LastTime_0" + str(i)))
				self.questLastCountList.append(self.GetChild("Quest_LastCount_0" + str(i)))

		if app.ENABLE_DETAILS_UI:
			self.MainBoard = self.GetChild("board")
			self.ExpandBtn = ui.MakeButton(self.MainBoard, 240, 120, "", "d:/ymir work/ui/game/belt_inventory/", "btn_minimize_normal.tga", "btn_minimize_over.tga", "btn_minimize_down.tga")
			self.ExpandBtn.SetEvent(ui.__mem_func__(self.__ClickExpandButton))
			self.MinimizeBtn = ui.MakeButton(self.MainBoard, 240, 120, "", "d:/ymir work/ui/game/belt_inventory/", "btn_expand_normal.tga", "btn_expand_over.tga", "btn_expand_down.tga")
			self.MinimizeBtn.SetEvent(ui.__mem_func__(self.__ClickMinimizeButton))

	if app.ENABLE_DETAILS_UI:
		def __InitCharacterDetailsUIButton(self):
			self.ExpandBtn.Show()
			self.MinimizeBtn.Hide()

		def __ClickExpandButton(self):
			if not self.chDetailsWnd:
				self.chDetailsWnd = uiCharacterDetails.CharacterDetailsUI(self)
				self.chDetailsWnd.Show()
			else:
				self.chDetailsWnd.Show()

			self.ExpandBtn.Hide()
			self.MinimizeBtn.Show()

		def __ClickMinimizeButton(self):
			self.chDetailsWnd.Hide()
			self.MinimizeBtn.Hide()
			self.ExpandBtn.Show()

		def OnMoveWindow(self, x, y):
			if self.chDetailsWnd:
				self.chDetailsWnd.AdjustPosition(x, y)

			if app.ENABLE_RENEWAL_QUEST:
				if self.questSlideWndNewKey-1 >= 0:
					if self.questSlideWnd[self.questSlideWndNewKey-1] is not None:
						self.questSlideWnd[self.questSlideWndNewKey-1].AdjustPositionAndSize()

	def __SetSkillSlotEvent(self):
		for skillPageValue in self.skillPageDict.itervalues():
			skillPageValue.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			skillPageValue.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectSkill))
			skillPageValue.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			skillPageValue.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			skillPageValue.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			skillPageValue.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			skillPageValue.SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
			skillPageValue.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
				"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
				"d:/ymir work/ui/game/windows/btn_plus_down.sub")

	def OnClickPrevPage(self):
		if self.currentPage == 2:
			self.currentPage = self.currentPage - 1
			self.currentPageText.SetText(str(self.currentPage))
		self.RefreshEmotionPage()

	def OnClickNextPage(self):
		if self.currentPage == 1:
			self.currentPage = self.currentPage + 1
		self.currentPageText.SetText(str(self.currentPage))
		self.RefreshEmotionPage()

	def RefreshEmotionPage(self):
		if self.currentPage == 1:
			self.specialEmotionSlot.Show()
			self.specialEmotionSlot2.Hide()
		else:
			self.specialEmotionSlot.Hide()
			self.specialEmotionSlot2.Show()

	def __SetEmotionSlot(self):
		self.prevButton.SetEvent(ui.__mem_func__(self.OnClickPrevPage))
		self.nextButton.SetEvent(ui.__mem_func__(self.OnClickNextPage))
		self.currentPageText.SetText(str(self.currentPage))

		self.emotionToolTip = uiToolTip.ToolTip()

		for slot in (self.soloEmotionSlot, self.dualEmotionSlot, self.specialEmotionSlot, self.specialEmotionSlot2):
			slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectEmotion))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetUseSlotEvent(ui.__mem_func__(self.__ClickEmotionSlot))
			slot.SetOverInItemEvent(ui.__mem_func__(self.__OverInEmotion))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutEmotion))
			slot.AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub", "d:/ymir work/ui/game/windows/btn_plus_over.sub", "d:/ymir work/ui/game/windows/btn_plus_down.sub")

		for slotIdx, datadict in emotion.EMOTION_DICT.items():
			emotionIdx = slotIdx

			slot = self.soloEmotionSlot

			if slotIdx > 50 and slotIdx <= 60:
				slot = self.dualEmotionSlot
			if slotIdx > 60 and slotIdx <= 72:
				slot = self.specialEmotionSlot
			if slotIdx > 72:
				slot = self.specialEmotionSlot2

			slot.SetEmotionSlot(slotIdx, emotionIdx)
			slot.SetCoverButton(slotIdx)

		self.soloEmotionSlot.SetSlot(24, 2, 32, 32, skill.GetEmptySkill())
		for i in xrange(3):
			self.dualEmotionSlot.SetSlot(54+i, 2, 32, 32, skill.GetEmptySkill())
		for i in xrange(3):
			self.specialEmotionSlot2.SetSlot(82+i, 2, 32, 32, skill.GetEmptySkill())

	def __SelectEmotion(self, slotIndex):
		if not slotIndex in emotion.EMOTION_DICT:
			return

		if app.IsPressed(app.DIK_LCONTROL):
			player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_EMOTION, slotIndex)
			return

		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_EMOTION, slotIndex, slotIndex)

	def __ClickEmotionSlot(self, slotIndex):
		print "click emotion"
		if not slotIndex in emotion.EMOTION_DICT:
			return

		print "check acting"
		if app.ENABLE_SKILL_AFTER_EMOTION:
			if chrmgr.IsActingInteractiveEmotion():
				return
		else:
			if player.IsActingEmotion():
				return

		command = emotion.EMOTION_DICT[slotIndex]["command"]
		print "command", command

		if slotIndex > 50 and slotIndex <= 60:
			vid = player.GetTargetVID()

			if 0 == vid or vid == player.GetMainCharacterIndex() or chr.IsNPC(vid) or chr.IsEnemy(vid):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EMOTION_CHOOSE_ONE)
				return

			command += " " + chr.GetNameByVID(vid)

		net.SendChatPacket(command)

	def ActEmotion(self, emotionIndex):
		self.__ClickEmotionSlot(emotionIndex)

	def __OverInEmotion(self, slotIndex):
		if self.emotionToolTip:

			if not slotIndex in emotion.EMOTION_DICT:
				return

			self.emotionToolTip.ClearToolTip()
			self.emotionToolTip.SetTitle(emotion.EMOTION_DICT[slotIndex]["name"])
			self.emotionToolTip.AlignHorizonalCenter()
			self.emotionToolTip.ShowToolTip()

	def __OverOutEmotion(self):
		if self.emotionToolTip:
			self.emotionToolTip.HideToolTip()

	def __BindEvent(self):
		for i in xrange(len(self.skillGroupButton)):
			self.skillGroupButton[i].SetEvent(lambda arg=i: self.__SelectSkillGroup(arg))

		self.RefreshQuest()
		self.__HideJobToolTip()

		for (tabKey, tabButton) in self.tabButtonDict.items():
			tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)

		if app.ENABLE_CONQUEROR_LEVEL:
			for (tabKey, tabButton) in self.tabSungmaButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabSungmaButton), tabKey)

			for (statusPlusKey, statusPlusButton) in self.statusConquerorPlusButtonDict.items():
				statusPlusButton.SAFE_SetEvent(self.__OnClickConquerorStatusPlusButton, statusPlusKey)
				statusPlusButton.ShowToolTip = lambda arg = statusPlusKey: self.__OverInStatButton(arg)
				statusPlusButton.HideToolTip = lambda arg = statusPlusKey: self.__OverOutStatButton()

		for (statusPlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.SAFE_SetEvent(self.__OnClickStatusPlusButton, statusPlusKey)
			statusPlusButton.ShowToolTip = lambda arg=statusPlusKey: self.__OverInStatButton(arg)
			statusPlusButton.HideToolTip = lambda arg=statusPlusKey: self.__OverOutStatButton()

		for (statusMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.SAFE_SetEvent(self.__OnClickStatusMinusButton, statusMinusKey)
			statusMinusButton.ShowToolTip = lambda arg=statusMinusKey: self.__OverInStatMinusButton(arg)
			statusMinusButton.HideToolTip = lambda arg=statusMinusKey: self.__OverOutStatMinusButton()

		for titleBarValue in self.titleBarDict.itervalues():
			if app.ENABLE_DETAILS_UI:
				titleBarValue.SetCloseEvent(ui.__mem_func__(self.Close))
			else:
				titleBarValue.SetCloseEvent(ui.__mem_func__(self.Hide))

		if app.ENABLE_RENEWAL_QUEST:
			self.questTitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.questScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollQuest))

			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				self.questCategoryList[i].SetEvent(ui.__mem_func__(self.__OnClickQuestCategoryButton), i)
		else:
			self.questSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.__SelectQuest))

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			if localeInfo.IsARABIC() or localeInfo.IsVIETNAM() or localeInfo.IsJAPAN():
				self.__LoadScript(uiScriptLocale.LOCALE_UISCRIPT_PATH + "CharacterWindow.py")
			else:
				self.__LoadScript("UIScript/CharacterWindow.py")

			self.__BindObject()
			self.__BindEvent()
			if getattr(app, "ENABLE_SKILL_UPGRADE", 0):
				self.__CreateSkillUpgradeButton()
		except:
			import exception
			exception.Abort("CharacterWindow.__LoadWindow")

		#self.tabButtonDict["EMOTICON"].Disable()
		self.SetState("STATUS")
		if app.ENABLE_CONQUEROR_LEVEL:
			self.SetSubState("BASE")
	def Destroy(self):
		self.ClearDictionary()
		if app.ENABLE_PASSIVE_SYSTEM:
			if self.wndPassive:
				self.wndPassive.Destroy()
				self.wndPassive = 0
		if app.ENABLE_TITLE_SYSTEM:
			if self.wndTitleSystem:
				self.wndTitleSystem.Destroy()
				self.wndTitleSystem = None
		self.__Initialize()

	def Close(self):
		if app.ENABLE_DETAILS_UI:
			if self.chDetailsWnd and self.chDetailsWnd.IsShow():
				self.chDetailsWnd.Hide()

		if app.ENABLE_TITLE_SYSTEM:
			if self.wndTitleSystem and self.wndTitleSystem.IsShow():
				self.wndTitleSystem.Hide()

		self.Hide()

		if app.ENABLE_RENEWAL_QUEST:
			if self.questSlideWndNewKey > 0:
				if self.questSlideWnd[self.questSlideWndNewKey-1] is not None:
					self.questSlideWnd[self.questSlideWndNewKey-1].CloseSelf()

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd and self.skillColorWnd.IsShow():
				self.skillColorWnd.Hide()

	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill = toolTipSkill

	if app.ENABLE_CONQUEROR_LEVEL:
		def __OnClickConquerorStatusPlusButton(self, statusKey):
			try:
				statusConquerorPlusCommand=self.statusConquerorPlusCommandDict[statusKey]
				net.SendChatPacket(statusConquerorPlusCommand)
			except KeyError, msg:
				dbg.TraceError("CharacterWindow.__OnClickStatusPlusButton KeyError: %s", msg)

	def __OnClickStatusPlusButton(self, statusKey):
		try:
			if app.ENABLE_STATUS_UP_RENEWAL and app.IsPressed(app.DIK_LCONTROL):
				statusPlusCommand=self.faststatusPlusCommandDict[statusKey]
			else:
				statusPlusCommand=self.statusPlusCommandDict[statusKey]
			net.SendChatPacket(statusPlusCommand)
		except KeyError, msg:
			dbg.TraceError("CharacterWindow.__OnClickStatusPlusButton KeyError: %s", msg)

	def __OnClickStatusMinusButton(self, statusKey):
		try:
			statusMinusCommand = self.statusMinusCommandDict[statusKey]
			net.SendChatPacket(statusMinusCommand)
		except KeyError, msg:
			dbg.TraceError("CharacterWindow.__OnClickStatusMinusButton KeyError: %s", msg)


	def __OnClickTabButton(self, stateKey):
		self.SetState(stateKey)

	if app.ENABLE_TITLE_SYSTEM:
		def __ToggleTitleSystemWindow(self):
			if not app.ENABLE_TITLE_SYSTEM:
				return

			if not self.wndTitleSystem:
				self.wndTitleSystem = uicharactertitle.CharacterTitleWindow()
				try:
					self.wndTitleSystem.SetUnlockedTitleIDs(self.titleSystemUnlockedIDs)
					self.wndTitleSystem.SetActiveTitleID(self.titleSystemActiveID)
				except:
					pass

			if self.wndTitleSystem.IsShow():
				self.wndTitleSystem.Hide()
			else:
				self.wndTitleSystem.Open()

	if app.ENABLE_CONQUEROR_LEVEL:
		def __OnClickTabSungmaButton(self, stateKey):
			self.SetSubState(stateKey)

	# Skill Upgrader button left of the skill title (reference UIScript/characterwindownew.py "skill_upgrade_btn")
	def __CreateSkillUpgradeButton(self):
		titleBar = self.GetChild("Skill_TitleBar")
		titleBar.SetPosition(8 + 37, 7)
		titleBar.SetWidth(238 - 37)
		btn = ui.Button()
		btn.SetParent(self.GetChild("board"))
		btn.SetPosition(8, 7)
		btn.SetUpVisual("d:/ymir work/ui/game/skill_upgrade/title_btn_norm.png")
		btn.SetOverVisual("d:/ymir work/ui/game/skill_upgrade/title_btn_hover.png")
		btn.SetDownVisual("d:/ymir work/ui/game/skill_upgrade/title_btn_select.png")
		btn.SetToolTipText(getattr(localeInfo, "SKILL_UPGPRADE_TITLE", "Beceri Y\xfckseltici"), 0, -23)
		btn.SetEvent(ui.__mem_func__(self.__OnClickSkillUpgrade))
		btn.Hide()
		self.skillUpgradeButton = btn

	def __OnClickSkillUpgrade(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			interface.ToggleSkillUpgrade()

	def SetState(self, stateKey):
		self.state = stateKey

		if app.ENABLE_RENEWAL_QUEST:
			if stateKey != "QUEST":
				self.questPage.Hide()
				if self.questSlideWndNewKey > 0:
					if self.questSlideWnd[self.questSlideWndNewKey-1] is not None:
						self.questSlideWnd[self.questSlideWndNewKey-1].CloseSelf()
			else:
				self.__LoadQuestCategory()

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		for titleBarValue in self.titleBarDict.itervalues():
			titleBarValue.Hide()

		self.titleBarDict[stateKey].Show()
		if getattr(self, "skillUpgradeButton", None):
			if stateKey == "SKILL":
				self.skillUpgradeButton.Show()
			else:
				self.skillUpgradeButton.Hide()
		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()

	def GetState(self):
		return self.state

	if app.ENABLE_CONQUEROR_LEVEL:
		def SetSubState(self, stateSubKey):
			
			self.substate = stateSubKey

			for (tabKey, tabButton) in self.tabSungmaButtonDict.items():
				if stateSubKey!=tabKey:
					tabButton.SetUp()

			for pageValue in self.SungmaPageDict.itervalues():
				pageValue.Hide()

			self.__RefreshStatusPlusButtonList()
			self.SungmaPageDict[stateSubKey].Show()
			

		def GetSubState(self):
			return self.substate	

	def __GetTotalAtkText(self):
		minAtk=player.GetStatus(player.ATT_MIN)
		maxAtk=player.GetStatus(player.ATT_MAX)
		atkBonus=player.GetStatus(player.ATT_BONUS)
		attackerBonus=player.GetStatus(player.ATTACKER_BONUS)

		if minAtk==maxAtk:
			return "%d" % (minAtk+atkBonus+attackerBonus)
		else:
			return "%d-%d" % (minAtk+atkBonus+attackerBonus, maxAtk+atkBonus+attackerBonus)

	def __GetTotalMagAtkText(self):
		minMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MIN_MAGIC_WEP)
		maxMagAtk=player.GetStatus(player.MAG_ATT)+player.GetStatus(player.MAX_MAGIC_WEP)

		if minMagAtk==maxMagAtk:
			return "%d" % (minMagAtk)
		else:
			return "%d-%d" % (minMagAtk, maxMagAtk)

	def __GetTotalDefText(self):
		defValue=player.GetStatus(player.DEF_GRADE)
		if constInfo.ADD_DEF_BONUS_ENABLE:
			defValue+=player.GetStatus(player.DEF_BONUS)
		return "%d" % (defValue)

	def RefreshStatus(self):
		if self.isLoaded==0:
			return

		try:
			if app.ENABLE_CONQUEROR_LEVEL:
				if player.GetStatus(player.CONQUEROR_LEVEL) >= 1:
					self.GetChild("Level_Value").SetText(str(player.GetStatus(player.CONQUEROR_LEVEL)))
					self.GetChild("Exp_Value").SetText(str(unsigned32(player.GetConquerorEXP())))
					self.GetChild("RestExp_Value").SetText(str(unsigned32(player.GetStatus(player.CONQUEROR_NEXT_EXP)) - unsigned32(player.GetStatus(player.CONQUEROR_EXP))))
				else:
					self.GetChild("Level_Value").SetText(str(player.GetStatus(player.LEVEL)))
					self.GetChild("Exp_Value").SetText(str(unsigned32(player.GetEXP())))
					self.GetChild("RestExp_Value").SetText(str(unsigned32(player.GetStatus(player.NEXT_EXP)) - unsigned32(player.GetStatus(player.EXP))))
			else:
				self.GetChild("Level_Value").SetText(str(player.GetStatus(player.LEVEL)))
				self.GetChild("Exp_Value").SetText(str(unsigned32(player.GetEXP())))
				self.GetChild("RestExp_Value").SetText(str(unsigned32(player.GetStatus(player.NEXT_EXP)) - unsigned32(player.GetStatus(player.EXP))))
			self.GetChild("HP_Value").SetText(str(player.GetStatus(player.HP)) + '/' + str(player.GetStatus(player.MAX_HP)))
			self.GetChild("SP_Value").SetText(str(player.GetStatus(player.SP)) + '/' + str(player.GetStatus(player.MAX_SP)))

			self.GetChild("STR_Value").SetText(str(player.GetStatus(player.ST)))
			self.GetChild("DEX_Value").SetText(str(player.GetStatus(player.DX)))
			self.GetChild("HTH_Value").SetText(str(player.GetStatus(player.HT)))
			self.GetChild("INT_Value").SetText(str(player.GetStatus(player.IQ)))

			if app.ENABLE_CONQUEROR_LEVEL:
				self.GetChild("sungma_str_value").SetText(str(player.GetStatus(player.SUNGMA_STR)))
				self.GetChild("sungma_hp_value").SetText(str(player.GetStatus(player.SUNGMA_HP)))
				self.GetChild("sungma_move_value").SetText(str(player.GetStatus(player.SUNGMA_MOVE)))
				self.GetChild("sungma_immune_value").SetText(str(player.GetStatus(player.SUNGMA_IMMUNE)))

			self.GetChild("ATT_Value").SetText(self.__GetTotalAtkText())
			self.GetChild("DEF_Value").SetText(self.__GetTotalDefText())

			self.GetChild("MATT_Value").SetText(self.__GetTotalMagAtkText())
			#self.GetChild("MATT_Value").SetText(str(player.GetStatus(player.MAG_ATT)))

			self.GetChild("MDEF_Value").SetText(str(player.GetStatus(player.MAG_DEF)))
			self.GetChild("ASPD_Value").SetText(str(player.GetStatus(player.ATT_SPEED)))
			self.GetChild("MSPD_Value").SetText(str(player.GetStatus(player.MOVING_SPEED)))
			self.GetChild("CSPD_Value").SetText(str(player.GetStatus(player.CASTING_SPEED)))
			self.GetChild("ER_Value").SetText(str(player.GetStatus(player.EVADE_RATE)))

		except:
			#import exception
			#exception.Abort("CharacterWindow.RefreshStatus.BindObject")
			## °ÔÀÓÀÌ Æ¨°Ü ¹ö¸²
			pass

		self.__RefreshStatusPlusButtonList()
		self.__RefreshStatusMinusButtonList()
		self.RefreshAlignment()

		if self.refreshToolTip:
			self.refreshToolTip()
		if app.ENABLE_DETAILS_UI:
			if self.chDetailsWnd and self.chDetailsWnd.IsShow():
				self.chDetailsWnd.RefreshLabel()
	def __RefreshStatusPlusButtonList(self):
		if self.isLoaded==0:
			return

		if app.ENABLE_CONQUEROR_LEVEL:
			if self.GetSubState() == "SUNGMA":
				statusPlusPoint=player.GetStatus(player.CONQUEROR_POINT)
			else:
				statusPlusPoint=player.GetStatus(player.STAT)
		else:
			statusPlusPoint=player.GetStatus(player.STAT)

		if statusPlusPoint>0:
			self.statusPlusValue.SetText(str(statusPlusPoint))
			self.statusPlusLabel.Show()
			self.ShowStatusPlusButtonList()
		else:
			self.statusPlusValue.SetText(str(0))
			self.statusPlusLabel.Hide()
			self.HideStatusPlusButtonList()

	def __RefreshStatusMinusButtonList(self):
		if self.isLoaded == 0:
			return

		statusMinusPoint=self.__GetStatMinusPoint()

		if statusMinusPoint > 0:
			self.__ShowStatusMinusButtonList()
		else:
			self.__HideStatusMinusButtonList()

	def RefreshAlignment(self):
		point, grade = player.GetAlignmentData()

		import colorInfo
		COLOR_DICT = {
			0 : colorInfo.TITLE_RGB_GOOD_4,
			1 : colorInfo.TITLE_RGB_GOOD_3,
			2 : colorInfo.TITLE_RGB_GOOD_2,
			3 : colorInfo.TITLE_RGB_GOOD_1,
			4 : colorInfo.TITLE_RGB_NORMAL,
			5 : colorInfo.TITLE_RGB_EVIL_1,
			6 : colorInfo.TITLE_RGB_EVIL_2,
			7 : colorInfo.TITLE_RGB_EVIL_3,
			8 : colorInfo.TITLE_RGB_EVIL_4,
		}
		colorList = COLOR_DICT.get(grade, colorInfo.TITLE_RGB_NORMAL)
		gradeColor = ui.GenerateColor(colorList[0], colorList[1], colorList[2])

		self.toolTipAlignment.ClearToolTip()
		self.toolTipAlignment.AutoAppendTextLine(localeInfo.TITLE_NAME_LIST[grade], gradeColor)
		self.toolTipAlignment.AutoAppendTextLine(localeInfo.ALIGNMENT_NAME + localeInfo.NumberToDecimalString(point))
		self.toolTipAlignment.AlignHorizonalCenter()

	def __ShowStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Show()

	def __HideStatusMinusButtonList(self):
		for (stateMinusKey, statusMinusButton) in self.statusMinusButtonDict.items():
			statusMinusButton.Hide()

	def ShowStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Show()

	def HideStatusPlusButtonList(self):
		for (statePlusKey, statusPlusButton) in self.statusPlusButtonDict.items():
			statusPlusButton.Hide()

	if app.ENABLE_CONQUEROR_LEVEL:
		def ShowConquerorStatusPlusButtonList(self):
			for (statePlusKey, statusPlusButton) in self.statusConquerorPlusButtonDict.items():
				statusPlusButton.Show()

		def HideConquerorStatusPlusButtonList(self):
			for (statePlusKey, statusPlusButton) in self.statusConquerorPlusButtonDict.items():
				statusPlusButton.Hide()

	def SelectSkill(self, skillSlotIndex):

		mouseController = mouseModule.mouseController

		if False == mouseController.isAttached():

			srcSlotIndex = self.__RealSkillSlotToSourceSlot(skillSlotIndex)
			selectedSkillIndex = player.GetSkillIndex(srcSlotIndex)
			selectedSkillGrade = player.GetSkillGrade(srcSlotIndex)

			if selectedSkillIndex == 121:
				if selectedSkillGrade >= 3:
					# Board'u aç/kapa (toggle)
					if self.boardLeader.IsShow():
						# Board'u kapat
						self.boardLeader.Hide()
						# E?er leadership seçilmemi?se skill 121'i deaktive et
						if self.selectedLeadership is None:
							self.skillPageDict["SUPPORT"].DeactivateSlot(skillSlotIndex)
						# E?er leadership seçilmi?se skill 121'i aktif tut (k?rm?z? renk ile)
						else:
							colorType = wndMgr.COLOR_TYPE_RED
							self.skillPageDict["SUPPORT"].ActivateSlot(skillSlotIndex, colorType)
					else:
						# Board'u aç
						self.boardLeader.Show()
						# ActivateSlot takes 2 arguments: slotNumber and colortype
						# RGB (238, 11, 11) is red color, using COLOR_TYPE_RED if available, otherwise COLOR_TYPE_WHITE
						colorType = wndMgr.COLOR_TYPE_RED
						self.skillPageDict["SUPPORT"].ActivateSlot(skillSlotIndex, colorType)
					return
				else:
					return

			if skill.CanUseSkill(selectedSkillIndex):
				if app.IsPressed(app.DIK_LCONTROL):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_SKILL, srcSlotIndex)
					return

				mouseController.AttachObject(self, player.SLOT_TYPE_SKILL, srcSlotIndex, selectedSkillIndex)
		else:
			mouseController.DeattachObject()

	def SelectEmptySlot(self, SlotIndex):
		mouseModule.mouseController.DeattachObject()

	## ToolTip
	def OverInItem(self, slotNumber):
		if mouseModule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill:
			return

		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotNumber)
		skillIndex = player.GetSkillIndex(srcSlotIndex)
		skillLevel = player.GetSkillLevel(srcSlotIndex)
		skillGrade = player.GetSkillGrade(srcSlotIndex)
		skillType = skill.GetSkillType(skillIndex)

		## ACTIVE
		if skill.SKILL_TYPE_ACTIVE == skillType:
			overInSkillGrade = self.__GetSkillGradeFromSlot(slotNumber)

			if overInSkillGrade == skill.SKILL_GRADE_COUNT-1 and skillGrade == skill.SKILL_GRADE_COUNT:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)
			elif overInSkillGrade == skillGrade:
				self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, overInSkillGrade, skillLevel)
			else:
				self.toolTipSkill.SetSkillOnlyName(srcSlotIndex, skillIndex, overInSkillGrade)
		else:
			self.toolTipSkill.SetSkillNew(srcSlotIndex, skillIndex, skillGrade, skillLevel)

	def OverOutItem(self):
		if 0 != self.toolTipSkill:
			self.toolTipSkill.HideToolTip()

	## Quest
	def __SelectQuest(self, slotIndex):
		if app.ENABLE_RENEWAL_QUEST:
			questIndex = self.questIndexMap[slotIndex]

			if not questIndex in self.questClicked:
				self.questClicked.append(questIndex)
		else:
			questIndex = quest.GetQuestIndex(self.questShowingStartIndex + slotIndex)

		import event
		event.QuestButtonClick(-2147483648 + questIndex)

	def RefreshQuest(self):
		if app.ENABLE_RENEWAL_QUEST:
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			for cat in self.questOpenedCategories:
				self.RefreshQuestCategory(cat)

			self.RefreshQuestCategoriesCount()
		else:
			if self.isLoaded==0:
				return

			self.OnQuestScroll()

			questCount = quest.GetQuestCount()
			questRange = range(quest.QUEST_MAX_NUM)

			if questCount > quest.QUEST_MAX_NUM:
				self.questScrollBar.Show()
			else:
				self.questScrollBar.Hide()

			for i in questRange[:questCount]:
				(questName, questIcon, questCounterName, questCounterValue) = quest.GetQuestData(self.questShowingStartIndex + i)

				self.questNameList[i].SetText(questName)
				self.questNameList[i].Show()
				self.questLastCountList[i].Show()
				self.questLastTimeList[i].Show()

				if len(questCounterName) > 0:
					self.questLastCountList[i].SetText("%s : %d" % (questCounterName, questCounterValue))
				else:
					self.questLastCountList[i].SetText("")

				## Icon
				self.questSlot.SetSlot(i, i, 1, 1, questIcon)

			for i in questRange[questCount:]:
				self.questNameList[i].Hide()
				self.questLastTimeList[i].Hide()
				self.questLastCountList[i].Hide()
				self.questSlot.ClearSlot(i)
				self.questSlot.HideSlotBaseImage(i)

			self.__UpdateQuestClock()

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.questScrollBar.OnUp()
		else:
			self.questScrollBar.OnDown()

	def __UpdateQuestClock(self):
		if "QUEST" == self.state:
			if app.ENABLE_RENEWAL_QUEST:
				for clock in self.questClockList:
					clockText = localeInfo.QUEST_UNLIMITED_TIME

					if clock.GetProperty("idx"):
						(lastName, lastTime) = quest.GetQuestLastTime(clock.GetProperty("idx"))

						if len(lastName) > 0:
							if lastTime <= 0:
								clockText = localeInfo.QUEST_TIMEOVER
							else:
								questLastMinute = lastTime / 60
								questLastSecond = lastTime % 60

								clockText = lastName + " : "

								if questLastMinute > 0:
									clockText += str(questLastMinute) + localeInfo.QUEST_MIN
									if questLastSecond > 0:
										clockText += " "

								if questLastSecond > 0:
									clockText += str(questLastSecond) + localeInfo.QUEST_SEC

					clock.SetText(clockText)
			else:
				# QUEST_LIMIT_COUNT_BUG_FIX
				for i in xrange(min(quest.GetQuestCount(), quest.QUEST_MAX_NUM)):
				# END_OF_QUEST_LIMIT_COUNT_BUG_FIX
					(lastName, lastTime) = quest.GetQuestLastTime(i + self.questShowingStartIndex)

					clockText = localeInfo.QUEST_UNLIMITED_TIME
					if len(lastName) > 0:

						if lastTime <= 0:
							clockText = localeInfo.QUEST_TIMEOVER
						else:
							questLastMinute = lastTime / 60
							questLastSecond = lastTime % 60

							clockText = lastName + " : "

							if questLastMinute > 0:
								clockText += str(questLastMinute) + localeInfo.QUEST_MIN
								if questLastSecond > 0:
									clockText += " "

							if questLastSecond > 0:
								clockText += str(questLastSecond) + localeInfo.QUEST_SEC

					self.questLastTimeList[i].SetText(clockText)

	def __GetStatMinusPoint(self):
		POINT_STAT_RESET_COUNT = 112
		return player.GetStatus(POINT_STAT_RESET_COUNT)

	def __OverInStatMinusButton(self, stat):
		try:
			self.__ShowStatToolTip(self.STAT_MINUS_DESCRIPTION[stat] % self.__GetStatMinusPoint())
		except KeyError:
			pass

		self.refreshToolTip = lambda arg = stat: self.__OverInStatMinusButton(arg)

	def __OverOutStatMinusButton(self):
		self.__HideStatToolTip()
		self.refreshToolTip = 0

	def __OverInStatButton(self, stat):
		try:
			if app.ENABLE_STATUS_UP_RENEWAL:
				self.__ShowStatToolTip(localeInfo.FAST_STATUS_UP)
			else:
				self.__ShowStatToolTip(self.STAT_DESCRIPTION[stat])
		except KeyError:
			pass

	def __OverInStatButtonNew(self, stat):
		try:
			self.__ShowStatToolTip(self.STAT_DESCRIPTION[stat])
		except KeyError:
			pass

	def __OverOutStatButton(self):
		self.__HideStatToolTip()

	def __ShowStatToolTip(self, statDesc):
		self.toolTip.ClearToolTip()
		self.toolTip.AppendTextLine(statDesc)
		self.toolTip.Show()

	def __HideStatToolTip(self):
		self.toolTip.Hide()

	def OnPressEscapeKey(self):
		if app.ENABLE_RENEWAL_QUEST:
			if self.questSlideWndNewKey > 0:
				if self.questSlideWnd[self.questSlideWndNewKey-1] is not None:
					self.questSlideWnd[self.questSlideWndNewKey-1].OnPressEscapeKey()

		self.Close()
		return TRUE

	def OnUpdate(self):
		self.__UpdateQuestClock()

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.skillColorWnd:
				self.__UpdateSkillColorPosition()

	## Skill Process
	def __RefreshSkillPage(self, name, slotCount):
		global SHOW_LIMIT_SUPPORT_SKILL_LIST

		skillPage = self.skillPageDict[name]

		startSlotIndex = skillPage.GetStartIndex()
		if "ACTIVE" == name:
			if self.PAGE_HORSE == self.curSelectedSkillGroup:
				startSlotIndex += slotCount

		getSkillType = skill.GetSkillType
		getSkillIndex = player.GetSkillIndex
		getSkillGrade = player.GetSkillGrade
		getSkillLevel = player.GetSkillLevel
		getSkillLevelUpPoint = skill.GetSkillLevelUpPoint
		getSkillMaxLevel = skill.GetSkillMaxLevel

		for i in xrange(slotCount + 1):
			slotIndex = i + startSlotIndex
			skillIndex = getSkillIndex(slotIndex)

			for j in xrange(skill.SKILL_GRADE_COUNT):
				skillPage.ClearSlot(self.__GetRealSkillSlot(j, i))

			if 0 == skillIndex:
				continue

			skillGrade = getSkillGrade(slotIndex)
			skillLevel = getSkillLevel(slotIndex)
			skillType = getSkillType(skillIndex)

			## MountSkill
			if player.SKILL_INDEX_RIDING == skillIndex:
				if skillGrade == 1:
					skillLevel += 19
				elif skillGrade == 2:
					skillLevel += 29
				elif skillGrade == 3:
					skillLevel = 40

				skillPage.SetSkillSlotNew(slotIndex, skillIndex, max(skillLevel - 1, 0), skillLevel)
				skillPage.SetSlotCount(slotIndex, skillLevel)

			## ACTIVE
			elif skill.SKILL_TYPE_ACTIVE == skillType:
				for j in xrange(skill.SKILL_GRADE_COUNT):
					realSlotIndex = self.__GetRealSkillSlot(j, slotIndex)
					skillPage.SetSkillSlotNew(realSlotIndex, skillIndex, j, skillLevel)
					skillPage.SetCoverButton(realSlotIndex)

					if (skillGrade == skill.SKILL_GRADE_COUNT) and j == (skill.SKILL_GRADE_COUNT-1):
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)
					elif (not self.__CanUseSkillNow()) or (skillGrade != j):
						skillPage.SetSlotCount(realSlotIndex, 0)
						skillPage.DisableCoverButton(realSlotIndex)
						skillPage.DeactivateSlot(realSlotIndex) # hotfix
					else:
						skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)

					if player.IsSkillActive(slotIndex) and (skillGrade == j): # hotfix
						skillPage.ActivateSlot(realSlotIndex, wndMgr.COLOR_TYPE_WHITE)
			## SupportSkill
			else:
				if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
					realSlotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)
					skillPage.SetSkillSlot(realSlotIndex, skillIndex, skillLevel)
					skillPage.SetSlotCountNew(realSlotIndex, skillGrade, skillLevel)

					if skill.CanUseSkill(skillIndex):
						skillPage.SetCoverButton(realSlotIndex)

					# Skill 121 (leadership skill) sadece leadership seçilmi?se aktif tutulacak
					if skillIndex == 121:
						# Board aç?ksa veya leadership seçilmi?se aktif et
						if self.boardLeader.IsShow() or self.selectedLeadership is not None:
							# K?rm?z? renk ile aktif et (COLOR_TYPE_RED)
							colorType = wndMgr.COLOR_TYPE_RED
							skillPage.ActivateSlot(realSlotIndex, colorType)
						else:
							skillPage.DeactivateSlot(realSlotIndex)
					elif player.IsSkillActive(slotIndex): # hotfix
						skillPage.ActivateSlot(realSlotIndex, wndMgr.COLOR_TYPE_WHITE)
					else:
						skillPage.DeactivateSlot(realSlotIndex)

			if player.IsSkillCoolTime(slotIndex):
				(needCoolTime, elapsedTime) = player.GetSkillCoolTime(slotIndex)
				if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
					realSlotIndex = self.__GetRealSlotIndexBySkillType(skillType, slotIndex)
					skillPage.SetSlotCoolTime(realSlotIndex, needCoolTime, elapsedTime)
				else:
					realSlotIndex = 0
					if skill.SKILL_TYPE_ACTIVE == skillType:
						realSlotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)
					else:
						realSlotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)
					skillPage.SetSlotCoolTime(realSlotIndex, needCoolTime, elapsedTime)

			skillPage.RefreshSlot()

		self.__RestoreSlotCoolTime(skillPage)

		if app.ENABLE_SKILL_COLOR_SYSTEM:
			if "ACTIVE" == name:
				# Horse skill kategorisinde skill color button'lar? gösterme
				if self.curSelectedSkillGroup != self.PAGE_HORSE:
					self.__CreateSkillColorButton(skillPage)
				else:
					# Horse skill kategorisinde mevcut skill color button'lar? gizle
					if self.skillColorButton:
						for button in self.skillColorButton:
							if button:
								button.Hide()

	def __RestoreSlotCoolTime(self, skillPage):
		restoreType = skill.SKILL_TYPE_NONE
		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			restoreType = skill.SKILL_TYPE_HORSE
		else:
			restoreType = skill.SKILL_TYPE_ACTIVE

		skillPage.RestoreSlotCoolTime(restoreType)

	def RefreshSkill(self):
		if self.isLoaded == 0:
			return

		if self.__IsChangedHorseRidingSkillLevel():
			self.RefreshCharacter()
			return

		# Horse grubu seçiliyken (ayrı sekme), support slot'unu gizle ve doldurma.
		# Aksi halde horse skill'ler RegisterSkills sonrası support listesinde görünebiliyor.
		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			try:
				self.skillPageDict["SUPPORT"].Hide()
				self.GetChild("Skill_ETC_Title_Bar").Hide()
				self.GetChild("Support_Skill_Point_Label").Hide()
				self.GetChild("Support_Skill_ToolTip").Hide()
			except:
				pass
		else:
			try:
				self.skillPageDict["SUPPORT"].Show()
				self.GetChild("Skill_ETC_Title_Bar").Show()
				self.GetChild("Support_Skill_ToolTip").Show()
			except:
				pass

		global SHOW_ONLY_ACTIVE_SKILL
		if SHOW_ONLY_ACTIVE_SKILL:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
		else:
			self.__RefreshSkillPage("ACTIVE", self.ACTIVE_PAGE_SLOT_COUNT)
			if self.PAGE_HORSE != self.curSelectedSkillGroup:
				self.__RefreshSkillPage("SUPPORT", self.SUPPORT_PAGE_SLOT_COUNT)

		self.RefreshSkillPlusButtonList()

	def CanShowPlusButton(self, skillIndex, skillLevel, curStatPoint):
		if 0 == skillIndex:
			return False

		if not skill.CanLevelUpSkill(skillIndex, skillLevel):
			return False

		return True

	def __RefreshSkillPlusButton(self, name):
		global HIDE_SUPPORT_SKILL_POINT
		if HIDE_SUPPORT_SKILL_POINT and "SUPPORT" == name:
			return

		slotWindow = self.skillPageDict[name]
		slotWindow.HideAllSlotButton()

		slotStatType = self.skillPageStatDict[name]
		if 0 == slotStatType:
			return

		statPoint = player.GetStatus(slotStatType)
		startSlotIndex = slotWindow.GetStartIndex()
		if "HORSE" == name:
			startSlotIndex += self.ACTIVE_PAGE_SLOT_COUNT

		if statPoint > 0:
			for i in xrange(self.PAGE_SLOT_COUNT):
				slotIndex = i + startSlotIndex
				skillIndex = player.GetSkillIndex(slotIndex)
				skillGrade = player.GetSkillGrade(slotIndex)
				skillLevel = player.GetSkillLevel(slotIndex)

				if skillIndex == 0:
					continue
				if skillGrade != 0:
					continue

				if name == "HORSE":
					if player.GetStatus(player.LEVEL) >= skill.GetSkillLevelLimit(skillIndex):
						if skillLevel < 20:
							slotWindow.ShowSlotButton(self.__GetETCSkillRealSlotIndex(slotIndex))
				else:
					if "SUPPORT" == name:
						if not SHOW_LIMIT_SUPPORT_SKILL_LIST or skillIndex in SHOW_LIMIT_SUPPORT_SKILL_LIST:
							if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
								slotWindow.ShowSlotButton(slotIndex)
					else:
						if self.CanShowPlusButton(skillIndex, skillLevel, statPoint):
							slotWindow.ShowSlotButton(slotIndex)

	if app.ENABLE_SKILL_COLOR_SYSTEM:
		def __CreateSkillColorButton(self, parent):
			self.skillColorButton = []

			xPos, yPos = 0, 0
			for idx in xrange(self.PAGE_SLOT_COUNT):
				skillSlot = idx
				if app.ENABLE_NINETH_SKILL:
					if skillSlot < 10: # ACTIVE_PAGE_SLOT_COUNT
						if skillSlot != 6 and skillSlot != 7: # we dont want include 7/8 passive 
							if (skillSlot % 2) == 0:
								xPos = 75
								yPos = 4 + (skillSlot / 2 * 36)
							else:
								xPos = 187
								yPos = 4 + (skillSlot / 2 * 36)

						skillIndex = player.GetSkillIndex(skillSlot + 1)
						skillMaxGrade = 3

						if len(self.skillColorButton) == skillSlot:
							self.skillColorButton.append([])
							self.skillColorButton[skillSlot] = ui.Button()
							self.skillColorButton[skillSlot].SetParent(parent)
							self.skillColorButton[skillSlot].SetUpVisual("d:/ymir work/ui/game/skill_color/skill_color_button_default.tga")
							self.skillColorButton[skillSlot].SetOverVisual("d:/ymir work/ui/game/skill_color/skill_color_button_over.tga")
							self.skillColorButton[skillSlot].SetDownVisual("d:/ymir work/ui/game/skill_color/skill_color_button_down.tga")
							self.skillColorButton[skillSlot].SetPosition(xPos, yPos)
							self.skillColorButton[skillSlot].SetEvent(lambda arg = skillSlot, arg2 = skillIndex: self.__OnPressSkillColorButton(arg, arg2))
							if player.GetSkillGrade(skillSlot + 1) >= skillMaxGrade and chr.IsGameMaster(player.GetMainCharacterIndex()):
								self.skillColorButton[skillSlot].Show()
							else:
								self.skillColorButton[skillSlot].Hide()
						else:
							self.skillColorButton[skillSlot].SetPosition(xPos, yPos)
				else:
					if skillSlot < 6:
						if (skillSlot % 2) == 0:
							xPos = 75
							yPos = 4 + (skillSlot / 2 * 36)
						else:
							xPos = 187
							yPos = 4 + (skillSlot / 2 * 36)

						skillIndex = player.GetSkillIndex(skillSlot + 1)
						skillMaxGrade = 3

						if len(self.skillColorButton) == skillSlot:
							self.skillColorButton.append([])
							self.skillColorButton[skillSlot] = ui.Button()
							self.skillColorButton[skillSlot].SetParent(parent)
							self.skillColorButton[skillSlot].SetUpVisual("d:/ymir work/ui/game/skill_color/skill_color_button_default.tga")
							self.skillColorButton[skillSlot].SetOverVisual("d:/ymir work/ui/game/skill_color/skill_color_button_over.tga")
							self.skillColorButton[skillSlot].SetDownVisual("d:/ymir work/ui/game/skill_color/skill_color_button_down.tga")
							self.skillColorButton[skillSlot].SetPosition(xPos, yPos)
							self.skillColorButton[skillSlot].SetEvent(lambda arg = skillSlot, arg2 = skillIndex: self.__OnPressSkillColorButton(arg, arg2))
							if player.GetSkillGrade(skillSlot + 1) >= skillMaxGrade and chr.IsGameMaster(player.GetMainCharacterIndex()):
								self.skillColorButton[skillSlot].Show()
							else:
								self.skillColorButton[skillSlot].Hide()
						else:
							self.skillColorButton[skillSlot].SetPosition(xPos, yPos)

		def __UpdateSkillColorPosition(self):
			x, y = self.GetGlobalPosition()
			self.skillColorWnd.SetPosition(x + 250, y)

		def __OnPressSkillColorButton(self, skillSlot, skillIndex):
			self.skillColorWnd = uiSkillColor.SkillColorWindow(skillSlot, skillIndex)
			if self.skillColorWnd and not self.skillColorWnd.IsShow():
				self.skillColorWnd.Show()

				"""
				if app.ENABLE_DETAILS_UI:
					if self.chDetailsWnd and self.chDetailsWnd.IsShow():
						self.chDetailsWnd.Hide()
						self.__InitCharacterDetailsUIButton()
				"""

	def RefreshSkillPlusButtonList(self):
		if self.isLoaded==0:
			return

		self.RefreshSkillPlusPointLabel()

		if not self.__CanUseSkillNow():
			return

		try:
			if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
				(cur_page_name, cur_page_skill_type) = self.__GetCurrentPageSkillType()
				self.__RefreshSkillPlusButton(cur_page_name)
			else:
				if self.PAGE_HORSE == self.curSelectedSkillGroup:
					self.__RefreshSkillPlusButton("HORSE")
				else:
					self.__RefreshSkillPlusButton("ACTIVE")

			if self.PAGE_HORSE != self.curSelectedSkillGroup:
				self.__RefreshSkillPlusButton("SUPPORT")

		except:
			import exception
			exception.Abort("CharacterWindow.RefreshSkillPlusButtonList.BindObject")

	def RefreshSkillPlusPointLabel(self):
		if self.isLoaded==0:
			return

		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			activeStatPoint = player.GetStatus(player.SKILL_HORSE)
			self.activeSkillPointValue.SetText(str(activeStatPoint))

		else:
			activeStatPoint = player.GetStatus(player.SKILL_ACTIVE)
			self.activeSkillPointValue.SetText(str(activeStatPoint))

		supportStatPoint = max(0, player.GetStatus(player.SKILL_SUPPORT))
		self.supportSkillPointValue.SetText(str(supportStatPoint))

	## Skill Level Up Button
	def OnPressedSlotButton(self, slotNumber):
		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotNumber)

		skillIndex = player.GetSkillIndex(srcSlotIndex)
		curLevel = player.GetSkillLevel(srcSlotIndex)
		maxLevel = skill.GetSkillMaxLevel(skillIndex)

		net.SendChatPacket("/skillup " + str(skillIndex))

	## Use Skill
	def ClickSkillSlot(self, slotIndex):

		srcSlotIndex = self.__RealSkillSlotToSourceSlot(slotIndex)
		skillIndex = player.GetSkillIndex(srcSlotIndex)
		skillType = skill.GetSkillType(skillIndex)

		if not self.__CanUseSkillNow():
			if skill.SKILL_TYPE_ACTIVE == skillType:
				return

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				if skill.CanUseSkill(skillIndex):
					player.ClickSkillSlot(srcSlotIndex)
					return

		mouseModule.mouseController.DeattachObject()

	def OnUseSkill(self, slotIndex, coolTime):
		if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
			skillIndex = player.GetSkillIndex(slotIndex)
			skillType = skill.GetSkillType(skillIndex)
			(cur_page_name, cur_page_skill_type) = self.__GetCurrentPageSkillType()
			if skillType == cur_page_skill_type:
				realSlotIndex = self.__GetRealSlotIndexBySkillType(skillType, slotIndex)
				skillPage = self.skillPageDict.get(cur_page_name)
				if skillPage:
					if skillPage.HasSlot(realSlotIndex):
						skillPage.SetSlotCoolTime(realSlotIndex, coolTime)
						return
		else:
			skillIndex = player.GetSkillIndex(slotIndex)
			skillType = skill.GetSkillType(skillIndex)

			## ACTIVE
			if skill.SKILL_TYPE_ACTIVE == skillType:
				skillGrade = player.GetSkillGrade(slotIndex)
				slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)
			## ETC
			else:
				slotIndex = self.__GetETCSkillRealSlotIndex(slotIndex)

			for slotWindow in self.skillPageDict.values():
				if slotWindow.HasSlot(slotIndex):
					slotWindow.StoreSlotCoolTime(skillType, slotIndex, coolTime)
					self.__RestoreSlotCoolTime(slotWindow)
					#slotWindow.SetSlotCoolTime(slotIndex, coolTime)
					return

	def OnActivateSkill(self, slotIndex):
		skillGrade = player.GetSkillGrade(slotIndex)
		slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.ActivateSlot(slotIndex, wndMgr.COLOR_TYPE_WHITE)
				return

	def OnDeactivateSkill(self, slotIndex):
		# Skill 121 (leadership skill) sadece leadership seçilmemi?se deaktive edilebilir
		skillIndex = player.GetSkillIndex(slotIndex)
		if skillIndex == 121:
			# Board aç?ksa veya leadership seçilmi?se deaktive etme
			if self.boardLeader.IsShow() or self.selectedLeadership is not None:
				return
			# Leadership seçilmemi?se deaktive et

		skillGrade = player.GetSkillGrade(slotIndex)
		slotIndex = self.__GetRealSkillSlot(skillGrade, slotIndex)

		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.DeactivateSlot(slotIndex)
				return

		self.RefreshSkill()

	def SkillClearCoolTime(self, slotIndex):
		slotIndex = self.__GetRealSkillSlot(player.GetSkillGrade(slotIndex), slotIndex)
		for slotWindow in self.skillPageDict.values():
			if slotWindow.HasSlot(slotIndex):
				slotWindow.SetSlotCoolTime(slotIndex, 0)

	def __ShowJobToolTip(self):
		self.toolTipJob.ShowToolTip()

	def __HideJobToolTip(self):
		self.toolTipJob.HideToolTip()

	def __SetJobText(self, mainJob, subJob):
		if player.GetStatus(player.LEVEL)<5:
			subJob=0

		if 949 == app.GetDefaultCodePage():
			self.toolTipJob.ClearToolTip()

			try:
				jobInfoTitle=localeInfo.JOBINFO_TITLE[mainJob][subJob]
				jobInfoData=localeInfo.JOBINFO_DATA_LIST[mainJob][subJob]
			except IndexError:
				print "uiCharacter.CharacterWindow.__SetJobText(mainJob=%d, subJob=%d)" % (mainJob, subJob)
				return

			self.toolTipJob.AutoAppendTextLine(jobInfoTitle)
			self.toolTipJob.AppendSpace(5)

			for jobInfoDataLine in jobInfoData:
				self.toolTipJob.AutoAppendTextLine(jobInfoDataLine)

			self.toolTipJob.AlignHorizonalCenter()

	def __ShowAlignmentToolTip(self):
		self.toolTipAlignment.ShowToolTip()

	if app.ENABLE_PASSIVE_SYSTEM:
		def __ShowPassiveButton(self):
			print "Click Passive Button"
			if self.wndPassive:
				if self.wndPassive.IsShow(): 
					self.wndPassive.Hide()
				else:
					self.wndPassive.Show()
			else:
				self.wndPassive = PassiveWindow(self)
				self.wndPassive.Show()

		def TryAttachPassiveMaterialFromInventory(self, inventorySlot):
			if not self.wndPassive or not self.wndPassive.IsShow():
				return FALSE

			return self.wndPassive.TryAttachMaterialFromInventory(inventorySlot)

	def __HideAlignmentToolTip(self):
		self.toolTipAlignment.HideToolTip()

	if app.ENABLE_CONQUEROR_LEVEL:
		def __ShowHTHToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_IMG_CON)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideHTHToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowINTToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_IMG_INT)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideINTToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSTRToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_IMG_STR)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSTRToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()
			
		def __ShowDEXToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_IMG_DEX)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideDEXToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowBaseToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_BASE_LEVEL)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideBaseToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSungmaToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_CONQUEROR_LEVEL)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSungmaToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()
		
		def __ShowMSPDToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_MOVE_SPEED)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideMSPDToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowASPDToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_ATT_SPEED)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideASPDToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowCSPDToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_CAST_SPEED)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideCSPDToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()
			
		def __ShowMATTToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_MAG_ATT)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideMATTToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()
			
		def __ShowMDEFToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_MAG_DEF)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideMDEFToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowMERToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_PASSIVE)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideMERToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSungmaStrToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_SUNGMA_STR)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSungmaStrToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSungmaHpToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_SUNGMA_HP)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSungmaHpToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSunmaMoveToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_SUNGMA_MOVE)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSunmaMoveToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSungmaImmuneToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_SUNGMA_IMMUNE)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSungmaImmuneToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowHELToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_HP)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideHELToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowSPToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_SP)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideSPToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowATTToolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_ATT)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideATTToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

		def __ShowDEFolTip(self):
			self.toolTipConquerorInfoButton.ClearToolTip()
			self.toolTipConquerorInfoButton.AutoAppendTextLine(localeInfo.STAT_TOOLTIP_DEF)
			self.toolTipConquerorInfoButton.AlignHorizonalCenter()
			
			self.toolTipConquerorInfoButton.ShowToolTip()

		def __HideDEFToolTip(self):
			self.toolTipConquerorInfoButton.HideToolTip()

	def RefreshCharacter(self):

		if self.isLoaded==0:
			return

		## Name
		try:
			characterName = player.GetName()
			guildName = player.GetGuildName()
			self.characterNameValue.SetText(characterName)
			self.guildNameValue.SetText(guildName)
			if not guildName:
				if localeInfo.IsARABIC():
					self.characterNameSlot.SetPosition(190, 34)
				else:
					self.characterNameSlot.SetPosition(109, 34)

				self.guildNameSlot.Hide()
			else:
				if localeInfo.IsJAPAN():
					self.characterNameSlot.SetPosition(143, 34)
				else:
					self.characterNameSlot.SetPosition(153, 34)
				self.guildNameSlot.Show()

		except:
			import exception
			exception.Abort("CharacterWindow.RefreshCharacter.BindObject")

		race = net.GetMainActorRace()
		group = net.GetMainActorSkillGroup()
		empire = net.GetMainActorEmpire()

		## Job Text
		job = chr.RaceToJob(race)
		self.__SetJobText(job, group)

		## FaceImage
		try:
			faceImageName = FACE_IMAGE_DICT[race]

			try:
				self.faceImage.LoadImage(faceImageName)
			except:
				print "CharacterWindow.RefreshCharacter(race=%d, faceImageName=%s)" % (race, faceImageName)
				self.faceImage.Hide()

		except KeyError:
			self.faceImage.Hide()

		## GroupName
		self.__SetSkillGroupName(race, group)

		## Skill
		if 0 == group:
			self.__SelectSkillGroup(0)

		else:
			self.__SetSkillSlotData(race, group, empire)

			if self.__CanUseHorseSkill():
				self.__SelectSkillGroup(0)

	def OverInLeaderBonus(self, index):
		k = constInfo.LEADERSHIP_POWER
		Bonus = 0
		
		if index == PARTY_AFFECT_ATTACKER:
			Bonus = int(10 + 60 * k)
		elif index == PARTY_AFFECT_TANKER:
			Bonus = int(50 + 1450 * k)
		elif index == PARTY_AFFECT_BUFFER:
			Bonus = int(5 + 45 * k)
		elif index == PARTY_AFFECT_SKILL_MASTER:
			Bonus = int(25 + 600 * k)
		elif index == PARTY_AFFECT_BERSERKER:
			Bonus = int(1 + 5 * k)
		elif index == PARTY_AFFECT_DEFENDER:
			Bonus = int(5 + 30 * k)

		AFFECT_STRING_DICT = {
			PARTY_AFFECT_ATTACKER : localeInfo.PARTY_BONUS_ATTACKER,
			PARTY_AFFECT_TANKER : localeInfo.PARTY_BONUS_TANKER,
			PARTY_AFFECT_BUFFER : localeInfo.PARTY_BONUS_BUFFER,
			PARTY_AFFECT_SKILL_MASTER : localeInfo.PARTY_BONUS_SKILL_MASTER,
			PARTY_AFFECT_BERSERKER : localeInfo.PARTY_BONUS_BERSERKER,
			PARTY_AFFECT_DEFENDER : localeInfo.PARTY_BONUS_DEFENDER,
		}

		AFFECT_TITLE_DICT = {
			PARTY_AFFECT_ATTACKER : "Attacker",
			PARTY_AFFECT_TANKER : "Tanker",
			PARTY_AFFECT_BUFFER : "Buffer",
			PARTY_AFFECT_SKILL_MASTER : "Skill Master",
			PARTY_AFFECT_BERSERKER : "Berserker",
			PARTY_AFFECT_DEFENDER : "Defender",
		}

		if not AFFECT_STRING_DICT.has_key(index):
			return

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(AFFECT_TITLE_DICT[index])
		self.toolTip.AppendTextLine("Grants: " + AFFECT_STRING_DICT[index](Bonus))
		self.toolTip.Show()

	def OverOutLeaderBonus(self, index):
		self.toolTip.Hide()

	def SelectLeaderBonus(self, idx):
		net.SendChatPacket("/leadership_bonus %d" % (idx))
		# Leadership seçildi, flag'i güncelle
		self.selectedLeadership = idx
		# Board'u kapat
		self.boardLeader.Hide()
		# Skill 121'i aktif et (k?rm?z? renk ile)
		skill121SlotIndex = player.GetSkillSlotIndex(121)
		if skill121SlotIndex:
			# Skill 121 bir SUPPORT skill oldu?u için __GetETCSkillRealSlotIndex kullan?lmal?
			realSlotIndex = self.__GetETCSkillRealSlotIndex(skill121SlotIndex)
			# K?rm?z? renk ile aktif et (COLOR_TYPE_RED)
			colorType = wndMgr.COLOR_TYPE_RED
			if "SUPPORT" in self.skillPageDict:
				slotWindow = self.skillPageDict["SUPPORT"]
				if slotWindow.HasSlot(realSlotIndex):
					slotWindow.ActivateSlot(realSlotIndex, colorType)

	def __SetSkillGroupName(self, race, group):

		job = chr.RaceToJob(race)

		if not self.SKILL_GROUP_NAME_DICT.has_key(job):
			return

		nameList = self.SKILL_GROUP_NAME_DICT[job]

		if 0 == group:
			self.skillGroupButton1.SetText(nameList[1])
			self.skillGroupButton2.SetText(nameList[2])
			self.skillGroupButton1.Show()
			self.skillGroupButton2.Show()
			self.activeSkillGroupName.Hide()

		else:

			if self.__CanUseHorseSkill():
				self.activeSkillGroupName.Hide()
				self.skillGroupButton1.SetText(nameList.get(group, "Noname"))
				self.skillGroupButton2.SetText(localeInfo.SKILL_GROUP_HORSE)
				self.skillGroupButton1.Show()
				self.skillGroupButton2.Show()

			else:
				self.activeSkillGroupName.SetText(nameList.get(group, "Noname"))
				self.activeSkillGroupName.Show()
				self.skillGroupButton1.Hide()
				self.skillGroupButton2.Hide()

	def __SetSkillSlotData(self, race, group, empire = 0):
		## SkillIndex
		net.RegisterSkills(race, group, empire)

		## Event
		self.__SetSkillSlotEvent()

		## Refresh
		self.RefreshSkill()

	def __SelectSkillGroup(self, index):
		for btn in self.skillGroupButton:
			btn.SetUp()
		self.skillGroupButton[index].Down()

		if self.__CanUseHorseSkill():
			if 0 == index:
				if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
					index = self.__GetActiveSkillPage()
				else:
					index = net.GetMainActorSkillGroup() - 1
			elif 1 == index:
				index = self.PAGE_HORSE

		self.curSelectedSkillGroup = index
		self.__SetSkillSlotData(net.GetMainActorRace(), index+1, net.GetMainActorEmpire())

	def __CanUseSkillNow(self):
		if 0 == net.GetMainActorSkillGroup():
			return FALSE

		return TRUE

	def __CanUseHorseSkill(self):

		slotIndex = player.GetSkillSlotIndex(player.SKILL_INDEX_RIDING)

		if not slotIndex:
			return FALSE

		grade = player.GetSkillGrade(slotIndex) # hotfix
		level = player.GetSkillLevel(slotIndex)

		if level < 0:
			level *= -1

		if level >= 20:
			return TRUE

		if grade >= 1 and level >= 1: # hotfix
			return True

		return FALSE

	def __IsChangedHorseRidingSkillLevel(self):
		ret = FALSE

		if -1 == self.canUseHorseSkill:
			self.canUseHorseSkill = self.__CanUseHorseSkill()

		if self.canUseHorseSkill != self.__CanUseHorseSkill():
			ret = TRUE

		self.canUseHorseSkill = self.__CanUseHorseSkill()
		return ret

	def __GetRealSkillSlot(self, skillGrade, skillSlot):
		return skillSlot + min(skill.SKILL_GRADE_COUNT-1, skillGrade) * skill.SKILL_GRADE_STEP_COUNT

	def __GetETCSkillRealSlotIndex(self, skillSlot):
		if skillSlot > 100:
			return skillSlot
		return skillSlot % self.ACTIVE_PAGE_SLOT_COUNT

	def __RealSkillSlotToSourceSlot(self, realSkillSlot):
		if realSkillSlot > 100:
			return realSkillSlot
		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			return realSkillSlot + self.ACTIVE_PAGE_SLOT_COUNT
		return realSkillSlot % skill.SKILL_GRADE_STEP_COUNT

	def __GetSkillGradeFromSlot(self, skillSlot):
		return int(skillSlot / skill.SKILL_GRADE_STEP_COUNT)

	if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
		def __GetRealSlotIndexBySkillType(self, skill_type, slot_index):
			if skill_type == skill.SKILL_TYPE_ACTIVE:
				return self.__GetRealSkillSlot(player.GetSkillGrade(slot_index), slot_index)
			else:
				return self.__GetETCSkillRealSlotIndex(slot_index)

		def __GetActiveSkillPage(self):
			return net.GetMainActorSkillGroup() - 1

		def __GetCurrentPageSkillType(self):
			if self.curSelectedSkillGroup == self.PAGE_HORSE:
				return ("HORSE", skill.SKILL_TYPE_HORSE)
			else:
				return ("ACTIVE", skill.SKILL_TYPE_ACTIVE)

	def RefreshSkillCoolTime(self):
		if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
			skillPage = self.skillPageDict.get("ACTIVE")
			if None == skillPage:
				return
		else:
			skillPage = self.skillPageDict["ACTIVE"]
			if None == skillPage:
				return

		if self.PAGE_HORSE == self.curSelectedSkillGroup:
			slotIndex = self.ACTIVE_PAGE_SLOT_COUNT
		else:
			slotIndex = 1

		for i in xrange(slotIndex, slotIndex + self.ACTIVE_PAGE_SLOT_COUNT):
			skillIndex = player.GetSkillIndex(i)
			if 0 != skillIndex:
				if app.ENABLE_WRONG_SKILL_COOLTIME_SLOT:
					realSlotIndex = self.__GetRealSlotIndexBySkillType(skill.GetSkillType(skillIndex), i)
				else:
					realSlotIndex = 0
					if skill.SKILL_TYPE_ACTIVE == skill.GetSkillType(skillIndex):
						realSlotIndex = self.__GetRealSkillSlot(player.GetSkillGrade(i), i)
					else:
						realSlotIndex = self.__GetETCSkillRealSlotIndex(i)

				(needCoolTime, elapsedTime) = player.GetSkillCoolTime(i)
				skillPage.SetSlotCoolTime(realSlotIndex, needCoolTime, elapsedTime)

	def SelectSkillGroup(self, index):
		self.__SelectSkillGroup(index)

	def OnQuestScroll(self):
		questCount = quest.GetQuestCount()
		scrollLineCount = max(0, questCount - quest.QUEST_MAX_NUM)
		startIndex = int(scrollLineCount * self.questScrollBar.GetPos())

		if startIndex != self.questShowingStartIndex:
			self.questShowingStartIndex = startIndex
			self.RefreshQuest()

	if app.ENABLE_RENEWAL_QUEST:
		def __OnScrollQuest(self):
			if self.state != "QUEST":
				return

			curPos = self.questScrollBar.GetPos()
			if math.fabs(curPos - self.questLastScrollPosition) >= 0.001:
				self.RerenderQuestPage()
				self.questLastScrollPosition = curPos

		def ResetQuestScroll(self):
			self.questScrollBar.Hide()

			if self.questScrollBar.GetPos() != 0:
				self.questScrollBar.SetPos(0)

		def RerenderQuestPage(self):
			overflowingY = self.displayY - self.MAX_QUEST_PAGE_HEIGHT
			if overflowingY < 0:
				overflowingY = 0

			self.baseCutY = math.ceil(overflowingY * self.questScrollBar.GetPos())
			self.displayY = 0
			self.RearrangeQuestCategories(xrange(quest.QUEST_CATEGORY_MAX_NUM))
			self.RefreshQuestCategory()

			if overflowingY > 0:
				if (len(self.questOpenedCategories)) == 0:
					self.ResetQuestScroll()
				else:
					self.questScrollBar.Show()
			else:
				self.ResetQuestScroll()

		def __LoadQuestCategory(self):
			self.questPage.Show()

			if self.isLoaded == 0:
				return

			for i in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				category = self.questCategoryList[i]

				categoryName = category.GetProperty("name")
				if not categoryName:
					category.SetProperty("name", category.GetText())
					categoryName = category.GetText()

				questCount = self.GetQuestCountInCategory(i)
				self.questCategoryList[i].SetTextAlignLeft(categoryName + " (" + str(questCount) + ")")
				self.questCategoryList[i].SetTextColor(self.GetQuestCategoryColor(i))
				self.questCategoryList[i].SetQuestLabel(quest_lable_expend_img_path_dict[i], self.GetQuestCountInCategory(i))
				self.questCategoryList[i].Show()

			self.RefreshQuestCategory()
			if self.isQuestCategoryLoad == FALSE:
				self.questScrollBar.Hide()
			else:
				self.RerenderQuestPage()

			self.isQuestCategoryLoad = TRUE

		def GetQuestCategoryColor(self, category):
			return self.questColorList["default_title"]

		def GetQuestProperties(self, questName):
			findString = {
				"*" : "blue",
				"&" : "green",
				"~" : "golden"
			}

			if questName[0] in findString:
				return (questName[1:], findString[questName[0]])

			return (questName, None)

		def IsQuestCategoryOpen(self, category):
			return (category in self.questOpenedCategories)

		def ToggleCategory(self, category):
			if self.IsQuestCategoryOpen(category):
				self.CloseQuestCategory(category)
			else:
				self.OpenQuestCategory(category)

		def RearrangeQuestCategories(self, categoryRange):
			i = 0
			for i in categoryRange:
				if (self.displayY - self.baseCutY) >= 0 and (self.displayY - self.baseCutY) < self.MAX_QUEST_PAGE_HEIGHT - 20:
					self.questCategoryList[i].SetPosition(13, (self.displayY - self.baseCutY) + 10)
					self.questCategoryList[i].Show()
				else:
					self.questCategoryList[i].Hide()

				self.displayY += 20
				self.questCategoryRenderPos[i] = self.displayY

		def CloseQuestCategory(self, category):
			self.questCategoryList[category].CloseCategory(self.GetQuestCountInCategory(category))

			if category in self.questOpenedCategories:
				self.questOpenedCategories.remove(category)

			for currentSlot in self.questSlotList:
				if currentSlot.GetProperty("category") == category:
					currentSlot.Hide()
					self.displayY -= currentSlot.GetHeight()

			self.RerenderQuestPage()

		def OpenQuestCategory(self, category):
			if self.GetQuestCountInCategory(category) == 0:
				return

			while len(self.questOpenedCategories) >= self.questMaxOpenedCategories:
				openedCategories = self.questOpenedCategories.pop()
				self.CloseQuestCategory(openedCategories)

			self.questCategoryList[category].OpenCategory(self.GetQuestCountInCategory(category))
			self.questOpenedCategories.append(category)
			self.RefreshQuestCategory(category)
			self.RerenderQuestPage()

		def RefreshQuestCategory(self, category = -1):
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			categories = []
			if category == -1:
				categories = self.questOpenedCategories
			elif not category in self.questOpenedCategories:
				self.OpenQuestCategory(category)
				return
			else:
				categories.append(category)

			for currentCategory in categories:
				self.displayY = self.questCategoryRenderPos[currentCategory]

				self.LoadCategory(currentCategory)
				self.RearrangeQuestCategories(xrange(currentCategory + 1, quest.QUEST_CATEGORY_MAX_NUM))

		def RefreshQuestCategoriesCount(self):
			for category in xrange(quest.QUEST_CATEGORY_MAX_NUM):
				categoryName = self.questCategoryList[category].GetProperty("name")
				questCount = self.GetQuestCountInCategory(category)
				self.questCategoryList[category].SetTextAlignLeft(categoryName + " (" + str(questCount) + ")")

		def RefreshQuest(self):
			if self.isLoaded == 0 or self.state != "QUEST":
				return

			for category in self.questOpenedCategories:
				self.RefreshQuestCategory(category)

			self.RefreshQuestCategoriesCount()

		def CreateQuestSlot(self, name):
			for questSlot in self.questSlotList:
				if questSlot.GetWindowName() == name:
					return questSlot

			pyScrLoader = ui.PythonScriptLoader()
			slot = ui.ListBar()
			pyScrLoader.LoadElementListBar(slot, quest_slot_listbar, self.questPage)

			slot.SetParent(self.quest_page_board_window)
			slot.SetWindowName(name)
			slot.Hide()
			self.questSlotList.append(slot)
			return slot

		def SetQuest(self, slot, questID, questName, questCounterName, questCounterValue):
			(name, color) = self.GetQuestProperties(questName)
			slot.SetTextAlignLeft(name, 20)
			if color:
				slot.SetTextColor(self.questColorList[color])
			slot.SetEvent(ui.__mem_func__(self.__SelectQuest), questID)
			slot.SetWindowHorizontalAlignLeft()
			slot.Show()

		def LoadCategory(self, category):
			self.questIndexMap = {}
			self.questCounterList = []
			self.questClockList = []
			self.questSeparatorList = []

			for questSlot in self.questSlotList:
				questSlot.Hide()

			questCount = 0
			for questIdx in self.GetQuestsInCategory(category):
				questCount += 1
				(questID, questIndex, questName, questCategory, _, questCounterName, questCounterValue) = questIdx
				(lastName, lastTime) = quest.GetQuestLastTime(questID)

				slot = self.CreateQuestSlot("QuestSlotList_" + str(questCategory) + "_" + str(questID))

				slot.SetPosition(0, (self.displayY - self.baseCutY))
				slot.SetParent(self.quest_page_board_window)
				baseDisplayY = self.displayY

				## -- Quest Counter
				hasCounter = FALSE
				if questCounterName != "":
					self.displayY += 15

					counter = ui.TextLine()
					counter.SetParent(slot)
					counter.SetPosition(20, 20 - 2.5)
					counter.SetText(questCounterName + ": " + str(questCounterValue))
					counter.Show()

					self.questCounterList.append(counter)
					hasCounter = TRUE
				## -- Quest Counter

				## -- Quest Clock
				self.displayY += 15

				clockText = localeInfo.QUEST_UNLIMITED_TIME
				if len(lastName) > 0:
					if lastTime <= 0:
						clockText = localeInfo.QUEST_TIMEOVER
					else:
						questLastMinute = lastTime / 60
						questLastSecond = lastTime % 60

						clockText = lastName + " : "

						if questLastMinute > 0:
							clockText += str(questLastMinute) + localeInfo.QUEST_MIN
							if questLastSecond > 0:
								clockText += " "

						if questLastSecond > 0:
							clockText += str(questLastSecond) + localeInfo.QUEST_SEC

				clock = ui.TextLine()
				clock.SetParent(slot)
				clock.SetPosition(20, 20 + (int(hasCounter) * 14) - 2.5)
				clock.SetText(clockText)
				clock.SetProperty("idx", questID)
				self.questClockList.append(clock)
				clock.Show()
				## -- Quest Clock

				## -- Quest Separator
				self.displayY += 5
				if questCount < self.GetQuestCountInCategory(category):
					seperator = ui.ImageBox()
					seperator.SetParent(slot)
					seperator.SetPosition(4, 20 + (int(hasCounter) * 14 - 2.5) + 15)
					seperator.LoadImage("d:/ymir work/ui/quest_re/quest_list_line_01.tga")
					seperator.Show()
					self.questSeparatorList.append(seperator)
				## -- Quest Separator

				slot.SetProperty("category", questCategory)

				if questIndex in self.questClicked:
					slot.OnClickEvent()

				if (baseDisplayY - self.baseCutY) + 2 >= 0 and (baseDisplayY - self.baseCutY) + 2 < self.MAX_QUEST_PAGE_HEIGHT - 30:
					self.questIndexMap[questID] = questIndex
					self.SetQuest(slot, questID, questName, questCounterName, questCounterValue)

				self.displayY += 15

			newList = []
			for questSlot in self.questSlotList:
				if questSlot.IsShow():
					newList.append(questSlot)

			self.questSlotList = newList

		def __OnClickQuestCategoryButton(self, category):
			self.ToggleCategory(category)

		def GetQuestsInCategory(self, category, retCount = FALSE):
			questList = []
			count = 0
			for i in xrange(quest.GetQuestCount()):
				(questIndex, questName, questCategory, questIcon, questCounterName, questCounterValue) = quest.GetQuestData(i)
				if questCategory == category:
					count += 1
					questList.append((i, questIndex, questName, questCategory, questIcon, questCounterName, questCounterValue))

			if retCount:
				return count

			return questList

		def GetQuestCountInCategory(self, category):
			return self.GetQuestsInCategory(category, TRUE)

if app.ENABLE_PASSIVE_SYSTEM:
	class PassiveWindow(ui.ScriptWindow):
	
		def __init__(self, wndInventory):
			ui.ScriptWindow.__init__(self)
	
			self.isLoaded = 0
			self.wndInventory = wndInventory
			self.wndPassive = None
			self.wndPassiveMaterial = None
			self.wndPassiveJobDownSlot = None
			self.chargeButton = None
			self.addButton = None
			self.activateButton = None
			self.deckButton1 = None
			self.deckButton2 = None
			self.selectedDeckIndex = 0
			self.toolTipItem = uiToolTip.ItemToolTip()
			self.toolTipItem.SetFollow(TRUE)
			self.toolTipItem.HideToolTip()
			self.questionDialog = None
			self.materialSlotDataByDeck = {
				0 : self.__CreateDefaultMaterialSlotData(),
				1 : self.__CreateDefaultMaterialSlotData(),
			}
			self.materialSlotData = self.materialSlotDataByDeck[self.selectedDeckIndex]
	
			self.__LoadWindow()
	
		def __del__(self):
			ui.ScriptWindow.__del__(self)
	
		def Show(self):
			self.__LoadWindow()
			self.RefreshEquipSlotWindow()
			self.SetCenterPosition()
			self.SetTop()
	
			ui.ScriptWindow.Show(self)
	
		def Close(self):
			if self.toolTipItem:
				self.toolTipItem.HideToolTip()
			self.__CloseQuestionDialog()
			self.Hide()
	
		def __LoadWindow(self):
			if self.isLoaded == 1:
				return
	
			self.isLoaded = 1
	
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "uiscript/passiveattr.py")
			except:
				import exception
				exception.Abort("PassiveWindow.LoadWindow.LoadObject")
	
			try:
				wndPassive = self.GetChild("passive_attr_sub_slot")
				wndPassiveMaterial = self.GetChild("passive_material_slot")
				wndPassiveJobDownSlot = self.GetChild("passive_job_down_slot")
				chargeButton = self.GetChild("ChargeButton")
				addButton = self.GetChild("AddButton")
				activateButton = self.GetChild("ActivateButton")
				deckButton1 = self.GetChild("deck_button1")
				deckButton2 = self.GetChild("deck_button2")
				self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
				
			except:
				import exception
				exception.Abort("PassiveWindow.LoadWindow.BindObject")
	
			self.wndPassive = wndPassive
			self.wndPassiveMaterial = wndPassiveMaterial
			self.wndPassiveJobDownSlot = wndPassiveJobDownSlot
			self.chargeButton = chargeButton
			self.addButton = addButton
			self.activateButton = activateButton
			self.deckButton1 = deckButton1
			self.deckButton2 = deckButton2
			self.wndPassive.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.wndPassive.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.wndPassive.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.wndPassive.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.wndPassive.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.wndPassive.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.wndPassive.Show()
			self.wndPassiveMaterial.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyMaterialSlot))
			self.wndPassiveMaterial.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectMaterialItemSlot))
			self.wndPassiveMaterial.SetUnselectItemSlotEvent(ui.__mem_func__(self.SelectMaterialItemSlot))
			self.wndPassiveMaterial.SetUseSlotEvent(ui.__mem_func__(self.SelectMaterialItemSlot))
			self.wndPassiveMaterial.SetOverInItemEvent(ui.__mem_func__(self.OverInMaterialItem))
			self.wndPassiveMaterial.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.wndPassiveMaterial.Show()

			if self.wndPassiveJobDownSlot:
				self.wndPassiveJobDownSlot.Hide()

			self.deckButton1.SetEvent(ui.__mem_func__(self.__SelectDeck), 0)
			self.deckButton2.SetEvent(ui.__mem_func__(self.__SelectDeck), 1)
			self.deckButton1.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__SelectDeck), 0)
			self.deckButton2.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__SelectDeck), 1)
			self.chargeButton.SetEvent(ui.__mem_func__(self.__OpenChargeQuestionDialog))
			self.addButton.SetEvent(ui.__mem_func__(self.__AddRelicBonus))
			self.activateButton.SetToggleDownEvent(ui.__mem_func__(self.__ToggleRelicActive))
			self.activateButton.SetToggleUpEvent(ui.__mem_func__(self.__ToggleRelicActive))
			self.__RefreshDeckButtonState()
			self.__RefreshButtonState()

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def __GetPassiveSlotIndex(self):
			return item.EQUIPMENT_PASSIVE

		def __GetMaterialSlotOrder(self):
			return (1, 2, 3, 4)

		def __CreateDefaultMaterialSlotData(self):
			return {
				1 : {"vnum" : 30255, "pos" : -1},
				2 : {"vnum" : 30258, "pos" : -1},
				3 : {"vnum" : 30256, "pos" : -1},
				4 : {"vnum" : 30257, "pos" : -1},
			}

		def __RefreshDeckButtonState(self):
			if not self.deckButton1 or not self.deckButton2:
				return

			if self.selectedDeckIndex == 0:
				self.deckButton1.Down()
				self.deckButton2.SetUp()
			else:
				self.deckButton1.SetUp()
				self.deckButton2.Down()

		def __SelectDeck(self, deckIndex):
			if deckIndex == self.selectedDeckIndex:
				self.__RefreshDeckButtonState()
				self.__SendRelicCommand("sky" if deckIndex == 1 else "earth")
				return

			self.selectedDeckIndex = deckIndex
			self.materialSlotData = self.materialSlotDataByDeck[self.selectedDeckIndex]
			self.__RefreshDeckButtonState()
			self.__RefreshMaterialSlots()
			self.__SendRelicCommand("sky" if deckIndex == 1 else "earth")

		def __SyncDeckSelectionFromButtons(self):
			if not self.deckButton1 or not self.deckButton2:
				return

			if self.deckButton2.IsDown() and self.selectedDeckIndex != 1:
				self.__SelectDeck(1)
			elif self.deckButton1.IsDown() and self.selectedDeckIndex != 0:
				self.__SelectDeck(0)

		def __ValidateMaterialSlots(self):
			for slotIndex in self.__GetMaterialSlotOrder():
				itemData = self.materialSlotData[slotIndex]
				slotPos = itemData["pos"]
				if slotPos < 0:
					continue

				if player.GetItemIndex(slotPos) != itemData["vnum"]:
					itemData["pos"] = -1

		def __RefreshMaterialSlots(self):
			self.__ValidateMaterialSlots()

			for slotIndex in self.__GetMaterialSlotOrder():
				itemData = self.materialSlotData[slotIndex]
				slotPos = itemData["pos"]
				if slotPos >= 0:
					itemVNum = player.GetItemIndex(slotPos)
					itemCount = player.GetItemCount(slotPos)
					if itemCount <= 1:
						itemCount = 0

					self.wndPassiveMaterial.SetItemSlot(slotIndex, itemVNum, itemCount)
					self.wndPassiveMaterial.ActivateSlot(slotIndex, wndMgr.COLOR_TYPE_WHITE)
				else:
					self.wndPassiveMaterial.ClearSlot(slotIndex)
					self.wndPassiveMaterial.DeactivateSlot(slotIndex)

			self.wndPassiveMaterial.RefreshSlot()

		def __HasPassiveRelic(self):
			return player.GetItemIndex(self.__GetPassiveSlotIndex()) != 0

		def __IsPassiveRelicActive(self):
			if not self.__HasPassiveRelic():
				return FALSE

			return player.GetItemMetinSocket(self.__GetPassiveSlotIndex(), 1) != 0

		def __RefreshButtonState(self):
			hasPassiveRelic = self.__HasPassiveRelic()
			if hasPassiveRelic:
				self.chargeButton.Enable()
				self.addButton.Enable()
				self.activateButton.Enable()
			else:
				self.chargeButton.Disable()
				self.addButton.Disable()
				self.activateButton.Disable()

			if self.__IsPassiveRelicActive():
				self.activateButton.Down()
			else:
				self.activateButton.SetUp()

		def __GetMaterialArgumentString(self):
			positions = []
			for slotIndex in self.__GetMaterialSlotOrder():
				slotPos = self.materialSlotData[slotIndex]["pos"]
				if slotPos < 0:
					return ""
				positions.append(str(slotPos))

			return " ".join(positions)

		def __SendRelicCommand(self, command, useMaterials = FALSE):
			chatCommand = "/passive_relic " + command
			if useMaterials:
				materialArgs = self.__GetMaterialArgumentString()
				if not materialArgs:
					chat.AppendChat(chat.CHAT_TYPE_INFO, "Tum ruh itemlerini yuvalara yerlestir.")
					return

				chatCommand += " " + materialArgs

			net.SendChatPacket(chatCommand)

		def __CloseQuestionDialog(self):
			if not self.questionDialog:
				return

			self.questionDialog.Close()
			self.questionDialog = None

		def __OpenChargeQuestionDialog(self):
			if not self.__HasPassiveRelic():
				return

			self.__CloseQuestionDialog()
			questionDialog = uiCommon.QuestionDialog("thin")
			questionDialog.SetText("Kutsal ruhu þarj ederken nesneler bu esnada kaybolur. Emin misin?")
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.__AcceptCharge))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.__CloseQuestionDialog))
			questionDialog.Open()
			self.questionDialog = questionDialog

		def __AcceptCharge(self):
			self.__CloseQuestionDialog()
			self.__SendRelicCommand("charge", TRUE)

		def __AddRelicBonus(self):
			if not self.__HasPassiveRelic():
				return

			self.__SendRelicCommand("add", TRUE)

		def __ToggleRelicActive(self):
			if not self.__HasPassiveRelic():
				return

			self.__SendRelicCommand("activate")

		def __SetToolTipPosition(self):
			if not self.toolTipItem:
				return

			(anchorX, anchorY) = self.wndPassive.GetGlobalPosition()
			anchorWidth = self.wndPassive.GetWidth()

			if anchorWidth <= 0 or (anchorX == 0 and anchorY == 0):
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				self.toolTipItem.SetToolTipPosition(int(mouseX), int(mouseY))
				return

			tooltipWidth = self.toolTipItem.GetWidth()
			tooltipHeight = self.toolTipItem.GetHeight()
			screenWidth = wndMgr.GetScreenWidth()
			screenHeight = wndMgr.GetScreenHeight()

			tooltipLeft = anchorX + anchorWidth + 8
			if tooltipLeft + tooltipWidth > screenWidth:
				tooltipLeft = max(0, anchorX - tooltipWidth - 8)

			tooltipTop = max(0, min(anchorY, screenHeight - tooltipHeight - 10))
			tooltipPosX = int(tooltipLeft + tooltipWidth / 2)
			tooltipPosY = int(tooltipTop + tooltipHeight)
			self.toolTipItem.SetToolTipPosition(tooltipPosX, tooltipPosY)

		def __ShowToolTip(self):
			if self.toolTipItem:
				self.__SetToolTipPosition()
				self.toolTipItem.SetInventoryItem(self.__GetPassiveSlotIndex(), player.EQUIPMENT)
				self.__SetToolTipPosition()

		def OverInItem(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				return

			if slotIndex != self.__GetPassiveSlotIndex():
				return

			if player.GetItemIndex(slotIndex) == 0:
				return

			self.__ShowToolTip()

		def OverInMaterialItem(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				return

			if not slotIndex in self.materialSlotData:
				return

			slotPos = self.materialSlotData[slotIndex]["pos"]
			if slotPos < 0:
				return

			self.__SetToolTipPosition()
			self.toolTipItem.SetInventoryItem(slotPos)
			self.__SetToolTipPosition()

		def OverOutItem(self):
			if self.toolTipItem:
				self.toolTipItem.HideToolTip()

		def SelectItemSlot(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if attachedSlotType == player.SLOT_TYPE_INVENTORY and attachedItemVNum in (100100, 100101):
					if player.GetItemIndex(self.__GetPassiveSlotIndex()) != 0:
						mouseModule.mouseController.DeattachObject()
						net.SendChatPacket("/passive_relic extract %d" % attachedSlotPos)
						return

			self.UseItemSlot(slotIndex)

		def SelectEmptySlot(self, selectedSlotPos):
			passiveSlotIndex = self.__GetPassiveSlotIndex()
			if selectedSlotPos != passiveSlotIndex and selectedSlotPos != 0:
				return

			if not mouseModule.mouseController.isAttached():
				return

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()
			attachedWindow = player.SlotTypeToInvenType(attachedSlotType)

			if player.RESERVED_WINDOW == attachedWindow:
				return

			mouseModule.mouseController.DeattachObject()

			if attachedWindow == player.EQUIPMENT:
				return

			if attachedSlotType == player.SLOT_TYPE_INVENTORY and attachedItemVNum in (100100, 100101):
				if player.GetItemIndex(passiveSlotIndex) != 0:
					net.SendChatPacket("/passive_relic extract %d" % attachedSlotPos)
				return

			net.SendItemUsePacket(attachedSlotPos)

		def SelectEmptyMaterialSlot(self, selectedSlotPos):
			if not selectedSlotPos in self.materialSlotData:
				return

			if not mouseModule.mouseController.isAttached():
				return

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY != attachedSlotType:
				return

			if attachedItemVNum != self.materialSlotData[selectedSlotPos]["vnum"]:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Bu item bu yuvaya yerlestirilemez.")
				return

			self.materialSlotData[selectedSlotPos]["pos"] = attachedSlotPos
			mouseModule.mouseController.DeattachObject()
			self.__RefreshMaterialSlots()

		def SelectMaterialItemSlot(self, selectedSlotPos):
			if not selectedSlotPos in self.materialSlotData:
				return

			self.materialSlotData[selectedSlotPos]["pos"] = -1
			self.__RefreshMaterialSlots()

		def TryAttachMaterialFromInventory(self, inventorySlot):
			if inventorySlot < 0:
				return FALSE

			itemVNum = player.GetItemIndex(inventorySlot)
			if itemVNum == 0:
				return FALSE

			for slotIndex in self.__GetMaterialSlotOrder():
				itemData = self.materialSlotData[slotIndex]
				if itemData["vnum"] != itemVNum:
					continue

				currentPos = itemData["pos"]
				if currentPos == inventorySlot:
					return TRUE

				if currentPos >= 0 and player.GetItemIndex(currentPos) == itemData["vnum"]:
					continue

				itemData["pos"] = inventorySlot
				self.__RefreshMaterialSlots()
				return TRUE

			return FALSE

		def __FindEmptyInventorySlot(self):
			for slotPos in xrange(player.INVENTORY_PAGE_COUNT * player.INVENTORY_PAGE_SIZE):
				if player.GetItemIndex(slotPos) == 0:
					return slotPos
			return -1

		def UseItemSlot(self, slotIndex):
			passiveSlotIndex = self.__GetPassiveSlotIndex()
			if player.GetItemIndex(passiveSlotIndex) == 0:
				return

			emptyInventorySlot = self.__FindEmptyInventorySlot()
			if emptyInventorySlot >= 0:
				net.SendItemMovePacket(passiveSlotIndex, emptyInventorySlot, 1)

			self.__SendRelicCommand("unequip")
			net.SendItemUsePacket(passiveSlotIndex)

		def RefreshEquipSlotWindow(self):
			slotNumber = self.__GetPassiveSlotIndex()
			itemVNum = player.GetItemIndex(slotNumber)
			itemCount = player.GetItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0

			self.wndPassive.SetItemSlot(slotNumber, itemVNum, itemCount)
			if itemVNum:
				self.wndPassive.ActivateSlot(slotNumber, wndMgr.COLOR_TYPE_WHITE)
			else:
				self.wndPassive.DeactivateSlot(slotNumber)

			self.wndPassive.RefreshSlot()
			self.__RefreshMaterialSlots()
			self.__RefreshDeckButtonState()
			self.__RefreshButtonState()

		def OnUpdate(self):
			if self.IsShow():
				self.__SyncDeckSelectionFromButtons()
				self.RefreshEquipSlotWindow()
				if self.toolTipItem and self.toolTipItem.IsShow():
					self.__SetToolTipPosition()
					if not self.wndPassive.IsIn() and not self.wndPassiveMaterial.IsIn():
						self.toolTipItem.HideToolTip()

	def OnMoveWindow(self, x, y):
		pass
