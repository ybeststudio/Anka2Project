# -*- coding: utf-8 -*-
"""Baerim-style damage colour palette, backed by client-side preferences."""

import grp
import ui
import wndMgr
import advanced_options_runtime as options

ASSET = "d:/ymir work/ui/game/advanced_game_options/"
PALETTE = "d:/ymir work/ui/game/skillcolor/color_picker_background.tga"
KINDS = (
    ("normal", "Normal", (255, 255, 0)),
    ("critical", "Kritik", (255, 190, 0)),
    ("penetrate", "Delici", (80, 220, 255)),
    ("critical_penetrate", "Kritik + Delici", (255, 80, 255)),
    ("poison", "Zehir", (90, 220, 80)),
    ("received", "Al\xfdnan Hasar", (255, 80, 80)),
)

def _text(value):
    return value

def _color(kind):
    default = dict((name, rgb) for name, label, rgb in KINDS)[kind]
    return tuple(max(0, min(255, int(round(float(options.Get(
        "damage_%s_%s" % (kind, channel), default[index] / 255.0)) * 255))))
        for index, channel in enumerate("rgb"))

def _hsv_to_rgb(hue, saturation, value):
    sector = int(hue * 6.0) % 6
    fraction = hue * 6.0 - int(hue * 6.0)
    p = value * (1.0 - saturation)
    q = value * (1.0 - fraction * saturation)
    t = value * (1.0 - (1.0 - fraction) * saturation)
    return ((value, t, p), (q, value, p), (p, value, t),
            (p, q, value), (t, p, value), (value, p, q))[sector]

def _picked_rgb(picker):
    x, y = wndMgr.GetMousePosition()
    left, top = picker.GetGlobalPosition()
    hue = max(0.0, min(1.0, float(x - left) / 224.0))
    row = max(0.0, min(224.0, float(y - top)))
    saturation = min(1.0, row / 112.0)
    value = min(1.0, (224.0 - row) / 112.0)
    return tuple(int(round(c * 255)) for c in _hsv_to_rgb(hue, saturation, value))

class DamageColorWindow(ui.BoardWithTitleBar):
    def __init__(self):
        ui.BoardWithTitleBar.__init__(self)
        self.SetSize(330, 494)
        self.SetTitleName(_text("Hasar Renkleri"))
        self.SetCloseEvent(self.Hide)
        self.AddFlag("movable")
        self.SetCenterPosition()
        self.titleBar.imgLeft.LoadImage(ASSET + "panel_skin/titlebar_left.tga")
        self.titleBar.imgCenter.LoadImage(ASSET + "panel_skin/titlebar_center.tga")
        self.titleBar.imgRight.LoadImage(ASSET + "panel_skin/titlebar_right.tga")
        self.titleBar.btnClose.SetUpVisual(ASSET + "panel_skin/close_button_01.sub")
        self.titleBar.btnClose.SetOverVisual(ASSET + "panel_skin/close_button_02.sub")
        self.titleBar.btnClose.SetDownVisual(ASSET + "panel_skin/close_button_03.sub")
        self.titleBar.SetWidth(self.GetWidth() - 15)
        self.kind = "normal"
        self.current = dict((name, _color(name)) for name, label, rgb in KINDS)
        self.tabs = []
        self.parts = []
        for index, (name, label, default) in enumerate(KINDS):
            button = ui.Button()
            button.SetParent(self)
            button.SetPosition(15 + (index % 2) * 154, 29 + (index / 2) * 29)
            button.SetUpVisual(ASSET + "btn_0.png")
            button.SetOverVisual(ASSET + "btn_1.png")
            button.SetDownVisual(ASSET + "btn_2.png")
            button.SetText(_text(label))
            button.SAFE_SetEvent(self.SelectKind, name)
            button.Show()
            self.tabs.append(button)
        self._label(14, 122, "Se\xe7ilen renk", bold=True)
        self._bar(15, 138, 304, 28, 0xff302922)
        self.preview = self._bar(17, 140, 300, 24, 0xffffff00)

        self.image = ui.ImageBox()
        self.image.SetParent(self)
        self.image.SetPosition(50, 179)
        self.image.LoadImage(PALETTE)
        self.image.Show()
        self.picker = ui.Window()
        self.picker.SetParent(self.image)
        self.picker.SetPosition(0, 0)
        self.picker.SetSize(225, 225)
        self.picker.SetMouseLeftButtonDownEvent(self.PickColor)
        self.picker.Show()
        self._label(22, 417, "Bir sekme se\xe7in, ard\xfdndan rengini ayarlamak i\xe7in palete t\xfdklay\xfdn.")
        self._bar(105, 436, 120, 20, 0xff202020)
        self.hexText = ui.EditLine()
        self.hexText.SetParent(self)
        self.hexText.SetPosition(139, 439)
        self.hexText.SetSize(82, 17)
        self.hexText.SetMax(7)
        self.hexText.SetFontName("Tahoma:11")
        self.hexText.SetOutline()
        self.hexText.SetReturnEvent(self.ApplyHex)
        self.hexText.Show()
        self.parts.append(self.hexText)
        self._button(60, 461, "S\xfdf\xfdrla", self.ResetColor)
        self._button(172, 461, "Uygula", self.ApplyColor)
        self.SelectKind("normal")

    def _label(self, x, y, value, bold=False):
        label = ui.TextLine()
        label.SetParent(self)
        label.SetPosition(x, y)
        label.SetFontName("Tahoma:12b" if bold else "Tahoma:11")
        label.SetOutline()
        label.SetText(_text(value))
        label.Show()
        self.parts.append(label)
        return label

    def _bar(self, x, y, width, height, color):
        bar = ui.Bar()
        bar.SetParent(self)
        bar.SetPosition(x, y)
        bar.SetSize(width, height)
        bar.SetColor(color)
        bar.Show()
        self.parts.append(bar)
        return bar

    def _button(self, x, y, value, callback):
        button = ui.Button()
        button.SetParent(self)
        button.SetPosition(x, y)
        button.SetUpVisual("d:/ymir work/ui/game/common/button_small_norm.png")
        button.SetOverVisual("d:/ymir work/ui/game/common/button_small_hover.png")
        button.SetDownVisual("d:/ymir work/ui/game/common/button_small_down.png")
        button.SetText(_text(value))
        button.SAFE_SetEvent(callback)
        button.Show()
        self.parts.append(button)

    def SelectKind(self, kind):
        self.kind = kind
        self._refresh()

    def _refresh(self):
        r, g, b = self.current[self.kind]
        self.preview.SetColor(grp.GenerateColor(r / 255.0, g / 255.0, b / 255.0, 1.0))
        self.hexText.SetText("#%02X%02X%02X" % (r, g, b))
        for index, (name, label, default) in enumerate(KINDS):
            self.tabs[index].Down() if name == self.kind else self.tabs[index].SetUp()

    def PickColor(self):
        self.current[self.kind] = _picked_rgb(self.picker)
        self._refresh()

    def ResetColor(self):
        self.current[self.kind] = dict((name, rgb) for name, label, rgb in KINDS)[self.kind]
        self._refresh()

    def ApplyHex(self):
        value = self.hexText.GetText().strip()
        if len(value) == 7 and value[0] == "#":
            try:
                self.current[self.kind] = tuple(int(value[index:index + 2], 16)
                                                for index in (1, 3, 5))
            except ValueError:
                pass
        self._refresh()

    def ApplyColor(self):
        self.ApplyHex()
        for name, rgb in self.current.items():
            for index, channel in enumerate("rgb"):
                options.Set("damage_%s_%s" % (name, channel), rgb[index] / 255.0)
        options.Set("damage_color_custom", True)
        self.Hide()

    def OnPressEscapeKey(self):
        self.Hide()
        return True

class QueueColorWindow(DamageColorWindow):
    COLOR_PREFIX = "queue_color_"
    WINDOW_TITLE = "Queue-Metin Rengi"
    PICK_HINT = "Efekt rengini se\xe7mek i\xe7in palete t\xfdkla."

    def __init__(self):
        ui.BoardWithTitleBar.__init__(self)
        self.SetSize(270, 404)
        self.SetTitleName(self.WINDOW_TITLE)
        self.SetCloseEvent(self.Hide)
        self.AddFlag("movable")
        self.SetCenterPosition()
        self.titleBar.imgLeft.LoadImage(ASSET + "panel_skin/titlebar_left.tga")
        self.titleBar.imgCenter.LoadImage(ASSET + "panel_skin/titlebar_center.tga")
        self.titleBar.imgRight.LoadImage(ASSET + "panel_skin/titlebar_right.tga")
        self.titleBar.btnClose.SetUpVisual(ASSET + "panel_skin/close_button_01.sub")
        self.titleBar.btnClose.SetOverVisual(ASSET + "panel_skin/close_button_02.sub")
        self.titleBar.btnClose.SetDownVisual(ASSET + "panel_skin/close_button_03.sub")
        self.titleBar.SetWidth(self.GetWidth() - 15)
        self.parts = []
        self.currentColor = tuple(max(0, min(255, int(round(float(options.Get(
            self.COLOR_PREFIX + channel, 1.0)) * 255)))) for channel in "rgb")
        self._label(12, 34, "Se\xe7ilen renk", bold=True)
        self._bar(10, 47, 250, 29, 0xff302922)
        self.preview = self._bar(12, 49, 246, 25, 0xffffffff)
        self.image = ui.ImageBox()
        self.image.SetParent(self)
        self.image.SetPosition(20, 88)
        self.image.LoadImage(PALETTE)
        self.image.Show()
        self.picker = ui.Window()
        self.picker.SetParent(self.image)
        self.picker.SetPosition(0, 0)
        self.picker.SetSize(225, 225)
        self.picker.SetMouseLeftButtonDownEvent(self.PickColor)
        self.picker.Show()
        self._label(51, 328, self.PICK_HINT)
        self._bar(75, 343, 120, 20, 0xff202020)
        self.hexText = ui.EditLine()
        self.hexText.SetParent(self)
        self.hexText.SetPosition(109, 346)
        self.hexText.SetSize(82, 17)
        self.hexText.SetMax(7)
        self.hexText.SetFontName("Tahoma:11")
        self.hexText.SetOutline()
        self.hexText.SetReturnEvent(self.ApplyHex)
        self.hexText.Show()
        self.parts.append(self.hexText)
        self._button(29, 371, "S\xfdf\xfdrla", self.ResetColor)
        self._button(141, 371, "Uygula", self.ApplyColor)
        self._refresh()

    def _refresh(self):
        r, g, b = self.currentColor
        self.preview.SetColor(grp.GenerateColor(r / 255.0, g / 255.0, b / 255.0, 1.0))
        self.hexText.SetText("#%02X%02X%02X" % (r, g, b))

    def PickColor(self):
        self.currentColor = _picked_rgb(self.picker)
        self._refresh()

    def ResetColor(self):
        self.currentColor = (255, 255, 255)
        self._refresh()

    def ApplyHex(self):
        value = self.hexText.GetText().strip()
        if len(value) == 7 and value[0] == "#":
            try:
                self.currentColor = tuple(int(value[index:index + 2], 16)
                                          for index in (1, 3, 5))
            except ValueError:
                pass
        self._refresh()

    def ApplyColor(self):
        self.ApplyHex()
        for index, channel in enumerate("rgb"):
            options.Set(self.COLOR_PREFIX + channel, self.currentColor[index] / 255.0)
        self.Hide()

class EnvironmentColorWindow(QueueColorWindow):
    COLOR_PREFIX = "environment_color_"
    WINDOW_TITLE = "Ortam rengi"
    PICK_HINT = "3B ortama bir renk tonu uygular."
