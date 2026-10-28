# Tarama + plan: her pack için hangi dosyaların yazılacağını, hangi mevcut entry'lerin taşınacağını
# (kaynakta yok ama pack'te var) ve pack'ler arası çakışmaların kime ait olduğunu belirler.
#
# Sahiplik kuralı (muhafazakâr — oyunun bugün okuduğu dağılım korunur):
#   1) Ad bugün hangi pack(ler)de varsa orada kalır; kaynağı olan pack içeriği günceller, kaynağı
#      olmayan pack eski şifreli kopyayı taşır. Aynı ad başka bir kaynakta da varsa o pack'e eklenmez.
#   2) Hiçbir pack'te olmayan yeni ad en uzun (en spesifik) prefix'e sahip pack'e yazılır.
#   0) settings.pack_priority tanımlıysa, birden fazla kaynaktaki ad için öncelik listesi 1) ve 2)'den önce gelir.
# (Client FileSystem::get pack'leri ad sırasıyla gezer, ilk bulan kazanır.)
import os
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass, field

from . import config, foxfs


@dataclass
class SourceFile:
    rel: str               # kaynak klasöre göre (posix)
    path: str              # \\?\ mutlak yol
    archived: bytes
    name: int
    size: int
    mtime_ns: int
    generated: bool = False  # .pyc üretimi gibi; kaynakta yok, önbellekten


@dataclass
class PackPlan:
    spec: config.PackSpec
    files: list = field(default_factory=list)          # yazılacak (sahip olunan) dosyalar
    ignored: list = field(default_factory=list)        # ignore deseniyle dışlanan
    unencodable: list = field(default_factory=list)    # adı kurtarılamayan
    collisions: list = field(default_factory=list)     # aynı pack içinde CRC çakışması
    given_away: list = field(default_factory=list)     # başka pack'e ait (rel, sahip)
    pyc_needed: list = field(default_factory=list)     # derlenecek (.py rel, .pyc rel)
    existing: dict = None                              # mevcut pack index'i {name: Entry}
    existing_error: str = ""
    carry: list = field(default_factory=list)          # mevcut pack'ten korunacak entry adları
    dropped: list = field(default_factory=list)        # mevcut pack'ten çıkacak entry adları
    source_bytes: int = 0

    @property
    def new_names(self):
        return {f.name for f in self.files}

    @property
    def added(self):
        if self.existing is None:
            return len(self.files)
        return sum(1 for f in self.files if f.name not in self.existing)

    @property
    def total_entries(self):
        return len(self.files) + len(self.carry)


def scan(spec: config.PackSpec, compile_pyc=False, cancel=None):
    plan = PackPlan(spec)
    if not spec.source_exists:
        return plan
    base = config.long_path(spec.source)
    seen = {}
    pys = {}
    stack = [(base, "")]          # (mutlak klasör, kaynağa göre posix önek) — relpath maliyeti yok
    while stack:
        d, rd = stack.pop()
        if cancel and cancel.is_set():
            raise InterruptedError
        try:
            it = list(os.scandir(d))
        except OSError:
            continue
        for e in it:
            if e.is_dir(follow_symlinks=False):
                if e.name.lower() not in config.GLOBAL_IGNORE_DIRS:
                    stack.append((e.path, rd + e.name + "/"))
                continue
            rel = rd + e.name
            low = e.name.lower()
            if low in config.GLOBAL_IGNORE_FILES or spec.ignored(rel):
                plan.ignored.append(rel)
                continue
            nb = config.encode_name(spec.archived(rel))
            if nb is None:
                plan.unencodable.append(rel)
                continue
            st = e.stat()
            sf = SourceFile(rel, e.path, nb, foxfs.name_hash(nb), st.st_size, st.st_mtime_ns)
            if sf.name in seen:
                old = seen[sf.name]
                if spec.canonical(rel) and not spec.canonical(old.rel):
                    # baerim yapısındaki yol (locale/x, ymir work/x) eski prefix'li kopyaya karşı kazanır
                    plan.collisions.append((old.rel, rel))
                    pys.pop(old.rel, None)
                else:
                    plan.collisions.append((rel, old.rel))
                    continue
            seen[sf.name] = sf
            if compile_pyc and low.endswith(".py"):
                pys[rel] = sf
    if compile_pyc:
        for rel, py in pys.items():
            pyc = seen.get(foxfs.name_hash(config.encode_name(spec.archived(rel + "c"))))
            if pyc is None or pyc.mtime_ns < py.mtime_ns:
                plan.pyc_needed.append((rel, rel + "c"))
    plan.files = sorted(seen.values(), key=lambda f: f.archived)
    plan.source_bytes = sum(f.size for f in plan.files)
    return plan


def load_existing(plan: PackPlan):
    if not os.path.isfile(plan.spec.output):
        plan.existing = None
        return
    try:
        with foxfs.PackReader(plan.spec.output) as r:
            plan.existing = r.entries
    except Exception as ex:  # bozuk pack: taşıma yapılamaz ama yeniden yazılabilir
        plan.existing = None
        plan.existing_error = str(ex)


class _Orphan:
    """Kaynağı olmayan pack: yazılmaz, yalnız sahiplik hesabında 'holder' olarak sayılır."""

    def __init__(self, path):
        self.spec = config.PackSpec(os.path.basename(path), "", "", "", path)
        try:
            with foxfs.PackReader(path) as r:
                self.existing = r.entries
        except Exception:
            self.existing = None


def build_plans(specs, settings, cancel=None, progress=None):
    """Tüm pack'leri tarar (çakışma çözümü için gerekli) ve plan döndürür."""
    pyc_packs = settings.get("compile_pyc", [])

    def one(s):
        p = scan(s, compile_pyc=s.name in pyc_packs, cancel=cancel)
        load_existing(p)
        return p

    # Klasör tarama + index okuma G/Ç ağırlıklı: pack'ler iş parçacıklarında paralel taranır (sıra korunur).
    plans = [None] * len(specs)
    with ThreadPoolExecutor(max_workers=min(8, max(1, len(specs)))) as ex:
        futs = {ex.submit(one, s): i for i, s in enumerate(specs)}
        orph = ex.submit(lambda: [_Orphan(path) for path in config.orphan_packs(settings, specs)])
        done = 0
        try:
            for fut in as_completed(futs):
                i = futs[fut]
                plans[i] = fut.result()
                done += 1
                if progress:
                    progress("Taranıyor: %s" % specs[i].name, done, len(specs))
        except BaseException:
            for f in futs:
                f.cancel()
            raise
        orphans = orph.result()

    owners = {}
    for p in plans:
        for f in p.files:
            owners.setdefault(f.name, []).append(p)
    holders_of = {}
    for p in plans + orphans:
        for name in (p.existing or ()):
            holders_of.setdefault(name, []).append(p)

    # pack_priority (ör. baerim kayıt sırası): aynı ad birden fazla kaynakta ise listede önce gelen kazanır.
    # Client FoxFS arşivleri ad sırasıyla gezdiği için öncelik build'de tekilleştirilerek uygulanır.
    prio = {n.lower(): i for i, n in enumerate(settings.get("pack_priority") or [])}

    def rank(p):
        return (prio.get(p.spec.name.lower(), len(prio)), -len(p.spec.prefix), p.spec.name)

    by_name = {id(p): {f.name: f for f in p.files} for p in plans}
    away_of = {}

    for name, ps in owners.items():
        holders = holders_of.get(name)
        if len(ps) == 1 and (not holders or ps[0] in holders):
            continue
        if prio and len(ps) > 1:
            writers = [min(ps, key=rank)]
            label = writers[0].spec.name
        elif holders:
            writers = [p for p in ps if p in holders]
            label = min(holders, key=lambda p: p.spec.name.lower()).spec.name
        else:
            writers = [sorted(ps, key=lambda p: (-len(p.spec.prefix), p.spec.name))[0]]
            label = writers[0].spec.name
        for p in ps:
            if p in writers:
                continue
            p.given_away.append((by_name[id(p)][name].rel, label))
            away_of.setdefault(id(p), set()).add(name)
    for p in plans:
        if p.given_away:
            away = away_of[id(p)]
            p.files = [f for f in p.files if f.name not in away]
            p.source_bytes = sum(f.size for f in p.files)

    # Mevcut entry'ler yerinde kalır; kaynağı yoksa şifreli hali aynen taşınır.
    keep = settings.get("keep_missing", True)
    for p in plans:
        if p.existing is None:
            continue
        mine = p.new_names | {foxfs.name_hash(config.encode_name(p.spec.archived(c))) for _, c in p.pyc_needed}
        for name in p.existing:
            if name in mine:
                continue
            (p.carry if keep else p.dropped).append(name)
    return plans
