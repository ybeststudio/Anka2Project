# XML yönetimi
#   bulk_configs  : tüm klasörler için config XML (eksikleri oluştur ya da prefix'i pack'ten yeniden ölçerek yenile)
#   new_pack      : özel pack — kaynak kökte klasör (gerekirse oluşturur) + config XML
#   export_legacy : eski Archiver formatında dosya listesi XML'i (<CreateEterPack><File ArchivedPath>...)
#   manifest      : ad hafızası — pack'teki her entry'nin (crc32 -> arşiv yolu) kaydı; kaynak silinse bile
#                   'Kaynak Onar' dosyayı adıyla geri çıkarabilsin diye her paketlemede birikimli güncellenir.
import html
import json
import os
import re

from . import config, foxfs

MANIFEST_DIR = os.path.join(config.DATA_DIR, "manifests")
_RX_NAME = re.compile(r"^[A-Za-z0-9_\-]+$")


def bulk_configs(settings, specs, regenerate=False, log=print):
    _, _, cd = config.paths(settings)
    done = 0
    for s in specs:
        if s.config_path and os.path.isfile(s.config_path) and not regenerate:
            continue
        if regenerate or not s.config_path:
            prefix, ok, why = config.guess_prefix(s.name, s.source, s.output)
            old = s.prefix
            s.prefix, s.prefix_guessed, s.prefix_score = prefix, not ok, why
            config.write_config(s, cd)
            log("%s: %s (prefix '%s'%s, %s)" % (s.name, os.path.basename(s.config_path), prefix,
                                                "" if old == prefix or not regenerate else " — önceki '%s'" % old, why))
            done += 1
    return done


def new_pack(settings, name, folder, prefix, ignores, create_folder=True):
    """-> PackSpec. folder: kaynak kök altındaki klasör adı ya da mutlak yolu (kök altında olmalı)."""
    src_root, pack_dir, cd = config.paths(settings)
    if not _RX_NAME.match(name):
        raise ValueError("pack adı yalnız harf, rakam, '_' ve '-' içerebilir")
    folder = folder.strip() or name
    path = folder if os.path.isabs(folder) else os.path.join(src_root, folder)
    path = os.path.normpath(path)
    if os.path.normcase(os.path.dirname(path)) != os.path.normcase(src_root):
        raise ValueError("klasör doğrudan kaynak kökün altında olmalı:\n%s" % src_root)
    if os.path.basename(path).lower() != name.lower():
        raise ValueError("klasör adı pack adıyla aynı olmalı (pack = klasör): %s" % os.path.basename(path))
    if not os.path.isdir(path):
        if not create_folder:
            raise FileNotFoundError(path)
        os.makedirs(path)
    spec = config.PackSpec(name, "", path, prefix.strip().replace("\\", "/"), os.path.join(pack_dir, name),
                           [config.IgnoreRule(p) for p in ignores])
    existing = os.path.join(cd, name + "_create.xml")
    if os.path.isfile(existing):
        spec.config_path = existing
    config.write_config(spec, cd)
    return spec


def export_legacy(plan, out_dir):
    """Eski Archiver.exe'nin okuduğu liste formatı (kaynak yolları archiver klasörüne göreli)."""
    os.makedirs(out_dir, exist_ok=True)
    s = plan.spec

    def rel(p):
        p = p[len(config.LP):] if p.startswith(config.LP) else p
        try:
            return os.path.relpath(p, config.ARCHIVER_DIR).replace("\\", "/")
        except ValueError:
            return p.replace("\\", "/")
    try:
        arch = os.path.relpath(s.output, config.ARCHIVER_DIR).replace("\\", "/")
    except ValueError:
        arch = s.output.replace("\\", "/")
    out = [b"<ScriptFile>", b'\t<CreateEterPack ArchivePath="%s" BuildKeyfile="false" >' % html.escape(arch).encode("utf-8")]
    for f in plan.files:
        src = rel(f.path).encode("mbcs", "replace")
        out.append(b'\t\t<File ArchivedPath="%s"><![CDATA[%s]]></File>' % (
            f.archived.replace(b"&", b"&amp;").replace(b'"', b"&quot;"), src))
    out += [b"\t</CreateEterPack>", b"</ScriptFile>", b""]
    path = os.path.join(out_dir, s.name + "_create.xml")
    with open(path, "wb") as fh:
        fh.write(b"\r\n".join(out))
    return path


# ------------------------------------------------------------------------- ad hafızası
def _manifest_path(name):
    return os.path.join(MANIFEST_DIR, name + ".json")


def load_manifest(name):
    try:
        with open(_manifest_path(name), encoding="utf-8") as f:
            d = json.load(f)
        return {int(k): v.encode("latin1") for k, v in d.get("names", {}).items()}, d.get("sources", {})
    except (OSError, ValueError):
        return {}, {}


def update_manifest(spec, files, pack_names):
    """files: SourceFile listesi; pack_names: pack'te kalan tüm entry hash'leri (taşınanlar dahil)."""
    names, sources = load_manifest(spec.name)
    for f in files:
        names[f.name] = f.archived
        if not getattr(f, "generated", False):
            p = f.path[len(config.LP):] if f.path.startswith(config.LP) else f.path
            sources[str(f.name)] = p
    names = {k: v for k, v in names.items() if k in pack_names}
    sources = {k: v for k, v in sources.items() if int(k) in names}
    os.makedirs(MANIFEST_DIR, exist_ok=True)
    tmp = _manifest_path(spec.name) + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump({"pack": spec.output, "names": {str(k): v.decode("latin1") for k, v in names.items()},
                   "sources": sources}, f)
    os.replace(tmp, _manifest_path(spec.name))
    return len(names)


def all_manifests():
    out = {}
    if os.path.isdir(MANIFEST_DIR):
        for fn in os.listdir(MANIFEST_DIR):
            if fn.endswith(".json"):
                out[fn[:-5]] = load_manifest(fn[:-5])
    return out
