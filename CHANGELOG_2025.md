# Deðiþiklik Notlarý - 2025

## Son 24 Saat Ýçinde Yapýlan Deðiþiklikler

### 1. NPC/Binek/Pet'lere Skill Damage ve Boss Saldýrý Hatasý Düzeltme

**Sorun:** 
- NPC'lere, bineklere ve pet'lere skill damage iþleniyordu
- Bosslar birbirine skill ile saldýrabiliyordu
- `ComputeSkill` fonksiyonunda `battle_is_attackable` kontrolü eksikti

**Yapýlan Deðiþiklik:**
- `Source/Server/game/src/char_skill.cpp` (satýr 2075):
  - `ComputeSkill` fonksiyonuna `battle_is_attackable` kontrolü eklendi
  - `pkVictim` null kontrolünden hemen sonra eklendi
  - Artýk skill'ler sadece saldýrýlabilir hedeflere uygulanýyor
  - Bosslar birbirine skill atamaz
  - Oyuncular binek/pet/NPC'lere skill atamaz

**Etkilenen Dosyalar:**
- `Source/Server/game/src/char_skill.cpp` (ComputeSkill fonksiyonu)

**Sonuç:** ? NPC'lere, bineklere ve pet'lere skill damage iþlenmiyor. Bosslar birbirine skill ile saldýramýyor.

---

### 2. Zindan Haritalarýnda Kat Atlatma Nesnelerinin + Basma Hatasý Düzeltme

**Sorun:** 
- Zindan haritalarýnda düþen kat atlatma nesnelerinin + basma iþlemi gerçekleþtirme sýrasýnda "Bu eþyayý takas edemezsiniz." hatasý oluþuyordu
- Item takas kontrolü yanlýþ çalýþýyordu

**Yapýlan Deðiþiklik:**
- `Source/Binary/source/UserInterface/input_main.cpp`:
  - Zindan haritalarýnda kat atlatma nesneleri için özel kontrol eklendi
  - Item takas kontrolü düzeltildi
  - + basma iþlemi artýk sorunsuz çalýþýyor

**Etkilenen Dosyalar:**
- `Source/Binary/source/UserInterface/input_main.cpp` (item takas kontrolü)

**Sonuç:** ? Zindan haritalarýnda kat atlatma nesnelerinin + basma iþlemi artýk sorunsuz çalýþýyor, "Bu eþyayý takas edemezsiniz." hatasý oluþmuyor.

---

### 3. Dil Deðiþtirme Esnasýnda HORSE_LEVEL4 Hatasý Düzeltme

**Sorun:** 
- Dil deðiþtirme esnasýnda `NameError: name 'HORSE_LEVEL4' is not defined` hatasý oluþuyordu
- `localeinfo.py` dosyasýnda `HORSE_LEVEL4` tanýmý eksikti veya yanlýþ import ediliyordu

**Yapýlan Deðiþiklik:**
- `Tools/binary_unpack/root/localeinfo.py`:
  - `HORSE_LEVEL4` tanýmý eklendi veya import düzeltildi
  - Dil deðiþtirme sýrasýnda tüm horse level tanýmlarý kontrol edildi
  - Eksik tanýmlar tamamlandý

**Etkilenen Dosyalar:**
- `Tools/binary_unpack/root/localeinfo.py` (HORSE_LEVEL4 tanýmý)

**Sonuç:** ? Dil deðiþtirme esnasýnda `HORSE_LEVEL4` hatasý oluþmuyor, dil deðiþtirme iþlemi sorunsuz çalýþýyor.

---

### 4. Item Sil/Sat/Düþür ve Battle Pass Pencerelerinde Tooltip Kalma Sorunu Düzeltme

**Sorun:** 
- Item yere atýldýðýnda açýlan diyalog penceresinde (ItemQuestionDialog) item'in tooltip'i gösteriliyordu
- ESC ile kapatýldýðýnda tooltip ekranda kalýyordu
- Battle Pass penceresi ESC ile kapatýldýðýnda tooltip'ler ekranda kalýyordu
- Mission list item'larýndaki tooltip'ler de ekranda kalýyordu

**Yapýlan Deðiþiklikler:**

#### 4.1 ItemQuestionDialog Tooltip Temizleme
- `Tools/binary_unpack/root/uicommon.py` (satýr 701-710):
  - `Close()` metoduna tooltip temizleme eklendi
  - `tooltipItem` için `HideToolTip()` ve `ClearToolTip()` çaðrýlýyor
  - ESC ile kapatýldýðýnda tooltip ekranda kalmýyor

#### 4.2 BattlePassWindow Tooltip Temizleme
- `Tools/binary_unpack/root/uibattlepass.py` (satýr 82-120):
  - `Close()` metoduna kapsamlý tooltip temizleme eklendi
  - Merkezi tooltip sistemi (`ToolTip._allToolTips`) kullanýlarak tüm tooltip'ler temizleniyor
  - Kendi tooltip'leri (`tooltipItem`, `tooltip`) temizleniyor
  - Mission list içindeki tüm item'larýn tooltip'leri temizleniyor (normal ve premium listeler)
  - ESC ile kapatýldýðýnda tüm tooltip'ler ekranda kalmýyor

**Etkilenen Dosyalar:**
- `Tools/binary_unpack/root/uicommon.py` (ItemQuestionDialog.Close fonksiyonu)
- `Tools/binary_unpack/root/uibattlepass.py` (BattlePassWindow.Close fonksiyonu)

**Sonuç:** ? Item sil/sat/düþür diyalog penceresi ve Battle Pass penceresi ESC ile kapatýldýðýnda tooltip'ler ekranda kalmýyor. Tüm tooltip'ler düzgün þekilde temizleniyor.

---

### 5. Metin Keserken Core Dump Hatasý Düzeltme (Null Pointer)

**Sorun:** 
- Metin keserken (poison damage) core dump oluþuyordu
- `pAttacker` null olduðunda `IsBotCharacter()` çaðrýlýyordu ve segfault oluþuyordu
- Operatör önceliði nedeniyle `pAttacker && pAttacker->IsPC() || pAttacker->IsBotCharacter()` ifadesi yanlýþ deðerlendiriliyordu

**Hata Mesajý:**
```
Address not mapped to object.
#0  CHARACTER::IsBotCharacter (this=<optimized out>) at ./char.h:2392
#1  CHARACTER::Damage (this=0x304ac280, pAttacker=0x0, dam=855, type=DAMAGE_TYPE_POISON)
```

**Yapýlan Deðiþiklik:**
- `Source/Server/game/src/char_battle.cpp` (satýr 2571-2575):
  - Operatör önceliði düzeltildi
  - `pAttacker && pAttacker->IsPC() || pAttacker->IsBotCharacter()` ? `pAttacker && (pAttacker->IsPC() || pAttacker->IsBotCharacter())`
  - Artýk `pAttacker` null kontrolü hem `IsPC()` hem de `IsBotCharacter()` için geçerli
  - Parantezler eklendi, null pointer dereference engellendi

**Etkilenen Dosyalar:**
- `Source/Server/game/src/char_battle.cpp` (satýr 2571-2579)

**Sonuç:** ? Metin keserken (poison damage) core dump oluþmuyor. Null pointer kontrolü düzgün çalýþýyor.

---

### 6. Otomatik Toplama Sisteminde Beceri Kitaplarý Birleþtirme Sorunu Düzeltme

**Sorun:** 
- Otomatik toplama açýkken metinlerden düþen farklý beceri kitaplarý (hamle beceri kitabý, hava kýlýcý kitabý vb.) birleþtiriliyordu
- 10 farklý beceri kitabý düþerken, otomatik toplama açýk olduðunda hepsi ayný tip (ayný skill vnum'ýna sahip) oluyordu
- Otomatik toplama kapalý olduðunda beceri kitaplarý doðru þekilde düþüyordu
- `AutoGiveItem` fonksiyonunda beceri kitaplarý için sadece vnum kontrolü yapýlýyordu, socket kontrolü yapýlmýyordu
- Farklý skill vnum'larýna sahip beceri kitaplarý birleþtiriliyordu

**Yapýlan Deðiþiklikler:**

#### 6.1 AutoGiveItem Socket Kontrolü
- `Source/Server/game/src/char_item.cpp` (satýr 8486-8490):
  - Beceri kitaplarý için socket kontrolü eklendi
  - `item2->GetSocket(0) != item->GetSocket(0)` kontrolü eklendi
  - Artýk sadece ayný skill vnum'ýna sahip beceri kitaplarý birleþtiriliyor
  - Farklý skill vnum'larýna sahip beceri kitaplarý ayrý item'lar olarak kalýyor

#### 6.2 Otomatik Toplama Mesaj Gönderimi
- `Source/Server/game/src/char_battle.cpp` (satýr 884-905 ve 1007-1028):
  - Beceri kitaplarý için `SendPickupItemPacket` yerine `LocaleChatPacket` kullanýlýyor
  - `item->IsSkillBook()` kontrolü eklendi
  - Beceri kitaplarý için item ismini direkt gönderiyor (socket'lerdeki skill vnum'larýna göre doðru isim oluþturuluyor)

**Etkilenen Dosyalar:**
- `Source/Server/game/src/char_item.cpp` (AutoGiveItem fonksiyonu - socket kontrolü)
- `Source/Server/game/src/char_battle.cpp` (otomatik toplama mesaj gönderimi)

**Sonuç:** ? Otomatik toplama açýkken metinlerden düþen farklý beceri kitaplarý artýk birleþtirilmiyor. Her beceri kitabý kendi skill vnum'ýna göre ayrý item olarak kalýyor ve doðru isimle gösteriliyor.

---

## Özet

| # | Deðiþiklik | Dosya Sayýsý | Durum |
|---|-----------|--------------|-------|
| 1 | NPC/Binek/Pet'lere Skill Damage ve Boss Saldýrý Hatasý Düzeltme | 1 | ? Tamamlandý |
| 2 | Zindan Haritalarýnda Kat Atlatma Nesnelerinin + Basma Hatasý Düzeltme | 1 | ? Tamamlandý |
| 3 | Dil Deðiþtirme Esnasýnda HORSE_LEVEL4 Hatasý Düzeltme | 1 | ? Tamamlandý |
| 4 | Item Sil/Sat/Düþür ve Battle Pass Pencerelerinde Tooltip Kalma Sorunu Düzeltme | 2 | ? Tamamlandý |
| 5 | Metin Keserken Core Dump Hatasý Düzeltme (Null Pointer) | 1 | ? Tamamlandý |
| 6 | Otomatik Toplama Sisteminde Beceri Kitaplarý Birleþtirme Sorunu Düzeltme | 2 | ? Tamamlandý |

**Toplam:** 8 dosya deðiþtirildi (bug fix'ler ile)

---

## Notlar

- Tüm deðiþiklikler geriye uyumlu
- Mevcut sistemler etkilenmedi
- NPC/Binek/Pet'lere skill damage iþlenmesi engellendi (`battle_is_attackable` kontrolü eklendi)
- Bosslar birbirine skill ile saldýramýyor (`ComputeSkill` fonksiyonunda kontrol eklendi)
- Zindan haritalarýnda kat atlatma nesnelerinin + basma iþlemi sorunsuz çalýþýyor
- Dil deðiþtirme esnasýnda `HORSE_LEVEL4` hatasý oluþmuyor
- Item sil/sat/düþür diyalog penceresi ESC ile kapatýldýðýnda tooltip'ler temizleniyor
- Battle Pass penceresi ESC ile kapatýldýðýnda tüm tooltip'ler (merkezi sistem, mission item'larý dahil) temizleniyor
- Metin keserken (poison damage) core dump oluþmuyor, null pointer kontrolü düzgün çalýþýyor
- Otomatik toplama açýkken metinlerden düþen farklý beceri kitaplarý artýk birleþtirilmiyor (her beceri kitabý kendi skill vnum'ýna göre ayrý item olarak kalýyor ve doðru isimle gösteriliyor)
