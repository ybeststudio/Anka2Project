if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import chat
import localeInfo
import ime
import uiScriptLocale

if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
	import messenger 
	import uiGameOption

if app.ENABLE_EMOTICONS_SYSTEM:
	import uiEmoticons

class WhisperButton(ui.Button):
	def __init__(self):
		ui.Button.__init__(self, "TOP_MOST")

	def __del__(self):
		ui.Button.__del__(self)

	def SetToolTipText(self, text, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.Show()

	def SetToolTipTextWithColor(self, text, color, x=0, y = 32):
		ui.Button.SetToolTipText(self, text, x, y)
		self.ToolTipText.SetPackedFontColor(color)
		self.ToolTipText.Show()

	def ShowToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

	def HideToolTip(self):
		if 0 != self.ToolTipText:
			self.ToolTipText.Show()

class WhisperDialog(ui.ScriptWindow):

	if app.ENABLE_EMOTICONS_SYSTEM:
		NEW_WIDTH = 0
		NEW_HEIGHT = 0

	class TextRenderer(ui.Window):
		def SetTargetName(self, targetName):
			self.targetName = targetName

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			chat.RenderWhisper(self.targetName, x, y)

	class ResizeButton(ui.DragButton):

		def __init__(self):
			ui.DragButton.__init__(self)

		def __del__(self):
			ui.DragButton.__del__(self)

		def OnMouseOverIn(self):
			app.SetCursor(app.HVSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	def __init__(self, eventMinimize, eventClose):
		print "NEW WHISPER DIALOG  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.targetName = ""
		self.eventMinimize = eventMinimize
		self.eventClose = eventClose
		self.eventAcceptTarget = None

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryID = 0

	def __del__(self):
		print "---------------------------------------------------------------------------- DELETE WHISPER DIALOG"
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/WhisperDialog.py")
		except:
			import exception
			exception.Abort("WhisperDialog.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.titleName = GetObject("titlename")
			self.titleNameEdit = GetObject("titlename_edit")
			self.closeButton = GetObject("closebutton")
			self.scrollBar = GetObject("scrollbar")
			self.chatLine = GetObject("chatline")
			self.minimizeButton = GetObject("minimizebutton")
			self.ignoreButton = GetObject("ignorebutton")
			self.reportViolentWhisperButton = GetObject("reportviolentwhisperbutton")
			self.acceptButton = GetObject("acceptbutton")
			self.sendButton = GetObject("sendbutton")
			self.board = GetObject("board")
			self.editBar = GetObject("editbar")
			self.gamemasterMark = GetObject("gamemastermark")

			if app.ENABLE_EMOTICONS_SYSTEM:
				emoticonsBoard = uiEmoticons.EmoticonsBoardWhisper(self.chatLine)
				emoticonsBoard.SetParent(self.board)
				self.emoticonsBoard = emoticonsBoard

		except:
			import exception
			exception.Abort("DialogWindow.LoadDialog.BindObject")

		self.gamemasterMark.Hide()
		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.minimizeButton.SetEvent(ui.__mem_func__(self.Minimize))
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetPos(1.0)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.chatLine.SetReturnEvent(ui.__mem_func__(self.SendWhisper))
		self.chatLine.SetEscapeEvent(ui.__mem_func__(self.Minimize))
		self.chatLine.SetMultiLine()
		self.sendButton.SetEvent(ui.__mem_func__(self.SendWhisper))
		self.titleNameEdit.SetReturnEvent(ui.__mem_func__(self.AcceptTarget))
		self.titleNameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.ignoreButton.SetToggleDownEvent(ui.__mem_func__(self.IgnoreTarget))
		self.ignoreButton.SetToggleUpEvent(ui.__mem_func__(self.IgnoreTarget))
		self.reportViolentWhisperButton.SetEvent(ui.__mem_func__(self.ReportViolentWhisper))
		self.acceptButton.SetEvent(ui.__mem_func__(self.AcceptTarget))

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.countryFlag = ui.ExpandedImageBox()
			self.countryFlag.SetParent(self.board)
			self.countryFlag.SetPosition(130, 28)
			self.countryFlag.Show()

		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.teleportButton = ui.Button()
			self.teleportButton.SetParent(self)
			self.teleportButton.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
			self.teleportButton.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
			self.teleportButton.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")
			self.teleportButton.SetText(uiScriptLocale.OPTION_TELEPORT)
			self.teleportButton.SetPosition(145, 9)
			self.teleportButton.SAFE_SetEvent(self.TeleportToFriend)
			self.teleportButton.Hide()

		if app.ENABLE_EMOTICONS_SYSTEM:
			self.btnEmoticonsOpen = ui.Button()
			self.btnEmoticonsOpen.SetParent(self)
			self.btnEmoticonsOpen.SetUpVisual("d:/ymir work/ui/game/emoji/emoticons/gui/open_norm.png")
			self.btnEmoticonsOpen.SetOverVisual("d:/ymir work/ui/game/emoji/emoticons/gui/open_over.png")
			self.btnEmoticonsOpen.SetDownVisual("d:/ymir work/ui/game/emoji/emoticons/gui/open_down.png")
			self.btnEmoticonsOpen.SetButtonScale(0.26, 0.26)
			self.btnEmoticonsOpen.SetPosition(210, 11)
			self.btnEmoticonsOpen.SAFE_SetEvent(self.OpenEmoticons)
			self.btnEmoticonsOpen.Hide()

			toolTipEx = ui.Ballon()
			toolTipEx.SetParent(self.btnEmoticonsOpen)
			toolTipEx.SetPosition(0, -38)
			toolTipEx.SetWindowHorizontalAlignCenter()
			toolTipEx.Hide()
			toolTipEx.SetText(localeInfo.TOOLTIP_EMOJI_OPEN)
			self.toolTipEx = toolTipEx
			self.btnEmoticonsOpen.SetToolTipWindow(self.toolTipEx)

			self.btnEmoticonsClose = ui.Button()
			self.btnEmoticonsClose.SetParent(self)
			self.btnEmoticonsClose.SetUpVisual("d:/ymir work/ui/game/emoji/emoticons/gui/close_norm.png")
			self.btnEmoticonsClose.SetOverVisual("d:/ymir work/ui/game/emoji/emoticons/gui/close_over.png")
			self.btnEmoticonsClose.SetDownVisual("d:/ymir work/ui/game/emoji/emoticons/gui/close_down.png")
			self.btnEmoticonsClose.SetButtonScale(0.26, 0.26)
			self.btnEmoticonsClose.SetPosition(210, 11)
			self.btnEmoticonsClose.SAFE_SetEvent(self.CloseEmoticons)
			self.btnEmoticonsClose.Hide()

			toolTipEr = ui.Ballon()
			toolTipEr.SetParent(self.btnEmoticonsClose)
			toolTipEr.SetPosition(0, -38)
			toolTipEr.SetWindowHorizontalAlignCenter()
			toolTipEr.Hide()
			toolTipEr.SetText(localeInfo.TOOLTIP_EMOJI_CLOSE)
			self.toolTipEr = toolTipEr
			self.btnEmoticonsClose.SetToolTipWindow(self.toolTipEr)

		self.textRenderer = self.TextRenderer()
		self.textRenderer.SetParent(self)
		self.textRenderer.SetPosition(20, 28)
		self.textRenderer.SetTargetName("")
		self.textRenderer.Show()

		self.resizeButton = self.ResizeButton()
		self.resizeButton.SetParent(self)
		self.resizeButton.SetSize(20, 20)
		if app.ENABLE_EMOTICONS_SYSTEM:
			self.resizeButton.SetPosition(280 + 110, 180 + 30)
		else:
			self.resizeButton.SetPosition(280, 180)
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.ResizeWhisperDialog))
		self.resizeButton.Show()

		self.ResizeWhisperDialog()

	def Destroy(self):

		self.eventMinimize = None
		self.eventClose = None
		self.eventAcceptTarget = None

		self.ClearDictionary()
		self.scrollBar.Destroy()
		self.titleName = None
		self.titleNameEdit = None
		self.closeButton = None
		self.scrollBar = None
		self.chatLine = None
		self.sendButton = None
		self.ignoreButton = None
		self.reportViolentWhisperButton = None
		self.acceptButton = None
		self.minimizeButton = None
		self.textRenderer = None
		self.board = None
		self.editBar = None
		self.resizeButton = None

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryFlag = None
			self.countryID = 0

		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.teleportButton = None

		if app.ENABLE_EMOTICONS_SYSTEM:
			self.emoticonsBoard.Destroy()
			self.emoticonsBoard = None
			self.btnEmoticonsOpen = None
			self.btnEmoticonsClose = None
			self.size_count = 0

	if app.ENABLE_EMOTICONS_SYSTEM:
		def OpenEmoticons(self):
			self.NEW_WIDTH = 0
			self.NEW_HEIGHT = 240

			(xPos, yPos) = self.resizeButton.GetLocalPosition()
			self.SetWhisperDialogSize(xPos + 20, yPos + self.NEW_HEIGHT + 20)
			self.resizeButton.SetPosition(xPos, yPos + self.NEW_HEIGHT)

			self.btnEmoticonsOpen.Hide()
			self.btnEmoticonsClose.Show()
			self.emoticonsBoard.Open(self.board)

		def CloseEmoticons(self):
			self.NEW_WIDTH = 0
			self.NEW_HEIGHT = 0

			(xPos, yPos) = self.resizeButton.GetLocalPosition()
			self.SetWhisperDialogSize(xPos + 20, yPos - 240 + 20)
			self.resizeButton.SetPosition(xPos, yPos - 240)

			self.btnEmoticonsOpen.Show()
			self.btnEmoticonsClose.Hide()
			self.emoticonsBoard.Close()

	def ResizeWhisperDialog(self):
		(xPos, yPos) = self.resizeButton.GetLocalPosition()

		if app.ENABLE_EMOTICONS_SYSTEM:
			if xPos < 390 + self.NEW_WIDTH:
				self.resizeButton.SetPosition(390 + self.NEW_WIDTH, yPos)
				return

			if yPos < 180 + 30 + self.NEW_HEIGHT:
				self.resizeButton.SetPosition(xPos, 180 + 30 + self.NEW_HEIGHT)
				return
		else:
			if xPos < 280:
				self.resizeButton.SetPosition(280, yPos)
				return

			if yPos < 150:
				self.resizeButton.SetPosition(xPos, 150)
				return

		self.SetWhisperDialogSize(xPos + 20, yPos + 20)

		if app.ENABLE_EMOTICONS_SYSTEM:
			self.emoticonsBoard.Refresh(self.board)

	def SetWhisperDialogSize(self, width, height):
		try:
			if app.ENABLE_EMOTICONS_SYSTEM:
				max = 1000
				self.size_count = width - 100
			else:
				max = int((width-90)/6) * 3 - 6

			self.board.SetSize(width, height)
			self.scrollBar.SetPosition(width-25, 35)

			if app.ENABLE_EMOTICONS_SYSTEM:
				self.scrollBar.SetScrollBarSize(height - 110 - self.NEW_HEIGHT)
			else:
				self.scrollBar.SetScrollBarSize(height-100)

			self.scrollBar.SetPos(1.0)
			self.editBar.SetSize(width-18, 50)

			if app.ENABLE_EMOTICONS_SYSTEM:
				self.chatLine.SetSize(width - 98, 40)
				self.chatLine.SetLimitWidth(width - 98)
			else:
				self.chatLine.SetSize(width-90, 40)
				self.chatLine.SetLimitWidth(width-90)

			self.SetSize(width, height)

			if app.ENABLE_EMOTICONS_SYSTEM:
				if 0 !=self.targetName:
					chat.SetWhisperBoxSize(self.targetName, width - 50, height - 90 - self.NEW_HEIGHT)
			else:
				if 0 != self.targetName:
					chat.SetWhisperBoxSize(self.targetName, width - 50, height - 90)

			self.textRenderer.SetPosition(20, 28)

			if app.ENABLE_EMOTICONS_SYSTEM:
				self.scrollBar.SetPosition(width - 25, 35 + 7)
				self.editBar.SetPosition(10, height - 60 - self.NEW_HEIGHT)
			else:
				self.scrollBar.SetPosition(width-25, 35)
				self.editBar.SetPosition(10, height-60)

			self.sendButton.SetPosition(width-80, 10)
			self.minimizeButton.SetPosition(width-42, 12)
			self.closeButton.SetPosition(width-24, 12)

			self.SetChatLineMax(max)

			if app.ENABLE_EMOTICONS_SYSTEM:
				self.emoticonsBoard.SetSizeCount(self.size_count)

		except:
			import exception
			exception.Abort("WhisperDialog.SetWhisperDialogSize.BindObject")

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
		def RequestCountryFlag(self, targetName):
			if targetName:
				net.SendWhisperDetails(targetName)

			if self.countryFlag:
				fixedWidth = 280
				if messenger.IsFriendByName(targetName):
					self.countryFlag.SetPosition(fixedWidth - 159, 13.5)
				else:
					self.countryFlag.SetPosition(fixedWidth - 159, 13.5)

		def SetCountryFlag(self, countryID):
			self.countryID = countryID

			import advanced_options_runtime
			if countryID != 0 and advanced_options_runtime.Get("show_country_flag", True):
				self.countryFlag.LoadImage("d:/ymir work/ui/game/emoji/flag/%s.tga" % uiGameOption.LOCALE_LANG_DICT[countryID]["locale"])
				self.countryFlag.Show()
			else:
				self.countryFlag.Hide()

	def SetChatLineMax(self, max):
		self.chatLine.SetMax(max)

		if app.ENABLE_EMOTICONS_SYSTEM:
			import grpText
			text = self.chatLine.GetText()

			if text:
				lineCount = grpText.GetSplitingTextLineCount(text, 310 * 3)
		else:
			from grpText import GetSplitingTextLine

			text = self.chatLine.GetText()
			if text:
				self.chatLine.SetText(GetSplitingTextLine(text, max, 0))

	def ApplyOptionSize(self, small):
		if not hasattr(self, "_optionOriginalSize"):
			self._optionOriginalSize = (self.GetWidth(), self.GetHeight())
		width, height = (300, 210) if small else self._optionOriginalSize
		self.resizeButton.SetPosition(width-20, height-20)
		self.SetWhisperDialogSize(width, height)

	def OpenWithTarget(self, targetName):
		import advanced_options_runtime
		self.ApplyOptionSize(advanced_options_runtime.Get("small_whisper_window", False))
		chat.CreateWhisper(targetName)
		chat.SetWhisperBoxSize(targetName, self.GetWidth() - 60, self.GetHeight() - 90)
		self.chatLine.SetFocus()
		self.titleName.SetText(targetName)
		self.targetName = targetName
		self.textRenderer.SetTargetName(targetName)
		self.titleNameEdit.Hide()
		self.ignoreButton.Hide()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.RequestCountryFlag(targetName)
			self.countryFlag.Hide()

		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.teleportButton.Show()

		self.reportViolentWhisperButton.Hide()

		self.acceptButton.Hide()
		self.gamemasterMark.Hide()
		self.minimizeButton.Show()

		if app.ENABLE_EMOTICONS_SYSTEM:
			self.btnEmoticonsOpen.Show()
			self.btnEmoticonsClose.Hide()
			self.ResizeWhisperDialog()

	def OpenWithoutTarget(self, event):
		self.eventAcceptTarget = event
		self.titleName.SetText("")
		self.titleNameEdit.SetText("")
		self.titleNameEdit.SetFocus()
		self.targetName = 0
		self.titleNameEdit.Show()
		self.ignoreButton.Hide()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryFlag.Hide()

		if app.ENABLE_TELEPORT_TO_A_FRIEND:
			self.teleportButton.Hide()

		self.reportViolentWhisperButton.Hide()
		self.acceptButton.Show()
		self.minimizeButton.Hide()
		self.gamemasterMark.Hide()

	def SetGameMasterLook(self):
		self.gamemasterMark.Show()
		self.reportViolentWhisperButton.Hide()

	def Minimize(self):
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventMinimize:
			self.eventMinimize(self.targetName)

	if app.ENABLE_TELEPORT_TO_A_FRIEND:
		def TeleportToFriend(self):
			net.SendMessengerRequestWarpByNamePacket(self.targetName)

	def Close(self):
		chat.ClearWhisper(self.targetName)
		self.titleNameEdit.KillFocus()
		self.chatLine.KillFocus()
		self.Hide()

		if None != self.eventClose:
			self.eventClose(self.targetName)

	def ReportViolentWhisper(self):
		net.SendChatPacket("/reportviolentwhisper " + self.targetName)

	def IgnoreTarget(self):
		net.SendChatPacket("/ignore " + self.targetName)

	def AcceptTarget(self):
		name = self.titleNameEdit.GetText()
		if len(name) <= 0:
			self.Close()
			return

		if None != self.eventAcceptTarget:
			self.titleNameEdit.KillFocus()
			self.eventAcceptTarget(name)

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
				self.RequestCountryFlag(name)

	def OnScroll(self):
		chat.SetWhisperPosition(self.targetName, self.scrollBar.GetPos())

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar.IsShow():
			if nLen > 0:
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()

	if app.ENABLE_EMOTICONS_SYSTEM:
		def OnUpdate(self):
			if self.chatLine.GetTextSize()[0] > self.size_count * 3:
				ime.PasteBackspace()

	def SendWhisper(self):
		text = self.chatLine.GetText()
		if self.targetName == "[Guild]" and text:
			if not net.IsInsultIn(text):
				net.SendChatPacket(text, chat.CHAT_TYPE_GUILD)
				self.chatLine.SetText("")
			return
		textLength = len(text)

		if textLength > 0:
			if app.LINK_IN_CHAT:
				link = self.GetLink(text)
				if link != "":
					if not chr.IsGameMaster():
						text = text.replace(link, "|cFF00C0FC|h|Hweb:" + link.replace("://", "XxX") + "|h" + link + "|h|r")
					else:
						text = text.replace(link, "|cFF00C0FC|h|Hsysweb:" + link.replace("://", "XxX") + "|h" + link + "|h|r")

			if net.IsInsultIn(text):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHAT_INSULT_STRING)
				return

			if app.ENABLE_MULTILANGUAGE_CHATTING:
				net.SendWhisperPacket(self.targetName, text, self.chatLine.GetInputSystemLangID())
			else:
				net.SendWhisperPacket(self.targetName, text)

			if app.ENABLE_EMOTICONS_SYSTEM:
				chat.SaveEmojisChat(text)
				if self.emoticonsBoard:
					self.emoticonsBoard.RefreshCategory(0)

			self.chatLine.SetText("")
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, self.targetName, player.GetName() + " : " + text)

	def OnTop(self):
		self.chatLine.SetFocus()

	def BindInterface(self, interface):
		self.interface = interface

	def OnMouseLeftButtonDown(self):
		hyperlink = ui.GetHyperlink()
		if hyperlink:
			if app.IsPressed(app.DIK_LALT):
				link = chat.GetLinkFromHyperlink(hyperlink)
				ime.PasteString(link)
			else:
				self.interface.MakeHyperlinkTooltip(hyperlink)

	if app.LINK_IN_CHAT:
		def GetLink(self, text):
			link = ""
			start = text.find("http://")
			if start == -1:
				start = text.find("https://")
			if start == -1:
				return ""

			return text[start:len(text)].split(" ")[0]

if "__main__" == __name__:
	import uiTest

	class TestApp(uiTest.App):
		def OnInit(self):
			wnd = WhisperDialog(self.OnMax, self.OnMin)
			wnd.LoadDialog()
			wnd.OpenWithoutTarget(self.OnNew)
			wnd.SetPosition(0, 0)
			wnd.Show()

			self.wnd = wnd

		def OnMax(self):
			pass

		def OnMin(self):
			pass

		def OnNew(self):
			pass

	TestApp().MainLoop()
