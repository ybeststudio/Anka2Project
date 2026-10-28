CC	= clang++
BIN_DIR = ../lib
BIN     = $(BIN_DIR)/libthecore.a
INCLUDE = ../include

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

CFLAGS  = -Wall -w -O2 -pipe -g -I$(INCLUDE)
CFLAGS += -std=c++14

LIBS    = 

OBJFILES = socket.o fdwatch.o buffer.o signal.o log.o utils.o \
	kstbl.o hangul.o heart.o main.o tea.o des.o gost.o memcpy.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)
	rm -f Depend

dep:
	$(CC) $(CFLAGS) -MM *.c > Depend

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

memcpy: memcpy.o utils.o log.o
	$(CC) $(CFLAGS) -c -D__MAIN__ memcpy.c
	$(CC) $(CFLAGS) -o memcpy memcpy.o utils.o log.o

sinclude Depend