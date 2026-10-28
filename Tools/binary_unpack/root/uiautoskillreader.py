if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import chat
import wndMgr

if app.USE_PASSIVE_ABILITY_TAMER:
	import skill

import ui
import constInfo
import localeInfo

SKILL_MASTER = 0
SKILL_GRAND_MASTER = 1
SKILL_PERFECT_MASTER = 2
SKILL_L_MASTER = 3

CHARACTER_PAGE = -1
PASSIVE_PAGE = -1
BUFFI_PAGE = -1

USE_YMIR_50300_SKILLBOOK = False

if app.USE_PASSIVE_ABILITY_TAMER:
	PASSIVE_TAMER_ABILITY_VNUM_LIST = []

	for i in range(skill.PASSIVE_ABILITY_VNUM_TAMER_1, skill.PASSIVE_ABILITY_VNUM_TAMER_MAX):
		PASSIVE_TAMER_ABILITY_VNUM_LIST.extend([i,])

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	window.SetPosition(*windowPos)

	if windowSize != (-1, -1):
		window.SetSize(*windowSize)

	if windowPositionRule:
		splitList = windowPositionRule.split(":")

		if len(splitList) == 2:
			(type, mode) = (splitList[0], splitList[1])

			if type == "horizontal":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetHorizontalAlignCenter()
					elif mode == "right":
						window.SetHorizontalAlignRight()
					elif mode == "left":
						window.SetHorizontalAlignLeft()
				else:
					if mode == "center":
						window.SetWindowHorizontalAlignCenter()
					elif mode == "right":
						window.SetWindowHorizontalAlignRight()
					elif mode == "left":
						window.SetWindowHorizontalAlignLeft()
			elif type == "vertical":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetVerticalAlignCenter()
					elif mode == "top":
						window.SetVerticalAlignTop()
					elif mode == "bottom":
						window.SetVerticalAlignBottom()
				else:
					if mode == "top":
						window.SetWindowVerticalAlignTop()
					elif mode == "center":
						window.SetWindowVerticalAlignCenter()
					elif mode == "bottom":
						window.SetWindowVerticalAlignBottom()

	if windowArgument:
		if isinstance(window, ui.TextLine):
			if windowFontName != -1:
				window.SetFontName(windowFontName)

			window.SetText(windowArgument)
		elif isinstance(window, ui.NumberLine):
			window.SetNumber(windowArgument)
		elif isinstance(window, ui.ExpandedImageBox) or isinstance(window, ui.ImageBox):
			window.LoadImage(windowArgument if windowArgument.find("gr2") == -1 else "icon/item/27995.tga")

	window.Show()
	return window

class Window(ui.BoardWithTitleBar):
	def Show(self):
		self.Refresh()
		wndMgr.Show(self.hWnd, False)

	def Hide(self):
		wndMgr.Hide(self.hWnd, False)
		return True

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def __LoadWindow(self):
		self.__children = {}

		self.AddFlag("movable")
		self.AddFlag("float")
		self.AddFlag("attach")

		self.SetTitleName(localeInfo.AUTO_SKILL_READER_TITLE)
		self.SetCloseEvent(self.Hide)
		self.SetSize(260, 252)
		self.SetCenterPosition()

		board = CreateWindow(ui.ThinBoardCircle(), self, (11, 60), "", "", (240, 185))
		self.__children["board"] = board

		text1 = CreateWindow(ui.TextLine(), board, (240 / 2, 7), localeInfo.AUTO_SKILL_READER_DESC1, "horizontal:center")
		self.__children["text1"] = text1

		text2 = CreateWindow(ui.TextLine(), board, (240 / 2, 24), localeInfo.AUTO_SKILL_READER_DESC2, "horizontal:center")
		self.__children["text2"] = text2

		__data = {}

		iLastPos = 0

		__data[iLastPos] = {\
								"name": localeInfo.AUTO_SKILL_READER_OPTION1,
								"exorcism" : 71001,
								"concentrated" : 71094,
								"slots" : {
									1 : [17 + (36 * 0), 80],
									2 : [17 + (36 * 1), 80],
									3 : [17 + (36 * 2), 80],
									4 : [17 + (36 * 3), 80],
									5 : [17 + (36 * 4), 80],
									6 : [17 + (36 * 5), 80]
								}
		}

		global CHARACTER_PAGE
		CHARACTER_PAGE = iLastPos

		iLastPos += 1

		if app.USE_PASSIVE_ABILITY_TAMER:
			xStart = 36
		else:
			xStart = self.__children["board"].GetWidth() / 2 - (32 / 2) + 2

		__data[iLastPos] = {\
								"name": localeInfo.AUTO_SKILL_READER_OPTION2,
								"exorcism" : 71001,
								"concentrated" : 71094,
								"slots" : {
									121 : [xStart + (36 * 0), 80],
								}
		}

		if app.USE_PASSIVE_ABILITY_TAMER:
			j = 1
			for i in range(skill.PASSIVE_ABILITY_VNUM_TAMER_1, skill.PASSIVE_ABILITY_VNUM_TAMER_MAX):
				__data[iLastPos]["slots"][i] = [xStart + (36 * j), 80]
				j += 1

		global PASSIVE_PAGE
		PASSIVE_PAGE = iLastPos

		iLastPos += 1

		if app.__BUFFI_SUPPORT__:
			__data[iLastPos] = {\
									"name": localeInfo.AUTO_SKILL_READER_OPTION3,
									"exorcism" : 71001,
									"concentrated" : 71094,
									"slots" : {
										164 : [52 + (36 * 0), 80],
										165 : [52 + (36 * 1), 80],
										166 : [52 + (36 * 2), 80],
										167 : [52 + (36 * 3), 80]
									}
			}

			global BUFFI_PAGE
			BUFFI_PAGE = iLastPos

			iLastPos += 1

		self.__children["data"] = __data

		i = 0
		for key, data in __data.items():
			categoryBtn = CreateWindow(ui.RadioButton(), self, (11 + (60 * i), 39))
			categoryBtn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
			categoryBtn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
			categoryBtn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
			categoryBtn.SetText(data["name"])
			categoryBtn.SAFE_SetEvent(self.__SetPage, i)
			self.__children["categoryBtn" + str(key)] = categoryBtn

			slots = data["slots"]

			for slotIdx, slotData in slots.items():
				self.__children["slotImg" + str(key) + str(slotIdx)] = self.__CreateSlot(board, *slotData)

			slotWindow = CreateWindow(ui.SlotWindow(), board, (0, 0), "", "", (board.GetWidth(), board.GetHeight()))
			slotWindow.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnSelectItemSlot))
			slotWindow.SetUnselectItemSlotEvent(ui.__mem_func__(self.__OnSelectItemSlot))
			slotWindow.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInSkill))
			slotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkill))

			for slotIdx, slotData in slots.items():
				slotWindow.AppendSlot(slotIdx, slotData[0], slotData[1], 32, 32)
			self.__children["slotWindow"+str(key)] = slotWindow
			i += 1

		for j in xrange(3):
			x = j * (32 + 4)
			self.__children["defaultslotImg"+str(j)] = self.__CreateSlot(board, 71+x, 40+0)

		defaultSlotWindow = CreateWindow(ui.SlotWindow(), board, (71, 40), "", "", (104, 32))
		for j in xrange(3):
			x = j * (32 + 4)
			defaultSlotWindow.AppendSlot(j, x, 0, 32, 32)
		defaultSlotWindow.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		defaultSlotWindow.SetSelectItemSlotEvent(ui.__mem_func__(self.__OnSelectItem))
		defaultSlotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		defaultSlotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.__children["defaultSlotWindow"] = defaultSlotWindow

		startBtn = CreateWindow(ui.RadioButton(), board, (36, 161))
		startBtn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		startBtn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		startBtn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		startBtn.SetText(localeInfo.AUTO_SKILL_READER_START)
		startBtn.SAFE_SetEvent(self.__SetStatus, 1)
		self.__children["startBtn"] = startBtn

		stopBtn = CreateWindow(ui.RadioButton(), board, (126, 161))
		stopBtn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		stopBtn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		stopBtn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		stopBtn.SetText(localeInfo.AUTO_SKILL_READER_STOP)
		stopBtn.SAFE_SetEvent(self.__SetStatus, 0)
		self.__children["stopBtn"] = stopBtn

		self.__children["botStatus"] = 0
		self.__children["selectedSkillIdx"] = -1

		self.__SetPage(0)

	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()

	def Destroy(self):
		self.__children = {}

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def ServerSetStatus(self, status):
		self.__children["botStatus"] = True if int(status) else False
		self.Refresh()

	def __SetStatus(self, status):
		botStatus = self.__children["botStatus"] if self.__children.has_key("botStatus") else 0
		if botStatus == status:
			self.Refresh()
			return
		self.Refresh()
		selectedSkill = self.__children["selectedSkill"] if self.__children.has_key("selectedSkill") else 0
		if status:
			if selectedSkill == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AUTO_SKILL_READER_MSG1)
				return
			elif self.__SkillToBook(selectedSkill) == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.AUTO_SKILL_READER_MSG2)
				return

		if selectedSkill >= 1 and selectedSkill <= 6:
			net.SendChatPacket("/auto_skill_reader %d %d" % (player.GetSkillIndex(selectedSkill), status))
		else:
			net.SendChatPacket("/auto_skill_reader %d %d" % (selectedSkill, status))

	def __SkillToMaxGrade(self, skillIdx):
		if skillIdx >= 1 and skillIdx <= 6:
			return SKILL_L_MASTER
		else:
			__skillToMaxGrade = {
				122 : SKILL_PERFECT_MASTER,
				124 : SKILL_PERFECT_MASTER,
				121 : SKILL_PERFECT_MASTER,
				129 : SKILL_PERFECT_MASTER,
			}
			if __skillBookItemList.has_key(skillIdx):
				return __skillBookItemList[skillIdx]
		return 0

	def __SkillToBook(self, skillIdx):
		if skillIdx >= 1 and skillIdx <= 6:
			grade = player.GetSkillGrade(skillIdx)
			if grade == 1:
				global USE_YMIR_50300_SKILLBOOK
				return [50300, player.GetSkillIndex(skillIdx)] if USE_YMIR_50300_SKILLBOOK else 50400 + player.GetSkillIndex(skillIdx)
			elif grade == 2:
				return 50513
		else:
			if 122 == skillIdx and player.GetSkillLevelNew(skillIdx) >= 2:#combo_checking_level
				return 0

			# if skill has 1 book put here
			__skillBookItemList = {
				122 : 50304,
				124 : 50600,
			}

			if __skillBookItemList.has_key(skillIdx):
				return __skillBookItemList[skillIdx]

			#if skill different book for every stage(M | G | P)
			__skillBookItemList = {
				121 : {
					SKILL_MASTER : 50301,
					SKILL_GRAND_MASTER : 50302,
					SKILL_PERFECT_MASTER : 50303,
				},
				129 : {
					SKILL_MASTER : 50314,
					SKILL_GRAND_MASTER : 50315,
					SKILL_PERFECT_MASTER : 50316,
				},
			}

			if app.USE_PASSIVE_ABILITY_TAMER:
				j = 0
				for i in range(skill.PASSIVE_ABILITY_VNUM_TAMER_1, skill.PASSIVE_ABILITY_VNUM_TAMER_MAX):
					__skillBookItemList[i] = {\
											SKILL_MASTER : 50940 + (j * 10),
											SKILL_GRAND_MASTER : 50941 + (j * 10),
											SKILL_PERFECT_MASTER : 50942 + (j * 10),
					}

					j += 1

			if app.__BUFFI_SUPPORT__:
				__skillBookItemList[164] = {\
												SKILL_MASTER : 0,
												SKILL_GRAND_MASTER : 61035,
												SKILL_PERFECT_MASTER : 61031,
				}

				__skillBookItemList[165] = {\
												SKILL_MASTER : 0,
												SKILL_GRAND_MASTER : 61036,
												SKILL_PERFECT_MASTER : 61031,
				}

				__skillBookItemList[166] = {\
												SKILL_MASTER : 0,
												SKILL_GRAND_MASTER : 61037,
												SKILL_PERFECT_MASTER : 61031,
				}

				__skillBookItemList[167] = {\
												SKILL_MASTER : 0,
												SKILL_GRAND_MASTER : 61038,
												SKILL_PERFECT_MASTER : 61031,
				}

			if __skillBookItemList.has_key(skillIdx):
				grade = player.GetSkillGradeNew(skillIdx)
				if __skillBookItemList[skillIdx].has_key(grade):
					return __skillBookItemList[skillIdx][grade]

		return 0

	def __OnSelectItem(self, selectedSlotPos):
		botStatus = self.__children["botStatus"] if self.__children.has_key("botStatus") else 0
		if botStatus:
			return
		self.__children["selectedSkill"] = 0
		self.Refresh()
		return True
	def __OnSelectItemSlot(self, selectedSlotPos):
		botStatus = self.__children["botStatus"] if self.__children.has_key("botStatus") else 0
		if botStatus:
			return
		selectedSkill = self.__children["selectedSkill"] if self.__children.has_key("selectedSkill") else 0
		self.__children["selectedSkill"] = 0 if selectedSkill == selectedSlotPos else selectedSlotPos
		self.Refresh()
		return True

	def Refresh(self):
		__data = self.__children["data"] if self.__children.has_key("data") else {}

		currentPage = self.__children["currentPage"]
		selectedSkill = self.__children["selectedSkill"] if self.__children.has_key("selectedSkill") else 0

		global CHARACTER_PAGE
		i = CHARACTER_PAGE

		if app.USE_PASSIVE_ABILITY_TAMER:
			isTamer = False

			global PASSIVE_ABILITY_VNUM_LIST
			if selectedSkill in PASSIVE_TAMER_ABILITY_VNUM_LIST:
				isTamer = True

		for key, data in __data.items():
			if currentPage == i:
				slots = data["slots"]
				for slotIdx, slotData in slots.items():
					self.__children["slotImg"+str(key)+str(slotIdx)].Show()

				self.__children["categoryBtn"+str(key)].Down()
				slotWindow = self.__children["slotWindow"+str(key)]

				for slotIdx, slotData in slots.items():
					skillGrade = player.GetSkillGrade(slotIdx) if (currentPage == CHARACTER_PAGE) else player.GetSkillGradeNew(slotIdx)
					skillLevel = player.GetSkillLevel(slotIdx) if (currentPage == CHARACTER_PAGE) else player.GetSkillLevelNew(slotIdx)

					if currentPage == CHARACTER_PAGE:
						slotWindow.SetSkillSlotNew(slotIdx, player.GetSkillIndex(slotIdx), skillGrade, skillLevel)
					elif app.__BUFFI_SUPPORT__ and currentPage == BUFFI_PAGE:
						slotWindow.SetSkillSlotNew(slotIdx, slotIdx, skillGrade, skillLevel)
					else:
						slotWindow.SetSkillSlot(slotIdx, slotIdx, skillLevel)

					slotWindow.SetSlotCountNew(slotIdx, skillGrade, skillLevel)

					if selectedSkill == slotIdx:
						import wndMgr
						# ActivateSlot takes 2 parameters: slotIndex and colorType
						# RGB (8, 159, 205) is a blue color, using COLOR_TYPE_WHITE as default
						colorType = wndMgr.COLOR_TYPE_WHITE
						if hasattr(wndMgr, 'COLOR_TYPE_BLUE'):
							colorType = wndMgr.COLOR_TYPE_BLUE
						slotWindow.ActivateSlot(slotIdx, colorType)
					else:
						slotWindow.DeactivateSlot(slotIdx)

				slotWindow.RefreshSlot()
				slotWindow.Show()

				# default slot window!

				defaultSlotWindow = self.__children["defaultSlotWindow"]
				book = self.__SkillToBook(selectedSkill)
				if isinstance(book, list):
					if book == 50513:
						book_count = player.GetItemCountByVnum(book)
					else:
						book_count = player.GetItemCountByVnumNew(book[0], book[1])
					book = book[0]
				else:
					if book == 50513:
						book_count = player.GetItemCountByVnum(book)
					else:
						if app.ENABLE_SPECIAL_INVENTORY and hasattr(player, 'GetSpecialBookItemCount'):
							book_count = player.GetSpecialBookItemCount(book)
						else:
							book_count = player.GetItemCountByVnum(book)
				defaultSlotWindow.SetItemSlot(0, book, book_count)
				if book_count:
					defaultSlotWindow.EnableSlot(0)
				else:
					defaultSlotWindow.DisableSlot(0)

				exorcism = data["exorcism"] if data.has_key("exorcism") else 0

				if app.USE_PASSIVE_ABILITY_TAMER and isTamer == True:
					exorcism = 50973

				exorcism_count = player.GetItemCountByVnum(exorcism)

				defaultSlotWindow.SetItemSlot(1, exorcism, exorcism_count)

				if exorcism_count:
					defaultSlotWindow.EnableSlot(1)
				else:
					defaultSlotWindow.DisableSlot(1)

				concentrated = data["concentrated"] if data.has_key("concentrated") else 0

				if app.USE_PASSIVE_ABILITY_TAMER and isTamer == True:
					concentrated = 50974

				concentrated_count = player.GetItemCountByVnum(concentrated)

				defaultSlotWindow.SetItemSlot(2, concentrated, concentrated_count)

				if concentrated_count:
					defaultSlotWindow.EnableSlot(2)
				else:
					defaultSlotWindow.DisableSlot(2)

				defaultSlotWindow.RefreshSlot()
			else:
				for slotIdx, slotData in data["slots"].items():
					self.__children["slotImg"+str(key)+str(slotIdx)].Hide()

				self.__children["categoryBtn"+str(key)].SetUp()
				self.__children["slotWindow"+str(key)].Hide()

			i+=1

		botStatus = self.__children["botStatus"]
		self.__children["startBtn" if not botStatus else "stopBtn"].SetUp()
		self.__children["startBtn" if botStatus else "stopBtn"].Down()


	def OverInItem(self, slotNumber):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		elif not interface.tooltipItem:
			return

		currentPage = self.__children["currentPage"]
		__data = self.__children["data"] if self.__children.has_key("data") else {}

		global CHARACTER_PAGE
		i = CHARACTER_PAGE

		selectedSkill = self.__children["selectedSkill"] if self.__children.has_key("selectedSkill") else 0

		if app.USE_PASSIVE_ABILITY_TAMER:
			isTamer = False

			global PASSIVE_ABILITY_VNUM_LIST
			if selectedSkill in PASSIVE_TAMER_ABILITY_VNUM_LIST:
				isTamer = True

		for key, data in __data.items():
			if currentPage == i:
				if slotNumber == 1:
					itemIdx = data["exorcism"] if data.has_key("exorcism") else 0

					if app.USE_PASSIVE_ABILITY_TAMER and isTamer == True:
						itemIdx = 50973
				elif slotNumber == 2:
					itemIdx = data["concentrated"] if data.has_key("concentrated") else 0

					if app.USE_PASSIVE_ABILITY_TAMER and isTamer == True:
						itemIdx = 50974
				else:
					itemIdx = 0
					if selectedSkill:
						book = self.__SkillToBook(selectedSkill)
						if not isinstance(book, list):
							itemIdx = book
						else:
							tooltipItem = interface.tooltipItem
							tooltipItem.ClearToolTip()
							metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)]
							attrSlot = [(0, 0) for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
							metinSlot[0] = book[1]
							tooltipItem.AddItemData(book[0], metinSlot, attrSlot)
							return

				if itemIdx:
					interface.tooltipItem.SetItemToolTip(itemIdx)

				return

			i += 1

	def OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		elif not interface.tooltipItem:
			return

		interface.tooltipItem.HideToolTip()

	def OverOutSkill(self):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		elif not interface.tooltipSkill:
			return

		interface.tooltipSkill.HideToolTip()

	def OverInSkill(self, slotNumber):
		interface = constInfo.GetInterfaceInstance()
		if not interface:
			return
		elif not interface.tooltipSkill:
			return

		global CHARACTER_PAGE

		currentPage = self.__children["currentPage"]
		skillIndex = player.GetSkillIndex(slotNumber) if currentPage == CHARACTER_PAGE else slotNumber
		skillLevel = player.GetSkillLevel(slotNumber) if currentPage == CHARACTER_PAGE else player.GetSkillLevelNew(slotNumber)
		skillGrade = player.GetSkillGrade(slotNumber) if currentPage == CHARACTER_PAGE else player.GetSkillGradeNew(slotNumber)
		interface.tooltipSkill.SetSkillNew(slotNumber, skillIndex, skillGrade, skillLevel)

	def __SetPage(self, pageIdx):
		self.__children["currentPage"] = pageIdx
		self.Refresh()

	def __CreateSlot(self, parent, x, y):
		return CreateWindow(ui.ImageBox(), parent, (x, y), "d:/ymir work/ui/public/slot_base.sub")
