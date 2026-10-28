# -*- coding: utf-8 -*-
# wikipages.py - Anka2 wiki page widgets (reference uiWiki parity, Phase 3)
#
# ARCHITECTURE NOTE
#   * Data layer  : module level functions (Get*/Build*) that normalise whatever the native
#                   `wiki` module offers into plain python structures. Every native call that
#                   the current Anka client does NOT export is reached through getattr(), so a
#                   missing C++ function degrades to "empty list" instead of an AttributeError.
#   * Widget layer: small classes (WikiItemSlot, WikiClippedItemIcon, DropDown, Category*,
#                   Equipment/Refine, Obtention list boxes, Monster/Drop pages, SpecialClass)
#                   built on WikiUI.DefaultWikiImage / DefaultWikiWindow so the existing
#                   RenderWindow clipping and constInfo.GetListBox() plumbing keeps working.
#   * No import of uiwiki here (uiwiki imports us) -> no import cycle.
#   * Python 2.7, tabs, ASCII only (locale strings come from localeInfo with fallbacks).

import ui
import wiki
import item
import nonplayer
import renderTarget
import constInfo
import localeInfo
import WikiUI

TRUE = True
FALSE = False

IMG_DIR = "d:/ymir work/ui/game/wiki/"
IMG_CATEGORY = IMG_DIR + "category/"
IMG_SLOT = IMG_DIR + "slot/"
NO_ICON = "icon/item/27995.tga"

# amberlight metin family: special stones (+0 grade bases 28046/28048/28049, +100 per grade, 5 grades)
AMBERLIGHT_MOBS = (8870, 8318, 8418)
AMBERLIGHT_STONE_BASES = (28046, 28048, 28049)
AMBERLIGHT_RANDOM_STONE = 94150

def _L(name, default):
	return getattr(localeInfo, name, default)

def NativeCall(name, default, *args):
	fn = getattr(wiki, name, None)
	if fn is None:
		return default
	try:
		return fn(*args)
	except:
		return default

# ---------------------------------------------------------------------------
# data layer
# ---------------------------------------------------------------------------

def GetItemIconPath(vnum):
	item.SelectItem(vnum)
	path = item.GetIconImageFileName()
	if path.find("gr2") != -1:
		return NO_ICON
	return path

def GetWikiRefineMaxLevel(vnum):
	return NativeCall("GetRefineMaxLevel", 0, vnum)

def GetWikiItemBaseVnum(vnum):
	# vnum of a refine chain is given as the max level vnum (+9) by the category lists
	maxLevel = GetWikiRefineMaxLevel(vnum)
	if maxLevel > 0:
		return vnum - maxLevel
	return vnum

def IsWikiRefineCategoryItem(vnum):
	return GetWikiRefineMaxLevel(vnum) > 0

def IsEnigmaWikiItem(vnum):
	fn = getattr(constInfo, "IsEnigmaItem", None)
	if fn is None:
		return FALSE
	try:
		return fn(vnum)
	except:
		return FALSE

def IsEquipmentObtentionPageItem(vnum):
	item.SelectItem(vnum)
	t = item.GetItemType()
	return t == item.ITEM_TYPE_WEAPON or t == item.ITEM_TYPE_ARMOR

def GetWikiItemMobSources(lookupVnum, selectedVnum, isRefineItem):
	# native result is copied (never mutated); amberlight family gets its 3 metin variants appended
	vnum = selectedVnum if selectedVnum else lookupVnum
	native = NativeCall("GetItemDropFromMonster", [], vnum, isRefineItem)
	if isinstance(native, (list, tuple)):
		sources = list(native)
	elif native:
		sources = [native]
	else:
		sources = []
	for base in AMBERLIGHT_STONE_BASES:
		for grade in xrange(5):
			if vnum == base + grade * 100:
				for mob in AMBERLIGHT_MOBS:
					if mob not in sources:
						sources.append(mob)
				return sources
	return sources

def GetStoneExtraNames(vnum, sourceMobVnum):
	# amberlight random stone: tooltip lists the 3 special stones that can also drop
	if vnum != AMBERLIGHT_RANDOM_STONE or sourceMobVnum not in AMBERLIGHT_MOBS:
		return []
	return [item.GetItemNameByVnum(v) for v in AMBERLIGHT_STONE_BASES]

def BuildRefineTable(baseVnum, maxLevel, alternative = 0):
	# -> list (maxLevel+1) of dicts. Native `GetRefineAlternativeSets(vnum, level)` (optional) yields
	#   several refine set ids per level; without it there is exactly one alternative.
	table = []
	for level in xrange(maxLevel + 1):
		refineSet = 0
		selected = FALSE
		select = getattr(item, "SelectItemWiki", None) or item.SelectItem
		try:
			selected = select(baseVnum + level)
		except:
			selected = FALSE
		if selected != 0:
			sets = NativeCall("GetRefineAlternativeSets", None, baseVnum + level)
			if sets:
				if alternative < len(sets):
					refineSet = sets[alternative]
			elif alternative == 0:
				refineSet = item.GetRefineSet()
		row = {"id": 0, "cost": 0, "prob": 0, "refine_count": 0}
		for i in xrange(5):
			row["item_vnum_%d" % i] = 0
			row["item_count_%d" % i] = 0
		argv = NativeCall("GetRefineItems", 0, refineSet) if refineSet else 0
		if argv:
			row["id"] = int(argv[0])
			for i in xrange(5):
				row["item_vnum_%d" % i] = int(argv[1 + i * 2])
				row["item_count_%d" % i] = int(argv[2 + i * 2])
			row["cost"] = long(argv[11])
			row["prob"] = long(argv[12])
			row["refine_count"] = int(argv[13])
		table.append(row)
	return table

def GetRefineAlternativeCount(baseVnum, maxLevel):
	best = 1
	for level in xrange(maxLevel + 1):
		sets = NativeCall("GetRefineAlternativeSets", None, baseVnum + level)
		if sets and len(sets) > best:
			best = len(sets)
	return best

def GetMountRuneRefines(vnum):
	# native (optional): wiki.GetMountRuneRefines(vnum) -> [(runeVnum, energy, stoneVnum, stoneCount), ...]
	rows = NativeCall("GetMountRuneRefines", [], vnum)
	out = []
	for r in rows or []:
		out.append({"rune": r[0], "energy": r[1], "stone": r[2], "stoneCount": r[3]})
	return out

def GetPetEvolution(vnum):
	# native (optional): wiki.GetPetEvolutionData(vnum) -> [(stage, level, itemVnum, itemCount), ...]
	rows = NativeCall("GetPetEvolutionData", [], vnum)
	return [{"stage": r[0], "level": r[1], "vnum": r[2], "count": r[3]} for r in rows or []]

def _NormalizeRecipes(rows):
	out = []
	for r in rows or []:
		# (npcVnum, gold, [(vnum,count)...materials], [(vnum,count)...results], [(vnum,count)...increase])
		inc = r[4] if len(r) > 4 else []
		out.append({"npc": r[0], "gold": r[1], "materials": list(r[2]), "results": list(r[3]), "increase": list(inc)})
	return out

def GetItemCubeRecipes(vnum):
	# native (optional): wiki.GetItemCubeRecipes(vnum)
	return _NormalizeRecipes(NativeCall("GetItemCubeRecipes", [], vnum))

def GetItemCraftRecipes(vnum):
	# native (optional): wiki.GetItemCraftRecipes(vnum)
	return _NormalizeRecipes(NativeCall("GetItemCraftRecipes", [], vnum))

def GetItemShopOffers(vnum):
	# native (optional): wiki.GetItemShopOffers(vnum) -> [(npcVnum, price, currencyVnum(0=yang), count)]
	rows = NativeCall("GetItemShopOffers", [], vnum)
	return [{"npc": r[0], "price": r[1], "currency": r[2], "count": r[3]} for r in rows or []]

def MoneyText(value):
	fn = getattr(localeInfo, "MoneyFormat", None)
	if fn:
		return fn(value)
	return str(value)

# ---------------------------------------------------------------------------
# low level helpers
# ---------------------------------------------------------------------------

def _SetupChild(w):
	w.itsRendered = FALSE
	w.itsNeedDoubleRender = FALSE
	return w

def _LoadIcon(box, vnum):
	box.LoadImage(GetItemIconPath(vnum))

def _TextLine(parent, text, x, y, center = FALSE):
	t = ui.TextLine()
	t.SetParent(parent)
	t.SetText(text)
	t.SetPosition(x, y)
	if center:
		t.SetHorizontalAlignCenter()
	_SetupChild(t)
	t.Show()
	return t

def _TryLoad(box, path, fallbackPath = None):
	try:
		box.LoadImage(path)
		return TRUE
	except:
		if fallbackPath:
			try:
				box.LoadImage(fallbackPath)
				return TRUE
			except:
				pass
	return FALSE

def ShowItemInfoOnParent(vnum, argType = 0):
	parent = constInfo.GetMainParent()
	if parent != None:
		parent.ShowItemInfo(vnum, argType)

def ShowItemToolTip(tooltip, vnum, sourceMobVnum = 0):
	tooltip.SetItemToolTip(vnum)
	names = GetStoneExtraNames(vnum, sourceMobVnum)
	if names:
		tooltip.AppendSpace(5)
		tooltip.AppendTextLine(_L("WIKI_STONES_ALSO_AVAILABLE", "Also available:"))
		for n in names:
			tooltip.AppendTextLine(n + "+0")
		tooltip.ResizeToolTip()
	tooltip.ShowToolTip()

# ---------------------------------------------------------------------------
# item slots
# ---------------------------------------------------------------------------

class WikiItemSlot(ui.ExpandedImageBox):
	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		_SetupChild(self)
		self.vnum = 0
		self.count = 0
		self.sourceMob = 0
		self.countText = None
		self.tooltipItem = WikiUI.DefaultWikiImage.tooltipItem
		self.onClick = None

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def Destroy(self):
		self.__OverOutItem()
		self.countText = None
		self.onClick = None
		self.vnum = 0

	def SetItem(self, vnum, count = 0, sourceMobVnum = 0):
		self.vnum = vnum
		self.count = count
		self.sourceMob = sourceMobVnum
		_LoadIcon(self, vnum)
		self.SAFE_SetStringEvent("MOUSE_OVER_IN", self.__OverInItem)
		self.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.__OverOutItem)
		self.SetEvent(ui.__mem_func__(self.__SelectItem), "mouse_click", 0)
		if count > 1:
			if self.countText == None:
				self.countText = ui.TextLine()
				self.countText.SetParent(self)
				_SetupChild(self.countText)
			self.countText.SetText(str(count))
			self.countText.SetPosition(self.GetWidth() - 8 - 6 * len(str(count)), self.GetHeight() - 14)
			self.countText.Show()
		elif self.countText != None:
			self.countText.Hide()

	def SetClickEvent(self, func):
		self.onClick = func

	def AppendTo(self, owner):
		# register us (+ count text) in the owner's render list
		owner.children.append(self)
		if self.countText != None:
			owner.children.append(self.countText)

	def __OverInItem(self):
		if self.tooltipItem and self.vnum:
			ShowItemToolTip(self.tooltipItem, self.vnum, self.sourceMob)

	def __OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def __SelectItem(self, *args):
		self.__OverOutItem()
		if self.onClick:
			self.onClick(self.vnum)
		else:
			ShowItemInfoOnParent(self.vnum, 0)

	def __UnselectItem(self):
		pass

	def __SelectItemUnused(self):
		pass

class WikiClippedItemIcon(WikiItemSlot):
	# slot whose visible part is clipped to a vertical window range (global y coords)
	def __init__(self):
		WikiItemSlot.__init__(self)
		self.clipTop = None
		self.clipBottom = None

	def SetClip(self, top, bottom):
		self.clipTop = top
		self.clipBottom = bottom
		self.ApplyClip()

	def ApplyClip(self):
		if self.clipTop == None:
			return
		(x, y) = self.GetGlobalPosition()
		h = self.GetHeight()
		topCut = 0
		bottomCut = 0
		if y + h < self.clipTop or y > self.clipBottom:
			self.Hide()
			return
		if y < self.clipTop:
			topCut = self.clipTop - y
		if y + h > self.clipBottom:
			bottomCut = (y + h) - self.clipBottom
		if topCut or bottomCut:
			self.SetRenderingRect(0, WikiUI.calculateRect(h - topCut, h), 0, WikiUI.calculateRect(h - bottomCut, h))
			self.itsRendered = TRUE
		elif self.itsRendered:
			self.SetRenderingRect(0, 0, 0, 0)
			self.itsRendered = FALSE
		if not self.IsShow():
			self.Show()

	def OnRender(self):
		self.ApplyClip()

# ---------------------------------------------------------------------------
# list entries: ItemTitle / Item / CostumeGroupTitle
# ---------------------------------------------------------------------------

class ItemTitle(WikiUI.DefaultWikiWindow):
	def __init__(self, text = "", width = 540):
		WikiUI.DefaultWikiWindow.__init__(self)
		self.SetSize(width, 24)
		self.text = _TextLine(self, text, 8, 4)
		self.children.append(self.text)
		self.divider = ui.ExpandedImageBox()
		self.divider.SetParent(self)
		_TryLoad(self.divider, IMG_DIR + "divider_long.tga")
		self.divider.SetPosition(0, 20)
		_SetupChild(self.divider)
		self.divider.Show()
		self.children.append(self.divider)

	def SetParent(self, parent):
		WikiUI.DefaultWikiWindow.SetParent(self, parent)

	def SetSize(self, width, height = 24):
		WikiUI.DefaultWikiWindow.SetSize(self, width, height)

	def OnMouseLeftButtonDown(self):
		pass

class CostumeGroupTitle(ItemTitle):
	# group header of the costume page (4 icons per row, CanAddNewItem tells when to start a new row)
	def __init__(self, text = "", width = 540):
		ItemTitle.__init__(self, text, width)
		self.vnumList = []
		self.SetSize(width, 24 + 4 * 0)

	def CanAddNewItem(self):
		return len(self.vnumList) < 4

	def AddItem(self, vnum):
		slot = WikiItemSlot()
		slot.SetParent(self)
		slot.SetItem(vnum)
		slot.SetPosition(8 + len(self.vnumList) * 40, 26)
		slot.Show()
		slot.AppendTo(self)
		self.vnumList.append(vnum)
		self.SetSize(self.GetWidth(), 26 + slot.GetHeight() + 4)

class Item(WikiUI.DefaultWikiWindow):
	# one row of a result list: icon + name (+ optional level), click -> item page
	def __init__(self, vnum = 0, width = 540):
		WikiUI.DefaultWikiWindow.__init__(self)
		self.vnum = vnum
		self.SetSize(width, 40)
		self.slot = WikiItemSlot()
		self.slot.SetParent(self)
		self.slot.SetItem(vnum)
		self.slot.SetPosition(4, 4)
		self.slot.Show()
		self.slot.AppendTo(self)
		item.SelectItem(vnum)
		self.nameText = _TextLine(self, item.GetItemName(), 48, 12)
		self.children.append(self.nameText)

	def SetSize(self, width, height = 40):
		WikiUI.DefaultWikiWindow.SetSize(self, width, height)

	def OnMouseLeftButtonDown(self):
		ShowItemInfoOnParent(self.vnum, 0)

# ---------------------------------------------------------------------------
# DropDown (search results)
# ---------------------------------------------------------------------------

class _DropRow(ui.Window):
	def __init__(self, owner, text, vnum, kind, width, isTitle):
		ui.Window.__init__(self)
		self.owner = owner
		self.vnum = vnum
		self.kind = kind
		self.isTitle = isTitle
		self.SetSize(width, 18)
		self.bar = ui.Bar()
		self.bar.SetParent(self)
		self.bar.SetSize(width, 18)
		self.bar.SetColor(0x00000000)
		self.bar.AddFlag("not_pick")
		self.bar.Show()
		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.AddFlag("not_pick")
		if isTitle:
			self.text.SetPackedFontColor(0xffcebb98)
			self.text.SetText(text)
			self.text.SetPosition(4, 2)
		else:
			self.text.SetText(text)
			self.text.SetPosition(12, 2)
		self.text.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def OnMouseOverIn(self):
		if not self.isTitle:
			self.bar.SetColor(0x40ffffff)

	def OnMouseOverOut(self):
		self.bar.SetColor(0x00000000)

	def OnMouseLeftButtonUp(self):
		if not self.isTitle:
			self.owner.OnSelectRow(self.vnum, self.kind)

class DropDown(ui.Window):
	# AppendItemTitle(title) / AppendItem(name, vnum, kind) / Clear() / ExpandMe() / dropped
	VIEW_ROWS = 10

	def __init__(self):
		ui.Window.__init__(self)
		self.rows = []
		self.basePos = 0
		self.dropped = 0
		self.selectEvent = None
		self.rowWidth = 180
		self.back = ui.Bar()
		self.back.SetParent(self)
		self.back.SetColor(0xf0101010)
		self.back.AddFlag("not_pick")
		self.back.Hide()

	def __del__(self):
		ui.Window.__del__(self)

	def SetRowWidth(self, width):
		self.rowWidth = width

	def SetSelectEvent(self, func):
		self.selectEvent = func

	def Clear(self):
		for r in self.rows:
			r.Hide()
		self.rows = []
		self.basePos = 0
		self.__Layout()

	def AppendItemTitle(self, title):
		self.rows.append(_DropRow(self, title, 0, 0, self.rowWidth, TRUE))
		self.__Layout()

	def AppendItem(self, name, vnum, kind):
		self.rows.append(_DropRow(self, name, vnum, kind, self.rowWidth, FALSE))
		self.__Layout()

	def OnSelectRow(self, vnum, kind):
		self.Collapse()
		if self.selectEvent:
			self.selectEvent(vnum, kind)

	def ExpandMe(self):
		if self.dropped:
			self.Collapse()
		else:
			self.dropped = 1
			self.__Layout()

	def Collapse(self):
		self.dropped = 0
		self.__Layout()

	def OnMouseWheel(self, nLen):
		if not self.dropped:
			return FALSE
		maxBase = max(0, len(self.rows) - self.VIEW_ROWS)
		if nLen > 0:
			self.basePos = max(0, self.basePos - 1)
		else:
			self.basePos = min(maxBase, self.basePos + 1)
		self.__Layout()
		return TRUE

	def __Layout(self):
		for r in self.rows:
			r.Hide()
		if not self.dropped or len(self.rows) == 0:
			self.back.Hide()
			self.SetSize(self.rowWidth, 1)
			return
		visible = self.rows[self.basePos:self.basePos + self.VIEW_ROWS]
		self.SetSize(self.rowWidth, 18 * len(visible))
		self.back.SetSize(self.rowWidth, 18 * len(visible))
		self.back.Show()
		y = 0
		for r in visible:
			r.SetParent(self)
			r.SetPosition(0, y)
			r.Show()
			y += 18

# ---------------------------------------------------------------------------
# Category widgets (data driven by wiki_categories.json style dicts)
#   category = {"id":, "name":, "btn": <icon name>, "items": [{"id":, "name":, ...}]}
# ---------------------------------------------------------------------------

CATEGORY_PITCH = 40
SUB_PITCH = 26
CATEGORY_W = 199

class CategoryDefaultItem(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.expanded = FALSE
		self.offset = 0
		self.onExpand = None
		self.onCollapse = None
		self.onSelect = None
		self.clipWindow = None
		self.children = {}
		self.selected = FALSE

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.children = {}
		self.onExpand = None
		self.onCollapse = None
		self.onSelect = None
		self.clipWindow = None

	def SetParent(self, parent):
		ui.Window.SetParent(self, parent)

	def SetEvent(self, func, *args):
		self.onSelect = (func, args)

	def SetOnExpandEvent(self, func):
		self.onExpand = func

	def SetOnCollapseEvent(self, func):
		self.onCollapse = func

	def SetOffset(self, offset):
		self.offset = offset

	def GetOffset(self):
		return self.offset

	def SetClippingMaskWindow(self, window):
		self.clipWindow = window

	def IsExpanded(self):
		return self.expanded

	def Expand(self):
		self.expanded = TRUE
		if self.onExpand:
			self.onExpand()

	def Collapse(self):
		self.expanded = FALSE
		if self.onCollapse:
			self.onCollapse()

	def OnSelect(self):
		if self.onSelect:
			self.onSelect[0](*self.onSelect[1])

	def OnMouseLeftButtonDown(self):
		self.OnSelect()

class CategoryItem(CategoryDefaultItem):
	def __init__(self, text, iconName = None):
		CategoryDefaultItem.__init__(self)
		self.SetSize(CATEGORY_W, 38)
		bg = ui.ExpandedImageBox()
		bg.SetParent(self)
		bg.LoadImage(IMG_CATEGORY + "btn_norm.png")
		bg.AddFlag("not_pick")
		_SetupChild(bg)
		bg.Show()
		self.children["bg"] = bg
		if iconName:
			icon = ui.ExpandedImageBox()
			icon.SetParent(self)
			if _TryLoad(icon, IMG_CATEGORY + "icons/%s.png" % iconName):
				icon.SetPosition(10, (38 - icon.GetHeight()) / 2)
				icon.AddFlag("not_pick")
				_SetupChild(icon)
				icon.Show()
				self.children["miniIcon"] = icon
		label = ui.TextLine()
		label.SetParent(self)
		label.SetText(text)
		label.SetPosition(52, 12)
		label.AddFlag("not_pick")
		_SetupChild(label)
		label.Show()
		self.children["textLine"] = label
		arrow = ui.ExpandedImageBox()
		arrow.SetParent(self)
		if _TryLoad(arrow, IMG_DIR + "down.tga"):
			arrow.SetPosition(CATEGORY_W - 28, 15)
			arrow.AddFlag("not_pick")
			_SetupChild(arrow)
			arrow.Show()
			self.children["arrowIcon"] = arrow

	def SetBtn(self, state):
		# state: norm / hover / select
		self.children["bg"].LoadImage(IMG_CATEGORY + "btn_%s.png" % state)

	def OnMouseOverIn(self):
		if not self.selected:
			self.SetBtn("hover")

	def OnMouseOverOut(self):
		self.SetBtn("select" if self.selected else "norm")

	def ExpandEvent(self):
		self.selected = TRUE
		self.SetBtn("select")

	def CollapseEvent(self):
		self.selected = FALSE
		self.SetBtn("norm")

	def Expand(self):
		CategoryDefaultItem.Expand(self)
		self.ExpandEvent()

	def Collapse(self):
		CategoryDefaultItem.Collapse(self)
		self.CollapseEvent()

	def OnMouseLeftButtonDown(self):
		if self.expanded:
			self.Collapse()
		else:
			self.Expand()
		self.OnSelect()

class CategorySubItem(CategoryDefaultItem):
	def __init__(self, text):
		CategoryDefaultItem.__init__(self)
		self.SetSize(CATEGORY_W, 24)
		bg = ui.ExpandedImageBox()
		bg.SetParent(self)
		bg.LoadImage(IMG_CATEGORY + "sub_norm.png")
		bg.AddFlag("not_pick")
		_SetupChild(bg)
		bg.Show()
		self.children["underBoard"] = bg
		label = ui.TextLine()
		label.SetParent(self)
		label.SetText(text)
		label.SetPosition(30, 5)
		label.AddFlag("not_pick")
		_SetupChild(label)
		label.Show()
		self.children["textLine"] = label

	def SetState(self, state):
		self.children["underBoard"].LoadImage(IMG_CATEGORY + "sub_%s.png" % state)

	def OnMouseOverIn(self):
		if not self.selected:
			self.SetState("hover")

	def OnMouseOverOut(self):
		self.SetState("select" if self.selected else "norm")

	def OnMouseLeftButtonDown(self):
		self.OnSelect()

	def OnRender(self):
		pass

class CategoryList(ui.Window):
	# AppendItemList(categoryData) / ClearItem() / SelectItem(sub) / GetSelectedItem()
	# SetSelectEvent(func(categoryId, itemId)) ; SetScrollBar(WikiUI.ScrollBarNew)
	def __init__(self):
		ui.Window.__init__(self)
		self.categories = []
		self.rows = []
		self.basePos = 0
		self.contentHeight = 0
		self.scrollBar = None
		self.selectedItem = None
		self.selectEvent = None

	def __del__(self):
		ui.Window.__del__(self)

	def SetSelectEvent(self, func):
		self.selectEvent = func

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		self.scrollBar = scrollBar

	def ClearItem(self):
		for r in self.rows:
			r.Hide()
			r.Destroy()
		self.rows = []
		self.categories = []
		self.selectedItem = None
		self.basePos = 0
		self.__Relayout()

	def GetSelectedItem(self):
		return self.selectedItem

	def AppendItemList(self, categoryData):
		for cat in categoryData:
			head = CategoryItem(cat.get("name", ""), cat.get("btn"))
			head.SetParent(self)
			entry = {"head": head, "subs": [], "data": cat}
			head.SetOnExpandEvent(ui.__mem_func__(self.__Relayout))
			head.SetOnCollapseEvent(ui.__mem_func__(self.__Relayout))
			for sub in cat.get("items", []):
				s = CategorySubItem(sub.get("name", ""))
				s.SetParent(self)
				s.SetEvent(ui.__mem_func__(self.__OnSubClicked), cat, sub, s)
				s.Hide()
				entry["subs"].append(s)
			self.categories.append(entry)
		self.__Relayout()

	def SelectItem(self, subItem):
		if self.selectedItem != None:
			self.selectedItem.selected = FALSE
			self.selectedItem.SetState("norm")
		self.selectedItem = subItem
		if subItem != None:
			subItem.selected = TRUE
			subItem.SetState("select")
			self.__ScrollToItemTop(subItem)

	def __OnSubClicked(self, cat, sub, widget):
		self.SelectItem(widget)
		if self.selectEvent:
			self.selectEvent(cat.get("id"), sub.get("id"))

	def __ScrollToItemTop(self, widget):
		(lx, ly) = self.GetGlobalPosition()
		(wx, wy) = widget.GetGlobalPosition()
		if wy < ly or wy + widget.GetHeight() > ly + self.GetHeight():
			self.basePos = max(0, min(self.contentHeight - self.GetHeight(), self.basePos + (wy - ly)))
			self.__Relayout()
			self.__SyncScrollBar()

	def __OnScroll(self):
		if self.scrollBar:
			maxBase = max(0, self.contentHeight - self.GetHeight())
			self.basePos = int(self.scrollBar.GetPos() * maxBase)
			self.__Relayout(FALSE)

	def __SyncScrollBar(self):
		if self.scrollBar and self.contentHeight > self.GetHeight():
			self.scrollBar.SetPos(float(self.basePos) / float(self.contentHeight - self.GetHeight()), FALSE)

	def OnMouseWheel(self, nLen):
		if self.contentHeight <= self.GetHeight():
			return FALSE
		self.basePos += -CATEGORY_PITCH if nLen > 0 else CATEGORY_PITCH
		self.basePos = max(0, min(self.contentHeight - self.GetHeight(), self.basePos))
		self.__Relayout()
		self.__SyncScrollBar()
		return TRUE

	def __Relayout(self, updateBar = TRUE):
		y = 0
		visibleRows = []
		for entry in self.categories:
			head = entry["head"]
			head.SetPosition(0, y - self.basePos)
			visibleRows.append((head, y, CATEGORY_PITCH))
			y += CATEGORY_PITCH
			for s in entry["subs"]:
				if head.IsExpanded():
					s.SetPosition(0, y - self.basePos)
					visibleRows.append((s, y, SUB_PITCH))
					y += SUB_PITCH
				else:
					s.Hide()
		self.contentHeight = y
		h = self.GetHeight()
		for (w, top, pitch) in visibleRows:
			if top + pitch <= self.basePos or top >= self.basePos + h:
				w.Hide()
				continue
			self.__ClipRow(w, top - self.basePos, h)
			w.Show()
		if updateBar and self.scrollBar:
			if self.contentHeight > h:
				self.scrollBar.SetMiddleBarSize(float(h) / float(self.contentHeight))
				self.scrollBar.Show()
			else:
				self.scrollBar.Hide()

	def __ClipRow(self, w, localTop, viewHeight):
		# partially visible rows: clip image children, hide text that does not fit
		for key, child in w.children.items():
			ch = child.GetHeight()
			(cx, cy) = child.GetLocalPosition()
			top = localTop + cy
			cut1 = max(0, -top)
			cut2 = max(0, (top + ch) - viewHeight)
			if isinstance(child, ui.TextLine):
				if cut1 or cut2:
					child.Hide()
				else:
					child.Show()
			elif cut1 or cut2:
				child.SetRenderingRect(0, WikiUI.calculateRect(ch - cut1, ch), 0, WikiUI.calculateRect(ch - cut2, ch))
				child.itsRendered = TRUE
			elif child.itsRendered:
				child.SetRenderingRect(0, 0, 0, 0)
				child.itsRendered = FALSE

# ---------------------------------------------------------------------------
# Refine: EquipmentItem + RefineItem (with refine alternatives)
# ---------------------------------------------------------------------------

REFINE_PRICE_Y = [140, 140, 140, 180, 230, 275]
REFINE_BOX = [[0, 0], [0, 0], [411, 136], [411, 180], [411, 228], [411, 276]]
REFINE_ICON_Y = [[0, 0], [0, 0], [10, 55], [10, 70], [10, 80], [10, 115]]

class RefineItem(WikiUI.DefaultWikiWindow):
	# one "+N" column of the refine table
	def __init__(self):
		WikiUI.DefaultWikiWindow.__init__(self)

	def LoadData(self, refine, itemVnum, refineCount, refineData, showNext = TRUE):
		maxLevel = GetWikiRefineMaxLevel(itemVnum)
		tip = ui.ImageBox()
		tip.SetParent(self)
		tip.SetPosition(0, 0)
		tip.SetSize(41, 19)
		tip.SAFE_SetStringEvent("MOUSE_OVER_IN", self.OverInItem, (itemVnum - maxLevel) + refine)
		tip.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.OverOutItem)
		tip.Show()
		self.children.append(tip)
		self.children.append(_TextLine(self, "+%d" % refine, 21, 5, TRUE))
		if showNext or refine:
			priceText = MoneyText(refineData["cost"]).replace(".000", "k")
		else:
			priceText = "-"
		self.children.append(_TextLine(self, priceText, 21, REFINE_PRICE_Y[refineCount] - 21, TRUE))
		for i in xrange(refineCount):
			vnum = refineData["item_vnum_%d" % i]
			if vnum != 0 and (showNext or refine != 0):
				slot = WikiItemSlot()
				slot.SetParent(self)
				slot.SetItem(vnum, refineData["item_count_%d" % i])
				slot.SetClickEvent(self.OnClickRefineItem)
				slot.SetPosition(5, 25 + i * 47)
				slot.Show()
				slot.AppendTo(self)
			else:
				self.children.append(_TextLine(self, "-", self.GetWidth() / 2, 39 + i * 44, TRUE))

	def OnClickRefineItem(self, vnum):
		ShowItemInfoOnParent(vnum + GetWikiRefineMaxLevel(vnum), 0)

class EquipmentItem(WikiUI.DefaultWikiImage):
	RefineItem = RefineItem

	def __init__(self, listIndex, itemVnum):
		WikiUI.DefaultWikiImage.__init__(self)
		self.Destroy()
		self.listIndex = listIndex
		self.itemVnum = itemVnum
		self.refineLevel = GetWikiRefineMaxLevel(itemVnum)
		self.baseVnum = itemVnum - self.refineLevel
		self.alternativeCount = GetRefineAlternativeCount(self.baseVnum, self.refineLevel)
		self.alternative = 0
		self.altButtons = []
		self.refineItems = []
		self.refineCount = 2
		self.__ReadRefineTable()
		self.LoadImage(IMG_DIR + "slot/slot_%d.png" % self.refineCount)
		self.children.append(self)
		if self.refineLevel >= 11:
			self.SetSize(self.GetWidth(), self.GetHeight() + 4)

	def __del__(self):
		WikiUI.DefaultWikiImage.__del__(self)

	def Destroy(self):
		self.refineItems = []
		self.altButtons = []
		self.listIndex = 0
		self.itemVnum = 0
		self.refineCount = 0
		WikiUI.DefaultWikiImage.Destroy(self)

	def __ReadRefineTable(self):
		self.refineItems = BuildRefineTable(self.baseVnum, self.refineLevel, self.alternative)
		self.refineCount = 2
		for row in self.refineItems:
			if row["refine_count"] > self.refineCount:
				self.refineCount = row["refine_count"]

	def __GetMaxRefineAlternativeCount(self):
		return self.alternativeCount

	def LoadItemInfos(self):
		if self.IsLoaded != FALSE:
			return
		self.IsLoaded = TRUE
		item.SelectItem(self.baseVnum)
		firstLevel = self.__GetLevelLimit()
		item.SelectItem(self.itemVnum)
		secondLevel = self.__GetLevelLimit()
		name = item.GetItemName()
		if name.find("+") != -1:
			name = name[:name.find("+")]
		self.children.append(_TextLine(self, name, 5, 5))
		if secondLevel != firstLevel:
			lvl = _L("WIKI_LEVEL_TEXT2", "Lv %d-%d (+%d-+%d)") % (firstLevel, secondLevel, 0, self.refineLevel)
		else:
			lvl = _L("WIKI_LEVEL_TEXT", "Lv %d (+%d-+%d)") % (firstLevel, 0, self.refineLevel)
		self.children.append(_TextLine(self, lvl, 355, 5))
		self.children.append(_TextLine(self, _L("WIKI_UPGRADE_COSTS", "Upgrade costs"), 50, 24))
		self.children.append(_TextLine(self, _L("WIKI_YANG_COSTS", "Yang"), 58, REFINE_PRICE_Y[self.refineCount]))
		baseIcon = WikiItemSlot()
		baseIcon.SetParent(self)
		baseIcon.SetItem(self.baseVnum if self.listIndex == 99 else self.itemVnum)
		baseIcon.SetPosition(REFINE_ICON_Y[self.refineCount][0], REFINE_ICON_Y[self.refineCount][1])
		baseIcon.Show()
		baseIcon.AppendTo(self)
		item.SelectItem(self.itemVnum)
		refined = item.GetRefinedVnum()
		if refined != 0:
			nextIcon = WikiItemSlot()
			nextIcon.SetParent(self)
			nextIcon.SetItem(refined)
			nextIcon.SetPosition(75, REFINE_ICON_Y[self.refineCount][1])
			nextIcon.Show()
			nextIcon.AppendTo(self)
		self.__CreateRefineAlternativeControls()
		self.__BuildRefineListbox()
		self.OnRender()
		self.Show()

	def __GetLevelLimit(self):
		for i in xrange(item.LIMIT_MAX_NUM):
			(limitType, limitValue) = item.GetLimit(i)
			if item.LIMIT_LEVEL == limitType and limitValue > 0:
				return limitValue
		return 0

	def __CreateRefineAlternativeControls(self):
		# small "1 2 3" tabs at the top right, only when the server exposes several refine sets
		self.altButtons = []
		if self.alternativeCount <= 1:
			return
		for i in xrange(self.alternativeCount):
			btn = ui.RadioButton()
			btn.SetParent(self)
			btn.SetUpVisual(IMG_DIR + "btn_search_norm.png")
			btn.SetOverVisual(IMG_DIR + "btn_search_hover.png")
			btn.SetDownVisual(IMG_DIR + "btn_search_select.png")
			btn.SetText(str(i + 1))
			btn.SetPosition(250 + i * 32, 0)
			btn.SetEvent(ui.__mem_func__(self.__ChangeRefineAlternative), i)
			_SetupChild(btn)
			btn.ButtonText = getattr(btn, "ButtonText", None)
			btn.Show()
			self.altButtons.append(btn)
			self.children.append(btn)
		self.__RefreshRefineAlternativeControls()

	def __RefreshRefineAlternativeControls(self):
		for i, btn in enumerate(self.altButtons):
			if i == self.alternative:
				btn.Down()
			else:
				btn.SetUp()

	def __ChangeRefineAlternative(self, index):
		if index == self.alternative or index >= self.alternativeCount:
			return
		self.alternative = index
		self.__ReadRefineTable()
		self.__RefreshRefineAlternativeControls()
		self.__DestroyRefineListbox()
		self.__BuildRefineListbox()
		self.OnRender()

	def __DestroyRefineListbox(self):
		if self.Listbox:
			self.Listbox.RemoveAllItems()
			self.Listbox.Hide()
			self.Listbox = None

	def __GetRefineDataIndex(self, level, showNext):
		if showNext or level == 0:
			return level
		return level - 1

	def __GetSelectedRefineData(self, level, showNext = TRUE):
		return self.refineItems[self.__GetRefineDataIndex(level, showNext)]

	def __BuildRefineListbox(self, showNext = TRUE):
		size = REFINE_BOX[self.refineCount]
		box = WikiUI.ListBoxEx(TRUE)
		box.SetParent(self)
		box.SetSize(size[0], size[1])
		box.SetPosition(130, 20)
		box.SetItemStep(41)
		box.SetItemSize(41, size[1])
		box.SetViewItemCount(10)
		box.SetOnMouseWhell(FALSE)
		if self.refineLevel >= 11:
			bar = WikiUI.HorizontalScrollBarNew()
			bar.SetParent(self)
			bar.SetPosition(0, [156, 156, 156, 200, 255, 296][self.refineCount])
			bar.SetScrollBarSize(540)
			bar.itsNeedDoubleRender = TRUE
			bar.Show()
			box.SetScrollBar(bar)
			self.children.append(bar)
		for i in xrange(self.refineLevel + 1):
			col = RefineItem()
			box.AppendItem(col)
			col.LoadData(i, self.itemVnum, self.refineCount, self.__GetSelectedRefineData(i, showNext), showNext)
		box.SetBasePos(0)
		box.Show()
		self.Listbox = box

	def OnClickItem(self, arg, vnum = 0):
		self.OverOutItem()
		ShowItemInfoOnParent(vnum + GetWikiRefineMaxLevel(vnum) if vnum else self.itemVnum, 0)

	def InsertRefine(self, *argv):
		# kept for API parity with the old class (native tuple -> dict)
		row = {"id": int(argv[0]), "cost": long(argv[11]), "prob": long(argv[12]), "refine_count": int(argv[13])}
		for i in xrange(5):
			row["item_vnum_%d" % i] = int(argv[1 + i * 2])
			row["item_count_%d" % i] = int(argv[2 + i * 2])
		self.refineItems.append(row)
		if row["refine_count"] > self.refineCount:
			self.refineCount = row["refine_count"]

# ---------------------------------------------------------------------------
# Obtention lists (craft / cube / shop): header + selector buttons + body
# ---------------------------------------------------------------------------

class _ObtentionBase(WikiUI.DefaultWikiImage):
	BG = IMG_DIR + "slot/big_empty.png"

	def __init__(self, vnum):
		WikiUI.DefaultWikiImage.__init__(self)
		self.Destroy()
		self.vnum = vnum
		self.entries = []
		self.current = 0
		self.pageButtons = []
		self.body = []
		self.LoadImage(self.BG)
		self.children.append(self)

	def __del__(self):
		WikiUI.DefaultWikiImage.__del__(self)

	def Destroy(self):
		self.entries = []
		self.pageButtons = []
		self.body = []
		WikiUI.DefaultWikiImage.Destroy(self)

	def GetEntries(self):
		return []

	def LoadItemInfos(self):
		if self.IsLoaded != FALSE:
			return
		self.IsLoaded = TRUE
		self.entries = self.GetEntries()
		item.SelectItem(self.vnum)
		self.children.append(_TextLine(self, item.GetItemName(), 8, 6))
		if len(self.entries) == 0:
			self.children.append(_TextLine(self, _L("WIKI_NO_DATA", "-"), self.GetWidth() / 2, self.GetHeight() / 2, TRUE))
		else:
			self.__RefreshButtons()
			self._Select(0)
		self.OnRender()
		self.Show()

	def __RefreshButtons(self):
		# numbered selector for > 1 entries (recipe / offer pages)
		for b in self.pageButtons:
			b.Hide()
		self.pageButtons = []
		if len(self.entries) <= 1:
			return
		for i in xrange(len(self.entries)):
			btn = ui.RadioButton()
			btn.SetParent(self)
			btn.SetUpVisual(IMG_DIR + "btn_search_norm.png")
			btn.SetOverVisual(IMG_DIR + "btn_search_hover.png")
			btn.SetDownVisual(IMG_DIR + "btn_search_select.png")
			btn.SetText(str(i + 1))
			btn.SetPosition(self.GetWidth() - 8 - (len(self.entries) - i) * 32, 4)
			btn.SetEvent(ui.__mem_func__(self._Select), i)
			_SetupChild(btn)
			btn.Show()
			self.pageButtons.append(btn)
			self.children.append(btn)

	def _Clear(self):
		for w in self.body:
			w.Hide()
			if w in self.children:
				self.children.remove(w)
		self.body = []

	def _Add(self, w):
		self.body.append(w)
		self.children.append(w)
		return w

	def _Select(self, index):
		self.current = index
		for i, b in enumerate(self.pageButtons):
			if i == index:
				b.Down()
			else:
				b.SetUp()
		self._Clear()
		self._Refresh(self.entries[index])
		self.OnRender()

	def _Refresh(self, entry):
		pass

	def _PlaceSlots(self, rows, x, y, spacing = 4):
		for (vnum, count) in rows:
			slot = WikiItemSlot()
			slot.SetParent(self)
			slot.SetItem(vnum, count)
			slot.SetPosition(x, y)
			slot.Show()
			self._Add(slot)
			if slot.countText != None:
				self.body.append(slot.countText)
				self.children.append(slot.countText)
			x += slot.GetWidth() + spacing
		return x

	def _AddNpcButton(self, npcVnum, x, y):
		if not npcVnum:
			return
		btn = ui.Button()
		btn.SetParent(self)
		btn.SetUpVisual(IMG_DIR + "btn_pnj.png")
		btn.SetOverVisual(IMG_DIR + "btn_pnj.png")
		btn.SetDownVisual(IMG_DIR + "btn_pnj.png")
		btn.SetPosition(x, y)
		btn.SetEvent(ui.__mem_func__(self.OnClickPNJ), npcVnum)
		_SetupChild(btn)
		btn.Show()
		self._Add(btn)
		self._Add(_TextLine(self, nonplayer.GetMonsterName(npcVnum), x + 28, y + 4))

	def OnClickPNJ(self, npcVnum):
		parent = constInfo.GetMainParent()
		fn = getattr(parent, "OnClickPNJ", None) if parent != None else None
		if fn:
			fn(npcVnum)

class ListBoxCraftObtentionItem(_ObtentionBase):
	def GetEntries(self):
		return GetItemCraftRecipes(self.vnum)

	def _Refresh(self, r):
		self._Add(_TextLine(self, _L("WIKI_CRAFT_MATERIALS", "Materials"), 8, 28))
		x = self._PlaceSlots(r["materials"], 8, 44)
		self._Add(_TextLine(self, _L("WIKI_CRAFT_RESULT", "Result"), 8, 92))
		self._PlaceSlots(r["results"], 8, 108)
		if r["increase"]:
			self._Add(_TextLine(self, _L("WIKI_CRAFT_INCREASE", "Increase"), 270, 92))
			self._PlaceSlots(r["increase"], 270, 108)
		if r["gold"]:
			self._Add(_TextLine(self, _L("WIKI_YANG_COSTS", "Yang") + ": " + MoneyText(r["gold"]), 270, 28))
		self._AddNpcButton(r["npc"], 270, 52)

class ListBoxCubeItem(ListBoxCraftObtentionItem):
	def GetEntries(self):
		return GetItemCubeRecipes(self.vnum)

class ListBoxShopObtentionItem(_ObtentionBase):
	def GetEntries(self):
		return GetItemShopOffers(self.vnum)

	def _Refresh(self, o):
		self._Add(_TextLine(self, _L("WIKI_SHOP_PRICE", "Price"), 8, 28))
		if o["currency"]:
			self._PlaceSlots([(o["currency"], o["price"])], 8, 44)
		else:
			self._Add(_TextLine(self, MoneyText(o["price"]) + " " + _L("WIKI_YANG_COSTS", "Yang"), 8, 50))
		if o["count"] > 1:
			self._Add(_TextLine(self, "x%d" % o["count"], 8, 92))
		self._AddNpcButton(o["npc"], 270, 40)

# ---------------------------------------------------------------------------
# Drop grid + monster / item source pages
# ---------------------------------------------------------------------------

def BuildDropGrid(owner, listbox, rows, colCount = 11):
	# rows: [(vnum, count)] -> icon grid inside a WikiUI.ListBoxGrid, children registered in owner
	grid = ui.Grid(width = colCount, height = 50)
	for (vnum, count) in rows:
		if vnum == 0:
			continue
		item.SelectItem(vnum)
		(width, height) = item.GetItemSize()
		if width == 0 or height == 0:
			continue
		pos = grid.find_blank(width, height)
		grid.put(pos, width, height)
		(x, y) = WikiUI.calculatePos(pos, colCount - 1)
		slot = WikiItemSlot()
		slot.SetParent(listbox)
		slot.SetItem(vnum, count)
		slot.SetPosition(x, y, TRUE)
		slot.itsNeedDoubleRender = TRUE
		slot.Show()
		listbox.AppendItem(slot)
		owner.children.append(slot)
		if slot.countText != None:
			slot.countText.itsNeedDoubleRender = TRUE
			owner.children.append(slot.countText)

def AppendListText(owner, listbox, text, x, y):
	t = ui.TextLine()
	t.SetParent(listbox)
	t.itsNeedDoubleRender = TRUE
	t.SetText(text)
	t.SetPosition(x, y, TRUE)
	t.Show()
	owner.children.append(t)
	listbox.AppendItem(t)

def _DropRows(infoSize, infoData, key):
	rows = []
	for j in xrange(infoSize(key)):
		rows.append(infoData(key, j))
	return rows

def BuildSourcePage(owner, listbox, vnum):
	# "chests" + "monsters" text list of where an item comes from (item sources page)
	item.SelectItem(60001)
	unknownItem = item.GetItemName()
	(realVnum, isRefine) = WikiUI.getRealVnum(vnum)
	x = 5
	y = 5
	chests = NativeCall("GetItemDropFromChest", [], realVnum, isRefine)
	if chests:
		AppendListText(owner, listbox, "|cFF0080FF" + _L("WIKI_CHESTS", "Chests") + ":", x, y)
		y += 14
		for chest in chests:
			item.SelectItem(chest)
			name = item.GetItemName()
			if name == unknownItem:
				name = _L("WIKI_UNKOWN_ITEM", "Unknown item %d") % chest
			AppendListText(owner, listbox, "|Eemoji/e_wiki|e " + name, x, y)
			y += 14
		y += 28
	mobs = GetWikiItemMobSources(realVnum, vnum, isRefine)
	if mobs:
		AppendListText(owner, listbox, "|cFF0080FF" + _L("WIKI_MONSTER", "Monsters") + ":", x, y)
		y += 14
		for mob in mobs:
			level = nonplayer.GetMonsterLevel(mob)
			if level <= 0:
				text = _L("WIKI_UNKOWN_MOB", "Unknown monster %d") % mob
			else:
				text = nonplayer.GetMonsterName(mob) + " - Lv." + str(level)
			AppendListText(owner, listbox, "|Eemoji/e_wiki|e " + text, x, y)
			y += 14

def BuildMountSourcePage(owner, listbox, vnum):
	# mount rune / energy / stone sources (native optional: GetMountRuneRefines)
	rows = GetMountRuneRefines(vnum)
	if not rows:
		return
	y = 5
	AppendListText(owner, listbox, "|cFF0080FF" + _L("WIKI_MOUNT_RUNES", "Runes") + ":", 5, y)
	y += 14
	for r in rows:
		item.SelectItem(r["rune"])
		line = "%s - %d %s" % (item.GetItemName(), r["energy"], _L("WIKI_MOUNT_ENERGY", "energy"))
		AppendListText(owner, listbox, "|Eemoji/e_wiki|e " + line, 5, y)
		y += 14
		if r["stone"]:
			item.SelectItem(r["stone"])
			AppendListText(owner, listbox, "      %s x%d" % (item.GetItemName(), r["stoneCount"]), 5, y)
			y += 14

def BuildPetEvolutionPage(owner, listbox, vnum):
	rows = GetPetEvolution(vnum)
	y = 5
	for r in rows:
		item.SelectItem(r["vnum"])
		AppendListText(owner, listbox, "|Eemoji/e_wiki|e %s %d: %s x%d (Lv.%d)" % (_L("WIKI_PET_EVOLUTION", "Evolution"), r["stage"], item.GetItemName(), r["count"], r["level"]), 5, y)
		y += 14

def _AttachScroll(owner, listbox, x, h):
	if listbox.isNeedScrollBar():
		bar = WikiUI.ScrollBarNew()
		bar.SetParent(listbox)
		bar.SetPosition(x, 0)
		bar.SetScrollBarSize(h)
		bar.Show()
		owner.children.append(bar)
		listbox.SetScrollBar(bar)

RACE_FLAGS = (
	(0, "WIKI_INFO_RACE_ANIMAL"), (1, "WIKI_INFO_RACE_UNDEAD"), (2, "WIKI_INFO_RACE_DEVIL"),
	(3, "WIKI_INFO_RACE_HUMAN"), (4, "WIKI_INFO_RACE_ORC"), (5, "WIKI_INFO_RACE_MILGYO"))
SUB_RACE_FLAGS = (
	(11, "WIKI_INFO_RACE_ELEC"), (12, "WIKI_INFO_RACE_FIRE"), (13, "WIKI_INFO_RACE_ICE"),
	(14, "WIKI_INFO_RACE_WIND"), (15, "WIKI_INFO_RACE_EARTH"), (16, "WIKI_INFO_RACE_DARK"),
	(17, "WIKI_INFO_RACE_ZODIAC"))

class MonsterStatics(WikiUI.DefaultWikiImage):
	# isType 0: monster statistics, 3: item sources (chests/monsters/mount runes/pet evolution)
	def __init__(self, mobVnum, isType):
		WikiUI.DefaultWikiImage.__init__(self)
		self.Destroy()
		self.mobVnum = mobVnum
		self.isType = isType
		self.LoadImage(IMG_DIR + "slot/big_empty.png")
		self.children.append(self)

	def __del__(self):
		WikiUI.DefaultWikiImage.__del__(self)

	def Destroy(self):
		self.mobVnum = 0
		WikiUI.DefaultWikiImage.Destroy(self)

	def SetListBoxItemClip(self, top, bottom):
		pass

	def LoadItemInfos(self):
		if self.IsLoaded != FALSE:
			return
		self.IsLoaded = TRUE
		box = WikiUI.ListBoxGrid()
		box.SetParent(self)
		box.SetSize(self.GetWidth(), self.GetHeight() - 10)
		box.AddRenderEvent(self.OnRender)
		if self.isType == 3:
			BuildSourcePage(self, box, self.mobVnum)
			BuildMountSourcePage(self, box, self.mobVnum)
			BuildPetEvolutionPage(self, box, self.mobVnum)
		elif self.isType == 0:
			self.__BuildStats(box)
		_AttachScroll(self, box, box.GetWidth() - 10, 150)
		self.OnRender()
		box.Show()
		self.Listbox = box
		self.Show()

	def __Flags(self, flags, raceFlag, table):
		names = []
		for (bit, key) in table:
			if constInfo.IS_SET(raceFlag, 1 << bit):
				names.append(_L(key, key))
		return names

	def __BuildStats(self, box):
		mob = self.mobVnum
		E = "|Eemoji/e_wiki|e "
		AppendListText(self, box, E + _L("WIKI_LEVEL_MOB_TEXT", "Level: %d") % nonplayer.GetMonsterLevel(mob), 5, 3)
		flag = nonplayer.GetMonsterRaceFlag(mob)
		main = self.__Flags(0, flag, RACE_FLAGS)
		sub = self.__Flags(0, flag, SUB_RACE_FLAGS)
		if nonplayer.IsMonsterStone(mob):
			main.append(_L("WIKI_INFO_RACE_METIN", "Metin"))
		none = _L("WIKI_INFO_NO_RACE", "-")
		AppendListText(self, box, E + _L("WIKI_STATICS_INFO_TYPE", "%s / %s") % (", ".join(main) or none, ", ".join(sub) or none), 5, 21)
		(mn, mx) = nonplayer.GetMonsterDamage(mob)
		AppendListText(self, box, E + _L("WIKI_STATICS_INFO_DMG", "%d-%d / %d HP") % (mn, mx, nonplayer.GetMonsterMaxHP(mob)), 5, 42)
		(ya, yb) = nonplayer.GetMonsterPrice(mob)
		AppendListText(self, box, E + _L("WIKI_STATICS_INFO_YNG", "%d-%d / %d exp") % (ya, yb, nonplayer.GetMonsterExp(mob)), 5, 63)
		AppendListText(self, box, E + _L("WIKI_STATICS_INFO_DEFENSES", "Defenses"), 5, 84)
		y = 98
		for (const, key) in (("MOB_RESIST_SWORD", "WIKI_INFO_RESIST_SWORD"), ("MOB_RESIST_TWOHAND", "WIKI_INFO_RESIST_TWOHAND"),
			("MOB_RESIST_DAGGER", "WIKI_INFO_RESIST_DAGGER"), ("MOB_RESIST_BELL", "WIKI_INFO_RESIST_BELL"),
			("MOB_RESIST_FAN", "WIKI_INFO_RESIST_FAN"), ("MOB_RESIST_BOW", "WIKI_INFO_RESIST_BOW"),
			("MOB_RESIST_MAGIC", "WIKI_INFO_RESIST_MAGIC")):
			idx = getattr(nonplayer, const, None)
			if idx is None:
				continue
			AppendListText(self, box, E + _L(key, key + " %d") % nonplayer.GetMonsterResist(mob, idx), 13, y)
			y += 14

class MonsterItemSpecial(WikiUI.DefaultWikiImage):
	# isType 0: monster (3D model + drop grid), 3: chest/item (icon + contents grid + sources)
	def __init__(self, vnum, type):
		WikiUI.DefaultWikiImage.__init__(self)
		self.vnum = vnum
		self.isType = type
		self.LoadImage(IMG_DIR + "slot/special_slot.png")
		self.children.append(self)

	def __del__(self):
		WikiUI.DefaultWikiImage.__del__(self)

	def Destroy(self):
		if self.renderIndex:
			renderTarget.SetVisibility(self.renderIndex, FALSE)
			renderTarget.ResetModel(self.renderIndex)
		self.vnum = 0
		WikiUI.DefaultWikiImage.Destroy(self)

	def ApplyListBoxClipRect(self, top, bottom):
		pass

	def __IsCaseSimulatorChest(self):
		return FALSE

	def __OpenCaseSimulatorLoot(self, *args):
		pass

	def LoadItemInfos(self):
		if self.IsLoaded != FALSE:
			return
		self.IsLoaded = TRUE
		self.renderIndex = renderTarget.GetFreeIndex(1000, 1000000)
		if self.isType == 0:
			self.__ShowModel()
			head = _L("WIKI_DROPLIST_INFO", "Drops of %s") % nonplayer.GetMonsterName(self.vnum)
			rows = _DropRows(wiki.GetMobInfoSize, wiki.GetMobInfoData, self.vnum)
		else:
			item.SelectItem(self.vnum)
			slot = WikiItemSlot()
			slot.SetParent(self)
			slot.SetItem(self.vnum)
			slot.SetPosition(70, 45)
			slot.Show()
			slot.AppendTo(self)
			head = _L("WIKI_CONTENT_INFO", "Contents of %s") % item.GetItemName()
			rows = _DropRows(wiki.GetSpecialInfoSize, wiki.GetSpecialInfoData, self.vnum)
		self.children.append(_TextLine(self, head, 300, 6))
		box = WikiUI.ListBoxGrid()
		box.SetParent(self)
		box.SetPosition(190, 25)
		box.SetSize(350, 138)
		box.AddRenderEvent(self.OnRender)
		if self.isType == 0:
			BuildDropGrid(self, box, rows, 11)
		else:
			BuildDropGrid(self, box, rows, 11)
			BuildSourcePage(self, box, self.vnum)
			BuildMountSourcePage(self, box, self.vnum)
		_AttachScroll(self, box, box.GetWidth() - 10, 137)
		if len(box.itemList) > 0:
			self.Listbox = box
			box.SetBasePos(0, FALSE)
			box.Show()
		self.OnRender()
		self.Show()

	def __ShowModel(self):
		rt = ui.RenderTarget()
		rt.SetParent(self)
		rt.SetPosition(1, 1)
		rt.SetSize(187, 163)
		rt.SetRenderTarget(self.renderIndex)
		rt.Show()
		self.children.append(rt)
		renderTarget.SelectModel(self.renderIndex, self.vnum)
		renderTarget.SetVisibility(self.renderIndex, TRUE)

class ListBoxItemSpecial(WikiUI.DefaultWikiImage):
	# result row: type 0 = chest/item row (icon, content grid, origin), else monster row (model, drop grid)
	def __init__(self, index, itemVnum, mobVnum, type):
		WikiUI.DefaultWikiImage.__init__(self)
		self.listIndex = index
		self.itemVnum = itemVnum
		self.mobVnum = mobVnum
		self.isType = type
		self.renderTargetShown = FALSE
		self.LoadImage(IMG_DIR + "slot/slot.png")
		self.children.append(self)

	def __del__(self):
		WikiUI.DefaultWikiImage.__del__(self)

	def Destroy(self):
		self.renderTargetShown = FALSE
		WikiUI.DefaultWikiImage.Destroy(self)

	def RenderTargetIsLoaded(self):
		return self.renderTarget != None

	def RenderTargetIsShowing(self):
		return self.renderTargetShown

	def SetRenderTargetVisible(self, flag):
		self.renderTargetShown = flag
		if self.renderTarget != None:
			renderTarget.SetVisibility(20 + self.listIndex, flag)

	def ShowRenderTarget(self):
		self.SetRenderTargetVisible(TRUE)

	def SetListBoxClip(self, top, bottom):
		self.clip = (top, bottom)

	def SetListBoxOriginClip(self, top, bottom):
		self.originClip = (top, bottom)

	def ApplyListBoxItemClipRect(self, *args):
		pass

	def ApplyDropListClip(self, *args):
		pass

	def LoadItemInfos(self):
		if self.IsLoaded != FALSE:
			return
		self.IsLoaded = TRUE
		if self.isType == 0:
			item.SelectItem(self.itemVnum)
			name = nonplayer.GetMonsterName(self.mobVnum)
			title = _L("WIKI_CONTENT_INFO", "Contents of %s") % item.GetItemName()
			rows = _DropRows(wiki.GetSpecialInfoSize, wiki.GetSpecialInfoData, self.itemVnum)
		else:
			name = nonplayer.GetMonsterName(self.itemVnum)
			title = _L("WIKI_DROPLIST_INFO", "Drops of %s") % name + " - Level %d - Mob Vnum %d" % (nonplayer.GetMonsterLevel(self.itemVnum), self.itemVnum)
			rows = _DropRows(wiki.GetMobInfoSize, wiki.GetMobInfoData, self.itemVnum)
		self.children.append(_TextLine(self, title, 270, 5, TRUE))
		self.children.append(_TextLine(self, _L("WIKI_ORIGIN", "Origin"), 480, 5))
		self.children.append(_TextLine(self, name if (self.isType == 0 and self.mobVnum != 0) else "-", 490, 45, TRUE))
		if self.isType == 0:
			slot = WikiItemSlot()
			slot.SetParent(self)
			slot.SetItem(self.itemVnum)
			slot.SetPosition(10, 25)
			slot.Show()
			slot.AppendTo(self)
		else:
			rt = ui.RenderTarget()
			rt.SetParent(self)
			rt.SetPosition(1, 1)
			rt.SetSize(47, 87)
			rt.SetRenderTarget(20 + self.listIndex)
			rt.SetEvent(ui.__mem_func__(self.OnClickItem), "mouse_click", 1, self.itemVnum)
			rt.Show()
			self.children.append(rt)
			self.renderTarget = rt
			renderTarget.SelectModel(20 + self.listIndex, self.itemVnum)
			renderTarget.SetVisibility(20 + self.listIndex, TRUE)
			self.renderTargetShown = TRUE
		if len(rows) != 0:
			box = WikiUI.ListBoxGrid()
			box.SetParent(self)
			box.SetPosition(48, 22)
			box.SetSize(403, 65)
			box.AddRenderEvent(self.OnRender)
			box.Show()
			BuildDropGrid(self, box, rows, 12)
			if box.isNeedScrollBar():
				bar = WikiUI.ScrollBarNew()
				bar.SetParent(self)
				bar.SetPosition(443, 23)
				bar.SetScrollBarSize(63)
				bar.itsNeedDoubleRender = TRUE
				bar.Show()
				self.children.append(bar)
				box.SetScrollBar(bar)
			self.Listbox = box
		self.OnRender()
		self.Show()

class ShopSpecialClass(WikiUI.DefaultWikiWindow):
	# shop page of the wiki: one NPC (listIndex) with its sold items as slots
	def __init__(self, listIndex, npcVnum = 0):
		WikiUI.DefaultWikiWindow.__init__(self)
		self.listIndex = listIndex
		self.npcVnum = npcVnum
		self.SetSize(540, 147)
		self.vnumList = []

	def LoadItemInfos(self, shopItems = None):
		if self.IsLoaded != FALSE or shopItems is None:
			return
		self.IsLoaded = TRUE
		self.children.append(_TextLine(self, nonplayer.GetMonsterName(self.npcVnum), 8, 4))
		x = 8
		y = 24
		for (vnum, price) in shopItems:
			slot = WikiItemSlot()
			slot.SetParent(self)
			slot.SetItem(vnum)
			slot.SetPosition(x, y)
			slot.Show()
			slot.AppendTo(self)
			self.children.append(_TextLine(self, MoneyText(price), x, y + slot.GetHeight() + 2))
			x += 60
			if x + 60 > self.GetWidth():
				x = 8
				y += slot.GetHeight() + 20
		self.OnRender()
		self.Show()

	def OverInShopItem(self, vnum):
		self.OverInItem(vnum)

class SpecialClass(WikiUI.DefaultWikiWindow):
	# row of up to 4 special item cards (name + icon), hovering shows a 3D preview tooltip
	def __init__(self, listIndex, isMonster):
		WikiUI.DefaultWikiWindow.__init__(self)
		self.Destroy()
		self.isType = isMonster
		self.listIndex = listIndex
		self.SetSize(540, 147)

	def Destroy(self):
		self.vnumList = []
		WikiUI.DefaultWikiWindow.Destroy(self)

	def CanAddNewItem(self):
		return len(self.vnumList) < 4

	def LoadItemInfos(self, data = -1):
		if data == -1:
			return
		x = len(self.vnumList) * (127 + 9)
		card = ui.ExpandedImageBox()
		card.SetParent(self)
		if not _TryLoad(card, "d:/ymir work/ui/game/wiki_new/grid/small_grid.png", IMG_DIR + "slot/empty_slot.png"):
			card = ui.ImageBox()
		card.SetPosition(x, 0)
		_SetupChild(card)
		card.Show()
		self.children.append(card)
		slot = WikiItemSlot()
		slot.SetParent(card)
		slot.SetItem(data)
		slot.SetPosition((card.GetWidth() - slot.GetWidth()) / 2, (card.GetHeight() - 20 - slot.GetHeight()) / 2)
		slot.SAFE_SetStringEvent("MOUSE_OVER_IN", self.OverInItemSpecial, data)
		slot.SAFE_SetStringEvent("MOUSE_OVER_OUT", self.OverOutItem)
		slot.Show()
		slot.AppendTo(self)
		item.SelectItem(data)
		name = item.GetItemName()
		label = _TextLine(card, name, card.GetWidth() / 2, card.GetHeight() - 18, TRUE)
		maxW = card.GetWidth() - 15
		while label.GetTextSize()[0] > maxW and len(name) > 3:
			name = name[:-1]
			label.SetText(name + "...")
		self.children.append(label)
		self.vnumList.append(data)
		self.OnRender()
		self.Show()

	def OverOutItem(self):
		if self.renderIndex != 0:
			renderTarget.SetVisibility(self.renderIndex, FALSE)
			renderTarget.ResetModel(self.renderIndex)
		WikiUI.DefaultWikiWindow.OverOutItem(self)

	def OverInItemSpecial(self, itemVnum):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		tip = interface.tooltipItem
		if not tip:
			return
		tip.ClearToolTip()
		self.renderIndex = renderTarget.GetFreeIndex(1000, 1000000)
		tip.toolTipWidth -= 35
		rt = ui.RenderTarget()
		rt.SetParent(tip)
		rt.SetPosition(10, 5)
		rt.SetSize(tip.toolTipWidth - 20, 150)
		rt.SetRenderTarget(self.renderIndex)
		rt.Show()
		tip.childrenList.append(rt)
		tip.toolTipHeight += 150
		tip.ResizeToolTip()
		WikiUI.SetItemToModelPreview(self.renderIndex, itemVnum)
		tip.SetItemToolTipWiki(itemVnum)
		tip.ShowToolTip()
