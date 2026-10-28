# property.xml üreteci (client: GameLib/PropertyManager.cpp, ENABLE_FOXFS_ENCRYPT).
#
# Client root pack'teki "property.xml"i okur; her <Property> CRC'siyle kaydedilir, haritalardaki
# bina/ağaç/efekt/ambiyans objeleri bu CRC ile çağrılır -> listede olmayan property oyunda görünmez.
#
# Kaynak: property klasöründeki her YPRT dosyası (.prb/.prt/.pre/.pra/.prd ...)
#   YPRT\n<crc>\n<anahtar>\t\t"<değer>"\n ...   (CProperty::ReadFromMemory ile aynı ayrıştırma:
#   boşlukla bölünür, tırnak gruplar, anahtar küçük harf)
# + (isteğe bağlı) property pack'inde olup kaynakta olmayan entry'ler — liste pack ile tam eşleşir.
# Çıktı byte-düzeyinde yazılır (cp949 Korece adlar/değerler bozulmaz), sıralama deterministik.
import os
import re

from . import config, foxfs

_RX_TOK = re.compile(rb'"([^"]*)"|(\S+)')


def parse(data):
    """YPRT içeriği -> (crc_bytes, {anahtar: değer}) ya da None"""
    if not data.startswith(b"YPRT"):
        return None
    body = data[4:]
    if body.startswith(b"\r\n"):
        body = body[2:]
    elif body[:1] in (b"\n", b"\r"):
        body = body[1:]
    else:
        return None
    lines = body.splitlines()
    if not lines:
        return None
    crc = lines[0].strip()
    if not crc.isdigit():
        return None
    attrs = {}
    for line in lines[1:]:
        toks = [a if a or not b else b for a, b in _RX_TOK.findall(line)]
        if not toks:
            continue
        attrs[toks[0].lower()] = b" ".join(toks[1:])
    return crc, attrs


def _esc(b):
    return b.replace(b"&", b"&amp;").replace(b"<", b"&lt;").replace(b">", b"&gt;").replace(b'"', b"&quot;")


_RX_ELEM = re.compile(rb"<Property\s[^>]*?/>", re.S)
_RX_CRC = re.compile(rb'\scrc\s*=\s*"(\d+)"')
_RX_FN = re.compile(rb'\sfilename\s*=\s*"([^"]*)"')
_RX_BARE_AMP = re.compile(rb"&(?!(?:amp|lt|gt|quot|apos|#\d+|#x[0-9a-fA-F]+);)")


def existing_elements(xml_bytes):
    """Mevcut property.xml -> {crc: (filename, element_bytes)} (birleştirmede aynen korunur).
    Eski PropertyGen '&' karakterini escape etmemiş; rapidxml bunu olduğu gibi okur, '&amp;' ile aynı
    değeri verir -> çıktı geçerli XML olsun diye düzeltilir."""
    out = {}
    for m in _RX_ELEM.finditer(xml_bytes or b""):
        el = _RX_BARE_AMP.sub(b"&amp;", m.group(0))
        c = _RX_CRC.search(el)
        if c:
            fn = _RX_FN.search(el)
            out[c.group(1)] = (fn.group(1) if fn else b"", el)
    return out


def generate(spec, include_pack=True, merge=None, log=None):
    """-> (xml_bytes, istatistik dict)
    merge: mevcut property.xml byte'ları — kaynakta karşılığı olmayan girdiler aynen korunur
    (kaynaktan silinmiş property'ler haritalardan kaybolmasın). None = sıfırdan."""
    stats = {"files": 0, "properties": 0, "from_pack": 0, "kept": 0, "broken_names": 0, "skipped": 0,
             "duplicate_crc": 0, "types": {}}
    props = {}                          # crc -> (filename_bytes, attrs)
    seen_names = set()
    base = config.long_path(spec.source)
    for r, d, f in os.walk(base):
        d[:] = [x for x in d if x.lower() not in config.GLOBAL_IGNORE_DIRS]
        for x in f:
            rel = os.path.relpath(os.path.join(r, x), base).replace("\\", "/")
            nb = config.encode_name(spec.archived(rel))
            if nb is None:
                # adı bozuk: client property'yi dosyadan değil property.xml'den okur -> içerik yeterli
                nb = spec.archived(rel).encode("ascii", "replace")
                stats["broken_names"] += 1
            else:
                seen_names.add(foxfs.name_hash(nb))
            stats["files"] += 1
            with open(os.path.join(r, x), "rb") as fh:
                head = fh.read(4)
                if head != b"YPRT":
                    continue
                parsed = parse(head + fh.read())
            if not parsed:
                stats["skipped"] += 1
                continue
            crc, attrs = parsed
            if crc in props:
                stats["duplicate_crc"] += 1
            props[crc] = (nb, attrs)

    if include_pack and os.path.isfile(spec.output):
        try:
            with foxfs.PackReader(spec.output) as rd:
                for name, e in rd.entries.items():
                    if name in seen_names:
                        continue
                    data = rd.read(e)
                    parsed = parse(data) if data[:4] == b"YPRT" else None
                    if not parsed or parsed[0] in props:
                        continue
                    props[parsed[0]] = (b"%s/_pack/%08x" % (spec.prefix.rstrip("/").encode("latin1") or b"property", name),
                                        parsed[1])
                    stats["from_pack"] += 1
        except Exception as ex:
            if log:
                log("property pack okunamadı: %s" % ex)

    rows = []
    for crc, (fn, attrs) in props.items():
        a = dict(attrs)
        a.pop(b"crc", None)
        a.pop(b"filename", None)
        a[b"crc"] = crc
        a[b"filename"] = fn
        t = a.get(b"propertytype", b"?").decode("latin1")
        stats["types"][t] = stats["types"].get(t, 0) + 1
        rows.append((fn.lower(), b"<Property " + b" ".join(b'%s = "%s"' % (k, _esc(v)) for k, v in sorted(a.items())) + b" />"))
    if merge is not None:
        for crc, (fn, el) in existing_elements(merge).items():
            if crc in props:
                continue
            rows.append((fn.lower(), el))
            stats["kept"] += 1
            m = re.search(rb'propertytype\s*=\s*"([^"]*)"', el)
            t = m.group(1).decode("latin1") if m else "?"
            stats["types"][t] = stats["types"].get(t, 0) + 1
    rows.sort(key=lambda r: r[0])
    out = [b"<PropertyList>"] + [b"\t" + el for _, el in rows] + [b"</PropertyList>"]
    stats["properties"] = len(rows)
    return b"\r\n".join(out) + b"\r\n", stats


_RX_OBJ = re.compile(rb"Start Object\w*\s+[-\d.]+\s+[-\d.]+\s+[-\d.]+\s+(\d+)")


def map_report(settings, xml_bytes):
    """Haritaların (areadata.txt) kullandığı ama property.xml'de olmayan CRC'ler -> {crc: {harita}}"""
    have = {int(x) for x in _RX_CRC.findall(xml_bytes or b"")}
    src_root = config.paths(settings)[0]
    base = config.long_path(src_root)
    missing, total, used = {}, 0, set()
    for r, d, f in os.walk(base):
        for x in f:
            if x.lower() != "areadata.txt":
                continue
            p = os.path.join(r, x)
            parts = os.path.relpath(p, base).split(os.sep)
            mapname = parts[-3] if len(parts) >= 3 else parts[0]
            for m in _RX_OBJ.finditer(open(p, "rb").read()):
                c = int(m.group(1))
                total += 1
                used.add(c)
                if c not in have:
                    missing.setdefault(c, set()).add(mapname)
    return missing, total, len(used)


def target_path(settings, specs):
    """property.xml'in yazılacağı yer: hedef pack (varsayılan root) kaynağının kökü."""
    tname = settings.get("property_xml_pack", "root")
    t = next((s for s in specs if s.name == tname), None)
    return (os.path.join(t.source, "property.xml"), t) if t else (None, None)


def update(settings, specs, log=print, fresh=False, base=None):
    """property.xml'i üretip hedefe yazar. -> (hedef pack güncellenmeli_mi, istatistik, hedef spec)
    fresh=True yalnız elle istenir (otomatik güncelleme daima birleştirir: kaynaktan silinen property
    haritalardan kaybolmasın). base: ek birleştirme tabanı (örn. eski tam property.xml byte'ları)."""
    pname = settings.get("property_pack", "property")
    p = next((s for s in specs if s.name == pname), None)
    path, tspec = target_path(settings, specs)
    if p is None or path is None:
        log("property.xml: '%s' ya da '%s' pack klasörü yok, atlandı" % (pname, settings.get("property_xml_pack", "root")))
        return False, None, None
    old = open(path, "rb").read() if os.path.isfile(path) else b""
    merge = None if fresh else (old + b"\r\n" + base if base else old)
    data, stats = generate(p, include_pack=settings.get("property_include_pack", True), merge=merge, log=log)
    if old:
        lost = set(_RX_CRC.findall(old)) - set(_RX_CRC.findall(data))
        if lost:
            log("UYARI: %d property eski property.xml'de vardı, yenisinde yok (sıfırdan üretim haritalarda obje kaybettirebilir)" % len(lost))
    file_changed = old != data
    if file_changed:
        with open(path, "wb") as f:
            f.write(data)
    # hedef pack'teki kopya eskiyse (elle üretilip henüz paketlenmemişse) hedef yine paketlenmeli
    pack_stale = True
    if os.path.isfile(tspec.output):
        try:
            with foxfs.PackReader(tspec.output) as r:
                e = r.entries.get(foxfs.name_hash(config.encode_name(tspec.archived("property.xml"))))
                pack_stale = e is None or r.read(e) != data
        except Exception:
            pass
    log("property.xml: %d property (%s)%s%s%s -> %s%s" % (
        stats["properties"], ", ".join("%s %d" % kv for kv in sorted(stats["types"].items())),
        ", %d pack'ten" % stats["from_pack"] if stats["from_pack"] else "",
        ", %d eski girdi korundu" % stats["kept"] if stats["kept"] else "",
        ", %d CRC tekrarı" % stats["duplicate_crc"] if stats["duplicate_crc"] else "",
        ("güncellendi: " if file_changed else "değişmedi: ") + path,
        "  ['%s' pack'indeki kopya eski]" % tspec.name if pack_stale else ""))
    return pack_stale, stats, tspec
