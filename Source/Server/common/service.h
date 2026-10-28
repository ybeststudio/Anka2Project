/************************************************************
*                ANKA2 SYSTEM INFO GAME                     *
*-----------------------------------------------------------*
*   File Name : ../../common/service.h                      *
*   Author    : BestStudio                                  *
*   Version   : 3.1.0                                       *
*   Date      : 03-11-2025                                  *
*   Update    : 16-10-2026                                  *
*-----------------------------------------------------------*
*           Anka2Project Configuration Definition           *
************************************************************/

#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__
#pragma once

enum eAnkaDefines {
	//DEFINES						//VALUE				//AÇIKLAMA											// VARSAYILAN
	CONQUEROR_MAX_LEVEL				= 30,				// Maksimum þampiyon seviyesi							// 30		[ENABLE_YOHARA_SYSTEM]
	DROPABLE_GOLD_LIMIT				= 100000,			// Düþürülebilir altýn limiti						// 1000
	SB_NEED_EXP						= 20000,			// Becerikitabý için gereken deneyim				// 20000
	SB_C_NEED_EXP					= 20000,			// 9. beceriler için gereken deneyim				// 20000
};

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
#define ENABLE_MOB_DROP_INFO						// Mob ve taþ düþürme penceresini etkinleþtir;
#define ENABLE_SHOW_MOB_INFO						// Mob seviyesi ve saldýrganlýk bilgisini etkinleþtir;
#define ENABLE_RENEWAL_QUEST						// Resmi sunucularla ayný yeni görev sayfasý;
#define ENABLE_CHANGE_CHANNEL						// Kanal deðiþtirme seçeneðini etkinleþtir;
#define ENABLE_SEALBIND_SYSTEM						// Eþya mühürleme / ruha baðlama sistemi;
#define ENABLE_UN_SEAL_SCROLL_PLUS					// Ruh Baðý Ayýrma Parþömeni+ sistemi;
#define ENABLE_MESSENGER_TEAM						// Mesajlaþmada takým listesini etkinleþtir;
#define ENABLE_PENDANT_SYSTEM						// Resmi pendant sistemini etkinleþtir;
#define ENABLE_SUNGMA_SCORE							// Resmi SungMa iradesi puani sistemi;
#define ENABLE_DRAGON_SOUL_7_SLOT					// Resmi 7. Ejderha Ruhu Slotu (Amethyst Slot)
#define ENABLE_SET_ITEM								// Resmi Set Efekti Sistemi (Official Set Item System);
#define ENABLE_GEM_CONVERTER						// Resmi Gaya Donusturucu Sistemi (Official Gem Converter)
#define ENABLE_GAYA_STACK							// Resmi Gaya Istifleme / Secim Kontrolleri (Official Gaya Stack & Selection System)
#define ENABLE_GEM_BAG_ANTI_SELL_TOOLTIP			// Resmi Gaya Kesesi Satilamaz Bilgilendirme Tooltipi (Official Gem Bag Anti-Sell Tooltip)
#define ENABLE_AURA_CLEAR							// Resmi Aura Deger Temizleme Sistemi (Official Aura Clear System)
#define ENABLE_AURA_CLEAR_MESSAGE_WHEN_EQUIP		// Resmi Aura Giyiliyken Temizleme Engeli Uyarisi (Official Aura Clear Message When Equip)
#define ENABLE_MOVE_COSTUME_ACCE_ATTR				// Resmi Kostum ve Kusak Bonus Aktarimi Sistemi (Item Combination System v26.1.14)
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
#define ENABLE_NEW_BONUS_SYSTEM						// Patronlara karþý bonusu etkinleþtir;
	#ifdef ENABLE_NEW_BONUS_SYSTEM
		#define ENABLE_ATTBONUS_METIN				// Metin taslarina saldiri bonusu
		#define ENABLE_ATTBONUS_BOSS				// Patron yaratiklara saldiri bonusu
	#endif
#define ENABLE_AVG_PVM								// Yüksek ortalama zarar bonusu etkinleþtir;
	#ifdef ENABLE_AVG_PVM
		#define ENABLE_AVG_PVM_DISABLE				// Yüksek ortalama zarar bonusu etkinleþtir;
	#endif
#define ENABLE_MAGIC_REDUCTION_SYSTEM				// Büyü direnci & bozma bonusu etkinleþtir;
	#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		//#define USE_MAGIC_REDUCTION_STONES		// Büyü bozma taþlarýný etkinleþtir;
	#endif
#define ENABLE_HUMAN_RESIST_PVP						// Yarý insanlara karþý bonusu etkinleþtir;
#define ENABLE_RENEWAL_AFFECT						// Yenileme etkisini etkinleþtir (1 yeni alt tip + etki simgesi);
#define ENABLE_OFFLINE_MESSAGE						// Oyuncu baðlý deðilse çevrimdýþý mesaj býrakma seçeneðini etkinleþtir;
#define ENABLE_RESTART_INSTANT						// 500.000 Yang için anýnda yeniden baþlatmayý etkinleþtir;
#define ENABLE_NEW_DUNGEON_LIB						// Yeni zindan fonksiyonlarýný etkinleþtir;
#define ENABLE_RENEWAL_OX_EVENT						// Resmi gibi yenileme ox etkinliðini etkinleþtir;
#define ENABLE_MULTI_FARM_BLOCK						// 2'den fazla istemci için çoklu farm'ý imkansýz hale getir;
#define ENABLE_SPECIAL_INVENTORY					// Kitaplar, yükseltme vb. için özel envanter;
#define ENABLE_EXTEND_ITEM_AWARD                	// Geniþletilmiþ eþya ödül sistemini etkinleþtir (çoðunlukla Item Shop ödülleri için)
	#ifdef ENABLE_EXTEND_ITEM_AWARD
		#define USE_ITEM_AWARD_CHECK_ATTRIBUTES    // Ödül verilirken eþyalarýn attribute/özellik kontrolünü kullan
	#endif
#define ENABLE_MAINTENANCE_SYSTEM					// Oyunda bakým bilgisini etkinleþtir;
#define ENABLE_SKILL_COLOR_SYSTEM					// Yetenek renk sistemini etkinleþtir;
#define ENABLE_PICKUP_ITEM_EFFECT					// Envanter / özel envanterde alýnan eþyalar için efekti etkinleþtir;
#define ENABLE_RENEWAL_OFFLINESHOP					// Yeni offlineshop sistemini etkinleþtir;
#define ENABLE_RENEWAL_TEAM_AFFECT					// Yenileme takým etkisini etkinleþtir (logo);
#define ENABLE_SLOT_MARKING_SYSTEM					// Resmi gibi slot iþaretleme sistemini etkinleþtir;
#define ENABLE_RENEWAL_SKILL_SELECT					// Yetenek seçmek için yeni bir pencere etkinleþtir;
#define ENABLE_RENEWAL_SPECIAL_CHAT					// Yang, Eþyalar vb. için yenileme özel sohbeti etkinleþtir;
#define ENABLE_LARGE_DYNAMIC_PACKET					// Büyük dinamik paket boyutunu etkinleþtir;
#define ENABLE_CLIENT_LOCALE_STRING					// Ýstemciden yerel dize yüklemesini etkinleþtir;
#define ENABLE_BONUS_COSTUME_SYSTEM					// Kostümlerde bonus ekleme ve deðiþtirmeyi etkinleþtir;
#define ENABLE_GUILD_LEADER_TEXTAIL					// [Lider] ile lonca generali için metni etkinleþtir;
#define ENABLE_CROSS_CHANNEL_REQUESTS				// Kanal arasý istekleri etkinleþtir (gelecekte geniþletilecek);
#define ENABLE_AUTOMATIC_PICK_UP_SYSTEM				// Otomatik toplama sistemini etkinleþtir;
#define ENABLE_OFFLINESHOP_SEARCH_SYSTEM			// Yenileme offlineshop için arama sistemini etkinleþtir;
#define ENABLE_EMOTICONS_SYSTEM						// Sohbet ve fýsýldamada emoji sistemi;
#define ENABLE_ANTI_EQUIP_FLOOD						// Ekipman için hýzlý kontrolü etkinleþtir;
#define ENABLE_HANDSHAKE_SESSION					// El sýkýþma oturumu düzeltmesini etkinleþtir;
#define ENABLE_GROWTH_PET_SYSTEM					// Resmi evcil pet sistemini etkinleþtir;
#define ENABLE_CHANGE_LOOK_SYSTEM					// Görünüm deðiþtirme sistemini etkinleþtir;
#define ENABLE_KILL_STATISTICS						// Resmi ile neredeyse ayný yeni bonus panosu;
#define ENABLE_AURA_COSTUME_SYSTEM					// Aura kostüm sistemini etkinleþtir;
#define ENABLE_RENEWAL_TELEPORT_SYSTEM				// Yeni ýþýnlanma sistemini etkinleþtir;
#define ENABLE_INVENTORY_EXPANSION_SYSTEM			// Resmi gibi envanter geniþletme sistemini etkinleþtir;
#define ENABLE_GM_INV_EFFECT         				// Resmi GM /inv efekt güncellemesi
#define ENABLE_RIDING_EXTENDED         				// Resmi At seviyesi geniþletildi
#define ENABLE_BOT_PLAYER							// Sahte oyuncu sistemi
#define ENABLE_BUFFI_SYSTEM						// Buffi (destek saman) sistemi
#ifdef ENABLE_BUFFI_SYSTEM
	#define ENABLE_BUFFI_EVOLUTION					// Buffi evrimi (Lv 55/75/90, malzeme + yang)
	#define ENABLE_BUFFI_SKILL_BOOK					// Buffi beceri kitaplari (M -> G kitap, G -> P ruh tasi)
	#define ENABLE_BUFFI_CHANGE_SEX					// Buffi cinsiyet degistirme tilsimi
#endif
#define ENABLE_SKILL_UPGRADE						// Beceri Yukseltici (/upgrade_skill): kitap / ruh tasi okuma, 71001 / 71094 / 70102
#define ENABLE_STANDING_MOUNT						// Ayakta kullanýlan binek sistemini etkinleþtir	
	#ifdef ENABLE_STANDING_MOUNT	
		#define STANDING_MOUNT_VNUM_1 40003 			// Turbo Sörf Tahtasý
		#define STANDING_MOUNT_VNUM_2 40004 			// Wukong'un Fýrtýnasý
		#define STANDING_MOUNT_VNUM_3 40005 			// Wukong'un Gürlemesi
		#define SURFBOARD STANDING_MOUNT_VNUM_1 		// Eski sistemlerle uyumluluk için alias
		#define IS_STANDING_MOUNT_VNUM(v) ((v)==STANDING_MOUNT_VNUM_1 || (v)==STANDING_MOUNT_VNUM_2 || (v)==STANDING_MOUNT_VNUM_3)
	#endif
#define ENABLE_HUNTING_SYSTEM						// Avcýlýk görev sistemini etkinleþtir;
	#ifdef ENABLE_HUNTING_SYSTEM
		#define HUNTING_MISSION_COUNT 90        	// Ayný anda aktif olabilecek av görevi sayýsý
		#define HUNTING_MONEY_TABLE_SIZE 9     		// Her görev seviyesi için yang ödül tablosu giriþi sayýsý
		#define HUNTING_EXP_TABLE_SIZE 9       		// Her görev seviyesi için EXP tablosu giriþi sayýsý
	#endif
#define ENABLE_SKILL_BOOK_READING					// Beceri Kitabý Okuma sistemi
#define ENABLE_SPIRIT_STONE_READING					// Ruh Taþý Okuma sistemi
#define __AUTO_HUNT__								// Otomatik Av sistemi
#define ENABLE_AUTO_SELL_SYSTEM						// Otomatik item satmayý sistemini etkinleþtir;
	#ifdef ENABLE_AUTO_SELL_SYSTEM
		#define MAX_AUTO_SELL_ITEMS 100				// Listeye eklenecek maksimum item sayýsý
	#endif
#define ENABLE_PLAYERS_ONLINE                  		// Çevrimiçi oyuncu /players_online komutunu  etkinleþtir;
#define ENABLE_STYLE_ATTRIBUTE_SYSTEM      			// Seçmeli özel kadim efsunu sistemini  etkinleþtir;
#define ENABLE_ITEM_MODIFIER_AVG_SKILL				// Ortalama ve Beceri efsun nesnesi sistemini etkinleþtir;
#define ENABLE_RANDOM_STATUS_PER_LEVEL 				// Seviye atladýkça karaktere rastgele ek statüler kazandýrýr
#define ENABLE_PITTY_REFINE                    		// Yeni nesil yükseltme sistemi
#define ENABLE_NPC_LOCATION_TRACE  					// Belirtilen NPC veya mob'un harita üzerindeki konumunu tespit eder ve oyuncuya yönlendirme saðlar
#define ENABLE_ITEMSHOP								// Oyunda nesne market'i etkinleþtir;		
	#ifdef ENABLE_ITEMSHOP	  
		#define USE_ITEMSHOP_RENEWED			   	// Oyunda nesne market yenilemeyi etkinleþtir;
	#endif
#define ENABLE_BATTLE_PASS                      	// Yenileme savaþ bileti sistemini etkinleþtir;
	#ifdef ENABLE_BATTLE_PASS                      	// Yenileme savaþ bileti sistemini etkinleþtir;
		//#define ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	#endif
#define ENABLE_BIOLOGIST_SYSTEM						// Biyolog sistemini etkinleþtir
#define ENABLE_AFFECT_BUFF_REMOVE              		// Buff Etkilerini Kaldýrma
#define RENEWAL_HWID_BLOCK    						// Geliþtirilmiþ HWID doðrulama ve engelleme sistemini etkinleþtirir
#define __LEADERSHIP__BONUS__                  		// Liderlik Bonusu
#define ENABLE_NEWSTUFF    							// Yeni eklenen sistemleri, özellikleri veya geliþtirmeleri toplu olarak aktif etmek için kullanýlan genel kontrol bayraðý
#define ENABLE_DUNGEON_RENEWAL_SYSTEM      			// Zindan sistemini tümüyle etkinleþtirir
	#ifdef ENABLE_DUNGEON_RENEWAL_SYSTEM
		#define ENABLE_DUNGEON_INFO           		// Zindanla ilgili detaylý bilgi (harita, süre, ödüller) gösterim desteðini saðlar
		#define __DUNGEON_INFO__             		// Yeni tip zindan bilgisi patron mekaniklerini aktif eder
		#define ENABLE_NEW_DUNGEON_LIB       		// Zindan iþlevleri için yeni kütüphane/alt sistem kullanýmýna geçirir
		#define ENABLE_TRACK_WINDOW          		// Zindan takibi için özel UI/pencere (track window) desteðini etkinleþtirir
		#define ENABLE_ULTIMATE_REGEN        		// Geliþmiþ/alternatif "ultimate" regen algoritmasýný devreye alýr
	#endif
#define ENABLE_YOHARA_SYSTEM          				// Yohara kýtasý ve SungMa tabanlý sistemleri etkinleþtirir
	#ifdef ENABLE_YOHARA_SYSTEM
		#define ENABLE_CONQUEROR_LEVEL				// Þampiyon seviyesi sistemini etkinleþtir;
		#define ENABLE_CONQUEROR_LEVEL_UPDATE		// Þampiyon seviyesi anýnda yenilemeyi etkinleþtir;
		#define ENABLE_PASSIVE_SYSTEM				// Resmi Kalýntý Bonusu sistemi etkinleþtirir;
		#define ENABLE_NINETH_SKILL					// 9. becerileri etkinleþtir
		#define ENABLE_PVP_BALANCE					// Yeni skill animasyonlarý, resmi oyun gibi
	#endif
#define ENABLE_DUNGEON_MAPS_SYSTEM        			// Tüm yeni/yenilenmiþ zindan ve harita altyapýsýný etkinleþtirir
	#ifdef ENABLE_DUNGEON_MAPS_SYSTEM
		// Yohara Zindanlarý Start
		#define ENABLE_SUNG_MAHI_TOWER    			// SungMa Ýstilasý zindaný
		#define ENABLE_WHITE_DRAGON       			// Alastor (Kuzey Rüzgarý Derinlikleri)
			#ifdef ENABLE_WHITE_DRAGON
				//#define ENABLE_WHITE_DRAGON_EX 	// Alastor canavar / event uzantýlarý
			#endif
		#define ENABLE_QUEEN_NETHIS       			// Yýlan Tapýnaðý (Queen Nethis)
		// Yohara Zindanlarý End

		#define ENABLE_MELEY_LAIR_DUNGEON     			// Meley'in Ýni haritasýný etkinleþtir
		#define ENABLE_ZODIAC_MISSION     				// Zodyak Tapýnaðý görev sistemi
		#define ENABLE_DRAGON_LAIR            			// Mavi Ejderha Odasý'ný etkinleþtir
		#define ENABLE_DEFENSAWE_SHIP         			// Resmi sürümdeki Gemi Savunma Zindaný’ný etkinleþtir
		#define ENABLE_OCHAO_TEMPLE_SYSTEM              // Ochao Tapýnaðý ve Büyülü Orman zindanýný etkinleþtirir
			#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM		
				#define OCHAO_HEALING_SKILL_VNUM 265    // 1.62 sürümü için Ochao heal skill vnum tanýmý
			#endif
	#endif
#define __DOJANG_SRC_FUNCTIONS__      				// Turnuva (PvE/PvP arena) kaynak fonksiyonlarýný açar
	#ifdef __DOJANG_SRC_FUNCTIONS__
		#define DOJANG_MAPINDEX 92        			// Turnuva harita index deðeri
	#endif
#define ENABLE_REMOTE_SHOP_SYSTEM                   // Uzaktan Market sistemini etkinleþtirir;
#define __SYSTEM_SEARCH_ITEM_MOB__  				// Mob kodu girildiðinde o mobdan düþen item’leri panel üzerinden listelemeyi saðlar.
#define ENABLE_REFINE_SUCCESS_NOTICE				// Ýtem son refine seviyesine (+9) ulaþtýðýnda global duyuru gönderir.
	#ifdef ENABLE_REFINE_SUCCESS_NOTICE
		#define REFINE_SUCCESS_NOTICE_LEVEL 9		// Global duyuru gönderilecek minimum refine seviyesi.
	#endif
#define ENABLE_EQUIPMENT_HAND_EFFECT				// Karakterin sað ve sol eline takýlan ekipmanlar için özel efekt desteði saðlar.
#define ENABLE_RESP_SYSTEM							// Patron ve Metin ýþýnlanma sistemini etkinleþtirir
#define ENABLE_REAL_TIME_REGEN						// Karakterlerin HP/SP yenilenmesinin gerçek zamanlý (online) olarak çalýþmasýný saðlar
#define ENABLE_COLLECT_WINDOW                 		// Koleksiyon sistemine ait GUI (pencere) arayüzünü aktif eder
#define __ENABLE_COLLECTIONS_SYSTEM__         		// Koleksiyon (Collections) sisteminin tamamýný aktif eder
#define ENABLE_NEW_ITEM_TYPE_GACHA          		// Gacha sistemi için yeni sandýk / item tipini aktif eder
#define ENABLE_RANKING          					// Rank sýralama sistemi
#define ENABLE_MOB_FOLLOW_SYSTEM          			// Canavar (Mob) takip sistemi
#define ENABLE_ITEM_UPGRADE_NOTICE          		// Belirlenen seviye ve üzerindeki eþya geliþtirmelerini global olarak duyurur.
	#ifdef ENABLE_ITEM_UPGRADE_NOTICE
		#define ITEM_UPGRADE_NOTICE_MIN_LEVEL 9 	// Duyuru yapýlacak minimum geliþtirme seviyesi (örn: +9 ve üzeri).
	#endif
#define ENABLE_GAYA_SYSTEM                         	// Gaya sistemini (Gaya para birimi, NPC ve takas mekanikleri) aktif eder
	#ifdef ENABLE_GAYA_SYSTEM
		#define ENABLE_GAYA_SHOP_SYSTEM             // Gaya para birimi ile çalýþan market / alýþveriþ sistemini aktif eder
		#define ENABLE_GAYA_TICKET_SYSTEM           // Gaya sistemi için kupon (bilet) kullanýmý ve dönüþüm mekanizmasýný aktif eder
	#endif
#define ENABLE_EVENT_SYSTEM                         // Etkinlik yöneticisini (otomatik ve zamanlý etkinlikler) aktif eder
	#ifdef ENABLE_EVENT_SYSTEM
		#define ENABLE_EVENT_BANNER_FLAG            // Etkinlikler için oyun içi banner / bayrak gösterim sistemini aktif eder
		#define ENABLE_SOUL_ROULETTE_SYSTEM         // Kan Ritüeli (Soul Roulette) mini oyun / etkinlik sistemini aktif eder
		#define ENABLE_ATTENDANCE_EVENT             // Canavar Avý (Attendance) etkinliðini ve ödül mekanizmasýný aktif eder
		#define ENABLE_FISH_EVENT                   // Resmi yeni balýk tutma mini oyununu aktif eder
		#define ENABLE_MINI_GAME_CATCH_KING         // Kralý Yakala (Catch the King) kart mini oyununu aktif eder
		#define ENABLE_MINIGAME_RUMI_EVENT          // Rumi (Okey kartlarý) mini oyun sistemini aktif eder
		#define ENABLE_HALLOWEEN_EVENT_SYSTEM       // Cadýlar Bayramý (Halloween) etkinlik sistemini aktif eder
		#define ENABLE_STONE_EVENT_SYSTEM           // Metin Taþý kesme / sýralama etkinlik sistemini aktif eder
		#define ENABLE_SOCCER_BALL_EVENT            // Futbol Topu etkinliðini ve ilgili oyun mekaniklerini aktif eder
		#define ENABLE_WORD_GAME_EVENT              // Kelime oyunu etkinliðini aktif eder
		#define ENABLE_EVENT_DROPS                  // Etkinliklere özel item düþürme (drop) ayarlarýný aktif eder
	#endif
#define ENABLE_DROP_GOLD							// Yang düþürme sistemi
#define ENABLE_GOLD_MULTIPLIER						// Yang çarpan sistemi
#define ENABLE_GOLD10_PCT							// Yang %10 çarpaný
#define ENABLE_STATUS_UP_RENEWAL					// Hýzlý statü sistemi etkinleþtir;
#define ENABLE_TITLE_SYSTEM							// Resmi Ünvan Sistemi etkinleþtirir;

/*** KRÝTÝK HATA, ÇÖKME VE AÇIK DÜZELTMELERÝ (CRASH / CORE / BUG / EXPLOIT / LEAK) ***/
#define __FIX_TIMER_EVENT__ 						// Timer event (zamanlayýcý) gecikme ve tetiklenme hatalarýný düzeltir
#define __FIX_SECONDARY_SKILL__ 					// Ýkincil skill (secondary skill) kullaným ve hesaplama hatalarýný düzeltir
#define ENABLE_COUNT_MONSTER_FIX 					// Canavar sayacý (kill count) hatasýný düzeltir
#define FLUSH_AT_SHUTDOWN							// Sunucu kapanýrken verileri zorla diske yazar (veri kaybýný önler)
#define __FIX_INFO_REFINE_DRAGONSOUL__ 				// Ejderha taþý simyasý refine bilgi/tooltip hatasýný düzeltir
#define __FIX_COSTUM_NUNTA_PESTE_COSTUM_NORMAL__ 	// Kostüm slot çakýþmasýný düzeltir (smokin & gelinlik kostüm normal kostümü etkilemez)
#define FIX_BLOCK_MOB_SAFEZONE 						// Güvenli bölgede mob saldýrýsýný engeller
#define DISABLE_STOP_RIDING_WHEN_DIE				// Karakter öldüðünde binek ve sürüþ durumunu temizler.
#define ENABLE_STONES_STACKFIX   					// Taþ itemlerinde stack (birikme) iþlemlerindeki hatalarý düzeltmek için özel split ve ekleme mantýðýný aktif eder.
#define ENABLE_TELEPORT_SKILL						// Her ýþýnlanmada becerilerin tekrar sönme sorununu giderir.
#define ENABLE_BOSS_METIN_POISON_DISABLE    		// Boss ve Metin taþlarýna karþý zehir etkisini devre dýþý býrakýr.
#define DISABLE_STAMINA    							// Stamina (ST) sistemini devre dýþý býrakýr; karakter yorulmaz ve ST tüketimi gerçekleþmez.
#define ENABLE_FIX_SWAP_ITEM						// Slot uyuþmazlýðý nedeniyle ekipman deðiþtirilememesi sorununu giderir.
#define ENABLE_RELOAD_COMMAND_ALL_CORES  			// Reload komutunun tüm core’lara P2P üzerinden senkron þekilde iletilmesini saðlar.
#define ENABLE_ITEM_DUPE_FIX 						// Eþya iþlemlerinde oluþan senkronizasyon ve zamanlama hatalarýndan kaynaklý çoðaltma açýklarýný düzeltir.
#define ENABLE_FIX_EXP_DROP_STONES 					// Metin (Stone) yaratýklarýndan EXP düþme hesaplamalarýnda oluþan hatalarý giderir.
#define ENABLE_MULTI_AFFIX_APPLY_SYSTEM 			// Tek iþlemde birden fazla efsun (bonus) eklenmesini saðlar.
#define ENABLE_REGEN_RENEWAL 						// Regen event’lerinin baþlatýlma zamanýný sabitleyerek rastgele gecikmeleri ortadan kaldýrýr ve daha tutarlý spawn döngüsü saðlar.
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
#define ENABLE_SKILL_PAERYONG_PROCESSING   			// SKILL_PAERYONG (Ejderha Kükresi) için ComputeSkill çaðrýsýný ekleyerek hasar/efekt iþlenmesini saðlar
#define ENABLE_HORSE_SKILL_DAMAGE					// At üzerinde Sura beceri vurma düzeltmesini etkinleþtirir
#define ENABLE_ANALYZE_CLOSE_FIX 					// Analiz/inceleme arayüzünün beklenmedik þekilde kapanmasýna neden olan hatalarý düzeltir.
#define ENABLE_CRASH_CORE_ARROW_FIX 				// Okçu sýnýfýnýn Core (çekirdek) kaynaklý çökme (crash) hatasýný giderir.
#define ENABLE_DUNGEON_MUSIC_FIX  					// Zindanlarda müziðin çalmamasý veya hatalý çalmasý sorununu düzeltir.
#define ENABLE_FLY_FIX 								// Fly senkronizasyonunu milisaniye bazlý kontrol ederek exploit ve desync riskini azaltýr.
#define ENABLE_CHANGE_SKILL_VISUAL_BUG_FIX  		// Skill deðiþiminde oluþan görsel buff/efekt hatalarýný düzeltir.
#define ENABLE_CHANGE_SEX_WITHOUT_RELOG_FIX  		// Karakter cinsiyeti deðiþtirildiðinde relog gerektiren senkronizasyon hatasýný düzeltir.
#define ENABLE_CH_CRASH_CORE_FIX  					// Quest fonksiyonlarýnda geçersiz karakter pointer’ý nedeniyle oluþan crash hatasýný önler.
#define ENABLE_KILL_EVENT_FIX  						// Öldürme etkinlik'lerinde hasara göre doðru oyuncunun seçilmesini saðlar.
#define ENABLE_CUBE_RELOAD_FIX  					// Craft tariflerinin server restart olmadan güvenli þekilde yeniden yüklenmesini saðlar.
#define ENABLE_EXP_GROUP_FIX  						// Grup EXP daðýtýmýnýn yalnýzca parti lideri tarafýndan yapýlmasýný saðlar.
#define ENABLE_FISHING_TIME_INC  					// Balýkçýlýk bekleme süresini düþürerek daha hýzlý balýk tutmayý saðlar.
#define ENABLE_DISABLE_EMPIRE_DAMAGE_BONUS			// Baþka bir imparatorluktan geliyorsanýz, hasar %10 azalýr.
#define ENABLE_STONES_STACKFIX  					// Ýstiflenen taþlarýn refine sýrasýnda yanlýþ silinmesini ve dupe oluþmasýný önler.
#define ENABLE_ITEM_AWARD_FIX  						// Ödül öðesi iþlemlerinde veri kalýntýlarýndan kaynaklý hatalarý önler.
#define ENABLE_SKILL_COOLDOWN_CHECK  				// Skill kullanýmýnda server-side cooldown ve spam kontrolü saðlar.
#define ENABLE_SAFE_LEVEL_CHANGE_FIX  				// Seviye deðiþiminde PointChange senkronizasyon hatalarýný ve exploit riskini önler.
/*** KRÝTÝK HATA, ÇÖKME VE AÇIK DÜZELTMELERÝ (CRASH / CORE / BUG / EXPLOIT / LEAK) ***/


/* === ifdef-audit: definesiz eklenmis custom sistemler icin merkezi define'lar (hepsi ON; davranis degismez) === */
#define ENABLE_DRAGON_LAIR_RENEWAL_IMMUNE		// Dragon Lair v17.3 yeni baginsizlik blogu (char_battle)
#define ENABLE_PCSHOP_GOLD_OVERFLOW_FIX			// PC-shop satici altin tasma kontrolu (shop.cpp)
#define ENABLE_SHOP_STACK_MERGE					// Satin alinan istiflenebilir esyalarin mevcut yigina eklenmesi (shop/shopEx)
#define ENABLE_SHOP_REMOVE_ALL_GUESTS			// CShop::RemoveAllGuests (shop.cpp)
#define ENABLE_ITEM_ATTR_CHANGE_LOG				// SET_ATTR/SET_FORCE_ATTR degisiklik loglama (item_attribute)
#define ENABLE_QUEST_ITEM_FUNCTIONS				// questlua_item ozel item accessor fonksiyonlari
#define ENABLE_CHANGE_SEX_QUEST					// questlua_pc pc.change_sex_degisemen
#define ENABLE_GET_COSTUME_QUEST				// questlua_pc pc.get_costume
#define ENABLE_BLOCK_EXP						// questlua_pc _block_exp/_unblock_exp/_getexp_blocked
#define ENABLE_QUEST_MYSQL_QUERY				// questlua_game game.mysql_query
#define ENABLE_BIG_MAP_NOTICE					// questlua_global big_map_notice
#define ENABLE_NPC_IS_STONE						// questlua_npc npc.is_stone
#define ENABLE_PARTY_MEMBER_COUNT				// questlua_party party.get_member_count
#define ENABLE_AUTO_CAPE_SYSTEM					// cmd_general auto-cape / advanced_options.cape_*
#define ENABLE_CONFIRM_LEVELUP					// cmd_general do_confirm_levelup
/* === ifdef-audit end === */
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

#@common
@fixme301: tables.h üzerinde; TPlayerTable hp/mp int16_t'den int'e deðiþtirildi (hp/mp >32767 olmalý)

#@db/src
@fixme201: ProtoReader.cpp üzerinde; 'SAMLL' 'SMALL' olarak deðiþtirildi
@fixme202: ClientManagerGuild.cpp üzerinde; eðer oyuncu çevrimdýþý ise lonca üyesi çýkarma zamaný sorunu düzeltildi 
			(withdraw_time -> new_withdraw_time)
@fixme203: ClientManagerPlayer.cpp üzerinde; "command" için boþta kalan gösterici (dangling pointer)
@fixme203: ClientManagerPlayer.cpp üzerinde; "command" için boþta kalan gösterici (dangling pointer)
@fixme204: Cache.cpp üzerinde; kiþisel dükkanda ayný vnum’a sahip çok fazla eþya varsa, myshop_pricelist birincil anahtar çoðaltma hatasý

#@game/src
@fixme102: cmd_general.cpp üzerinde ACMD(do_war) içinde unsigned hatasý düzeltildi.
@fixme103: config, input_login, input_main.cpp dosyalarýnda clientcheckversion (sürüm > tarih) ifadesi (sürüm != tarih) ve gecikme süresi 10’dan 0’a olarak deðiþtirildi.
@fixme104: char.cpp, questlua_pc.cpp dosyalarýnda lv90'dan sonra statü puaný alma düzeltildi; 90 deðeri gPlayerMaxLevel ile deðiþtirildi.
@fixme106: input_main.cpp dosyasýnda, yüksek hýzdaki bir oyuncu binekteyken (örneðin, aslan) mesafe sýnýrý nedeniyle geri getirilecektir.
@fixme108: char.cpp dosyasýnda, mevcut binek 0 deðilken bineði deðiþtirdiðinizde, yalnýzca oyuncunun ekranýndaki tüm nesneler (npc vb.) kaybolur.
@fixme109: questmanager.cpp dosyasýnda, bir oyuncu öldürüldüðünde (savaþ m), `when kill begin` iki kez tetiklenir.
@fixme110: char_affect.cpp dosyasýnda, yarý saydamken (yeniden canlanmýþ, ninja yeteneði veya beyaz bayrak) saldýrý yaptýðýnýzda hala yarý saydam kalýrsýnýz.
@fixme112: char_item.cpp dosyasýnda, tekrar giriþ yapýlana kadar ekipman üzerindeki bonuslar deðiþtirilebilir.
@fixme113: char_item.cpp dosyasýnda taþlarý çýkarýrken, bonus kaybý olmadan çýkarýlabilir.
@fixme114: char_item.cpp dosyasýnda, #111, #112 ve diðer birkaç hata bir araya getirildi.
@fixme115: char_item.cpp dosyasýnda, partideyseniz ve eþyalarýn sahibi siz deðilse, yerdeki tüm eþyalar alýnabilir.
@fixme116: char_skill.cpp dosyasýnda, normal at binme yetenekleri hasar veremez.
@fixme117: char_item.cpp dosyasýnda, envanter doluyken ekipman deðiþtirilemez ve boþ olmadýðýnda gereksiz kemer deðiþimleri engellenir.
@fixme118: char.cpp dosyasýnda ComputePoints çaðrýldýðýnda, ekipman bonuslarý kadar hp/mp kazanýrsýnýz.
@fixme119: input_main.cpp dosyasýnda, kasadaki/alýþveriþ merkezindeki eþyalar kemer envanterine tür kontrolü olmadan yerleþtirilebilir.
@fixme423: char_battle.cpp dosyasýnda, silahsýzken bufflar devre dýþý býrakýldý (VZK, AURA).
@fixme433: char_affect.cpp dosyasýnda, Bedensel Savaþçý'nýn Hamle becerisi kullandýðýnda HP düþmesi, Büyülü Silah Sura’nýn Çözme ve Ýyileþtirici Þaman’ýn þifa gecikmesi hatasý düzeltildi.
@fixme435: char_battle.cpp dosyasýnda, zehir canavarlarý artýk agresifleþtirmeyecek.
@fixme463: input_main.cpp dosyasýnda, 40. seviyenin altýnda lonca kurulamýyor.
@fixme469: char_battle.cpp dosyasýnda, arena haritasýnda hiçbir þey býrakýlamaz.
@fixme499: battle.cpp dosyasýnda saldýrý mesafesi düzeltildi.
@fixme502: char_item.cpp dosyasýnda kamp ateþi düzeltmesi.
@fixme503: battle.cpp ve pvp.cpp dosyalarýnda bekleme hack düzeltmesi.
@fixme504: char_battle.cpp dosyasýnda düzeltme yapýldý.
@fixme522: char_battle.cpp ve char_item.cpp dosyalarýnda farklý haritalarda Grup Exp/Yang/Düþüþ Paylaþýmý düzeltildi.
@fixme541: Performans düzenlemesi [Önek ++/-- operatörleri, ilkel olmayan türler için tercih edilmelidir.]
*/
#endif