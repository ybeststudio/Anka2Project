if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
player = __import__(pyapi.GetModuleName("player"))
import ui
import localeInfo
import uiScriptLocale

if app.ENABLE_RENDER_TARGET:
	import renderTarget


class CharacterTitleWindow(ui.ScriptWindow):
	RENDER_TARGET_INDEX = 4
	VISIBLE_COUNT = 10
	BUTTON_START_Y = 28
	BUTTON_STEP_Y = 22
	BUTTON_X = 4
	TITLE_SUB_PATH = "d:/ymir work/ui/game/title/titles/title_%04d.sub"
	TITLE_EFFECT_SPRITE_PATH = "d:/ymir work/ui/game/title/titles/sprites_%04d/%02d.sub"
	TITLE_NAMES_FILE = "title_names.txt"
	TITLE_STATE_FILE = "title_state.txt"
	STATE_ICON_PATH = {
		"locked": "d:/ymir work/ui/game/title/window/state_locked.sub",
		"hourglass": "d:/ymir work/ui/game/title/window/state_hourglass.sub",
		"infinite": "d:/ymir work/ui/game/title/window/state_infinite.sub",
	}
	UNLOCKED_TITLE_IDS = (
		
	)
	INFINITE_TITLE_IDS = ()
	TITLE_NAME_FALLBACK = {
		1000: "Sansli",
		1001: "Metin+",
		1002: "Destansi",
		1003: "Efsanevi",
		1004: "Mistik",
		1005: "Gunessever",
		2000: "Son kurtulan",
		2001: "Yenilmez",
		2002: "Savas habercisi",
		2003: "Kasap",
		3000: "Metin+",
		3001: "Destansi",
		3002: "Efsanevi",
		3003: "Mistik",
		3004: "Sansli",
	}

	TITLE_IDS = (
		1000, 1001, 1002, 1003, 1004, 1005,
		2000, 2001, 2002, 2003,
		3000, 3001, 3002, 3003, 3004,
	)

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		self.selectedTitleID = 0
		self.baseIndex = 0
		self.titleButtons = []
		self.equippedName = None
		self.applyButton = None
		self.clearButton = None
		self.backButton = None
		self.mainBoard = None
		self.listBoard = None
		self.scrollBar = None
		self.rightBoard = None
		self.previewImage = None
		self.previewEffect = None
		self.previewTitleBar = None
		self.previewTitleName = None
		self.modelRenderTarget = None
		self.unlockedTitleIDs = set(self.UNLOCKED_TITLE_IDS)
		self.infiniteTitleIDs = set(self.INFINITE_TITLE_IDS)
		self.titleNameByID = dict(self.TITLE_NAME_FALLBACK)
		self.activeTitleID = 0
		self.__LoadTitleNames()
		self.__LoadTitleState()
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		self.titleButtons = []
		self.equippedName = None
		self.applyButton = None
		self.clearButton = None
		self.backButton = None
		self.mainBoard = None
		self.listBoard = None
		self.scrollBar = None
		self.rightBoard = None
		self.previewImage = None
		self.previewEffect = None
		self.previewTitleBar = None
		self.previewTitleName = None
		self.modelRenderTarget = None
		self.isLoaded = False

	def __LoadWindow(self):
		if self.isLoaded:
			return

		try:
			if localeInfo.IsARABIC() or localeInfo.IsVIETNAM() or localeInfo.IsJAPAN():
				self.__LoadScript(uiScriptLocale.LOCALE_UISCRIPT_PATH + "CharacterTitleWindow.py")
			else:
				self.__LoadScript("UIScript/CharacterTitleWindow.py")
		except:
			import exception
			exception.Abort("CharacterTitleWindow.__LoadWindow")

		self.mainBoard = self.GetChild("main_board")
		self.listBoard = self.GetChild("list_board")
		self.equippedName = self.GetChild("equipped_name")
		self.applyButton = self.GetChild("apply_button")
		self.clearButton = self.GetChild("clear_button")
		self.backButton = self.GetChild("back_button")
		self.scrollBar = self.GetChild("scroll_bar")
		self.rightBoard = self.GetChild("right_board")
		self.modelRenderTarget = self.GetChild("model_render_target")

		self.applyButton.SetEvent(ui.__mem_func__(self.__OnClickApply))
		self.clearButton.SetEvent(ui.__mem_func__(self.__OnClickClear))
		self.backButton.SetEvent(ui.__mem_func__(self.Close))
		self.GetChild("main_title_bar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScrollList))

		self.__BuildTitleList()
		self.__SetupPreview()
		self.__RefreshScrollBar()
		self.__RefreshList()
		self.__RefreshRenderTarget()
		self.isLoaded = True

	def __LoadScript(self, fileName):
		loader = ui.PythonScriptLoader()
		loader.LoadScriptFile(self, fileName)

	def __LoadTitleNames(self):
		try:
			lines = pack_open(self.TITLE_NAMES_FILE, "r").readlines()
		except:
			return

		for rawLine in lines:
			try:
				line = rawLine.strip()
			except:
				continue
			if not line or line.startswith("#"):
				continue

			parts = line.split(None, 1)
			if len(parts) < 2:
				continue

			try:
				titleID = int(parts[0])
			except:
				continue

			titleName = parts[1].strip()
			if not titleName:
				continue

			self.titleNameByID[titleID] = titleName

	def __GetStateKey(self):
		try:
			return str(player.GetName())
		except:
			return "default"

	def __LoadTitleState(self):
		try:
			lines = pack_open(self.TITLE_STATE_FILE, "r").readlines()
		except:
			try:
				lines = open(self.TITLE_STATE_FILE, "r").readlines()
			except:
				return

		stateMap = {}
		for rawLine in lines:
			try:
				line = rawLine.strip()
			except:
				continue
			if not line or line.startswith("#"):
				continue

			parts = line.split(None, 1)
			if len(parts) < 2:
				continue

			key = parts[0]
			try:
				titleID = int(parts[1])
			except:
				continue
			stateMap[key] = titleID

		activeID = stateMap.get(self.__GetStateKey(), 0)
		if activeID in self.TITLE_IDS:
			self.activeTitleID = activeID
			self.selectedTitleID = activeID

	def __SaveTitleState(self):
		lines = []
		try:
			lines = open(self.TITLE_STATE_FILE, "r").readlines()
		except:
			lines = []

		stateMap = {}
		for rawLine in lines:
			try:
				line = rawLine.strip()
			except:
				continue
			if not line or line.startswith("#"):
				continue
			parts = line.split(None, 1)
			if len(parts) < 2:
				continue
			try:
				stateMap[parts[0]] = int(parts[1])
			except:
				continue

		stateMap[self.__GetStateKey()] = int(self.activeTitleID)

		try:
			f = open(self.TITLE_STATE_FILE, "w")
			for key, value in stateMap.items():
				f.write("%s %d\n" % (key, value))
			f.close()
		except:
			pass

	def __SafeCenterX(self, areaWidth, contentWidth):
		try:
			area = int(areaWidth)
		except:
			area = 0
		try:
			content = int(contentWidth)
		except:
			content = 0

		pos = (area - content) / 2
		if pos < 0:
			return 0
		return int(pos)

	def __BuildTitleList(self):
		for idx, titleID in enumerate(self.TITLE_IDS):
			btn = ui.Button()
			btn.SetParent(self.listBoard)
			btn.SetPosition(self.BUTTON_X, self.BUTTON_START_Y + idx * self.BUTTON_STEP_Y)
			btn.SetUpVisual("d:/ymir work/ui/game/title/window/list_button_default.sub")
			btn.SetOverVisual("d:/ymir work/ui/game/title/window/list_button_over.sub")
			btn.SetDownVisual("d:/ymir work/ui/game/title/window/list_button_down.sub")
			btn.SetEvent(ui.__mem_func__(self.__SelectTitle), titleID)

			titleText = ui.TextLine()
			titleText.SetParent(btn)
			titleText.SetHorizontalAlignCenter()
			titleText.SetVerticalAlignCenter()
			titleText.SetPosition(98, 11)
			titleText.SetText(self.__GetTitleName(titleID))
			titleText.Show()

			titleImage = ui.ImageBox()
			titleImage.SetParent(btn)
			titleImage.AddFlag("not_pick")
			try:
				titleImage.LoadImage(self.TITLE_SUB_PATH % titleID)
				titleImage.SetPosition(self.__SafeCenterX(196, titleImage.GetWidth()), 2)
				titleImage.Show()
				titleText.Hide()
			except:
				titleImage.Hide()

			stateIcon = ui.ImageBox()
			stateIcon.SetParent(btn)
			stateIcon.AddFlag("not_pick")
			stateIcon.SetPosition(176, 3)
			stateIcon.Show()

			btn.Show()
			self.titleButtons.append({
				"id": titleID,
				"button": btn,
				"text": titleText,
				"image": titleImage,
				"state": stateIcon,
			})

	def __RefreshScrollBar(self):
		if not self.scrollBar:
			return

		totalCount = len(self.titleButtons)
		if totalCount <= self.VISIBLE_COUNT:
			self.scrollBar.Hide()
			self.baseIndex = 0
			return

		self.scrollBar.Show()
		self.scrollBar.SetMiddleBarSize(float(self.VISIBLE_COUNT) / float(totalCount))
		self.scrollBar.SetPos(0.0)

	def __OnScrollList(self):
		maxBaseIndex = max(0, len(self.titleButtons) - self.VISIBLE_COUNT)
		self.baseIndex = int(self.scrollBar.GetPos() * maxBaseIndex + 0.5)
		self.__RefreshList()

	def __RefreshList(self):
		for idx, entry in enumerate(self.titleButtons):
			btn = entry["button"]
			titleID = entry["id"]
			if idx < self.baseIndex or idx >= self.baseIndex + self.VISIBLE_COUNT:
				btn.Hide()
				continue

			showIndex = idx - self.baseIndex
			btn.SetPosition(self.BUTTON_X, self.BUTTON_START_Y + showIndex * self.BUTTON_STEP_Y)
			btn.Show()

			isLocked = self.__IsTitleLocked(titleID)
			stateType = self.__GetTitleStateType(titleID)
			if isLocked:
				btn.SetUpVisual("d:/ymir work/ui/game/title/window/list_button_default_disable.sub")
				btn.SetOverVisual("d:/ymir work/ui/game/title/window/list_button_default_disable.sub")
				btn.SetDownVisual("d:/ymir work/ui/game/title/window/list_button_default_disable.sub")
			elif titleID == self.activeTitleID:
				btn.SetUpVisual("d:/ymir work/ui/game/title/window/list_button_default_equipped.sub")
				btn.SetOverVisual("d:/ymir work/ui/game/title/window/list_button_over.sub")
				btn.SetDownVisual("d:/ymir work/ui/game/title/window/list_button_down.sub")
			else:
				btn.SetUpVisual("d:/ymir work/ui/game/title/window/list_button_default.sub")
				btn.SetOverVisual("d:/ymir work/ui/game/title/window/list_button_over.sub")
				btn.SetDownVisual("d:/ymir work/ui/game/title/window/list_button_down.sub")

			if stateType:
				try:
					entry["state"].LoadImage(self.STATE_ICON_PATH[stateType])
					entry["state"].Show()
				except:
					entry["state"].Hide()
			else:
				entry["state"].Hide()

			if titleID == self.selectedTitleID and not isLocked:
				btn.Down()
			else:
				btn.SetUp()

	def __GetTitleName(self, titleID):
		return self.titleNameByID.get(titleID, "Unvan %d" % titleID)

	def __IsTitleLocked(self, titleID):
		return titleID not in self.unlockedTitleIDs

	def __GetTitleStateType(self, titleID):
		if self.__IsTitleLocked(titleID):
			return "locked"
		if titleID in self.infiniteTitleIDs:
			return "infinite"
		return "hourglass"

	def __SetupPreview(self):
		self.previewTitleName = ui.TextLine()
		self.previewTitleName.SetParent(self.rightBoard)
		self.previewTitleName.SetHorizontalAlignCenter()
		self.previewTitleName.SetPosition(112, 15)
		self.previewTitleName.SetText("")
		self.previewTitleName.Show()

		self.previewImage = ui.ImageBox()
		self.previewImage.SetParent(self.rightBoard)
		self.previewImage.AddFlag("not_pick")
		self.previewImage.SetPosition(8, 120)
		self.previewImage.Show()
		self.__RefreshPreview()

	def __RefreshPreview(self):
		if not self.previewImage:
			return

		targetID = self.selectedTitleID if self.selectedTitleID > 0 else self.TITLE_IDS[0]
		try:
			self.previewImage.LoadImage(self.TITLE_SUB_PATH % targetID)
			self.previewImage.SetPosition(self.__SafeCenterX(224, self.previewImage.GetWidth()), 146)
			self.previewImage.Show()
		except:
			self.previewImage.Hide()

		if self.previewTitleName:
			self.previewTitleName.SetText(self.__GetTitleName(targetID))

		self.__RebuildPreviewEffect(targetID)

	def __RebuildPreviewEffect(self, titleID):
		if self.previewEffect:
			self.previewEffect.Hide()
			self.previewEffect = None

		effectTitleCandidates = [int(titleID)]
		if titleID not in (1000, 1001, 1002):
			effectTitleCandidates.append(1000 + (int(titleID) % 3))

		effect = ui.AniImageBox()
		effect.SetParent(self.rightBoard)
		effect.AddFlag("not_pick")
		effect.SetDelay(10)
		frameCount = 0
		loadedTitleID = int(titleID)
		for candidateID in effectTitleCandidates:
			frameCount = 0
			for frame in xrange(36):
				try:
					effect.AppendImage(self.TITLE_EFFECT_SPRITE_PATH % (candidateID, frame))
					frameCount += 1
				except:
					break
			if frameCount > 0:
				loadedTitleID = candidateID
				break

		if frameCount <= 0:
			return

		effectWidth = 0
		try:
			effectWidth = effect.GetWidth()
		except:
			effectWidth = 0

		if effectWidth <= 0:
			probe = ui.ImageBox()
			probe.SetParent(self.rightBoard)
			probe.AddFlag("not_pick")
			try:
				probe.LoadImage(self.TITLE_EFFECT_SPRITE_PATH % (loadedTitleID, 0))
				effectWidth = probe.GetWidth()
			except:
				effectWidth = 96
			probe.Hide()

		effect.SetPosition(self.__SafeCenterX(224, effectWidth), 104)
		effect.Show()
		self.previewEffect = effect

	def __RefreshRenderTarget(self):
		if not app.ENABLE_RENDER_TARGET:
			return

		try:
			renderTarget.SetBackground(self.RENDER_TARGET_INDEX, "d:/ymir work/ui/game/title/window/model_view_background.sub")
		except:
			pass

		try:
			renderTarget.SetRotation(self.RENDER_TARGET_INDEX, False)
		except:
			pass

		try:
			renderTarget.SetScale(self.RENDER_TARGET_INDEX, 1.0)
		except:
			pass

		try:
			renderTarget.ResetModel(self.RENDER_TARGET_INDEX)
		except:
			pass

		try:
			renderTarget.SetVisibility(self.RENDER_TARGET_INDEX, True)
		except:
			pass

	def __SelectTitle(self, titleID):
		if self.__IsTitleLocked(titleID):
			return

		self.selectedTitleID = int(titleID)
		self.__RefreshList()
		self.__RefreshPreview()
		if self.equippedName:
			self.equippedName.SetText(self.__GetTitleName(self.selectedTitleID))

	def __OnClickApply(self):
		if self.selectedTitleID <= 0:
			return
		if self.__IsTitleLocked(self.selectedTitleID):
			return
		if self.activeTitleID == self.selectedTitleID:
			return
		self.activeTitleID = self.selectedTitleID
		self.selectedTitleID = self.activeTitleID
		self.__SaveTitleState()
		self.__RefreshList()
		net.SendChatPacket("/title equip %d" % self.selectedTitleID)

	def __OnClickClear(self):
		if self.activeTitleID <= 0:
			self.selectedTitleID = 0
			self.__RefreshList()
			self.__RefreshPreview()
			if self.equippedName:
				self.equippedName.SetText("")
			return

		net.SendChatPacket("/title clear")
		self.activeTitleID = 0
		self.selectedTitleID = 0
		self.__SaveTitleState()
		self.__RefreshList()
		self.__RefreshPreview()
		if self.equippedName:
			self.equippedName.SetText("")

	def SetUnlockedTitleIDs(self, unlockedIDs):
		try:
			self.unlockedTitleIDs = set(int(v) for v in unlockedIDs)
		except:
			return
		self.__RefreshList()
		self.__RefreshPreview()

	def SetActiveTitleID(self, activeTitleID):
		try:
			self.activeTitleID = int(activeTitleID)
		except:
			self.activeTitleID = 0

		if self.activeTitleID not in self.TITLE_IDS:
			self.activeTitleID = 0

		if self.selectedTitleID <= 0 and self.activeTitleID > 0:
			self.selectedTitleID = self.activeTitleID

		self.__SaveTitleState()
		self.__RefreshList()
		self.__RefreshPreview()
		if self.equippedName:
			if self.activeTitleID > 0:
				self.equippedName.SetText(self.__GetTitleName(self.activeTitleID))
			else:
				self.equippedName.SetText("")

	def Open(self):
		net.SendChatPacket("/title sync")
		self.__RefreshRenderTarget()
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()
