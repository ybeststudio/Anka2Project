#define LSS_SECURITY_KEY	"1234abcd5678efgh"
#define CLIENT_VERSION		"ext_180923"
#define APP_TITLE			"Anka2.com # Gerçek Rekabet Burada Baþlar! | [Best Studio]"				// APP_TITLE binary dosyasýndan yüklenir;

#define DEFAULT_DEFINE_YMIR							// Ymir tarafýndan belirlenen varsayýlan tanýmlama;
#ifdef DEFAULT_DEFINE_YMIR
	#define LOCALE_SERVICE_EUROPE					// Avrupa sunucu hizmetine taþýndý;
	#define ENABLE_ENERGY_SYSTEM					// Resmi enerji sistemi etkin;
	#define ENABLE_COSTUME_SYSTEM					// Resmi kostüm sistemi etkin;
	#define ENABLE_DRAGON_SOUL_SYSTEM				// Resmi ejderha ruhu (Dragon Soul) sistemi etkin;
	#define ENABLE_NEW_EQUIPMENT_SYSTEM				// Resmi yeni ekipman sistemi etkin.
#endif

#define ENABLE_FOG_FIX								// Resmi 16.2 sürümünden sis düzeltmesi;
#define ENABLE_ENB_MODE								// ENB modu (HD grafik) etkin;
#define ENABLE_ANTI_EXP								// Anti EXP sistemi etkin;
#define ENABLE_MOB_SCALE							// Yaratýk ölçeklendirmesi etkin;
#define ENABLE_LEVEL_INT							// Seviye tipi byte'tan int'e taþýndý (maks. seviye 300+);
#define ENABLE_GOLD_LIMIT							// Yang limiti 20kkk'a yükseltildi;
#define ENABLE_MDE_EFFECT							// 3D silahlar için MDE efekti;
#define ENABLE_PET_SYSTEM							// Evcil hayvan sistemi etkin;
#define ENABLE_DISCORD_RPC							// Discord RPC entegrasyonu etkin;
#define ENABLE_AUTO_REFINE							// Otomatik arýtma (auto refine) seçeneði etkin;
#define ENABLE_DICE_SYSTEM							// Zar sistemi etkin: Eðer yaratýk kral veya boss ise ve partideysen, düþen eþya rastgele daðýtýlýr;
#define ENABLE_BLINK_ALERT							// Mesaj geldiðinde görev çubuðunda yanýp sönme uyarýsý;
#define ENABLE_STACK_LIMIT							// Eþya yýðýn limiti 2000'e yükseltildi;
#define ENABLE_RENEWAL_CUBE							// Yenilenmiþ küp sistemi etkin;
#define ENABLE_FIX_MOBS_LAG							// Yaratýk gecikme (lag) düzeltmesi etkin;
#define ENABLE_MAP_TELEPORT							// Yetkililer minimap'e týklayarak ýþýnlanabilir;
#define ENABLE_MOUNT_SYSTEM							// Binek sistemi etkin;
#define ENABLE_OBJ_SCALLING							// Oyun içi nesne ölçeklendirme etkin;
#define ENABLE_FOXFS_ENCRYPT						// Fox FS þifreleme etkin;
#define ENABLE_SHOW_MOB_INFO						// Yaratýk seviye ve agresiflik bilgisi gösterimi;
#define ENABLE_RENEWAL_QUEST						// Resmi sunuculardaki yeni görev sayfasý;
#define ENABLE_CONFIG_MODULE						// Ayarlarýn kaydedilmesi için yapýlandýrma modülü etkin;
#define ENABLE_HAIR_SPECULAR						// Saçlarda parlama (specular) etkin;
#define ENABLE_BIOLOG_SYSTEM						// Biyolog sistemi etkin;
#define ENABLE_MOB_DROP_INFO						// Yaratýk/taþ düþürme penceresi etkin;
#define ENABLE_CHANGE_CHANNEL						// Kanal deðiþtirme seçeneði etkin;
#define ENABLE_MOUNT_PET_SKIN						// Binek ve pet kostüm (skin) sistemi etkin;
#define ENABLE_PENDANT_SYSTEM						// Resmi sürümdeki týlsým (pendant) sistemi etkin;
#define ENABLE_ATTBONUS_METIN						// Metin taþlarýna saldýrý bonusu sistemi etkin;
#define ENABLE_TEXTLINE_EMOJI						// Yazý satýrlarýnda / ipuçlarýnda emoji desteði;
#define ENABLE_MESSENGER_TEAM						// Arkadaþ listesindeki ekip sekmesi etkin;
#define ENABLE_DESTROY_DIALOG						// Eþyayý yere atarken yok etme seçeneði;
#define ENABLE_SORT_INVENTORY						// Envanter ve özel envanter sýralama sistemi etkin;
#define ENABLE_GRAPHIC_ON_OFF						// Grafik ayarlarýný aç/kapat sistemi etkin;
#define ENABLE_PARTY_POSITION						// Parti üyeleri haritada görülebilir;
#define ENABLE_RENEWAL_SHOPEX						// Geniþletilmiþ, yenilenmiþ ShopEx (çok sayfa) sistemi;
#define ENABLE_INSTANT_PICKUP						// Eþyalarý yerden anýnda alma etkin;
#define ENABLE_VIEW_CHEST_DROP						// Sandýk içeriðini görme özelliði etkin;
#define ENABLE_ATLAS_MARK_INFO						// Atlas üzerindeki iþaret bilgisi etkin;
#define ENABLE_QUICK_SELL_ITEM						// Dükkanlarda hýzlý satýþ seçeneði etkin;
#define ENABLE_MESSENGER_BLOCK						// Oyuncu engelleme sistemi etkin;
#define ENABLE_COINS_INVENTORY						// Envantere yeni para birimi eklendi;
#define ENABLE_EXTENDED_SAFEBOX						// Depo sayfasý 3'ten 6'ya çýkarýldý;
#define ENABLE_RENEWAL_SWITCHBOT					// Yenilenmiþ switchbot sistemi etkin;
#define ENABLE_RENEWAL_BOOK_NAME					// Yaratýk/taþlardan düþen kitap isimleri görünür;
#define ENABLE_GUILD_RANK_SYSTEM					// Lonca sýralama panosu etkin;
#define ENABLE_LOAD_INDEX_BINARY					// Index dosyasý binary olarak yüklenir;
#define ENABLE_CLIENT_PERFORMANCE					// Ýyileþtirilmiþ client performansý ve yeni playersettingmodule;
#define ENABLE_ETERPACK_OBSCURING					// EterPack anahtar gizleme (binary'de anahtarlarý gizler);
#define ENABLE_RENEWAL_DEAD_PACKET					// Zamanlayýcýlý "burada yeniden baþla / þehirde yeniden baþla" sistemi;
#define ENABLE_RENEWAL_BONUS_BOARD					// Resmiye yakýn yeni bonus panosu;
#define ENABLE_RENEWAL_SERVER_LIST					// Yenilenmiþ sunucu listesi etkin;
#define ENABLE_HIDE_COSTUME_SYSTEM					// Kostüm gizleme sistemi etkin;
#define ENABLE_ACCE_COSTUME_SYSTEM					// Aksesuar kostüm sistemi etkin;
#define ENABLE_RENEWAL_MINIMAP_MARK					// Minimap'ta yeni NPC/yaratýk/taþ iþaretleri;
#define ENABLE_TELEPORT_TO_A_FRIEND					// Arkadaþa ýþýnlanma sistemi etkin;
#define ENABLE_METIN_STONES_MINIMAP					// Minimap'ta metin taþlarý gösterimi;
#define ENABLE_MULTI_LANGUAGE_SYSTEM				// Çoklu dil sistemi etkin;
#define ENABLE_WEAPON_COSTUME_SYSTEM				// Silah kostümü sistemi etkin;
#define ENABLE_VIEW_TARGET_PLAYER_HP				// Hedef çubuðunda HP gösterimi;
#define ENABLE_VIEW_TARGET_DECIMAL_HP				// HP deðerleri ondalýklý gösterim;
#define ENABLE_RENEWAL_CLIENT_VERSION				// Yeni client sürüm kontrol sistemi;
#define ENABLE_CANSEEHIDDENTHING_FOR_GM				// GM'ler görünmez karakterleri görebilir (normal veya GM görünmezliði fark etmez);
#define ENABLE_EXTENDED_WHISPER_DETAILS				// Özel mesajlarda geniþletilmiþ hedef bilgisi, ülke bayraðý desteði;
#define ENABLE_ENVIRONMENT_EFFECT_OPTION			// Ortam efekt ayarlarý etkin;
#define ENABLE_EXTEND_TIME_COSTUME_SYSTEM			// Kostüm süresi uzatma sistemi etkin.
#define ENABLE_FOV_OPTION							// Görüþ alaný (Görüþ Alaný) özelleþtirmesini etkinleþtir;
#define ENABLE_TITLE_SYSTEM							// Eþyalar üzerinde unvan sistemi etkin;
#define ENABLE_RENDER_TARGET						// Render hedefleme (render target) sistemi etkin;
#define ENABLE_ATTBONUS_BOSS						// Boss yaratýklara saldýrý bonusu sistemi etkin;
#define ENABLE_EVENT_MANAGER						// Etkinlik yöneticisi (otomatik event sistemi) etkin;
#define ENABLE_MULTI_TEXTLINE						// Çok satýrlý yazý (multi textline) desteði etkin;
#define ENABLE_RENEWAL_AFFECT						// Yenilenmiþ affect sistemi (1 yeni alt tip + affect ikonu);
#define ENABLE_OFFLINE_MESSAGE						// Oyuncu çevrimdýþýysa offline mesaj býrakma seçeneði etkin;
#define ENABLE_RESTART_INSTANT						// 500.000 Yang karþýlýðýnda anýnda yeniden baþlatma etkin;
#define ENABLE_NEW_DUNGEON_LIB						// Yeni zindan (dungeon) fonksiyonlarý etkin;
#define ENABLE_RENEWAL_OX_EVENT						// Resmi sunuculara benzer yenilenmiþ OX etkinliði;
#define ENABLE_MULTI_FARM_BLOCK						// 2'den fazla istemcide (client) farm yapýlmasýný engelle;
#define ENABLE_SPECIAL_INVENTORY					// Kitaplar, yükseltmeler vb. için özel envanter etkin;
#define ENABLE_STONE_SCALE_OPTION					// Metin taþlarý için ölçeklendirme özelleþtirmesi etkin;
#define ENABLE_MAINTENANCE_SYSTEM					// Oyun içinde bakým bilgisi gösterim sistemi etkin;
#define ENABLE_INGAME_WIKI_SYSTEM					// Oyun içi wiki sistemi etkin;
#define ENABLE_PICKUP_ITEM_EFFECT					// Envanter / özel envantere alýnan eþyalar için efekt gösterimi;
#define ENABLE_RENEWAL_OFFLINESHOP					// Yeni nesil offlineshop sistemi etkin;
#define ENABLE_RENEWAL_BATTLE_PASS					// Yenilenmiþ Battle Pass sistemi etkin;
#define ENABLE_RENEWAL_TEAM_AFFECT					// Takým etki (logo) sistemi yenilenmiþ haliyle etkin;
#define ENABLE_RENEWAL_TEXT_SHADOW					// Ýsim metinlerine gölge efekti seçeneði etkin;
#define ENABLE_SLOT_MARKING_SYSTEM					// Resmi sürümlerdeki gibi slot iþaretleme sistemi etkin;
#define ENABLE_RENEWAL_SKILL_SELECT					// Becerileri seçmek için yeni pencere sistemi etkin;
#define ENABLE_BONUS_COSTUME_SYSTEM					// Kostümlerde bonus ekleme ve deðiþtirme sistemi etkin;
#define ENABLE_RENEWAL_SPECIAL_CHAT					// Yang, eþya vb. için özel yenilenmiþ sohbet sistemi;
#define ENABLE_LARGE_DYNAMIC_PACKET					// Büyük dinamik paket boyutu desteði etkin;
#define ENABLE_CLIENT_LOCALE_STRING					// Client'tan dil dizeleri (locale string) yükleme sistemi etkin;
#define ENABLE_GUILD_LEADER_TEXTAIL					// Lonca liderlerinin isimlerinde [Leader] etiketi gösterimi etkin;
#define ENABLE_BOSS_EFFECT_OVER_HEAD				// Boss'larýn kafasýnýn üzerinde özel efekt gösterimi;
#define ENABLE_MONSTER_TARGET_ELEMENT				// Her yaratýk için hedef element bilgisi gösterimi;
#define ENABLE_RENEWAL_PREMIUM_SYSTEM				// Oyuncular için premium sistem etkin;
#define ENABLE_RENEWAL_INGAME_ITEMSHOP				// Oyun içi yepyeni itemshop sistemi etkin;
#define ENABLE_AUTOMATIC_PICK_UP_SYSTEM				// Otomatik eþya toplama sistemi etkin;
#define ENABLE_OFFLINESHOP_SEARCH_SYSTEM			// Yenilenmiþ offlineshop için arama sistemi etkin;
#define ENABLE_MINIGAME_OKEY_CARDS_SYSTEM			// Okey kartlarý mini oyun sistemi etkin;
#define ENABLE_PYTHON_DYNAMIC_MODULE_NAME			// Python dinamik modül isimlendirmesi (ekstra güvenlik katmaný) etkin.
#define ENABLE_SKILL_COLOR_SYSTEM					// Beceri (skill) renk deðiþtirme sistemi etkin;
	#ifdef ENABLE_SKILL_COLOR_SYSTEM
	#define ENABLE_5LAYER_SKILL_COLOR				// Skill renkleri için 5 katman desteði etkin.
#endif
#define ENABLE_FISH_GAME                        	// Resmi sunuculardaki yeni balýk tutma mini oyununu etkinleþtir
#define ENABLE_CLIP_MASKING                     	// Resmi sürümdeki clip masking (görsel kýrpma) özelliðini etkinleþtir
#define ENABLE_UTF8_ENCODING                    	// Client’ta UTF-8 kodlamasýný ana kodlama olarak etkinleþtir (Windows-1254)
#define ENABLE_NPC_WEAR_ITEM                    	// NPC’lerin eþya giyebilmesini etkinleþtir
#define ENABLE_RENEWAL_REGEN                    	// Yeni tip can yenilenme (regen) sistemini etkinleþtir
#define ENABLE_HUNTING_SYSTEM                   	// Özgün görevlerle avcýlýk (hunting) sistemini etkinleþtir
#define ENABLE_EMOTICONS_SYSTEM                 	// Sohbet ve fýsýlda (whisper) emoji sistemini etkinleþtir
#define ENABLE_GUILD_TOKEN_AUTH                 	// MartySama tarafýndan geliþtirilen lonca yetkilendirme yamasýný etkinleþtir
#define ENABLE_GROWTH_PET_SYSTEM                	// Resmi sürümdeki geliþim (growth) pet sistemini etkinleþtir
#define ENABLE_CHANGE_LOOK_SYSTEM               	// Eþya görünümü deðiþtirme (change look) sistemini etkinleþtir
#define ENABLE_AURA_COSTUME_SYSTEM              	// Aura kostüm sistemini etkinleþtir
#define ENABLE_SHIP_DEFENCE_DUNGEON             	// Resmi sürümdeki Gemi Savunma Zindaný’ný etkinleþtir
#define ENABLE_PYTHON_EVENT_FUNCTIONS           	// Resmi sürümdeki gibi Python event fonksiyonlarýný etkinleþtir
#define ENABLE_DUNGEON_TRACKING_SYSTEM          	// Yeni zindan bilgi (takip) sistemini etkinleþtir
#define ENABLE_RENEWAL_TELEPORT_SYSTEM          	// Yenilenmiþ ýþýnlanma (teleport) sistemini etkinleþtir
#define ENABLE_INVENTORY_EXPANSION_SYSTEM       	// Resmi sürümdeki envanter geniþletme sistemini etkinleþtir
#define ENABLE_MORE_FPS								// FPS sýnýrý yükseltme
#define ENABLE_GM_INV_EFFECT         				// Resmi GM /inv efekt güncellemesi
#define ENABLE_MINIMAP_SMOOTH_ZOOM             		// Minimap yumuþak yakýnlaþtýrma sistemi
#define ENABLE_OFFICAL_FEATURES						// Resmi özellikleri etkinleþtir
#define WJ_SHOW_NPC_QUEST_NAME              		// NPC Görev Ýsimlerini Göster
#define ENABLE_ATLAS_SCALE							// Harita atlasý ölçekleme
#define ENABLE_FALLING_EFFECT_SYSTEM				// Yere düþen itemlere efekt verme sistemi
#define ENABLE_STANDING_MOUNT
#ifdef ENABLE_STANDING_MOUNT
	#define STANDING_MOUNT_VNUM_1 40003 			// Turbo Sörf Tahtasý
	#define STANDING_MOUNT_VNUM_2 40004 			// Wukong'un Fýrtýnasý
	#define STANDING_MOUNT_VNUM_3 40005 			// Wukong'un Gürlemesi
	#define SURFBOARD STANDING_MOUNT_VNUM_1 		// Geriye uyumluluk için
#endif
#define ENABLE_RIDING_EXTENDED 						// Resmi At seviyesi geniþletildi
#define ENABLE_SPIRIT_STONE_READING					// Ruh Taþý Okuma sistemi
#define ENABLE_SKILL_BOOK_READING					// Beceri Kitabý Okuma sistemi
#define __AUTO_HUNT__								// Otomatik Av sistemi
#define ENABLE_AUTO_SELL_SYSTEM						// Otomatik item satmayý sistemini etkinleþtir;
#define ENABLE_STYLE_ATTRIBUTE_SYSTEM      			// Seçmeli özel kadim efsunu sistemini  etkinleþtir;
#define ENABLE_GM_MOB_VNUM_DISPLAY   				// GM'lere (oyun yöneticilerine) moblarýn VNUM deðerini oyun içi veya log çýktýsýnda gösterir

/*** DÜZELTMELER ***/
#define MULTIPLE_DAMAGE_DISPLAY_SYSTEM				// Çoklu damage gösterimi  düzeltir
#define ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX		// Damage efekt birikme sorunu düzetlmesi
#define ENABLE_BOSS_FALLING_FIX						// Bosslarýn zeminden aþaðý düþmesi veya haritanýn dýþýna kaymasý sorununu düzeltir
#define ENABLE_CLIENT_UNEXPECTED_CLOSE_FIX			// Clientin syserr vermeden aniden kapanmasýna sebep olan durumu düzeltir
#define ENABLE_INVISIBLE_MODE_EFFECT_FIX			// Görünmez moddan çýktýðýnda karakter üzerinde kalan efektlerin temizlenmesini saðlar
#define ENABLE_LENS_RENDER_FIX						// Lens / kamera / zoom ile ilgili yanlýþ render problemlerini düzeltir
#define ENABLE_AUTO_ATTACK_ARCHER_NINJA_FIX     	// Otomatik saldýrýda okçu/ninja sýnýflarýna özel hatalarý giderir
#define ENABLE_TEXTURE_SCALE_FIX                	// Texture %100 / scaling kaynaklý hatalarý düzeltir
#define ENABLE_TIMESTAMPED_EXCEPTION_DUMP   		// Her bir istisna için "errorlog" klasörüne zaman damgalý (ErrorLog_DD-MM-YYYY_HH-MM-SS.txt) bir hata dökümü dosyasý yazar ve errorlog.exe'yi çalýþtýrýr
#define ENABLE_NEW_STOP_ON_BLOCK    				// Engelleyici (ATTRIBUTE_BLOCK) tespit edildiðinde mevcut BlockMovement() çaðrýsýna ek olarak NEW_Stop() fonksiyonunu çaðýrýr (çarpýþma sonrasý hareket durdurma iyileþtirmesi)
#define ENABLE_ABORT_TRACEBACK_WITH_LINE   			// Abort sýrasýnda Python traceback bilgilerini (filename, funcname, line) loglar. Kullanýlmazsa standart abort davranýþý korunur
/*** DÜZELTMELER ***/

/*** TAMAMLANMAMIÞ SÝSTEMLER (YAKINDA) ***/
#define ENABLE_GUILDRENEWAL_SYSTEM					// Geliþmiþ Resmi gibi lonca altyapýsý için çekirdek yenileme sistemi
	#ifdef ENABLE_GUILDRENEWAL_SYSTEM	
		#define ENABLE_GUILD_MARK_RENEWAL			// Lonca logo (mark) yükleme ve güncelleme iþ akýþýný etkinleþtirir
		#define ENABLE_SECOND_GUILDRENEWAL_SYSTEM	// Ek modüllerle geniþletilmiþ ikinci lonca yenileme katmaný
		#define ENABLE_NEW_WAR_OPTIONS				// Tur, puan ve süre bazlý modern lonca savaþý seçenekleri
		#define ENABLE_EXTENDED_GUILD_LEVEL			// Lonca seviye kapasitesinin 30'a çýkarýlmasý
		//#define ENABLE_MEDAL_OF_HONOR				// Lonca ekonomisi için yeni para birimi (Onur Madalyasý)
		#define ENABLE_GUILD_DONATE_ATTENDANCE		// Günlük lonca baðýþý ve katýlým takip sistemi
		#define ENABLE_GUILD_WAR_SCORE				// Lonca savaþ istatistiklerinin detaylý skor ekraný
		#define ENABLE_GUILD_LAND_INFO				// Lonca arazilerine ait detaylý bilgi arayüzü
		#define ENABLE_GUILDBANK_LOG				// Lonca banka ve iþlem kayýtlarýnýn günlük sistemi
		#define ENABLE_EXTENDED_RENEWAL_FEATURES	// Lonca lideri devri ve arazi silme yönetimi gibi ek yenileme özellikleri
		#define ENABLE_GUILDWAR_BUTTON				// Arayüzde Lonca Savaþý eriþim düðmesini aktif eder
		//#define ENABLE_GUILD_REQUEST				// Lonca istek/baþvuru altyapýsý (eksik modüller nedeniyle devre dýþý)
		#define ENABLE_NEW_WORLD_GUILDRENEWAL		// Geliþmiþ lonca yenileme/güncelleme sistemi (sunucu tarafý eksik modüller)
		//#define ENABLE_GUILD_RANKING				// Loncalar arasý sýralama sistemi (tam entegre edilmediði için kapalý)
		//#define ENABLE_GUILDSTORAGE_SYSTEM     	// Loncanýn ortak deposu: üyelerin paylaþýmlý eþya deposu sistemi
	#endif
/*** TAMAMLANMAMIÞ SÝSTEMLER (YAKINDA) ***/
