# Makefile for ps for 386 COHERENT
#
# La Monte H. Yarroll <piggy@mwc.com>, November 1991

DESTDIR=/
LIB=/usr/local/lib

TARGETS=nps
OBJECTS=nps.o mem.o print.o proc.o user.o
SOURCES=mem.c nps.c print.c proc.c user.c

AR=ar
RANLIB=ranlib
INCLUDE=
LIBS=
CFLAGS=$(INCLUDE)

all: $(TARGETS)
	@true	# nop

nps:	$(OBJECTS)
	$(CC) -o nps $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) core

