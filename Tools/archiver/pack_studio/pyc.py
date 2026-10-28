# .py -> Python 2.7 .pyc (client Python 2.7; Python 3 bytecode kullanılamaz).
# Çıktı kaynak ağaca değil _data/pyc/<pack>/ altına yazılır; pakete oradan girer.
import json
import os
import subprocess
import tempfile

from . import config

_SCRIPT = r'''
import sys, json, py_compile, os
jobs = json.load(open(sys.argv[1]))
fails = []
for src, dst, dfile in jobs:
    d = os.path.dirname(dst)
    if not os.path.isdir(d):
        os.makedirs(d)
    try:
        py_compile.compile(src, cfile=dst, dfile=dfile, doraise=True)
    except Exception as e:
        fails.append([dfile, str(e)])
json.dump(fails, open(sys.argv[2], "w"))
'''


def compile_many(pack_name, source_dir, pairs):
    """pairs: [(py_rel, pyc_rel)] -> ({pyc_rel: abs_path}, [(rel, hata)])"""
    if not pairs:
        return {}, []
    if not os.path.isfile(config.PY27):
        raise FileNotFoundError("Python 2.7 bulunamadı: %s" % config.PY27)
    out_dir = os.path.join(config.DATA_DIR, "pyc", pack_name)
    jobs, result = [], {}
    for py_rel, pyc_rel in pairs:
        src = os.path.join(source_dir, py_rel)
        dst = os.path.join(out_dir, pyc_rel)
        jobs.append([src, dst, py_rel])
        result[pyc_rel] = dst
    with tempfile.TemporaryDirectory() as td:
        jp, fp, sp = (os.path.join(td, n) for n in ("jobs.json", "fails.json", "c.py"))
        with open(jp, "w", encoding="mbcs") as f:
            json.dump(jobs, f, ensure_ascii=True)
        with open(sp, "w") as f:
            f.write(_SCRIPT)
        subprocess.run([config.PY27, "-B", sp, jp, fp], check=True, capture_output=True,
                       creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
        fails = json.load(open(fp))
    for rel, _ in fails:
        result.pop(rel + "c", None)
    return result, [tuple(x) for x in fails]
