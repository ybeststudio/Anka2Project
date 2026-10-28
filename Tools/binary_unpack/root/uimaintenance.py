if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui
import wndMgr
import time
import localeInfo
import chat
import constInfo

pText_x = 65
pText_y = 5 

def pTableTranslate(i):
	translate = {
					1 : localeInfo.MAINTENANCE_TEXT_INFO,
					2 : localeInfo.MAINTENANCE_TEXT_LENGTH,
					3 : localeInfo.MAINTENANCE_TEXT_REASON_TECHNICAL,
					4 : localeInfo.MAINTENANCE_TEXT_REASON,
					5 : localeInfo.MAINTENANCE_TEXT_TIME_DAYS,
					6 : localeInfo.MAINTENANCE_TEXT_TIME_HOURS,
					7 : localeInfo.MAINTENANCE_TEXT_TIME_MINUTES,
				}

	if translate.has_key(i):
		return translate[i]

global_t = (0, 24, 60)

def CalculateTimeLeft(iTime):
	A, B = divmod(iTime, global_t[2])
	C, A = divmod(A, global_t[2])
	return "%02d:%02d:%02d" % (C, A, B)

def CalculateDuration(iDuration):
	if iDuration < global_t[2]:
		return "0" + (pTableTranslate(7))

	pMin = int((iDuration / global_t[2]) % global_t[2])
	pHour = int((iDuration / global_t[2]) / global_t[2]) % global_t[1]
	pDay = int(int((iDuration / global_t[2]) / global_t[2]) / global_t[1])

	iText = ""
	if pDay > global_t[0]:
		iText += str(pDay) + (pTableTranslate(5))
		iText += " "

	if pHour > global_t[0]:
		iText += str(pHour) + (pTableTranslate(6))
		iText += " "

	if pMin > global_t[0]:
		iText += str(pMin) + (pTableTranslate(7))

	return iText

class MaintenanceClass(ui.InvisibleBoard):
	def __init__(self):
		ui.InvisibleBoard.__init__(self)

		self.Destroy()
		self.LoadInfo()

	def __del__(self):
		ui.InvisibleBoard.__del__(self)

	def Destroy(self):
		self.sBoard = None

	def LoadInfo(self):
		sBoard = ui.ImageBox()
		sBoard.SetParent(self)
		sBoard.AddFlag("attach")
		sBoard.AddFlag("float")
		sBoard.LoadImage("d:/ymir work/ui/game/option/info_bg.tga")
		sBoard.SetPosition(0, 0)
		sBoard.Show()
		self.sBoard = sBoard

		self.sTime = ui.TextLine()
		self.sTime.SetPosition(pText_x - 2, pText_y + 9)

		self.sDuration = ui.TextLine()
		self.sDuration.SetPosition(pText_x - 2, pText_y + 25)

		self.sReason = ui.TextLine()
		self.sReason.SetPosition(pText_x - 2, pText_y + 41)

		pWindow = [self.sTime, self.sDuration, self.sReason]

		for i in pWindow:
			i.SetFontName("Verdana:15")
			i.SetParent(sBoard)
			i.SetOutline(1)
			i.Show()

		self.SetPosition(wndMgr.GetScreenWidth()/2 - 320/2, 4)

	def SetTime(self, iLeft):
		timeLeft = iLeft - app.GetGlobalTimeStamp()

		if timeLeft < 0:
			timeLeft = 0
			self.Hide()

		self.sTime.SetText((pTableTranslate(1) % (CalculateTimeLeft(timeLeft))))

	def OpenMaintenance(self, iTime, iDuration, iReason):
		self.leftTime = app.GetGlobalTimeStamp() + int(iTime)

		self.sDuration.SetText((pTableTranslate(2) % (CalculateDuration(int(iDuration)))))

		if str(iReason) == "no_reason":
			self.sReason.SetText((pTableTranslate(3)))
		else:
			self.sReason.SetText((pTableTranslate(4) % (str(iReason).replace("//"," "))))

		self.Show()

	def OnUpdate(self):
		self.SetTime(int(self.leftTime))

	def Close(self):
		self.Hide()
