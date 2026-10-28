if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui
import grp
import wndMgr

class TextBar(ui.Window):
	def __init__(self, width, height):
		ui.Window.__init__(self)
		self.handle = grp.CreateTextBar(width, height)

	def __del__(self):
		ui.Window.__del__(self)
		grp.DestroyTextBar(self.handle)

	def ClearBar(self):
		grp.ClearTextBar(self.handle)

	def SetClipRect(self, x1, y1, x2, y2):
		grp.SetTextBarClipRect(self.handle, x1, y1, x2, y2)

	def TextOut(self, x, y, text):
		grp.TextBarTextOut(self.handle, x, y, text)

	def OnRender(self):
		x, y = self.GetGlobalPosition()
		grp.RenderTextBar(self.handle, x, y)

	def SetTextColor(self, r, g, b):
		grp.TextBarSetTextColor(self.handle, r, g, b)

	def GetTextExtent(self, text):
		return grp.TextBarGetTextExtent(self.handle, text)

class TipBoard(ui.Bar):
	SCROLL_WAIT_TIME = 3.0
	TIP_DURATION = 5.0
	FONT_HEIGHT = 15
	TEXT_POS_Y = 10
	LINE_HEIGHT = FONT_HEIGHT + 5
	STEP_HEIGHT = LINE_HEIGHT + 5
	SCREEN_WIDTH = wndMgr.GetScreenWidth()

	def __init__(self):
		ui.Bar.__init__(self)

		self.AddFlag("not_pick")
		self.tipList = []
		self.curPos = 0
		self.dstPos = 0
		self.nextScrollTime = 0
		self.width = self.SCREEN_WIDTH

		self.SetPosition(0, 140)
		self.SetSize(self.SCREEN_WIDTH, 35)
		self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
		self.SetWindowHorizontalAlignCenter()
		self.__CreateTextBar()

	def __del__(self):
		ui.Bar.__del__(self)

	def __CreateTextBar(self):
		x, y = self.GetGlobalPosition()

		self.textBar = BigTextBar(self.SCREEN_WIDTH*2, 300, self.FONT_HEIGHT)
		self.textBar.SetParent(self)
		self.textBar.SetPosition(6, 8)
		self.textBar.SetTextColor(242, 231, 193)
		self.textBar.SetClipRect(0, y, self.SCREEN_WIDTH, y+8+self.STEP_HEIGHT)
		self.textBar.Show()

	def __CleanOldTip(self):
		leaveList = []
		for tip in self.tipList:
			madeTime = tip[0]
			if app.GetTime() - madeTime > self.TIP_DURATION:
				pass
			else:
				leaveList.append(tip)

		self.tipList = leaveList

		if not leaveList:
			self.textBar.ClearBar()
			self.Hide()
		return

		self.__RefreshBoard()

	def __RefreshBoard(self):
		self.textBar.ClearBar()

		index = 0
		for tip in self.tipList:
			text = tip[1]

			if text:
				(text_width, text_height) = self.textBar.GetTextExtent(text)

				if text_width>self.SCREEN_WIDTH:
					self.textBar.TextOut(0, index*self.STEP_HEIGHT, text)
				else:
					self.textBar.TextOut((wndMgr.GetScreenWidth()-text_width)/2, index*self.STEP_HEIGHT, text)

			index += 1

	def SetTip(self, text):
		if not app.IsVisibleNotice():
			return

		curTime = app.GetTime()
		self.tipList.append((curTime, text))
		self.__RefreshBoard()

		self.nextScrollTime = app.GetTime() + 1.0

		if not self.IsShow():
			self.curPos = -self.STEP_HEIGHT
			self.dstPos = -self.STEP_HEIGHT
			self.textBar.SetPosition(wndMgr.GetScreenWidth()/3, self.TEXT_POS_Y - self.curPos)
			self.Show()

	def OnUpdate(self):
		if not self.tipList:
			self.Hide()
			return

		if app.GetTime() > self.nextScrollTime:
			self.nextScrollTime = app.GetTime() + self.SCROLL_WAIT_TIME

			self.dstPos = self.curPos + self.STEP_HEIGHT

		if self.dstPos > self.curPos:
			self.curPos += 1
			self.textBar.SetPosition(wndMgr.GetScreenWidth()/3, self.TEXT_POS_Y - self.curPos)

			if self.curPos > len(self.tipList)*self.STEP_HEIGHT:
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT

				self.__CleanOldTip()

class TipBoardNew(ui.ThinBoard):
	def __del__(self):
		ui.ThinBoard.__del__(self)

	def Destroy(self):
		self.__tipList=[]
		self.__min_width=0
		self.__max_duration=0.0
		self.__process_duration=0.0
		self.__text_duration=0.0
		self.__sleepTime=0.0
		self.__currentTip=""
		self.__textBar=None
		self.processType = FALSE
		self.__open_max_duration =  0.0
		self.__open_text_duration =  0.0
		self.__close_max_duration =  0.0
		self.__close_text_duration =  0.0
		self.__click_close_max_duration =  0.0
		self.__click_close_text_duration = 0.0

	def __init__(self):
		ui.ThinBoard.__init__(self)
		self.SetWindowHorizontalAlignCenter()
		self.Destroy()

		self.__min_width = 40
		self.__min_height = 30
		self.__yPos = 70

		self.processType = TRUE

		self.__open_max_duration = 5.0
		self.__open_text_duration = 0.30

		self.__close_max_duration = 1.0
		self.__close_text_duration = 0.1

		self.__click_close_max_duration = 0.7
		self.__click_close_text_duration = 0.05

		textBar = ui.TextLine()
		textBar.SetParent(self)
		textBar.AddFlag("not_pick")
		textBar.SetOutline()
		textBar.SetHorizontalAlignCenter()
		textBar.SetVerticalAlignCenter()
		textBar.SetWindowHorizontalAlignCenter()
		textBar.SetWindowVerticalAlignCenter()
		self.__textBar = textBar

		self.AddFlag("attach")
		self.AddFlag("movable")
		self.AddFlag("float")
		self.SetMouseOverInEvent(ui.__mem_func__(self.OnMouseOverIn))
		self.SetMouseOverOutEvent(ui.__mem_func__(self.OnMouseOverOut))
		self.SetMouseLeftButtonUpEvent(ui.__mem_func__(self.OnMouseClick))

	def OnMouseClick(self):
		self.processType = FALSE
		self.__process_duration = app.GetTime()+self.__click_close_max_duration
		self.__text_duration = (self.__click_close_text_duration/(len(self.__currentTip)))

	def SetYPos(self, globalY):
		self.__yPos = globalY

	def OnMouseOverIn(self):
		self.SetAlpha(0.5)

	def OnMouseOverOut(self):
		self.SetAlpha(1.0)

	def SetDefaultData(self):
		self.__sleepTime = 0.0
		self.__text_duration = 0.0
		self.__process_text = 0.0
		self.__process_duration = 0.0
		self.__currentTip = ""

	def SetNewTipHideData(self):
		self.__process_text = 0.0
		self.__process_duration = app.GetTime()+self.__close_max_duration
		self.__text_duration = (self.__close_text_duration/(len(self.__currentTip)))
		self.processType = FALSE

	def SetNewTipOpenData(self, tipMessage):
		self.__textBar.SetText("")
		self.__process_text = 0.0
		self.__process_duration = app.GetTime()+self.__open_max_duration
		self.__currentTip = tipMessage
		self.__text_duration = (self.__open_text_duration/(len(self.__currentTip)))
		self.processType = TRUE
		self.SetPosition(0, self.__yPos)

		if not self.__textBar.IsShow():
			self.__textBar.Show()
			self.__textBar.SetTop()
			self.SetSize(self.__min_width+self.__textBar.GetTextSize()[0],self.__min_height)
			self.SetPosition(0, self.__yPos)

	def SetTip(self, newTip):
		if self.__process_duration != 0.0:
			self.__tipList.append(newTip)
			return

		self.__sleepTime = app.GetTime()+0.5
		self.__alpha_duration = 0.0
		self.__alphaValue = 0.0
		self.__alpha = 0.5/30.0
		self.SetNewTipOpenData(newTip)
		self.Show()

	def OnUpdate(self):
		if self.__sleepTime >= app.GetTime():
			if app.GetTime() >= self.__alpha_duration:
				self.__alphaValue += self.__alpha
				self.__alpha_duration = app.GetTime()+0.01
				if self.processType:
					self.SetAlpha(0.5+self.__alphaValue)
				else:
					self.SetAlpha(1.0-self.__alphaValue)
			return

		if self.__process_duration != 0:
			lastDuration = self.__process_duration-app.GetTime()
			if self.processType:
				if app.GetTime() > self.__process_text:
					self.__process_text = app.GetTime()+self.__text_duration
					nextChar = len(self.__textBar.GetText())

					if nextChar < len(self.__currentTip):
						self.__textBar.SetText(self.__textBar.GetText()+self.__currentTip[nextChar])

					if lastDuration <= 0.0:
						self.SetNewTipHideData()
			else:
				if app.GetTime() > self.__process_text:
					self.__process_text = app.GetTime()+self.__text_duration
					nextChar = len(self.__textBar.GetText())

					if nextChar > 0:
						self.__textBar.SetText(self.__textBar.GetText()[:nextChar-1])

						if len(self.__textBar.GetText()) == 0:
							if len(self.__tipList) > 0:
								lastDuration = 0
							else:
								self.__sleepTime = app.GetTime()+0.5
								self.__alpha_duration = 0.0
								self.__alphaValue = 0.0
								self.__alpha = 0.5/30.0
					else:
						lastDuration = 0.0

				if lastDuration <= 0.0:
					if len(self.__tipList)>0:
						self.SetNewTipOpenData(self.__tipList[0])
						del self.__tipList[0]
						return

					self.SetDefaultData()
					self.Hide()

			self.SetSize(self.__min_width+self.__textBar.GetTextSize()[0],self.__min_height)
			self.SetPosition(0, self.__yPos)

class BigTextBar(TextBar):
	def __init__(self, width, height, fontSize):
		ui.Window.__init__(self)
		self.handle = grp.CreateBigTextBar(width, height, fontSize)

class BigBoard(ui.Bar):

	SCROLL_WAIT_TIME = 5.0
	TIP_DURATION = 10.0
	FONT_WIDTH = 18
	FONT_HEIGHT	= 18
	LINE_WIDTH = 500
	LINE_HEIGHT = FONT_HEIGHT + 5
	STEP_HEIGHT = LINE_HEIGHT * 2
	LINE_CHANGE_LIMIT_WIDTH = 350

	FRAME_IMAGE_FILE_NAME_LIST = [
		"d:/ymir work/ui/game/oxevent/frame_0.sub",
		"d:/ymir work/ui/game/oxevent/frame_1.sub",
		"d:/ymir work/ui/game/oxevent/frame_2.sub",
	]

	FRAME_IMAGE_STEP = 256

	FRAME_BASE_X = -20
	FRAME_BASE_Y = -12

	def __init__(self):
		ui.Bar.__init__(self)

		self.AddFlag("not_pick")
		self.tipList = []
		self.curPos = 0
		self.dstPos = 0
		self.nextScrollTime = 0

		self.SetPosition(0, 150)
		self.SetSize(512, 55)
		self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
		self.SetWindowHorizontalAlignCenter()

		self.__CreateTextBar()
		self.__LoadFrameImages()

	def __LoadFrameImages(self):
		x = self.FRAME_BASE_X
		y = self.FRAME_BASE_Y
		self.imgList = []
		for imgFileName in self.FRAME_IMAGE_FILE_NAME_LIST:
			self.imgList.append(self.__LoadImage(x, y, imgFileName))
			x += self.FRAME_IMAGE_STEP

	def __LoadImage(self, x, y, fileName):
		img = ui.ImageBox()
		img.SetParent(self)
		img.AddFlag("not_pick")
		img.LoadImage(fileName)
		img.SetPosition(x, y)
		img.Show()
		return img

	def __del__(self):
		ui.Bar.__del__(self)

	def __CreateTextBar(self):
		x, y = self.GetGlobalPosition()

		self.textBar = BigTextBar(self.LINE_WIDTH, 300, self.FONT_HEIGHT)
		self.textBar.SetParent(self)
		self.textBar.SetPosition(6, 8)
		self.textBar.SetTextColor(242, 231, 193)
		self.textBar.SetClipRect(0, y+8, wndMgr.GetScreenWidth(), y+8+self.STEP_HEIGHT)
		self.textBar.Show()

	def __CleanOldTip(self):
		curTime = app.GetTime()
		leaveList = []
		for madeTime, text in self.tipList:
			if curTime + self.TIP_DURATION <= madeTime:
				leaveList.append(text)

		self.tipList = leaveList

		if not leaveList:
			self.textBar.ClearBar()
			self.Hide()
			return

		self.__RefreshBoard()

	def __RefreshBoard(self):

		self.textBar.ClearBar()

		if len(self.tipList) == 1:
			checkTime, text = self.tipList[0]
			(text_width, text_height) = self.textBar.GetTextExtent(text)
			self.textBar.TextOut((500-text_width)/2, (self.STEP_HEIGHT-8-text_height)/2, text)

		else:
			index = 0
			for checkTime, text in self.tipList:
				(text_width, text_height) = self.textBar.GetTextExtent(text)
				self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, text)
				index += 1

	def SetTip(self, text):

		if not app.IsVisibleNotice():
			return

		curTime = app.GetTime()
		self.__AppendText(curTime, text)
		self.__RefreshBoard()

		self.nextScrollTime = curTime + 1.0

		if not self.IsShow():
			self.curPos = -self.STEP_HEIGHT
			self.dstPos = -self.STEP_HEIGHT
			self.textBar.SetPosition(3, 8 - self.curPos)
			self.Show()

	def __AppendText(self, curTime, text):
		import dbg
		prevPos = 0
		while 1:
			curPos = text.find(" ", prevPos)
			if curPos < 0:
				break

			(text_width, text_height) = self.textBar.GetTextExtent(text[:curPos])
			if text_width > self.LINE_CHANGE_LIMIT_WIDTH:
				self.tipList.append((curTime, text[:prevPos]))
				self.tipList.append((curTime, text[prevPos:]))
				return

			prevPos = curPos + 1

		self.tipList.append((curTime, text))

	def OnUpdate(self):
		if not self.tipList:
			self.Hide()
			return

		if app.GetTime() > self.nextScrollTime:
			self.nextScrollTime = app.GetTime() + self.SCROLL_WAIT_TIME

			self.dstPos = self.curPos + self.STEP_HEIGHT

		if self.dstPos > self.curPos:
			self.curPos += 1
			self.textBar.SetPosition(3, 8 - self.curPos)

			if self.curPos > len(self.tipList)*self.LINE_HEIGHT:
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT

				self.__CleanOldTip()

if app.ENABLE_RENEWAL_OX_EVENT:
	class BigBoardControl(ui.Bar):

		FONT_WIDTH = 18
		FONT_HEIGHT = 18
		LINE_WIDTH = 500
		LINE_HEIGHT = FONT_HEIGHT + 5
		STEP_HEIGHT = LINE_HEIGHT * 2
		LINE_CHANGE_LIMIT_WIDTH = 450
		BIG_TEXTBAR_MAX_HIGHT = 1000

		FRAME_IMAGE_FILE_NAME_LIST = [
			"d:/ymir work/ui/game/oxevent/frame_new_0.sub", 
			"d:/ymir work/ui/game/oxevent/frame_new_1.sub", 
		]

		FRAME_IMAGE_STEP = 256

		FRAME_BASE_X = -20
		FRAME_BASE_Y = -12

		def __init__(self):
			ui.Bar.__init__(self)

			self.AddFlag("not_pick")
			self.tipList = []
			self.curPos = 0
			self.dstPos = 0
			self.nextScrollTime = 0
			self.scrollstop = 0
			self.pretexsize = 0
			self.nexttextsize = 0
			self.changeline = 0
			self.endsayi = 0
			self.addtipListcount = 0
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				self.__code_page = 0

			self.SetPosition(0, 150)
			self.SetSize(512, 55)
			self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
			self.SetWindowHorizontalAlignCenter()

			self.__CreateTextBar()
			self.__LoadFrameImages()

		def __LoadFrameImages(self):
			x = self.FRAME_BASE_X
			y = self.FRAME_BASE_Y
			self.imgList = []
			for imgFileName in self.FRAME_IMAGE_FILE_NAME_LIST:
				self.imgList.append(self.__LoadImage(x, y, imgFileName))

		def __LoadImage(self, x, y, fileName):
			img = ui.ImageBox()
			img.SetParent(self)
			img.AddFlag("not_pick")
			img.LoadImage(fileName)
			img.SetPosition(x, y)
			img.Show()
			return img

		def __del__(self):
			self.tipList = []
			if not app.ENABLE_MULTILANGUAGE_CHATTING:
				self.textBar.ClearBar()
			self.Hide()
			ui.Bar.__del__(self)

		def __CreateTextBar(self):
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				self.__text_window = ui.Window()
				self.__text_window.SetParent(self)
				self.__text_window.SetPosition(6, 8)
				self.__text_window.SetSize(self.LINE_WIDTH, self.STEP_HEIGHT)
				self.__text_window.Show()

				self.__text_line = ui.TextLine()
				self.__text_line.SetParent(self.__text_window)
				self.__text_line.SetFontName(app.GetFontFromCodePage(app.GetDefaultCodePage()) + ":" + str(self.FONT_HEIGHT))
				self.__text_line.SetPackedFontColor(grp.GenerateColor(242.0/255.0, 231.0/255.0, 193.0/255.0, 1.0))
				self.__text_line.SetPosition(0, 0)
				self.__text_line.Show()
			else:
				x, y = self.GetGlobalPosition()

				self.textBar = BigTextBar(self.LINE_WIDTH, self.BIG_TEXTBAR_MAX_HIGHT, self.FONT_HEIGHT)
				self.textBar.SetParent(self)
				self.textBar.SetPosition(6, 8)
				self.textBar.SetTextColor(242, 231, 193)
				self.textBar.SetClipRect(0, y+8, wndMgr.GetScreenWidth(), y+8+self.STEP_HEIGHT)
				self.textBar.Show()

		def __CleanOldTip(self):
			self.tipList = []
			if not app.ENABLE_MULTILANGUAGE_CHATTING:
				self.textBar.ClearBar()
			self.Hide()

		def __RefreshBoard(self):
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				return
			self.textBar.ClearBar()
			preaddtipListcount = self.addtipListcount
			self.addtipListcount = 0

			index = 0
			for text in self.tipList:
				(text_width, text_height) = self.textBar.GetTextExtent(text)
				if index == 0:
					self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, " ")
					index += 1
					self.addtipListcount += 1

				if index == (len(self.tipList) - (len(self.tipList) - self.pretexsize))+1 and not self.pretexsize == 0:

					self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, " ")
					index += 1
					self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, " ")
					index += 1
					self.addtipListcount += 2

				self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, text)
				index += 1

			if self.tipList:
				index += 1
				self.addtipListcount += 1
				self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, " ")

		def SetTip(self, text):

			if not app.IsVisibleNotice():
				return

			if text == "#start" or text == "#end" or text == "#send":
				pass
			else:
				self.__AppendText(text)

			if text == "#start" or text == "#send":

				if text == "#send" and self.pretexsize == 0:
					text = "#end"

				self.__RefreshBoard()

				if text == "#start":
					self.STEP_HEIGHT = (self.LINE_HEIGHT * (len(self.tipList) + self.addtipListcount))
					self.pretexsize = len(self.tipList)

					x, y = self.GetGlobalPosition()
					if not app.ENABLE_MULTILANGUAGE_CHATTING:
						self.textBar.SetClipRect(0, y+8, wndMgr.GetScreenWidth(), y+8+self.STEP_HEIGHT)
					self.SetSize(512, 10 + (self.LINE_HEIGHT * (len(self.tipList) + self.addtipListcount))+10)
					self.imgList[1].SetPosition(self.FRAME_BASE_X, self.STEP_HEIGHT + 10)

					self.scrollstop = 0
					self.changeline = 1

				if text == "#send":
					self.scrollstop = 1
					self.nexttextsize = len(self.tipList) - self.pretexsize

				if not self.IsShow():
					self.curPos = -self.STEP_HEIGHT
					self.dstPos = -self.STEP_HEIGHT
					if app.ENABLE_MULTILANGUAGE_CHATTING:
						self.__text_window.SetPosition(3, 8 - self.curPos)
					else:
						self.textBar.SetPosition(3, 8 - self.curPos)
					self.Show()

			if text == "#end":
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT
				if app.ENABLE_MULTILANGUAGE_CHATTING:
					self.__text_window.SetPosition(3, 8 - self.curPos)
				else:
					self.textBar.SetPosition(3, 8 - self.curPos)
				self.textcount = 0
				self.changeline = 0
				self.pretexsize = 0
				self.nexttextsize = 0
				self.addtipListcount = 0
				self.__CleanOldTip()

		def __AppendText(self, text):
			if app.ENABLE_MULTILANGUAGE_CHATTING:
				self.__text_line.SetTextWithCodePage(text, self.__code_page)
				self.tipList.append(text)
				return
			import dbg
			prevPos = 0
			while 1:
				curPos = text.find(" ", prevPos)
				if curPos < 0:
					break

				(text_width, text_height) = self.textBar.GetTextExtent(text[:curPos])
				if text_width > self.LINE_CHANGE_LIMIT_WIDTH:
					self.tipList.append(text[:prevPos])

					(text_width, text_height) = self.textBar.GetTextExtent(text[prevPos:])
					if text_width > self.LINE_CHANGE_LIMIT_WIDTH:
						text = text[prevPos:]
						if text[:prevPos] == "":
							prevPos = curPos + 1
						else:
							prevPos = 0
					else:
						text = text[prevPos:]
						break
				else:
					prevPos = curPos + 1

			self.tipList.append(text)

		def OnUpdate(self):

			if not self.tipList:
				self.Hide()
				return

			if self.changeline:
				self.changeline = 0
				self.dstPos = self.curPos + self.STEP_HEIGHT

			if self.dstPos > self.curPos:
				self.curPos += 1
				self.textBar.SetPosition(3, 8 - self.curPos)

			if self.curPos > len(self.tipList)*self.LINE_HEIGHT:
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT

				self.__CleanOldTip()

if __name__ == "__main__":	
	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui
	
	#wndMgr.SetOutlineFlag(TRUE)

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(TRUE)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create("METIN2 CLOSED BETA", systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	wnd = BigBoard()
	wnd.Show()
	wnd.SetTip("안녕하세요")
	wnd.SetTip("저는 빗자루 입니다")

	app.Loop()
