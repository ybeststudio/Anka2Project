# Pack üretimi: çok çekirdekli encode (oku + xxh32 + lz4 + AES işçilerde), sıralı yazım,
# artımlı yeniden kullanım, taşınan (kaynakta olmayan) entry'ler, tam doğrulama, atomik değişim.
#
# Akış:  pyc derle -> anahtar (mevcut pack'inki korunur) -> .building dosyasına yaz
#        -> index + tüm entry'ler çözülerek doğrulanır -> eski pack yedeklenir -> os.replace
import json
import os
import shutil
import time
from dataclasses import dataclass, field

from . import config, foxfs, pyc
from .planner import PackPlan, SourceFile

CHUNK_BYTES = 32 * 1024 * 1024


# ---------------------------------------------------------------- işçi fonksiyonları (process)
def _encode_job(job):
    path, key, iv, level = job
    with open(path, "rb") as f:
        data = f.read()
    return foxfs.encode(data, key, iv, level)


def _verify_job(job):
    path, key, iv, items = job
    bad = []
    with open(path, "rb") as f:
        for name, offset, size, dec, digest in items:
            f.seek(offset)
            try:
                data = foxfs.decode(f.read(size), dec, key, iv)
                if len(data) != dec or foxfs.content_hash(data) != digest:
                    bad.append(name)
            except Exception:
                bad.append(name)
    return len(items), bad


# ---------------------------------------------------------------- sonuç
@dataclass
class BuildResult:
    name: str
    status: str = "ok"            # ok | skipped | failed | cancelled
    message: str = ""
    entries: int = 0
    encoded: int = 0
    reused: int = 0
    carried: int = 0
    pyc: int = 0
    source_bytes: int = 0
    pack_bytes: int = 0
    old_pack_bytes: int = 0
    seconds: float = 0.0
    warnings: list = field(default_factory=list)


def _cache_path(name):
    return os.path.join(config.DATA_DIR, "cache", name + ".json")


def _load_cache(name, key):
    try:
        with open(_cache_path(name), encoding="utf-8") as f:
            c = json.load(f)
        if c.get("key") == key.hex():
            return {int(k): v for k, v in c["entries"].items()}
    except (OSError, ValueError, KeyError):
        pass
    return {}


def _save_cache(name, key, entries):
    os.makedirs(os.path.dirname(_cache_path(name)), exist_ok=True)
    tmp = _cache_path(name) + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump({"key": key.hex(), "entries": {str(k): v for k, v in entries.items()}}, f)
    os.replace(tmp, _cache_path(name))


def _backup(output, name):
    bdir = os.path.join(config.DATA_DIR, "backups")
    os.makedirs(bdir, exist_ok=True)
    for old in os.listdir(bdir):                       # pack başına yalnız son yedek
        if old.startswith(name + "."):
            os.remove(os.path.join(bdir, old))
    dst = os.path.join(bdir, "%s.%s.pack" % (name, time.strftime("%Y%m%d_%H%M%S")))
    shutil.move(output, dst)
    return dst


class Progress:
    """GUI/CLI'ye iletilen ilerleme: aşama, işlenen/ toplam bayt."""

    def __init__(self, cb, name, total):
        self.cb, self.name, self.total, self.done, self.t0 = cb, name, max(total, 1), 0, time.time()

    def step(self, nbytes, stage):
        self.done += nbytes
        if self.cb:
            el = max(time.time() - self.t0, 1e-6)
            self.cb(self.name, stage, self.done, self.total, self.done / el)


def build_pack(plan: PackPlan, settings, pool, progress=None, log=None, cancel=None, out_dir=None) -> BuildResult:
    """out_dir verilirse pack oraya yazılır (Client/pack'e dokunulmaz; eski pack yalnız okunur)."""
    spec = plan.spec
    log = log or (lambda *a: None)
    res = BuildResult(spec.name)
    t0 = time.time()
    target = os.path.join(out_dir, spec.name) if out_dir else spec.output
    if not spec.source_exists:
        res.status, res.message = "skipped", "kaynak klasör yok: %s (mevcut pack'e dokunulmadı)" % spec.source
        return res
    if plan.existing_error:
        res.warnings.append("mevcut pack okunamadı, taşıma yapılamaz: " + plan.existing_error)

    files = list(plan.files)

    # 1) .pyc üretimi
    if plan.pyc_needed:
        made, fails = pyc.compile_many(spec.name, spec.source, plan.pyc_needed)
        for rel, err in fails:
            res.warnings.append("pyc derlenemedi: %s: %s" % (rel, err.strip()[:200]))
        by_name = {f.name: i for i, f in enumerate(files)}
        for pyc_rel, path in made.items():
            nb = config.encode_name(spec.archived(pyc_rel))
            st = os.stat(path)
            sf = SourceFile(pyc_rel, config.long_path(path), nb, foxfs.name_hash(nb), st.st_size, st.st_mtime_ns, True)
            if sf.name in by_name:
                files[by_name[sf.name]] = sf
            else:
                files.append(sf)
        res.pyc = len(made)
        log("%s: %d .pyc üretildi" % (spec.name, len(made)))

    # 2) anahtar + eski pack
    old = None
    if os.path.isfile(spec.output) and not plan.existing_error:
        old = foxfs.PackReader(spec.output)
        key, iv = old.key, old.iv
        res.old_pack_bytes = old.file_size
    else:
        key, iv = os.urandom(32), os.urandom(32)
    cache = _load_cache(spec.name, key) if (old and settings.get("incremental", True)) else {}

    reuse, encode_list = [], []
    for f in files:
        c = cache.get(f.name)
        e = old.entries.get(f.name) if old else None
        if c and e and c[0] == f.mtime_ns and c[1] == f.size and c[2] == e.digest and e.decompressed == f.size:
            reuse.append((f, e))
        else:
            encode_list.append(f)
    carry = [old.entries[n] for n in plan.carry] if old else []

    total = sum(f.size for f in encode_list) + sum(e.size for _, e in reuse) + sum(e.size for e in carry)
    prog = Progress(progress, spec.name, total)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
    tmp = target + ".building"
    writer = foxfs.PackWriter(tmp, key, iv)
    new_cache = {}
    level = settings.get("level", "default")
    try:
        # 3) encode (paralel, sıra korunur)
        jobs = ((f.path, key, iv, level) for f in encode_list)
        for f, (dec, digest, payload) in zip(encode_list, pool.imap(_encode_job, jobs, chunksize=4)):
            if cancel and cancel.is_set():
                raise InterruptedError
            writer.add(f.name, dec, digest, payload)
            new_cache[f.name] = [f.mtime_ns, f.size, digest]
            prog.step(f.size, "sıkıştırma")
        # 4) değişmeyenler + taşınanlar: şifreli veri aynen kopyalanır
        for f, e in reuse:
            if cancel and cancel.is_set():
                raise InterruptedError
            writer.add(e.name, e.decompressed, e.digest, old.payload(e))
            new_cache[f.name] = [f.mtime_ns, f.size, e.digest]
            prog.step(e.size, "kopyalama")
        for e in carry:
            if cancel and cancel.is_set():
                raise InterruptedError
            writer.add(e.name, e.decompressed, e.digest, old.payload(e))
            prog.step(e.size, "taşıma")
        writer.close()
        if old:
            old.close()
            old = None

        res.encoded, res.reused, res.carried = len(encode_list), len(reuse), len(carry)
        res.entries = writer.count
        res.source_bytes = sum(f.size for f in files)

        # 5) doğrulama: index yeniden okunur, her entry çözülüp hash kontrol edilir
        with foxfs.PackReader(tmp) as chk:
            if len(chk.entries) != writer.count or chk.duplicates:
                raise RuntimeError("index doğrulaması başarısız (%d/%d)" % (len(chk.entries), writer.count))
            ents = list(chk.entries.values())
            res.pack_bytes = chk.file_size
        if settings.get("verify", True):
            vprog = Progress(progress, spec.name, sum(e.size for e in ents))
            batches, cur, cur_b = [], [], 0
            for e in ents:
                cur.append((e.name, e.offset, e.size, e.decompressed, e.digest))
                cur_b += e.size
                if cur_b >= CHUNK_BYTES or len(cur) >= 512:
                    batches.append(cur)
                    cur, cur_b = [], 0
            if cur:
                batches.append(cur)
            bad = []
            for batch, (n, b) in zip(batches, pool.imap(_verify_job, ((tmp, key, iv, x) for x in batches))):
                if cancel and cancel.is_set():
                    raise InterruptedError
                bad += b
                vprog.step(sum(x[2] for x in batch), "doğrulama")
            if bad:
                raise RuntimeError("%d entry doğrulanamadı (ilk: %08x)" % (len(bad), bad[0]))

        # 6) değiştir
        if not out_dir and os.path.isfile(target) and settings.get("backup", True):
            log("%s: eski pack yedeklendi -> %s" % (spec.name, _backup(target, spec.name)))
        os.replace(tmp, target)
        if not out_dir:      # cache + ad hafızası yalnız gerçek pack klasörü için anlamlı
            _save_cache(spec.name, key, new_cache)
            from . import xmltools
            xmltools.update_manifest(spec, files, writer.names)
    except InterruptedError:
        res.status, res.message = "cancelled", "iptal edildi"
    except PermissionError as ex:
        res.status, res.message = "failed", "pack dosyası kilitli (client açık olabilir): %s" % ex
    except Exception as ex:
        res.status, res.message = "failed", "%s: %s" % (type(ex).__name__, ex)
    finally:
        if not writer.f.closed:
            writer.close()
        if old:
            old.close()
        if res.status != "ok" and os.path.exists(tmp):
            os.remove(tmp)
    res.seconds = time.time() - t0
    return res
