if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import ui
import grp
import wndMgr

# Baerim introLoading (Cython) rebuilt from its runtime dump: paths and durations are theirs.
# Our client loads the map in C++, so the window only covers the screen until the game phase shows the map.

class LoadingLogoWindow(ui.Bar):
	BACKGROUND_IMAGE_PATHS = (
		"d:/ymir work/ui/loading_bg_1.png",
		"d:/ymir work/ui/loading_bg_2.png",
		"d:/ymir work/ui/loading_bg_3.png",
		"d:/ymir work/ui/loading_bg_4.png",
	)
	LOADING_REVEAL_DURATION = 0.65
	MAP_REVEAL_DURATION = 0.75

	def __init__(self, isWarp):
		ui.Bar.__init__(self, "TOP_MOST")
		self.isWarp = isWarp
		self.closeStartTime = 0.0
		self.closeCallback = None
		self.backgroundImage = None
		self.__Initialize()

	def __del__(self):
		ui.Bar.__del__(self)

	def __Initialize(self):
		width = wndMgr.GetScreenWidth()
		height = wndMgr.GetScreenHeight()

		self.AddFlag("not_pick")
		self.SetPosition(0, 0)
		self.SetSize(width, height)

		backgroundImage = ui.ExpandedImageBox()
		backgroundImage.SetParent(self)
		backgroundImage.AddFlag("not_pick")
		backgroundImage.LoadImage(self.BACKGROUND_IMAGE_PATHS[app.GetRandom(0, len(self.BACKGROUND_IMAGE_PATHS) - 1)])

		# cover the screen, crop the overflow evenly; read the size before SetScale (it resizes the window)
		imageWidth = float(backgroundImage.GetWidth())
		imageHeight = float(backgroundImage.GetHeight())
		scale = max(float(width) / imageWidth, float(height) / imageHeight)
		backgroundImage.SetScale(scale, scale)
		backgroundImage.SetPosition(int((width - imageWidth * scale) / 2), int((height - imageHeight * scale) / 2))
		backgroundImage.Show()
		self.backgroundImage = backgroundImage

		self.__SetShadeAlpha(1.0)
		self.Show()

	def Destroy(self):
		self.closeCallback = None
		self.backgroundImage = None
		self.Hide()

	def __SetShadeAlpha(self, alpha):
		self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, alpha))
		if self.backgroundImage:
			self.backgroundImage.SetAlpha(alpha)

	def __GetFadeProgress(self, duration):
		return min(1.0, (app.GetTime() - self.closeStartTime) / duration)

	def BeginClose(self, callback = None):
		if self.closeStartTime:
			return

		self.closeStartTime = app.GetTime()
		self.closeCallback = callback

	def OnUpdate(self):
		if not self.closeStartTime:
			return

		duration = self.MAP_REVEAL_DURATION if self.isWarp else self.LOADING_REVEAL_DURATION
		progress = self.__GetFadeProgress(duration)
		self.__SetShadeAlpha(1.0 - progress)

		if progress >= 1.0:
			callback = self.closeCallback
			self.Destroy()
			if callback:
				callback()

LOADING_LOGO = None

def __OnLogoClosed():
	global LOADING_LOGO
	LOADING_LOGO = None

def OpenLogo(isWarp):
	global LOADING_LOGO
	if LOADING_LOGO:
		LOADING_LOGO.Destroy()

	LOADING_LOGO = LoadingLogoWindow(isWarp)

def CloseLogo():
	if LOADING_LOGO:
		LOADING_LOGO.BeginClose(__OnLogoClosed)

class LoadingWindow(ui.Window):
	def __init__(self, stream, isWarp = FALSE):
		ui.Window.__init__(self)
		self.isWarp = isWarp

	def __del__(self):
		ui.Window.__del__(self)

	def Open(self):
		OpenLogo(self.isWarp)

	def Close(self):
		pass

	def OnPressEscapeKey(self):
		return TRUE
