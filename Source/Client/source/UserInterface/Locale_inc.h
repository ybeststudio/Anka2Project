#define LSS_SECURITY_KEY	"1234abcd5678efgh"
#define CLIENT_VERSION		"ext_180923"
#define APP_TITLE			"A N K A  |  U L T I M A T E  E D I T I O N  [ DX12 / x64 ]"		// APP_TITLE binary dosyasından yüklenir;

#define DEFAULT_DEFINE_YMIR									// Ymir tarafından belirlenen varsayılan tanımlama;
	#ifdef DEFAULT_DEFINE_YMIR
		#define LOCALE_SERVICE_EUROPE						// Avrupa sunucu hizmetine taşındı;
		#define ENABLE_ENERGY_SYSTEM						// Resmi enerji sistemi etkin;
		#define ENABLE_COSTUME_SYSTEM						// Resmi kostüm sistemi etkin;
		#define ENABLE_DRAGON_SOUL_SYSTEM					// Resmi ejderha ruhu (Dragon Soul) sistemi etkin;
		#define ENABLE_DRAGON_SOUL_7_SLOT					// Resmi 7. Ejderha Ruhu Slotu (Amethyst Slot)
		#define ENABLE_GEM_CONVERTER						// Resmi Gaya Donusturucu Sistemi (Official Gem Converter)
		#define ENABLE_GAYA_STACK							// Resmi Gaya Istifleme / Secim Kontrolleri (Official Gaya Stack & Selection System)
		#define ENABLE_GEM_BAG_ANTI_SELL_TOOLTIP			// Resmi Gaya Kesesi Satilamaz Bilgilendirme Tooltipi (Official Gem Bag Anti-Sell Tooltip)
		#define ENABLE_AURA_CLEAR							// Resmi Aura Deger Temizleme Sistemi (Official Aura Clear System)
		#define ENABLE_AURA_CLEAR_MESSAGE_WHEN_EQUIP		// Resmi Aura Giyiliyken Temizleme Engeli Uyarisi (Official Aura Clear Message When Equip)
		#define ENABLE_MOVE_COSTUME_ACCE_ATTR				// Resmi Kostum ve Kusak Bonus Aktarimi Sistemi (Item Combination System v26.1.14)
		#define ENABLE_NEW_EQUIPMENT_SYSTEM					// Resmi yeni ekipman sistemi etkin.
	#endif

/* === ifdef-audit: definesiz eklenmis custom paket sistemi (ON) === */
#define ENABLE_LEFT_SEAT_SYSTEM					// Sandalyeden kalkma / AFK sit-timer paketi (Packet.h)
/* === ifdef-audit end === */

#define ENABLE_FOG_FIX										// Resmi 16.2 sürümünden sis düzeltmesi;
#define ENABLE_ENB_MODE										// ENB modu (HD grafik) etkin;
#define ENABLE_ANTI_EXP										// Anti EXP sistemi etkin;
#define ENABLE_MOB_SCALE									// Yaratık ölçeklendirmesi etkin;
#define ENABLE_LEVEL_INT									// Seviye tipi byte'tan int'e taşındı (maks. seviye 300+);
#define ENABLE_GOLD_LIMIT									// Yang limiti 20kkk'a yükseltildi;
#define ENABLE_MDE_EFFECT									// 3D silahlar için MDE efekti;
#define ENABLE_PET_SYSTEM									// Evcil hayvan sistemi etkin;
#define ENABLE_DISCORD_RPC									// Discord RPC entegrasyonu etkin;
#define ENABLE_AUTO_REFINE									// Otomatik arıtma (auto refine) seçeneği etkin;
#define ENABLE_DICE_SYSTEM									// Zar sistemi etkin: Eğer yaratık kral veya boss ise ve partideysen, düşen eşya rastgele dağıtılır;
#define ENABLE_BLINK_ALERT									// Mesaj geldiğinde görev çubuğunda yanıp sönme uyarısı;
#define ENABLE_STACK_LIMIT									// Eşya yığın limiti 2000'e yükseltildi;
#define ENABLE_RENEWAL_CUBE									// Yenilenmiş küp sistemi etkin;
//#define ENABLE_FIX_MOBS_LAG									// Yaratık gecikme (lag) düzeltmesi etkin;
#define ENABLE_MAP_TELEPORT									// Yetkililer minimap'e tıklayarak ışınlanabilir;
#define ENABLE_MOUNT_SYSTEM									// Binek sistemi etkin;
#define ENABLE_OBJ_SCALLING									// Oyun içi nesne ölçeklendirme etkin;
#define ENABLE_FOXFS_ENCRYPT								// Fox FS şifreleme etkin;
#define ENABLE_SHOW_MOB_INFO								// Yaratık seviye ve agresiflik bilgisi gösterimi;
#define ENABLE_RENEWAL_QUEST								// Resmi sunuculardaki yeni görev sayfası;
#define ENABLE_CONFIG_MODULE								// Ayarların kaydedilmesi için yapılandırma modülü etkin;
#define ENABLE_HAIR_SPECULAR								// Saçlarda parlama (specular) etkin;
#define ENABLE_MOB_DROP_INFO								// Yaratık/taş düşürme penceresi etkin;
#define ENABLE_CHANGE_CHANNEL								// Kanal değiştirme seçeneği etkin;
#define ENABLE_MOUNT_PET_SKIN								// Binek ve pet kostüm (skin) sistemi etkin;
#define ENABLE_PENDANT_SYSTEM								// Resmi sürümdeki tılsım (pendant) sistemi etkin;
#define ENABLE_ATTBONUS_METIN								// Metin taşlarına saldırı bonusu sistemi etkin;
#define ENABLE_TEXTLINE_EMOJI								// Yazı satırlarında / ipuçlarında emoji desteği;
#define ENABLE_MESSENGER_TEAM								// Arkadaş listesindeki ekip sekmesi etkin;
#define ENABLE_DESTROY_DIALOG								// Eşyayı yere atarken yok etme seçeneği;
#define ENABLE_SORT_INVENTORY								// Envanter ve özel envanter sıralama sistemi etkin;
#define ENABLE_GRAPHIC_ON_OFF								// Grafik ayarlarını aç/kapat sistemi etkin;
#define ENABLE_PARTY_POSITION								// Parti üyeleri haritada görülebilir;
#define ENABLE_RENEWAL_SHOPEX								// Genişletilmiş, yenilenmiş ShopEx (çok sayfa) sistemi;
#define ENABLE_INSTANT_PICKUP								// Eşyaları yerden anında alma etkin;
#define ENABLE_VIEW_CHEST_DROP								// Sandık içeriğini görme özelliği etkin;
#define ENABLE_ATLAS_MARK_INFO								// Atlas üzerindeki işaret bilgisi etkin;
#define ENABLE_QUICK_SELL_ITEM								// Dükkanlarda hızlı satış seçeneği etkin;
#define ENABLE_MESSENGER_BLOCK								// Oyuncu engelleme sistemi etkin;
#define ENABLE_COINS_INVENTORY								// Envantere yeni para birimi eklendi;
#define ENABLE_EXTENDED_SAFEBOX								// Depo sayfası 3'ten 6'ya çıkarıldı;
#define ENABLE_RENEWAL_SWITCHBOT							// Yenilenmiş switchbot sistemi etkin;
#define ENABLE_RENEWAL_BOOK_NAME							// Yaratık/taşlardan düşen kitap isimleri görünür;
#define ENABLE_GUILD_RANK_SYSTEM							// Lonca sıralama panosu etkin;
#define ENABLE_LOAD_INDEX_BINARY							// Index dosyası binary olarak yüklenir;
#define ENABLE_CLIENT_PERFORMANCE							// İyileştirilmiş client performansı ve yeni playersettingmodule;
#define ENABLE_ETERPACK_OBSCURING							// EterPack anahtar gizleme (binary'de anahtarları gizler);
#define ENABLE_RENEWAL_DEAD_PACKET							// Zamanlayıcılı "burada yeniden başla / şehirde yeniden başla" sistemi;
#define __BL__DETAILS_UI__									// Resmiye yakın yeni bonus panosu;
#define ENABLE_KILL_STATISTICS								// Resmiye yakın yeni bonus panosu;
#define ENABLE_RENEWAL_SERVER_LIST							// Yenilenmiş sunucu listesi etkin;
#define ENABLE_HIDE_COSTUME_SYSTEM							// Kostüm gizleme sistemi etkin;
#define ENABLE_ACCE_COSTUME_SYSTEM							// Aksesuar kostüm sistemi etkin;
#define ENABLE_RENEWAL_MINIMAP_MARK							// Minimap'ta yeni NPC/yaratık/taş işaretleri;
#define ENABLE_TELEPORT_TO_A_FRIEND							// Arkadaşa ışınlanma sistemi etkin;
#define ENABLE_METIN_STONES_MINIMAP							// Minimap'ta metin taşları gösterimi;
#define ENABLE_MULTI_LANGUAGE_SYSTEM						// Çoklu dil sistemi etkin;
#define ENABLE_WEAPON_COSTUME_SYSTEM						// Silah kostümü sistemi etkin;
#define ENABLE_VIEW_TARGET_PLAYER_HP						// Hedef çubuğunda HP gösterimi;
#define ENABLE_VIEW_TARGET_DECIMAL_HP						// HP değerleri ondalıklı gösterim;
#define ENABLE_RENEWAL_CLIENT_VERSION						// Yeni client sürüm kontrol sistemi;
#define ENABLE_CANSEEHIDDENTHING_FOR_GM						// GM'ler görünmez karakterleri görebilir (normal veya GM görünmezliği fark etmez);
#define ENABLE_EXTENDED_WHISPER_DETAILS						// Özel mesajlarda genişletilmiş hedef bilgisi, ülke bayrağı desteği;
#define ENABLE_ENVIRONMENT_EFFECT_OPTION					// Ortam efekt ayarları etkin;
#define ENABLE_EXTEND_TIME_COSTUME_SYSTEM					// Kostüm süresi uzatma sistemi etkin.
#define ENABLE_FOV_OPTION									// Görüş alanı (Görüş Alanı) özelleştirmesini etkinleştir;
#define ENABLE_RENDER_TARGET								// Render hedefleme (render target) sistemi etkin;
#define ENABLE_ATTBONUS_BOSS								// Boss yaratıklara saldırı bonusu sistemi etkin;
#define ENABLE_HUMAN_RESIST_PVP								// Yarı insanlara saldırı bonusu sistemi etkin;
#define ENABLE_EVENT_MANAGER								// Etkinlik yöneticisi (otomatik event sistemi) etkin;
#define ENABLE_MULTI_TEXTLINE								// Çok satırlı yazı (multi textline) desteği etkin;
#define ENABLE_RENEWAL_AFFECT								// Yenilenmiş affect sistemi (1 yeni alt tip + affect ikonu);
#define ENABLE_AFFECTSHOWER_TOOLTIP_RENEWAL					// Resmi sunuculardaki gibi yenilenmis affect shower tooltip ve dinamik satir duzeni
#define ENABLE_AUTO_CLEAR_WHEN_LOGOUT						// Otomatik av sisteminde oturum kapatildiginda verilerin otomatik temizlenmesi
#define ENABLE_VISUALIZE_CUBE_COUNT_WITH_GOLD				// Kup yenileme arayuzunde uretim adet sayacinda Yang/Gaya miktarini dikkate alma
#define ENABLE_AE_GUILD_MARK_NAME_ALIGNMENT					// Arapca/RTL dillerinde lonca simgesi ve isim hizalamasini duzenleme
#define ENABLE_MULTILANGUAGE_CHATTING						// Coklu dil sohbet destegi (Farkli dil kod sayfalarinda ve klavye dillerinde mesajlasma)
#define ENABLE_SUNGMA_SCORE									// SungMa iradesi puani sistemi
#define ENABLE_MAX_ITEM_SUNGMA_SCORE						// Esyalarda maksimum SungMa puani sistemi
#define ENABLE_SUNGMA_SCORE_TOOLTIP							// Esya ipuclarinda SungMa iradesi puani gosterimi
#define ENABLE_MERCENARY_REWARD_FORMAT						// Parali asker odul formatlandirma sistemi
#define ENABLE_SET_RESET_POPUP_OFF							// Set etkisi kaldirma onay penceresi duzenlemesi
#define ENABLE_AE_ARROW_EVENT_SNOWFLAKE						// Ok etkinligi kar tanesi cubugu destegi
#define ENABLE_PRECISION_TOOLTIP_FIX						// Hassasiyet tooltip gosterim duzeltmesi
#define ENABLE_ZENBEAN_ERROR_MESSAGE						// Zen fasulyesi pozitif derece hata mesaji
#define ENABLE_LEVELUP_QUEST_IMPROVE						// Seviye atlama gorev ekrani iyilestirmesi
#define ENABLE_MONSTER_BACK_10TH_INGAME_EVENT				// 10. Yil Canavar Avi oyun ici etkinligi
#define ENABLE_PREMIUM_SEARCH_IMPROVE						// Ticaret cami arama gelistirmeleri
#define ENABLE_AUTO_HUNTING_IMPROVE							// Otomatik av gelistirmeleri
#define ENABLE_FORCE_EVENT									// Force event sistemi
#define ENABLE_FIX_BLINK_TOOLTIP							// Yanip sonen tooltip duzeltmesi
#define ENABLE_FIX_SYSERR_CANT_ATTACH_METIN_ITEM			// Metin tasi eklenememe syserr duzeltmesi
#define ENABLE_MSG_FISHING_WHILE_POLY						// Donusmus halde balik tutma mesaji
#define ENABLE_FIX_BUYING_ITEM_PREFIX						// Satin alma esya on ek duzeltmesi
#define ENABLE_DRAGONSOUL_TOOLTIP_PRIVSHOP_CREATE			// Pazar kurarken ejderha tasi tooltip destegi
#define ENABLE_SKILL_AFTER_EMOTION							// Duygu sonrasi beceri kullanimi
#define ENABLE_AFFECT_MALL_ATT_SPEED_ICON					// Nesne market saldiri hizi affect ikonu
#define ENABLE_LOGIN_WEB_PAGE								// Giris web sayfasi destegi
#define ENABLE_POPUP_NOTICE_IMPROVE							// Acilir bildirim duyuru gelistirmesi
#define ENABLE_EMPTY_APPLY_IMMUNE_FALL						// Bos immune fall bonus destegi
#define ENABLE_MYSTERY_BOX_EVENT_REWARD_SHOW				// Gizemli sandik etkinligi odul gosterimi
#define ENABLE_NPC_LOCATION_HELPER_SUMMER_SERVER			// Yaz sunucusu NPC konum yardimcisi
#define ENABLE_ADD_INFO_PETBAG_TOOLTIP						// Pet cantasi ek bilgi tooltip destegi
#define ENABLE_FIX_CRASH_DESTROY_ITEM_WHILE_REFINE			// Yukseltme sirasinda esya yok olma cokme duzeltmesi
#define ENABLE_PETBAG_TRADABLE								// Pet cantasi ticaret yapilabilirligi
#define ENABLE_STATUS_EFFECT_SHOWER							// Durum efekti gosterim sistemi
#define ENABLE_MSG_DRAGON_SOUL_WHILE_ATTACK					// Saldiri esnasinda ejderha tasi mesaji
#define ENABLE_GAME_OPTION_RENEWAL							// Yenilenmis oyun secenekleri sistemi
#define ENABLE_MESSENGER_RENEWAL							// Yenilenmis arkadas/mesajlasma sistemi
#define ENABLE_FIX_MESSAGE_MOVE_INVALID_TYPE_ITEM			// Gecersiz tipteki esya tasima mesaj duzeltmesi
#define ENABLE_MSG_AUTO_HUNTING_WITHOUT_WEAPON				// Silahsiz otomatik av baslatma uyari mesaji
#define ENABLE_APPEAR_CHANGE_LOOK_TOOLTIP					// Yansitma gorunumu tooltip destegi
#define ENABLE_ADJUST_TOOLTIP_POSITION						// Tooltip pozisyon hizalama duzeltmesi
#define ENABLE_GREEDY_ROOM									// Acgozlu Odasi (Greedy Room) zindan sistemi
#define ENABLE_SELECT_REWARD_BOX							// Secim odul sandigi arayuz sistemi
#define ENABLE_FIX_SPORTS_MATCH_DESC						// Spor maci aciklama duzeltmesi
#define ENABLE_GEM_BAG_ANTI_SELL_TOOLTIP					// Gaya cantasi satilamaz tooltip uyarisi
#define ENABLE_PET_VERIFY_POPUP_CLOSE						// Pet dogrulama onay penceresi kapatma duzeltmesi
#define ENABLE_FIX_INVALID_PLAYER_GAUGE						// Gecersiz oyuncu gostergesi duzeltmesi
#define ENABLE_CHECK_ITEM_MOVE_EXCEPTION					// Esya tasima istisna kontrolu
#define ENABLE_MARA_DUNGEON									// Mara zindani sistemi
#define ENABLE_IMPROVE_EMPIRE_PRIV							// Gelismis imparatorluk ayricaligi duyurusu
#define ENABLE_PREMIUM_MEMBERSHIP_AND_NEW_VOUCHER			// Premium uyelik ve yeni kupon sistemi
#define ENABLE_EASTER_EGG_RENEWAL							// Paskalya yumurtasi etkinligi yenilemesi
#define ENABLE_FIX_OPEN_PRIVATESHOP_WITH_SAFEBOX			// Depo acikken pazar acilmasi duzeltmesi
#define ENABLE_RAMADAN_EVENT_2025							// Ramazan etkinligi 2025 destegi
#define ENABLE_GUILD_LAND_RENEWAL							// Lonca arazisi yenileme sistemi
#define ENABLE_FIX_PRIVATE_SHOP_SEARCH_PAGING				// Ticaret cami arama sayfalama duzeltmesi
#define ENABLE_REMOVE_UNUSED_FLAG							// Kullanilmayan bayrak optimizasyonu
#define ENABLE_NEW_WORLD_4TH_SHOES_SOCKET					// Yeni dunya 4. ayakkabi cevher yuvasi destegi
#define ENABLE_PAID_CHARACTER								// Ucretli karakter slotu destegi
#define ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL				// Gelistirilebilir pet sampiyon seviyesi destegi
#define ENABLE_USER_REPORT_SYSTEM							// Oyuncu bildirim sistemi
#define ENABLE_MT_5439										// Resmi hata duzeltmesi MT-5439
#define ENABLE_GROUND_PICK_IMPROVE							// Yerden esya toplama iyilestirmesi
#define ENABLE_MESSAGE_CANNOT_UNEQUIP_IN_BATTLE				// Savasta esya cikaramama uyari mesaji
#define ENABLE_CLIENT_VERSION_RENEWAL						// Istemci surum kontrol yenilemesi
#define ENABLE_MESSAGE_DRAGON_SOUL_EXTRACT_FAIL				// Ejderha tasi arindirma basarisizlik mesaji
#define ENABLE_FIX_WHISPER_MAX_LENGTH						// Fisilti maksimum karakter uzunlugu duzeltmesi
#define ENABLE_MOONLIGHT_VALLEY								// Ayisigi Vadisi (Moonlight Valley) harita sistemi
#define ENABLE_AUTO_HUNTING_SAVE_FILE_NAME_CHANGE			// Otomatik av kayit dosyasi adi guncellemesi
#define ENABLE_SHOP_OPEN_FIX								// Pazar acilis durumu duzeltmesi
#define ENABLE_PARTY_REQUEST_TO_OTHER_SERVER				// Diger sunucuya grup istegi gonderimi
#define ENABLE_PARTY_CONTAINER_KEY_CHANGE					// Grup konteyner tus degisimi destegi
#define ENABLE_INGAME_EVENT_LEFT_TIME_SHOW					// Oyun ici etkinlik kalan sure gosterimi
#define ENABLE_MINI_GAME_EVENT_START_CHECK					// Mini oyun etkinlik baslangic kontrolu
#define ENABLE_ITEM_DROP_DUPLICATE_CHECK					// Esya yere dusme ciftleme kontrolu
#define ENABLE_FIX_BELT_EXPAND_RENDER						// Kemer genisletme cizim duzeltmesi
#define ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH				// Ortak pack dil yolu degisikligi
#define ENABLE_SHARED_PACK_CHANGE_MALL_ICON					// Ortak pack nesne market ikonu degisikligi
#define ENABLE_MOVE_TO_WOLF_MAN_HOMETOWN					// Lycan koyune isinlanma destegi
#define ENABLE_MAP_ATTR_BANSHOP								// Harita pazar engeli (banshop) attr destegi
#define ENABLE_GUILD_DRAGONLAIR_ENTRY_RANK_IMPROVE			// Lonca ejderha ini siralama gelistirmesi
#define ENABLE_MERCENARTY_MAX_ACTIVE_AFFECT_TIME			// Parali asker maksimum aktif etki suresi destegi
#define ENABLE_GUILD_BUILD_EMOTION_ACTIVE					// Lonca binasi insasinda duygu aktifligi
#define ENABLE_SUNGMAHEE_TOOLTIP_SHOW						// SungMa iradesi tooltip gosterimi
#define ENABLE_GUILD_APPLICATION_BUTTON_HIDE				// Lonca basvuru butonu gizleme destegi
#define ENABLE_DAMAGE_PRINT_IMPROVE							// Hasar yazdirma gelistirmesi
#define ENABLE_CRAFT_INTEGRATE								// Zanaat (craft) entegrasyon sistemi
#define ENABLE_PASSIVE_ATTR_TOOLTIP							// Pasif efsun tooltip destegi
#define ENABLE_PRIVATE_SHOP_SEARCH_CHECK					// Ticaret cami arama kontrolu
#define ENABLE_DEACTIVATE_PET_EMPTY_EXP_TOOLTIP				// Pet pasifken bos tecrube tooltip destegi
#define ENABLE_REMOTE_CALL									// Uzaktan cagri / iletisim sistemi
#define ENABLE_PET_SKILL_UPGRADE_ISSUE_FIX					// Pet beceri yukseltme sorunu duzeltmesi
#define ENABLE_UI_TITLEBAR_TEXT_NOT_COVERED					// Baslik cubugu metin kapanmama duzeltmesi
#define ENABLE_ALT_WHILE_KEYSETTING							// Tus atama sirasinda Alt tusu destegi
#define ENABLE_QUICKSLOT_IMPROVE							// Yenilenmis hizli erisim slotu sistemi
#define ENABLE_PARTY_WHITE_DRAGON_DUNGEON					// Beyaz Ejderha grup zindani sistemi
#define ENABLE_NEW_USER_RESTRICTION							// Yeni oyuncu kisitlama sistemi
#define ENABLE_SKILLBAR_ICON_CHANGE							// Beceri cubugu simge degisim destegi
#define ENABLE_FIX_PET_HATCHING_UI_INCUBATE					// Pet kulucka arayuzu duzeltmesi
#define ENABLE_USING_BUNDLE_ITEM_SLOT_FIX					// Paket esya slot kullanim duzeltmesi
#define ENABLE_MULTISLOT_ALL_TOOLTIP						// Coklu slot hepsi icin tooltip destegi
#define ENABLE_CHANGE_GUILD_TIMER_MESSAGE					// Lonca sure degisimi uyari mesaji
#define ENABLE_REPEATABLE_QUEST								// Tekrarlanabilir gorevler sistemi
#define ENABLE_LIMIT_REFINE_TOOLTIP_HEIGHT					// Yukseltme tooltip yukseklik sinirlamasi
#define ENABLE_SINGIJEON_POLY_CANNOT_REMOVE_DEBUFF			// Singijeon donusumde debuff silinmeme kontrolu
#define ENABLE_SET_ITEM										// Resmi Set Efekti Sistemi (Official Set Item System)
#define ENABLE_OFFLINE_MESSAGE								// Oyuncu çevrimdışıysa offline mesaj bırakma seçeneği etkin;
#define ENABLE_RESTART_INSTANT								// 500.000 Yang karşılığında anında yeniden başlatma etkin;
#define ENABLE_RENEWAL_OX_EVENT								// Resmi sunuculara benzer yenilenmiş OX etkinliği;
#define ENABLE_MULTI_FARM_BLOCK								// 2'den fazla istemcide (client) farm yapılmasını engelle;
#define ENABLE_SPECIAL_INVENTORY							// Kitaplar, yükseltmeler vb. için özel envanter etkin;
#define ENABLE_STONE_SCALE_OPTION							// Metin taşları için ölçeklendirme özelleştirmesi etkin;
#define ENABLE_MAINTENANCE_SYSTEM							// Oyun içinde bakım bilgisi gösterim sistemi etkin;
#define ENABLE_WIKI_SYSTEM									// Oyun içi wiki sistemi etkin;
#define ENABLE_PICKUP_ITEM_EFFECT							// Envanter / özel envantere alınan eşyalar için efekt gösterimi;
#define ENABLE_RENEWAL_OFFLINESHOP							// Yeni nesil offlineshop sistemi etkin;
#define ENABLE_RENEWAL_TEAM_AFFECT							// Takım etki (logo) sistemi yenilenmiş haliyle etkin;
#define ENABLE_RENEWAL_TEXT_SHADOW							// İsim metinlerine gölge efekti seçeneği etkin;
#define ENABLE_SLOT_MARKING_SYSTEM							// Resmi sürümlerdeki gibi slot işaretleme sistemi etkin;
#define ENABLE_RENEWAL_SKILL_SELECT							// Becerileri seçmek için yeni pencere sistemi etkin;
#define ENABLE_BONUS_COSTUME_SYSTEM							// Kostümlerde bonus ekleme ve değiştirme sistemi etkin;
#define ENABLE_RENEWAL_SPECIAL_CHAT							// Yang, eşya vb. için özel yenilenmiş sohbet sistemi;
#define ENABLE_LARGE_DYNAMIC_PACKET							// Büyük dinamik paket boyutu desteği etkin;
#define ENABLE_CLIENT_LOCALE_STRING							// Client'tan dil dizeleri (locale string) yükleme sistemi etkin;
#define ENABLE_GUILD_LEADER_TEXTAIL							// Lonca liderlerinin isimlerinde [Leader] etiketi gösterimi etkin;
#define ENABLE_BOSS_EFFECT_OVER_HEAD						// Boss'ların kafasının üzerinde özel efekt gösterimi;
#define ENABLE_MONSTER_TARGET_ELEMENT						// Her yaratık için hedef element bilgisi gösterimi;
#define ENABLE_AUTOMATIC_PICK_UP_SYSTEM						// Otomatik eşya toplama sistemi etkin;
#define ENABLE_OFFLINESHOP_SEARCH_SYSTEM					// Yenilenmiş offlineshop için arama sistemi etkin;
#define ENABLE_PYTHON_DYNAMIC_MODULE_NAME					// Python dinamik modül isimlendirmesi (ekstra güvenlik katmanı) etkin.
#define ENABLE_SKILL_COLOR_SYSTEM							// Beceri (skill) renk değiştirme sistemi etkin;
	#ifdef ENABLE_SKILL_COLOR_SYSTEM
		#define ENABLE_5LAYER_SKILL_COLOR					// Skill renkleri için 5 katman desteği etkin.
	#endif
#define ENABLE_CLIP_MASKING                     			// Resmi sürümdeki clip masking (görsel kırpma) özelliğini etkinleştir
#define ENABLE_UTF8_ENCODING                    			// Client’ta UTF-8 kodlamasını ana kodlama olarak etkinleştir (Windows-1254)
#define ENABLE_BUFFI_SYSTEM							// Buffi (destek þamaný): beceri paketi + kostüm parçalarý
	#ifdef ENABLE_BUFFI_SYSTEM
		#define ENABLE_BUFFI_EVOLUTION					// Buffi evolution window
		#define ENABLE_BUFFI_SKILL_BOOK					// Buffi skill books / soul stones
		#define ENABLE_BUFFI_CHANGE_SEX					// Buffi sex change talisman
	#endif
#define ENABLE_SKILL_UPGRADE							// Skill Upgrader window (/upgrade_skill)
#define ENABLE_NPC_WEAR_ITEM                    			// NPC’lerin eşya giyebilmesini etkinleştir
#define ENABLE_HUNTING_SYSTEM                   			// Özgün görevlerle avcılık (hunting) sistemini etkinleştir
#define ENABLE_EMOTICONS_SYSTEM                 			// Sohbet ve fısılda (whisper) emoji sistemini etkinleştir
#define ENABLE_GROWTH_PET_SYSTEM                			// Resmi sürümdeki gelişim (growth) pet sistemini etkinleştir
	#ifdef ENABLE_GROWTH_PET_SYSTEM 
		#define ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL       // Resmi sürümdeki gelişim (growth) pet şampiyon sistemini etkinleştir
	#endif
#define ENABLE_CHANGE_LOOK_SYSTEM               			// Eşya görünümü değiştirme (change look) sistemini etkinleştir
#define ENABLE_AURA_COSTUME_SYSTEM              			// Aura kostüm sistemini etkinleştir
#define ENABLE_PYTHON_EVENT_FUNCTIONS           			// Resmi sürümdeki gibi Python event fonksiyonlarını etkinleştir
#define ENABLE_RENEWAL_TELEPORT_SYSTEM          			// Yenilenmiş ışınlanma (teleport) sistemini etkinleştir
#define ENABLE_INVENTORY_EXPANSION_SYSTEM       			// Resmi sürümdeki envanter genişletme sistemini etkinleştir
#define ENABLE_SEALBIND_SYSTEM								// Resmi ruha baglama / muhurleme sistemi
#define ENABLE_UN_SEAL_SCROLL_PLUS							// Ruh Bagi Ayirma Parssomeni+ sistemi
//#define ENABLE_MORE_FPS										// FPS sınırı yükseltme
#define ENABLE_GM_INV_EFFECT         						// Resmi GM /inv efekt güncellemesi
#define ENABLE_MINIMAP_SMOOTH_ZOOM             				// Minimap yumuşak yakınlaştırma sistemi
#define ENABLE_OFFICAL_FEATURES								// Resmi özellikleri etkinleştir
#define WJ_SHOW_NPC_QUEST_NAME              				// NPC Görev İsimlerini Göster
#define ENABLE_ATLAS_SCALE									// Harita atlası ölçekleme
#define ENABLE_FALLING_EFFECT_SYSTEM						// Yere düşen itemlere efekt verme sistemi
#define ENABLE_STANDING_MOUNT								// Ayakta kullanılan binek sistemini etkinleştir
	#ifdef ENABLE_STANDING_MOUNT
		#define STANDING_MOUNT_VNUM_1 40003 				// Turbo Sörf Tahtası
		#define STANDING_MOUNT_VNUM_2 40004 				// Wukong'un Fırtınası
		#define STANDING_MOUNT_VNUM_3 40005 				// Wukong'un Gürlemesi
		#define SURFBOARD STANDING_MOUNT_VNUM_1 			// Eski sistemlerle uyumluluk için alias
	#endif
#define ENABLE_RIDING_EXTENDED 								// Resmi At seviyesi genişletildi
#define ENABLE_SPIRIT_STONE_READING							// Ruh Taşı Okuma sistemi
#define ENABLE_SKILL_BOOK_READING							// Beceri Kitabı Okuma sistemi
#define __AUTO_HUNT__										// Otomatik Av sistemi
#define ENABLE_AUTO_SELL_SYSTEM								// Otomatik item satmayı sistemini etkinleştir;
#define ENABLE_STYLE_ATTRIBUTE_SYSTEM      					// Seçmeli özel kadim efsunu sistemini  etkinleştir;
#define ENABLE_GM_MOB_VNUM_DISPLAY   						// GM'lere (oyun yöneticilerine) mobların VNUM değerini oyun içi veya log çıktısında gösterir
#define ENABLE_RENDERING_ONLY_IN_AREA						// Harita bölgesi için ağır efekt kullanımını düzeltir
//#define ENABLE_RENDERING_ONLY_IN_AREA_V2					// Harita yükleme optimizasyonu (V2) - yürürken donma/kasma sebebi devre dışı
#define ENABLE_NEW_SHINING_EFFEKT							// Yeni nesil Resmi parlama efekt sistemini açar
	#ifdef ENABLE_NEW_SHINING_EFFEKT
		#define ENABLE_LVL115_ARMOR_EFFECT					// 115 seviye zırhlara özel parlama efektini aktif eder
		#define ENABLE_LVL96_WEAPON_EFFECT					// 96 seviye silahlara özel parlama efektini aktif eder
		#define ENABLE_LVL96_ARMOR_EFFECT					// 96 seviye zırhlara özel parlama efektini aktif eder

		//#define USE_WEAPON_COSTUME_WITH_EFFECT			// Silah kostümlerinin de geliştirme/parlama efektini göstermesini sağlar
		//#define USE_BODY_COSTUME_WITH_EFFECT				// Zırh kostümlerinin de geliştirme/parlama efektini göstermesini sağlar
	#endif
#define ENABLE_PITTY_REFINE                    				// Yeni nesil yükseltme sistemi
#define ENABLE_HYPERLINK_ITEM_ICON                  		// Görevlerde nesne simgesi göstermeyi etkinleştirir
#define ENABLE_ITEMSHOP
	#ifdef ENABLE_ITEMSHOP									// Oyun içi nesne market sistemi etkinleştirir;
		#define USE_ITEMSHOP_RENEWED						// Oyun içi nesne market sistemi yenilemeyi etkinleştirir;
	#endif
#define ENABLE_BIOLOGIST_SYSTEM								// Biyolog sistemini etkinleştir
#define ENABLE_YOHARA_SYSTEM          						// Yohara kıtası ve SungMa tabanlı sistemleri etkinleştirir
	#ifdef ENABLE_YOHARA_SYSTEM
		#define ENABLE_CONQUEROR_LEVEL						// Şampiyon seviyesi sistemini etkinleştir;
		#define ENABLE_CONQUEROR_LEVEL_UPDATE				// Şampiyon seviyesi anında yenilemeyi etkinleştir;
		#define ENABLE_PASSIVE_SYSTEM						// Resmi Kalıntı Bonusu sistemi etkinleştirir;
		#define ENABLE_NINETH_SKILL							// 9. becerileri etkinleştir
		#define ENABLE_PVP_BALANCE							// Yeni skill animasyonları, resmi oyun gibi
	#endif
#define ENABLE_DUNGEON_RENEWAL_SYSTEM      					// Zindan sistemini tümüyle etkinleştirir
	#ifdef ENABLE_DUNGEON_RENEWAL_SYSTEM
		#define ENABLE_DUNGEON_INFO							// Yeni zindan (dungeon) fonksiyonları etkin;
		#define __DUNGEON_INFO__               				// Yeni tip can yenilenme (regen) sistemini etkinleştir
		#define ENABLE_NEW_DUNGEON_LIB						// Yeni zindan (dungeon) fonksiyonları etkin;
		#define ENABLE_TRACK_WINDOW    						// Yeni zindan bilgi (takip) sistemini etkinleştir
		#define ENABLE_ULTIMATE_REGEN               		// Yeni tip can yenilenme (regen) sistemini etkinleştir
	#endif
#define ENABLE_DUNGEON_MAPS_SYSTEM      					// Tüm yeni/yenilenmiş zindan ve harita altyapısını etkinleştirir
	#ifdef ENABLE_DUNGEON_MAPS_SYSTEM
		// Yohara Zindanları
		#define ENABLE_SUNG_MAHI_TOWER    					// SungMa İstilası zindanı
		#define ENABLE_WHITE_DRAGON       					// Alastor (Kuzey Rüzgarı Derinlikleri)
			#ifdef ENABLE_WHITE_DRAGON
				//#define ENABLE_WHITE_DRAGON_EX 			// Alastor canavar / event uzantıları
			#endif
		#define ENABLE_QUEEN_NETHIS       					// Yılan Tapınağı (Kraliçe Nethis)
		#define ENABLE_MELEY_LAIR_DUNGEON     				// Meley'in İni haritasını etkinleştir
			#ifdef ENABLE_MELEY_LAIR_DUNGEON
				#define MELEY_LAIR_DUNGEON_STATUE 6230 		// Meley'in İni içerisinde kullanılan ana heykel (statue) NPC vnum'u
			#endif
		#define ENABLE_ZODIAC_MISSION						// Zodyak Tapınağı görev sistemi
		#define ENABLE_DEFENSAWESHIP             			// Resmi sürümdeki Gemi Savunma Zindanı’nı etkinleştir
		#define ENABLE_OCHAO_TEMPLE_SYSTEM   				// Ochao Tapınağı ve Büyülü Orman zindanını etkinleştirir
	#endif
#define INSIDE_RENDER
#define ENABLE_BATTLE_PASS                      			// Yenileme savaş bileti sistemini etkinleştir;
#define ENABLE_WHISPER_CLOSE_SHORTCUT        				// Özel mesaj (whisper) penceresini kısayol ile hızlıca kapatma özelliğini aktif eder
#define ENABLE_AFFECT_BUFF_REMOVE            				// Karakter üzerindeki aktif buff / effect’lerin kaldırılabilmesini sağlar
#define LINK_IN_CHAT 										// Sohbet paneline bağlantı (link) eklemeyi etkinleştir;
#define ENABLE_REMOTE_SHOP_SYSTEM							// Uzaktan Market sistemini  etkinleştir;
#define ENABLE_SHOP_REPURCHASE								// Resmi Geri Alim Marketi Sistemi (Repurchase Shop System v26.1.14)
#define ENABLE_CHANNEL_LIST									// Kanal listesi durum gosterim sistemi (v26.1.14)
#define ENABLE_FLEXIBLE_WIDTH_BUTTON						// Resmi dinamik genislikli buton sistemi (FlexibleWidthButton v26.1.14)
#define ENABLE_YIN_AND_YANG_ATTR							// Yin ve Yang Efsun Sistemi (v26.1.14)
#define ENABLE_UI_TAB_FOCUS_SYSTEM							// Resmi arayuz Tab / odaklanma navigasyon sistemi (FocusNavigator v26.1.14)
#define ENABLE_QUEST_WIDTH_EXPANSION						// Resmi genisletilmis gorev dialog sistemi (v26.1.14)
#define ENABLE_SLOT_CHAR_ENLARGEMENT						// Resmi slot karakter buyutme sistemi (v26.1.14)
#define ENABLE_CANNOT_MINING_IF_POLY						// Resmi donusmus / binekli / kazmasiz madencilik engeli (v26.1.14)
#define ENABLE_COUNTLINE_RENDER_AFTER_UI					// Resmi UI sonrasi sayac cizimi (v26.1.14)
#define ENABLE_RENDER_COUNTLINE_ATTACHED_MOUSE				// Resmi fareye bagli sayac cizimi (v26.1.14)
#define ENABLE_GET_ITEM_DATA_CHECK							// Resmi nesne veri dogrulama kontrolu (v26.1.14)
#define ENABLE_LOCK_SLOT_WHEN_SHOP_BUILDING					// Resmi pazar kurarken envanter slot kilitleme sistemi (v26.1.14)
#define ENABLE_WRONG_SKILL_COOLTIME_SLOT					// Resmi skill bekleme suresi slot uyumsuzlugu duzeltmesi (v26.1.14)
#define NAMECOLOR_BOSS_CLIENT								// Patron isim rengini değiştirir
#define ENABLE_ADVANCED_GAME_OPTIONS						// Gelişmiş oyun seçenekleri etkinleştir;
#define ENABLE_EQUIPMENT_HAND_EFFECT						// Karakterin sağ ve sol eline takılan ekipmanlar için özel efekt desteği sağlar.
#define ENABLE_RESP_SYSTEM									// Patron ve Metin ışınlanma sistemini etkinleştirir
#define ENABLE_COLLECT_WINDOW								// Koleksiyon sistemine ait GUI (pencere) arayüzünü aktif eder
#define ENABLE_COLLECTIONS_SYSTEM							// Koleksiyon (Collections) sisteminin tamamını aktif eder
#define ENABLE_NEW_ITEM_TYPE_GACHA							// Gacha sistemi için yeni sandık / item tipini aktif eder
#define ENABLE_GAYA_SYSTEM                         			// Gaya sistemini (Gaya para birimi, NPC ve takas mekanikleri) aktif eder
#define ENABLE_RANKING										// Rank sıralama sistemi
#define ENABLE_MAGIC_REDUCTION_SYSTEM						// Büyü direnci & bozma bonusu etkinleştir;
#define ENABLE_NEW_BONUS_SYSTEM								// Patronlara karşı bonusu etkinleştir;
#define ENABLE_AVG_PVM										// Yüksek ortalama zarar bonusu etkinleştir;
#define ENABLE_EXPRESSING_EMOTION							// Yeni dans ve emoji sistemini  etkinleştir;
#define ENABLE_STATUS_UP_RENEWAL							// Hızlı statü sistemi etkinleştir;
#define ENABLE_BINARY_SERVERINFO							// Sunucu bilgilerini binary üzerinden aktarma
#define ENABLE_TITLE_SYSTEM									// Resmi Ünvan Sistemi etkinleştirir;
#define ENABLE_UI_DEBUG_WINDOW								// Oyun içi UI debug penceresi etkinleştirir;
/*#define ENABLE_WOLFMAN_CHARACTER							// Lycan (Wolfman) karakterini etkinleştirir; Ama tam ekli değil açmayın.
	#ifdef ENABLE_WOLFMAN_CHARACTER
		//#define DISABLE_WOLFMAN_ON_CREATE					// Lycan (Wolfman) karakterini python üzerinden açma/kapatma'yı etkinleştirir;
#endif*/
#define ENABLE_EVENT_SYSTEM									// Etkinlik yöneticisini (otomatik ve zamanlı etkinlikler) aktif eder
	#ifdef ENABLE_EVENT_SYSTEM
		#define ENABLE_MINI_GAME_OKEY						// Rumi (Okey kartları) mini oyun sistemini aktif eder
		#define ENABLE_EVENT_BANNER_FLAG					// Etkinlikler için oyun içi banner / bayrak gösterim sistemini aktif eder
		#define ENABLE_SOUL_ROULETTE_SYSTEM					// Kan Ritüeli (Soul Roulette) mini oyun / etkinlik sistemini aktif eder
		#define ENABLE_FISH_GAME                    		// Resmi sunuculardaki yeni balık tutma mini oyununu etkinleştir
		#define ENABLE_FISH_EVENT_SYSTEM					// Resmi yeni balık tutma mini oyununu aktif eder
		#define ENABLE_MINI_GAME							// Resmi sunuculardaki yeni mini oyunları etkinleştir
		#define ENABLE_ATTENDANCE_EVENT						// Canavar Avı (Attendance) etkinliğini ve ödül mekanizmasını aktif eder
		#define ENABLE_MINI_GAME_CATCH_KING					// Kralı Yakala (Catch the King) kart mini oyununu aktif eder
		#define ENABLE_HALLOWEEN_EVENT_SYSTEM				// Cadılar Bayramı (Halloween) etkinlik sistemini aktif eder
		#define ENABLE_STONE_EVENT							// Metin Taşı kesme / sıralama etkinlik sistemini aktif eder
		#define ENABLE_SOCCER_BALL_EVENT					// Futbol Topu etkinliğini ve ilgili oyun mekaniklerini aktif eder
		#define ENABLE_WORD_GAME_EVENT						// Kelime oyunu etkinliğini aktif eder
	#endif
/*** TAMAMLANMAMIŞ SİSTEMLER (YAKINDA) ***/
#define ENABLE_GUILDRENEWAL_SYSTEM							// Gelişmiş Resmi gibi lonca altyapısı için çekirdek yenileme sistemi
	#ifdef ENABLE_GUILDRENEWAL_SYSTEM	
		#define ENABLE_GUILD_MARK_RENEWAL					// Lonca logo (mark) yükleme ve güncelleme iş akışını etkinleştirir
		#define ENABLE_SECOND_GUILDRENEWAL_SYSTEM			// Ek modüllerle genişletilmiş ikinci lonca yenileme katmanı
		#define ENABLE_NEW_WAR_OPTIONS						// Tur, puan ve süre bazlı modern lonca savaşı seçenekleri
		#define ENABLE_EXTENDED_GUILD_LEVEL					// Lonca seviye kapasitesinin 30'a çıkarılması
		//#define ENABLE_MEDAL_OF_HONOR						// Lonca ekonomisi için yeni para birimi (Onur Madalyası)
		#define ENABLE_GUILD_DONATE_ATTENDANCE				// Günlük lonca bağışı ve katılım takip sistemi
		#define ENABLE_GUILD_WAR_SCORE						// Lonca savaş istatistiklerinin detaylı skor ekranı
		#define ENABLE_GUILD_LAND_INFO						// Lonca arazilerine ait detaylı bilgi arayüzü
		#define ENABLE_GUILDBANK_LOG						// Lonca banka ve işlem kayıtlarının günlük sistemi
		#define ENABLE_EXTENDED_RENEWAL_FEATURES			// Lonca lideri devri ve arazi silme yönetimi gibi ek yenileme özellikleri
		#define ENABLE_GUILDWAR_BUTTON						// Arayüzde Lonca Savaşı erişim düğmesini aktif eder
		//#define ENABLE_GUILD_REQUEST						// Lonca istek/başvuru altyapısı (eksik modüller nedeniyle devre dışı)
		#define ENABLE_NEW_WORLD_GUILDRENEWAL				// Gelişmiş lonca yenileme/güncelleme sistemi (sunucu tarafı eksik modüller)
		//#define ENABLE_GUILD_RANKING						// Loncalar arası sıralama sistemi (tam entegre edilmediği için kapalı)
		//#define ENABLE_GUILDSTORAGE_SYSTEM     			// Loncanın ortak deposu: üyelerin paylaşımlı eşya deposu sistemi
	#endif
/*** TAMAMLANMAMIŞ SİSTEMLER (YAKINDA) ***/

/*** GRAFİK AYARLARI & DÜZELTMELER***/
#define ANK_SYSTEM_GRAPHICS
#ifdef ANK_SYSTEM_GRAPHICS
	#define ANKIRA_TRANSPARENCY_BUILDING_FIX
	#define ANKIRA_IMPROVE_ENTITY_FREEZE
	#define ANKIRA_BLOOD_EFFECT
	#define ANKIRA_GPU_LEAF_PLACEMENT
	#define ANKIRA_GPU_TREE_WIND
	#define ANKIRA_TREE_DYNAMIC_LIGHT
	#define ANKIRA_DEATH_CAMERA
	#define ANKIRA_WATER_REFLECTION
	#define ANKIRA_UNDERWATER_CAMERA
	#define ANKIRA_SHARPNESS
	#define ANKIRA_DYNAMIC_LIGHT
	#define ANKIRA_SPECULAR
	#define ANKIRA_ENVIRONMENT_EFFECT_OPTION
	#define ANKIRA_GRAPHICS_SOUND
#endif
#define ENABLE_DIRECTX9_UPDATE
#define DirectX9Ex
#define DIRECTX9
#if defined(DIRECTX9)
	#define D3DLIGHT8					D3DLIGHT9
	#define D3DMATERIAL8				D3DMATERIAL9
	#define IDirect3DVertexBuffer8		IDirect3DVertexBuffer9
	#define LPDIRECT3DTEXTURE8			LPDIRECT3DTEXTURE9
	#define LPDIRECT3DSURFACE8			LPDIRECT3DSURFACE9
	#define D3DVIEWPORT8				D3DVIEWPORT9
	#define LPDIRECT3DDEVICE8			LPDIRECT3DDEVICE9
	#define LPDIRECT3DVERTEXBUFFER8		LPDIRECT3DVERTEXBUFFER9
	#define LPDIRECT3DINDEXBUFFER8		LPDIRECT3DINDEXBUFFER9
	#define DXLOCKTYPE					(void**)
	#define D3DVERTEXELEMENT8			D3DVERTEXELEMENT9
	#define LPDIRECT3DBASETEXTURE8		LPDIRECT3DBASETEXTURE9
	#define LPDIRECT3DPIXELSHADER8		LPDIRECT3DPIXELSHADER9
	#define D3DADAPTER_IDENTIFIER8		D3DADAPTER_IDENTIFIER9
	#define IDirect3D8					IDirect3D9
	#define IDirect3DDevice8			IDirect3DDevice9
	#define D3DCAPS8					D3DCAPS9
	#define LPDIRECT3D8					LPDIRECT3D9
	#define D3DCAPS2_CANRENDERWINDOWED	DDCAPS2_CANRENDERWINDOWED
	#define IDirect3DTexture8			IDirect3DTexture9
	#define Direct3DCreate8				Direct3DCreate9
	#define IDirect3DSurface8			IDirect3DSurface9
#else
	#define DXLOCKTYPE					(BYTE**)
#endif
#define JTX_GPU_LEAF_PLACEMENT
#define JTX_GPU_TREE_WIND
#define JTX_TREE_DYNAMIC_LIGHT
#define JTX_UNDERWATER_CAMERA
#define JTX_WATER_REFLECTION
#define JTX_DEATHCAM
//#define JTX_SHARPNESS
#define FIX_MEMORY_SPEEDTREE
//#define ENABLE_RENDER_CORE 								// DirectX 12 Ultimate Render Engine
//#define FPS_APPLICATON_DEVICE_LIMITATION 					// FPS sınırlaması aktif: belirli cihazlarda aşırı yüksek FPS kaynaklı performans sorunlarını önler
//	#ifdef FPS_APPLICATON_DEVICE_LIMITATION
//		#define RENDER_MAX_FPS        120  					// Maksimum render FPS değeri: cihaz performansına göre üst sınır
//	#endif

/*** DÜZELTMELER ***/
#define __ENABLE_STEALTH_FIX__
#define MULTIPLE_DAMAGE_DISPLAY_SYSTEM						// Çoklu damage gösterimi  düzeltir
#define ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX				// Damage efekt birikme sorunu düzetlmesi
#define ENABLE_BOSS_FALLING_FIX								// Bossların zeminden aşağı düşmesi veya haritanın dışına kayması sorununu düzeltir
#define ENABLE_CLIENT_UNEXPECTED_CLOSE_FIX					// Clientin syserr vermeden aniden kapanmasına sebep olan durumu düzeltir
#define ENABLE_INVISIBLE_MODE_EFFECT_FIX					// Görünmez moddan çıktığında karakter üzerinde kalan efektlerin temizlenmesini sağlar
#define ENABLE_LENS_RENDER_FIX								// Lens / kamera / zoom ile ilgili yanlış render problemlerini düzeltir
#define ENABLE_AUTO_ATTACK_ARCHER_NINJA_FIX     			// Otomatik saldırıda okçu/ninja sınıflarına özel hataları giderir
#define ENABLE_TEXTURE_SCALE_FIX                			// Texture %100 / scaling kaynaklı hataları düzeltir
#define ENABLE_TIMESTAMPED_EXCEPTION_DUMP   				// Her bir istisna için "errorlog" klasörüne zaman damgalı (ErrorLog_DD-MM-YYYY_HH-MM-SS.txt) bir hata dökümü dosyası yazar ve errorlog.exe'yi çalıştırır
#define ENABLE_NEW_STOP_ON_BLOCK    						// Engelleyici (ATTRIBUTE_BLOCK) tespit edildiğinde mevcut BlockMovement() çağrısına ek olarak NEW_Stop() fonksiyonunu çağırır (çarpışma sonrası hareket durdurma iyileştirmesi)
#define ENABLE_ABORT_TRACEBACK_WITH_LINE   					// Abort sırasında Python traceback bilgilerini (filename, funcname, line) loglar. Kullanılmazsa standart abort davranışı korunur
#define ENABLE_EXTENDED_SYSERR								// Genişletilmiş hata kayıtlarını etkinleştir
#define DISABLE_BACKGROUND_DARKENING						// Arkaplanda ekran kararmasını devre dışı bırakır
// #define ENABLE_SYSERR_BY_PID								// [Debug.cpp] syserr.txt yerine syserr-{pid}.txt oluşturur
/*** DÜZELTMELER ***/

/*<----Bilgilendirme---->*/
// Eğer bir kostüm sistemi devre dışı bırakılırsa, uiscript içindeki costumewindow.py dosyasını düzenlemeniz gerekir (CostumeSlot'u yorum satırı haline getirin)

/*
@#GENEL MAKROLAR
#define __OBSOLETE__								// Kullanılmayan ve gereksiz kodlar kaldırıldı
#define __UNIMPLEMENTED__							// Uygulanacak yeni şeylerin taslakları
#define __UNIMPLEMENTED_EXTEND__					// Uygulanacak yeni şeylerin genişletilmiş taslakları
*/

/*
#@@Genel Olarak
@warme601: syserr.txt gibi hata dosyaları ile birlikte gelişmiş dağıtım için release sürümünü kullanın.
*/