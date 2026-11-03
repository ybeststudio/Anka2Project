import app
import ui
import grp
import net
import guild
import player
import messenger
import localeInfo
import uiScriptLocale
import constInfo
import uiCommon
import uiToolTip
import userData
import wndMgr

# Baerim friend list (uiMessenger.MessengerWindow 223x373): search field, collapsible categories (Favorites / Friends /
# Guild / Team / Block), member cards with race face, level, channel + map or last seen, favorite heart and team flag.
# Member details come from the server command "MessengerInfo" (ENABLE_MESSENGER_DETAILS).

ROOT_PATH = "d:/ymir work/ui/game/messenger/"
CRAFTING_PATH = "d:/ymir work/ui/game/crafting/"
FLAG_PATH = "d:/ymir work/ui/intro/login/server_flag_%s.sub"
FAVORITE_PATH = ROOT_PATH + "heart_%s_info.png"

DEFAULT_RACE_IMAGE = ROOT_PATH + "war_m.png"
RACE_IMAGE = {
	0 : ROOT_PATH + "war_m.png",
	1 : ROOT_PATH + "ninja_w.png",
	2 : ROOT_PATH + "sura_m.png",
	3 : ROOT_PATH + "sham_w.png",
	4 : ROOT_PATH + "war_f.png",
	5 : ROOT_PATH + "ninja_m.png",
	6 : ROOT_PATH + "sura_w.png",
	7 : ROOT_PATH + "sham_m.png",
}

FAVORITES = -1
FRIEND = 0
GUILD = 1
TEAM = 2
BLOCK = 3
FAMILY = 4

WINDOW_WIDTH = 223
WINDOW_HEIGHT = 373
VIEW_X = 0
VIEW_Y = 4
VIEW_WIDTH = 205
VIEW_HEIGHT = 262
CATEGORY_X = 3
CATEGORY_WIDTH = 194
CATEGORY_HEIGHT = 25			# collapsed category (header image is 29 high and overlaps the gap)
CATEGORY_SPACING = 3
MEMBER_TOP = 28
MEMBER_HEIGHT = 35
MEMBER_SPACING = 1
ANIM_TICK_COUNT = 10
SCROLL_SPEED = 30
BUTTON_SIZE = 24
BUTTON_SPACING = 7
BUTTON_Y = 337
OFFLINE_REFRESH_SEC = 30

HEADER_COLOR = 0xffd4c1a5
NAME_COLOR = 0xffd4c1a5
POSITIVE_COLOR = 0xff89b88d
NEGATIVE_COLOR = 0xffe57875
LEVEL_PREFIX = "|cfffad388Lv.%d |r"


def GetRaceImage(race):
	return RACE_IMAGE.get(race, DEFAULT_RACE_IMAGE)


def GetServerTime():
	try:
		return app.GetGlobalTimeStamp()
	except:
		import time
		return int(time.time())


def FormatLastSeen(timestamp):
	if timestamp <= 0:
		return localeInfo.MESSENGER_FRIEND_OFFLINE
	elapsed = max(0, GetServerTime() - timestamp)
	if elapsed < 60:
		return localeInfo.MESSENGER_OFFLINE_RECENT
	if elapsed < 3600:
		return localeInfo.MESSENGER_OFFLINE_MINUTES % (elapsed / 60)
	if elapsed < 86400:
		return localeInfo.MESSENGER_OFFLINE_HOURS % (elapsed / 3600)
	return localeInfo.MESSENGER_OFFLINE_DAYS % (elapsed / 86400)


def GetMapName(mapIndex):
	if mapIndex <= 0:
		return ""
	if mapIndex >= 10000:
		mapIndex /= 10000
	try:
		name = localeInfo.GetMiniMapZoneNameByIdx(mapIndex)
	except:
		return ""
	if name == getattr(localeInfo, "MAP_NONE", None):
		return ""
	return name


class Member(object):
	"""One messenger entry (reference MessengerMemberItem state)."""

	def __init__(self, group, key, name):
		self.group = group
		self.key = key
		self.name = name
		self.online = False
		self.level = 0
		self.race = -1
		self.channel = 0
		self.mapIndex = 0
		self.lastSeen = 0
		self.lang = 0
		self.lovePoint = -1

	def CanWhisper(self):
		return self.online and self.group != BLOCK

	def CanRemove(self):
		if self.group in (FRIEND, BLOCK):
			return True
		if self.group == GUILD:
			for i in xrange(guild.ENEMY_GUILD_SLOT_MAX_COUNT):
				if guild.GetEnemyGuildName(i) != "":
					return False
			return guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER) and guild.IsMemberByName(self.name)
		return False

	def CanFavorite(self):
		return self.group in (FRIEND, GUILD, TEAM)

	def GetNameText(self):
		if self.online and self.level > 0:
			return (LEVEL_PREFIX % self.level) + self.name
		return self.name

	def GetStatusText(self):
		if self.online:
			if self.channel > 0:
				text = localeInfo.MESSENGER_FRIEND_CHANNEL % self.channel
				mapName = GetMapName(self.mapIndex)
				if mapName:
					text += " " + mapName
				return text
			return localeInfo.MESSENGER_FRIEND_ONLINE
		return FormatLastSeen(self.lastSeen)


class HitWindow(ui.Window):
	"""Mouse target kept inside the list view: the window manager hit-tests the full rect, the renderer clips it."""

	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.onClick = None
		self.onDoubleClick = None
		self.onOverIn = None
		self.onOverOut = None

	def __del__(self):
		ui.Window.__del__(self)

	def Place(self, x, y, width, height):
		top = max(0, y)
		bottom = min(VIEW_HEIGHT, y + height)
		if bottom <= top:
			self.Hide()
			return
		self.SetPosition(x, top)
		self.SetSize(width, bottom - top)
		self.Show()

	def OnMouseLeftButtonDown(self):
		if self.onClick:
			self.onClick()

	def OnMouseLeftButtonDoubleClick(self):
		if self.onDoubleClick:
			self.onDoubleClick()

	def OnMouseOverIn(self):
		if self.onOverIn:
			self.onOverIn()

	def OnMouseOverOut(self):
		if self.onOverOut:
			self.onOverOut()


class MemberSlot(object):
	"""Card visuals (content layer) + hit window and heart (overlay layer) of one member."""

	def __init__(self, owner, content, overlay):
		self.owner = owner
		self.member = None
		self.isOver = False

		self.image = self.__Image(content, ROOT_PATH + "friend_box.png")
		self.hoverImage = self.__Image(content, ROOT_PATH + "friend_box_hover.png")
		self.hoverImage.Hide()
		self.charImage = self.__Image(content, DEFAULT_RACE_IMAGE)
		self.text = self.__Text(content)
		self.statusText = self.__Text(content)
		self.languageFlag = ui.ImageBox()
		self.languageFlag.SetParent(content)
		self.languageFlag.AddFlag("not_pick")

		self.window = HitWindow(overlay)
		self.window.onClick = ui.__mem_func__(self.OnClick)
		self.window.onDoubleClick = ui.__mem_func__(self.OnDoubleClick)
		self.window.onOverIn = ui.__mem_func__(self.OnOverIn)
		self.window.onOverOut = ui.__mem_func__(self.OnOverOut)

		self.favoriteButton = ui.Button()
		self.favoriteButton.SetParent(overlay)
		self.favoriteButton.SetEvent(ui.__mem_func__(self.OnClickFavorite))
		self.favoriteState = None
		self.__SetFavoriteVisual(False)

		self.toolTip = uiToolTip.ToolTip(175)
		self.toolTip.HideToolTip()

	def __Image(self, parent, path):
		image = ui.ExpandedImageBox()
		image.SetParent(parent)
		image.AddFlag("not_pick")
		image.LoadImage(path)
		image.Show()
		return image

	def __Text(self, parent):
		text = ui.TextLine()
		text.SetParent(parent)
		text.AddFlag("not_pick")
		text.SetFontName("Tahoma:12")
		text.Show()
		return text

	def __SetFavoriteVisual(self, favorite):
		if self.favoriteState == favorite:
			return
		self.favoriteState = favorite
		path = FAVORITE_PATH % ("full" if favorite else "empty")
		self.favoriteButton.SetUpVisual(path)
		self.favoriteButton.SetOverVisual(path)
		self.favoriteButton.SetDownVisual(path)
		if favorite:
			self.favoriteButton.SetToolTipText(localeInfo.MESSENGER_REMOVE_FAVORITE)
		else:
			self.favoriteButton.SetToolTipText(localeInfo.MESSENGER_ADD_FAVORITE)

	def Destroy(self):
		for window in (self.image, self.hoverImage, self.charImage, self.text, self.statusText, self.languageFlag,
				self.window, self.favoriteButton):
			window.Hide()
		self.toolTip.HideToolTip()
		self.window.onClick = None
		self.window.onDoubleClick = None
		self.window.onOverIn = None
		self.window.onOverOut = None
		self.owner = None
		self.member = None

	def SetMember(self, member):
		self.member = member
		self.Refresh()

	def Refresh(self):
		member = self.member
		if not member:
			return
		self.charImage.LoadImage(GetRaceImage(member.race))
		self.text.SetText(member.GetNameText())
		self.text.SetPackedFontColor(NAME_COLOR)
		self.statusText.SetText(member.GetStatusText())
		self.statusText.SetPackedFontColor(POSITIVE_COLOR if member.online else NEGATIVE_COLOR)
		if member.group == TEAM and member.lang:
			try:
				self.languageFlag.LoadImage(FLAG_PATH % constInfo.GetLang(member.lang))
				self.languageFlag.Show()
			except:
				self.languageFlag.Hide()
		else:
			self.languageFlag.Hide()
		self.__SetFavoriteVisual(self.owner.IsFavorite(member.name))
		self.toolTip.ClearToolTip()
		self.toolTip.AppendTextLine(localeInfo.MESSENGER_SLOT_TOOLTIP)
		if member.lovePoint >= 0:
			self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % member.lovePoint)
		self.toolTip.ResizeToolTip()

	def RefreshStatus(self):
		if self.member:
			self.statusText.SetText(self.member.GetStatusText())

	def Layout(self, x, y, visible, viewOffset):
		"""x, y: slot position in the content window; viewOffset: content y inside the view."""
		for window in (self.image, self.charImage, self.text, self.statusText):
			window.Show() if visible else window.Hide()
		if not visible:
			self.hoverImage.Hide()
			self.languageFlag.Hide()
			self.window.Hide()
			self.favoriteButton.Hide()
			return
		self.image.SetPosition(x, y)
		self.hoverImage.SetPosition(x, y)
		self.charImage.SetPosition(x + 3, y + 3)
		self.text.SetPosition(x + 38, y + 3)
		self.statusText.SetPosition(x + 38, y + 17)
		self.languageFlag.SetPosition(x + 138, y + 9)
		if self.member and self.member.group == TEAM and self.member.lang:
			self.languageFlag.Show()
		self.__RefreshHover()

		viewY = y + viewOffset
		self.window.Place(x + VIEW_X, viewY, CATEGORY_WIDTH, MEMBER_HEIGHT)
		heartY = viewY + 8
		if self.member and self.member.CanFavorite() and heartY >= 0 and heartY + 19 <= VIEW_HEIGHT:
			self.favoriteButton.SetPosition(x + 165, heartY)
			self.favoriteButton.Show()
		else:
			self.favoriteButton.Hide()

	def __RefreshHover(self):
		if self.isOver or (self.member and self.owner and self.owner.IsSelected(self.member)):
			self.hoverImage.Show()
		else:
			self.hoverImage.Hide()

	def OnClick(self):
		if self.owner and self.member:
			self.owner.OnSelectMember(self.member)

	def OnDoubleClick(self):
		if self.owner and self.member:
			self.owner.OnDoubleClickMember(self.member)

	def OnOverIn(self):
		self.isOver = True
		self.__RefreshHover()
		self.toolTip.ShowToolTip()

	def OnOverOut(self):
		self.isOver = False
		self.__RefreshHover()
		self.toolTip.HideToolTip()

	def OnClickFavorite(self):
		if self.owner and self.member:
			self.owner.ToggleFavorite(self.member.name)


class Category(object):
	"""Collapsible group (reference MessengerCategory): header 194x29, members from y 28 every 36 px."""

	def __init__(self, owner, groupIndex, name, content, overlay):
		self.owner = owner
		self.groupIndex = groupIndex
		self.name = name
		self.content = content
		self.overlay = overlay
		self.expanded = True
		self.slots = []
		self.members = []
		self.curHeight = CATEGORY_HEIGHT
		self.animFrom = CATEGORY_HEIGHT
		self.animTick = ANIM_TICK_COUNT

		self.header = ui.ExpandedImageBox()
		self.header.SetParent(content)
		self.header.AddFlag("not_pick")
		self.header.LoadImage(ROOT_PATH + "category_button.png")
		self.header.Show()
		self.headerText = ui.TextLine()
		self.headerText.SetParent(content)
		self.headerText.AddFlag("not_pick")
		self.headerText.SetFontName("Tahoma:13b")
		self.headerText.SetPackedFontColor(HEADER_COLOR)
		self.headerText.Show()
		self.headerStateImage = ui.ImageBox()
		self.headerStateImage.SetParent(content)
		self.headerStateImage.AddFlag("not_pick")
		self.headerStateImage.Show()
		self.headerHit = HitWindow(overlay)
		self.headerHit.onClick = ui.__mem_func__(self.Toggle)
		self.__RefreshStateImage()

	def Destroy(self):
		for slot in self.slots:
			slot.Destroy()
		self.slots = []
		for window in (self.header, self.headerText, self.headerStateImage, self.headerHit):
			window.Hide()
		self.headerHit.onClick = None
		self.owner = None

	def __RefreshStateImage(self):
		self.headerStateImage.LoadImage(CRAFTING_PATH + ("cat_open_0.png" if self.expanded else "cat_close_0.png"))

	def GetTargetHeight(self):
		if self.expanded and self.members:
			return MEMBER_TOP + len(self.members) * (MEMBER_HEIGHT + MEMBER_SPACING)
		return CATEGORY_HEIGHT

	def SetMembers(self, members):
		self.members = members
		while len(self.slots) < len(members):
			self.slots.append(MemberSlot(self.owner, self.content, self.overlay))
		while len(self.slots) > len(members):
			self.slots.pop().Destroy()
		for (slot, member) in zip(self.slots, members):
			slot.SetMember(member)
		online = len([m for m in members if m.online])
		self.headerText.SetText("%s (%d/%d)" % (self.name, online, len(members)))
		if self.animTick >= ANIM_TICK_COUNT:
			self.curHeight = self.GetTargetHeight()

	def RefreshStatusTexts(self):
		for slot in self.slots:
			slot.RefreshStatus()

	def Toggle(self):
		self.expanded = not self.expanded
		self.__RefreshStateImage()
		self.animFrom = self.curHeight
		self.animTick = 0
		if self.owner:
			self.owner.OnCategoryToggled(self)

	def IsAnimating(self):
		return self.animTick < ANIM_TICK_COUNT

	def UpdateAnimation(self):
		if not self.IsAnimating():
			return False
		self.animTick += 1
		target = self.GetTargetHeight()
		self.curHeight = int(self.animFrom + (target - self.animFrom) * float(self.animTick) / ANIM_TICK_COUNT)
		return True

	def Layout(self, y, viewOffset):
		self.header.SetPosition(CATEGORY_X, y)
		self.headerText.SetPosition(CATEGORY_X + 9, y + 7)
		self.headerStateImage.SetPosition(CATEGORY_X + 170, y + 9)
		self.headerHit.Place(CATEGORY_X + VIEW_X, y + viewOffset, CATEGORY_WIDTH, CATEGORY_HEIGHT + 4)
		for (index, slot) in enumerate(self.slots):
			slotY = MEMBER_TOP + index * (MEMBER_HEIGHT + MEMBER_SPACING)
			visible = slotY + MEMBER_HEIGHT <= self.curHeight
			slot.Layout(CATEGORY_X, y + slotY, visible, viewOffset)


class MessengerWindow(ui.Window):

	def __init__(self):
		ui.Window.__init__(self)
		messenger.SetMessengerHandler(self)

		self.members = {FRIEND : [], GUILD : [], TEAM : [], BLOCK : [], FAMILY : []}
		self.pendingInfo = {}		# (group, name) -> MessengerInfo arriving before the member itself
		self.favorites = []
		self.favoritesLoaded = False
		self.categories = []
		self.selected = None
		self.whisperButtonEvent = lambda *arg : None
		self.guildButtonEvent = None
		self.questionDialog = None
		self.friendNameBoard = None
		self.blockNameBoard = None
		self.onlinePopup = None
		self.needsRefresh = False
		self.currentScrollPos = 0.0
		self.targetScrollPos = 0
		self.contentHeight = 0
		self.lastOfflineRefresh = 0
		self.isLoaded = False

		messenger.RefreshGuildMember()

	def __del__(self):
		messenger.SetMessengerHandler(None)
		ui.Window.__del__(self)

	# ------------------------------------------------------------------ window
	def __LoadWindow(self):
		self.isLoaded = True
		self.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetPosition(wndMgr.GetScreenWidth() - WINDOW_WIDTH - 40, wndMgr.GetScreenHeight() - WINDOW_HEIGHT - 80)

		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.AddFlag("attach")
		self.board.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT)
		self.board.SetPosition(0, 0)
		self.board.SetTitleName(uiScriptLocale.MESSENGER_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.Show()

		self.searchField = ui.ExpandedImageBox()
		self.searchField.SetParent(self.board)
		self.searchField.LoadImage(ROOT_PATH + "search_field.png")
		self.searchField.SetPosition(10, 32)
		self.searchField.OnMouseLeftButtonDown = ui.__mem_func__(self.__FocusSearch)
		self.searchField.Show()

		self.searchInput = ui.EditLine()
		self.searchInput.SetParent(self.searchField)
		self.searchInput.SetPosition(5, 5)
		self.searchInput.SetSize(147, 15)
		self.searchInput.SetMax(24)
		self.searchInput.SetFontName("Tahoma:12")
		self.searchInput.SetUpdateEvent(ui.__mem_func__(self.__OnSearchTextChange))
		self.searchInput.SetEscapeEvent(ui.__mem_func__(self.__OnPressSearchEscape))
		self.searchInput.SetReturnEvent(ui.__mem_func__(self.__OnClickSearch))
		self.searchInput.Show()

		self.clearBtn = self.__Button(self.searchField, 155, 5, "x_norm.png", "x_hover.png", "x_down.png")
		self.clearBtn.SetEvent(ui.__mem_func__(self.__ClearSearchField))
		self.searchBtn = self.__Button(self.searchField, 173, -1, "search_01.png", "search_02.png", "search_03.png")
		self.searchBtn.SetEvent(ui.__mem_func__(self.__OnClickSearch))

		self.inviteBg = ui.ExpandedImageBox()
		self.inviteBg.SetParent(self.board)
		self.inviteBg.AddFlag("not_pick")
		self.inviteBg.LoadImage(ROOT_PATH + "bg.png")
		self.inviteBg.SetPosition(8, 63)
		self.inviteBg.Show()

		# list view: everything inside is clipped to it; the overlay holds the mouse targets placed inside the view
		self.view = ui.Window()
		self.view.SetParent(self.board)
		self.view.SetPosition(8 + VIEW_X, 63 + VIEW_Y)
		self.view.SetSize(VIEW_WIDTH, VIEW_HEIGHT)
		self.view.SetInsideRender(True)
		self.view.Show()
		self.content = ui.Window()
		self.content.SetParent(self.view)
		self.content.AddFlag("not_pick")
		self.content.SetPosition(0, 0)
		self.content.SetSize(VIEW_WIDTH, VIEW_HEIGHT)
		self.content.Show()
		self.overlay = self.view		# hit windows / hearts are view children created after the content

		self.scrollBar = ThinScrollBar()
		self.scrollBar.SetParent(self.board)
		self.scrollBar.SetPosition(8 + 195, 63 + 4)
		self.scrollBar.SetSize(6, VIEW_HEIGHT)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollChange))
		self.scrollBar.Hide()

		self.__CreateMessengerButtons()
		self.__UpdateButtonStates()

	def __Button(self, parent, x, y, up, over, down, disable = None):
		button = ui.Button()
		button.SetParent(parent)
		button.SetUpVisual(ROOT_PATH + up)
		button.SetOverVisual(ROOT_PATH + over)
		button.SetDownVisual(ROOT_PATH + down)
		if disable:
			button.SetDisableVisual(ROOT_PATH + disable)
		button.SetPosition(x, y)
		button.Show()
		return button

	def __CreateMessengerButtons(self):
		buttonDefs = [
			("addFriendButton", "add_friend", uiScriptLocale.MESSENGER_ADD_FRIEND, self.OnPressAddFriendButton, "add_friend_2.png"),
			("whisperButton", "whisper", uiScriptLocale.MESSENGER_WHISPER, self.OnPressWhisperButton, "whisper_2.png"),
			("removeButton", "remove", uiScriptLocale.MESSENGER_DELETE_FRIEND, self.OnPressRemoveButton, "remove_3.png"),
		]
		if app.ENABLE_MESSENGER_BLOCK:
			buttonDefs.append(("addBlockButton", "block", uiScriptLocale.MESSENGER_BLOCK_FRIEND, self.OnPressAddBlockButton, "block_2.png"))
		buttonDefs.append(("guildButton", "guild", uiScriptLocale.MESSENGER_OPEN_GUILD, self.OnPressGuildButton, "guild_2.png"))

		totalW = len(buttonDefs) * BUTTON_SIZE + (len(buttonDefs) - 1) * BUTTON_SPACING
		startX = (WINDOW_WIDTH - totalW) / 2
		for (i, (name, basePath, tooltip, event, disable)) in enumerate(buttonDefs):
			btn = self.__Button(self.board, startX + i * (BUTTON_SIZE + BUTTON_SPACING), BUTTON_Y,
				basePath + "_0.png", basePath + "_1.png", basePath + "_2.png", disable)
			btn.SetToolTipText(tooltip)
			btn.SetEvent(ui.__mem_func__(event))
			setattr(self, name, btn)

	def Show(self):
		if not self.isLoaded:
			self.__LoadWindow()
			self.__LoadFavorites()
			self.__BuildCategories()
		ui.Window.Show(self)

	def Close(self):
		self.questionDialog = None
		self.searchInput.KillFocus()
		self.Hide()

	def Destroy(self):
		for cat in self.categories:
			cat.Destroy()
		self.categories = []
		self.questionDialog = None
		self.friendNameBoard = None
		self.blockNameBoard = None
		self.onlinePopup = None
		self.whisperButtonEvent = lambda *arg : None
		self.guildButtonEvent = None

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def GetSelf(self):
		return self

	def SetWhisperButtonEvent(self, event):
		self.whisperButtonEvent = event

	def SetGuildButtonEvent(self, event):
		self.guildButtonEvent = event

	# ------------------------------------------------------------------ favorites
	def __FavoriteStoragePath(self):
		return userData.GetPath("%s.favorites" % player.GetName(), "messenger")

	def __LoadFavorites(self):
		self.favoritesLoaded = True
		self.favorites = []
		try:
			f = open(self.__FavoriteStoragePath(), "r")
			for line in f.readlines():
				name = line.strip()
				if name and name not in self.favorites:
					self.favorites.append(name)
			f.close()
		except IOError:
			pass

	def __SaveFavorites(self):
		try:
			f = open(self.__FavoriteStoragePath(), "w")
			for name in self.favorites:
				f.write(name + "\n")
			f.close()
		except IOError:
			pass

	def IsFavorite(self, name):
		return name in self.favorites

	def ToggleFavorite(self, name):
		if not self.favoritesLoaded:
			self.__LoadFavorites()
		if name in self.favorites:
			self.favorites.remove(name)
		else:
			self.favorites.append(name)
		self.__SaveFavorites()
		self.DeferRefresh()

	# ------------------------------------------------------------------ categories
	def __CategoryDefs(self):
		defs = []
		if self.favorites:
			defs.append((FAVORITES, localeInfo.MESSENGER_FAVORITES))
		if self.members[FAMILY]:
			defs.append((FAMILY, localeInfo.MESSENGER_FAMILY))
		defs.append((FRIEND, localeInfo.MESSENGER_FRIEND))
		defs.append((GUILD, localeInfo.MESSENGER_GUILD))
		if app.ENABLE_MESSENGER_TEAM:
			defs.append((TEAM, localeInfo.MESSENGER_TEAM))
		if app.ENABLE_MESSENGER_BLOCK:
			defs.append((BLOCK, localeInfo.MESSENGER_BLOCK))
		return defs

	def __GetSearchQuery(self):
		if not self.isLoaded:
			return ""
		return self.searchInput.GetText().strip().lower()

	def __GetCategoryMembers(self, groupIndex):
		query = self.__GetSearchQuery()
		if groupIndex == FAVORITES:
			# one card per name: a friend can also be a guild or team member
			members = []
			names = set()
			for group in (FRIEND, GUILD, TEAM):
				for m in self.members[group]:
					if m.name in self.favorites and m.name not in names:
						names.add(m.name)
						members.append(m)
		else:
			members = [m for m in self.members[groupIndex] if groupIndex == BLOCK or groupIndex == FAMILY or m.name not in self.favorites]
		if query:
			members = [m for m in members if query in m.name.lower()]
		# reference order: online members first, each part in arrival order
		return [m for m in members if m.online] + [m for m in members if not m.online]

	def __FindCategory(self, groupIndex):
		for cat in self.categories:
			if cat.groupIndex == groupIndex:
				return cat
		return None

	def __BuildCategories(self):
		if not self.isLoaded:
			return
		expanded = dict([(cat.groupIndex, cat.expanded) for cat in self.categories])
		wanted = self.__CategoryDefs()
		if [cat.groupIndex for cat in self.categories] != [g for (g, n) in wanted]:
			for cat in self.categories:
				cat.Destroy()
			self.categories = []
			for (groupIndex, name) in wanted:
				cat = Category(self, groupIndex, name, self.content, self.overlay)
				cat.expanded = expanded.get(groupIndex, True)
				cat.curHeight = CATEGORY_HEIGHT
				self.categories.append(cat)
		for cat in self.categories:
			cat.SetMembers(self.__GetCategoryMembers(cat.groupIndex))
		if self.selected and not self.__IsMemberListed(self.selected):
			self.selected = None
		self.__Layout()
		self.__UpdateButtonStates()

	def __IsMemberListed(self, member):
		for cat in self.categories:
			if member in cat.members:
				return True
		return False

	def __Layout(self):
		self.contentHeight = max(0, sum([cat.curHeight + CATEGORY_SPACING for cat in self.categories]) - CATEGORY_SPACING)
		self.__UpdateScrollbar()
		offset = -int(self.currentScrollPos)
		# the content is clipped by its own rect as well: keep it as high as the whole list
		self.content.SetSize(VIEW_WIDTH, max(VIEW_HEIGHT, self.contentHeight))
		self.content.SetPosition(0, offset)
		yPos = 0
		for cat in self.categories:
			cat.Layout(yPos, offset)
			yPos += cat.curHeight + CATEGORY_SPACING

	def __MaxScroll(self):
		return max(0, self.contentHeight - VIEW_HEIGHT)

	def __UpdateScrollbar(self):
		maxVal = self.__MaxScroll()
		self.targetScrollPos = max(0, min(self.targetScrollPos, maxVal))
		self.currentScrollPos = max(0.0, min(self.currentScrollPos, float(maxVal)))
		if maxVal <= 0:
			self.scrollBar.Hide()
			return
		self.scrollBar.SetMiddleBarScale(float(VIEW_HEIGHT) / float(self.contentHeight))
		self.scrollBar.SetPosScale(float(self.targetScrollPos) / float(maxVal))
		self.scrollBar.Show()

	def __OnScrollChange(self, fScale):
		self.targetScrollPos = int(self.__MaxScroll() * fScale)
		self.currentScrollPos = float(self.targetScrollPos)
		self.__Layout()

	def __ScrollByWheel(self, nLen):
		maxVal = self.__MaxScroll()
		if maxVal <= 0:
			return
		step = SCROLL_SPEED if nLen < 0 else -SCROLL_SPEED
		self.targetScrollPos = max(0, min(maxVal, self.targetScrollPos + step))
		self.scrollBar.SetPosScale(float(self.targetScrollPos) / float(maxVal))

	def OnRunMouseWheel(self, nLen):
		if not self.IsShow():
			return False
		self.__ScrollByWheel(nLen)
		return True

	def OnMouseWheel(self, nLen):
		return self.OnRunMouseWheel(nLen)

	def OnCategoryToggled(self, cat):
		self.__Layout()

	def DeferRefresh(self):
		self.needsRefresh = True

	def OnUpdate(self):
		if self.needsRefresh:
			self.needsRefresh = False
			self.__BuildCategories()

		changed = False
		for cat in self.categories:
			if cat.UpdateAnimation():
				changed = True
		if abs(self.currentScrollPos - self.targetScrollPos) > 0.5:
			self.currentScrollPos += (self.targetScrollPos - self.currentScrollPos) * 0.35
			changed = True
		elif self.currentScrollPos != self.targetScrollPos:
			self.currentScrollPos = float(self.targetScrollPos)
			changed = True
		if changed:
			self.__Layout()

		now = app.GetTime()
		if now - self.lastOfflineRefresh > OFFLINE_REFRESH_SEC:
			self.lastOfflineRefresh = now
			for cat in self.categories:
				cat.RefreshStatusTexts()

	# ------------------------------------------------------------------ search
	def __FocusSearch(self):
		self.searchInput.SetFocus()

	def __OnSearchTextChange(self):
		self.targetScrollPos = 0
		self.currentScrollPos = 0.0
		self.DeferRefresh()

	def __OnClickSearch(self):
		self.searchInput.SetFocus()
		self.DeferRefresh()

	def __ClearSearchField(self):
		self.searchInput.SetText("")
		self.__OnSearchTextChange()

	def __OnPressSearchEscape(self):
		if self.searchInput.GetText():
			self.__ClearSearchField()
		else:
			self.searchInput.KillFocus()
			self.Close()
		return True

	# ------------------------------------------------------------------ selection / buttons
	def IsSelected(self, member):
		return self.selected is member

	def OnSelectMember(self, member):
		self.selected = member
		self.__Layout()
		self.__UpdateButtonStates()

	def OnDoubleClickMember(self, member):
		self.selected = member
		self.__UpdateButtonStates()
		if member.CanWhisper():
			self.whisperButtonEvent(member.name)

	def __UpdateButtonStates(self):
		if not self.isLoaded:
			return
		member = self.selected
		if member and member.CanWhisper():
			self.whisperButton.Enable()
		else:
			self.whisperButton.Disable()
		if member and member.CanRemove():
			self.removeButton.Enable()
		else:
			self.removeButton.Disable()

	def OnPressGuildButton(self):
		if self.guildButtonEvent:
			self.guildButtonEvent()

	def OnPressWhisperButton(self):
		if self.selected and self.selected.CanWhisper():
			self.whisperButtonEvent(self.selected.name)

	def OnPressAddFriendButton(self):
		friendNameBoard = uiCommon.InputDialog()
		friendNameBoard.SetTitle(localeInfo.MESSENGER_ADD_FRIEND)
		friendNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddFriend))
		friendNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddFriend))
		friendNameBoard.Open()
		self.friendNameBoard = friendNameBoard

	def OnAddFriend(self):
		text = self.friendNameBoard.GetText()
		if text:
			net.SendMessengerAddByNamePacket(text)
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	def OnCancelAddFriend(self):
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return True

	if app.ENABLE_MESSENGER_BLOCK:
		def OnPressAddBlockButton(self):
			blockNameBoard = uiCommon.InputDialog()
			blockNameBoard.SetTitle(localeInfo.MESSENGER_ADD_BLOCK_FRIEND)
			blockNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddBlock))
			blockNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddBlock))
			blockNameBoard.Open()
			self.blockNameBoard = blockNameBoard

		def OnAddBlock(self):
			text = self.blockNameBoard.GetText()
			if text:
				net.SendMessengerBlockAddByNamePacket(text)
			self.blockNameBoard.Close()
			self.blockNameBoard = None
			return True

		def OnCancelAddBlock(self):
			self.blockNameBoard.Close()
			self.blockNameBoard = None
			return True

	def OnPressRemoveButton(self):
		if self.selected and self.selected.CanRemove():
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRemove))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()

	def OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnRemove(self):
		member = self.selected
		if member and member.CanRemove():
			if member.group == FRIEND:
				messenger.RemoveFriend(member.key)
				net.SendMessengerRemovePacket(member.key, member.name)
			elif member.group == BLOCK:
				net.SendMessengerBlockRemovePacket(member.key, member.name)
			elif member.group == GUILD:
				net.SendGuildRemoveMemberPacket(member.key)
			if member in self.members[member.group]:
				self.members[member.group].remove(member)
			self.selected = None
			self.DeferRefresh()
		self.OnCloseQuestionDialog()

	# ------------------------------------------------------------------ data from the client core
	def __FindMember(self, groupIndex, key):
		for member in self.members.get(groupIndex, []):
			if member.key == key:
				return member
		return None

	def IsGuildMemberOnline(self, name):
		for member in self.members[GUILD]:
			if member.name == name:
				return member.online
		return False

	def __AddList(self, groupIndex, key, name):
		if groupIndex not in self.members:
			return None
		member = self.__FindMember(groupIndex, key)
		if not member:
			member = Member(groupIndex, key, name or key)
			self.members[groupIndex].append(member)
			info = self.pendingInfo.pop((groupIndex, member.name), None)
			if info:
				self.__ApplyInfo(member, info)
		elif name:
			member.name = name
		return member

	def OnLogin(self, groupIndex, key, name = None):
		member = self.__AddList(groupIndex, key, name)
		if not member:
			return
		member.online = True
		member.lastSeen = 0
		self.DeferRefresh()
		self.__NotifyOnline(member.name)
		self.__NotifyGuildWindow(groupIndex)

	def OnLogout(self, groupIndex, key, name = None):
		member = self.__AddList(groupIndex, key, name)
		if not member:
			return
		if member.online:
			member.lastSeen = GetServerTime()
		member.online = False
		member.channel = 0
		member.mapIndex = 0
		self.DeferRefresh()
		self.__NotifyGuildWindow(groupIndex)

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def OnLoginWithLang(self, groupIndex, key, lang, lang2nd, name = None):
			member = self.__AddList(groupIndex, key, name)
			if not member:
				return
			member.lang = lang
			member.online = True
			self.DeferRefresh()
			self.__NotifyOnline(member.name)
			self.__NotifyGuildWindow(groupIndex)

		def OnLogoutWithLang(self, groupIndex, key, lang, lang2nd, name = None):
			member = self.__AddList(groupIndex, key, name)
			if not member:
				return
			member.lang = lang
			member.online = False
			self.DeferRefresh()
			self.__NotifyGuildWindow(groupIndex)

	def __NotifyGuildWindow(self, groupIndex):
		# the guild member page shows the online state of the guild group
		if groupIndex != GUILD:
			return
		interface = constInfo.GetInterfaceInstance()
		wndGuild = getattr(interface, "wndGuild", None) if interface else None
		if wndGuild:
			wndGuild.RefreshGuildMemberPage()

	def __NotifyOnline(self, name):
		if name in constInfo.ALREADY_NOTIFY_LIST:
			return
		self.onlinePopup = uiCommon.OnlinePopup()
		self.onlinePopup.SetUserName(name)
		self.onlinePopup.SetEvent(ui.__mem_func__(self.OpenWhisper), "MOUSE_LEFT_BUTTON_UP", name)
		self.onlinePopup.SlideIn()
		constInfo.ALREADY_NOTIFY_LIST.append(name)

	def OpenWhisper(self, eventType, userName):
		self.whisperButtonEvent(userName)

	def OnRemoveList(self, groupIndex, key):
		member = self.__FindMember(groupIndex, key)
		if member:
			self.members[groupIndex].remove(member)
			if self.selected is member:
				self.selected = None
		self.DeferRefresh()

	def OnRemoveAllList(self, groupIndex):
		if groupIndex in self.members:
			self.members[groupIndex] = []
		self.DeferRefresh()

	def ClearGuildMember(self):
		self.members[GUILD] = []
		self.DeferRefresh()

	def RefreshMessenger(self):
		self.DeferRefresh()

	def OnRefreshList(self):
		self.DeferRefresh()

	# server "MessengerInfo <group> <name> <level> <race> <channel> <mapIndex> <lastSeen>"
	def OnMemberInfo(self, groupIndex, name, level, race, channel, mapIndex, lastSeen):
		info = (level, race, channel, mapIndex, lastSeen)
		member = None
		for m in self.members.get(groupIndex, []):
			if m.name == name:
				member = m
				break
		if not member:
			self.pendingInfo[(groupIndex, name)] = info
			return
		self.__ApplyInfo(member, info)
		self.DeferRefresh()

	def __ApplyInfo(self, member, info):
		(level, race, channel, mapIndex, lastSeen) = info
		if race >= 0:
			member.race = race
		if lastSeen:
			member.lastSeen = lastSeen
		else:
			member.level = level
			member.channel = channel
			member.mapIndex = mapIndex

	# ------------------------------------------------------------------ lover
	def OnAddLover(self, name, lovePoint):
		self.members[FAMILY] = []
		member = self.__AddList(FAMILY, name, name)
		member.lovePoint = lovePoint
		self.DeferRefresh()

	def OnUpdateLovePoint(self, lovePoint):
		for member in self.members[FAMILY]:
			member.lovePoint = lovePoint
		self.DeferRefresh()

	def OnLoginLover(self):
		for member in self.members[FAMILY]:
			member.online = True
		self.DeferRefresh()

	def OnLogoutLover(self):
		for member in self.members[FAMILY]:
			if member.online:
				member.lastSeen = GetServerTime()
			member.online = False
		self.DeferRefresh()

	def ClearLoverInfo(self):
		self.members[FAMILY] = []
		self.DeferRefresh()


class ThinScrollBar(ui.Window):
	"""6 px track with a movable middle bar (reference ScrollBarSpecial)."""
	BASE_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
	LINE_COLOR = grp.GenerateColor(0.3411, 0.3411, 0.3411, 1.0)
	MIDDLE_COLOR = grp.GenerateColor(0.6470, 0.6470, 0.6470, 1.0)

	def __init__(self):
		ui.Window.__init__(self)
		self.scrollEvent = None
		self.base = ui.Bar()
		self.base.SetParent(self)
		self.base.AddFlag("not_pick")
		self.base.SetColor(self.BASE_COLOR)
		self.base.Show()
		self.lines = []
		for i in xrange(4):
			line = ui.Bar()
			line.SetParent(self)
			line.AddFlag("not_pick")
			line.SetColor(self.LINE_COLOR)
			line.Show()
			self.lines.append(line)
		self.middleBar = ui.DragButton()
		self.middleBar.SetParent(self)
		self.middleBar.AddFlag("movable")
		self.middleBar.AddFlag("restrict_x")
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.middleBar.Show()
		self.middle = ui.Bar()
		self.middle.SetParent(self.middleBar)
		self.middle.AddFlag("not_pick")
		self.middle.SetColor(self.MIDDLE_COLOR)
		self.middle.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def SetScrollEvent(self, event):
		self.scrollEvent = event

	def SetSize(self, width, height):
		ui.Window.SetSize(self, width, height)
		self.base.SetPosition(1, 1)
		self.base.SetSize(width - 2, height - 2)
		self.lines[0].SetPosition(0, 1)
		self.lines[0].SetSize(1, height - 2)
		self.lines[1].SetPosition(width - 1, 1)
		self.lines[1].SetSize(1, height - 2)
		self.lines[2].SetPosition(1, 0)
		self.lines[2].SetSize(width - 2, 1)
		self.lines[3].SetPosition(1, height - 1)
		self.lines[3].SetSize(width - 2, 1)
		self.SetMiddleBarScale(1.0)

	def SetMiddleBarScale(self, scale):
		height = max(12, int((self.GetHeight() - 2) * min(1.0, scale)))
		width = self.GetWidth() - 2
		self.middleBar.SetSize(width, height)
		self.middle.SetSize(width, height)
		self.middleBar.SetRestrictMovementArea(1, 1, width, self.GetHeight() - 2)

	def SetPosScale(self, fScale):
		travel = max(0, self.GetHeight() - 2 - self.middleBar.GetHeight())
		self.middleBar.SetPosition(1, 1 + int(travel * max(0.0, min(1.0, fScale))))

	def __OnMove(self):
		travel = max(1, self.GetHeight() - 2 - self.middleBar.GetHeight())
		(x, y) = self.middleBar.GetLocalPosition()
		if self.scrollEvent:
			self.scrollEvent(max(0.0, min(1.0, float(y - 1) / float(travel))))

	def OnMouseLeftButtonDown(self):
		(x, y) = self.GetMouseLocalPosition()
		travel = max(1, self.GetHeight() - 2 - self.middleBar.GetHeight())
		self.SetPosScale(float(y - self.middleBar.GetHeight() / 2) / float(travel))
		self.__OnMove()

