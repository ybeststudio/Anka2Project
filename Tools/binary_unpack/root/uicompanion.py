import app
import net
import player
import ui
import uiToolTip
import grpText
import uiSkillColor
import mouseModule
import localeInfo
import constInfo

# Companion window (Pet / Horse / Buffi). Layout after the reference uiscript/companionsystem.py,
# uiCompanion.CompanionWindow.__LoadBuffiWindow (baerim.exe) and an in-game screenshot of the Buffi tab.
COMPANION_PATH = "d:/ymir work/ui/game/companion{}"

PET_WIDTH = 280
PET_HEIGHT = 415

TAB_PET = 0
TAB_HORSE = 1
TAB_BUFFI = 2

BUFFI_SEAL_VNUM = 57100
BUFFI_ORE_VNUM = 57101
BUFFI_ORE_PERM_VNUM = 57103
BUFF_SKILL_INDEX = (94, 95, 96, 110, 111)
BUFFI_INT_STAGES = (120, 140, 160, 180)
BUFFI_DURATION_MAX = 30 * 24 * 60 * 60

# Buffi skill tooltip (reference skilldesc.txt + SUPPORT_SKILL_AMOUNT_DESC_*):
# (name, name G, name P, description, condition 1, condition 2, value line)
BUFFI_SKILL_TOOLTIP = (
	("Kutsama", "Cennet", "Ejderha", "Koruyucu bir \xe7ember olu\xfeturur.", "Buff | Kendine uyguland\xfd\xf0\xfdnda bonus", "Yak\xfdn d\xf6v\xfc\xfe sald\xfdr\xfdlar\xfdna kar\xfe\xfd diren\xe7", "Yak\xfdn d\xf6v\xfc\xfe sald\xfdr\xfdlar\xfdna kar\xfe\xfd diren\xe7: %d%%"),
	("Yans\xfdtmak", "Yans\xfdtan Su", "Kristal Ayna", "Koruyucu bir yans\xfdt\xfdc\xfd daire olu\xfeturur.", "Buff | Kendine uyguland\xfd\xf0\xfdnda bonus", "Hasar\xfd sald\xfdrgana geri yans\xfdt\xfdr", "Yans\xfdyan yak\xfdn d\xf6v\xfc\xfe hasar\xfd: %d%%"),
	("Ejderhan\xfdn Yard\xfdm\xfd", "Ejderhan\xfdn Deste\xf0i", "Ejderha G\xfcc\xfc", "Sizin veya bir grup \xfcyesinin sald\xfdr\xfds\xfdn\xfd g\xfc\xe7lendirir.", "Buff | Kendine uyguland\xfd\xf0\xfdnda bonus", "Kritik vuru\xfe \xfeans\xfdn\xfd art\xfdr\xfdr", "Kritik vuru\xfe \xfeans\xfd: %d%%"),
	("\xc7abukluk", "\xc7eviklik", "R\xfczgar H\xfdz\xfd", "R\xfczgar kadar h\xfdzl\xfd ko\xfe.", "Hareket ve b\xfcy\xfc h\xfdz\xfdn\xfd art\xfdr\xfdr", "Kendine at\xfdld\xfd\xf0\xfdnda bonus", "Hareket h\xfdz\xfd: %d%%"),
	("Sald\xfdr\xfd Art\xfd\xfe\xfd", "Destek Sald\xfdr\xfds\xfd", "G\xfc\xe7lendirilmi\xfe Sald\xfdr\xfd", "Bir orta\xf0\xfd g\xfc\xe7lendirmek.", "Sald\xfdr\xfd de\xf0erini art\xfdr\xfdr", "Grup \xfcyeleri \xfczerinde kullan\xfdlabilir", "Sald\xfdr\xfd G\xfcc\xfc: +%d"),
)
BUFFI_INT_POWERED = "INT taraf\xfdndan g\xfc\xe7lendirildi"
BUFFI_GRADE_SUFFIX = ("", " Master", " Grand Master", " Perfect Master")
# reference tooltip value lines (Swiftness also shows the casting speed)
BUFFI_SKILL_VALUE = (
	("Yak\xfdn d\xf6v\xfc\xfe sald\xfdr\xfdlar\xfdna kar\xfe\xfd diren\xe7: %d%%",),
	("Yans\xfdyan yak\xfdn d\xf6v\xfc\xfe hasar\xfd: %d%%",),
	("Kritik Vuru\xfe \xdeans\xfd: %d%%",),
	("Hareket H\xfdz\xfd: %d%%", "B\xfcy\xfc H\xfdz\xfd: %d%%",),
	("Sald\xfdr\xfd G\xfcc\xfc: +%d",),
)
# reference tooltip colours
BUFFI_TT_WHITE = 0xffffffff
BUFFI_TT_GREEN = 0xff8cf07a
BUFFI_TT_YELLOW = 0xfff0d878

# last Buffi state sent by the server; read by uiToolTip for the seal (57100) tooltip
g_buffiState = {"level": 0, "duration": 0, "time": 0.0, "skills": [0] * 5, "points": 0}

BONUS_ROW_HEIGHT = 27
BONUS_VISIBLE_ROWS = 2


def _T(name, default):
	return getattr(localeInfo, name, default)


def _SkillGrade(level):
	if level >= 40:
		return 3, 1
	if level >= 30:
		return 2, level - 29
	if level >= 20:
		return 1, level - 19
	return 0, level


def _FormatTime(sec):
	d = sec / 86400
	h = (sec % 86400) / 3600
	m = (sec % 3600) / 60
	if d > 0:
		return "%dg %dsa. %ddk." % (d, h, m)
	if h > 0:
		return "%dsa. %ddk." % (h, m)
	return "%ddk." % m


class ThinScrollBar(ui.Window):
	"""Thin scrollbar like the reference (ScrollBarSpecial): grey framed dark track with a grey thumb."""
	WIDTH = 5

	def __init__(self):
		ui.Window.__init__(self)
		self.eventScroll = None
		self.pos = 0.0
		self.height = 0
		self.barHeight = 10

		# ui.Box is drawn filled here, so the frame is an outer grey bar with a black bar inset by 1px
		frame = ui.Bar()
		frame.SetParent(self)
		frame.SetColor(0xff464646)
		frame.AddFlag("not_pick")
		frame.Show()
		self.frame = frame

		track = ui.Bar()
		track.SetParent(self)
		track.SetPosition(1, 1)
		track.SetColor(0xff000000)
		track.AddFlag("not_pick")
		track.Show()
		self.track = track

		bar = ui.Bar()
		bar.SetParent(self)
		bar.SetColor(0xff6a6a6a)
		bar.AddFlag("not_pick")
		bar.Show()
		self.bar = bar

	def __del__(self):
		ui.Window.__del__(self)

	def SetScrollBarSize(self, height):
		self.height = height
		self.SetSize(self.WIDTH, height)
		self.frame.SetSize(self.WIDTH, height)
		self.track.SetSize(self.WIDTH - 2, height - 2)
		self.__Refresh()

	def SetMiddleBarSize(self, scale):
		self.barHeight = max(8, int((self.height - 2) * min(1.0, scale)))
		self.bar.SetSize(self.WIDTH - 2, self.barHeight)
		self.__Refresh()

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def GetPos(self):
		return self.pos

	def SetPos(self, pos):
		self.pos = max(0.0, min(1.0, pos))
		self.__Refresh()
		if self.eventScroll:
			self.eventScroll()

	def __Refresh(self):
		self.bar.SetPosition(1, 1 + int((self.height - 2 - self.barHeight) * self.pos))

	def OnMouseLeftButtonDown(self):
		x, y = self.GetMouseLocalPosition()
		span = max(1, self.height - 2 - self.barHeight)
		self.SetPos(float(y - 1 - self.barHeight / 2) / span)


class HoverArea(ui.Window):
	def __init__(self, overIn, overOut):
		ui.Window.__init__(self)
		self.overIn = overIn
		self.overOut = overOut

	def __del__(self):
		ui.Window.__del__(self)

	def OnMouseOverIn(self):
		if self.overIn:
			self.overIn()

	def OnMouseOverOut(self):
		if self.overOut:
			self.overOut()


class BuffiSkillColorWindow(uiSkillColor.SkillColorWindow):
	"""Reference skill colour window, but reading / writing the colours of a Buffi skill (/buffi_skill_color)."""

	def __init__(self, owner, buffiSlot, skillIndex):
		self.owner = owner
		self.buffiSlot = buffiSlot
		uiSkillColor.SkillColorWindow.__init__(self, buffiSlot, skillIndex)

	def GetSkillColorData(self, skillSlot):
		return list(self.owner.GetBuffiSkillColor(self.buffiSlot))

	def SetSkillColorData(self, skillSlot, c0, c1, c2, c3, c4):
		net.SendChatPacket("/buffi_skill_color %d %d %d %d %d %d" % (self.buffiSlot, c0, c1, c2, c3, c4))


# Buffi evolution (reference uiCompanion.BUFFI_EVOLVE_REQUIREMENTS; 26002 = yang). Must match CBuffiActor::Evolve.
BUFFI_EVOLVE_REQUIREMENTS = {
	0: ((40912, 5), (40812, 5), (40714, 10), (50628, 10), (40914, 3), (26002, 10000000)),
	1: ((40942, 5), (40914, 3), (50632, 10), (40718, 3), (40820, 5), (26002, 50000000)),
	2: ((40917, 10), (40919, 5), (40920, 5), (40718, 5), (40821, 5), (26002, 100000000)),
}
BUFFI_EVOLVE_LEVEL = (55, 75, 90)
BUFFI_MAX_EVOLUTION = 3
EVOLVE_PATH = "d:/ymir work/ui/game/companion/evolve/{}"
EVOLVE_YANG_VNUM = 26002
EVOLVE_COLOR_OK = 0xff8cf07a
EVOLVE_COLOR_LACK = 0xffff5050


class BuffiEvolutionWindow(ui.BoardWithTitleBar):
	WIDTH = 218
	HEIGHT = 334
	# slot centres inside background.png (202x156): 3 on the top row, 2 below
	SLOT_CENTER = ((41, 14), (101, 14), (161, 14), (71, 82), (131, 82))

	def __init__(self, owner):
		ui.BoardWithTitleBar.__init__(self)
		self.owner = owner
		self.evolution = 0
		self.items = ()
		self.price = 0
		self.keep = []
		self.counts = []
		self.questionDialog = None
		self.updateTime = 0.0
		self.toolTipItem = uiToolTip.ItemToolTip()
		self.toolTipItem.HideToolTip()
		self.__CreateWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.Close()
		self.keep = []
		self.counts = []
		self.owner = None
		self.toolTipItem = None

	def __Image(self, parent, name, x, y):
		img = ui.ImageBox()
		img.SetParent(parent)
		img.LoadImage(name if name.startswith("d:/") else EVOLVE_PATH.format(name))
		img.SetPosition(x, y)
		img.AddFlag("not_pick")
		img.Show()
		self.keep.append(img)
		return img

	def __Text(self, parent, text, x, y):
		line = ui.TextLine()
		line.SetParent(parent)
		line.SetPosition(x, y)
		line.SetHorizontalAlignCenter()
		line.SetOutline()
		line.SetText(text)
		line.Show()
		self.keep.append(line)
		return line

	def __CreateWindow(self):
		self.SetSize(self.WIDTH, self.HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetTitleName(_T("SUPPORT_TITLE_EVOLVE", "Buffi Evrimi"))
		self.SetCloseEvent(ui.__mem_func__(self.Close))

		header = self.__Image(self, "header_bg.png", 8, 32)
		self.evoText = self.__Text(header, "", 101, 4)
		self.levelText = self.__Text(header, "", 101, 18)

		bg = self.__Image(self, "background.png", 8, 68)
		for cx, cy in self.SLOT_CENTER:
			self.__Image(bg, "d:/ymir work/ui/public/slot_base.sub", cx - 16, cy)
			cnt = self.__Image(bg, "count_bg.png", cx - 20, cy + 36)
			self.counts.append(self.__Text(cnt, "", 20, 4))

		slot = ui.SlotWindow()
		slot.SetParent(bg)
		slot.SetPosition(0, 0)
		slot.SetSize(202, 156)
		for i, (cx, cy) in enumerate(self.SLOT_CENTER):
			slot.AppendSlot(i, cx - 16, cy, 32, 32)
		slot.SetOverInItemEvent(ui.__mem_func__(self.__OnSlotOverIn))
		slot.SetOverOutItemEvent(ui.__mem_func__(self.__OnSlotOverOut))
		slot.Show()
		self.slot = slot

		costTitle = self.__Image(self, "cost_title_bg.png", 17, 230)
		self.__Text(costTitle, "Geli\xfetirme Fiyat\xfd", 92, 3)
		cost = self.__Image(self, "cost_bg.png", 17, 252)
		self.priceText = self.__Text(cost, "", 92, 1)
		chance = self.__Image(self, "bg_chance_success.png", 17, 272)
		self.__Text(chance, _T("COMPANION_EVOLUTION_CANNOT_FAIL", "\xddyile\xfetirme ba\xfear\xfds\xfdz olamaz"), 92, 3)

		btn = ui.Button()
		btn.SetParent(self)
		btn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		btn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		btn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		btn.SetPosition((self.WIDTH - 88) / 2, 300)
		btn.SetText(_T("PET_BTN_EVOLVE", "Evrim"))
		btn.SetEvent(ui.__mem_func__(self.__OnClickEvolve))
		btn.Show()
		self.evolveButton = btn

	def Open(self, evolution):
		self.evolution = evolution
		reqs = BUFFI_EVOLVE_REQUIREMENTS.get(evolution, ())
		self.items = tuple(r for r in reqs if r[0] != EVOLVE_YANG_VNUM)
		self.price = sum(r[1] for r in reqs if r[0] == EVOLVE_YANG_VNUM)
		self.evoText.SetText("Evrim %d -> %d" % (evolution, evolution + 1))
		self.levelText.SetText("Gereken seviye: %d" % BUFFI_EVOLVE_LEVEL[evolution])
		for i in xrange(len(self.SLOT_CENTER)):
			if i < len(self.items):
				self.slot.SetItemSlot(i, self.items[i][0], 0)
			else:
				self.slot.ClearSlot(i)
		self.slot.RefreshSlot()
		self.RefreshItemCounts()
		if not self.IsShow() and self.owner:
			x, y = self.owner.GetGlobalPosition()
			self.SetPosition(x + self.owner.GetWidth(), y)
		self.Show()
		self.SetTop()

	def Close(self):
		self.CloseQuestionDialog()
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshItemCounts(self):
		for i, line in enumerate(self.counts):
			if i < len(self.items):
				vnum, need = self.items[i]
				have = player.GetItemCountByVnum(vnum)
				line.SetText("%d/%d" % (min(have, need), need))
				line.SetPackedFontColor(EVOLVE_COLOR_OK if have >= need else EVOLVE_COLOR_LACK)
			else:
				line.SetText("")
		self.priceText.SetText(localeInfo.NumberToMoneyString(self.price))
		self.priceText.SetPackedFontColor(EVOLVE_COLOR_OK if player.GetElk() >= self.price else EVOLVE_COLOR_LACK)

	def OnUpdate(self):
		# inventory / yang changes while the window is open
		if app.GetTime() > self.updateTime:
			self.updateTime = app.GetTime() + 0.5
			self.RefreshItemCounts()

	def __OnSlotOverIn(self, index):
		if index < len(self.items) and self.toolTipItem:
			self.toolTipItem.SetItemToolTip(self.items[index][0])

	def __OnSlotOverOut(self):
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	def __OnClickEvolve(self):
		import uiCommon
		self.CloseQuestionDialog()
		dlg = uiCommon.QuestionDialog2()
		dlg.SetText1(_T("MOUNT_EVOLVE_CONFIRMATION_1", "Geli\xfeime devam etmek istedi\xf0inizden emin misiniz?"))
		dlg.SetText2(_T("MOUNT_EVOLVE_CONFIRMATION_2", "Evrim i\xe7in kullan\xfdlan itemler kaybolacak ve kurtar\xfdlamaz"))
		dlg.SetAcceptEvent(ui.__mem_func__(self.__SendUpgradeEvolution))
		dlg.SetCancelEvent(ui.__mem_func__(self.CloseQuestionDialog))
		dlg.Open()
		self.questionDialog = dlg

	def __SendUpgradeEvolution(self):
		self.CloseQuestionDialog()
		net.SendChatPacket("/support_set_evolution")

	def CloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None


class CompanionWindow(ui.Window):

	def __init__(self):
		ui.Window.__init__(self)
		self.interface = None
		self.tab = TAB_BUFFI
		self.children = {}
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		self.buffiSummoned = False
		self.buffiName = "Buffi"
		self.buffiInfo = [1, 120, 0, 0, 0, 0, 0, 0, 0]
		self.buffiSkillPts = 0
		self.buffiSkillAmount = [0] * 6
		self.buffiSkillColor = [[0] * 5 for i in xrange(5)]
		self.skillColorWnd = None
		self.evolutionWnd = None
		self.buffiSkillPower = [0] * 5
		self.buffiSkillBook = [0] * 5
		self.buffiSkillCooldown = [0] * 5
		self.buffiInfoTime = app.GetTime()
		self.bonusPos = 0
		self.keep = []	# ui objects are destroyed when no python reference is left

		self.__LoadWindow()
		self.__SendBuffiOptions()

	def __SendBuffiOptions(self):
		# tell the server the auto unequip (PvP) / auto equip (PvM) preferences on login
		try:
			import advanced_options_runtime
			pvp = bool(advanced_options_runtime.Get("auto_unequip_buffi_pvp", False))
			pvm = bool(advanced_options_runtime.Get("auto_equip_buffi_pvm", True))
			net.SendChatPacket("/buffi_option %d %d" % (pvp, pvm))
		except Exception:
			pass

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.Hide()
		if self.evolutionWnd:
			self.evolutionWnd.Destroy()
			self.evolutionWnd = None
		self.children = {}
		self.keep = []
		self.interface = None
		self.toolTip = None

	def BindInterface(self, interface):
		self.interface = interface

	# ------------------------------------------------------------------ helpers
	def __Image(self, parent, path, x, y, expanded = False):
		img = ui.ExpandedImageBox() if expanded else ui.ImageBox()
		img.SetParent(parent)
		img.LoadImage(path)
		img.SetPosition(x, y)
		img.AddFlag("not_pick")
		img.Show()
		self.keep.append(img)
		return img

	def __Text(self, parent, text, x, y, center = True):
		line = ui.TextLine()
		line.SetParent(parent)
		line.SetPosition(x, y)
		if center:
			line.SetHorizontalAlignCenter()
		line.SetOutline()
		line.SetText(text)
		line.Show()
		self.keep.append(line)
		return line

	def __Button(self, parent, name, x, y, text, event):
		btn = ui.Button()
		btn.SetParent(parent)
		btn.SetPosition(x, y)
		btn.SetUpVisual(COMPANION_PATH.format("/btn_%s_norm.png" % name))
		btn.SetOverVisual(COMPANION_PATH.format("/btn_%s_hover.png" % name))
		btn.SetDownVisual(COMPANION_PATH.format("/btn_%s_select.png" % name))
		btn.SetText(text)
		btn.Show()
		btn.SetEvent(event)
		return btn

	def __Subheader(self, parent, text, x, y):
		bg = self.__Image(parent, COMPANION_PATH.format("/subheader_bg.png"), x, y)
		self.__Text(bg, text, 66, 6)
		return bg

	# ------------------------------------------------------------------ layout
	def __LoadWindow(self):
		self.SetSize(PET_WIDTH + 30, PET_HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetPosition(300, 120)

		tabs = []
		for i, name in enumerate(("pet", "horse", "buffi")):
			btn = ui.RadioButton()
			btn.SetParent(self)
			btn.SetPosition(0, 30 + 34 * i)
			btn.SetUpVisual(COMPANION_PATH.format("/btn_%s_norm.png" % name))
			btn.SetOverVisual(COMPANION_PATH.format("/btn_%s_hover.png" % name))
			btn.SetDownVisual(COMPANION_PATH.format("/btn_%s_select.png" % name))
			btn.SetEvent(ui.__mem_func__(self.__OnClickTab), i)
			btn.Show()
			tabs.append(btn)
		self.tabButtons = tabs

		board = ui.BoardWithTitleBar()
		board.SetParent(self)
		board.SetSize(PET_WIDTH, PET_HEIGHT)
		board.SetPosition(30, 0)
		board.AddFlag("attach")
		board.SetCloseEvent(ui.__mem_func__(self.Close))
		board.SetTitleName(_T("PET_INFORMATION_TITLE", "Yolda\xfe"))
		board.Show()
		self.board = board

		self.__LoadBuffiWindow()
		self.__SelectTab(TAB_BUFFI)

	def __LoadBuffiWindow(self):
		c = {}
		win = ui.Window()
		win.SetParent(self.board)
		win.SetPosition(3, 2)	# fine offset measured against the reference screenshot
		win.SetSize(PET_WIDTH, PET_HEIGHT)
		win.AddFlag("not_pick")
		win.Show()
		c["buffi_window"] = win

		# Layout measured on the reference Buffi tab (board-local pixels, board 280x415).
		# Left column x=5 (130 wide), right column x=136. Content frame = background.png.
		c["backgound"] = self.__Image(win, COMPANION_PATH.format("/background.png"), 4, 28)

		# portrait + Name / Level / Intelligence
		c["slot_bg"] = self.__Image(win, COMPANION_PATH.format("/buffi_bg.png"), 8, 29)
		titles = ("Name", "Level", "Intelligence")
		for i in xrange(3):
			y = 31 + 22 * i
			c["bg_desc_%d" % i] = self.__Image(win, COMPANION_PATH.format("/detail_title.png"), 74, y)
			c["desc_%d" % i] = self.__Text(c["bg_desc_%d" % i], titles[i], 8, 3, False)
			c["bg_value_%d" % i] = self.__Image(win, COMPANION_PATH.format("/detail_value.png"), 166, y)
			c["value_%d" % i] = self.__Text(c["bg_value_%d" % i], "-", 49, 3)

		info = ui.Button()
		info.SetParent(c["bg_desc_2"])
		info.SetPosition(72, 2)
		info.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		info.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
		info.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
		info.SetOverEvent(ui.__mem_func__(self.__OverInIntInfo))
		info.SetOverOutEvent(ui.__mem_func__(self.__OverOut))
		info.Show()
		c["int_info_btn"] = info

		# EXP header + orb box | Evolve / Duration buttons
		c["subheader_bg_1"] = self.__Subheader(win, "EXP", 4, 96)
		c["exp_bg"] = self.__Image(win, COMPANION_PATH.format("/detail_exp_bg.png"), 5, 120)
		balls = []
		for i in xrange(4):
			x = 20 + 25 * i
			self.__Image(win, COMPANION_PATH.format("/progress_empty.png"), x, 127)
			balls.append(self.__Image(win, COMPANION_PATH.format("/progress.png"), x, 127, True))
		c["exp_balls"] = balls

		expHover = HoverArea(ui.__mem_func__(self.__OverInExp), ui.__mem_func__(self.__OverOut))
		expHover.SetParent(win)
		expHover.SetPosition(5, 120)
		expHover.SetSize(c["exp_bg"].GetWidth(), c["exp_bg"].GetHeight())
		expHover.Show()
		self.keep.append(expHover)

		c["evolve_btn"] = self.__Button(win, "evolve", 135, 95, _T("PET_BTN_EVOLVE", "Evrim"), ui.__mem_func__(self.OnClickEvolutionBuffi))
		c["duration_btn"] = self.__Button(win, "duration", 135, 126, _T("SUPPORT_DURATION_BUTTON", "S\xfcre"), ui.__mem_func__(self.OnClickDurationBuffi))

		# Ore header + slots | Duration header + gauge
		c["subheader_bg_2"] = self.__Subheader(win, _T("ORE_NAME", "cevher"), 4, 156)
		c["subheader_bg_3"] = self.__Subheader(win, _T("PET_INFORMATION_DURATION", "S\xfcre"), 135, 156)
		c["runes_bg"] = self.__Image(win, COMPANION_PATH.format("/details_bg.png"), 5, 180)
		c["duration_bg"] = self.__Image(win, COMPANION_PATH.format("/details_bg.png"), 136, 180)

		# slot frames first: a sibling created later is drawn on top and would hide the ore icons
		for i in xrange(3):
			self.__Image(win, "d:/ymir work/ui/game/common/slots/slot_runes.png", 9 + 41 * i, 185)
		runes = ui.SlotWindow()
		runes.SetParent(win)
		runes.SetPosition(9, 185)
		runes.SetSize(118, 36)
		for i in xrange(3):
			runes.AppendSlot(i, 41 * i + 2, 2, 32, 32)
		runes.SetOverInItemEvent(ui.__mem_func__(self.__OverInRune))
		runes.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))
		runes.SetSelectEmptySlotEvent(ui.__mem_func__(self.__OnSelectEmptyRune))
		runes.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnRemoveRune))
		runes.SetUnselectItemSlotEvent(ui.__mem_func__(self.__OnRemoveRune))
		runes.Show()
		c["runes"] = runes

		c["duration_empty"] = self.__Image(win, COMPANION_PATH.format("/gauge_empty.png"), 141, 186)
		c["duration_full"] = self.__Image(c["duration_empty"], COMPANION_PATH.format("/gauge_full.png"), 2, 2, True)
		c["duration_text"] = self.__Text(win, "", 201, 207)

		# Skills header + row
		c["header_bg_3"] = self.__Image(win, COMPANION_PATH.format("/header_title.png"), 4, 227)
		c["header_3"] = self.__Text(c["header_bg_3"], "", 130, 7)
		c["skill_bg"] = self.__Image(win, COMPANION_PATH.format("/skill_background.png"), 5, 254)

		grid = ui.GridSlotWindow()
		grid.SetParent(win)
		grid.SetPosition(29, 262)
		grid.ArrangeSlot(0, 5, 1, 32, 32, 13, 0)
		grid.SetOverInItemEvent(ui.__mem_func__(self.__OverInItemSkill))
		grid.SetOverOutItemEvent(ui.__mem_func__(self.__OverOut))
		grid.Show()
		c["buffi_skill"] = grid

		plus = []
		colors = []
		for i in xrange(5):
			x = 29 + 45 * i
			btn = ui.Button()
			btn.SetParent(win)
			btn.SetPosition(x, 262)
			btn.SetUpVisual("d:/ymir work/ui/game/windows/btn_plus_up.sub")
			btn.SetOverVisual("d:/ymir work/ui/game/windows/btn_plus_over.sub")
			btn.SetDownVisual("d:/ymir work/ui/game/windows/btn_plus_down.sub")
			btn.SetEvent(ui.__mem_func__(self.__OnPressedSlotButton), i)
			btn.Hide()
			plus.append(btn)

			# skill colour: only offered once the skill reached Perfect Master
			col = ui.Button()
			col.SetParent(win)
			col.SetPosition(x + 27, 261)
			col.SetUpVisual("d:/ymir work/ui/game/skillcolor/skill_color_button_default.tga")
			col.SetOverVisual("d:/ymir work/ui/game/skillcolor/skill_color_button_over.tga")
			col.SetDownVisual("d:/ymir work/ui/game/skillcolor/skill_color_button_down.tga")
			col.SetEvent(ui.__mem_func__(self.__OnPressBuffiSkillColorButton), i)
			col.Hide()
			colors.append(col)
		c["skill_plus"] = plus
		c["skill_color"] = colors

		# Bonus list (SUPPORT_SKILL_AMOUNT_DESC_*), two rows visible with a scrollbar
		self.bonusDesc = (
			(0, _T("SUPPORT_SKILL_AMOUNT_DESC_94", "Yak\xfdn d\xf6v\xfc\xfe sald\xfdr\xfdlar\xfdna kar\xfe\xfd diren\xe7: %s"), "%d%%"),
			(1, _T("SUPPORT_SKILL_AMOUNT_DESC_95", "Yans\xfdyan yak\xfdn d\xf6v\xfc\xfe hasar\xfd: %s"), "%d%%"),
			(2, _T("SUPPORT_SKILL_AMOUNT_DESC_96", "Kritik Vuru\xfe \xdeans\xfd: %s"), "%d%%"),
			(3, _T("SUPPORT_SKILL_AMOUNT_DESC_110", "Hareket h\xfdz\xfd: %s"), "%d%%"),
			(4, _T("SUPPORT_SKILL_AMOUNT_DESC_111", "Sald\xfdr\xfd G\xfcc\xfc: +%s"), "+%d"),
			(5, _T("SUPPORT_SKILL_AMOUNT_DESC_110_B", "B\xfcy\xfc H\xfdz\xfd: %s"), "%d%%"),
		)
		listWin = ui.Window()
		listWin.SetParent(win)
		listWin.SetPosition(8, 300)
		listWin.SetSize(250, BONUS_ROW_HEIGHT * BONUS_VISIBLE_ROWS)
		listWin.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheelBonus))
		listWin.Show()
		c["bonus_list"] = listWin
		rows = []
		for i in xrange(BONUS_VISIBLE_ROWS):
			bg = self.__Image(listWin, COMPANION_PATH.format("/bonus_bg.png"), 0, BONUS_ROW_HEIGHT * i, True)
			bg.SetRenderingRect(0.0, 0.0, -0.027, 0.0)
			desc = self.__Text(bg, "", 7, 6, False)
			value = self.__Text(bg, "", 207, 6)
			rows.append((desc, value))
		c["bonus_rows"] = rows

		scroll = ThinScrollBar()
		scroll.SetParent(win)
		scroll.SetPosition(261, 300)
		scroll.SetScrollBarSize(BONUS_ROW_HEIGHT * BONUS_VISIBLE_ROWS)
		scroll.SetMiddleBarSize(float(BONUS_VISIBLE_ROWS) / len(self.bonusDesc))
		scroll.SetScrollEvent(ui.__mem_func__(self.__OnScrollBonus))
		scroll.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheelBonus))
		scroll.Show()
		c["bonus_scroll"] = scroll

		# Costume visibility toggles (body / hair / weapon / sash)
		names = (("V\xfccut", 0), ("Sa\xe7", 1), ("Silah", 2), ("Ku\xfeak", 3))
		c["costume_visibility"] = []
		hidden = getattr(constInfo, "BUFFI_COSTUME_HIDDEN_STATES", (0, 0, 0, 0))
		for label, idx in names:
			x = 8 + 130 * (idx % 2)
			y = 361 + 22 * (idx / 2)
			btn = ui.ToggleButton()
			btn.SetParent(win)
			btn.SetPosition(x, y + 2)
			btn.SetUpVisual("d:/ymir work/ui/game/common/eye_normal_01.tga")
			btn.SetOverVisual("d:/ymir work/ui/game/common/eye_normal_02.tga")
			btn.SetDownVisual("d:/ymir work/ui/game/common/eye_closed_01.tga")
			btn.SetToggleDownEvent(lambda i = idx, f = ui.__mem_func__(self.SetBuffiCostumeVisibility): f(i, False))
			btn.SetToggleUpEvent(lambda i = idx, f = ui.__mem_func__(self.SetBuffiCostumeVisibility): f(i, True))
			if idx < len(hidden) and hidden[idx]:
				btn.Down()
			btn.Show()
			c["costume_visibility"].append(btn)
			self.__Text(win, label, x + 19, y, False)

		# shown while no Buffi is summoned
		notb = ui.ThinBoardCircle()
		notb.SetParent(win)
		notb.SetPosition(8, 96)
		notb.SetSize(PET_WIDTH - 16, PET_HEIGHT - 104)
		notb.Hide()
		c["not_buffi"] = notb
		c["not_buffi_text"] = self.__Text(notb, _T("BUFFI_NO_SPAWN", "Aktif buffi'niz yok"), (PET_WIDTH - 16) / 2, (PET_HEIGHT - 104) / 2 - 6)

		self.children["buffi"] = c
		self.__RefreshBonusList()
		self.UnsummonBuffi()

	# ------------------------------------------------------------------ tabs
	def __OnClickTab(self, tab):
		if tab == TAB_PET and self.interface and hasattr(self.interface, "TogglePetInformationWindow"):
			self.Close()
			self.interface.TogglePetInformationWindow()
			return
		self.__SelectTab(TAB_BUFFI)

	def __SelectTab(self, tab):
		self.tab = tab
		for i, btn in enumerate(self.tabButtons):
			if i == tab:
				btn.Down()
			else:
				btn.SetUp()

	def Open(self):
		self.Show()
		self.SetTop()
		net.SendChatPacket("/buffi_info")

	def Close(self):
		self.toolTip.HideToolTip()
		self.Hide()

	def Toggle(self):
		if self.IsShow():
			self.Close()
		else:
			self.Open()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	# ------------------------------------------------------------------ data from server
	def SetBuffiName(self, name):
		self.buffiName = name
		self.children["buffi"]["value_0"].SetText(name)

	def SetBuffiInformation(self, level, intel, exp, exp_total, evo, duration, runes_1, runes_2, runes_3):
		self.buffiInfo = [level, intel, exp, exp_total, evo, duration, runes_1, runes_2, runes_3]
		self.buffiInfoTime = app.GetTime()
		g_buffiState["level"] = level
		g_buffiState["duration"] = duration
		g_buffiState["time"] = self.buffiInfoTime
		if not self.buffiSummoned:
			self.buffiSummoned = True
			self.children["buffi"]["not_buffi"].Hide()
		self.UpdateBuffiInformation()
		self.__RefreshGameButton()

	def SetBuffiSkills(self, pts, *amountPower):
		self.buffiSkillPts = pts
		for i in xrange(5):
			self.buffiSkillAmount[i] = amountPower[i * 2]
			self.buffiSkillPower[i] = amountPower[i * 2 + 1]
		g_buffiState["skills"] = list(self.buffiSkillPower)
		g_buffiState["points"] = pts
		self.UpdateBuffiSkill()
		self.__RefreshGameButton()

	def SetBuffiSkillsBook(self, *books):
		self.buffiSkillBook = list(books[:5])

	def SetBuffiSkillsCooldown(self, *cooldowns):
		self.buffiSkillCooldown = list(cooldowns[:5])

	def GetBuffiSkillPts(self):
		return self.buffiSkillPts if self.buffiSummoned else 0

	def GetBuffiSkillsPower(self):
		return self.buffiSkillPower

	def GetBuffiSkillsBook(self):
		return self.buffiSkillBook

	def GetBuffiSkillsCooldown(self):
		return self.buffiSkillCooldown

	def __RefreshGameButton(self):
		if self.interface and hasattr(self.interface, "CheckGameButton"):
			self.interface.CheckGameButton()

	def UnsummonBuffi(self):
		self.buffiSummoned = False
		self.children["buffi"]["not_buffi"].Show()
		self.__RefreshGameButton()

	def UpdateBuffiInformation(self):
		c = self.children["buffi"]
		level, intel, exp, exp_total, evo, duration = self.buffiInfo[:6]

		c["value_0"].SetText(self.buffiName)
		c["value_1"].SetText("%d" % level)
		# runes that ran out since the last server update no longer add their +3 INT
		expired = 0
		for i in xrange(3):
			if self.buffiInfo[6 + i] > 0 and self.__GetRuneRemain(i) == 0:
				expired += 1
		c["value_2"].SetText("%d" % (intel - 3 * expired))

		ratio = float(exp) / float(exp_total) if exp_total > 0 else 1.0
		for i, ball in enumerate(c["exp_balls"]):
			part = min(1.0, max(0.0, ratio * 4 - i))
			ball.SetRenderingRect(0.0, part - 1.0, 0.0, 0.0)

		self.__UpdateDuration()

		runes = c["runes"]
		for i in xrange(3):
			v = self.__GetRuneRemain(i)
			if v == 0:
				runes.ClearSlot(i)
			else:
				runes.SetItemSlot(i, BUFFI_ORE_PERM_VNUM if v < 0 else BUFFI_ORE_VNUM, 0)
		runes.RefreshSlot()

	def __GetRuneRemain(self, idx):
		# -1 permanent, 0 empty / ran out, otherwise seconds left (the time runs even while the Buffi is away)
		v = self.buffiInfo[6 + idx]
		if v <= 0:
			return v
		return max(0, v - int(app.GetTime() - self.buffiInfoTime))

	def __GetRemainDuration(self):
		duration = self.buffiInfo[5]
		if duration < 0:
			return -1
		return max(0, duration - int(app.GetTime() - self.buffiInfoTime))

	def __UpdateDuration(self):
		c = self.children["buffi"]
		remain = self.__GetRemainDuration()
		if remain < 0:
			c["duration_full"].SetPercentage(1, 1)
			c["duration_text"].SetText(_T("BUFFI_DURATION_PERM", "Kal\xfdc\xfd"))
			return
		c["duration_full"].SetPercentage(min(remain, BUFFI_DURATION_MAX), BUFFI_DURATION_MAX)
		c["duration_text"].SetText("%s / 30g (%d%%)" % (_FormatTime(remain), remain * 100 / BUFFI_DURATION_MAX))

	def UpdateBuffiSkill(self):
		c = self.children["buffi"]
		c["header_3"].SetText("%s [%d]" % (_T("SUPPORT_TITLE_SKILL", "Beceriler"), self.buffiSkillPts))
		grid = c["buffi_skill"]
		for i in xrange(5):
			level = self.buffiSkillPower[i]
			grade, shown = _SkillGrade(level)
			grid.ClearSlot(i)
			grid.SetSkillSlotNew(i, BUFF_SKILL_INDEX[i], grade, max(1, shown))
			grid.SetSlotCountNew(i, grade, shown)
			# reference: "+" up to Grand Master (points until M1, skill books from M1 to G1)
			bookMax = 40 if getattr(app, "ENABLE_BUFFI_SKILL_BOOK", 0) else 30
			if (level < 20 and self.buffiSkillPts > 0) or 20 <= level < bookMax:
				c["skill_plus"][i].Show()
			else:
				c["skill_plus"][i].Hide()
			c["skill_color"][i].Show()
		grid.RefreshSlot()
		self.__RefreshBonusList()

	def __RefreshBonusList(self):
		c = self.children["buffi"]
		total = len(self.bonusDesc)
		for row, (desc, value) in enumerate(c["bonus_rows"]):
			idx = self.bonusPos + row
			if idx >= total:
				desc.SetText("")
				value.SetText("")
				continue
			skill, text, fmt = self.bonusDesc[idx]
			label = text.replace("%s", "").replace("+", "").rstrip(": ").strip()
			if len(label) > 30:
				label = label[:28] + ".."
			desc.SetText(label)
			value.SetText(fmt % self.buffiSkillAmount[skill])

	def __OnWheelBonus(self, down):
		maxPos = max(0, len(self.bonusDesc) - BONUS_VISIBLE_ROWS)
		if maxPos <= 0:
			return
		pos = min(maxPos, max(0, self.bonusPos + (1 if down else -1)))
		self.children["buffi"]["bonus_scroll"].SetPos(float(pos) / maxPos)

	def __OnScrollBonus(self):
		maxPos = max(0, len(self.bonusDesc) - BONUS_VISIBLE_ROWS)
		self.bonusPos = int(round(self.children["buffi"]["bonus_scroll"].GetPos() * maxPos))
		self.__RefreshBonusList()

	def OnUpdate(self):
		if not self.IsShow():
			return
		self.__UpdateDuration()
		now = app.GetTime()
		if now - getattr(self, "runeCheckTime", 0) >= 1.0:
			self.runeCheckTime = now
			active = tuple(self.__GetRuneRemain(i) != 0 for i in xrange(3))
			if active != getattr(self, "runeActive", active):
				self.UpdateBuffiInformation()
			self.runeActive = active

	# ------------------------------------------------------------------ actions
	def __OnSelectEmptyRune(self, slot):
		if not mouseModule.mouseController.isAttached():
			return
		if mouseModule.mouseController.GetAttachedType() != player.SLOT_TYPE_INVENTORY:
			return
		vnum = mouseModule.mouseController.GetAttachedItemIndex()
		cell = mouseModule.mouseController.GetAttachedSlotNumber()
		mouseModule.mouseController.DeattachObject()
		if vnum in (BUFFI_ORE_VNUM, BUFFI_ORE_PERM_VNUM):
			self.SendAddOreBuffiPacket(cell)

	def __OnRemoveRune(self, slot):
		# take the ore out of the slot (timed ores keep their remaining time)
		if self.__GetRuneRemain(slot) != 0:
			self.toolTip.HideToolTip()
			net.SendChatPacket("/support_remove_ore %d" % slot)

	def SendAddOreBuffiPacket(self, cell):
		net.SendChatPacket("/support_add_ore %d" % cell)

	def SendBuffiDurationPacket(self):
		net.SendChatPacket("/support_full_duration")

	def OnClickDurationBuffi(self):
		self.SendBuffiDurationPacket()

	def OnClickEvolutionBuffi(self):
		import chat
		if not getattr(app, "ENABLE_BUFFI_EVOLUTION", 0):
			return
		if not self.buffiSummoned:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "\xd6nce Buffi'nizi \xe7a\xf0\xfdr\xfdn.")
			return
		evo = self.buffiInfo[4]
		if evo >= BUFFI_MAX_EVOLUTION:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Buffi'niz daha fazla evrimle\xfeemez.")
			return
		if not self.evolutionWnd:
			self.evolutionWnd = BuffiEvolutionWindow(self)
		self.evolutionWnd.Open(evo)

	def OnBuffiEvolutionResult(self, success):
		if self.evolutionWnd:
			self.evolutionWnd.Close()

	def __OnPressBuffiSkillColorButton(self, slot):
		if self.skillColorWnd:
			self.skillColorWnd.Close()
			self.skillColorWnd = None
		wnd = BuffiSkillColorWindow(self, slot, BUFF_SKILL_INDEX[slot])
		wnd.Show()
		self.skillColorWnd = wnd

	def SetBuffiSkillColor(self, idx, colors):
		if 0 <= idx < 5:
			self.buffiSkillColor[idx] = list(colors[:5])

	def GetBuffiSkillColor(self, idx):
		return self.buffiSkillColor[idx]

	def SetBuffiCastSpeed(self, value):
		self.buffiSkillAmount[5] = value
		self.__RefreshBonusList()

	def __OnPressedSlotButton(self, slot):
		# books / soul stones are read in the Skill Upgrader (reference: + opens it on the Buffi page)
		if getattr(app, "ENABLE_SKILL_UPGRADE", 0) and self.buffiSkillPower[slot] >= 20 and self.interface:
			self.interface.OpenSkillUpgrade(2, BUFF_SKILL_INDEX[slot])
			return
		net.SendChatPacket("/support_skill_power %d" % slot)

	def SetBuffiCostumeVisibility(self, idx, visible):
		states = list(getattr(constInfo, "BUFFI_COSTUME_HIDDEN_STATES", (0, 0, 0, 0)))
		while len(states) < 4:
			states.append(0)
		states[idx] = 0 if visible else 1
		constInfo.BUFFI_COSTUME_HIDDEN_STATES = tuple(states)

	def RefreshBuffiCostumeVisibility(self):
		hidden = getattr(constInfo, "BUFFI_COSTUME_HIDDEN_STATES", (0, 0, 0, 0))
		for idx, btn in enumerate(self.children["buffi"]["costume_visibility"]):
			if idx < len(hidden) and hidden[idx]:
				btn.Down()
			else:
				btn.SetUp()

	# ------------------------------------------------------------------ tooltips
	def __ShowLines(self, lines):
		self.toolTip.ClearToolTip()
		self.toolTip.toolTipWidth = self.toolTip.TOOL_TIP_WIDTH
		for text in lines:
			self.toolTip.AppendTextLine(text)
		self.toolTip.ShowToolTip()

	def __OverOut(self):
		self.toolTip.HideToolTip()

	# used by the Skill Upgrader (uiSkillUpgrade) for its Buffi page
	def ShowBuffiSkillToolTip(self, slot):
		if 0 <= slot < len(BUFF_SKILL_INDEX):
			self.__OverInItemSkill(slot)

	def HideBuffiSkillToolTip(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

	def __OverInRune(self, slot):
		v = self.__GetRuneRemain(slot)
		if v == 0:
			return
		self.__ShowLines(("%s +3 %s" % (_T("ORE_NAME", "cevher"), _T("BUFFI_INT_TABLE_INTELLIGENCE", "Zeka")), _T("BUFFI_DURATION_PERM", "Kal\xfdc\xfd") if v < 0 else _FormatTime(v), _T("BUFFI_ORE_REMOVE_HINT", "\xc7\xfdkarmak i\xe7in t\xfdkla")))

	def __OverInIntInfo(self):
		# reference: title + two column table (Evrim | Zeka)
		t = self.toolTip
		t.ClearToolTip()
		t.toolTipWidth = 240
		t.AppendTextLine(_T("BUFFI_INT_TOOLTIP", "Buffi'nin zekas\xfd evrim ge\xe7irdik\xe7e artacakt\xfdr"))
		t.AppendSpace(4)
		stages = (_T("BUFFI_INT_STAGE_DEFAULT", "Varsay\xfdlan"), _T("BUFFI_INT_STAGE_1", "Evrim 1 (Seviye 55)"),
			_T("BUFFI_INT_STAGE_2", "Evrim 2 (Seviye 75)"), _T("BUFFI_INT_STAGE_3", "Evrim 3 (Seviye 90)"))
		rows = [(_T("BUFFI_INT_TABLE_EVOLUTION", "Evrim"), _T("BUFFI_INT_TABLE_INTELLIGENCE", "Zeka"))]
		for i in xrange(4):
			rows.append((stages[i], str(BUFFI_INT_STAGES[i])))

		left, width, rowH, split = 10, t.toolTipWidth - 20, 22, 150
		top = t.toolTipHeight
		for r, (col1, col2) in enumerate(rows):
			y = top + r * rowH
			bar = ui.Bar()
			bar.SetParent(t)
			bar.SetPosition(left, y)
			bar.SetSize(width, rowH)
			bar.SetColor(0xff2a2018 if r == 0 else (0x40000000 if r % 2 else 0x20000000))
			bar.AddFlag("not_pick")
			bar.Show()
			t.childrenList.append(bar)
			for x, text in ((left + split / 2, col1), (left + split + (width - split) / 2, col2)):
				line = ui.TextLine()
				line.SetParent(t)
				line.SetPosition(x, y + 4)
				line.SetHorizontalAlignCenter()
				line.SetOutline()
				line.SetPackedFontColor(t.TITLE_COLOR if r == 0 else t.FONT_COLOR)
				line.SetText(text)
				line.Show()
				t.childrenList.append(line)
		# grid lines
		for x, y, w, h in [(left, top + r * rowH, width, 1) for r in xrange(len(rows) + 1)] + \
				[(left, top, 1, rowH * len(rows)), (left + split, top, 1, rowH * len(rows)), (left + width, top, 1, rowH * len(rows))]:
			ln = ui.Bar()
			ln.SetParent(t)
			ln.SetPosition(x, y)
			ln.SetSize(w, h)
			ln.SetColor(0xff5a4a3a)
			ln.AddFlag("not_pick")
			ln.Show()
			t.childrenList.append(ln)
		t.toolTipHeight = top + rowH * len(rows) + 8
		t.ResizeToolTip()
		t.ShowToolTip()

	def __OverInExp(self):
		exp, total = self.buffiInfo[2], self.buffiInfo[3]
		t = self.toolTip
		t.ClearToolTip()
		t.toolTipWidth = 160
		t.AppendTextLine("EXP", t.TITLE_COLOR)
		t.AppendTextLine("%d / %d" % (exp, total))
		t.ShowToolTip()

	def __OverInItemSkill(self, slot):
		name, nameG, nameP, desc, cond1, cond2, valueFmt = BUFFI_SKILL_TOOLTIP[slot]
		level = self.buffiSkillPower[slot]
		grade, shown = _SkillGrade(level)

		t = self.toolTip
		t.ClearToolTip()
		t.toolTipWidth = 200
		if grade:
			# reference: the next stage name over "<name> Master"
			nextName = (nameG, nameG, nameP, "")[grade]
			if nextName:
				t.AppendTextLine(nextName, BUFFI_TT_WHITE)
			t.AppendTextLine((name, name, nameG, nameP)[grade] + BUFFI_GRADE_SUFFIX[grade], BUFFI_TT_GREEN)
		else:
			t.AppendTextLine(name, BUFFI_TT_WHITE)
		t.AppendSpace(5)
		for i in xrange(grpText.GetSplitingTextLineCount(desc, 30)):
			t.AppendTextLine(grpText.GetSplitingTextLine(desc, 30, i), BUFFI_TT_WHITE)
		t.AppendSpace(5)
		t.AppendTextLine(cond1, BUFFI_TT_YELLOW)
		t.AppendTextLine(cond2, BUFFI_TT_YELLOW)
		t.AppendTextLine(BUFFI_INT_POWERED, BUFFI_TT_YELLOW)
		t.AppendSpace(5)
		lines = BUFFI_SKILL_VALUE[slot]
		t.AppendTextLine(lines[0] % self.buffiSkillAmount[slot], BUFFI_TT_WHITE)
		if len(lines) > 1:
			t.AppendTextLine(lines[1] % self.buffiSkillAmount[5], BUFFI_TT_WHITE)
		t.ShowToolTip()
