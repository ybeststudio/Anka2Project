# FreeBSD 1 GB RAM optimizasyonu — 17 Ekim 2026

## Kök neden ve kod değişikliği

Eski `CHARACTER` nesnesi mob/NPC dahil her karakter için oyuncu envanteri ve grid dizilerini inline taşıyordu. DWARF ölçümünde `sizeof(CHARACTER)` 31.176 byte, `CHARACTER_POINT_INSTANT` 25.360 byte idi. `char.h` içinde bu dört dizi `TCharacterItemStorage` yapısına taşındı. `CHARACTER::Create` PC için storage oluşturur; item yazma yolları gerektiğinde lazy allocation yapar; `Destroy` storage'ı serbest bırakır. Okuma yolları (`GetItem`, `GetWear`) storage yoksa `NULL` döndürür ve mob için allocation yapmaz.

Yeni derlemede `sizeof(CHARACTER)` 8.248 byte ve `TCharacterItemStorage` 22.952 byte. Karakter başına sabit yük 22.928 byte azaldı. Değişiklikler `Source/Server/game/src/char.h`, `char.cpp`, `char_item.cpp` dosyalarındadır. Kaynaklar FreeBSD 15.1 VM üzerinde derlendi; kurulu binary `/usr/home/share/bin/game` SHA-256: `87ccef2a8ecbce38b7df98b2f7c208572b6b1ad021af9ee84679a0620560a3d1`.

## VM doğrulaması

- Fiziksel RAM: `hw.physmem=1033420800` byte. Aktif swap yalnızca özgün `/dev/gpt/swapfs`, 1 GiB.
- Ek 3 GiB `/usr/swap0` ve ilgili `fstab` satırı kaldırıldı. `admin.sh` içindeki açılış sırası özgün sürüme döndü; core başına 1 saniye bekleme var, önceki 25 saniyelik workaround yok.
- Oyuncu bağlantısı yokken `admin.sh` ile stop/start yapıldı. MariaDB, DB, üç Channel1 core'u, Channel99 ve Auth portları (`3306`, `15000`, `30101`, `30103`, `30105`, `30003`, `30001`) açıldı.
- Açılıştan yaklaşık 1 dakika sonra swap 569–571 MiB kullanımda, core PID'leri sabit. `dmesg` OOM öldürme sayısı 18; bu sayı son açılışta artmadı.
- Önceki binary ile gözlenen sabit swap yaklaşık 2,1 GiB idi. Bu değer aynı koşullarda birebir benchmark değildir, ancak yeni binary'nin 1 GiB özgün swap ile açılışı doğrudan doğrulandı.

## Sınırlar ve geri dönüş

Client ile giriş, envanter/equipment ve NPC/mob etkileşimi henüz canlı olarak doğrulanmadı. Sunucu açılışı ve portlar doğrulandı. Yük altında RAM/swap izlemesi ayrıca gerekir.

Eski game binary yedeği `/usr/home/share/bin/game.anka_before_item_storage`; `admin.sh` yedeği `/usr/home/admin.sh.anka_1gb_before`; swap kaldırılmadan önceki `fstab` yedeği `/etc/fstab.anka_1gb_temp_swap_removed_backup`. Binary yedeği daha önce mevcut bir sürümden alınmıştır; optimizasyon öncesindeki tam son binary ile byte-for-byte eşitliği doğrulanmamıştır.
