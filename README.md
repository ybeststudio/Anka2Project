# Anka2 Game Server & Client Projesi

## 📋 Proje Hakkında

Anka2, Mainline altyapısı üzerine kurulu bir Metin2 server + client geliştirme projesidir. Hedef Metin2 sunucularındaki sistemler reverse engineering ile analiz edilir ve Anka2 altyapısına mümkün olduğunca 1:1 uyarlanır.

### ⚠️ Önemli Notlar

- **FoxFS formatı**: Client pack'leri FoxFS (`ENABLE_FOXFS_ENCRYPT`) ile şifrelenir. Pack dosya yapısını ve dizin organizasyonunu değiştirmeyin; paketleme `Tools/archiver` ile yapılır.
- **Dosya kodlamaları**: `game.py` ve bazı C++ kaynakları cp1254/cp949 kodlamalı, bir kısmı CRLF satır sonlu. Dosyaları körlemesine UTF-8'e veya LF'e dönüştürmeyin.
- **Python**: Client script'leri Python 2.7'dir; Python 3 bytecode dağıtmayın.
- **Lokalizasyon**: Mob sohbetleri, OX Quiz ve `locale_string` client'ta `locale/[lang]` dizininden yüklenir. Görev çevirileri `[lang]/translate.lua` dosyasındadır.
- **Bağlantı bilgileri**: VM / MySQL / admin hesap bilgileri repoda ve bu README'de tutulmaz.

---

## 🏗️ Altyapı Bilgileri

| | Server | Client |
|---|---|---|
| **İşletim sistemi** | FreeBSD 15.1-RELEASE (amd64), VirtualBox VM | Windows 10/11 |
| **Mimari** | 64-bit (x86-64) | x64 (ana hedef), Win32 konfigürasyonu da mevcut |
| **Derleyici** | Base sistem `clang++` 19.1.7 | Visual Studio 2026 (v145 toolset; `GameShaders` v143) |
| **C++ standardı** | game: C++2a (C++20), db: C++14 | Çoğunlukla C++20; bazı projelerde C++17 / `latest` konfigürasyonları |
| **Build sistemi** | GNU Make (`gmake`), toplu `Makefile` | `client.sln` (Debug / Release / WE × Win32 / x64) |
| **Veritabanı** | MariaDB 10.11 (`mariadb1011-server/client`) | — |
| **Script** | Lua (quest) | Python 2.7 |
| **Render** | — | Direct3D 9 + Direct3D 12 `RenderCore` (`ENABLE_RENDER_CORE`) |

> Eski README'deki FreeBSD 13.3, MariaDB 10.6, 32-bit server ve `clang++-devel` bilgileri artık geçerli değil. `llvm-devel` paketine gerek yoktur: build base sistemdeki `clang++` ile yapılır, game'in kullandığı `libomp` da base sistemden (`/usr/lib/libomp.so`) gelir.

---

## 📁 Proje Yapısı

```
C:\Anka2Project\
├── Client/                     # Oyuncuya dağıtılabilir temiz client (yeri sabit: archiver ../../Client/pack)
├── Source/
│   ├── Client/                 # Client kaynak kodu (client.sln) — tüm proje yolları relative
│   ├── Server/                 # Canlı /usr/src/Server ile senkron yerel kopya
│   ├── DumpProto/              # item/mob proto dönüştürücü
│   ├── FoxFS/                  # FoxFS pack sistemi kaynağı (FoxFS.sln)
│   └── ThirdParty/             # giflib, jpeg-9d
├── Tools/
│   ├── archiver/               # Pack Studio (FoxFS paketleyici, arayüz + CLI) ve eski araçlar
│   ├── binary_unpack/          # Açılmış client pack'leri (git'e yalnız root/ Python kaynakları girer)
│   └── dump_proto/             # item/mob proto dönüştürücü (dil başına item_names / mob_names)
├── Reference/                  # RE referansları (resmi client, Baerim dump'ları) — git'e girmez
├── Server/VM/Anka2_FreeBSD15.vdi  # Server VM diski — git'e girmez
```

> 2026-10-16 yeniden yapılandırma: build çıktıları (`vs_files` ara dosyaları, `binary/`, `.vs/`, FoxFS/Server objeleri), `Tools/backups`, `server_cleanup_backup_20261016`, `server_audit_20261016` ve eski `home/` kopyası silindi (~13.1 GB).

### Client dağıtımı (`Client/`)

Dağıtılan içerik: `Metin2Release_x64.exe`, `pack/`, `lib/` (Python 2.7 stdlib), `bgm/`, `upload/`, `Data/`, `mark/` + `syserr/` (boş, oyun yazar), `locale.cfg`, `config.exe` + `config.ini`, default `metin2.cfg` (windowed) ve `channel.inf`.

**x64 runtime** (hepsi x64; exe 64-bit olduğu için 32-bit kopyalar yüklenemez):

| DLL | Kim kullanıyor |
|---|---|
| `python27.dll` | exe (import). Kendisi `MSVCR90.dll` (VC++ 2008 x64, SxS) ister → oyuncu PC'sinde **VC++ 2008 SP1 x64 redist** gerekir |
| `d3dx9_43.dll` | exe (DirectX June 2010 redist, x64) |
| `DevIL.dll` | exe → `msvcp140.dll`, `vcruntime140.dll`, `vcruntime140_1.dll` (VC++ 2015-2022 x64) + UCRT (Win10'da hazır) |

`config.exe` 32-bit, statik CRT; yalnız sistem `d3d8.dll`'ini kullanır. `channel.inf` silinmemeli: yoksa `introLogin.__LoadChannelInfo` `selectedChannel` set etmiyor ve login ekranı `AttributeError` ile kapanıyor.

Runtime'da oyunun ürettiği (dağıtıma girmeyen) dosyalar: `log.txt`, `syserr/*`, `screenshot/`, `mark/*`, `advanced_options.cfg`, `advanced_notifications.json`, `mouse.cfg`, `title_state.txt`, `bgm/lastplay.inf`, `lib/*_track`, `lib/respdialop.pcl`. Kayıtlı hesaplar registry'de (`set_reg`) tutulur.

### Client (`Source/Client`)

```
Source/Client/
├── source/
│   ├── UserInterface/   # Ana modül: ağ, oyuncu, envanter, Python modülleri (Locale_inc.h: ~350 ENABLE_ flag)
│   ├── GameLib/         # Aktörler, hareket, çarpışma
│   ├── EterLib/         # Grafik motoru (D3D9)
│   ├── RenderCore/      # D3D12 render çekirdeği (GpuScene, ClusteredLighting, PostProcessing, Raytracing...)
│   ├── GameShaders/     # Shader projesi
│   ├── EterBase/, EterGrnLib/, EterImageLib/, EterLocale/, EterPack/, EterPythonLib/
│   ├── EffectLib/, ScriptLib/, SpeedTreeLib/, SphereLib/, TerrainLib/, PRTerrainLib/
│   ├── AudioLib/, MilesLib/   # Ses
│   ├── Discord/, CWebBrowser/
│   └── WorldEditor/     # Harita editörü (WE konfigürasyonu)
├── extern/              # Harici header ve .lib dosyaları (boost, cryptopp, Python-2.7, FoxFS...)
├── vs_files/            # .vcxproj/.filters/.user + ara build dosyaları (git'e yalnız proje dosyaları girer)
├── binary/              # Build çıktıları (Metin2Release_x64.exe, WorldEditor_*.exe) — build ile oluşur
└── build_*.bat / rebuild_*.bat / clear_*.bat
```

> `extern/devil_src/DevIL/build_x64/` CMake tarafından üretilmiş ve içinde **mutlak yol** (`C:/Anka2Project/Source/Binary/...`, CMakeCache) var. `client.sln` bunu kullanmaz; DevIL yeniden derlenecekse bu klasör silinip `cmake -S extern/devil_src/DevIL -B extern/devil_src/DevIL/build_x64` ile yeniden üretilmeli.

### Server (`/usr/src/Server` — sunucudaki **tek güncel kaynak**)

`Source/Server` bunun yerel kopyasıdır; 2026-10-16'da audit'li 32 dosya + `input_login.cpp` ile canlıyla byte-byte senkronlandı. Kaynakta değişiklik önce canlıda yapılır, sonra buraya çekilir (kodlama cp1254/cp949 korunarak).

```
/usr/src/Server/
├── Makefile             # Toplu build: libs → db → game
├── game/src/            # Oyun sunucusu (182 .cpp)
├── db/src/              # DB sunucusu (29 .cpp)
├── common/              # Ortak header'lar (service.h: ~260 ENABLE_ flag, tables.h, length.h)
└── library/             # libthecore, libsql, libgame, libpoly, liblua, libdevil, msl, boost, nlohmann, rapidjson
```

### Sunucu çalışma dizini (`/usr/home`)

```
/usr/home/
├── admin.sh             # Başlat / durdur / log temizle / quest derle / yedek
├── main/
│   ├── db/              # db process'i
│   ├── auth/            # auth core
│   └── channels/        # channel1-4 (core1-3) + channel99
└── share/
    ├── bin/             # game, db, vrunner (kanal dizinlerindeki game/db bunlara symlink)
    ├── conf/, data/, locale/
```

---

## 🔧 Derleme Gereksinimleri

### Server (FreeBSD 15.1)

**Gerekli paketler:**
```bash
pkg install boost-all cryptopp devil lzo2 gmake makedepend binutils \
            mariadb1011-server mariadb1011-client python27 ccache gdb git subversion
```
- Derleyici base sistemle gelir (`/usr/bin/clang++` 19). `llvm-devel` **gerekmez**.
- MariaDB client statik linklenir (`/usr/local/lib/mysql/libmariadbclient.a`).
- `squish` paketi `devil` bağımlılığıdır; game binary'si `libIL → libsquish → libomp` zincirini kullanır.

**Build:**
```bash
cd /usr/src/Server
gmake -j2 all        # libs + db + game
gmake -j2 game       # sadece game (libs önce derlenir)
```
Çıktılar `Makefile`'daki `GAMEDIR`/`DBDIR` ile doğrudan `/usr/home/share/bin`'e taşınır. Çalışan sunucunun binary'sini değiştirmeden test etmek için kaynağı kopyalayıp bu yolları geçici bir dizine yönlendirin.

> Makefile'lar `-w` ile derler (uyarılar gizli). `common/tables.h` veya `char.h` gibi header'lar değişirse game'in tamamı yeniden derlenir (2 vCPU'da ~7 dk).

### Client (Windows)

**Gereksinimler:**
- Visual Studio 2026 (v145 toolset) + v143 (GameShaders)
- Windows SDK 10.0
- Python 2.7 headers/lib (`extern/` altında)

**Build:**
```batch
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    Source\Client\client.sln /p:Configuration=Release /p:Platform=x64 /m
```
veya build script'leri (varsayılan Release x64):
```powershell
.\build_client.ps1 build                 # Release x64
.\build_client.ps1 build Release Win32   # Release 32-bit
.\build_client.ps1 info                  # MSBuild yolu ve mevcut exe'ler
```
`build_client.bat` aynı komutları destekler. İki script de önce VS 2026 (`Microsoft Visual Studio\18`) MSBuild'ini arar; `.ps1` ayrıca `vswhere` kullanır.

Çıktı adları: x64 → `binary\Metin2<Config>_x64.exe`, Win32 → `binary\Metin2<Config>.exe`. Çıktı `Client\` dizinine kopyalanır.

**Paketleme:** `Tools\archiver\Pack Studio.bat` (arayüz) veya `Tools\archiver\pack.bat build <pack...>` — ayrıntı: `Tools/archiver/pack_studio/README.md`. Eski `Archiver.exe` klasör atlıyor ve Korece adları paketlemiyordu (eksik pack); kullanılmamalı. root için Python 2.7 `.pyc` otomatik üretilir.

---

## 🚀 Sunucu Yönetimi

```bash
cd /usr/home
sh admin.sh     # 1: başlat (kanal sayısı sorulur) | 2: durdur | 3: log temizle | 4: quest derle | 5: yedek
```

**Durdurma sırası** (`admin.sh` → 2, 2026-10-16'da düzeltildi):
1. `vrunner` supervisor'ları durdurulur (aksi halde kapanan core'u hemen yeniden başlatır).
2. auth, tüm kanal core'ları ve channel99'a pid dosyasındaki PID ile `kill -1` gönderilir.
3. Core'ların kapanması 120 sn'ye kadar beklenir (oyuncu verisi kaydedilir).
4. En son db durdurulur.

> Eski script `pkill -1 <pid>` kullanıyordu; `pkill` isim aradığı için hiçbir process durmuyordu. Eski hali: `/usr/home/admin.sh.before_stopfix_20261016`.

Portlar: db `15000`, game `3000x` / `3010x`, MariaDB `3306`.

---

## ✨ Özellikler

Server `common/service.h` (~260) ve client `UserInterface/Locale_inc.h` (~350) içindeki `ENABLE_*` flag'leriyle yönetilir. Başlıcaları:

- Pet / Büyüyen Pet, Binek, Kostüm, Aura, Kuşak (Acce), Görünüm Değiştirme
- Çevrimdışı Pazar (renewal + arama), Hızlı Satış, Gem (Gaya) Pazarı
- Cube Renewal, Simya (Dragon Soul), Biyolog
- Avcılık Görevleri, Savaş Bileti (Normal + Premium), Koleksiyonlar
- Zindan Bilgi Sistemi, Zodiac, Meley, Queen Nethis, Battle Arena
- Fatih (Conqueror) seviye ve Sung Ma statları, Ünvan, Global Sıralama
- BotPlayer, Switchbot, Otomatik Av, Ruh Ruleti, Kral Yakala
- Çoklu dil, D3D12 RenderCore
- **Buffi (destek şamanı):** mühürle çağrılan şaman; seviye/EXP, 5 buff becerisi, beceri rengi, cevherler, kostümler, evrim, beceri kitapları, cinsiyet değiştirme (`ENABLE_BUFFI_*`)
- **Beceri Yükseltici:** karakter, pasif ve Buffi becerileri için otomatik kitap / ruh taşı okuma penceresi (`ENABLE_SKILL_UPGRADE`)
- **Referans envanter:** ekipman + Buffi / pet / binek slotları, sıralama, birleştirme, arama; 7x7 kemer envanteri
- Gelişmiş oyun seçenekleri (devam ediyor)

---

## 🔒 Güvenlik ve Performans

### 2026-10-16 Server Audit

`/usr/src/Server` üzerinde güvenlik ve performans audit'i yapıldı ve ~40 düzeltme canlıya alındı. Değişiklikler `Source/Server`'dadır.

- **Ekonomi / dupe:** Cube'da negatif adet ile sınırsız yang; offline shop'ta negatif fiyat, `display_pos`/`pos` taşması (game'de stack overflow, DB process'inde OOB write); trade'deki item'ın satılması/tüketilmesi; gem market negatif slot.
- **Crash / bellek:** Hunting `type` ile kalıcı core crash; auth'ta çoklu LOGIN3 ile pre-auth use-after-free; item silme/satma, safebox/mall ve snake skill event'lerinde UAF; `/warp_on`, `/battlepass_bitirici`, guild grade OOB; `sys_err(0, ...)` crash'leri.
- **DB syserr kök nedeni:** `int32_t` alanlar `%ld` ile basılıyordu (`exit_y = 5282853634286281024`, quest `lValue = 4294967295`, item socket'leri). `%d` olarak düzeltildi.
- **Diğer:** Messenger ile zorla ışınlama, `/remove_buff` ile chat ban kaldırma, depo şifresi brute-force, offline shop sign SQL escape, format-string açıkları, `vsnprintf` uzunluk clamp'i.
- **Performans:** Her `PointsPacket`'te çalışan senkron coins SELECT'i cache'lendi, `SQLMsg` leak'leri kapatıldı, drop başına log seviyesi düşürüldü.

**Açık kalanlar:** guild mark/symbol upload yetkisi, `/open_range_npc` whitelist'i, ranking SQL cache'i, `new_mob_timer` zaman araması, quest lua dosyalarının kontrolü (raporda ayrıntılı).

**Geri alma:**
- Kaynak: `/usr/src/Server_before_audit_20261016.tgz`
- Binary'ler: `/usr/home/share/bin/{game,db}.before_audit_20261016`

### VM Disk Bakımı

2026-10-16'da gereksiz paketler (`llvm-devel`, debug sembolleri, test paketleri), pkg cache'i, ccache ve eski build kalıntıları temizlendi. VDI dosyası 30 GB'tan 6.8 GB'a indirildi (eski build yedeği `server_cleanup_backup_20261016` 2026-10-16'da silindi). VDI yolu: `Server/VM/Anka2_FreeBSD15.vdi` (VirtualBox kaydı `modifymedium --move` ile güncellendi).

VDI tekrar büyürse:
```bash
# VM içinde
service mysql-server stop
dd if=/dev/zero of=/zero.fill bs=1m ; sync ; rm -f /zero.fill ; sync
shutdown -p now
```
```powershell
# Host'ta (VM kapalıyken)
& "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" modifymedium disk "C:\Anka2Project\Server\VM\Anka2_FreeBSD15.vdi" --compact
```

---

## 📅 Güncellemeler

### 2026-10 (son)
Ayrıntılı liste: [`CHANGELOG.md`](CHANGELOG.md).
- **Buffi sistemi:** referansla 1:1 pencere, tooltip'ler, isim etiketi, beceri rengi, cevherler, evrim, beceri kitapları, cinsiyet tılsımı, 14 yeni item.
- **Beceri Yükseltici:** Standart / Pasif / Buffi sekmeleri; okuma beklemesi, Kötü Ruh Kovma Kağıdı, Münzevi Tavsiyesi, Zen Fasulyesi; bekleme bitince PM ile hatırlatma.
- **Envanter:** referans düzeni, Buffi / pet / binek slotları, sırala / birleştir / ara, 7x7 kemer envanteri ve kemer proto düzeltmesi.
- **Client:** GPU tüketimi düzeltmeleri (arka planda FPS sınırı), Pack Studio ile paketleme.

### 2026-10
- **Server:** FreeBSD 15.1, MariaDB 10.11, 64-bit build, base `clang++` 19.
- **Server güvenlik audit'i** (yukarıda) ve `admin.sh` durdurma düzeltmesi.
- **VM:** Disk temizliği ve VDI compact işlemi (30 GB → 6.8 GB).
- **Client:** x64 build (VS 2026 / v145), D3D12 RenderCore.
- **Gelişmiş oyun seçenekleri:** Arayüz ölçeği ve fare koordinatı, efekte özel hasar rengi, lonca rütbesi gösterimi, 200 kayıtlık bildirim geçmişi ve filtreler, Ctrl + sol tık ile 8 metinlik hedef sırası. 14 seçenek hâlâ açık.

### 2026 (önceki)
- Yeni biyolog; çevrimdışı pazar / depo / sistem seçenekleri / otomatik av düzeltmeleri; zindan ve dünya patronu notice; vuruş-kaçırma; client kamera; binek/kaykay rotasyon; şaman skill ve animasyon düzeltmeleri; skill 6–9 etkin.
- DumpProto Release `.txt` kapatma davranışı iyileştirildi.
- İsteğe bağlı `ENABLE_BINARY_SERVERINFO`: sunucu bilgisi `PythonNetworkStreamModule.cpp` / binary üzerinden.

---

## 📜 Lisans ve Kullanım

Bu proje eğitim ve araştırma amaçlıdır. Ticari kullanım için gerekli lisansları kontrol edin.

---

## İletişim

- 💻 **GitHub:** [github.com/ybeststudio](https://github.com/ybeststudio)
- 🎧 **Discord Server:** [discord.gg/NXmc6JrwYr](https://discord.gg/NXmc6JrwYr)
- 🆔 **Discord ID:** beststudio
- 🌐 **Web:** [beststudio.dev](https://beststudio.dev)
- 💬 **TurkMMO Forum:** [Best Studio](https://forum.turkmmo.com/uye/2104546-best-studio/)
- 📺 **YouTube:** [@ybeststudio](https://www.youtube.com/@ybeststudiostr)
- 📷 **Instagram:** [@ybeststudio](https://www.instagram.com/ybeststudio)
- 👥 **Facebook:** [ybeststudio](https://www.facebook.com/ybeststudio/)
- 🐦 **Twitter:** [@ybeststudio](https://twitter.com/ybeststudio)
- 🎵 **TikTok:** [@ybeststudio](https://tiktok.com/@ybeststudio)
