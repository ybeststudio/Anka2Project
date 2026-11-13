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

if app.ENABLE_DAMAGE_TRACE:
	import uiTrackDamage

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

	# Baerim target board, measured pixel by pixel on Baerim screenshots (constants from its uiTarget dump):
	# 40 px high, name at 23, "?" 7 px after the name, "W" 3 px after "?", HP gauge 15 px after the buttons,
	# gauge 175 px and close button 30 px from the right edge
	BOARD_TOP = 10
	BOARD_MIN_WIDTH = 250
	BOARD_SIDE_PADDING = 23
	NAME_Y = 14
	NAME_BUTTON_GAP = 7
	INFO_BUTTON_GAP = 3
	INFO_BUTTON_Y = 14
	WIKI_BUTTON_Y = 15
	BUTTON_GAUGE_GAP = 15
	GAUGE_RIGHT = 175
	GAUGE_Y = 22
	CLOSE_RIGHT = 30
	CLOSE_PATH = "d:/ymir work/ui/game/delete_sell/btn_quit_%s.png"

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
		closeButton.SetUpVisual(self.CLOSE_PATH % "norm")
		closeButton.SetOverVisual(self.CLOSE_PATH % "hover")
		closeButton.SetDownVisual(self.CLOSE_PATH % "select")
		closeButton.SetPosition(self.CLOSE_RIGHT, 9)

		hpGauge.SetPosition(self.GAUGE_RIGHT, self.GAUGE_Y)
		hpGauge.SetWindowHorizontalAlignRight()

		closeButton.SetWindowHorizontalAlignRight()
		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()

		if app.ENABLE_MOB_DROP_INFO:
			infoButton = ui.Button()
			infoButton.SetParent(self)
			infoButton.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")	# Baerim "?" button
			infoButton.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
			infoButton.SetDownVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
			infoButton.SetEvent(ui.__mem_func__(self.OnPressedInfoButton))
			infoButton.Hide()

			infoBoard = self.InfoBoard()
			infoBoard.Hide()
			infoButton.showWnd = infoBoard

		if app.ENABLE_WIKI_SYSTEM:
			# Baerim "W" button: opens the monster's wiki page
			wikiButton = ui.Button()
			wikiButton.SetParent(self)
			wikiButton.SetUpVisual("d:/ymir work/ui/pattern/wikipediainfo_btn.png")
			wikiButton.SetOverVisual("d:/ymir work/ui/pattern/wikipediainfo_btn_hover.png")
			wikiButton.SetDownVisual("d:/ymir work/ui/pattern/wikipediainfo_btn_pressed.png")
			wikiButton.SetEvent(ui.__mem_func__(self.__OnWiki))
			wikiButton.Hide()
			self.wikiButton = wikiButton
			self.eventWiki = None

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
		if app.ENABLE_DAMAGE_TRACE:
			self.damageDetails = None
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
		if app.ENABLE_DAMAGE_TRACE:
			if self.damageDetails:
				self.damageDetails.Destroy()
			self.damageDetails = None
		if app.ENABLE_WIKI_SYSTEM:
			self.wikiButton = None
			self.eventWiki = None
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
			# drop chances depend on the player's level: drop the cached list and ask the server again
			MONSTER_INFO_DATA.pop(self.vnum, None)
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
		if app.ENABLE_DAMAGE_TRACE:
			self.CloseDamageDetails()
		self.Hide()

	if app.ENABLE_DAMAGE_TRACE:
		# Baerim "Hasar Takibi": opened from the "?" info board, follows the current target
		def ToggleDamageDetails(self):
			if self.damageDetails and self.damageDetails.IsShow():
				self.CloseDamageDetails()
				return
			if not self.vid:
				return
			if not self.damageDetails:
				self.damageDetails = uiTrackDamage.DamageDetails()
			anchor = self.infoButton.showWnd if app.ENABLE_MOB_DROP_INFO else self
			self.damageDetails.Open(self.vid, anchor)

		def CloseDamageDetails(self):
			if self.damageDetails and self.damageDetails.IsShow():
				self.damageDetails.Close()

		def OnDamageTraceBegin(self, request, vid, hitType, count):
			if self.damageDetails:
				self.damageDetails.Begin(request, vid, hitType, count)

		def OnDamageTraceRow(self, request, index, key, before, after):
			if self.damageDetails:
				self.damageDetails.Row(request, index, key, before, after)

		def OnDamageTraceEnd(self, request, displayed, hpLoss):
			if self.damageDetails:
				self.damageDetails.End(request, displayed, hpLoss)

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

	if app.ENABLE_WIKI_SYSTEM:
		def SetWikiEvent(self, event):
			self.eventWiki = event

		def __OnWiki(self):
			vnum = nonplayer.GetRaceNumByVID(self.vid)
			if self.eventWiki and vnum:
				self.eventWiki(vnum)

	def UpdatePosition(self):
		self.SetPosition(wndMgr.GetScreenWidth() / 2 - self.GetWidth() / 2, self.BOARD_TOP)

	def __ApplyHealthLayout(self):
		# width from the real text size (the old 200 + 7 * len(name) counted colour codes and flag tags as letters)
		(nameWidth, nameHeight) = self.name.GetTextSize()
		x = self.BOARD_SIDE_PADDING + nameWidth
		gap = self.NAME_BUTTON_GAP
		if app.ENABLE_MOB_DROP_INFO and self.infoButton.IsShow():
			x += gap
			self.infoButton.SetWindowHorizontalAlignLeft()
			self.infoButton.SetPosition(x, self.INFO_BUTTON_Y)
			x += self.infoButton.GetWidth()
			gap = self.INFO_BUTTON_GAP
		if app.ENABLE_WIKI_SYSTEM and self.wikiButton.IsShow():
			x += gap
			self.wikiButton.SetPosition(x, self.WIKI_BUTTON_Y)
			x += self.wikiButton.GetWidth()
		width = max(self.BOARD_MIN_WIDTH, x + self.BUTTON_GAUGE_GAP + self.GAUGE_RIGHT)
		if app.ENABLE_VIEW_TARGET_PLAYER_HP and self.showingButtonList and chr.GetInstanceType(self.vid) == chr.INSTANCE_TYPE_PLAYER:
			width = max(width, len(self.showingButtonList) * 75)
		self.SetSize(width, self.GetHeight())
		self.name.SetWindowHorizontalAlignLeft()
		self.name.SetHorizontalAlignLeft()
		self.name.SetPosition(self.BOARD_SIDE_PADDING, self.NAME_Y)
		self.closeButton.SetPosition(self.CLOSE_RIGHT, 9)

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
		if app.ENABLE_WIKI_SYSTEM:
			self.wikiButton.Hide()
		if app.ENABLE_MONSTER_TARGET_ELEMENT:
			self.elementImage.Hide()
		self.SetSize(250, 40)

	def SetTargetVID(self, vid):
		if app.ENABLE_DAMAGE_TRACE and vid != self.vid:
			self.CloseDamageDetails()
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

		# Baerim: "Lv.4 (1.Seviye) Name" in the plain text colour
		nameFront = ""
		if -1 != level:
			nameFront += "Lv." + str(level) + " "
		if self.GRADE_NAME.has_key(grade):
			nameFront += "(" + self.GRADE_NAME[grade] + ") "
		self.SetTargetName(nameFront + name)

		if app.ENABLE_ATTENDANCE_EVENT:
			countHits = player.GetHitCountByVID(vid)
			if countHits:
				self.hitCount.SetText(localeInfo.TARGET_TOOLTIP_ATTACK_COUNT % countHits)
				self.hitCount.Show()
			else:
				self.hitCount.Hide()
		if app.ENABLE_MOB_DROP_INFO:
			self.vnum = vnum
			self.infoButton.Show()
		if app.ENABLE_WIKI_SYSTEM and nonplayer.GetRaceNumByVID(vid):
			self.wikiButton.Show()
		if app.ENABLE_MOB_DROP_INFO or app.ENABLE_WIKI_SYSTEM:
			if self.hpGauge.IsShow():
				self.__ApplyHealthLayout()
				self.UpdatePosition()

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
				self.hpGauge.Show()
				self.__ApplyHealthLayout()
				self.UpdatePosition()

			self.hpGauge.SetPercentage(hpPercentage, 100)
			if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
				iMinHPText = '.'.join([i - 3 < 0 and str(iMinHP)[:i] or str(iMinHP)[i-3:i] for i in range(len(str(iMinHP)) % 3, len(str(iMinHP))+1, 3) if i])
				iMaxHPText = '.'.join([i - 3 < 0 and str(iMaxHP)[:i] or str(iMaxHP)[i-3:i] for i in range(len(str(iMaxHP)) % 3, len(str(iMaxHP))+1, 3) if i])
				self.hpDecimal.SetText("%s/%s (%d%%)" % (iMinHPText, iMaxHPText, hpPercentage))	# Baerim: "145/145 (100%)"
				(textWidth, textHeight)=self.hpDecimal.GetTextSize()
				self.hpDecimal.SetPosition(130 / 2 - textWidth / 2, -14)

				self.hpDecimal.Show()
	else:
		def SetHP(self, hpPercentage):
			if not self.hpGauge.IsShow():
				self.hpGauge.Show()
				self.__ApplyHealthLayout()
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
			self.SetSize(self.BOARD_MIN_WIDTH, 40)
			if self.hpGauge.IsShow():
				self.__ApplyHealthLayout()
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
			# Baerim target "?" board (uiTarget.InfoBoard): mob / metin stats, the player's bonuses against the target,
			# level warning and the drop list (icon, name, rarity from the real drop chance), 5 rows + scrollbar
			BOARD_MIN_WIDTH = 250
			TOP_PADDING = 12
			BOTTOM_PADDING = 10
			TEXT_LINE_HEIGHT = 17			# not LINE_HEIGHT: that name is ThinBoard's frame image size (16)
			BOARD_TEXT_PADDING = 42			# Baerim: the board widens to the longest text + 42
			MAX_ITEM_COUNT = 5
			ROW_HEIGHT = 37
			ROW_X = 17
			NAME_X = 38					# name / rarity start this far right of the icon
			SEPARATOR = "d:/ymir work/ui/pattern/seperator.tga"
			VALUE_COLOR = 0xffb4d684		# Baerim bonus value green (most saturated glyph pixels of a Baerim screenshot)
			CAP_COLOR = 0xffff5a4a
			NAME_COLOR = 0xffe6e6e6

			# player point ids (Packet.h EPoints)
			POINT_CRITICAL_PCT = 40
			POINT_PENETRATE_PCT = 41
			POINT_ATTBONUS_MONSTER = 53
			POINT_ATTBONUS_STONE = 155
			POINT_ATTBONUS_BOSS = 156
			RACE_FLAG_BONUS = (
				("HUMAN", 43, "TARGET_INFO_BONUS_HUMAN"),
				("ANIMAL", 44, "TARGET_INFO_BONUS_ANIMAL"),
				("ORC", 45, "TARGET_INFO_BONUS_ORC"),
				("MILGYO", 46, "TARGET_INFO_BONUS_MILGYO"),
				("UNDEAD", 47, "TARGET_INFO_BONUS_UNDEAD"),
				("DEVIL", 48, "TARGET_INFO_BONUS_DEVIL"),
				("INSECT", 49, "TARGET_INFO_BONUS_INSECT"),
				("FIRE", 50, "TARGET_INFO_BONUS_FIRE"),
				("ICE", 51, "TARGET_INFO_BONUS_ICE"),
				("DESERT", 52, "TARGET_INFO_BONUS_DESERT"),
				("TREE", 58, "TARGET_INFO_BONUS_TREE"),
			)
			RACE_FLAG_BITS = {"ANIMAL": 1 << 0, "UNDEAD": 1 << 1, "DEVIL": 1 << 2, "HUMAN": 1 << 3, "ORC": 1 << 4, "MILGYO": 1 << 5,
				"INSECT": 1 << 6, "FIRE": 1 << 7, "ICE": 1 << 8, "DESERT": 1 << 9, "TREE": 1 << 10}

			# rarity by drop chance in 1/1,000,000: (minimum, locale key, colour)
			RARITY_TABLE = (
				(1000000, "DROP_GUARANTEED", 0xffffa64d),
				(200000, "DROP_COMMON", 0xffc8c8c8),
				(50000, "DROP_UNCOMMON", 0xff8fd16a),
				(10000, "DROP_RARE", 0xff4fa3ff),
				(1000, "DROP_VERY_RARE", 0xffb565ff),
				(1, "DROP_LEGENDARY", 0xffffcc33),
			)

			def __init__(self):
				ui.ThinBoard.__init__(self)
				self.race = 0
				self.targetBoard = None
				self.children = []
				self.rows = []
				self.drops = []
				self.scrollPos = 0
				self.listTop = 0
				self.yPos = 0

				self.itemTooltip = uiToolTip.ItemToolTip()
				self.itemTooltip.HideToolTip()

				scrollBar = ui.ScrollBar()
				scrollBar.SetParent(self)
				scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
				scrollBar.Hide()
				self.scrollBar = scrollBar

				self.SetSize(self.BOARD_MIN_WIDTH, 0)
				self.SetOnRunMouseWheelEvent(ui.__mem_func__(self.__OnWheel))

			def __del__(self):
				ui.ThinBoard.__del__(self)

			def __UpdatePosition(self, targetBoard):
				# Baerim: 250 px wide, centred under the target board and joined to it: the top frame lines sit under
				# the target board's bottom frame lines (2 px overlap), the target board stays on top
				self.SetPosition(targetBoard.GetLeft() + (targetBoard.GetWidth() - self.GetWidth()) / 2, targetBoard.GetBottom() - 2)
				if self.IsShow():
					targetBoard.SetTop()

			def Open(self, targetBoard, race):
				self.targetBoard = targetBoard
				self.__LoadInformation(race)
				self.Show()
				self.__UpdatePosition(targetBoard)

			def Refresh(self):
				if self.targetBoard:
					self.__LoadInformation(self.race)
					self.__UpdatePosition(self.targetBoard)

			def Close(self):
				self.itemTooltip.HideToolTip()
				self.Hide()

			# ---------------------------------------------------------------- build
			def __LoadInformation(self, race):
				# built twice when a text is wider than the board: the second pass uses the widened board
				self.boardWidth = self.BOARD_MIN_WIDTH
				self.maxTextWidth = 0
				self.__Build(race)
				if self.maxTextWidth + self.BOARD_TEXT_PADDING > self.boardWidth:
					self.boardWidth = self.maxTextWidth + self.BOARD_TEXT_PADDING
					self.__Build(race)

			def __Build(self, race):
				self.race = race
				self.children = []
				self.rows = []
				self.yPos = self.TOP_PADDING
				width = self.boardWidth
				self.SetSize(width, 100)

				(defense, regenCycle, regenPercent, critical, penetrate, maxHP, level, isStone, raceFlag) = nonplayer.GetMonsterTargetInfo(race)
				self.isStone = isStone

				self.AppendTextLine(localeInfo.TARGET_INFO_DEF % defense)
				if isStone:
					self.AppendTextLine(localeInfo.TARGET_INFO_DEF_DESC)
					self.AppendTextLine(localeInfo.TARGET_INFO_DEF_DESC2)
					if maxHP > 0:	# server ENABLE_METIN_DAMAGE_CAP: one hit deals at most max HP / 10
						self.AppendTextLine(localeInfo.TARGET_INFO_METIN_DMG_CAP % self.__FormatNumber(max(1, maxHP / 10)), self.CAP_COLOR)
				else:
					self.AppendTextLine(localeInfo.TARGET_INFO_CRITICAL % critical)
					self.AppendTextLine(localeInfo.TARGET_INFO_PENETRATE % penetrate)
					if regenPercent > 0 and regenCycle > 0:
						self.AppendTextLine(localeInfo.TARGET_INFO_REGEN_INFO % (regenCycle, regenPercent))

				self.AppendSeperator()
				self.__AppendTargetBonuses(isStone, raceFlag, nonplayer.GetGradeByVID(player.GetTargetVID()) if player.GetTargetVID() else 0)
				if app.ENABLE_DAMAGE_TRACE:
					self.__AppendDamageButton()
				self.AppendSeperator()
				self.__AppendDrops(race, level, isStone)

				self.SetSize(width, self.yPos + self.BOTTOM_PADDING)

			def __AppendBonusLine(self, label, point):
				self.__AppendBonusValue(label, player.GetStatus(point))

			def __AppendBonusValue(self, label, value):
				# label and "(N%)" are separate text lines: a colour tag right before digits is misread (digits look like hex)
				labelLine = ui.TextLine()
				labelLine.SetParent(self)
				labelLine.SetText(label + " ")
				labelLine.SetOutline()
				valueLine = ui.TextLine()
				valueLine.SetParent(self)
				valueLine.SetText("(%d%%)" % value)
				valueLine.SetPackedFontColor(self.VALUE_COLOR)
				valueLine.SetOutline()
				(labelWidth, h) = labelLine.GetTextSize()
				(valueWidth, h) = valueLine.GetTextSize()
				self.maxTextWidth = max(self.maxTextWidth, labelWidth + valueWidth)
				x = (self.GetWidth() - labelWidth - valueWidth) / 2
				labelLine.SetPosition(x, self.yPos)
				valueLine.SetPosition(x + labelWidth, self.yPos)
				labelLine.Show()
				valueLine.Show()
				self.children.extend((labelLine, valueLine))
				self.yPos += self.TEXT_LINE_HEIGHT

			def __AppendTargetBonuses(self, isStone, raceFlag, grade):
				if isStone:
					self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_STONE, self.POINT_ATTBONUS_STONE)
					self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_MONSTER, self.POINT_ATTBONUS_MONSTER)
					self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_CRITICAL, self.POINT_CRITICAL_PCT)
					self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_PENETRATE, self.POINT_PENETRATE_PCT)
					# Baerim also lists critical / piercing hit power; no item gives these here, so they are always 0
					self.__AppendBonusValue(localeInfo.TARGET_INFO_BONUS_CRITICAL_POWER, 0)
					self.__AppendBonusValue(localeInfo.TARGET_INFO_BONUS_PENETRATE_POWER, 0)
					return

				self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_MONSTER, self.POINT_ATTBONUS_MONSTER)
				if grade >= nonplayer.BOSS:
					self.__AppendBonusLine(localeInfo.TARGET_INFO_BONUS_BOSS, self.POINT_ATTBONUS_BOSS)
				for (flagName, point, key) in self.RACE_FLAG_BONUS:
					if raceFlag & self.RACE_FLAG_BITS[flagName]:
						self.__AppendBonusLine(getattr(localeInfo, key), point)

			def __AppendDamageButton(self):
				button = ui.Button()
				button.SetParent(self)
				button.SetUpVisual("d:/ymir work/ui/game/common/button_large_normal.png")
				button.SetOverVisual("d:/ymir work/ui/game/common/button_large_hover.png")
				button.SetDownVisual("d:/ymir work/ui/game/common/button_large_down.png")
				button.SetWindowHorizontalAlignCenter()
				button.SetText(localeInfo.TRACK_DAMAGE_BUTTON)
				button.SetPosition(0, self.yPos + 4)
				button.SetEvent(ui.__mem_func__(self.__OnDamageButton))
				button.Show()
				self.children.append(button)
				self.yPos += button.GetHeight() + 14

			def __OnDamageButton(self):
				if self.targetBoard:
					self.targetBoard.ToggleDamageDetails()

			def __GetRarity(self, chance):
				if chance <= 0:
					return None
				for (minimum, key, color) in self.RARITY_TABLE:
					if chance >= minimum:
						return (getattr(localeInfo, key), color)
				return None

			def __AppendDrops(self, race, mobLevel, isStone):
				# level warning: the server lowers the drop rate once the player is 2+ levels above the target
				if player.GetStatus(player.LEVEL) >= mobLevel + 2:
					self.AppendTextLine(localeInfo.DROP_LEVEL_WARNING_METIN if isStone else localeInfo.DROP_LEVEL_WARNING_MONSTER)
					self.AppendTextLine(localeInfo.DROP_LEVEL_WARNING_GUARANTEE)
					self.yPos += 6

				drops = []
				if race in MONSTER_INFO_DATA:
					for curItem in MONSTER_INFO_DATA[race]["items"]:
						vnum = curItem["vnum_list"][0] if curItem.has_key("vnum_list") else curItem["vnum"]
						drops.append((curItem.get("chance", 0), vnum, curItem["count"]))
				# rarest first, unknown chances last
				drops.sort(key = lambda d: (d[0] <= 0, d[0]))
				self.drops = drops

				if not drops:
					self.AppendTextLine(localeInfo.TARGET_INFO_NO_ITEM_TEXT)
					self.scrollBar.Hide()
					return

				self.listTop = self.yPos
				visible = min(len(drops), self.MAX_ITEM_COUNT)
				for i in xrange(visible):
					self.rows.append(self.__MakeRow(i))
				self.yPos += visible * self.ROW_HEIGHT

				self.scrollPos = 0
				if len(drops) > self.MAX_ITEM_COUNT:
					self.scrollBar.SetPosition(self.GetWidth() - 28, self.listTop)
					self.scrollBar.SetScrollBarSize(visible * self.ROW_HEIGHT)
					self.scrollBar.SetMiddleBarSize(float(self.MAX_ITEM_COUNT) / float(len(drops)))
					self.scrollBar.SetPos(0.0)
					self.scrollBar.Show()
				else:
					self.scrollBar.Hide()
				self.__RefreshRows()

			def __MakeRow(self, index):
				y = self.listTop + index * self.ROW_HEIGHT
				icon = ui.ExpandedImageBox()
				icon.SetParent(self)
				icon.SetPosition(self.ROW_X, y + 2)
				icon.SetStringEvent("MOUSE_OVER_IN", ui.__mem_func__(self.__OnRowOver), index)
				icon.SetStringEvent("MOUSE_OVER_OUT", ui.__mem_func__(self.OnHideItemTooltip))
				icon.Show()
				name = ui.TextLine()
				name.SetParent(self)
				name.SetPosition(self.ROW_X + self.NAME_X, y + 3)
				name.SetPackedFontColor(self.NAME_COLOR)
				name.SetOutline()
				name.Show()
				rarity = ui.TextLine()
				rarity.SetParent(self)
				rarity.SetPosition(self.ROW_X + self.NAME_X, y + 17)
				rarity.SetOutline()
				rarity.Show()
				self.children.extend((icon, name, rarity))
				return (icon, name, rarity)

			def __RefreshRows(self):
				for (index, (icon, name, rarity)) in enumerate(self.rows):
					dropIndex = self.scrollPos + index
					if dropIndex >= len(self.drops):
						icon.Hide()
						name.Hide()
						rarity.Hide()
						continue
					(chance, vnum, count) = self.drops[dropIndex]
					item.SelectItem(vnum)
					icon.LoadImage(item.GetIconImageFileName())
					(w, h) = (icon.GetWidth(), icon.GetHeight())
					if h > 32:		# tall icons (weapons, armour) are shown 32 px high like Baerim
						icon.SetScale(32.0 / h, 32.0 / h)
					else:
						icon.SetScale(1.0, 1.0)
					icon.Show()
					label = item.GetItemName()
					if count > 1:
						label = "%s (%d x)" % (label, count)
					name.SetText(label)
					name.Show()
					# the name must fit between the icon and the scrollbar
					self.maxTextWidth = max(self.maxTextWidth, self.ROW_X + self.NAME_X + name.GetTextSize()[0] + 30 - self.BOARD_TEXT_PADDING)
					rarityInfo = self.__GetRarity(chance)
					if rarityInfo:
						rarity.SetText(rarityInfo[0])
						rarity.SetPackedFontColor(rarityInfo[1])
						rarity.Show()
					else:
						rarity.Hide()

			def __OnScroll(self):
				maxPos = max(0, len(self.drops) - self.MAX_ITEM_COUNT)
				self.scrollPos = int(round(self.scrollBar.GetPos() * maxPos))
				self.__RefreshRows()

			def __OnWheel(self, isDown):
				if not self.scrollBar.IsShow():
					return
				maxPos = max(1, len(self.drops) - self.MAX_ITEM_COUNT)
				step = 1.0 / maxPos
				pos = self.scrollBar.GetPos() + (step if isDown else -step)
				self.scrollBar.SetPos(max(0.0, min(1.0, pos)))

			def __OnRowOver(self, index):
				dropIndex = self.scrollPos + index
				if dropIndex < len(self.drops):
					self.OnShowItemTooltip(self.drops[dropIndex][1])

			# ---------------------------------------------------------------- helpers
			def __FormatNumber(self, value):
				text = str(int(value))
				parts = []
				while len(text) > 3:
					parts.insert(0, text[-3:])
					text = text[:-3]
				parts.insert(0, text)
				return ".".join(parts)

			def AppendTextLine(self, text, color = None):
				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.SetText(text)
				if color:
					textLine.SetPackedFontColor(color)
				textLine.SetOutline()
				textLine.SetPosition(0, self.yPos)
				textLine.Show()
				self.maxTextWidth = max(self.maxTextWidth, textLine.GetTextSize()[0])
				self.children.append(textLine)
				self.yPos += self.TEXT_LINE_HEIGHT

			def AppendSeperator(self):
				img = ui.ImageBox()
				img.SetParent(self)
				img.LoadImage(self.SEPARATOR)
				img.SetWindowHorizontalAlignCenter()
				img.SetPosition(0, self.yPos - img.GetHeight() / 2)	# Baerim: the line sits one text line below the last text
				img.AddFlag("not_pick")
				img.Show()
				self.children.append(img)
				self.yPos += 22

			def OnShowItemTooltip(self, vnum):
				item.SelectItem(vnum)
				self.itemTooltip.isBook = TRUE
				self.itemTooltip.isBook2 = TRUE
				self.itemTooltip.SetItemToolTip(vnum)

			def OnHideItemTooltip(self):
				self.itemTooltip.HideToolTip()
