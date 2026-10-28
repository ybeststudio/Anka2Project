# -*- coding: utf-8 -*-
"""Drop/loot log window (view_drop_ui).

Original implementation, not a byte-for-byte reconstruction: the reference
module (uiDropInfo, from baerim_unpack) is a Cython-native compile whose
method bodies are not recoverable by decompilation, only its method surface
(Open/Close/CreateWindow/CreateDropSlot/AppendDropInfo/AppendChatInfo/
OverInItem/OverOutItem/SlideIn/OnWarping/...). This module reproduces the
same observable behaviour -- a small floating list of recently acquired
items with icon, count, hover tooltip and auto-expiry -- using Anka's own
native ui.GridSlotWindow/uitooltip widgets, matching the recovered method
names where practical (CreateWindow, AppendDropInfo, AppendChatInfo,
OverInItem, OverOutItem, Open, Close, OnWarping, _ApplyAlpha, SlideIn).
"""
import ui
import wndMgr
import app
import uitooltip

SLOT_COUNT = 6
SLOT_SIZE = 32
SLOT_GAP = 2
ENTRY_LIFETIME = 6.0
SLIDE_DURATION = 0.18
FADE_DURATION = 0.4


class DropInfoWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.entries = []
		self.tooltipItem = None
		self.slideStart = 0.0
		self.CreateWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def CreateWindow(self):
		width = SLOT_SIZE + 10
		height = SLOT_COUNT * (SLOT_SIZE + SLOT_GAP) + 8
		self.SetWindowName("DropInfoWindow")
		self.SetSize(width, height)
		self.SetPosition(wndMgr.GetScreenWidth() - width - 14, wndMgr.GetScreenHeight() // 2 - height // 2)

		self.board = ui.ExpandedImageBox()
		self.board.SetParent(self)
		self.board.LoadImage("d:/ymir work/ui/game/advanced_game_options/panel_skin/board_base.tga")
		self.board.SetPosition(0, 0)
		self.board.SetScale(float(width) / max(1, self.board.GetWidth()), float(height) / max(1, self.board.GetHeight()))
		self.board.Show()

		self.grid = ui.GridSlotWindow()
		self.grid.SetParent(self)
		self.grid.SetPosition(5, 4)
		self.grid.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.grid.ArrangeSlot(0, 1, SLOT_COUNT, SLOT_SIZE, SLOT_SIZE, 0, SLOT_GAP)
		self.grid.SetSlotBaseImage("d:/ymir work/ui/game/dropinfo/slot_1.tga", 1.0, 1.0, 1.0, 1.0)
		self.grid.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.grid.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.grid.ClearAllSlot()
		self.grid.Show()

		self.Hide()

	def Destroy(self):
		self.tooltipItem = None
		self.entries = []

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def _select_display_item(self, slotIndex):
		if 0 <= slotIndex < len(self.entries):
			return self.entries[slotIndex]
		return None

	def _ApplyAlpha(self, alpha):
		# ui.Window (unlike ui.ImageBox) has no SetAlpha; fading the whole window
		# (board + grid slots together) isn't available cheaply, so this is a no-op.
		pass

	def SlideIn(self):
		pass

	def AppendDropInfo(self, itemVnum, itemCount=1):
		now = app.GetTime()
		self.entries.insert(0, {"vnum": int(itemVnum), "count": max(1, int(itemCount)), "expire": now + ENTRY_LIFETIME})
		del self.entries[SLOT_COUNT:]
		self._RefreshDropSlots()
		self.SlideIn()
		self.Show()

	def AppendChatInfo(self, text):
		# Fallback entry when no real item vnum applies (kept for API parity with the reference).
		chat = None
		try:
			import chat as chatModule
			chat = chatModule
		except ImportError:
			pass
		if chat:
			chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	def _RefreshDropSlots(self):
		for index in range(SLOT_COUNT):
			entry = self._select_display_item(index)
			if entry:
				self.grid.SetItemSlot(index, entry["vnum"], entry["count"])
			else:
				self.grid.ClearSlot(index)

	def OverInItem(self, slotIndex):
		entry = self._select_display_item(slotIndex)
		if not entry:
			return
		if self.tooltipItem is None:
			self.tooltipItem = uitooltip.ItemToolTip()
		self.tooltipItem.ClearToolTip()
		self.tooltipItem.SetItemToolTip(entry["vnum"])

	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnWarping(self):
		self.entries = []
		self._RefreshDropSlots()
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnUpdate(self):
		now = app.GetTime()
		before = len(self.entries)
		self.entries = [entry for entry in self.entries if entry["expire"] > now]
		if len(self.entries) != before:
			self._RefreshDropSlots()
			if not self.entries:
				self.Hide()
		if self.slideStart:
			ratio = (now - self.slideStart) / FADE_DURATION
			if ratio >= 1.0:
				self._ApplyAlpha(1.0)
				self.slideStart = 0.0
			else:
				self._ApplyAlpha(max(0.0, ratio))
