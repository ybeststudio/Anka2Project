if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiScriptLocale
import uiCommon
import wndMgr
import miniMap
import localeInfo
import colorInfo
import constInfo
import background
import time
import interfacemodule

if app.ENABLE_BIOLOGIST_SYSTEM:
	import uiBiyolog

if app.ENABLE_ULTIMATE_REGEN:
	def FormatTime(seconds):
		if seconds <= 0:
			return "0s"
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		d, h = divmod(h, 24)
		text = ""
		if d > 0:
			text+= str(d)+"d "
		if h > 0:
			text+= str(h)+"h "
		if m > 0:
			text+= str(m)+"m "
		if s > 0:
			text+= str(s)+"s "
		return text

class MapTextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.SetHorizontalAlignRight()
		textLine.Show()
		self.textLine = textLine

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetTooltipPosition(self, PosX, PosY):
		self.textLine.SetPosition(PosX - 5, PosY)

	def SetTextColor(self, TextColor):
		self.textLine.SetPackedFontColor(TextColor)

	def GetTextSize(self):
		return self.textLine.GetTextSize()

class AtlasWindow(ui.ScriptWindow):

	class AtlasRenderer(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")

		def __del__(self):
			ui.Window.__del__(self)

		def OnUpdate(self):
			miniMap.UpdateAtlas()

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			fx = float(x)
			fy = float(y)
			miniMap.RenderAtlas(fx, fy)

		def HideAtlas(self):
			miniMap.HideAtlas()

		def ShowAtlas(self):
			miniMap.ShowAtlas()

	def __init__(self):
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Hide()

		if app.ENABLE_ULTIMATE_REGEN:
			self.tooltipInfoEx = MapTextToolTip()
			self.tooltipInfoEx.Hide()

		self.infoGuildMark = ui.MarkBox()
		self.infoGuildMark.Hide()
		self.AtlasMainWindow = None
		self.mapName = ""
		self.board = 0

		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetMapName(self, mapName):
		if 949 == app.GetDefaultCodePage():
			try:
				self.board.SetTitleName(localeInfo.MINIMAP_ZONE_NAME_DICT[mapName])
			except:
				pass

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/AtlasWindow.py")
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.LoadScript")

		try:
			self.board = self.GetChild("board")

		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.BindObject")

		self.AtlasMainWindow = self.AtlasRenderer()
		self.board.SetCloseEvent(self.Hide)
		self.AtlasMainWindow.SetParent(self.board)
		self.AtlasMainWindow.SetPosition(7, 30)
		self.tooltipInfo.SetParent(self.board)
		if app.ENABLE_ULTIMATE_REGEN:
			self.tooltipInfoEx.SetParent(self.board)
		self.infoGuildMark.SetParent(self.board)
		self.SetPosition(wndMgr.GetScreenWidth() - 136 - 256 - 10 - 165, 78)
		if app.ENABLE_MAP_TELEPORT:
			self.board.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.OnMouseLeftButtonUpEvent))
		self.Hide()

		miniMap.RegisterAtlasWindow(self)

	def Destroy(self):
		miniMap.UnregisterAtlasWindow()
		self.ClearDictionary()
		self.AtlasMainWindow = None
		self.tooltipAtlasClose = 0
		self.tooltipInfo = None
		if app.ENABLE_ULTIMATE_REGEN:
			self.tooltipInfoEx = None
		self.infoGuildMark = None
		self.board = None

	def OnUpdate(self):
		if not self.tooltipInfo:
			return

		if not self.infoGuildMark:
			return

		self.infoGuildMark.Hide()
		self.tooltipInfo.Hide()
		if app.ENABLE_ULTIMATE_REGEN:
			self.tooltipInfoEx.Hide()

		if FALSE == self.board.IsIn():
			return

		(mouseX, mouseY) = wndMgr.GetMousePosition()
		if app.ENABLE_ULTIMATE_REGEN:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, diRegenTime) = miniMap.GetAtlasInfo(mouseX, mouseY)
		else:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)

		if FALSE == bFind:
			return

		(x, y) = self.GetGlobalPosition()

		if "empty_guild_area" == sName:
			sName = localeInfo.GUILD_EMPTY_AREA

		if app.ENABLE_ULTIMATE_REGEN:
			if diRegenTime != 0:
				self.tooltipInfo.SetText(localeInfo.MINIMAP_BOSS_RESPAWN_TIME % FormatTime(diRegenTime))

				self.tooltipInfoEx.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
				self.tooltipInfoEx.SetTooltipPosition(mouseX - x, mouseY - y + 12)
				self.tooltipInfoEx.SetTextColor(dwTextColor)
				self.tooltipInfoEx.Show()
				self.tooltipInfoEx.SetTop()
			else:
				self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
				self.tooltipInfo.SetTooltipPosition(mouseX - x, mouseY - y)

			self.tooltipInfo.SetTextColor(dwTextColor)
			self.tooltipInfo.Show()
			self.tooltipInfo.SetTop()
		else:
			self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
			self.tooltipInfo.SetTooltipPosition(mouseX - x, mouseY - y)
			self.tooltipInfo.SetTextColor(dwTextColor)
			self.tooltipInfo.Show()
			self.tooltipInfo.SetTop()

		if 0 != dwGuildID:
			textWidth, textHeight = self.tooltipInfo.GetTextSize()
			self.infoGuildMark.SetIndex(dwGuildID)
			self.infoGuildMark.SetPosition(mouseX - x - textWidth - 18 - 5, mouseY - y)
			self.infoGuildMark.Show()

	def Hide(self):
		if self.AtlasMainWindow:
			self.AtlasMainWindow.HideAtlas()
			self.AtlasMainWindow.Hide()
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.AtlasMainWindow:
			(bGet, iSizeX, iSizeY) = miniMap.GetAtlasSize()
			if bGet:
				self.SetSize(iSizeX + 15, iSizeY + 38)
				self.board.SetSize(iSizeX + 15, iSizeY + 38)
				self.AtlasMainWindow.ShowAtlas()
				self.AtlasMainWindow.Show()
		ui.ScriptWindow.Show(self)

	def SetCenterPositionAdjust(self, x, y):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	if app.ENABLE_MAP_TELEPORT:
		def OnMouseLeftButtonUpEvent(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			if app.ENABLE_ULTIMATE_REGEN:
				(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, diRegenTime) = miniMap.GetAtlasInfo(mouseX, mouseY)
			else:
				(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)

			if chr.IsGameMaster(player.GetMainCharacterIndex()):
				net.SendChatPacket("/go %s %s" % (str(iPosX), str(iPosY)))

	def OnPressEscapeKey(self):
		self.Hide()
		return TRUE

if app.ENABLE_SUNG_MAHI_TOWER:
	class MovableText(ui.TextLine):
		def __init__(self):
			ui.TextLine.__init__(self)

			try:
				self.__Initialize()
				self.__LoadObjects()

			except:
				import exception
				exception.Abort("MovableText.__init__")

		def __del__(self):
			ui.TextLine.__del__(self)

		def __Initialize(self):
			self.posX = 0
			self.posY = 0

			self.defaultText = ""
			self.default_x_pos = 0
			self.default_y_pos = 0

			self.updateContor = 0
			self.defaultTextSize = 0

			self.hideParameter = 0

		def __LoadObjects(self):
			self.SetPosition(115, 57)
			self.SetText("")
			self.Show()

			self.defaultTextSize = self.GetTextSize()[0]

		def SetPos(self, x, y, hide_parameter):
			self.SetPosition(x, y)

			self.posX = x
			self.posY = y

			self.default_x_pos = x
			self.default_y_pos = y

			self.hideParameter = hide_parameter

		def SetTextInfo(self, text):
			self.SetText(text)
			self.defaultText = text
			self.defaultTextSize = self.GetTextSize()[0]

		def GetTextX(self):
			return self.posX

		def GetTextY(self):
			return self.posY

		def SetTextPos(self, value):
			self.posX = value

		def GetDefaultText(self):
			return self.defaultText

		def GetLevelDescriptionUpdate(self):
			return self.updateContor

		def SeUpdateContor(self, value):
			self.updateContor = value

		def GetDefaultTextSize(self):
			return self.defaultTextSize

		def UpdateRender(self):
			defaultText = self.GetDefaultText()

			if not self.IsShow():
				self.Show()

			if self.GetTextX() < (self.hideParameter - self.GetDefaultTextSize()):
				self.SetTextPos(self.default_x_pos)
				self.SeUpdateContor(0)

				self.SetText(self.GetDefaultText())
				self.Hide()

			if self.GetTextX() < self.hideParameter:
				if self.GetTextX() % 5 == 0:
					textSize = len(defaultText)
					self.SeUpdateContor(self.GetLevelDescriptionUpdate() + 1)

					self.SetTextPos(self.GetTextX() + (self.GetCharSize() - 1))
					self.SetText(defaultText[self.GetLevelDescriptionUpdate():textSize])

			self.SetPosition((self.GetTextX() - 1), self.GetTextY())
			self.SetTextPos(self.GetTextX() - 1)

	class SungMahiCover(ui.ImageBox):
		def __init__(self):
			ui.ImageBox.__init__(self)
			
			try:
				self.__Initialize()
				self.__LoadObjects()
			
			except:
				import exception
				exception.Abort("SungMahiCover.__init__")

		def __del__(self):
			ui.ImageBox.__del__(self)

		def __Initialize(self):
			self.levelDescription = None
			self.levelCurse = None

			self.minimapBG = None
			self.roomLevel = []

			self.levelInfo = None
			self.timeGauge = None
			self.timeGaugeText = None

			self.exitDlgBox = None
			self.dlgBoxText = None
			self.dlgBoxAcceptButton = None
			self.exitButton = None

			self.levelCurseBG = None
			self.levelCurseIcon = None

			self.textUpdateTime = 0
			self.gaugeUpTime = 0
			self.gaugeGlobalTime = 0

		def __LoadObjects(self):
			self.LoadImage("d:/ymir work/ui/game/sungmahee_tower/information_bg.sub")

			self.levelDescription = MovableText()
			self.levelDescription.SetParent(self)
			self.levelDescription.Hide()

			self.minimapBG = ui.ImageBox()
			self.minimapBG.SetParent(self)
			self.minimapBG.LoadImage("d:/ymir work/ui/game/sungmahee_tower/smhtower_bg.png")
			self.minimapBG.Show()

			roomLevelPos = [[17, 18], [47, 12], [88, 18]]
			for index in xrange(3):
				roomLevel = ui.ImageBox()
				roomLevel.SetParent(self)
				roomLevel.LoadImage("d:/ymir work/ui/game/sungmahee_tower/{}stage_clear_icon.sub".format(index + 1))
				roomLevel.SetPosition(roomLevelPos[index][0], roomLevelPos[index][1])
				roomLevel.Hide()

				self.roomLevel.append(roomLevel)

			self.levelInfo = ui.TextLine()
			self.levelInfo.SetParent(self)
			self.levelInfo.SetPosition(68, 36)
			self.levelInfo.SetHorizontalAlignCenter()
			self.levelInfo.SetPackedFontColor(0xffffaf00)
			self.levelInfo.SetText("L50")
			self.levelInfo.Show()

			self.timeGauge = ui.ExpandedImageBox()
			self.timeGauge.SetParent(self)
			self.timeGauge.SetPosition(16, 78)
			self.timeGauge.LoadImage("d:/ymir work/ui/game/sungmahee_tower/information_gauge.sub")
			self.timeGauge.SetPercentage(0, 0)
			self.timeGauge.Show()

			self.timeGaugeText = ui.TextLine()
			self.timeGaugeText.SetParent(self.timeGauge)
			self.timeGaugeText.SetPosition(53, 0)
			self.timeGaugeText.SetHorizontalAlignCenter()
			self.timeGaugeText.Show()

			self.exitDlgBox = ui.Board()
			self.exitDlgBox.SetSize(300, 100)
			self.exitDlgBox.SetPosition((wndMgr.GetScreenWidth() / 2) - 150, (wndMgr.GetScreenHeight() / 2) - 100)
			# self.exitDlgBox.SetCloseEvent(self.DenyExit)

			self.dlgBoxText = ui.TextLine()
			self.dlgBoxText.SetParent(self.exitDlgBox)
			self.dlgBoxText.SetPosition(150, 30)
			self.dlgBoxText.SetHorizontalAlignCenter()
			self.dlgBoxText.SetText(uiScriptLocale.SUNGMAHEE_TOWER_DUNGEON_EXIT_QUESTION)
			self.dlgBoxText.Show()

			self.dlgBoxDenyButton = ui.Button()
			self.dlgBoxDenyButton.SetParent(self.exitDlgBox)
			self.dlgBoxDenyButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
			self.dlgBoxDenyButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
			self.dlgBoxDenyButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
			self.dlgBoxDenyButton.SetPosition(155, 55)
			self.dlgBoxDenyButton.SetEvent(ui.__mem_func__(self.DenyExit))
			self.dlgBoxDenyButton.SetText(uiScriptLocale.NO)
			self.dlgBoxDenyButton.Show()

			self.dlgBoxAcceptButton = ui.Button()
			self.dlgBoxAcceptButton.SetParent(self.exitDlgBox)
			self.dlgBoxAcceptButton.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
			self.dlgBoxAcceptButton.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
			self.dlgBoxAcceptButton.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
			self.dlgBoxAcceptButton.SetPosition(75, 55)
			self.dlgBoxAcceptButton.SetEvent(ui.__mem_func__(self.AcceptExit))
			self.dlgBoxAcceptButton.SetText(uiScriptLocale.YES)
			self.dlgBoxAcceptButton.Show()

			self.exitDlgBox.Hide()

			self.exitButton = ui.Button()
			self.exitButton.SetParent(self)
			self.exitButton.SetUpVisual("d:/ymir work/ui/game/sungmahee_tower/exit_button_default.sub")
			self.exitButton.SetOverVisual("d:/ymir work/ui/game/sungmahee_tower/exit_button_over.sub")
			self.exitButton.SetDownVisual("d:/ymir work/ui/game/sungmahee_tower/exit_button_down.sub")
			self.exitButton.SetPosition(54, 98)
			self.exitButton.SetEvent(ui.__mem_func__(self.ShowExitDialog))
			self.exitButton.Show()

			self.levelCurseBG = ui.ImageBox()
			self.levelCurseBG.SetParent(self)
			self.levelCurseBG.LoadImage("d:/ymir work/ui/game/sungmahee_tower/buff_debuff.sub")
			self.levelCurseBG.SetPosition(15, 140)
			self.levelCurseBG.Hide()

			self.levelCurse = MovableText()
			self.levelCurse.SetParent(self)
			self.levelCurse.Hide()

			self.levelCurseIcon = ui.ExpandedImageBox()
			self.levelCurseIcon.SetParent(self)
			self.levelCurseIcon.LoadImage("d:/ymir work/ui/skill/common/affect/sungmahee_tower_debuff.sub")
			self.levelCurseIcon.SetScale(0.7, 0.7)
			self.levelCurseIcon.SetPosition(-7, 141)
			self.levelCurseIcon.Hide()

			self.Show()

		def Destroy(self):
			self.__Initialize()

		def SetLevelInfo(self, level):
			self.levelInfo.SetText(uiScriptLocale.SUNGMAHEE_TOWER_INFO_BOARD_FLOOR % int(level))

		def UpdateRoomTime(self, roomTime):
			roomTime = int(roomTime)

			self.timeGauge.SetPercentage(roomTime, roomTime)
			self.timeGaugeText.Show()

			self.gaugeUpTime = roomTime
			self.gaugeGlobalTime = app.GetGlobalTimeStamp()

		def SetRoomLevel(self, level):
			for roomIndex in self.roomLevel:
				roomIndex.Hide()

			for index in xrange(int(level)):
				self.roomLevel[index].Show()

			#since it's completing a stage, reset the gauge
			self.timeGauge.SetPercentage(0, 0)
			self.timeGaugeText.Hide()
			self.gaugeUpTime = 0

		def SetLevelDescriptionText(self, noticeText):
			self.levelDescription.SetPos(115, 57, 17)
			self.levelDescription.SetTextInfo(noticeText)
			self.levelDescription.Show()

		def SetLevelCurseText(self, noticeText, noticeType = 2):
			self.levelCurse.SetPos(134, 144, 5)
			self.levelCurse.SetTextInfo(noticeText)

			self.levelCurse.Show()
			self.levelCurseBG.Show()

			iconPath = (noticeType > 2 and "d:/ymir work/ui/skill/common/affect/sungmahee_tower_debuff.sub" or "d:/ymir work/ui/skill/common/affect/sungmahee_tower_buff.sub")
			self.levelCurseIcon.LoadImage(iconPath)
			self.levelCurseIcon.SetScale(0.7, 0.7)

			self.levelCurseIcon.Show()

		def SungMahiClearNotice(self, noticeType):
			if noticeType == 1:
				self.SetLevelDescriptionText("")
			else:
				self.SetLevelCurseText("")

				self.levelCurse.Hide()
				self.levelCurseBG.Hide()
				self.levelCurseIcon.Hide()

		def AcceptExit(self):
			net.SendChatPacket("/restart_here")
			self.DenyExit()

		def DenyExit(self):
			if self.exitDlgBox.IsShow():
				self.exitDlgBox.Hide()

		def ShowExitDialog(self):
			self.exitDlgBox.Show()

		def Update(self):
			curTime = app.GetTime()
			if (self.textUpdateTime - curTime) <= 1:
				self.textUpdateTime = (curTime + 1.01)

				actualTime = (app.GetGlobalTimeStamp() - self.gaugeGlobalTime)
				if actualTime <= self.gaugeUpTime:
					self.timeGauge.SetPercentage((self.gaugeUpTime - actualTime), self.gaugeUpTime)
					endTime = self.gaugeUpTime - actualTime

					self.timeGaugeText.SetText("{}".format(localeInfo.SecondToM(endTime)))
					if endTime < 60:
						self.timeGaugeText.SetText("< {}".format(localeInfo.SecondToM(endTime)))

				self.levelDescription.UpdateRender()
				self.levelCurse.UpdateRender()


def __RegisterMiniMapColor(type, rgb):
	miniMap.RegisterColor(type, rgb[0], rgb[1], rgb[2])

class MiniMap(ui.ScriptWindow):
	CANNOT_SEE_INFO_MAP_DICT = {
		"metin2_map_monkeydungeon" : FALSE,
		"metin2_map_monkeydungeon_02" : FALSE,
		"metin2_map_monkeydungeon_03" : FALSE,
		"metin2_map_devilsCatacomb" : FALSE,
		"metin2_map_maze_dungeon1" : FALSE,
		"metin2_map_maze_dungeon2" : FALSE,
		"metin2_map_maze_dungeon3" : FALSE,
	}

	MAPS_GLOBAL = {
		"metin2_map_a1",
		"metin2_map_c1",
		"metin2_map_b1",
		"metin2_map_battleroyale"
	}
	#if app.ENABLE_ZODIAC_MISSION:
	#	CANNOT_SEE_INFO_MAP_DICT.update({
	#		"metin2_12zi_stage" : FALSE,
	#	})

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__Initialize()

		miniMap.Create()
		miniMap.SetScale(2.0)

		self.AtlasWindow = AtlasWindow()
		self.AtlasWindow.LoadWindow()
		self.AtlasWindow.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			self.sungMahiCover = SungMahiCover()
			self.sungMahiCover.SetParent(self)
			self.sungMahiCover.SetPosition(3, 0)
			self.sungMahiCover.Hide()

		self.interface = None

		self.tooltipMiniMapOpen = MapTextToolTip()
		self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)
		self.tooltipMiniMapOpen.Show()
		self.tooltipMiniMapClose = MapTextToolTip()
		self.tooltipMiniMapClose.SetText(localeInfo.UI_CLOSE)
		self.tooltipMiniMapClose.Show()
		self.tooltipScaleUp = MapTextToolTip()
		self.tooltipScaleUp.SetText(localeInfo.MINIMAP_INC_SCALE)
		self.tooltipScaleUp.Show()
		self.tooltipScaleDown = MapTextToolTip()
		self.tooltipScaleDown.SetText(localeInfo.MINIMAP_DEC_SCALE)
		self.tooltipScaleDown.Show()
		self.tooltipAtlasOpen = MapTextToolTip()
		self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		self.tooltipAtlasOpen.Show()

		self.tooltipTeleport = MapTextToolTip()
		self.tooltipTeleport.SetText(getattr(localeInfo, "TELEPORT_WINDOW_TITLE", "I\xfe\xfdnlanma"))
		self.tooltipWarpLink = MapTextToolTip()
		self.tooltipWarpLink.SetText(getattr(localeInfo, "TOOLTIP_SHARE_POSITION", "Konum Payla\xfe"))
		self.tooltipBattlePass = MapTextToolTip()
		self.tooltipBattlePass.SetText(getattr(localeInfo, "BATTLEPASS_TITLE", "Sava\xfe Bileti"))

		self.tooltipDungeonInfo = MapTextToolTip()
		self.tooltipDungeonInfo.SetText(localeInfo.SYSTEM_DUNGEON_BUTTON)
		#self.tooltipDungeonInfo.Show()

		self.tooltiprank = MapTextToolTip()
		self.tooltiprank.SetText(localeInfo.RANK_TITLE)

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.tooltipBiologist = MapTextToolTip()
			#self.tooltipBiologist.SetText("Biyolog")
			self.tooltipBiologist.SetText(localeInfo.BIO_TITLE)

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			self.tooltipOfflineShopSearch = MapTextToolTip()
			#self.tooltipOfflineShopSearch.SetText("Dükkan Ara")
			self.tooltipOfflineShopSearch.SetText(localeInfo.SEARCH_TITLE)

		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		if miniMap.IsAtlas():
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		else:
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_CAN_NOT_SHOW_AREAMAP)

		self.mapName = ""

		self.isLoaded = 0
		self.canSeeInfo = TRUE

		# AUTOBAN
		self.imprisonmentDuration = 0
		self.imprisonmentEndTime = 0
		self.imprisonmentEndTimeText = ""
		# END_OF_AUTOBAN

	def __del__(self):
		miniMap.Destroy()
		ui.ScriptWindow.__del__(self)

	def BindInterface(self, interface):
		self.interface = interface

	def __Initialize(self):
		self.positionInfo = 0
		self.observerCount = 0

		self.OpenWindow = 0
		self.CloseWindow = 0
		self.ScaleUpButton = 0
		self.ScaleDownButton = 0
		self.MiniMapHideButton = 0
		self.MiniMapShowButton = 0
		self.AtlasShowButton = 0

		self.tooltipMiniMapOpen = 0
		self.tooltipMiniMapClose = 0
		self.tooltipScaleUp = 0
		self.tooltipScaleDown = 0
		self.tooltipAtlasOpen = 0
		self.tooltipInfo = None
		self.serverInfo = None
		self.interface = None

		self.FPSInfo = 0
		self.dateInfo = None
		self.timeInfo = None
		self.teleportButton = None
		self.warpLinkButton = None
		self.tooltipTeleport = None
		self.tooltipWarpLink = None
		self.tooltipBattlePass = None
		self._lastClockSecond = -1

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.BiologistButton = 0
			self.tooltipBiologist = 0

		self.tooltipDungeonInfo = 0
		self.tooltiprank = 0
		self.btnRank = 0

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			self.OfflineShopSearchButton = 0
			self.tooltipOfflineShopSearch = 0

		self.battlepassButton = 0
		self.wndBattlePassMenu = None

		if app.ENABLE_SUNG_MAHI_TOWER:
			self.sungMahiCover = None

	def SetMapName(self, mapName):
		self.mapName = mapName
		self.AtlasWindow.SetMapName(mapName)

		if self.CANNOT_SEE_INFO_MAP_DICT.has_key(mapName):
			self.canSeeInfo = FALSE
			self.HideMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP_CANNOT_SEE)
		else:
			self.canSeeInfo = TRUE
			self.ShowMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)

	# AUTOBAN
	def SetImprisonmentDuration(self, duration):
		self.imprisonmentDuration = duration
		self.imprisonmentEndTime = app.GetGlobalTimeStamp() + duration

		self.__UpdateImprisonmentDurationText()

	def __UpdateImprisonmentDurationText(self):
		restTime = max(self.imprisonmentEndTime - app.GetGlobalTimeStamp(), 0)

		imprisonmentEndTimeText = localeInfo.SecondToDHM(restTime)
		if imprisonmentEndTimeText != self.imprisonmentEndTimeText:
			self.imprisonmentEndTimeText = imprisonmentEndTimeText
			self.serverInfo.SetText("%s: %s" % (uiScriptLocale.AUTOBAN_QUIZ_REST_TIME, self.imprisonmentEndTimeText))
	# END_OF_AUTOBAN

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/MiniMap.py")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.LoadScript")

		try:
			self.OpenWindow = self.GetChild("OpenWindow")
			self.MiniMapWindow = self.GetChild("MiniMapWindow")
			self.ScaleUpButton = self.GetChild("ScaleUpButton")
			self.ScaleDownButton = self.GetChild("ScaleDownButton")
			self.MiniMapHideButton = self.GetChild("MiniMapHideButton")
			self.AtlasShowButton = self.GetChild("AtlasShowButton")
			self.CloseWindow = self.GetChild("CloseWindow")
			self.MiniMapShowButton = self.GetChild("MiniMapShowButton")
			self.positionInfo = self.GetChild("PositionInfo")
			self.observerCount = self.GetChild("ObserverCount")
			self.serverInfo = self.GetChild("ServerInfo")
			#if app.ENABLE_ZODIAC_MISSION:
			#	self.bead = self.GetChild("bead")
			#	self.beadInfo = self.GetChild("beadInfo")

			if app.ENABLE_BIOLOGIST_SYSTEM:
				self.BiologistButton = self.GetChild("BiologistButton")

			if app.__DUNGEON_INFO__:
				self.btnDungeonSystem = self.GetChild("DungeonSystemButton")

			if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
				self.OfflineShopSearchButton = self.GetChild("PortableSearchButton")

			self.btnRank = self.GetChild("OpenRewardWindow")

			self.FPSInfo = self.GetChild("FPSInfo")
			self.dateInfo = self.GetChild("DateInfo")
			self.timeInfo = self.GetChild("TimeInfo")
			self.teleportButton = self.GetChild("TeleportButton")
			self.warpLinkButton = self.GetChild("WarpLinkButton")

		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.Bind")

		try:
			self.battlepassButton = self.GetChild("battlepass")
		except (KeyError, TypeError):
			try:
				self.battlepassButton = self.OpenWindow.GetChild("battlepass")
			except (KeyError, TypeError):
				self.battlepassButton = None

		if constInfo.MINIMAP_POSITIONINFO_ENABLE == 0:
			self.positionInfo.Hide()

		#if app.ENABLE_ZODIAC_MISSION:
		#	self.bead.Hide()
		#	self.beadInfo.Hide()

		self.serverInfo.SetText(net.GetServerInfo())
		self.ScaleUpButton.SetEvent(ui.__mem_func__(self.ScaleUp))
		self.ScaleDownButton.SetEvent(ui.__mem_func__(self.ScaleDown))
		self.MiniMapHideButton.SetEvent(ui.__mem_func__(self.HideMiniMap))
		self.MiniMapShowButton.SetEvent(ui.__mem_func__(self.ShowMiniMap))

		if app.ENABLE_BIOLOGIST_SYSTEM:
			self.BiologistButton.SetEvent(ui.__mem_func__(self.OpenBiyologDialog))

		if app.__DUNGEON_INFO__:
			self.btnDungeonSystem.SetEvent(ui.__mem_func__(self.OpenTableDungeonInfo))

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			self.OfflineShopSearchButton.SetEvent(ui.__mem_func__(self.OfflineShopSearch))

		self.btnRank.SetEvent(ui.__mem_func__(self.OpenRanking))
		self.teleportButton.SetEvent(ui.__mem_func__(self.OpenTeleportWindow))

		if self.battlepassButton:
			self.battlepassButton.SetEvent(ui.__mem_func__(self.OpenBattlePassMenu))
			self.__CreateBattlePassMenu()

		if miniMap.IsAtlas():
			self.AtlasShowButton.SetEvent(ui.__mem_func__(self.ShowAtlas))

		(ButtonPosX, ButtonPosY) = self.MiniMapShowButton.GetGlobalPosition()
		self.tooltipMiniMapOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.MiniMapHideButton.GetGlobalPosition()
		self.tooltipMiniMapClose.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.ScaleUpButton.GetGlobalPosition()
		self.tooltipScaleUp.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.ScaleDownButton.GetGlobalPosition()
		self.tooltipScaleDown.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.AtlasShowButton.GetGlobalPosition()
		self.tooltipAtlasOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.teleportButton.GetGlobalPosition()
		self.tooltipTeleport.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.warpLinkButton.GetGlobalPosition()
		self.tooltipWarpLink.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if self.battlepassButton:
			(ButtonPosX, ButtonPosY) = self.battlepassButton.GetGlobalPosition()
			self.tooltipBattlePass.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if app.__DUNGEON_INFO__:
			if self.btnDungeonSystem:
				(ButtonPosX, ButtonPosY) = self.btnDungeonSystem.GetGlobalPosition()
				self.tooltipDungeonInfo.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.btnRank.GetGlobalPosition()
		self.tooltiprank.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.BiologistButton:
				(ButtonPosX, ButtonPosY) = self.BiologistButton.GetGlobalPosition()
				self.tooltipBiologist.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			if self.OfflineShopSearchButton:
				(ButtonPosX, ButtonPosY) = self.OfflineShopSearchButton.GetGlobalPosition()
				self.tooltipOfflineShopSearch.SetTooltipPosition(ButtonPosX, ButtonPosY)

		self.ShowMiniMap()

		self.GetMapsGlobal()

	if app.ENABLE_RANKING:
		def OpenRanking(self):
			if self.interface:
				self.interface.OpenRanking()

		def RankingClearData(self):
			if self.interface:
				self.interface.RankingClearData()

		def RankingAddRank(self, position, level, points, name, realPosition):
			if self.interface:
				self.interface.RankingAddRank(position, level, points, name, realPosition)

		def RankingRefresh(self):
			if self.interface:
				self.interface.RankingRefresh()

	def __CreateBattlePassMenu(self):
		if self.wndBattlePassMenu:
			return
		self.wndBattlePassMenu = uiCommon.QuestionDialog()
		self.wndBattlePassMenu.SetText("Savaþ Bileti Seçiniz")
		self.wndBattlePassMenu.SetAcceptText("Ücretsiz")
		self.wndBattlePassMenu.SetCancelText("Premium")
		self.wndBattlePassMenu.SetAcceptEvent(ui.__mem_func__(self._OnBattlePassAccept))
		self.wndBattlePassMenu.SetCancelEvent(ui.__mem_func__(self._OnBattlePassCancel))

	def _OnBattlePassAccept(self):
		self.wndBattlePassMenu.Close()
		net.SendChatPacket("/open_battlepass")

	def _OnBattlePassCancel(self):
		self.wndBattlePassMenu.Close()
		net.SendChatPacket("/open_battlepass_premium")

	def OpenBattlePassMenu(self):
		if not self.battlepassButton or not self.wndBattlePassMenu:
			return
		self.wndBattlePassMenu.Open()

	def BattlePassMenu(self):
		pass

	def BattlePassPremiumMenu(self):
		pass

	def OpenTeleportWindow(self):
		if self.interface and hasattr(self.interface, "OpenWarpWindow"):
			self.interface.OpenWarpWindow()

	def GetMapsGlobal(self):
		for x in self.MAPS_GLOBAL:
			if background.GetCurrentMapName() == x:
				text = net.GetServerInfo().split(",")
				self.serverInfo.SetText("{}, Ortak Kanal".format(text[0]))
				break
			else:
				self.serverInfo.SetText(net.GetServerInfo())

	if app.__DUNGEON_INFO__:
		def OpenTableDungeonInfo(self):
			try:
				if self.interface:
					if hasattr(self.interface, 'OpenDungeonInfo'):
						self.interface.OpenDungeonInfo()
					else:
						import dbg
						dbg.TraceError("OpenTableDungeonInfo: self.interface.OpenDungeonInfo not found")
				else:
					interface = constInfo.GetInterfaceInstance()
					if interface:
						if hasattr(interface, 'OpenDungeonInfo'):
							interface.OpenDungeonInfo()
						else:
							import dbg
							dbg.TraceError("OpenTableDungeonInfo: interface.OpenDungeonInfo not found")
			except Exception, e:
				import dbg
				dbg.TraceError("OpenTableDungeonInfo Error: %s" % str(e))

	def OpenWikiWindow(self):
		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			interface.OpenWikiWindow()

	def Destroy(self):
		self.HideMiniMap()
		if self.wndBattlePassMenu and self.wndBattlePassMenu.IsShow():
			self.wndBattlePassMenu.Hide()

		self.AtlasWindow.Destroy()
		self.AtlasWindow = None

		self.ClearDictionary()

		self.__Initialize()

	def UpdateObserverCount(self, observerCount):
		if observerCount > 0:
			self.observerCount.Show()
		elif observerCount <= 0:
			self.observerCount.Hide()

		self.observerCount.SetText(localeInfo.MINIMAP_OBSERVER_COUNT % observerCount)

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		miniMap.Update(x, y)

		self.positionInfo.SetText("(%.0f, %.0f)" % (x/100, y/100))

		nRenderFPS = app.GetRenderFPS()
		fps = "%3d" % (nRenderFPS)
		self.FPSInfo.SetText("FPS:%d" % nRenderFPS)

		clockSecond = int(time.time())
		if clockSecond != self._lastClockSecond:
			self._lastClockSecond = clockSecond
			stamp = time.localtime(clockSecond)
			weekdays = ("Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun")
			months = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")
			self.dateInfo.SetText("%s, %02d %s %04d" % (weekdays[stamp.tm_wday], stamp.tm_mday, months[stamp.tm_mon - 1], stamp.tm_year))
			self.timeInfo.SetText("[%02d:%02d:%02d]" % (stamp.tm_hour, stamp.tm_min, stamp.tm_sec))

		#if app.ENABLE_ZODIAC_MISSION:
		#	if self.beadInfo.IsShow():
		#		if not self.bead.IsShow():
		#			self.bead.Show()
		#	elif self.bead.IsShow():
		#		self.bead.Hide()

		if self.tooltipInfo:
			if TRUE == self.MiniMapWindow.IsIn():
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				(bFind, sName, iPosX, iPosY, dwTextColor) = miniMap.GetInfo(mouseX, mouseY)
				if bFind == 0:
					self.tooltipInfo.Hide()
				elif not self.canSeeInfo:
					self.tooltipInfo.SetText("%s(%s)" % (sName, localeInfo.UI_POS_UNKNOWN))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
				else:
					self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
			else:
				self.tooltipInfo.Hide()

			# AUTOBAN
			if self.imprisonmentDuration:
				self.__UpdateImprisonmentDurationText()
			# END_OF_AUTOBAN

		if TRUE == self.MiniMapShowButton.IsIn():
			self.tooltipMiniMapOpen.Show()
		else:
			self.tooltipMiniMapOpen.Hide()

		if TRUE == self.MiniMapHideButton.IsIn():
			self.tooltipMiniMapClose.Show()
		else:
			self.tooltipMiniMapClose.Hide()

		if TRUE == self.ScaleUpButton.IsIn():
			self.tooltipScaleUp.Show()
		else:
			self.tooltipScaleUp.Hide()

		if TRUE == self.ScaleDownButton.IsIn():
			self.tooltipScaleDown.Show()
		else:
			self.tooltipScaleDown.Hide()

		if TRUE == self.AtlasShowButton.IsIn():
			self.tooltipAtlasOpen.Show()
		else:
			self.tooltipAtlasOpen.Hide()

		if TRUE == self.teleportButton.IsIn():
			self.tooltipTeleport.Show()
		else:
			self.tooltipTeleport.Hide()

		if TRUE == self.warpLinkButton.IsIn():
			self.tooltipWarpLink.Show()
		else:
			self.tooltipWarpLink.Hide()

		if self.battlepassButton and TRUE == self.battlepassButton.IsIn():
			self.tooltipBattlePass.Show()
		else:
			self.tooltipBattlePass.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.sungMahiCover.IsShow():
				self.sungMahiCover.Update()

		if app.__DUNGEON_INFO__:
			if self.btnDungeonSystem and True == self.btnDungeonSystem.IsIn():
				self.tooltipDungeonInfo.Show()
			else:
				self.tooltipDungeonInfo.Hide()

		if True == self.btnRank.IsIn():
			self.tooltiprank.Show()
		else:
			self.tooltiprank.Hide()

		if app.ENABLE_BIOLOGIST_SYSTEM:
			if self.BiologistButton and True == self.BiologistButton.IsIn():
				self.tooltipBiologist.Show()
			else:
				self.tooltipBiologist.Hide()

		if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
			if self.OfflineShopSearchButton and True == self.OfflineShopSearchButton.IsIn():
				self.tooltipOfflineShopSearch.Show()
			else:
				self.tooltipOfflineShopSearch.Hide()

	def OnRender(self):
		(x, y) = self.GetGlobalPosition()
		fx = float(x)
		fy = float(y)
		miniMap.Render(fx + 34.0, fy + 8.0)

	def Close(self):
		self.HideMiniMap()

	def HideMiniMap(self):
		miniMap.Hide()
		self.OpenWindow.Hide()
		self.CloseWindow.Show()
		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.sungMahiCover:
				self.sungMahiCover.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.sungMahiCover:
				self.sungMahiCover.Hide()
	def ShowMiniMap(self):
		if not self.canSeeInfo:
			return

		miniMap.Show()
		self.OpenWindow.Show()
		self.CloseWindow.Hide()

		if app.ENABLE_SUNG_MAHI_TOWER:
			if self.sungMahiCover and self.mapName == "metin2_map_smhdungeon_02":
				self.HideMiniMap()
				self.CloseWindow.Hide()
				self.sungMahiCover.Show()
				self.GetChild("bio").Hide()
				self.btnDungeonSystem.Hide()
				# self.evenimente.Hide()
				self.btnSearch.Hide()
				self.btnRank.Hide()
				self.btnWiki.Hide()

	def isShowMiniMap(self):
		return miniMap.isShow()

	def ScaleUp(self):
		miniMap.ScaleUp()

	def ScaleDown(self):
		miniMap.ScaleDown()

	def ShowAtlas(self):
		if not miniMap.IsAtlas():
			return
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()

	def ToggleAtlasWindow(self):
		if not miniMap.IsAtlas():
			return
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	if app.ENABLE_BIOLOGIST_SYSTEM:
		def SetBiyologMission(self, mission, missionState, givenCount, needCount, remainingTime):
			if self.wndBiyologWindow:
				self.wndBiyologWindow.SetMission(mission, missionState, givenCount, needCount, remainingTime)

		def OpenBiyologDialog(self):
			if self.interface and hasattr(self.interface, 'OpenBiyologDialog'):
				self.interface.OpenBiyologDialog()


	if app.ENABLE_OFFLINESHOP_SEARCH_SYSTEM:
		def OfflineShopSearch(self):
			if self.interface:
				self.interface.OpenPrivateShopSearch(0)
