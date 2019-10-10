# Makefile for Tertiary boot code for COHERENT
# Targets are stand-alone bootable programs.
#
# La Monte H. Yarroll <piggy@mwc.com>, September 1991


#CC=cc286
#LD=cc286
#AR=/bin/286/ar
#RANLIB=/bin/286/ranlib
CP=cp
#AS=/bin/286/as

CC=cc
LD=ld
AR=/bin/ar
RANLIB=/bin/ranlib
AS=/bin/as

DESTDIR=/bin386/
DESTDIR2=/bin386/etc/

LIBOBJECTS=bootlib.o cbootlib.o diskio.o indirect.o intcall.o monitor.o \
	l.out.o coff.o bio.o gift.o objects.o pacifier.o \
	fifo_b.o fifo_k.o sys.o fdisk.o arg_exist.o execute.o \
	cpu_type.o printf.o

LIBSOURCES=bootlib.m cbootlib.c diskio.c indirect.c intcall.m monitor.c \
	l.out.c coff.c bio.c gift.c objects.c pacifier.c \
	fifo_b.c fifo_k.c sys.c fdisk.c arg_exist.c execute.c \
	cpu_type.s printf.c

OBJECTS=Startup.o $(LIBOBJECTS)

SOURCES=Startup.s $(LIBSOURCES)

LIBS=libtboot.a /lib/libc.a
#LIBS=libtboot.a libtboot.a libtboot.a /lib/286/libc.a
INCLUDE=-I. -I/usr/include -I/usr/include/sys
CFLAGS=$(INCLUDE)

all: tboot bootcmd
	touch all

# NB: Startup.o must come first in the link line, so the entry
# point in Startup lines up correctly.
tboot: sys/ptypes.h  Startup.o tboot.o builtin.o libtboot.a
	$(LD) -o $(DESTDIR)tboot Startup.o tboot.o builtin.o $(LIBS)

bootcmd: bootcmd.o fifo_k.o
	$(LD) -o $(DESTDIR2)bootcmd bootcmd.o fifo_k.o

libtboot.a: $(LIBOBJECTS) 
	rm -f $@
	$(AR) rkus $@ $(LIBOBJECTS)
	$(RANLIB) $@

sys/ptypes.h: ptypes.h
	$(CP) ptypes.h sys/ptypes.h

pacifier.o:	pacifier.c pac_states.h
	$(CC) -c $(CFLAGS) pacifier.c

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
