# -*- coding: utf-8 -*-
"""Explicit Ctrl-click metin queue; native engine performs normal target attacks."""
_queue=[]
_modules={}
_current=0
_status=None
MAX_QUEUE=8

def Configure(**modules):
    global _status
    _modules.update(modules)
    if _status is None and 'ui' in _modules and 'wndMgr' in _modules:
        _status=_modules['ui'].TextLine(); _status.Hide()
def _Settings():
    import advanced_options_runtime
    return advanced_options_runtime
def Entries(): return list(_queue)
def _Mark():
    player=_modules.get('player')
    if player:
        for index,vid in enumerate(_queue): player.SetOptionQueueIndex(vid,index+1)
def Toggle(vid):
    player=_modules.get('player')
    if not player or not player.IsOptionQueueStoneAlive(vid): return False
    if vid in _queue:
        _queue.remove(vid); player.SetOptionQueueIndex(vid,0)
    elif len(_queue)<MAX_QUEUE: _queue.append(vid)
    else: return True
    _Mark(); Update(); return True
def Clear():
    global _current
    player=_modules.get('player')
    if player:
        for vid in _queue: player.SetOptionQueueIndex(vid,0)
        if _current: player.AttackOptionQueueStone(0)
    del _queue[:]; _current=0
def Stop():
    global _status
    Clear()
    if _status: _status.Hide()
    _status=None; _modules.clear()
def Update():
    global _current
    player=_modules.get('player')
    if not player: return
    previous=bool(_queue)
    for vid in list(_queue):
        if not player.IsOptionQueueStoneAlive(vid):
            _queue.remove(vid); player.SetOptionQueueIndex(vid,0)
    if previous and not _queue and _Settings().Get('queue_flash',True):
        _modules['app'].FlashApplication()
    target=_queue[0] if _queue else 0
    if target!=_current:
        player.AttackOptionQueueStone(target); _current=target; _Mark()
    if _status:
        _status.SetPosition(max(0,_modules['wndMgr'].GetScreenWidth()-300),245)
        _status.SetText('Metin sirasi: %d/%d  (Ctrl+Tik / Esc)'%(len(_queue),MAX_QUEUE))
        _status.Show() if _queue else _status.Hide()
