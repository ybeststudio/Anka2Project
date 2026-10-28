if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import textTail

class PlayerGauge(ui.Gauge):

	def __init__(self, parent):
		ui.Gauge.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.MakeGauge(100, "red")

		self.curHP = 0
		self.maxHP = 0

		self.showAlways = FALSE

	def __del__(self):
		ui.Gauge.__del__(self)

	def Hide(self):
		self.SetPosition(-100, -100)
		ui.Gauge.Hide(self)

	def OnUpdate(self):
		playerIndex = player.GetMainCharacterIndex()

		(x, y, z)=textTail.GetPosition(playerIndex)

		isChat = textTail.IsChat(playerIndex)
		ui.Gauge.SetPosition(self, int(x - self.GetWidth()/2), int(y + 5) + isChat*17)

	def RefreshGauge(self):

		self.curHP = player.GetStatus(player.HP)
		self.maxHP = player.GetStatus(player.MAX_HP)
		self.SetPercentage(self.curHP, self.maxHP)

		if self.showAlways:
			self.Show()

		else:

			if self.IsShow():
				if self.curHP > self.maxHP / 2:
					self.Hide()

			else:
				if self.curHP < self.maxHP / 2:
					self.OnUpdate()
					self.Show()

	def EnableShowAlways(self):
		self.showAlways = TRUE
		self.RefreshGauge()

	def DisableShowAlways(self):
		self.showAlways = FALSE
		self.RefreshGauge()
