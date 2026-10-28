# Anka2 Pack Studio

`Tools/archiver/tools/Archiver.exe` yerine geçen FoxFS paketleyici (arayüz + CLI).

## Neden
Eski `Archiver.exe` (kaynak kodu projede yok) eksik pack üretiyordu:

| Sorun | Etki (2026-10-17 ölçümü) |
|---|---|
| Liste üretiminde (`make_xml`) bazı klasörleri tamamen atlıyor | etc: 6.983 dosya listeye girmiyor; 1.214'ü hiçbir pack'te yok |
| ASCII olmayan (Korece, cp949) dosya adlarını paketlemiyor | property/monster/yw_graphic: 1.088 dosya |
| Liste kaynaktan yeniden üretilince kaynakta olmayan entry'ler pack'ten düşüyor | zone 5.833, root ~550, maps 1.629 |
| `.pyc` üretmiyor | root modülleri yalnız `.py` |

## Çalıştırma
```bat
Pack Studio.bat            :: arayüz
pack.bat plan              :: tüm pack'lerin raporu (yazmaz)
pack.bat build root etc    :: seçili pack'leri Client\pack'e yaz
pack.bat build --all
pack.bat build --all --out D:\test   :: Client\pack'e dokunmadan test
pack.bat verify            :: mevcut pack'leri tamamen çözüp hash doğrula
pack.bat restore --dry     :: kaynakta silinmiş dosyaları raporla (--dry olmadan geri yazar)
```
Gereksinim: Python 3.11+. `Pack Studio.bat` Python sürümünü kontrol eder, eksik paketleri (`requirements.txt`) ilk açılışta kurar.

## Arayüz
- **Tablo:** sütun başlığına tıklayarak sıralama (boyut/adet/zaman sayısal), metin + durum filtresi (Eksik var, Güncel, Yeni, Korunan entry'li, Bozuk adlı, Prefix tahmini, Hata). Sağ tık: paketle, test klasörüne paketle, doğrula, Pack Gezgini, kaynak klasörü / pack dosyası / config XML aç, işaretle.
- **Pack Gezgini** (çift tık / Ctrl+E): pack içeriği ad hafızası + güncel kaynakla adlandırılır; arama, durum filtresi (kaynakta / korunan / adı bilinmeyen), sıralama; önizleme (resim, metin, hex, DDS boyutu); seçili/tümünü çıkarma, dışarıda açma, CSV listesi. 100k entry'de filtre/sıralama onlarca ms.
- **Paketleme özeti:** her paketlemeden sonra pack başına sonuç, entry sayıları, boyut farkı, süre. Uzun işler bitince görev çubuğu uyarısı + (pencere arkadaysa) bildirim.
- **Günlük:** seviye filtresi, arama, kopyala/kaydet; her satır `_data/logs/<gün>.log`'a da yazılır.
- **Durum çubuğu:** client açık mı (pack'ler kilitli), pack diskindeki boş alan. Pencere/splitter/sütun düzeni `_data/ui.ini`'de saklanır.
- **Kısayollar:** F5 analiz • Ctrl+B seçilenleri paketle • Ctrl+Shift+B tümü • Ctrl+Shift+V doğrula • Ctrl+E gezgin • Ctrl+F ara • Ctrl+L günlüğü temizle • Esc durdur.

## Davranış
- **Otomatik keşif:** kaynak kökteki (varsayılan `Tools/binary_unpack`) her klasör bir pack'tir; panelde yalnız bunlar görünür. Klasörün `<ad>_create.xml`'i yoksa otomatik oluşturulur. Prefix aynı adlı mevcut pack'in index'inden ölçülür (mevcut 23 XML'de 23/23 doğru); pack yoksa içerikten tahmin edilir ve ⚠ ile işaretlenir — **Pack ayarları** sekmesinden düzeltilir.
- **Dizinler:** kaynak kök, pack klasörü ve config klasörü **Dizinler** sekmesinden seçilir (`settings.json`; proje içi yollar göreli saklanır). CLI: `--source`, `--packs-dir`, `--config`.
- **Temizlik:** adında `U+FFFD` olan (unpack sırasında cp949 adı kaybolmuş, paketlenemez) dosyalar; isteğe bağlı Windows kopyaları (`- Kopya`, `- Copy`, `복사본`). Mod: Kapalı / Manuel / Otomatik (paketlemeden önce); eylem: karantina (`_data/quarantine`) ya da kalıcı silme. Gerçek Korece asset adlarına dokunulmaz. CLI: `pack.bat clean [--dry] [--copies] [--delete]`.
- **property.xml:** client haritadaki bina/ağaç/efekt objelerini root pack'teki `property.xml`'den CRC ile bulur. Üreteç (`propgen.py`) property klasöründeki tüm YPRT dosyalarını client'ın ayrıştırma kuralıyla okur; *birleştir* modunda kaynakta olmayan eski girdileri korur (kaynaktan silinen property haritadan kaybolmaz), *sıfırdan* modunda yalnız kaynak. Her paketlemeden önce otomatik güncellenir; root pack'teki kopya eskiyse root listeye otomatik eklenir. **Harita kontrolü** `areadata.txt`'lerin kullandığı ama listede olmayan CRC'leri raporlar. CLI: `pack.bat propxml [--fresh] [--check]`.
- **XML sekmesi:** toplu (eksik config'leri oluştur / prefix'i yeniden ölçerek yenile), özel (**Yeni pack…** sihirbazı: klasör + XML, hazır prefix şablonları), eski Archiver liste XML'i dışa aktarma. CLI: `xml [--regen]`, `newpack <ad> [--prefix] [--ignore]`, `export [--to]`.
- **Ad hafızası:** her paketlemede pack'teki dosya adları `_data/manifests`'e birikimli kaydedilir; kaynak silinse bile **Kaynak Onar** dosyaları adlarıyla geri çıkarabilir.
- **Kaynaksız pack'ler:** gizlenir ama index'leri sahiplik hesabına girer (aynı dosya başka pack'e kopyalanmaz).
- **Konfigürasyon formatı:** `make_xml/*_create.xml` (Input `kaynak:prefix`, ArchivePath, Ignore). Ignore deseni eski araçla birebir: regex, göreli yolun tamamına (`regex_match`).
- **Format:** `foxfs.py` — client `Source/FoxFS/FoxFS/Archive.cc` ile aynı (AES-256-CFB128, LZ4 blok, xxh32 seed `0xFED0C`, ad = CRC32(lowercase, `/`)). Pack < 4 GiB (client u32 offset).
- **Ad kodlaması:** binary_unpack cp949 adları cp1252 mojibake olarak yazmış (`01.¼º`); geri cp1252 ile orijinal byte'lar elde edilir. `U+FFFD` içeren adlar unpack sırasında kaybolmuştur, raporlanır.
- **Sahiplik (muhafazakâr):** bir ad bugün hangi pack'teyse orada kalır (client pack'leri ad sırasıyla gezer, ilk bulan kazanır → oyunun gördüğü içerik değişmez). Hiçbir pack'te olmayan yeni ad en spesifik prefix'e sahip pack'e yazılır.
- **Koruma:** kaynakta olmayan mevcut entry'ler şifreli halleriyle taşınır (anahtar korunur). Kaynak klasörü olmayan pack hiç yazılmaz.
- **Hız:** analiz pack'leri iş parçacıklarında paralel tarar, pack index'lerini mmap ile okur (34 pack ~3 sn). oku+hash+LZ4+AES işçi süreçlerinde (çekirdek-1). Artımlı modda değişmeyen dosyaların şifreli verisi yeniden kullanılır (`_data/cache`). Tüm pack'ler ~90 sn, tek pack saniyeler.
- **Güvenlik:** `.building` dosyasına yazılır → index + tüm entry'ler çözülerek doğrulanır → eski pack `_data/backups`'a taşınır (pack başına son 1) → atomik `os.replace`. Client açıkken (dosya kilitli) yazmaz.
- **Kaynak Onar:** eski `xml/*.xml` listelerindeki arşiv yolu ↔ kaynak yolu eşlemesiyle, kaynakta silinmiş dosyaları pack'ten adlarıyla `binary_unpack`'e geri çıkarır; var olan dosyaya dokunmaz.

## Dosyalar
`foxfs.py` format • `config.py` keşif/ayar/prefix tahmini • `planner.py` tarama+plan • `builder.py` üretim • `pipeline.py` paketleme öncesi adımlar • `propgen.py` property.xml • `xmltools.py` XML yönetimi + ad hafızası • `pyc.py` Python 2.7 derleme • `cleanup.py` bozuk ad temizliği • `restore.py` kaynak onarımı • `cli.py` • `gui.py` (PyQt6)
