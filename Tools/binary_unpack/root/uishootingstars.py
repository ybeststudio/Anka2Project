# -*- coding: utf-8 -*-
"""Night-sky shooting star overlay (shooting_stars option).

This engine has no sky/weather particle system (no equivalent of the
reference's SetShootingStars -> sky particle call exists anywhere in
GameLib/EffectLib). Rather than leave the option a no-op or invent a new
3D particle subsystem blind (no way to verify DirectX-level rendering
changes without a live client), this reproduces the same *observable*
idea -- occasional streaking stars -- as a simple 2D screen-space overlay
built from existing UI primitives and an existing achievement-star icon.
This is a deliberate simplification, not a reconstruction of the
reference's own (3D, world-space) effect.
"""
import ui
import wndMgr
import app
# This embedded Python build does not package the stdlib "random" module;
# use the native app.GetRandom(min, max) already relied on elsewhere (game.py).

STAR_IMAGE = "d:/ymir work/ui/game/monster_card/achiev/achiev_apply_star1_img.sub"
MIN_INTERVAL = 6.0
MAX_INTERVAL = 16.0
TRAVEL_TIME = 0.9
STAR_SIZE = 10
MAX_CONCURRENT = 2


class _Star:
	def __init__(self, parent, now):
		# Deliberately NOT parented to the (invisible, 1x1) ShootingStarLayer window:
		# each star is its own independent top-level ImageBox so it never inherits a
		# large hit-test rectangle that could swallow world clicks/camera drag.
		self.image = ui.ExpandedImageBox()
		self.image.LoadImage(STAR_IMAGE)
		w = max(1.0, float(self.image.GetWidth()))
		h = max(1.0, float(self.image.GetHeight()))
		self.image.SetScale(STAR_SIZE / w, STAR_SIZE / h)
		self.startTime = now
		width = wndMgr.GetScreenWidth()
		self.startX = app.GetRandom(0, max(0, width - 160))
		self.startY = app.GetRandom(0, 60)
		self.dx = app.GetRandom(60, 140)
		self.dy = app.GetRandom(50, 90)
		self.image.SetPosition(self.startX, self.startY)
		self.image.Show()

	def Update(self, now):
		ratio = (now - self.startTime) / TRAVEL_TIME
		if ratio >= 1.0:
			return False
		self.image.SetPosition(int(self.startX + self.dx * ratio), int(self.startY + self.dy * ratio))
		alpha = 1.0 - abs(ratio - 0.5) * 2.0
		self.image.SetAlpha(max(0.0, min(1.0, alpha)))
		return True

	def Destroy(self):
		if self.image:
			self.image.Hide()
		self.image = None


class ShootingStarLayer(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ShootingStarLayer")
		# 1x1, off in the corner: this window exists only to receive the engine's
		# per-frame OnUpdate callback. It must NOT cover the screen -- a full-screen
		# ui.Window (even with no click handlers) can shadow world clicks/camera drag
		# under this engine's rectangle-based window picking.
		self.SetSize(1, 1)
		self.SetPosition(0, 0)
		self.stars = []
		self.nextSpawn = 0.0
		self.enabled = False
		self.outdoor = True
		self.starry = False   # baerim: "Show shooting stars in the Starry Night sky"
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		for star in self.stars:
			star.Destroy()
		self.stars = []

	def SetEnabled(self, flag):
		self.enabled = bool(flag)
		if not self.enabled:
			for star in self.stars:
				star.Destroy()
			self.stars = []

	def SetOutdoor(self, flag):
		self.outdoor = bool(flag)

	def SetStarrySky(self, flag):
		self.starry = bool(flag)
		if not self.starry:
			for star in self.stars:
				star.Destroy()
			self.stars = []

	def OnUpdate(self):
		if not self.enabled:
			return
		now = app.GetTime()
		self.stars = [star for star in self.stars if star.Update(now)]
		if not self.starry:
			return
		if now >= self.nextSpawn and len(self.stars) < MAX_CONCURRENT:
			self.stars.append(_Star(self, now))
			spanTenths = app.GetRandom(int(MIN_INTERVAL * 10), int(MAX_INTERVAL * 10))
			self.nextSpawn = now + spanTenths / 10.0

	def OnWarping(self):
		for star in self.stars:
			star.Destroy()
		self.stars = []
		self.nextSpawn = 0.0
