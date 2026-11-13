if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import item
import wndMgr
import chat
import renderTarget
import localeInfo
import mouseModule
import uiCommon
try:
	import userData
except ImportError:
	userData = None

# Gardirop (reference Baerim uiWardrobe: WardrobeInventoryController + WardrobeWindow).
# Layout measured from the live Baerim client (794x586), protocol from Baerim's own client tests.
PATH = "d:/ymir work/ui/game/wardrobe/"
STAR_EMPTY = "d:/ymir work/ui/game/expedition/star_empty_info.png"
STAR_FULL = "d:/ymir work/ui/game/expedition/star_full_info.png"
OUTFIT_PATH = PATH + "outfits/"

LOOK_COUNT = 9
(LOOK_BODY, LOOK_HAIR, LOOK_MOUNT, LOOK_ACCE, LOOK_WEAPON, LOOK_ACCE_SKIN, LOOK_PET, LOOK_SHINING_WEAPON, LOOK_SHINING_ARMOR) = range(LOOK_COUNT)
OUTFIT_MAX = 15
OUTFIT_NAME_MAX = 48
TIMEOUT = 5.0
ACTION_DELAY = 0.35
AUTOSAVE_DELAY = 3.0
OWNED_CHECK_DELAY = 0.2

(TAB_CATALOG, TAB_OUTFITS, TAB_STORAGE) = range(3)

# type buttons (Baerim typeTitles) -> look slots
TYPE_LIST = (
	("WARDROBE_TYPE_BODY", (LOOK_BODY,)),
	("WARDROBE_TYPE_HAIR", (LOOK_HAIR,)),
	("WARDROBE_TYPE_WEAPON", (LOOK_WEAPON,)),
	("WARDROBE_TYPE_ACCE", (LOOK_ACCE, LOOK_ACCE_SKIN)),
	("WARDROBE_TYPE_MOUNT", (LOOK_MOUNT,)),
	("WARDROBE_TYPE_PET", (LOOK_PET,)),
	("WARDROBE_SHININGS", (LOOK_SHINING_WEAPON, LOOK_SHINING_ARMOR)),
)


# outfit costume slots on outfits/inv.png: (look slots shown there, x, y, width, height)
OUTFIT_SLOT_LAYOUT = (
	((LOOK_WEAPON,), 13, 25, 32, 96),
	((LOOK_HAIR,), 63, 17, 32, 32),
	((LOOK_BODY,), 63, 63, 32, 64),
	((LOOK_ACCE_SKIN, LOOK_ACCE), 113, 29, 32, 32),	# the sash skin wins (Baerim: top right slot)
	((LOOK_MOUNT,), 113, 74, 32, 32),
	((LOOK_PET,), 113, 119, 32, 32),
	((LOOK_SHINING_WEAPON,), 13, 147, 32, 32),
	((LOOK_SHINING_ARMOR,), 63, 148, 32, 32),
)

# server result codes (wardrobe.h EWardrobeResult)
RESULT_OK, RESULT_ERROR, RESULT_INVALID, RESULT_FULL, RESULT_BUSY, RESULT_UNOWNED, RESULT_BLOCKED = range(7)

CARD_W, CARD_H = 90, 118
CARD_STEP_X, CARD_STEP_Y = 94, 122
CARD_COLUMNS = 8
CARD_X = 5
HEADER_H = 30
HEADER_GAP = 8
CONTENT_X, CONTENT_Y, CONTENT_W, CONTENT_H = 13, 144, 760, 400
WHEEL_STEP = 61

TEXT_COLOR = 0xfff4e6c3
HELP_COLOR = 0xffd5c4a5
TITLE_COLOR = 0xffcabdab
PLACEHOLDER_COLOR = 0xff9b907c
MARK_COLOR = 0xffffc45c
MISSING_COLOR = 0xffff5050

PART_WEAPON, PART_HAIR, PART_ACCE = 1, 4, 5
ACCE_BASE_VNUM = 85000
SHINING_WEAPON, SHINING_ARMOR = 0, 1
PREVIEW_SCALE = 0.85

GENDER_MALE, GENDER_FEMALE = 0, 1
RACE_IMAGES = (("warm", "warf"), ("ninjam", "ninjaf"), ("suram", "suraf"), ("samanm", "samanf"))


def _Text(key, default = ""):
	return getattr(localeInfo, key, default)


def _Hex(text):
	return "".join(["%02x" % ord(c) for c in text])


def _UnHex(text):
	try:
		return "".join([chr(int(text[i:i + 2], 16)) for i in xrange(0, len(text), 2)])
	except ValueError:
		return ""


def _IsMale():
	return player.GetRace() in (0, 2, 5, 7, 8)


def _Job():
	return { 0 : 0, 4 : 0, 1 : 1, 5 : 1, 2 : 2, 6 : 2, 3 : 3, 7 : 3 }.get(player.GetRace(), -1)


class LookItem:
	# cached prototype data of one look vnum
	def __init__(self, vnum):
		self.vnum = vnum
		item.SelectItem(vnum)
		self.name = item.GetItemName()
		self.lowerName = self.name.lower()
		self.icon = item.GetIconImageFileName()
		(self.width, self.height) = item.GetItemSize()
		self.antiMale = item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE)
		self.antiFemale = item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE)
		self.antiJob = (item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR), item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN),
			item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA), item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN))
		self.value0 = item.GetValue(0)
		self.value1 = item.GetValue(1)
		self.mountVnum = 0
		for i in xrange(item.ITEM_APPLY_MAX_NUM):
			(applyType, applyValue) = item.GetAffect(i)
			if applyType == item.APPLY_MOUNT and applyValue > 0:
				self.mountVnum = applyValue
		if not self.mountVnum:
			self.mountVnum = self.value1

	def FitsGender(self, gender):
		return not ((gender == GENDER_MALE and self.antiMale) or (gender == GENDER_FEMALE and self.antiFemale))

	def FitsJob(self, job):
		return job < 0 or job >= len(self.antiJob) or not self.antiJob[job]

	def FitsCharacter(self):
		return self.FitsGender(GENDER_MALE if _IsMale() else GENDER_FEMALE) and self.FitsJob(_Job())

	def PetMob(self):
		return self.value1 or self.value0


LOOK_CACHE = {}
def GetLook(vnum):
	if not vnum:
		return None
	look = LOOK_CACHE.get(vnum)
	if look is None:
		look = LookItem(vnum)
		LOOK_CACHE[vnum] = look
	return look


def LookSlotOfVnum(vnum):
	item.SelectItem(vnum)
	itemType = item.GetItemType()
	subType = item.GetItemSubType()
	if itemType == item.ITEM_TYPE_COSTUME:
		table = {
			item.COSTUME_TYPE_BODY : LOOK_BODY,
			item.COSTUME_TYPE_HAIR : LOOK_HAIR,
		}
		for (name, slot) in (("COSTUME_TYPE_MOUNT", LOOK_MOUNT), ("COSTUME_TYPE_ACCE", LOOK_ACCE), ("COSTUME_TYPE_WEAPON", LOOK_WEAPON),
				("COSTUME_TYPE_ACCE_SKIN", LOOK_ACCE_SKIN), ("COSTUME_TYPE_PET", LOOK_PET)):
			if hasattr(item, name):
				table[getattr(item, name)] = slot
		return table.get(subType, -1)
	if itemType == getattr(item, "ITEM_TYPE_PET", -1) and (item.GetValue(0) or item.GetValue(1)):
		return LOOK_PET		# ITEM_PET sub types are not read by the protos (PET_PAY = 0): a pet item with a mob
	if itemType == getattr(item, "ITEM_TYPE_SHINING", -1):
		if subType == item.SHINING_WEAPON:
			return LOOK_SHINING_WEAPON
		if subType == item.SHINING_ARMOR:
			return LOOK_SHINING_ARMOR
	return -1


class WardrobeController(object):
	# protocol state (Baerim WardrobeInventoryController); lives in the interface, the window is created on demand
	def __init__(self):
		self.serial = 0
		self.pending = None			# (serial, action)
		self.pendingTime = 0.0
		self.lastSent = -10.0
		self.result = None
		self.resultAction = None
		self.resultEvent = None

		self.catalog = []			# [(vnum, categoryId, lookSlot)] server order
		self.categories = []		# [(categoryId, key)]
		self.catalogReady = False
		self.catalogBuffer = None

		self.outfits = {}			# slot -> [revision, name, looks] (name "" = deleted)
		self.outfitsReady = False
		self.outfitBuffer = None
		self.rememberedOutfit = None
		self.appliedOutfit = None

		self.activeLooks = [0] * LOOK_COUNT
		self.progress = None		# (costumes, level, totalExp, expInLevel, expPerLevel)

		self.ownedRevision = -1
		self.owned = set()

		self.favorites = set()		# Baerim: favourite storage costumes (vnums), kept on this computer per character
		self.favoritesName = None
		self.ownedCheckTime = 0.0
		self.changeEvent = None

	def SetChangeEvent(self, event):
		self.changeEvent = event

	def __Changed(self, what):
		if self.changeEvent:
			self.changeEvent(what)

	# --- sending -------------------------------------------------------------------------------
	def CanSend(self):
		if self.pending and app.GetTime() - self.pendingTime > TIMEOUT:
			self.Update()
		return self.pending is None and app.GetTime() - self.lastSent >= ACTION_DELAY

	def __Send(self, action, fmt, *args):
		if not getattr(app, "ENABLE_WARDROBE_SYSTEM", 0):
			return False
		if not self.CanSend():
			return False
		self.serial += 1
		self.pending = (self.serial, action)
		self.pendingTime = app.GetTime()
		self.lastSent = self.pendingTime
		net.SendChatPacket(("/wardrobe %s %d " % (action, self.serial)) + (fmt % args))
		return True

	def RequestCatalog(self):
		return self.__Send("catalog", "0 0 0")

	def RequestOutfits(self):
		return self.__Send("outfits", "0")

	def Deposit(self, cell):
		itemVnum = player.GetItemIndex(player.INVENTORY, cell)
		if not itemVnum or cell >= player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT or LookSlotOfVnum(itemVnum) < 0:
			return False
		return self.__Send("deposit", "%d %d", cell, itemVnum)

	def Withdraw(self, cell):
		itemVnum = player.GetItemIndex(player.WARDROBE, cell)
		if not itemVnum:
			return False
		return self.__Send("withdraw", "%d %d", cell, itemVnum)

	def MoveToSwitchbot(self, cell):
		itemVnum = player.GetItemIndex(player.WARDROBE, cell)
		if not itemVnum:
			return False
		return self.__Send("switchbot", "%d %d", cell, itemVnum)

	def Equip(self, cell):
		itemVnum = player.GetItemIndex(player.WARDROBE, cell)
		if not itemVnum:
			return False
		return self.__Send("equip", "%d %d", cell, itemVnum)

	# --- favourites (Baerim: client side only, never sent) --------------------------------------
	def __FavoritePath(self):
		name = player.GetName()
		if not name or not userData:
			return None
		safe = "".join([ch if ch.isalnum() else "_" for ch in name])
		return userData.GetPath("wardrobe_favorites_%s.txt" % safe)

	def LoadFavorites(self):
		name = player.GetName()
		if name == self.favoritesName:
			return
		self.favoritesName = name
		self.favorites = set()
		path = self.__FavoritePath()
		if not path:
			return
		try:
			stream = open(path, "r")
		except IOError:
			return
		try:
			for line in stream:
				try:
					vnum = int(line.strip())
				except ValueError:
					continue
				if 0 < vnum < 0x7fffffff:
					self.favorites.add(vnum)
		finally:
			stream.close()

	def ToggleFavorite(self, vnum):
		self.LoadFavorites()
		path = self.__FavoritePath()
		if not path or not vnum:
			return False
		favorites = set(self.favorites)
		if vnum in favorites:
			favorites.remove(vnum)
		else:
			favorites.add(vnum)
		try:
			stream = open(path, "w")
			try:
				stream.write("\n".join(["%d" % v for v in sorted(favorites)]))
			finally:
				stream.close()
		except IOError:
			return False
		self.favorites = favorites
		return True

	def IsFavorite(self, vnum):
		self.LoadFavorites()
		return vnum in self.favorites

	def __LooksArg(self, looks):
		return " ".join(["%d" % int(v) for v in looks])

	def ApplyLooks(self, looks, outfitSlot = None):
		if len(looks) != LOOK_COUNT:
			return False
		if not self.__Send("apply", "%d %s", 0 if outfitSlot is None else outfitSlot + 1, self.__LooksArg(looks)):
			return False
		self.applySlot = outfitSlot
		self.lastApplyLooks = list(looks)
		return True

	def SaveOutfit(self, slot, name, looks):
		if not self.outfitsReady or slot < 0 or slot >= OUTFIT_MAX:
			return False
		name = name.strip()
		if not name or len(name) > OUTFIT_NAME_MAX or "|" in name:
			return False
		revision = self.outfits[slot][0] if slot in self.outfits else 0
		return self.__Send("save", "%d %d %s %s", slot, revision, _Hex(name), self.__LooksArg(looks))

	def DeleteOutfit(self, slot):
		if not self.outfitsReady or slot not in self.outfits or not self.outfits[slot][1]:
			return False
		return self.__Send("delete", "%d %d", slot, self.outfits[slot][0])

	# --- replies -------------------------------------------------------------------------------
	def __IsCurrent(self, serial):
		try:
			return self.pending is not None and int(serial) == self.pending[0]
		except ValueError:
			return False

	def OnResult(self, serial, code):
		if not self.__IsCurrent(serial):
			return
		action = self.pending[1]
		self.pending = None
		self.resultAction = action
		self.result = int(code)
		if action == "apply" and self.result == RESULT_OK:
			self.appliedOutfit = getattr(self, "applySlot", None)
		if self.resultEvent:
			self.resultEvent(action, self.result)

	def OnCatalogBegin(self, serial, rows):
		if self.__IsCurrent(serial):
			self.catalogBuffer = ([], [])

	def OnCatalogCategory(self, serial, categoryId, nameHex):
		if self.__IsCurrent(serial) and self.catalogBuffer is not None:
			self.catalogBuffer[0].append((int(categoryId), _UnHex(nameHex)))

	def OnCatalogRows(self, serial, data):
		if not self.__IsCurrent(serial) or self.catalogBuffer is None:
			return
		for row in data.split(";"):
			parts = row.split(",")
			if len(parts) == 3:
				try:
					self.catalogBuffer[1].append((int(parts[0]), int(parts[1]), int(parts[2])))
				except ValueError:
					pass

	def OnCatalogEnd(self, serial):
		if not self.__IsCurrent(serial) or self.catalogBuffer is None:
			return
		(self.categories, self.catalog) = self.catalogBuffer
		self.catalogBuffer = None
		self.catalogReady = True
		self.pending = None
		self.__Changed("catalog")

	def OnOutfitsBegin(self, serial, count, remembered, applied):
		if self.__IsCurrent(serial):
			self.outfitBuffer = ({}, int(remembered), int(applied))

	def OnOutfitRow(self, serial, slot, revision, nameHex, *looks):
		if not self.__IsCurrent(serial) or self.outfitBuffer is None or len(looks) != LOOK_COUNT:
			return
		name = "" if nameHex == "-" else _UnHex(nameHex)
		self.outfitBuffer[0][int(slot)] = [int(revision), name, [int(v) for v in looks]]

	def OnOutfitsEnd(self, serial):
		if not self.__IsCurrent(serial) or self.outfitBuffer is None:
			return
		(self.outfits, remembered, applied) = self.outfitBuffer
		self.outfitBuffer = None
		self.rememberedOutfit = remembered - 1 if remembered > 0 else None
		self.appliedOutfit = applied - 1 if applied > 0 else None
		self.outfitsReady = True
		self.pending = None
		self.__Changed("outfits")

	def OnAppearance(self, *looks):
		if len(looks) != LOOK_COUNT:
			return
		self.activeLooks = [int(v) for v in looks]
		self.__Changed("appearance")

	def OnProgress(self, costumes, level, totalExp, expInLevel, expPerLevel):
		self.progress = (int(costumes), int(level), int(totalExp), int(expInLevel), int(expPerLevel))
		self.__Changed("progress")

	# --- local state ---------------------------------------------------------------------------
	def IsOwned(self, vnum):
		return vnum in self.owned

	def Update(self):
		now = app.GetTime()
		if self.pending and now - self.pendingTime > TIMEOUT:
			action = self.pending[1]
			self.pending = None
			self.catalogBuffer = None
			self.outfitBuffer = None
			self.resultAction = action
			self.result = "timeout"
			if self.resultEvent:
				self.resultEvent(action, "timeout")
		if now >= self.ownedCheckTime:
			self.ownedCheckTime = now + OWNED_CHECK_DELAY
			revision = player.GetWardrobeInventoryRevision()
			if revision != self.ownedRevision:
				self.ownedRevision = revision
				self.owned = set(player.GetOwnedCostumeVnums())
				self.__Changed("owned")


class PreviewRenderTarget(ui.RenderTarget):
	# right drag rotates, wheel zooms (uiCostumeShop.CostumeRenderTarget)
	def __init__(self):
		ui.RenderTarget.__init__(self)
		self.isDrag = False
		self.lastPos = (0, 0)
		self.totalPos = [0, 0]
		self.renderIndex = -1

	def __del__(self):
		ui.RenderTarget.__del__(self)

	def SetIndex(self, renderIndex):
		self.renderIndex = renderIndex
		self.SetRenderTarget(renderIndex)
		self.SetMouseRightButtonDownEvent(ui.__mem_func__(self.__RightDown))
		self.SetMouseRightButtonUpEvent(ui.__mem_func__(self.__RightUp))

	def __RightDown(self):
		app.SetCursor(app.CAMERA_ROTATE)
		self.isDrag = True
		self.lastPos = app.GetCursorPosition()
		return True

	def __RightUp(self):
		app.SetCursor(app.NORMAL)
		self.isDrag = False
		return True

	def EndDrag(self):
		if self.isDrag:
			self.__RightUp()

	def OnRunMouseWheel(self, nLen):
		if renderTarget.IsShow(self.renderIndex) != 1:
			return False
		renderTarget.Zoom(self.renderIndex, app.CAMERA_TO_NEGATIVE if nLen > 0 else app.CAMERA_TO_POSITIVE)
		return True

	def OnUpdate(self):
		if not self.isDrag or renderTarget.IsShow(self.renderIndex) != 1:
			return
		currentPos = app.GetCursorPosition()
		x = (currentPos[0] - self.lastPos[0]) + self.totalPos[0]
		y = (currentPos[1] - self.lastPos[1]) + self.totalPos[1]
		renderTarget.RotateEyeAroundTarget(self.renderIndex, y * 0.3, x * 0.3)
		self.totalPos = [x, y]
		self.lastPos = currentPos


class ContentWindow(ui.Window):
	# the card area: wheel scroll and item drop (deposit)
	def __init__(self):
		ui.Window.__init__(self)
		self.wheelEvent = None
		self.dropEvent = None

	def __del__(self):
		ui.Window.__del__(self)

	def OnRunMouseWheel(self, nLen):
		if self.wheelEvent:
			self.wheelEvent(nLen)
			return True
		return False

	def OnMouseLeftButtonUp(self):
		if self.dropEvent:
			self.dropEvent()
		return True


class WardrobeScrollBar(ui.Window):
	# thin bar with the Baerim wardrobe thumb (scroll.png)
	def __init__(self, height):
		ui.Window.__init__(self)
		self.SetSize(7, height)
		self.event = None
		self.pos = 0.0
		self.thumb = ui.DragButton()
		self.thumb.SetParent(self)
		self.thumb.AddFlag("movable")
		self.thumb.SetUpVisual(PATH + "scroll.png")
		self.thumb.SetOverVisual(PATH + "scroll.png")
		self.thumb.SetDownVisual(PATH + "scroll.png")
		self.thumb.SetMoveEvent(ui.__mem_func__(self.__OnMove))
		self.thumb.SetRestrictMovementArea(0, 0, 6, height)
		self.thumb.SetPosition(0, 0)
		self.thumb.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.event = None

	def SetScrollEvent(self, event):
		self.event = event

	def SetPos(self, pos):
		self.pos = max(0.0, min(1.0, pos))
		movable = self.GetHeight() - self.thumb.GetHeight()
		self.thumb.SetPosition(0, int(movable * self.pos))

	def GetPos(self):
		return self.pos

	def __OnMove(self):
		movable = self.GetHeight() - self.thumb.GetHeight()
		(x, y) = self.thumb.GetLocalPosition()
		self.pos = float(y) / movable if movable > 0 else 0.0
		if self.event:
			self.event()


class DarkFilterBoard(ui.Window):
	# Baerim DarkFilterTitleBoard: 1 px grey + 1 px black frame (searchshop/bg_filtertitle.png), ornament_title/base.png inside
	FRAME_COLOR = 0xff68645a
	TILE = 256

	def __init__(self, parent, x, y, width, height):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.SetSize(width, height)
		self.AddFlag("not_pick")
		self.children = []
		for (bx, by, bw, bh, color) in ((0, 0, width, height, self.FRAME_COLOR), (1, 1, width - 2, height - 2, 0xff000000)):
			bar = ui.Bar()
			bar.SetParent(self)
			bar.SetPosition(bx, by)
			bar.SetSize(bw, bh)
			bar.SetColor(color)
			bar.AddFlag("not_pick")
			bar.Show()
			self.children.append(bar)
		innerW = width - 4
		innerH = height - 4
		ty = 0
		while ty < innerH:
			tx = 0
			while tx < innerW:
				w = min(self.TILE, innerW - tx)
				h = min(self.TILE, innerH - ty)
				tile = ui.ExpandedImageBox()
				tile.SetParent(self)
				tile.LoadImage("d:/ymir work/ui/game/common/ornament_title/base.png")
				tile.SetPosition(2 + tx, 2 + ty)
				tile.SetRenderingRect(0.0, 0.0, -(1.0 - float(w) / self.TILE), -(1.0 - float(h) / self.TILE))
				tile.AddFlag("not_pick")
				tile.Show()
				self.children.append(tile)
				tx += self.TILE
			ty += self.TILE
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)


class WardrobeWindow(ui.Window):
	def __init__(self, controller, interface):
		ui.Window.__init__(self)
		self.controller = controller
		self.interface = interface
		self.tooltipItem = interface.tooltipItem if interface else None
		self.tab = TAB_CATALOG
		self.typeIndex = 0
		self.ownedOnly = False
		self.gender = None
		self.race = None
		self.search = ""
		self.scrollOffset = 0
		self.contentHeight = 0
		self.displayRows = []
		self.layout = []
		self.cards = []
		self.cardStars = []
		self.headers = []
		self.selectedOutfit = None
		self.draft = [0] * LOOK_COUNT
		self.draftName = ""
		self.draftDirty = False
		self.autoSaveDue = 0.0
		self.saveQueue = {}			# slot -> (name, looks): sent one by one when the server takes a request
		self.savingOutfit = None	# (slot, name, looks) of the save on the way
		self.applyQueue = None		# (looks, outfit slot or None) sent after the queued saves
		self.previewKey = None		# the preview is rebuilt only when what it shows changes
		self.outfitOffset = 0
		self.renderIndex = -1
		self.petRenderIndex = -1
		self.progressHovered = False
		self.popup = None
		self.isBuilt = False
		self.keepAlive = []		# helper widgets without an attribute (python would destroy them)
		self.__Build()

	def __del__(self):
		ui.Window.__del__(self)

	# --- build -----------------------------------------------------------------------------------
	def __Image(self, parent, x, y, path, cls = ui.ExpandedImageBox):
		image = cls()
		image.SetParent(parent)
		image.LoadImage(path)
		image.SetPosition(x, y)
		image.Show()
		self.keepAlive.append(image)
		return image

	def __Button(self, parent, x, y, normal, hover, down, text = None):
		button = ui.Button()
		button.SetParent(parent)
		button.SetUpVisual(normal)
		button.SetOverVisual(hover)
		button.SetDownVisual(down)
		button.SetPosition(x, y)
		if text is not None:
			button.SetText(text)
		button.Show()
		self.keepAlive.append(button)
		return button

	def __TextLine(self, parent, x, y, text, color = None, center = True, vcenter = False, outline = False, font = None):
		line = ui.TextLine()
		line.SetParent(parent)
		if font:
			line.SetFontName(font)
		line.SetPosition(x, y)
		if center:
			line.SetHorizontalAlignCenter()
		if vcenter:
			line.SetVerticalAlignCenter()
		if outline:
			line.SetOutline()
		if color is not None:
			line.SetPackedFontColor(color)
		line.SetText(text)
		line.Show()
		self.keepAlive.append(line)
		return line

	def __Build(self):
		self.SetSize(794, 586)
		self.AddFlag("movable")
		self.AddFlag("float")

		self.board = ui.BoardWithTitleBar()
		self.board.SetParent(self)
		self.board.SetSize(794, 560)
		self.board.SetPosition(0, 0)
		self.board.AddFlag("not_pick")
		self.board.SetTitleName(localeInfo.WARDROBE_TITLE)
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.board.Show()

		# top bar: search, owned only, gender / class filters
		self.topBoard = DarkFilterBoard(self, 13, 35, 768, 40)
		self.topBackground = self.__Image(self, 14, 36, PATH + "top_bg.png", ui.ImageBox)	# holds the owned check box: catalog / inventory only
		self.topShade = self.__Image(self, 15, 37, OUTFIT_PATH + "top_shade.png", ui.ImageBox)	# outfits: the darkened board of Baerim
		self.topShade.AddFlag("not_pick")
		self.__Image(self, 20, 43, PATH + "search_bg.png", ui.ImageBox)
		self.searchEdit = ui.EditLine()
		self.searchEdit.SetParent(self)
		self.searchEdit.SetPosition(26, 47)
		self.searchEdit.SetSize(268, 18)
		self.searchEdit.SetMax(30)
		self.searchEdit.SetPackedFontColor(TITLE_COLOR)
		self.searchEdit.OnIMEUpdate = ui.__mem_func__(self.__OnSearchUpdate)
		self.searchEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.searchEdit.Show()
		self.searchButton = self.__Button(self, 320, 43, PATH + "btn_search.png", PATH + "btn_search_hover.png", PATH + "btn_search_clicked.png")
		self.searchButton.SetToolTipText(localeInfo.WARDROBE_SEARCH)
		self.searchButton.SetEvent(ui.__mem_func__(self.__OnSearchUpdate))
		self.clearSearchButton = self.__Button(self, 298, 46, "d:/ymir work/ui/public/close_button_01.sub", "d:/ymir work/ui/public/close_button_02.sub", "d:/ymir work/ui/public/close_button_03.sub")
		self.clearSearchButton.SetToolTipText(localeInfo.WARDROBE_CLEAR_SEARCH)
		self.clearSearchButton.SetEvent(ui.__mem_func__(self.__ClearSearch))
		self.clearSearchButton.Hide()

		self.ownedCheck = ContentWindow()
		self.ownedCheck.SetParent(self)
		self.ownedCheck.SetPosition(352, 47)
		self.ownedCheck.SetSize(148, 18)
		self.ownedCheck.dropEvent = ui.__mem_func__(self.__ToggleOwned)
		self.ownedCheck.Show()
		self.ownedCheckMark = self.__Image(self, 353, 49, PATH + "check (1).png")
		self.ownedCheckMark.AddFlag("not_pick")
		self.ownedCheckMark.Hide()
		self.ownedText = self.__TextLine(self, 376, 55, localeInfo.WARDROBE_OWNED_ONLY, TEXT_COLOR, center = False, vcenter = True, outline = True, font = "Tahoma:12b")

		self.genderButtons = []
		for (gender, name, x) in ((GENDER_MALE, "male", 508), (GENDER_FEMALE, "female", 550)):
			button = self.__Button(self, x, 39, PATH + "btn_%s.png" % name, PATH + "btn_%s_hover.png" % name, PATH + "btn_%s_clicked.png" % name)
			button.SetEvent(ui.__mem_func__(self.__SelectGender), gender)
			self.genderButtons.append((button, name))
		self.raceButtons = []
		for race in xrange(4):
			button = ui.Button()
			button.SetParent(self)
			button.SetPosition(596 + race * 46, 39)
			button.SetEvent(ui.__mem_func__(self.__SelectRace), race)
			button.Show()
			self.raceButtons.append(button)

		# collection level
		self.__Image(self, 13, 74, PATH + "gauge_bg.png", ui.ImageBox)
		self.levelGauge = self.__Image(self, 122, 76, PATH + "gauge.png")
		self.levelIcon = self.__Image(self, 14, 74, PATH + "xp_icon.png")
		self.levelText = self.__TextLine(self, 76, 77, localeInfo.WARDROBE_PROGRESS_LOADING)
		self.levelHover = ui.Window()
		self.levelHover.SetParent(self)
		self.levelHover.SetPosition(13, 74)
		self.levelHover.SetSize(768, 20)
		self.levelHover.Hide()

		# type buttons
		self.typeBackground = self.__Image(self, 13, 98, PATH + "bg2.png", ui.ImageBox)
		self.typeButtons = []
		for (i, (key, slots)) in enumerate(TYPE_LIST):
			button = self.__Button(self, 20 + i * 94, 108, PATH + "tab.png", PATH + "tab_active.png", PATH + "tab_active.png", _Text(key))
			button.SetTextColor(TEXT_COLOR)
			button.SetEvent(ui.__mem_func__(self.__SelectType), i)
			self.typeButtons.append(button)

		# card area
		self.contentBackground = self.__Image(self, 13, 140, PATH + "bg.png", ui.ImageBox)
		self.content = ContentWindow()
		self.content.SetParent(self)
		self.content.SetPosition(CONTENT_X, CONTENT_Y)
		self.content.SetSize(CONTENT_W, CONTENT_H)
		self.content.wheelEvent = ui.__mem_func__(self.__OnWheel)
		self.content.dropEvent = ui.__mem_func__(self.__OnDropOnContent)
		self.content.Show()
		self.content.SetInsideRender(True)	# cards / icons are clipped to the card area while scrolling
		self.scrollBar = WardrobeScrollBar(406)
		self.scrollBar.SetParent(self)
		self.scrollBar.SetPosition(773, 141)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollBar))
		self.scrollBar.Show()
		self.loadingText = self.__TextLine(self, 396, 300, localeInfo.WARDROBE_LOADING, TITLE_COLOR, vcenter = True)
		self.loadingText.Hide()
		self.emptyText = self.__TextLine(self, 396, 300, "", HELP_COLOR, vcenter = True)
		self.emptyText.Hide()

		self.__BuildOutfitTab()

		# bottom navigation (Baerim order: Gardirop, Kombinler, Envanter)
		self.navButtons = {}
		for (tab, x, key) in ((TAB_CATALOG, 20, "WARDROBE_TITLE"), (TAB_OUTFITS, 116, "WARDROBE_OUTFITS"), (TAB_STORAGE, 212, "WARDROBE_INVENTORY")):
			button = self.__Button(self, x, 551, PATH + "nav_tab_normal.png", PATH + "nav_tab_active.png", PATH + "nav_tab_active.png", _Text(key))
			button.SetEvent(ui.__mem_func__(self.SelectTab), tab)
			self.navButtons[tab] = button

		self.SetCenterPosition()
		self.__RefreshFilterButtons()
		self.isBuilt = True

	def __BuildOutfitTab(self):
		self.outfitWidgets = []
		def keep(w):
			self.outfitWidgets.append(w)
			w.Hide()
			return w

		keep(self.__Image(self, 13, 100, OUTFIT_PATH + "bg2.png"))
		self.outfitList = keep(ContentWindow())
		self.outfitList.SetParent(self)
		self.outfitList.SetPosition(13, 100)
		self.outfitList.SetSize(180, 447)
		self.outfitList.wheelEvent = ui.__mem_func__(self.__OnOutfitWheel)
		self.outfitList.SetInsideRender(True)
		self.outfitRows = []
		for i in xrange(OUTFIT_MAX):
			button = ui.Button()
			button.SetParent(self.outfitList)
			button.SetUpVisual(OUTFIT_PATH + "costume_tab.png")
			button.SetOverVisual(OUTFIT_PATH + "costume_tab_hover.png")
			button.SetDownVisual(OUTFIT_PATH + "costume_tab_clicked.png")
			button.SetEvent(ui.__mem_func__(self.__SelectOutfit), i)
			mask = self.__Image(button, 8, 9, OUTFIT_PATH + "mask.png")
			mask.AddFlag("not_pick")
			name = self.__TextLine(button, 52, 19, "", TEXT_COLOR, center = False, vcenter = True)
			name.SetClippingMaskWindow(self.outfitList)
			mark = self.__TextLine(button, 146, 19, localeInfo.WARDROBE_OUTFIT_UNAPPLIED_MARK, MARK_COLOR, vcenter = True, outline = True)
			mark.SetClippingMaskWindow(self.outfitList)
			check = self.__Image(button, 159, 14, PATH + "checked.png")
			check.AddFlag("not_pick")
			self.outfitRows.append((button, mask, name, mark, check))
		self.outfitScroll = keep(WardrobeScrollBar(441))
		self.outfitScroll.SetParent(self)
		self.outfitScroll.SetPosition(193, 103)
		self.outfitScroll.SetScrollEvent(ui.__mem_func__(self.__OnOutfitScroll))
		keep(self.__Image(self, 15, 434, OUTFIT_PATH + "shadow_footer.png")).AddFlag("not_pick")

		# preview
		keep(self.__Image(self, 207, 100, OUTFIT_PATH + "bg.png"))
		self.renderIndex = renderTarget.GetFreeIndex(50, 85)
		self.preview = keep(PreviewRenderTarget())
		self.preview.SetParent(self)
		self.preview.SetSize(411, 447)
		self.preview.SetPosition(207, 100)
		self.preview.SetIndex(self.renderIndex)
		renderTarget.SetBackground(self.renderIndex, OUTFIT_PATH + "bg.png")
		renderTarget.SetRotation(self.renderIndex, False)
		self.petRenderIndex = renderTarget.GetFreeIndex(50, 85)
		self.petPreview = PreviewRenderTarget()
		self.petPreview.SetParent(self)
		self.petPreview.SetSize(85, 95)
		self.petPreview.SetPosition(523, 442)
		self.petPreview.SetIndex(self.petRenderIndex)
		self.petPreview.Hide()
		renderTarget.SetTransparent(self.petRenderIndex)
		renderTarget.SetRotation(self.petRenderIndex, True)
		self.zoomHint = keep(self.__TextLine(self, 594, 110, localeInfo.COSTUME_SHOP_ZOOM, center = False))
		self.zoomHint.SetHorizontalAlignRight()
		self.moveHint = keep(self.__TextLine(self, 594, 140, localeInfo.COSTUME_SHOP_MOVE, center = False))
		self.moveHint.SetHorizontalAlignRight()
		self.outfitHelp = []
		for (i, key) in enumerate(("WARDROBE_OUTFIT_HELP_SELECT", "WARDROBE_OUTFIT_HELP_SAVE", "WARDROBE_OUTFIT_HELP_APPLY")):
			self.outfitHelp.append(keep(self.__TextLine(self, 412, 120 + i * 14, _Text(key), HELP_COLOR)))

		# costumes of the outfit
		header = keep(self.__Image(self, 622, 100, OUTFIT_PATH + "header.png"))
		self.__TextLine(header, 79, 15, localeInfo.WARDROBE_COSTUMES_TITLE, TEXT_COLOR, vcenter = True)
		self.outfitInv = keep(self.__Image(self, 622, 130, OUTFIT_PATH + "inv.png"))
		self.lookSlots = ui.SlotWindow()
		self.lookSlots.SetParent(self.outfitInv)
		self.lookSlots.SetPosition(0, 0)
		self.lookSlots.SetSize(self.outfitInv.GetWidth(), self.outfitInv.GetHeight())
		self.lookSlots.AddFlag("not_pick")
		for (index, (looks, x, y, w, h)) in enumerate(OUTFIT_SLOT_LAYOUT):
			self.lookSlots.AppendSlot(index, x, y, w, h)
		self.lookSlots.Show()
		self.outfitSlots = []
		for (looks, x, y, w, h) in OUTFIT_SLOT_LAYOUT:
			hover = ui.Window()
			hover.SetParent(self.outfitInv)
			hover.SetPosition(x, y)
			hover.SetSize(w, h)
			hover.Show()
			icon = ui.ExpandedImageBox()
			icon.SetParent(self.outfitInv)
			icon.SetPosition(x, y)
			icon.AddFlag("not_pick")
			remove = self.__Button(self.outfitInv, x + 22, y + 3, "d:/ymir work/ui/game/costumeshop/delete_btn_0.tga", "d:/ymir work/ui/game/costumeshop/delete_btn_1.tga", "d:/ymir work/ui/game/costumeshop/delete_btn_2.tga")
			remove.SetToolTipText(localeInfo.WARDROBE_REMOVE)
			remove.SetEvent(ui.__mem_func__(self.__RemoveDraftLook), looks)
			remove.Hide()
			index = len(self.outfitSlots)
			self.outfitSlots.append((looks, hover, icon, remove))
			hover.OnMouseOverIn = lambda i = index : self.__ShowOutfitSlotToolTip(i)
			hover.OnMouseOverOut = lambda : self.__HideToolTip()

		# outfit name / apply
		header = keep(self.__Image(self, 622, 324, OUTFIT_PATH + "header.png"))
		self.__TextLine(header, 79, 15, localeInfo.WARDROBE_OUTFIT_NAME, TEXT_COLOR, vcenter = True)
		keep(self.__Image(self, 622, 354, OUTFIT_PATH + "input.png"))
		self.nameEdit = keep(ui.EditLine())
		self.nameEdit.SetParent(self)
		self.nameEdit.SetPosition(630, 360)
		self.nameEdit.SetSize(143, 18)
		self.nameEdit.SetMax(OUTFIT_NAME_MAX)
		self.nameEdit.OnIMEUpdate = ui.__mem_func__(self.__OnNameUpdate)
		self.nameEdit.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.applyButton = keep(self.__Button(self, 622, 382, OUTFIT_PATH + "btn_save.png", OUTFIT_PATH + "btn_save_hover.png", OUTFIT_PATH + "btn_save_clicked.png", localeInfo.WARDROBE_APPLY))
		self.applyButton.SetEvent(ui.__mem_func__(self.__OnApplyOutfit))

		header = keep(self.__Image(self, 622, 412, OUTFIT_PATH + "header.png"))
		self.__TextLine(header, 79, 15, localeInfo.WARDROBE_ACTIONS_TITLE, TEXT_COLOR, vcenter = True)
		self.linkButton = keep(self.__Button(self, 622, 446, OUTFIT_PATH + "btn_link.png", OUTFIT_PATH + "btn_link_hover.png", OUTFIT_PATH + "btn_link_clicked.png", localeInfo.WARDROBE_LINK_OUTFIT))
		self.linkButton.SetEvent(ui.__mem_func__(self.__CopyOutfitLink))
		self.randomButton = keep(self.__Button(self, 622, 481, OUTFIT_PATH + "btn_random.png", OUTFIT_PATH + "btn_random_hover.png", OUTFIT_PATH + "btn_random_clicked.png", localeInfo.WARDROBE_RANDOM_OUTFIT))
		self.randomButton.SetEvent(ui.__mem_func__(self.__RandomOutfit))
		self.clearButton = keep(self.__Button(self, 622, 516, OUTFIT_PATH + "btn_reset.png", OUTFIT_PATH + "btn_reset_hover.png", OUTFIT_PATH + "btn_reset_clicked.png", localeInfo.WARDROBE_CLEAR_OUTFIT))
		self.clearButton.SetEvent(ui.__mem_func__(self.__ClearOutfit))

	def Destroy(self):
		self.__HideToolTip()
		if self.renderIndex >= 0:
			renderTarget.SetVisibility(self.renderIndex, False)
		if self.petRenderIndex >= 0:
			renderTarget.SetVisibility(self.petRenderIndex, False)
		self.scrollBar.Destroy()
		self.outfitScroll.Destroy()
		self.controller = None
		self.interface = None
		self.tooltipItem = None
		self.popup = None
		self.keepAlive = []
		self.Hide()

	# --- open / close ----------------------------------------------------------------------------
	def Open(self):
		c = self.controller
		self.Show()
		self.SetTop()
		self.__RefreshProgress()
		if not c.catalogReady:
			c.RequestCatalog()
		self.SelectTab(self.tab)

	def Close(self):
		self.__FlushAutoSave()
		self.__HideToolTip()
		self.preview.EndDrag()
		self.searchEdit.KillFocus()
		self.nameEdit.KillFocus()
		self.previewKey = None
		if self.renderIndex >= 0:
			renderTarget.SetVisibility(self.renderIndex, False)
		if self.petRenderIndex >= 0:
			renderTarget.SetVisibility(self.petRenderIndex, False)
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	# --- tabs ------------------------------------------------------------------------------------
	def SelectTab(self, tab):
		if tab not in (TAB_CATALOG, TAB_OUTFITS, TAB_STORAGE):
			return
		self.__HideToolTip()
		self.tab = tab
		for (t, button) in self.navButtons.items():
			visual = "nav_tab_active.png" if t == tab else "nav_tab_normal.png"
			button.SetUpVisual(PATH + visual)
		isOutfits = (tab == TAB_OUTFITS)
		for widget in (self.typeBackground, self.contentBackground, self.content, self.scrollBar):
			widget.Show() if not isOutfits else widget.Hide()
		for button in self.typeButtons:
			button.Show() if not isOutfits else button.Hide()
		# Baerim: the owned check (and its brown band) is in the catalog only, the class filters in the catalog and the inventory
		isCatalog = (tab == TAB_CATALOG)
		for widget in (self.topBackground, self.ownedCheck, self.ownedText):
			widget.Show() if isCatalog else widget.Hide()
		for widget in [button for (button, name) in self.genderButtons] + self.raceButtons:
			widget.Show() if not isOutfits else widget.Hide()
		self.topShade.Show() if not isCatalog else self.topShade.Hide()
		if not isCatalog or not self.ownedOnly:
			self.ownedCheckMark.Hide()
		else:
			self.ownedCheckMark.Show()
		for widget in self.outfitWidgets:
			widget.Show() if isOutfits else widget.Hide()
		if isOutfits:
			if not self.controller.outfitsReady:
				self.controller.RequestOutfits()
			if self.selectedOutfit is None:
				slot = self.controller.rememberedOutfit
				self.selectedOutfit = slot if slot is not None else 0
				self.draft = self.__OutfitLooks(self.selectedOutfit)
				self.draftName = self.__OutfitName(self.selectedOutfit)
				self.draftDirty = False
			self.__RefreshOutfitList()
			self.__RefreshOutfitEditor()
		else:
			self.preview.EndDrag()
			renderTarget.SetVisibility(self.renderIndex, False)
			renderTarget.SetVisibility(self.petRenderIndex, False)
			self.petPreview.Hide()
			self.previewKey = None
			if tab == TAB_CATALOG and not self.controller.catalogReady and self.controller.CanSend():
				self.controller.RequestCatalog()
			self.scrollOffset = 0
			self.RefreshPage()

	# --- filters ---------------------------------------------------------------------------------
	def __SelectType(self, index):
		self.typeIndex = None if self.typeIndex == index else index	# a second click shows every type
		self.scrollOffset = 0
		self.__RefreshFilterButtons()
		self.RefreshPage()

	def OpenInventory(self, lookSlot = -1):
		if not self.IsShow():
			self.Open()
		self.search = ""
		self.searchEdit.SetText("")
		self.gender = None
		self.race = None
		self.ownedOnly = False
		self.typeIndex = None
		for (index, (key, slots)) in enumerate(TYPE_LIST):
			if lookSlot in slots:
				self.typeIndex = index
		self.scrollOffset = 0
		self.SelectTab(TAB_STORAGE)
		self.__RefreshFilterButtons()
		self.RefreshPage()

	def __SelectGender(self, gender):
		self.gender = None if self.gender == gender else gender
		self.__RefreshFilterButtons()
		self.RefreshPage()

	def __SelectRace(self, race):
		self.race = None if self.race == race else race
		self.__RefreshFilterButtons()
		self.RefreshPage()

	def __ToggleOwned(self):
		self.ownedOnly = not self.ownedOnly
		self.ownedCheckMark.Show() if self.ownedOnly else self.ownedCheckMark.Hide()
		self.scrollOffset = 0
		self.RefreshPage()

	def __RefreshFilterButtons(self):
		for (i, button) in enumerate(self.typeButtons):
			button.SetUpVisual(PATH + ("tab_active.png" if i == self.typeIndex else "tab.png"))
		for (gender, (button, name)) in enumerate(self.genderButtons):
			button.SetUpVisual(PATH + ("btn_%s_clicked.png" if gender == self.gender else "btn_%s.png") % name)
		suffix = 1 if self.gender == GENDER_FEMALE else 0
		for (race, button) in enumerate(self.raceButtons):
			name = RACE_IMAGES[race][suffix]
			button.SetUpVisual(PATH + ("%s_clicked.png" if race == self.race else "%s.png") % name)
			button.SetOverVisual(PATH + "%s_hover.png" % name)
			button.SetDownVisual(PATH + "%s_clicked.png" % name)

	def __OnSearchUpdate(self):
		ui.EditLine.OnIMEUpdate(self.searchEdit)
		text = self.searchEdit.GetText().strip().lower()
		self.clearSearchButton.Show() if text else self.clearSearchButton.Hide()
		if text == self.search:
			return
		self.search = text
		self.scrollOffset = 0
		if self.tab == TAB_OUTFITS:
			self.__RefreshOutfitList()
		else:
			self.RefreshPage()

	def __ClearSearch(self):
		self.searchEdit.SetText("")
		self.__OnSearchUpdate()

	def __Matches(self, look, slot):
		if self.typeIndex is not None and slot not in TYPE_LIST[self.typeIndex][1]:
			return False
		if self.search and self.search not in look.lowerName and self.search not in str(look.vnum):
			return False
		if self.gender is not None and not look.FitsGender(self.gender):
			return False
		if self.race is not None and not look.FitsJob(self.race):
			return False
		return True

	# --- catalog / storage page ------------------------------------------------------------------
	def RefreshPage(self):
		if not self.isBuilt or self.tab == TAB_OUTFITS:
			return
		c = self.controller
		rows = []		# (kind, data): ("header", text) / ("card", (vnum, slot, cell))
		if self.tab == TAB_CATALOG:
			if not c.catalogReady:
				self.__ShowRows([], localeInfo.WARDROBE_LOADING if c.pending else localeInfo.WARDROBE_CATALOG_UNAVAILABLE)
				return
			firstCategory = c.categories[0][0] if c.categories else None
			for (categoryId, categoryName) in c.categories:
				section = []
				for (vnum, rowCategory, slot) in c.catalog:
					if rowCategory != categoryId:
						continue
					look = GetLook(vnum)
					if not look or not self.__Matches(look, slot):
						continue
					if self.ownedOnly and not c.IsOwned(vnum):
						continue
					section.append(("card", (vnum, slot, -1)))
				if section:
					if categoryId != firstCategory:
						rows.append(("header", categoryName or (localeInfo.WARDROBE_CATEGORY % categoryId)))
					rows.extend(section)
			self.__ShowRows(rows, localeInfo.WARDROBE_CATALOG_COUNT % 0 if not rows else "")
		else:
			favorites = []
			for cell in player.GetWardrobeCells():
				vnum = player.GetItemIndex(player.WARDROBE, cell)
				look = GetLook(vnum)
				slot = LookSlotOfVnum(vnum)
				if look and self.__Matches(look, slot):
					(favorites if c.IsFavorite(vnum) else rows).append(("card", (vnum, slot, cell)))
			self.__ShowRows(favorites + rows, "")

	def __ShowRows(self, rows, emptyText):
		self.displayRows = rows
		# layout: section header (30) + 8 gap, cards 90x118 in 8 columns, 122 per row
		layout = []
		y = 0
		column = 0
		for (kind, data) in rows:
			if kind == "header":
				if column:
					y += CARD_STEP_Y
					column = 0
				layout.append((kind, data, 0, y))
				y += HEADER_H + HEADER_GAP
				continue
			layout.append((kind, data, CARD_X + column * CARD_STEP_X, y))
			column += 1
			if column == CARD_COLUMNS:
				column = 0
				y += CARD_STEP_Y
		if column:
			y += CARD_STEP_Y
		self.layout = layout
		self.contentHeight = y
		maxOffset = max(0, self.contentHeight - CONTENT_H)
		self.scrollOffset = max(0, min(self.scrollOffset, maxOffset))
		self.scrollBar.Show() if maxOffset > 0 and self.tab != TAB_OUTFITS else self.scrollBar.Hide()
		self.scrollBar.SetPos(float(self.scrollOffset) / maxOffset if maxOffset else 0.0)
		self.loadingText.Hide()
		self.emptyText.Hide()
		if not rows and emptyText:
			text = self.loadingText if emptyText == localeInfo.WARDROBE_LOADING else self.emptyText
			text.SetText(emptyText)
			text.Show()
		self.__RenderCards()

	def __NewCard(self):
		card = ui.Button()
		card.SetParent(self.content)
		card.SetUpVisual(PATH + "card.png")
		card.SetOverVisual(PATH + "card.png")
		card.SetDownVisual(PATH + "card.png")
		icon = ui.ExpandedImageBox()
		icon.SetParent(card)
		icon.AddFlag("not_pick")
		index = len(self.cards)
		card.SetEvent(ui.__mem_func__(self.ClickCard), index)
		card.SetOverEvent(ui.__mem_func__(self.__ShowCardToolTip), index)
		card.SetOverOutEvent(ui.__mem_func__(self.__HideToolTip))
		card.OnMouseRightButtonUp = lambda i = index : self.RightClickCard(i)
		star = ui.Button()
		star.SetParent(card)
		star.SetPosition(CARD_W - 22, 3)
		star.SetUpVisual(STAR_EMPTY)
		star.SetOverVisual(STAR_EMPTY)
		star.SetDownVisual(STAR_EMPTY)
		star.SetEvent(ui.__mem_func__(self.__ToggleFavorite), index)
		star.Hide()
		self.cardStars.append(star)
		self.cards.append([card, icon, None])
		return self.cards[-1]

	def __ToggleFavorite(self, index):
		if self.tab != TAB_STORAGE or index >= len(self.cards) or not self.cards[index][2]:
			return
		(vnum, slot, cell) = self.cards[index][2]
		if player.GetItemIndex(player.WARDROBE, cell) != vnum:
			return		# the item changed meanwhile
		if not self.controller.ToggleFavorite(vnum):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.WARDROBE_FAVORITE_SAVE_FAILED)
			return
		self.RefreshPage()

	def __NewHeader(self):
		header = ui.ExpandedImageBox()
		header.SetParent(self.content)
		header.LoadImage(PATH + "header.png")
		header.AddFlag("not_pick")
		header.SetClippingMaskWindow(self.content)
		text = ui.TextLine()
		text.SetParent(header)
		text.SetPosition(56, 15)
		text.SetVerticalAlignCenter()
		text.SetPackedFontColor(TEXT_COLOR)
		text.SetClippingMaskWindow(self.content)
		text.Show()
		self.headers.append((header, text))
		return self.headers[-1]

	def __RenderCards(self):
		c = self.controller
		cardIndex = 0
		headerIndex = 0
		for (kind, data, x, y) in self.layout:
			top = y - self.scrollOffset
			if top + (HEADER_H if kind == "header" else CARD_H) <= 0 or top >= CONTENT_H:
				continue
			if kind == "header":
				if headerIndex >= len(self.headers):
					self.__NewHeader()
				(header, text) = self.headers[headerIndex]
				headerIndex += 1
				header.SetPosition(0, top)
				text.SetText(data)
				header.Show()
				continue
			if cardIndex >= len(self.cards):
				self.__NewCard()
			entry = self.cards[cardIndex]
			cardIndex += 1
			(card, icon, _) = entry
			(vnum, slot, cell) = data
			entry[2] = data
			look = GetLook(vnum)
			if self.tab == TAB_STORAGE:
				visual = "card.png"
			elif self.__IsEditing():
				visual = "card_owned_wearing.png" if self.draft[slot] == vnum else "card.png"
			else:
				visual = "card_owned_wearing.png" if vnum in self.__WornLooks(slot) else "card.png"
			card.SetUpVisual(PATH + visual)
			card.SetOverVisual(PATH + visual)
			card.SetDownVisual(PATH + visual)
			card.SetPosition(x, top)
			card.Show()
			star = self.cardStars[cardIndex - 1]
			if self.tab == TAB_STORAGE:
				favorite = c.IsFavorite(vnum)
				visual = STAR_FULL if favorite else STAR_EMPTY
				star.SetUpVisual(visual)
				star.SetOverVisual(visual)
				star.SetDownVisual(visual)
				star.Show()
			else:
				star.Hide()
			if look and look.icon:
				icon.LoadImage(look.icon)
				iconW = icon.GetWidth()
				iconH = icon.GetHeight()
				icon.SetPosition((CARD_W - iconW) / 2, (CARD_H - iconH) / 2)
				icon.Show()
			else:
				icon.Hide()
		for i in xrange(cardIndex, len(self.cards)):
			self.cards[i][0].Hide()
			self.cards[i][2] = None
		for i in xrange(headerIndex, len(self.headers)):
			self.headers[i][0].Hide()

	def __WornLooks(self, slot):
		# what the character shows in that slot: the wardrobe look, else the worn costume / shining / pet
		looks = set()
		if self.controller.activeLooks[slot]:
			looks.add(self.controller.activeLooks[slot])
			return looks
		names = {
			LOOK_BODY : ("COSTUME_SLOT_BODY",), LOOK_HAIR : ("COSTUME_SLOT_HAIR",), LOOK_WEAPON : ("COSTUME_SLOT_WEAPON",),
			LOOK_ACCE : ("COSTUME_SLOT_ACCE",), LOOK_ACCE_SKIN : ("COSTUME_SLOT_ACCE_SKIN",), LOOK_MOUNT : ("COSTUME_SLOT_MOUNT",),
			LOOK_PET : ("COSTUME_SLOT_PET", "EQUIPMENT_PET"), LOOK_SHINING_WEAPON : ("SHINING_SLOT_WEAPON",), LOOK_SHINING_ARMOR : ("SHINING_SLOT_ARMOR",),
		}.get(slot, ())
		for name in names:
			if hasattr(item, name):
				vnum = player.GetItemIndex(getattr(item, name))
				if vnum:
					looks.add(vnum)
		return looks

	def __OnWheel(self, nLen):
		self.__ScrollTo(self.scrollOffset + (-WHEEL_STEP if nLen > 0 else WHEEL_STEP))

	def __ScrollTo(self, offset):
		maxOffset = max(0, self.contentHeight - CONTENT_H)
		self.scrollOffset = max(0, min(int(offset), maxOffset))
		self.scrollBar.SetPos(float(self.scrollOffset) / maxOffset if maxOffset else 0.0)
		self.__RenderCards()

	def __OnScrollBar(self):
		maxOffset = max(0, self.contentHeight - CONTENT_H)
		self.scrollOffset = int(self.scrollBar.GetPos() * maxOffset)
		self.__RenderCards()

	def OnRunMouseWheel(self, nLen):
		if self.tab == TAB_OUTFITS:
			if self.outfitList.IsShow():
				(mx, my) = wndMgr.GetMousePosition()
				(lx, ly) = self.outfitList.GetGlobalPosition()
				if lx <= mx < lx + self.outfitList.GetWidth() + 10 and ly <= my < ly + self.outfitList.GetHeight():
					self.__OnOutfitWheel(nLen)
					return True
			return False
		self.__OnWheel(nLen)
		return True

	# --- card actions ----------------------------------------------------------------------------
	def __IsEditing(self):
		return self.selectedOutfit is not None

	def ClickCard(self, index):
		if index < 0 or index >= len(self.cards) or not self.cards[index][2]:
			return
		if mouseModule.mouseController.isAttached():
			self.__OnDropOnContent()
			return
		(vnum, slot, cell) = self.cards[index][2]
		c = self.controller
		if self.tab == TAB_STORAGE:
			if cell < 0:
				return
			if (app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)) and (app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT)):
				if getattr(app, "ENABLE_WIKI_SYSTEM", 0) and self.interface and hasattr(self.interface, "SearchWikiForItem"):
					self.interface.SearchWikiForItem(vnum)
				return
			if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
				if getattr(app, "ENABLE_RENEWAL_SWITCHBOT", 0):
					c.MoveToSwitchbot(cell)
				return
			return		# Baerim: the costumes stay in the wardrobe inventory (right click wears them)
		look = GetLook(vnum)
		if not look:
			return
		if self.__IsEditing():
			# outfit editor: only the draft changes, nothing is equipped
			if self.draft[slot] == vnum:
				self.draft[slot] = 0
			else:
				if not look.FitsCharacter():
					return
				self.draft[slot] = vnum
				if slot == LOOK_ACCE_SKIN:
					self.draft[LOOK_ACCE] = 0
				elif slot == LOOK_ACCE:
					self.draft[LOOK_ACCE_SKIN] = 0
			self.__MarkDirty()
			self.__RenderCards()
			return
		# no outfit selected: apply / remove the look right away
		looks = list(c.activeLooks)
		if looks[slot] == vnum:
			looks[slot] = 0
		else:
			if not c.IsOwned(vnum) or not look.FitsCharacter():
				return
			looks[slot] = vnum
		c.ApplyLooks(looks)

	def RightClickCard(self, index):
		if index < 0 or index >= len(self.cards) or not self.cards[index][2]:
			return True
		(vnum, slot, cell) = self.cards[index][2]
		if self.tab == TAB_STORAGE and cell >= 0:
			self.controller.Equip(cell)
		return True

	def __OnDropOnContent(self):
		# an inventory costume dropped on the card area goes into the storage
		if self.tab != TAB_STORAGE or not mouseModule.mouseController.isAttached():
			return
		attachedType = mouseModule.mouseController.GetAttachedType()
		cell = mouseModule.mouseController.GetAttachedSlotNumber()
		if attachedType != player.SLOT_TYPE_INVENTORY:
			return
		mouseModule.mouseController.DeattachObject()
		if not self.controller.Deposit(cell):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.WARDROBE_INVALID)

	# --- tooltips --------------------------------------------------------------------------------
	def __ShowCardToolTip(self, index):
		if not self.tooltipItem or index >= len(self.cards) or not self.cards[index][2]:
			return
		(vnum, slot, cell) = self.cards[index][2]
		tooltip = self.tooltipItem
		tooltip.ClearToolTip()
		isStored = (self.tab == TAB_STORAGE and cell >= 0)
		# Baerim: name, "Kostumler: <type>", bonus lines (empty ones too), wearable, preview, shortcuts
		tooltip.deferModelPreview = True
		tooltip.showEmptyCostumeBonus = True
		tooltip.previewItemOnly = True
		if isStored:
			tooltip.SetInventoryItem(cell, player.WARDROBE)		# the real item: its bonuses and time
		else:
			tooltip.hideItemTime = True
			tooltip.SetItemToolTip(vnum, 310)
			tooltip.hideItemTime = False
		tooltip.showEmptyCostumeBonus = False
		if hasattr(tooltip, "AppendPendingModelPreview"):
			tooltip.AppendPendingModelPreview()
		tooltip.deferModelPreview = False
		tooltip.previewItemOnly = False
		rows = []
		if getattr(app, "ENABLE_OFFLINESHOP_SEARCH_SYSTEM", 0):
			rows.append((("ctrl", "mouse_right"), localeInfo.TOOLTIP_SHORTCUT_SHOP_SEARCH))
		if isStored:
			if getattr(app, "ENABLE_WIKI_SYSTEM", 0):
				rows.append((("ctrl", "alt", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_WIKI))
			if getattr(app, "ENABLE_RENEWAL_SWITCHBOT", 0):
				import constInfo
				if constInfo.CanPutItemInSwitchbot(vnum):
					rows.append((("shift", "mouse_left"), localeInfo.TOOLTIP_SHORTCUT_SWITCHBOT))
		if rows and hasattr(tooltip, "AppendShortcutTable"):
			tooltip.AppendShortcutTable(rows)
		tooltip.ShowToolTip()

	def __ShowOutfitSlotToolTip(self, index):
		vnum = self.__DraftLookOf(OUTFIT_SLOT_LAYOUT[index][0])
		if not vnum or not self.tooltipItem:
			return
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.hideItemTime = True
		self.tooltipItem.SetItemToolTip(vnum)
		self.tooltipItem.hideItemTime = False

	def __ShowProgressToolTip(self):
		if not self.tooltipItem:
			return
		self.progressHovered = True
		self.tooltipItem.ClearToolTip()
		p = self.controller.progress
		if not p:
			self.tooltipItem.AppendTextLine(localeInfo.WARDROBE_PROGRESS_LOADING)
		else:
			self.tooltipItem.AppendTextLine(localeInfo.WARDROBE_LEVEL_TOOLTIP % p[1])
			self.tooltipItem.AppendTextLine(localeInfo.WARDROBE_EXP_TOOLTIP % (p[3], p[4]))
			self.tooltipItem.AppendTextLine(localeInfo.WARDROBE_COSTUMES_TOOLTIP % p[0])
		self.tooltipItem.ShowToolTip()

	def __HideToolTip(self):
		self.progressHovered = False
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	# --- progress --------------------------------------------------------------------------------
	def __RefreshProgress(self):
		p = self.controller.progress
		if not p:
			self.levelText.SetText(localeInfo.WARDROBE_PROGRESS_LOADING)
			self.levelGauge.SetRenderingRect(0.0, 0.0, -1.0, 0.0)
			return
		(costumes, level, totalExp, expInLevel, expPerLevel) = p
		self.levelText.SetText("%s %d" % (localeInfo.WARDROBE_LEVEL_LABEL, level))
		ratio = float(expInLevel) / expPerLevel if expPerLevel > 0 else 0.0
		self.levelGauge.SetRenderingRect(0.0, 0.0, -(1.0 - max(0.0, min(1.0, ratio))), 0.0)

	# --- outfits ---------------------------------------------------------------------------------
	def __LocalOutfit(self, slot):
		if slot in self.saveQueue:
			return self.saveQueue[slot]
		if self.savingOutfit and self.savingOutfit[0] == slot:
			return self.savingOutfit[1:]
		return None

	def __OutfitName(self, slot):
		local = self.__LocalOutfit(slot)
		if local:
			return local[0]
		data = self.controller.outfits.get(slot)
		if data and data[1]:
			return data[1]
		return localeInfo.WARDROBE_OUTFIT_SLOT % (slot + 1)

	def __OutfitLooks(self, slot):
		local = self.__LocalOutfit(slot)
		if local:
			return list(local[1])
		data = self.controller.outfits.get(slot)
		if data and data[1]:
			return list(data[2])
		return [0] * LOOK_COUNT

	def __RefreshOutfitList(self):
		c = self.controller
		visible = []
		for slot in xrange(OUTFIT_MAX):
			if self.search and self.search not in self.__OutfitName(slot).lower():
				continue
			visible.append(slot)
		self.visibleOutfits = visible
		listHeight = len(visible) * 39
		maxOffset = max(0, listHeight - 447)
		self.outfitOffset = max(0, min(self.outfitOffset, maxOffset))
		for (i, (button, mask, name, mark, check)) in enumerate(self.outfitRows):
			if i >= len(visible):
				button.Hide()
				continue
			slot = visible[i]
			button.SetPosition(2, 4 + i * 39 - self.outfitOffset)
			isSelected = (slot == self.selectedOutfit)
			button.SetUpVisual(OUTFIT_PATH + ("costume_tab_active.png" if isSelected else "costume_tab.png"))
			button.SetEvent(ui.__mem_func__(self.__SelectOutfit), slot)
			name.SetText(self.__OutfitName(slot))
			isApplied = (c.appliedOutfit == slot)	# Baerim: tick = applied outfit, "!" next to it = changed since it was applied
			check.Show() if isApplied else check.Hide()
			mark.Show() if isApplied and self.__HasUnappliedChanges(slot) else mark.Hide()
			button.Show()
		self.outfitScroll.SetPos(float(self.outfitOffset) / maxOffset if maxOffset else 0.0)
		if self.tab == TAB_OUTFITS and maxOffset > 0:
			self.outfitScroll.Show()
		else:
			self.outfitScroll.Hide()

	def __HasUnappliedChanges(self, slot):
		looks = self.draft if slot == self.selectedOutfit else self.__OutfitLooks(slot)
		return list(looks) != list(self.controller.activeLooks)

	def __OnOutfitWheel(self, nLen):
		self.outfitOffset += -39 if nLen > 0 else 39
		self.__RefreshOutfitList()

	def __OnOutfitScroll(self):
		maxOffset = max(0, len(getattr(self, "visibleOutfits", [])) * 39 - 447)
		self.outfitOffset = int(self.outfitScroll.GetPos() * maxOffset)
		self.__RefreshOutfitList()

	def __SelectOutfit(self, slot):
		self.__FlushAutoSave()
		if self.selectedOutfit == slot:
			return
		self.selectedOutfit = slot
		self.draft = self.__OutfitLooks(slot)
		self.draftName = self.__OutfitName(slot)
		self.draftDirty = False
		self.__RefreshOutfitList()
		self.__RefreshOutfitEditor()

	def __RefreshOutfitEditor(self):
		editing = self.__IsEditing()
		for line in self.outfitHelp:
			line.Hide()
		if self.tab == TAB_OUTFITS:
			if not editing:
				self.outfitHelp[0].Show()
		self.nameEdit.SetText(self.draftName if editing else "")
		for widget in (self.nameEdit, self.applyButton, self.linkButton, self.randomButton, self.clearButton):
			if self.tab == TAB_OUTFITS and editing:
				widget.Show()
			else:
				widget.Hide()
		self.__RefreshApplyButton()
		self.__RefreshOutfitSlots()
		self.__UpdatePreview()

	def __RefreshApplyButton(self):
		if not self.__IsEditing():
			return
		c = self.controller
		unapply = (c.appliedOutfit == self.selectedOutfit and not self.draftDirty and list(self.draft) == list(c.activeLooks))
		self.applyButton.SetText(localeInfo.WARDROBE_UNAPPLY if unapply else localeInfo.WARDROBE_APPLY)

	def __DraftLookOf(self, looks):
		source = self.draft if self.__IsEditing() else self.controller.activeLooks
		for slot in looks:
			if source[slot]:
				return source[slot]
		return 0

	def __RefreshOutfitSlots(self):
		editing = self.__IsEditing()
		for (index, (looks, hover, icon, remove)) in enumerate(self.outfitSlots):
			icon.Hide()		# the slot window draws the icon (the activation frame is drawn over it)
			vnum = self.__DraftLookOf(looks)
			look = GetLook(vnum)
			if look and look.icon:
				self.lookSlots.SetItemSlot(index, vnum, 0)
				# Baerim: every filled slot has the turning highlight, white = owned, red = not owned
				self.lookSlots.ActivateSlot(index, wndMgr.COLOR_TYPE_WHITE if self.controller.IsOwned(vnum) else wndMgr.COLOR_TYPE_RED)
				remove.Show() if editing else remove.Hide()
			else:
				self.lookSlots.ClearSlot(index)
				self.lookSlots.DeactivateSlot(index)
				remove.Hide()
		self.lookSlots.RefreshSlot()

	def __RemoveDraftLook(self, looks):
		if not self.__IsEditing():
			return
		for slot in looks:
			self.draft[slot] = 0
		self.__MarkDirty()

	def __MarkDirty(self):
		self.draftDirty = True
		self.autoSaveDue = app.GetTime() + AUTOSAVE_DELAY
		if self.tab == TAB_OUTFITS:
			self.__RefreshOutfitSlots()
			self.__UpdatePreview()
			self.__RefreshApplyButton()
			self.__RefreshOutfitList()

	def __OnNameUpdate(self):
		ui.EditLine.OnIMEUpdate(self.nameEdit)
		if not self.__IsEditing():
			return
		name = self.nameEdit.GetText()
		if name != self.draftName:
			self.draftName = name
			self.__MarkDirty()

	def __FlushAutoSave(self):
		# Baerim: changes are saved automatically (3 s after the last edit, or before switching / closing)
		if not self.__IsEditing() or not self.draftDirty:
			return True
		name = self.draftName.strip() or (localeInfo.WARDROBE_OUTFIT_SLOT % (self.selectedOutfit + 1))
		self.saveQueue[self.selectedOutfit] = (name, list(self.draft))
		self.draftDirty = False
		self.autoSaveDue = 0.0
		self.__PumpRequests()
		return True

	def __PumpRequests(self):
		# one request at a time: queued saves first, then the apply
		c = self.controller
		if not c or not c.CanSend():
			return
		if self.saveQueue:
			if not c.outfitsReady:
				c.RequestOutfits()
				return
			slot = sorted(self.saveQueue.keys())[0]
			(name, looks) = self.saveQueue[slot]
			del self.saveQueue[slot]		# a save the controller refuses here (bad name) is dropped, it would block the queue
			if c.SaveOutfit(slot, name, looks):
				self.savingOutfit = (slot, name, list(looks))
			return
		if self.applyQueue and not self.savingOutfit:
			(looks, slot) = self.applyQueue
			if c.ApplyLooks(looks, slot):
				self.applyQueue = None

	def __OnApplyOutfit(self):
		if not self.__IsEditing():
			return
		c = self.controller
		slot = self.selectedOutfit
		if c.appliedOutfit == slot and not self.draftDirty and list(self.draft) == list(c.activeLooks):
			self.applyQueue = ([0] * LOOK_COUNT, None)		# Cikar: back to the equipment look
			self.__PumpRequests()
			return
		for vnum in self.draft:
			if vnum and not c.IsOwned(vnum):
				self.__Popup(localeInfo.WARDROBE_UNOWNED_OUTFIT)
				return
		if slot not in c.outfits or not c.outfits[slot][1]:
			self.draftDirty = True		# the server applies saved outfits only
		self.__FlushAutoSave()
		self.applyQueue = (list(self.draft), slot)
		self.__PumpRequests()

	def __ClearOutfit(self):
		if self.__IsEditing():
			self.draft = [0] * LOOK_COUNT
			self.__MarkDirty()

	def __RandomOutfit(self):
		if not self.__IsEditing():
			return
		c = self.controller
		if not c.catalogReady:
			c.RequestCatalog()
			return
		bySlot = {}
		for (vnum, category, slot) in c.catalog:
			look = GetLook(vnum)
			if look and c.IsOwned(vnum) and look.FitsCharacter():
				bySlot.setdefault(slot, []).append(vnum)
		draft = [0] * LOOK_COUNT
		for (slot, vnums) in bySlot.items():
			if slot == LOOK_ACCE and LOOK_ACCE_SKIN in bySlot:
				continue
			draft[slot] = vnums[app.GetRandom(0, len(vnums) - 1)]
		self.draft = draft
		self.__MarkDirty()

	def __CopyOutfitLink(self):
		looks = self.draft if self.__IsEditing() else self.controller.activeLooks
		text = "outfit:1:" + ":".join(["%d" % v for v in looks])
		copied = False
		try:
			if hasattr(app, "SetClipboardText"):
				app.SetClipboardText(text)
				copied = True
		except Exception:
			copied = False
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.WARDROBE_LINK_COPIED if copied else localeInfo.WARDROBE_LINK_COPY_FAILED)

	def __Popup(self, text):
		popup = uiCommon.PopupDialog()
		popup.SetText(text)
		(textWidth, textHeight) = popup.GetTextSize()
		if textWidth + 40 > popup.GetWidth():
			popup.SetWidth(textWidth + 40)	# the dialog grows with the text (it used to overflow)
		popup.Open()
		self.popup = popup

	# --- preview ---------------------------------------------------------------------------------
	def __UpdatePreview(self):
		if self.tab != TAB_OUTFITS or self.renderIndex < 0 or not self.IsShow():
			return
		looks = self.draft if self.__IsEditing() else self.controller.activeLooks
		race = player.GetRace()
		key = (race, tuple(looks), self.__WornArmor(), player.GetMainCharacterPart(PART_WEAPON), player.GetMainCharacterPart(PART_HAIR), player.GetMainCharacterPart(PART_ACCE),
			player.GetItemIndex(item.SHINING_SLOT_WEAPON) if hasattr(item, "SHINING_SLOT_WEAPON") else 0, player.GetItemIndex(item.SHINING_SLOT_ARMOR) if hasattr(item, "SHINING_SLOT_ARMOR") else 0)
		if key == self.previewKey:
			return
		self.previewKey = key
		renderTarget.ResetModel(self.renderIndex)
		renderTarget.SelectModel(self.renderIndex, race)
		renderTarget.SetScale(self.renderIndex, PREVIEW_SCALE)
		renderTarget.SetVisibility(self.renderIndex, True)
		# order like the costume shop: weapon, hair, armour, sash (after the armour), shinings, mount
		renderTarget.SetWeapon(self.renderIndex, looks[LOOK_WEAPON] or player.GetMainCharacterPart(PART_WEAPON))
		hair = GetLook(looks[LOOK_HAIR])
		if hair:
			renderTarget.SetHair(self.renderIndex, hair.vnum, False)
		else:
			renderTarget.SetHair(self.renderIndex, player.GetMainCharacterPart(PART_HAIR), True)
		renderTarget.SetArmor(self.renderIndex, looks[LOOK_BODY] or self.__WornArmor())
		acce = looks[LOOK_ACCE_SKIN] or looks[LOOK_ACCE]
		if acce:
			renderTarget.SetAcce(self.renderIndex, acce - ACCE_BASE_VNUM)
		else:
			worn = player.GetMainCharacterPart(PART_ACCE)
			renderTarget.SetAcce(self.renderIndex, worn - ACCE_BASE_VNUM if worn >= ACCE_BASE_VNUM else worn)
		if hasattr(renderTarget, "SetShining"):
			renderTarget.SetShining(self.renderIndex, SHINING_WEAPON, looks[LOOK_SHINING_WEAPON] or player.GetItemIndex(item.SHINING_SLOT_WEAPON))
			renderTarget.SetShining(self.renderIndex, SHINING_ARMOR, looks[LOOK_SHINING_ARMOR] or player.GetItemIndex(item.SHINING_SLOT_ARMOR))
		mount = GetLook(looks[LOOK_MOUNT])
		if mount and mount.mountVnum:
			renderTarget.SetMount(self.renderIndex, mount.mountVnum)
		pet = GetLook(looks[LOOK_PET])
		if pet and pet.PetMob():
			renderTarget.ResetModel(self.petRenderIndex)
			renderTarget.SelectModel(self.petRenderIndex, pet.PetMob())
			renderTarget.SetVisibility(self.petRenderIndex, True)
			self.petPreview.Show()
		else:
			renderTarget.SetVisibility(self.petRenderIndex, False)
			self.petPreview.Hide()

	def __WornArmor(self):
		active = self.controller.activeLooks[LOOK_BODY]
		if active:
			return active
		costume = player.GetItemIndex(item.COSTUME_SLOT_BODY)
		if costume:
			return costume
		return player.GetItemIndex(item.EQUIPMENT_BODY)

	# --- controller events -----------------------------------------------------------------------
	def OnControllerChanged(self, what):
		if not self.isBuilt:
			return
		if what == "progress":
			self.__RefreshProgress()
			return
		if self.tab == TAB_OUTFITS:
			if what == "outfits" and self.__IsEditing() and not self.draftDirty and not self.__LocalOutfit(self.selectedOutfit):
				self.draft = self.__OutfitLooks(self.selectedOutfit)
				self.draftName = self.__OutfitName(self.selectedOutfit)
			self.__RefreshOutfitList()
			self.__RefreshOutfitEditor()
		else:
			self.RefreshPage()

	def OnResult(self, action, result):
		c = self.controller
		if action == "save" and self.savingOutfit:
			(slot, name, looks) = self.savingOutfit
			self.savingOutfit = None
			if result == RESULT_OK:
				revision = c.outfits[slot][0] + 1 if slot in c.outfits else 1
				c.outfits[slot] = [revision, name, looks]
				c.rememberedOutfit = slot
			else:
				if slot not in self.saveQueue:
					self.saveQueue[slot] = (name, looks)		# sent again (after a refresh when the revision was old)
				if result != RESULT_BUSY:
					c.outfitsReady = False
		elif action == "apply" and result == RESULT_BUSY and self.applyQueue is None:
			self.applyQueue = (list(c.lastApplyLooks), c.applySlot) if hasattr(c, "lastApplyLooks") else None
		elif action in ("deposit", "withdraw") and result == RESULT_OK:
			pass
		if result != RESULT_OK:
			key = { RESULT_FULL : "WARDROBE_FULL", RESULT_BLOCKED : "WARDROBE_BLOCKED", RESULT_UNOWNED : "WARDROBE_UNOWNED_OUTFIT",
				RESULT_INVALID : "WARDROBE_INVALID_SELECTION", "timeout" : "WARDROBE_TIMEOUT" }.get(result, "WARDROBE_ERROR")
			if result != RESULT_BUSY and not (action == "equip" and result == RESULT_ERROR):
				chat.AppendChat(chat.CHAT_TYPE_INFO, _Text(key, localeInfo.WARDROBE_ERROR))
		self.OnControllerChanged("result")

	def OnUpdate(self):
		c = self.controller
		if not c:
			return
		c.Update()
		if self.__IsEditing() and self.draftDirty and self.autoSaveDue and app.GetTime() >= self.autoSaveDue:
			self.__FlushAutoSave()
		self.__PumpRequests()
