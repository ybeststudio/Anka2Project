if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import exception
import localeInfo
import uiScriptLocale
import chat
import uiCommon
import uiToolTip
import wndMgr
import mupgrd

class MountUpGradeWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		mupgrd.SetWindow(self)
		self.Initialize()
		self.LoadWindow()
		
	def __del__(self):
		mupgrd.DestroyWindow()
		ui.ScriptWindow.__del__(self)
		
		del self.toolTipText
		
	def Initialize(self):
		self.cur_level_text = None
		self.next_level_text = None
		self.level_up_arrow = None
		self.exp_gauge_text = None
		self.gem_text = None
		self.yang_text = None
		self.feed_button = None
		self.level_up_button = None
		self.feed_question_dialog = None
		self.level_question_dialog = None
		
		self.toolTipText = uiToolTip.ToolTip()
		self.toolTipText.ClearToolTip()

	def Destroy(self):
		self.Initialize()
		self.ClearDictionary()
		self.Hide()
		
	def Open(self):
		self.SetCenterPosition()
		self.Refresh()
		self.Show()
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/MountUpGradeSystemWindow.py")
		except:
			exception.Abort("MountUpGradeWindow.LoadWindow.LoadScript")
		
		try:
			self.board = self.GetChild("board")
			self.cur_level_text = self.GetChild("cur_level_text")
			self.next_level_text = self.GetChild("next_level_text")
			self.level_up_arrow = self.GetChild("level_up_arrow")
		
			self.exp_gauge_bar = self.GetChild("exp_gauge_bar")
			self.exp_gauge_text = self.GetChild("exp_gauge_text")
		
			self.feed_button = self.GetChild("feed_button")
			self.level_up_button = self.GetChild("level_up_button")
		
			self.yang_icon = self.GetChild("yang_icon")
			self.yang_text = self.GetChild("yang_text")
		
			if self.yang_icon:
				self.yang_icon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", mupgrd.TOOLTIP_YANG)
				self.yang_icon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", mupgrd.TOOLTIP_YANG)
		
			self.gem_icon = self.GetChild("gem_icon")
			self.gem_text = self.GetChild("gem_text")
		
			if self.gem_icon:
				self.gem_icon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", mupgrd.TOOLTIP_GEM)
				self.gem_icon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", mupgrd.TOOLTIP_GEM)
		
		except:
			exception.Abort("MountUpGradeWindow.LoadWindow.BindObject")
		
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.level_up_button.SetEvent(ui.__mem_func__(self.LevelUpButton))
		self.feed_button.SetEvent(ui.__mem_func__(self.FeedButton))
		
	def Chat(self, type, value):
		if type == mupgrd.CHAT_TYPE_BANN_WHILE_MOUNTING:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_BANN_WHILE_MOUNTING)
		
		elif type == mupgrd.CHAT_TYPE_LEVEL_UP_YANG_OR_FEED_NOT_ENOUGH:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_RESULT_YANG_OR_FEED_NOT_ENOUGH)
		
		elif type == mupgrd.CHAT_TYPE_LEVEL_UP_GEM_NOT_ENOUGH:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_RESULT_GEM_NOT_ENOUGH)
		
		elif type == mupgrd.CHAT_TYPE_LEVEL_UP_PERCENT_SUCCESSFUL:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_RESULT_PERCENT_SUCCESS % value)
		
		elif type == mupgrd.CHAT_TYPE_LEVEL_UP_PERCENT_FAIL:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_RESULT_PERCENT_FAIL)
		
		elif type == mupgrd.CHAT_TYPE_EXP_HORSE_FEED:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOUNT_UPGRADE_SYSTEM_EXP_MOUNT_FEED % value)
		
	def LevelUpButton(self):
		# Level
		level = mupgrd.GetHorseLevel()
		
		# Horse Level -> MAX
		if level >= player.HORSE_MAX_LEVEL:
			self.PopupMessage(uiScriptLocale.MOUNT_UPGRADE_SYSTEM_DISABLE_GIVE_FEED_BUTTON_POPUP_MESSAGE)
			return
		
		# Feed Not Enough
		if player.GetItemCountByVnum(mupgrd.HORSE_FEED_ITEM_ID) < mupgrd.HORSE_FEED_LEVEL_COUNT:
			self.PopupMessage(uiScriptLocale.MOUNT_UPGRADE_SYSTEM_GIVE_FEED_NOT_ENOUGH)
			return
		
		# Exp
		curValue, maxValue = mupgrd.GetMountExistingExp(), mupgrd.GetMountNecessaryExp()
		maxExp = curValue >= maxValue
		
		# You did not complete the requirement.
		if curValue < maxValue:
			return
		
		# Price (Yang)
		yangCost = mupgrd.GetMountUpGradePrice()
		
		# Price (Gem)
		gemCost = mupgrd.HORSE_LOWER_LEVEL_RETRY_GEM_COST if level < mupgrd.HORSE_LEVEL_DETERMINES_GEM_COST else mupgrd.HORSE_UPPER_LEVEL_RETRY_GEM_COST
		
		if self.level_question_dialog:
			return
		
		level_question_dialog = uiCommon.MountUpGradeDialog()
		level_question_dialog.SetText1(localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_POPUP_MSG_LEVEL % (level + 1))
		
		# Fail Type
		fType = mupgrd.IsMountUpGradeFail()
		if fType > mupgrd.MOUNT_UP_GRADE_FAIL_OFF and maxExp:
			level_question_dialog.SetText2(localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_POPUP_RETRY_COST % (mupgrd.HORSE_FEED_LEVEL_COUNT, gemCost, yangCost))
		
		elif fType < mupgrd.MOUNT_UP_GRADE_FAIL_ON and maxExp:
			level_question_dialog.SetText2(localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_POPUP_COST % (mupgrd.HORSE_FEED_LEVEL_COUNT, yangCost))
		
		level_question_dialog.SetAcceptEvent(ui.__mem_func__(self.LevelQuestionAnswer))
		level_question_dialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		level_question_dialog.Open()
		self.level_question_dialog = level_question_dialog
		
	def LevelQuestionAnswer(self):
		mupgrd.Send(mupgrd.MOUNT_UP_GRADE_LEVEL_UP)
		self.OnCloseQuestionDialog()
		
	def FeedButton(self):
		# Level
		level = mupgrd.GetHorseLevel()
		
		# Horse Level -> MAX
		if level >= player.HORSE_MAX_LEVEL:
			self.PopupMessage(uiScriptLocale.MOUNT_UPGRADE_SYSTEM_DISABLE_GIVE_FEED_BUTTON_POPUP_MESSAGE)
			return
		
		# Feed Not Enough
		if player.GetItemCountByVnum(mupgrd.HORSE_FEED_ITEM_ID) < mupgrd.HORSE_FEED_EXP_COUNT:
			self.PopupMessage(uiScriptLocale.MOUNT_UPGRADE_SYSTEM_GIVE_FEED_NOT_ENOUGH)
			return
		
		# Exp
		curValue, maxValue = mupgrd.GetMountExistingExp(), mupgrd.GetMountNecessaryExp()
		maxExp = curValue >= maxValue
		
		# If the horse level is not max, "Level can be given"
		if maxExp:
			self.PopupMessage(uiScriptLocale.MOUNT_UPGRADE_SYSTEM_DISABLE_GIVE_FEED_BUTTON_POPUP_MESSAGE)
			return
		
		if self.feed_question_dialog:
			return
		
		feed_question_dialog = uiCommon.QuestionDialog()
		feed_question_dialog.SetText(localeInfo.MOUNT_UPGRADE_SYSTEM_LEVEL_UP_POPUP_MSG_FEED)
		feed_question_dialog.SetAcceptEvent(ui.__mem_func__(self.FeedQuestionAnswer))
		feed_question_dialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		feed_question_dialog.Open()
		self.feed_question_dialog = feed_question_dialog
		
	def FeedQuestionAnswer(self):
		mupgrd.Send(mupgrd.MOUNT_UP_GRADE_EXP)
		self.OnCloseQuestionDialog()
		
	def Refresh(self):
		# Level
		level = mupgrd.GetHorseLevel()
		self.cur_level_text.SetText("{}".format(level))
		
		# Next Level
		if level >= player.HORSE_MAX_LEVEL:
			self.next_level_text.Hide()
		else:
			self.next_level_text.SetText("{}".format(level + 1))
			self.next_level_text.Show()
		
		# Exp
		curValue, maxValue = mupgrd.GetMountExistingExp(), mupgrd.GetMountNecessaryExp()
		maxExp = curValue >= maxValue
		
		# Price (Yang)
		yangCost = mupgrd.GetMountUpGradePrice()
		if level < player.HORSE_MAX_LEVEL and maxExp:
			self.yang_text.Show()
			self.yang_text.SetText("{}".format(localeInfo.NumberToMoneyString(yangCost)))
		else:
			self.yang_text.Hide()
		
		# Price (Gem)
		if mupgrd.IsMountUpGradeFail() > mupgrd.MOUNT_UP_GRADE_FAIL_OFF and maxExp:
			gemCost = mupgrd.HORSE_LOWER_LEVEL_RETRY_GEM_COST if level < mupgrd.HORSE_LEVEL_DETERMINES_GEM_COST else mupgrd.HORSE_UPPER_LEVEL_RETRY_GEM_COST
			self.gem_text.SetText("{}".format(gemCost))
			self.gem_text.Show()
		else:
			self.gem_text.Hide()
		
		# Buttons & Arrow
		level_up = level < player.HORSE_MAX_LEVEL and maxExp
		self.level_up_button.Show() if level_up else self.level_up_button.Hide()
		self.feed_button.Show() if not level_up else self.feed_button.Hide()
		self.level_up_arrow.Show() if level_up else self.level_up_arrow.Hide()
		
		if maxExp:
			curValue = maxValue
		
		# Percentage
		self.exp_gauge_bar.SetPercentage(curValue, maxValue)
		
		self.exp_gauge_text.SetText("MAX" if level >= player.HORSE_MAX_LEVEL else "{}/{}".format(curValue, maxValue))
		
	def OnCloseQuestionDialog(self):
		if self.level_question_dialog:
			self.level_question_dialog.Close()
			self.level_question_dialog = None
		
		if self.feed_question_dialog:
			self.feed_question_dialog.Close()
			self.feed_question_dialog = None
		
	def OverInToolTip(self, arg):
		arglen = len(str(arg))
		pos_x, pos_y = wndMgr.GetMousePosition()
		
		self.toolTipText.ClearToolTip()
		self.toolTipText.SetThinBoardSize(5 * arglen)
		self.toolTipText.SetToolTipPosition(pos_x + 5, pos_y - 10)
		self.toolTipText.AppendTextLine(arg, 0xffffff00)
		self.toolTipText.Show()
		
	def OverOutToolTip(self):
		self.toolTipText.Hide()
		
	def EventProgress(self, event_type, idx):
		if "mouse_over_in" == str(event_type):
			if idx == mupgrd.TOOLTIP_YANG:
				self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_YANG)
			elif idx == mupgrd.TOOLTIP_GEM:
				self.OverInToolTip(localeInfo.GEM_SYSTEM_NAME)
			else:
				return 
		elif "mouse_over_out" == str(event_type):
			self.OverOutToolTip()
		else:
			return
	
	def PopupMessage(self, msg):
		self.wndPopupDialog = uiCommon.PopupDialog()
		self.wndPopupDialog.SetText(msg)
		self.wndPopupDialog.Open()
	
	def Close(self):
		self.OnCloseQuestionDialog()
		self.Hide()
		self.Reset()
		
	def Reset(self):
		mupgrd.Reset()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
