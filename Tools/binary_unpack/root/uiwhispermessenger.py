import app
import ui
import net
import chat
import player
import localeInfo
import uiCommon
import grp
import time
import uiCharacterDetails
import uiEmojiPicker
import guild
import ime
import wndMgr
import userData

# Messenger window (reference Baerim uiWhisperWindow.WhisperWindow / EmbeddedWhisperDialog):
# conversation list on the left, chat bubbles + composer on the right, local history per character,
# "Anka2 Global" channel (server ENABLE_ANKA2_GLOBAL_CHAT: /anka_global, "AnkaGlobalChat" command).

IMG = "d:/ymir work/ui/game/whisper/"
GLOBAL_NAME = "[Anka2 Global]"
GLOBAL_TITLE = localeInfo.WHISPER_MESSENGER_GLOBAL_TITLE
GUILD_NAME = "[Guild]"
GUILD_WHISPER_MARK = " : [[GW]]"	# server guild_donation.cpp: whisper sent to the whole guild, read only conversation
FACE_IMAGES = ("warrior_m", "assassin_w", "sura_m", "shaman_w", "warrior_w", "assassin_m", "sura_w", "shaman_m", "wolfman_m")
EMPIRE_FLAG_PATH = "d:/ymir work/ui/game/guild_application/empire_%d.png"	# reference WHISPER_EMPIRE_FLAG_PATH
INFO_REFRESH_INTERVAL = 20			# reference WHISPER_INFO_REFRESH_INTERVAL
GUILD_TAG_COLOR = 0xff7fd4ff
OFFLINE_NAME_COLOR = 0xff9a9a9a
DATE_LINE_COLOR = 0xff4a4a4a
DATE_TEXT_COLOR = 0xffb0b0b0
EXCLUDED_NAMES = (localeInfo.WIKI_WHISPER_NAME, "[W?K?]")	# wiki window pseudo whisper keeps its own button
PAYLOAD_SEPARATOR = "\x1f"

WIDTH = 637
HEIGHT = 427
LEFT_X = 5
RIGHT_X = 191
PANEL_Y = 28

CONV_ITEM_HEIGHT = 78
CONV_ITEM_STEP = 74		# conv_item_*.png: rows 0-2 and 75-77 are transparent
LIST_TOP = 32
LIST_HEIGHT = 306

CHAT_LEFT = 12
CHAT_TOP = 46
CHAT_WIDTH = 410
CHAT_HEIGHT = 272
BUBBLE_MAX_TEXT_WIDTH = 250
BUBBLE_PADDING = 8
BUBBLE_GAP = 5
LINE_HEIGHT = 15

HISTORY_LIMIT = 200
NAME_COLOR = 0xffffc94d
MINE_NAME_COLOR = 0xff9fd6ff
TEXT_COLOR = 0xffe8e8e8
TIME_COLOR = 0xff9a9a9a
SYSTEM_COLOR = 0xffd0a060
BUBBLE_COLOR = 0xff2b2e33		# reference bubble of the other side
MINE_BUBBLE_COLOR = 0xff20574a	# reference own bubble (teal)
MINE_TIME_COLOR = 0xffa9c9bd
BUBBLE_TAIL = 7				# bubble_tail_*.png width
RULES_TITLE_COLOR = 0xffe0b060
RULES_TEXT_COLOR = 0xffc8c8c8
RULES_BG_COLOR = 0x50000000
DIVIDER_COLOR = 0xb0161616
DIVIDER_BORDER_COLOR = 0xff3a3a3a
DIVIDER_TEXT_COLOR = 0xffd8d8d8
DIVIDER_HEIGHT = 22

GLOBAL_RULES = (
	localeInfo.WHISPER_MESSENGER_RULE_1,
	localeInfo.WHISPER_MESSENGER_RULE_2,
	localeInfo.WHISPER_MESSENGER_RULE_3,
)


def _Now():
	return int(time.time())


def _Clock(ts):
	try:
		return time.strftime("%H:%M", time.localtime(ts))
	except:
		return ""


def _Clean(text):
	return text.replace("\t", " ").replace("\n", " ").replace("\r", "")


class MessageContextMenuItem(ui.Window):
	def __init__(self, label, icon, event):
		ui.Window.__init__(self, "TOP_MOST")
		self.event = event
		self.SetSize(MENU_WIDTH - MENU_PADDING * 2, MENU_ITEM_HEIGHT)
		self.bg = _Bar(self, 0, 0, MENU_WIDTH - MENU_PADDING * 2, MENU_ITEM_HEIGHT, MENU_NORMAL_COLOR)
		self.icon = _MakeImage(self, ICON_DIR + icon, 8, 5)
		self.text = _MakeText(self, label, 34, 6, MENU_TEXT_COLOR)

	def __del__(self):
		ui.Window.__del__(self)

	def OnMouseOverIn(self):
		self.bg.SetColor(MENU_HOVER_COLOR)
		self.text.SetPackedFontColor(MENU_HOVER_TEXT_COLOR)

	def OnMouseOverOut(self):
		self.bg.SetColor(MENU_NORMAL_COLOR)
		self.text.SetPackedFontColor(MENU_TEXT_COLOR)

	def OnMouseLeftButtonDown(self):
		# plain windows never get the button-up of a click they did not capture: run on press
		if self.event:
			self.event()
		return TRUE


class MessageContextMenu(ui.Window):
	"""Right click menu of a chat bubble (reference WhisperMessageContextMenu)."""

	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		self.items = []
		# full screen catcher: a click outside closes the menu. The menu is its child, so it always
		# draws and picks above the cover (as two top windows the cover could end up on top).
		self.cover = ui.Window("TOP_MOST")
		self.cover.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.cover.OnMouseLeftButtonDown = ui.__mem_func__(self.Close)
		self.cover.OnMouseRightButtonDown = ui.__mem_func__(self.Close)
		self.cover.Hide()
		self.SetParent(self.cover)
		self.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def Open(self, entries):
		for item in self.items:
			item.Hide()
		self.items = []
		height = MENU_PADDING * 2 + len(entries) * MENU_ITEM_HEIGHT
		self.SetSize(MENU_WIDTH, height)
		self.border = [_Bar(self, 0, 0, MENU_WIDTH, height, MENU_BORDER_COLOR), _Bar(self, 1, 1, MENU_WIDTH - 2, height - 2, MENU_NORMAL_COLOR)]
		for (index, (label, icon, event)) in enumerate(entries):
			item = MessageContextMenuItem(label, icon, lambda arg = event: self.__Run(arg))
			item.SetParent(self)
			item.SetPosition(MENU_PADDING, MENU_PADDING + index * MENU_ITEM_HEIGHT)
			item.Show()
			self.items.append(item)
		(x, y) = wndMgr.GetMousePosition()
		x = min(x, wndMgr.GetScreenWidth() - MENU_WIDTH)
		y = min(y, wndMgr.GetScreenHeight() - height)
		self.SetPosition(int(x), int(y))
		self.cover.SetPosition(0, 0)
		self.cover.Show()
		self.cover.SetTop()
		self.Show()

	def __Run(self, event):
		self.Close()
		if event:
			event()

	def Close(self):
		self.cover.Hide()
		self.Hide()
		return TRUE

	def Destroy(self):
		self.Close()
		self.items = []


class ReactionPicker(ui.Window):
	"""Row of the reaction emojis (reference OpenReactionSelector), opened from the bubble menu."""
	CELL = 26

	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		self.cover = ui.Window("TOP_MOST")
		self.cover.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.cover.OnMouseLeftButtonDown = ui.__mem_func__(self.Close)
		self.cover.OnMouseRightButtonDown = ui.__mem_func__(self.Close)
		self.cover.Hide()
		self.SetParent(self.cover)
		width = MENU_PADDING * 2 + len(REACTION_EMOJIS) * self.CELL + 4
		height = self.CELL + MENU_PADDING * 2 + 4
		self.SetSize(width, height)
		self.frame = [_Bar(self, 0, 0, width, height, MENU_BORDER_COLOR), _Bar(self, 1, 1, width - 2, height - 2, MENU_NORMAL_COLOR)]
		self.buttons = []
		self.event = None
		for (index, name) in enumerate(REACTION_EMOJIS):
			button = ui.Button()
			button.SetParent(self)
			button.SetSize(self.CELL, self.CELL)
			button.SetPosition(MENU_PADDING + 2 + index * self.CELL, MENU_PADDING + 2)
			image = ui.ExpandedImageBox()
			image.SetParent(button)
			image.LoadImage("icon/emote/%s.tga" % name)
			image.SetScale(1.2, 1.2)
			image.SetPosition(2, 2)
			image.AddFlag("not_pick")
			image.Show()
			button.image = image
			button.SetEvent(lambda arg = name: self.__Pick(arg))
			button.Show()
			self.buttons.append(button)
		self.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def Open(self, event):
		self.event = event
		(x, y) = wndMgr.GetMousePosition()
		self.SetPosition(int(min(x, wndMgr.GetScreenWidth() - self.GetWidth())), int(min(y, wndMgr.GetScreenHeight() - self.GetHeight())))
		self.cover.Show()
		self.cover.SetTop()
		self.Show()

	def __Pick(self, name):
		event = self.event
		self.Close()
		if event:
			event(name)
		return TRUE

	def Close(self):
		self.cover.Hide()
		self.Hide()
		return TRUE


class LinkTextLine(ui.TextLine):
	"""Chat bubble line: pickable, a click on an item / player link opens it (reference whisper bubble links)."""

	def __init__(self):
		ui.TextLine.__init__(self)
		self.clickEvent = None
		self.rightClickEvent = None

	def __del__(self):
		ui.TextLine.__del__(self)

	def OnMouseLeftButtonDown(self):
		if self.clickEvent:
			self.clickEvent()
		return TRUE

	def OnMouseRightButtonDown(self):
		if self.rightClickEvent:
			self.rightClickEvent()
		return TRUE


class TextMeasure(object):
	def __init__(self):
		self.textLine = ui.TextLine()

	def Width(self, text):
		self.textLine.SetText(text)
		return self.textLine.GetTextSize()[0]

	def __Tokens(self, paragraph):
		tokens = []
		for word in paragraph.split(" "):
			if tokens and tokens[-1].count("|h") % 2 == 1:	# inside a hyperlink: glue the words back
				tokens[-1] += " " + word
			else:
				tokens.append(word)
		return tokens

	def Wrap(self, text, maxWidth):
		lines = []
		for paragraph in text.split("\n"):
			line = ""
			for word in self.__Tokens(paragraph):
				candidate = (line + " " + word) if line else word
				if line and self.Width(candidate) > maxWidth:
					lines.append(line)
					line = word
				else:
					line = candidate
			lines.append(line)
		return lines


def _MakeText(parent, text, x, y, color = TEXT_COLOR, font = None):
	textLine = ui.TextLine()
	textLine.SetParent(parent)
	if font:
		textLine.SetFontName(font)
	textLine.SetPackedFontColor(color)
	textLine.SetText(text)
	textLine.SetPosition(int(x), int(y))
	textLine.AddFlag("not_pick")
	textLine.Show()
	return textLine


def _MakeImage(parent, path, x, y):
	image = ui.ImageBox()
	image.SetParent(parent)
	image.LoadImage(path)
	image.SetPosition(int(x), int(y))
	image.AddFlag("not_pick")
	image.Show()
	return image


def _Bar(parent, x, y, w, h, color):
	bar = ui.Bar()
	bar.SetParent(parent)
	bar.SetPosition(int(x), int(y))
	bar.SetSize(int(w), int(h))
	bar.SetColor(color)
	bar.AddFlag("not_pick")
	bar.Show()
	return bar


def _MakeButton(parent, prefix, x, y, text = None):
	button = ui.Button()
	button.SetParent(parent)
	button.SetUpVisual(IMG + prefix + "_norm.png")
	button.SetOverVisual(IMG + prefix + "_hover.png")
	button.SetDownVisual(IMG + prefix + "_select.png")
	button.SetPosition(int(x), int(y))
	if text:
		button.SetText(text)
	button.Show()
	return button


WHISPER_MIN_LEVEL = 30		# reference: private messages need level 30
GLOBAL_COOLDOWN = 60			# server ANKA2_GLOBAL_CHAT_COOLDOWN (s)
CARD_NAME_COLOR = 0xffd7c9b9		# reference conversation card name (beige)
CARD_PREVIEW_COLOR = 0xffc2b5a3
REACTION_PREFIX = "~WRR~"			# whisper carrying a reaction: ~WRR~<emoji>~<excerpt of the reacted message>
REACTION_EMOJIS = ("like", "heart", "grin", "cry", "fire", "cheers")	# reference GUILD_REACTION_TOKENS
REACTION_TAG = "\x02R"				# history row of a reaction (sender column)
REACTION_HEIGHT = 14
REACTION_PILL_COLOR = 0xff1d2024
REACTION_PILL_BORDER = 0xff4a4f57
REPLY_PREFIX = "~WR1~"			# reference whisperStorage reply marker: ~WR1~<author>~<excerpt>~<message>
REPLY_EXCERPT_LEN = 60
REPLY_BAR_COLOR = 0xff4caf6e
REPLY_AUTHOR_COLOR = 0xff8fd19e
REPLY_TEXT_COLOR = 0xffa8a8a8
REPLY_BG_COLOR = 0x60000000
ICON_DIR = "d:/ymir work/ui/game/common/icons/"
# reference WhisperMessageContextMenu / WhisperMessageContextMenuItem
MENU_WIDTH = 180
MENU_ITEM_HEIGHT = 26
MENU_PADDING = 2
MENU_NORMAL_COLOR = 0xff191512
MENU_HOVER_COLOR = 0xff372b24
MENU_BORDER_COLOR = 0xff52392b
MENU_TEXT_COLOR = 0xffd7c9b9
MENU_HOVER_TEXT_COLOR = 0xffffe7c0


def StripMarkup(text):
	"""Plain text of a chat line: colours, hyperlinks and emoji tags removed, link labels kept."""
	out = ""
	i = 0
	while i < len(text):
		if text[i] == "|" and i + 1 < len(text):
			tag = text[i + 1]
			if tag == "c":
				i += 10
				continue
			if tag in ("r", "h"):
				i += 2
				continue
			if tag == "H":
				end = text.find("|h", i + 2)
				i = end + 2 if end >= 0 else len(text)
				continue
			if tag == "E":
				end = text.find("|e", i + 2)
				i = end + 2 if end >= 0 else len(text)
				continue
		out += text[i]
		i += 1
	return out


def SplitReply(text):
	"""(author, excerpt, message) for a reply line, None for a plain one."""
	if not text.startswith(REPLY_PREFIX):
		return None
	parts = text[len(REPLY_PREFIX):].split("~", 2)
	if len(parts) != 3:
		return None
	return parts


def MessageText(text):
	reply = SplitReply(text)
	return reply[2] if reply else text
OFFLINE_MESSAGE_COST = "500.000"	# server WHISPER_OFFLINE_MESSAGE_COST
LABEL_COLOR = 0xffe8e8e8
LABEL_FLASH_COLOR = 0xffffc94d


class UnreadBadge(ui.ImageBox):
	"""Red unread counter on the [MESSENGER] envelope; hidden while there is nothing to read."""

	def __init__(self, parent):
		ui.ImageBox.__init__(self, "TOP_MOST")
		self.SetParent(parent)
		self.LoadImage(IMG + "badge_unread.png")
		self.SetPosition(18, -6)
		self.AddFlag("not_pick")
		self.text = _MakeText(self, "", 8, 1, 0xffffffff)
		self.text.SetHorizontalAlignCenter()
		self.Hide()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetUnread(self, count):
		if count > 0:
			self.text.SetText(str(count) if count < 100 else "99")
			self.Show()
		else:
			self.Hide()


class MessengerButton(ui.Button):
	"""Fixed screen envelope "[MESSENGER]" with the unread badge (reference mail button)."""

	def __init__(self):
		ui.Button.__init__(self)
		self.SetUpVisual(IMG + "mp_close.png")
		self.SetOverVisual(IMG + "mp_open.png")
		self.SetDownVisual(IMG + "mp_open.png")
		self.label = _MakeText(self, localeInfo.WHISPER_MESSENGER_ENVELOPE, 14, 28, LABEL_COLOR)
		self.label.SetHorizontalAlignCenter()
		self.label.SetOutline()
		self.badge = _MakeImage(self, IMG + "badge_unread.png", 18, -6)
		self.badgeText = _MakeText(self.badge, "", 8, 1, 0xffffffff)
		self.badgeText.SetHorizontalAlignCenter()
		self.SetUnread(0)

	def __del__(self):
		ui.Button.__del__(self)

	def SetUnread(self, count):
		if count > 0:
			self.badgeText.SetText(str(count) if count < 100 else "99")
			self.badge.Show()
		else:
			self.badge.Hide()


class SystemWhisperButton(ui.Button):
	"""Envelope of a system sender ([Biolog] ...): opens / closes with a yellow label while it has news."""
	FLASH_INTERVAL = 0.5

	def __init__(self, name):
		ui.Button.__init__(self)
		self.name = name
		self.flashing = FALSE
		self.flashOpen = FALSE
		self.flashTime = 0
		self.label = _MakeText(self, name, 14, 28, LABEL_COLOR)
		self.label.SetHorizontalAlignCenter()
		self.label.SetOutline()
		self.hoverInEvent = None
		self.hoverOutEvent = None
		self.__SetOpen(FALSE)

	def __del__(self):
		ui.Button.__del__(self)

	def SetHoverEvent(self, hoverIn, hoverOut):
		self.hoverInEvent = hoverIn
		self.hoverOutEvent = hoverOut

	def ShowToolTip(self):
		if self.hoverInEvent:
			self.hoverInEvent()

	def HideToolTip(self):
		if self.hoverOutEvent:
			self.hoverOutEvent()

	def __SetOpen(self, isOpen):
		image = IMG + ("mp_open.png" if isOpen else "mp_close.png")
		self.SetUpVisual(image)
		self.SetOverVisual(IMG + "mp_open.png")
		self.SetDownVisual(IMG + "mp_open.png")
		self.label.SetPackedFontColor(LABEL_FLASH_COLOR if isOpen else LABEL_COLOR)

	def Flash(self):
		self.flashing = TRUE

	def OnUpdate(self):
		if not self.flashing:
			return
		now = time.time()
		if now - self.flashTime >= self.FLASH_INTERVAL:
			self.flashTime = now
			self.flashOpen = not self.flashOpen
			self.__SetOpen(self.flashOpen)


class ConversationItem(ui.Button):
	"""One card of the left list (reference WhisperButton, conv_item_*.png)."""

	def __init__(self, messenger, name):
		ui.Button.__init__(self)
		self.messenger = messenger
		self.name = name
		self.isGM = FALSE
		self.selected = FALSE
		self.icon = _MakeImage(self, IMG + "online_player_icon.png", 7, 6)
		self.nameText = _MakeText(self, "", 49, 19, CARD_NAME_COLOR)
		self.timeText = _MakeText(self, "", 166, 19, TIME_COLOR)
		self.timeText.SetHorizontalAlignRight()
		self.previewText = _MakeText(self, "", 12, 50, CARD_PREVIEW_COLOR)
		self.deleteButton = None
		if name != GLOBAL_NAME:
			self.deleteButton = _MakeButton(self, "close_btn", 154, 50)
			self.deleteButton.SetEvent(ui.__mem_func__(self.__OnDelete))
		# muted mark: only shown while the conversation is muted (left of the delete cross)
		self.muteButton = ui.Button()
		self.muteButton.SetParent(self)
		self.muteButton.SetUpVisual(IMG + "bell_off.png")
		self.muteButton.SetOverVisual(IMG + "bell_off.png")
		self.muteButton.SetDownVisual(IMG + "bell_off.png")
		self.muteButton.SetPosition(148 if name == GLOBAL_NAME else 132, 47)
		self.muteButton.SetEvent(ui.__mem_func__(self.__OnToggleMute))
		self.muteButton.Hide()
		self.SetEvent(ui.__mem_func__(self.__OnClick))
		self.__ApplyVisual()

	def __del__(self):
		ui.Button.__del__(self)

	def __ApplyVisual(self):
		prefix = "conv_item_gm_" if self.isGM else "conv_item_"
		self.SetUpVisual(IMG + prefix + ("select.png" if self.selected else "norm.png"))
		self.SetOverVisual(IMG + prefix + ("select.png" if self.selected else "hover.png"))
		self.SetDownVisual(IMG + prefix + "select.png")

	def Refresh(self, title, unread, lastTime, preview, isGM, selected):
		self.isGM = isGM
		self.selected = selected
		self.__ApplyVisual()
		if self.name == GLOBAL_NAME:
			self.icon.LoadImage(IMG + "baerim_global_icon.png")
		elif isGM:
			self.icon.LoadImage(IMG + "gm_icon.png")
		elif self.name == GUILD_NAME or self.messenger.IsGuildSender(self.name):
			self.icon.LoadImage(IMG + "guild_icon.png")
		else:
			self.icon.LoadImage(IMG + "online_player_icon.png")
		self.nameText.SetText(self.messenger.FitText(title, 93))
		self.timeText.SetText(_Clock(lastTime) if lastTime else "")
		self.previewText.SetText(preview)
		if self.messenger.IsMuted(self.name):
			self.muteButton.Show()
		else:
			self.muteButton.Hide()

	def __OnClick(self):
		self.messenger.SelectConversation(self.name)

	def __OnToggleMute(self):
		self.messenger.ToggleMute(self.name)

	def OnMouseRightButtonDown(self):
		self.messenger.OpenConversationMenu(self.name)
		return TRUE

	def OnMouseWheel(self, length):
		return self.messenger.OnMouseWheel(length)

	def __OnDelete(self):
		self.messenger.RequestDeleteConversation(self.name)


class WhisperMessenger(ui.BoardWithTitleBar):
	_oldFilesMoved = False

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.interface = None
		self.measure = TextMeasure()
		self.guildSenders = set()	# guild whisper senders (GUILD_WHISPER_MARK)
		self.conversations = {}		# name -> {"lines": [(sender, text, ts, mine, system)], "unread": n, "last": ts, "gm": bool}
		self.order = []				# names, most recent first
		self.items = {}
		self.current = None
		self.listScroll = 0
		self.bubbles = []
		self.rulesClosed = FALSE
		self.questionDialog = None
		self.inputDialog = None
		self.historyFile = None
		self.emojiPicker = None
		self.unreadDivider = None
		self.mutedConversations = {}	# name -> 1
		self.globalCooldownEnd = 0
		self.replyDraft = {}		# conversation -> (author, excerpt)
		self.reactions = {}		# (conversation, ts, raw text) -> emoji name
		self.reactionPicker = None
		self.countdownShown = -1
		self.pendingWhisper = {}	# name -> last text sent as a normal whisper (offered as offline message on "not logged on")
		self.pendingOffline = {}	# name -> text waiting for AnkaOfflineMsgResult
		self.__Build()
		self.__EnsureConversation(GLOBAL_NAME)

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def BindInterface(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	# ------------------------------------------------------------------ layout
	def __Build(self):
		self.SetSize(WIDTH, HEIGHT)
		self.SetTitleName(localeInfo.WHISPER_MESSENGER_TITLE)
		self.SetCloseEvent(ui.__mem_func__(self.Close))
		self.AddFlag("movable")
		self.AddFlag("float")

		self.leftBg = _MakeImage(self, IMG + "background_left.png", LEFT_X, PANEL_Y)
		self.rightBg = _MakeImage(self, IMG + "background_right.png", RIGHT_X, PANEL_Y)

		# left: search
		self.searchBg = _MakeImage(self, IMG + "input_search.png", LEFT_X + 5, PANEL_Y + 6)
		self.searchLine = ui.EditLine()
		self.searchLine.SetParent(self)
		self.searchLine.SetPosition(LEFT_X + 13, PANEL_Y + 13)
		self.searchLine.SetSize(130, 16)
		self.searchLine.SetMax(24)
		self.searchLine.SetOverlayText(localeInfo.WHISPER_MESSENGER_SEARCH)
		self.searchLine.SetUpdateEvent(ui.__mem_func__(self.__OnSearchChanged))
		self.searchLine.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.searchLine.Show()
		self.searchButton = _MakeButton(self, "btn_search", LEFT_X + 151, PANEL_Y + 10)
		self.searchButton.SetEvent(ui.__mem_func__(self.__OnSearchChanged))

		# left: list area + "new message"
		self.listWindow = ui.Window()
		self.listWindow.SetParent(self)
		self.listWindow.SetPosition(LEFT_X + 5, PANEL_Y + LIST_TOP)
		self.listWindow.SetSize(176, LIST_HEIGHT)
		self.listWindow.OnMouseWheel = ui.__mem_func__(self.__OnListWheel)
		self.listWindow.Show()

		# reference conversation list scrollbar (thin track on the right edge of the list)
		self.listScrollBar = uiCharacterDetails.BaerimScrollBar()
		self.listScrollBar.SetParent(self)
		self.listScrollBar.SetPosition(LEFT_X + 176, PANEL_Y + LIST_TOP + 7)
		self.listScrollBar.SetScrollBarSize(LIST_HEIGHT + 1)
		self.listScrollBar.SetScrollEvent(ui.__mem_func__(self.__OnListScrollBar))
		self.listScrollBar.Hide()
		self.listMaxScroll = 0

		self.createButton = _MakeButton(self, "btn_create", LEFT_X + 7, PANEL_Y + 352, localeInfo.WHISPER_MESSENGER_NEW_MESSAGE)
		self.createButton.SetEvent(ui.__mem_func__(self.OpenNewConversation))

		# right: header
		self.header = _MakeImage(self, IMG + "header_right.png", RIGHT_X + 2, PANEL_Y + 2)
		self.headerIcon = ui.ExpandedImageBox()
		self.headerIcon.SetParent(self)
		self.headerIcon.SetPosition(RIGHT_X + 8, PANEL_Y + 8)
		self.headerIcon.AddFlag("not_pick")
		self.headerIcon.Show()
		self.headerFlag = _MakeImage(self, EMPIRE_FLAG_PATH % 1, RIGHT_X + 42, PANEL_Y + 15)
		self.headerFlag.Hide()
		self.headerName = _MakeText(self, "", RIGHT_X + 44, PANEL_Y + 14, 0xffffffff, "Tahoma:14b")
		self.headerGuild = _MakeText(self, "", RIGHT_X + 44, PANEL_Y + 14, GUILD_TAG_COLOR, "Tahoma:14b")
		self.conversationInfo = {}	# name -> (found, empire, race, online, guild, requestTime)
		# reference AddFriendTarget / BlockPlayer header buttons (player conversations only)
		self.shopButton = _MakeButton(self, "header/shop", RIGHT_X + 292, PANEL_Y + 6)
		self.shopButton.SetEvent(ui.__mem_func__(self.__OpenShop))
		self.shopButton.SetToolTipText(localeInfo.WHISPER_MESSENGER_OPEN_SHOP)
		self.addFriendButton = _MakeButton(self, "header/add", RIGHT_X + 328, PANEL_Y + 6)
		self.addFriendButton.SetEvent(ui.__mem_func__(self.__AddFriend))
		self.addFriendButton.SetToolTipText(localeInfo.WHISPER_MESSENGER_ADD_FRIEND)
		self.blockButton = _MakeButton(self, "header/block", RIGHT_X + 364, PANEL_Y + 6)
		self.blockButton.SetEvent(ui.__mem_func__(self.__BlockPlayer))
		self.blockButton.SetToolTipText(localeInfo.WHISPER_MESSENGER_BLOCK)
		self.guildButton = _MakeButton(self, "header/add_guild", RIGHT_X + 400, PANEL_Y + 6)
		self.guildButton.SetEvent(ui.__mem_func__(self.__InviteGuild))
		self.guildButton.SetToolTipText(localeInfo.WHISPER_MESSENGER_GUILD_INVITE)

		# right: chat area
		self.chatWindow = ui.Window()
		self.chatWindow.SetParent(self)
		self.chatWindow.SetPosition(RIGHT_X + CHAT_LEFT, PANEL_Y + CHAT_TOP)
		self.chatWindow.SetSize(CHAT_WIDTH, CHAT_HEIGHT)
		self.chatWindow.OnMouseWheel = ui.__mem_func__(self.__OnChatWheel)
		self.chatWindow.Show()

		self.chatScroll = uiCharacterDetails.BaerimScrollBar()	# thin reference scrollbar, no arrows
		self.chatScroll.SetParent(self)
		self.chatScroll.SetPosition(RIGHT_X + CHAT_LEFT + CHAT_WIDTH + 3, PANEL_Y + CHAT_TOP - 2)
		self.chatScroll.SetScrollBarSize(CHAT_HEIGHT + 4)
		self.chatScroll.SetScrollEvent(ui.__mem_func__(self.__OnChatScroll))
		self.chatScroll.Hide()

		# global chat rules (reference __UpdateGlobalRules)
		self.rulesWindow = ui.Window()
		self.rulesWindow.SetParent(self.chatWindow)
		self.rulesWindow.SetPosition(0, 0)
		self.rulesWindow.SetSize(CHAT_WIDTH, 70)
		self.rulesBg = ui.Bar()
		self.rulesBg.SetParent(self.rulesWindow)
		self.rulesBg.SetPosition(0, 0)
		self.rulesBg.SetSize(CHAT_WIDTH, 68)
		self.rulesBg.SetColor(RULES_BG_COLOR)
		self.rulesBg.AddFlag("not_pick")
		self.rulesBg.Show()
		self.rulesTexts = [_MakeText(self.rulesWindow, localeInfo.WHISPER_MESSENGER_RULES_TITLE, 4, 2, RULES_TITLE_COLOR)]
		for index, text in enumerate(GLOBAL_RULES):
			self.rulesTexts.append(_MakeText(self.rulesWindow, text, 4, 18 + index * 15, RULES_TEXT_COLOR))
		self.rulesClose = _MakeButton(self.rulesWindow, "close_btn", CHAT_WIDTH - 22, 4)
		self.rulesClose.SetEvent(ui.__mem_func__(self.__CloseRules))

		# right: composer
		self.inputBg = _MakeImage(self, IMG + "input_msg_bg.png", RIGHT_X + 8, PANEL_Y + 326)
		self.chatLine = ui.EditLine()
		self.chatLine.SetParent(self)
		self.chatLine.SetPosition(RIGHT_X + 20, PANEL_Y + 346)
		self.chatLine.SetSize(270, 18)
		self.chatLine.SetMax(200)
		self.chatLine.SetLimitWidth(270)
		self.chatLine.SetOverlayText(localeInfo.WHISPER_MESSENGER_INPUT_HINT)
		self.chatLine.SetReturnEvent(ui.__mem_func__(self.SendMessage))
		self.chatLine.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.chatLine.Show()
		self.sendButton = _MakeButton(self, "btn_send", RIGHT_X + 335, PANEL_Y + 338)
		self.sendButton.SetEvent(ui.__mem_func__(self.SendMessage))
		self.levelWarning = ui.Bar()
		self.levelWarning.SetParent(self)
		self.levelWarning.SetPosition(RIGHT_X + 8, PANEL_Y + 304)
		self.levelWarning.SetSize(424, 20)
		self.levelWarning.SetColor(0xc0400c0c)
		self.levelWarning.AddFlag("not_pick")
		self.levelWarningText = _MakeText(self.levelWarning, localeInfo.WHISPER_MESSENGER_LEVEL_REQUIRED % WHISPER_MIN_LEVEL, 212, 3, 0xffffe0e0)
		self.levelWarningText.SetHorizontalAlignCenter()
		self.levelWarning.Hide()
		self.replyPreview = ui.Bar()
		self.replyPreview.SetParent(self)
		self.replyPreview.SetPosition(RIGHT_X + 8, PANEL_Y + 284)
		self.replyPreview.SetSize(424, 40)
		self.replyPreview.SetColor(0xd0161616)
		self.replyPreviewBar = _Bar(self.replyPreview, 0, 0, 3, 40, REPLY_BAR_COLOR)
		self.replyPreviewTitle = _MakeText(self.replyPreview, "", 10, 5, REPLY_AUTHOR_COLOR)
		self.replyPreviewText = _MakeText(self.replyPreview, "", 10, 21, REPLY_TEXT_COLOR)
		self.replyPreviewClose = _MakeButton(self.replyPreview, "close_btn", 404, 13)
		self.replyPreviewClose.SetEvent(ui.__mem_func__(self.CancelReply))
		self.replyPreview.Hide()
		self.sendCountdown = _MakeText(self, "", RIGHT_X + 335 + 40, PANEL_Y + 348, 0xffffffff, "Tahoma:14b")
		self.sendCountdown.SetHorizontalAlignCenter()
		self.sendCountdown.Hide()
		self.messageMenu = MessageContextMenu()

		self.emojiButton = _MakeButton(self, "emoji_btn", RIGHT_X + 300, PANEL_Y + 340)
		self.emojiButton.SetEvent(ui.__mem_func__(self.__OnEmojiButton))
		self.emojiPicker = uiEmojiPicker.EmojiSelectorWindow()
		self.emojiPicker.SetChatLine(self.chatLine)
		self.emojiPicker.Hide()

	# ------------------------------------------------------------------ open / close
	def __EnsureGuildConversation(self):
		# reference: the guild conversation stays in the list like the global one while the player has a guild
		if player.GetGuildID():
			self.__EnsureConversation(GUILD_NAME)
		elif GUILD_NAME in self.order and not self.conversations[GUILD_NAME]["lines"]:
			self.order.remove(GUILD_NAME)
			del self.conversations[GUILD_NAME]

	def Open(self, name = None):
		self.__LoadHistory()
		self.__EnsureGuildConversation()
		if name:
			self.__EnsureConversation(name)
			self.SelectConversation(name)
		elif not self.current:
			self.SelectConversation(self.order[0] if self.order else GLOBAL_NAME)
		else:
			self.SelectConversation(self.current)
		if not self.IsShow():
			self.SetCenterPosition()
		self.Show()
		self.SetTop()
		self.chatLine.SetFocus()

	def Toggle(self):
		if self.IsShow():
			self.Close()
		else:
			self.Open()

	def Close(self):
		if self.messageMenu:
			self.messageMenu.Close()
		if self.reactionPicker:
			self.reactionPicker.Close()
		if self.emojiPicker:
			self.emojiPicker.Hide()
		self.chatLine.KillFocus()
		self.searchLine.KillFocus()
		self.__CloseDialogs()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def Destroy(self):
		self.__CloseDialogs()
		if self.emojiPicker:
			self.emojiPicker.Hide()
			self.emojiPicker.Destroy()
			self.emojiPicker = None
		self.items = {}
		self.bubbles = []
		self.interface = None

	def __CloseDialogs(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None
		if self.inputDialog:
			self.inputDialog.Hide()
			self.inputDialog = None

	# ------------------------------------------------------------------ data
	def __EnsureConversation(self, name):
		if not self.conversations.has_key(name):
			self.conversations[name] = {"lines" : [], "unread" : 0, "last" : 0, "gm" : FALSE}
			if name == GLOBAL_NAME:
				self.order.insert(0, name)
			elif name == GUILD_NAME:
				self.order.insert(1 if GLOBAL_NAME in self.order else 0, name)
			else:
				self.order.insert(self.__PinnedCount(), name)
		return self.conversations[name]

	def __PinnedCount(self):
		return len([name for name in (GLOBAL_NAME, GUILD_NAME) if name in self.order])

	def __Touch(self, name):
		if name in (GLOBAL_NAME, GUILD_NAME):
			return
		if name in self.order:
			self.order.remove(name)
		self.order.insert(self.__PinnedCount(), name)

	def __AppendLine(self, name, sender, text, mine, system = FALSE, save = TRUE, ts = None):
		conv = self.__EnsureConversation(name)
		ts = ts or _Now()
		conv["lines"].append((sender, text, ts, mine, system))
		if len(conv["lines"]) > HISTORY_LIMIT:
			del conv["lines"][:len(conv["lines"]) - HISTORY_LIMIT]
		conv["last"] = ts
		if save:
			self.__SaveLine(name, sender, text, ts, mine, system)
		if name != GLOBAL_NAME:
			self.__Touch(name)
		isOpen = self.IsShow() and self.current == name
		if not mine and not system and not isOpen and not self.IsMuted(name):
			conv["unread"] += 1
		self.RefreshList()
		if isOpen:
			self.RefreshChat(TRUE)
		return isOpen

	def SetGuildSender(self, name):
		self.guildSenders.add(name)

	def IsGuildSender(self, name):
		return name in self.guildSenders

	def __SetComposerVisible(self, visible):
		for window in (self.inputBg, self.chatLine, self.sendButton, self.emojiButton):
			if visible:
				window.Show()
			else:
				window.Hide()

	def IsMuted(self, name):
		return self.mutedConversations.has_key(name)

	def ToggleMute(self, name):
		if self.mutedConversations.has_key(name):
			del self.mutedConversations[name]
		else:
			self.mutedConversations[name] = 1
			if self.conversations.has_key(name):
				self.conversations[name]["unread"] = 0
		try:
			f = open(self.__SettingsPath(), "w")
			for muted in self.mutedConversations.keys():
				f.write("muted\t%s\n" % _Clean(muted))
			f.close()
		except:
			pass
		self.RefreshList()

	def __LoadSettings(self):
		try:
			f = open(self.__SettingsPath(), "r")
			rows = f.readlines()
			f.close()
		except:
			return
		for row in rows:
			parts = row.strip().split("\t")
			if len(parts) == 2 and parts[0] == "muted":
				self.mutedConversations[parts[1]] = 1
			elif len(parts) == 2 and parts[0] == "global_muted" and parts[1] == "1":	# old settings file
				self.mutedConversations[GLOBAL_NAME] = 1

	def IsExcluded(self, name):
		return name in EXCLUDED_NAMES

	def IsConversationOpen(self, name):
		return self.IsShow() and self.current == name

	def SetGameMaster(self, name):
		self.__EnsureConversation(name)["gm"] = TRUE
		self.RefreshList()

	def RecvWhisperLine(self, name, line, isSystem = FALSE):
		"""line comes as 'Sender : text' (chat.AppendWhisper format) or plain system text."""
		sender = name
		text = line
		if not isSystem and " : " in line:
			(sender, text) = line.split(" : ", 1)
		if "|Hpm:" in sender:	# guild chat sender: "<language> <flag> |Hpm:<name>|h<name>|h|r"
			sender = sender.split("|Hpm:", 1)[1].split("|h", 1)[0]
		mine = (sender == player.GetName())
		if not isSystem and text.startswith(REACTION_PREFIX):
			self.__ApplyRemoteReaction(name, text)
			return TRUE		# a reaction is no message: no envelope, no unread
		return self.__AppendLine(name, sender, text, mine, isSystem)

	def __ApplyRemoteReaction(self, name, text):
		parts = text[len(REACTION_PREFIX):].split("~", 1)
		if len(parts) != 2:
			return
		(emoji, excerpt) = parts
		conv = self.conversations.get(name)
		if not conv:
			return
		for (sender, lineText, ts, mine, system) in reversed(conv["lines"]):
			if mine and not system and StripMarkup(MessageText(lineText)).startswith(excerpt):
				self.__SetReaction(name, ts, lineText, emoji)
				return

	def __SetReaction(self, name, ts, rawText, emoji):
		key = (name, ts, rawText)
		if emoji and self.emojiPicker.IsEmojiName(emoji):
			self.reactions[key] = emoji
		else:
			self.reactions.pop(key, None)
		self.__SaveLine(name, REACTION_TAG, (emoji or "-") + "~" + rawText, ts, FALSE, FALSE)
		if self.current == name:
			self.RefreshChat(FALSE)

	def RecvGlobalChat(self, payload):
		parts = payload.split(PAYLOAD_SEPARATOR, 2)
		if len(parts) < 3:
			return
		(sender, isGM, text) = (parts[0], parts[1] == "1", parts[2])
		text = text.replace(PAYLOAD_SEPARATOR, " ")
		self.__AppendLine(GLOBAL_NAME, sender, text, sender == player.GetName())

	def RecvGlobalCooldown(self, seconds):
		# the server still counts down: sync the send button instead of adding a line
		self.globalCooldownEnd = time.time() + int(seconds)
		self.__RefreshSendButton()

	def __GlobalCooldownLeft(self):
		return max(0, int(self.globalCooldownEnd - time.time() + 0.999))

	def __RefreshSendButton(self):
		left = self.__GlobalCooldownLeft() if self.current == GLOBAL_NAME else 0
		if left == self.countdownShown:
			return
		self.countdownShown = left
		prefix = "btn_send_empty" if left > 0 else "btn_send"
		self.sendButton.SetUpVisual(IMG + prefix + "_norm.png")
		self.sendButton.SetOverVisual(IMG + prefix + "_hover.png")
		self.sendButton.SetDownVisual(IMG + prefix + "_select.png")
		if left > 0:
			self.sendCountdown.SetText(str(left))
			self.sendCountdown.Show()
		else:
			self.sendCountdown.Hide()

	def OnUpdate(self):
		if self.globalCooldownEnd:
			self.__RefreshSendButton()

	# ------------------------------------------------------------------ message menu / reply
	def __OpenMessageMenu(self, data):
		(sender, text, mine, system, ts, rawText) = data
		if system:
			return
		entries = []
		isPrivate = self.current and not self.current in (GLOBAL_NAME, GUILD_NAME) and not self.current.startswith("[")
		if isPrivate:
			# private chat: "Tepki ver" first (reference private message menu)
			entries.append((localeInfo.WHISPER_MESSENGER_MENU_REACT, "menu_react.png", lambda arg = (ts, rawText): self.__OpenReactionPicker(arg)))
		elif not mine and sender and sender != player.GetName():
			entries.append((localeInfo.WHISPER_MESSENGER_MENU_WHISPER, "menu_whisper.png", lambda arg = sender: self.Open(arg)))
		entries.append((localeInfo.WHISPER_MESSENGER_MENU_REPLY, "menu_reply.png", lambda arg = (sender, text): self.SelectReply(arg)))
		entries.append((localeInfo.WHISPER_MESSENGER_MENU_COPY, "menu_copy.png", lambda arg = text: self.__CopyMessage(arg)))
		self.messageMenu.Open(entries)

	def __OpenReactionPicker(self, data):
		# reference: "Tepki ver" opens the composer emoji picker, the pick becomes the reaction
		self.emojiPicker.Hide()
		self.emojiPicker.SetSelectionEvent(lambda emoji, arg = data: self.__React(arg, emoji))
		self.__ToggleEmoji()

	def __React(self, data, emoji):
		(ts, rawText) = data
		name = self.current
		if not name:
			return
		key = (name, ts, rawText)
		if self.reactions.get(key) == emoji:
			emoji = ""		# picking the same emoji again removes it
		self.__SetReaction(name, ts, rawText, emoji)
		excerpt = StripMarkup(MessageText(rawText)).replace("~", "-")[:REPLY_EXCERPT_LEN]
		if app.ENABLE_MULTILANGUAGE_CHATTING:
			net.SendWhisperPacket(name, "%s%s~%s" % (REACTION_PREFIX, emoji, excerpt), self.chatLine.GetInputSystemLangID())
		else:
			net.SendWhisperPacket(name, "%s%s~%s" % (REACTION_PREFIX, emoji, excerpt))

	def OpenConversationMenu(self, name):
		# reference conversation card menu: copy + mute / unmute
		muteLabel = localeInfo.WHISPER_MESSENGER_MENU_UNMUTE if self.IsMuted(name) else localeInfo.WHISPER_MESSENGER_MENU_MUTE
		entries = [(localeInfo.WHISPER_MESSENGER_MENU_COPY, "menu_copy.png", lambda arg = self.__Title(name): self.__CopyText(arg)),
			(muteLabel, "menu_mute.png", lambda arg = name: self.ToggleMute(arg))]
		self.messageMenu.Open(entries)

	def __CopyText(self, text):
		if hasattr(ime, "CopyTextToClipBoard"):
			ime.CopyTextToClipBoard(text)

	def __CopyMessage(self, text):
		plain = StripMarkup(MessageText(text))
		if hasattr(ime, "CopyTextToClipBoard"):
			ime.CopyTextToClipBoard(plain)

	def SelectReply(self, data):
		(sender, text) = data
		if not self.current:
			return
		excerpt = StripMarkup(MessageText(text)).replace("~", "-")
		if len(excerpt) > REPLY_EXCERPT_LEN:
			excerpt = excerpt[:REPLY_EXCERPT_LEN]
		author = (sender or player.GetName()).replace("~", "-")
		self.replyDraft[self.current] = (author, excerpt)
		self.__RefreshReplyPreview()
		self.chatLine.SetFocus()

	def CancelReply(self):
		if self.current:
			self.replyDraft.pop(self.current, None)
		self.__RefreshReplyPreview()

	def __RefreshReplyPreview(self):
		draft = self.replyDraft.get(self.current)
		if not draft:
			self.replyPreview.Hide()
			return
		self.replyPreviewTitle.SetText(localeInfo.WHISPER_MESSENGER_REPLY_TO % draft[0])
		self.replyPreviewText.SetText(self.FitText(draft[1], 380))
		self.replyPreview.Show()

	# ------------------------------------------------------------------ send

	def GetTotalUnreadCount(self):
		# reference badge: one per conversation with news, not one per line
		return len([name for (name, conv) in self.conversations.items() if name != GLOBAL_NAME and conv["unread"] > 0])

	def MarkRead(self, name):
		conv = self.conversations.get(name)
		if conv:
			conv["unread"] = 0
			self.RefreshList()

	def MarkAllRead(self):
		for conv in self.conversations.values():
			conv["unread"] = 0
		self.RefreshList()

	# ------------------------------------------------------------------ history file (per character)
	# reference Data/UserData/whisper/<character> files (old root files are moved over once)
	def __HistoryPath(self):
		if not WhisperMessenger._oldFilesMoved:
			WhisperMessenger._oldFilesMoved = True
			userData.MoveOldFiles("whisper_history_", ".dat", "whisper", "%s.dat")
			userData.MoveOldFiles("whisper_settings_", ".dat", "whisper", "%s.muted.dat")
		name = player.GetName()
		return userData.GetPath("%s.dat" % name, "whisper", "whisper_history_%s.dat" % name)

	def __SettingsPath(self):
		name = player.GetName()
		return userData.GetPath("%s.muted.dat" % name, "whisper", "whisper_settings_%s.dat" % name)

	def __LoadHistory(self):
		path = self.__HistoryPath()
		if self.historyFile == path:
			return
		self.historyFile = path
		self.__LoadSettings()
		try:
			f = open(path, "r")
			rows = f.readlines()
			f.close()
		except:
			return
		globalRows = 0
		for row in rows:
			parts = row.rstrip("\r\n").split("\t", 5)
			if len(parts) != 6:
				continue
			(name, sender, ts, mine, system, text) = parts
			if name in EXCLUDED_NAMES:
				continue
			if name == GLOBAL_NAME:	# reference: the global chat is not kept, it starts empty on every client start
				globalRows += 1
				continue
			if sender == REACTION_TAG:
				try:
					(emoji, rawText) = text.split("~", 1)
					key = (name, int(ts), rawText)
					if emoji and emoji != "-":
						self.reactions[key] = emoji
					else:
						self.reactions.pop(key, None)
				except:
					pass
				continue
			try:
				conv = self.__EnsureConversation(name)
				conv["lines"].append((sender, text, int(ts), mine == "1", system == "1"))
				conv["last"] = max(conv["last"], int(ts))
			except:
				continue
		for conv in self.conversations.values():
			if len(conv["lines"]) > HISTORY_LIMIT:
				del conv["lines"][:len(conv["lines"]) - HISTORY_LIMIT]
		if globalRows or len(rows) > HISTORY_LIMIT * 20:	# drop old global rows / keep the append-only file from growing forever
			self.__RewriteHistory()
		others = [name for name in self.conversations.keys() if not name in (GLOBAL_NAME, GUILD_NAME)]
		others.sort(key = lambda name: -self.conversations[name]["last"])
		self.order = [GLOBAL_NAME] + ([GUILD_NAME] if GUILD_NAME in self.conversations else []) + others

	def __SaveLine(self, name, sender, text, ts, mine, system):
		if name == GLOBAL_NAME:
			return
		try:
			f = open(self.__HistoryPath(), "a")
			f.write("%s\t%s\t%d\t%d\t%d\t%s\n" % (_Clean(name), _Clean(sender), ts, int(bool(mine)), int(bool(system)), _Clean(text)))
			f.close()
		except:
			pass

	def __RewriteHistory(self):
		try:
			f = open(self.__HistoryPath(), "w")
			for (name, conv) in self.conversations.items():
				if name == GLOBAL_NAME:
					continue
				for (sender, text, ts, mine, system) in conv["lines"]:
					emoji = self.reactions.get((name, ts, text))
					if emoji:
						f.write("%s\t%s\t%d\t0\t0\t%s\n" % (_Clean(name), REACTION_TAG, ts, _Clean(emoji + "~" + text)))
					f.write("%s\t%s\t%d\t%d\t%d\t%s\n" % (_Clean(name), _Clean(sender), ts, int(bool(mine)), int(bool(system)), _Clean(text)))
			f.close()
		except:
			pass

	# ------------------------------------------------------------------ left list
	def TextWidth(self, text):
		return self.measure.Width(text) if text else 0

	def FitText(self, text, maxWidth):
		if self.measure.Width(text) <= maxWidth:
			return text
		while text and self.measure.Width(text + "..") > maxWidth:
			text = text[:-1]
		return text + ".."

	def __Title(self, name, header = FALSE):
		if name == GLOBAL_NAME:
			return GLOBAL_TITLE
		if name == GUILD_NAME:	# reference "[GUILD] <guild name>"
			guildName = guild.GetGuildName() if player.GetGuildID() else ""
			guildName = guildName or localeInfo.WHISPER_MESSENGER_GUILD
			return ("[GUILD] " + guildName) if header else guildName	# the tag only shows in the chat header
		return name

	def __Preview(self, conv):
		if not conv["lines"]:
			return ""
		(sender, text, ts, mine, system) = conv["lines"][-1]
		text = StripMarkup(MessageText(text))
		if self.measure.Width(text) <= 140:
			return text
		while text and self.measure.Width(text + "...") > 140:
			text = text[:-1]
		return text + "..."

	def RefreshList(self):
		if self.interface and hasattr(self.interface, "RefreshMessengerButton"):
			self.interface.RefreshMessengerButton()
		search = self.searchLine.GetText().lower() if self.searchLine else ""
		names = [name for name in self.order if not search or search in self.__Title(name).lower()]
		maxScroll = max(0, len(names) * CONV_ITEM_STEP - LIST_HEIGHT)
		self.listScroll = min(self.listScroll, maxScroll)
		self.listMaxScroll = maxScroll
		if maxScroll > 0:
			self.listScrollBar.SetPos(float(self.listScroll) / maxScroll, FALSE)
			self.listScrollBar.Show()
		else:
			self.listScrollBar.Hide()

		for name in self.items.keys():
			if not name in self.conversations:
				self.items[name].Hide()
				del self.items[name]

		y = -self.listScroll
		for name in names:
			item = self.items.get(name)
			if not item:
				item = ConversationItem(self, name)
				item.SetParent(self.listWindow)
				self.items[name] = item
			conv = self.conversations[name]
			item.Refresh(self.__Title(name), conv["unread"], conv["last"], self.__Preview(conv), conv["gm"], name == self.current)
			if y >= 0 and y + CONV_ITEM_HEIGHT <= LIST_HEIGHT:
				item.SetPosition(0, int(y))
				item.Show()
			else:
				item.Hide()
			y += CONV_ITEM_STEP
		for (name, item) in self.items.items():
			if not name in names:
				item.Hide()

	def __OnListScrollBar(self):
		self.listScroll = int(self.listMaxScroll * self.listScrollBar.GetPos())
		self.RefreshList()

	def OnRunMouseWheel(self, nLen):
		# the client sends the wheel to the picked window, then to its root (this board)
		self.OnMouseWheel(nLen)
		return True

	def __OnListWheel(self, length):
		self.listScroll = max(0, self.listScroll + (-CONV_ITEM_STEP if length > 0 else CONV_ITEM_STEP))
		self.RefreshList()
		return TRUE

	def __OnSearchChanged(self):
		self.listScroll = 0
		self.RefreshList()

	# ------------------------------------------------------------------ conversation
	def SelectConversation(self, name):
		conv = self.__EnsureConversation(name)
		self.current = name
		unread = conv["unread"]
		conv["unread"] = 0
		isGlobal = (name == GLOBAL_NAME)
		self.__RefreshHeader()
		isGuildSender = name in self.guildSenders
		if not isGlobal and name != GUILD_NAME and not isGuildSender:
			self.__RequestConversationInfo(name)
		self.__SetComposerVisible(not isGuildSender)
		isPlayer = not isGlobal and name != GUILD_NAME and not conv["gm"] and not isGuildSender
		if self.__IsLevelBlocked(name):
			self.levelWarning.Show()
		else:
			self.levelWarning.Hide()
		for button in (self.shopButton, self.addFriendButton, self.blockButton):
			if isPlayer:
				button.Show()
			else:
				button.Hide()
		if isPlayer and player.GetGuildID():
			self.guildButton.Show()
		else:
			self.guildButton.Hide()
		# reference "N okunmamis mesaj" divider in front of the first unread line
		self.unreadDivider = (len(conv["lines"]) - unread, unread) if unread > 0 else None
		self.RefreshList()
		self.RefreshChat(TRUE)
		self.__RefreshReplyPreview()
		self.countdownShown = -1
		self.__RefreshSendButton()
		if self.interface and hasattr(self.interface, "OnWhisperConversationRead"):
			self.interface.OnWhisperConversationRead(name)

	def __RequestConversationInfo(self, name, force = FALSE):
		info = self.conversationInfo.get(name)
		now = _Now()
		if not force and info and now - info[5] < INFO_REFRESH_INTERVAL:
			return
		self.conversationInfo[name] = (info or (0, 0, -1, 0, "-", 0))[:5] + (now,)
		net.SendChatPacket("/anka_whisper_info " + name)

	def SetConversationInfo(self, name, found, empire, race, online, guildName):
		self.conversationInfo[name] = (found, empire, race, online, guildName, _Now())
		if name == self.current:
			self.__RefreshHeader()

	def __RefreshHeader(self):
		name = self.current
		if not name:
			return
		conv = self.conversations.get(name, {"gm" : FALSE})
		self.headerIcon.SetScale(1.0, 1.0)
		self.headerFlag.Hide()
		self.headerGuild.SetText("")
		nameColor = CARD_NAME_COLOR
		if name == GLOBAL_NAME:
			self.headerIcon.LoadImage(IMG + "baerim_global_icon_mini.png")
		elif conv["gm"]:
			self.headerIcon.LoadImage(IMG + "gm_icon_mini.png")
		elif name == GUILD_NAME or name in self.guildSenders:
			self.headerIcon.LoadImage(IMG + "guild_icon_mini.png")
		else:
			info = self.conversationInfo.get(name)
			race = info[2] if info else -1
			if 0 <= race < len(FACE_IMAGES):
				self.headerIcon.LoadImage("icon/face/%s.tga" % FACE_IMAGES[race])
				self.headerIcon.SetScale(27.0 / 43.0, 27.0 / 43.0)
			else:
				self.headerIcon.LoadImage(IMG + "player_icon_mini.png")
			if info and info[0] and 1 <= info[1] <= 3:
				self.headerFlag.LoadImage(EMPIRE_FLAG_PATH % info[1])
				self.headerFlag.Show()
			if info and info[0] and not info[3]:
				nameColor = OFFLINE_NAME_COLOR
			if info and info[4] and info[4] != "-":
				self.headerGuild.SetText("[%s]" % info[4])
		x = RIGHT_X + 44
		if self.headerFlag.IsShow():
			self.headerFlag.SetPosition(x, PANEL_Y + 15)
			x += self.headerFlag.GetWidth() + 6
		self.headerName.SetPosition(x, PANEL_Y + 14)
		self.headerName.SetPackedFontColor(nameColor)
		self.headerName.SetText(self.__Title(name, TRUE))
		self.headerGuild.SetPosition(x + self.headerName.GetTextSize()[0] + 6, PANEL_Y + 14)

	def __CloseRules(self):
		self.rulesClosed = TRUE
		self.RefreshChat(FALSE)

	def __ClearBubbles(self):
		for bubble in self.bubbles:
			bubble.Hide()
		self.bubbles = []

	def __DayLabel(self, ts):
		today = time.localtime(_Now())
		day = time.localtime(ts)
		if day[:3] == today[:3]:
			return localeInfo.WHISPER_MESSENGER_TODAY
		if day[:3] == time.localtime(_Now() - 86400)[:3]:
			return localeInfo.WHISPER_MESSENGER_YESTERDAY
		return time.strftime("%d.%m.%Y", day)

	def __MakeDateSeparator(self, ts):
		label = self.__DayLabel(ts)
		separator = ui.Window()
		separator.SetParent(self.chatWindow)
		separator.SetSize(CHAT_WIDTH - 8, 18)
		separator.AddFlag("not_pick")
		text = _MakeText(separator, label, int((CHAT_WIDTH - 8) / 2), 2, DATE_TEXT_COLOR)
		text.SetHorizontalAlignCenter()
		lineWidth = int((CHAT_WIDTH - 8) / 2) - int(self.measure.Width(label) / 2) - 30
		separator.children = [text, _Bar(separator, 20, 9, lineWidth, 1, DATE_LINE_COLOR),
			_Bar(separator, CHAT_WIDTH - 8 - 20 - lineWidth, 9, lineWidth, 1, DATE_LINE_COLOR)]
		separator.centered = TRUE
		separator.mine = FALSE
		return separator

	def __MakeDivider(self, text):
		width = min(CHAT_WIDTH - 40, self.measure.Width(text) + 40)
		divider = ui.Bar()
		divider.SetParent(self.chatWindow)
		divider.SetSize(int(width), DIVIDER_HEIGHT)
		divider.SetColor(DIVIDER_COLOR)
		divider.children = [
			_Bar(divider, 0, 0, width, 1, DIVIDER_BORDER_COLOR), _Bar(divider, 0, DIVIDER_HEIGHT - 1, width, 1, DIVIDER_BORDER_COLOR),
			_Bar(divider, 0, 0, 1, DIVIDER_HEIGHT, DIVIDER_BORDER_COLOR), _Bar(divider, width - 1, 0, 1, DIVIDER_HEIGHT, DIVIDER_BORDER_COLOR)]
		label = _MakeText(divider, text, int(width / 2), 4, DIVIDER_TEXT_COLOR)
		label.SetHorizontalAlignCenter()
		divider.children.append(label)
		divider.centered = TRUE
		divider.mine = FALSE
		divider.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		return divider

	def __MakeBubble(self, sender, text, ts, mine, system, showName):
		if system:
			return self.__MakeDivider(text)
		rawText = text
		reply = SplitReply(text)
		if reply:
			text = reply[2]
		textWidth = 0
		lines = self.measure.Wrap(text, BUBBLE_MAX_TEXT_WIDTH)
		for line in lines:
			textWidth = max(textWidth, self.measure.Width(line))
		clock = _Clock(ts)
		nameText = sender if (showName and not mine and not system) else ""
		textWidth = max(textWidth, self.measure.Width(nameText), self.measure.Width(clock) + 10)
		replyTitle = ""
		if reply:
			replyTitle = localeInfo.WHISPER_MESSENGER_REPLY_TO % reply[0]
			textWidth = max(textWidth, min(BUBBLE_MAX_TEXT_WIDTH, self.measure.Width(replyTitle) + 12), min(BUBBLE_MAX_TEXT_WIDTH, self.measure.Width(reply[1]) + 12))
		# reference: a short message keeps its time on the same line ("malmisin 23:03")
		inlineClock = not nameText and not reply and len(lines) == 1 and 			self.measure.Width(lines[0]) + 8 + self.measure.Width(clock) <= BUBBLE_MAX_TEXT_WIDTH
		if inlineClock:
			textWidth = self.measure.Width(lines[0]) + 8 + self.measure.Width(clock)
		width = textWidth + BUBBLE_PADDING * 2
		height = BUBBLE_PADDING + (LINE_HEIGHT if nameText else 0) + (LINE_HEIGHT * 2 + 8 if reply else 0) + len(lines) * LINE_HEIGHT + (BUBBLE_PADDING - 2 if inlineClock else LINE_HEIGHT + 2)
		if inlineClock:
			height = LINE_HEIGHT + 6	# reference one-line bubble is ~21px high

		bubble = ui.Bar()
		bubble.SetParent(self.chatWindow)
		bubble.SetSize(int(width), int(height))
		bubble.SetColor(MINE_BUBBLE_COLOR if mine else BUBBLE_COLOR)
		y = 3 if inlineClock else BUBBLE_PADDING - 2
		bubble.children = []
		if nameText:
			bubble.children.append(_MakeText(bubble, nameText, BUBBLE_PADDING, y, NAME_COLOR))
			y += LINE_HEIGHT
		if reply:
			# reference quote card: green bar, "<author> adli kisiye yanit", excerpt
			cardWidth = width - BUBBLE_PADDING * 2
			bubble.children.append(_Bar(bubble, BUBBLE_PADDING, y, cardWidth, LINE_HEIGHT * 2 + 4, REPLY_BG_COLOR))
			bubble.children.append(_Bar(bubble, BUBBLE_PADDING, y, 2, LINE_HEIGHT * 2 + 4, REPLY_BAR_COLOR))
			bubble.children.append(_MakeText(bubble, self.FitText(replyTitle, cardWidth - 10), BUBBLE_PADDING + 7, y + 1, REPLY_AUTHOR_COLOR))
			bubble.children.append(_MakeText(bubble, self.FitText(reply[1], cardWidth - 10), BUBBLE_PADDING + 7, y + 1 + LINE_HEIGHT, REPLY_TEXT_COLOR))
			y += LINE_HEIGHT * 2 + 8
		for line in lines:
			textLine = LinkTextLine()
			textLine.SetParent(bubble)
			textLine.SetPackedFontColor(SYSTEM_COLOR if system else TEXT_COLOR)
			textLine.SetText(line)
			textLine.SetPosition(BUBBLE_PADDING, int(y))
			textLine.clickEvent = ui.__mem_func__(self.__OnClickBubbleText)
			textLine.rightClickEvent = lambda arg = (sender, text, mine, system, ts, rawText): self.__OpenMessageMenu(arg)
			textLine.Show()
			bubble.children.append(textLine)
			y += LINE_HEIGHT
		timeText = _MakeText(bubble, clock, width - BUBBLE_PADDING, y - LINE_HEIGHT if inlineClock else y, MINE_TIME_COLOR if mine else TIME_COLOR)
		timeText.SetHorizontalAlignRight()
		bubble.children.append(timeText)
		# reference speech tail on the upper corner, outside the bubble
		bubble.children.append(_MakeImage(bubble, IMG + ("bubble_tail_mine.png" if mine else "bubble_tail_other.png"), width if mine else -BUBBLE_TAIL, 0))
		bubble.mine = mine
		bubble.centered = FALSE
		bubble.OnMouseRightButtonDown = lambda arg = (sender, text, mine, system, ts, rawText): self.__OpenMessageMenu(arg)
		bubble.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		for child in bubble.children:
			if isinstance(child, LinkTextLine):
				child.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		bubble.extra = 0
		emoji = self.reactions.get((self.current, ts, rawText))
		if emoji:
			# reaction pill hanging under the bubble corner (reference WhisperReactionPill)
			pill = ui.ExpandedImageBox()
			pill.SetParent(bubble)
			pill.LoadImage("icon/emote/%s.tga" % emoji)
			pill.SetScale(0.9, 0.9)
			pill.SetPosition(4 if not mine else width - 20, height - 3)
			pill.AddFlag("not_pick")
			pill.Show()
			bubble.children.append(pill)
			bubble.extra = REACTION_HEIGHT
		return bubble

	def RefreshChat(self, scrollToBottom):
		self.__ClearBubbles()
		if not self.current:
			return
		conv = self.conversations.get(self.current)
		if not conv:
			return
		isGlobal = (self.current == GLOBAL_NAME)

		top = 0
		if isGlobal and not self.rulesClosed:
			self.rulesWindow.Show()
			top = 74
		else:
			self.rulesWindow.Hide()

		bubbles = []
		divider = getattr(self, "unreadDivider", None)
		lastDay = None
		for (index, (sender, text, ts, mine, system)) in enumerate(conv["lines"]):
			day = time.localtime(ts)[:3]
			if day != lastDay:
				bubbles.append(self.__MakeDateSeparator(ts))
				lastDay = day
			if divider and index == divider[0]:
				bubbles.append(self.__MakeDivider(localeInfo.WHISPER_MESSENGER_UNREAD_DIVIDER % divider[1]))
			bubbles.append(self.__MakeBubble(sender, text, ts, mine, system, isGlobal or self.current in self.guildSenders))
		totalHeight = sum([bubble.GetHeight() + getattr(bubble, "extra", 0) + BUBBLE_GAP for bubble in bubbles])
		visible = CHAT_HEIGHT - top
		overflow = max(0, totalHeight - visible)
		if overflow > 0:
			self.chatScroll.SetMiddleBarSize(max(0.1, float(visible) / float(totalHeight)))
			if scrollToBottom:
				self.refreshingChat = TRUE
				self.chatScroll.SetPos(1.0)
				self.refreshingChat = FALSE
			self.chatScroll.Show()
			offset = int(overflow * self.chatScroll.GetPos())
		else:
			self.chatScroll.Hide()
			offset = 0

		y = top - offset
		for bubble in bubbles:
			height = bubble.GetHeight() + getattr(bubble, "extra", 0)
			if y >= top and y + height <= CHAT_HEIGHT:
				if bubble.centered:
					x = (CHAT_WIDTH - bubble.GetWidth()) / 2
				elif bubble.mine:
					x = CHAT_WIDTH - bubble.GetWidth() - 4 - BUBBLE_TAIL
				else:
					x = BUBBLE_TAIL
				bubble.SetPosition(int(x), int(y))
				bubble.Show()
			else:
				bubble.Hide()
			y += height + BUBBLE_GAP
		self.bubbles = bubbles

	def __OnChatScroll(self):
		if not getattr(self, "refreshingChat", FALSE):	# SetPos inside RefreshChat fires the scroll event
			self.RefreshChat(FALSE)

	def OnMouseWheel(self, length):
		(mx, my) = wndMgr.GetMousePosition()
		(lx, ly) = self.listWindow.GetGlobalPosition()
		if lx <= mx < lx + self.listWindow.GetWidth() and ly <= my < ly + self.listWindow.GetHeight():
			return self.__OnListWheel(length)
		return self.__OnChatWheel(length)

	def __OnChatWheel(self, length):
		if self.chatScroll.IsShow():
			if length > 0:
				self.chatScroll.OnUp()
			else:
				self.chatScroll.OnDown()
		return TRUE

	def __AddFriend(self):
		if self.current and not self.current in (GLOBAL_NAME, GUILD_NAME):
			net.SendMessengerAddByNamePacket(self.current)

	def __BlockPlayer(self):
		if self.current and not self.current in (GLOBAL_NAME, GUILD_NAME):
			net.SendMessengerBlockAddByNamePacket(self.current)

	def __OpenShop(self):
		if self.current and not self.current in (GLOBAL_NAME, GUILD_NAME):
			net.SendChatPacket("/anka_open_shop " + self.current)

	def __InviteGuild(self):
		if self.current and not self.current in (GLOBAL_NAME, GUILD_NAME):
			net.SendChatPacket("/anka_guild_invite " + self.current)

	def __OnEmojiButton(self):
		self.emojiPicker.SetSelectionEvent(None)	# composer button: picks go to the chat line again
		self.__ToggleEmoji()

	def __ToggleEmoji(self):
		(x, y) = self.GetGlobalPosition()
		self.emojiPicker.Toggle(x + RIGHT_X + 440 - uiEmojiPicker.WIDTH, y + PANEL_Y + 322 - uiEmojiPicker.HEIGHT)

	def __IsLevelBlocked(self, name):
		if name in (GLOBAL_NAME, GUILD_NAME) or name.startswith("["):
			return FALSE
		return player.GetStatus(player.LEVEL) < WHISPER_MIN_LEVEL

	def __OnClickBubbleText(self):
		hyperlink = ui.GetHyperlink()
		if not hyperlink:
			return
		if app.IsPressed(app.DIK_LALT):
			ime.PasteString(chat.GetLinkFromHyperlink(hyperlink))
		elif self.interface:
			self.interface.MakeHyperlinkTooltip(hyperlink)

	# ------------------------------------------------------------------ send
	def SendMessage(self):
		text = self.chatLine.GetText()
		if not text or not self.current:
			return
		if net.IsInsultIn(text):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHAT_INSULT_STRING)
			return

		if self.__IsLevelBlocked(self.current):
			self.levelWarning.Show()
			return

		if not self.current in (GLOBAL_NAME, GUILD_NAME) and not self.current.startswith("["):
			info = self.conversationInfo.get(self.current)
			if info and info[0] and not info[3]:	# known and offline
				self.chatLine.SetText("")
				self.__AskOfflineMessage(self.current, text)
				return

		draft = self.replyDraft.pop(self.current, None)
		if draft:
			text = "%s%s~%s~%s" % (REPLY_PREFIX, draft[0], draft[1], text)
			self.__RefreshReplyPreview()

		if self.current == GLOBAL_NAME:
			if self.__GlobalCooldownLeft() > 0:	# 60 s per player, counted down on the send button
				if draft:
					self.replyDraft[self.current] = draft
					self.__RefreshReplyPreview()
				return
			net.SendChatPacket("/anka_global " + text)		# echoed back to everybody, sender included
			self.globalCooldownEnd = time.time() + GLOBAL_COOLDOWN
			self.__RefreshSendButton()
		elif self.current == GUILD_NAME:
			net.SendChatPacket(text, chat.CHAT_TYPE_GUILD)
		else:
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				net.SendWhisperPacket(self.current, text, self.chatLine.GetInputSystemLangID())
			else:
				net.SendWhisperPacket(self.current, text)
			chat.AppendWhisper(chat.WHISPER_TYPE_CHAT, self.current, player.GetName() + " : " + text)
			self.__AppendLine(self.current, player.GetName(), text, TRUE)
			self.pendingWhisper[self.current] = text
		self.chatLine.SetText("")

	# ------------------------------------------------------------------ offline messages
	def OnWhisperNotLogon(self, name):
		text = self.pendingWhisper.pop(name, None)
		if not text:
			return FALSE
		conv = self.conversations.get(name)
		if conv and conv["lines"] and conv["lines"][-1][3] and conv["lines"][-1][1] == text:
			conv["lines"].pop()		# the whisper never arrived, the offline message replaces it
			self.__RewriteHistory()
			if self.current == name:
				self.RefreshChat(TRUE)
		info = self.conversationInfo.get(name)
		self.conversationInfo[name] = (1, info[1] if info else 0, info[2] if info else -1, 0, info[4] if info else "-", _Now())
		if self.current == name:
			self.__RefreshHeader()
		self.__AskOfflineMessage(name, text)
		return TRUE

	def __AskOfflineMessage(self, name, text):
		self.__CloseDialogs()
		dialog = uiCommon.QuestionDialog2()
		dialog.SetText1(localeInfo.WHISPER_MESSENGER_OFFLINE_ASK % name)
		dialog.SetText2(localeInfo.WHISPER_MESSENGER_OFFLINE_COST % OFFLINE_MESSAGE_COST)
		dialog.SetAcceptEvent(lambda arg1 = name, arg2 = text: self.__SendOfflineMessage(arg1, arg2))
		dialog.SetCancelEvent(ui.__mem_func__(self.__CloseDialogs))
		dialog.Open()
		self.questionDialog = dialog

	def __SendOfflineMessage(self, name, text):
		self.__CloseDialogs()
		self.pendingOffline[name] = text
		net.SendChatPacket("/anka_offline_msg %s %s" % (name, text))

	def OnOfflineMessageResult(self, result, name):
		text = self.pendingOffline.pop(name, None)
		if result == 1 and text:
			self.__AppendLine(name, player.GetName(), text, TRUE)
			self.__AppendLine(name, "", localeInfo.WHISPER_MESSENGER_OFFLINE_SENT, TRUE, TRUE)
		elif result == 2 and text:		# came online meanwhile
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				net.SendWhisperPacket(name, text, self.chatLine.GetInputSystemLangID())
			else:
				net.SendWhisperPacket(name, text)
			self.__AppendLine(name, player.GetName(), text, TRUE)
		elif result == 0:
			self.__AppendLine(name, "", localeInfo.WHISPER_MESSENGER_NO_SUCH_PLAYER, TRUE, TRUE)

	def RecvOfflineMessage(self, sender, sentAt, text):
		return self.__AppendLine(sender, sender, text, FALSE, ts = sentAt)

	# ------------------------------------------------------------------ new / delete
	def OpenNewConversation(self):
		self.__CloseDialogs()
		dialog = uiCommon.InputDialog()
		dialog.SetTitle(localeInfo.WHISPER_MESSENGER_NEW_MESSAGE)
		dialog.SetMaxLength(24)
		dialog.SetAcceptEvent(ui.__mem_func__(self.__OnAcceptNewConversation))
		dialog.SetCancelEvent(ui.__mem_func__(self.__CloseDialogs))
		dialog.Open()
		self.inputDialog = dialog

	def __OnAcceptNewConversation(self):
		name = self.inputDialog.GetText().strip() if self.inputDialog else ""
		self.__CloseDialogs()
		if not name or name == player.GetName():
			return
		self.__EnsureConversation(name)
		self.__Touch(name)
		self.SelectConversation(name)
		self.chatLine.SetFocus()

	def RequestDeleteConversation(self, name):
		self.__CloseDialogs()
		dialog = uiCommon.QuestionDialog()
		dialog.SetText(localeInfo.WHISPER_MESSENGER_DELETE_CONVERSATION % name)
		dialog.SetAcceptEvent(lambda arg = name: self.__DeleteConversation(arg))
		dialog.SetCancelEvent(ui.__mem_func__(self.__CloseDialogs))
		dialog.Open()
		self.questionDialog = dialog

	def __DeleteConversation(self, name):
		self.__CloseDialogs()
		if name == GLOBAL_NAME or not self.conversations.has_key(name):
			return
		del self.conversations[name]
		if name in self.order:
			self.order.remove(name)
		if self.items.has_key(name):
			self.items[name].Hide()
			del self.items[name]
		self.__RewriteHistory()
		if self.current == name:
			self.current = None
			self.SelectConversation(self.order[0] if self.order else GLOBAL_NAME)
		else:
			self.RefreshList()
