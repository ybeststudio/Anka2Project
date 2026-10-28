if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))
pack = __import__(pyapi.GetModuleName("pack"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
chr = __import__(pyapi.GetModuleName("chr"))

import background
import chat
import textTail
import event
import effect
import systemSetting
import quest
import guild
import skill
import messenger
import ime
import item
import grp, wndMgr, uiCommon, ui, time, playerSettingModule, localeInfo, snd, mouseModule, constInfo, uiScriptLocale, interfacemodule, dbg, uiToolTip

class StoneEventWindow(ui.ScriptWindow):
	tooltipInfo = None

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Initialize()
		self.LoadWindow()

	def Initialize(self):
		self.KillPoint = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/stone_event.py")
		
		self.Thinboard = self.GetChild("Thinboard")
		self.StoneEventTitle = self.GetChild("StoneEventTitle")
		self.Point = self.GetChild("Point")
		self.InformationBtn = self.GetChild("InformationBtn")

		self.tlInfo = uiToolTip.ItemToolTip()
		self.tlInfo.Hide()
		self.tooltipInfo = [self.tlInfo]*5
		self.InformationText = [localeInfo.STONE_EVENT_INFO_TITLE,
								localeInfo.STONE_EVENT_INFO_TOOLTIP_LINE1,
								localeInfo.STONE_EVENT_INFO_TOOLTIP_LINE2,
								localeInfo.STONE_EVENT_INFO_TOOLTIP_LINE3,
								""
		]
		for i in xrange(5):
			self.tooltipInfo[i].SetFollow(True)
			self.tooltipInfo[i].AlignHorizonalCenter()
			if i == 0:
				TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
				self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i], TITLE_COLOR)
			else:
				self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i])
			self.tooltipInfo[i].Hide()
			self.tooltipInfo[i].toolTipWidth += 55
			
	def Hide(self):
		for i in xrange(5):
			if self.tooltipInfo != None:
				self.tooltipInfo[i].Hide()

	def SetPoint(self, arg1):
		self.KillPoint = int(arg1)
		
	def Destroy(self):
		self.tlInfo.Hide()

		
	def OnUpdate(self):
		for i in xrange(5):
			if self.InformationBtn.IsIn():
				self.tooltipInfo[i].Show()
			else:
				self.tooltipInfo[i].Hide()

		if self.Point < 1:
			self.Point.SetText(localeInfo.STONE_KILL_TEXT + str(0))
		else:
			self.Point.SetText(localeInfo.STONE_KILL_TEXT + str(self.KillPoint))