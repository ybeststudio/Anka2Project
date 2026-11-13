if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import wndMgr
import localeInfo
import uiToolTip

# Baerim "Hasar Takibi" (uiTrackDamage.DamageDetails): the server records the steps of the damage formula for one real
# hit on the watched target (at most one per second) and sends DamageTraceBegin / Row / End (server damage_trace.cpp).
# Table: calculation step | before | change | after, "?" help per step, total damage, freeze button.

WIDTH = 400						# Baerim DamageDetails.WIDTH
ROW_HEIGHT = 22					# Baerim DamageDetails.ROW_HEIGHT
MAX_VISIBLE_ROWS = 12
TABLE_X = 8
TABLE_Y = 10
TABLE_WIDTH = WIDTH - 2 * TABLE_X
# column borders inside the table: help | step | before | change | after
COLUMNS = (0, 24, 210, 272, 334, TABLE_WIDTH)
BUTTON_PATH = "d:/ymir work/ui/game/common/button_large_%s.png"
HELP_PATH = "d:/ymir work/ui/pattern/q_mark_%02d.tga"

HEADER_COLOR = 0xff2d1c11
BORDER_COLOR = 0xff42392f
ROW_COLOR = 0xff1f1511
LINE_COLOR = 0xff3a3028
TITLE_COLOR = 0xfff3e9c8
TEXT_COLOR = 0xffd8d0c0
UP_COLOR = 0xff7fd65f
DOWN_COLOR = 0xffe25b5b
TOTAL_COLOR = 0xfff3e9c8

# server step key -> (label locale key, help locale key)
STEP_TEXT = {
	"INPUT": ("TRACK_DAMAGE_INPUT", "TRACK_DAMAGE_HELP_RULE"),
	"ATTACK": ("TRACK_DAMAGE_ATTACK", "TRACK_DAMAGE_HELP_ATTACK"),
	"WEAPON": ("TRACK_DAMAGE_WEAPON", "TRACK_DAMAGE_HELP_WEAPON"),
	"ACCURACY": ("TRACK_DAMAGE_ACCURACY", "TRACK_DAMAGE_HELP_ACCURACY"),
	"REFINE": ("TRACK_DAMAGE_REFINE", "TRACK_DAMAGE_HELP_REFINE"),
	"PARTY": ("TRACK_DAMAGE_PARTY", "TRACK_DAMAGE_HELP_RULE"),
	"ATTACK_PERCENT": ("TRACK_DAMAGE_ATTACK_PERCENT", "TRACK_DAMAGE_HELP_PERCENT"),
	"MARRIAGE": ("TRACK_DAMAGE_MARRIAGE", "TRACK_DAMAGE_HELP_RULE"),
	"ATTBONUS_MONSTER": ("TARGET_INFO_BONUS_MONSTER", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_STONE": ("TRACK_DAMAGE_METIN_BONUS", "TRACK_DAMAGE_HELP_METIN"),
	"ATTBONUS_BOSS": ("TARGET_INFO_BONUS_BOSS", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_ELEMENTS": ("TRACK_DAMAGE_ELEMENT", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_CZ": ("TRACK_DAMAGE_BONUS_CZ", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_HUMAN": ("TARGET_INFO_BONUS_HUMAN", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_ANIMAL": ("TARGET_INFO_BONUS_ANIMAL", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_ORC": ("TARGET_INFO_BONUS_ORC", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_MILGYO": ("TARGET_INFO_BONUS_MILGYO", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_UNDEAD": ("TARGET_INFO_BONUS_UNDEAD", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_DEVIL": ("TARGET_INFO_BONUS_DEVIL", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_INSECT": ("TARGET_INFO_BONUS_INSECT", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_FIRE": ("TARGET_INFO_BONUS_FIRE", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_ICE": ("TARGET_INFO_BONUS_ICE", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_DESERT": ("TARGET_INFO_BONUS_DESERT", "TRACK_DAMAGE_HELP_PERCENT"),
	"ATTBONUS_TREE": ("TARGET_INFO_BONUS_TREE", "TRACK_DAMAGE_HELP_PERCENT"),
	"DEFENSE": ("TRACK_DAMAGE_DEFENSE", "TRACK_DAMAGE_HELP_DEFENSE"),
	"MINIMUM": ("TRACK_DAMAGE_MINIMUM", "TRACK_DAMAGE_HELP_MINIMUM"),
	"WEAPON_RESIST": ("TRACK_DAMAGE_WEAPON_RESIST", "TRACK_DAMAGE_HELP_RESIST"),
	"ATTACK_MULTIPLIER": ("TRACK_DAMAGE_ATTACK_MULTIPLIER", "TRACK_DAMAGE_HELP_RULE"),
	"CRITICAL": ("TRACK_DAMAGE_CRITICAL", "TRACK_DAMAGE_HELP_CRITICAL"),
	"PENETRATE": ("TRACK_DAMAGE_PENETRATE_HIT", "TRACK_DAMAGE_HELP_RULE"),
	"RULES": ("TRACK_DAMAGE_RULES", "TRACK_DAMAGE_HELP_RULE"),
	"NORMAL_HIT": ("TRACK_DAMAGE_NORMAL_HIT", "TRACK_DAMAGE_HELP_PERCENT"),
	"SKILL_BONUS": ("TRACK_DAMAGE_SKILL_BONUS", "TRACK_DAMAGE_HELP_PERCENT"),
	"SERVER_RATE": ("TRACK_DAMAGE_SERVER_RATE", "TRACK_DAMAGE_HELP_RULE"),
	"METIN_CAP": ("TRACK_DAMAGE_METIN_CAP", "TRACK_DAMAGE_HELP_CAP"),
	"DISTANCE": ("TRACK_DAMAGE_DISTANCE", "TRACK_DAMAGE_HELP_RULE"),
	"SKILL_FORMULA": ("TRACK_DAMAGE_SKILL_FORMULA", "TRACK_DAMAGE_HELP_RULE"),
}


def FormatNumber(value):
	sign = "-" if value < 0 else ""
	text = str(abs(int(value)))
	parts = []
	while len(text) > 3:
		parts.insert(0, text[-3:])
		text = text[:-3]
	parts.insert(0, text)
	return sign + ".".join(parts)


def StepLabel(key):
	(labelKey, helpKey) = STEP_TEXT.get(key, (None, "TRACK_DAMAGE_HELP_RULE"))
	if labelKey and hasattr(localeInfo, labelKey):
		return getattr(localeInfo, labelKey)
	if key.startswith("ATTBONUS_"):
		return getattr(localeInfo, "TRACK_DAMAGE_BONUS_PVP", key)
	return key


def StepHelp(key):
	(labelKey, helpKey) = STEP_TEXT.get(key, (None, "TRACK_DAMAGE_HELP_RULE"))
	return getattr(localeInfo, helpKey, "")


class DamageDetails(ui.ThinBoard):

	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.AddFlag("float")
		self.targetVID = 0
		self.request = 0
		self.paused = False
		self.rows = []				# [(key, before, after)]
		self.pending = None
		self.pendingRequest = -1
		self.total = 0
		self.scroll = 0
		self.children = []
		self.rowWindows = []
		self.anchor = None
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		self.totalText = ui.TextLine()
		self.totalText.SetParent(self)
		self.totalText.SetFontName("Tahoma:14b")
		self.totalText.SetHorizontalAlignCenter()
		self.totalText.SetWindowHorizontalAlignCenter()
		self.totalText.SetPackedFontColor(TOTAL_COLOR)
		self.totalText.SetOutline()
		self.totalText.Show()

		self.statusText = ui.TextLine()
		self.statusText.SetParent(self)
		self.statusText.SetHorizontalAlignCenter()
		self.statusText.SetWindowHorizontalAlignCenter()
		self.statusText.SetPackedFontColor(TEXT_COLOR)
		self.statusText.SetOutline()

		self.pauseButton = ui.Button()
		self.pauseButton.SetParent(self)
		self.pauseButton.SetUpVisual(BUTTON_PATH % "normal")
		self.pauseButton.SetOverVisual(BUTTON_PATH % "hover")
		self.pauseButton.SetDownVisual(BUTTON_PATH % "down")
		self.pauseButton.SetWindowHorizontalAlignCenter()
		self.pauseButton.SetText(localeInfo.TRACK_DAMAGE_PAUSE)
		self.pauseButton.SetEvent(ui.__mem_func__(self.TogglePause))
		self.pauseButton.Show()

		self.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheel))
		self.SetSize(WIDTH, 100)

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def Destroy(self):
		self.Close()
		self.children = []
		self.rowWindows = []
		self.toolTip = None

	# ---------------------------------------------------------------- open / close
	def Open(self, targetVID, anchor = None):
		self.targetVID = targetVID
		self.anchor = anchor
		self.request += 1
		self.paused = False
		self.rows = []
		self.pending = None
		self.total = 0
		self.scroll = 0
		self.pauseButton.SetText(localeInfo.TRACK_DAMAGE_PAUSE)
		net.SendChatPacket("/damage_trace %d %d" % (targetVID, self.request))
		self.__Refresh()
		self.Show()
		self.SetTop()
		self.UpdateAnchor()

	def Close(self):
		if self.targetVID:
			net.SendChatPacket("/damage_trace 0 0")
		self.targetVID = 0
		self.pending = None
		if self.toolTip:
			self.toolTip.HideToolTip()
		self.Hide()

	def GetTargetVID(self):
		return self.targetVID

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def TogglePause(self):
		self.paused = not self.paused
		self.pauseButton.SetText(localeInfo.TRACK_DAMAGE_RESUME if self.paused else localeInfo.TRACK_DAMAGE_PAUSE)
		self.__Refresh()

	def UpdateAnchor(self):
		# Baerim: next to the target info board, kept on screen
		(screenWidth, screenHeight) = (wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		if self.anchor and self.anchor.IsShow():
			x = self.anchor.GetRight() + 4
			y = self.anchor.GetTop()
			if x + self.GetWidth() > screenWidth:
				x = self.anchor.GetLeft() - self.GetWidth() - 4
		else:
			x = (screenWidth - self.GetWidth()) / 2
			y = 120
		x = max(0, min(screenWidth - self.GetWidth(), x))
		y = max(0, min(screenHeight - self.GetHeight(), y))
		self.SetPosition(x, y)

	# ---------------------------------------------------------------- server data
	def Begin(self, request, vid, hitType, count):
		if request != self.request or vid != self.targetVID or self.paused:
			self.pending = None
			return
		self.pending = []
		self.pendingRequest = request

	def Row(self, request, index, key, before, after):
		if self.pending is None or request != self.pendingRequest:
			return
		self.pending.append((key, before, after))

	def End(self, request, displayed, hpLoss):
		if self.pending is None or request != self.pendingRequest:
			return
		self.rows = self.pending
		self.pending = None
		self.total = displayed
		self.scroll = 0
		self.__Refresh()

	# ---------------------------------------------------------------- layout
	def __Bar(self, x, y, w, h, color):
		bar = ui.Bar()
		bar.SetParent(self)
		bar.SetPosition(x, y)
		bar.SetSize(w, h)
		bar.SetColor(color)
		bar.AddFlag("not_pick")
		bar.Show()
		self.children.append(bar)

	def __Text(self, x, y, text, color, center = True):
		line = ui.TextLine()
		line.SetParent(self)
		line.SetText(text)
		line.SetPackedFontColor(color)
		line.SetOutline()
		if center:
			line.SetHorizontalAlignCenter()
		line.SetPosition(x, y)
		line.Show()
		self.children.append(line)
		return line

	def __Refresh(self):
		self.children = []
		self.rowWindows = []
		left = TABLE_X
		top = TABLE_Y

		# header
		self.__Bar(left, top, TABLE_WIDTH, ROW_HEIGHT, HEADER_COLOR)
		self.__Bar(left, top, TABLE_WIDTH, 1, BORDER_COLOR)
		self.__Bar(left, top + ROW_HEIGHT - 1, TABLE_WIDTH, 1, BORDER_COLOR)
		self.__Bar(left, top, 1, ROW_HEIGHT, BORDER_COLOR)
		self.__Bar(left + TABLE_WIDTH - 1, top, 1, ROW_HEIGHT, BORDER_COLOR)
		textY = top + ROW_HEIGHT / 2 - 7
		self.__Text(left + (COLUMNS[0] + COLUMNS[2]) / 2, textY, localeInfo.TRACK_DAMAGE_STEP, TITLE_COLOR)
		self.__Text(left + (COLUMNS[2] + COLUMNS[3]) / 2, textY, localeInfo.TRACK_DAMAGE_BEFORE, TITLE_COLOR)
		self.__Text(left + (COLUMNS[3] + COLUMNS[4]) / 2, textY, localeInfo.TRACK_DAMAGE_CHANGE, TITLE_COLOR)
		self.__Text(left + (COLUMNS[4] + COLUMNS[5]) / 2, textY, localeInfo.TRACK_DAMAGE_AFTER, TITLE_COLOR)
		y = top + ROW_HEIGHT

		visible = self.rows[self.scroll:self.scroll + MAX_VISIBLE_ROWS]
		for (index, (key, before, after)) in enumerate(visible):
			self.__Bar(left, y, TABLE_WIDTH, ROW_HEIGHT, ROW_COLOR)
			self.__Bar(left, y, 1, ROW_HEIGHT, LINE_COLOR)
			self.__Bar(left + TABLE_WIDTH - 1, y, 1, ROW_HEIGHT, LINE_COLOR)
			self.__Bar(left, y + ROW_HEIGHT - 1, TABLE_WIDTH, 1, LINE_COLOR)
			for column in COLUMNS[1:-1]:
				if column != COLUMNS[1]:
					self.__Bar(left + column, y, 1, ROW_HEIGHT, LINE_COLOR)

			helpButton = ui.Button()
			helpButton.SetParent(self)
			helpButton.SetUpVisual(HELP_PATH % 1)
			helpButton.SetOverVisual(HELP_PATH % 2)
			helpButton.SetDownVisual(HELP_PATH % 2)
			helpButton.SetPosition(left + (COLUMNS[1] - helpButton.GetWidth()) / 2 + 2, y + (ROW_HEIGHT - helpButton.GetHeight()) / 2)
			helpButton.SetShowToolTipEvent(ui.__mem_func__(self.__ShowStepTip), key)
			helpButton.SetHideToolTipEvent(ui.__mem_func__(self.__HideStepTip))
			helpButton.Show()
			self.children.append(helpButton)

			delta = after - before
			textY = y + ROW_HEIGHT / 2 - 7
			self.__Text(left + (COLUMNS[1] + COLUMNS[2]) / 2, textY, StepLabel(key), TEXT_COLOR)
			self.__Text(left + (COLUMNS[2] + COLUMNS[3]) / 2, textY, FormatNumber(before), TEXT_COLOR)
			self.__Text(left + (COLUMNS[3] + COLUMNS[4]) / 2, textY, ("+" if delta >= 0 else "") + FormatNumber(delta), UP_COLOR if delta >= 0 else DOWN_COLOR)
			self.__Text(left + (COLUMNS[4] + COLUMNS[5]) / 2, textY, FormatNumber(after), TEXT_COLOR)
			y += ROW_HEIGHT

		if not self.rows:
			self.statusText.SetText(localeInfo.TRACK_DAMAGE_PAUSED if self.paused else localeInfo.TRACK_DAMAGE_WAITING)
			self.statusText.SetPosition(0, y + 6)
			self.statusText.Show()
			y += 26
		elif self.paused:
			self.statusText.SetText(localeInfo.TRACK_DAMAGE_PAUSED)
			self.statusText.SetPosition(0, y + 6)
			self.statusText.Show()
			y += 26
		else:
			self.statusText.Hide()

		self.totalText.SetText(localeInfo.TRACK_DAMAGE_TOTAL % FormatNumber(self.total))
		self.totalText.SetPosition(0, y + 10)
		self.pauseButton.SetPosition(0, y + 34)
		self.SetSize(WIDTH, y + 34 + self.pauseButton.GetHeight() + 12)
		self.UpdateAnchor()

	def __OnWheel(self, isDown):
		maximum = max(0, len(self.rows) - MAX_VISIBLE_ROWS)
		if not maximum:
			return
		self.scroll = max(0, min(maximum, self.scroll + (1 if isDown else -1)))
		self.__Refresh()

	def __ShowStepTip(self, key):
		if not self.toolTip:
			return
		self.toolTip.ClearToolTip()
		self.toolTip.SetThinBoardSize(260)
		self.toolTip.AutoAppendTextLine(StepLabel(key), TITLE_COLOR)
		self.toolTip.AppendSpace(4)
		helpText = StepHelp(key)
		for line in self.__Wrap(helpText, 40):
			self.toolTip.AutoAppendTextLine(line, TEXT_COLOR)
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.ShowToolTip()

	def __HideStepTip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	def __Wrap(self, text, width):
		result = []
		current = ""
		for word in text.split(" "):
			candidate = (current + " " + word).strip()
			if len(candidate) > width and current:
				result.append(current)
				current = word
			else:
				current = candidate
		if current:
			result.append(current)
		return result
