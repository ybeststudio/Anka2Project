CXX	= clang++
BIN = ./libpoly.a

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)

CFLAGS  = -Wall -w -O2 -pipe -fno-exceptions -fno-rtti
CFLAGS += -std=c++14

LIBS =

OBJFILES = Base.o Poly.o SymTable.o Symbol.o

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
	$(CXX) $(CFLAGS) -MM *.cc > Depend

# Pattern rule for .cc to .o
%.o: %.cc
	$(CXX) $(CFLAGS) -c $< -o $@

sinclude Depend