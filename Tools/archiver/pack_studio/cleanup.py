# Bozuk adlı kaynak dosyaları temizliği.
#   bozuk : adında U+FFFD var — unpack sırasında cp949 ad kaybolmuş; client bu dosyayı hiçbir adla
#           isteyemez, paketlenemez. Silmek güvenli.
#   kopya : (isteğe bağlı) Windows kopyaları: "x - Kopya.ext", "x - Copy (2).ext", "x - 복사본.ext"
#           ve bunun cp1252 mojibake hali ("»çº»"). Oyun bunları referans etmez.
# Gerçek Korece asset adları (örn. 'Á¤¸éÈ¥...gr2') dokunulmaz — client onları cp949 byte'larıyla ister.
# Eylem: karantina (_data/quarantine/<zaman>/<pack>/...) ya da kalıcı silme; boş kalan klasörler kaldırılır.
import os
import re
import shutil
import time

from . import config

_KO = ["복사본", "사본"]
_TOKENS = ["kopya", "copy"] + _KO + [t.encode("cp949").decode("cp1252", "replace") for t in _KO]
_RX_COPY = re.compile(r" - (?:%s)(?: \(\d+\))?(?:\.[^./]+)?$" % "|".join(re.escape(t) for t in _TOKENS), re.I)


def is_broken(rel):
    return "�" in rel


def is_copy(rel):
    return _RX_COPY.search(rel.rsplit("/", 1)[-1]) is not None


def find(specs, copies=False, cancel=None):
    """{pack: [(rel, sebep)]}"""
    out = {}
    for s in specs:
        base = config.long_path(s.source)
        hits = []
        for r, d, f in os.walk(base):
            if cancel and cancel.is_set():
                raise InterruptedError
            d[:] = [x for x in d if x.lower() not in config.GLOBAL_IGNORE_DIRS]
            rdir = os.path.relpath(r, base).replace("\\", "/")
            rdir = "" if rdir == "." else rdir + "/"
            for x in f:
                rel = rdir + x
                if is_broken(rel):
                    hits.append((rel, "bozuk ad"))
                elif copies and is_copy(rel):
                    hits.append((rel, "kopya"))
        if hits:
            out[s.name] = hits
    return out


def clean(found, specs, action="quarantine", log=print):
    by = {s.name: s for s in specs}
    qroot = os.path.join(config.DATA_DIR, "quarantine", time.strftime("%Y%m%d_%H%M%S"))
    total = 0
    for name, items in found.items():
        s = by[name]
        base = config.long_path(s.source)
        dirs = set()
        for rel, why in items:
            src = os.path.join(base, rel.replace("/", "\\"))
            if not os.path.exists(src):
                continue
            if action == "delete":
                os.chmod(src, 0o666)
                os.remove(src)
            else:
                dst = config.long_path(os.path.join(qroot, name, rel.replace("/", "\\")))
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                shutil.move(src, dst)
            dirs.add(os.path.dirname(src))
            total += 1
        for d in sorted(dirs, key=len, reverse=True):       # boş kalan klasörler
            while d.startswith(base) and d != base:
                try:
                    os.rmdir(d)
                except OSError:
                    break
                d = os.path.dirname(d)
        log("%s: %d dosya %s" % (name, len(items), "silindi" if action == "delete" else "karantinaya taşındı"))
    if total and action != "delete":
        log("karantina: %s" % qroot)
    return total
