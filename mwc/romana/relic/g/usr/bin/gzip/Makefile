# Generated automatically from Makefile.in by configure.
# Makefile for gzip (GNU zip)    -*- Indented-Text -*-
# Copyright (C) 1992-1993 Jean-loup Gailly and the Free Software Foundation

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# $Id: Makefile.in,v 0.14 1993/02/24 18:23:13 jloup Exp $

#### Start of system configuration section. ####


srcdir = .
VPATH = .

CC = cc -O
CPP = /lib/cpp -DNO_UNDERLINE

#INSTALL = /usr/bbx/install -c
#INSTALL_PROGRAM = $(INSTALL)
#INSTALL_DATA = $(INSTALL) -m 644

# Things you might add to DEFS (configure usually figures out what to do):
# -DDIRENT              Use <dirent.h>  for recursion (-r)
# -DSYSDIR              Use <sys/dir.h> for recursion (-r)
# -DSYSNDIR             Use <sys/ndir.h> for recursion (-r)
# -DNDIR                Use <ndir.h> for recursion (-r)
# -DSTDC_HEADERS        Use <stdlib.h>
# -DHAVE_UNISTD_H	Use <unistd.h>
# -DNO_UTIME_H		Don't use <utime.h>
# -DHAVE_SYSUTIME_H	Use <sys/utime.h>
# -DNO_MEMORY_H         Don't use <memory.h>. Not needed if STDC_HEADERS.
# -DNO_STRING_H         Use strings.h, not string.h. Not needed if STDC_HEADERS
# -DRETSIGTYPE=int      Define this if signal handlers must return an int.
# -DNO_SYMLINK          OS defines S_IFLNK but does not support symbolic links
# -DNO_MULTIPLE_DOTS    System does not allow file names with multiple dots
# -DNO_UTIME		System does not support setting file modification time
# -DNO_CHOWN		System does not support setting file owner
# -DNO_DIR		System does not support readdir()
# -DPROTO		Force function prototypes even if __STDC__ not defined
# -DASMV		Use asm version match.S
# -DMSDOS		MSDOS specific
# -DOS2			OS/2 specific
# -DVAXC		Vax/VMS with Vax C compiler
# -DVMS			Vax/VMS with gcc
# -DDEBUG		Debug code
# -DDYN_ALLOC		Use dynamic allocation of large data structures
# -DMAXSEG_64K		Maximum array size is 64K (for 16 bit system)
# -DRECORD_IO           read() and write() are rounded to record sizes.
# -DNO_STDIN_FSTAT      fstat() is not available on stdin
# -DNO_SIZE_CHECK       stat() does not give a reliable file size

DESTDIR=/bin386/usr/bin/

DEFS =  -D_POSIX_1_SOURCE=2 -D_MINIX=1 \
        -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DDIRENT=1 -DVOID_CLOSEDIR=1 \
	-DNO_UTIME -DHAVE_SYSUTIME_H
LIBS = 
ALLOCA =  

# additional assembly sources for particular systems may be required.
#OBJA = match.o
OBJA =

SEDCMD = 

CFLAGS = -DNO_ASM
# If you want debug on by default, define: CFLAGS=-g 
LDFLAGS = $(CFLAGS)

X=
# For OS/2 or MSDOS, use: X=.exe

O=.o
# For OS/2 or MSDOS, use: O=.obj

prefix = /usr
exec_prefix = ${prefix}

bindir = $(exec_prefix)/bin
datadir = $(prefix)/lib
libdir = $(prefix)/lib
infodir = $(prefix)/info

# Extension (not including `.') for the installed manual page filenames.
# If you change it to a value other than 1, you should also update
# zcat.1 and gunzip.1.
manext = 1
# Where to install the manual pages.
mandir = $(prefix)/man/man$(manext)

#### End of system configuration section. ####

SHELL = /bin/sh

LOADLIBES = $(LIBS)

TAR = tar

SRCS = gzip.c zip.c deflate.c trees.c bits.c unzip.c inflate.c util.c crypt.c\
       lzw.c unlzw.c unpack.c getopt.c alloca.c match.S makecrc.c zread.c

OBJS = gzip$O zip$O deflate$O trees$O bits$O unzip$O inflate$O util$O \
       crypt$O lzw$O unlzw$O unpack$O getopt$O $(OBJA) $(ALLOCA)

HDRS = gzip.h lzw.h tailor.h revision.h crypt.h getopt.h

GENFILES =  README NEWS INSTALL Makefile.in configure.in configure COPYING \
  TODO THANKS ChangeLog $(SRCS) $(HDRS) zcmp.in zmore.in znew.in zdiff.in \
  zforce.in gzexe.in gzip.1 gunzip.1 zcat.1 zcmp.1 zmore.1 znew.1 gzexe.1 \
  zforce.1 gzip.doc algorithm.doc gzip.texi texinfo.tex gpl.texinfo gzip.info \
  gzip-tar.patch

msdosFILES = msdos/tailor.c msdos/match.asm msdos/gzip.prj msdos/doturboc.bat \
  msdos/Makefile.msc msdos/Makefile.bor msdos/Makefile.djg

os2FILES = os2/Makefile.os2

vmsFILES = vms/Makefile.vms vms/makegzip.com vms/gzip.hlp

amigaFILES = amiga/Makefile.sasc amiga/Makefile.gcc amiga/tailor.c \
  amiga/utime.h

atariFILES = atari/Makefile.st

DISTFILES = $(GENFILES) $(msdosFILES) $(os2FILES) $(vmsFILES) $(amigaFILES) \
	  $(atariFILES)

.c$O:
	$(CC) -c $(DEFS) $(CFLAGS) $(CPPFLAGS) $<

#.PHONY: default all force test check

default:  all
all:	gzip$X zcmp zdiff zmore znew zforce gzexe
	cp gzip $(DESTDIR)
	chmog 511 bin bin $(DESTDIR)gzip
	ln -f $(DESTDIR)gzip $(DESTDIR)gunzip
	ln -f $(DESTDIR)gzip $(DESTDIR)unpack

force:

#### Start of specific targets section. ####
#
# 'configure' works only on Unix systems. For other systems able to make
# sense of this makefile, you can define target specific entries here.
# For other systems such as MSDOS, separate Makefiles are
# provided in subdirectories.

# gcc with emx 0.8f kit:
os2_gcc:
	$(MAKE) all CC=gcc CFLAGS="-O -DOS2" X=".exe"

# Xenix 2.3.2 for 286:
xenix_286:
	$(MAKE) all CFLAGS="-LARGE -M2l"

# Coherent (with broken /bin/sh):
coherent:
	$(MAKE) all OBJA=match.o DEFS=\
	"-DASMV -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DDIRENT=1"
	cp gzip $(DESTDIR)
	chmog 511 bin bin $(DESTDIR)gzip
	ln $(DESTDIR)gzip $(DESTDIR)gunzip

#### End of specific targets section. ####

install: all gzip.info
	for f in gzip$X zcmp zdiff zmore znew zforce gzexe; do \
	  $(INSTALL_PROGRAM) $${f} $(bindir)/$${f}; done
	for f in gunzip$X zcat$X ; do \
	  rm -f $(bindir)/$${f}; ln $(bindir)/gzip$X $(bindir)/$${f}; done
	-cd $(srcdir); \
	  for f in gzip gunzip zcat zcmp zmore znew zforce gzexe; do \
	  rm -f $(mandir)/$${f}.$(manext); \
	  $(INSTALL_DATA) $${f}.1 $(mandir)/$${f}.$(manext); done
	-cd $(srcdir); for f in gzip.info* ; do $(INSTALL_DATA) $${f} \
	  $(infodir)/$${f}; done

uninstall: force
	-cd $(bindir); rm -f gzip$X gunzip$X zcat$X \
	  zcmp zdiff zmore znew zforce gzexe
	-for f in gzip gunzip zcat zcmp zmore znew gzexe; do \
	  rm -f $(mandir)/$${f}.$(manext); done
	-cd $(infodir); rm -f gzip.info*

# install all files and replace compress (not recommended)
install_compress: install
	-test -f $(bindir)/compress.old || \
	  mv $(bindir)/compress$X $(bindir)/compress.old
	ln $(bindir)/gzip$X $(bindir)/compress$X
	rm -f $(bindir)/uncompress$X
	ln $(bindir)/gzip$X $(bindir)/uncompress$X

test: check
check:	all
	./gzip -5 < $(srcdir)/texinfo.tex > _gztest.z
	@if test `wc -c < _gztest.z` -eq 30890; then \
	   true; \
	else \
	   echo FAILED gzip test: incorrect size; \
	fi
	rm -f _gztest
	./gzip -d _gztest.z
	@if cmp _gztest $(srcdir)/texinfo.tex; then \
	   echo gzip test OK; \
	else \
	   echo FAILED gzip test: incorrect decompress; \
	fi
	rm -f _gztest*

TAGS: $(SRCS) $(HDRS)
	cd $(srcdir); etags $(SRCS) $(HDRS)

Makefile: Makefile.in ./config.status
	./config.status

./config.status: configure
	$(srcdir)/configure --srcdir=$(srcdir) --no-create

configure: configure.in
	@echo Warning: configure is out of date
#	cd $(srcdir); autoconf

clean:
	rm -f *$O gzip$X gunzip$X zcat$X a.out core
	rm -f zcmp zdiff zmore znew zforce gzexe _gztest*
	rm -f *.aux *.cp *.cps *.dvi *.fn *.fns *.ky *.kys *.log
	rm -f *.pg *.pgs *.toc *.tp *.tps *.vr *.vrs

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status

realclean: distclean
	rm -f TAGS gzip.info*

dist: $(DISTFILES) Makefile
	d=gzip-`sed -e '/VERSION/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' \
	            -e q revision.h` ; \
	rm -rf $$d; \
	mkdir $$d;       chmod 777 $$d;       ln $(GENFILES)   $$d; \
	mkdir $$d/msdos; chmod 777 $$d/msdos; ln $(msdosFILES) $$d/msdos; \
	mkdir $$d/os2;   chmod 777 $$d/os2;   ln $(os2FILES)   $$d/os2; \
	mkdir $$d/vms;   chmod 777 $$d/vms;   ln $(vmsFILES)   $$d/vms; \
	mkdir $$d/amiga; chmod 777 $$d/amiga; ln $(amigaFILES) $$d/amiga; \
	mkdir $$d/atari; chmod 777 $$d/atari; ln $(atariFILES) $$d/atari; \
	$(TAR) chofz $$d.tar.z $$d ; \
	rm -rf $$d

zipdist: $(DISTFILES) Makefile
	zip -u9T gzip`sed -e '/VERSION/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' \
			  -e s/[.]//g -e q revision.h` $(DISTFILES)
 
# Actual build-related targets

gzip$X:	Makefile $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

gzip$O zip$O deflate$O trees$O bits$O unzip$O inflate$O: gzip.h tailor.h
util$O lzw$O unlzw$O unpack$O crypt$O: gzip.h tailor.h

gzip$O unlzw$O: revision.h lzw.h

bits$O unzip$O util$O zip$O: crypt.h

gzip$O getopt$O: getopt.h

match$O: match.S
	$(CPP) $(srcdir)/match.S > _match.s
	$(CC) -c _match.s
	mv _match$O match$O
	rm -f _match.s

zcmp: zcmp.in
	sed "$(SEDCMD)" $(srcdir)/zcmp.in > zcmp
	chmod 755 zcmp

zdiff: zdiff.in
	sed "$(SEDCMD)" $(srcdir)/zdiff.in > zdiff
	chmod 755 zdiff

zmore: zmore.in
	sed "$(SEDCMD)" $(srcdir)/zmore.in > zmore
	chmod 755 zmore

znew: znew.in
	sed "$(SEDCMD)" $(srcdir)/znew.in > znew
	chmod 755 znew

zforce: zforce.in
	sed "$(SEDCMD)" $(srcdir)/zforce.in > zforce
	chmod 755 zforce

gzexe: gzexe.in
	sed "$(SEDCMD)" $(srcdir)/gzexe.in > gzexe
	chmod 755 gzexe

gzip.info: gzip.texi
	cd $(srcdir); makeinfo gzip.texi

gzip.dvi: gzip.texi
	cd $(srcdir); texi2dvi gzip.texi

gzip.doc: gzip.1
	cd $(srcdir); nroff -man gzip.1 | col -b | uniq > gzip.doc

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:

# end of file
