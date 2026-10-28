# Pack tanımları ve ayarlar.
#
# Keşif: kaynak kök klasördeki (varsayılan Tools/binary_unpack) her alt klasör bir pack'tir.
#   - Config klasöründe (varsayılan Tools/archiver/make_xml) aynı ada ait <ad>_create.xml varsa prefix
#     ve Ignore desenleri oradan okunur (eski Archiver formatı, geriye uyumlu).
#   - Yoksa XML otomatik oluşturulur; prefix aynı adlı mevcut pack'in index'inden ölçülür, pack yoksa
#     içerikten tahmin edilir (spec.prefix_guessed=True — arayüzde uyarı gösterilir).
#   - Kaynağı olmayan pack'ler spec değildir (panelde görünmez); ama index'leri sahiplik için okunur.
# Kaynak / pack / config klasörleri settings.json'dan gelir (herkese açık proje: her kurulum kendi yolunu seçer).
import fnmatch
import json
import os
import random
import re
import zlib
from dataclasses import dataclass, field

STUDIO_DIR = os.path.dirname(os.path.abspath(__file__))
ARCHIVER_DIR = os.path.dirname(STUDIO_DIR)
PROJECT_DIR = os.path.normpath(os.path.join(ARCHIVER_DIR, "..", ".."))
LEGACY_XML_DIR = os.path.join(ARCHIVER_DIR, "xml")
DATA_DIR = os.path.join(STUDIO_DIR, "_data")          # cache, pyc, yedekler, karantina
SETTINGS_PATH = os.path.join(STUDIO_DIR, "settings.json")
PY27 = os.path.join(ARCHIVER_DIR, "tools", "Python27", "python.exe")

DEFAULT_SOURCE_ROOT = os.path.join(ARCHIVER_DIR, "..", "binary_unpack")
DEFAULT_PACK_DIR = os.path.join(PROJECT_DIR, "Client", "pack")
DEFAULT_CONFIG_DIR = os.path.join(ARCHIVER_DIR, "make_xml")

LP = "\\\\?\\"

GLOBAL_IGNORE_DIRS = {"__pycache__", ".git", ".svn"}
GLOBAL_IGNORE_FILES = {"thumbs.db", "desktop.ini", ".ds_store"}

# Prefix'siz pack'lerde sürücü öneki geri eklenecek kökler (baerim unpack çıktısı "d:" önekini siler)
DRIVE_PREFIX = "d:/"
DRIVE_ROOTS = ("ymir work/",)
# d:/ymir work/ prefix'li pack içinde bile kök seviye kalan klasörler (baerim yw_etc: ikashop/)
ROOT_PASSTHROUGH_DIRS = {"ikashop"}
# İç içe aynı klasörü katlayan prefix'ler (yalnız bunlar; rastgele prefix'te meşru x/x/ yolları bozulmasın)
FOLD_PREFIXES = {"d:/ymir work/", "locale/"}

# Prefix tahmini için Metin2'de d:/ymir work/<ad>/ altında yaşayan asset klasörleri
YMIR_WORK_DIRS = {"effect", "guild", "item", "monster", "monster2", "npc", "npc2", "npc_mount", "npc_pet", "pc",
                  "pc2", "pc3", "terrainmaps", "tree", "zone", "special", "environment", "ui", "action_effect"}
ROOT_LEVEL_DIRS = {"locale", "icon", "sound", "bgm", "property", "textureset", "season"}

DEFAULT_SETTINGS = {
    "level": "max",              # store | fast | default | max (LZ4-HC: eski Archiver boyutu)
    "keep_missing": True,        # kaynakta olmayan mevcut entry'leri koru
    "incremental": True,         # değişmeyen dosyaların şifreli verisini yeniden kullan
    "verify": True,              # yazdıktan sonra tüm entry'leri çöz + hash doğrula
    "backup": True,              # eski pack'i _data/backups'a taşı (pack başına son 1)
    "compile_pyc": ["root"],     # .py -> Python 2.7 .pyc üretilecek pack'ler
    "pack_priority": [],         # aynı ad birden fazla kaynakta: listede önce gelen pack kazanır (ör. baerim sırası)
    "workers": max(1, (os.cpu_count() or 2) - 1),
    "source_root": "",           # boş = varsayılan (Tools/binary_unpack)
    "pack_dir": "",              # boş = varsayılan (Client/pack)
    "config_dir": "",            # boş = varsayılan (Tools/archiver/make_xml)
    "auto_config": True,         # yeni klasörler için XML otomatik oluştur
    "broken_names": "manual",    # off | manual | auto  (adı bozuk dosyalar)
    "broken_copies": False,      # '... - Kopya/Copy/복사본' kopyalarını da bozuk say
    "broken_action": "quarantine",  # quarantine | delete
    "auto_property_xml": True,   # her paketlemeden önce property.xml'i güncelle (değiştiyse root da paketlenir)
    "property_pack": "property",  # property dosyalarının klasörü/pack'i
    "property_xml_pack": "root",  # property.xml'in yazılacağı pack (kaynak kökü)
    "property_xml_mode": "merge",  # merge | fresh (fresh yalnız elle üretimde; otomatik akış daima merge)
    "property_include_pack": True,  # property pack'inde olup kaynakta olmayanları da ekle
}


def long_path(p):
    p = os.path.abspath(p)
    return p if p.startswith(LP) else LP + p


def load_settings():
    s = dict(DEFAULT_SETTINGS)
    try:
        with open(SETTINGS_PATH, encoding="utf-8") as f:
            s.update(json.load(f))
    except (OSError, ValueError):
        pass
    return s


def save_settings(s):
    with open(SETTINGS_PATH, "w", encoding="utf-8") as f:
        json.dump(s, f, indent=2, ensure_ascii=False)


def _resolve(value, default):
    p = value.strip() if value else ""
    if not p:
        p = default
    elif not os.path.isabs(p):
        p = os.path.join(PROJECT_DIR, p)
    return os.path.normpath(p)


def paths(settings):
    """(kaynak kök, pack klasörü, config klasörü) — mutlak."""
    return (_resolve(settings.get("source_root"), DEFAULT_SOURCE_ROOT),
            _resolve(settings.get("pack_dir"), DEFAULT_PACK_DIR),
            _resolve(settings.get("config_dir"), DEFAULT_CONFIG_DIR))


def portable(path):
    """Ayarlara yazılacak yol: proje içindeyse göreli (taşınabilir)."""
    try:
        rel = os.path.relpath(path, PROJECT_DIR)
        if not rel.startswith(".."):
            return rel
    except ValueError:
        pass
    return path


class IgnoreRule:
    """Eski Archiver ile birebir: regex, kaynak klasöre göre posix yolun TAMAMINA uygulanır
    (std::regex_match). Eski liste XML'leriyle %100 tutarlı olduğu doğrulandı; bu yüzden
    '[a-zA-z0-9]+.png' alt klasördeki png'leri dışlamaz. '*.psd' gibi glob desenler fnmatch ile."""

    def __init__(self, pattern):
        self.pattern = pattern
        try:
            self.rx = re.compile(fnmatch.translate(pattern), re.I) if pattern.startswith("*") else re.compile(pattern)
        except re.error:
            self.rx = re.compile(re.escape(pattern))

    def match(self, rel_posix):
        return self.rx.fullmatch(rel_posix) is not None


@dataclass
class PackSpec:
    name: str
    config_path: str
    source: str            # mutlak kaynak klasör
    prefix: str            # arşiv yolu prefix'i ("" / "d:/ymir work/" ...)
    output: str            # mutlak pack yolu
    ignores: list = field(default_factory=list)
    config_created: bool = False     # bu çalıştırmada otomatik oluşturuldu
    prefix_guessed: bool = False     # prefix mevcut pack'ten doğrulanamadı (tahmin)
    prefix_score: str = ""           # "312/312 pack'te" gibi

    @property
    def source_exists(self):
        return os.path.isdir(self.source)

    def ignored(self, rel_posix):
        return any(r.match(rel_posix) for r in self.ignores)

    def canonical(self, rel_posix):
        """Karışık kaynakta yol baerim yapısında mı (prefix'in son klasörüyle başlıyor)? Pack içi çakışmada kazanır."""
        pl = self.prefix.lower()
        if not pl.endswith("/"):
            pl += "/"
        if pl not in FOLD_PREFIXES:
            return False
        return rel_posix.lower().startswith(pl.rstrip("/").rsplit("/", 1)[-1] + "/")

    def archived(self, rel_posix):
        p = self.prefix
        if not p:
            # Baerim düzeni: kaynak klasör oyun içi yolu birebir taşır; unpacker sürücü önekini sildiği
            # için "ymir work/..." -> "d:/ymir work/..." (client'ın istediği kanonik ad).
            return DRIVE_PREFIX + rel_posix if rel_posix.lower().startswith(DRIVE_ROOTS) else rel_posix
        if not p.endswith("/"):
            p += "/"
        low, pl = rel_posix.lower(), p.lower()
        if pl in FOLD_PREFIXES:
            # Karışık kaynak (baerim yapısı + eski prefix'li içerik aynı klasörde): prefix'in son klasörüyle
            # başlayan yol çift önek almaz (locale/ + locale/x -> locale/x, d:/ymir work/ + ymir work/x -> d:/ymir work/x).
            last = pl.rstrip("/").rsplit("/", 1)[-1] + "/"
            if low.startswith(last):
                return p[:-len(last)] + rel_posix
            # kök seviye yollar (ör. baerim yw_etc'deki ikashop/) önek almaz
            if pl == DRIVE_PREFIX + DRIVE_ROOTS[0] and low.split("/", 1)[0] in ROOT_PASSTHROUGH_DIRS:
                return rel_posix
        return p + rel_posix


_RX_CFG = re.compile(r'<CreateEterPackXml\s+Input="([^"]*)"\s+ArchivePath="([^"]*)"[^>]*>(.*?)</CreateEterPackXml>', re.S)
_RX_IGN = re.compile(r'<Ignore\s+Pattern="([^"]*)"')


def read_configs(config_dir):
    """{pack adı: (prefix, [ignore], xml yolu, kaynak klasör adı)}"""
    out = {}
    if not os.path.isdir(config_dir):
        return out
    for fn in sorted(os.listdir(config_dir)):
        if not fn.lower().endswith(".xml"):
            continue
        path = os.path.join(config_dir, fn)
        text = open(path, "rb").read().decode("utf-8", "replace")
        for inp, arch, body in _RX_CFG.findall(text):
            src, _, prefix = inp.partition(":")
            folder = os.path.basename(src.rstrip("/\\"))
            name = os.path.basename(arch.rstrip("/\\")) or folder
            out[name] = (prefix.replace("\\", "/"), _RX_IGN.findall(body), path, folder)
    return out


def _xml_attr(s):
    return s.replace("&", "&amp;").replace('"', "&quot;").replace("<", "&lt;")


def write_config(spec, config_dir, ignores=None):
    """Eski Archiver ile uyumlu <ad>_create.xml yazar (yollar archiver klasörüne göreli)."""
    os.makedirs(config_dir, exist_ok=True)

    def rel(p):
        try:
            return os.path.relpath(p, ARCHIVER_DIR).replace("\\", "/")
        except ValueError:
            return p.replace("\\", "/")
    pats = [r.pattern for r in spec.ignores] if ignores is None else ignores
    lines = ['<ScriptFile>',
             '\t<CreateEterPackXml Input="%s:%s" ArchivePath="%s" XmlPath="xml/%s_create.xml">' % (
                 _xml_attr(rel(spec.source)), _xml_attr(spec.prefix), _xml_attr(rel(spec.output)), _xml_attr(spec.name))]
    lines += ['\t\t<Ignore Pattern="%s" />' % _xml_attr(p) for p in pats]
    lines += ['\t</CreateEterPackXml>', '</ScriptFile>', '']
    path = spec.config_path or os.path.join(config_dir, spec.name + "_create.xml")
    with open(path, "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines))
    spec.config_path = path
    return path


# ---------------------------------------------------------------------------------- prefix tahmini
def _sample_rel(source, n=400):
    out = []
    for r, d, f in os.walk(long_path(source)):
        d[:] = [x for x in d if x.lower() not in GLOBAL_IGNORE_DIRS]
        for x in f:
            out.append(os.path.relpath(os.path.join(r, x)[len(LP):], source).replace("\\", "/"))
        if len(out) > 20000:
            break
    random.Random(7).shuffle(out)
    return out[:n]


def _pack_names(path):
    from . import foxfs
    try:
        with foxfs.PackReader(path) as r:
            return set(r.entries)
    except Exception:
        return None


def guess_prefix(name, source, pack_path):
    """-> (prefix, doğrulandı_mı, skor metni)"""
    cands = ["", name + "/", "d:/ymir work/%s/" % name, "d:/ymir work/", "d:/", "d:/ymir work/%s" % name]
    sample = _sample_rel(source)
    names = _pack_names(pack_path) if os.path.isfile(pack_path) else None
    if names and sample:
        best, hits = None, -1
        for c in cands:
            h = 0
            for rel in sample:
                nb = encode_name((c if not c or c.endswith("/") else c + "/") + rel)
                if nb is not None and zlib.crc32(nb.replace(b"\\", b"/").lower()) & 0xFFFFFFFF in names:
                    h += 1
            if h > hits:
                best, hits = c, h
        if hits > 0:
            return best, True, "%d/%d örnek mevcut pack'te" % (hits, len(sample))
    # içerikten tahmin
    top = {s.split("/", 1)[0].lower() for s in sample if "/" in s}
    if "ymir work" in top:
        return "d:/", False, "içerikten tahmin ('ymir work' klasörü)"
    if name.lower() in YMIR_WORK_DIRS:
        return "d:/ymir work/%s/" % name, False, "bilinen asset klasörü"
    if name.lower() in ROOT_LEVEL_DIRS:
        return name + "/", False, "bilinen kök klasör"
    py = sum(1 for s in sample if s.lower().endswith((".py", ".pyc")))
    if sample and py * 2 > len(sample):
        return "", False, "Python script klasörü"
    if top & {"effect", "item", "zone", "terrainmaps", "ui", "pc", "monster"}:
        return "d:/ymir work/", False, "içerikten tahmin (ymir work alt klasörleri)"
    return "d:/ymir work/%s/" % name, False, "varsayılan tahmin"


# ---------------------------------------------------------------------------------- keşif
def discover(settings, log=None, create=None):
    """Kaynak kökteki klasörlerden spec listesi. create=None -> settings['auto_config']."""
    source_root, pack_dir, config_dir = paths(settings)
    create = settings.get("auto_config", True) if create is None else create
    cfgs = read_configs(config_dir)
    by_folder = {v[3].lower(): k for k, v in cfgs.items()}
    specs = []
    if not os.path.isdir(source_root):
        return specs
    for folder in sorted(os.listdir(source_root), key=str.lower):
        src = os.path.join(source_root, folder)
        if not os.path.isdir(src) or folder.startswith((".", "_")) or folder.lower() in GLOBAL_IGNORE_DIRS:
            continue
        name = folder if folder in cfgs else by_folder.get(folder.lower(), folder)
        if name in cfgs:
            prefix, ign, path, _ = cfgs[name]
            specs.append(PackSpec(name, path, src, prefix, os.path.join(pack_dir, name), [IgnoreRule(p) for p in ign]))
            continue
        prefix, ok, why = guess_prefix(name, src, os.path.join(pack_dir, name))
        spec = PackSpec(name, "", src, prefix, os.path.join(pack_dir, name), [],
                        prefix_guessed=not ok, prefix_score=why)
        if create:
            write_config(spec, config_dir)
            spec.config_created = True
            if log:
                log("yeni klasör: %s -> %s oluşturuldu (prefix '%s', %s)" % (
                    folder, os.path.basename(spec.config_path), prefix, why))
        specs.append(spec)
    return specs


def orphan_packs(settings, specs):
    """Pack klasöründe olup kaynağı olmayan pack dosyaları (gizli, sadece sahiplik için)."""
    _, pack_dir, _ = paths(settings)
    have = {s.name.lower() for s in specs}
    out = []
    if os.path.isdir(pack_dir):
        for fn in os.listdir(pack_dir):
            p = os.path.join(pack_dir, fn)
            if os.path.isfile(p) and "." not in fn and fn.lower() not in have:
                out.append(p)
    return out


def load_specs(settings=None):
    return discover(settings or load_settings(), create=False)


def encode_name(text):
    """Diskteki (unicode) yol -> client'ın istediği byte dizisi.
    binary_unpack cp949 adları cp1252 olarak yazmış ('01.¼º' = cp949 '01.성'); cp1252 encode bu
    byte'ları geri verir. Türkçe adlar cp1254'e düşer. U+FFFD içeren ad kurtarılamaz (None)."""
    if "\ufffd" in text:
        return None
    for enc in ("cp1252", "latin1", "cp1254", "cp949"):
        try:
            return text.encode(enc)
        except UnicodeEncodeError:
            continue
    return None
