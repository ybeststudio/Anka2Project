if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import guild
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
		"""Reference uiAffectShower.MultiFarmImage: title, click hint and a Status / Name table of the farm slots."""
		TOOLTIP_WIDTH = 350		# measured on the reference tooltip
		TABLE_MARGIN = 27
		ROW_HEIGHT = 22
		BASE_SLOT_COUNT = 2		# server CHARACTER_MANAGER::CheckMultiFarmAccount limit, + NEW_AFFECT_MULTI_FARM value
		DISABLED_ICON = "d:/ymir work/ui/game/wiki/dungeon/no.png"

		def SetScale(self, x, y):
			import advanced_options_runtime
			factor = 0.75 if advanced_options_runtime.Get("small_affect_icon", False) else 1.0
			ui.ExpandedImageBox.SetScale(self, x*factor, y*factor)
		def __del__(self):
			ui.ExpandedImageBox.__del__(self)

		def Destroy(self):
			self.multiFarmStatus = 0
			self.players = []
			self.premiumSlots = 0

		def __init__(self):
			ui.ExpandedImageBox.__init__(self)

			self.toolTip = uiToolTip.ToolTip(self.TOOLTIP_WIDTH)
			self.toolTip.HideToolTip()

			self.Destroy()

		def SetMultiFarmInfo(self, multiFarmStatus):
			self.multiFarmStatus = multiFarmStatus
			self.players = []	# the server sends UpdateMultiFarmPlayer for every farming character right after
			self.LoadImage("d:/ymir work/ui/game/affect_icon/farmeron.png" if multiFarmStatus else "d:/ymir work/ui/game/affect_icon/farmeroff.png")
			self.SetScale(1.0, 1.0)

		def AddPlayer(self, playerName):
			if playerName not in self.players:
				self.players.append(playerName)
			if self.toolTip.IsShow():
				self.__BuildToolTip()

		def SetPremiumSlots(self, count):
			self.premiumSlots = max(0, count)

		def __AddBar(self, x, y, width, height, color):
			bar = ui.Bar()
			bar.SetParent(self.toolTip)
			bar.SetPosition(x, y)
			bar.SetSize(width, height)
			bar.SetColor(color)
			bar.AddFlag("not_pick")
			bar.Show()
			self.toolTip.childrenList.append(bar)

		def __AddCellText(self, text, x, y, color):
			textLine = ui.TextLine()
			textLine.SetParent(self.toolTip)
			textLine.SetFontName(self.toolTip.defFontName)
			textLine.SetPackedFontColor(color)
			textLine.SetText(text)
			textLine.SetOutline()
			textLine.SetHorizontalAlignCenter()
			textLine.SetPosition(x, y + 4)
			textLine.Show()
			self.toolTip.childrenList.append(textLine)

		def __TextWidth(self, text):
			textLine = ui.TextLine()
			textLine.SetFontName(self.toolTip.defFontName)
			textLine.SetText(text)
			return textLine.GetTextSize()[0]

		def __BuildToolTip(self):
			toolTip = self.toolTip
			toolTip.ClearToolTip()
			toolTip.Base.SetColor(0xf0141210)	# reference tooltip is opaque, the affect icons must not show through
			toolTip.SetTitle(localeInfo.MULTI_FARM_BLOCK_TITLE)
			toolTip.AppendSpace(4)

			hint = localeInfo.MULTI_FARM_BLOCK_TEXT
			# wrap by pixel width (the emoji tag is not text); continuation lines start under the first word
			textLeft = 10
			maxWidth = self.TOOLTIP_WIDTH - textLeft * 2
			words = hint.split(" ")
			# the |E..|e tag is not measured as text: the key_lclick emoji is 17px plus one space
			emoji = ""
			if words and words[0].startswith("|E"):
				emoji = words.pop(0)
			emojiOffset = (17 + self.__TextWidth(" ")) if emoji else 0
			lines = []
			line = ""
			for word in words:
				candidate = (line + " " + word) if line else word
				if line and self.__TextWidth(candidate) > maxWidth - emojiOffset:
					lines.append(line)
					line = word
				else:
					line = candidate
			if line:
				lines.append(line)
			for (index, text) in enumerate(lines):
				if index == 0 and emoji:
					text = emoji + " " + text
				textLine = toolTip.AppendTextLine(text, toolTip.FONT_COLOR, FALSE)
				textLine.SetPosition(textLeft if index == 0 else textLeft + emojiOffset, textLine.GetLocalPosition()[1])

			toolTip.AppendSpace(8)

			left = self.TABLE_MARGIN
			width = self.TOOLTIP_WIDTH - self.TABLE_MARGIN * 2
			half = width / 2
			top = toolTip.toolTipHeight
			slotCount = max(self.BASE_SLOT_COUNT + self.premiumSlots, len(self.players))
			rowCount = slotCount + 1
			height = rowCount * self.ROW_HEIGHT

			self.__AddBar(left, top, width, self.ROW_HEIGHT, 0xc0151515)	# header background
			self.__AddBar(left, top + self.ROW_HEIGHT, width, height - self.ROW_HEIGHT, 0x60000000)
			for row in xrange(rowCount + 1):
				self.__AddBar(left, top + row * self.ROW_HEIGHT, width, 1, 0xff505050)
			for x in (left, left + half, left + width - 1):
				self.__AddBar(x, top, 1, height + 1, 0xff505050)

			statusName = localeInfo.MULTI_FARM_STATUS_ALL
			self.__AddCellText(localeInfo.MULTI_FARM_STATUS, left + half / 2, top, 0xffffffff)
			self.__AddCellText(localeInfo.MULTI_FARM_NAME, left + half + half / 2, top, 0xffffffff)
			for index in xrange(slotCount):
				y = top + (index + 1) * self.ROW_HEIGHT
				self.__AddCellText(statusName, left + half / 2, y, 0xffd0d0d0)
				if index < len(self.players):
					self.__AddCellText(self.players[index], left + half + half / 2, y, 0xffd0d0d0)
				else:
					icon = ui.ImageBox()
					icon.SetParent(toolTip)
					icon.LoadImage(self.DISABLED_ICON)
					icon.SetPosition(left + half + half / 2 - icon.GetWidth() / 2, y + (self.ROW_HEIGHT - icon.GetHeight()) / 2)
					icon.AddFlag("not_pick")
					icon.Show()
					toolTip.childrenList.append(icon)

			toolTip.toolTipHeight = top + height + 10
			toolTip.ResizeToolTip()

		def OnMouseOverIn(self):
			self.__BuildToolTip()
			self.toolTip.ShowToolTip()

		def OnMouseOverOut(self):
			self.toolTip.HideToolTip()

		def OnUpdate(self):
			if self.toolTip.IsShow() and not self.IsIn():
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
				FitBoardToolTip(self.__tool_tip)
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

			self.SetScale(1.0, 1.0)

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

		self.SetScale(1.0, 1.0)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.__tool_tip != None:
				self.__tool_tip.ClearToolTip()
				self.__tool_tip.AppendTextLine(self.__tool_tip_text)
				FitBoardToolTip(self.__tool_tip)
				self.__tool_tip.ShowToolTip()
		else:
			self.SetScale(1.0, 1.0)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL:
			if self.__tool_tip != None:
				self.__tool_tip.HideToolTip()
		else:
			self.SetScale(1.0, 1.0)
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


class BenedictionToolTip(uiToolTip.ToolTip):
	"""Reference guild blessing tooltip: six blessings with activator and duration."""

	WIDTH = 430
	TABLE_WIDTH = 310
	COLUMNS = ((0, 50, "GUILD_BLESSING_COLUMN_TYPE"), (50, 120, "GUILD_BLESSING_COLUMN_BONUS"), (170, 80, "GUILD_BLESSING_COLUMN_ACTIVATOR"), (250, 60, "GUILD_BLESSING_COLUMN_DURATION"))
	ICONS = ("exp_bene", "drop_bene", "yang_bene", "double_stone_bene", "double_boss_bene", "double_fish_mine_bene")
	HEAD_HEIGHT = 26
	ROW_HEIGHT = 38

	def __init__(self):
		uiToolTip.ToolTip.__init__(self, self.WIDTH)
		self.AppendTextLine(localeInfo.GUILD_BLESSING_TOOLTIP_TITLE, self.TITLE_COLOR)
		self.AppendTextLine(localeInfo.GUILD_BLESSING_TOOLTIP_DESC, 0xffc1c1c1)
		left = (self.WIDTH - self.TABLE_WIDTH) / 2
		top = self.toolTipHeight + 5
		self.__Bar(left, top, self.TABLE_WIDTH, self.HEAD_HEIGHT, 0xFF2D1C11)
		for (x, width, key) in self.COLUMNS:
			self.__Text(getattr(localeInfo, key), left + x + width / 2, top + 6, self.TITLE_COLOR, "Tahoma:12b")
		y = top + self.HEAD_HEIGHT
		for (index, icon) in enumerate(self.ICONS):
			self.__Bar(left, y, self.TABLE_WIDTH, self.ROW_HEIGHT - 1, 0xFF1F1511)
			self.__Bar(left, y + self.ROW_HEIGHT - 1, self.TABLE_WIDTH, 1, 0xFF756657)
			for (x, width, key) in self.COLUMNS[1:]:
				self.__Bar(left + x, y, 1, self.ROW_HEIGHT - 1, 0xFF756657)
			image = ui.ExpandedImageBox()
			image.SetParent(self)
			image.AddFlag("not_pick")
			image.LoadImage("icon/item/item_benediction/%s.png" % icon)
			image.SetPosition(left + 9, y + 3)
			image.Show()
			self.childrenList.append(image)
			self.__Text(getattr(localeInfo, "GUILD_BLESSING_ROW_%d" % index), left + 110, y + 12, 0xffc1c1c1)
			self.__Text("X", left + 210, y + 11, 0xffff3030, "Tahoma:14b")
			self.__Text("X", left + 280, y + 11, 0xffff3030, "Tahoma:14b")
			y += self.ROW_HEIGHT
		self.toolTipHeight = y + 2
		self.ResizeToolTip()

	def __Bar(self, x, y, width, height, color):
		bar = ui.Bar()
		bar.SetParent(self)
		bar.AddFlag("not_pick")
		bar.SetPosition(x, y)
		bar.SetSize(width, height)
		bar.SetColor(color)
		bar.Show()
		self.childrenList.append(bar)

	def __Text(self, text, x, y, color, font = "Tahoma:12"):
		line = ui.TextLine()
		line.SetParent(self)
		line.SetFontName(font)
		line.SetHorizontalAlignCenter()
		line.SetPackedFontColor(color)
		line.SetOutline()
		line.SetText(text)
		line.SetPosition(x, y)
		line.Show()
		self.childrenList.append(line)


class BenedictionImage(ui.ExpandedImageBox):
	"""Reference guild blessing icon of the affect row."""

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.LoadImage("d:/ymir work/ui/game/affect_icon/empire_buff.png")
		self.toolTip = BenedictionToolTip()
		self.toolTip.HideToolTip()
		self.Show()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()
		return True

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
		return True


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

		self.SetScale(1.0, 1.0)

	def __AppendText(self, text):
		self.toolTip.AppendTextLine(text)
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.SetScale(1.0, 1.0)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(1.0, 1.0)
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

		self.SetScale(1.0, 1.0)

		self.toolTip.ClearToolTip()

		if player.AUTO_POTION_TYPE_HP == type:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_HP)
		else:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_SP)

		self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AUTO_POTION_REST	% (amountPercent))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.SetScale(1.0, 1.0)
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(1.0, 1.0)
		self.toolTip.HideToolTip()

SKILL_POINT_TEXT_DICT = {
	17 : "TOOLTIP_ATT_SPEED",				# POINT_ATT_SPEED
	19 : "TOOLTIP_MOV_SPEED",				# POINT_MOV_SPEED
	21 : "TOOLTIP_CAST_SPEED",				# POINT_CASTING_SPEED
	40 : "TOOLTIP_APPLY_CRITICAL_PCT",		# POINT_CRITICAL_PCT
	41 : "TOOLTIP_APPLY_PENETRATE_PCT",		# POINT_PENETRATE_PCT
	79 : "TOOLTIP_APPLY_REFLECT_MELEE",		# POINT_REFLECT_MELEE
	95 : "TOOLTIP_ATT_GRADE",				# POINT_ATT_GRADE_BONUS
	96 : "TOOLTIP_DEF_GRADE",				# POINT_DEF_GRADE_BONUS
	97 : "TOOLTIP_MAGIC_ATT_GRADE",			# POINT_MAGIC_ATT_GRADE_BONUS
	99 : "TOOLTIP_NORMAL_HIT_DEFEND_BONUS",	# POINT_RESIST_NORMAL_DAMAGE
}

def FormatSkillPointText(pointIdx, value):
	fmt = getattr(localeInfo, SKILL_POINT_TEXT_DICT.get(pointIdx, ""), None)
	if fmt is None:
		return None
	try:
		if callable(fmt):
			return fmt(value)
		return fmt % value
	except Exception:
		return None

# reference uiAffectShower.SKILL_CAST_STAT_LOCALE_DICT: caster stat shown under the bonus lines
SKILL_CAST_STAT_LOCALE_DICT = {
	3 : "STR on activation: %d", 4 : "STR on activation: %d", 19 : "STR on activation: %d",
	63 : "INT on activation: %d", 64 : "INT on activation: %d", 65 : "INT on activation: %d",
	79 : "INT on activation: %d", 94 : "INT on activation: %d", 95 : "INT on activation: %d",
	96 : "INT on activation: %d", 110 : "INT on activation: %d", 111 : "INT on activation: %d",
}

def StripEmoji(text):
	"""LEFT_TIME carries an |E...|e emoji prefix; the reference tooltip shows the plain text."""
	while "|E" in text and "|e" in text[text.index("|E"):]:
		i = text.index("|E")
		j = text.index("|e", i) + 2
		text = text[:i] + text[j:]
	return text.strip()

def FormatAffectLeftTime(seconds):
	seconds = max(0, int(seconds))
	(h, m, s) = (seconds // 3600, seconds % 3600 // 60, seconds % 60)
	if h:
		return "%dh %dm %ds" % (h, m, s)
	if m:
		return "%dm %ds" % (m, s)
	return "%ds" % s

def AppendWrappedText(toolTip, text, limit):
	"""Word wrap for affect tooltips (grpText splits in the middle of words)."""
	line = ""
	for word in (text if isinstance(text, basestring) else str(text)).split(" "):
		if line and len(line) + 1 + len(word) > limit:
			toolTip.AppendTextLine(line)
			line = word
		else:
			line = (line + " " + word) if line else word
	if line:
		toolTip.AppendTextLine(line)

def FitAffectToolTip(toolTip):
	FitBoardToolTip(toolTip)

def FitBoardToolTip(toolTip, minWidth = 120):
	"""Board width from the longest line, every line re-centred on the new width."""
	oldCenter = toolTip.toolTipWidth / 2
	needWidth = 0
	for child in toolTip.childrenList:
		if isinstance(child, ui.TextLine):
			x = child.GetLocalPosition()[0]
			width = child.GetTextSize()[0]
			needWidth = max(needWidth, width + 24 if x == oldCenter else x + width + 14)
	toolTip.toolTipWidth = max(needWidth, minWidth)
	toolTip.ResizeToolTip()
	for child in toolTip.childrenList:
		if isinstance(child, ui.TextLine):
			(x, y) = child.GetLocalPosition()
			if x == oldCenter:	# centred lines sit on the old width / 2
				child.SetPosition(toolTip.toolTipWidth / 2, y)

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
		AppendWrappedText(self.toolTip, text, 38)
		FitAffectToolTip(self.toolTip)

	def SetDescription(self, description):
		self.description = description
		self.__UpdateDescription2()

	def SetDuration(self, duration):
		self.endTime = 0
		if duration > 0:
			self.endTime = app.GetGlobalTimeStamp() + duration
			leftTime = self.FormatTime(self.endTime - app.GetGlobalTimeStamp())
			self.toolTip.AppendTextLine("%s : %s" % (localeInfo.LEFT_TIME, leftTime))
			FitAffectToolTip(self.toolTip)

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

	def SetSkillToolTip(self, name, skillIndex, pointDict, castStat = 0):
		"""pointDict: {pointIdx : (value, endTime)} from the server affect packets of this skill."""
		self.skillToolTipData = (name, skillIndex, pointDict, castStat)
		self.RefreshSkillToolTip()

	def RefreshSkillToolTip(self):
		data = getattr(self, "skillToolTipData", None)
		if not data:
			return
		(name, skillIndex, pointDict, castStat) = data
		toolTip = self.toolTip
		toolTip.ClearToolTip()
		toolTip.SetTitle(name)
		endTime = 0
		lines = []
		for pointIdx in sorted(pointDict.keys()):
			(value, pointEnd) = pointDict[pointIdx]
			endTime = max(endTime, pointEnd)
			text = FormatSkillPointText(pointIdx, value)
			if text:
				lines.append(text)
		toolTip.AppendSpace(6)
		for text in lines:
			toolTip.AppendTextLine(text, toolTip.POSITIVE_COLOR)
		if castStat and SKILL_CAST_STAT_LOCALE_DICT.has_key(skillIndex):
			toolTip.AppendTextLine(SKILL_CAST_STAT_LOCALE_DICT[skillIndex] % castStat, toolTip.SPECIAL_TITLE_COLOR)
		if endTime > 0:
			toolTip.AppendSpace(6)
			toolTip.AppendTextLine("%s: %s" % (StripEmoji(localeInfo.LEFT_TIME), FormatAffectLeftTime(endTime - app.GetGlobalTimeStamp())))
		if app.ENABLE_AFFECT_BUFF_REMOVE and getattr(self, "skillIndex", None):
			toolTip.AppendSpace(6)
			toolTip.AppendTextLine(localeInfo.AFFECT_REMOVE_HINT)
		FitBoardToolTip(toolTip, 230)

	def OnUpdate(self):
		# the tooltip must never outlive the hover (fast mouse moves, icon re-arranged under the cursor)
		if self.toolTip.IsShow() and not self.IsIn():
			self.toolTip.HideToolTip()
			return
		if getattr(self, "skillToolTipData", None) and self.toolTip.IsShow():
			now = app.GetGlobalTime()
			if now - getattr(self, "skillToolTipTime", 0) >= 1000:
				self.skillToolTipTime = now
				self.RefreshSkillToolTip()

	def OnMouseOverIn(self):
		self.SetScale(1.0, 1.0)
		self.RefreshSkillToolTip()
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.SetScale(1.0, 1.0)
		self.toolTip.HideToolTip()

class CombinedAffectImage(AffectImage):
	"""One icon for a whole potion group; the tooltip lists every active potion with its item icon."""
	TEXT_X_OFFSET = 50
	ICON_SIZE = 32
	TOOLTIP_WIDTH = 230

	COMBAT_TIMER_PAUSED = -1	# server ENABLE_AFFECT_COMBAT_TIMER: affect packet castStat while the potion clock is stopped
	UNLIMITED = -1				# endTime of a potion without duration

	def __init__(self, title, iconPath, showCombatText = FALSE):
		AffectImage.__init__(self)
		self.title = title
		self.showCombatText = showCombatText
		self.members = {}	# affect -> (order, itemVnum, description, endTime, pausedLeft or None)
		self.LoadImage(iconPath)
		self.SetScale(1.0, 1.0)
		self.SetSkillAffectFlag(FALSE)

	def SetMember(self, affect, order, itemVnum, description, endTime, pausedLeft = None):
		self.members[affect] = (order, itemVnum, description, endTime, pausedLeft)
		self.RefreshCombinedToolTip()

	def RemoveMember(self, affect):
		self.members.pop(affect, None)
		self.RefreshCombinedToolTip()

	def IsEmpty(self):
		return not self.members

	def RefreshCombinedToolTip(self):
		toolTip = self.toolTip
		toolTip.ClearToolTip()
		toolTip.SetTitle(self.title)
		toolTip.AppendSpace(6)
		now = app.GetGlobalTimeStamp()
		for (affect, (order, itemVnum, description, endTime, pausedLeft)) in sorted(self.members.items(), key = lambda x: x[1][0]):
			name = ""
			iconPath = ""
			if itemVnum:
				item.SelectItem(itemVnum)
				name = item.GetItemName()
				iconPath = item.GetIconImageFileName()
			top = toolTip.toolTipHeight
			if iconPath:
				icon = ui.ImageBox()
				icon.SetParent(toolTip)
				try:
					icon.LoadImage(iconPath)
				except:
					pass
				icon.SetPosition(10, top)
				icon.Show()
				toolTip.childrenList.append(icon)
			lines = []
			if name:
				lines.append((name, 0xffffffff))
			lines.append((description, toolTip.POSITIVE_COLOR))
			if endTime == self.UNLIMITED:
				lines.append((localeInfo.TOOLTIP_POTION_UNLIMITED, toolTip.FONT_COLOR))
			elif pausedLeft is not None:
				lines.append(("%s: %s" % (StripEmoji(localeInfo.LEFT_TIME), localeInfo.SecondToHM(max(0, pausedLeft))), toolTip.FONT_COLOR))
			elif endTime > 0:
				lines.append(("%s: %s" % (StripEmoji(localeInfo.LEFT_TIME), localeInfo.SecondToHM(max(0, endTime - now))), toolTip.FONT_COLOR))
			if self.showCombatText and endTime != self.UNLIMITED:
				lines.append((localeInfo.TOOLTIP_BLEND_ONLY_BATTLE, toolTip.POSITIVE_COLOR))
			for (text, color) in lines:
				textLine = toolTip.AppendTextLine(text, color, FALSE)
				textLine.SetPosition(self.TEXT_X_OFFSET, textLine.GetLocalPosition()[1])
			used = toolTip.toolTipHeight - top
			if used < self.ICON_SIZE:
				toolTip.AppendSpace(self.ICON_SIZE - used)
			toolTip.AppendSpace(6)
		FitBoardToolTip(toolTip, self.TOOLTIP_WIDTH)

	def UpdateDescription(self):
		if self.toolTip.IsShow():
			self.RefreshCombinedToolTip()

	def OnMouseOverIn(self):
		self.RefreshCombinedToolTip()
		self.toolTip.ShowToolTip()

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

	IMAGE_STEP = 36

	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF
	RENEWAL_AFFECT_ICON = "icon/affect/"

	AFFECT_DATA_DICT =	{
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub"),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub"),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub"),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub"),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub"),

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
			chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub"),

			chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/item_bonus.sub"),
			chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub"),
			chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub"),
			chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub"),
			chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub"),
			chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub"),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, RENEWAL_AFFECT_ICON + "affect_37.png"),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, RENEWAL_AFFECT_ICON + "affect_38.png"),

			MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub"),
			MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, "d:/ymir work/ui/skill/common/affect/item_bonus.sub"),
			MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_ATT_BONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub"),
			MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/skill/common/affect/critical.sub"),
			MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			AFF_LEADERSHIP: ("Liderlik", "d:/ymir work/ui/game/affect_icon/commandement_affect.png"),
	}

	if app.ENABLE_RENEWAL_AFFECT:
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, RENEWAL_AFFECT_ICON + "affect_40.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, RENEWAL_AFFECT_ICON + "affect_41.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, RENEWAL_AFFECT_ICON + "affect_44.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_45.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_MAGIC_DEF_GRADE, RENEWAL_AFFECT_ICON + "affect_46.png")
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, RENEWAL_AFFECT_ICON + "affect_47.png")

		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affect_icon/blend_item/red_affect.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affect_icon/blend_item/orange_affect.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, "d:/ymir work/ui/game/affect_icon/blend_item/yellow_affect.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, "d:/ymir work/ui/game/affect_icon/blend_item/green_affect.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affect_icon/blend_item/blue_affect.png")
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, "d:/ymir work/ui/game/affect_icon/blend_item/white_affect.png")

		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_HP_PCT] = (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/game/affect_icon/blend_item/71027.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_ATT_BONUS] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/game/affect_icon/blend_item/71028.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MALL_DEFBONUS] = (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/game/affect_icon/blend_item/71030.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_SP_PCT] = (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/game/affect_icon/blend_item/71030.png")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "icon/item/71044.tga")
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "icon/item/71045.tga")

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
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_PICK_UP] = (localeInfo.TOOLTIP_NEW_AFFECT_AUTO_PICK_UP, "d:/ymir work/ui/game/affect_icon/pickup_active.tga")

	if app.__AUTO_HUNT__:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_HUNT] =  (localeInfo.NEW_AFFECT_AUTO_HUNT, RENEWAL_AFFECT_ICON + "auto_hunt.tga")

	if app.ENABLE_CONQUEROR_LEVEL:
		AFFECT_DATA_DICT[chr.AFFECT_CHEONUN] = (localeInfo.SKILL_CHEONUN, "d:/ymir work/ui/skill/shaman/cheonun_03.sub")
		# AFFECT_DATA_DICT[43] = (localeInfo.SKILL_CHEONUN, "d:/ymir work/ui/skill/shaman/cheonun" + END_STRING + ".sub")

	# Baerim affect shower layout: every affect has a (row, order) pair, rows are stacked top to
	# bottom (empty rows collapse) and each row is sorted by order. Row 1 general buffs / potions /
	# premium, row 2 skill buffs, row 3 alchemy potions, row 5 dews and dragon god potions.
	AFFECT_ROW_DICT = {
		chr.AFFECT_JEONGWI : (2, 0), chr.AFFECT_GEOMGYEONG : (2, 0), chr.AFFECT_CHEONGEUN : (2, 0),
		chr.AFFECT_GYEONGGONG : (2, 0), chr.AFFECT_EUNHYEONG : (2, 0), chr.AFFECT_GWIGEOM : (2, 0),
		chr.AFFECT_GONGPO : (2, 0), chr.AFFECT_JUMAGAP : (2, 0), chr.AFFECT_HOSIN : (2, 0),
		chr.AFFECT_BOHO : (2, 0), chr.AFFECT_KWAESOK : (2, 0), chr.AFFECT_HEUKSIN : (2, 0),
		chr.AFFECT_MUYEONG : (2, 0), chr.AFFECT_GICHEON : (2, 0), chr.AFFECT_JEUNGRYEOK : (2, 0),
		chr.AFFECT_PABEOP : (2, 0), chr.AFFECT_FALLEN_CHEONGEUN : (2, 0), chr.AFFECT_CHINA_FIREWORK : (2, 0),
	}
	if app.ENABLE_CONQUEROR_LEVEL:
		AFFECT_ROW_DICT[chr.AFFECT_CHEONUN] = (2, 0)
	if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		AFFECT_ROW_DICT[chr.NEW_AFFECT_AUTO_PICK_UP] = (1, 5)
	# row 1 opens with: multi farm, guild blessing, auto hunt, leadership
	if app.__AUTO_HUNT__:
		AFFECT_ROW_DICT[chr.NEW_AFFECT_AUTO_HUNT] = (1, -2)
	AFFECT_ROW_DICT[AFF_LEADERSHIP] = (1, -1)
	if app.ENABLE_RENEWAL_AFFECT:
		for (point, order) in ((player.POINT_CRITICAL_PCT, 0), (player.POINT_PENETRATE_PCT, 1), (player.ATT_SPEED, 2),
				(player.RESIST_MAGIC, 3), (player.ATT_BONUS, 4), (player.DEF_BONUS, 5)):
			AFFECT_ROW_DICT[DEW_DESC_IDX_START + point] = (5, order)
			AFFECT_ROW_DICT[DEW_EX_DESC_IDX_START + point] = (5, order)
			AFFECT_ROW_DICT[WATER_DESC_IDX_START + point] = (3, order)
		for (point, row, order) in ((player.POINT_MAX_HP_PCT, 5, 7), (player.POINT_ATT_BONUS, 5, 8), (player.POINT_MALL_DEFBONUS, 5, 9),
				(player.POINT_MAX_SP_PCT, 5, 10), (player.POINT_CRITICAL_PCT, 5, 11), (player.POINT_PENETRATE_PCT, 5, 12)):
			AFFECT_ROW_DICT[DRAGON_GOD_DESC_IDX_START + point] = (row, order)
		del point, row, order

	# affect -> (group, item vnum shown in the tooltip, order inside the group)
	COMBINED_AFFECT_DICT = {}
	COMBINED_GROUP_DICT = {	# title, icon, combat-only clock (reference SHOW_COMBAT_TIMER_TEXT)
		"blend" : ("Blend Bonuses", "d:/ymir work/ui/game/affect_icon/blend.png", TRUE),
		"dgod" : ("Dragon God Bonuses", "d:/ymir work/ui/game/affect_icon/dgod.png", TRUE),
	}
	AFFECT_ROW_DICT["combined_blend"] = (5, 0)
	AFFECT_ROW_DICT["combined_dgod"] = (5, 1)
	# timed dragon god potions 71027-71030 (AFFECT_MALL) join the group, their crit / pierce siblings stay on row 3
	for (point, vnum, order) in ((player.POINT_MAX_HP_PCT, 71027, 0), (player.POINT_ATT_BONUS, 71028, 1),
			(player.POINT_MALL_DEFBONUS, 71030, 2), (player.POINT_MAX_SP_PCT, 71029, 3)):
		COMBINED_AFFECT_DICT[MALL_DESC_IDX_START + point] = ("dgod", vnum, order)
	AFFECT_ROW_DICT[MALL_DESC_IDX_START + player.POINT_CRITICAL_PCT] = (3, 11)
	AFFECT_ROW_DICT[MALL_DESC_IDX_START + player.POINT_PENETRATE_PCT] = (3, 12)
	del point, vnum, order
	if app.ENABLE_RENEWAL_AFFECT:
		for (point, vnum, order) in ((player.POINT_CRITICAL_PCT, 50821, 0), (player.POINT_PENETRATE_PCT, 50822, 1), (player.ATT_SPEED, 50823, 2),
				(player.RESIST_MAGIC, 50824, 3), (player.ATT_BONUS, 50825, 4), (player.DEF_BONUS, 50826, 5)):
			COMBINED_AFFECT_DICT[DEW_DESC_IDX_START + point] = ("blend", vnum, order)
			COMBINED_AFFECT_DICT[DEW_EX_DESC_IDX_START + point] = ("blend", vnum, order)	# ITEM_BLEND dews are toggled as AFFECT_BLEND_PLUS
		for (point, vnum, order) in ((player.POINT_MAX_HP_PCT, 71027, 0), (player.POINT_ATT_BONUS, 71028, 1),
				(player.POINT_MALL_DEFBONUS, 71030, 2), (player.POINT_MAX_SP_PCT, 71029, 3)):
			COMBINED_AFFECT_DICT[DRAGON_GOD_DESC_IDX_START + point] = ("dgod", vnum, order)
		del point, vnum, order

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
		self.showBtn.Hide()	# reference has no affect hide toggle

		self.lastUpdateTime = 0
		self.affectImageDict = {}
		self.skillAffectPointDict = {}	# skill vnum -> {pointIdx : (value, endTime)}
		self.skillAffectCastStat = {}	# skill vnum -> caster STR / INT (ENABLE_AFFECT_CAST_STAT)
		self.horseImage = None
		self.lovePointImage = None
		self.benedictionImage = None
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
		for image in self.affectImageDict.values():
			if getattr(image, "toolTip", None):
				image.toolTip.HideToolTip()
		self.horseImage = None
		self.lovePointImage = None
		if app.ENABLE_CONQUEROR_LEVEL:
			self.SungmaImage = None
		if app.ENABLE_MULTI_FARM_BLOCK:
			self.multiFarmBlockDialog = None
			self.farmStatusImage = None
		self.affectImageDict = {}
		self.skillAffectPointDict = {}
		self.skillAffectCastStat = {}
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
			elif getattr(image, "toolTip", None):
				image.toolTip.HideToolTip()
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()

	def SetLeaderShipStatus(self, status, pointIdx = 0, value = 0):
		if not self.affectImageDict.has_key(AFF_LEADERSHIP):
			self.BINARY_NEW_AddAffect(AFF_LEADERSHIP, 0, 0, 0)

		# AFF_LEADERSHIP key'i hala yoksa erken dönü?
		if not self.affectImageDict.has_key(AFF_LEADERSHIP):
			return

		ACTIVE_IMG = "d:/ymir work/ui/game/affect_icon/commandement_affect.png"
		DEACTIVE_IMG = "d:/ymir work/ui/game/affect_icon/commandement_affect.png"

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
			AppendWrappedText(toolTip, localeInfo.LEADERSHIP_DESCRIPTION, 38)
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
			self.affectImageDict[AFF_LEADERSHIP].SetScale(1.0, 1.0)

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
			self.affectImageDict[AFF_LEADERSHIP].SetScale(1.0, 1.0)
			
			toolTip.AppendSpace(5)
			toolTip.AppendTextLine("--------------------------")
			toolTip.AppendTextLine("Pasif")

		FitBoardToolTip(toolTip)

	def __RefreshSkillAffectToolTip(self, skillIndex):
		for (affect, image) in self.affectImageDict.items():
			if getattr(image, "skillVnum", 0) == skillIndex:
				image.SetSkillToolTip(skill.GetSkillName(skillIndex), skillIndex, self.skillAffectPointDict.get(skillIndex, {}), self.skillAffectCastStat.get(skillIndex, 0))

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration, castStat = 0):
		if type < 500:
			endTime = app.GetGlobalTimeStamp() + duration if duration > 0 else 0
			self.skillAffectPointDict.setdefault(type, {})[pointIdx] = (value, endTime)
			if castStat:
				self.skillAffectCastStat[type] = castStat
			self.__RefreshSkillAffectToolTip(type)
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

		if self.COMBINED_AFFECT_DICT.has_key(affect):
			self.__AddCombinedAffect(affect, description, value, duration, castStat)
			return

		if app.ENABLE_MULTI_FARM_BLOCK and affect == chr.NEW_AFFECT_MULTI_FARM:
			self.multiFarmPremiumSlots = value
			if self.farmStatusImage:
				self.farmStatusImage.SetPremiumSlots(value)

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

				image.SetScale(1.0, 1.0)
				image.SetSkillAffectFlag(FALSE)
				image.Show()
				self.affectImageDict[affect] = image
				self.__ArrangeImageList()
			except Exception, e:
				print "except Aff affect ", e
				pass

	def __AddCombinedAffect(self, affect, description, value, duration, castStat = 0):
		(group, itemVnum, order) = self.COMBINED_AFFECT_DICT[affect]
		if isinstance(description, str) or isinstance(description, unicode):
			try:
				if '%' in description and description.count('%') - description.count('%%') > 0:
					description = description % float(value)
			except (TypeError, ValueError):
				pass
		else:
			description = description(float(value))
		if duration >= self.INFINITE_AFFECT_DURATION:
			endTime = CombinedAffectImage.UNLIMITED	# toggled "plus" potions: no clock at all
			castStat = 0
		else:
			endTime = app.GetGlobalTimeStamp() + duration if duration > 0 else 0

		key = "combined_" + group
		image = self.affectImageDict.get(key)
		if not image:
			(title, iconPath, showCombatText) = self.COMBINED_GROUP_DICT[group]
			image = CombinedAffectImage(title, iconPath, showCombatText)
			image.SetParent(self)
			image.Show()
			self.affectImageDict[key] = image
		pausedLeft = duration if castStat == CombinedAffectImage.COMBAT_TIMER_PAUSED else None
		image.SetMember(affect, order, itemVnum, description, endTime, pausedLeft)
		self.__ArrangeImageList()

	def __RemoveCombinedAffect(self, affect):
		key = "combined_" + self.COMBINED_AFFECT_DICT[affect][0]
		image = self.affectImageDict.get(key)
		if not image:
			return
		image.RemoveMember(affect)
		if image.IsEmpty():
			image.toolTip.HideToolTip()
			image.Hide()
			del self.affectImageDict[key]
		self.__ArrangeImageList()

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type < 500:
			points = self.skillAffectPointDict.get(type, {})
			points.pop(pointIdx, None)
			if not points:
				self.skillAffectPointDict.pop(type, None)
				self.skillAffectCastStat.pop(type, None)
			self.__RefreshSkillAffectToolTip(type)
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

		if affect == AFF_LEADERSHIP:
			self.SetLeaderShipStatus(False)
			return

		if self.COMBINED_AFFECT_DICT.has_key(affect):
			self.__RemoveCombinedAffect(affect)
			return

		if app.ENABLE_MULTI_FARM_BLOCK and affect == chr.NEW_AFFECT_MULTI_FARM:
			self.multiFarmPremiumSlots = 0
			if self.farmStatusImage:
				self.farmStatusImage.SetPremiumSlots(0)

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
		if 0 != skillIndex:
			image.skillVnum = skillIndex
			image.SetSkillToolTip(name, skillIndex, self.skillAffectPointDict.get(skillIndex, {}), self.skillAffectCastStat.get(skillIndex, 0))
		image.SetScale(1.0, 1.0)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		if not self.affectImageDict.has_key(affect):
			print "__RemoveAffect %s ( No Affect )" % affect
			return

		print "__RemoveAffect %s ( Affect )" % affect
		image = self.affectImageDict[affect]
		if getattr(image, "toolTip", None):
			image.toolTip.HideToolTip()
		image.Hide()
		del self.affectImageDict[affect]

		self.__ArrangeImageList()

	def ApplyIconSize(self, small):
		self.IMAGE_STEP = 27 if small else 36
		images = list(self.affectImageDict.values())
		for name in ("lovePointImage", "horseImage", "farmStatusImage", "SungmaImage"):
			image = getattr(self, name, None)
			if image: images.append(image)
		for image in images: image.SetScale(1.0, 1.0)
		self.__ArrangeImageList()

	def __ArrangeImageList(self):
		import advanced_options_runtime
		self.IMAGE_STEP = 27 if advanced_options_runtime.Get("small_affect_icon", False) else 36
		self.isShowingAffects = True

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

		xPos = 0
		yPos = 0

		# reference (Baerim AffectShower.__ArrangeImageList): rows from AFFECT_ROW_DICT, the horse /
		# love point / farm icons open row 1, each row is a stable sort by order
		special = []
		if app.ENABLE_GUILD_RENEWAL_BAERIM and guild.IsGuildEnable():
			if not self.benedictionImage:
				self.benedictionImage = BenedictionImage()
				self.benedictionImage.SetParent(self)
		elif self.benedictionImage:
			self.benedictionImage.Hide()
			self.benedictionImage = None
		leading = []
		if app.ENABLE_MULTI_FARM_BLOCK and self.farmStatusImage:
			leading.append(self.farmStatusImage)
		if self.benedictionImage:
			leading.append(self.benedictionImage)
		if self.lovePointImage and self.lovePointImage.IsShow():
			special.append(self.lovePointImage)
		if self.horseImage:
			special.append(self.horseImage)
		if app.ENABLE_CONQUEROR_LEVEL and self.SungmaImage and self.SungmaImage.IsShow():
			special.append(self.SungmaImage)

		rows = {}
		for key in sorted(self.affectImageDict.keys()):
			(row, order) = self.AFFECT_ROW_DICT.get(key, (1, 0))
			rows.setdefault(row, []).append((order, self.affectImageDict[key]))
		groups = []
		for row in sorted(rows.keys()):
			ordered = sorted(rows[row], key = lambda x: x[0])
			images = [image for (order, image) in ordered if order >= 0 or row != 1]
			if row == 1:
				images = leading + [image for (order, image) in ordered if order < 0] + special + images
			groups.append(images)
		if (leading or special) and not rows.has_key(1):
			groups.insert(0, leading + special)

		lineLimit = 0
		lineLimitWidth = wndMgr.GetScreenWidth() - getattr(self, "_AffectShower__minimap_width", 0) * 2
		if lineLimitWidth > 0:
			lineLimit = max(1, (lineLimitWidth ) // self.IMAGE_STEP)

		yPos = 0
		maxWidth = 0
		for group in groups:
			if not group:
				continue
			xPos = 0
			for index, image in enumerate(group):
				if lineLimit and index and index % lineLimit == 0:
					xPos = 0
					yPos += self.IMAGE_STEP
				image.SetPosition(xPos, yPos)
				image.Show()
				xPos += self.IMAGE_STEP
				maxWidth = max(maxWidth, xPos)
			yPos += self.IMAGE_STEP

		height = max(self.IMAGE_STEP, yPos)
		if app.ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL and getattr(self, "_AffectShower__party_window", None):
			self.__party_window.SetWindowPosY(self.BASE_POS_Y + height)
		self.SetSize(max(maxWidth, 32), height)

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
				self.farmStatusImage.AddPlayer(playerName)

		def SetMultiFarmInfo(self, farmStatus):
			# reuse the icon: the server resends status + player list on every change
			image = self.farmStatusImage
			if not image:
				image = MultiFarmImage()
				image.SetParent(self)
				image.SetEvent(ui.__mem_func__(self.__OnClickMultiFarm), "mouse_click")
				image.Show()
				self.farmStatusImage = image
			image.SetPremiumSlots(getattr(self, "multiFarmPremiumSlots", 0))
			image.SetMultiFarmInfo(farmStatus)
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
			image.SetScale(1.0, 1.0)
			image.Show()

			self.petSkillaffectImageDict[affect] = [index, image]

			return TRUE

	def __IterToolTipOwners(self):
		for value in self.__dict__.values():
			items = value.values() if isinstance(value, dict) else (value,)
			for item in items:
				if isinstance(item, list) and len(item) == 2:	# petSkillaffectImageDict: [index, image]
					item = item[1]
				if isinstance(item, ui.Window):
					yield item

	def __HideStaleToolTips(self, force = FALSE):
		# Every icon tooltip is a separate top window that only closes on the icon's OnMouseOverOut. An icon that is
		# hidden, moved or rebuilt while hovered never gets that event, so check the hovered state every frame.
		for owner in self.__IterToolTipOwners():
			tips = [getattr(owner, "toolTip", None)] + [v for (k, v) in owner.__dict__.items() if k.endswith("__tool_tip")]
			for tip in tips:
				if tip and hasattr(tip, "IsShow") and tip.IsShow() and (force or not owner.IsShow() or not owner.IsIn()):
					tip.HideToolTip()

	def Hide(self):
		self.__HideStaleToolTips(TRUE)
		ui.Window.Hide(self)

	def OnUpdate(self):
		try:
			self.__HideStaleToolTips()
		except Exception, e:
			print "AffectShower::HideStaleToolTips error : ", e
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

