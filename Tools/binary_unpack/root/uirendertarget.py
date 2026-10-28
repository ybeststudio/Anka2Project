if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import renderTarget
import item
import dbg
import nonplayer
import chat
import playersettingmodule
import localeInfo

from _weakref import proxy

IMG_DIR = "d:/ymir work/ui/game/render/"
RENDER_WINDOW_SIZE = [500, 500]

def IsCanShowItems(itemVnum):
	item.SelectItem(itemVnum)
	(itemType, itemSubType) = (item.GetItemType(),item.GetItemSubType())

	if (item.ITEM_TYPE_COSTUME == itemType and (itemSubType == item.COSTUME_TYPE_HAIR or itemSubType == item.COSTUME_TYPE_BODY or itemSubType == item.COSTUME_TYPE_ACCE or itemSubType == item.COSTUME_TYPE_WEAPON)):
		return TRUE
	elif item.ITEM_TYPE_ARMOR == itemType and itemSubType == item.ARMOR_BODY:
		return TRUE
	elif item.ITEM_TYPE_WEAPON == itemType and itemSubType != item.WEAPON_ARROW:
		return TRUE
	elif item.ITEM_TYPE_PET == itemType:
		return TRUE
	elif item.ITEM_TYPE_MOUNT == itemType:
		return TRUE

	if app.ENABLE_MOUNT_PET_SKIN and item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_MOUNT:
		return TRUE

	if app.ENABLE_MOUNT_PET_SKIN and item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_PET:
		return TRUE

	return FALSE

def get_length(x):
	return len(x[0])

def GetRefineMaxLevel(vnum):
	return 9

def getRealVnum(vnum):
	isRefineItem = FALSE
	item.SelectItem(vnum)
	isRefineItem = FALSE
	level = "0"
	itemname = item.GetItemName()
	pos = itemname.find("+")

	if pos != -1:
		level = itemname[pos+1:]
		if level.isdigit():
			isRefineItem = TRUE
			vnum -= int(level)
	return (vnum, isRefineItem)

class RenderTargetWindow(ui.BoardWithTitleBar):
	children = {}

	def OnPressEscapeKey(self):
		return self.Close()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Destroy(self):
		renderWindow = self.GetRenderWindow()
		if renderWindow != None:
			renderWindow.Destroy()

		self.children = {}

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.Destroy()
		self.__LoadWindow()

	def GetRenderWindow(self):
		if self.children.has_key("renderWindow"):
			return self.children["renderWindow"]
		return None

	def Close(self):
		renderWindow = self.GetRenderWindow()
		if renderWindow != None:
			renderWindow.Close()

		self.Hide()
		return TRUE

	def Open(self, renderType = 0, vnumIndex = 11299):
		renderWindow = self.GetRenderWindow()
		if renderWindow != None:
			if renderType == 0 and IsCanShowItems(vnumIndex) == FALSE:
				return
			renderWindow.PrepareRenderTarget(renderType, vnumIndex)

		self.Show()
		self.SetFocus()
		self.SetTop()

	def __LoadWindow(self):
		global RENDER_WINDOW_SIZE
		self.SetSize(7 + RENDER_WINDOW_SIZE[0] + 7, RENDER_WINDOW_SIZE[1] + 30 + 5)
		self.SetCenterPosition()
		self.SetCloseEvent(self.Close)

		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		self.SetTitleName("Render Target")

		renderWindow = MultiFunctionalRender(self, 7, 30, RENDER_WINDOW_SIZE[0], RENDER_WINDOW_SIZE[1])
		renderWindow.Show()
		self.children["renderWindow"] = renderWindow

class MultiFunctionalRender(ui.RenderTarget):
	def Close(self):
		renderTarget.SetVisibility(self.GetRenderIndex(), FALSE)
		return TRUE

	def __del__(self):
		ui.RenderTarget.__del__(self)

	def GetRenderIndex(self):
		if self.children.has_key("renderIndex"):
			return self.children["renderIndex"]
		return -1

	def CanCheckMouse(self):
		if renderTarget.IsShow(self.GetRenderIndex()) != 1:
			return FALSE

		if self.children.has_key("isDrag"):
			return self.children["isDrag"]
		return FALSE

	def Destroy(self):
		self.children = {}

	def __init__(self, parent, x, y, width, height):
		ui.RenderTarget.__init__(self)
		self.Destroy()
		self.SetParent(parent)
		self.__LoadWindow(x, y, width, height)

	def __LoadWindow(self, x, y, width, height):
		renderIndex = renderTarget.GetFreeIndex(50, 85)
		self.SetSize(width, height)
		self.SetPosition(x, y)
		self.SetRenderTarget(renderIndex)
		self.Show()

		self.SetMouseRightButtonDownEvent(ui.__mem_func__(self.RenderMouseRightDown))
		self.SetMouseRightButtonUpEvent(ui.__mem_func__(self.RenderMouseRightUp))

		renderTarget.SetBackground(renderIndex, IMG_DIR + "preview_back.tga")
		renderTarget.SetRotation(renderIndex, FALSE)
		renderTarget.SetScale(renderIndex, 0.3)

		self.children["renderIndex"] = renderIndex

		raceCount = 4

		raceList = []
		for j in xrange(raceCount):
			playerRace = ui.RadioButton()
			playerRace.SetParent(self)
			playerRace.SetUpVisual("%sface/race_%d_0.tga" % (IMG_DIR, j))
			playerRace.SetOverVisual("%sface/race_%d_1.tga" % (IMG_DIR, j))
			playerRace.SetDownVisual("%sface/race_%d_1.tga" % (IMG_DIR, j))
			playerRace.SetDisableVisual("%sface/race_%d_2.tga" % (IMG_DIR, j))
			x = ((width/2)-100)+j*(playerRace.GetWidth() + 5)
			playerRace.SetPosition(x, height-playerRace.GetHeight() - 5)
			playerRace.SAFE_SetEvent(self.SetRaceIndex, j)
			playerRace.Show()
			raceList.append(playerRace)
		self.children["raceList"] = raceList

		genderList = []
		for j in xrange(raceCount):
			genderType = ui.RadioButton()
			genderType.SetParent(self)
			genderType.SetUpVisual("%sgender/%d_0.tga" % (IMG_DIR, j))
			genderType.SetOverVisual("%sgender/%d_1.tga" % (IMG_DIR, j))
			genderType.SetDownVisual("%sgender/%d_1.tga" % (IMG_DIR, j))
			genderType.SetPosition(((width/2)-130) if j == 0 else (((width/2)-100)+4*(42+5)), height-genderType.GetHeight() - 30)
			genderType.SAFE_SetEvent(self.SetGenderIndex, j)
			genderType.Show()
			genderList.append(genderType)
		self.children["genderList"] = genderList

		self.children["blockRace"]=[]

		self.SetGenderIndex(0, FALSE)
		self.SetRaceIndex(0, FALSE)

		moveText = ui.TextLine()
		moveText.SetParent(self)
		moveText.SetHorizontalAlignRight()
		moveText.SetPosition(self.GetWidth()-10, self.GetHeight()-75)
		moveText.SetText(localeInfo.EMOJI_RENDER_TARGET_MOVE)
		moveText.Show()
		self.children["moveText"] = moveText

		zoomText = ui.TextLine()
		zoomText.SetParent(self)
		zoomText.SetHorizontalAlignRight()
		zoomText.SetPosition(self.GetWidth()-10, self.GetHeight()-50)
		zoomText.SetText(localeInfo.EMOJI_RENDER_TARGET_WHEEL)
		zoomText.Show()
		self.children["zoomText"] = zoomText

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			btn = buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()
		btn.Down()

	def SetGenderIndex(self, genderIndex, isFromButton = TRUE):
		self.__ClickRadioButton(self.children["genderList"], genderIndex)
		self.children["genderIndex"] = genderIndex

		if isFromButton:
			self.RefreshButtons()

	def SetRaceIndex(self, raceIndex, isFromButton = TRUE):
		if raceIndex >= 4:
			raceIndex -= 4

		for j in xrange(len(self.children["raceList"])):
			if self.GetRealRace(self.children["genderIndex"])[j] in self.children["blockRace"]:
				continue

			if j == raceIndex:
				self.children["raceList"][j].Down()
			else:
				self.children["raceList"][j].SetUp()

		self.children["raceIndex"] = raceIndex

		if isFromButton:
			self.RefreshButtons()

	def RenderMouseRightUp(self):
		app.SetCursor(app.NORMAL)
		self.children["isDrag"] = FALSE
		return TRUE

	def RenderMouseRightDown(self):
		app.SetCursor(app.CAMERA_ROTATE)
		self.children["isDrag"] = TRUE
		self.children["lastPos"] = app.GetCursorPosition()
		return TRUE

	def OnRunMouseWheel(self, nLen):
		renderIndex = self.GetRenderIndex()
		if renderTarget.IsShow(renderIndex) != 1:
			return FALSE

		renderTarget.Zoom(renderIndex, app.CAMERA_TO_NEGATIVE if nLen > 0 else app.CAMERA_TO_POSITIVE)
		return TRUE

	def GetOtherSexRace(self, race):
		otherSexMapping = {
			playersettingmodule.RACE_WARRIOR_W : playersettingmodule.RACE_WARRIOR_M,
			playersettingmodule.RACE_ASSASSIN_W : playersettingmodule.RACE_ASSASSIN_M,
			playersettingmodule.RACE_SHAMAN_W : playersettingmodule.RACE_SHAMAN_M,
			playersettingmodule.RACE_SURA_W : playersettingmodule.RACE_SURA_M,
			playersettingmodule.RACE_WARRIOR_M : playersettingmodule.RACE_WARRIOR_W,
			playersettingmodule.RACE_ASSASSIN_M : playersettingmodule.RACE_ASSASSIN_W,
			playersettingmodule.RACE_SHAMAN_M : playersettingmodule.RACE_SHAMAN_W,
			playersettingmodule.RACE_SURA_M : playersettingmodule.RACE_SURA_W,
		}
		return otherSexMapping[race]

	def GetValidRace(self, raceIndex = 0):
		can_equip = self.CanEquipItem(raceIndex)
		race = raceIndex
		sex = chr.RaceToSex(race)

		MALE = 1
		FEMALE = 0

		if can_equip == 0:
			return race
		elif can_equip == 1:
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_WEAPON:
				raceDict = {
					0 :	[ playersettingmodule.RACE_WARRIOR_W, playersettingmodule.RACE_WARRIOR_M, ],
					1 :	[ playersettingmodule.RACE_ASSASSIN_W, playersettingmodule.RACE_ASSASSIN_M ],
					2 :	[ playersettingmodule.RACE_ASSASSIN_W, playersettingmodule.RACE_ASSASSIN_M ],
					3 :	[ playersettingmodule.RACE_WARRIOR_W, playersettingmodule.RACE_WARRIOR_M, ],
					4 :	[ playersettingmodule.RACE_SHAMAN_W, playersettingmodule.RACE_SHAMAN_M ],
					5 :	[ playersettingmodule.RACE_SHAMAN_W, playersettingmodule.RACE_SHAMAN_M ],
				}

				item_type = item.GetValue(3)
				return raceDict[item_type][sex]
			else:
				raceDict = {
					0 :	[ playersettingmodule.RACE_WARRIOR_W, playersettingmodule.RACE_WARRIOR_M ],
					1 :	[ playersettingmodule.RACE_ASSASSIN_W, playersettingmodule.RACE_ASSASSIN_M ],
					2 :	[ playersettingmodule.RACE_SURA_W, playersettingmodule.RACE_SURA_M ],
					3 :	[ playersettingmodule.RACE_SHAMAN_W, playersettingmodule.RACE_SHAMAN_M ],
				}

				flags = []
				ANTI_FLAG_DICT = {
					0 : item.ITEM_ANTIFLAG_WARRIOR,
					1 : item.ITEM_ANTIFLAG_ASSASSIN,
					2 : item.ITEM_ANTIFLAG_SURA,
					3 : item.ITEM_ANTIFLAG_SHAMAN,
				}

				for i in xrange(len(ANTI_FLAG_DICT)):
					if not item.IsAntiFlag(ANTI_FLAG_DICT[i]):
						flags.append(i)

				if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
					sex = FEMALE

				if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
					sex = MALE

				return raceDict[flags[0]][sex] if len(flags) == 1 else 0
		elif can_equip == 2:
			return self.GetOtherSexRace(race)

	def CanEquipItem(self, raceIndex):
		ANTI_FLAG_DICT = {
			0 : item.ITEM_ANTIFLAG_WARRIOR,
			1 : item.ITEM_ANTIFLAG_ASSASSIN,
			2 : item.ITEM_ANTIFLAG_SURA,
			3 : item.ITEM_ANTIFLAG_SHAMAN,
		}

		(job,sex) = (chr.RaceToJob(raceIndex), chr.RaceToSex(raceIndex))
		(MALE, FEMALE) = (1, 0)

		if item.IsAntiFlag(ANTI_FLAG_DICT[job]):
			return 1
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and sex == MALE:
			return 2
		elif item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and sex == FEMALE:
			return 2
		return 0

	def GetRealRace(self, gender):
		return [0, 5, 2, 7] if gender == 0 else [4, 1, 6, 3]

	def IsAllRaceItems(self):
		(itemType, itemSubType) = (item.GetItemType(), item.GetItemSubType())

		if (item.ITEM_TYPE_COSTUME == itemType and (itemSubType == item.COSTUME_TYPE_HAIR or itemSubType == item.COSTUME_TYPE_BODY or itemSubType == item.COSTUME_TYPE_ACCE)):
			return TRUE
		elif item.ITEM_TYPE_ARMOR == itemType and itemSubType == item.ARMOR_BODY:
			return TRUE

		return FALSE

	def SetButtonShowStatus(self, status):
		map(lambda x : x.Show() if status == TRUE else x.Hide(), self.children["genderList"])
		map(lambda x : x.Show() if status == TRUE else x.Hide(), self.children["raceList"])

	def RefreshButtons(self):
		(renderType, vnumIndex) = (self.children["renderType"], self.children["vnumIndex"])
		if renderType != 0:
			self.SetButtonShowStatus(FALSE)
			self.PrepareRenderTarget(renderType, vnumIndex, TRUE)
			return
		else:
			self.SetButtonShowStatus(TRUE)
			self.children["blockRace"]=[]

		(genderIndex, raceIndex) = (self.children["genderIndex"], self.children["raceIndex"])
		if genderIndex == 1:
			raceIndex += 4

		__old_race = raceIndex
		if not IsCanShowItems(vnumIndex):
			return

		isAllRaceItems = self.IsAllRaceItems()
		for j in xrange(4):
			genderRace = self.GetRealRace(genderIndex)[j]
			status = self.CanEquipItem(genderRace)
			if status == 0 or isAllRaceItems == TRUE:
				self.children["raceList"][j].Enable()
			else:
				self.children["blockRace"].append(genderRace)
				self.children["raceList"][j].Disable()

		if raceIndex in self.children["blockRace"] or raceIndex-4 in self.children["blockRace"] or raceIndex + 4 in self.children["blockRace"]:
			raceIndex = self.GetValidRace(raceIndex)

		self.SetRaceIndex(raceIndex - 4  if raceIndex >= 4 else raceIndex, FALSE)
		self.SetGenderIndex(1 if raceIndex >= 4 else 0, FALSE)

		if raceIndex != __old_race:
			self.PrepareRenderTarget(renderType, vnumIndex, FALSE)
			return

		self.PrepareRenderTarget(renderType, vnumIndex, TRUE)

	def OnUpdate(self):
		if self.CanCheckMouse():
			[currentMousePos, lastPos] = [app.GetCursorPosition(), self.children["lastPos"]]
			totalPos = self.children["totalPos"] if self.children.has_key("totalPos") else [0, 0]

			_x = (currentMousePos[0] - lastPos[0]) + totalPos[0]
			_y = (currentMousePos[1] - lastPos[1]) + totalPos[1]

			fNewPitchVelocity = _y * 0.3
			fNewRotationVelocity = _x * 0.3
			renderTarget.RotateEyeAroundTarget(self.GetRenderIndex(), fNewPitchVelocity, fNewRotationVelocity)

			self.children["totalPos"] = [_x, _y]
			self.children["lastPos"] = currentMousePos

	def PrepareRenderTarget(self, renderType, vnumIndex, isFromRefresh = FALSE):
		self.children["renderType"] = renderType
		self.children["vnumIndex"] = vnumIndex

		if not isFromRefresh:
			self.RefreshButtons()
			return

		renderIndex = self.GetRenderIndex()

		renderTarget.SetScale(renderIndex, 1.0)
		renderTarget.ResetModel(renderIndex)

		if renderType == 0:

			item.SelectItem(vnumIndex)
			itemType = item.GetItemType()
			itemSubType = item.GetItemSubType()

			isArmor = ((item.ITEM_TYPE_ARMOR == itemType and itemSubType == item.ARMOR_BODY) or (item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_BODY))
			isWeapon = ((item.ITEM_TYPE_WEAPON == itemType and itemSubType != item.WEAPON_ARROW) or (item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_WEAPON))
			isAcce = ((item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_ACCE))
			isHair = (item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_HAIR)
			isMount = (item.ITEM_TYPE_MOUNT == itemType)
			isPet = (item.ITEM_TYPE_PET == itemType)

			if app.ENABLE_MOUNT_PET_SKIN and isMount == FALSE:
				isMount = (item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_MOUNT)

			if app.ENABLE_MOUNT_PET_SKIN and isPet == FALSE:
				isMount = (item.ITEM_TYPE_COSTUME == itemType and itemSubType == item.COSTUME_TYPE_PET)

			if isMount or isPet:
				self.PrepareRenderTarget(1, item.GetValue(0))
			else:
				renderTarget.SelectModel(renderIndex, self.GetRealRace(self.children["genderIndex"])[self.children["raceIndex"]])
				renderTarget.SetVisibility(renderIndex, TRUE)
				renderTarget.SetWeapon(renderIndex, vnumIndex if isWeapon else 0)
				renderTarget.SetHair(renderIndex, vnumIndex if isHair else player.GetMainCharacterPart(4), FALSE if isHair else TRUE)
				renderTarget.SetAcce(renderIndex, vnumIndex - 85000 if isAcce else player.GetMainCharacterPart(5))
				renderTarget.SetArmor(renderIndex, vnumIndex if isArmor else player.GetMainCharacterPart(0))

		elif renderType == 1:
			renderTarget.SelectModel(renderIndex, vnumIndex)
			renderTarget.SetVisibility(renderIndex, TRUE)

class SearchSlotBoard(ui.Window):
	CORNER_WIDTH = 7
	CORNER_HEIGHT = 7

	LINE_WIDTH = 7
	LINE_HEIGHT = 7

	LT = 0
	LB = 1
	RT = 2
	RB = 3

	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self):
		ui.Window.__init__(self)
		self.MakeBoard()
		self.MakeBase()

	def MakeBoard(self):
		cornerPath = IMG_DIR + "board/corner_"
		linePath = IMG_DIR + "board/"
		CornerFileNames = [ cornerPath + dir + ".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
		LineFileNames = [ linePath + dir + ".tga" for dir in ("left", "right", "top", "bottom") ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ui.ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ui.ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def MakeBase(self):
		self.Base = ui.ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage(IMG_DIR + "board/base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.Base = 0
		self.Corners = 0
		self.Lines = 0

		self.CORNER_WIDTH = 0
		self.CORNER_HEIGHT = 0
		self.LINE_WIDTH = 0
		self.LINE_HEIGHT = 0

		self.LT = 0
		self.LB = 0
		self.RT = 0
		self.RB = 0

		self.L = 0
		self.R = 0
		self.T = 0
		self.B = 0

	def SetSize(self, width, height):
		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)

		ui.Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)

		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH

		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)
