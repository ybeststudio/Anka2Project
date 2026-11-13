if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))
import wndMgr

import ui
import uiToolTip
import localeInfo
import chat
import teleportData
import uiDungeonTrack
if app.ENABLE_MOKOKO_SYSTEM:
	import uiMokokoRewards

# Baerim uiTeleport map page (Cython) rebuilt from its runtime dump (Reference/pydump_runtime/dungeonprobe: consts_uiTeleport.json
# "data", full_TeleportWindow_*.json layout) and its client tests (baerim_unpack/out_live/test/test_teleport_map_client.py):
# continent pages with hover regions, page arrows / bullets, a dragged page that follows the mouse (swipe), wheel paging and the
# SelectTeleportation list of the clicked region. The server owns every point (/teleport_map <tpIdx>, teleport_points.txt).

WARP_PATH = "d:/ymir work/ui/game/teleport_window/warp/%s"
DELETE_SELL_PATH = "d:/ymir work/ui/game/delete_sell/%s"
NEW_THINBOARD_PATH = "d:/ymir work/ui/pattern/new_thinboard/%s"

PAGE_WIDTH = 674
PAGE_HEIGHT = 529

MAP_PAGE_CLICK_TOLERANCE = 10		# Baerim consts
MAP_PAGE_SWIPE_DISTANCE = 60
MAP_PAGE_SLIDE_DURATION = 0.35
MAP_PAGE_SLIDE_DISTANCE = 24
MAP_PAGE_EDGE_RESISTANCE = 0.15		# test: a 100 px pull past the first page moves it 15 px

LIST_BUTTON_STEP = 30
PREVIEW_IMAGE_SIZE = 208			# Baerim point preview: the atlas of the map in a 220 px frame right of the list
PREVIEW_PADDING = 6
PREVIEW_MARKER = "d:/ymir work/ui/minimap/mini_waypoint%02d.sub"
PREVIEW_MARKER_SIZE = 30
MOKOKO_TOTAL = 530					# live Baerim counter "0 / 530" (the Mokoko system sends the real numbers)
MOKOKO_COUNTER_BG = "ikashop/lite/business/counter.png"	# Baerim mokokoRewardCounterBg: pending rewards bubble on the button
LIST_HEIGHT = 228
TOOLTIP_TEXT_COLOR = 0xffc1c1c1
TOOLTIP_TITLE_COLOR = 0xfff3e9c8
TOOLTIP_LOCKED_COLOR = 0xffff6060
POINT_LOCKED_COLOR = 0xff8a7f72

RESULT_OK = 0
RESULT_TEXT = {
	1 : "TELEPORT_MAP_RESULT_INVALID",
	2 : "TELEPORT_MAP_RESULT_LEVEL",
	3 : "TELEPORT_MAP_RESULT_GOLD",
	4 : "TELEPORT_MAP_RESULT_BUSY",
	5 : "TELEPORT_MAP_RESULT_NOT_AVAILABLE",
	6 : "TELEPORT_MAP_RESULT_COOLDOWN",
}

def ShowResult(result, tpIdx):
	key = RESULT_TEXT.get(result)
	if key and hasattr(localeInfo, key):
		chat.AppendChat(chat.CHAT_TYPE_INFO, getattr(localeInfo, key))

def _Text(key, default = ""):
	return getattr(localeInfo, key, default)

def MapName(regionId):
	return _Text("TELEPORT_MAP_%d" % regionId, str(regionId))

def PointName(tpIdx):
	return _Text("TELEPORT_POINT_%d" % tpIdx, str(tpIdx))

def _Ease(t):
	t = max(0.0, min(1.0, t))
	return 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t)	# ease out


class NewThinBoard(ui.Window):
	# Baerim ThinBoardNew: new_thinboard corners / lines around a tiled base
	CORNER = 16

	def __init__(self):
		ui.Window.__init__(self)
		self.corners = []
		self.lines = []
		for name in ("thinboard_corner_lefttop", "thinboard_corner_leftbottom", "thinboard_corner_righttop", "thinboard_corner_rightbottom"):
			self.corners.append(self.__Image(NEW_THINBOARD_PATH % (name + ".png")))
		for name in ("thinboard_line_left", "thinboard_line_right", "thinboard_line_top", "thinboard_line_bottom"):
			self.lines.append(self.__Image(NEW_THINBOARD_PATH % (name + ".png")))
		self.base = self.__Image(NEW_THINBOARD_PATH % "base.png")
		self.base.SetPosition(self.CORNER, self.CORNER)

	def __del__(self):
		ui.Window.__del__(self)

	def __Image(self, name):
		image = ui.ExpandedImageBox()
		image.SetParent(self)
		image.AddFlag("not_pick")
		image.LoadImage(name)
		image.Show()
		return image

	def SetSize(self, width, height):
		c = self.CORNER
		ui.Window.SetSize(self, width, height)
		self.corners[1].SetPosition(0, height - c)
		self.corners[2].SetPosition(width - c, 0)
		self.corners[3].SetPosition(width - c, height - c)
		self.lines[0].SetPosition(0, c)
		self.lines[1].SetPosition(width - c, c)
		self.lines[2].SetPosition(c, 0)
		self.lines[3].SetPosition(c, height - c)
		vertical = float(height - c * 2 - c) / c
		horizontal = float(width - c * 2 - c) / c
		self.lines[0].SetRenderingRect(0, 0, 0, vertical)
		self.lines[1].SetRenderingRect(0, 0, 0, vertical)
		self.lines[2].SetRenderingRect(0, 0, horizontal, 0)
		self.lines[3].SetRenderingRect(0, 0, horizontal, 0)
		self.base.SetRenderingRect(0, 0, horizontal, vertical)


class MapPreview(ui.Window):
	# the map of the hovered point with a marker on it
	def __init__(self):
		ui.Window.__init__(self)
		self.AddFlag("float")
		self.AddFlag("not_pick")
		self.frame = NewThinBoard()
		self.frame.SetParent(self)
		self.frame.AddFlag("not_pick")
		self.frame.Show()
		self.image = ui.ExpandedImageBox()
		self.image.SetParent(self)
		self.image.AddFlag("not_pick")
		self.image.SetPosition(PREVIEW_PADDING, PREVIEW_PADDING)
		self.image.Show()
		# Baerim: the blinking minimap waypoint ring (mini_waypoint01..07, 30x30)
		self.marker = ui.AniImageBox()
		self.marker.SetParent(self)
		self.marker.AddFlag("not_pick")
		for frame in xrange(1, 8):	# 01..07 are the ring; 08 is a separate 10x10 red dot of minimap.dds
			self.marker.AppendImage(PREVIEW_MARKER % frame)
		self.marker.SetDelay(6)
		self.marker.Show()
		self.imageName = ""

	def __del__(self):
		ui.Window.__del__(self)

	def Open(self, point, x, y):
		(atlas, fx, fy, ratio, nativeW, nativeH) = point[4:10]
		if not atlas:
			self.Hide()
			return
		if atlas != self.imageName:
			self.image.LoadImage(atlas)
			self.imageName = atlas
		if ratio <= 1.0:
			width = PREVIEW_IMAGE_SIZE
			height = int(PREVIEW_IMAGE_SIZE * ratio)
		else:
			height = PREVIEW_IMAGE_SIZE
			width = int(PREVIEW_IMAGE_SIZE / ratio)
		# the .sub size from the data: GetWidth() is 0 until the atlas texture is loaded
		nativeW = max(1, nativeW)
		nativeH = max(1, nativeH)
		self.image.SetScale(float(width) / nativeW, float(height) / nativeH)
		self.SetSize(width + PREVIEW_PADDING * 2, height + PREVIEW_PADDING * 2)
		self.frame.SetSize(width + PREVIEW_PADDING * 2, height + PREVIEW_PADDING * 2)
		self.marker.SetPosition(PREVIEW_PADDING + int(width * fx) - PREVIEW_MARKER_SIZE / 2, PREVIEW_PADDING + int(height * fy) - PREVIEW_MARKER_SIZE / 2)
		self.SetPosition(x, y)
		self.Show()
		self.SetTop()


class SelectTeleportation(ui.Window):
	# the points of the clicked region (Baerim SelectTeleportation 250x300, measured on the live Baerim: classic board,
	# "Select the locations:" y14, cost line y33, close (220,13), 188x26 buttons at (32,48) every 30 px, scrollbar (230,40))
	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = owner
		self.buttons = []
		self.scroll = 0
		self.entered = False
		self.SetSize(250, 300)
		self.AddFlag("float")		# Baerim: the list stays where it opened (not movable)

		self.board = ui.Board()
		self.board.SetParent(self)
		self.board.SetSize(250, 300)
		self.board.AddFlag("not_pick")
		self.board.Show()

		# Baerim: a thin bordered panel inside the board (ThinBoardNew 230x280 at 10,10)
		self.panel = NewThinBoard()
		self.panel.SetParent(self)
		self.panel.SetPosition(10, 10)
		self.panel.SetSize(230, 280)
		self.panel.AddFlag("not_pick")
		self.panel.Show()

		self.preview = MapPreview()

		self.title = ui.TextLine()
		self.title.SetParent(self)
		self.title.SetFontName("Tahoma:14b")
		self.title.SetPosition(125, 14)
		self.title.SetHorizontalAlignCenter()
		self.title.SetPackedFontColor(TOOLTIP_TITLE_COLOR)
		self.title.Show()

		self.price = ui.TextLine()
		self.price.SetParent(self)
		self.price.SetPosition(125, 33)
		self.price.SetHorizontalAlignCenter()
		self.price.Show()

		self.closeButton = ui.Button()
		self.closeButton.SetParent(self)
		self.closeButton.SetPosition(220, 13)
		self.closeButton.SetUpVisual(DELETE_SELL_PATH % "btn_quit_norm.png")
		self.closeButton.SetOverVisual(DELETE_SELL_PATH % "btn_quit_hover.png")
		self.closeButton.SetDownVisual(DELETE_SELL_PATH % "btn_quit_select.png")
		self.closeButton.SetEvent(ui.__mem_func__(self.Close))
		self.closeButton.Show()

		self.listWindow = ui.Window()
		self.listWindow.SetParent(self)
		self.listWindow.SetPosition(32, 48)
		self.listWindow.SetSize(188, LIST_HEIGHT)
		self.listWindow.SetInsideRender(True)
		self.listWindow.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheel))
		self.listWindow.Show()

		self.scrollBar = uiDungeonTrack.ScrollBarSpecial()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(230, 40)
		self.scrollBar.SetSize(7, LIST_HEIGHT)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBar))
		self.scrollBar.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.owner = None
		self.buttons = []
		if self.preview:
			self.preview.Hide()
			self.preview = None

	def Open(self, region):
		self.region = region
		self.title.SetText(localeInfo.TELEPORT_SELECT_LOCATIONS)
		yang = max([point[3] for point in region["points"]] + [0])
		self.price.SetText(localeInfo.TELEPORT_MAP_PRICE % localeInfo.NumberToMoneyStringNEW(yang))

		for button in self.buttons:
			button.Hide()
		self.buttons = []
		level = player.GetStatus(player.LEVEL)
		for point in region["points"]:
			(tpIdx, minLevel, maxLevel, yang) = point[:4]
			button = ui.Button()
			button.SetParent(self.listWindow)
			button.SetUpVisual(WARP_PATH % "btn_0.png")
			button.SetOverVisual(WARP_PATH % "btn_1.png")
			button.SetDownVisual(WARP_PATH % "btn_2.png")
			button.SetText(PointName(tpIdx))
			if level < minLevel or (maxLevel and level > maxLevel):
				button.SetTextColor(POINT_LOCKED_COLOR)
			button.SetEvent(ui.__mem_func__(self.__Click), tpIdx, minLevel, maxLevel)
			button.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheel))
			if app.ENABLE_CLIP_MASKING:
				button.SetClippingMaskWindow(self.listWindow)	# rows half out of the list are cut, not drawn over the title
			button.SetShowToolTipEvent(ui.__mem_func__(self.__ShowPreview), point)
			button.SetHideToolTipEvent(ui.__mem_func__(self.__HidePreview))
			self.buttons.append(button)
		self.scroll = 0
		self.entered = False
		if len(self.buttons) * LIST_BUTTON_STEP > LIST_HEIGHT:
			total = len(self.buttons) * LIST_BUTTON_STEP
			self.scrollBar.SetScale(LIST_HEIGHT, total)
			# thumb size / position as integers: the engine ignores the floats SetScale / SetPosScale pass
			self.scrollBar.SetStaticScale(max(12, int(LIST_HEIGHT * LIST_HEIGHT / total)))
			self.scrollBar.SetPos(1)
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()
		self.__Layout()
		self.SetTop()
		self.Show()

	def __Layout(self):
		for (i, button) in enumerate(self.buttons):
			y = i * LIST_BUTTON_STEP - self.scroll
			button.SetPosition(0, y)
			if y <= -LIST_BUTTON_STEP or y >= LIST_HEIGHT:
				button.Hide()
			else:
				button.Show()

	def __OnWheel(self, isDown):
		overflow = max(0, len(self.buttons) * LIST_BUTTON_STEP - LIST_HEIGHT)
		self.scroll = max(0, min(overflow, self.scroll + (LIST_BUTTON_STEP if isDown else -LIST_BUTTON_STEP)))
		if overflow:
			track = LIST_HEIGHT - 2 - self.scrollBar.middleBar.GetHeight()
			self.scrollBar.SetPos(int(1 + track * float(self.scroll) / overflow))
		self.__Layout()

	def __OnScrollBar(self):
		overflow = max(0, len(self.buttons) * LIST_BUTTON_STEP - LIST_HEIGHT)
		self.scroll = int(self.scrollBar.GetPos() * overflow)
		self.__Layout()

	def OnMouseWheel(self, nLen):
		self.__OnWheel(nLen < 0)
		return True

	def OnRunMouseWheel(self, nLen):	# the engine sends the wheel as OnRunMouseWheel
		return self.OnMouseWheel(nLen)

	def OnUpdate(self):
		# Baerim: the list closes when the mouse, once inside, leaves it
		(x, y) = wndMgr.GetMousePosition()
		(left, top) = self.GetGlobalPosition()
		inside = left - 8 <= x < left + self.GetWidth() + 8 and top - 8 <= y < top + self.GetHeight() + 8
		if inside:
			self.entered = True
		elif self.entered:
			self.Close()

	def __Click(self, tpIdx, minLevel, maxLevel):
		level = player.GetStatus(player.LEVEL)
		if level < minLevel:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_NO_REQUIRED_LEVEL.format(minLevel))
			return
		if maxLevel and level > maxLevel:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_MAP_RESULT_LEVEL)
			return
		net.SendChatPacket("/teleport_map %d" % tpIdx)
		self.Close()

	def __ShowPreview(self, point):
		if self.preview:
			(x, y) = self.GetGlobalPosition()
			self.preview.Open(point, x + self.GetWidth() + 6, y + 31)

	def __HidePreview(self):
		if self.preview:
			self.preview.Hide()

	def Close(self):
		self.__HidePreview()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True


class _PageLayer(object):
	# the regions / labels / frame of one page: direct children of the map page (the engine clips only direct children
	# correctly) moved together with their page background (base) and the arrow slide offset (pos)
	def __init__(self):
		self.items = []
		self.pos = (0, 0)
		self.base = 0

	def Add(self, window, x, y):
		self.items.append((window, x, y))
		window.SetPosition(self.base + self.pos[0] + x, self.pos[1] + y)

	def SetPosition(self, x, y):
		self.pos = (x, y)
		self.__Apply()

	def SetBase(self, x):
		self.base = x
		self.__Apply()

	def __Apply(self):
		for (window, x, y) in self.items:
			window.SetPosition(self.base + self.pos[0] + x, self.pos[1] + y)

	def Show(self):
		for (window, x, y) in self.items:
			window.Show()

	def Hide(self):
		for (window, x, y) in self.items:
			window.Hide()


class TeleportMapPage(ui.Window):
	# the map tab of the dungeon window (Baerim teleportBg: board (8,30) 674x529)
	def __init__(self, owner):
		ui.Window.__init__(self)
		self.owner = owner
		self.SetSize(PAGE_WIDTH, PAGE_HEIGHT)
		self.pageBg = []
		self.pageContent = []
		self.regions = {}				# id -> (image, region data, page)
		self.keep = []
		self.pageIdx = 0
		self.pageSlide = None			# {"page", "start", "from", "to", "content"}
		self.mapGesture = None			# {"region", "x", "y", "moved"}
		self.toolTip = None
		self.hoverRegion = None
		self.select = None
		self.__Build()
		self.SetPage(0)

	def __del__(self):
		ui.Window.__del__(self)

	def EnableClip(self):
		# after SetParent: the engine ignores SetInsideRender on a window without parent. Page backgrounds and region images
		# (direct children) are clipped to the map area while they are dragged; labels use the clipping mask
		self.SetInsideRender(True)

	def Destroy(self):
		if app.ENABLE_MOKOKO_SYSTEM:
			uiMokokoRewards.RemoveListener(self.RefreshMokoko)
		if self.select:
			self.select.Destroy()
			self.select.Hide()
			self.select = None
		self.owner = None
		self.regions = {}
		self.keep = []

	# ---------------------------------------------------------------- build

	def __Image(self, parent, name, x, y, pick = False):
		image = ui.ExpandedImageBox()
		image.SetParent(parent)
		image.LoadImage(name)
		image.SetPosition(x, y)
		if not pick:
			image.AddFlag("not_pick")
		image.Show()
		self.keep.append(image)
		return image

	def __Build(self):
		# fixed backdrop under the dragged pages: the strip a page leaves free at the edge shows map background, not the dungeon page
		self.backdrop = self.__Image(self, teleportData.PAGES[0]["background"], 0, 0)
		for (index, page) in enumerate(teleportData.PAGES):
			bg = ui.ExpandedImageBox()
			bg.SetParent(self)
			bg.LoadImage(page["background"])
			bg.SetPosition(0, 0)
			bg.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__BeginMapGesture), None)
			bg.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__EndMapGesture))
			bg.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
			bg.OnRunMouseWheel = ui.__mem_func__(self.OnMouseWheel)
			self.pageBg.append(bg)

			content = _PageLayer()
			self.pageContent.append(content)

			content.Add(self.__Image(self, page["frame"], 0, 0), page["frame_pos"][0], page["frame_pos"][1])
			for region in page["maps"]:
				self.__BuildRegion(content, region, index)
			# labels over every region (Baerim map<N>_text1 are window children): a later region image drew over a label child
			for region in page["maps"]:
				self.__BuildLabel(content, region)

		# page arrows and bullets (on top, outside the dragged pages)
		self.prevButton = self.__Button(215, 493, "btn_back")
		self.prevButton.SetEvent(ui.__mem_func__(self.__RequestPage), -1)
		self.nextButton = self.__Button(377, 493, "btn_next")
		self.nextButton.SetEvent(ui.__mem_func__(self.__RequestPage), 1)
		self.pageBullet = []
		count = len(teleportData.PAGES)
		left = PAGE_WIDTH / 2 - (count * 22 - 4) / 2
		for index in xrange(count):
			bullet = self.__Image(self, WARP_PATH % "bullet_inactive.png", left + index * 22, 500)
			self.pageBullet.append(bullet)
		if count < 2:
			self.prevButton.Hide()
			self.nextButton.Hide()

		# Mokoko collection counter (filled by the Mokoko system)
		self.mokokoButton = self.__Button(556, 5, "btn_mokako")
		self.mokokoText = ui.TextLine()
		self.mokokoText.SetParent(self)
		self.mokokoText.SetPosition(630, 15)
		self.mokokoText.SetHorizontalAlignCenter()
		self.mokokoText.SetText(localeInfo.MOKOKO_TOTAL_PROGRESS.format(0, MOKOKO_TOTAL))
		self.mokokoText.Show()
		self.keep.append(self.mokokoText)
		if app.ENABLE_MOKOKO_SYSTEM:
			self.mokokoButton.SetEvent(ui.__mem_func__(self.__OpenMokokoRewards))
			self.mokokoCounterBg = self.__Image(self.mokokoButton, MOKOKO_COUNTER_BG, 26, -3)
			self.mokokoCounterText = ui.TextLine()
			self.mokokoCounterText.SetParent(self.mokokoCounterBg)
			self.mokokoCounterText.SetPosition(10, 3)
			self.mokokoCounterText.SetHorizontalAlignCenter()
			self.mokokoCounterText.Show()
			self.keep.append(self.mokokoCounterText)
			self.mokokoCounterBg.Hide()
			uiMokokoRewards.AddListener(self.RefreshMokoko)
			self.RefreshMokoko()

	if app.ENABLE_MOKOKO_SYSTEM:
		def __OpenMokokoRewards(self):
			uiMokokoRewards.Open()

		def RefreshMokoko(self):
			state = uiMokokoRewards.STATE
			if state.pending > 0:
				self.mokokoText.SetText(localeInfo.MOKOKO_TOTAL_PROGRESS_AVAILABLE.format(state.found, state.total, state.pending))
				self.mokokoCounterText.SetText(str(state.pending) if state.pending < 100 else "99+")
				self.mokokoCounterBg.Show()
			else:
				self.mokokoText.SetText(localeInfo.MOKOKO_TOTAL_PROGRESS.format(state.found, state.total))
				self.mokokoCounterBg.Hide()
			if self.hoverRegion and self.hoverRegion in self.regions:
				self.__ShowToolTip(self.regions[self.hoverRegion][1])

	def __Button(self, x, y, name):
		button = ui.Button()
		button.SetParent(self)
		button.SetPosition(x, y)
		button.SetUpVisual(WARP_PATH % (name + ".png"))
		button.SetOverVisual(WARP_PATH % (name + "_hover.png"))
		button.SetDownVisual(WARP_PATH % (name + "_clicked.png"))
		button.Show()
		self.keep.append(button)
		return button

	def __BuildRegion(self, content, region, page):
		image = ui.ExpandedImageBox()
		image.SetParent(self)
		image.LoadImage(region["norm"])
		content.Add(image, region["pos"][0], region["pos"][1])
		image.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__BeginMapGesture), region["id"])
		image.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__EndMapGesture))
		image.OnMouseOverIn = lambda regionId = region["id"]: self.OverInMap(regionId)
		image.OnMouseOverOut = lambda regionId = region["id"]: self.OverOutMap(regionId)
		image.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		image.OnRunMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		image.Show()
		self.regions[region["id"]] = (image, region, page)

	def __BuildLabel(self, content, region):
		# label: centred on text_1 x/y, a second line when wider than text_max_width (Baerim "_wrapped", 14 px lower)
		lines = self.__WrapText(MapName(region["id"]), region["text_font"], region["text_width"])
		for (index, text) in enumerate(lines):
			label = ui.TextLine()
			label.SetParent(self)
			label.SetFontName(region["text_font"])
			label.SetPackedFontColor(region["text_color"])
			label.SetOutline()
			if app.ENABLE_CLIP_MASKING:
				label.SetClippingMaskWindow(self)	# text is not cut by the render box, only hidden when fully outside
			content.Add(label, region["pos"][0] + region["text_pos"][0], region["pos"][1] + region["text_pos"][1] + index * 14)
			label.SetHorizontalAlignCenter()
			label.SetVerticalAlignCenter()
			label.SetText(text)
			label.AddFlag("not_pick")
			label.Show()
			self.keep.append(label)

	def __WrapText(self, text, font, width):
		measure = ui.TextLine()
		measure.SetFontName(font)
		measure.SetText(text)
		(textWidth, textHeight) = measure.GetTextSize()
		words = text.split(" ")
		if textWidth <= width or len(words) < 2:
			return [text]
		# the split that keeps the first line inside the width with the most words on it
		best = 1
		for count in xrange(1, len(words)):
			measure.SetText(" ".join(words[:count]))
			if measure.GetTextSize()[0] <= width:
				best = count
		return [" ".join(words[:best]), " ".join(words[best:])]

	# ---------------------------------------------------------------- pages

	def SetPage(self, index):
		self.mapGesture = None
		self.pageSlide = None
		self.pageIdx = max(0, min(len(self.pageBg) - 1, index))
		self.backdrop.LoadImage(teleportData.PAGES[self.pageIdx]["background"])
		for (i, bg) in enumerate(self.pageBg):
			self.__PlacePage(i, 0)
			self.pageContent[i].SetPosition(0, 0)
			self.__ShowPage(i, i == self.pageIdx)
		self.__RefreshBullets()

	def __PlacePage(self, index, x):
		self.pageBg[index].SetPosition(int(x), 0)
		self.pageContent[index].SetBase(int(x))

	def __ShowPage(self, index, show):
		if show:
			self.pageBg[index].Show()
			self.pageContent[index].Show()
		else:
			self.pageBg[index].Hide()
			self.pageContent[index].Hide()

	def __RefreshBullets(self):
		for (i, bullet) in enumerate(self.pageBullet):
			bullet.LoadImage(WARP_PATH % ("bullet_active.png" if i == self.pageIdx else "bullet_inactive.png"))

	def __RequestPage(self, direction, animate = True):
		if self.pageSlide or self.mapGesture:
			return
		target = self.pageIdx + direction
		if target < 0 or target >= len(self.pageBg):
			return
		self.__HideToolTip()
		self.SetPage(target)
		if animate:
			# the new page eases in from MAP_PAGE_SLIDE_DISTANCE px on the side it comes from
			self.pageSlide = {"page" : target, "start" : app.GetTime(), "from" : direction * MAP_PAGE_SLIDE_DISTANCE, "to" : 0, "content" : True}
			self.pageContent[target].SetPosition(direction * MAP_PAGE_SLIDE_DISTANCE, 0)

	def __PositionMapPages(self, page, offset):
		# page at offset, the neighbour it reveals right next to it
		for (i, bg) in enumerate(self.pageBg):
			if i == page:
				self.__PlacePage(i, offset)
				self.__ShowPage(i, True)
			elif offset < 0 and i == page + 1:
				self.__PlacePage(i, offset + PAGE_WIDTH)
				self.__ShowPage(i, True)
			elif offset > 0 and i == page - 1:
				self.__PlacePage(i, offset - PAGE_WIDTH)
				self.__ShowPage(i, True)
			else:
				self.__ShowPage(i, False)

	def __UpdatePageSlide(self):
		slide = self.pageSlide
		if not slide:
			return
		progress = (app.GetTime() - slide["start"]) / MAP_PAGE_SLIDE_DURATION
		value = slide["from"] + (slide["to"] - slide["from"]) * _Ease(progress)
		if slide["content"]:
			self.pageContent[slide["page"]].SetPosition(int(value), 0)
		else:
			self.__PositionMapPages(slide["page"], value)
		if progress >= 1.0:
			page = slide["page"]
			self.pageSlide = None
			self.SetPage(page)

	# ---------------------------------------------------------------- drag (swipe) and click

	def __BeginMapGesture(self, regionId = None):
		if self.pageSlide:
			return
		if self.select and self.select.IsShow():
			self.select.Close()		# a click on the map outside the list closes it
			return
		(x, y) = wndMgr.GetMousePosition()
		self.mapGesture = {"region" : regionId, "x" : x, "y" : y, "moved" : False, "offset" : 0}

	def __UpdateMapGesture(self):
		gesture = self.mapGesture
		if not gesture:
			return
		(x, y) = wndMgr.GetMousePosition()
		dx = x - gesture["x"]
		dy = y - gesture["y"]
		if abs(dx) > MAP_PAGE_CLICK_TOLERANCE or abs(dy) > MAP_PAGE_CLICK_TOLERANCE:
			if not gesture["moved"]:
				self.__HideToolTip()
			gesture["moved"] = True
		if not gesture["moved"]:
			return
		offset = dx
		last = len(self.pageBg) - 1
		if (self.pageIdx == 0 and offset > 0) or (self.pageIdx == last and offset < 0):
			offset = int(offset * MAP_PAGE_EDGE_RESISTANCE)	# nothing on that side: resist
		offset = max(-PAGE_WIDTH, min(PAGE_WIDTH, offset))
		gesture["offset"] = offset
		self.__PositionMapPages(self.pageIdx, offset)

	def __EndMapGesture(self):
		gesture = self.mapGesture
		self.mapGesture = None
		if not gesture:
			return True
		self.__UpdateMapGestureFrom(gesture)
		if not gesture["moved"]:
			regionId = gesture["region"]
			if regionId is not None and regionId in self.regions and self.regions[regionId][0].IsIn():
				self.__ClickMap(None, regionId)
			return True

		offset = gesture["offset"]
		target = self.pageIdx
		if offset <= -MAP_PAGE_SWIPE_DISTANCE and self.pageIdx < len(self.pageBg) - 1:
			target = self.pageIdx + 1
		elif offset >= MAP_PAGE_SWIPE_DISTANCE and self.pageIdx > 0:
			target = self.pageIdx - 1

		# continue from where the page was dropped
		if target == self.pageIdx:
			self.pageSlide = {"page" : self.pageIdx, "start" : app.GetTime(), "from" : offset, "to" : 0, "content" : False}
		else:
			start = offset + (PAGE_WIDTH if target > self.pageIdx else -PAGE_WIDTH)
			self.pageIdx = target
			self.__RefreshBullets()
			self.pageSlide = {"page" : target, "start" : app.GetTime(), "from" : start, "to" : 0, "content" : False}
		return True

	def __UpdateMapGestureFrom(self, gesture):
		self.mapGesture = gesture
		self.__UpdateMapGesture()
		self.mapGesture = None

	def __ClickMap(self, unused, regionId):
		if self.pageSlide or regionId not in self.regions:
			return
		region = self.regions[regionId][1]
		if player.GetStatus(player.LEVEL) < region["level"]:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.TELEPORT_NO_REQUIRED_LEVEL.format(region["level"]))
			return
		if not self.select:
			self.select = SelectTeleportation(self)
		(x, y) = self.GetGlobalPosition()
		self.select.SetPosition(x + (PAGE_WIDTH - 250) / 2, y + (PAGE_HEIGHT - 300) / 2)
		self.__HideToolTip()
		self.select.Open(region)

	# ---------------------------------------------------------------- hover

	def OverInMap(self, regionId):
		if self.pageSlide or (self.mapGesture and self.mapGesture["moved"]) or regionId not in self.regions:
			return
		(image, region, page) = self.regions[regionId]
		image.LoadImage(region["hover"])
		image.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)
		self.hoverRegion = regionId
		self.__ShowToolTip(region)

	def OverOutMap(self, regionId):
		if regionId not in self.regions:
			return
		(image, region, page) = self.regions[regionId]
		image.LoadImage(region["norm"])
		image.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)
		if self.hoverRegion == regionId:
			self.hoverRegion = None
			self.__HideToolTip()

	def __ShowToolTip(self, region):
		if not self.toolTip:
			self.toolTip = uiToolTip.ToolTip(0)
			self.toolTip.HideToolTip()
		self.toolTip.ClearToolTip()
		self.toolTip.toolTipWidth = 0
		self.toolTip.AutoAppendTextLine(MapName(region["id"]), TOOLTIP_TITLE_COLOR)
		color = TOOLTIP_TEXT_COLOR if player.GetStatus(player.LEVEL) >= region["level"] else TOOLTIP_LOCKED_COLOR
		self.toolTip.AutoAppendTextLine(localeInfo.MOKOKO_MAP_REQUIRED_LEVEL.format(region["level"]), color)
		if app.ENABLE_MOKOKO_SYSTEM and region["id"] in uiMokokoRewards.STATE.regions:
			(found, total) = uiMokokoRewards.STATE.regions[region["id"]]
			self.toolTip.AutoAppendTextLine(localeInfo.MOKOKO_MAP_PROGRESS.format(found, total), TOOLTIP_TEXT_COLOR)
		self.toolTip.AlignHorizonalCenter()
		self.toolTip.ShowToolTip()

	def __HideToolTip(self):
		toolTip = getattr(self, "toolTip", None)	# ui.Window.__init__ already calls Hide()
		if toolTip:
			toolTip.HideToolTip()

	# ---------------------------------------------------------------- input / update

	def OnMouseWheel(self, nLen):
		if self.pageSlide or self.mapGesture or not nLen:
			return True
		self.__RequestPage(-1 if nLen > 0 else 1, False)
		return True

	def OnRunMouseWheel(self, nLen):
		return self.OnMouseWheel(nLen)

	def OnUpdate(self):
		self.__UpdateMapGesture()
		self.__UpdatePageSlide()
		if self.select and self.select.IsShow():
			self.select.SetTop()	# never behind the teleport window
			if self.select.preview and self.select.preview.IsShow():
				self.select.preview.SetTop()

	def Show(self):
		ui.Window.Show(self)
		if app.ENABLE_MOKOKO_SYSTEM:
			uiMokokoRewards.RequestInfo()

	def Hide(self):
		self.mapGesture = None
		self.__HideToolTip()
		if getattr(self, "select", None):
			self.select.Close()
		ui.Window.Hide(self)
