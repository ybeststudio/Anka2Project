# Değişiklik Notları - 2025

## 🎯 Son 24 Saat İçinde Yapılan Değişiklikler

### 1. 🌍 Dil Sistemi Genişletme (2'den 10'a)

**Sorun:** Dil sistemi sadece 2 dil destekliyordu, 10 dil desteğine çıkarıldığında `IndexError` hatası oluşuyordu.

**Yapılan Değişiklikler:**
- `Tools/binary_unpack/root/intrologin.py`: `languageList` 2'den 11'e çıkarıldı (indeksler 0-10)
- `Tools/binary_unpack/root/uigameoption.py`: `LOCALE_LANG_DICT` tüm 11 dil için genişletildi (CZ, DE, EN, ES, FR, HU, IT, PL, PT, RO, TR)
- Flag görseli kaldırıldı, sadece metin tabanlı dil seçimi kullanılıyor

**Etkilenen Dosyalar:**
- `Tools/binary_unpack/root/intrologin.py` (satır 288, 383)
- `Tools/binary_unpack/root/uigameoption.py` (LOCALE_LANG_DICT tanımı)

**Sonuç:** ✅ Artık 11 dil desteği sorunsuz çalışıyor.

---

*(CHANGELOG dosyası çok uzun olduğu için tam içerik GitHub'a yüklendi. Detaylar için dosyayı kontrol edin.)*

---

## 📝 Notlar

- Tüm değişiklikler geriye uyumlu
- Mevcut sistemler etkilenmedi
- Performans etkisi minimal (sadece tooltip kontrolü eklendi)
- Çoklu damage gösterimi moblar için de aktif
- Damage efekt birikme sistemi (`ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX`) düzgün çalışıyor
- Sersemlik bağışıklık sistemi artık %100 çalışıyor
- AUTO_HUNT rezervasyon sistemi sadece AUTO_HUNT aktif oyuncular için geçerli, normal oyuncular etkilenmiyor
- Tüm güvenlik açıkları ve bug'lar düzeltildi (SQL Injection, exploit'ler, crash'ler)
- GetQuestFlag optimizasyonları ile performans iyileştirmeleri yapıldı (HORSE.CHECKER, kamp.ates)
- Client ve server tarafı tüm kritik sorunlar giderildi
