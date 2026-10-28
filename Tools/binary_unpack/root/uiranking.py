if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import ui
import uiScriptLocale
import grp
import uiToolTip
import localeInfo
import chat

TIME_WAIT = 60

class RankingWindow(ui.ScriptWindow):
	MAX_CATEGORIES_BTN = 9
	MAX_CATEGORIES_TO_VIEW = 17
	#TITLE_CATEGORIES_TO_VIEW = [uiScriptLocale.RANKING_CAT0, uiScriptLocale.RANKING_CAT1, uiScriptLocale.RANKING_CAT2, uiScriptLocale.RANKING_CAT3, uiScriptLocale.RANKING_CAT4, uiScriptLocale.RANKING_CAT5, uiScriptLocale.RANKING_CAT6, uiScriptLocale.RANKING_CAT7, uiScriptLocale.RANKING_CAT8, uiScriptLocale.RANKING_CAT9, uiScriptLocale.RANKING_CAT10, uiScriptLocale.RANKING_CAT11, uiScriptLocale.RANKING_CAT12, uiScriptLocale.RANKING_CAT13, uiScriptLocale.RANKING_CAT14, uiScriptLocale.RANKING_CAT15, uiScriptLocale.RANKING_CAT16, uiScriptLocale.RANKING_CAT17, uiScriptLocale.RANKING_CAT18 ]
	TITLE_CATEGORIES_TO_VIEW = [uiScriptLocale.RANKING_CAT0, uiScriptLocale.RANKING_CAT1, uiScriptLocale.RANKING_CAT2, uiScriptLocale.RANKING_CAT3, uiScriptLocale.RANKING_CAT4, uiScriptLocale.RANKING_CAT5, uiScriptLocale.RANKING_CAT6, uiScriptLocale.RANKING_CAT7, uiScriptLocale.RANKING_CAT8, uiScriptLocale.RANKING_CAT9, uiScriptLocale.RANKING_CAT10, uiScriptLocale.RANKING_CAT11, uiScriptLocale.RANKING_CAT14, uiScriptLocale.RANKING_CAT15, uiScriptLocale.RANKING_CAT16, uiScriptLocale.RANKING_CAT17, uiScriptLocale.RANKING_CAT18 ]
	IMAGES_CATEGORIES_TO_VIEW = [
									["d:/ymir work/ui/rank_button/btn_cat0_n.png", "d:/ymir work/ui/rank_button/btn_cat0_h.png", "d:/ymir work/ui/rank_button/btn_cat0_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat1_n.png", "d:/ymir work/ui/rank_button/btn_cat1_h.png", "d:/ymir work/ui/rank_button/btn_cat1_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat2_n.png", "d:/ymir work/ui/rank_button/btn_cat2_h.png", "d:/ymir work/ui/rank_button/btn_cat2_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat3_n.png", "d:/ymir work/ui/rank_button/btn_cat3_h.png", "d:/ymir work/ui/rank_button/btn_cat3_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat4_n.png", "d:/ymir work/ui/rank_button/btn_cat4_h.png", "d:/ymir work/ui/rank_button/btn_cat4_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat5_n.png", "d:/ymir work/ui/rank_button/btn_cat5_h.png", "d:/ymir work/ui/rank_button/btn_cat5_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat6_n.png", "d:/ymir work/ui/rank_button/btn_cat6_h.png", "d:/ymir work/ui/rank_button/btn_cat6_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat7_n.png", "d:/ymir work/ui/rank_button/btn_cat7_h.png", "d:/ymir work/ui/rank_button/btn_cat7_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat8_n.png", "d:/ymir work/ui/rank_button/btn_cat8_h.png", "d:/ymir work/ui/rank_button/btn_cat8_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat9_n.png", "d:/ymir work/ui/rank_button/btn_cat9_h.png", "d:/ymir work/ui/rank_button/btn_cat9_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat10_n.png", "d:/ymir work/ui/rank_button/btn_cat10_h.png", "d:/ymir work/ui/rank_button/btn_cat10_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat11_n.png", "d:/ymir work/ui/rank_button/btn_cat11_h.png", "d:/ymir work/ui/rank_button/btn_cat11_d.png"],
									#["d:/ymir work/ui/btn_cat12_n.png", "d:/ymir work/ui/btn_cat12_h.png", "d:/ymir work/ui/btn_cat12_d.png"],
									#["d:/ymir work/ui/btn_cat13_n.png", "d:/ymir work/ui/btn_cat13_h.png", "d:/ymir work/ui/btn_cat13_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat14_n.png", "d:/ymir work/ui/rank_button/btn_cat14_h.png", "d:/ymir work/ui/rank_button/btn_cat14_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat15_n.png", "d:/ymir work/ui/rank_button/btn_cat15_h.png", "d:/ymir work/ui/rank_button/btn_cat15_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat16_n.png", "d:/ymir work/ui/rank_button/btn_cat16_h.png", "d:/ymir work/ui/rank_button/btn_cat16_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat17_n.png", "d:/ymir work/ui/rank_button/btn_cat17_h.png", "d:/ymir work/ui/rank_button/btn_cat17_d.png"],
									["d:/ymir work/ui/rank_button/btn_cat18_n.png", "d:/ymir work/ui/rank_button/btn_cat18_h.png", "d:/ymir work/ui/rank_button/btn_cat18_d.png"]
	]

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.titlebar = None
		self.scrollbar = None
		self.scrollbarCat = None
		self.catBtn = None
		self.rankLines = None
		self.rankData = None
		self.rankDataBackground = None
		self.rankInfo = None
		self.category = 0
		self.isLoaded = 0
		self.waitTime = 0

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Load(self):
		if self.isLoaded:
			return

		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/rankingboardwindow_sys.py")
		except:
			import exception
			exception.Abort("RankingWindow.LoadDialog.LoadObject")

		try:
			self.titlebar = self.GetChild("titlebar")
			self.scrollbar = self.GetChild("scrollbar")
			self.scrollbarCat = self.GetChild("scrollbarCat")
			self.RankingInfo = self.GetChild("RankingInfo")
			self.catBtn = []
			for i in xrange(self.MAX_CATEGORIES_BTN):
				child = self.GetChild("btn_cat%d" % (i))
				self.catBtn.append(child)

			self.rankLines = []
			self.rankData = {
								"ID" : [],
								"NAME" : [],
								"LEVEL" : [],
			}

			self.rankDataBackground = {
								"ID" : [],
								"NAME" : [],
								"LEVEL" : [],
			}

			for i in xrange(14):
				lineStep = 24
				yPos = i * lineStep + 62
				fileName = "d:/ymir work/ui/public/ranking/rank_list.sub"
				if i == 13:
					yPos = (14) * lineStep + 50
					fileName = "d:/ymir work/ui/public/ranking/my_rank.sub"

				line = ui.MakeImageBox(self, fileName, 219, yPos)
				self.rankLines.append(line)

				rank = ui.Bar("TOP_MOST")
				rank.SetParent(self.rankLines[i])
				rank.SetPosition(12, 1)
				rank.SetSize(42, 17)
				rank.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				rank.Show()
				self.rankDataBackground["ID"].append(rank)

				name = ui.Bar("TOP_MOST")
				name.SetParent(self.rankLines[i])
				name.SetPosition(66, 1)
				name.SetSize(114, 17)
				name.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				name.Show()
				self.rankDataBackground["NAME"].append(name)

				level = ui.Bar("TOP_MOST")
				level.SetParent(self.rankLines[i])
				level.SetPosition(193, 1)
				level.SetSize(97, 17)
				level.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				level.Show()
				self.rankDataBackground["LEVEL"].append(level)

				ID = ui.MakeTextLine(self.rankDataBackground["ID"][i])
				NAME = ui.MakeTextLine(self.rankDataBackground["NAME"][i])
				LEVEL = ui.MakeTextLine(self.rankDataBackground["LEVEL"][i])

				self.rankData["ID"].append(ID)
				self.rankData["NAME"].append(NAME)
				self.rankData["LEVEL"].append(LEVEL)
		except:
			import exception
			exception.Abort("RankingWindow.LoadDialog.BindObject")

		self.tooltipItem = None
		self.tl = uiToolTip.ItemToolTip()
		self.tlInfo = uiToolTip.ItemToolTip()
		self.tl.Hide()
		self.tlInfo.Hide()
		self.tooltipItem = self.tl
		self.tooltipInfo = [self.tlInfo]*7
		self.InformationText = [localeInfo.RANKING_BILGI,
								localeInfo.RANKING_TEXT1,
								localeInfo.RANKING_TEXT2,
								localeInfo.RANKING_TEXT3,
								localeInfo.RANKING_TEXT4,
								localeInfo.RANKING_TEXT5,
								localeInfo.RANKING_TEXT6
		]

		for i in xrange(7):
			self.tooltipInfo[i].SetFollow(True)
			self.tooltipInfo[i].AlignHorizonalCenter()
			if i == 0:
				TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
				self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i], TITLE_COLOR)
			else:
				self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i])
			self.tooltipInfo[i].Hide()
			self.tooltipInfo[i].toolTipWidth += 10

		self.titlebar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.scrollbar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.scrollbarCat.SetScrollEvent(ui.__mem_func__(self.OnScrollCat))

		i = 0
		for item in self.catBtn:
			item.SetEvent(lambda arg = i: self.Choose(arg))
			i += 1

	def OnRunMouseWheel(self, nLen):
		if self.IsInPosition():
			cat = False
			for item in self.catBtn:
				if item.IsIn():
					cat = True
					break;
			subCat = False
			for item in self.rankLines:
				if item.IsIn():
					subCat = True
					break;
			if not subCat:
				for item in self.rankDataBackground["NAME"]:
					if item.IsIn():
						subCat = True
						break;
			if not subCat:
				for item in self.rankDataBackground["ID"]:
					if item.IsIn():
						subCat = True
						break;
			if not subCat:
				for item in self.rankDataBackground["LEVEL"]:
					if item.IsIn():
						subCat = True
						break;
			if nLen > 0:
				if subCat:
					self.scrollbar.OnUp()
					return True
				elif cat:
					self.scrollbarCat.OnUp()
					return True
			else:
				if subCat:
					self.scrollbar.OnDown()
					return True
				elif cat:
					self.scrollbarCat.OnDown()
					return True

		return False

	def ClearData(self):
		self.rankInfo = {
							"ID" : [],
							"NAME" : [],
							"EMPIRE" : [],
							"LEVEL" : [],
							"REAL_POSITION" : [],
		}
		
		for i in xrange(51):
			self.rankInfo["ID"].append("")
			self.rankInfo["NAME"].append("")
			self.rankInfo["EMPIRE"].append("")
			self.rankInfo["LEVEL"].append("")
			self.rankInfo["REAL_POSITION"].append("")

	def AddRank(self, line, name, points, level, realPosition):
		try:
			if realPosition != 0:
				self.rankInfo["ID"][50] = str(line)
				self.rankInfo["NAME"][50] = name
				self.rankInfo["EMPIRE"][50] = str(points)
				self.rankInfo["LEVEL"][50] = str(level)
				self.rankInfo["REAL_POSITION"][50] = str(realPosition)
			else:
				self.rankInfo["ID"][line] = str(line)
				self.rankInfo["NAME"][line] = name
				self.rankInfo["EMPIRE"][line] = str(points)
				self.rankInfo["LEVEL"][line] = str(level)
				self.rankInfo["REAL_POSITION"][line] = str(realPosition)
		except IndexError:
			return

	def RefreshList(self):
		i = 0
		c = 0
		for item in self.rankInfo["LEVEL"]:
			if self.rankInfo["LEVEL"][i] != "0" and i != 50:
				c += 1

			i += 1

		self.scrollbar.SetPos(0)
		if c < 14:
			self.scrollbar.SetMiddleBarSize(0.98)
		elif c == 14:
			size = float(13) / float(c)
			self.scrollbar.SetMiddleBarSize(size)
			self.scrollbar.Show()
		else:
			size = float(14) / float(c)
			self.scrollbar.SetMiddleBarSize(size)
			self.scrollbar.Show()

	def Destroy(self):
		self.Close()
		self.ClearDictionary()
		self.titlebar = None
		self.scrollbar = None
		self.scrollbarCat = None
		self.catBtn = None
		self.rankLines = None
		self.rankData = None
		self.rankDataBackground = None
		self.rankInfo = None
		self.category = 0
		self.isLoaded = 0

	def Open(self):
		if self.waitTime > app.GetGlobalTimeStamp():
			chat.AppendChat(1, localeInfo.TEKRAR_ARAMA_ICIN_BEKLE1 % (TIME_WAIT))
			return
		self.waitTime = app.GetGlobalTimeStamp() + TIME_WAIT

		if self.IsShow():
			self.Close()
			return

		if not self.isLoaded:
			self.Load()

		self.scrollbarCat.SetPos(0)
		size = float(9) / float(self.MAX_CATEGORIES_TO_VIEW)
		self.scrollbarCat.SetMiddleBarSize(size)

		self.Choose(0)
		self.OnScrollCat()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Choose(self, cat):
		for i in self.catBtn:
			i.SetUp()

		try:
			self.catBtn[cat].Down()
		except IndexError:
			return

		startIndex = int(float(self.MAX_CATEGORIES_TO_VIEW - 9) * self.scrollbarCat.GetPos())
		cat += startIndex
		self.category = cat
		net.SendChatPacket("/ranking_subcategory " + str(cat))

	def RefreshCatBtn(self):
		for i in self.catBtn:
			i.SetUp()

		startIndex = int(float(self.MAX_CATEGORIES_TO_VIEW - 9) * self.scrollbarCat.GetPos())
		if self.category-startIndex >= 0:
			try:
				self.catBtn[self.category-startIndex].Down()
			except IndexError:
				return

	def OnScrollCat(self):
		startIndex = int(float(self.MAX_CATEGORIES_TO_VIEW - 9) * self.scrollbarCat.GetPos())
		for i in xrange(self.MAX_CATEGORIES_BTN):
			idx = i + startIndex
			self.catBtn[i].SetText(self.TITLE_CATEGORIES_TO_VIEW[idx])
			self.catBtn[i].SetTextPosition(40, 35 / 2)
			self.catBtn[i].SetUpVisual(self.IMAGES_CATEGORIES_TO_VIEW[idx][0])
			self.catBtn[i].SetOverVisual(self.IMAGES_CATEGORIES_TO_VIEW[idx][1])
			self.catBtn[i].SetDownVisual(self.IMAGES_CATEGORIES_TO_VIEW[idx][2])
		
		self.RefreshCatBtn()

	def OnScroll(self):
		i = 0
		c = 0
		for item in self.rankInfo["LEVEL"]:
			if self.rankInfo["LEVEL"][i] != "0" and i != 50:
				c += 1
			
			i += 1

		startIndex = int(float(c - 13) * self.scrollbar.GetPos())
		j = 0
		m = False
		for i in xrange(startIndex, startIndex + 13):
			if i > 37 and j == 0:
				m = True

			if j == 12 and startIndex == 51:
				self.rankData["ID"][j].SetText("")
				self.rankData["NAME"][j].SetText("")
				self.rankData["LEVEL"][j].SetText("")
				break

			r = i
			if m:
				r = i - 13

			try:
				if str(int(self.rankInfo["ID"][r]) + 1) == "1":
					fileName = "d:/ymir work/ui/public/ranking/rank_list1.sub"
				elif str(int(self.rankInfo["ID"][r]) + 1) == "2":
					fileName = "d:/ymir work/ui/public/ranking/rank_list2.sub"
				elif str(int(self.rankInfo["ID"][r]) + 1) == "3":
					fileName = "d:/ymir work/ui/public/ranking/rank_list3.sub"
				else:
					fileName = "d:/ymir work/ui/public/ranking/rank_list.sub"

				self.rankLines[j].LoadImage(fileName)
				self.rankData["ID"][j].SetText(str(int(self.rankInfo["ID"][r]) + 1))
				self.rankData["NAME"][j].SetText(self.rankInfo["NAME"][r])
				self.rankData["LEVEL"][j].SetText(localeInfo.DottedNumber(self.rankInfo["EMPIRE"][r]))

				if self.rankInfo["LEVEL"][r] == "0":
					self.rankLines[j].Hide()
				else:
					self.rankLines[j].Show()

				j += 1
			except IndexError:
				pass

		try:
			if self.rankInfo["LEVEL"][50] == "0":
				self.rankLines[13].Hide()
			else:
				self.rankLines[13].Show()
			
			self.rankData["ID"][13].SetText(self.rankInfo["REAL_POSITION"][50])
			self.rankData["NAME"][13].SetText(self.rankInfo["NAME"][50])
			self.rankData["LEVEL"][13].SetText(localeInfo.DottedNumber(self.rankInfo["EMPIRE"][50]))
		except IndexError:
			pass

	def OnUpdate(self):
		for i in xrange(7):
			if self.RankingInfo.IsIn():
				self.tooltipInfo[i].Show()
			else:
				self.tooltipInfo[i].Hide()
