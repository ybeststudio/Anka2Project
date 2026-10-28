CPP = ccache clang++-devel
CPPFLAGS=-g
INCLUDEDIR=../../../library/liblua/include
LIBDIR=../../../library/liblua/lib
LIBS=-llua -llualib
all: qc

qc: qc.cc qc.o crc32.o
	$(CPP) -g -o ./qc -I$(INCLUDEDIR) -L$(LIBDIR) qc.o crc32.o $(LIBS) 

crc32.o: crc32.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR)

qc.o: qc.cc
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDEDIR) -L$(LIBDIR)

clean:
	rm -f qc qc.o
