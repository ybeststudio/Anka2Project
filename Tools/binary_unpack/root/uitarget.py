if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chr = __import__(pyapi.GetModuleName("chr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import wndMgr
import messenger
import guild
import nonplayer
import localeInfo
import constInfo

if app.ENABLE_MESSENGER_BLOCK:
	import uiCommon

if app.ENABLE_MOB_DROP_INFO:
	import uiToolTip
	import item

	def HAS_FLAG(value, flag):
		return (value & flag) == flag

	MONSTER_INFO_DATA = {}

class TargetBoard(ui.ThinBoard):

	BUTTON_NAME_LIST = [
		localeInfo.TARGET_BUTTON_WHISPER,
		localeInfo.TARGET_BUTTON_EXCHANGE,
		localeInfo.TARGET_BUTTON_FIGHT,
		localeInfo.TARGET_BUTTON_ACCEPT_FIGHT,
		localeInfo.TARGET_BUTTON_AVENGE,
		localeInfo.TARGET_BUTTON_FRIEND,
		localeInfo.TARGET_BUTTON_INVITE_PARTY,
		localeInfo.TARGET_BUTTON_LEAVE_PARTY,
		localeInfo.TARGET_BUTTON_EXCLUDE,
		localeInfo.TARGET_BUTTON_INVITE_GUILD,
		localeInfo.TARGET_BUTTON_DISMOUNT,
		localeInfo.TARGET_BUTTON_EXIT_OBSERVER,
		localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
		localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
		localeInfo.TARGET_BUTTON_BUILDING_DESTROY,
		localeInfo.TARGET_BUTTON_EMOTION_ALLOW,
		"VOTE_BLOCK_CHAT",
	]

	if app.ENABLE_MESSENGER_BLOCK:
		BUTTON_NAME_LIST.append(localeInfo.TARGET_BUTTON_BLOCK)
		BUTTON_NAME_LIST.append(localeInfo.TARGET_BUTTON_UNBLOCK)

	GRADE_NAME =	{
						nonplayer.PAWN : localeInfo.TARGET_LEVEL_PAWN,
						nonplayer.S_PAWN : localeInfo.TARGET_LEVEL_S_PAWN,
						nonplayer.KNIGHT : localeInfo.TARGET_LEVEL_KNIGHT,
						nonplayer.S_KNIGHT : localeInfo.TARGET_LEVEL_S_KNIGHT,
						nonplayer.BOSS : localeInfo.TARGET_LEVEL_BOSS,
						nonplayer.KING : localeInfo.TARGET_LEVEL_KING,
					}

	EXCHANGE_LIMIT_RANGE = 3000

	def __init__(self):
		ui.ThinBoard.__init__(self)

		if app.ENABLE_MESSENGER_BLOCK:
			self.AddFlag("float")

		name = ui.TextLine()
		name.SetParent(self)
		name.SetDefaultFontName()
		name.SetOutline()
		name.Show()

		hpGauge = ui.DynamicGauge()
		hpGauge.SetParent(self)
		hpGauge.MakeGauge(130, "red", "yellow")
		hpGauge.Hide()

		if app.ENABLE_ATTENDANCE_EVENT:
			hitCount = ui.TextLine()
			hitCount.SetParent(name)
			hitCount.SetDefaultFontName()
			hitCount.SetOutline()
			hitCount.Show()
			self.hitCount = hitCount

		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			hpDecimal = ui.TextLine()
			hpDecimal.SetParent(hpGauge)
			hpDecimal.SetDefaultFontName()
			hpDecimal.SetPosition(5, 5)
			hpDecimal.SetOutline()
			hpDecimal.Hide()

		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			elementImage = ui.ImageBox()
			elementImage.SetParent(self)
			elementImage.Hide()

		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		closeButton.SetPosition(30, 13)

		hpGauge.SetPosition(175, 17)
		hpGauge.SetWindowHorizontalAlignRight()

		closeButton.SetWindowHorizontalAlignRight()
		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()

		if app.ENABLE_MOB_DROP_INFO:
			infoButton = ui.Button()
			infoButton.SetParent(self)
			infoButton.SetUpVisual("d:/ymir work/ui/pattern/i_mark_01.tga")
			infoButton.SetOverVisual("d:/ymir work/ui/pattern/i_mark_02.tga")
			infoButton.SetDownVisual("d:/ymir work/ui/pattern/i_mark_03.tga")
			infoButton.SetEvent(ui.__mem_func__(self.OnPressedInfoButton))
			infoButton.Hide()

			infoBoard = self.InfoBoard()
			infoBoard.Hide()
			infoButton.showWnd = infoBoard

		self.buttonDict = {}
		self.showingButtonList = []
		for buttonName in self.BUTTON_NAME_LIST:
			button = ui.Button()
			button.SetParent(self)

			button.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
			button.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
			button.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")

			button.SetWindowHorizontalAlignCenter()
			button.SetText(buttonName)
			button.Hide()
			self.buttonDict[buttonName] = button
			self.showingButtonList.append(button)

		self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER].SetEvent(ui.__mem_func__(self.OnWhisper))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE].SetEvent(ui.__mem_func__(self.OnExchange))
		self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_ACCEPT_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_AVENGE].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyInvite))
		self.buttonDict[localeInfo.TARGET_BUTTON_LEAVE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyExit))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCLUDE].SetEvent(ui.__mem_func__(self.OnPartyRemove))

		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_GUILD].SAFE_SetEvent(self.__OnGuildAddMember)
		self.buttonDict[localeInfo.TARGET_BUTTON_DISMOUNT].SAFE_SetEvent(self.__OnDismount)
		self.buttonDict[localeInfo.TARGET_BUTTON_EXIT_OBSERVER].SAFE_SetEvent(self.__OnExitObserver)
		self.buttonDict[localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT].SAFE_SetEvent(self.__OnViewEquipment)
		self.buttonDict[localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY].SAFE_SetEvent(self.__OnRequestParty)
		self.buttonDict[localeInfo.TARGET_BUTTON_BUILDING_DESTROY].SAFE_SetEvent(self.__OnDestroyBuilding)
		self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW].SAFE_SetEvent(self.__OnEmotionAllow)

		if app.ENABLE_MESSENGER_BLOCK:
			self.buttonDict[localeInfo.TARGET_BUTTON_BLOCK].SetEvent(ui.__mem_func__(self.__OnBlock))
			self.buttonDict[localeInfo.TARGET_BUTTON_UNBLOCK].SetEvent(ui.__mem_func__(self.__OnBlockRemove))

		self.buttonDict["VOTE_BLOCK_CHAT"].SetEvent(ui.__mem_func__(self.__OnVoteBlockChat))

		self.name = name
		self.hpGauge = hpGauge

		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = hpDecimal

		if app.ENABLE_MOB_DROP_INFO:
			self.infoButton = infoButton
			self.vnum = 0

		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			self.elementImage = elementImage

		self.closeButton = closeButton
		self.nameString = 0
		self.nameLength = 0
		self.vid = 0
		self.eventWhisper = None
		self.isShowButton = FALSE

		if app.ENABLE_MESSENGER_BLOCK:
			self.questionDialog = None

		self.__Initialize()
		self.ResetTargetBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

		print "===================================================== DESTROYED TARGET BOARD"

	def __Initialize(self):
		self.nameString = ""
		self.nameLength = 0
		self.vid = 0
		if app.ENABLE_MOB_DROP_INFO:
			self.vnum = 0
		self.isShowButton = FALSE

	def Destroy(self):
		self.eventWhisper = None
		self.closeButton = None
		self.showingButtonList = None
		self.buttonDict = None
		self.name = None
		self.hpGauge = None
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = None
		if app.ENABLE_MOB_DROP_INFO:
			self.infoButton = None
		if app.ENABLE_ATTENDANCE_EVENT:
			self.hitCount = None
		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			self.elementImage = None
		self.__Initialize()

	if app.ENABLE_MOB_DROP_INFO:
		def RefreshMonsterInfoBoard(self):
			if not self.infoButton.showWnd.IsShow():
				return

			self.infoButton.showWnd.Refresh()

		def OnPressedInfoButton(self):
			net.SendTargetInfoLoad(player.GetTargetVID())
			if self.infoButton.showWnd.IsShow():
				self.infoButton.showWnd.Close()
			elif self.vnum != 0:
				self.infoButton.showWnd.Open(self, self.vnum)

	def OnPressedCloseButton(self):
		player.ClearTarget()
		self.Close()

	def Close(self):
		self.__Initialize()
		if app.ENABLE_MOB_DROP_INFO:
			self.infoButton.showWnd.Close()
		self.Hide()

	def Open(self, vid, name):
		if app.ENABLE_ATTENDANCE_EVENT:
			if self.hitCount:
				self.hitCount.Hide()
				
		if vid:
			if not constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
				if not player.IsSameEmpire(vid):
					self.Hide()
					return

			if vid != self.GetTargetVID():
				self.ResetTargetBoard()
				self.SetTargetVID(vid)
				self.SetTargetName(name)

			if player.IsMainCharacterIndex(vid):
				self.__ShowMainCharacterMenu()
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
				self.Hide()
			else:
				self.RefreshButton()
				self.Show()
		else:
			self.HideAllButton()
			self.__ShowButton(localeInfo.TARGET_BUTTON_WHISPER)
			self.__ShowButton("VOTE_BLOCK_CHAT")
			self.__ArrangeButtonPosition()
			self.SetTargetName(name)
			self.Show()

	def Refresh(self):
		if self.IsShow():
			if self.IsShowButton():
				self.RefreshButton()

	def RefreshByVID(self, vid):
		if vid == self.GetTargetVID():
			self.Refresh()

	def RefreshByName(self, name):
		if name == self.GetTargetName():
			self.Refresh()

	def __ShowMainCharacterMenu(self):
		canShow=0

		self.HideAllButton()

		if player.IsMountingHorse():
			self.__ShowButton(localeInfo.TARGET_BUTTON_DISMOUNT)
			canShow = 1

		if player.IsObserverMode():
			self.__ShowButton(localeInfo.TARGET_BUTTON_EXIT_OBSERVER)
			canShow = 1

		if canShow:
			self.__ArrangeButtonPosition()
			self.Show()
		else:
			self.Hide()

	def __ShowNameOnlyMenu(self):
		self.HideAllButton()

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def UpdatePosition(self):
		self.SetPosition(wndMgr.GetScreenWidth() / 2 - self.GetWidth() / 2, 10)

	def ResetTargetBoard(self):
		for btn in self.buttonDict.values():
			btn.Hide()

		self.__Initialize()

		self.name.SetPosition(0, 13)
		self.name.SetHorizontalAlignCenter()
		self.name.SetWindowHorizontalAlignCenter()
		if app.ENABLE_ATTENDANCE_EVENT:
			if self.hitCount:
				self.hitCount.SetPosition(0, 30)
				self.hitCount.SetHorizontalAlignLeft()
				self.hitCount.SetWindowHorizontalAlignCenter()
		self.hpGauge.Hide()
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal.Hide()
		if app.ENABLE_MOB_DROP_INFO:
			self.infoButton.Hide()
			self.infoButton.showWnd.Close()
		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			self.elementImage.Hide()
		self.SetSize(250, 40)

	def SetTargetVID(self, vid):
		self.vid = vid
		if app.ENABLE_MOB_DROP_INFO:
			self.vnum = 0

	def SetEnemyVID(self, vid):
		self.SetTargetVID(vid)

		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			self.SetRaceElement(vid)

		name = chr.GetNameByVID(vid)
		level = nonplayer.GetLevelByVID(vid)
		grade = nonplayer.GetGradeByVID(vid)
		if app.ENABLE_MOB_DROP_INFO:
			vnum = nonplayer.GetRaceNumByVID(vid)

		nameFront = ""
		if -1 != level:
			nameFront += "|cFF98ff33Lv." + str(level) + " |r"
		if self.GRADE_NAME.has_key(grade):
			nameFront += "(" + self.GRADE_NAME[grade] + ") "

		if grade == 5:
			self.SetTargetName(nameFront + "|cFFffffff"+name+"|r")
		else:
			self.SetTargetName(nameFront + "|cFFeb1609"+name+"|r")

		if app.ENABLE_ATTENDANCE_EVENT:
			countHits = player.GetHitCountByVID(vid)
			if countHits:
				self.hitCount.SetText(localeInfo.TARGET_TOOLTIP_ATTACK_COUNT % countHits)
				self.hitCount.Show()
			else:
				self.hitCount.Hide()
		if app.ENABLE_MOB_DROP_INFO:
			(textWidth, textHeight) = self.name.GetTextSize()

			self.infoButton.SetPosition(textWidth + 25, 9.5)
			self.infoButton.SetWindowHorizontalAlignLeft()

			self.vnum = vnum
			self.infoButton.Show()

	if app.ENABLE_MONSTER_TARGET_ELEMENT:
		def SetRaceElement(self, vid):
			flagList = {
				"ATT_ELEC" : "elect", 
				"ATT_FIRE" : "fire",
				"ATT_ICE" : "ice",
				"ATT_WIND" : "wind",
				"ATT_EARTH" : "earth",
				"ATT_DARK" : "dark"
			}

			vnum = nonplayer.GetRaceNumByVID(vid)
			hasElement = FALSE
			elementName = ""

			if vnum != 0:
				for key, value in flagList.iteritems():
					if nonplayer.MonsterHasRaceFlag(vnum, key):
						hasElement = TRUE
						elementName = key
						break

			if vnum != 0 and hasElement == TRUE:
				self.elementImage.LoadImage("d:/ymir work/ui/game/12zi/element/%s.sub" % (flagList[elementName]))
				self.elementImage.SetPosition(-self.elementImage.GetWidth(), 0)
				self.elementImage.Show()

	def GetTargetVID(self):
		return self.vid

	def GetTargetName(self):
		return self.nameString

	def SetTargetName(self, name):
		self.nameString = name
		self.nameLength = len(name)
		self.name.SetText(name)

	if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
		def SetHP(self, hpPercentage, iMinHP, iMaxHP):
			if not self.hpGauge.IsShow():
				if app.ENABLE_VIEW_TARGET_PLAYER_HP:
					showingButtonCount = len(self.showingButtonList)
					if showingButtonCount > 0:
						if chr.GetInstanceType(self.vid) == chr.INSTANCE_TYPE_PLAYER:
							self.SetSize(max(150 + 75 * 3, showingButtonCount * 75), self.GetHeight())
						else:
							self.SetSize(200 + 7*self.nameLength, self.GetHeight())
					else:
						self.SetSize(200 + 7*self.nameLength, self.GetHeight())
				else:
					self.SetSize(200 + 7*self.nameLength, self.GetHeight())

				self.name.SetPosition(23, 13)

				self.name.SetWindowHorizontalAlignLeft()
				self.name.SetHorizontalAlignLeft()
				self.hpGauge.Show()
				self.UpdatePosition()

			self.hpGauge.SetPercentage(hpPercentage, 100)
			if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
				iMinHPText = '.'.join([i - 3 < 0 and str(iMinHP)[:i] or str(iMinHP)[i-3:i] for i in range(len(str(iMinHP)) % 3, len(str(iMinHP))+1, 3) if i])
				iMaxHPText = '.'.join([i - 3 < 0 and str(iMaxHP)[:i] or str(iMaxHP)[i-3:i] for i in range(len(str(iMaxHP)) % 3, len(str(iMaxHP))+1, 3) if i])
				self.hpDecimal.SetText(str(iMinHPText) + "/" + str(iMaxHPText))
				(textWidth, textHeight)=self.hpDecimal.GetTextSize()
				self.hpDecimal.SetPosition(130 / 2 - textWidth / 2, -13)

				self.hpDecimal.Show()
	else:
		def SetHP(self, hpPercentage):
			if not self.hpGauge.IsShow():
				if app.ENABLE_VIEW_TARGET_PLAYER_HP:
					showingButtonCount = len(self.showingButtonList)
					if showingButtonCount > 0:
						if chr.GetInstanceType(self.GetTargetVID) != chr.INSTANCE_TYPE_PLAYER:
							if showingButtonCount != 1:
								self.SetSize(max(150, showingButtonCount * 75), self.GetHeight())
							else:
								self.SetSize(max(150, 2 * 75), self.GetHeight())
						else:
							self.SetSize(200 + 7*self.nameLength, self.GetHeight())
					else:
						self.SetSize(200 + 7*self.nameLength, self.GetHeight())
				else:
					self.SetSize(200 + 7*self.nameLength, self.GetHeight())

				self.name.SetPosition(23, 13)

				self.name.SetWindowHorizontalAlignLeft()
				self.name.SetHorizontalAlignLeft()
				self.hpGauge.Show()
				self.UpdatePosition()

	def ShowDefaultButton(self):
		self.isShowButton = TRUE
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW])
		for button in self.showingButtonList:
			button.Show()

	def HideAllButton(self):
		self.isShowButton = FALSE
		for button in self.showingButtonList:
			button.Hide()
		self.showingButtonList = []

	def __ShowButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		self.buttonDict[name].Show()
		self.showingButtonList.append(self.buttonDict[name])

	def __HideButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		button = self.buttonDict[name]
		button.Hide()

		for btnInList in self.showingButtonList:
			if btnInList == button:
				self.showingButtonList.remove(button)
				break

	def OnWhisper(self):
		if None != self.eventWhisper:
			self.eventWhisper(self.nameString)

	def OnExchange(self):
		net.SendExchangeStartPacket(self.vid)

	def OnPVP(self):
		net.SendChatPacket("/pvp %d" % (self.vid))

	def OnAppendToMessenger(self):
		net.SendMessengerAddByVIDPacket(self.vid)

	def OnPartyInvite(self):
		net.SendPartyInvitePacket(self.vid)

	def OnPartyExit(self):
		net.SendPartyExitPacket()

	def OnPartyRemove(self):
		net.SendPartyRemovePacket(self.vid)

	def __OnGuildAddMember(self):
		net.SendGuildAddMemberPacket(self.vid)

	def __OnDismount(self):
		net.SendChatPacket("/unmount")

	def __OnExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __OnViewEquipment(self):
		net.SendChatPacket("/view_equip " + str(self.vid))

	def __OnRequestParty(self):
		net.SendChatPacket("/party_request " + str(self.vid))

	def __OnDestroyBuilding(self):
		net.SendChatPacket("/build d %d" % (self.vid))

	def __OnEmotionAllow(self):
		net.SendChatPacket("/emotion_allow %d" % (self.vid))

	def __OnVoteBlockChat(self):
		cmd = "/vote_block_chat %s" % (self.nameString)
		net.SendChatPacket(cmd)

	def OnPressEscapeKey(self):
		self.OnPressedCloseButton()
		return TRUE

	def IsShowButton(self):
		return self.isShowButton

	def RefreshButton(self):
		self.HideAllButton()

		if chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
			return

		if player.IsPVPInstance(self.vid) or player.IsObserverMode():
			# PVP_INFO_SIZE_BUG_FIX
			self.SetSize(200 + 7 * self.nameLength, 40)
			self.UpdatePosition()
			# END_OF_PVP_INFO_SIZE_BUG_FIX
			return

		self.ShowDefaultButton()

		if guild.MainPlayerHasAuthority(guild.AUTH_ADD_MEMBER):
			if not guild.IsMemberByName(self.nameString):
				if 0 == chr.GetGuildID(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_GUILD)

		if not messenger.IsFriendByName(self.nameString):
			self.__ShowButton(localeInfo.TARGET_BUTTON_FRIEND)

		if app.ENABLE_MESSENGER_BLOCK and not str(self.nameString)[0] == "[":
			if not messenger.IsBlockFriendByName(self.nameString):
				self.__ShowButton(localeInfo.TARGET_BUTTON_BLOCK)
			else:
				self.__ShowButton(localeInfo.TARGET_BUTTON_UNBLOCK)

		if player.IsPartyMember(self.vid):

			self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if player.IsPartyLeader(self.vid):
				self.__ShowButton(localeInfo.TARGET_BUTTON_LEAVE_PARTY)
			elif player.IsPartyLeader(player.GetMainCharacterIndex()):
				self.__ShowButton(localeInfo.TARGET_BUTTON_EXCLUDE)

		else:
			if player.IsPartyMember(player.GetMainCharacterIndex()):
				if player.IsPartyLeader(player.GetMainCharacterIndex()):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
			else:
				if chr.IsPartyMember(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY)
				else:
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)

			if player.IsRevengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_AVENGE)
			elif player.IsChallengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_ACCEPT_FIGHT)
			elif player.IsCantFightInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if not player.IsSameEmpire(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_INVITE_GUILD)
				self.__HideButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
				self.__HideButton(localeInfo.TARGET_BUTTON_FRIEND)
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

		distance = player.GetCharacterDistance(self.vid)
		if distance > self.EXCHANGE_LIMIT_RANGE:
			self.__HideButton(localeInfo.TARGET_BUTTON_EXCHANGE)
			self.__ArrangeButtonPosition()

		self.__ArrangeButtonPosition()

	def __ArrangeButtonPosition(self):
		showingButtonCount = len(self.showingButtonList)

		pos = -(showingButtonCount / 2) * 68
		if 0 == showingButtonCount % 2:
			pos += 34

		for button in self.showingButtonList:
			button.SetPosition(pos, 33)
			pos += 68

		if app.ENABLE_VIEW_TARGET_PLAYER_HP:
			if showingButtonCount <= 2:
				self.SetSize(max(150 + 125, showingButtonCount * 75), 65)
			else:
				self.SetSize(max(150, showingButtonCount * 75), 65)
		else:
			self.SetSize(max(150, showingButtonCount * 75), 65)

		self.UpdatePosition()

	if app.ENABLE_ATTENDANCE_EVENT:
		def RefreshHitCount(self, vid):
			countHits = player.GetHitCountByVID(vid)
			if countHits:
				self.hitCount.SetText(localeInfo.TARGET_TOOLTIP_ATTACK_COUNT % countHits)
				self.hitCount.Show()
			else:
				self.hitCount.Hide()

	def OnUpdate(self):
		if self.isShowButton:

			exchangeButton = self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE]
			distance = player.GetCharacterDistance(self.vid)

			if distance < 0:
				return

			if exchangeButton.IsShow():
				if distance > self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

			else:
				if distance < self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

	if app.ENABLE_MESSENGER_BLOCK:
		def __OnBlock(self):
			net.SendMessengerBlockAddByVIDPacket(self.vid)

		def __OnBlockRemove(self):
			self.questionDialog = uiCommon.QuestionDialog("thin")
			self.questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnBlockRemove))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnBlockRemoveClose))
			self.questionDialog.Open()

		def OnBlockRemove(self):
			net.SendMessengerBlockRemoveByVIDPacket(self.vid)
			self.OnBlockRemoveClose()

		def OnBlockRemoveClose(self):
			self.questionDialog.Close()
			self.questionDialog = None
			return TRUE

	if app.ENABLE_MOB_DROP_INFO:
		class InfoBoard(ui.ThinBoard):
			STONE_START_VNUM = 28030
			STONE_LAST_VNUM = 28042

			BOARD_WIDTH = 16*32+20

			def __init__(self):
				ui.ThinBoard.__init__(self)

				self.race = 0
				self.hasItems = FALSE

				self.itemTooltip = uiToolTip.ItemToolTip()
				self.itemTooltip.HideToolTip()

				wndItem = ui.GridSlotWindow()
				wndItem.SetParent(self)
				wndItem.ArrangeSlot(0, 16, 5, 32, 32, 0, 0)
				wndItem.RefreshSlot()
				wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
				wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
				wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
				wndItem.Hide()
				self.wndItem = wndItem
				self.itemArray = [0 for i in xrange(80)]
				self.itemVnums = [0 for i in xrange(80)]

				self.SetSize(self.BOARD_WIDTH, 0)

			def __del__(self):
				ui.ThinBoard.__del__(self)

			def __UpdatePosition(self, targetBoard):
				self.SetPosition(targetBoard.GetLeft() + (targetBoard.GetWidth() - self.GetWidth()) / 2, targetBoard.GetBottom() + 5)

			def Open(self, targetBoard, race):
				self.__LoadInformation(race)

				self.SetSize(self.BOARD_WIDTH, self.yPos + 10)
				self.__UpdatePosition(targetBoard)

				self.Show()

			def Refresh(self):
				self.__LoadInformation(self.race)
				self.SetSize(self.BOARD_WIDTH, self.yPos + 10)

			def Close(self):
				self.itemTooltip.HideToolTip()
				self.Hide()

			def __LoadInformation(self, race):
				self.yPos = 7
				self.children = []
				self.race = race

				for i in xrange(80):
					self.wndItem.ClearSlot(i)

				self.itemArray = [0 for i in xrange(80)]
				self.itemVnums = [0 for i in xrange(80)]

				self.__LoadInformation_Drops(race)

			def SerachEmptySlot(self, size):
				for value in xrange(80):
					if 0 == self.itemArray[value]:
						if 1 == size:
							return value

						emptySlotIndex	= value
						searchSucceed	= TRUE

						for i in range(size - 1):
							emptySlotIndex = emptySlotIndex + 16

							if emptySlotIndex >= 80:
								searchSucceed = FALSE
								continue

							if 1 == self.itemArray[emptySlotIndex]:
								searchSucceed = FALSE

						if TRUE == searchSucceed:
							return value

				return -1

			def __LoadInformation_Drops(self, race):
				self.AppendSeperator()

				if race in MONSTER_INFO_DATA:
					if len(MONSTER_INFO_DATA[race]["items"]) == 0:
						self.wndItem.Hide()
					else:
						self.wndItem.SetPosition(10, self.yPos - 7)
						self.yPos += 32*5
						self.SetSize(self.BOARD_WIDTH, self.yPos + 5)
						self.UpdateRect()
						self.wndItem.Show()

						for curItem in MONSTER_INFO_DATA[race]["items"]:
							getItemID = 0

							if curItem.has_key("vnum_list"):
								getItemID = curItem["vnum_list"][0]
							else:
								getItemID = curItem["vnum"]

							getItemCount=curItem["count"]
							item.SelectItem(getItemID)
							itemSize = item.GetItemSize()

							emptySlotPos = self.SerachEmptySlot(itemSize[1])

							if -1 != emptySlotPos:
								self.itemArray[emptySlotPos] = 1

								if itemSize[1] == 2:
									self.itemArray[emptySlotPos + 16] = 1
								elif itemSize[1] == 3:
									self.itemArray[emptySlotPos + 16] = 1
									self.itemArray[emptySlotPos + 32] = 1

								self.wndItem.SetItemSlot(emptySlotPos, getItemID, getItemCount)
								self.itemVnums[emptySlotPos] = getItemID

						self.wndItem.RefreshSlot()
				else:
					self.wndItem.Hide()

			def OverInItem(self, slotIndex):
				vnum = self.itemVnums[slotIndex]
				if vnum != 0:
					self.OnShowItemTooltip(vnum)

			def OverOutItem(self):
				self.OnHideItemTooltip()

			def AppendTextLine(self, text):
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.SetText(text)
				textLine.SetPosition(0, self.yPos)
				textLine.Show()

				self.children.append(textLine)
				self.yPos += 17

			def AppendSeperator(self):
				img = ui.ImageBox()
				self.AppendWindow(img)
				img.SetPosition(img.GetLeft(), img.GetTop() - 15)
				self.yPos += 15

			def OnShowItemTooltip(self, vnum):
				item.SelectItem(vnum)

				self.itemTooltip.isBook = TRUE
				self.itemTooltip.isBook2 = TRUE
				self.itemTooltip.SetItemToolTip(vnum)

			def OnHideItemTooltip(self):
				self.itemTooltip.HideToolTip()

			def AppendWindow(self, wnd, x = 0, width = 0, height = 0):
				if width == 0:
					width = wnd.GetWidth()
				if height == 0:
					height = wnd.GetHeight()

				wnd.SetParent(self)
				if x == 0:
					wnd.SetPosition((self.GetWidth() - width) / 2, self.yPos)
				else:
					wnd.SetPosition(x, self.yPos)
				wnd.Show()

				self.children.append(wnd)
				self.yPos += height + 5
