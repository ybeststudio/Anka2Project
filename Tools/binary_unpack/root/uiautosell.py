if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import mouseModule
import snd
import item
import chat
import uiScriptLocale
import uiCommon
import uiPickMoney
import localeInfo
import constInfo
import uiToolTip

from ui import ListBoxAutoSell

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH

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
		cornerPath = IMG_DIR+"board/corner_"
		linePath = IMG_DIR+"board/"
		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("left", "right", "top", "bottom") ]
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
		self.Base.LoadImage(IMG_DIR+"board/base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()
	def __del__(self):
		ui.Window.__del__(self)
	def Destroy(self):
		self.Base=0
		self.Corners=0
		self.Lines=0
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

IMG_DIR = "d:/ymir work/ui/game/autosell/"

if app.ENABLE_AUTO_SELL_SYSTEM:
	class AutoSellWindow(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.isLoaded = False
			self.itemStock = {}
			self.autoSellStatus = False
			self.onRunMouseWheelEvent = None
			self.Initialize()
			self.listBox = ListBoxAutoSell()
			self.selItem = None

		def __del__(self):
			ui.ScriptWindow.__del__(self)
	
		def Initialize(self):
			self.LoadWindow()
	
		def LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/AutoSellWindow.py")
			except:
				import exception
				exception.Abort("AutoSellWindow.LoadWindow.LoadObject")
	
			try:
				self.board = self.GetChild("Board")
				self.otoSat = self.GetChild("OtoSat")
				self.removeItemButton = self.GetChild("RemoveItem")
				self.removeAllItemButton = self.GetChild("RemoveAllItems")

				self.scrollBar = ui.ScrollBar()
				self.scrollBar.SetParent(self)
				self.scrollBar.SetPosition(304, 40)
				self.scrollBar.SetScrollBarSize(246)
				self.scrollBar.Show()

######################### ÝTEM ARAMA ########################################
				self.AramaKutusuGoruntu = SearchSlotBoard()
				self.AramaKutusuGoruntu.SetParent(self)
				self.AramaKutusuGoruntu.SetPosition(13, 46)
				self.AramaKutusuGoruntu.SetSize(130, 20)

				self.AramaButonu = ui.Button()
				self.AramaButonu.SetParent(self)
				self.AramaButonu.SetPosition(147, 46)
				self.AramaButonu.SetUpVisual(IMG_DIR+"search_btn_0.tga")
				self.AramaButonu.SetOverVisual(IMG_DIR+"search_btn_1.tga")
				self.AramaButonu.SetDownVisual(IMG_DIR+"search_btn_2.tga")
				self.AramaButonu.SetEvent(self.OnSearchKeyDown)
				self.AramaButonu.Show()

				self.AramaKutusu = ui.EditLine()
				self.AramaKutusu.SetParent(self.AramaKutusuGoruntu)
				self.AramaKutusu.SetPosition(7, 2)
				self.AramaKutusu.SetSize(110, 20)
				self.AramaKutusu.SetMax(20)
				self.AramaKutusu.SetText("")
				self.AramaKutusu.SetFocus()
				self.AramaKutusu.SetReturnEvent(ui.__mem_func__(self.OnSearchKeyDown))
				self.AramaKutusu.SetEscapeEvent(ui.__mem_func__(self.OnClose))
				self.AramaKutusu.Show()
				self.AramaKutusuGoruntu.Show()
######################### ÝTEM ARAMA ########################################

				self.itemList = ui.ListBoxAutoSell()
				self.itemList.SetParent(self)
				self.itemList.SetPosition(10, 65)
				self.itemList.SetSize(110, 190)
				self.itemList.SetScrollBar(self.scrollBar)
				self.itemList.Show()
			except:
				import exception
				exception.Abort("AutoSellWindow.LoadWindow.BindObject")
				
			self.board.SetCloseEvent(ui.__mem_func__(self.OnClose))
			self.otoSat.SetEvent(lambda: self.SetMod(int(not self.autoSellStatus)))
			self.removeItemButton.SetEvent(lambda: self.RemoveSelectedItem())
			self.removeAllItemButton.SetEvent(lambda: self.RemoveAllItem())
			
		def Open(self):
			if self.IsShow():
				return
		
			net.SendChatPacket("/autosell_info 1")
			self.Show()


			
		def Close(self):
			self.Hide()
			net.SendChatPacket("/autosell_info 1")
			
		def OnClose(self):
			self.Close()
			
		def OnPressEscapeKey(self):
			self.Close()
			return True

		def SendAutoSellStatus(self, isActive):
			net.SendChatPacket("/autosell_status %d" % (1 if isActive else 0))
	
		def SendAutoSellAdd(self, itemVnum):
			net.SendChatPacket("/autosell_add %d" % itemVnum)
	
		def SendAutoSellRemove(self, itemVnum):
			net.SendChatPacket("/autosell_remove %d" % itemVnum)

		def OnSearchKeyDown(self):
			search_text = self.AramaKutusu.GetText().strip().lower()
			# chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Arama yapýlýyor: {}".format(search_text))
			self.UpdateItemList(search_text)

		def UpdateItemList(self, filter_text=""):
			self.itemList.RemoveAllItems()
			matching_items = []
		
			for item_id, item_name in self.itemStock.items():
				if filter_text and filter_text.lower() in item_name.lower():
					matching_items.append((item_id, item_name))
				elif not filter_text:
					matching_items.append((item_id, item_name))
			matching_items.sort(key=lambda x: x[1].lower())
			for item_id, item_name in matching_items:
				list_item = ListBoxAutoSell.Item()
				list_item.SetText(item_name.capitalize())
				self.itemList.AppendItem(list_item)
		
			self.itemList.UpdateRect()
			self.scrollBar.SetPos(0)

		def RemoveAllItem(self):
			if not self.itemStock:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Ýtem listesi boþ!")
				return
			self.itemStock.clear()
			self.itemList.RemoveAllItems()
			self.itemList.UpdateRect()
			net.SendChatPacket("/autosell_remove_all")
			# chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Tüm eþyalar kaldýrýldý!")

		def RemoveSelectedItem(self):
			if not self.itemStock:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Ýtem listesi boþ!")
				return
			selected_item = self.itemList.GetSelectedItem()
			if selected_item is None:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Bir item seçmelisin!")
				return
			if isinstance(selected_item, ui.ListBoxAutoSell.Item):
				item_name = selected_item.GetText().strip().lower()
				selected_item_id = next((item_id for item_id, name in self.itemStock.items() if name.lower() == item_name), None)
			
				if selected_item_id is not None:
					chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> " + item_name + " listeden kaldýrýlýyor...")
					self.SendAutoSellRemove(selected_item_id)
					del self.itemStock[selected_item_id]
					self.itemList.RemoveItem(selected_item)
					net.SendChatPacket("/autosell_info 1")
					self.itemList.UpdateRect()
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Seçilen item geçerli deðil!")
		def AddItem(self, item_id):
			try:
				item_id = int(item_id)
				if item_id <= 0:
					return 
				item.SelectItem(item_id)
				item_name = item.GetItemName().lower()
				if item_id in self.itemStock:
					# chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> {} zaten listede!".format(item_name))
					return 
				self.itemStock[item_id] = item_name  
				# chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> {} listeye eklendi.".format(item_name))
				self.UpdateItemList()
				net.SendChatPacket("/autosell_info 1")
			except Exception as e:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "<Otomatik Satma> Hata: {}".format(str(e)))

		def SetMod(self, status):
			try:
				status_int = int(status)
				if status_int not in [0, 1]:
					raise ValueError("Geçersiz Deðer! Sadece 0 veya 1 olabilir.")
				new_status = bool(status_int)
				if new_status == self.autoSellStatus:
					return
				self.autoSellStatus = new_status
	
				if self.autoSellStatus:
					self.otoSat.SetText("Aktif")
					self.SendAutoSellStatus(True)
				else:
					self.otoSat.SetText("Pasif")
					self.SendAutoSellStatus(False)
			except ValueError as e:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Hata: {}".format(str(e)))
	
		def OnRunMouseWheel(self, nLen):
			scroll = self.scrollBar
			if self.itemList.IsInPosition():
				scroll = self.scrollBar
	
			if nLen > 0:
				scroll.OnUp()
			else:
				scroll.OnDown()
	
			return True
	
		def SetOnRunMouseWheelEvent(self, event):
			self.onRunMouseWheelEvent = __mem_func__(event)