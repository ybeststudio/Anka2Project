if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import grp
import wndMgr
import uiScriptLocale
import localeInfo
import chat
import uiToolTip
import skill
import cfg
import uiCommon
import sys

COLOR_PRESET_SELECT_UI_SHOW_MAX = 10
HEX_CODE_LENGTH = 7
COLOR_PRESET_SAVE_MAX = (COLOR_PRESET_SELECT_UI_SHOW_MAX * (HEX_CODE_LENGTH + 1)) - 1

class SkillColorWindow(ui.ScriptWindow):
	def __init__(self, skillSlot, skillIndex):
		ui.Window.__init__(self)
		self.isLoaded = FALSE

		self.skillSlot = skillSlot
		self.skillIndex = skillIndex

		self.pickerPos = (0, 0)
		self.genColor = None
		self.tmpGenColor = None
		self.colorMarker = None
		self.toolTip = None
		self.popup = None
		self.questionDialog = None
		self.selectedColor = FALSE
		self.page = 1
		self.colorPresetDict = {}
		self.colorPresetListOpen = FALSE
		self.colorPresetWindowHeight = 0
		self.colorPresetPreview = FALSE
		self.selectedColorPreset = 0

		self.selectedColorTab = 0
		self.updatesSinceColorUpdate = 0
		self.ColorAll = FALSE
		self.skillColors = []

	def __del__(self):
		ui.Window.__del__(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/SkillColorWindow.py")
		except:
			import exception
			exception.Abort("SkillColorWindow.__LoadWindow.LoadScriptFile")

		try:
			self.__BindObject()
		except:
			import exception
			exception.Abort("SkillColorWindow.__LoadWindow.__BindObject")

		try:
			self.__BindEvent()
		except:
			import exception
			exception.Abort("SkillColorWindow.__LoadWindow.__BindEvent")

	def __BindObject(self):
		self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.Close))

		self.bgColorBar = self.GetChild("ColorPreview")
		self.bgImg = self.GetChild("MainFrame")

		self.bgColorPickerImg = self.GetChild("ColorMap")
		self.bgColorPickerButton = self.GetChild("BGColorPickerButton")

		self.bgColorPickerDotImg = self.GetChild("BGColorPickerDotImage")
		self.bgColorPickerDotImg.Hide()

		self.confirmButton = self.GetChild("ConfirmButton")
		self.defaultButton = self.GetChild("DefaultButton")

		self.bg2CustomColorEditLine = self.GetChild("ColorInputValue")

		self.bgColorLayerButton = [self.GetChild("LayerBtn{}".format(i)) for i in range(5)]

		self.checkBox = ui.CheckBox()
		self.checkBox.SetParent(self.GetChild("Board"))
		self.checkBox.SetPosition(-85, 47)
		self.checkBox.SetWindowHorizontalAlignCenter()
		self.checkBox.SetWindowVerticalAlignBottom()
		self.checkBox.SetEvent(ui.__mem_func__(self.SetColorAll), "ON_CHECK", TRUE)
		self.checkBox.SetEvent(ui.__mem_func__(self.SetColorAll), "ON_UNCKECK", FALSE)
		self.checkBox.SetCheckStatus(self.ColorAll)
		self.checkBox.SetTextInfo(uiScriptLocale.SKILL_COLOR_BUTTON_ALL)
		self.checkBox.Show()

	def SetColorAll(self, checkType, Flag):
		self.ColorAll = Flag

	def __BindEvent(self):
		if self.bgColorPickerButton:
			self.bgColorPickerButton.SetEvent(ui.__mem_func__(self.OnClickColorPicker))

		if self.confirmButton:
			self.confirmButton.SetEvent(ui.__mem_func__(self.OnClickConfirmButton))

		if self.defaultButton:
			self.defaultButton.SetEvent(ui.__mem_func__(self.OnClickDefaultButton))

		if self.bg2CustomColorEditLine:
			self.bg2CustomColorEditLine.OnIMEUpdate = ui.__mem_func__(self.OnUpdateHex)

		i = 0
		for btn in self.bgColorLayerButton:
			btn.SetEvent(ui.__mem_func__(self.OnClickColorTab), i)
			i = i + 1

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.ClearToolTip()

		self.popup = uiCommon.PopupDialog()

		self.currentSkillColor = []
		tmpSkillColor = self.GetSkillColorData(self.skillSlot)
		for tmpColor in tmpSkillColor:
			self.currentSkillColor.append(tmpColor)
			self.skillColors.append(tmpColor)

		self.GetCurrentColor(self.skillSlot)
		self.OnClickColorTab(0)

	def OnClickColorTab(self, colorID):
		for btn in self.bgColorLayerButton:
			btn.Enable()

		self.bgColorLayerButton[colorID].Disable()
		self.selectedColorTab = colorID

		decColor = 0

		if self.currentSkillColor[colorID] > 0:
			decColor = self.currentSkillColor[colorID]

		skillColorCode = hex(decColor).split('x')[-1]
		self.OnUpdateHex(skillColorCode)

	def GetCurrentColor(self, skillSlot):
		currentSkillColor = self.GetSkillColorData(skillSlot)
		decColor = 0

		if currentSkillColor[0] > 0:
			decColor = currentSkillColor[0]

		elif currentSkillColor[1] > 0:
			decColor = currentSkillColor[1]

		elif currentSkillColor[2] > 0:
			decColor = currentSkillColor[2]

		elif currentSkillColor[3] > 0:
			decColor = currentSkillColor[3]

		elif currentSkillColor[4] > 0:
			decColor = currentSkillColor[4]

		else:
			return

		skillColorCode = hex(decColor).split('x')[-1]
		self.OnUpdateHex(skillColorCode)

	# colour source; uiCompanion.BuffiSkillColorWindow overrides these for the Buffi skills
	def GetSkillColorData(self, skillSlot):
		return player.GetSkillColor(skillSlot)

	def SetSkillColorData(self, skillSlot, c0, c1, c2, c3, c4):
		player.SetSkillColor(skillSlot, c0, c1, c2, c3, c4)

	def OnClickCancelButton(self):
		self.Close()

	def OnClickDefaultButton(self):
		questionDialog = uiCommon.QuestionDialog("thin")
		questionDialog.SetText(localeInfo.SKILL_COLOR_DO_YOU_RESET)
		questionDialog.SetAcceptEvent(lambda arg = 0 : ui.__mem_func__(self.OnAcceptQuestionDialog)(arg))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OnClickConfirmButton(self):
		if not self.genColor or not self.selectedColor:
			self.popup.SetText(localeInfo.SKILL_COLOR_SELECT_FIRST)
			self.popup.Open()
			return

		questionDialog = uiCommon.QuestionDialog("thin")
		questionDialog.SetText(localeInfo.SKILL_COLOR_DO_YOU_CHANGE)
		questionDialog.SetAcceptEvent(lambda arg = 1 : ui.__mem_func__(self.OnAcceptQuestionDialog)(arg))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OnAcceptQuestionDialog(self, arg):
		if arg == 0:
			self.SetSkillColorData(self.skillSlot, 0, 0, 0, 0, 0)
		else:
			if self.ColorAll:
				self.SetSkillColorData(self.skillSlot, self.skillColors[0], self.skillColors[0],\
					self.skillColors[0], self.skillColors[0], self.skillColors[0])
			else:
				self.SetSkillColorData(self.skillSlot, self.skillColors[0], self.skillColors[1],\
					self.skillColors[2], self.skillColors[3], self.skillColors[4])

		self.Close()

		self.OnCloseQuestionDialog()
		return TRUE

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return TRUE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def Close(self):
		self.Hide()

	def Show(self, page = 1):
		ui.ScriptWindow.Show(self)

		if not self.isLoaded:
			self.page = page
			self.__LoadWindow()

		self.SetCenterPosition()
		self.SetTop()

	def OnClickColorPicker(self):
		rgbColor = self.GetRGBColor()

		if rgbColor[0] <= 20 and rgbColor[1] <= 20 and rgbColor[2] <= 20:
			rgbColorNew = list(rgbColor)
			rgbColorNew[0] = 0
			rgbColorNew[1] = 0
			rgbColorNew[2] = 0
			rgbColor = tuple(rgbColorNew)
			self.selectedColor = FALSE
		else:
			self.selectedColor = TRUE

		r, g, b = (float(rgbColor[0]) / 255, float(rgbColor[1]) / 255, float(rgbColor[2]) / 255)
		self.genColor = (r, g, b)

		if self.bgColorBar:
			self.bgColorBar.SetColor(grp.GenerateColor(r, g, b, 1.0))

		if self.bgColorPickerDotImg:
			self.bgColorPickerDotImg.SetPosition(self.pickerPos[0] - (self.bgColorPickerDotImg.GetWidth()/2), self.pickerPos[1] - (self.bgColorPickerDotImg.GetHeight()/2))
			self.bgColorPickerDotImg.Show()

			colorMarker = ui.TextLine()
			colorMarker.SetParent(self.bgColorPickerDotImg)
			colorMarker.SetPosition(5, -15)
			colorMarker.SetHorizontalAlignCenter()
			r, g, b = self.GetRGBColor()
			hexCode = "#{:02x}{:02x}{:02x}".format(int(r), int(g), int(b))
			colorMarker.SetText("%s" % hexCode)
			colorMarker.Show()
			self.colorMarker = colorMarker

		tmpR, tmpG, tmpB = (float(rgbColor[0]), float(rgbColor[1]), float(rgbColor[2]))
		self.skillColors[self.selectedColorTab] = grp.GenerateColor(tmpR / 255, tmpG / 255, tmpB / 255, 0.0)
		self.currentSkillColor[self.selectedColorTab] = grp.GenerateColor(tmpR / 255, tmpG / 255, tmpB / 255, 0.0)
		self.tmpGenColor = (tmpR, tmpG, tmpB)

		if self.bg2CustomColorEditLine:
			r, g, b = (self.tmpGenColor[0], self.tmpGenColor[1], self.tmpGenColor[2])
			hexCode = "#{:02x}{:02x}{:02x}".format(int(r), int(g), int(b))
			self.bg2CustomColorEditLine.SetText(str(hexCode))

	def HexToRGB(self, strValue):
		strValue = strValue.lstrip("#")
		lv = len(strValue)
		rgbCode = (0, 0 ,0)
		try:
			rgbCode = tuple(int(strValue[i:i+int(lv/3)], 16) for i in range(0, lv, int(lv/3)))
		except:
			pass

		return rgbCode

	def GetRGBColor(self):
		xMouse, yMouse = wndMgr.GetRealMousePosition()
		return wndMgr.GetColorAtPosition(xMouse, yMouse)

	def ChangeColor(self, x, y):
		if x > 255:
			x = 255

		if y > 255:
			y = 255

		rgbColor = self.GetRGBColor()
		r, g, b = (float(rgbColor[0]) / 255, float(rgbColor[1]) / 255, float(rgbColor[2]) / 255)

		self.updatesSinceColorUpdate = 0

		if self.bgColorBar:
			self.bgColorBar.SetColor(grp.GenerateColor(r, g, b, 1.0))

	def OnUpdateHex(self, hexCode = ""):
		ui.EditLine.OnIMEUpdate(self.bg2CustomColorEditLine)
		text = self.bg2CustomColorEditLine.GetText()

		if len(text):
			self.bg2CustomColorEditLine.SetText(str(text))
			self.bgColorPickerDotImg.Hide()

		strLen = len(str(text))
		if strLen >= HEX_CODE_LENGTH - 1:
			if text.find("#") == -1:
				self.bg2CustomColorEditLine.SetText("")
				self.bg2CustomColorEditLine.SetText("#" + str(text) + "")

			color = str(self.bg2CustomColorEditLine.GetText()).split("#")
			rgbColor = self.HexToRGB(str(color[1]))

			if rgbColor[0] <= 20 and rgbColor[1] <= 20 and rgbColor[2] <= 20:
				rgbColorNew = list(rgbColor)
				rgbColorNew[0] = 0
				rgbColorNew[1] = 0
				rgbColorNew[2] = 0
				rgbColor = tuple(rgbColorNew)
				self.selectedColor = FALSE
			else:
				self.selectedColor = TRUE

			r, g, b = (float(rgbColor[0]) / 255, float(rgbColor[1]) / 255, float(rgbColor[2]) / 255)
			self.genColor = (r, g, b)

			if self.bgColorBar:
				self.bgColorBar.SetColor(grp.GenerateColor(r, g, b, 1.0))

			tmpR, tmpG, tmpB = (float(rgbColor[0]), float(rgbColor[1]), float(rgbColor[2]))
			self.skillColors[self.selectedColorTab] = grp.GenerateColor(tmpR / 255, tmpG / 255, tmpB / 255, 0.0)
			self.currentSkillColor[self.selectedColorTab] = grp.GenerateColor(tmpR / 255, tmpG / 255, tmpB / 255, 0.0)
			self.tmpGenColor = (tmpR, tmpG, tmpB)
		else:
			self.selectedColor = FALSE

	def OnUpdate(self):
		self.updatesSinceColorUpdate = self.updatesSinceColorUpdate + 1
		if self.bgColorPickerButton.IsIn():
			xBtn, yBtn = self.bgColorPickerButton.GetGlobalPosition()
			btnHeight = self.bgColorPickerButton.GetHeight()
			xMousePos, yMousePos = wndMgr.GetMousePosition()

			if yMousePos - yBtn < btnHeight - 1:
				xMouse = xMousePos - xBtn
				yMouse = yMousePos - yBtn

				if xMouse > 255:
					xMouse = 255

				if yMouse > 255:
					yMouse = 255

				self.pickerPos = (xMouse, yMouse)
				if self.updatesSinceColorUpdate > 5:
					self.ChangeColor(xMouse, yMouse)
					if self.toolTip:
						try:
							r, g, b = self.GetRGBColor()
							hexCode = "#{:02x}{:02x}{:02x}".format(int(r), int(g), int(b))
							toolTipText = localeInfo.SKILL_COLOR_PICK_TOOLTIP % (skill.GetSkillName(self.skillIndex, 0), hexCode)
							ToolTipText = hexCode
							arglen = len(str(toolTipText))
							self.toolTip.ClearToolTip()
							self.toolTip.SetThinBoardSize(5 * arglen)
							self.toolTip.AppendTextLine(toolTipText, 0xffffff00)
							self.toolTip.Show()
						except:
							return
					else:
						self.toolTip.Hide()
		else:
			if self.genColor and self.bgColorBar:
				self.bgColorBar.SetColor(grp.GenerateColor(self.genColor[0], self.genColor[1], self.genColor[2], 1.0))
			else:
				self.bgColorBar.SetColor(grp.GenerateColor(0, 0, 0, 0.0))

			if self.toolTip:
				self.toolTip.Hide()

	def __CreateGameTypeToolTip(self, title, descList):
		tooltip = uiToolTip.ToolTip()
		tooltip.SetTitle(title)
		tooltip.AppendSpace(7)

		for desc in descList:
			tooltip.AutoAppendTextLine(desc)

		tooltip.AlignHorizonalCenter()
		tooltip.SetTop()
		return tooltip
