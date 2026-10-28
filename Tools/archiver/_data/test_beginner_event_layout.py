import builtins
import json as test_json
import os
import sys
import types

builtins.xrange = range
builtins.__USE_DYNAMIC_MODULE__ = False

class Widget(object):
    def __init__(self):
        self.visible = False
        self.items = {}
    def __del__(self): pass
    def __getattr__(self, key):
        if key.startswith('Set') or key in ('AddFlag', 'LoadImage', 'AppendSlot', 'RefreshSlot', 'ClearDictionary'):
            return lambda *args: None
        raise AttributeError(key)
    def SetPosition(self, x, y): self.position = (x, y)
    def GetLocalPosition(self): return getattr(self, 'position', (0, 0))
    def SetSize(self, width, height): self.size = (width, height)
    def SetCenterPosition(self, x=0, y=0):
        self.position = ((800 - self.size[0]) // 2 + x, (600 - self.size[1]) // 2 + y)
    def SetParent(self, parent): self.parent = parent
    def SetEvent(self, event, *args): self.clickEvent = (event, args)
    def LoadImage(self, path): self.path = path
    def SetText(self, text): self.text = text
    def SetPackedFontColor(self, color): self.color = color
    def SetItemSlot(self, index, vnum, count): self.items[index] = (vnum, count)
    def SetSlotBaseImage(self, path, r, g, b, a): self.slotBase = path
    def Show(self): self.visible = True
    def Hide(self): self.visible = False
    def GetPos(self): return getattr(self, 'pos', 0)
    def SetPos(self, pos):
        self.pos = pos
        self.event()
    def SetScrollEvent(self, event): self.event = event

ui = types.ModuleType('ui')
for name in ('Window','ScriptWindow','BoardWithTitleBar','ImageBox','TextLine','Button','SlotWindow','ThinScrollBar','Bar','DragButton'):
    setattr(ui, name, type(name, (Widget,), {}))
def unsupported_thumb_resize(self, *args):
    raise AttributeError("'MiddleBar' object has no attribute 'bottom'")
ui.ThinScrollBar.SetMiddleBarSize = unsupported_thumb_resize
ui.__mem_func__ = lambda fn: fn
sys.modules['ui'] = ui
sys.modules['json'] = None
wiki = types.ModuleType('wikicontent')
wiki.LoadCommonJson = lambda name: test_json.load(open(os.path.join(r'C:\Anka2Project\Tools\binary_unpack\locale\common', name.replace('/', os.sep))))
sys.modules['wikicontent'] = wiki
pyapi = types.ModuleType('pyapi')
pyapi.GetModuleName = lambda name: name
builtins.pyapi = pyapi
sys.modules['app'] = types.ModuleType('app')

locale = types.ModuleType('localeInfo')
for key in ('METINSTONES','BOSSES','DUNGEONS','UPGRADE_ITEM','EXPLORATION','SPIRIT'):
    setattr(locale, 'BEGINNER_EVENT_' + key, key)
sys.modules['localeInfo'] = locale
ui_locale = types.ModuleType('uiScriptLocale')
ui_locale.BEGINNER_EVENT_TITLE = 'event'
ui_locale.BEGINNER_EVENT_DAILY_QUEST = 'daily'
ui_locale.BEGINNER_EVENT_REWARD_HINT = 'hint'
sys.modules['uiScriptLocale'] = ui_locale
dbg = types.ModuleType('dbg')
dbg.TraceError = lambda error: (_ for _ in ()).throw(AssertionError(error))
sys.modules['dbg'] = dbg

base = r'C:\Anka2Project\Tools\binary_unpack'
builtins.pack_open = lambda name, mode: open(os.path.join(base, name.replace('/', os.sep)), mode)
source = os.path.join(base, 'root', 'uiBeginnerEvent.py')
module = types.ModuleType('uiBeginnerEvent')
exec(compile(open(source, 'rb').read(), source, 'exec'), module.__dict__)
window = module.BeginnerEventWindow()
assert len(window.rewardData) == 9
assert len(window.rewardRows) == 6
assert window.rewardScroll.size == (5, 326)
assert window.rewardScroll.position == (337, 35)
assert window.rewardScroll.thumbHeight == 227
assert all(row[1].size == (96, 32) for row in window.rewardRows)
assert all(row[1].slotBase.endswith('/Slot_Base.sub') for row in window.rewardRows)
assert not any(widget.visible for widget in window.rewardWidgets)
assert len([widget for widget in window.questWidgets if getattr(widget, 'text', None) == '10']) == 6
assert window.size == (388, 385)
assert window.board.position == (28, 0)
assert window.position[0] + window.board.position[0] == 220  # old centered board position
assert all(0 <= button.position[0] < window.size[0] for button in window.tabButtons)
prices = [widget for widget in window.questWidgets if getattr(widget, 'text', None) == '10']
assert [price.position for price in prices] == [(22, 123), (132, 123), (242, 123), (22, 238), (132, 238), (242, 238)]
window.SetClaimedLevels((10,20,30))
assert not any(widget.visible for widget in window.rewardWidgets)
event, args = window.tabButtons[1].clickEvent
event(*args)  # exercise the button callback used by the mouse click
assert window.currentTab == 1
assert all(row[0].visible for row in window.rewardRows)
assert [row[1].items[0] for row in window.rewardRows] == [(76023,100),(76023,200),(60050,3),(65152,5),(71052,200),(65181,3)]
assert [row[2].visible for row in window.rewardRows] == [True,True,True,False,False,False]
window.rewardScroll.SetPos(1)
assert [os.path.basename(row[0].path) for row in window.rewardRows] == ['level_40.png','level_50.png','level_60.png','level_70.png','level_80.png','level_90.png']
assert not any(row[2].visible for row in window.rewardRows)
window.rewardScroll.thumb.SetPosition(1, 0)
window.rewardScroll._BeginnerRewardScrollBar__OnMove()
assert window.rewardScroll.GetPos() == 0
assert os.path.basename(window.rewardRows[0][0].path) == 'level_10.png'
event, args = window.tabButtons[0].clickEvent
event(*args)
assert not any(widget.visible for widget in window.rewardWidgets)
print('Beginner event layout/state smoke OK')
