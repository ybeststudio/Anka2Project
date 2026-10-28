if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui
import localeInfo
import wndMgr

class ShipMastHP(ui.ThinBoard):
	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.textLine = None
		self.hpGauge = None
		self.__MakeBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

	def __MakeBoard(self):
		self.SetSize(120, 40)
		self.SetPosition(wndMgr.GetScreenWidth()-130, 225)
		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetText(localeInfo.SHIP_MAST)
		self.textLine.SetPosition(10, 5)
		self.textLine.SetOutline()
		self.textLine.Show()

		self.hpGauge = ui.Gauge()
		self.hpGauge.SetParent(self)
		self.hpGauge.SetPosition(13, 25)
		self.hpGauge.MakeGauge(95, "red")
		self.hpGauge.Show()

	def Open(self, curPoint, maxPoint):
		self.SetShipMastHP(curPoint, maxPoint)
		self.Show()

	def Close(self):
		self.Hide()

	def SetShipMastHP(self, curPoint, maxPoint):
		curPoint = int(curPoint)
		maxPoint = int(maxPoint)

		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			if not self.IsShow():
				self.Show()
			if self.hpGauge:
				self.hpGauge.SetPercentage(curPoint, maxPoint)
