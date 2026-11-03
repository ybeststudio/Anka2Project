import ui
import uiCharacterDetails

# Emoji picker (reference Baerim emoji.EmojiSelectorWindow): Recently Used / Faces / Custom tabs, search,
# one scrolling list with a header per section. A pick inserts "|Eemote/<name>|e" (icon/emote/<name>.tga).

EMOTE_PATH = "icon/emote/%s.tga"
EMOJI_TOKEN = "|Eemote/%s|e"

# reference emoji.LIST_EMOTICON split by EMOJI_CATEGORY_GROUPS
FACES = ('angel', 'blush', 'broken', 'cheers', 'confounded', 'cool', 'cry', 'dislike', 'drooling', 'eggplant', 'exclamation', 'eyes', 'finger', 'fire', 'grin', 'heart', 'heart_eyes', 'hearteyes', 'hotface', 'hushed', 'hype', 'interro', 'kiss', 'kissing', 'like', 'lying', 'mask', 'money', 'muscle', 'peach', 'pensive', 'poop', 'rainbow', 'raised', 'relax', 'rofl', 'scream', 'shushing', 'sleepy', 'slight', 'smile', 'smirk', 'sob', 'swear', 'sweatsmile', 'think', 'tongue', 'tonguewinking', 'upside', 'vomiting', 'wink', 'yum', 'zippermouth', 'thumbsup', 'thumbsdown', 'bae', 'ryo')
CUSTOM = ('100', 'ahhaha', 'baerimalo', 'baerimblanket', 'baerimblush', 'baerimbussiness', 'baerimchill', 'baerimcoffee', 'baerimcool', 'baerimez', 'baerimflower', 'baerimfriends', 'baerimgiggle', 'baerimhug', 'baerimhype', 'baerimlost', 'baerimlove', 'baerimno', 'baerimoky', 'baerimpat', 'baerimpoint', 'baerimpog', 'baerimpolice', 'baerimpopcorn', 'baerimscared', 'baerimsleepy', 'baerimsus', 'baerimweird', 'baerimwtf', 'baerimyes', 'pepo1', 'pepo4', 'pepo5', 'pepo6', 'pepo8', 'pepo9', 'pepo10', 'pepo11', 'pepo12', 'pepo13', 'pepo14', 'pepo15', 'pepo16', 'pepo17', 'pepo18', 'pepo19', 'pepo20', 'pepo21', 'pepo22', 'pepo23', 'pepo24', 'pepo25', 'pepo26', 'pepo27', 'pepo28', 'pepo29', 'pepo30', 'pepo31', 'pepo32', 'pepo33', 'pepo34', 'pepo35', 'pepo36', 'pepo37', 'pepo38', 'pepo40', 'pepo42', 'pepo43', 'pepo45', 'pepo46', 'pepo47', 'pepo48', 'pepo49', 'pepo50', 'pepo51', 'pepo52', 'pepo53', 'pepo54', 'pepo55', 'pepo56', 'pepo57', 'pepo58', 'pepo59', 'pepo60', 'pepo61', 'pepo62', 'pepo63', 'pepo64', 'pepo65', 'pepo66', 'pepo67', 'pepo68', 'pepo69', 'pepo70', 'pepo71', 'pepo72', 'pepo75', 'pepo76', 'pepo77', 'pepo78', 'pepo79', 'pepo80', 'pepo81', 'pepo82', 'pepo83', 'pepo84', 'pepo85', 'pepo86', 'pepo87', 'pepo88', 'pepo89', 'pepo90', 'pepo91', 'pepo92', 'pepo93', 'pepo94', 'pepo95', 'pepo96', 'pepo_bad', 'pepo_bear', 'pepo_broski', 'pepo_cry_note', 'pepo_nice', 'pepo_nope', 'pepo_old', 'pepo_yes')

WIDTH = 290
HEIGHT = 262
# reference EmojiSelectorWindow / EmojiCategoryButton colours
BG_COLOR = 0xef1b120c
BORDER_COLOR = 0xff63492d
SEARCH_BG_COLOR = 0xff0e0b08
TAB_UP_COLOR = 0xff2a241f
TAB_OVER_COLOR = 0xff3a3028
TAB_DOWN_COLOR = 0xff4a3c2f
TAB_TEXT_UP_COLOR = 0xffbfae95
TAB_TEXT_DOWN_COLOR = 0xffe8d8bf
HEADER_COLOR = 0xffbfae95

CONTENT_X = 10
CONTENT_Y = 62
CONTENT_W = 258
CONTENT_H = 192
CELL = 26
COLUMNS = 9
EMOJI_SCALE = 1.2			# 18px tga -> ~22px like the reference grid
HEADER_HEIGHT = 22
RECENT_LIMIT = 18

CATEGORY_ORDER = ("recent", "faces", "custom")
CATEGORY_LABEL = {"recent" : "Recently Used", "faces" : "Faces", "custom" : "Custom"}


def _Bar(parent, x, y, w, h, color):
	bar = ui.Bar()
	bar.SetParent(parent)
	bar.SetPosition(int(x), int(y))
	bar.SetSize(int(w), int(h))
	bar.SetColor(color)
	bar.AddFlag("not_pick")
	bar.Show()
	return bar


class CategoryButton(ui.Bar):
	def __init__(self, label, event):
		ui.Bar.__init__(self)
		self.event = event
		self.selected = FALSE
		self.over = FALSE
		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetText(label)
		self.text.SetHorizontalAlignCenter()
		self.text.AddFlag("not_pick")
		self.text.Show()
		self.__Refresh()

	def __del__(self):
		ui.Bar.__del__(self)

	def SetSize(self, width, height):
		ui.Bar.SetSize(self, width, height)
		self.text.SetPosition(int(width / 2), int((height - 12) / 2))

	def SetSelected(self, selected):
		self.selected = selected
		self.__Refresh()

	def __Refresh(self):
		if self.selected:
			self.SetColor(TAB_DOWN_COLOR)
			self.text.SetPackedFontColor(TAB_TEXT_DOWN_COLOR)
		else:
			self.SetColor(TAB_OVER_COLOR if self.over else TAB_UP_COLOR)
			self.text.SetPackedFontColor(TAB_TEXT_UP_COLOR)

	def OnMouseOverIn(self):
		self.over = TRUE
		self.__Refresh()

	def OnMouseOverOut(self):
		self.over = FALSE
		self.__Refresh()

	def OnMouseLeftButtonDown(self):	# plain bar: the button-up never reaches it
		if self.event:
			self.event()
		return TRUE


class EmojiButton(ui.Button):
	def __init__(self, name, event):
		ui.Button.__init__(self)
		self.name = name
		self.event = event
		self.SetSize(CELL, CELL)
		self.image = ui.ExpandedImageBox()
		self.image.SetParent(self)
		self.image.LoadImage(EMOTE_PATH % name)
		self.image.SetScale(EMOJI_SCALE, EMOJI_SCALE)
		self.image.SetPosition(int((CELL - 18 * EMOJI_SCALE) / 2), int((CELL - 18 * EMOJI_SCALE) / 2))
		self.image.AddFlag("not_pick")
		self.image.Show()
		self.SetEvent(ui.__mem_func__(self.__OnPick))

	def __del__(self):
		ui.Button.__del__(self)

	def __OnPick(self):
		if self.event:
			self.event(self.name)


class EmojiSelectorWindow(ui.Window):
	_recentUsed = []

	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		self.chatLine = None
		self.selectionEvent = None	# reference SetSelectionEvent: a pick goes there instead of the chat line
		self.rows = []			# (window, y, height)
		self.sectionTop = {}
		self.totalHeight = 0
		self.__Build()

	def __del__(self):
		ui.Window.__del__(self)

	def __Build(self):
		self.SetSize(WIDTH, HEIGHT)
		self.AddFlag("float")
		self.bg = _Bar(self, 0, 0, WIDTH, HEIGHT, BG_COLOR)
		self.border = [_Bar(self, 0, 0, WIDTH, 1, BORDER_COLOR), _Bar(self, 0, HEIGHT - 1, WIDTH, 1, BORDER_COLOR),
			_Bar(self, 0, 0, 1, HEIGHT, BORDER_COLOR), _Bar(self, WIDTH - 1, 0, 1, HEIGHT, BORDER_COLOR)]

		self.tabs = {}
		tabWidth = 80
		for (index, key) in enumerate(CATEGORY_ORDER):
			tab = CategoryButton(CATEGORY_LABEL[key], lambda arg = key: self.__OnTabClick(arg))
			tab.SetParent(self)
			tab.SetPosition(8 + index * (tabWidth + 4), 6)
			tab.SetSize(tabWidth, 22)
			tab.Show()
			self.tabs[key] = tab

		self.closeButton = ui.Button()
		self.closeButton.SetParent(self)
		self.closeButton.SetUpVisual("d:/ymir work/ui/game/whisper/close_btn_norm.png")
		self.closeButton.SetOverVisual("d:/ymir work/ui/game/whisper/close_btn_hover.png")
		self.closeButton.SetDownVisual("d:/ymir work/ui/game/whisper/close_btn_select.png")
		self.closeButton.SetPosition(WIDTH - 22, 10)
		self.closeButton.SetEvent(ui.__mem_func__(self.Hide))
		self.closeButton.Show()

		self.searchBg = _Bar(self, 8, 33, WIDTH - 16, 22, SEARCH_BG_COLOR)
		self.searchBorder = [_Bar(self, 8, 33, WIDTH - 16, 1, BORDER_COLOR), _Bar(self, 8, 54, WIDTH - 16, 1, BORDER_COLOR),
			_Bar(self, 8, 33, 1, 22, BORDER_COLOR), _Bar(self, WIDTH - 9, 33, 1, 22, BORDER_COLOR)]
		self.searchIcon = ui.ImageBox()
		self.searchIcon.SetParent(self)
		self.searchIcon.LoadImage("d:/ymir work/ui/game/whisper/btn_search_norm.png")
		self.searchIcon.SetPosition(10, 34)
		self.searchIcon.AddFlag("not_pick")
		self.searchIcon.Show()
		self.searchLine = ui.EditLine()
		self.searchLine.SetParent(self)
		self.searchLine.SetPosition(38, 38)
		self.searchLine.SetSize(WIDTH - 54, 16)
		self.searchLine.SetMax(20)
		self.searchLine.SetOverlayText("Search...")
		self.searchLine.SetUpdateEvent(ui.__mem_func__(self.__RebuildList))
		self.searchLine.SetEscapeEvent(ui.__mem_func__(self.Hide))
		self.searchLine.Show()

		self.content = ui.Window()
		self.content.SetParent(self)
		self.content.SetPosition(CONTENT_X, CONTENT_Y)
		self.content.SetSize(CONTENT_W, CONTENT_H)
		self.content.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		self.content.Show()

		self.scrollBar = uiCharacterDetails.BaerimScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(WIDTH - 16, CONTENT_Y)
		self.scrollBar.SetScrollBarSize(CONTENT_H)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__Place))
		self.scrollBar.Show()

	def Destroy(self):
		self.rows = []
		self.chatLine = None

	def SetChatLine(self, chatLine):
		self.chatLine = chatLine

	def SetSelectionEvent(self, event):
		self.selectionEvent = event

	def Toggle(self, x, y):
		if self.IsShow():
			self.Hide()
			return
		self.SetPosition(int(x), int(y))
		self.searchLine.SetText("")
		self.__RebuildList()
		self.__SelectTab("recent")
		self.Show()
		self.SetTop()

	def IsEmojiName(self, name):
		return name in FACES or name in CUSTOM

	def Hide(self):
		# ui.Window.__init__ calls Hide() before __Build created the search line
		searchLine = getattr(self, "searchLine", None)
		if searchLine:
			searchLine.KillFocus()
		ui.Window.Hide(self)

	def OnPressEscapeKey(self):
		self.Hide()
		return TRUE

	# ------------------------------------------------------------------ list
	def __Sections(self):
		query = self.searchLine.GetText().lower()
		sections = (("recent", list(EmojiSelectorWindow._recentUsed)), ("faces", list(FACES)), ("custom", list(CUSTOM)))
		if query:
			sections = [(key, [name for name in names if query in name.lower()]) for (key, names) in sections]
		# reference keeps the "Recently Used" header even while it is empty
		return [(key, names) for (key, names) in sections if names or (key == "recent" and not query)]

	def __MakeHeader(self, key):
		header = ui.Window()
		header.SetParent(self.content)
		header.SetSize(CONTENT_W, HEADER_HEIGHT)
		header.AddFlag("not_pick")
		title = ui.TextLine()
		title.SetParent(header)
		title.SetText(CATEGORY_LABEL[key])
		title.SetPackedFontColor(HEADER_COLOR)
		title.SetHorizontalAlignCenter()
		title.SetPosition(int(CONTENT_W / 2), 4)
		title.Show()
		lineWidth = int(CONTENT_W / 2) - 55
		header.children = [title, _Bar(header, 4, 11, lineWidth, 1, BORDER_COLOR), _Bar(header, CONTENT_W - 4 - lineWidth, 11, lineWidth, 1, BORDER_COLOR)]
		return header

	def __RebuildList(self):
		for (window, y, height) in self.rows:
			window.Hide()
		self.rows = []
		self.sectionTop = {}

		y = 0
		for (key, names) in self.__Sections():
			self.sectionTop[key] = y
			self.rows.append((self.__MakeHeader(key), y, HEADER_HEIGHT))
			y += HEADER_HEIGHT

			for start in xrange(0, len(names), COLUMNS):
				row = ui.Window()
				row.SetParent(self.content)
				row.SetSize(CONTENT_W, CELL)
				row.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
				row.children = []
				for (column, name) in enumerate(names[start:start + COLUMNS]):
					button = EmojiButton(name, ui.__mem_func__(self.__OnPick))
					button.SetParent(row)
					button.SetPosition(column * (CELL + 2), 0)
					button.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
					button.Show()
					row.children.append(button)
				self.rows.append((row, y, CELL))
				y += CELL + 2
			y += 4

		self.totalHeight = y
		self.scrollBar.SetPos(0)
		self.__Place()

	def __Place(self):
		overflow = max(0, self.totalHeight - CONTENT_H)
		offset = int(overflow * self.scrollBar.GetPos())
		for (window, y, height) in self.rows:
			top = y - offset
			if top >= 0 and top + height <= CONTENT_H:
				window.SetPosition(0, int(top))
				window.Show()
			else:
				window.Hide()

	def OnRunMouseWheel(self, nLen):	# wheel arrives at the root window of the picked child
		self.OnMouseWheel(nLen)
		return True

	def OnMouseWheel(self, length):
		overflow = max(1, self.totalHeight - CONTENT_H)
		step = float(CELL * 2) / overflow
		self.scrollBar.SetPos(max(0.0, min(1.0, self.scrollBar.GetPos() + (-step if length > 0 else step))))
		self.__Place()
		return TRUE

	def __SelectTab(self, key):
		for (tabKey, tab) in self.tabs.items():
			tab.SetSelected(tabKey == key)

	def __OnTabClick(self, key):
		self.__SelectTab(key)
		if self.sectionTop.has_key(key):
			overflow = max(1, self.totalHeight - CONTENT_H)
			self.scrollBar.SetPos(min(1.0, float(self.sectionTop[key]) / overflow))
			self.__Place()

	def __OnPick(self, name):
		if name in EmojiSelectorWindow._recentUsed:
			EmojiSelectorWindow._recentUsed.remove(name)
		EmojiSelectorWindow._recentUsed.insert(0, name)
		del EmojiSelectorWindow._recentUsed[RECENT_LIMIT:]
		if self.selectionEvent:
			event = self.selectionEvent
			self.selectionEvent = None
			self.Hide()
			event(name)
			return
		if self.chatLine:
			self.chatLine.SetText(self.chatLine.GetText() + EMOJI_TOKEN % name)
			self.chatLine.SetFocus()
			if hasattr(self.chatLine, "SetEndPosition"):
				self.chatLine.SetEndPosition()
