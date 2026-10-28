# Komut satırı:
#   python -m pack_studio plan  [pack ...]           plan/rapor (yazmaz)
#   python -m pack_studio build [pack ...|--all]     paketle
#   python -m pack_studio verify [pack ...]          mevcut pack'leri çözerek doğrula
#   python -m pack_studio restore [pack ...] [--dry] silinmiş kaynakları pack'ten geri çıkar
#   python -m pack_studio clean [pack ...] [--dry] [--copies] [--delete]   bozuk adlı dosyaları temizle
#   python -m pack_studio propxml [--fresh] [--check]  property.xml üret (+ harita eksik CRC raporu)
#   python -m pack_studio xml [pack ...] [--regen]     config XML'leri toplu oluştur / yenile
#   python -m pack_studio newpack <ad> [--prefix P] [--ignore D]   özel pack (klasör + XML)
#   python -m pack_studio export [pack ...] [--to DIR] eski Archiver liste XML'i dışa aktar
# Seçenekler: --level store|fast|default|max  --no-keep  --full  --no-verify  --no-backup  -j N  --out DIR
#             --source DIR --packs DIR --config DIR  (ayarlardaki klasörleri bu çalıştırma için ezer)
import argparse
import multiprocessing as mp
import os
import sys
import time

from . import builder, cleanup, config, foxfs, pipeline, planner, propgen, restore, xmltools


def fmt_size(n):
    for u in ("B", "KB", "MB", "GB"):
        if n < 1024 or u == "GB":
            return "%.1f %s" % (n, u) if u != "B" else "%d B" % n
        n /= 1024.0


def print_plan(plans):
    hdr = "%-18s %8s %8s %8s %8s %8s %8s %8s  %s" % ("PACK", "DOSYA", "YENI", "KORUNAN", "CIKAN", "BASKA", "IGNORE", "BOZUK", "KAYNAK")
    print(hdr)
    print("-" * len(hdr))
    for p in plans:
        s = p.spec
        flag = ("  [xml yeni]" if s.config_created else "") + ("  [prefix tahmini: '%s']" % s.prefix if s.prefix_guessed else "")
        print("%-18s %8d %8d %8d %8d %8d %8d %8d  %s%s%s" % (
            s.name, len(p.files), p.added, len(p.carry), len(p.dropped), len(p.given_away), len(p.ignored),
            len(p.unencodable), fmt_size(p.source_bytes), "  +%d pyc" % len(p.pyc_needed) if p.pyc_needed else "", flag))


def select(specs, names):
    by = {s.name: s for s in specs}
    bad = [n for n in names if n not in by]
    if bad:
        sys.exit("bilinmeyen pack: %s (mevcut: %s)" % (", ".join(bad), ", ".join(by)))
    return [by[n] for n in names]


def main(argv=None):
    ap = argparse.ArgumentParser(prog="pack_studio")
    ap.add_argument("cmd", choices=["plan", "build", "verify", "restore", "clean", "propxml", "xml", "newpack", "export"])
    ap.add_argument("packs", nargs="*")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--level", choices=list(foxfs.LEVELS))
    ap.add_argument("--no-keep", action="store_true", help="kaynakta olmayan entry'leri taşıma")
    ap.add_argument("--full", action="store_true", help="artımlı yeniden kullanım kapalı")
    ap.add_argument("--no-verify", action="store_true")
    ap.add_argument("--no-backup", action="store_true")
    ap.add_argument("--dry", action="store_true")
    ap.add_argument("--copies", action="store_true", help="clean: Windows kopya dosyalarını da temizle")
    ap.add_argument("--delete", action="store_true", help="clean: karantina yerine kalıcı sil")
    ap.add_argument("--out", help="pack'leri bu klasöre yaz (pack klasörüne dokunma)")
    ap.add_argument("--source")
    ap.add_argument("--packs-dir", dest="packs_dir")
    ap.add_argument("--config")
    ap.add_argument("-j", "--jobs", type=int)
    ap.add_argument("--fresh", action="store_true", help="propxml: eski girdileri korumadan sıfırdan üret")
    ap.add_argument("--check", action="store_true", help="propxml: haritaların kullandığı eksik CRC raporu")
    ap.add_argument("--regen", action="store_true", help="xml: mevcut config'leri de prefix'i yeniden ölçerek yenile")
    ap.add_argument("--prefix", default=None, help="newpack: arşiv prefix'i (verilmezse tespit edilir)")
    ap.add_argument("--ignore", action="append", default=[], help="newpack: ignore deseni (tekrarlanabilir)")
    ap.add_argument("--to", help="export: eski liste XML'lerinin yazılacağı klasör")
    a = ap.parse_args(argv)

    st = config.load_settings()
    if a.level: st["level"] = a.level
    if a.no_keep: st["keep_missing"] = False
    if a.full: st["incremental"] = False
    if a.no_verify: st["verify"] = False
    if a.no_backup: st["backup"] = False
    if a.jobs: st["workers"] = a.jobs
    if a.source: st["source_root"] = a.source
    if a.packs_dir: st["pack_dir"] = a.packs_dir
    if a.config: st["config_dir"] = a.config

    src, pk, cd = config.paths(st)
    print("kaynak: %s\npack  : %s\nconfig: %s" % (src, pk, cd))

    if a.cmd == "newpack":
        if len(a.packs) != 1:
            sys.exit("kullanım: newpack <ad> [--prefix P] [--ignore DESEN ...]")
        name = a.packs[0]
        prefix = a.prefix
        folder = os.path.join(src, name)
        if prefix is None:
            prefix, ok, why = config.guess_prefix(name, folder, os.path.join(pk, name)) if os.path.isdir(folder) else ("d:/ymir work/%s/" % name, False, "varsayılan")
            print("prefix tespiti: '%s' (%s)" % (prefix, why))
        s = xmltools.new_pack(st, name, name, prefix, a.ignore)
        print("oluşturuldu: %s -> %s" % (s.source, s.config_path))
        return 0

    specs = config.discover(st, log=lambda m: print("  " + m))
    chosen = select(specs, a.packs) if (a.packs and not a.all) else specs

    if a.cmd == "xml":
        n = xmltools.bulk_configs(st, chosen, regenerate=a.regen, log=lambda m: print("  " + m))
        print("%d config XML yazıldı" % n)
        return 0

    if a.cmd == "propxml":
        base = open(a.to, "rb").read() if a.to and os.path.isfile(a.to) else None   # --to <eski property.xml>
        changed, stats, tspec = propgen.update(st, specs, fresh=a.fresh, base=base)
        if a.check and tspec is not None:
            data = open(os.path.join(tspec.source, "property.xml"), "rb").read()
            miss, total, used = propgen.map_report(st, data)
            print("haritalar: %d obje, %d farklı property; property.xml'de olmayan: %d CRC" % (total, used, len(miss)))
            for c, maps in sorted(miss.items()):
                print("   %10d  %s" % (c, ", ".join(sorted(maps))))
        return 0

    if a.cmd == "restore":
        if a.dry:
            for pack, (n, miss) in restore.missing_report([s.name for s in chosen] if a.packs else None).items():
                print("%-18s listede %6d, kaynakta eksik %6d" % (os.path.basename(pack), n, len(miss)))
        else:
            print("toplam %d dosya geri yazıldı" % restore.restore([s.name for s in chosen] if a.packs else None))
        return 0

    if a.cmd == "clean":
        found = cleanup.find(chosen, copies=a.copies or st.get("broken_copies"))
        for name, items in found.items():
            print("%-18s %d" % (name, len(items)))
            for rel, why in items[:5]:
                print("    %-9s %r" % (why, rel))
        if not a.dry and found:
            cleanup.clean(found, chosen, "delete" if a.delete else st.get("broken_action", "quarantine"))
        return 0

    t0 = time.time()
    names = [s.name for s in chosen]
    if a.cmd == "build" and not a.out:
        names = pipeline.prepare_build(st, names, log=lambda m: print("  " + m))
        specs = config.discover(st, create=False)
    plans = planner.build_plans(specs, st)          # çakışma çözümü tüm pack'leri ister
    plans = [p for p in plans if p.spec.name in names]
    print("tarama: %.1f sn" % (time.time() - t0))

    if a.cmd == "plan":
        print_plan(plans)
        return 0

    if a.cmd == "export":
        out = a.to or os.path.join(config.DATA_DIR, "legacy_xml")
        for p in plans:
            print("  %s -> %s" % (p.spec.name, xmltools.export_legacy(p, out)))
        return 0

    with mp.Pool(st["workers"]) as pool:
        if a.cmd == "verify":
            rc = 0
            for p in plans:
                if p.existing is None:
                    continue
                with foxfs.PackReader(p.spec.output) as r:
                    ents = [(e.name, e.offset, e.size, e.decompressed, e.digest) for e in r.entries.values()]
                    jobs = [(r.path, r.key, r.iv, ents[i:i + 512]) for i in range(0, len(ents), 512)]
                bad = [b for _, bb in pool.imap(builder._verify_job, jobs) for b in bb]
                print("%-18s %6d entry  %s" % (p.spec.name, len(p.existing), "OK" if not bad else "BOZUK: %d" % len(bad)))
                rc |= bool(bad)
            return rc

        last = [0.0]

        def prog(name, stage, done, total, rate):
            now = time.time()
            if now - last[0] < 0.2 and done < total:
                return
            last[0] = now
            if sys.stdout.isatty():
                sys.stdout.write("\r  %-14s %-10s %5.1f%%  %7.1f MB/s   " % (name, stage, 100.0 * done / total, rate / 2**20))
                sys.stdout.flush()

        rc = 0
        for p in plans:
            r = builder.build_pack(p, st, pool, progress=prog, log=print, out_dir=a.out)
            if sys.stdout.isatty():
                sys.stdout.write("\r" + " " * 70 + "\r")
            if r.status == "ok":
                print("[OK] %-14s %6d entry (yeni %d, aynı %d, korunan %d, pyc %d)  %s -> %s  %.1f sn" % (
                    r.name, r.entries, r.encoded, r.reused, r.carried, r.pyc,
                    fmt_size(r.old_pack_bytes), fmt_size(r.pack_bytes), r.seconds))
            else:
                print("[%s] %-14s %s" % (r.status.upper(), r.name, r.message))
                rc |= r.status == "failed"
            for w in r.warnings:
                print("     uyarı:", w)
        print("toplam %.1f sn" % (time.time() - t0))
        return rc
