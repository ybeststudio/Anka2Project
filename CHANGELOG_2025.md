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

## Özet

| # | Deðiþiklik | Dosya Sayýsý | Durum |
|---|-----------|--------------|-------|
| 1 | NPC/Binek/Pet'lere Skill Damage ve Boss Saldýrý Hatasý Düzeltme | 1 | ? Tamamlandý |
| 2 | Zindan Haritalarýnda Kat Atlatma Nesnelerinin + Basma Hatasý Düzeltme | 1 | ? Tamamlandý |
| 3 | Dil Deðiþtirme Esnasýnda HORSE_LEVEL4 Hatasý Düzeltme | 1 | ? Tamamlandý |

**Toplam:** 3 dosya deðiþtirildi (bug fix'ler ile)

---

## Notlar

- Tüm deðiþiklikler geriye uyumlu
- Mevcut sistemler etkilenmedi
- NPC/Binek/Pet'lere skill damage iþlenmesi engellendi (`battle_is_attackable` kontrolü eklendi)
- Bosslar birbirine skill ile saldýramýyor (`ComputeSkill` fonksiyonunda kontrol eklendi)
- Zindan haritalarýnda kat atlatma nesnelerinin + basma iþlemi sorunsuz çalýþýyor
- Dil deðiþtirme esnasýnda `HORSE_LEVEL4` hatasý oluþmuyor
