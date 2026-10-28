# Kaynak onarımı: eski liste XML'leri (xml/*.xml) "arşiv yolu -> kaynak yolu" eşlemesini tutar.
# Kaynakta olmayan ama pack'te bulunan dosyalar pack'ten çözülüp kaynak yoluna yazılır.
# Var olan dosyaların üzerine asla yazılmaz.
import html
import json
import os
import re

from . import config, foxfs

_RX_PACK = re.compile(rb'<CreateEterPack\s+ArchivePath="([^"]+)"')
_RX_FILE = re.compile(rb'<File\s+ArchivedPath="([^"]*)"\s*><!\[CDATA\[(.*?)\]\]></File>')


def _decode_path(b):
    for enc in ("mbcs", "cp1252", "cp949"):
        try:
            return b.decode(enc)
        except UnicodeDecodeError:
            continue
    return None


def collect(packs=None):
    """{pack_path: [(archived_bytes, source_abs)]} — aynı kaynak birden çok listede olabilir."""
    out = {}
    for fn in sorted(os.listdir(config.LEGACY_XML_DIR)):
        if not fn.lower().endswith(".xml"):
            continue
        data = open(os.path.join(config.LEGACY_XML_DIR, fn), "rb").read()
        m = _RX_PACK.search(data)
        if not m:
            continue
        pack = os.path.normpath(os.path.join(config.ARCHIVER_DIR, m.group(1).decode("latin1")))
        if packs and os.path.basename(pack) not in packs:
            continue
        lst = out.setdefault(pack, {})
        for arch, src in _RX_FILE.findall(data):
            arch = html.unescape(arch.decode("latin1")).encode("latin1")
            s = _decode_path(src)
            if s is None:
                continue
            lst[os.path.normpath(os.path.join(config.ARCHIVER_DIR, s))] = arch
    # Pack Studio ad hafızası (her paketlemede birikimli; eski listelerden daha güncel)
    from . import xmltools
    for name, (names, sources) in xmltools.all_manifests().items():
        try:
            pack = json.load(open(xmltools._manifest_path(name), encoding="utf-8")).get("pack")
        except (OSError, ValueError):
            continue
        if not pack or (packs and os.path.basename(pack) not in packs):
            continue
        lst = out.setdefault(os.path.normpath(pack), {})
        for h, arch in names.items():
            src = sources.get(str(h))
            if src:
                lst[os.path.normpath(src)] = arch
    return {p: [(a, s) for s, a in d.items()] for p, d in out.items()}


def missing_report(packs=None):
    rep = {}
    for pack, items in collect(packs).items():
        miss = [(a, s) for a, s in items if not os.path.exists(config.long_path(s))]
        rep[pack] = (len(items), miss)
    return rep


def restore(packs=None, log=print, cancel=None):
    total = 0
    for pack, (n, miss) in missing_report(packs).items():
        if not miss or not os.path.isfile(pack):
            continue
        written = notfound = 0
        with foxfs.PackReader(pack) as r:
            for arch, src in miss:
                if cancel and cancel.is_set():
                    return total
                e = r.entries.get(foxfs.name_hash(arch))
                if not e:
                    notfound += 1
                    continue
                data = r.read(e)
                if foxfs.content_hash(data) != e.digest:
                    notfound += 1
                    continue
                lp = config.long_path(src)
                os.makedirs(os.path.dirname(lp), exist_ok=True)
                with open(lp, "wb") as f:
                    f.write(data)
                written += 1
        total += written
        log("%s: %d dosya pack'ten kaynağa geri yazıldı, %d pack'te yok" % (os.path.basename(pack), written, notfound))
    return total
