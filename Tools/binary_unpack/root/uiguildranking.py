if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))

import ui
import guildranking
import math

class GuildRankingDialog(ui.ScriptWindow):
	SLOT_RANKING = 0
	SLOT_GUILD_NAME = 1
	SLOT_OWNER_NAME = 2
	SLOT_LEVEL = 3
	SLOT_POINT = 4
	SLOT_WIN = 5
	SLOT_DRAW = 6
	SLOT_LOSS = 7
	MAX_LINE_COUNT = guildranking.GUILD_RANK_PAGE_MAX_NUM

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.Page = 1
		self.IsShow = FALSE
		self.board = None
		self.ResultButtonList = []
		self.ResultSlotList = {}
		self.MyResultSlotList = []
		self.ResultButtonRankList = []
		self.ScrollBar = None
		self.NowStartLineNumber = 0
		self.LoadWindow()
		self.MakeUiBoard()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		self.isLoaded = 0
		self.IsShow = FALSE
		self.board = None
		self.ScrollBar = None
		self.ResultButtonList = []
		self.ResultSlotList = {}
		self.MyResultSlotList = []
		self.ResultButtonRankList = []

	def Destory(self):
		self.Close()
		self.isLoaded = 0
		self.IsShow = FALSE
		self.board = None
		self.ScrollBar = None
		self.ResultButtonList = []
		self.ResultSlotList = {}
		self.MyResultSlotList = []
		self.ResultButtonRankList = []
		
	def Open(self):
		if not self.IsShow:
			self.IsShow = TRUE
			self.SetCenterPosition()
			self.SetTop()
			self.Page = 1
			self.ScrollBar.SetPos(0)
			self.NowStartLineNumber = 0
			self.RefreshRankingBoard()
			ui.ScriptWindow.Show(self)

	def Close(self):
		self.IsShow = FALSE
		guildranking.GuildRankClear()
		self.Hide()
	
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/GuildRanking.py")
		except:
			import exception
			exception.Abort("GuildRankingDialog.__LoadWindow.LoadScript")
		getObject = self.GetChild
		self.board = getObject("TitleBar")
		self.prevbtn = getObject("prev_button")
		self.prevfirstbtn = getObject("prev_first_button")
		self.nextbtn = getObject("next_button")
		self.nextlastbtn = getObject("next_last_button")
		self.CurrentPageText = getObject("CurrentPage")
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.prevfirstbtn.SAFE_SetEvent(self.ChangePageFL, FALSE)
		self.nextlastbtn.SAFE_SetEvent(self.ChangePageFL, TRUE)
		self.prevbtn.SAFE_SetEvent(self.ChangePage, -1)
		self.nextbtn.SAFE_SetEvent(self.ChangePage, 1)
		self.ScrollBar = getObject("ScrollBar")
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScrollControl))
			
	def ChangePage(self, count):
		self.Page += count
		self.ScrollBar.SetPos(0)
		self.NowStartLineNumber = 0
		self.RefreshRankingBoard()
		
	def PageControl(self):
		for prev in (self.prevfirstbtn, self.prevbtn): 
			if self.Page <= 1:
				prev.Hide()
			else:
				prev.Show()			
		for next in (self.nextlastbtn, self.nextbtn): 
			if self.Page >= self.GetLastPage():
				next.Hide()
			else:
				next.Show()	
		self.CurrentPageText.SetText(str(self.Page))
	
	def ChangePageFL(self, Last):
		self.Page = [1, self.GetLastPage()][Last == TRUE]
		self.ScrollBar.SetPos(0)
		self.NowStartLineNumber = 0
		self.RefreshRankingBoard()

	def MakeUiBoard(self):
		yPos = 0
		for i in range(0, self.MAX_LINE_COUNT+1):			
			yPos = 60 + i * 24
			if i == 10:
				yPos += 10
				
			GuildRankingSlotImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 23, yPos)
			GuildRankingSlotImage.SetAlpha(0)
			GuildRankingSlot = ui.MakeTextLine(GuildRankingSlotImage)
			self.Children.append(GuildRankingSlotImage)
			self.Children.append(GuildRankingSlot)

			GuildNameImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_04.sub", 53, yPos)
			GuildNameImage.SetAlpha(0)
			GuildNameSlot = ui.MakeTextLine(GuildNameImage)
			self.Children.append(GuildNameImage)
			self.Children.append(GuildNameSlot)

			OwnerNameImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_04.sub", 154, yPos)
			OwnerNameImage.SetAlpha(0)
			OwnerNameSlot = ui.MakeTextLine(OwnerNameImage)
			self.Children.append(OwnerNameImage)
			self.Children.append(OwnerNameSlot)

			LevelImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 257, yPos)
			LevelImage.SetAlpha(0)
			LevelSlot = ui.MakeTextLine(LevelImage)
			self.Children.append(LevelImage)
			self.Children.append(LevelSlot)
		
			PointImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_04.sub", 274, yPos)
			PointImage.SetAlpha(0)
			PointSlot = ui.MakeTextLine(PointImage)
			self.Children.append(PointImage)
			self.Children.append(PointSlot)

			WinImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 373, yPos)
			WinImage.SetAlpha(0)
			WinSlot = ui.MakeTextLine(WinImage)
			self.Children.append(WinImage)
			self.Children.append(WinSlot)

			DrawImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 413, yPos)
			DrawImage.SetAlpha(0)
			DrawSlot = ui.MakeTextLine(DrawImage)
			self.Children.append(DrawImage)
			self.Children.append(DrawSlot)

			LossImage = ui.MakeImageBox(self, "d:/ymir work/ui/public/parameter_slot_00.sub", 453, yPos) #410
			LossImage.SetAlpha(0)
			LossSlot = ui.MakeTextLine(LossImage)
			self.Children.append(LossImage)
			self.Children.append(LossSlot)

			if i < self.MAX_LINE_COUNT:
				temprankingslotlist = []
				temprankingslotlist.append(GuildRankingSlot)	
				temprankingslotlist.append(GuildNameSlot)		
				temprankingslotlist.append(OwnerNameSlot)
				temprankingslotlist.append(LevelSlot)
				temprankingslotlist.append(PointSlot)
				temprankingslotlist.append(WinSlot)
				temprankingslotlist.append(DrawSlot)
				temprankingslotlist.append(LossSlot)
				self.ResultSlotList[i] = temprankingslotlist
			else:
				self.MyResultSlotList.append(GuildRankingSlot)
				self.MyResultSlotList.append(GuildNameSlot)
				self.MyResultSlotList.append(OwnerNameSlot)
				self.MyResultSlotList.append(LevelSlot)
				self.MyResultSlotList.append(PointSlot)
				self.MyResultSlotList.append(WinSlot)
				self.MyResultSlotList.append(DrawSlot)
				self.MyResultSlotList.append(LossSlot)

			itemSlotButtonImage = ui.MakeButton(self, 21, yPos, "", "d:/ymir work/ui/game/guild/guildrankingsystem/", "ranking_list_button01.sub", "ranking_list_button02.sub", "ranking_list_button02.sub")
			itemSlotButtonImage.Show()
			itemSlotButtonImage.Disable()
			self.Children.append(itemSlotButtonImage)				
			if i < self.MAX_LINE_COUNT:
				self.ResultButtonList.append(itemSlotButtonImage)
	
	def RefreshRankingBoard(self):
		self.AllClear()
		self.PageControl()

		for line, ResultSlotList in self.ResultSlotList.items():
			linewpage = line + self.NowStartLineNumber + (self.Page - 1) * guildranking.GUILD_RANK_SHOW_COUNT
			if linewpage >= guildranking.GetGuildRankCount():
				break

			(guildid, guildname, ownername, level, point, win, draw, loss) = guildranking.GetGuildRankByLine(linewpage)

			ResultSlotList[self.SLOT_RANKING].SetText(str(linewpage+1))
			ResultSlotList[self.SLOT_GUILD_NAME].SetText(guildname)
			ResultSlotList[self.SLOT_OWNER_NAME].SetText(ownername)
			ResultSlotList[self.SLOT_LEVEL].SetText(str(level))
			ResultSlotList[self.SLOT_POINT].SetText(str(point))
			ResultSlotList[self.SLOT_WIN].SetText(str(win))
			ResultSlotList[self.SLOT_DRAW].SetText(str(draw))
			ResultSlotList[self.SLOT_LOSS].SetText(str(loss))

			self.ResultButtonList[line].Show()

			if player.GetGuildID() == guildid:
				self.ResultButtonList[line].Down()
				self.MyResultSlotList[self.SLOT_RANKING].SetText(str(linewpage+1))
				self.MyResultSlotList[self.SLOT_GUILD_NAME].SetText(guildname)
				self.MyResultSlotList[self.SLOT_OWNER_NAME].SetText(ownername)
				self.MyResultSlotList[self.SLOT_LEVEL].SetText(str(level))
				self.MyResultSlotList[self.SLOT_POINT].SetText(str(point))
				self.MyResultSlotList[self.SLOT_WIN].SetText(str(win))
				self.MyResultSlotList[self.SLOT_DRAW].SetText(str(draw))
				self.MyResultSlotList[self.SLOT_LOSS].SetText(str(loss))

		if 0 == player.GetGuildID():
			self.MyResultSlotList[self.SLOT_RANKING].SetText("-")
			self.MyResultSlotList[self.SLOT_GUILD_NAME].SetText("-")
			self.MyResultSlotList[self.SLOT_OWNER_NAME].SetText("-")
			self.MyResultSlotList[self.SLOT_LEVEL].SetText("-")
			self.MyResultSlotList[self.SLOT_POINT].SetText("-")
			self.MyResultSlotList[self.SLOT_WIN].SetText("-")
			self.MyResultSlotList[self.SLOT_DRAW].SetText("-")
			self.MyResultSlotList[self.SLOT_LOSS].SetText("-")
	
		self.ScrollBar.SetMiddleBarSize(float(self.MAX_LINE_COUNT) / float(self.CheckNowItemCount()))
		
	def AllClear(self):
		for line, ResultSlotList in self.ResultSlotList.items():
			ResultSlotList[self.SLOT_RANKING].SetText("")
			ResultSlotList[self.SLOT_GUILD_NAME].SetText("")
			ResultSlotList[self.SLOT_OWNER_NAME].SetText("")
			ResultSlotList[self.SLOT_LEVEL].SetText("")
			ResultSlotList[self.SLOT_POINT].SetText("")
			ResultSlotList[self.SLOT_WIN].SetText("")
			ResultSlotList[self.SLOT_DRAW].SetText("")
			ResultSlotList[self.SLOT_LOSS].SetText("")
			self.ResultButtonList[line].SetUp()
			self.ResultButtonList[line].Hide()

	def GetLastPage(self):
		return int(math.ceil(float(guildranking.GetGuildRankCount()) / guildranking.GUILD_RANK_SHOW_COUNT))
		
	def GetRemaining(self):
		remaining = [guildranking.GUILD_RANK_SHOW_COUNT, guildranking.GetGuildRankCount() % guildranking.GUILD_RANK_SHOW_COUNT][self.Page == self.GetLastPage()]
		return remaining
	
	def CheckNowItemCount(self):
		remaining = self.GetRemaining()
		if remaining <= self.MAX_LINE_COUNT:
			return self.MAX_LINE_COUNT
		return remaining
	
	def OnScrollControl(self):
		remaining = self.GetRemaining()
		if remaining <= self.MAX_LINE_COUNT:
			nowitemcount = 0
		else:
			nowitemcount = (remaining - self.MAX_LINE_COUNT)
			
		pos = self.ScrollBar.GetPos() * nowitemcount
		if not int(pos) == self.NowStartLineNumber:
			self.NowStartLineNumber = int(pos)
			self.RefreshRankingBoard()

	def OnRunMouseWheel(self, nLen):
		if self.ScrollBar.IsShow():
			if nLen > 0:
				self.ScrollBar.OnUp()
			else:
				self.ScrollBar.OnDown()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
 