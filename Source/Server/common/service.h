#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__

#define CLIENT_VERSION		"ext_180923"			// Client sürüm etiketidir; sunucu–client uyumluluðunu kontrol eder.

#define ENABLE_ANTI_EXP								// Anti exp sistemini etkinleþtir;
#define ENABLE_LEVEL_INT							// Seviye byte'dan int'e taþýndý (maksimum seviye 300+);
#define ENABLE_GOLD_LIMIT							// Altýn limiti 20kkk'ya taþýndý;
#define ENABLE_PET_SYSTEM							// Pet sistemini etkinleþtir;
#define ENABLE_DICE_SYSTEM							// Zar sistemini etkinleþtir: mob kral veya boss ise ve partideyseniz, düþen eþya rastgele zar ile belirlenir;
#define ENABLE_GLOBAL_CHAT							// Bayrak ile global sohbeti etkinleþtir;
#define ENABLE_STACK_LIMIT							// Eþya yýðýn limiti 2000'e taþýndý;
#define ENABLE_RENEWAL_CUBE							// Yenileme küp sistemini etkinleþtir;
#define ENABLE_MOUNT_SYSTEM							// Binek sistemi etkinleþtir;
#define ENABLE_MOUNT_PET_SKIN						// Binek ve pet için skin etkinleþtir;
#define ENABLE_BIOLOG_SYSTEM						// Biolog sistemini etkinleþtir;
#define ENABLE_MOB_DROP_INFO						// Mob ve taþ düþürme penceresini etkinleþtir;
#define ENABLE_SHOW_MOB_INFO						// Mob seviyesi ve saldýrganlýk bilgisini etkinleþtir;
#define ENABLE_RENEWAL_QUEST						// Resmi sunucularla ayný yeni görev sayfasý;
#define ENABLE_CHANGE_CHANNEL						// Kanal deðiþtirme seçeneðini etkinleþtir;
#define ENABLE_MESSENGER_TEAM						// Mesajlaþmada takým listesini etkinleþtir;
#define ENABLE_PENDANT_SYSTEM						// Resmi pendant sistemini etkinleþtir;
#define ENABLE_ATTBONUS_METIN						// Attbonus metin etkinleþtir;
#define ENABLE_DESTROY_DIALOG						// Eþyalarý düþürdüðünüzde yok etme seçeneðini etkinleþtir;
#define ENABLE_SORT_INVENTORY						// Envanter ve özel envanterdeki eþyalar sýralanabilir;
#define ENABLE_PARTY_POSITION						// Parti üyeleri haritada görülebilir;
#define ENABLE_RENEWAL_SHOPEX						// Daha fazla sayfa ile yenileme shopex;
#define ENABLE_VIEW_CHEST_DROP						// Oyuncular sandýðýn içindekileri görebilir;
#define ENABLE_MESSENGER_BLOCK						// Oyuncu engellemeyi etkinleþtir;
#define ENABLE_COINS_INVENTORY						// Envanterde yeni para birimi eklendi;
#define ENABLE_QUICK_SELL_ITEM						// Dükkanda eþyalar için hýzlý satýþý etkinleþtir;
#define ENABLE_EXTENDED_SAFEBOX						// 3 yerine 6 kasa sayfasý etkinleþtir;
#define ENABLE_PARTY_BUFF_SYSTEM					// Parti buff sistemini etkinleþtir;
#define ENABLE_IGNORE_LOW_POWER_BUFF				// Düþük buff sistemini etkinleþtir;
#define ENABLE_BOSS_KILL_NOTICE						// Patron öldürme duyurusu etkinleþtir;
#define ENABLE_ANNONUNCEMENT_LEVELUP				// Seviye duyuru (120) bilgilendirmeyi etkinleþtir;
#define ENABLE_SKILL_MASTER_LEVEL					// Beceri master seviyesi 17' de  etkinleþtir;
#define ENABLE_COORDINATES_COMMAND					// User komutunda koordinat'ý etkinleþtir;
#define ENABLE_GOHOME_IFNOMAP						// Harita bulunamadýðýnda köye ýþýnlanmayý etkinleþtir;
#define ENABLE_CLEAR_GUILD_LANDS					// Kullanýlmayan lonca arazilerini silmeyi etkinleþtir;
#define ENABLE_RENEWAL_SWITCHBOT					// Switchbot sistemini etkinleþtir;
#define ENABLE_RENEWAL_BOOK_NAME					// Oyuncular mob ve taþlardan düþen kitabýn adýný görebilir;
#define ENABLE_GUILD_RANK_SYSTEM					// Lonca sýralamasý için pano etkinleþtir;
#define ENABLE_RENEWAL_DEAD_PACKET					// Zamanlayýcý ile burada yeniden baþlat ve kasabada yeniden baþlat;
#define ENABLE_HIDE_COSTUME_SYSTEM					// Kostüm sistemi için gizleme etkinleþtir;
#define ENABLE_ACCE_COSTUME_SYSTEM					// Acce kostüm sistemini etkinleþtir;
#define ENABLE_TELEPORT_TO_A_FRIEND					// Arkadaþa ýþýnlanmayý etkinleþtir;
#define ENABLE_WEAPON_COSTUME_SYSTEM				// Silah kostüm sistemini etkinleþtir.
#define ENABLE_MULTI_LANGUAGE_SYSTEM				// Çoklu dil sistemi;
#define ENABLE_VIEW_TARGET_PLAYER_HP				// Hedef çubuðunda HP'yi etkinleþtir;
#define ENABLE_RENEWAL_CLIENT_VERSION				// Yeni istemci sürüm kontrolünü etkinleþtir;
#define ENABLE_VIEW_TARGET_DECIMAL_HP				// Ondalýk HP'yi etkinleþtir.
#define ENABLE_EXTENDED_WHISPER_DETAILS				// Geniþletilmiþ fýsýldama hedef bilgisi çoklu dil ülke bayraðý;
#define ENABLE_EXTEND_TIME_COSTUME_SYSTEM			// Kostüm sistemi için süre uzatmayý etkinleþtir;
#define ENABLE_IMPROVED_HANDSHAKE_PROCESS			// Owsap tarafýndan geliþtirilmiþ el sýkýþma sürecini etkinleþtir.
#define ENABLE_TITLE_SYSTEM							// Eþyada baþlýk sistemini etkinleþtir;
#define ENABLE_ATTBONUS_BOSS						// Attbonus boss etkinleþtir;
#define ENABLE_EVENT_MANAGER						// Etkinlik yöneticisini etkinleþtir (otomatik etkinlikler);
#define ENABLE_RENEWAL_AFFECT						// Yenileme etkisini etkinleþtir (1 yeni alt tip + etki simgesi);
#define ENABLE_OFFLINE_MESSAGE						// Oyuncu baðlý deðilse çevrimdýþý mesaj býrakma seçeneðini etkinleþtir;
#define ENABLE_RESTART_INSTANT						// 500.000 Yang için anýnda yeniden baþlatmayý etkinleþtir;
#define ENABLE_NEW_DUNGEON_LIB						// Yeni zindan fonksiyonlarýný etkinleþtir;
#define ENABLE_RENEWAL_OX_EVENT						// Resmi gibi yenileme ox etkinliðini etkinleþtir;
#define ENABLE_MULTI_FARM_BLOCK						// 2'den fazla istemci için çoklu farm'ý imkansýz hale getir;
#define ENABLE_SPECIAL_INVENTORY					// Kitaplar, yükseltme vb. için özel envanter;
#define ENABLE_EXTEND_ITEM_AWARD					// Geniþletilmiþ eþya ödülünü etkinleþtir (çoðunlukla ishop için kullanýlýr);
#define ENABLE_MAINTENANCE_SYSTEM					// Oyunda bakým bilgisini etkinleþtir;
#define ENABLE_SKILL_COLOR_SYSTEM					// Yetenek renk sistemini etkinleþtir;
#define ENABLE_PICKUP_ITEM_EFFECT					// Envanter / özel envanterde alýnan eþyalar için efekti etkinleþtir;
#define ENABLE_RENEWAL_BATTLE_PASS					// Yenileme savaþ geçiþ sistemini etkinleþtir;
#define ENABLE_RENEWAL_OFFLINESHOP					// Yeni offlineshop sistemini etkinleþtir;
#define ENABLE_RENEWAL_TEAM_AFFECT					// Yenileme takým etkisini etkinleþtir (logo);
#define ENABLE_SLOT_MARKING_SYSTEM					// Resmi gibi slot iþaretleme sistemini etkinleþtir;
#define ENABLE_RENEWAL_SKILL_SELECT					// Yetenek seçmek için yeni bir pencere etkinleþtir;
#define ENABLE_RENEWAL_SPECIAL_CHAT					// Yang, Eþyalar vb. için yenileme özel sohbeti etkinleþtir;
#define ENABLE_LARGE_DYNAMIC_PACKET					// Büyük dinamik paket boyutunu etkinleþtir;
#define ENABLE_CLIENT_LOCALE_STRING					// Ýstemciden yerel dize yüklemesini etkinleþtir;
#define ENABLE_BONUS_COSTUME_SYSTEM					// Kostümlerde bonus ekleme ve deðiþtirmeyi etkinleþtir;
#define ENABLE_GUILD_LEADER_TEXTAIL					// [Lider] ile lonca generali için metni etkinleþtir;
#define ENABLE_RENEWAL_PREMIUM_SYSTEM				// Premium sistemini etkinleþtir (oyuncu, hesap);
#define ENABLE_CROSS_CHANNEL_REQUESTS				// Kanal arasý istekleri etkinleþtir (gelecekte geniþletilecek);
#define ENABLE_DROP_LEVEL_LIMIT_RANGE				// Tip limitinde level_limit_start ve level_limit_end gibi düþüþ için seviye limit aralýðýný etkinleþtir;
#define ENABLE_RENEWAL_INGAME_ITEMSHOP				// Oyunda nesne market etkinleþtir;
#define ENABLE_AUTOMATIC_PICK_UP_SYSTEM				// Otomatik toplama sistemini etkinleþtir;
#define ENABLE_OFFLINESHOP_SEARCH_SYSTEM			// Yenileme offlineshop için arama sistemini etkinleþtir;
#define ENABLE_MINIGAME_OKEY_CARDS_SYSTEM			// Minigame okey kartlarý sistemini etkinleþtir.
#ifdef ENABLE_EXTEND_ITEM_AWARD	
	#define USE_ITEM_AWARD_CHECK_ATTRIBUTES	
#endif	
#define ENABLE_FISH_GAME							// Resmi yeni balýk tutma oyununu etkinleþtir;
#define ENABLE_RENEWAL_REGEN						// Yeni bir regen türünü etkinleþtir;
#define ENABLE_HUNTING_SYSTEM						// Benzersiz görevlerle avcýlýk sistemini etkinleþtir;
#define ENABLE_EMOTICONS_SYSTEM						// Sohbet ve fýsýldamada emoji sistemi;
#define ENABLE_ANTI_EQUIP_FLOOD						// Ekipman için hýzlý kontrolü etkinleþtir;
#define ENABLE_GUILD_TOKEN_AUTH						// MartySama tarafýndan lonca için yama etkinleþtir;
#define ENABLE_HANDSHAKE_SESSION					// El sýkýþma oturumu düzeltmesini etkinleþtir;
#define ENABLE_GROWTH_PET_SYSTEM					// Resmi evcil pet sistemini etkinleþtir;
#define ENABLE_CHANGE_LOOK_SYSTEM					// Görünüm deðiþtirme sistemini etkinleþtir;
#define ENABLE_RENEWAL_BONUS_BOARD					// Resmi ile neredeyse ayný yeni bonus panosu;
#define ENABLE_AURA_COSTUME_SYSTEM					// Aura kostüm sistemini etkinleþtir;
#define ENABLE_SHIP_DEFENCE_DUNGEON					// Resmi Zindan Gemi Savunmasýný etkinleþtir;
#define ENABLE_DUNGEON_TRACKING_SYSTEM				// Yeni zindan bilgisini etkinleþtir;
#define ENABLE_RENEWAL_TELEPORT_SYSTEM				// Yeni ýþýnlanma sistemini etkinleþtir;
#define ENABLE_INVENTORY_EXPANSION_SYSTEM			// Resmi gibi envanter geniþletme sistemini etkinleþtir;
#define ENABLE_GM_INV_EFFECT         				// Resmi GM /inv efekt güncellemesi
#define ENABLE_RIDING_EXTENDED         				// Resmi At seviyesi geniþletildi
#define ENABLE_BOT_PLAYER							// Bot oyuncu sistemi
#define ENABLE_STANDING_MOUNT	
#ifdef ENABLE_STANDING_MOUNT	
	#define STANDING_MOUNT_VNUM_1 40003 			// Turbo Sörf Tahtasý
	#define STANDING_MOUNT_VNUM_2 40004 			// Wukong'un Fýrtýnasý
	#define STANDING_MOUNT_VNUM_3 40005 			// Wukong'un Gürlemesi
	#define SURFBOARD STANDING_MOUNT_VNUM_1 		// Geriye uyumluluk için
	#define IS_STANDING_MOUNT_VNUM(v) ((v)==STANDING_MOUNT_VNUM_1 || (v)==STANDING_MOUNT_VNUM_2 || (v)==STANDING_MOUNT_VNUM_3)
#endif
#ifdef ENABLE_HUNTING_SYSTEM 						// Avcýlýk görev sistemini etkinleþtir;
	#define HUNTING_MISSION_COUNT 90        		// Ayný anda aktif olabilecek av görevi sayýsý
	#define HUNTING_MONEY_TABLE_SIZE 9     			// Her görev seviyesi için yang ödül tablosu giriþi sayýsý
	#define HUNTING_EXP_TABLE_SIZE 9       			// Her görev seviyesi için EXP tablosu giriþi sayýsý
#endif
#define ENABLE_SKILL_BOOK_READING					// Beceri Kitabý Okuma sistemi
#define ENABLE_SPIRIT_STONE_READING					// Ruh Taþý Okuma sistemi
#define __AUTO_HUNT__								// Otomatik Av sistemi
#define ENABLE_AUTO_SELL_SYSTEM						// Otomatik item satmayý sistemini etkinleþtir;
	#ifdef ENABLE_AUTO_SELL_SYSTEM
	#define MAX_AUTO_SELL_ITEMS 100					// Listeye eklenecek maksimum item sayýsý
#endif
#define ENABLE_PLAYERS_ONLINE                  		// Çevrimiçi oyuncu /players_online komutunu  etkinleþtir;
#define ENABLE_STYLE_ATTRIBUTE_SYSTEM      			// Seçmeli özel kadim efsunu sistemini  etkinleþtir;
#define ENABLE_ITEM_MODIFIER_AVG_SKILL				// Ortalama ve Beceri efsun nesnesi sistemini etkinleþtir;

/*** DÜZELTMELER ***/
#define FIX_SyncPosition                       		// Pozisyon senkronizasyon sorunlarýný düzeltir
#define ENABLE_PROXY_IP								// Proxy/VPN tespiti ve reverse-proxy arkasýndaki gerçek istemci IP'sinin doðru okunmasýný saðlayarak IP algýlama hatalarýný düzeltir
#define ENABLE_UDP_BLOCK							// UDP port engellemeyi etkinleþtir;
#define ENABLE_PORT_SECURITY						// db_port, p2p_port ve uzak admin sayfasý açýklarýný engelle;
#define MULTIPLE_DAMAGE_DISPLAY_SYSTEM				// Çoklu damage gösterimi  düzeltir
#define ENABLE_DBMANAGER_SQL_INJECT_FIX				// Veritabaný tarafýnda SQL Injection açýklarýný engeller
#define ENABLE_LOG_SQL_INJECTION_FIX				// Log tablosu üzerinden yapýlabilecek SQL injection açýklýðýný kapatýr
#define ENABLE_LOCALHOST_IP_FIX						// Localhost / 127.0.0.1 IP kullanýmýndan kaynaklý baðlantý sorunlarýný düzeltir
#define ENABLE_INTERNAL_IP_FIX						// Dahili (internal) IP algýlama / yönlendirme ile ilgili sorunlarý düzeltir
#define ENABLE_POINT_EXP_FIX						// EXP ve karakter puaný hesaplama hatalarýný düzeltir
#define ENABLE_END_PVP_WHEN_DISCONNECT				// Baðlantý kesildiðinde aktif PvP durumunu otomatik sonlandýrýr
#define ENABLE_MINING_DISTANCE_FIX					// Kazý (mining) mesafe kontrol hatalarýný düzeltir
#define ENABLE_STOP_FUNCTION_FIX					// Hareket/aksiyon durdurma fonksiyonundaki buglarý giderir
#define ENABLE_STR_NEW_NAME_FIX						// Yeni isimlendirme (string) hatalarýný düzeltir
#define ENABLE_LAST_ATTACK_TIME_FIX					// Son saldýrý zaman hesaplamasýný doðru þekilde düzenler
#define ENABLE_COMPUTE_POINT_FIX					// Karakter stat/point hesaplama hatalarýný giderir
#define ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX		// Damage efekt birikme sorunu düzeltmesi
#define ENABLE_QUEST_PC_GETFLAG_CRASH_FIX			// Quest pc.getf/pc.getflag kullanýldýðýnda oluþan olasý crash sorununu düzeltir
#define ENABLE_FRIEND_LIST_REMOVE_FIX				// Arkadaþ listesinden oyuncu silememe / hatalý silme problemini düzeltir
#define ENABLE_MOUNT_LEVEL_BUG_FIX					// Binek level alma / level gösterimi ile ilgili hatalarý düzeltir
#define ENABLE_MOUNT_FIRE_SPIRIT_ATTACK_FIX			// Binek üzerindeyken Ateþ Hayaleti saldýrýsýnýn çalýþmama sorununu giderir
#define DEFAULT_REMOVE_ALL_BOOKSS					// Beceri kitaplarýnýn tek seferde yanlýþlýkla toplu silinmesini engeller
#define ENABLE_MOUNT_PUSHBACK_FIX					// Bineklerin geriye atma / geriye sarma bug’ýný düzeltir
#define ENABLE_BOSS_WALL_CLIP_FIX					// Bosslarýn duvar içine sýkýþtýrýlmasý / duvar içinden vurulmasý exploitini engeller
#define ENABLE_MAGIC_WEAPON_BUG_FIX					// Büyülü silah / hava kýlýcý benzeri skilllerdeki hasar veya görünüm bug’larýný düzeltir
#define ENABLE_CUBE_GAME_CORE_FIX					// Cube (dönüþtürme) iþlemi sýrasýnda oluþan core hatalarýný düzeltir
#define ENABLE_CUBE_REQUEST_LIST_FIX				// Cube request / result list verilerinin yanlýþ veya eksik gelmesi problemini düzeltir
#define ENABLE_TRANSFORMATION_SKILL_CLOSE_FIX		// Dönüþüm halindeyken kapanmasý gereken skilllerin açýk kalma sorununu giderir
#define ENABLE_DUNGEON_CORE_CRASH_FIX				// Zindan giriþ/çýkýþlarýnda oluþan core crash problemlerini düzeltir
#define ENABLE_ETC_ITEM_DROP_FIX					// ETC (çeþitli) itemlerin yanlýþ veya hatalý drop olma sorununu düzeltir
#define ENABLE_GUARDIAN_MOB_AGGRO_FIX				// Köy gardiyanýnýn mob gördüðünde yanlýþ veya tepkisiz davranma problemini düzeltir
#define ENABLE_WEDDING_COSTUME_EQUIP_FIX			// Gelinlik/smokýn giyiliyken kostüm giyememe veya çakýþma sorununu giderir
#define ENABLE_EQUIPPED_ITEM_STORAGE_FIX			// Takýlý (equipli) itemlerin depoya konabilmesiyle oluþan bug’ý engeller
#define ENABLE_QUEST_PC_SELECT_FIX					// Quest komutu pc.select ile ilgili seçim / hedefleme sorunlarýný düzeltir
#define ENABLE_PARTY_DUNGEON_CORE_FIX				// Grup halinde zindanda oluþan core crash / senkron sorunlarýný giderir
#define ENABLE_PARTY_FLAG_CHANGE_FIX				// Gruptayken bayrak (empire) deðiþtirme hatasýný / kýsýtýný düzeltir
#define ENABLE_HORSE_SPAWN_EXPLOIT_FIX				// At çaðýrma / horse spawn ile yapýlan exploit ve kötüye kullanýmlarý engeller
#define ENABLE_HP_SP_ABSORB_ORB_FIX					// HP-SP absorb orb eþyalarýnýn yanlýþ çalýþmasý veya deðer bug’ýný düzeltir
#define ENABLE_HP_SP_STEAL_FIX						// HP-SP çalma (lifesteal / manasteal) hesaplama ve uygulama hatalarýný giderir
#define ENABLE_SOCKET_STONE_MASS_DELETE_FIX			// Ýstiflenmiþ taþlarýn tek seferde yanlýþlýkla toplu silinmesini engeller
#define ENABLE_WARP_HP_LOSS_FIX						// Iþýnlanýnca (warp) HP düþmesi / can kaybý yaþanmasý sorununu düzeltir
#define ENABLE_ITEM_SWAP_FIX						// Ýki item arasýnda slot deðiþtirirken oluþan item swap bug’ýný düzeltir
#define ENABLE_CAMPFIRE_BUG_FIX						// Kamp ateþi sistemindeki görsel/etki kaynaklý bug’larý düzeltir
#define ENABLE_CHARACTER_NAME_HACK_FIX				// Karakter ismi üzerinden yapýlan hile / exploit giriþimlerini engeller
#define ENABLE_COSTUME_GENDER_CHANGE_FIX			// Kostüm giyiliyken cinsiyet deðiþtirme ile oluþan görünüm/slot bug’ýný giderir
#define ENABLE_GUILD_EMPIRE_BUG_FIX             	// Lonca-Ýmparatorluk (empire/ownership) devretme / miras hatasýný düzeltir
#define ENABLE_GUILD_YANG_ACCOUNTING_FIX        	// Lonca iþlemlerinde yanlýþ yang hesaplamalarýný giderir
#define ENABLE_LUA_ESC_BEHAVIOR_FIX             	// Lua scriptlerinde ESC/exit davranýþýndan kaynaklý hatalarý düzeltir
#define ENABLE_MESSENGER_MANAGER_FIX            	// Mesajlaþma / messenger yöneticisi ile ilgili hatalarý düzeltir
#define ENABLE_QUICK_SLOT_FIX                   	// Hýzlý slot (quickslot) atama / deðiþtirme sorunlarýný giderir
#define ENABLE_WARRIOR_SKILL_RESET_FIX          	// Savaþçý (warrior) skill reset / sýfýrlama hatalarýný düzeltir
#define ENABLE_BERSERKER_MODE_FIX               	// Berserker / sersemlik ile ilgili bilinen oyun içi hatalarý giderir
#define ENABLE_DEMON_TOWER_SMALL_FIX            	// Þeytan Kulesi gibi küçük edge-case hatalarýný düzeltir
#define ENABLE_STONE_ITEM_BUG_FIX               	// Taþ/soket itemleriyle ilgili kullaným/istif/bellek hatalarýný giderir
#define ENABLE_WAR_CRASHER_PROTECTION_FIX       	// Savaþ (war) sýrasýnda crash/exploit korumalarý ekler
#define HORSE_FIX									// At/binek sistemindeki spawn, hareket, saldýrý ve pozisyon hatalarýný giderir; exploit risklerini azaltýr
#define SYS_ERR_SOURCE_TRACKING    					// Hata tetiklendiðinde log çýktýsýna kaynak dosya ve satýr bilgisi ekler
#define ENABLE_SKILL_YONGBI_PROCESSING_FIX   		// SKILL_YONGBI (Ejderha Atýþý) için hasar iþlemesini düzelten fix (movement-only case kaldýrýlýp normal skill-case grubuna eklenir)
#define ENABLE_SKILL_PAERYONG_PROCESSING   			// SKILL_PAERYONG (Ejderha Kükresi) için ComputeSkill çaðrýsýný ekleyerek hasar/efekt iþlenmesini saðlar
/*** DÜZELTMELER ***/

/*** TAMAMLANMAMIÞ SÝSTEMLER (YAKINDA) ***/
#define ENABLE_GUILDRENEWAL_SYSTEM					// Geliþmiþ resmi gibi lonca altyapýsý için çekirdek yenileme sistemi
	#ifdef ENABLE_GUILDRENEWAL_SYSTEM
		#define ENABLE_USE_MONEY_FROM_GUILD			// Lonca Parasý ile ödeme yapmayý etkinleþtirir
		#define ENABLE_NEW_WAR_OPTIONS				// Tur, puan ve süre bazlý modern lonca savaþý seçenekleri
		#define ENABLE_EXTENDED_GUILD_LEVEL			// Lonca seviye kapasitesinin 30'a çýkarýlmasý
		//#define ENABLE_MEDAL_OF_HONOR				// Lonca ekonomisi için yeni para birimi (Onur Madalyasý)
		#define ENABLE_GUILD_DONATE_ATTENDANCE		// Günlük lonca baðýþý ve katýlým takip sistemi
		#define ENABLE_GUILD_WAR_SCORE				// Lonca savaþ istatistiklerinin detaylý skor ekraný
		#define ENABLE_GUILD_LAND_INFO				// Lonca arazilerine ait detaylý bilgi arayüzü
		#define ENABLE_GUILDBANK_LOG				// Lonca banka ve iþlem kayýtlarýnýn günlük sistemi
		#define ENABLE_GUILDBANK_EXTENDED_LOGS		// Geniþletilmiþ günlükler için Log.cpp baðlantýsý
		#define ENABLE_EXTENDED_RENEWAL_FEATURES	// Lonca lideri devri ve arazi silme yönetimi gibi ek yenileme özellikleri
		#define ENABLE_COLEADER_WAR_PRIVILEGES		// Lider çevrimdýþý olduðunda, Yardýmcý Lider lider ile ayný yetkilere sahip olur
		#define ENABLE_GUILDWAR_BUTTON				// Arayüzde Lonca Savaþý eriþim düðmesini aktif eder
		//#define ENABLE_GUILD_RANKING				// Loncalar arasý sýralama sistemi (tam entegre edilmediði için kapalý)
		//#define ENABLE_GUILDSTORAGE_SYSTEM        // Loncanýn ortak deposu: üyelerin paylaþýmlý eþya deposu sistemi
	#endif
/*** TAMAMLANMAMIÞ SÝSTEMLER (YAKINDA) ***/

/*
#@GENEL
@fixme433: char_affect.cpp dosyasýnda, Bedensel Savaþçý'nýn Hamle becerisi kullandýðýnda HP düþmesi, Büyülü Silah Sura’nýn Çözme ve Ýyileþtirici Þaman’ýn þifa gecikmesi hatasý düzeltildi.
*/
#endif
