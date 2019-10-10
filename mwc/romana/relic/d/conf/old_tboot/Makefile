# Makefile for Tertiary boot code for COHERENT
# Targets are stand-alone bootable programs.
#
# La Monte H. Yarroll <piggy@mwc.com>, September 1991

LD=ld
AR=ar
RANLIB=ranlib
CP=cp

DESTDIR=/interim/

LIBOBJECTS=bootlib.o cbootlib.o diskio.o indirect.o intcall.o monitor.o \
	l.out.o coff.o bio.o gift.o objects.o \
	fifo_b.o fifo_k.o sys.o fdisk.o arg_exist.o

LIBSOURCES=bootlib.m cbootlib.c diskio.c indirect.c intcall.m monitor.c \
	l.out.c coff.c bio.c gift.c objects.c \
	fifo_b.c fifo_k.c sys.c fdisk.c arg_exist.c

OBJECTS=Startup.o $(LIBOBJECTS)

SOURCES=Startup.s $(LIBSOURCES)

LIBS=libtboot.a -lc
INCLUDE=-I/usr/include/coff -I.
CFLAGS=$(INCLUDE)

# NB: Startup.o must come first in the link line, so the entry
# point in Startup lines up correctly.
tboot: sys/typed.h sys/ptypes.h tboot.o builtin.o libtboot.a
	$(LD) -o tboot Startup.o tboot.o builtin.o $(LIBS)

libtboot.a: $(LIBOBJECTS)
	rm -f $@
	$(AR) rku $@ $(LIBOBJECTS)
	$(RANLIB) $@

sys/typed.h: typed.h
	$(CP) typed.h sys/typed.h

sys/ptypes.h: ptypes.h
	$(CP) ptypes.h sys/ptypes.h

hello: hello.o $(OBJECTS)
	$(LD) -o hello $(OBJECTS) hello.o $(LIBS)

hello.o: Examples/hello.c

dir: dir.o $(OBJECTS) 
	$(LD) -o dir $(OBJECTS) dir.o $(LIBS)

dir.o: Examples/dir.c

newdir: newdir.o libtboot.a
	$(LD) -o newdir Startup.o newdir.o $(LIBS)

newdir.o: Examples/newdir.c

tags:
	ctags -r *.[ch]

clean:
	rm -f l.out *.o *.a

install:
	cp tboot $(DESTDIR)
	chown root $(DESTDIR)/tboot
	chgrp daemon $(DESTDIR)/tboot
	chmod ugo=r $(DESTDIR)/tboot
