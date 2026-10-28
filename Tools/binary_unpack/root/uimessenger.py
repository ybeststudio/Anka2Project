if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import grp
import guild
import messenger
import localeInfo
import constInfo
import uiToolTip
import uiGameOption

import uiCommon
from _weakref import proxy

FRIEND = 0
GUILD = 1

if app.ENABLE_MESSENGER_BLOCK:
	BLOCK_FRIEND = 2

class MessengerItem(ui.Window):

	def __init__(self, getParentEvent):
		ui.Window.__init__(self)

		self.SetParent(getParentEvent())
		self.AddFlag("float")

		self.name = ""
		self.image = ui.ImageBox()
		self.image.AddFlag("not_pick")
		self.image.SetParent(self)
		self.image.Show()
		self.text = ui.TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(20, 2)
		self.text.Show()

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.LanguageID = 0
			self.secondLanguageID = 0

			self.LanguageFlag = ui.ImageBox()
			self.LanguageFlag.AddFlag("not_pick")
			self.LanguageFlag.SetParent(self)
			self.LanguageFlag.SetPosition(-13, 2)
			self.LanguageFlag.Hide()

			self.secondLanguageFlag = ui.ImageBox()
			self.secondLanguageFlag.AddFlag("not_pick")
			self.secondLanguageFlag.SetParent(self)
			self.secondLanguageFlag.SetPosition(7, 2)
			self.secondLanguageFlag.Hide()

		self.lovePoint = -1
		self.lovePointToolTip = None

		self.isSelected = FALSE

		self.getParentEvent = getParentEvent

	def SetName(self, name):
		self.name = name
		if name:
			self.text.SetText(name)
			new_x = 0

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if self.secondLanguageFlag.IsShow():
					new_x += 20

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.SetSize(20 + 6*len(name) + 4 + new_x, 16)
			else:
				self.SetSize(20 + 6*len(name) + 4, 16)

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def SetLang(self, LanguageID, secondLanguageID):
			self.LanguageID = LanguageID
			self.secondLanguageID = secondLanguageID

			if LanguageID != 0:
				self.LanguageFlag.LoadImage("d:/ymir work/ui/game/emoji/flag/%s.tga" % constInfo.GetLang(LanguageID))
				self.LanguageFlag.Show()
				self.image.SetPosition(7, -2)
				self.text.SetPosition(26, 1)

				if secondLanguageID != 0 and secondLanguageID < app.LOCALE_MAX_NUM + 1:
					self.secondLanguageFlag.LoadImage("d:/ymir work/ui/game/emoji/flag/%s.tga" % constInfo.GetLang(secondLanguageID))
					self.secondLanguageFlag.Show()
					self.image.SetPosition(28, -2)
					self.text.SetPosition(46, 1)
				else:
					self.secondLanguageFlag.Hide()
			else:
				self.LanguageFlag.Hide()

	def SetLovePoint(self, lovePoint):
		self.lovePoint = lovePoint

	def Select(self):
		self.isSelected = TRUE

	def UnSelect(self):
		self.isSelected = FALSE

	def GetName(self):
		return self.name

	def GetStepWidth(self):
		return 0

	# Whisper
	def CanWhisper(self):
		return FALSE

	def IsOnline(self):
		return FALSE

	def OnWhisper(self):
		pass

	# Remove
	def CanRemove(self):
		return FALSE

	def OnRemove(self):
		return FALSE

	# Warp
	def CanWarp(self):
		return FALSE

	def OnWarp(self):
		pass

	def OnMouseOverIn(self):
		if -1 != self.lovePoint:
			if not self.lovePointToolTip:
				self.lovePointToolTip = uiToolTip.ToolTip(100)
				self.lovePointToolTip.SetTitle(self.name)
				self.lovePointToolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
				self.lovePointToolTip.ResizeToolTip()
			self.lovePointToolTip.ShowToolTip()

	def OnMouseOverOut(self):
		if self.lovePointToolTip:
			self.lovePointToolTip.HideToolTip()

	def OnMouseLeftButtonDown(self):
		self.getParentEvent().OnSelectItem(self)

	def OnMouseLeftButtonDoubleClick(self):
		self.getParentEvent().OnDoubleClickItem(self)

	def OnRender(self):
		if self.isSelected:
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				new_x = 0

				if self.secondLanguageFlag.IsShow():
					new_x += 23

				grp.RenderBar(x+16+new_x, y+4, self.GetWidth()-16-new_x, self.GetHeight()-4)
			else:
				grp.RenderBar(x+16, y, self.GetWidth()-16, self.GetHeight())

class MessengerMemberItem(MessengerItem):

	STATE_OFFLINE = 0
	STATE_ONLINE = 1

	IMAGE_FILE_NAME = {"ONLINE" : "d:/ymir work/ui/game/windows/messenger_list_online.sub", "OFFLINE" : "d:/ymir work/ui/game/windows/messenger_list_offline.sub",}

	def __init__(self, getParentEvent):
		MessengerItem.__init__(self, getParentEvent)
		self.key = None
		self.state = self.STATE_OFFLINE
		self.Offline()

	def GetStepWidth(self):
		return 15

	def SetKey(self, key):
		self.key = key

	def IsSameKey(self, key):
		return self.key == key

	def IsOnline(self):
		if self.STATE_ONLINE == self.state:
			return TRUE

		return FALSE

	def Online(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["ONLINE"])
		self.state = self.STATE_ONLINE

	def Offline(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OFFLINE"])
		self.state = self.STATE_OFFLINE

	def CanWhisper(self):
		if self.IsOnline():
			return TRUE

		return FALSE

	def OnWhisper(self):
		if self.IsOnline():
			self.getParentEvent().whisperButtonEvent(self.GetName())

	def Select(self):
		MessengerItem.Select(self)

class MessengerGroupItem(MessengerItem):

	IMAGE_FILE_NAME = {	"OPEN" : "d:/ymir work/ui/game/windows/messenger_list_open.sub",
						"CLOSE" : "d:/ymir work/ui/game/windows/messenger_list_close.sub", }

	def __init__(self, getParentEvent):
		self.isOpen = FALSE
		self.memberList = []

		MessengerItem.__init__(self, getParentEvent)

	def AppendMember(self, member, key, name):
		member.SetKey(key)
		member.SetName(name)
		self.memberList.append(member)
		return member

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def AppendMemberWithLang(self, member, key, name, lang, lang2nd):
			member.SetKey(key)
			member.SetName(name)
			member.SetLang(lang, lang2nd)
			self.memberList.append(member)
			return member

	def RemoveMember(self, item):
		for i in xrange(len(self.memberList)):
			if item == self.memberList[i]:
				del self.memberList[i]
				return

	def ClearMember(self):
		self.memberList = []

	def FindMember(self, key):
		list = filter(lambda argMember, argKey=key: argMember.IsSameKey(argKey), self.memberList)
		if list:
			return list[0]

		return None

	def GetLoginMemberList(self):
		return filter(MessengerMemberItem.IsOnline, self.memberList)

	def GetLogoutMemberList(self):
		return filter(lambda arg: not arg.IsOnline(), self.memberList)

	def IsOpen(self):
		return self.isOpen

	def Open(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["OPEN"])
		self.isOpen = TRUE

	def Close(self):
		self.image.LoadImage(self.IMAGE_FILE_NAME["CLOSE"])
		self.isOpen = FALSE

		map(ui.Window.Hide, self.memberList)

	def Select(self):

		if self.IsOpen():
			self.Close()
		else:
			self.Open()

		MessengerItem.Select(self)
		self.getParentEvent().OnRefreshList()

class MessengerFriendItem(MessengerMemberItem):

	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanRemove(self):
		return TRUE

	def OnRemove(self):
		messenger.RemoveFriend(self.key)
		net.SendMessengerRemovePacket(self.key, self.name)
		return TRUE

	if app.ENABLE_TELEPORT_TO_A_FRIEND:
		def CanWarp(self):
			if not self.IsOnline():
				return FASLE
			return TRUE

		def OnWarp(self):
			net.SendMessengerRequestWarpByNamePacket(self.name)

if app.ENABLE_MESSENGER_TEAM:
	class MessengerTeamItem(MessengerMemberItem):
		def __init__(self, getParentEvent):
			MessengerMemberItem.__init__(self, getParentEvent)

		def CanRemove(self):
			return FALSE

		def OnRemove(self):
			return FASLE

if app.ENABLE_MESSENGER_BLOCK:
	class MessengerBlockFriendItem(MessengerMemberItem):
		def __init__(self, getParentEvent):
			MessengerMemberItem.__init__(self, getParentEvent)

		def __del__(self):
			MessengerMemberItem.__del__(self)

		def CanRemove(self):
			return TRUE

		def OnRemove(self):
			net.SendMessengerBlockRemovePacket(self.key, self.name)
			return TRUE

class MessengerGuildItem(MessengerMemberItem):

	def __init__(self, getParentEvent):
		MessengerMemberItem.__init__(self, getParentEvent)

	def CanWarp(self):
		if not self.IsOnline():
			return FALSE
		return TRUE

	def OnWarp(self):
		net.SendGuildUseSkillPacket(155, self.key)

	def CanRemove(self):
		for i in xrange(guild.ENEMY_GUILD_SLOT_MAX_COUNT):
			if guild.GetEnemyGuildName(i) != "":
				return FALSE

		if guild.MainPlayerHasAuthority(guild.AUTH_REMOVE_MEMBER):
			if guild.IsMemberByName(self.name):
				return TRUE

		return FALSE

	def OnRemove(self):
		net.SendGuildRemoveMemberPacket(self.key)
		return TRUE

class MessengerFriendGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FRIEND)

	def AppendMember(self, key, name):
		item = MessengerFriendItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

if app.ENABLE_MESSENGER_TEAM:
	class MessengerTeamGroup(MessengerGroupItem):

		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName(localeInfo.MESSENGER_TEAM)

		def AppendMember(self, key, name):
			item = MessengerTeamItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			def AppendMemberWithLang(self, key, name, lang, lang2nd):
				item = MessengerTeamItem(self.getParentEvent)
				return MessengerGroupItem.AppendMemberWithLang(self, item, key, name, lang, lang2nd)

if app.ENABLE_MESSENGER_BLOCK:
	class MessengerBlockGroup(MessengerGroupItem):
		def __init__(self, getParentEvent):
			MessengerGroupItem.__init__(self, getParentEvent)
			self.SetName(localeInfo.MESSENGER_BLOCK)
			self.AddFlag("float")

		def __del__(self):
			MessengerGroupItem.__del__(self)

		def AppendMember(self, key, name):
			item = MessengerBlockFriendItem(self.getParentEvent)
			return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerGuildGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_GUILD)
		self.AddFlag("float")

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		return MessengerGroupItem.AppendMember(self, item, key, name)

class MessengerFamilyGroup(MessengerGroupItem):

	def __init__(self, getParentEvent):
		MessengerGroupItem.__init__(self, getParentEvent)
		self.SetName(localeInfo.MESSENGER_FAMILY)
		self.AddFlag("float")

		self.lover = None

	def AppendMember(self, key, name):
		item = MessengerGuildItem(self.getParentEvent)
		self.lover = item
		return MessengerGroupItem.AppendMember(self, item, key, name)

	def GetLover(self):
		return self.lover

###################################################################################################
###################################################################################################
###################################################################################################

class MessengerWindow(ui.ScriptWindow):

	START_POSITION = 40

	class ResizeButton(ui.DragButton):

		def OnMouseOverIn(self):
			app.SetCursor(app.VSIZE)

		def OnMouseOverOut(self):
			app.SetCursor(app.NORMAL)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		messenger.SetMessengerHandler(self)

		self.board = None
		self.groupList = []
		self.showingItemList = []
		self.selectedItem = None
		self.whisperButtonEvent = lambda *arg: None
		self.familyGroup = None

		self.guildButtonEvent = None

		self.showingPageSize = 0
		self.startLine = 0

		self.isLoaded = 0

		self.__AddGroup()
		messenger.RefreshGuildMember()

	def Show(self):
		if self.isLoaded==0:
			self.isLoaded=1

			self.__LoadWindow()
			self.OnRefreshList()
			self.OnResizeDialog()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "UIScript/MessengerWindow.py")

		try:
			self.board = self.GetChild("board")
			self.scrollBar = self.GetChild("ScrollBar")
			self.whisperButton = self.GetChild("WhisperButton")
			self.removeButton = self.GetChild("RemoveButton")
			self.addFriendButton = self.GetChild("AddFriendButton")
			if app.ENABLE_MESSENGER_BLOCK:
				self.addBlockButton = self.GetChild("BlockFriendButton")
			self.guildButton = self.GetChild("GuildButton")
		except:
			import exception
			exception.Abort("MessengerWindow.__LoadWindow.__Bind")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.whisperButton.SetEvent(ui.__mem_func__(self.OnPressWhisperButton))
		self.removeButton.SetEvent(ui.__mem_func__(self.OnPressRemoveButton))
		self.addFriendButton.SetEvent(ui.__mem_func__(self.OnPressAddFriendButton))
		if app.ENABLE_MESSENGER_BLOCK:
			self.addBlockButton.SetEvent(ui.__mem_func__(self.OnPressAddBlockButton))
		self.guildButton.SetEvent(ui.__mem_func__(self.OnPressGuildButton))

		width = self.GetWidth()
		height = self.GetHeight()
		if not app.ENABLE_MESSENGER_BLOCK:
			self.addFriendButton.SetPosition(-60, 30)
			self.whisperButton.SetPosition(-20, 30)
			self.removeButton.SetPosition(20, 30)
			self.guildButton.SetPosition(60, 30)

		self.whisperButton.Disable()
		self.removeButton.Disable()

		resizeButton = self.ResizeButton()
		resizeButton.AddFlag("restrict_x")
		resizeButton.SetParent(self)
		resizeButton.SetSize(self.GetWidth(), 10)
		resizeButton.SetWindowVerticalAlignBottom()
		resizeButton.SetPosition(0, 0)
		resizeButton.Show()
		self.resizeButton = resizeButton
		self.resizeButton.SetMoveEvent(ui.__mem_func__(self.OnResizeDialog))
		self.resizeButton.SetPosition(0, 300)

		for list in self.groupList:
			list.SetTop()

	def __del__(self):
		messenger.SetMessengerHandler(None)
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.board = None
		self.scrollBar = None
		self.resizeButton = None
		self.friendNameBoard = None
		self.questionDialog = None
		self.popupDialog = None
		self.inputDialog = None
		self.familyGroup = None

		self.whisperButton = None
		self.removeButton = None

		if app.ENABLE_MESSENGER_BLOCK:
			self.blockFriendNameBoard = None

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return TRUE

	def Close(self):
		self.questionDialog = None
		self.Hide()

	def SetSize(self, width, height):
		ui.ScriptWindow.SetSize(self, width, height)
		if self.board:
			self.board.SetSize(width, height)

	def OnResizeDialog(self):
		x, y = self.resizeButton.GetLocalPosition()
		if y < 140:
			self.resizeButton.SetPosition(x, 140)
			return
		self.SetSize(self.GetWidth(), y + self.resizeButton.GetHeight())

		self.showingPageSize = y - (self.START_POSITION + 26)
		self.scrollBar.SetScrollBarSize(self.showingPageSize)

		self.__LocateMember()

		self.resizeButton.TurnOffCallBack()
		self.UpdateRect()
		self.resizeButton.TurnOnCallBack()

	def __LocateMember(self):

		if self.isLoaded==0:
			return

		if self.showingPageSize/20 >= len(self.showingItemList):
			self.scrollBar.Hide()
			self.startLine = 0
		else:
			if self.showingItemList:
				self.scrollBar.SetMiddleBarSize(float(self.showingPageSize/20) / float(len(self.showingItemList)))
			self.scrollBar.Show()

		#####

		yPos = self.START_POSITION
		heightLimit = self.GetHeight() - (self.START_POSITION + 13)

		map(ui.Window.Hide, self.showingItemList)

		for item in self.showingItemList[self.startLine:]:
			item.SetPosition(20 + item.GetStepWidth(), yPos)
			item.SetTop()
			item.Show()

			yPos += 20
			if yPos > heightLimit:
				break

	def __AddGroup(self):
		member = MessengerFriendGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		member = MessengerGuildGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()
		self.groupList.append(member)

		if app.ENABLE_MESSENGER_TEAM:
			member = MessengerTeamGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

		if app.ENABLE_MESSENGER_BLOCK:
			member = MessengerBlockGroup(ui.__mem_func__(self.GetSelf))
			member.Open()
			member.Show()
			self.groupList.append(member)

	def __AddFamilyGroup(self):
		member = MessengerFamilyGroup(ui.__mem_func__(self.GetSelf))
		member.Open()
		member.Show()

		self.familyGroup = member

	def ClearGuildMember(self):
		self.groupList[GUILD].ClearMember()

	def SetWhisperButtonEvent(self, event):
		self.whisperButtonEvent=event

	def SetGuildButtonEvent(self, event):
		self.guildButtonEvent=event

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return TRUE

	def OnPressGuildButton(self):
		self.guildButtonEvent()

	def OnPressAddFriendButton(self):
		friendNameBoard = uiCommon.InputDialog()
		friendNameBoard.SetTitle(localeInfo.MESSENGER_ADD_FRIEND)
		friendNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddFriend))
		friendNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddFriend))
		friendNameBoard.Open()
		self.friendNameBoard = friendNameBoard

	def OnAddFriend(self):
		text = self.friendNameBoard.GetText()
		if text:
			net.SendMessengerAddByNamePacket(text)
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return TRUE

	def OnCancelAddFriend(self):
		self.friendNameBoard.Close()
		self.friendNameBoard = None
		return TRUE

	if app.ENABLE_MESSENGER_BLOCK:
		def OnPressAddBlockButton(self):
			blockNameBoard = uiCommon.InputDialog()
			blockNameBoard.SetTitle(localeInfo.MESSENGER_ADD_BLOCK_FRIEND)
			blockNameBoard.SetAcceptEvent(ui.__mem_func__(self.OnAddBlock))
			blockNameBoard.SetCancelEvent(ui.__mem_func__(self.OnCancelAddBlock))
			blockNameBoard.Open()
			self.blockFriendNameBoard = blockNameBoard

		def OnAddBlock(self):
			text = self.blockFriendNameBoard.GetText()
			if text:
				net.SendMessengerBlockAddByNamePacket(text)
			self.blockFriendNameBoard.Close()
			self.blockFriendNameBoard = None
			return TRUE

		def OnCancelAddBlock(self):
			self.blockFriendNameBoard.Close()
			self.blockFriendNameBoard = None
			return TRUE

	def OnPressWhisperButton(self):
		if self.selectedItem:
			self.selectedItem.OnWhisper()

	def OnPressRemoveButton(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				self.questionDialog = uiCommon.QuestionDialog("thin")
				self.questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnRemove))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
				self.questionDialog.Open()

	def OnRemove(self):
		if self.selectedItem:
			if self.selectedItem.CanRemove():
				map(lambda arg, argDeletingItem=self.selectedItem: arg.RemoveMember(argDeletingItem), self.groupList)
				self.selectedItem.OnRemove()
				self.selectedItem.UnSelect()
				self.selectedItem = None
				self.OnRefreshList()

		self.OnCloseQuestionDialog()

	def OnScroll(self):
		scrollLineCount = len(self.showingItemList) - (self.showingPageSize/20)
		startLine = int(scrollLineCount * self.scrollBar.GetPos())

		if startLine != self.startLine:
			self.startLine = startLine
			self.__LocateMember()

	def OnRunMouseWheel(self, nLen):
		if self.scrollBar.IsShow():
			if nLen > 0:
				self.scrollBar.OnUp()
			else:
				self.scrollBar.OnDown()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	## CallBack
	def OnSelectItem(self, item):

		if self.selectedItem:
			if item != self.selectedItem:
				self.selectedItem.UnSelect()

		self.selectedItem = item

		if self.selectedItem:
			self.selectedItem.Select()

			if self.selectedItem.CanWhisper():
				self.whisperButton.Enable()
			else:
				self.whisperButton.Disable()

			if self.selectedItem.CanRemove():
				self.removeButton.Enable()
			else:
				self.removeButton.Disable()

	def OnDoubleClickItem(self, item):

		if not self.selectedItem:
			return

		if self.selectedItem.IsOnline():
			self.OnPressWhisperButton()

	def GetSelf(self):
		return self

	def OnRefreshList(self):
		self.showingItemList = []

		if self.familyGroup:
			self.showingItemList.append(self.familyGroup)
			if self.familyGroup.GetLover():
				self.showingItemList.append(self.familyGroup.GetLover())

		for group in self.groupList:

			self.showingItemList.append(group)

			if group.IsOpen():

				loginMemberList = group.GetLoginMemberList()
				logoutMemberList = group.GetLogoutMemberList()

				if loginMemberList or logoutMemberList:
					for member in loginMemberList:
						self.showingItemList.append(member)
					for member in logoutMemberList:
						self.showingItemList.append(member)

				else:
					item = MessengerItem(ui.__mem_func__(self.GetSelf))
					item.SetName(localeInfo.MESSENGER_EMPTY_LIST)
					self.showingItemList.append(item)

		self.__LocateMember()

	def RefreshMessenger(self):
		self.OnRefreshList()

	## EventHandler
	def __AddList(self, groupIndex, key, name):
		group = self.groupList[groupIndex]
		member = group.FindMember(key)
		if not member:
			member = group.AppendMember(key, name)
			self.OnSelectItem(None)
		return member

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def __AddListWithLang(self, groupIndex, key, name, lang, lang2nd):
			group = self.groupList[groupIndex]
			member = group.FindMember(key)
			if not member:
				member = group.AppendMemberWithLang(key, name, lang, lang2nd)
				self.OnSelectItem(None)

			return member

	def OnRemoveList(self, groupIndex, key):
		group = self.groupList[groupIndex]
		group.RemoveMember(group.FindMember(key))
		self.OnRefreshList()

	def OnRemoveAllList(self, groupIndex):
		group = self.groupList[groupIndex]
		group.ClearMember()
		self.OnRefreshList()

	def OpenWhisper(self, eventType, userName):
		self.whisperButtonEvent(userName)

	def OnLogin(self, groupIndex, key, name=None):
		if not name:
			name = key
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		member.SetName(name)
		member.Online()
		self.OnRefreshList()

		if not name in constInfo.ALREADY_NOTIFY_LIST:
			self.onlinePopup = uiCommon.OnlinePopup()
			self.onlinePopup.SetUserName(name)
			self.onlinePopup.SetEvent(ui.__mem_func__(self.OpenWhisper), "MOUSE_LEFT_BUTTON_UP", name)
			self.onlinePopup.SlideIn()

			constInfo.ALREADY_NOTIFY_LIST.append(name)

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def OnLoginWithLang(self, groupIndex, key, lang, lang2nd, name=None):
			if not name:
				name = key
			group = self.groupList[groupIndex]
			member = self.__AddListWithLang(groupIndex, key, name, lang, lang2nd)
			member.SetName(name)
			member.SetLang(lang, lang2nd)
			member.Online()
			self.OnRefreshList()

			if not name in constInfo.ALREADY_NOTIFY_LIST:
				self.onlinePopup = uiCommon.OnlinePopup()
				self.onlinePopup.SetUserName(name)
				self.onlinePopup.SetEvent(ui.__mem_func__(self.OpenWhisper), "MOUSE_LEFT_BUTTON_UP", name)
				self.onlinePopup.SlideIn()

				constInfo.ALREADY_NOTIFY_LIST.append(name)

	def OnLogout(self, groupIndex, key, name=None):
		group = self.groupList[groupIndex]
		member = self.__AddList(groupIndex, key, name)
		if not name:
			name = key
		member.SetName(name)
		member.Offline()
		self.OnRefreshList()

	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def OnLogoutWithLang(self, groupIndex, key, lang, lang2nd, name=None):
			group = self.groupList[groupIndex]
			member = self.__AddListWithLang(groupIndex, key, name, lang, lang2nd)
			if not name:
				name = key
			member.SetName(name)
			member.SetLang(lang, lang2nd)
			member.Offline()
			self.OnRefreshList()

	def OnAddLover(self, name, lovePoint):
		if not self.familyGroup:
			self.__AddFamilyGroup()

		member = self.familyGroup.AppendMember(0, name)

		member.SetName(name)
		member.SetLovePoint(lovePoint)
		member.Offline()
		self.OnRefreshList()

	def OnUpdateLovePoint(self, lovePoint):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.SetLovePoint(lovePoint)

	def OnLoginLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Online()

	def OnLogoutLover(self):
		if not self.familyGroup:
			return

		lover = self.familyGroup.GetLover()
		if not lover:
			return

		lover.Offline()

	def ClearLoverInfo(self):
		if not self.familyGroup:
			return

		self.familyGroup.ClearMember()
		self.familyGroup = None
		self.OnRefreshList()
