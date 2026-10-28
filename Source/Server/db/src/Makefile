# ==============================================================================
# >  Metin2 DB Sunucusu Derleme Sistemi
# > Updated: Kasim 2025
# > Purpose: DB sunucusunu Clang++ ile derleyin
# ==============================================================================

# >> DERLEYICI VE VERSIYON <<
CC = clang++
DB_VERSION = 40142

# >> DOSYA SISTEMI YAPILANDIRMASI <<
INCDIR =
LIBDIR =
BINDIR = ..
OBJDIR = .obj
DBDIR = /usr/home/share/bin
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

# >> YAPILANDIRMA SABITLERI <<
ENABLE_GCC_AUTODEPEND = 1

# >> BAGIMLILIK DOSYASI AYARI <<
ifneq ($(ENABLE_GCC_AUTODEPEND), 1)
DEPFILE = Depend
endif

# >> DERLEYICI BAYRAKLARI <<
CFLAGS = -g -w -Ofast -pipe -fno-rtti -fno-exceptions -std=c++14 -static-libstdc++ -Wno-long-long -pthread -D_THREAD_SAFE

# >> VERSIYON VE SISTEM BILGISI <<
CFLAGS += -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__DB_VERSION__=\"$(DB_VERSION)\"

# >> HARICI KUTUPHANE AYARLARI <<

# OpenSSL Sifreleme
INCDIR += -I/usr/include
LIBS += -lssl -lcrypto

# MySQL Veritabani
INCDIR += -I/usr/local/include/mysql
LIBS += -L/usr/local/lib/mysql -lmariadbclient -lz

# >> PROJE KUTUPHANELERI <<
INCDIR += -I/usr/local/include
LIBDIR += -L../../library/libthecore/lib -L../../library/libsql -L../../library/libpoly -L../../library/libgame/lib
LIBS += -lthecore -lsql -lpoly -lgame -lm

# >> HEDEF DOSYA <<
TARGET = $(BINDIR)/db

# >> KAYNAK DOSYA LISTESI <<
CPPFILE =	BufferManager.cpp Config.cpp NetBase.cpp Peer.cpp PeerBase.cpp Main.cpp Lock.cpp DBManager.cpp \
			Cache.cpp LoginData.cpp ClientManager.cpp ClientManagerPlayer.cpp ClientManagerLogin.cpp \
			ClientManagerBoot.cpp ClientManagerParty.cpp ClientManagerGuild.cpp GuildManager.cpp \
			PrivManager.cpp MoneyLog.cpp ItemAwardManager.cpp ClientManagerEventFlag.cpp Marriage.cpp \
			ItemIDRangeManager.cpp ClientManagerHorseName.cpp version.cpp ProtoReader.cpp CsvReader.cpp \
			ClientManagerOfflineshop.cpp

# >> HEDEF DOSYA DONUSUMLERI <<
OBJS = $(CPPFILE:%.cpp=$(OBJDIR)/%.o)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
CPPDEPS = $(OBJS:%.o=%.d)
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
.PHONY: default all clean dep help

default: $(TARGET)

$(TARGET): $(OBJS)
	@echo -e "${MAGENTA}[BIRLESTIRME]${RESET} ${WHITE}$(TARGET)${RESET} olusturuluyor..."
	@$(CC) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)
	@echo -e "${CYAN}[DOSYA TASI]${RESET} ${WHITE}$(DBDIR)${RESET} dizinine tasiniyor..."
	@mv $(TARGET) $(DBDIR)
	@echo -e "${GREEN}[TAMAMLANDI]${RESET} DB dosyasi ${WHITE}$(DBDIR)${RESET} dizininde hazir!"
	@echo -e "${BOLD}${CYAN}=== DB SUNUCUSU BASARIYLA DERLENDI ===${RESET}"

$(OBJDIR)/%.o: %.cpp
	@echo -e "[CPP]  ${YELLOW}$<${RESET} -> ${RED}$@${RESET}"
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@$(CC) -MM -MG -MP $(CFLAGS) $(INCDIR) -c $< -o $(OBJDIR)/$*.d
	@sed -i '' -e's/$*.o:/$(OBJDIR)\/$*.o:/g' $(OBJDIR)/$*.d
endif

$(OBJDIR):
	@echo -e "${BLUE}[DIZIN HAZIRLIGI]${RESET} ${WHITE}$(OBJDIR)${RESET} dizini olusturuluyor..."
	@mkdir $(OBJDIR)

clean:
	@echo -e "${RED}[TEMIZLIK]${RESET} Tum obje dosyalari temizleniyor..."
	@rm -f $(OBJS) $(DBDIR)/db
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@rm -f $(CPPDEPS)
endif
	@rm -rf $(OBJDIR)
	@echo -e "${GREEN}[OK]${RESET} Tum temizlik islemleri tamamlandi"

dep:
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@echo -e "${YELLOW}[BILGI]${RESET} GCC otomatik bagimlilik takibi aktif"
else
	@echo -e "${BLUE}[BAGIMLILIK]${RESET} Manuel bagimlilik dosyasi guncelleniyor..."
	@makedepend -f $(DEPFILE) $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(CPPFILE) 2> /dev/null > $(DEPFILE)
	@echo -e "${GREEN}[OK]${RESET} Bagimlilik dosyasi guncellendi"
endif

# >> YARDIM MESAJI <<
help:
	@echo -e "${BOLD}${CYAN}"
	@echo -e "================================================"
	@echo -e "         DB SUNUCUSU DERLEME SISTEMI"
	@echo -e "================================================"
	@echo -e "${RESET}"
	@echo -e "${BOLD}${WHITE}KULLANIM:${RESET}"
	@echo -e "  ${GREEN}make${RESET}        -> ${YELLOW}DB sunucusunu derle${RESET}"
	@echo -e "  ${GREEN}make clean${RESET}  -> ${RED}Tum obje dosyalarini temizle${RESET}"
	@echo -e "  ${GREEN}make dep${RESET}    -> ${MAGENTA}Bagimliliklari guncelle${RESET}"
	@echo -e "  ${GREEN}make help${RESET}   -> ${CYAN}Bu yardim mesajini goster${RESET}"
	@echo -e ""
	@echo -e "${BOLD}${WHITE}AYARLAR:${RESET}"
	@echo -e "  ${YELLOW}Derleyici:${RESET} $(CC)"
	@echo -e "  ${YELLOW}DB Versiyon:${RESET} $(DB_VERSION)"
	@echo -e "  ${YELLOW}C++ Standard:${RESET} C++14"
	@echo -e "  ${YELLOW}Mimari:${RESET} 64-bit"
	@echo -e "  ${YELLOW}Cikti Dizini:${RESET} $(DBDIR)"
	@echo -e "  ${YELLOW}Kaynak Dosya:${RESET} $(words $(CPPFILE)) adet"
	@echo -e ""
	@echo -e "${BOLD}${WHITE}OZELLIKLER:${RESET}"
	@echo -e "  ${GREEN}?${RESET} MySQL veritabani destegi"
	@echo -e "  ${GREEN}?${RESET} OpenSSL sifreleme"
	@echo -e "  ${GREEN}?${RESET} Cache ve buffer yonetimi"
	@echo -e "  ${GREEN}?${RESET} Client yonetimi"
	@echo -e "  ${GREEN}?${RESET} Guild sistemleri"

# >> OTOMATIK BAGIMLILIK KONTROLU <<
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
sinclude $(CPPDEPS)
else
sinclude $(DEPFILE)
endif
