# Anka2 Server Güvenlik & Performans Audit — 2026-10-16

Kaynak: `/usr/src/Server` (FreeBSD 15.1, clang++ 19). Çalışma kopyası: `C:\Anka2Project\Tools\server_audit_20261016\Server` (git baseline: `baseline /usr/src/Server 2026-10-16`).
Orijinal kaynak arşivi: `Server_20261016.tgz`. Değişen 32 dosya: `patch_20261016.tar`.
Kodlama (cp1254/cp949) ve satır sonları (CRLF/LF) her dosyada orijinalle aynı tutuldu.

## Düzeltilenler

### Kritik — ekonomi / dupe
| # | Yer | Sorun | Düzeltme |
|---|-----|-------|----------|
| 1 | `cube_renewal.cpp` Cube_Make | Negatif `count_item` → check'ler geçer, `PointChange(GOLD, -gold*neg)` = sınırsız yang. Guard'lar tanımsız `CUBE_NO_ITEM` içindeydi | `count_item` 1..9999 (client `make_mul_max`), `index>=0` |
| 2 | `cube_renewal.cpp` | Herhangi bir stack "improve item" gösterilerek +%40 başarı | Client hep `-1` gönderiyor → sunucuda yol kapatıldı |
| 3 | `offlineshop_manager.cpp` AddItem/OpenMyOfflineShop/Buy | Negatif fiyat → alıcıya yang, sahibin borcu withdraw ile siliniyor | Fiyat `0 < p < GOLD_MAX` (game + DB) |
| 4 | `offlineshop_manager.cpp` OpenMyOfflineShop | `display_pos` sınırsız → stack `TOfflineShop` taşması (crash/RCE); duplicate kontrolü ölüydü (`insert` yok) | `display_pos < 80`, pos+display_pos set kontrolü, `bItemCount` sınırı |
| 5 | `offlineshop_manager.cpp` OpenMyOfflineShop | `continue` dalları `++pTable` atlıyordu → item/fiyat kayması | Döngü `++pTable` for-header'a alındı |
| 6 | `offlineshop_manager.cpp` Buy + `db/ClientManagerOfflineshop.cpp` | `pos` sınırsız → game OOB read, **DB process OOB write** | Game+DB'de `pos < 80`; DB BuyItem'da `id` ve `price` eşleşmesi |
| 7 | `db/ClientManagerOfflineshop.cpp` boot | `pos > MAX` off-by-one | `>=` |
| 8 | `offlineshop_manager.cpp` AddItem | `CanAddItemShop()` hiç çağrılmıyordu; transmutation/refine açıkken item shop'a taşınıp tekrar kullanım | `CanAddItemShop() / CanHandleItem() / GetExchange()` + dolu slot kontrolü |
| 9 | `shop_manager.cpp` Sell | Trade'deki stack'in bir kısmı satılıp karşı taraf kandırılabiliyor / UAF | `IsExchanging() / GetExchange()` |
| 10 | `char_item.cpp` Count/RemoveSpecifyItem | Trade'deki item tüketilebiliyor | İkisi de exchanging item'ı atlıyor (tutarlı) |
| 11 | `char_gem.cpp` | `/gem market -N` → `info_items[-N]` OOB (rastgele item/gem); CraftGemItems trade/lock kontrolsüz | `0 <= slot < size`; handle/exchange/lock/equip kontrolü |

### Kritik — crash / bellek bozulması
| # | Yer | Sorun | Düzeltme |
|---|-----|-------|----------|
| 12 | `input_main.cpp` Hunting (HEADER 99) | Client `type` → global array OOB; flag DB'de kalıp her login'de core crash | `type <= 1`; `CheckHunting`'te bozuk flag sanitize |
| 13 | `desc.cpp` SetLoginKey | Aynı bağlantıda çoklu LOGIN3 → eski key expire olmuyor → async auth sonucu freed DESC'e yazıyor (pre-auth UAF) + leak | Yeni key set edilirken eskisi `Expire()` |
| 14 | `char_item.cpp` DestroyItem/SellItem | Item silindikten sonra okunuyor (UAF) | Link/mesaj silmeden önce oluşturuluyor |
| 15 | `input_main.cpp` ItemDestroy | `DestroyItem` iki kez çağrılıyor (gold drop'ta bile item siliniyor), `ch` null kontrolü eksik | Tek çağrı + null check |
| 16 | `cmd_general.cpp`/`char.cpp` `/warp_on` | 0-255 index, tablo 35 satır → OOB, bedava ışınlanma, ölü/warp kontrolü yok | `1 <= idx < tablo`, `IsDead/IsWarping/CanWarp` |
| 17 | `char_pass*.cpp` `/battlepass_bitirici` | vector OOB (crash / OOB write) | Index bounds (Send + DoMission) |
| 18 | `char.cpp` Destroy | Safebox/mall açma event'leri raw `ch` tutuyor, karakter silinince iptal edilmiyor | `Destroy()`'da cancel |
| 19 | `char_skill.cpp` snake skill | Event bitince holder sıfırlanmıyor → sonraki cast freed event'i cancel ediyor; Destroy'da cancel yok | `ClearSnakeSkillEvent()` + Destroy'da cancel |
| 20 | `guild` grade (`input_main.cpp`) | grade 0/16-255 → `grade_array[grade-1]` OOB auth okuma | `2 <= grade <= GUILD_GRADE_COUNT` |
| 21 | `char_manager.cpp`, `input_db.cpp` | `sys_err(0, "...")` → fmt=NULL, hata yolunda crash | `0,` kaldırıldı |
| 22 | `input_main.cpp` | FishEvent `wCell`, OfflineShop `OPEN_WITH_VID` tek signed byte okunuyor; ChestDropInfo boş vector `&v[0]` | `memcpy` ile tam okuma; boş vector guard |
| 23 | `char.cpp`, `desc.cpp`, `cmd_gm.cpp` | `vsnprintf` dönüşü clamp edilmeden length → stack verisi client'a | Clamp |

### DB syserr kök nedeni (exit_y / quest lValue / item socket)
`int32_t` alanlar `%ld` ile basılıyordu (FreeBSD amd64'te `long` 64-bit). Register'daki argümanlarda negatif değer `4294967295` oluyor, stack'teki argümanlarda üst 32 bit çöp (`exit_y = 5282853634286281024 = 0x495077640003E140`, alt 32 bit = gerçek 254272).
Düzeltilen: `ClientManagerPlayer.cpp` (exit_x/y/map_index, alignment, affect), `ClientManager.cpp` (quest lValue, **item socket0-2**), `ClientManagerEventFlag.cpp`, `ClientManagerOfflineshop.cpp` (x/y/z, sockets), `GuildManager.cpp` (guild war). Game tarafındaki `%ld`'ler `long` argümanlı — sorun yok.
**Not:** Bu hatadan önce item socket'leri ve quest flag'leri DB'ye bozuk/eksik yazılmış olabilir (query fail ediyordu).

### sungma_move = -30384
`cmov+` vb. üst sınırı `GetPoint` (bonus dahil) ile hesaplıyordu → negatif `nPoint` real değeri düşürüyordu; quest `pc.set_c*` sınırsız. Düzeltme: sınır `GetRealPoint` ile, `0 < nPoint <= conqueror point`; quest setter'larda negatif/yetersiz puan reddi; save'de `MINMAX(0, v, SHRT_MAX)`.

### Diğer güvenlik
| Yer | Sorun | Düzeltme |
|-----|-------|----------|
| Messenger `SUMMON_BY_NAME` | Herhangi bir oyuncu zorla yanına çekilebiliyor | Hedefin son 60 sn içinde istek göndermiş olması şart; isim kopyaları NUL-terminate |
| `/remove_buff` | Chat ban (223), debuff, PvP cezası silinebiliyor | Sadece skill buff (`<200`, skill proto var, 66 hariç) |
| `/stat-` | Stat başlangıç değerinin altına/negatife | `real - initial` ile sınır |
| Safebox şifre | Sınırsız deneme + her denemede senkron SQL | 1 deneme/sn, 5 hatada 60 sn |
| Offline shop sign/title/decoration | NUL-terminate yok (OOB read); DB'de escape yok (`\` ile query kırma) | Handler'da NUL-terminate; DB'de `EscapeString` |
| `game.mysql_query` (quest) | Lua string format olarak; leak | `"%s"` + `unique_ptr` |
| `notice_all`, dungeon notice | Lua string format olarak | `"%s"` |
| `check_name_euckr` | Trail byte kontrolsüz (`'`/`\` geçer) | 0xA1-0xFE doğrulaması (canlı locale `europe` → `check_name_alphabet`, şu an etkilenmiyor) |

### Performans / leak
| Yer | Sorun | Düzeltme |
|-----|-------|----------|
| `GetCoins()` | Her `PointsPacket`'te main thread'de senkron SELECT | Sadece gösterim için 5 sn cache (`GetCoinsCached`); harcama yolları taze okuyor (coin dupe riski yok) |
| catch_king, stone_event, messenger | `DirectQuery` sonucu free edilmiyor | Async `Query` |
| maintenance | 0 satırda leak | `unique_ptr` |
| char_gem | Her çağrıda sadece isim için `CreateItem` (silinmiyor) | Item table'dan isim |
| char_battle | Her drop'ta `sys_log(0)` | `sys_log(1)` (PK drop log 0'da kaldı) |

## Açık kalanlar (canlı test / veri gerektiriyor)
1. **Guild mark/symbol upload yetkisiz** (`input_login.cpp` GuildMarkUpload/SymbolUpload): login olmamış socket herhangi bir loncanın bayrağını değiştirebilir. Düzeltme, mark-login `handle/random_key` doğrulaması ister; client'ın mark sunucusuna hangi core üzerinden bağlandığı canlı test edilmeden yapılırsa bayrak yükleme bozulabilir.
2. **`/open_range_npc <vnum>`**: herhangi bir shop uzaktan açılabiliyor. İzin verilen shop vnum listesi (uzaktan market UI'ındaki NPC'ler) gerekiyor.
3. **`/ranking_subcategory`**: her çağrıda full-scan SQL; global 60 sn cache önerilir.
4. **`new_mob_timer.cpp`**: saniye saniye brute-force zaman araması (2-3 sn main thread bloğu olabilir), `mday==32` rollover hatası.
5. **Quest lua dosyaları** pakette yok: `mysql_query`/`give_award`/highscore'a `input()` değeri giden quest var mı kontrol edilmeli.
6. **Offline shop buy race**: gold DB onayından önce düşüyor, reddedilirse iade yok.
7. `GetCoins/SetCoins` read-modify-write atomik değil (web shop ile eşzamanlı harcama). `UPDATE ... SET coins = coins - X WHERE coins >= X` önerilir.
