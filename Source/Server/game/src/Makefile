# ==============================================================================
# >  Metin2 Game Sunucusu Derleme Sistemi
# > Updated: Kasim 2025
# > Purpose: Game sunucusunu Clang++ ile derleyin
# ==============================================================================

# >> DERLEYICI VE VERSIYON <<
CC = clang++
GAME_VERSION = 40250

# >> DOSYA SISTEMI YAPILANDIRMASI <<
INCDIR =
LIBDIR =
BINDIR = ..
OBJDIR = .obj
GAMEDIR = /usr/home/share/bin
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

# >> YAPILANDIRMA SABITLERI <<
ENABLE_GCC_AUTODEPEND = 1

# >> BAGIMLILIK DOSYASI AYARI <<
ifneq ($(ENABLE_GCC_AUTODEPEND), 1)
DEPFILE = Depend
endif

# >> STANDART KUTUPHANELER <<
LIBS = -lm -lmd

# >> DERLEYICI BAYRAKLARI <<
CFLAGS = -g -w -Ofast -pipe -fexceptions -Wno-enum-constexpr-conversion -std=c++2a -static-libstdc++ -pthread -D_THREAD_SAFE -DNDEBUG

# >> GUVENLIK OZELLIKLERI <<
CFLAGS += -fstack-protector-all

# >> VERSIYON VE SISTEM BILGISI <<
CFLAGS += -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__GAME_VERSION__=\"$(GAME_VERSION)\"

# >> HARICI KUTUPHANE AYARLARI <<

# OpenSSL Sifreleme
INCDIR += -I/usr/include
LIBS += -lssl -lcrypto -lcryptopp

# DevIL Grafik Kutuphanesi
INCDIR += -I../../library/libdevil
LIBDIR += -L../../library/libdevil
# lcms: FreeBSD'de pkg install lcms (veya lcms2 ise -llcms2 kullan)
LIBS += -lIL -lpng -ltiff -lmng -ljpeg

# MySQL Veritabani
INCDIR += -I/usr/local/include/mysql
LIBS += /usr/local/lib/mysql/libmariadbclient.a /usr/lib/libz.a

# >> PROJE KUTUPHANELERI <<
INCDIR += -I/usr/local/include
LIBDIR += -L/usr/local/lib

INCDIR += -I../../library/liblua/include
INCDIR += -I../../library/msl
INCDIR += -I../../library
INCDIR += -I../../library/nlohmann
LIBDIR += -L../../library/libthecore/lib -L../../library/libpoly -L../../library/libsql -L../../library/libgame/lib -L../../library/liblua/lib
LIBS += -lthecore -lpoly -llua -llualib -lsql -lgame

# >> HEDEF DOSYA <<
TARGET = $(BINDIR)/game

# >> KAYNAK DOSYA LISTESI <<
MAINCPP = main.cpp
CFILE = minilzo.c

CPPFILE =	affect.cpp\
			ani.cpp\
			arena.cpp\
			auto_event_list.cpp\
			auto_event_manager.cpp\
			BattleArena.cpp\
			banword.cpp\
			battle.cpp\
			blend_item.cpp\
			BlueDragon_Binder.cpp\
			BlueDragon.cpp\
			BotPlayer.cpp\
			buffer_manager.cpp\
			buff_on_attributes.cpp\
			building.cpp\
			char_affect.cpp\
			char_aura.cpp\
			char_battle.cpp\
			char_cards.cpp\
			char_change_empire.cpp\
			char_dragonsoul.cpp\
			char_gem.cpp\
			char_horse.cpp\
			char_hunting.cpp\
			char_item.cpp\
			char_manager.cpp\
			char_pass.cpp\
			char_pass_premium.cpp\
			char_quickslot.cpp\
			char_resist.cpp\
			char_skill.cpp\
			char_state.cpp\
			char.cpp\
			changelook.cpp\
			cmd_emotion.cpp\
			cmd_general.cpp\
			cmd_gm.cpp\
			cmd_oxevent.cpp\
			cmd.cpp\
			CollectionsSystem.cpp\
			config.cpp\
			constants.cpp\
			crc32.cpp\
			cube_renewal.cpp\
			CsvReader.cpp\
			db.cpp\
			desc_client.cpp\
			desc_manager.cpp\
			desc_p2p.cpp\
			desc.cpp\
			dragon_soul_table.cpp\
			DragonLair.cpp\
			DragonSoul.cpp\
			dungeon_info.cpp\
			dungeon.cpp\
			empire_text_convert.cpp\
			entity_view.cpp\
			entity.cpp\
			event_queue.cpp\
			event.cpp\
			exchange.cpp\
			file_loader.cpp\
			fishing.cpp\
			FSM.cpp\
			gm.cpp\
			group_text_parse_tree.cpp\
			growth_pet.cpp\
			guild_manager.cpp\
			guild_war.cpp\
			guild.cpp\
			halloween.cpp\
			horsename_manager.cpp\
			horse_rider.cpp\
			input_auth.cpp\
			input_db.cpp\
			input_login.cpp\
			input_main.cpp\
			input_p2p.cpp\
			input_udp.cpp\
			input.cpp\
			ip_ban.cpp\
			item_addon.cpp\
			item_attribute.cpp\
			item_manager_idrange.cpp\
			item_manager_read_tables.cpp\
			item_manager.cpp\
			item.cpp\
			locale_service.cpp\
			locale.cpp\
			log.cpp\
			login_data.cpp\
			lzo_manager.cpp\
			maintenance.cpp\
			map_location.cpp\
			MarkConvert.cpp\
			MarkImage.cpp\
			MarkManager.cpp\
			marriage.cpp\
			messenger_manager.cpp\
			minigame_attendance.cpp\
			minigame_catch_king.cpp\
			mining.cpp\
			mob_manager.cpp\
			motion.cpp\
			mount_up_grade.cpp\
			MountSystem.cpp\
			new_mob_timer.cpp\
			offlineshop_manager.cpp\
			offlineshop_search_manager.cpp\
			offline_shop.cpp\
			OXEvent.cpp\
			p2p.cpp\
			packet_info.cpp\
			party.cpp\
			PetSystem.cpp\
			buffi_system.cpp\
			skill_upgrade.cpp\
			polymorph.cpp\
			priv_manager.cpp\
			pvp.cpp\
			questevent.cpp\
			questlua_affect.cpp\
			questlua_arena.cpp\
			questlua_ba.cpp\
			questlua_building.cpp\
			questlua_danceevent.cpp\
			questlua_dragonlair.cpp\
			questlua_dragonsoul.cpp\
			questlua_dungeon.cpp\
			questlua_dungeoninfo.cpp\
			questlua_game.cpp\
			questlua_global.cpp\
			questlua_guild.cpp\
			questlua_horse.cpp\
			questlua_item.cpp\
			questlua_marriage.cpp\
			questlua_npc.cpp\
			questlua_oxevent.cpp\
			questlua_party.cpp\
			questlua_pc.cpp\
			questlua_pet.cpp\
			questlua_quest.cpp\
			questlua_snakelair.cpp\
			questlua_target.cpp\
			questlua_TempleOchao.cpp\
			questlua_whitedragon.cpp\
			questlua_zodiac.cpp\
			questlua.cpp\
			questmanager.cpp\
			questnpc.cpp\
			questpc.cpp\
			RealTimeRegen.cpp\
			refine.cpp\
			regen.cpp\
			resp_manager.cpp\
			safebox.cpp\
			sectree_manager.cpp\
			sectree.cpp\
			shop_manager.cpp\
			shopEx.cpp\
			shop.cpp\
			skill_power.cpp\
			skill.cpp\
			SnakeLair.cpp\
			soulroulette.cpp\
			start_position.cpp\
			stone_event.cpp\
			switchbot_manager.cpp\
			target.cpp\
			TempleOchao.cpp\
			text_file_loader.cpp\
			TrafficProfiler.cpp\
			trigger.cpp\
			utils.cpp\
			version.cpp\
			vector.cpp\
			war_map.cpp\
			wedding.cpp\
			WhiteDragon.cpp\
			xmas_event.cpp

# >> HEDEF DOSYA DONUSUMLERI <<
COBJS = $(CFILE:%.c=$(OBJDIR)/%.o)
CPPOBJS = $(CPPFILE:%.cpp=$(OBJDIR)/%.o)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
CDEPS = $(COBJS:%.o=%.d)
CPPDEPS = $(CPPOBJS:%.o=%.d)
endif

MAINOBJ = $(MAINCPP:%.cpp=$(OBJDIR)/%.o)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
MAINDEPS = $(MAINOBJ:%.o=%.d)
endif

# >> RENK KODLARI <<
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
MAGENTA = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[0;37m
BOLD = \033[1m
RESET = \033[0m

# >> ANA HEDEFLER <<
.PHONY: default all clean tag dep help

default: $(TARGET)

$(OBJDIR)/%.o: %.c
	@echo -e "[C]    ${YELLOW}$<${RESET} -> ${RED}$@${RESET}"
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@$(CC) -MM -MG -MP $(CFLAGS) $(INCDIR) -c $< -o $(OBJDIR)/$*.d
	@sed -i '' -e's/$*.o:/$(OBJDIR)\/$*.o:/g' $(OBJDIR)/$*.d
endif

$(OBJDIR)/%.o: %.cpp
	@echo -e "[CPP]  ${YELLOW}$<${RESET} -> ${RED}$@${RESET}"
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@$(CC) -MM -MG -MP $(CFLAGS) $(INCDIR) -c $< -o $(OBJDIR)/$*.d
	@sed -i '' -e's/$*.o:/$(OBJDIR)\/$*.o:/g' $(OBJDIR)/$*.d
endif

$(TARGET): $(CPPOBJS) $(COBJS) $(MAINOBJ)
	@echo -e "${MAGENTA}[BIRLESTIRME]${RESET} ${WHITE}$(TARGET)${RESET} olusturuluyor..."
	@$(CC) $(CFLAGS) $(LIBDIR) $(COBJS) $(CPPOBJS) $(MAINOBJ) $(LIBS) -o $(TARGET)
	@echo -e "${CYAN}[DOSYA TASI]${RESET} ${WHITE}$(GAMEDIR)${RESET} dizinine tasiniyor..."
	@mv $(TARGET) $(GAMEDIR)
	@echo -e "${GREEN}[TAMAMLANDI]${RESET} Game dosyasi ${WHITE}$(GAMEDIR)${RESET} dizininde hazir!"

dump:
	@$(CC) -dM -E - < /dev/null > .nope.nope

clean:
	@echo -e "${RED}[TEMIZLIK]${RESET} Tum obje dosyalari temizleniyor..."
	@rm -f $(COBJS) $(CPPOBJS) $(MAINOBJ)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@rm -f $(CDEPS) $(CPPDEPS) $(MAINDEPS)
endif
	@rm -f $(GAMEDIR)/game_r* $(BINDIR)/conv
	@rm -f $(GAMEDIR)/game
	@rm -rf $(OBJDIR)
	@echo -e "${GREEN}[OK]${RESET} Tum temizlik islemleri tamamlandi"

tag:
	@echo -e "${BLUE}[TAG OLUSTURMA]${RESET} CTags dosyasi olusturuluyor..."
	@ctags *.cpp *.h *.c
	@echo -e "${GREEN}[OK]${RESET} Tag dosyasi olusturuldu"

dep:
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@echo -e "${YELLOW}[BILGI]${RESET} GCC otomatik bagimlilik takibi aktif"
else
	@echo -e "${BLUE}[BAGIMLILIK]${RESET} Manuel bagimlilik dosyasi guncelleniyor..."
	@makedepend -f $(DEPFILE) $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(CPPFILE) $(CFILE) $(MAINCPP) 2> /dev/null > $(DEPFILE)
	@echo -e "${GREEN}[OK]${RESET} Bagimlilik dosyasi guncellendi"
endif

# >> YARDIM MESAJI <<
help:
	@echo -e "${BOLD}${CYAN}"
	@echo -e "================================================"
	@echo -e "         GAME SUNUCUSU DERLEME SISTEMI"
	@echo -e "================================================"
	@echo -e "${RESET}"
	@echo -e "${BOLD}${WHITE}KULLANIM:${RESET}"
	@echo -e "  ${GREEN}make${RESET}        -> ${YELLOW}Game sunucusunu derle${RESET}"
	@echo -e "  ${GREEN}make clean${RESET}  -> ${RED}Tum obje dosyalarini temizle${RESET}"
	@echo -e "  ${GREEN}make tag${RESET}    -> ${BLUE}CTags dosyasi olustur${RESET}"
	@echo -e "  ${GREEN}make dep${RESET}    -> ${MAGENTA}Bagimliliklari guncelle${RESET}"
	@echo -e "  ${GREEN}make help${RESET}   -> ${CYAN}Bu yardim mesajini goster${RESET}"
	@echo -e ""
	@echo -e "${BOLD}${WHITE}AYARLAR:${RESET}"
	@echo -e "  ${YELLOW}Derleyici:${RESET} $(CC)"
	@echo -e "  ${YELLOW}Game Versiyon:${RESET} $(GAME_VERSION)"
	@echo -e "  ${YELLOW}C++ Standard:${RESET} C++2a"
	@echo -e "  ${YELLOW}Mimari:${RESET} 32-bit"
	@echo -e "  ${YELLOW}Cikti Dizini:${RESET} $(GAMEDIR)"
	@echo -e "  ${YELLOW}Toplam Kaynak:${RESET} $(words $(CPPFILE)) CPP, $(words $(CFILE)) C"

# >> OTOMATIK BAGIMLILIK KONTROLU <<
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
sinclude $(CDEPS)
sinclude $(CPPDEPS)
sinclude $(MAINDEPS)
else
sinclude $(DEPFILE)
endif
