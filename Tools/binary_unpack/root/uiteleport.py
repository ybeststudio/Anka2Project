if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiToolTip
import wndMgr
import localeInfo
import chat
import constInfo
import uiCommon
import item

WINDOW_WIDTH = 543
WINDOW_HEIGHT = 450

buttons_list = {
	'nav_btns': [
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_0},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_1},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_2},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_3},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_4},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_5},
		{'btn_name': localeInfo.TELEPORT_WINDOW_CLASS_6},
	],

	'subBoardsButtons': {
		0: [
		   {'btn_name': localeInfo.MAP_A1,						'index': 1,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_A3,						'index': 2,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_B1,						'index': 3,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_B3,						'index': 4,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_C1,						'index': 5,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_C3,						'index': 6,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0}
		],
		1: [
		   {'btn_name': localeInfo.MAP_SPIDERDUNGEON,			'index': 13,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SPIDERDUNGEON_02,		'index': 14,	'minLevel': 40,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SPIDERDUNGEON_03,		'index': 15,	'minLevel': 40,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SKIPIA_DUNGEON_01,		'index': 16,	'minLevel': 75,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SKIPIA_DUNGEON_02,		'index': 17,	'minLevel': 75,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		],
		2: [
		   {'btn_name': localeInfo.MAP_CAPE,					'index': 19,	'minLevel': 90,	'maxLevel': 110,	'costReq': 5000000,	'itemReq': 0},
		   {'btn_name': localeInfo.MAP_DAWN,					'index': 20,	'minLevel': 90,	'maxLevel': 110,	'costReq': 5000000,	'itemReq': 0},
		   {'btn_name': localeInfo.MAP_BAY,						'index': 21,	'minLevel': 90,	'maxLevel': 110,	'costReq': 5000000,	'itemReq': 0},
		   {'btn_name': localeInfo.MAP_THUNDER,					'index': 22,	'minLevel': 90,	'maxLevel': 110,	'costReq': 5000000,	'itemReq': 0}
		],
		3: [
		   {'btn_name': localeInfo.MAP_A2,						'index': 7,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_DESERT,					'index': 8,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SNOW,					'index': 9,		'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_N_FLAME_01,				'index': 10,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_NUSLUCK,					'index': 18,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_TRENT,					'index': 11,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_TRENT02,					'index': 12,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0},
		],
		4: [
		   {'btn_name': localeInfo.MAP_MAGIC_FOREST,			'index': 24,		'minLevel': 100,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0}
		],
		5: [
		   {'btn_name': localeInfo.MAP_ABANDONED_CASTLE,		'index': 25,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_DONGAN_PLAIN,			'index': 26,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_SEOGAN_SWAMP,			'index': 27,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_NAMGAN_GAP,			    'index': 28,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_NORTH_WIND_CACHE,		'index': 29,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_NORTH_R_CLIFF,	        'index': 30,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_NORTH_WIND_DESCEN,		'index': 31,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_MYSTERIOUS_DUNGEON,		'index': 32,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_YILAD_PASS,			    'index': 33,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0},
		   {'btn_name': localeInfo.MAP_CRYSTAL_CAVE,			'index': 34,		'minLevel': 100,	'maxLevel': 120,	'costReq': 0,		'itemReq': 0}
		],
		6: [
		   {'btn_name': localeInfo.MAP_TRADE,					'index': 23,	'minLevel': 1,	'maxLevel': 110,	'costReq': 0,		'itemReq': 0}
		],
	}
}

class TeleportWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Initialize()
		self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.Initialize()

	def Destroy(self):
		self.ClearDictionary()

	def Initialize(self):
		self.popUp = None
		self.board = None
		self.navBoard = None
		self.subNavBoard = None
		self.subNavBoardTitle = None

		self.navButtons = []
		self.subBoards = []

		self.subPageBtnIndex = -1
		self.navBtnIndex = -1
		self.index = 0

		self.subBoardButtons = []
		self.infoBoard = []

		self.buttontooltip = None
		self.ShowButtonToolTip = FALSE
		self.questionDialog = None
		self.tooltipItem = None
		self.mapName = None
		self.levelReqValue = None
		self.moneyReqValue = None
		self.itemReqValue = None

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/TeleportWindow.py")
		except:
			import exception
			exception.Abort("TeleportWindow.LoadWindow.LoadScriptError")

		try:
			self.buttontooltip = uiToolTip.ToolTip()
			self.buttontooltip.ClearToolTip()

			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.HideToolTip()

			self.board = self.GetChild('board')
			self.board.SetCloseEvent(self.Close)

			self.navBoard = self.GetChild('nav_board')
			self.subNavBoard = self.GetChild('sub_nav_board')
			self.subNavBoard.Hide()
			self.subNavBoardTitle = self.GetChild('sub_nav_board_title')

			self.infoBoard = self.GetChild('info_board')
			self.infoBoard.Hide()

			self.GetChild('lvReqIcon').OnMouseOverIn = lambda arg = localeInfo.TELEPORT_WINDOW_LEVEL_ICON_TOOLTIP : self.OverInToolTipButton(arg)
			self.GetChild('lvReqIcon').OnMouseOverOut = lambda : self.OverOutToolTipButton()

			self.GetChild('moneyReqIcon').OnMouseOverIn = lambda arg = localeInfo.TELEPORT_WINDOW_MONEY_ICON_TOOLTIP : self.OverInToolTipButton(arg)
			self.GetChild('moneyReqIcon').OnMouseOverOut = lambda : self.OverOutToolTipButton()

			self.GetChild('itemReqIcon').OnMouseOverIn = lambda arg = localeInfo.TELEPORT_WINDOW_ITEM_ICON_TOOLTIP : self.OverInToolTipButton(arg)
			self.GetChild('itemReqIcon').OnMouseOverOut = lambda : self.OverOutToolTipButton()

			self.GetChild('itemImage').OnMouseOverIn = lambda arg = 0 : self.OverInToolTipButtonItem(arg)
			self.GetChild('itemImage').OnMouseOverOut = lambda : self.OverOutToolTipButton()

			self.GetChild('backButton').SetEvent(ui.__mem_func__(self.__OnPressBackButton))
			self.GetChild('backButton').Hide()

			self.GetChild('warpButton').SetEvent(ui.__mem_func__(self.__OnPressWarpButton))

			self.mapName = self.GetChild('mapName')
			self.levelReqValue = self.GetChild('lvReqValue')
			self.moneyReqValue = self.GetChild('moneyReqValue')
			self.itemReqValue = self.GetChild('itemReqValue')

		except KeyError:
			pass

	def __GetResults(self, arg):
		return buttons_list['subBoardsButtons'][self.navBtnIndex][self.subPageBtnIndex][arg]

	def __Refresh(self):
		self.index = 0

		self.mapName.SetText('--')
		self.levelReqValue.SetText('--')
		self.moneyReqValue.SetText('--')
		self.itemReqValue.SetText('--')

	def __OnPressBackButton(self):
		self.navBoard.Show()
		self.subNavBoard.Hide()
		self.infoBoard.Hide()

		self.GetChild('backButton').Hide()

		navBtnCount = len(buttons_list['nav_btns'])
		self.__SetNavHeight(navBtnCount)
		self.__Refresh()

	def __OnPressWarpButton(self):
		if self.index == 0:
			self.OnCreatePopUp(localeInfo.TELEPORT_WINDOW_UNSELECTED_MAP_INDEX)
			return

		questionDialog = uiCommon.QuestionDialog("thin")
		questionDialog.SetText(localeInfo.TELEPORT_WINDOW_CONFIRM_WARP % self.__GetResults('btn_name'))
		questionDialog.SetAcceptEvent(lambda arg = TRUE: self.__AnswerWarpButton(arg))
		questionDialog.SetCancelEvent(lambda arg = FALSE: self.__AnswerWarpButton(arg))
		questionDialog.Open()
		self.questionDialog = questionDialog

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __AnswerWarpButton(self, isConfirm):
		if isConfirm:
			net.SendChatPacket("/warp_on {}".format(self.index))
			self.__Refresh()
			self.Close()

		if self.questionDialog:
			self.questionDialog.Close()

		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	def __OnPressNavButton(self, navBtnIndex):
		self.navBtnIndex = navBtnIndex

		self.navBoard.Hide()
		self.subNavBoard.Show()
		self.infoBoard.Hide()

		self.GetChild('backButton').Show()

		self.subNavBoardTitle.SetText('{}'.format(buttons_list['nav_btns'][navBtnIndex]['btn_name']))

		buttonsPerRow = 2

		yPos = 25
		hPos = 40

		subBoardBtnCount = len(buttons_list['subBoardsButtons'][navBtnIndex])
		self.subBoardButtons = []
		for i in xrange(subBoardBtnCount):
			row = i // buttonsPerRow
			col = i % buttonsPerRow

			radioBtn = ui.RadioButton()
			radioBtn.SetParent(self.subNavBoard)
			radioBtn.SetUpVisual('d:/ymir work/ui/game/teleport/subnav_image/{}_in.png'.format(buttons_list['subBoardsButtons'][navBtnIndex][i]['index']))
			radioBtn.SetOverVisual('d:/ymir work/ui/game/teleport/subnav_image/{}_out.png'.format(buttons_list['subBoardsButtons'][navBtnIndex][i]['index']))
			radioBtn.SetDownVisual('d:/ymir work/ui/game/teleport/subnav_image/{}_out.png'.format(buttons_list['subBoardsButtons'][navBtnIndex][i]['index']))
			radioBtn.SetText('{}'.format(buttons_list['subBoardsButtons'][navBtnIndex][i]['btn_name']))
			radioBtn.SetTextPosition(10, 50)
			radioBtn.SetPosition(7 + col * (radioBtn.GetWidth() + 10), yPos + (25 + hPos) * row)
			radioBtn.SetWindowHorizontalAlignLeft()
			radioBtn.SetEvent(ui.__mem_func__(self.__OnPressSubBoardButton), i)
			radioBtn.Show()
			self.subBoardButtons.append(radioBtn)

		if self.infoBoard.IsShow():
			self.__SetSubBoardHeight(subBoardBtnCount, buttonsPerRow)

		self.__SetSubBoardHeight(subBoardBtnCount)

	def __OnPressSubBoardButton(self, subPageBtnIndex):
		self.infoBoard.Show()

		buttons = self.subBoardButtons
		for btn in buttons:
			btn.SetUp()
		buttons[subPageBtnIndex].Down()

		self.subPageBtnIndex = subPageBtnIndex
		self.index = self.__GetResults('index')

		self.mapName.SetText(self.__GetResults('btn_name'))
		self.levelReqValue.SetText(localeInfo.TELEPORT_WINDOW_LEVEL_REQ % (self.__GetResults('minLevel'), self.__GetResults('maxLevel')))
		self.moneyReqValue.SetText(localeInfo.TELEPORT_WINDOW_MONEY_REQ % localeInfo.NumberToDecimalString(self.__GetResults('costReq')))

		if self.__GetResults('itemReq') != 0:
			item.SelectItem(self.__GetResults('itemReq'))
			itemName = item.GetItemName()
			self.itemReqValue.SetText(localeInfo.TELEPORT_WINDOW_ITEM_REQ % itemName)
		else:
			self.itemReqValue.SetText(localeInfo.TELEPORT_WINDOW_ITEM_REQ % "--")

		if self.infoBoard.IsShow():
			self.__SetSubBoardHeight(len(buttons_list['subBoardsButtons'][self.navBtnIndex]))

	def __CreateNavButtons(self):
		yPos = 25
		navBtnCount = len(buttons_list['nav_btns'])

		buttonsPerRow = 2

		for i in xrange(navBtnCount):
			row = i // buttonsPerRow
			col = i % buttonsPerRow

			btn = ui.MakeButton(self.navBoard, 0, yPos+25*i, '', 'd:/ymir work/ui/game/teleport/nav_image/', 'btn_{}_normal.png'.format(i), 'btn_{}_over.png'.format(i), 'btn_{}_down.png'.format(i))
			btn.SetWindowHorizontalAlignLeft()
			btn.SetPosition(7 + col * (btn.GetWidth() + 10), yPos + (25 + 40) * row)
			btn.SetText('{}'.format(buttons_list['nav_btns'][i]['btn_name']))
			btn.SetTextPosition(10, 50)
			btn.SetEvent(ui.__mem_func__(self.__OnPressNavButton), i)
			self.navButtons.append(btn)

		self.__SetNavHeight(navBtnCount)

	def __SetNavHeight(self, navBtnCount):
		buttonsPerRow = 2
		num_rows = (navBtnCount + buttonsPerRow - 1) // buttonsPerRow

		self.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40))
		self.board.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40))
		self.navBoard.SetSize(WINDOW_WIDTH - 20, 74 + num_rows*(25+40) - 42)
		self.UpdateRect()

	def __SetSubBoardHeight(self, subBoardBtnCount):
		buttonsPerRow = 2
		num_rows = (subBoardBtnCount + buttonsPerRow - 1) // buttonsPerRow

		if self.infoBoard.IsShow():
			self.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40) + 135)
			self.board.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40))
			self.subNavBoard.SetSize(WINDOW_WIDTH - 20, 74 + num_rows*(25+40) - 42)
		else:
			self.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40))
			self.board.SetSize(WINDOW_WIDTH, 74 + num_rows*(25+40))
			self.subNavBoard.SetSize(WINDOW_WIDTH - 20, 74 + num_rows*(25+40) - 42)

		self.UpdateRect()

	def OverInToolTipButtonItem(self, arg = 0):
		self.tooltipItem.ClearToolTip()

		if self.index == 0 and arg == 0:
			return

		if arg == 0:
			itemVnum = self.__GetResults('itemReq')
		else:
			itemVnum = arg
		if itemVnum != 0:
			self.tooltipItem.SetItemToolTip(itemVnum)
		else:
			self.tooltipItem.AppendTextLine(localeInfo.TELEPORT_WINDOW_NO_ITEM_REQ)

		if arg != 0:
			self.tooltipItem.AppendTextLine("--------------------------------------")
			self.tooltipItem.AppendTextLine(localeInfo.TELEPORT_WINDOW_ITEM_REQ_DESC_0, 0xFF2b8029)
			self.tooltipItem.AppendTextLine(localeInfo.TELEPORT_WINDOW_ITEM_REQ_DESC_1, 0xFF2b8029)

		if self.tooltipItem:
			self.tooltipItem.ShowToolTip()

	def OverInToolTipButton(self, arg):
		arglen = len(str(arg))
		pos_x, pos_y = wndMgr.GetMousePosition()

		self.buttontooltip.ClearToolTip()
		self.buttontooltip.SetThinBoardSize(40 + (4 * arglen))
		self.buttontooltip.SetToolTipPosition(pos_x + 50, pos_y + 50)
		self.buttontooltip.AppendTextLine(arg, 0xffffffff)
		self.buttontooltip.Show()
		self.ShowButtonToolTip = TRUE

	def OverOutToolTipButton(self):
		self.buttontooltip.Hide()
		self.ShowButtonToolTip = FALSE

		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def ButtonToolTipProgress(self) :
		if self.ShowButtonToolTip :
			pos_x, pos_y = wndMgr.GetMousePosition()
			self.buttontooltip.SetToolTipPosition(pos_x - 50, pos_y - 15)

	def OnUpdate(self):
		self.ButtonToolTipProgress()

	def OnCreatePopUp(self, msg):
		popup = uiCommon.PopupDialog()
		popup.SetText(msg)
		popup.Open()
		self.popup = popup

	def Open(self):
		self.__CreateNavButtons()
		self.Show()
		self.SetTop()

	def Close(self):
		self.__OnPressBackButton()
		self.Hide()

	def OnPressEscapeKey(self):
		if self.subNavBoard.IsShow():
			self.__OnPressBackButton()
		else:
			self.Close()

		return TRUE
