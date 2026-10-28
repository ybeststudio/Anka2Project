if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import background
import ui, renderTarget, constInfo

ROOT_YENI = "remoteshop_2022/"

#######################################################

RENDER_TARGET_INDEX = 62
MAX_VIEW_COUNT = 8

BUTTON_X_START = 15
BUTTON_Y_START = 45
BUTTON_Y_GAP = 51


#######################################################

class RemoteShop(ui.ScriptWindow):

	NPC_LIST = {
	
	0 : ["Silah Satýcýsý ", 1, 9001],
	1 : ["Zýrh Satýcýsý ", 4, 9002],
	2 : ["Satýcý Market ", 90101, 9003],
	3 : ["Satýcý Cevherler. ", 90102, 9003],
	4 : ["Satýcý Taþlar ", 90104, 9003],
	5 : ["Emek Puan ( 1 ) ", 8, 9004],
	6 : ["Emek Puan ( 2 ) ", 20503, 9004],
	7 : ["KAPALI ", 302, 20082],
	}


	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.titleBar = 0
		self.secilen = 0
		self.tab = {}
		self.text = {}
		self.img = {}

	def Destroy(self):
		self.ClearDictionary()
		self.__Initialize()
		print " -------------------------------------- DESTROY SYSTEM HIDRAGAME DIALOG"

	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("System.Dialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:

			self.titleBar = self.GetChild("titlebar")
			self.board = self.GetChild("board")
			self.name = self.GetChild("test")
			self.OpenButton = self.GetChild("OpenButton")
			self.CancelButton = self.GetChild("OpenButton")
			self.BlackBoard2 = self.GetChild("BlackBoard2")
			self.BlackBoard1 = self.GetChild("BlackBoard1")
			self.CancelButton = self.GetChild("cancel")
			self.ScrollBar = self.GetChild("ScrollBar")
		except:
			import exception
			exception.Abort("Dialog.__Load_BindObject")

	def __Load(self):
		self.__Load_LoadScript("uiscript/npcekran.py")
		self.__Load_BindObject()

		self.renderTarget = ui.RenderTarget()
		self.renderTarget.SetParent(self.BlackBoard2)
		self.renderTarget.SetSize(230,350)
		self.renderTarget.SetPosition(10,40)
		self.renderTarget.SetRenderTarget(RENDER_TARGET_INDEX)
		self.renderTarget.Show()

		renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/render_oval2.tga")
		renderTarget.SetScale(RENDER_TARGET_INDEX, 1.0)
		renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)
		
		self.SetCenterPosition()
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.OpenButton.SetEvent(ui.__mem_func__(self.ClickOpen))
		self.CancelButton.SAFE_SetEvent(self.Close)

	def CreateButton(self):
		self.ScrollBar.SetMiddleBarSize(float(MAX_VIEW_COUNT) / float(len(self.NPC_LIST)))
		for i in range(MAX_VIEW_COUNT):
			self.MakeButton(
				i,\
				self.board,\
				BUTTON_X_START, BUTTON_Y_START + (BUTTON_Y_GAP * i)
			)

	def OnScroll(self):
		board_count = MAX_VIEW_COUNT
		pos = int(self.ScrollBar.GetPos() * (len(self.NPC_LIST) - board_count))

		for i in xrange(board_count):
			realPos = i + pos
			self.MakeButton(
				realPos,\
				self.board,\
				BUTTON_X_START, BUTTON_Y_START + (BUTTON_Y_GAP * i)
			)

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.ScrollBar.OnUp()
		else:
			self.ScrollBar.OnDown()
		return True

	def MakeButton(self, index, parent, x, y):
		self.tab[index] = ui.MakeButton(parent, x, y, False, "remoteshop_2022/butonlar/", "list_bg2.png", "list_bg2.png", "list_bg1.png")
		
		self.tab[index].SetEvent(ui.__mem_func__(self.__ClickButton), index)
		
		titleName = self.NPC_LIST[index][0]

		self.text[index] = ui.TextLine()
		self.text[index].SetParent(self.tab[index])
		self.text[index].SetPosition(75, 12)
		self.text[index].SetFontName("Tahoma:14")
		self.text[index].SetText(str(titleName))
		self.text[index].Show()
		
		self.img[index] = ui.ImageBox()
		self.img[index].SetParent(self.tab[index])
		self.img[index].LoadImage(ROOT_YENI + "face/%d.png" % self.NPC_LIST[index][2])
		self.img[index].SetPosition(0, 0)
		self.img[index].Show()

	def EnableButtons(self):
		for i in range(len(self.tab)):
			self.tab[i].Enable()
			self.tab[i].SetUp()

	def __ClickButton(self, index):
		self.EnableButtons()
		
		self.secilen = index
	
		self.tab[index].Disable()
		self.tab[index].Down()
		
		renderTarget.SelectModel(RENDER_TARGET_INDEX, self.NPC_LIST[index][2])
		self.name.SetText(self.NPC_LIST[index][0])
		
	def ClickOpen(self):
		constInfo.RANGE_SHOP_NOT_VIEW = 1
		net.SendChatPacket("/open_range_npc %d " % int(self.NPC_LIST[self.secilen][1]))
		self.Close()

	def Show(self):
		ui.ScriptWindow.Show(self)
		renderTarget.SelectModel(RENDER_TARGET_INDEX, self.NPC_LIST[0][2])
		self.name.SetText(self.NPC_LIST[0][0])
		self.CreateButton()

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
