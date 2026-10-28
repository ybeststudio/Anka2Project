import app
import net
import player
import item
import skill
import chat
import ui
import uiToolTip
import localeInfo
import constInfo
import wndMgr

# Skill Upgrader (reference uiSkillUpgrade.SkillUpgradeWindow / UIScript/skillupgrade.py). The server reads the books
# ("/upgrade_skill <type> <skill> <count> <exorcism> <hermit's advice> <zen bean> <allow negative rank> [mastery]") and
# answers with SkillUpgradeInfo / SkillUpgradeResult / SkillUpgradeNegativeGradeConfirm.
SKILL_PATH = "d:/ymir work/ui/game/skill_upgrade{}"
COMMON_PATH = "d:/ymir work/ui/game/common{}"

UPGRADE_STANDARD_SKILL = 0
UPGRADE_PASSIF_SKILL = 1
UPGRADE_BUFFI_SKILL = 2

STANDARD_SKILL_SLOTS = (1, 2, 3, 4, 5, 6)
PASSIVE_SKILL_INDEX = (121, 122, 124, 129)
BUFFI_SKILL_INDEX = (94, 95, 96, 110, 111)
BUFFI_MASTER_ITEMS = (57107, 57108)

TIME_ITEM_VNUM = 71001
CHANCE_ITEM_VNUM = 71094
ZEN_BEAN_VNUM = 70102
BOOST_ITEMS = (TIME_ITEM_VNUM, CHANCE_ITEM_VNUM, ZEN_BEAN_VNUM)
MAX_COUNT = 20
QUICK_COUNT = 10

WIDTH = 256
HEIGHT = 273
COLOR_GOLD = 0xffc79e65
COLOR_RED = 0xfff7230c
COLOR_WHITE = 0xffffffff

# boost check boxes are remembered for the session (reference keeps them while the client runs)
g_boostState = [False, False, False]
g_whisperAlert = {}
g_alertCheckTime = [0.0]


def CheckAlerts(interface):
	# reference SkillUpgradeWindow.CheckTime: a whisper when a watched skill can be read again
	now = app.GetTime()
	if not g_whisperAlert or now < g_alertCheckTime[0]:
		return
	g_alertCheckTime[0] = now + 1.0
	for key, (endTime, name) in g_whisperAlert.items():
		if not endTime or now < endTime:	# 0: armed, waits for the next read delay
			continue
		del g_whisperAlert[key]
		chat.AppendWhisper(chat.WHISPER_TYPE_SYSTEM, "[Hat\xfdrlat\xfdc\xfd]", "%s becerisinin bekleme s\xfcresi doldu, art\xfdk kitap okuyabilirsin." % name)
		if interface:
			interface.RecvWhisper("[Hat\xfdrlat\xfdc\xfd]")
			wnd = getattr(interface, "wndSkillUpgrade", None)
			if wnd:
				wnd.RefreshWhisperToggle()


def _T(name, default):
	return getattr(localeInfo, name, default)


def _BuffiGrade(level):
	if level >= 40:
		return 3, 1
	if level >= 30:
		return 2, level - 29
	if level >= 20:
		return 1, level - 19
	return 0, level


class SkillUpgradeWindow(ui.BoardWithTitleBar):

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.interface = None
		self.keep = []
		self.upgradeType = UPGRADE_STANDARD_SKILL
		self.skills = []		# [(skillIndex, grade, level, player slot / Buffi index)] of the current tab
		self.selected = -1
		self.info = None		# (bookVnum, count, progress, need, maxed) of the selected skill
		self.readEnd = 0.0		# app.GetTime() when the selected skill can be read again
		self.alertKey = None
		self.questionDialog = None
		self.effectEnd = 0.0
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()
		self.toolTipItem = uiToolTip.ItemToolTip()
		self.toolTipItem.HideToolTip()
		self.__LoadWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		self.Close()
		self.keep = []
		self.interface = None
		self.toolTip = None
		self.toolTipItem = None

	def BindInterface(self, interface):
		self.interface = interface

	# ------------------------------------------------------------------ helpers
	def __Image(self, parent, path, x, y):
		img = ui.ImageBox()
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

	def __Effect(self, name):
		ani = ui.AniImageBox()
		ani.SetParent(self)
		for i in xrange(1, 10):
			ani.AppendImage(COMMON_PATH.format("/animation/%s/%d.png" % (name, i)))
		ani.SetDelay(6)
		ani.SetPosition((WIDTH - 132) / 2, 154)
		ani.AddFlag("not_pick")
		ani.Hide()
		self.keep.append(ani)
		return ani

	# ------------------------------------------------------------------ layout
	def __LoadWindow(self):
		self.SetSize(WIDTH, HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetTitleName(_T("SKILL_UPGPRADE_TITLE", "Beceri Y\xfckseltici"))
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.SetCenterPosition()

		self.__Image(self, SKILL_PATH.format("/bg.png"), 7, 30)

		self.tabs = []
		titles = (_T("SKILL_UPGPRADE_STANDARD", "Standart"), _T("SKILL_UPGPRADE_PASSIVE", "Pasif"), _T("SKILL_UPGPRADE_BUFFI", "Buffi"))
		for i in xrange(3):
			btn = ui.RadioButton()
			btn.SetParent(self)
			btn.SetPosition(9 + 79 * i, 31)
			btn.SetUpVisual(SKILL_PATH.format("/btn_category_norm.png"))
			btn.SetOverVisual(SKILL_PATH.format("/btn_category_hover.png"))
			btn.SetDownVisual(SKILL_PATH.format("/btn_category_select.png"))
			btn.SetText(titles[i])
			btn.SetEvent(ui.__mem_func__(self.SetPage), i)
			btn.Show()
			self.tabs.append(btn)

		self.chooseText = self.__Text(self, "", WIDTH / 2, 63)

		alarm = ui.Button()
		alarm.SetParent(self)
		alarm.SetPosition(219, 61)
		alarm.SetEvent(ui.__mem_func__(self.ActivateWhisperAlert))
		alarm.SetToolTipText("Bekleme s\xfcresi bitince haber ver", 0, -23)
		alarm.Show()
		self.alarmButton = alarm
		self.RefreshWhisperToggle()

		# skills (UIScript/skillupgrade.py: x 19, y 86, 37 px apart)
		for i in xrange(6):
			self.__Image(self, SKILL_PATH.format("/slot_other.png"), 17 + 37 * i, 86)
		slot = ui.SlotWindow()
		slot.SetParent(self)
		slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		slot.SetPosition(19, 88)
		slot.SetSize(37 * 6, 32)
		for i in xrange(6):
			slot.AppendSlot(i, 37 * i, 0, 32, 32)
		slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectSkill))
		slot.SetOverInItemEvent(ui.__mem_func__(self.OverInSkill))
		slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		slot.Show()
		self.skillSlot = slot

		# book + progress
		self.detail = []
		self.detail.append(self.__Image(self, SKILL_PATH.format("/slot_other.png"), 14, 124))
		book = ui.SlotWindow()
		book.SetParent(self)
		book.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		book.SetPosition(16, 126)
		book.SetSize(32, 32)
		book.AppendSlot(0, 0, 0, 32, 32)
		book.SetOverInItemEvent(ui.__mem_func__(self.OverInBookSlot))
		book.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		book.SetUnselectItemSlotEvent(ui.__mem_func__(self.SelectRightBookSlot))
		book.Show()
		self.bookSlot = book
		self.bookText = self.__Text(self, "", 56, 127, False)
		self.detail += [book, self.bookText, self.__Image(self, SKILL_PATH.format("/jaugebar.png"), 54, 143)]
		gauge = ui.ExpandedImageBox()
		gauge.SetParent(self)
		gauge.LoadImage(SKILL_PATH.format("/jaugebar_full.png"))
		gauge.SetPosition(56, 145)
		gauge.AddFlag("not_pick")
		gauge.Show()
		self.keep.append(gauge)
		self.gauge = gauge
		self.cooldownText = self.__Text(self, "", 142, 143)

		self.chanceText = self.__Text(self, "", WIDTH / 2, 167)
		self.detail += [gauge, self.cooldownText, self.chanceText]

		# Exorcism Scroll / Hermit's Advice / Zen Bean
		self.boostCheck = []
		self.boostCount = []
		for i in xrange(3):
			self.detail.append(self.__Image(self, SKILL_PATH.format("/slot_item.png"), 67 + 45 * i, 196))
		boost = ui.SlotWindow()
		boost.SetParent(self)
		boost.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		boost.SetPosition(67, 196)
		boost.SetSize(45 * 2 + 32, 32)
		for i in xrange(3):
			boost.AppendSlot(i, 45 * i, 0, 32, 32)
		boost.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		boost.SetOverInItemEvent(ui.__mem_func__(self.OverInItemSlot))
		boost.SetOverOutItemEvent(ui.__mem_func__(self.OverOut))
		boost.Show()
		self.boostSlot = boost
		self.detail.append(boost)
		# item count (also 1, nothing when there is none)
		for i in xrange(3):
			line = ui.TextLine()
			line.SetParent(self)
			line.SetPosition(67 + 45 * i + 31, 196 + 20)
			line.SetHorizontalAlignRight()
			line.SetOutline()
			line.AddFlag("not_pick")
			line.Show()
			self.keep.append(line)
			self.boostCount.append(line)
			self.detail.append(line)
		for i in xrange(3):
			chk = ui.Button()
			chk.SetParent(self)
			chk.SetPosition(62 + 45 * i, 191)
			chk.SetEvent(ui.__mem_func__(self.SetUseItem), i)
			chk.Show()
			self.boostCheck.append(chk)
			self.detail.append(chk)

		btn = ui.Button()
		btn.SetParent(self)
		btn.SetPosition((WIDTH - 101) / 2, 234)
		btn.SetUpVisual(COMMON_PATH.format("/button_large_normal.png"))
		btn.SetOverVisual(COMMON_PATH.format("/button_large_hover.png"))
		btn.SetDownVisual(COMMON_PATH.format("/button_large_down.png"))
		btn.SetText("Ba\xfelang\xfd\xe7")
		btn.SetToolTipText("Ctrl + T\xfdkla: animasyonsuz x10", 0, -23)
		btn.SetEvent(ui.__mem_func__(self.StartUpgrade))
		btn.Show()
		self.startButton = btn
		self.detail.append(btn)

		self.successEffect = self.__Effect("effect_success")
		self.failedEffect = self.__Effect("effect_failed")

		self.SetPage(UPGRADE_STANDARD_SKILL)

	# ------------------------------------------------------------------ open / close
	def Open(self, upgradeType = None, skillIndex = 0):
		if upgradeType is not None:
			self.SetPage(upgradeType)
		else:
			self.RefreshWindow()
		if not self.IsShow():
			self.__AttachToCharacterWindow()
		if skillIndex:
			for i, data in enumerate(self.skills):
				if data[0] == skillIndex:
					self.SelectSkill(i)
					break
		self.Show()
		self.SetTop()

	def __AttachToCharacterWindow(self):
		wnd = getattr(self.interface, "wndCharacter", None) if self.interface else None
		if wnd and wnd.IsShow():
			x, y = wnd.GetGlobalPosition()
			self.SetPosition(x + wnd.GetWidth(), y)
		else:
			self.SetCenterPosition()

	def Toggle(self):
		if self.IsShow():
			self.Close()
		else:
			self.Open()

	def Close(self):
		self.CloseQuestionDialog()
		self.OverOut()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	# ------------------------------------------------------------------ pages
	def SetPage(self, page):
		self.upgradeType = page
		for i, btn in enumerate(self.tabs):
			if i == page:
				btn.Down()
			else:
				btn.SetUp()
		self.selected = -1
		self.info = None
		self.RefreshWindow()

	def __CollectSkills(self):
		skills = []
		if self.upgradeType == UPGRADE_STANDARD_SKILL:
			for slot in STANDARD_SKILL_SLOTS:
				skillIndex = player.GetSkillIndex(slot)
				if skillIndex:
					skills.append((skillIndex, player.GetSkillGrade(slot), player.GetSkillLevel(slot), slot))
		elif self.upgradeType == UPGRADE_PASSIF_SKILL:
			for skillIndex in PASSIVE_SKILL_INDEX:
				slot = player.GetSkillSlotIndex(skillIndex)
				if slot:
					skills.append((skillIndex, player.GetSkillGrade(slot), player.GetSkillLevel(slot), slot))
				else:
					skills.append((skillIndex, 0, 0, 0))
		else:
			power = []
			if self.interface:
				power = self.interface.GetBuffiSkillsPower() or []
			for i, skillIndex in enumerate(BUFFI_SKILL_INDEX):
				level = power[i] if i < len(power) else 0
				grade, shown = _BuffiGrade(level)
				skills.append((skillIndex, grade, shown, i))
		return skills

	def RefreshWindow(self):
		self.skills = self.__CollectSkills()
		slot = self.skillSlot
		for i in xrange(6):
			slot.ClearSlot(i)
			slot.DeactivateSlot(i)
		for i, (skillIndex, grade, level, srcSlot) in enumerate(self.skills[:6]):
			slot.SetSkillSlotNew(i, skillIndex, grade, max(1, level))
			slot.SetSlotCountNew(i, grade, level)
			if i == self.selected:
				slot.ActivateSlot(i, 0)
		slot.RefreshSlot()
		for element in self.detail:
			if self.selected >= 0:
				element.Show()
			else:
				element.Hide()
		self.SetChooseSkillText()
		self.SetInformationText()
		self.RefreshItemSlot()

	# ------------------------------------------------------------------ selection
	def SelectSkill(self, index, *args):
		if index >= len(self.skills):
			return
		self.selected = index
		self.info = None
		self.readEnd = 0.0
		self.RefreshWindow()
		self.RequestInfo()

	def RequestInfo(self):
		if 0 <= self.selected < len(self.skills):
			net.SendChatPacket("/upgrade_skill_info %d %d" % (self.upgradeType, self.skills[self.selected][0]))

	def __SelectedSkill(self):
		if 0 <= self.selected < len(self.skills):
			return self.skills[self.selected][0]
		return 0

	def SetChooseSkillText(self):
		skillIndex = self.__SelectedSkill()
		self.RefreshWhisperToggle()
		if not skillIndex:
			self.chooseText.SetText(_T("SKILL_UPGRADE_TEXT", "Bir Beceri Se\xe7in"))
			return
		grade = self.skills[self.selected][1]
		name = skill.GetSkillName(skillIndex, min(grade, 3)) or skill.GetSkillName(skillIndex)
		self.chooseText.SetText("Beceri: |cFFc79e65%s|h|r" % name)

	# server: SkillUpgradeInfo <type> <skill> <book vnum> <book count> <progress> <need> <remaining delay> <maxed>
	def SetUpgradeInfo(self, upgradeType, skillIndex, bookVnum, count, progress, need, remain, maxed):
		if upgradeType != self.upgradeType or skillIndex != self.__SelectedSkill():
			return
		self.info = (bookVnum, count, progress, need, maxed)
		self.readEnd = app.GetTime() + remain
		self.alertKey = (upgradeType, skillIndex)
		if self.alertKey in g_whisperAlert and remain > 0:
			g_whisperAlert[self.alertKey] = (self.readEnd, g_whisperAlert[self.alertKey][1])
		self.RefreshWindow()

	def SetInformationText(self):
		self.bookSlot.ClearSlot(0)
		if not self.info:
			self.bookText.SetText("")
			self.gauge.SetPercentage(0, 1)
			self.bookSlot.RefreshSlot()
			return
		bookVnum, count, progress, need, maxed = self.info
		if maxed:
			self.bookText.SetText(_T("SKILL_UPGRADE_ALREADY_MAX", "Bu beceri zaten maksimum seviyede."))
			self.gauge.SetPercentage(1, 1)
		elif bookVnum:
			self.bookSlot.SetItemSlot(0, bookVnum, count)
			item.SelectItem(bookVnum)
			self.bookText.SetText("%s |cFFc79e65[%d / %d]|h|r" % (item.GetItemName(), progress, need))
			self.gauge.SetPercentage(progress, max(1, need))
		else:
			self.bookText.SetText("Bu beceri hen\xfcz kitapla geli\xfetirilemez.")
			self.gauge.SetPercentage(0, 1)
		self.bookSlot.RefreshSlot()
		self.__RefreshChanceText()

	def __RefreshChanceText(self):
		masterBook = self.info and self.info[0] not in (0, 50513) and self.info[2] < self.info[3]
		if masterBook and g_boostState[1] and player.GetItemCountByVnum(CHANCE_ITEM_VNUM) > 0 and self.upgradeType != UPGRADE_PASSIF_SKILL:
			self.chanceText.SetText("Okuma ba\xfear\xfds\xfdz olamaz")
		else:
			self.chanceText.SetText(_T("READING_MAY_FAIL", "Okuma ba\xfear\xfds\xfdz olabilir"))

	def RefreshItemSlot(self):
		for i, vnum in enumerate(BOOST_ITEMS):
			count = player.GetItemCountByVnum(vnum)
			self.boostSlot.SetItemSlot(i, vnum, 0)
			self.boostCount[i].SetText(str(count) if count > 0 else "")
			btn = self.boostCheck[i]
			img = SKILL_PATH.format("/btn_yes.png" if g_boostState[i] else "/btn_no.png")
			btn.SetUpVisual(img)
			btn.SetOverVisual(img)
			btn.SetDownVisual(img)
		self.boostSlot.RefreshSlot()
		self.__RefreshChanceText()

	def Refresh(self):
		# inventory / skill / Buffi changed
		if self.IsShow():
			self.RefreshWindow()
			self.RequestInfo()

	def SetUseItem(self, index):
		g_boostState[index] = not g_boostState[index]
		self.RefreshItemSlot()

	def SelectItemSlot(self, index, *args):
		self.SetUseItem(index)

	def SelectRightBookSlot(self, index, *args):
		# Buffi Mastery Book / Stone: one whole grade at once
		skillIndex = self.__SelectedSkill()
		if self.upgradeType != UPGRADE_BUFFI_SKILL or not skillIndex or not self.info or self.info[4]:
			return
		vnum = BUFFI_MASTER_ITEMS[0 if self.info[0] != 50513 else 1]
		if player.GetItemCountByVnum(vnum) <= 0:
			item.SelectItem(vnum)
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Gerekli beceri kitab\xfdna sahip de\xf0ilsiniz.")
			return
		self.__DoSendRequest(1, False, True)

	# ------------------------------------------------------------------ tooltips
	def OverInSkill(self, index, *args):
		if index >= len(self.skills):
			return
		skillIndex, grade, level, srcSlot = self.skills[index]
		if self.upgradeType == UPGRADE_BUFFI_SKILL:
			wnd = getattr(self.interface, "wndCompanion", None) if self.interface else None
			if wnd:
				wnd.ShowBuffiSkillToolTip(srcSlot)
			return
		toolTipSkill = getattr(self.interface, "tooltipSkill", None) if self.interface else None
		if toolTipSkill and srcSlot:
			toolTipSkill.SetSkillNew(srcSlot, skillIndex, grade, player.GetSkillLevel(srcSlot))
			return
		name = skill.GetSkillName(skillIndex, min(grade, 3)) or skill.GetSkillName(skillIndex)
		self.toolTip.ClearToolTip()
		self.toolTip.AppendTextLine(name, COLOR_GOLD)
		self.toolTip.ShowToolTip()

	def OverInBookSlot(self, index, *args):
		if self.info and self.info[0]:
			self.toolTipItem.SetItemToolTip(self.info[0])
			if self.upgradeType == UPGRADE_BUFFI_SKILL:
				self.toolTipItem.AppendTextLine("Sa\xf0 t\xfdk: Ustal\xfdk kitab\xfd / ta\xfe\xfd kullan", COLOR_GOLD)

	def OverInItemSlot(self, index, *args):
		if index < len(BOOST_ITEMS):
			self.toolTipItem.SetItemToolTip(BOOST_ITEMS[index])

	def OverOut(self, *args):
		if self.interface:
			if getattr(self.interface, "tooltipSkill", None):
				self.interface.tooltipSkill.HideToolTip()
			if getattr(self.interface, "wndCompanion", None):
				self.interface.wndCompanion.HideBuffiSkillToolTip()
		if self.toolTip:
			self.toolTip.HideToolTip()
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	# ------------------------------------------------------------------ whisper alert
	def RefreshWhisperToggle(self):
		skillIndex = self.__SelectedSkill()
		state = "on" if skillIndex and (self.upgradeType, skillIndex) in g_whisperAlert else "off"
		self.alarmButton.SetUpVisual(SKILL_PATH.format("/btn_alarm_%s.png" % state))
		self.alarmButton.SetOverVisual(SKILL_PATH.format("/btn_alarm_%s1.png" % state))
		self.alarmButton.SetDownVisual(SKILL_PATH.format("/btn_alarm_%s1.png" % state))

	def ActivateWhisperAlert(self):
		# Toggle whisper alert for the currently selected skill, return new state as bool.
		skillIndex = self.__SelectedSkill()
		if not skillIndex:
			chat.AppendChat(chat.CHAT_TYPE_INFO, _T("UPGRADE_SKILL_CHOOSE_SKILL", "Y\xfckseltmek i\xe7in bir beceri se\xe7melisiniz."))
			return False
		key = (self.upgradeType, skillIndex)
		name = skill.GetSkillName(skillIndex)
		if key in g_whisperAlert:
			del g_whisperAlert[key]
			chat.AppendChat(chat.CHAT_TYPE_INFO, "%s becerisi i\xe7in hat\xfdrlatma kapat\xfdld\xfd." % name)
		else:
			active = self.alertKey == key and self.readEnd > app.GetTime()
			g_whisperAlert[key] = (self.readEnd if active else 0.0, name)
			chat.AppendChat(chat.CHAT_TYPE_INFO, "%s becerisinin bekleme s\xfcresi bitince haber verilecek." % name)
		self.RefreshWhisperToggle()
		return key in g_whisperAlert

	def CheckTime(self):
		left = int(self.readEnd - app.GetTime()) if self.info else 0
		if left > 0:
			self.cooldownText.SetText("Bekleme s\xfcresi: %02d:%02d:%02d" % (left / 3600, (left % 3600) / 60, left % 60))
			self.cooldownText.SetPackedFontColor(COLOR_RED)
		else:
			self.cooldownText.SetText("")

	def OnUpdate(self):
		self.CheckTime()
		# inventory changes (scrolls / books used or picked up)
		if self.selected >= 0 and app.GetTime() > getattr(self, "itemRefreshTime", 0.0):
			self.itemRefreshTime = app.GetTime() + 1.0
			self.RefreshItemSlot()
		if self.effectEnd and app.GetTime() > self.effectEnd:
			self.effectEnd = 0.0
			self.successEffect.Hide()
			self.failedEffect.Hide()

	# ------------------------------------------------------------------ upgrade
	def CheckCanStart(self):
		if not self.__SelectedSkill():
			chat.AppendChat(chat.CHAT_TYPE_INFO, _T("UPGRADE_SKILL_CHOOSE_SKILL", "Y\xfckseltmek i\xe7in bir beceri se\xe7melisiniz."))
			return False
		if self.info and self.info[4]:
			chat.AppendChat(chat.CHAT_TYPE_INFO, _T("SKILL_UPGRADE_ALREADY_MAX", "Bu beceri zaten maksimum seviyede."))
			return False
		return True

	def StartUpgrade(self):
		if not self.CheckCanStart():
			return
		count = QUICK_COUNT if app.IsPressed(app.DIK_LCONTROL) else 1
		self.__DoSendRequest(count, False, False)

	def __DoSendRequest(self, count, allowNegative, master):
		skillIndex = self.__SelectedSkill()
		self.lastRequest = (count, master)
		net.SendChatPacket("/upgrade_skill %d %d %d %d %d %d %d %d" % (self.upgradeType, skillIndex, min(count, MAX_COUNT),
			g_boostState[0], g_boostState[1], g_boostState[2], allowNegative, master))

	# server: soul stone would take the rank below zero
	def ShowNegativeGradeConfirmDialog(self):
		import uiCommon
		self.CloseQuestionDialog()
		dlg = uiCommon.QuestionDialog2()
		dlg.SetText1(_T("SKILL_UPGRADE_SOUL_STONE_NEGATIVE_GRADE_CONFIRM_1", "Bu Ruh Ta\xfe\xfd'n\xfd okumak, beceri kademeni s\xfdf\xfdr\xfdn alt\xfdna d\xfc\xfe\xfcrecek."))
		dlg.SetText2(_T("SKILL_UPGRADE_SOUL_STONE_NEGATIVE_GRADE_CONFIRM_2", "Devam etmek istiyor musun?"))
		dlg.SetAcceptEvent(ui.__mem_func__(self.__OnAcceptNegativeGrade))
		dlg.SetCancelEvent(ui.__mem_func__(self.CloseQuestionDialog))
		dlg.Open()
		self.questionDialog = dlg

	def __OnAcceptNegativeGrade(self):
		self.CloseQuestionDialog()
		count, master = getattr(self, "lastRequest", (1, False))
		self.__DoSendRequest(count, True, master)

	def CloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

	# server: SkillUpgradeResult <1 improved | 0 failed>
	def ShowResult(self, result):
		self.successEffect.Hide()
		self.failedEffect.Hide()
		effect = self.successEffect if result else self.failedEffect
		if hasattr(effect, "ResetFrame"):
			effect.ResetFrame()
		effect.Show()
		self.effectEnd = app.GetTime() + 1.0
		self.RefreshWindow()
