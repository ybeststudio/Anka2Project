# Anka2 — Güncellemeler

Sistemler hedef sunucudan reverse engineering ile çıkarılıp Anka2'ye 1:1 uyarlanır. Her yeni sistem kendi `ENABLE_*` define'ı ve `#ifdef` blokları arkasındadır (server `Source/Server/common/service.h`, client `Source/Client/source/UserInterface/Locale_inc.h`, Python tarafında `app.ENABLE_*`).

---

## 2026-10-29

### Beceri Yükseltici (`ENABLE_SKILL_UPGRADE`)
Referans `uiSkillUpgrade.SkillUpgradeWindow` penceresinin karşılığı.
- **Açılış:** Beceri penceresinin başlığının solundaki kitap butonu. Buffi penceresindeki `+` butonu da M ve G seviyelerinde bu pencereyi ilgili beceri seçili halde açar.
- **Sekmeler:**
  - Standart: karakterin 6 aktif becerisi
  - Pasif: Liderlik, Kombo, Madencilik, Dönüşüm
  - Buffi: 5 Buffi becerisi
- **Okuma:** **Başlangıç** bir kez, `Ctrl` + **Başlangıç** art arda 10 kez okur. Sunucu komutu: `/upgrade_skill <tür> <beceri> <adet> <kağıt> <münzevi> <fasulye> <negatif_derece> [ustalık]`.
- **Okuma kuralları** normal kitap okumayla aynıdır:
  - M seviyeleri: %35 başarı, 18–30 saat bekleme
  - Ruh taşı: Grand Master şans tablosu, 8–12 saat bekleme, derece puanı maliyeti
- **Malzeme kutuları:**
  - Kötü Ruh Kovma Kağıdı (71001): bekleme süresini atlatır
  - Münzevi Tavsiyesi (71094): M seviyesinde okumayı garantiler, G seviyesinde şansı artırır
  - Zen Fasulyesi (70102): negatif dereceyi ruh taşından önce yükseltir
  - Her biri okuma başına bir adet harcanır.
- **Derece onayı:** Derece sıfırın altına inecekse onay sorulur (`SkillUpgradeNegativeGradeConfirm`).
- **Pencere öğeleri:** kitap ilerlemesi (`[x / y]` ve dolum çubuğu), bekleme sayacı, başarı ve başarısızlık animasyonu.
- **Hatırlatıcı:** Alarm butonu beceri başına çalışır; bekleme bitince `[Hatırlatıcı]` adıyla PM gelir.
- **Tooltip'ler:** Beceri penceresi ve Buffi penceresiyle aynıdır.
- **Buffi kitapları:** Buffi Kitabı (M1 için 1, M10 için 10 başarılı okuma gerekir) ve ruh taşı aynı kurallarla okunur. Buffi Ustalık Kitabı (57107) ve Ustalık Taşı (57108) kitap slotuna sağ tıklanarak kullanılır.
- **Dosyalar:** `game/src/skill_upgrade.cpp`, `root/uiskillupgrade.py`. Kancalar: `interfacemodule.py`, `game.py`, `uicharacter.py`, `uicompanion.py`.

### Buffi — 2. aşama
- **Evrim (`ENABLE_BUFFI_EVOLUTION`):** referans tablosuyla 3 kademe.
  - 0→1: 55. seviye, 10M yang
  - 1→2: 75. seviye, 50M yang
  - 2→3: 90. seviye, 100M yang
  - Her kademe 5 çeşit malzeme ister.
  - Evrim penceresi `companion/evolve` görselleriyle hazırlandı. Komut: `/support_set_evolution`.
- **Beceri kitapları (`ENABLE_BUFFI_SKILL_BOOK`):** Buffi Kitabı, Ustalık Kitabı, Ustalık Taşı, Ruh Taşı. İlerleme `player_buffi.skill_book` sütununda saklanır.
- **Cinsiyet değiştirme (`ENABLE_BUFFI_CHANGE_SEX`):** tılsım 57105. Uyumsuz kostümler önce çıkarılmalıdır.
- **Item'lar:** 14 yeni item (11 evrim malzemesi ve 3 kitap/taş). İsimler 11 dilde, açıklamalar tr/en/de; ikonlar referanstan alındı.
- **Mesajlar:** Eksik sunucu mesajları `LC_TEXT` ile eklendi ve Türkçeye çevrildi.

---

## 2026-10-28

### Buffi sistemi (`ENABLE_BUFFI_SYSTEM`)
- **Çağırma:** mühür 57100 `WEAR_BUFFI` slotuna takılınca NPC 34900/34901 çağrılır.
- **Gelişim:** seviye / EXP, zeka kademeleri (120/140/160/180).
- **Beceriler:** 5 buff becerisi (Kutsama, Yansıtma, Ejderhanın Yardımı, Çabukluk, Saldırı Artışı).
- **Beceri rengi:** 5 katmanlı beceri rengi.
- **Cevherler:** 57101 ve 57103. Takılır ve çıkarılır, kalan süre korunur, zeka bonusu verir.
- **Kostümler:** zırh, silah, saç ve kuşak parçaları Buffi'nin üstünde görünür.
- **Pencere:** referansla 1:1 Buffi penceresi; beceri, EXP ve zeka tooltip'leri, mühür tooltip'i, sarı isim etiketi.
- **Davranış:** PvP'de mühür çıkarılınca buff'lar korunur; PvM'de otomatik takılır. Sahip ölünce Buffi gider.
- **Çabukluk:** büyü hızı formülü düzeltildi, Buffi'nin hareket hızı büyü hızına eşitlendi.

### Envanter
- **Düzen:** referans düzeni. Ekipman alanına Buffi / pet / binek ve kostüm slotları eklendi; boş slotlara isim tooltip'leri geldi.
- **Araçlar:** Sırala (`/sort_inventory`), birleştir ve arama.
- **Kemer envanteri:** 7x7 (49 slot). Kemer takılı değilken ızgara karartılır.
- **Düzeltmeler:**
  - Kemerler hiç takılmıyordu; kemer proto'su düzeltildi.
  - Petler çağrılmıyordu; pet çağırma düzeltildi.
  - Yüzük slotlarının referansla aynı davrandığı doğrulandı.

### Client
- **GPU tüketimi:** pencere arka plandayken FPS 30 ile sınırlandı. FPS sınırı hiç kaydedilmemişse 60 uygulanır.
- **Paketleme:** artık Pack Studio ile yapılıyor (`Tools/archiver`). Eski `Archiver.exe` eksik pack üretiyordu.

---

## 2026-10 (önceki)
- **Server:** FreeBSD 15.1, MariaDB 10.11, 64-bit build, base `clang++` 19.
- **Server:** Güvenlik ve performans audit'i yapıldı (~40 düzeltme); `admin.sh` durdurma sırası düzeltildi.
- **Server mesajları:** `LocaleChatPacket(index)` çağrıları `ChatPacket(LC_TEXT(...))`'e çevrildi (sohbet metinleri referansla 1:1).
- **Client:** x64 build (VS 2026 / v145), D3D12 RenderCore.
- **Gelişmiş oyun seçenekleri:**
  - arayüz ölçeği
  - efekte özel hasar rengi
  - bildirim geçmişi
  - metin hedef sırası
