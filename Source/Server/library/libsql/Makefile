CXX	= clang++

PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)
SVN_VERSION = $(shell svnversion -n -c . | cut -d':' -f2)

BIN     = ./libsql.a

IFLAGS = -I/usr/local/include -I/usr/local/include/mysql
CFLAGS  = $(IFLAGS) -Wall -w -O2 -pipe -D_THREAD_SAFE -fno-exceptions
CFLAGS += -std=c++14

LIBS =

OBJFILES = AsyncSQL.o Semaphore.o Tellwait.o Statement.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)
	rm -f Depend

dep:
	touch Depend
	$(CXX) $(CFLAGS) -MM *.cpp > Depend

# Pattern rule for .cpp to .o
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

sinclude Depend