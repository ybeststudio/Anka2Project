if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui
import snd
import wndMgr
import localeInfo

# Baerim uiCountryFlag (Cython) rebuilt from its runtime dump: sizes, colours and the code list are theirs.
FLAG_PATH = "d:/ymir work/ui/language_system/flags/%s.png"	# 33x20, Vegas' format (+ Baerim countries adapted)

COUNTRY_FLAGS = (
	"af", "al", "dz", "ad", "ao", "ar", "am", "au", "at", "az", "bs", "bh", "bd", "bb", "by", "be", "bz", "bj", "bt", "ba",
	"bw", "br", "bg", "bi", "kh", "cm", "ca", "td", "cl", "cn", "co", "km", "cg", "hr", "cu", "cy", "cz", "dj", "ec", "eg",
	"er", "et", "fi", "fr", "ga", "gm", "ge", "de", "gh", "gi", "gr", "gl", "gu", "gt", "gn", "gw", "gy", "rr", "ht", "hn",
	"hu", "is", "in", "id", "iq", "ie", "il", "it", "jm", "jp", "je", "jo", "kz", "ke", "kw", "kg", "lv", "lb", "ls", "lr",
	"ly", "li", "lt", "lu", "mg", "mw", "mv", "ml", "mt", "mr", "mu", "yt", "mx", "mc", "mn", "ms", "ma", "mz", "na", "nr",
	"np", "nl", "ni", "ne", "ng", "nu", "no", "om", "pk", "pw", "pa", "py", "pe", "ph", "pl", "pt", "qa", "ro", "ru", "rw",
	"ws", "sa", "sn", "rs", "sc", "sg", "sk", "si", "so", "es", "sd", "sr", "se", "ch", "tj", "th", "tl", "tg", "to", "tn",
	"tr", "tm", "ug", "ua", "ae", "gb", "uy", "uz", "vu", "vn", "ye", "zm", "zw",
)

BG_COLOR = 0xff171411
LINE_COLOR = 0xff3b332a
OVER_COLOR = 0x66594c38
SELECTED_BAR_COLOR = 0x7f384c66
SELECTED_COLOR = 0xff8cc6ff
TEXT_COLOR = 0xffe5e5e5
TEXT_OVER_COLOR = 0xffffe0a5
# measured on Baerim's screen
SCROLL_LINE_COLOR = 0xff565656
SCROLL_TRACK_COLOR = 0xff000000

def GetCountryName(code):
	return getattr(localeInfo, "COUNTRY_FLAG_" + code.upper(), code.upper())

class CountryRow(ui.Window):
	FLAG_WIDTH = 33
	FLAG_HEIGHT = 20

	def __init__(self, owner, code, width, height):
		ui.Window.__init__(self)
		self.owner = owner
		self.code = code
		self.isSelected = FALSE
		self.isOver = FALSE
		self.SetSize(width, height)

		self.bar = ui.Bar()
		self.bar.SetParent(self)
		self.bar.SetSize(width, height)
		self.bar.AddFlag("not_pick")
		self.bar.Show()

		self.flag = ui.ImageBox()
		self.flag.SetParent(self)
		self.flag.AddFlag("not_pick")
		self.flag.LoadImage(FLAG_PATH % code)
		self.flag.SetPosition(4, (height - self.FLAG_HEIGHT) / 2)
		self.flag.Show()

		self.name = ui.TextLine()
		self.name.SetParent(self)
		self.name.SetPosition(self.FLAG_WIDTH + 10, height / 2 - 7)
		self.name.SetText(GetCountryName(code))
		self.name.Show()

		self.__Refresh()

	def __del__(self):
		ui.Window.__del__(self)

	def SetSelected(self, isSelected):
		self.isSelected = isSelected
		self.__Refresh()

	def SetHighlight(self, isOver):
		self.isOver = isOver
		self.__Refresh()

	def __Refresh(self):
		if self.isSelected:
			self.bar.SetColor(SELECTED_BAR_COLOR)
			self.name.SetPackedFontColor(SELECTED_COLOR)
		elif self.isOver:
			self.bar.SetColor(OVER_COLOR)
			self.name.SetPackedFontColor(TEXT_OVER_COLOR)
		else:
			self.bar.SetColor(0x00000000)
			self.name.SetPackedFontColor(TEXT_COLOR)

	def OnMouseOverIn(self):
		self.owner.SetHover(self)

	def OnMouseOverOut(self):
		self.owner.SetHover(None)

	def OnMouseLeftButtonUp(self):
		self.owner.OnSelect(self.code)

class CountryFlagDropdown(ui.Window):
	WIDTH = 163
	ROW_HEIGHT = 26
	MAX_VISIBLE = 6
	SEARCH_HEIGHT = 25
	SCROLLBAR_WIDTH = 6

	def __init__(self):
		ui.Window.__init__(self, "TOP_MOST")
		self.event = None
		self.closeEvent = None
		self.current = ""
		self.matches = list(COUNTRY_FLAGS)
		self.rows = []
		self.offset = 0
		self.hover = None

		self.SetSize(self.WIDTH, self.SEARCH_HEIGHT + self.ROW_HEIGHT * self.MAX_VISIBLE + 2)

		self.border = ui.Bar()
		self.border.SetParent(self)
		self.border.SetColor(LINE_COLOR)
		self.border.SetSize(self.GetWidth(), self.GetHeight())
		self.border.AddFlag("not_pick")
		self.border.Show()

		self.background = ui.Bar()
		self.background.SetParent(self)
		self.background.SetColor(BG_COLOR)
		self.background.SetPosition(1, 1)
		self.background.SetSize(self.GetWidth() - 2, self.GetHeight() - 2)
		self.background.AddFlag("not_pick")
		self.background.Show()

		# sunken black search field
		self.searchSlot = ui.SlotBar()
		self.searchSlot.SetParent(self)
		self.searchSlot.SetPosition(6, 4)
		self.searchSlot.SetSize(self.WIDTH - 12, self.SEARCH_HEIGHT - 6)
		self.searchSlot.Show()

		self.search = ui.EditLine()
		self.search.SetParent(self.searchSlot)
		self.search.SetPosition(4, 3)
		self.search.SetSize(self.WIDTH - 20, 14)
		self.search.SetMax(24)
		self.search.SetInfoMessage(localeInfo.SELECT_COUNTRY_FLAG_SEARCH)
		self.search.SetUpdateEvent(ui.__mem_func__(self.OnSearchUpdate))
		self.search.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.search.SetReturnEvent(ui.__mem_func__(self.SelectFirst))
		self.search.Show()

		viewH = self.ROW_HEIGHT * self.MAX_VISIBLE
		self.scrollTrack = ui.Bar()
		self.scrollTrack.SetParent(self)
		self.scrollTrack.SetColor(SCROLL_LINE_COLOR)
		self.scrollTrack.SetPosition(self.WIDTH - self.SCROLLBAR_WIDTH - 2, self.SEARCH_HEIGHT + 1)
		self.scrollTrack.SetSize(self.SCROLLBAR_WIDTH, viewH - 1)
		self.scrollTrack.AddFlag("not_pick")

		self.scrollTrackInner = ui.Bar()
		self.scrollTrackInner.SetParent(self.scrollTrack)
		self.scrollTrackInner.SetColor(SCROLL_TRACK_COLOR)
		self.scrollTrackInner.SetPosition(1, 1)
		self.scrollTrackInner.SetSize(self.SCROLLBAR_WIDTH - 2, viewH - 3)
		self.scrollTrackInner.AddFlag("not_pick")
		self.scrollTrackInner.Show()

		self.scrollBar = ui.Bar()
		self.scrollBar.SetParent(self.scrollTrack)
		self.scrollBar.SetColor(SCROLL_LINE_COLOR)
		self.scrollBar.AddFlag("not_pick")

		self.__BuildRows()
		self.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.event = None
		self.closeEvent = None
		self.hover = None
		self.rows = []
		self.Hide()

	def SetEvent(self, event):
		self.event = event

	def SetCloseEvent(self, event):
		self.closeEvent = event

	def SetCurrent(self, code):
		self.current = code

	def Open(self, x, y):
		self.SetPosition(x, y)
		self.search.SetText("")
		self.matches = list(COUNTRY_FLAGS)
		self.offset = 0
		if self.current in self.matches:
			self.offset = max(0, min(self.matches.index(self.current), len(self.matches) - self.MAX_VISIBLE))
		self.__RefreshRows()
		self.Show()
		self.SetTop()
		self.search.SetFocus()

	def Close(self):
		self.search.KillFocus()
		self.Hide()
		if self.closeEvent:
			self.closeEvent()

	def IsOpened(self):
		return self.IsShow()

	def __BuildRows(self):
		for i in xrange(self.MAX_VISIBLE):
			row = None
			self.rows.append(row)

	def __RefreshRows(self):
		rowWidth = self.WIDTH - 2 - (self.SCROLLBAR_WIDTH + 2 if len(self.matches) > self.MAX_VISIBLE else 0)
		for i in xrange(self.MAX_VISIBLE):
			index = self.offset + i
			if self.rows[i]:
				self.rows[i].Hide()
				self.rows[i] = None

			if index >= len(self.matches):
				continue

			row = CountryRow(self, self.matches[index], rowWidth, self.ROW_HEIGHT)
			row.SetParent(self)
			row.SetPosition(1, self.SEARCH_HEIGHT + 1 + i * self.ROW_HEIGHT)
			row.SetSelected(self.matches[index] == self.current)
			row.Show()
			self.rows[i] = row

		self.hover = None
		self.__UpdateScrollbar()

	def __UpdateScrollbar(self):
		total = len(self.matches)
		if total <= self.MAX_VISIBLE:
			self.scrollTrack.Hide()
			return

		trackH = self.scrollTrack.GetHeight() - 2
		barH = max(12, trackH * self.MAX_VISIBLE / total)
		y = (trackH - barH) * self.offset / (total - self.MAX_VISIBLE)
		self.scrollBar.SetPosition(1, 1 + y)
		self.scrollBar.SetSize(self.SCROLLBAR_WIDTH - 2, barH)
		self.scrollBar.Show()
		self.scrollTrack.Show()

	def OnSearchUpdate(self):
		text = self.search.GetText().strip().lower()
		if not text:
			self.matches = list(COUNTRY_FLAGS)
		else:
			# names starting with the text first, then the ones containing it (Baerim UpdateResults)
			prefix = [code for code in COUNTRY_FLAGS if GetCountryName(code).lower().startswith(text) or code == text]
			contains = [code for code in COUNTRY_FLAGS if code not in prefix and text in GetCountryName(code).lower()]
			self.matches = prefix + contains
		self.offset = 0
		self.__RefreshRows()

	def SelectFirst(self):
		if self.matches:
			self.OnSelect(self.matches[0])

	def SetHover(self, row):
		if self.hover and self.hover != row:
			self.hover.SetHighlight(FALSE)
		self.hover = row
		if row:
			row.SetHighlight(TRUE)

	def OnSelect(self, code):
		snd.PlaySound("sound/ui/click.wav")
		self.current = code
		self.Close()
		if self.event:
			self.event(code)

	def Scroll(self, length):
		maxOffset = max(0, len(self.matches) - self.MAX_VISIBLE)
		step = -1 if length > 0 else 1
		newOffset = max(0, min(maxOffset, self.offset + step))
		if newOffset != self.offset:
			self.offset = newOffset
			self.__RefreshRows()
		return TRUE

	def OnRunMouseWheel(self, length):
		return self.Scroll(length)

	def IsMouseIn(self):
		(x, y) = self.GetGlobalPosition()
		(mx, my) = wndMgr.GetMousePosition()
		return x <= mx < x + self.GetWidth() and y <= my < y + self.GetHeight()
