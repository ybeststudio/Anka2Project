# -*- coding: utf-8 -*-
"""Bounded notification history and category filters, driven by real game events."""
import os
import time
import uiScriptLocale

CATEGORIES = ("system", "dungeon", "guild", "item", "mining", "fishing", "battle_pass", "market_sales", "item_expired", "companion_expired", "alchemy_expired", "auto_cape")
LABELS = {
	"system": uiScriptLocale.NOTIFY_SYSTEM,
	"dungeon": uiScriptLocale.NOTIFY_DUNGEON,
	"guild": uiScriptLocale.NOTIFY_GUILD,
	"item": uiScriptLocale.NOTIFY_ITEM,
	"mining": uiScriptLocale.NOTIFY_MINING,
	"fishing": uiScriptLocale.NOTIFY_FISHING,
	"battle_pass": uiScriptLocale.NOTIFY_BATTLE_PASS,
	"market_sales": uiScriptLocale.NOTIFY_MARKET_SALES,
	"auto_cape": uiScriptLocale.NOTIFY_AUTO_CAPE,
	"item_expired": uiScriptLocale.NOTIFY_ITEM_EXPIRED,
	"companion_expired": uiScriptLocale.NOTIFY_COMPANION_EXPIRED,
	"alchemy_expired": uiScriptLocale.NOTIFY_ALCHEMY_EXPIRED,
}
MAX_RECORDS = 200
_records = []
_modules = {}
_history = None
_button = None
_filterDialog = None
_toast = None
_filename = "advanced_notifications.json"

try:
    text_type = unicode
except NameError:
    text_type = str

def _Runtime():
    import advanced_options_runtime
    return advanced_options_runtime

def Configure(**modules):
    _modules.update(modules)

def _Text(text):
    if isinstance(text, text_type): return text
    page = getattr(_modules.get("app"), "GetDefaultCodePage", lambda: 1254)()
    return text.decode("cp%d" % page, "replace")

def _Client(text):
    page = getattr(_modules.get("app"), "GetDefaultCodePage", lambda: 1254)()
    return _Text(text).encode("cp%d" % page, "replace")

def _Escape(text):
    return text.replace(u"\\", u"\\\\").replace(u"\t", u"\\t").replace(u"\n", u"\\n")

def _Unescape(text):
    return text.replace(u"\\n", u"\n").replace(u"\\t", u"\t").replace(u"\\\\", u"\\")

def Load(filename=None):
    # No "json" module in this embedded Python build; store one record per line,
    # tab-separated, same escaping convention as advanced_options_runtime.py's
    # own settings file.
    global _filename
    if filename is not None: _filename = filename
    del _records[:]
    try:
        stream = open(_filename, "rb")
    except IOError:
        return
    try:
        lines = stream.readlines()
    finally:
        stream.close()
    for raw in lines[-MAX_RECORDS:]:
        try:
            line = raw.decode("utf-8").rstrip(u"\r\n")
            when, category, text = line.split(u"\t", 2)
            if category not in CATEGORIES: continue
            _records.append((float(when), category, _Unescape(text)[:2048]))
        except (ValueError, TypeError, UnicodeDecodeError):
            continue

def Save():
    stream = open(_filename, "wb")
    try:
        for when, category, text in _records:
            line = u"%r\t%s\t%s\n" % (when, category, _Escape(text))
            stream.write(line.encode("utf-8"))
    finally:
        stream.close()

def Visible(category):
    settings = _Runtime()
    if category == "dungeon" and settings.Get("hide_dungeon_announcement", False): return False
    if category == "guild" and not settings.Get("show_guild_join_notification", True): return False
    return not settings.Get("hidden_notification_"+category, False)

def Record(category, text, when=None, popup=False):
    if category not in CATEGORIES: category = "system"
    text = _Text(text)[:2048]
    if not text: return False
    _records.append((time.time() if when is None else float(when), category, text))
    del _records[:-MAX_RECORDS]
    Save()
    if _history and _history.IsShow(): _history.Refresh()
    visible = Visible(category)
    if popup and visible and _toast:
        _toast.message.SetText(_Client(text.replace("\n"," ")[:48]))
        _toast.expires = time.time()+8
        _toast.Show()
    return visible

def Records(): return list(_records)

def Clear():
    del _records[:]
    Save()
    if _history: _history.Refresh()

def _MakeButton(ui, parent, text, x, y, callback):
    button = ui.Button()
    if parent is not None: button.SetParent(parent)
    button.SetPosition(x,y)
    path = "d:/ymir work/ui/game/advanced_game_options/"
    button.SetUpVisual(path+"btn_0.png"); button.SetOverVisual(path+"btn_1.png"); button.SetDownVisual(path+"btn_2.png")
    button.SetText(text); button.SetEvent(callback); button.Show()
    return button

def Start():
    global _toast
    if _toast: return
    Load()
    ui = _modules["ui"]
    class Toast(ui.Board):
        def __init__(self):
            ui.Board.__init__(self)
            self.SetSize(320,45); self.expires=0
            self.message=ui.TextLine(); self.message.SetParent(self)
            self.message.SetPosition(12,15); self.message.Show(); self.Hide()
        def OnUpdate(self):
            if time.time() >= self.expires: self.Hide()
    _toast=Toast()
    ApplyVisibility()

def BindButton(button):
    # The real trigger is GameButtonWindow's NotificationHistoryButton (bottom-left
    # stack, next to Statu Gelistir); this just lets the view_notification_history
    # option show/hide it like any other floating widget here.
    global _button
    _button = button
    ApplyVisibility()

def ApplyVisibility():
    wnd = _modules.get("wndMgr")
    if _toast and wnd: _toast.SetPosition(max(0,wnd.GetScreenWidth()-330), 190)
    if not _button: return
    if _Runtime().Get("view_notification_history", True): _button.Show()
    else:
        _button.Hide()
        if _history: _history.Hide()

def _RelativeTime(when):
    delta = max(0, int(time.time() - when))
    if delta < 60: return u"az once"
    if delta < 3600: return u"%d dakika once" % (delta // 60)
    if delta < 86400: return u"%d saat once" % (delta // 3600)
    return u"%d gun once" % (delta // 86400)

ROW_COUNT = 12

def OpenHistory():
    global _history
    ui = _modules["ui"]
    if _history:
        _history.Refresh(); _history.Show(); _history.SetTop(); return
    class History(ui.BoardWithTitleBar):
        def __init__(self):
            ui.BoardWithTitleBar.__init__(self)
            self.SetSize(470,420); self.SetTitleName(uiScriptLocale.NOTIFY_HISTORY_TITLE)
            self.AddFlag("movable"); self.SetCenterPosition(); self.SetCloseEvent(self.Hide)

            headerText = ui.TextLine(); headerText.SetParent(self)
            headerText.SetPosition(15,34); headerText.SetText(uiScriptLocale.NOTIFY_HISTORY_MESSAGE); headerText.Show()
            headerTime = ui.TextLine(); headerTime.SetParent(self)
            headerTime.SetPosition(360,34); headerTime.SetText(uiScriptLocale.NOTIFY_HISTORY_TIME); headerTime.Show()

            self.textLines = []; self.timeLines = []
            for i in range(ROW_COUNT):
                y = 58 + i*28
                text = ui.TextLine(); text.SetParent(self); text.SetPosition(15,y); text.Show()
                stamp = ui.TextLine(); stamp.SetParent(self); stamp.SetPosition(360,y); stamp.Show()
                self.textLines.append(text); self.timeLines.append(stamp)

            self.buttons = [_MakeButton(ui,self,uiScriptLocale.NOTIFY_HISTORY_CLEAR,150,390,Clear)]
            self.Refresh()
        def Refresh(self):
            rows = list(reversed(_records))[:ROW_COUNT]
            for index in range(ROW_COUNT):
                if index >= len(rows):
                    self.textLines[index].SetText(""); self.timeLines[index].SetText(""); continue
                when, category, text = rows[index]
                # Keep one row per event; avoid a message pushing controls away.
                text = text.replace("\r"," ").replace("\n"," ")[:55]
                self.textLines[index].SetText(_Client(text))
                self.timeLines[index].SetText(_Client(_RelativeTime(when)))
        def OnPressEscapeKey(self): self.Hide(); return True
    _history = History(); _history.Show()

def OpenFilters():
    global _filterDialog
    ui = _modules["ui"]
    if _filterDialog:
        _filterDialog.Refresh(); _filterDialog.Show(); _filterDialog.SetTop(); return
    class Filters(ui.BoardWithTitleBar):
        def __init__(self):
            ui.BoardWithTitleBar.__init__(self)
            self.SetSize(540, 90 + ((len(CATEGORIES)+1)//2)*35); self.SetTitleName(uiScriptLocale.NOTIFY_FILTER_TITLE)
            self.AddFlag("movable"); self.SetCenterPosition(); self.SetCloseEvent(self.Hide)
            self.buttons = []
            for i,category in enumerate(CATEGORIES):
                self.buttons.append((category,_MakeButton(ui,self,LABELS[category],20+(i%2)*260,45+(i//2)*35,lambda c=category:self.Toggle(c))))
            self.Refresh()
        def Refresh(self):
            for category,button in self.buttons:
                hidden = _Runtime().Get("hidden_notification_"+category,False)
                button.SetText(LABELS[category]+(uiScriptLocale.NOTIFY_HIDDEN if hidden else uiScriptLocale.NOTIFY_VISIBLE))
        def Toggle(self, category):
            key = "hidden_notification_"+category
            _Runtime().Set(key,not _Runtime().Get(key,False)); self.Refresh()
        def OnPressEscapeKey(self): self.Hide(); return True
    _filterDialog = Filters(); _filterDialog.Show()

def Stop():
    global _history, _button, _filterDialog, _toast
    # _button is GameButtonWindow's NotificationHistoryButton: owned by that window,
    # not by this module, so only the dialogs we actually created get torn down here.
    for window in (_history,_filterDialog,_toast):
        if window:
            window.Hide()
            if hasattr(window,"Destroy"): window.Destroy()
    _history = _button = _filterDialog = _toast = None
    _modules.clear()
