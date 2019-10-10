# Makefile for ps for 386 COHERENT
#
# La Monte H. Yarroll <piggy@mwc.com>, November 1991

DESTDIR=/bin386/bin/
LIB=/usr/local/lib

TARGETS=ps

OBJECTS=ps.o coffnlist.o fifo.o pick_nfile.o

SOURCES=ps.c coffnlist.c fifo.c pick_nfile.c

AR=ar
RANLIB=ranlib
INCLUDE=-I/usr/include.55
LIBS=
CFLAGS=$(INCLUDE)

all: $(TARGETS)
	@true	# nop

ps:	$(OBJECTS)
	$(CC) -o $(DESTDIR)ps $(OBJECTS) $(LIBS)
	chmog 6511 sys sys $(DESTDIR)ps

readker: readker.o
	$(CC) -o readker readker.o $(LIBS)

pt: pt.o
	$(CC) -o pt pt.o $(LIBS)

search: search.o
	$(CC) -o search search.o $(LIBS)

coffnlist: coffnlist.c
	$(CC) $(CFLAGS) -DTEST -c coffnlist.c
	$(CC) -o coffnlist coffnlist.o $(LIBS)

clean:
	rm -f l.out *.o *.a

install:
	cp $(TARGETS) $(DESTDIR)
