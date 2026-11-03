C:\Anka2Project projesinde yarım kalan oyun seçenekleri entegrasyonunu tamamla.

Referans istemci dosyaları:
C:\Users\BestStudio\baerim_unpack

Önce şu devam raporunu oku:
C:\Anka2Project\Tools\options_functionality_audit\CLIENT_OPTIONS_20261014.md

KAPSAM:
Şimdilik yalnızca istemci tarafını tamamla. Yeni sunucu mekanikleri kurma, sunucu kaynaklarını değiştirme. Buffi, lonca görevi, rehber ve diğer sunucuya bağlı mekanikleri sonraki çalışmaya bırakıyoruz. Bunların istemci arayüzünü/protokol alıcılarını hazırlayabilirsin; fakat sunucu desteği olmadan çalışıyor diye gösterme.

MEVCUT DURUM:
Python seçenek arayüzünün büyük kısmı hazırlanmış ve bazı eksik C++ bağlantıları eklenmiş durumda. Son çalışmada:
- Gerçek arayüz ölçeği ve fare koordinat dönüşümü
- Efekt örneğine özel hasar rengi
- Mevcut lonca verisinden rütbe gösterimi
- 200 kayıtlık bildirim geçmişi ve filtreler
- Ctrl + sol tıklama ile en fazla 8 metinlik hedef sırası
- Metin sırası işaretleri, renk/görünüm seçimi ve bitiş uyarısı
eklendi.

Release x64 derlemesi 0 hata/17 uyarıyla geçti. Gerçek Python 2.7.3 üzerinde 23 test geçti. 459 referans UI düğümü kontrol edildi. Client EXE ve root paketi güncellendi. Bunlar canlı oyun testi değildir; oyun içi doğrulama henüz yapılmadı.

KALAN 14 KONTROL:
1. view_drop_ui
2. view_guideline
3. guild_quest_window
4. auto_unequip_buffi_pvp
5. auto_equip_buffi_pvm
6. hide_buffi
7. event_stone_index
8. auto_cape_after_warp
9. disable_level_up_confirmation
10. hide_metin_spawns
11. shooting_stars
12. speedtree_dynamic_light
13. speedtree_enhanced_foliage
14. speedtree_falling_leaves

Bu listenin tamamının saf istemci işi olduğunu varsayma. Her kontrolü referans davranışı, mevcut kaynak ve gereken varlık/protokol açısından incele. İstemci tarafında gerçekten uygulanabilenleri tamamla; sunucu gerektirenleri nedenleri ve hazır istemci bağlantılarıyla açıkça raporla.

ÖNEMLİ DOSYALAR:
- Python kaynakları:
  C:\Anka2Project\Tools\binary_unpack\root
- Seçenek davranışları:
  advanced_options_runtime.py
- Seçenek arayüzü:
  uiadvancedgameoptions.py
- Oyun bağlantıları:
  game.py, interfacemodule.py
- Yeni modüller:
  advanced_option_notifications.py
  advanced_metin_queue.py
- C++ kaynakları:
  C:\Anka2Project\Source\Client\source
- Solution:
  C:\Anka2Project\Source\Client\client.sln

Runtime içindeki UNAVAILABLE_OPTIONS şu anda 14 eksik kontrolü işaretliyor. Bir kontrolü yalnızca ayarı kaydedildiği için bu listeden çıkarma. Gerçek davranış bağlantısı kurulmalı.

REFERANS ANALİZİ:
C:\Anka2Project\Tools\options_functionality_audit altında:
- reference_native_apis.json
- reference_options_annotated.c
- reference_option_symbols.json
- extract_reference_image.py
- reference_api_map.py

Önce mevcut analizleri kullan; ağır analizi gereksiz yere baştan çalıştırma. Referans dump ve çıkarılmış PE yalnızca statik analiz içindir, çalıştırma.

Referans testleri:
C:\Users\BestStudio\baerim_unpack\out\test
İlgili dosyalar:
- test_levelup_confirmation_client.py
- test_guild_quest_client.py
- test_guideline_client.py

Referanstaki /confirm_levelup ve /get_guild_quest_reward komutlarının Anka sunucusunda bulunduğunu varsayma. Event stone seçimi referansta SetEventModelStatusFlag yoluna bağlı; uyumlu modelleri belirlemeden rastgele model kullanma. Anka BotPlayer buff sistemi ile referans Buffi sistemini aynı kabul etme.

ERTELENEN SUNUCU YAMALARI:
C:\Anka2Project\Tools\options_functionality_audit\pending_server_20261015
- dungeon.cpp.patch
- mining.cpp.patch

Bu yamaları şimdi uygulama. Zindan bildirim kategorisi ve madencilik bitişi istemci alıcıları hazır olsa da mevcut sunucunun bu olayları gönderdiği doğrulanmış değil.

ÇALIŞMA KURALLARI:
- Mevcut değişiklikleri ve çalışan özellikleri koru.
- Değiştireceğin dosyaları ayrı bir yedek klasörüne al.
- Python 2.7 uyumluluğunu koru; Python 3 bytecode dağıtma.
- game.py ve bazı C++ dosyalarında eski kodlamalar var. Dosyaları körlemesine UTF-8’e dönüştürme; kodlama ve satır sonlarını koru.
- Referanstaki görünüm ve davranışı kaynak/varlık kanıtıyla karşılaştır.
- Sahte başarı, boş fonksiyon veya yalnızca tercih kaydıyla “tamamlandı” sonucu üretme.
- Karşılaştırma ve plan aşamasında durma; uygulanabilir istemci eksiklerini kodla, doğrula ve paketle.
- Gerçekten eksik varlık/protokol varsa kesin dosya/API ihtiyacını belirt.

DOĞRULAMA:
Mevcut testler:
C:\Anka2Project\Tools\Tests\test_options_runtime.py
C:\Anka2Project\Tools\Tests\test_option_notifications.py
C:\Anka2Project\Tools\Tests\test_metin_queue.py
C:\Anka2Project\Tools\Tests\verify_options_reference.py
C:\Anka2Project\Tools\Tests\verify_options_layout.py
C:\Anka2Project\Tools\Tests\verify_options_pack.py

Python 2.7:
C:\Anka2Project\Tools\archiver\tools\Python27\python.exe
Çalıştırırken PATH’e C:\Anka2Project\Tools\archiver eklenmesi gerekiyor.

MSBuild:
C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe
Yapılandırma: Release / x64.

PAKETLEME:
C:\Anka2Project\Tools\archiver\xml\root_create.xml
Yeni Python modüllerinin hem .py hem gerçek Python2 .pyc girdilerini ekle.

Mevcut yardımcılar:
C:\Anka2Project\Tools\options_functionality_audit\stage_complete.py
C:\Anka2Project\Tools\options_functionality_audit\deploy_complete.py
Bunları okuyup kendi değişikliklerine göre güncelle; doğrulamadan dağıtım yapma.

Hedefler:
C:\Anka2Project\Client\Metin2Release_x64.exe
C:\Anka2Project\Client\pack\root

Önce derle, test et, geçici root paketini kaynaklarla karşılaştır. Sonra mevcut Client dosyalarını yedekleyerek güncelle. Oyun çalışıyorsa dosyaları zorla değiştirme.

SONUÇ:
Devam raporunu güncelle. Her eksik kontrol için “tamamlandı”, “sunucu bekliyor” veya “varlık/istemci işi kaldı” durumunu ve somut gerekçesini yaz. Derleme/test/paket doğrulamasını canlı oyun kanıtından ayrı belirt. Son yanıtını Türkçe ve kısa ver.