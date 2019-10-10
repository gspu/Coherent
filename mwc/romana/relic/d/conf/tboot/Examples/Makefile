# Makefile for Bootable stand-alone examples
#
# La Monte H. Yarroll <piggy@mwc.com>, September 1991

DESTDIR=/

LIBS=../libtboot.a -lc
LD=ld
AR=ar
RANLIB=ranlib
INCLUDE=-I/usr/include/coff -I. -I..
CFLAGS=$(INCLUDE)

hello: hello.o  ../libtboot.a ../Startup.o read16.o read_cmos.o
	$(LD) -o hello ../Startup.o hello.o read16.o read_cmos.o $(LIBS)

dir: dir.o ../libtboot.a ../Startup.o
	$(LD) -o dir ../Startup.o $(OBJECTS) dir.o $(LIBS)

newdir: newdir.o ../libtboot.a ../Startup.o
	$(LD) -o newdir Startup.o newdir.o $(LIBS)

../libtboot.a:
	(cd ..; make libtboot.a)

../Startup.o: ../Startup.s
	(cd ..; make Startup.o)

clean:
	rm -f l.out *.o *.a

install:
	cp tboot $(DESTDIR)
	chown root $(DESTDIR)/tboot
	chgrp daemon $(DESTDIR)/tboot
	chmod ugo=r $(DESTDIR)/tboot
