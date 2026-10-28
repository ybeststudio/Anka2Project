# Anka2 Pack Studio — PyQt6 arayüz.
#
# Mimari
#   MainWindow : görünüm + ayarlar. Pack listesi her analizde kaynak kökten yeniden keşfedilir
#                (config.discover): klasör = pack, XML yoksa otomatik oluşturulur, kaynağı olmayan pack gizlenir.
#   Worker     : QThread içinde scan / build / verify / restore / broken / clean işleri; sinyallerle
#                ilerleme, günlük ve sonuç iletir. Paketleme işçileri multiprocessing.Pool.
#   Sekmeler   : sağda Paketleme / Dizinler / Temizlik ayarları; altta Günlük / Pack detayı / Pack ayarları.
#   PackExplorer: pack içeriği (ad hafızası + kaynak adlarıyla), önizleme, seçili/tüm dosyaları çıkarma.
#   Kalıcılık  : pencere/splitter/sütun durumu _data/ui.ini, günlük _data/logs/<gün>.log.
import collections
import csv
import ctypes
import html
import multiprocessing as mp
import os
import shutil
import subprocess
import sys
import threading
import time
import traceback
from ctypes import wintypes

from PyQt6.QtCore import (QAbstractTableModel, QModelIndex, QObject, QRectF, QSettings, Qt, QThread, QTimer,
                          pyqtSignal)
from PyQt6.QtGui import (QBrush, QColor, QFont, QIcon, QImage, QKeySequence, QLinearGradient, QPainter, QPen, QPixmap,
                         QShortcut)
from PyQt6.QtWidgets import (
    QAbstractItemView, QApplication, QButtonGroup, QCheckBox, QComboBox, QDialog, QFileDialog, QFormLayout, QFrame,
    QGridLayout, QHBoxLayout, QHeaderView, QLabel, QLineEdit, QListWidget, QListWidgetItem, QMainWindow, QMenu,
    QMessageBox, QPlainTextEdit, QProgressBar, QProgressDialog, QPushButton, QRadioButton, QScrollArea, QSpinBox,
    QSplitter, QStackedWidget, QStyledItemDelegate, QStyle, QSystemTrayIcon, QTableView, QTableWidget,
    QTableWidgetItem, QTabWidget, QToolButton, QVBoxLayout, QWidget,
)

from . import __version__, builder, cleanup, config, foxfs, pipeline, planner, propgen, restore, xmltools

# ------------------------------------------------------------------------------------------- tema
C = {
    "bg": "#0b1120", "panel": "#111a2e", "panel2": "#16213a", "border": "#23314f", "text": "#e2e8f0",
    "muted": "#8a9bbd", "dim": "#55658a", "accent": "#6366f1", "accent_hi": "#818cf8", "sky": "#38bdf8",
    "blue": "#60a5fa", "amber": "#f59e0b", "red": "#ef4444", "violet": "#a78bfa", "slate": "#64748b",
}

STATE_COLORS = {
    "Güncel": C["sky"], "Eksik var": C["amber"], "Yeni pack": C["violet"], "Bekliyor": C["slate"],
    "Paketleniyor": C["accent_hi"], "Paketlendi": C["blue"], "Doğrulanıyor": C["accent_hi"],
    "Doğrulandı": C["blue"], "Hata": C["red"], "Bozuk": C["red"], "İptal": C["slate"], "Atlandı": C["slate"],
}

LEVEL_LABELS = [
    ("max", "Maksimum (LZ4-HC) — önerilen"),
    ("default", "Dengeli (LZ4)"),
    ("fast", "Hızlı (LZ4 fast)"),
    ("store", "Sıkıştırmasız"),
]

COLS = ["", "Pack", "Durum", "Prefix", "Dosya", "Yeni", "Korunan", "Bozuk", "Kaynak", "Pack", "Son paket"]
(COL_CHECK, COL_NAME, COL_STATE, COL_PREFIX, COL_FILES, COL_NEW, COL_KEEP, COL_BAD,
 COL_SRC, COL_PACK, COL_TIME) = range(len(COLS))

QSS = """
* { font-family: 'Segoe UI'; font-size: 10pt; color: %(text)s; }
QMainWindow, QWidget#root, QDialog, QMessageBox { background: %(bg)s; }
QFrame#header { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #0f172a, stop:0.6 #151f3b, stop:1 #1e1b4b);
                border-bottom: 1px solid %(border)s; }
QLabel#title { font-size: 17pt; font-weight: 600; color: #f8fafc; }
QLabel#subtitle, QLabel#muted { color: %(muted)s; font-size: 9pt; }
QLabel#section { color: %(muted)s; font-weight: 600; font-size: 9pt; letter-spacing: 1px; }
QFrame#card { background: %(panel)s; border: 1px solid %(border)s; border-radius: 10px; }
QLabel#statValue { font-size: 16pt; font-weight: 600; color: #f8fafc; }
QLabel#statLabel { color: %(muted)s; font-size: 8.5pt; }
QPushButton { background: %(panel2)s; border: 1px solid %(border)s; border-radius: 7px; padding: 7px 14px; }
QPushButton:hover { background: #1d2a48; border-color: %(accent)s; }
QPushButton:pressed { background: #141d33; }
QPushButton:disabled { color: %(dim)s; background: %(panel)s; border-color: %(panel2)s; }
QPushButton#primary { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #6d70f5, stop:1 #4f46e5);
                      border: 1px solid #7c7ff7; font-weight: 600; color: white; }
QPushButton#primary:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #8184f8, stop:1 #5b52ef); }
QPushButton#primary:disabled { background: #252a55; border-color: #2e3466; color: #7c84b8; }
QPushButton#danger { background: #2a1420; border-color: #7f1d1d; color: #fca5a5; }
QPushButton#danger:hover { background: #3b1726; border-color: %(red)s; }
QPushButton#link { background: transparent; border: none; color: %(accent_hi)s; padding: 3px 6px; }
QPushButton#link:hover { color: #c7d2fe; text-decoration: underline; }
QToolButton { background: %(panel2)s; border: 1px solid %(border)s; border-radius: 7px; padding: 5px 9px; }
QToolButton:hover { border-color: %(accent)s; }
QTableWidget { background: %(bg)s; alternate-background-color: #0e1528; gridline-color: transparent;
               border: 1px solid %(border)s; border-radius: 10px; selection-background-color: #23305a;
               selection-color: #f8fafc; outline: 0; }
QTableWidget::item { padding: 4px 6px; border-bottom: 1px solid #141d33; }
QHeaderView::section { background: %(panel)s; color: %(muted)s; border: none; border-bottom: 1px solid %(border)s;
                       padding: 8px 6px; font-weight: 600; font-size: 9pt; }
QPlainTextEdit, QListWidget { background: #070c18; border: 1px solid %(border)s; border-radius: 10px;
                              font-family: 'Cascadia Mono','Consolas'; font-size: 9pt; padding: 4px; }
QListWidget::item { padding: 1px 2px; }
QProgressBar { background: #1a2440; border: none; border-radius: 4px; max-height: 8px; text-align: center; color: transparent; }
QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #6366f1, stop:1 #38bdf8); border-radius: 4px; }
QProgressBar#overall::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #a78bfa, stop:1 #6366f1); }
QComboBox, QSpinBox, QLineEdit { background: #0a1122; border: 1px solid %(border)s; border-radius: 7px; padding: 6px 8px; }
QComboBox:focus, QSpinBox:focus, QLineEdit:focus, QPlainTextEdit:focus { border-color: %(accent)s; }
QLineEdit:read-only { color: %(muted)s; }
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView { background: %(panel2)s; border: 1px solid %(border)s; selection-background-color: %(accent)s; }
QCheckBox, QRadioButton { spacing: 8px; }
QCheckBox::indicator, QRadioButton::indicator { width: 16px; height: 16px; }
QCheckBox::indicator { border: 1px solid %(border)s; border-radius: 4px; background: #0a1122; }
QCheckBox::indicator:checked { background: %(accent)s; border-color: %(accent_hi)s; image: none; }
QRadioButton::indicator { border: 1px solid %(border)s; border-radius: 8px; background: #0a1122; }
QRadioButton::indicator:checked { background: qradialgradient(cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5, stop:0 #c7d2fe, stop:0.45 %(accent)s, stop:0.55 #0a1122); border-color: %(accent_hi)s; }
QTabWidget::pane { border: 1px solid %(border)s; border-radius: 10px; top: -1px; background: %(panel)s; }
QTabBar::tab { background: transparent; padding: 8px 16px; border: none; border-bottom: 2px solid transparent; color: %(muted)s; }
QTabBar::tab:selected { color: #f8fafc; border-bottom: 2px solid %(accent)s; }
QTabBar::tab:hover { color: %(text)s; }
QSplitter::handle { background: %(bg)s; }
QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: #26355a; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: #33477a; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
QScrollBar::handle:horizontal { background: #26355a; border-radius: 4px; min-width: 30px; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QToolTip { background: %(panel2)s; color: %(text)s; border: 1px solid %(border)s; padding: 6px; }
QStatusBar { background: %(panel)s; border-top: 1px solid %(border)s; }
QStatusBar QLabel { color: %(muted)s; }
""" % C


def fmt_size(n):
    if n is None:
        return "—"
    for u in ("B", "KB", "MB", "GB"):
        if n < 1024 or u == "GB":
            return ("%d B" % n) if u == "B" else "%.1f %s" % (n, u)
        n /= 1024.0


def fmt_int(n):
    return "{:,}".format(n).replace(",", ".")


CLIENT_EXES = {"metin2release_x64.exe", "metin2release.exe"}
SORT_ROLE = Qt.ItemDataRole.UserRole + 1
LOG_DIR = os.path.join(config.DATA_DIR, "logs")
UI_INI = os.path.join(config.DATA_DIR, "ui.ini")


class _PROCESSENTRY32W(ctypes.Structure):
    _fields_ = [("dwSize", wintypes.DWORD), ("cntUsage", wintypes.DWORD), ("th32ProcessID", wintypes.DWORD),
                ("th32DefaultHeapID", ctypes.c_size_t), ("th32ModuleID", wintypes.DWORD),
                ("cntThreads", wintypes.DWORD), ("th32ParentProcessID", wintypes.DWORD),
                ("pcPriClassBase", ctypes.c_long), ("dwFlags", wintypes.DWORD), ("szExeFile", ctypes.c_wchar * 260)]


_K32 = None


def _kernel32():
    global _K32
    if _K32 is None:
        k32 = ctypes.WinDLL("kernel32", use_last_error=True)
        k32.CreateToolhelp32Snapshot.restype = ctypes.c_void_p
        k32.CreateToolhelp32Snapshot.argtypes = (wintypes.DWORD, wintypes.DWORD)
        k32.Process32FirstW.argtypes = k32.Process32NextW.argtypes = (ctypes.c_void_p, ctypes.POINTER(_PROCESSENTRY32W))
        k32.CloseHandle.argtypes = (ctypes.c_void_p,)
        _K32 = k32
    return _K32


def _process_names():
    """Toolhelp snapshot (tasklist'e göre ~100x hızlı; durum çubuğu birkaç saniyede bir sorar)."""
    k32 = _kernel32()
    snap = k32.CreateToolhelp32Snapshot(0x2, 0)                 # TH32CS_SNAPPROCESS
    if not snap or snap == ctypes.c_void_p(-1).value:
        raise OSError("snapshot")
    names = set()
    try:
        pe = _PROCESSENTRY32W()
        pe.dwSize = ctypes.sizeof(pe)
        ok = k32.Process32FirstW(snap, ctypes.byref(pe))
        while ok:
            names.add(pe.szExeFile.lower())
            ok = k32.Process32NextW(snap, ctypes.byref(pe))
    finally:
        k32.CloseHandle(snap)
    return names


def client_running():
    try:
        return bool(_process_names() & CLIENT_EXES)
    except Exception:
        try:
            out = subprocess.run(["tasklist", "/NH"], capture_output=True, text=True,
                                 creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0)).stdout.lower()
            return any(x in out for x in CLIENT_EXES)
        except OSError:
            return False


def fmt_dur(sec):
    sec = int(sec)
    return "%d:%02d:%02d" % (sec // 3600, sec // 60 % 60, sec % 60) if sec >= 3600 else "%02d:%02d" % (sec // 60, sec % 60)


def display_name(b):
    """Arşiv adı (client byte'ları) -> okunur metin: ASCII, cp949 (Korece), cp1254 (Türkçe)."""
    for enc in ("ascii", "cp949", "cp1254"):
        try:
            return b.decode(enc)
        except UnicodeDecodeError:
            continue
    return b.decode("latin1")


def safe_rel(path):
    """Arşiv yolu -> diske yazılabilir göreli yol ('d:/ymir work/x' -> 'd/ymir work/x')."""
    parts = [p for p in path.replace("\\", "/").split("/") if p not in ("", ".", "..")]
    bad = '<>:"|?*'
    return os.path.join(*[("".join("_" if c in bad or ord(c) < 32 else c for c in p).rstrip(" .") or "_")
                          for p in parts]) if parts else "_"


def open_path(path, select=False):
    """select=True: Explorer'da dosyayı işaretle; aksi halde aç (yoksa klasör olarak oluşturulur)."""
    if select and os.path.isfile(path):
        subprocess.Popen('explorer /select,"%s"' % os.path.normpath(path))
        return
    if not os.path.exists(path):
        os.makedirs(path, exist_ok=True)
    os.startfile(path)


class SortItem(QTableWidgetItem):
    """SORT_ROLE'deki ham değere (bayt, adet, zaman) göre sıralanan hücre."""

    def __lt__(self, other):
        a, b = self.data(SORT_ROLE), other.data(SORT_ROLE)
        if a is not None and b is not None:
            return a < b
        return self.text().lower() < other.text().lower()


def app_icon():
    pm = QPixmap(64, 64)
    pm.fill(Qt.GlobalColor.transparent)
    p = QPainter(pm)
    p.setRenderHint(QPainter.RenderHint.Antialiasing)
    g = QLinearGradient(0, 0, 64, 64)
    g.setColorAt(0, QColor("#818cf8"))
    g.setColorAt(1, QColor("#4338ca"))
    p.setBrush(QBrush(g))
    p.setPen(Qt.PenStyle.NoPen)
    p.drawRoundedRect(4, 4, 56, 56, 14, 14)
    p.setPen(QColor("white"))
    p.setFont(QFont("Segoe UI", 24, QFont.Weight.Bold))
    p.drawText(pm.rect(), Qt.AlignmentFlag.AlignCenter, "A2")
    p.end()
    return QIcon(pm)


class PillDelegate(QStyledItemDelegate):
    """Durum sütunu: renkli hap."""

    def paint(self, painter, option, index):
        text = index.data() or ""
        color = QColor(index.data(Qt.ItemDataRole.UserRole) or C["slate"])
        painter.save()
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        if option.state & QStyle.StateFlag.State_Selected:
            painter.fillRect(option.rect, QColor("#23305a"))
        if text:
            fm = option.fontMetrics
            w = fm.horizontalAdvance(text) + 26
            r = QRectF(option.rect.x() + 6, option.rect.center().y() - 10, w, 20)
            bg = QColor(color)
            bg.setAlpha(38)
            painter.setBrush(bg)
            border = QColor(color)
            border.setAlpha(110)
            painter.setPen(QPen(border, 1))
            painter.drawRoundedRect(r, 10, 10)
            painter.setBrush(color)
            painter.setPen(Qt.PenStyle.NoPen)
            painter.drawEllipse(QRectF(r.x() + 8, r.center().y() - 3, 6, 6))
            painter.setPen(color.lighter(125))
            painter.drawText(r.adjusted(18, 0, 0, 0), Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignLeft, text)
        painter.restore()


# ------------------------------------------------------------------------------------------- worker
class Worker(QObject):
    log = pyqtSignal(str, str)                         # seviye, metin
    progress = pyqtSignal(str, str, float, float, float)
    overall = pyqtSignal(int, int)
    planned = pyqtSignal(object)
    packDone = pyqtSignal(object)
    packState = pyqtSignal(str, str)
    broken = pyqtSignal(object)
    propReport = pyqtSignal(object)
    finished = pyqtSignal(str)

    def __init__(self, task, settings, names=None, out_dir=None):
        super().__init__()
        self.task, self.settings, self.names, self.out_dir = task, dict(settings), names, out_dir
        self.cancel = threading.Event()
        self._last_emit = 0.0

    def _emit_progress(self, n, s, d, t, rate):
        now = time.time()
        if now - self._last_emit >= 0.08 or d >= t:      # UI'ı boğmamak için ~12 Hz
            self._last_emit = now
            self.progress.emit(n, s, d, t, rate)

    def run(self):
        status = "ok"
        try:
            getattr(self, "_" + self.task)()
            if self.cancel.is_set():
                status = "cancelled"
        except InterruptedError:
            status = "cancelled"
        except Exception:
            status = "error"
            self.log.emit("err", traceback.format_exc())
        self.finished.emit(status)

    def _specs(self):
        return config.discover(self.settings, log=lambda m: self.log.emit("new", m))

    def _scan(self):
        t0 = time.time()
        specs = self._specs()
        plans = planner.build_plans(specs, self.settings, self.cancel,
                                    progress=lambda m, i, n: self.overall.emit(i, n))
        self.overall.emit(1, 1)
        self.log.emit("info", "Analiz: %d pack, %.1f sn" % (len(plans), time.time() - t0))
        self.planned.emit(plans)
        return plans

    def _clean_broken(self, specs, copies):
        found = cleanup.find(specs, copies=copies, cancel=self.cancel)
        if found:
            n = cleanup.clean(found, specs, self.settings.get("broken_action", "quarantine"),
                              log=lambda m: self.log.emit("warn", m))
            self.log.emit("warn", "Temizlik: %d bozuk adlı dosya kaldırıldı" % n)
        else:
            self.log.emit("info", "Temizlik: bozuk adlı dosya yok")

    def _build(self):
        if not self.out_dir:      # temizlik + property.xml yalnız canlı paketlemede
            self.names = pipeline.prepare_build(self.settings, self.names, log=lambda m: self.log.emit("info", m),
                                                cancel=self.cancel)
        plans = self._scan()
        plans = [p for p in plans if p.spec.name in self.names]
        total = len(plans)
        self.log.emit("info", "Paketleme: %d pack • %d işçi • %s%s" % (
            total, self.settings["workers"], self.settings["level"], " • hedef " + self.out_dir if self.out_dir else ""))
        t0 = time.time()
        with mp.Pool(self.settings["workers"]) as pool:
            for i, p in enumerate(plans):
                if self.cancel.is_set():
                    break
                self.overall.emit(i, total)
                self.packState.emit(p.spec.name, "Paketleniyor")
                r = builder.build_pack(p, self.settings, pool, progress=self._emit_progress,
                                       log=lambda m: self.log.emit("info", m), cancel=self.cancel, out_dir=self.out_dir)
                self.packDone.emit(r)
        self.overall.emit(total, total)
        self.log.emit("ok", "Paketleme bitti: %.1f sn" % (time.time() - t0))

    def _verify(self):
        specs = [s for s in self._specs() if s.name in self.names]
        with mp.Pool(self.settings["workers"]) as pool:
            for i, s in enumerate(specs):
                if self.cancel.is_set():
                    break
                self.overall.emit(i, len(specs))
                if not os.path.isfile(s.output):
                    continue
                self.packState.emit(s.name, "Doğrulanıyor")
                with foxfs.PackReader(s.output) as r:
                    ents = [(e.name, e.offset, e.size, e.decompressed, e.digest) for e in r.entries.values()]
                    jobs = [(r.path, r.key, r.iv, ents[k:k + 512]) for k in range(0, len(ents), 512)]
                    total_b = max(sum(e[2] for e in ents), 1)
                bad, done, t0 = [], 0, time.time()
                for job, (n, b) in zip(jobs, pool.imap(builder._verify_job, jobs)):
                    bad += b
                    done += sum(x[2] for x in job[3])
                    self._emit_progress(s.name, "doğrulama", done, total_b, done / max(time.time() - t0, 1e-6))
                    if self.cancel.is_set():
                        raise InterruptedError
                self.packState.emit(s.name, "Doğrulandı" if not bad else "Bozuk")
                self.log.emit("ok" if not bad else "err", "%s: %s entry %s" % (
                    s.name, fmt_int(len(ents)), "doğrulandı" if not bad else "— %d BOZUK entry" % len(bad)))
        self.overall.emit(1, 1)

    def _restore(self):
        n = restore.restore(self.names or None, log=lambda m: self.log.emit("info", m), cancel=self.cancel)
        self.log.emit("ok", "Kaynak onarımı: %d dosya geri yazıldı" % n)

    def _broken_scan(self):
        specs = [s for s in self._specs() if not self.names or s.name in self.names]
        self.broken.emit(cleanup.find(specs, copies=self.settings.get("broken_copies", False), cancel=self.cancel))

    def _clean(self):
        specs = [s for s in self._specs() if not self.names or s.name in self.names]
        self._clean_broken(specs, self.settings.get("broken_copies", False))

    def _propxml(self):
        specs = self._specs()
        fresh = self.settings.get("property_xml_mode") == "fresh"     # yalnız elle; otomatik akış daima birleştirir
        changed, stats, tspec = propgen.update(self.settings, specs, fresh=fresh,
                                               log=lambda m: self.log.emit("warn" if m.startswith("UYARI") else "ok", m))
        if tspec is None:
            return
        data = open(os.path.join(tspec.source, "property.xml"), "rb").read()
        miss, total, used = propgen.map_report(self.settings, data)
        self.propReport.emit({"stats": stats, "changed": changed, "missing": miss, "objects": total, "used": used,
                              "target": tspec.name})

    def _bulkxml(self):
        specs = self._specs()
        if self.names:
            specs = [s for s in specs if s.name in self.names]
        n = xmltools.bulk_configs(self.settings, specs, regenerate=self.out_dir == "regen",
                                  log=lambda m: self.log.emit("new", m))
        self.log.emit("ok", "XML: %d config yazıldı" % n)

    def _export(self):
        plans = [p for p in self._scan() if not self.names or p.spec.name in self.names]
        for p in plans:
            self.log.emit("info", "%s -> %s" % (p.spec.name, xmltools.export_legacy(p, self.out_dir)))
        self.log.emit("ok", "Eski liste XML'i: %d pack dışa aktarıldı -> %s" % (len(plans), self.out_dir))


# ------------------------------------------------------------------------------------------- yardımcı widget'lar
class StatCard(QFrame):
    def __init__(self, label, accent):
        super().__init__()
        self.setObjectName("card")
        self.accent = accent
        lay = QVBoxLayout(self)
        lay.setContentsMargins(16, 10, 16, 10)
        lay.setSpacing(1)
        self.value = QLabel("—")
        self.value.setObjectName("statValue")
        lab = QLabel(label)
        lab.setObjectName("statLabel")
        lay.addWidget(self.value)
        lay.addWidget(lab)

    def paintEvent(self, e):
        super().paintEvent(e)
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(QColor(self.accent))
        p.drawRoundedRect(QRectF(12, 0, 36, 3), 1.5, 1.5)
        p.end()

    def set(self, text, color=None):
        self.value.setText(text)
        self.value.setStyleSheet("color:%s" % color if color else "")


def section(text):
    l = QLabel(text.upper())
    l.setObjectName("section")
    return l


def path_row(edit, on_browse):
    w = QWidget()
    h = QHBoxLayout(w)
    h.setContentsMargins(0, 0, 0, 0)
    h.setSpacing(6)
    h.addWidget(edit, 1)
    b = QToolButton()
    b.setText("…")
    b.setToolTip("Klasör seç")
    b.clicked.connect(on_browse)
    h.addWidget(b)
    return w


class NewPackDialog(QDialog):
    """Özel pack: kaynak kökte klasör (gerekirse oluşturulur) + config XML."""

    def __init__(self, parent, settings):
        super().__init__(parent)
        self.settings, self.spec = settings, None
        self.setWindowTitle("Yeni pack")
        self.setMinimumWidth(520)
        src_root, pack_dir, _ = config.paths(settings)
        f = QFormLayout(self)
        f.setContentsMargins(18, 16, 18, 16)
        f.setVerticalSpacing(10)
        hint = QLabel("Pack = kaynak kökteki klasör. Klasör yoksa oluşturulur; dosyalarını içine kopyalayıp "
                      "'Analiz' ile paketlemeye hazır hale getirirsin.\nKaynak kök: %s" % src_root)
        hint.setWordWrap(True)
        hint.setObjectName("muted")
        f.addRow(hint)
        self.ed_name = QLineEdit()
        self.ed_name.setPlaceholderText("örn. etc3")
        f.addRow("Pack adı", self.ed_name)
        row = QHBoxLayout()
        self.ed_prefix = QLineEdit("d:/ymir work/")
        b = QPushButton("Tespit et")
        b.clicked.connect(self._detect)
        row.addWidget(self.ed_prefix, 1)
        row.addWidget(b)
        f.addRow("Prefix", row)
        self.cmb_preset = QComboBox()
        for label, pfx in (("d:/ymir work/<ad>/  — asset klasörü", "d:/ymir work/{n}/"), ("d:/ymir work/  — etc tarzı", "d:/ymir work/"),
                           ("<ad>/  — locale/icon/sound tarzı", "{n}/"), ("(kök)  — root/maps tarzı", ""), ("d:/  — yw_graphic tarzı", "d:/")):
            self.cmb_preset.addItem(label, pfx)
        self.cmb_preset.activated.connect(lambda i: self.ed_prefix.setText(
            self.cmb_preset.itemData(i).format(n=self.ed_name.text().strip() or "ad")))
        f.addRow("Hazır prefix", self.cmb_preset)
        self.ed_ign = QPlainTextEdit()
        self.ed_ign.setPlaceholderText("*.psd\n*.pdn")
        self.ed_ign.setFixedHeight(64)
        f.addRow("Ignore", self.ed_ign)
        self.lbl = QLabel("")
        self.lbl.setObjectName("muted")
        f.addRow(self.lbl)
        hb = QHBoxLayout()
        hb.addStretch(1)
        bc = QPushButton("İptal")
        bc.clicked.connect(self.reject)
        bo = QPushButton("Oluştur")
        bo.setObjectName("primary")
        bo.clicked.connect(self._ok)
        hb.addWidget(bc)
        hb.addWidget(bo)
        f.addRow(hb)

    def _detect(self):
        n = self.ed_name.text().strip()
        src_root, pack_dir, _ = config.paths(self.settings)
        folder = os.path.join(src_root, n)
        if not n or not os.path.isdir(folder):
            self.lbl.setText("Tespit için klasör mevcut ve dolu olmalı.")
            return
        p, ok, why = config.guess_prefix(n, folder, os.path.join(pack_dir, n))
        self.ed_prefix.setText(p)
        self.lbl.setText(("✓ " if ok else "⚠ tahmini — ") + why)

    def _ok(self):
        try:
            self.spec = xmltools.new_pack(self.settings, self.ed_name.text().strip(), self.ed_name.text().strip(),
                                          self.ed_prefix.text(), [x.strip() for x in self.ed_ign.toPlainText().splitlines() if x.strip()])
        except Exception as ex:
            QMessageBox.warning(self, "Yeni pack", str(ex))
            return
        self.accept()


# ------------------------------------------------------------------------------------------- pack gezgini
IMAGE_EXTS = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tga", ".ico", ".webp"}
PREVIEW_LIMIT = 4 * 1024 * 1024
ENTRY_STATES = {"kaynakta": C["sky"], "korunan": C["violet"], "pyc": C["accent_hi"], "—": C["muted"]}


class PackModel(QAbstractTableModel):
    """Pack entry'leri: (yol, açık boyut, paketli boyut, durum, crc, xxh32, Entry).
    Filtre + sıralama modelin içinde liste üzerinde yapılır (proxy'nin satır başı Python çağrısı yok):
    100k entry'de sıralama/filtre onlarca ms."""
    HEAD = ["Arşiv yolu", "Boyut", "Pakette", "Oran", "Durum", "CRC32", "xxh32"]
    NUMERIC = {1, 2, 3}
    KEYS = (lambda r: r[0].lower(), lambda r: r[1], lambda r: r[2], lambda r: (r[2] / r[1]) if r[1] else 1.0,
            lambda r: r[3], lambda r: r[4], lambda r: r[5])

    def __init__(self, rows):
        super().__init__()
        self.all_rows = rows
        self.rows = list(rows)
        self.text, self.state = "", ""
        self.sort_col, self.sort_order = 0, Qt.SortOrder.AscendingOrder

    def _accept(self, r):
        if self.state and r[3] != self.state and not (self.state == "isimsiz" and r[0].startswith("<")):
            return False
        return not self.text or self.text in r[0].lower()

    def set_filter(self, text=None, state=None):
        if text is not None:
            self.text = text.strip().lower()
        if state is not None:
            self.state = state
        self.beginResetModel()
        self.rows = [r for r in self.all_rows if self._accept(r)] if (self.text or self.state) else list(self.all_rows)
        self.rows.sort(key=self.KEYS[self.sort_col], reverse=self.sort_order == Qt.SortOrder.DescendingOrder)
        self.endResetModel()

    def sort(self, column, order=Qt.SortOrder.AscendingOrder):
        self.sort_col, self.sort_order = column, order
        self.layoutAboutToBeChanged.emit()
        self.rows.sort(key=self.KEYS[column], reverse=order == Qt.SortOrder.DescendingOrder)
        self.layoutChanged.emit()

    def rowCount(self, parent=QModelIndex()):
        return 0 if parent.isValid() else len(self.rows)

    def columnCount(self, parent=QModelIndex()):
        return len(self.HEAD)

    def headerData(self, section, orient, role=Qt.ItemDataRole.DisplayRole):
        if orient == Qt.Orientation.Horizontal and role == Qt.ItemDataRole.DisplayRole:
            return self.HEAD[section]
        return None

    def data(self, idx, role=Qt.ItemDataRole.DisplayRole):
        r = self.rows[idx.row()]
        c = idx.column()
        if role == Qt.ItemDataRole.DisplayRole:
            if c == 0:
                return r[0]
            if c == 1:
                return fmt_size(r[1])
            if c == 2:
                return fmt_size(r[2])
            if c == 3:
                return "%.0f%%" % (100.0 * r[2] / r[1]) if r[1] else "—"
            if c == 4:
                return r[3]
            if c == 5:
                return "%08x" % r[4]
            if c == 6:
                return "%08x" % r[5]
        elif role == Qt.ItemDataRole.ForegroundRole:
            if c == 0 and r[0].startswith("<"):
                return QColor(C["dim"])
            if c == 4:
                return QColor(ENTRY_STATES.get(r[3], C["muted"]))
            if c >= 5 or c in self.NUMERIC:
                return QColor(C["muted"])
        elif role == Qt.ItemDataRole.TextAlignmentRole and c in self.NUMERIC:
            return int(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)
        return None


class PackExplorer(QDialog):
    """Pack içeriği: adlar ad hafızasından (_data/manifests) + güncel kaynaktan çözülür."""

    def __init__(self, parent, spec, plan=None):
        super().__init__(parent)
        self.spec, self.plan = spec, plan
        self.setWindowTitle("Pack Gezgini — %s" % spec.name)
        self.setWindowFlag(Qt.WindowType.WindowMaximizeButtonHint, True)
        self.resize(1280, 760)
        v = QVBoxLayout(self)
        v.setContentsMargins(14, 12, 14, 12)
        v.setSpacing(8)
        self.lbl_head = QLabel()
        self.lbl_head.setObjectName("muted")
        self.lbl_head.setTextInteractionFlags(Qt.TextInteractionFlag.TextSelectableByMouse)
        v.addWidget(self.lbl_head)

        tb = QHBoxLayout()
        self.ed_filter = QLineEdit()
        self.ed_filter.setPlaceholderText("Yol ara…  (Ctrl+F)")
        self.ed_filter.setClearButtonEnabled(True)
        self.cmb_state = QComboBox()
        for label, key in (("Tüm entry'ler", ""), ("Kaynakta olan", "kaynakta"), ("Korunan (kaynakta yok)", "korunan"),
                           ("Adı bilinmeyen", "isimsiz")):
            self.cmb_state.addItem(label, key)
        self.lbl_count = QLabel()
        self.lbl_count.setObjectName("muted")
        tb.addWidget(self.ed_filter, 1)
        tb.addWidget(self.cmb_state)
        tb.addWidget(self.lbl_count)
        v.addLayout(tb)

        split = QSplitter(Qt.Orientation.Horizontal)
        split.setHandleWidth(10)
        self.view = QTableView()
        self.view.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
        self.view.setSelectionMode(QAbstractItemView.SelectionMode.ExtendedSelection)
        self.view.setSortingEnabled(True)
        self.view.setAlternatingRowColors(True)
        self.view.setShowGrid(False)
        self.view.setWordWrap(False)
        self.view.verticalHeader().setVisible(False)
        self.view.verticalHeader().setDefaultSectionSize(24)
        self.view.setStyleSheet("QTableView { background:%s; alternate-background-color:#0e1528; border:1px solid %s;"
                                " border-radius:10px; selection-background-color:#23305a; outline:0; }" % (C["bg"], C["border"]))
        self.view.doubleClicked.connect(lambda _: self._open_external())
        self.view.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self.view.customContextMenuRequested.connect(self._menu)
        split.addWidget(self.view)

        pv = QWidget()
        pl = QVBoxLayout(pv)
        pl.setContentsMargins(0, 0, 0, 0)
        pl.setSpacing(6)
        pl.addWidget(section("Önizleme"))
        self.lbl_pinfo = QLabel("Bir dosya seç.")
        self.lbl_pinfo.setObjectName("muted")
        self.lbl_pinfo.setWordWrap(True)
        pl.addWidget(self.lbl_pinfo)
        self.stack = QStackedWidget()
        self.pv_text = QPlainTextEdit()
        self.pv_text.setReadOnly(True)
        self.pv_text.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        self.pv_img = QLabel()
        self.pv_img.setAlignment(Qt.AlignmentFlag.AlignCenter)
        sa = QScrollArea()
        sa.setWidget(self.pv_img)
        sa.setWidgetResizable(True)
        sa.setStyleSheet("QScrollArea { background:#070c18; border:1px solid %s; border-radius:10px; }" % C["border"])
        self.stack.addWidget(self.pv_text)
        self.stack.addWidget(sa)
        pl.addWidget(self.stack, 1)
        split.addWidget(pv)
        split.setSizes([820, 440])
        v.addWidget(split, 1)

        hb = QHBoxLayout()
        b_csv = QPushButton("Listeyi CSV'ye aktar…")
        b_csv.clicked.connect(self._export_csv)
        b_open = QPushButton("Dışarıda aç")
        b_open.setToolTip("Dosyayı geçici klasöre çıkarıp varsayılan uygulamayla açar (çift tık)")
        b_open.clicked.connect(self._open_external)
        b_sel = QPushButton("⤓  Seçilenleri çıkar…")
        b_sel.clicked.connect(lambda: self._extract(True))
        b_all = QPushButton("⤓  Tümünü çıkar…")
        b_all.setObjectName("primary")
        b_all.clicked.connect(lambda: self._extract(False))
        hb.addWidget(b_csv)
        hb.addStretch(1)
        for b in (b_open, b_sel, b_all):
            hb.addWidget(b)
        b_close = QPushButton("Kapat")
        b_close.clicked.connect(self.close)
        hb.addWidget(b_close)
        v.addLayout(hb)

        self._ftimer = QTimer(self, singleShot=True, interval=180)
        self._ftimer.timeout.connect(lambda: (self.model.set_filter(text=self.ed_filter.text()), self._update_count()))
        self.ed_filter.textChanged.connect(lambda _: self._ftimer.start())
        self.cmb_state.currentIndexChanged.connect(lambda _: (self.model.set_filter(state=self.cmb_state.currentData()),
                                                              self._update_count()))
        QShortcut(QKeySequence("Ctrl+F"), self, activated=self.ed_filter.setFocus)
        QShortcut(QKeySequence("Ctrl+E"), self, activated=lambda: self._extract(True))
        self._load()

    # ---------------------------------------------------------------- veri
    def _load(self):
        QApplication.setOverrideCursor(Qt.CursorShape.WaitCursor)
        try:
            with foxfs.PackReader(self.spec.output) as r:          # yalnız index; dosya hemen bırakılır (kilit yok)
                entries = list(r.entries.values())
                self.key, self.iv, fsize = r.key, r.iv, r.file_size
            names, _ = xmltools.load_manifest(self.spec.name)
            src, pyc_names = set(), set()
            if self.plan:
                for f in self.plan.files:
                    names[f.name] = f.archived
                    src.add(f.name)
                for _, pyc_rel in self.plan.pyc_needed:
                    nb = config.encode_name(self.spec.archived(pyc_rel))
                    if nb:
                        h = foxfs.name_hash(nb)
                        names[h] = nb
                        pyc_names.add(h)
            rows = []
            for e in entries:
                nb = names.get(e.name)
                path = display_name(nb) if nb else "<isimsiz %08x>" % e.name
                state = "kaynakta" if e.name in src else "pyc" if e.name in pyc_names else \
                    ("korunan" if self.plan else "—")
                rows.append((path, e.decompressed, e.size, state, e.name, e.digest, e))
            rows.sort(key=lambda x: x[0].lower())
        finally:
            QApplication.restoreOverrideCursor()
        self.model = PackModel(rows)
        self.view.setModel(self.model)
        hh = self.view.horizontalHeader()
        hh.setSectionResizeMode(0, QHeaderView.ResizeMode.Stretch)
        for c, wd in zip(range(1, len(PackModel.HEAD)), (80, 80, 56, 80, 82, 82)):   # ResizeToContents tüm satırları ölçer
            hh.setSectionResizeMode(c, QHeaderView.ResizeMode.Interactive)
            hh.resizeSection(c, wd)
        hh.setSortIndicator(0, Qt.SortOrder.AscendingOrder)          # rows zaten ada göre sıralı
        self.view.selectionModel().currentRowChanged.connect(lambda cur, _: self._preview(cur))
        dec = sum(r[1] for r in rows)
        named = sum(1 for r in rows if not r[0].startswith("<"))
        self.lbl_head.setText("%s   •   %s entry   •   pakette %s / açık %s (oran %%%d)   •   adı bilinen %s / %s" % (
            self.spec.output, fmt_int(len(rows)), fmt_size(fsize), fmt_size(dec), 100 * fsize // max(dec, 1),
            fmt_int(named), fmt_int(len(rows))))
        self._update_count()

    def _update_count(self):
        self.lbl_count.setText("%s gösteriliyor" % fmt_int(len(self.model.rows)))

    def _selected_rows(self):
        return [self.model.rows[i.row()] for i in self.view.selectionModel().selectedRows()]

    def _read(self, f, e):
        f.seek(e.offset)
        return foxfs.decode(f.read(e.size), e.decompressed, self.key, self.iv)

    # ---------------------------------------------------------------- önizleme
    def _preview(self, cur):
        if not cur.isValid():
            return
        r = self.model.rows[cur.row()]
        path, dec, e = r[0], r[1], r[6]
        ext = os.path.splitext(path)[1].lower()
        info = "%s\n%s açık • %s pakette • crc %08x • xxh32 %08x • %s" % (
            path, fmt_size(dec), fmt_size(r[2]), r[4], r[5], r[3])
        if dec > PREVIEW_LIMIT:
            self.lbl_pinfo.setText(info + "\n(önizleme için çok büyük)")
            self.pv_text.setPlainText("")
            self.stack.setCurrentIndex(0)
            return
        try:
            with open(self.spec.output, "rb") as f:
                data = self._read(f, e)
        except Exception as ex:
            self.lbl_pinfo.setText(info + "\nOkunamadı: %s" % ex)
            return
        ok = foxfs.content_hash(data) == e.digest
        info += "" if ok else "\n⚠ hash uyuşmuyor!"
        img = QImage.fromData(data) if ext in IMAGE_EXTS or data[:4] in (b"\x89PNG", b"\xff\xd8\xff\xe0", b"\xff\xd8\xff\xe1") else QImage()
        if not img.isNull():
            pm = QPixmap.fromImage(img)
            if pm.width() > 900 or pm.height() > 900:
                pm = pm.scaled(900, 900, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
            elif max(pm.width(), pm.height()) <= 128:                  # ikonlar: piksel netliğiyle büyüt
                k = max(1, 256 // max(pm.width(), pm.height(), 1))
                pm = pm.scaled(pm.width() * k, pm.height() * k, Qt.AspectRatioMode.KeepAspectRatio,
                               Qt.TransformationMode.FastTransformation)
            self.pv_img.setPixmap(pm)
            self.stack.setCurrentIndex(1)
            self.lbl_pinfo.setText(info + "\n%d × %d px" % (img.width(), img.height()))
            return
        if data[:4] == b"DDS ":
            h, w = int.from_bytes(data[12:16], "little"), int.from_bytes(data[16:20], "little")
            info += "\nDDS doku %d × %d (%s)" % (w, h, data[84:88].decode("latin1", "replace").strip("\0") or "RGB")
        sample = data[:65536]
        printable = sum(1 for b in sample if b >= 32 or b in (9, 10, 13))
        if sample and printable / len(sample) > 0.97 and b"\0" not in sample:
            try:
                text = data[:400000].decode("utf-8")
            except UnicodeDecodeError:
                text = data[:400000].decode("cp949", "replace")
            self.pv_text.setPlainText(text)
        else:
            lines = []
            for off in range(0, min(len(data), 4096), 16):
                chunk = data[off:off + 16]
                lines.append("%06x  %-48s %s" % (off, " ".join("%02x" % b for b in chunk),
                                                 "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)))
            if len(data) > 4096:
                lines.append("… (%s)" % fmt_size(len(data)))
            self.pv_text.setPlainText("\n".join(lines))
        self.stack.setCurrentIndex(0)
        self.lbl_pinfo.setText(info)

    # ---------------------------------------------------------------- eylemler
    def _menu(self, pos):
        m = QMenu(self)
        m.addAction("Dışarıda aç", self._open_external)
        m.addAction("Seçilenleri çıkar…", lambda: self._extract(True))
        m.addSeparator()
        m.addAction("Yolu kopyala", lambda: QApplication.clipboard().setText(
            "\n".join(r[0] for r in self._selected_rows())))
        m.addAction("CRC'yi kopyala", lambda: QApplication.clipboard().setText(
            "\n".join("%08x" % r[4] for r in self._selected_rows())))
        m.exec(self.view.viewport().mapToGlobal(pos))

    def _write_rows(self, rows, out_dir, progress=None):
        done, fails = 0, []
        with open(self.spec.output, "rb") as f:
            for i, r in enumerate(rows):
                if progress and (i % 64 == 0):
                    progress.setValue(i)
                    QApplication.processEvents()
                    if progress.wasCanceled():
                        break
                rel = safe_rel(r[0]) if not r[0].startswith("<") else os.path.join("_isimsiz", "%08x.bin" % r[4])
                dst = config.long_path(os.path.join(out_dir, rel))
                try:
                    data = self._read(f, r[6])
                    os.makedirs(os.path.dirname(dst), exist_ok=True)
                    with open(dst, "wb") as o:
                        o.write(data)
                    done += 1
                except Exception as ex:
                    fails.append("%s: %s" % (r[0], ex))
        return done, fails

    def _extract(self, selected):
        rows = self._selected_rows() if selected else list(self.model.rows)     # tümü = filtrede görünenler
        if not rows:
            QMessageBox.information(self, "Çıkar", "Önce dosya seç.")
            return
        d = QFileDialog.getExistingDirectory(self, "Çıkarılacak klasör", os.path.join(config.DATA_DIR, "extract", self.spec.name))
        if not d:
            return
        pd = QProgressDialog("Çıkarılıyor…", "İptal", 0, len(rows), self)
        pd.setWindowModality(Qt.WindowModality.WindowModal)
        pd.setMinimumDuration(300)
        t0 = time.time()
        done, fails = self._write_rows(rows, d, pd)
        pd.setValue(len(rows))
        msg = "%s dosya çıkarıldı (%.1f sn)\n%s" % (fmt_int(done), time.time() - t0, d)
        if fails:
            msg += "\n\n%d hata:\n%s" % (len(fails), "\n".join(fails[:15]))
        box = QMessageBox(QMessageBox.Icon.Information if not fails else QMessageBox.Icon.Warning, "Çıkar", msg,
                          QMessageBox.StandardButton.Ok, self)
        b = box.addButton("Klasörü aç", QMessageBox.ButtonRole.ActionRole)
        box.exec()
        if box.clickedButton() is b:
            open_path(d)

    def _open_external(self):
        rows = self._selected_rows()[:1]
        if not rows:
            return
        tmp = os.path.join(config.DATA_DIR, "tmp_view")
        n, fails = self._write_rows(rows, tmp)
        if fails:
            QMessageBox.warning(self, "Aç", fails[0])
            return
        r = rows[0]
        rel = safe_rel(r[0]) if not r[0].startswith("<") else os.path.join("_isimsiz", "%08x.bin" % r[4])
        try:
            os.startfile(os.path.join(tmp, rel))
        except OSError:                              # ilişkili uygulama yok: Explorer'da göster
            open_path(os.path.join(tmp, rel), select=True)

    def _export_csv(self):
        path, _ = QFileDialog.getSaveFileName(self, "CSV", os.path.join(config.DATA_DIR, "%s_entries.csv" % self.spec.name),
                                              "CSV (*.csv)")
        if not path:
            return
        with open(path, "w", newline="", encoding="utf-8-sig") as f:
            w = csv.writer(f, delimiter=";")
            w.writerow(["yol", "boyut", "pakette", "durum", "crc32", "xxh32"])
            for r in self.model.rows:
                w.writerow([r[0], r[1], r[2], r[3], "%08x" % r[4], "%08x" % r[5]])
        open_path(path, select=True)


class SummaryDialog(QDialog):
    """Paketleme sonu özeti: pack başına sonuç, boyut farkı, süre."""
    HEAD = ["Pack", "Sonuç", "Entry", "Yeni", "Aynı", "Korunan", "pyc", "Eski", "Yeni boyut", "Fark", "Süre", "Not"]

    def __init__(self, parent, results, seconds):
        super().__init__(parent)
        self.setWindowTitle("Paketleme özeti")
        self.resize(1100, 120 + 34 * min(len(results), 14))
        v = QVBoxLayout(self)
        v.setContentsMargins(14, 12, 14, 12)
        ok = [r for r in results if r.status == "ok"]
        bad = [r for r in results if r.status == "failed"]
        delta = sum(r.pack_bytes - r.old_pack_bytes for r in ok)
        head = QLabel("%d pack paketlendi%s  •  %s entry  •  boyut farkı %s%s  •  toplam %s" % (
            len(ok), (", %d HATA" % len(bad)) if bad else "", fmt_int(sum(r.entries for r in ok)),
            "+" if delta >= 0 else "−", fmt_size(abs(delta)), fmt_dur(seconds)))
        head.setStyleSheet("font-size:11pt; font-weight:600; color:%s" % (C["red"] if bad else C["sky"]))
        v.addWidget(head)
        t = QTableWidget(len(results), len(self.HEAD))
        t.setHorizontalHeaderLabels(self.HEAD)
        t.verticalHeader().setVisible(False)
        t.setEditTriggers(QAbstractItemView.EditTrigger.NoEditTriggers)
        t.setShowGrid(False)
        t.setAlternatingRowColors(True)
        names = {"ok": "Tamam", "skipped": "Atlandı", "failed": "Hata", "cancelled": "İptal"}
        colors = {"ok": C["sky"], "skipped": C["slate"], "failed": C["red"], "cancelled": C["slate"]}
        for i, r in enumerate(results):
            d = r.pack_bytes - r.old_pack_bytes if r.status == "ok" else None
            vals = [(r.name, None), (names[r.status], None), (fmt_int(r.entries), r.entries), (fmt_int(r.encoded), r.encoded),
                    (fmt_int(r.reused), r.reused), (fmt_int(r.carried), r.carried), (str(r.pyc), r.pyc),
                    (fmt_size(r.old_pack_bytes), r.old_pack_bytes), (fmt_size(r.pack_bytes), r.pack_bytes),
                    (("+" if d >= 0 else "−") + fmt_size(abs(d)) if d is not None else "—", d or 0),
                    ("%.1f sn" % r.seconds, r.seconds), (r.message + ("  (%d uyarı)" % len(r.warnings) if r.warnings else ""), None)]
            for c, (text, key) in enumerate(vals):
                it = SortItem(text)
                if key is not None:
                    it.setData(SORT_ROLE, key)
                    it.setTextAlignment(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)
                if c == 1:
                    it.setForeground(QColor(colors[r.status]))
                t.setItem(i, c, it)
        hh = t.horizontalHeader()
        hh.setSortIndicator(-1, Qt.SortOrder.AscendingOrder)       # açılışta paketleme sırası; başlığa tıkla sırala
        t.setSortingEnabled(True)
        hh.setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
        hh.setSectionResizeMode(len(self.HEAD) - 1, QHeaderView.ResizeMode.Stretch)
        v.addWidget(t, 1)
        hb = QHBoxLayout()
        hb.addStretch(1)
        b = QPushButton("Kapat")
        b.setObjectName("primary")
        b.clicked.connect(self.accept)
        hb.addWidget(b)
        v.addLayout(hb)


# ------------------------------------------------------------------------------------------- ana pencere
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.settings = config.load_settings()
        self.specs = []
        self.plans = {}
        self.results = {}
        self.broken_found = {}
        self.checked = set()
        self.thread = None
        self.worker = None
        self.ui = QSettings(UI_INI, QSettings.Format.IniFormat)
        self._log_entries = collections.deque(maxlen=20000)
        self._logf = None
        self._logf_day = ""
        self._run_results = []
        self._t_start = 0.0
        self._tray = None
        self.setWindowTitle("Anka2 Pack Studio")
        self.setWindowIcon(app_icon())
        self.resize(1480, 920)
        self._build_ui()
        self._load_settings_into_ui()
        self._shortcuts()
        self._restore_ui_state()
        self._elapsed_timer = QTimer(self, interval=1000)
        self._elapsed_timer.timeout.connect(self._tick)
        self._status_timer = QTimer(self, interval=3000)
        self._status_timer.timeout.connect(self._update_status)
        self._status_timer.start()
        self._update_status()
        QTimer.singleShot(150, self.scan)

    # ================================================================ kısayollar + kalıcı arayüz durumu
    def _shortcuts(self):
        for seq, fn in (("F5", self.scan), ("Ctrl+B", lambda: self.build(selected=True)),
                        ("Ctrl+Shift+B", lambda: self.build(selected=False)), ("Ctrl+Shift+V", self.verify),
                        ("Ctrl+E", self.open_explorer), ("Ctrl+F", lambda: (self.txt_filter.setFocus(), self.txt_filter.selectAll())),
                        ("Ctrl+L", self._clear_log), ("Escape", lambda: self._is_busy() and self.stop())):
            QShortcut(QKeySequence(seq), self, activated=fn)

    def _restore_ui_state(self):
        g = self.ui.value("geometry")
        if g is not None:
            self.restoreGeometry(g)
        for key, sp in (("hsplit", self.hsplit), ("vsplit", self.vsplit)):
            s = self.ui.value(key)
            if s is not None:
                sp.restoreState(s)
        h = self.ui.value("table_header")
        if h is not None:
            self.table.horizontalHeader().restoreState(h)
        for key, tabs in (("settings_tab", self.set_tabs), ("bottom_tab", self.tabs)):
            try:
                tabs.setCurrentIndex(int(self.ui.value(key, 0)))
            except (TypeError, ValueError):
                pass

    def _save_ui_state(self):
        self.ui.setValue("geometry", self.saveGeometry())
        self.ui.setValue("hsplit", self.hsplit.saveState())
        self.ui.setValue("vsplit", self.vsplit.saveState())
        self.ui.setValue("table_header", self.table.horizontalHeader().saveState())
        self.ui.setValue("settings_tab", self.set_tabs.currentIndex())
        self.ui.setValue("bottom_tab", self.tabs.currentIndex())
        self.ui.sync()

    def _update_status(self):
        run = client_running()
        self.sb_client.setText("●  Client %s" % ("açık — pack'ler kilitli" if run else "kapalı"))
        self.sb_client.setStyleSheet("color:%s" % (C["amber"] if run else C["muted"]))
        _, pk, _ = config.paths(self.settings)
        try:
            du = shutil.disk_usage(pk if os.path.isdir(pk) else os.path.splitdrive(pk)[0] + "\\")
            self.sb_disk.setText("Disk: %s boş" % fmt_size(du.free))
            self.sb_disk.setStyleSheet("color:%s" % (C["red"] if du.free < 2 * 2**30 else C["muted"]))
        except OSError:
            self.sb_disk.setText("")

    def _tick(self):
        if self._is_busy():
            self.lbl_elapsed.setText("Geçen süre  %s" % fmt_dur(time.time() - self._t_start))

    # ================================================================ yerleşim
    def _build_ui(self):
        root = QWidget()
        root.setObjectName("root")
        self.setCentralWidget(root)
        outer = QVBoxLayout(root)
        outer.setContentsMargins(0, 0, 0, 0)
        outer.setSpacing(0)
        outer.addWidget(self._header())

        body = QWidget()
        bl = QVBoxLayout(body)
        bl.setContentsMargins(18, 14, 18, 10)
        bl.setSpacing(12)
        outer.addWidget(body, 1)

        stats = QHBoxLayout()
        stats.setSpacing(10)
        self.st_packs = StatCard("pack (kaynak klasör)", C["accent"])
        self.st_files = StatCard("kaynak dosya", C["sky"])
        self.st_new = StatCard("pakete girmemiş dosya", C["amber"])
        self.st_keep = StatCard("korunan entry (kaynakta yok)", C["violet"])
        self.st_bad = StatCard("bozuk adlı dosya", C["red"])
        self.st_size = StatCard("toplam pack boyutu", C["blue"])
        for c in (self.st_packs, self.st_files, self.st_new, self.st_keep, self.st_bad, self.st_size):
            stats.addWidget(c)
        bl.addLayout(stats)

        split = QSplitter(Qt.Orientation.Horizontal)
        split.setHandleWidth(10)
        self.hsplit = split
        bl.addWidget(split, 1)

        left = QWidget()
        ll = QVBoxLayout(left)
        ll.setContentsMargins(0, 0, 0, 0)
        ll.setSpacing(8)
        ll.addLayout(self._table_toolbar())
        vsplit = QSplitter(Qt.Orientation.Vertical)
        vsplit.setHandleWidth(10)
        self.vsplit = vsplit
        vsplit.addWidget(self._table())
        vsplit.addWidget(self._bottom_tabs())
        vsplit.setSizes([520, 280])
        ll.addWidget(vsplit, 1)
        split.addWidget(left)

        right = QWidget()
        rl = QVBoxLayout(right)
        rl.setContentsMargins(0, 0, 0, 0)
        rl.setSpacing(10)
        rl.addWidget(self._settings_tabs(), 1)
        rl.addWidget(self._progress_card())
        right.setMinimumWidth(390)
        split.addWidget(right)
        split.setSizes([1010, 400])

        self.sb_paths = QLabel()
        self.statusBar().addWidget(self.sb_paths, 1)
        self.sb_client = QLabel()
        self.sb_disk = QLabel()
        sb_r = QLabel("Python %s  •  %d çekirdek  •  v%s" % (sys.version.split()[0], os.cpu_count() or 1, __version__))
        sb_r.setToolTip("Kısayollar: F5 analiz • Ctrl+B seçilenleri paketle • Ctrl+Shift+B tümü • Ctrl+Shift+V doğrula\n"
                        "Ctrl+E pack gezgini • Ctrl+F ara • Ctrl+L günlüğü temizle • Esc durdur")
        for w in (self.sb_client, self.sb_disk, sb_r):
            w.setContentsMargins(10, 0, 10, 0)
            self.statusBar().addPermanentWidget(w)

    def _header(self):
        header = QFrame()
        header.setObjectName("header")
        hl = QHBoxLayout(header)
        hl.setContentsMargins(20, 14, 20, 14)
        hl.setSpacing(10)
        logo = QLabel()
        logo.setPixmap(app_icon().pixmap(42, 42))
        hl.addWidget(logo)
        tl = QVBoxLayout()
        tl.setSpacing(0)
        t = QLabel("Anka2 Pack Studio")
        t.setObjectName("title")
        self.lbl_sub = QLabel()
        self.lbl_sub.setObjectName("subtitle")
        tl.addWidget(t)
        tl.addWidget(self.lbl_sub)
        hl.addLayout(tl)
        hl.addStretch(1)

        self.btn_scan = QPushButton("⟳  Analiz")
        self.btn_verify = QPushButton("✓  Doğrula")
        self.btn_clean = QPushButton("⌫  Temizle")
        self.btn_restore = QPushButton("⤓  Kaynak Onar")
        self.btn_build_sel = QPushButton("▶  Seçilenleri Paketle")
        self.btn_build_sel.setObjectName("primary")
        self.btn_build_all = QPushButton("▶▶  Tümünü Paketle")
        self.btn_build_all.setObjectName("primary")
        self.btn_stop = QPushButton("■  Durdur")
        self.btn_stop.setObjectName("danger")
        self.btn_stop.setEnabled(False)
        self.btn_explore = QPushButton("☰  Gezgin")
        self.btn_scan.setToolTip("Kaynak kökü yeniden keşfeder, mevcut pack'lerle karşılaştırır (pack yazmaz)  [F5]")
        self.btn_verify.setToolTip("Seçili (yoksa tüm) pack'lerin bütün entry'lerini çözüp hash doğrular  [Ctrl+Shift+V]")
        self.btn_clean.setToolTip("Adı unpack sırasında bozulmuş dosyaları temizler (Temizlik sekmesindeki ayarlarla)")
        self.btn_restore.setToolTip("Kaynakta silinmiş ama pack'te duran dosyaları eski listelerden adlarıyla geri çıkarır")
        self.btn_explore.setToolTip("Seçili pack'in içeriğini gez, önizle, dosya çıkar  [Ctrl+E / çift tık]")
        self.btn_build_sel.setToolTip("İşaretli (yoksa seçili satır) pack'leri paketler  [Ctrl+B]")
        self.btn_build_all.setToolTip("Tüm pack'leri paketler  [Ctrl+Shift+B]")
        self.btn_stop.setToolTip("Çalışan işi durdurur; yarım pack yazılmaz  [Esc]")
        self.btn_dirs = QToolButton()
        self.btn_dirs.setText("📁  Klasörler")
        self.btn_dirs.setPopupMode(QToolButton.ToolButtonPopupMode.InstantPopup)
        self.btn_dirs.setMenu(self._dirs_menu())
        for b in (self.btn_scan, self.btn_explore, self.btn_verify, self.btn_clean, self.btn_restore, self.btn_dirs):
            hl.addWidget(b)
        sep = QFrame()
        sep.setFixedWidth(1)
        sep.setStyleSheet("background:%s" % C["border"])
        hl.addWidget(sep)
        for b in (self.btn_build_sel, self.btn_build_all, self.btn_stop):
            hl.addWidget(b)

        self.btn_scan.clicked.connect(self.scan)
        self.btn_explore.clicked.connect(self.open_explorer)
        self.btn_verify.clicked.connect(self.verify)
        self.btn_clean.clicked.connect(self.clean_now)
        self.btn_restore.clicked.connect(self.restore_sources)
        self.btn_build_sel.clicked.connect(lambda: self.build(selected=True))
        self.btn_build_all.clicked.connect(lambda: self.build(selected=False))
        self.btn_stop.clicked.connect(self.stop)
        return header

    def _dirs_menu(self):
        m = QMenu(self)

        def add(label, fn):
            m.addAction(label, lambda: open_path(fn()))
        add("Kaynak kök", lambda: config.paths(self.settings)[0])
        add("Pack klasörü", lambda: config.paths(self.settings)[1])
        add("Config (XML)", lambda: config.paths(self.settings)[2])
        m.addSeparator()
        add("Test çıktısı", lambda: self.txt_out.text().strip() or os.path.join(config.DATA_DIR, "test_out"))
        add("Yedekler", lambda: os.path.join(config.DATA_DIR, "backups"))
        add("Karantina", lambda: os.path.join(config.DATA_DIR, "quarantine"))
        add("Çıkarılan dosyalar", lambda: os.path.join(config.DATA_DIR, "extract"))
        add("Günlük dosyaları", lambda: LOG_DIR)
        m.addSeparator()
        add("Pack Studio veri klasörü", lambda: config.DATA_DIR)
        return m

    def _table_toolbar(self):
        h = QHBoxLayout()
        h.setSpacing(6)
        self.txt_filter = QLineEdit()
        self.txt_filter.setPlaceholderText("Pack ara…  (Ctrl+F)")
        self.txt_filter.setClearButtonEnabled(True)
        self.txt_filter.setMinimumWidth(190)
        self.txt_filter.setMaximumWidth(260)
        self.txt_filter.textChanged.connect(self._apply_filter)
        h.addWidget(self.txt_filter)
        self.cmb_state = QComboBox()
        for label, key in (("Tüm durumlar", ""), ("Eksik var", "missing"), ("Güncel", "Güncel"), ("Yeni pack", "Yeni pack"),
                           ("Korunan entry'li", "carry"), ("Bozuk adlı", "bad"), ("Prefix tahmini ⚠", "guess"),
                           ("Hata / bozuk", "error")):
            self.cmb_state.addItem(label, key)
        self.cmb_state.currentIndexChanged.connect(self._apply_filter)
        h.addWidget(self.cmb_state)
        h.addSpacing(6)
        for text, fn in (("Tümünü seç", lambda: self._check_where(lambda n: True)),
                         ("Eksik olanlar", lambda: self._check_where(self._has_missing)),
                         ("Seçimi kaldır", lambda: self._check_where(lambda n: False))):
            b = QPushButton(text)
            b.setObjectName("link")
            b.clicked.connect(fn)
            h.addWidget(b)
        h.addStretch(1)
        self.lbl_sel = QLabel("")
        self.lbl_sel.setObjectName("muted")
        h.addWidget(self.lbl_sel)
        return h

    def _table(self):
        self.table = QTableWidget(0, len(COLS))
        self.table.setHorizontalHeaderLabels(COLS)
        self.table.setAlternatingRowColors(True)
        self.table.verticalHeader().setVisible(False)
        self.table.verticalHeader().setDefaultSectionSize(34)
        self.table.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
        self.table.setSelectionMode(QAbstractItemView.SelectionMode.ExtendedSelection)
        self.table.setEditTriggers(QAbstractItemView.EditTrigger.NoEditTriggers)
        self.table.setShowGrid(False)
        self.table.setItemDelegateForColumn(COL_STATE, PillDelegate(self.table))
        hh = self.table.horizontalHeader()
        hh.setSectionResizeMode(QHeaderView.ResizeMode.ResizeToContents)
        hh.setSectionResizeMode(COL_NAME, QHeaderView.ResizeMode.Stretch)
        hh.setSectionResizeMode(COL_STATE, QHeaderView.ResizeMode.Fixed)
        self.table.setColumnWidth(COL_STATE, 132)
        hh.setHighlightSections(False)
        hh.setSortIndicatorShown(True)
        self.table.setSortingEnabled(True)
        self.table.sortByColumn(COL_NAME, Qt.SortOrder.AscendingOrder)
        self.table.itemSelectionChanged.connect(self._on_select)
        self.table.itemChanged.connect(self._on_item_changed)
        self.table.cellDoubleClicked.connect(lambda r, c: c != COL_CHECK and self.open_explorer())
        self.table.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self.table.customContextMenuRequested.connect(self._table_menu)
        return self.table

    def _row_names(self):
        return [self.table.item(r, COL_NAME).text() for r in sorted({i.row() for i in self.table.selectedItems()})]

    def _table_menu(self, pos):
        r = self.table.rowAt(pos.y())
        if r < 0:
            return
        if not self.table.item(r, COL_NAME).isSelected():
            self.table.selectRow(r)
        names = self._row_names()
        p = self.plans.get(names[0]) if names else None
        if not p:
            return
        busy = self._is_busy()
        s = p.spec
        m = QMenu(self)
        title = names[0] if len(names) == 1 else "%d pack" % len(names)

        def act(label, fn, enabled=True):
            a = m.addAction(label, fn)
            a.setEnabled(enabled)
            return a
        act("▶  Paketle: %s" % title, lambda: self.build(True, names), not busy)
        act("▶  Test klasörüne paketle", lambda: self.build(True, names, test=True), not busy)
        act("✓  Doğrula", lambda: self._start("verify", names), not busy and any(
            os.path.isfile(self.plans[n].spec.output) for n in names))
        m.addSeparator()
        act("☰  Pack Gezgini", self.open_explorer, len(names) == 1 and os.path.isfile(s.output))
        act("Kaynak klasörünü aç", lambda: open_path(s.source), os.path.isdir(s.source))
        act("Pack dosyasını göster", lambda: open_path(s.output, select=True), os.path.isfile(s.output))
        act("Config XML'i aç", lambda: os.startfile(s.config_path), bool(s.config_path and os.path.isfile(s.config_path)))
        act("Pack ayarlarını düzenle", lambda: self.tabs.setCurrentIndex(2), len(names) == 1)
        m.addSeparator()
        act("İşaretle", lambda: self._set_checked(names, True))
        act("İşareti kaldır", lambda: self._set_checked(names, False))
        act("Adı kopyala", lambda: QApplication.clipboard().setText(", ".join(names)))
        m.exec(self.table.viewport().mapToGlobal(pos))

    def _set_checked(self, names, on):
        names = set(names)
        self._check_where(lambda n: (n in names and on) or (n not in names and n in self.checked))

    def open_explorer(self):
        name = self._selected_name()
        p = self.plans.get(name) if name else None
        if not p:
            QMessageBox.information(self, "Pack Gezgini", "Önce tablodan bir pack seç.")
            return
        if not os.path.isfile(p.spec.output):
            QMessageBox.information(self, "Pack Gezgini", "%s henüz paketlenmemiş:\n%s" % (name, p.spec.output))
            return
        try:
            dlg = PackExplorer(self, p.spec, p)
        except Exception as ex:
            QMessageBox.warning(self, "Pack Gezgini", "Pack okunamadı:\n%s" % ex)
            return
        dlg.setAttribute(Qt.WidgetAttribute.WA_DeleteOnClose)
        dlg.show()

    def _bottom_tabs(self):
        tabs = QTabWidget()
        self.log_view = QPlainTextEdit()
        self.log_view.setReadOnly(True)
        self.log_view.setMaximumBlockCount(8000)
        lw = QWidget()
        lv = QVBoxLayout(lw)
        lv.setContentsMargins(6, 6, 6, 6)
        lv.setSpacing(4)
        lt = QHBoxLayout()
        self.cmb_loglevel = QComboBox()
        for label, levels in (("Tüm kayıtlar", None), ("Uyarı + hata", {"warn", "err"}), ("Yalnız hata", {"err"}),
                              ("Başarılı", {"ok"})):
            self.cmb_loglevel.addItem(label, levels)
        self.cmb_loglevel.currentIndexChanged.connect(self._rerender_log)
        self.txt_logfind = QLineEdit()
        self.txt_logfind.setPlaceholderText("Günlükte ara…")
        self.txt_logfind.setClearButtonEnabled(True)
        self.txt_logfind.setMaximumWidth(240)
        self._logfind_timer = QTimer(self, singleShot=True, interval=200)
        self._logfind_timer.timeout.connect(self._rerender_log)
        self.txt_logfind.textChanged.connect(lambda _: self._logfind_timer.start())
        lt.addWidget(self.cmb_loglevel)
        lt.addWidget(self.txt_logfind)
        lt.addStretch(1)
        for text, fn, tip in (("Kopyala", self._copy_log, "Görünen günlüğü panoya kopyala"),
                              ("Kaydet…", self._save_log, "Görünen günlüğü dosyaya kaydet"),
                              ("Temizle", self._clear_log, "Ekranı temizle (Ctrl+L) — günlük dosyası korunur"),
                              ("Dosyalar", lambda: open_path(LOG_DIR), "Günlük dosyaları klasörü (_data/logs)")):
            b = QPushButton(text)
            b.setObjectName("link")
            b.setToolTip(tip)
            b.clicked.connect(fn)
            lt.addWidget(b)
        lv.addLayout(lt)
        lv.addWidget(self.log_view, 1)
        self.detail = QListWidget()
        self.detail.setUniformItemSizes(True)
        dw = QWidget()
        dv = QVBoxLayout(dw)
        dv.setContentsMargins(6, 6, 6, 6)
        dv.setSpacing(4)
        self.txt_detfind = QLineEdit()
        self.txt_detfind.setPlaceholderText("Detayda ara…")
        self.txt_detfind.setClearButtonEnabled(True)
        self.txt_detfind.setMaximumWidth(280)
        self.txt_detfind.textChanged.connect(self._filter_detail)
        dv.addWidget(self.txt_detfind)
        dv.addWidget(self.detail, 1)
        tabs.addTab(lw, "Günlük")
        tabs.addTab(dw, "Pack detayı")
        tabs.addTab(self._pack_editor(), "Pack ayarları")
        self.tabs = tabs
        return tabs

    def _pack_editor(self):
        w = QWidget()
        f = QFormLayout(w)
        f.setContentsMargins(14, 12, 14, 12)
        f.setHorizontalSpacing(14)
        f.setVerticalSpacing(8)
        self.ed_name = QLineEdit()
        self.ed_name.setReadOnly(True)
        self.ed_source = QLineEdit()
        self.ed_source.setReadOnly(True)
        self.ed_config = QLineEdit()
        self.ed_config.setReadOnly(True)
        pr = QHBoxLayout()
        self.ed_prefix = QLineEdit()
        self.ed_prefix.setPlaceholderText("örn. d:/ymir work/effect/  (boş = kök)")
        self.btn_detect = QPushButton("Pack'ten tespit et")
        self.btn_detect.setToolTip("Prefix'i aynı adlı mevcut pack'in index'ine göre ölçer")
        self.btn_detect.clicked.connect(self._detect_prefix)
        pr.addWidget(self.ed_prefix, 1)
        pr.addWidget(self.btn_detect)
        self.lbl_prefix_info = QLabel("")
        self.lbl_prefix_info.setObjectName("muted")
        self.ed_ignores = QPlainTextEdit()
        self.ed_ignores.setPlaceholderText("Her satıra bir desen. Regex göreli yolun tamamına uygulanır;\n'*.psd' gibi glob da olur.")
        self.ed_ignores.setFixedHeight(70)
        self.btn_save_cfg = QPushButton("Kaydet")
        self.btn_save_cfg.setObjectName("primary")
        self.btn_save_cfg.clicked.connect(self._save_pack_config)
        f.addRow("Pack", self.ed_name)
        f.addRow("Kaynak", self.ed_source)
        f.addRow("Config", self.ed_config)
        f.addRow("Prefix", pr)
        f.addRow("", self.lbl_prefix_info)
        f.addRow("Ignore", self.ed_ignores)
        f.addRow("", self.btn_save_cfg)
        self._set_editor_enabled(False)
        return w

    def _settings_tabs(self):
        tabs = QTabWidget()
        self.set_tabs = tabs

        # --- Paketleme
        w = QWidget()
        g = QGridLayout(w)
        g.setContentsMargins(14, 14, 14, 14)
        g.setVerticalSpacing(9)
        g.addWidget(QLabel("Sıkıştırma"), 0, 0)
        self.cmb_level = QComboBox()
        for k, v in LEVEL_LABELS:
            self.cmb_level.addItem(v, k)
        g.addWidget(self.cmb_level, 0, 1)
        g.addWidget(QLabel("İşçi (çekirdek)"), 1, 0)
        self.spn_workers = QSpinBox()
        self.spn_workers.setRange(1, max(1, os.cpu_count() or 1))
        g.addWidget(self.spn_workers, 1, 1)
        self.chk_keep = QCheckBox("Kaynakta olmayan mevcut entry'leri koru")
        self.chk_keep.setToolTip("Kaynak klasörden silinmiş dosyalar pack'ten düşmez (şifreli hali aynen taşınır)")
        self.chk_incr = QCheckBox("Artımlı: değişmeyen dosyaları yeniden sıkıştırma")
        self.chk_verify = QCheckBox("Yazdıktan sonra tüm entry'leri doğrula")
        self.chk_backup = QCheckBox("Eski pack'i yedekle (pack başına son 1)")
        for i, c in enumerate((self.chk_keep, self.chk_incr, self.chk_verify, self.chk_backup)):
            g.addWidget(c, 2 + i, 0, 1, 2)
        g.addWidget(QLabel(".pyc üret"), 6, 0)
        self.txt_pyc = QLineEdit()
        self.txt_pyc.setToolTip("Python 2.7 .pyc üretilecek pack adları (virgülle)")
        g.addWidget(self.txt_pyc, 6, 1)
        g.addWidget(section("Hedef"), 7, 0, 1, 2)
        self.rad_live = QRadioButton("Pack klasörü (canlı)")
        self.rad_test = QRadioButton("Test klasörü")
        self.rad_live.setChecked(True)
        g.addWidget(self.rad_live, 8, 0, 1, 2)
        g.addWidget(self.rad_test, 9, 0, 1, 2)
        self.txt_out = QLineEdit(os.path.join(config.DATA_DIR, "test_out"))
        g.addWidget(path_row(self.txt_out, lambda: self._browse(self.txt_out, select_test=True)), 10, 0, 1, 2)
        g.setRowStretch(11, 1)
        g.setColumnStretch(1, 1)
        tabs.addTab(w, "Paketleme")

        # --- Dizinler
        w = QWidget()
        f = QFormLayout(w)
        f.setContentsMargins(14, 14, 14, 14)
        f.setVerticalSpacing(10)
        self.txt_src = QLineEdit()
        self.txt_pack = QLineEdit()
        self.txt_cfg = QLineEdit()
        self.txt_src.setPlaceholderText("varsayılan: Tools/binary_unpack")
        self.txt_pack.setPlaceholderText("varsayılan: Client/pack")
        self.txt_cfg.setPlaceholderText("varsayılan: Tools/archiver/make_xml")
        f.addRow(section("Klasörler"))
        f.addRow("Kaynak kök", path_row(self.txt_src, lambda: self._browse(self.txt_src)))
        f.addRow("Pack klasörü", path_row(self.txt_pack, lambda: self._browse(self.txt_pack)))
        f.addRow("Config (XML)", path_row(self.txt_cfg, lambda: self._browse(self.txt_cfg)))
        info = QLabel("Kaynak kökteki her klasör bir pack'tir. Proje içindeki yollar göreli kaydedilir; "
                      "boş bırakılan alan varsayılanı kullanır.")
        info.setWordWrap(True)
        info.setObjectName("muted")
        f.addRow(info)
        self.chk_autocfg = QCheckBox("Yeni klasörler için XML'i otomatik oluştur")
        f.addRow(self.chk_autocfg)
        hb = QHBoxLayout()
        b1 = QPushButton("Varsayılanlar")
        b1.clicked.connect(self._reset_dirs)
        b2 = QPushButton("Uygula ve tara")
        b2.setObjectName("primary")
        b2.clicked.connect(self._apply_dirs)
        hb.addWidget(b1)
        hb.addStretch(1)
        hb.addWidget(b2)
        f.addRow(hb)
        self.lbl_resolved = QLabel()
        self.lbl_resolved.setWordWrap(True)
        self.lbl_resolved.setObjectName("muted")
        self.lbl_resolved.setTextInteractionFlags(Qt.TextInteractionFlag.TextSelectableByMouse)
        f.addRow(self.lbl_resolved)
        tabs.addTab(w, "Dizinler")

        # --- Temizlik
        w = QWidget()
        v = QVBoxLayout(w)
        v.setContentsMargins(14, 14, 14, 14)
        v.setSpacing(8)
        expl = QLabel("Adı unpack sırasında bozulmuş dosyalar (�) client tarafından hiçbir adla istenemez ve "
                      "paketlenemez. Gerçek Korece asset adlarına (cp949) dokunulmaz.")
        expl.setWordWrap(True)
        expl.setObjectName("muted")
        v.addWidget(expl)
        v.addWidget(section("Mod"))
        self.grp_mode = QButtonGroup(self)
        for i, (k, t) in enumerate((("off", "Kapalı"), ("manual", "Manuel — 'Temizle' butonu ile"),
                                    ("auto", "Otomatik — her paketlemeden önce"))):
            r = QRadioButton(t)
            r.setProperty("key", k)
            self.grp_mode.addButton(r, i)
            v.addWidget(r)
        v.addWidget(section("Eylem"))
        self.grp_action = QButtonGroup(self)
        for i, (k, t) in enumerate((("quarantine", "Karantinaya taşı (geri alınabilir)"), ("delete", "Kalıcı sil"))):
            r = QRadioButton(t)
            r.setProperty("key", k)
            self.grp_action.addButton(r, i)
            v.addWidget(r)
        self.chk_copies = QCheckBox("Windows kopyalarını da temizle ('- Kopya', '- Copy', '복사본')")
        v.addWidget(self.chk_copies)
        hb = QHBoxLayout()
        b1 = QPushButton("Tara ve listele")
        b1.clicked.connect(self.broken_scan)
        self.btn_clean2 = QPushButton("Şimdi temizle")
        self.btn_clean2.setObjectName("danger")
        self.btn_clean2.clicked.connect(self.clean_now)
        hb.addWidget(b1)
        hb.addStretch(1)
        hb.addWidget(self.btn_clean2)
        v.addLayout(hb)
        self.lbl_broken = QLabel("")
        self.lbl_broken.setObjectName("muted")
        self.lbl_broken.setWordWrap(True)
        v.addWidget(self.lbl_broken)
        bq = QPushButton("Karantina klasörünü aç")
        bq.setObjectName("link")
        bq.clicked.connect(lambda: self._open_dir(os.path.join(config.DATA_DIR, "quarantine")))
        v.addWidget(bq, 0, Qt.AlignmentFlag.AlignLeft)
        v.addStretch(1)
        tabs.addTab(w, "Temizlik")

        # --- Property (property.xml üreteci)
        w = QWidget()
        v = QVBoxLayout(w)
        v.setContentsMargins(14, 14, 14, 14)
        v.setSpacing(8)
        expl = QLabel("Client haritalardaki bina/ağaç/efekt objelerini root pack'teki property.xml'den CRC ile bulur; "
                      "listede olmayan obje oyunda görünmez. Üreteç property klasöründeki tüm YPRT dosyalarını okur.")
        expl.setWordWrap(True)
        expl.setObjectName("muted")
        v.addWidget(expl)
        self.chk_propauto = QCheckBox("Her paketlemeden önce otomatik güncelle\n(değiştiyse root da paketlenir)")
        v.addWidget(self.chk_propauto)
        v.addWidget(section("Mod"))
        self.grp_propmode = QButtonGroup(self)
        for i, (k, t) in enumerate((("merge", "Birleştir — kaynakta olmayan eski girdileri koru (önerilen)"),
                                    ("fresh", "Sıfırdan — yalnız 'oluştur' butonunda; haritada obje kaybettirebilir"))):
            r = QRadioButton(t)
            r.setProperty("key", k)
            self.grp_propmode.addButton(r, i)
            v.addWidget(r)
        self.chk_proppack = QCheckBox("Property pack'inde olup kaynakta olmayanları da ekle")
        v.addWidget(self.chk_proppack)
        hb = QHBoxLayout()
        bcheck = QPushButton("Harita kontrolü")
        bcheck.setToolTip("Haritaların (areadata.txt) kullandığı ama property.xml'de olmayan CRC'leri listeler")
        bcheck.clicked.connect(lambda: self._start("propxml"))
        self.btn_propgen = QPushButton("property.xml oluştur")
        self.btn_propgen.setObjectName("primary")
        self.btn_propgen.clicked.connect(lambda: self._start("propxml"))
        hb.addWidget(bcheck)
        hb.addStretch(1)
        hb.addWidget(self.btn_propgen)
        v.addLayout(hb)
        self.lbl_prop = QLabel("")
        self.lbl_prop.setWordWrap(True)
        self.lbl_prop.setObjectName("muted")
        v.addWidget(self.lbl_prop)
        v.addStretch(1)
        tabs.addTab(w, "Property")

        # --- XML (config yönetimi)
        w = QWidget()
        v = QVBoxLayout(w)
        v.setContentsMargins(14, 14, 14, 14)
        v.setSpacing(8)
        v.addWidget(section("Toplu"))
        b = QPushButton("Eksik config XML'lerini oluştur")
        b.setToolTip("XML'i olmayan tüm kaynak klasörleri için config yazar (prefix pack'ten ölçülür)")
        b.clicked.connect(lambda: self._start("bulkxml", self._checked_names(False) or None))
        v.addWidget(b)
        b = QPushButton("Seçili / tüm config'leri yeniden oluştur")
        b.setToolTip("Prefix'i mevcut pack'ten yeniden ölçerek XML'leri yeniden yazar (ignore desenleri korunur)")
        b.clicked.connect(self._regen_xml)
        v.addWidget(b)
        v.addWidget(section("Özel"))
        b = QPushButton("＋  Yeni pack…")
        b.setObjectName("primary")
        b.clicked.connect(self._new_pack_dialog)
        v.addWidget(b)
        v.addWidget(section("Eski Archiver uyumluluğu"))
        b = QPushButton("Dosya listesi XML'i dışa aktar…")
        b.setToolTip("Seçili (yoksa tüm) pack'ler için eski Archiver.exe'nin okuduğu <File ArchivedPath> listesini yazar")
        b.clicked.connect(self._export_legacy)
        v.addWidget(b)
        info = QLabel("Her paketlemede pack'teki dosya adları ad hafızasına (_data/manifests) kaydedilir; "
                      "kaynak silinse bile 'Kaynak Onar' dosyaları adlarıyla geri çıkarabilir.")
        info.setWordWrap(True)
        info.setObjectName("muted")
        v.addWidget(info)
        v.addStretch(1)
        tabs.addTab(w, "XML")
        self.chk_propauto.toggled.connect(self._save_settings)
        self.chk_proppack.toggled.connect(self._save_settings)
        self.grp_propmode.buttonToggled.connect(self._save_settings)

        # değişiklikleri kaydet
        for wdg in (self.cmb_level,):
            wdg.currentIndexChanged.connect(self._save_settings)
        self.spn_workers.valueChanged.connect(self._save_settings)
        for c in (self.chk_keep, self.chk_incr, self.chk_verify, self.chk_backup, self.chk_autocfg, self.chk_copies):
            c.toggled.connect(self._save_settings)
        self.txt_pyc.editingFinished.connect(self._save_settings)
        self.grp_mode.buttonToggled.connect(self._save_settings)
        self.grp_action.buttonToggled.connect(self._save_settings)
        return tabs

    def _progress_card(self):
        card = QFrame()
        card.setObjectName("card")
        v = QVBoxLayout(card)
        v.setContentsMargins(16, 12, 16, 14)
        v.setSpacing(6)
        v.addWidget(section("İlerleme"))
        self.lbl_cur = QLabel("Hazır")
        self.pb_cur = QProgressBar()
        self.pb_cur.setRange(0, 1000)
        self.lbl_rate = QLabel(" ")
        self.lbl_rate.setObjectName("muted")
        self.lbl_all = QLabel("Toplam")
        self.lbl_all.setObjectName("muted")
        self.pb_all = QProgressBar()
        self.pb_all.setObjectName("overall")
        self.pb_all.setRange(0, 1000)
        self.lbl_elapsed = QLabel(" ")
        self.lbl_elapsed.setObjectName("muted")
        for wdg in (self.lbl_cur, self.pb_cur, self.lbl_rate, self.lbl_all, self.pb_all, self.lbl_elapsed):
            v.addWidget(wdg)
        return card

    # ================================================================ ayarlar
    def _load_settings_into_ui(self):
        s = self.settings
        widgets = (self.cmb_level, self.spn_workers, self.chk_keep, self.chk_incr, self.chk_verify, self.chk_backup,
                   self.chk_autocfg, self.chk_copies, self.txt_pyc, self.grp_mode, self.grp_action,
                   self.chk_propauto, self.chk_proppack, self.grp_propmode)
        for wdg in widgets:
            wdg.blockSignals(True)
        self.cmb_level.setCurrentIndex(max(0, [k for k, _ in LEVEL_LABELS].index(s.get("level", "max"))))
        self.spn_workers.setValue(s["workers"])
        self.chk_keep.setChecked(s["keep_missing"])
        self.chk_incr.setChecked(s["incremental"])
        self.chk_verify.setChecked(s["verify"])
        self.chk_backup.setChecked(s["backup"])
        self.chk_autocfg.setChecked(s.get("auto_config", True))
        self.chk_copies.setChecked(s.get("broken_copies", False))
        self.txt_pyc.setText(", ".join(s.get("compile_pyc", [])))
        for b in self.grp_mode.buttons():
            b.setChecked(b.property("key") == s.get("broken_names", "manual"))
        for b in self.grp_action.buttons():
            b.setChecked(b.property("key") == s.get("broken_action", "quarantine"))
        self.chk_propauto.setChecked(s.get("auto_property_xml", True))
        self.chk_proppack.setChecked(s.get("property_include_pack", True))
        for b in self.grp_propmode.buttons():
            b.setChecked(b.property("key") == s.get("property_xml_mode", "merge"))
        self.txt_src.setText(s.get("source_root", ""))
        self.txt_pack.setText(s.get("pack_dir", ""))
        self.txt_cfg.setText(s.get("config_dir", ""))
        for wdg in widgets:
            wdg.blockSignals(False)
        self._refresh_paths()

    def _save_settings(self, *_):
        mode = next((b.property("key") for b in self.grp_mode.buttons() if b.isChecked()), "manual")
        action = next((b.property("key") for b in self.grp_action.buttons() if b.isChecked()), "quarantine")
        self.settings.update({
            "level": self.cmb_level.currentData(),
            "workers": self.spn_workers.value(),
            "keep_missing": self.chk_keep.isChecked(),
            "incremental": self.chk_incr.isChecked(),
            "verify": self.chk_verify.isChecked(),
            "backup": self.chk_backup.isChecked(),
            "auto_config": self.chk_autocfg.isChecked(),
            "broken_copies": self.chk_copies.isChecked(),
            "broken_names": mode,
            "broken_action": action,
            "compile_pyc": [x.strip() for x in self.txt_pyc.text().split(",") if x.strip()],
            "auto_property_xml": self.chk_propauto.isChecked(),
            "property_include_pack": self.chk_proppack.isChecked(),
            "property_xml_mode": next((b.property("key") for b in self.grp_propmode.buttons() if b.isChecked()), "merge"),
        })
        config.save_settings(self.settings)
        self.btn_clean.setEnabled(mode != "off" and not self._is_busy())

    def _refresh_paths(self):
        src, pk, cd = config.paths(self.settings)
        self.lbl_sub.setText("FoxFS paketleyici  •  %s  →  %s" % (src, pk))
        self.lbl_resolved.setText("Kaynak: %s%s\nPack: %s%s\nConfig: %s%s" % (
            src, "" if os.path.isdir(src) else "  (yok!)", pk, "" if os.path.isdir(pk) else "  (yok!)",
            cd, "" if os.path.isdir(cd) else "  (oluşturulacak)"))
        self.sb_paths.setText("config: %s" % cd)

    def _browse(self, edit, select_test=False):
        cur = edit.text().strip() or config.PROJECT_DIR
        if not os.path.isabs(cur):
            cur = os.path.join(config.PROJECT_DIR, cur)
        d = QFileDialog.getExistingDirectory(self, "Klasör seç", cur)
        if not d:
            return
        edit.setText(d if select_test else config.portable(os.path.normpath(d)))
        if select_test:
            self.rad_test.setChecked(True)

    def _reset_dirs(self):
        for e in (self.txt_src, self.txt_pack, self.txt_cfg):
            e.setText("")
        self._apply_dirs()

    def _apply_dirs(self):
        self.settings.update({"source_root": self.txt_src.text().strip(), "pack_dir": self.txt_pack.text().strip(),
                              "config_dir": self.txt_cfg.text().strip()})
        config.save_settings(self.settings)
        src, pk, _ = config.paths(self.settings)
        if not os.path.isdir(src):
            QMessageBox.warning(self, "Dizinler", "Kaynak kök bulunamadı:\n%s" % src)
        self._refresh_paths()
        self.scan()

    @staticmethod
    def _open_dir(d):
        os.makedirs(d, exist_ok=True)
        os.startfile(d)

    # ================================================================ tablo
    def _rebuild_table(self, plans):
        self.table.blockSignals(True)
        self.table.setSortingEnabled(False)          # doldururken satırlar yer değiştirmesin
        sel = set(self._row_names())
        self.table.setRowCount(len(plans))
        for row, p in enumerate(plans):
            s = p.spec
            chk = SortItem()
            chk.setFlags(Qt.ItemFlag.ItemIsUserCheckable | Qt.ItemFlag.ItemIsEnabled | Qt.ItemFlag.ItemIsSelectable)
            chk.setCheckState(Qt.CheckState.Checked if s.name in self.checked else Qt.CheckState.Unchecked)
            self.table.setItem(row, COL_CHECK, chk)
            name = SortItem(s.name)
            name.setFont(QFont("Segoe UI", 10, QFont.Weight.DemiBold))
            tip = "kaynak: %s\nconfig: %s\npack: %s" % (s.source, s.config_path, s.output)
            if s.config_created:
                tip += "\n\nXML bu analizde otomatik oluşturuldu"
            name.setToolTip(tip)
            self.table.setItem(row, COL_NAME, name)
            for c in range(COL_STATE, len(COLS)):
                it = SortItem("")
                if c >= COL_FILES:
                    it.setTextAlignment(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)
                self.table.setItem(row, c, it)
            self._fill_row(row, p)
        self.table.setSortingEnabled(True)
        self.table.blockSignals(False)
        self._apply_filter()
        self._update_sel_label()
        for r in range(self.table.rowCount()):          # seçimi analizler arasında koru
            if self.table.item(r, COL_NAME).text() in sel:
                self.table.selectRow(r)
                break

    def _fill_row(self, r, p):
        t, s = self.table, p.spec
        items = {c: t.item(r, c) for c in range(len(COLS))}   # sıralama açıkken satır kayabilir: önce al

        def cell(c, text, color=None, tip=None, key=None):
            it = items[c]
            it.setText(text)
            it.setForeground(QColor(color or C["text"]))
            it.setData(SORT_ROLE, key)
            if tip:
                it.setToolTip(tip)

        new = p.added
        nfiles = len(p.files) + len(p.pyc_needed)
        pref = s.prefix or "(kök)"
        cell(COL_PREFIX, pref + ("  ⚠" if s.prefix_guessed else ""), C["amber"] if s.prefix_guessed else C["muted"],
             "prefix tahmini (%s) — Pack ayarları'ndan doğrula" % s.prefix_score if s.prefix_guessed else s.prefix_score or None)
        cell(COL_FILES, fmt_int(nfiles), key=nfiles)
        cell(COL_NEW, fmt_int(new) if new else "–", C["amber"] if new else C["dim"], key=new)
        cell(COL_KEEP, fmt_int(len(p.carry)) if p.carry else "–", C["violet"] if p.carry else C["dim"], key=len(p.carry))
        cell(COL_BAD, fmt_int(len(p.unencodable)) if p.unencodable else "–", C["red"] if p.unencodable else C["dim"],
             key=len(p.unencodable))
        cell(COL_SRC, fmt_size(p.source_bytes), C["muted"], key=p.source_bytes)
        if p.existing is None and not os.path.isfile(s.output):
            state = "Yeni pack"
        elif p.existing_error:
            state = "Bozuk"
        else:
            state = "Eksik var" if new else "Güncel"
        it = items[COL_STATE]
        it.setText(state)
        it.setData(Qt.ItemDataRole.UserRole, STATE_COLORS.get(state, C["slate"]))
        it.setData(SORT_ROLE, list(STATE_COLORS).index(state))
        self._fill_pack_file(r, s, items)

    def _fill_pack_file(self, r, s, items=None):
        items = items or {c: self.table.item(r, c) for c in (COL_PACK, COL_TIME)}
        pk, tm = items[COL_PACK], items[COL_TIME]
        if os.path.isfile(s.output):
            st = os.stat(s.output)
            pk.setText(fmt_size(st.st_size))
            pk.setData(SORT_ROLE, st.st_size)
            tm.setText(time.strftime("%d.%m %H:%M", time.localtime(st.st_mtime)))
            tm.setData(SORT_ROLE, st.st_mtime)
        else:
            for it in (pk, tm):
                it.setText("—")
                it.setData(SORT_ROLE, -1)
        for it in (pk, tm):
            it.setForeground(QColor(C["muted"]))

    def _row(self, name):
        for r in range(self.table.rowCount()):
            it = self.table.item(r, COL_NAME)
            if it and it.text() == name:
                return r
        return -1

    def _set_state_row(self, r, state):
        it = self.table.item(r, COL_STATE)
        it.setText(state)
        it.setData(Qt.ItemDataRole.UserRole, STATE_COLORS.get(state, C["slate"]))
        it.setData(SORT_ROLE, list(STATE_COLORS).index(state) if state in STATE_COLORS else 99)

    def _set_state(self, name, state):
        r = self._row(name)
        if r >= 0:
            self._set_state_row(r, state)

    def _state_match(self, name, state, key):
        p = self.plans.get(name)
        if not key or not p:
            return True
        return {"missing": lambda: p.added > 0, "carry": lambda: bool(p.carry), "bad": lambda: bool(p.unencodable),
                "guess": lambda: p.spec.prefix_guessed, "error": lambda: state in ("Hata", "Bozuk")}.get(
            key, lambda: state == key)()

    def _apply_filter(self):
        q = self.txt_filter.text().strip().lower()
        key = self.cmb_state.currentData()
        shown = 0
        for r in range(self.table.rowCount()):
            n = self.table.item(r, COL_NAME).text()
            hide = (bool(q) and q not in n.lower()) or not self._state_match(n, self.table.item(r, COL_STATE).text(), key)
            self.table.setRowHidden(r, hide)
            shown += not hide
        self._shown = shown
        self._update_sel_label()

    def _has_missing(self, name):
        p = self.plans.get(name)
        return bool(p and p.added)

    def _check_where(self, pred):
        self.table.blockSignals(True)
        for r in range(self.table.rowCount()):
            if self.table.isRowHidden(r):
                continue
            n = self.table.item(r, COL_NAME).text()
            on = pred(n)
            self.table.item(r, COL_CHECK).setCheckState(Qt.CheckState.Checked if on else Qt.CheckState.Unchecked)
            (self.checked.add if on else self.checked.discard)(n)
        self.table.blockSignals(False)
        self._update_sel_label()

    def _on_item_changed(self, it):
        if it.column() == COL_CHECK:
            n = self.table.item(it.row(), COL_NAME).text()
            (self.checked.add if it.checkState() == Qt.CheckState.Checked else self.checked.discard)(n)
            self._update_sel_label()

    def _update_sel_label(self):
        shown = getattr(self, "_shown", len(self.plans))
        self.lbl_sel.setText("%d / %d işaretli%s" % (len(self.checked & set(self.plans)), len(self.plans),
                                                     "  •  %d gösteriliyor" % shown if shown != len(self.plans) else ""))

    def _apply_plans(self, plans):
        self.plans = {p.spec.name: p for p in plans}
        self.specs = [p.spec for p in plans]
        self.checked &= set(self.plans)
        self._rebuild_table(plans)
        tf = sum(len(p.files) for p in plans)
        tn = sum(p.added for p in plans)
        tk = sum(len(p.carry) for p in plans)
        tb = sum(len(p.unencodable) for p in plans)
        ts = sum(os.path.getsize(p.spec.output) for p in plans if os.path.isfile(p.spec.output))
        self.st_packs.set(str(len(plans)))
        self.st_files.set(fmt_int(tf))
        self.st_new.set(fmt_int(tn), C["amber"] if tn else None)
        self.st_keep.set(fmt_int(tk))
        self.st_bad.set(fmt_int(tb), C["red"] if tb else None)
        self.st_size.set(fmt_size(ts))
        self._on_select()

    # ================================================================ detay + editör
    def _selected_name(self):
        rows = sorted({i.row() for i in self.table.selectedItems()})
        return self.table.item(rows[0], COL_NAME).text() if rows else None

    def _on_select(self):
        name = self._selected_name()
        self._show_detail(name)
        if self.txt_detfind.text():
            self._filter_detail(self.txt_detfind.text())
        self._fill_editor(name)

    def _show_detail(self, name):
        self.detail.clear()
        if not name:
            return
        p = self.plans.get(name)
        if not p:
            return
        s = p.spec

        def add(text, color=C["text"]):
            it = QListWidgetItem(text)
            it.setForeground(QColor(color))
            self.detail.addItem(it)

        add("■ %s" % name, C["accent_hi"])
        add("  kaynak : %s" % s.source, C["muted"])
        add("  pack   : %s" % s.output, C["muted"])
        add("  config : %s%s" % (s.config_path, "   (otomatik oluşturuldu)" if s.config_created else ""), C["muted"])
        add("  prefix : '%s'   %s" % (s.prefix, s.prefix_score), C["amber"] if s.prefix_guessed else C["muted"])
        if s.ignores:
            add("  ignore : " + ", ".join(r.pattern for r in s.ignores), C["muted"])
        r = self.results.get(name)
        if r:
            add("  son paket: %s — %s entry (yeni %d, aynı %d, korunan %d, pyc %d) %s → %s, %.1f sn %s" % (
                r.status, fmt_int(r.entries), r.encoded, r.reused, r.carried, r.pyc, fmt_size(r.old_pack_bytes),
                fmt_size(r.pack_bytes), r.seconds, r.message), C["blue"] if r.status == "ok" else C["red"])
            for w in r.warnings:
                add("  uyarı: " + w, C["amber"])
        if p.existing_error:
            add("  mevcut pack okunamadı: " + p.existing_error, C["red"])
        new = [f for f in p.files if p.existing is None or f.name not in p.existing]
        add("  Pakete yeni girecek: %s" % fmt_int(len(new)), C["amber"] if new else C["sky"])
        for f in new[:300]:
            add("     + " + f.rel)
        if len(new) > 300:
            add("     … +%d" % (len(new) - 300), C["dim"])
        if p.pyc_needed:
            add("  Derlenecek .pyc: %d" % len(p.pyc_needed), C["accent_hi"])
        if p.carry:
            add("  Korunan (kaynakta yok, pack'ten aynen taşınır): %s" % fmt_int(len(p.carry)), C["violet"])
        if p.unencodable:
            add("  Bozuk adlı (paketlenemez — Temizlik sekmesi): %d" % len(p.unencodable), C["red"])
            for x in p.unencodable[:200]:
                add("     ! " + x, C["red"])
        if p.given_away:
            add("  Başka pack'te bulunduğu için eklenmeyen: %s" % fmt_int(len(p.given_away)), C["muted"])
            for rel, owner in p.given_away[:100]:
                add("     → %s  (%s)" % (rel, owner), C["dim"])
        if p.collisions:
            add("  Aynı ada düşen dosyalar: %d" % len(p.collisions), C["amber"])
            for a, b in p.collisions[:50]:
                add("     = %s  ⇄  %s" % (a, b), C["amber"])

    def _set_editor_enabled(self, on):
        for wdg in (self.ed_prefix, self.ed_ignores, self.btn_save_cfg, self.btn_detect):
            wdg.setEnabled(on)

    def _fill_editor(self, name):
        p = self.plans.get(name) if name else None
        if not p:
            for e in (self.ed_name, self.ed_source, self.ed_config, self.ed_prefix):
                e.setText("")
            self.ed_ignores.setPlainText("")
            self.lbl_prefix_info.setText("Düzenlemek için tablodan bir pack seç.")
            self._set_editor_enabled(False)
            return
        s = p.spec
        self.ed_name.setText(s.name)
        self.ed_source.setText(s.source)
        self.ed_config.setText(s.config_path or "(henüz yok — kaydedince oluşturulur)")
        self.ed_prefix.setText(s.prefix)
        self.ed_ignores.setPlainText("\n".join(r.pattern for r in s.ignores))
        self.lbl_prefix_info.setText(("⚠ tahmini prefix — " if s.prefix_guessed else "") + (s.prefix_score or ""))
        self._set_editor_enabled(not self._is_busy())

    def _detect_prefix(self):
        name = self.ed_name.text()
        p = self.plans.get(name)
        if not p:
            return
        QApplication.setOverrideCursor(Qt.CursorShape.WaitCursor)
        try:
            prefix, ok, why = config.guess_prefix(name, p.spec.source, p.spec.output)
        finally:
            QApplication.restoreOverrideCursor()
        self.ed_prefix.setText(prefix)
        self.lbl_prefix_info.setText(("✓ " if ok else "⚠ tahmini — ") + why)

    def _save_pack_config(self):
        name = self.ed_name.text()
        p = self.plans.get(name)
        if not p:
            return
        s = p.spec
        s.prefix = self.ed_prefix.text().strip().replace("\\", "/")
        pats = [x.strip() for x in self.ed_ignores.toPlainText().splitlines() if x.strip()]
        s.ignores = [config.IgnoreRule(x) for x in pats]
        _, _, cd = config.paths(self.settings)
        path = config.write_config(s, cd, pats)
        self._log("ok", "%s: config kaydedildi -> %s" % (name, path))
        self.scan()

    # ================================================================ görevler
    def _checked_names(self, selection_fallback=True):
        """Kutucuklu pack'ler; yoksa (paketleme için) seçili satırlar."""
        names = [n for n in self.plans if n in self.checked]
        if not names and selection_fallback:
            rows = {i.row() for i in self.table.selectedItems()}
            names = [self.table.item(r, COL_NAME).text() for r in sorted(rows)]
        return names

    def _is_busy(self):
        return bool(self.thread and self.thread.isRunning())

    def _start(self, task, names=None, out_dir=None):
        self._save_settings()
        self.worker = Worker(task, self.settings, names, out_dir)
        self.thread = QThread()
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.run)
        self.worker.log.connect(self._log)
        self.worker.progress.connect(self._progress)
        self.worker.overall.connect(self._overall)
        self.worker.planned.connect(self._apply_plans)
        self.worker.packDone.connect(self._pack_done)
        self.worker.packState.connect(self._set_state)
        self.worker.broken.connect(self._show_broken)
        self.worker.propReport.connect(self._show_prop_report)
        self.worker.finished.connect(self._finished)
        self.worker.finished.connect(self.thread.quit)
        self._busy(True)
        self.pb_cur.setValue(0)
        self.pb_all.setValue(0)
        self._run_results = []
        self._t_start = time.time()
        self.lbl_elapsed.setText("Geçen süre  00:00")
        self._elapsed_timer.start()
        self.thread.start()

    def _busy(self, b):
        for wdg in (self.btn_scan, self.btn_build_sel, self.btn_build_all, self.btn_verify, self.btn_restore,
                    self.btn_clean, self.btn_clean2, self.btn_propgen):
            wdg.setEnabled(not b)
        if not b:
            self.btn_clean.setEnabled(self.settings.get("broken_names") != "off")
        self.btn_stop.setEnabled(b)
        self._set_editor_enabled(not b and bool(self.ed_name.text()))

    def scan(self):
        if self._is_busy():
            return
        self.lbl_cur.setText("Kaynaklar taranıyor…")
        self._start("scan")

    def build(self, selected, names=None, test=None):
        if self._is_busy():
            return
        names = names or (self._checked_names() if selected else list(self.plans))
        if not names:
            QMessageBox.information(self, "Pack Studio", "Önce tablodan en az bir pack seç (kutucuk ya da satır).")
            return
        if test is None:
            test = self.rad_test.isChecked()
        out_dir = (self.txt_out.text().strip() or os.path.join(config.DATA_DIR, "test_out")) if test else None
        if not out_dir:
            if client_running():
                QMessageBox.warning(self, "Client açık", "Metin2Release_x64.exe çalışıyor; pack dosyaları kilitli.\n"
                                    "Client'ı kapatıp tekrar dene ya da hedefi 'Test klasörü' yap.")
                return
            _, pk, _ = config.paths(self.settings)
            msg = "%d pack şu klasöre yazılacak:\n%s\n\n%s" % (len(names), pk, ", ".join(names))
            guessed = [n for n in names if self.plans[n].spec.prefix_guessed]
            if guessed:
                msg += "\n\n⚠ Prefix'i tahmini olan pack'ler: %s\n(Pack ayarları sekmesinden kontrol et.)" % ", ".join(guessed)
            if self.settings.get("broken_names") == "auto":
                msg += "\n\nBozuk adlı dosyalar paketlemeden önce %s." % (
                    "kalıcı silinecek" if self.settings.get("broken_action") == "delete" else "karantinaya taşınacak")
            if self.settings.get("backup"):
                msg += "\n\nEski pack'ler pack_studio\\_data\\backups altına yedeklenir."
            if QMessageBox.question(self, "Paketle", msg) != QMessageBox.StandardButton.Yes:
                return
        for n in names:
            self._set_state(n, "Bekliyor")
        self._start("build", names, out_dir)

    def verify(self):
        if not self._is_busy():
            self._start("verify", self._checked_names(False) or list(self.plans))

    def broken_scan(self):
        self._start("broken_scan", self._checked_names(False) or None)

    def clean_now(self):
        if self.settings.get("broken_names") == "off":
            QMessageBox.information(self, "Temizlik", "Temizlik modu kapalı (Temizlik sekmesi).")
            return
        names = self._checked_names(False) or None
        found = cleanup.find([s for s in self.specs if not names or s.name in names],
                             copies=self.settings.get("broken_copies", False))
        if not found:
            QMessageBox.information(self, "Temizlik", "Bozuk adlı dosya bulunamadı.")
            return
        self._show_broken(found)
        total = sum(len(v) for v in found.values())
        act = "KALICI OLARAK SİLİNECEK" if self.settings.get("broken_action") == "delete" else "karantinaya taşınacak"
        lines = "\n".join("%-16s %d" % (k, len(v)) for k, v in found.items())
        if QMessageBox.question(self, "Temizlik", "%d dosya %s:\n\n%s\n\nListe 'Pack detayı' sekmesinde." % (total, act, lines)) \
                != QMessageBox.StandardButton.Yes:
            return
        self._start("clean", names)

    def restore_sources(self):
        rep = restore.missing_report(self._checked_names(False) or None)
        lines = ["%-18s %6d" % (os.path.basename(p), len(m)) for p, (n, m) in rep.items() if m]
        if not lines:
            QMessageBox.information(self, "Kaynak Onar", "Eski listelere göre kaynakta eksik dosya yok.")
            return
        total = sum(len(m) for _, (n, m) in rep.items())
        if QMessageBox.question(
                self, "Kaynak Onar",
                "Eski liste XML'lerine göre kaynakta olmayan %d dosya pack'lerden adlarıyla geri yazılacak "
                "(var olan dosyalara dokunulmaz):\n\n%s" % (total, "\n".join(lines))) != QMessageBox.StandardButton.Yes:
            return
        self._start("restore", self._checked_names(False) or None)

    def _regen_xml(self):
        names = self._checked_names(False)
        target = ", ".join(names) if names else "TÜM pack'ler"
        if QMessageBox.question(self, "XML", "Config XML'leri yeniden oluşturulacak (%s).\n"
                                "Prefix mevcut pack'ten yeniden ölçülür, ignore desenleri korunur." % target) \
                != QMessageBox.StandardButton.Yes:
            return
        self._start("bulkxml", names or None, "regen")

    def _new_pack_dialog(self):
        dlg = NewPackDialog(self, self.settings)
        if dlg.exec():
            self._log("new", "yeni pack: %s -> %s" % (dlg.spec.name, dlg.spec.config_path))
            self.checked.add(dlg.spec.name)
            self.scan()

    def _export_legacy(self):
        d = QFileDialog.getExistingDirectory(self, "Liste XML'lerinin yazılacağı klasör",
                                             os.path.join(config.DATA_DIR, "legacy_xml"))
        if d:
            self._start("export", self._checked_names(False) or None, d)

    def stop(self):
        if self.worker:
            self.worker.cancel.set()
            self._log("warn", "Durduruluyor… (yarım kalan pack yazılmaz, mevcut pack korunur)")

    # ================================================================ sinyaller
    LOG_COLORS = {"info": "#cbd5e1", "ok": C["sky"], "warn": C["amber"], "err": C["red"], "new": C["violet"]}
    LOG_TAGS = {"ok": "✓", "warn": "!", "err": "✕", "new": "✦"}

    def _log_visible(self, level, line):
        levels = self.cmb_loglevel.currentData()
        q = self.txt_logfind.text().strip().lower()
        return (not levels or level in levels) and (not q or q in line.lower())

    def _log_html(self, ts, level, line):
        return '<span style="color:%s">%s</span>&nbsp;&nbsp;<span style="color:%s">%s %s</span>' % (
            C["dim"], ts, self.LOG_COLORS.get(level, "#cbd5e1"), self.LOG_TAGS.get(level, "·"), html.escape(line))

    def _log(self, level, text):
        ts = time.strftime("%H:%M:%S")
        lines = text.rstrip().splitlines()
        for line in lines:
            self._log_entries.append((ts, level, line))
            if self._log_visible(level, line):
                self.log_view.appendHtml(self._log_html(ts, level, line))
        try:                                         # kalıcı günlük: _data/logs/<gün>.log
            if self._logf is None or self._logf_day != time.strftime("%Y-%m-%d"):
                os.makedirs(LOG_DIR, exist_ok=True)
                if self._logf:
                    self._logf.close()
                self._logf_day = time.strftime("%Y-%m-%d")
                self._logf = open(os.path.join(LOG_DIR, self._logf_day + ".log"), "a", encoding="utf-8", buffering=1)
            for line in lines:
                self._logf.write("%s %s %s\n" % (ts, self.LOG_TAGS.get(level, "·"), line))
        except OSError:
            pass

    def _rerender_log(self, *_):
        self.log_view.clear()
        rows = [self._log_html(ts, lv, ln) for ts, lv, ln in self._log_entries if self._log_visible(lv, ln)]
        for i in range(max(0, len(rows) - 8000), len(rows), 400):
            self.log_view.appendHtml("<br>".join(rows[i:i + 400]))
        self.log_view.verticalScrollBar().setValue(self.log_view.verticalScrollBar().maximum())

    def _visible_log_text(self):
        return "\n".join("%s %s %s" % (ts, self.LOG_TAGS.get(lv, "·"), ln)
                         for ts, lv, ln in self._log_entries if self._log_visible(lv, ln))

    def _copy_log(self):
        QApplication.clipboard().setText(self._visible_log_text())
        self.statusBar().showMessage("Günlük panoya kopyalandı", 2500)

    def _save_log(self):
        path, _ = QFileDialog.getSaveFileName(self, "Günlüğü kaydet", os.path.join(
            LOG_DIR, "pack_studio_%s.txt" % time.strftime("%Y%m%d_%H%M%S")), "Metin (*.txt *.log)")
        if path:
            with open(path, "w", encoding="utf-8") as f:
                f.write(self._visible_log_text() + "\n")
            self.statusBar().showMessage("Kaydedildi: %s" % path, 4000)

    def _clear_log(self):
        self._log_entries.clear()
        self.log_view.clear()

    def _filter_detail(self, text):
        q = text.strip().lower()
        for i in range(self.detail.count()):
            it = self.detail.item(i)
            it.setHidden(bool(q) and q not in it.text().lower() and not it.text().startswith("■"))

    def _show_broken(self, found):
        self.broken_found = found
        total = sum(len(v) for v in found.values())
        self.lbl_broken.setText("Bulunan: %d dosya  (%s)" % (total, ", ".join("%s %d" % (k, len(v)) for k, v in found.items()))
                                if found else "Bozuk adlı dosya yok.")
        self.detail.clear()
        head = QListWidgetItem("■ Bozuk / kopya adlı dosyalar: %d" % total)
        head.setForeground(QColor(C["red"] if total else C["sky"]))
        self.detail.addItem(head)
        for name, items in found.items():
            it = QListWidgetItem("  %s (%d)" % (name, len(items)))
            it.setForeground(QColor(C["accent_hi"]))
            self.detail.addItem(it)
            for rel, why in items[:500]:
                it = QListWidgetItem("     %-9s %s" % (why, rel))
                it.setForeground(QColor(C["red"] if why == "bozuk ad" else C["amber"]))
                self.detail.addItem(it)
        self.tabs.setCurrentIndex(1)

    def _show_prop_report(self, rep):
        st, miss = rep["stats"], rep["missing"]
        self.lbl_prop.setText("%d property  •  '%s' pack'i %s\nHaritalar: %s obje, %s farklı property  •  property.xml'de olmayan: %d CRC" % (
            st["properties"], rep["target"], "güncel değil → paketle" if rep["changed"] else "güncel",
            fmt_int(rep["objects"]), fmt_int(rep["used"]), len(miss)))
        self.detail.clear()

        def add(t, c=C["text"]):
            it = QListWidgetItem(t)
            it.setForeground(QColor(c))
            self.detail.addItem(it)
        add("■ property.xml — %d property (%s)" % (st["properties"], ", ".join("%s %d" % kv for kv in sorted(st["types"].items()))),
            C["accent_hi"])
        add("  kaynak dosya %d, eski girdi korunan %d, pack'ten eklenen %d, aynı CRC'li kopya %d" % (
            st["files"], st["kept"], st["from_pack"], st["duplicate_crc"]), C["muted"])
        add("  Haritaların kullanıp property.xml'de bulamadığı CRC: %d (property dosyası hiçbir kaynakta yok)" % len(miss),
            C["amber"] if miss else C["sky"])
        for crc, maps in sorted(miss.items(), key=lambda kv: sorted(kv[1])):
            add("     %10d   %s" % (crc, ", ".join(sorted(maps))), C["amber"])
        self.tabs.setCurrentIndex(1)

    def _progress(self, name, stage, done, total, rate):
        self.pb_cur.setValue(int(1000 * done / max(total, 1)))
        eta = (total - done) / rate if rate > 0 else 0
        self.lbl_cur.setText("%s  —  %s" % (name, stage))
        self.lbl_rate.setText("%s / %s   •   %.1f MB/s   •   kalan ~%ds" % (
            fmt_size(done), fmt_size(total), rate / 2**20, eta))

    def _overall(self, i, n):
        self.pb_all.setValue(int(1000 * i / max(n, 1)))
        self.lbl_all.setText("Toplam: %d / %d" % (i, n))

    def _pack_done(self, r):
        self.results[r.name] = r
        self._run_results.append(r)
        state = {"ok": "Paketlendi", "skipped": "Atlandı", "failed": "Hata", "cancelled": "İptal"}[r.status]
        self._set_state(r.name, state)
        if r.status == "ok":
            self._log("ok", "%s: %s entry (yeni %d, aynı %d, korunan %d, pyc %d)  %s → %s  %.1f sn" % (
                r.name, fmt_int(r.entries), r.encoded, r.reused, r.carried, r.pyc,
                fmt_size(r.old_pack_bytes), fmt_size(r.pack_bytes), r.seconds))
        else:
            self._log("warn" if r.status == "skipped" else "err", "%s: %s" % (r.name, r.message))
        for w in r.warnings:
            self._log("warn", "%s: %s" % (r.name, w))
        row = self._row(r.name)
        p = self.plans.get(r.name)
        if row >= 0 and p:
            self._fill_pack_file(row, p.spec)

    def _finished(self, status):
        task = self.worker.task if self.worker else ""
        secs = time.time() - self._t_start
        self._elapsed_timer.stop()
        self._busy(False)
        self.lbl_cur.setText({"ok": "Tamamlandı", "cancelled": "İptal edildi", "error": "Hata — günlüğe bak"}[status])
        self.lbl_elapsed.setText("Son iş: %s  •  %s" % (fmt_dur(secs), time.strftime("%H:%M:%S")))
        if task in ("build", "restore", "clean", "bulkxml") and status != "error":
            QTimer.singleShot(300, self._rescan_keep_states)
        if task != "scan" and secs > 8:
            self._notify(task, status, secs)
        if task == "build" and self._run_results:
            dlg = SummaryDialog(self, list(self._run_results), secs)
            dlg.setAttribute(Qt.WidgetAttribute.WA_DeleteOnClose)
            dlg.open()

    def _notify(self, task, status, secs):
        """Uzun iş bitince: görev çubuğunda yanıp sön + pencere arkadaysa bildirim balonu."""
        QApplication.alert(self)
        if self.isActiveWindow() or not QSystemTrayIcon.isSystemTrayAvailable():
            return
        if self._tray is None:
            self._tray = QSystemTrayIcon(app_icon(), self)
            self._tray.messageClicked.connect(lambda: (self.showNormal(), self.activateWindow()))
        self._tray.show()
        labels = {"build": "Paketleme", "verify": "Doğrulama", "restore": "Kaynak onarımı", "clean": "Temizlik",
                  "propxml": "property.xml", "bulkxml": "XML", "export": "Dışa aktarma", "broken_scan": "Tarama"}
        bad = [r for r in self._run_results if r.status == "failed"]
        text = "%s %s (%s)" % (labels.get(task, task), {"ok": "tamamlandı", "cancelled": "iptal edildi",
                                                        "error": "hata verdi"}[status], fmt_dur(secs))
        if bad:
            text += " — %d pack HATA" % len(bad)
        icon = QSystemTrayIcon.MessageIcon.Critical if bad or status == "error" else QSystemTrayIcon.MessageIcon.Information
        self._tray.showMessage("Anka2 Pack Studio", text, icon, 6000)

    def _rescan_keep_states(self):
        """Paketleme sonrası tabloyu tazele; 'Paketlendi/Hata' durumlarını koru."""
        keep = {n: r for n, r in self.results.items()}
        self.scan()

        def restore_states():
            if self._is_busy():
                QTimer.singleShot(300, restore_states)
                return
            for n, r in keep.items():
                if r.status != "ok":
                    self._set_state(n, {"skipped": "Atlandı", "failed": "Hata", "cancelled": "İptal"}[r.status])
        QTimer.singleShot(300, restore_states)

    def closeEvent(self, e):
        if self._is_busy():
            if QMessageBox.question(self, "Çıkış", "İşlem sürüyor. Durdurup çıkılsın mı?") != QMessageBox.StandardButton.Yes:
                e.ignore()
                return
            self.worker.cancel.set()
            self.thread.quit()
            self.thread.wait(15000)
        self._save_ui_state()
        if self._logf:
            self._logf.close()
        if self._tray:
            self._tray.hide()
        e.accept()


def main():
    mp.freeze_support()
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    app.setStyleSheet(QSS)
    app.setWindowIcon(app_icon())
    w = MainWindow()
    w.show()
    return app.exec()


if __name__ == "__main__":
    sys.exit(main())
