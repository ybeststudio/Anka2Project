if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import localeInfo
import item
import skill
import uiToolTip
import math
import wndMgr
import collections

AFF_LEADERSHIP = 901

if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
	import systemSetting

if app.ENABLE_MULTI_FARM_BLOCK:
	import uiCommon
	import constInfo

	class MultiFarmImage(ui.ExpandedImageBox):
		def SetScale(self, x, y):
			import advanced_options_runtime
			factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
			ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)
		def __del__(self):
			ui.ExpandedImageBox.__del__(self)

		def Destroy(self):
			self.multiFarmStatus = 0
			self.players = []

		def __init__(self):
			ui.ExpandedImageBox.__init__(self)

			self.toolTip = uiToolTip.ItemToolTip()
			self.toolTip.Hide()

			self.Destroy()

		def SetMultiFarmInfo(self, multiFarmStatus):
			self.multiFarmStatus = multiFarmStatus

			self.toolTip.ClearToolTip()
			self.toolTip.AppendTextLine("|cffF1E6C0" + localeInfo.MULTI_FARM_TITLE)
			self.toolTip.AppendSpace(5)

			if multiFarmStatus:
				self.LoadImage("icon/affect/farm_on.png")
				self.SetScale(0.7, 0.7)
				self.toolTip.AppendTextLine(localeInfo.MULTI_FARM_TEXT_CAN_FARM)
				self.toolTip.AppendSpace(5)
				self.toolTip.AppendTextLine("|cffF1E6C0" + player.GetName())
			else:
				self.LoadImage("icon/affect/farm_off.png")
				self.SetScale(0.7, 0.7)
				self.toolTip.AppendTextLine(localeInfo.MULTI_FARM_TEXT_CANT_FARM)

		def OnMouseOverIn(self):
			self.toolTip.ShowToolTip()

		def OnMouseOverOut(self):
			self.toolTip.HideToolTip()

if app.ENABLE_GROWTH_PET_SYSTEM:
	class GrowthPetImage(ui.ExpandedImageBox):
		def SetScale(self, x, y):
			import advanced_options_runtime
			factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
			ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)

		def __init__(self):
			ui.ExpandedImageBox.__init__(self)

			if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
				self.__tool_tip = uiToolTip.ToolTip()
				self.__tool_tip.HideToolTip()
			else:
				self.toolTipText = None
			self.description = None

		def __del__(self):
			ui.ExpandedImageBox.__del__(self)

		def SetToolTipText(self, text, x = 0, y = -19):
			if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
				if self.__tool_tip == None:
					return
				self.__tool_tip.ClearToolTip()
				self.__tool_tip.AppendTextLine(text)
				self.__tool_tip.OptimizeTooltipWindowSize()
				return

			if not self.toolTipText:
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetSize(0, 0)
				textLine.SetOutline()
				textLine.Hide()
				self.toolTipText = textLine

			self.toolTipText.SetText(text)
			w, h = self.toolTipText.GetTextSize()
			if localeInfo.IsARABIC():
				self.toolTipText.SetPosition(w+20, y)
			else:
				self.toolTipText.SetPosition(max(0, x + self.GetWidth()/2 - w/2), y)

		def SetDescription(self, description):
			self.description = description

		def OnMouseOverIn(self):
			if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
				if self.__tool_tip:
					self.__tool_tip.ShowToolTip()
			else:
				if self.toolTipText:
					self.toolTipText.Show()

		def OnMouseOverOut(self):
			if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
				if self.__tool_tip:
					self.__tool_tip.HideToolTip()
			else:
				if self.toolTipText:
					self.toolTipText.Hide()

if app.ENABLE_CONQUEROR_LEVEL:
	#Sungma
	class SungMaAffectImage(ui.ExpandedImageBox):		
		def SetScale(self, x, y):
			import advanced_options_runtime
			factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
			ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)
		FILE_PATH_SUNGMA = "d:/ymir work/ui/skill/common/affect/sungma_buff.sub"

		def __init__(self, str, hp, move, immune):
			ui.ExpandedImageBox.__init__(self)
			
			self.str = str
			self.hp = hp
			self.move = move
			self.immune = immune
			
			self.toolTip = uiToolTip.ToolTip(300)
			self.toolTip.HideToolTip()

		def __del__(self):
			ui.ExpandedImageBox.__del__(self)

		def SetSungmaValue(self, str, hp, move, immune):
			self.str = str
			self.hp = hp
			self.move = move
			self.immune = immune
			
			self.__Refresh()
		
		def __Refresh(self):
			self.str = self.str
			self.hp = self.hp
			self.move = self.move
			self.immune = self.immune
			
			fileName = self.FILE_PATH_SUNGMA
			
			try:
				self.LoadImage(fileName)
			except:
				import dbg
				dbg.TraceError("SungmaPointImage.(STR=%d, HP %d, MOVE %d, IMMUNE %d) - LoadError %s" % (self.str, self.hp, self.move, self.immune, fileName))

			self.SetScale(0.7, 0.7)

			self.toolTip.ClearToolTip()
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AFFECT_SUNGMA_DESC)
			self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_STR % (self.str))
			self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_HP % (self.hp))
			self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_MOVE % (self.move))
			self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AFFECT_SUNGMA_IMMUNE % (self.immune))
			self.toolTip.ResizeToolTip()
			
		def OnMouseOverIn(self):
			self.toolTip.ShowToolTip()

		def OnMouseOverOut(self):
			self.toolTip.HideToolTip()

class LovePointImage(ui.ExpandedImageBox):
	def SetScale(self, x, y):
		import advanced_options_runtime
		factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
		ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)

	FILE_PATH = "d:/ymir work/ui/pattern/LovePoint/"
	FILE_DICT = {
		0 : FILE_PATH + "01.dds",
		1 : FILE_PATH + "02.dds",
		2 : FILE_PATH + "02.dds",
		3 : FILE_PATH + "03.dds",
		4 : FILE_PATH + "04.dds",
		5 : FILE_PATH + "05.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetLoverInfo(self, name, lovePoint):
		self.loverName = name
		self.lovePoint = lovePoint
		self.__Refresh()

	def OnUpdateLovePoint(self, lovePoint):
		self.lovePoint = lovePoint
		self.__Refresh()

	def __Refresh(self):
		self.lovePoint = max(0, self.lovePoint)
		self.lovePoint = min(100, self.lovePoint)

		if 0 == self.lovePoint:
			loveGrade = 0
		else:
			loveGrade = self.lovePoint / 25 + 1
		fileName = self.FILE_DICT.get(loveGrade, self.FILE_PATH+"00.dds")

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("LovePointImage.SetLoverInfo(lovePoint=%d) - LoadError %s" % (self.lovePoint, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.__tool_tip != None:
				self.__tool_tip.ClearToolTip()
				self.__tool_tip.AppendTextLine(self.__tool_tip_text)
				self.__tool_tip.OptimizeTooltipWindowSize()
				self.__tool_tip.ShowToolTip()
		else:
			self.SetScale(0.8,0.8)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.__tool_tip != None:
				self.__tool_tip.HideToolTip()
		else:
			self.SetScale(0.7,0.7)
		self.toolTip.HideToolTip()

	def IsToolTipShow(self):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.__tool_tip:
				return self.__tool_tip.IsShow()
			return False
		if self.toolTip:
			return self.toolTip.IsShow()
		return False
# END_OF_WEDDING


class HorseImage(ui.ExpandedImageBox):
	def SetScale(self, x, y):
		import advanced_options_runtime
		factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
		ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)

	FILE_PATH = "d:/ymir work/ui/pattern/HorseState/"

	FILE_DICT = {
		00 : FILE_PATH+"00.dds",
		01 : FILE_PATH+"00.dds",
		02 : FILE_PATH+"00.dds",
		03 : FILE_PATH+"00.dds",
		10 : FILE_PATH+"10.dds",
		11 : FILE_PATH+"11.dds",
		12 : FILE_PATH+"12.dds",
		13 : FILE_PATH+"13.dds",
		20 : FILE_PATH+"20.dds",
		21 : FILE_PATH+"21.dds",
		22 : FILE_PATH+"22.dds",
		23 : FILE_PATH+"23.dds",
		30 : FILE_PATH+"30.dds",
		31 : FILE_PATH+"31.dds",
		32 : FILE_PATH+"32.dds",
		33 : FILE_PATH+"33.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __GetHorseGrade(self, level):
		if 0 == level:
			return 0

		return (level-1)/10 + 1

	def SetState(self, level, health, battery):
		self.toolTip.ClearToolTip()

		if level>0:

			try:
				grade = self.__GetHorseGrade(level)
				self.__AppendText(localeInfo.LEVEL_LIST[grade])
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			try:
				healthName=localeInfo.HEALTH_LIST[health]
				if len(healthName)>0:
					self.__AppendText(healthName)
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			if health>0:
				if battery==0:
					self.__AppendText(localeInfo.NEEFD_REST)

			try:
				fileName=self.FILE_DICT[health*10+battery]
			except KeyError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - KeyError" % (level, health, battery)

			try:
				self.LoadImage(fileName)
			except:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - LoadError %s" % (level, health, battery, fileName)

		self.SetScale(0.7, 0.7)

	def __AppendText(self, text):
		self.toolTip.AppendTextLine(text)
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.SetScale(0.8,0.8)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(0.7,0.7)
		self.toolTip.HideToolTip()

class AutoPotionImage(ui.ExpandedImageBox):
	def SetScale(self, x, y):
		import advanced_options_runtime
		factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
		ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)

	FILE_PATH_HP = "d:/ymir work/ui/pattern/auto_hpgauge/"
	FILE_PATH_SP = "d:/ymir work/ui/pattern/auto_spgauge/"

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0
		self.potionType = player.AUTO_POTION_TYPE_HP
		self.filePath = ""

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetPotionType(self, type):
		self.potionType = type

		if player.AUTO_POTION_TYPE_HP == type:
			self.filePath = self.FILE_PATH_HP
		elif player.AUTO_POTION_TYPE_SP == type:
			self.filePath = self.FILE_PATH_SP

	def OnUpdateAutoPotionImage(self):
		self.__Refresh()

	def __Refresh(self):
		print "__Refresh"

		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(self.potionType)

		amountPercent = (float(currentAmount) / totalAmount) * 100.0
		grade = math.ceil(amountPercent / 20)

		if 5.0 > amountPercent:
			grade = 0

		if 80.0 < amountPercent:
			grade = 4
			if 90.0 < amountPercent:
				grade = 5

		fmt = self.filePath + "%.2d.dds"
		fileName = fmt % grade

		print self.potionType, amountPercent, fileName

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("AutoPotionImage.__Refresh(potionType=%d) - LoadError %s" % (self.potionType, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()

		if player.AUTO_POTION_TYPE_HP == type:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_HP)
		else:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_SP)

		self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AUTO_POTION_REST	% (amountPercent))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.SetScale(0.8,0.8)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(0.7,0.7)
		self.toolTip.HideToolTip()

class AffectImage(ui.ExpandedImageBox):
	def SetScale(self, x, y):
		import advanced_options_runtime
		factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
		ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			self.__tool_tip_text = ""
			self.__tool_tip = self.toolTip

		self.isSkillAffect = TRUE
		self.description = None
		self.endTime = 0
		self.affect = None
		self.isClocked = TRUE
		if app.ENABLE_AFFECT_BUFF_REMOVE:
			self.buffQuestionDialog = None
			self.skillIndex = None
			self.SetEvent(ui.__mem_func__(self.OnBuffQuestionDialog), "mouse_click")

	def SetAffect(self, affect):
		self.affect = affect

	def GetAffect(self):
		return self.affect

	def FormatTime(self, time):
		text = ""

		d = time // (24 * 3600)
		time = time % (24 * 3600)
		h = time // 3600
		time %= 3600
		m = time // 60
		time %= 60
		s = time

		if d:
			text += "%dd " % d
		if text or h:
			text += "%dg " % h
		if text or m:
			text += "%dm " % m
		if text or s:
			text += "%ds " % s

		return text[:-1]

	def SetToolTipText(self, text, x = 0, y = -19):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			self.__tool_tip_text = text
		self.toolTip.ClearToolTip()
		self.toolTip.AppendSpace(-5)
		self.toolTip.AppendDescription(text, 35)
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			self.toolTip.OptimizeTooltipWindowSize()

	def SetDescription(self, description):
		self.description = description
		self.__UpdateDescription2()

	def SetDuration(self, duration):
		self.endTime = 0
		if duration > 0:
			self.endTime = app.GetGlobalTimeStamp() + duration
			leftTime = self.FormatTime(self.endTime - app.GetGlobalTimeStamp())
			self.toolTip.AppendTextLine("%s : %s" % (localeInfo.LEFT_TIME, leftTime))
			self.toolTip.ResizeToolTip()

	def SetSkillAffectFlag(self, flag):
		self.isSkillAffect = flag

	if app.ENABLE_AFFECT_BUFF_REMOVE:
		def SetSkillIndex(self, skillIndex):
			self.skillIndex = skillIndex

	def IsSkillAffect(self):
		return self.isSkillAffect

	if app.ENABLE_AFFECT_BUFF_REMOVE:
		def OnBuffQuestionDialog(self):
			skillIndex = self.skillIndex
			if not skillIndex or skillIndex == 66:
				return
			self.buffQuestionDialog = uiCommon.QuestionDialog()
			self.buffQuestionDialog.SetWidth(350)
			self.buffQuestionDialog.SetText(localeInfo.BUFF_AFFECT_REMOVE_QUESTION % (skill.GetSkillName(skillIndex)))
			self.buffQuestionDialog.SetAcceptEvent(lambda arg = skillIndex: self.OnCloseBuffQuestionDialog(arg))
			self.buffQuestionDialog.SetCancelEvent(lambda arg = 0: self.OnCloseBuffQuestionDialog(arg))
			self.buffQuestionDialog.Open()

		def OnCloseBuffQuestionDialog(self, answer):
			if not self.buffQuestionDialog:
				return

			self.buffQuestionDialog.Close()
			self.buffQuestionDialog = None

			if not answer:
				return

			net.SendChatPacket("/remove_buff %d" % answer)
			return TRUE

	def UpdateAutoPotionDescription(self):
		potionType = player.AUTO_POTION_TYPE_HP if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY\
			else player.AUTO_POTION_TYPE_SP
		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(potionType)

		try:
			amountPercent = (float(currentAmount) / totalAmount) * 100.0
		except:
			amountPercent = 100.0

		self.toolTip.childrenList[-1].SetText(self.description % amountPercent)

	def SetClock(self, isClocked):
		self.isClocked = isClocked
		self.SetDescription(self.description)

	def UpdateDescription(self):
		if not self.isClocked:
			return

		if not self.description:
			return

		if self.endTime > 0:
			leftTime = localeInfo.RTSecondToDHMS(self.endTime - app.GetGlobalTimeStamp())
			self.toolTip.childrenList[-1].SetText("%s : %s" % (localeInfo.LEFT_TIME, leftTime))

	def __UpdateDescription2(self):
		if not self.description:
			return

		toolTip = self.description
		self.SetToolTipText(toolTip, 0, 40)

	def OnMouseOverIn(self):
		self.SetScale(0.8,0.8)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(0.7,0.7)
		self.toolTip.HideToolTip()

class AffectShower(ui.Window):

	MALL_DESC_IDX_START = 1000

	if app.ENABLE_RENEWAL_AFFECT:
		WATER_DESC_IDX_START = 1100
		DEW_DESC_IDX_START = 1300
		DRAGON_GOD_DESC_IDX_START = 1500
		DEW_EX_DESC_IDX_START = 1700

	if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
		BASE_POS_X = 10
		BASE_POS_Y = 10

	IMAGE_STEP = 25

	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF
	RENEWAL_AFFECT_ICON = "icon/affect/"

	AFFECT_DATA_DICT =	{
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub"),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub"),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, RENEWAL_AFFECT_ICON + "affect_13.png"),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, RENEWAL_AFFECT_ICON + "affect_14.png"),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, RENEWAL_AFFECT_ICON + "affect_6.png"),

			chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub",),
			chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub",),
			chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub",),
			chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub",),
			chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub",),
			chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub",),
			chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub"),
			chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub",),
			chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub",),
			chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub",),
			chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub",),
			chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub",),
			chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub",),
			chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub",),
			chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub",),
			chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub",),
			chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub",),

			#64 - END
			chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_1.png"),

			chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_2.png"),
			chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, RENEWAL_AFFECT_ICON + "affect_8.png"),
			chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, RENEWAL_AFFECT_ICON + "affect_5.png"),
			chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, RENEWAL_AFFECT_ICON + "affect_4.png"),
			chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, RENEWAL_AFFECT_ICON + "affect_9.png"),
			chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_3.png"),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, RENEWAL_AFFECT_ICON + "affect_35.png"),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, RENEWAL_AFFECT_ICON + "affect_34.png"),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, RENEWAL_AFFECT_ICON + "affect_33.png"),

			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, RENEWAL_AFFECT_ICON + "affect_37.png"),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, RENEWAL_AFFECT_ICON + "affect_38.png"),

			MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, RENEWAL_AFFECT_ICON + "affect_1.png"),
			MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, RENEWAL_AFFECT_ICON + "affect_2.png"),
			MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, RENEWAL_AFFECT_ICON + "affect_3.png"),
			MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, RENEWAL_AFFECT_ICON + "affect_22.png"),
			MALL_DESC_IDX_START+player.POINT_ATT_BONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_23.png"),
			MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_25.png"),
			MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, RENEWAL_AFFECT_ICON + "affect_24.png"),
			MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_26.png"),
			MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_27.png"),

			AFF_LEADERSHIP: ("Liderlik", "d:/ymir work/ui/skill/common/affect/leadership.png"),
	}

	if app.ENABLE_RENEWAL_AFFECT:
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_40.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_41.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, RENEWAL_AFFECT_ICON + "affect_44.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_45.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_MAGIC_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_46.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, RENEWAL_AFFECT_ICON + "affect_47.png")

		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_15.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_16.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, RENEWAL_AFFECT_ICON + "affect_17.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, RENEWAL_AFFECT_ICON + "affect_18.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, RENEWAL_AFFECT_ICON + "affect_19.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_20.png")

		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_HP_PCT] = (localeInfo.TOOLTIP_MAX_HP_PCT, RENEWAL_AFFECT_ICON + "affect_22_p.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_ATT_BONUS] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_23_p.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MALL_DEFBONUS] = (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, RENEWAL_AFFECT_ICON + "affect_25_p.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_SP_PCT] = (localeInfo.TOOLTIP_MAX_SP_PCT, RENEWAL_AFFECT_ICON + "affect_24_p.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_26_p.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_27_p.png")

		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_15_p.png")
		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_16_p.png")
		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, RENEWAL_AFFECT_ICON + "affect_17_p.png")
		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, RENEWAL_AFFECT_ICON + "affect_18_p.png")
		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, RENEWAL_AFFECT_ICON + "affect_19_p.png")
		AFFECT_DATA_DICT[DEW_EX_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_20_p.png")

	if app.ENABLE_ATTENDANCE_EVENT:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_EXP_BONUS_EVENT] = (localeInfo.TOOLTIP_EXP_BONUS_EVENT, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub")
		AFFECT_DATA_DICT[chr.NEW_AFFECT_ATT_SPEED_SLOW] = (localeInfo.TOOLTIP_ATT_SPEED_SLOW, "d:/ymir work/ui/skill/common/affect/att_slow.sub")

	if app.ENABLE_MULTI_FARM_BLOCK:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_MULTI_FARM] = (localeInfo.MULTI_FARM_PREMIUM_EFFECT, RENEWAL_AFFECT_ICON + "premium_farm.png")

	if app.ENABLE_SUNG_MAHI_TOWER:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_SUNG_MAHI_CURSE] = (localeInfo.SUNG_MAHI_CURSE_TOOLTIP, "d:/ymir work/ui/skill/common/affect/sungmahee_tower_debuff.sub",)

	if app.ENABLE_RENEWAL_OFFLINESHOP:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DECORATION] = (localeInfo.OFFLINESHOP_AFFECT_DECORATION, RENEWAL_AFFECT_ICON + "shop_deco.png")

	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_PICK_UP] = (localeInfo.TOOLTIP_NEW_AFFECT_AUTO_PICK_UP, RENEWAL_AFFECT_ICON + "premium_pickup.png")

	if app.__AUTO_HUNT__:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_HUNT] =  (localeInfo.NEW_AFFECT_AUTO_HUNT, RENEWAL_AFFECT_ICON + "auto_hunt.tga")

	if app.ENABLE_CONQUEROR_LEVEL:
		AFFECT_DATA_DICT[chr.AFFECT_CHEONUN] = (localeInfo.SKILL_CHEONUN, "d:/ymir work/ui/skill/shaman/cheonun_03.sub")
		# AFFECT_DATA_DICT[43] = (localeInfo.SKILL_CHEONUN, "d:/ymir work/ui/skill/shaman/cheonun" + END_STRING + ".sub")

	def __init__(self):
		ui.Window.__init__(self)
		self.isShowingAffects = True

		self.serverPlayTime = 0
		self.clientPlayTime = 0

		self.showBtn = ui.Button()
		self.showBtn.SetParent(self)
		self.showBtn.SetUpVisual("d:/ymir work/ui/on_btn.png")
		self.showBtn.SetOverVisual("d:/ymir work/ui/off_btn.png")
		self.showBtn.SetDownVisual("d:/ymir work/ui/off_btn.png")
		self.showBtn.SetPosition(0, 0)
		self.showBtn.SetEvent(ui.__mem_func__(self.__OnClickToggleButton))
		self.showBtn.Show()

		self.lastUpdateTime = 0
		self.affectImageDict = {}
		self.horseImage = None
		self.lovePointImage = None
		# These must exist before SetLeaderShipStatus(): it adds an affect, which runs
		# __ArrangeImageList() and reads them.
		if app.ENABLE_CONQUEROR_LEVEL:
			self.SungmaImage = None

		if app.ENABLE_MULTI_FARM_BLOCK:
			self.multiFarmBlockDialog = None
			self.farmStatusImage = None

		self.SetLeaderShipStatus(False)

		if app.ENABLE_GROWTH_PET_SYSTEM:
			self.petSkillaffectImageDict = {}

		self.autoPotionImageHP = AutoPotionImage()
		self.autoPotionImageSP = AutoPotionImage()
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			self.SetPosition(self.BASE_POS_X, self.BASE_POS_Y)
			self.__minimap_width = 0
			self.__party_window = None
		else:
			self.SetPosition(10, 10)
		self.Show()

	def ClearAllAffects(self):
		self.horseImage = None
		self.lovePointImage = None
		if app.ENABLE_CONQUEROR_LEVEL:
			self.SungmaImage = None
		if app.ENABLE_MULTI_FARM_BLOCK:
			self.multiFarmBlockDialog = None
			self.farmStatusImage = None
		self.affectImageDict = {}
		self.__ArrangeImageList()

	if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
		def SetMiniMapWidth(self, width):
			self.__minimap_width = width

		def SetPartyWindow(self, wndParty):
			if wndParty == None:
				return
			import _weakref
			self.__party_window = _weakref.proxy(wndParty)

	def GetAffectData(self, affect_type):
		return self.AFFECT_DATA_DICT.get(affect_type, None)

	def __OnClickToggleButton(self):
			self.isShowingAffects = not self.isShowingAffects
			
			# If we are showing them again, we must make sure the images are actually 'Shown'
			for image in self.affectImageDict.values():
				if self.isShowingAffects:
					image.Show()
				else:
					image.Hide()
					
			if self.horseImage:
				self.horseImage.Show() if self.isShowingAffects else self.horseImage.Hide()
				
			self.__ArrangeImageList()

	def ClearAffects(self):
		self.living_affectImageDict={}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect():
				self.living_affectImageDict[key] = image
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()

	def SetLeaderShipStatus(self, status, pointIdx = 0, value = 0):
		if not self.affectImageDict.has_key(AFF_LEADERSHIP):
			self.BINARY_NEW_AddAffect(AFF_LEADERSHIP, 0, 0, 0)

		# AFF_LEADERSHIP key'i hala yoksa erken dönü?
		if not self.affectImageDict.has_key(AFF_LEADERSHIP):
			return

		ACTIVE_IMG = "d:/ymir work/ui/skill/common/affect/leadership.png"
		DEACTIVE_IMG = "d:/ymir work/ui/skill/common/affect/leadership.png"

		try:
			if self.affectImageDict[AFF_LEADERSHIP].toolTip == None:
				self.affectImageDict[AFF_LEADERSHIP].toolTip = uiToolTip.ToolTip()
				self.affectImageDict[AFF_LEADERSHIP].toolTip.HideToolTip()
			toolTip = self.affectImageDict[AFF_LEADERSHIP].toolTip
		except (AttributeError, KeyError):
			self.affectImageDict[AFF_LEADERSHIP].toolTip = uiToolTip.ToolTip()
			self.affectImageDict[AFF_LEADERSHIP].toolTip.HideToolTip()
			toolTip = self.affectImageDict[AFF_LEADERSHIP].toolTip

		toolTip.ClearToolTip()
		try:
			toolTip.SetTitle(localeInfo.LEADERSHIP_TITLE)
		except (AttributeError, NameError):
			toolTip.SetTitle("Liderlik")

		try:
			toolTip.AppendDescription(localeInfo.LEADERSHIP_DESCRIPTION, 26)
		except (AttributeError, NameError):
			pass

		toolTip.AppendSpace(5)
		toolTip.AppendTextLine("--------------------------")

		try:
			toolTip.AppendTextLine(localeInfo.LEADER_BONUS_RECIVED)
		except (AttributeError, NameError):
			toolTip.AppendTextLine("Alinan bonus:")

		if status == 1:
			self.affectImageDict[AFF_LEADERSHIP].LoadImage(ACTIVE_IMG)
			self.affectImageDict[AFF_LEADERSHIP].SetScale(0.7, 0.7)

			try:
				AFFECT_STRING_DICT = {
					91 : localeInfo.PARTY_BONUS_ATTACKER,
					92 : localeInfo.PARTY_BONUS_TANKER,
					103 : localeInfo.PARTY_BONUS_BUFFER,
					104 : localeInfo.PARTY_BONUS_SKILL_MASTER,
					110 : localeInfo.PARTY_BONUS_BERSERKER,
					111 : localeInfo.PARTY_BONUS_DEFENDER,
				}
				if AFFECT_STRING_DICT.has_key(pointIdx):
					toolTip.AppendTextLine(AFFECT_STRING_DICT[pointIdx](value))
			except:
				pass

			toolTip.AppendSpace(5)
			toolTip.AppendTextLine("--------------------------")
			toolTip.AppendTextLine("Aktif")

		elif status == 0:
			self.affectImageDict[AFF_LEADERSHIP].LoadImage(DEACTIVE_IMG)
			self.affectImageDict[AFF_LEADERSHIP].SetScale(0.7, 0.7)
			
			toolTip.AppendSpace(5)
			toolTip.AppendTextLine("--------------------------")
			toolTip.AppendTextLine("Pasif")

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):
		if type < 500:
			return

		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_MALL_PLUS:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_BLEND_PLUS:
			affect = self.DEW_EX_DESC_IDX_START + pointIdx
		else:
			affect = type

		if self.affectImageDict.has_key(affect):
			return

		if not self.AFFECT_DATA_DICT.has_key(affect):
			return

		if affect == chr.NEW_AFFECT_NO_DEATH_PENALTY or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_BONUS or\
		   affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or\
		   affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY or\
		   affect == chr.NEW_AFFECT_MULTI_FARM or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY:
			duration = 0

		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]
		filename = affectData[1]

		if pointIdx == player.POINT_MALL_ITEMBONUS or\
		   pointIdx == player.POINT_MALL_GOLDBONUS:
			value = 1 + float(value) / 100.0

		trashValue = 123
		#if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
		if trashValue == 1:
			try:
				#image = AutoPotionImage()
				#image.SetParent(self)
				image = None

				if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
					image.SetPotionType(player.AUTO_POTION_TYPE_SP)
					image = self.autoPotionImageSP
					#self.autoPotionImageSP = image;
				else:
					image.SetPotionType(player.AUTO_POTION_TYPE_HP)
					image = self.autoPotionImageHP
					#self.autoPotionImageHP = image;

				image.SetParent(self)
				image.Show()
				image.OnUpdateAutoPotionImage()

				self.affectImageDict[affect] = image
				self.__ArrangeImageList()

			except Exception, e:
				print "except Aff auto potion affect ", e
				pass

		else:
			if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				# description bir string veya fonksiyon olabilir
				if isinstance(description, str) or isinstance(description, unicode):
					# Eðer description bir string ise, format string olarak kullan
					try:
						# Format placeholder kontrolü (%d, %s, %f, %%, vb.)
						if '%' in description:
							# Placeholder sayýsýný kontrol et
							placeholder_count = description.count('%') - description.count('%%')
							if placeholder_count > 0:
								description = description % float(value)
							else:
								# Placeholder yoksa, value'yu ekle
								description = description + " " + str(float(value))
						else:
							# Format placeholder yoksa, value'yu ekle
							description = description + " " + str(float(value))
					except (TypeError, ValueError):
						# Format hatasý olursa, sadece string'i kullan
						description = str(description)
				else:
					# Eðer description bir fonksiyon ise çaðýr
					description = description(float(value))

			try:
				print "Add affect %s" % affect
				image = AffectImage()
				image.SetParent(self)
				image.LoadImage(filename)
				image.SetDescription(description)
				image.SetDuration(duration)
				image.SetAffect(affect)
				if affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE or\
					affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 or\
					self.INFINITE_AFFECT_DURATION < duration:
					image.SetClock(FALSE)
					image.UpdateDescription()
				elif app.ENABLE_ATTENDANCE_EVENT and affect == chr.NEW_AFFECT_ATT_SPEED_SLOW:
					image.SetClock(FALSE)
					image.UpdateDescription()
				elif affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
					image.UpdateAutoPotionDescription()
				else:
					image.UpdateDescription()

				image.SetScale(0.7, 0.7)
				image.SetSkillAffectFlag(FALSE)
				image.Show()
				self.affectImageDict[affect] = image
				self.__ArrangeImageList()
			except Exception, e:
				print "except Aff affect ", e
				pass

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_MALL_PLUS:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif app.ENABLE_RENEWAL_AFFECT and type == chr.NEW_AFFECT_BLEND_PLUS:
			affect = self.DEW_EX_DESC_IDX_START + pointIdx
		else:
			affect = type

		if affect == AFF_LEADERSHIP:
			self.SetLeaderShipStatus(False)
			return

		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetAffect(self, affect):
		self.__AppendAffect(affect)
		self.__ArrangeImageList()

	def ResetAffect(self, affect):
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetLoverInfo(self, name, lovePoint):
		image = LovePointImage()
		image.SetParent(self)
		image.SetLoverInfo(name, lovePoint)
		self.lovePointImage = image
		self.__ArrangeImageList()

	def ShowLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Show()
			self.__ArrangeImageList()

	def HideLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Hide()
			self.__ArrangeImageList()

	def ClearLoverState(self):
		self.lovePointImage = None
		self.__ArrangeImageList()

	def OnUpdateLovePoint(self, lovePoint):
		if self.lovePointImage:
			self.lovePointImage.OnUpdateLovePoint(lovePoint)

	def SetHorseState(self, level, health, battery):
		if level==0:
			self.horseImage=None
		else:
			image = HorseImage()
			image.SetParent(self)
			image.SetState(level, health, battery)
			image.Show()

			self.horseImage=image
		self.__ArrangeImageList()

	if app.ENABLE_CONQUEROR_LEVEL:
		def SetSungMaAffectImage(self, str, hp, move, immune):
			image = SungMaAffectImage(str, hp, move, immune)
			image.SetParent(self)
			image.SetSungmaValue(str, hp, move, immune)
			image.Show()

			self.SungmaImage=image
			self.__ArrangeImageList()

	def SetPlayTime(self, playTime):
		self.serverPlayTime = playTime
		self.clientPlayTime = app.GetTime()

	def __AppendAffect(self, affect):
		if self.affectImageDict.has_key(affect):
			return

		try:
			affectData = self.AFFECT_DATA_DICT[affect]
		except KeyError:
			return

		name = affectData[0]
		filename = affectData[1]

		skillIndex = player.AffectIndexToSkillIndex(affect)
		if 0 != skillIndex:
			name = skill.GetSkillName(skillIndex)

		image = AffectImage()
		image.SetParent(self)
		image.SetSkillAffectFlag(TRUE)

		if app.ENABLE_AFFECT_BUFF_REMOVE:
			if skillIndex == 94 or skillIndex == 95 or skillIndex == 96 or skillIndex == 109 or skillIndex == 110 or skillIndex == 111:
				image.SetSkillIndex(skillIndex)

		try:
			image.LoadImage(filename)
		except:
			pass

		image.SetToolTipText(name, 0, 40)
		image.SetScale(0.7, 0.7)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		if not self.affectImageDict.has_key(affect):
			print "__RemoveAffect %s ( No Affect )" % affect
			return

		print "__RemoveAffect %s ( Affect )" % affect
		del self.affectImageDict[affect]

		self.__ArrangeImageList()

	def ApplyIconSize(self, small):
		self.IMAGE_STEP = 19 if small else 25
		images = list(self.affectImageDict.values())
		for name in ("lovePointImage", "horseImage", "farmStatusImage", "SungmaImage"):
			image = getattr(self, name, None)
			if image: images.append(image)
		for image in images: image.SetScale(0.7, 0.7)
		self.__ArrangeImageList()

	def __ArrangeImageList(self):
		import advanced_options_runtime
		self.IMAGE_STEP = 19 if advanced_options_runtime.Get("small_affect_icon", False) else 25
		self.showBtn.SetPosition(-3, -3)

		if not self.isShowingAffects:
			self.SetSize(32, 32)
			if self.lovePointImage: self.lovePointImage.Hide()
			if self.horseImage: self.horseImage.Hide()
			if app.ENABLE_MULTI_FARM_BLOCK and self.farmStatusImage: self.farmStatusImage.Hide()
			for image in self.affectImageDict.values(): image.Hide()
			return

		width = len(self.affectImageDict) * self.IMAGE_STEP
		if self.lovePointImage:
			width += self.IMAGE_STEP
		if app.ENABLE_MULTI_FARM_BLOCK and self.farmStatusImage:
			width += self.IMAGE_STEP
		if self.horseImage:
			width += self.IMAGE_STEP
		if app.ENABLE_CONQUEROR_LEVEL and self.SungmaImage:
			width += self.IMAGE_STEP

		if not app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			self.SetSize(width + 22, 26)

		xPos = 22
		yPos = 0

		if self.lovePointImage:
			if self.lovePointImage.IsShow():
				self.lovePointImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

		if app.ENABLE_MULTI_FARM_BLOCK and self.farmStatusImage:
			self.farmStatusImage.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP

		if self.horseImage:
			self.horseImage.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP

		if app.ENABLE_CONQUEROR_LEVEL and self.SungmaImage:
			if self.SungmaImage.IsShow():
				self.SungmaImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			add_count = xPos // self.IMAGE_STEP
			height = self.IMAGE_STEP
			window_width = wndMgr.GetScreenWidth()
			line_limit_width = window_width - self.__minimap_width * 2
			line_limit_count = 0
			if line_limit_width > 0:
				line_limit_count = line_limit_width // self.IMAGE_STEP

			for index, (key, image) in enumerate(self.affectImageDict.items(), start=1):
				image.SetPosition(xPos, yPos)
				image.Show()
				xPos += self.IMAGE_STEP
				if index == len(self.affectImageDict):
					break
				if line_limit_count > 0:
					if (add_count + index) % line_limit_count == 0:
						xPos = 22
						yPos += self.IMAGE_STEP
						height += self.IMAGE_STEP

			if self.__party_window:
				self.__party_window.SetWindowPosY(self.BASE_POS_Y + height)

			self.SetSize(width + 22, height)
		else:
			for image in self.affectImageDict.values():
				image.SetPosition(xPos, 0)
				image.Show()
				xPos += self.IMAGE_STEP

			self.SetSize(width + 22, 26)

	if app.ENABLE_MULTI_FARM_BLOCK:
		def __OnClickMultiFarm(self, emptyArg):
			multiFarmBlockDialog = uiCommon.QuestionDialog("thin")

			if self.farmStatusImage.multiFarmStatus:
				multiFarmBlockDialog.SetText(localeInfo.MULTI_FARM_DEACTIVE_TEXT)
			else:
				multiFarmBlockDialog.SetText(localeInfo.MULTI_FARM_ACTIVE_TEXT)

			multiFarmBlockDialog.SetAcceptEvent(lambda arg = TRUE: self.OnCloseMultiFarm(arg))
			multiFarmBlockDialog.SetCancelEvent(lambda arg = FALSE: self.OnCloseMultiFarm(arg))
			multiFarmBlockDialog.Open()
			self.multiFarmBlockDialog = multiFarmBlockDialog

		def OnCloseMultiFarm(self, answer):
			if not self.multiFarmBlockDialog:
				return

			if answer:
				net.SendChatPacket("/multi_farm")

			self.multiFarmBlockDialog.Close()
			self.multiFarmBlockDialog = None

		def SetMultiFarmPlayer(self, playerName):
			if self.farmStatusImage:
				self.farmStatusImage.players.append(playerName)

		def SetMultiFarmInfo(self, farmStatus):
			image = MultiFarmImage()
			image.SetParent(self)
			image.SetMultiFarmInfo(farmStatus)
			image.SetEvent(ui.__mem_func__(self.__OnClickMultiFarm), "mouse_click")
			image.Show()
			self.farmStatusImage = image
			self.__ArrangeImageList()

	if app.ENABLE_GROWTH_PET_SYSTEM:
		def SetPetSkillAffect(self, index, affect):

			if self.__AppendPetSkillAffect(index, affect):
				self.__ArrangeImageList()

		def ClearPetSkillAffect(self):
			self.petSkillaffectImageDict.clear()
			self.__ArrangeImageList()

		def __AppendPetSkillAffect(self, index, affect):

			if self.petSkillaffectImageDict.has_key(affect):
				return FALSE

			filename = skill.GetPetSkillIconPath(affect)
			if "" == filename:
				return FALSE

			(pet_skill_name, pet_skill_desc, pet_skill_use_type, pet_skill_cool_time) = skill.GetPetSkillInfo(affect)

			image = GrowthPetImage()
			image.SetParent(self)

			try:
				image.LoadImage(filename)
			except:
				print "LoadImage Except nn"
				return FALSE

			image.SetToolTipText(pet_skill_name, 0, 40)
			image.SetDescription(pet_skill_desc)
			image.SetScale(0.7, 0.7)
			image.Show()

			self.petSkillaffectImageDict[affect] = [index, image]

			return TRUE

	def OnUpdate(self):
		try:
			if app.GetGlobalTime() - self.lastUpdateTime > 500:
			#if 0 < app.GetGlobalTime():
				self.lastUpdateTime = app.GetGlobalTime()

				for image in self.affectImageDict.values():
					if image.GetAffect() == chr.NEW_AFFECT_AUTO_HP_RECOVERY or image.GetAffect() == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
						image.UpdateAutoPotionDescription()
						continue

					if not image.IsSkillAffect():
						image.UpdateDescription()
		except Exception, e:
			print "AffectShower::OnUpdate error : ", e

