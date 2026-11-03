# Buffi Sistemi — Referans Analizi ve Tasarım

Kaynak: `Reference/baerim_unpack/baerim.exe` (Cython handler disasm), `out/locale/locale/common/*.json`,
`wiki/systems/buffi_system.txt`, `out/root/uiscript/companionsystem.py`, `saryong_ui/ymir work/ui/game/companion/*`.

## 1. Server → Client komutları (baerim.exe `game.GameWindow.*` disasm, kesin)

| Komut | Argümanlar (sırayla, hepsi int) | Client çağrısı |
|---|---|---|
| `SetSupportData` | level, intel, exp, exp_total, evo, duration, runes_1, runes_2, runes_3 | `interface.SetBuffiInformation(*map(int,..))` |
| `SetSupportSkill` | pts, amt0, pwr0, amt1, pwr1, amt2, pwr2, amt3, pwr3, amt4, pwr4 | `interface.SetBuffiSkills(..)` |
| `SetSupportSkillBook` | book0..book4 | `interface.SetBuffiSkillsBook(..)` |
| `SetSupportSkillCooldown` | skill0..skill4 | `interface.SetBuffiSkillsCooldown(..)` |
| `SetSupportName` | name | `interface.SetBuffiName(name)` |
| `SupportLeave` | — | `interface.Buffi_SetUnsummon()` |
| `SetBuffiCostumeHiddenStates` | tuple → `constInfo.BUFFI_COSTUME_HIDDEN_STATES` | `CompanionWindow.RefreshBuffiCostumeVisibility()` |

Client → server: `SendAddOreBuffiPacket`, `SendBuffiDurationPacket` (CompanionWindow), `OnClickEvolutionBuffi`,
skill yükseltme (`UpdateBuffiSkill`), renk butonları, `UnsummonBuffi`.

## 2. Mekanik (locale/wiki metinleri, kesin)

- 57000 **Buffi** (mühür): çağırınca seni takip eden Buff Şaman. Takılıyken **PvP hasarın 1 ile sınırlı**.
  Elle çıkarınca verdiği buff efektleri silinir.
- 57001 Buffi Cevheri: +3 INT, 24 saat (Buffi çağrılı olmasa da süre akar). 57003 kalıcı cevher (+3 INT).
  3 rün slotu (runes_1..3). Max INT 189 = seviye 90'da 180 + cevher 9.
- 57002 Süre (3 gün) + öldüyse diriltir. 57004 kalıcı süre. 57005 cinsiyet değiştirme tılsımı.
- 72051 Buffi Tecrübe Yüzüğü (+%100 Buffi EXP, 1 saat). 71003 beceri sıfırlama Buffi'de de çalışır.
- Beceriler (5): Kutsama(94) / Yansıtma(95) / Ejderha Yardımı(96) / Hızlanma(110) / Saldırı Artışı(111).
  Max değerler (wiki): Kutsama +53% (apply 149), Yansıtma +82% (39), Ejderha Yardımı +26% (15),
  Hızlanma +26% (8) +21% (9), Saldırı +79 (64).
- Beceri yolu: 1→M1 = 17 beceri puanı; M1→G1 = 55× 98506 (şanslı) veya 1× 98508 (%100);
  G1→P = 10× 50513 (şanslı) veya 1× 98509 (%100).
- Evrim: Lv55 / Lv75 / Lv90, her biri malzeme + yang (25M / 50M / 100M). Evrimle INT artar.
  Max seviyede evrim yapmadan ilerlemez ("reached its maximum. Evolve it").
- Kostüm slotları: saç / zırh / silah / kuşak (bonus vermez, kozmetik). 4'ü + premium → beceri rengi seçimi.
- Seçenekler: `auto_unequip_buffi_pvp`, `auto_equip_buffi_pvm`, `hide_buffi`.

## 3. Anka2 durum / çakışmalar

- Item 57000–57002 Anka2'de **Kırmızı/Mavi/Yeşil sertifika** olarak kullanılıyor.
- Mob 34001 Anka2'de **Ateş Ankası** peti. (Referansta 34001 = Buffi.)
- Client'ta `__BUFFI_SUPPORT__` sabiti var ama sistem yok. CompanionWindow (pet/at/buffi sekmeli) Anka2'de yok.
- Sunucu: CPetActor/CPetSystem takip AI'ı tekrar kullanılabilir; WEAR_PASSIVE sonrası yeni wear slotları gerekiyor.

## 3b. Kararlar (kullanıcı, 2026-10-27)

- Boş vnum kullan: 57100 Buffi, 57101 cevher (24s), 57102 süre 3g, 57103 kalıcı cevher, 57104 kalıcı süre,
  57105 cinsiyet tılsımı, 57106 Buffi kitabı (ref 98506), 57107 usta kitabı (98508), 57108 usta taşı (98509),
  57109 Buffi EXP yüzüğü (ref 72051). NPC: 34900. Sertifikalar ve Ateş Ankası olduğu gibi kalır.
- UI: tam Companion penceresi (Pet / At / Buffi sekmeleri) — önce Buffi, sonra pet ve at taşınacak.

## 3c. Ek referans bulguları (`__LoadBuffiWindow` disasm)

- INT evrim tablosu: varsayılan 120, Evrim 1 → 140, Evrim 2 → 160, Evrim 3 → 180 (+3/cevher, max 189).
- Buffi sekmesi: mühür slotu, İsim/Seviye/Zeka satırları (+INT tablo tooltip'i), Tecrübe (4 küre), Süre göstergesi,
  Evrim + Süre butonları, 3 cevher slotu, Beceriler başlığı + 5 beceri slotu (+ butonu), bonus listesi,
  4 kostüm görünürlük düğmesi, çağrılı değilken "BUFFI_NO_SPAWN" paneli.

## 3d. Faz 1 durumu (2026-10-27)

Sunucu (`game/src/buffi_system.*`, `ENABLE_BUFFI_SYSTEM`): çağırma/takip, 5 buff'ı gerçek beceri formülüyle sahibine atma
(ComputeSkill + şaman animasyonu), PvP hasar sınırı 1, EXP payı (sahibin kill EXP'sinin %50'si), seviye/evrim sınırı,
beceri puanı (1 → M1), cevher/süre itemleri, quest flag kalıcılığı (`buffi.*`), ışınlanma/girişte geri çağırma.
Komutlar: `/buffi_summon`, `/buffi_unsummon`, `/buffi_info`, `/support_skill_power <i>`, `/support_add_ore <hücre>`,
`/support_full_duration`, GM: `/buffi_set <seviye> [puan] [evrim]`.

Client: `uicompanion.py` (Buffi sekmesi), game.py `SetSupport*` handler'ları, interfacemodule, genişletilmiş görev çubuğuna
Companion butonu, npc_wear (34900 kadın şaman / 34901 erkek şaman), ikonlar, item/mob proto, itemdesc.

Varsayımlar (referansta görünmeyen sunucu değerleri): EXP payı %50 ve ihtiyaç = exp_table[lv]/4; seviye sınırları 55/75/90.

## 3e. Faz 1 tamam (2026-10-28, kullanıcı oyun içinde doğruladı)

Buffi penceresi referans ekran görüntüsüne birebir hizalandı (Yoldaş, 415 px), "Buffi'yi Yükselt" sol alt butonu,
cevher yuvaları, 30 gün süre sınırı. Seçenekler: `/buffi_option <pvp> <pvm>` → `CBuffiManager::OnOwnerAttack`
(PvP'den önce otomatik çıkar, buff'lar kalır; canavara vurunca otomatik çağır); `hide_buffi` client
`CInstanceBase::IsHiddenByOptions` (34900/34901, hide_pet'ten ayrı).

## 3f. Elvion Aslan Buff'tan alınanlar (2026-10-28, hibrit karar)

Kaynak: `C:\Elvion\Source` (`ENABLE_ASLAN_BUFF_NPC_SYSTEM`). Kullanıcı kararı: Yoldaş penceresi + 5 beceri kalır,
Elvion'un sağlam parçaları katılır; `ENABLE_EXTENDED_SOCKETS` (soket 3→6) **alınmadı**.

- Kalıcılık: quest flag yerine `player.player_buffi` tablosu (`Source/Server/sql/player_buffi.sql`). Girişte tek satır
  okunur (PK), kayıt async `REPLACE INTO`. Eski karakterler bir kez `buffi.*` quest flag'lerinden taşınır.
  Seçenekler (`buffi.opt_pvp/opt_pvm`) ve `buffi.summoned` quest flag olarak kaldı.
- Beceri gösterimi: Elvion `HEADER_GC_BUFF_NPC_USE_SKILL` gibi `HEADER_GC_BUFFI_USE_SKILL = 188`
  (vid, skill, level) → client `NEW_UseSkill(skill, GetSkillMotionIndex(grade))`; eski `FUNC_SKILL` move paketi kaldırıldı.
- Kostüm görünümü: kostüm vnum'ları satırda (`costume_body/hair/weapon/sash`), NPC'ye `SetPart` ile basılır; client
  `InstanceBase::Create` npc_wear dalında 34900/34901 için sunucudan gelen parçaları kullanır (boşsa npc_wear varsayılanı).
- Alınmayanlar ve nedeni: Elvion buff gücünü ortak skill proto'sundaki `iq` değişkenine yazıyor (yan etki); bizde NPC
  kendi `POINT_IQ`'su ile cast ediyor. Sabit `srv1_player` DB adı, `PulseManager`, `[LS;..]` Elvion'a özel.
  `BUFF_EQUIPMENT` pencere tipi faz 2'de kostüm slotları için değerlendirilecek.

## 3g. Referans görünüm düzeltmeleri (2026-10-28, kullanıcı oyunda doğruladı)

- İsim etiketi "Lv N Buffi": sunucu Buffi için `addPacket.dwLevel` gönderir (seviye atlayınca `ViewReencode`); client
  text tail'de Buffi'yi PC gibi sayar (level her zaman görünür), isim rengi `NAMECOLOR_PC + empire`, pet isim
  formatı ("sahip - mob") ve GM vnum eki "(3)" Buffi için kapalı.
- Yoldaş tooltip'leri: zekâ tablosu (Evrim | Zeka), beceri tooltip'i (referans skilldesc.txt satırları + Buffi değeri),
  EXP küre tooltip'i (mevcut / gereken).
- 57100 tooltip'i: itemdesc yalnız "Buffinizi çağırır."; PvP uyarısı (kırmızı) + çıkarma notu, yeşil seviye, 5 beceri,
  süre bitince yenileme ipucu `uiToolTip.__AppendBuffiSealInfo` içinde (veri `uiCompanion.g_buffiState`). Sunucu girişte
  Buffi çağrılı değilse de verileri gönderip `SupportLeave` yollar. 57100 antiflag'lerine ANTI_SELL | ANTI_SAFEBOX eklendi
  (referanstaki 4 ikon).

## 4. Fazlar

1. Sunucu çekirdek: item tipi, wear slotu, çağırma/takip (şaman görünümü), 5 buff otomatik atma, PvP hasar sınırı,
   DB kalıcılığı (level/exp/int/evo/süre/rün/beceri), komutlar (§1).
2. Client: Buffi penceresi + server command handler'ları + 3 seçenek.
3. İlerleme: EXP/seviye, evrim, cevher/süre itemleri, beceri kitapları, cinsiyet, kostümler.
