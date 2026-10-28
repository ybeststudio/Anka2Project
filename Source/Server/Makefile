# ==============================================================================
# Metin2 Server - Toplu derleme (kutuphaneler + DB + Game)
# Source/Server dizininden: make [hedef]
# ==============================================================================

TOP		:= $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# Renkler
RED		= \033[0;31m
GREEN	= \033[0;32m
YELLOW	= \033[0;33m
BLUE	= \033[0;34m
CYAN	= \033[0;36m
BOLD	= \033[1m
RESET	= \033[0m

.PHONY: all libs libthecore libpoly libsql libgame liblua db game clean clean-all help

# Varsayilan: once kutuphaneler, sonra db, sonra game
all: libs db game
	@echo ""
	@echo -e "${BOLD}${GREEN}=== TUM SERVER BILESENLERI DERLENDI ===${RESET}"

# Sadece kutuphaneler (db/game bagimlilari)
libs: libthecore libpoly libsql libgame liblua
	@echo -e "${GREEN}[libs]${RESET} Tum kutuphaneler hazir."

# --- Kutuphaneler (birbirinden bagimsiz, sira onemli degil) ---
libthecore:
	@echo -e "${BLUE}[1/5]${RESET} libthecore derleniyor..."
	@$(MAKE) -C $(TOP)/library/libthecore/src
	@echo -e "${GREEN}[OK]${RESET} libthecore"

libpoly:
	@echo -e "${BLUE}[2/5]${RESET} libpoly derleniyor..."
	@$(MAKE) -C $(TOP)/library/libpoly
	@echo -e "${GREEN}[OK]${RESET} libpoly"

libsql:
	@echo -e "${BLUE}[3/5]${RESET} libsql derleniyor..."
	@$(MAKE) -C $(TOP)/library/libsql
	@echo -e "${GREEN}[OK]${RESET} libsql"

libgame:
	@echo -e "${BLUE}[4/5]${RESET} libgame derleniyor..."
	@$(MAKE) -C $(TOP)/library/libgame/src
	@echo -e "${GREEN}[OK]${RESET} libgame"

liblua:
	@echo -e "${BLUE}[5/5]${RESET} liblua derleniyor..."
	@$(MAKE) -C $(TOP)/library/liblua all
	@echo -e "${GREEN}[OK]${RESET} liblua"

# --- DB (libthecore, libsql, libpoly, libgame kullanir) ---
db: libs
	@echo -e "${CYAN}[DB]${RESET} DB sunucusu derleniyor..."
	@$(MAKE) -C $(TOP)/db/src
	@echo -e "${GREEN}[OK]${RESET} db"

# --- Game (tum kutuphaneleri kullanir) ---
game: libs
	@echo -e "${CYAN}[GAME]${RESET} Game sunucusu derleniyor..."
	@$(MAKE) -C $(TOP)/game/src
	@echo -e "${GREEN}[OK]${RESET} game"

# --- Temizlik ---
clean: clean-libs clean-db clean-game
	@echo -e "${GREEN}[clean]${RESET} Tum obje/ara dosyalar temizlendi."

clean-libs:
	@$(MAKE) -C $(TOP)/library/libthecore/src clean 2>/dev/null || true
	@$(MAKE) -C $(TOP)/library/libpoly clean 2>/dev/null || true
	@$(MAKE) -C $(TOP)/library/libsql clean 2>/dev/null || true
	@$(MAKE) -C $(TOP)/library/libgame/src clean 2>/dev/null || true
	@$(MAKE) -C $(TOP)/library/liblua clean 2>/dev/null || true
	@echo -e "${YELLOW}[clean]${RESET} Kutuphaneler temizlendi."

clean-db:
	@$(MAKE) -C $(TOP)/db/src clean 2>/dev/null || true
	@echo -e "${YELLOW}[clean]${RESET} DB temizlendi."

clean-game:
	@$(MAKE) -C $(TOP)/game/src clean 2>/dev/null || true
	@echo -e "${YELLOW}[clean]${RESET} Game temizlendi."

# Yardim
help:
	@echo -e "${BOLD}${CYAN}========================================${RESET}"
	@echo -e "${BOLD}${CYAN}   Metin2 Server - Toplu Makefile${RESET}"
	@echo -e "${BOLD}${CYAN}========================================${RESET}"
	@echo ""
	@echo -e "${BOLD}Kullanim:${RESET} make [hedef]"
	@echo ""
	@echo -e "  ${GREEN}make${RESET} veya ${GREEN}make all${RESET}  -> Tum kutuphaneler + DB + Game"
	@echo -e "  ${GREEN}make libs${RESET}   -> Sadece kutuphaneler (libthecore, libpoly, libsql, libgame, liblua)"
	@echo -e "  ${GREEN}make db${RESET}     -> DB sunucusu (once libs gerekir)"
	@echo -e "  ${GREEN}make game${RESET}   -> Game sunucusu (once libs gerekir)"
	@echo ""
	@echo -e "  ${GREEN}make libthecore${RESET}  ${GREEN}make libpoly${RESET}  ${GREEN}make libsql${RESET}"
	@echo -e "  ${GREEN}make libgame${RESET}     ${GREEN}make liblua${RESET}   (tekil kutuphane)"
	@echo ""
	@echo -e "  ${RED}make clean${RESET}      -> Tum projelerde clean"
	@echo -e "  ${RED}make clean-libs${RESET} -> Sadece kutuphanelerde clean"
	@echo -e "  ${RED}make clean-db${RESET}   ${RED}make clean-game${RESET}"
	@echo ""
	@echo -e "  ${CYAN}make help${RESET}   -> Bu mesaj"
	@echo ""
	@echo -e "Dizin: ${YELLOW}$(TOP)${RESET}"
