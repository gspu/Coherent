#
#	Makefile for 'conf' commands
#
#					Angus Telfer (January 1987)
#

DESTDIR=/bin386/conf/

# RET is the 386 'ret' instruction.
RET=0xC3
LIBS=
INCLUDE=
CC=cc
LD=ld
CFLAGS=$(INCLUDE)

patch:	patch.o coffnlist.o setfile.o
	$(CC) -o $(DESTDIR)patch patch.o coffnlist.o setfile.o $(LIBS)

testit: testit.o nix.o
	$(CC) $(CFLAGS) -o testit testit.o nix.o $(LIBS)

test: testit
	./patch testit my_long=my_int+1:l
	./patch testit nix=$(RET):c
	./testit

install: patch
	cp patch $(DESTDIR)

clean:
	rm -f *.o *.a testit
