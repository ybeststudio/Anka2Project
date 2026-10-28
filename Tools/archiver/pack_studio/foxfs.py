# FoxFS pack formatı (client: Source/FoxFS/FoxFS/Archive.cc ile birebir).
#
#   [header 68B]  magic(u32)=0x000FED0C | key[32] | iv[32]
#   tekrar:       entry 20B = decompressed | xxh32 | offset | size | name   (hepsi u32 LE)
#                 payload   = AES-256-CFB128(key, iv[:16]) ( lz4 block | ham veri )
#
# - name   = CRC32(arşiv yolu; ASCII lowercase, '\' -> '/')  (client tolower + CryptoPP CRC32)
# - xxh32  = XXH32(düz veri, seed=FOXFS_MAGIC)
# - size == decompressed ise payload sıkıştırılmamış kabul edilir; bu yüzden lz4 çıktısı ancak
#   kesin küçükse kullanılır.
# - offset entry'nin hemen arkasıdır. Client offset'leri u32 okuduğu için pack < 4 GiB olmalı.
import mmap
import os
import struct
import zlib

import lz4.block
import xxhash
from Crypto.Cipher import AES

MAGIC = 0x000FED0C
HEADER = struct.Struct("<I32s32s")
ENTRY = struct.Struct("<5I")
MAX_PACK_SIZE = 0xFFFFFFFF - 1

# GUI/CLI seviyeleri -> lz4 parametreleri
LEVELS = {
    "store": None,                                   # sıkıştırma yok (en hızlı, en büyük)
    "fast": {"mode": "fast", "acceleration": 4},
    "default": {"mode": "default"},
    "max": {"mode": "high_compression", "compression": 9},
}


def name_hash(archived: bytes) -> int:
    return zlib.crc32(archived.replace(b"\\", b"/").lower()) & 0xFFFFFFFF


def content_hash(data) -> int:
    return xxhash.xxh32_intdigest(data, seed=MAGIC)


def _cipher(key, iv):
    return AES.new(key, AES.MODE_CFB, iv=iv[:16], segment_size=128)


def encode(data: bytes, key: bytes, iv: bytes, level: str):
    """düz veri -> (decompressed, xxh32, payload)"""
    digest = content_hash(data)
    body = data
    params = LEVELS[level]
    if params is not None and data:
        comp = lz4.block.compress(data, store_size=False, **params)
        if len(comp) < len(data):
            body = comp
    return len(data), digest, _cipher(key, iv).encrypt(body)


def decode(payload: bytes, decompressed: int, key: bytes, iv: bytes) -> bytes:
    body = _cipher(key, iv).decrypt(payload)
    if len(payload) == decompressed:
        return body
    return lz4.block.decompress(body, uncompressed_size=decompressed)


class Entry:
    __slots__ = ("name", "decompressed", "digest", "offset", "size")

    def __init__(self, name, decompressed, digest, offset, size):
        self.name, self.decompressed, self.digest, self.offset, self.size = name, decompressed, digest, offset, size


class PackReader:
    """Mevcut bir pack'in index'ini okur; payload'ları kopyalama/çözme için açık tutar."""

    def __init__(self, path):
        self.path = path
        self.f = open(path, "rb")
        magic, self.key, self.iv = HEADER.unpack(self.f.read(HEADER.size))
        if magic != MAGIC:
            self.f.close()
            raise ValueError("FoxFS magic uyuşmuyor: %s" % path)
        self.entries = {}
        self.duplicates = 0
        total = os.fstat(self.f.fileno()).st_size
        try:
            self._read_index(total)
        except Exception:
            self.f.close()
            raise
        self.file_size = total

    def _read_index(self, total):
        # Entry'ler payload'larla iç içe: entry başına seek+read yerine mmap üzerinden unpack_from
        # (index okuması ~20x hızlı). mmap AYRI bir tanıtıcıdan açılır ve hemen kapatılır: Windows'ta
        # self.f'in fileno'su map'lenirse BufferedReader'ın konum takibi bozulur, sonraki payload okumaları
        # yanlış bayt döndürür.
        path, entries, unpack, esz = self.path, self.entries, ENTRY.unpack_from, ENTRY.size
        pos = HEADER.size
        if total <= pos:
            return
        with open(path, "rb") as mf, mmap.mmap(mf.fileno(), 0, access=mmap.ACCESS_READ) as mm:
            while pos < total:
                if pos + esz > total:
                    raise ValueError("pack kesik: %s @%d" % (path, pos))
                dec, digest, off, size, name = unpack(mm, pos)
                if off != pos + esz or off + size > total:
                    raise ValueError("bozuk entry: %s @%d" % (path, pos))
                if name in entries:
                    self.duplicates += 1
                entries[name] = Entry(name, dec, digest, off, size)
                pos = off + size

    def payload(self, e: Entry) -> bytes:
        self.f.seek(e.offset)
        return self.f.read(e.size)

    def read(self, e: Entry) -> bytes:
        return decode(self.payload(e), e.decompressed, self.key, self.iv)

    def close(self):
        self.f.close()

    def __enter__(self):
        return self

    def __exit__(self, *a):
        self.close()


class PackWriter:
    def __init__(self, path, key, iv):
        self.path = path
        self.f = open(path, "wb", buffering=8 * 1024 * 1024)
        self.f.write(HEADER.pack(MAGIC, key, iv))
        self.pos = HEADER.size
        self.count = 0
        self.names = set()

    def add(self, name, decompressed, digest, payload):
        if name in self.names:
            raise ValueError("aynı isim hash'i iki kez yazılıyor: %08x" % name)
        offset = self.pos + ENTRY.size
        end = offset + len(payload)
        if end > MAX_PACK_SIZE:
            raise OverflowError("pack 4 GiB sınırını aşıyor (client u32 offset kullanıyor)")
        self.f.write(ENTRY.pack(decompressed, digest, offset, len(payload), name))
        self.f.write(payload)
        self.pos = end
        self.count += 1
        self.names.add(name)

    def close(self):
        self.f.close()
