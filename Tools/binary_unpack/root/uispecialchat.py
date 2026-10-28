if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import grp
import ui
import wndMgr
import chat

class SpecialChat(ui.ThinBoard):
	BOARD_WIDTH = 200
	BOARD_HEIGHT = 215

	def __init__(self, x = 0, y = 0):
		ui.ThinBoard.__init__(self)

		self.HideCorners(self.LB)
		self.HideCorners(self.RB)
		self.HideLine(self.B)

		self.texts = {}

		self.SpaceBet = 14
		self.x = x
		self.y = y
		self.ColorValue = 0xFFBEB47D
		self.OverIn = FALSE
		self.IsYang = FALSE
		self.lastlinetime = 0

		self.SetTop()
		self.SetSize(self.BOARD_WIDTH, self.BOARD_HEIGHT)
		self.SetPosition(wndMgr.GetScreenWidth()/2 - 539, wndMgr.GetScreenHeight() - 60)

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def AddSpecialChatLine(self, text, arg = 0, Yang = FALSE):
		for i in xrange(len(self.texts)):
			if len(self.texts) == 12 and i == 0:
				self.texts[i].Hide()

			x, y = self.texts[i].GetLocalPosition()
			self.texts[i].SetPosition(x, y + self.SpaceBet)

		i = 0
		if len(self.texts) == 12:
			for i in xrange(len(self.texts)-1):
				self.texts[i] = self.texts[i+1]
			i = 11
		else:
			i = len(self.texts)

		self.texts[i] = ui.TextLine()
		self.texts[i].SetParent(self)
		self.texts[i].SetPosition(self.GetWidth()/2, 5)
		self.texts[i].SetPackedFontColor(self.ColorValue)
		self.texts[i].SetHorizontalAlignCenter()
		self.texts[i].SetVerticalAlignTop()
		self.texts[i].SetOutline(TRUE)

		max_len = 48

		if Yang:
			max_len = 48

		if len(text) > max_len:
			text = text[:max_len-3] + "..."

		self.texts[i].SetText(text)
		self.texts[i].Show()

		self.Show()
		self.lastlinetime = app.GetTime()
		self.IsYang = Yang

	def ClearAll(self):
		self.Hide()
		self.texts = {}
		self.IsYang = FALSE

	def OnMouseOverIn(self):
		self.OverIn = TRUE

	def OnMouseOverOut(self):
		self.OverIn = FALSE

	def OnUpdate(self):
		if self.OverIn == TRUE:
			(x, y) = self.GetGlobalPosition()
			w, h = self.texts[0].GetTextSize()
			height = (h+2)*len(self.texts) + 42

			if y > wndMgr.GetScreenHeight() - height:
				self.SetPosition(wndMgr.GetScreenWidth()/2 - 539, max(self.GetHeight(), y - 4))
		else:
			(x, y) = self.GetGlobalPosition()
			if y < wndMgr.GetScreenHeight() - self.GetHeight()/2 + 50:
				self.SetPosition(wndMgr.GetScreenWidth()/2 - 539, max(self.GetHeight(), y + 4))
