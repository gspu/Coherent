# Note from hws:  the makes for zipnote, zipcloak, and zipsplit
# are screwed up.  I didn't have time to fix them.

# Makefile for Zip, ZipNote, ZipCloak and ZipSplit

# what you can make ...
all:
	@echo ''
	@echo 'Make what?  You must say what system to make Zip for--e.g.'
	@echo '"make bsd".  Choices: 3b1, aix, att6300, aux, bsd, bsdold,'
	@echo 'convex, cray, cray3, dec_osf1, dnix, hpux, isc, linux,'
	@echo 'minix, next, next10, scodos, sun, sun_gcc, sysv, sysv_386,'
	@echo 'sysv_old, ultrix, xenix, xos, zilog.'
	@echo 'See the files install.doc and zip.doc for more information.'
	@echo ''

CRYPTO =
CLOAK =
CRFLAG =
MAKE = make
#  **********************************************************************
#  *** For encryption version, remove the # at the front of next line ***
MAKE = make CRYPTO="crypt.o" CLOAK="zipcloak" CRFLAG="-DCRYPT"

# (to use the Gnu compiler, change cc to gcc in CC and BIND)
CC = cc
BIND = cc
E =
CPP = /usr/lib/cpp -Dunix

# probably can change this to 'install' if you have it
INSTALL = cp

# target directories - where to install executables and man pages to
BINDIR = /usr/local/bin
MANDIR = /usr/man/manl

# flags
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)
CFLAGS = -O
LFLAGS1 =
LFLAGS2 = -s

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o $(CRYPTO)

OBJI = deflate.o trees.o bits.o
OBJA =
OBJU = zipfile_.o zipup_.o fileio_.o util_.o globals.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crypt.o
OBJS = zipsplit.o $(OBJU)

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	rm -f $*_.c; ln $< $*_.c
	$(CC) $(CFLAGS) -DUTIL $(CRFLAG) -c $*_.c
	rm -f $*_.c
.c.o:
	$(CC) $(CFLAGS) $(CRFLAG) -c $<

.1.doc:
	nroff -man $< | col -b | uniq > $@

# rules for zip, zipnote, zipcloak, zipsplit, and zip.doc.
$(OBJZ): zip.h ziperr.h tailor.h
$(OBJI): zip.h ziperr.h tailor.h
$(OBJN): zip.h ziperr.h tailor.h
$(OBJS): zip.h ziperr.h tailor.h
$(OBJC): zip.h ziperr.h tailor.h

match.o: match.s
	$(CPP) match.s > _match.s
	$(CC) -c _match.s
	mv _match.o match.o
	rm -f _match.s

ZIPS = zip$E zipnote$E zipsplit$E $(CLOAK)$E

zip.o zipup.o zipnote.o zipcloak.o zipsplit.o: revision.h
zips: $(ZIPS)
zipsman: zip zipnote zipsplit $(CLOAK) zip.doc

zip$E: $(OBJZ) $(OBJI) $(OBJA)
	$(BIND) -o zip$E $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote$E: $(OBJN)
	$(BIND) -o zipnote$E $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak$E: $(OBJC)
	$(BIND) -o zipcloak$E $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit$E: $(OBJS)
	$(BIND) -o zipsplit$E $(LFLAGS1) $(OBJS) $(LFLAGS2)

# install
install:	$(ZIPS)
	$(INSTALL) $(ZIPS) $(BINDIR)
	$(INSTALL) zip.1 $(MANDIR)


# These symbols, when #defined using -D have these effects on compilation:
# ZMEM		- includes C language versions of memset(), memcpy(), and
#		  memcmp() (util.c).
# DIRENT	- use <sys/dirent.h> and getdents() instead of <sys/dir.h>
#		  and opendir(), etc. (fileio.c).
# NODIR		- used for 3B1, which has neither getdents() nor opendir().
# NDIR		- use "ndir.h" instead of <sys/dir.h> (fileio.c).
# UTIL		- select routines for utilities (note, cloak, and split).
# PROTO		- enable function prototypes.
# RMDIR		- remove directories using a system("rmdir ...") call.
# CONVEX	- for Convex make target.
# AIX		- for AIX make target.
# CRYPT 	- include the encryption code.
# LINUX		- for linux make target.

# BSD 4.3 (also Unisys 7000--AT&T System V with heavy BSD 4.2)
bsd:
	$(MAKE) zips CFLAGS="-O"

# BSD, but missing memset(), memcmp().
bsdold:
	$(MAKE) zips CFLAGS="-O -DZMEM"

# AT&T System V, Rel 3.  Also SCO Unix, OpenDeskTop, ETA-10P*, SGI.
sysv_old:
	$(MAKE) zips CFLAGS="-O -DDIRENT"

# AT&T System V, Rel 4. Also any system with readdir() and termio.
sysv:
	$(MAKE) zips CFLAGS="-O -DSYSV -DTERMIO"

# AT&T System V, Rel 4 for 386 (uses asm version):
sysv_386:
	$(MAKE) zips CFLAGS="-O -DSYSV -DTERMIO -DASM" OBJA=match.o

# SCO Xenix
xenix:
	$(MAKE) zips CFLAGS="-O -DSYSV -DTERMIO" LFLAGS2="-lx -s"

# DNIX 5.x: like System V but optimization is messed up.
dnix:
	$(MAKE) zips CFLAGS="-DDIRENT"

# Linux 0.96a with GCC 2.12, dies with <= 2.11c. builtin functions are
# disabled because '#define const' removes const from normal functions
# but not builtin ones. And keeping const causes problems on other systems.
linux:
	$(MAKE) zips CFLAGS="-O -fno-builtin -DSYSV -DTERMIO -DLINUX" \
	  CC=gcc BIND=gcc

# Sun OS 4.x: BSD, but use getdents(). If you have gcc, use 'make sun_gcc'
# instead since the code produced is better.
sun:
	$(MAKE) zips CFLAGS="-O4 -DDIRENT"

# Sun OS 4.x with gcc (bug with -s linker flag). Use -O if your version
# of gcc does not like -O2.
sun_gcc:
	$(MAKE) zips CFLAGS="-O2 -DDIRENT" CC=gcc BIND=gcc LFLAGS2=""
	strip $(ZIPS)

# NeXT 1.0: BSD, but use shared library.
next10:
	$(MAKE) zips CFLAGS="-O" LFLAGS2="-s -lsys_s"

# NeXT 2.0: BSD, but use MH_OBJECT format for smaller executables.
next:
	$(MAKE) zips CFLAGS="-O" LFLAGS2="-s -object"

# HPUX: System V, but use <ndir.h> and opendir(), etc.
hpux:
	$(MAKE) zips CFLAGS="-O -DNDIR"

# Cray Unicos 5.1.10 & 6.0.11, Standard C compiler 2.0
cray:
	$(MAKE) zips CFLAGS="-O -DDIRENT" CC="scc"

# Cray Unicos 6.1, Standard C compiler 3.0 (all routines except trees.c
# may be compiled with vector3; internal compiler bug in 3.0.2.3 and
# earlier requires vector2 for trees.c)
cray3:
	$(MAKE) zips CFLAGS="-O -h vector2 -h scalar3 -DDIRENT" CC="scc"

# AT&T 3B1: System V, but missing a few things.
3b1:
	$(MAKE) zips CFLAGS="-O -DNODIR -DRMDIR"

# AT&T 6300 PLUS (don't know yet how to allocate 64K bytes):
att6300:
	$(MAKE) zips LFLAGS1="-Ml" \
	CFLAGS="-O -Ml -DNODIR -DRMDIR -DDYN_ALLOC -DMEDIUM_MEM -DWSIZE=16384"

# zilog zeus 3.21
zilog:
	$(MAKE) zips CFLAGS="-O -DZMEM -DNDIR -DRMDIR" CC="scc -i" BIND="scc"

# SCO 386 cross compile for MS-DOS
# Note: zip.exe should be lzexe'd on DOS to reduce its size
scodos:
	$(MAKE) zips CFLAGS="-O -Mc -dos -DNO_ASM" LFLAGS1="-Mc -dos" \
	 LFLAGS2="-F 1000" E=".exe"

# A/UX:
aux:
	$(MAKE) zips CFLAGS="-O -DTERMIO"

# Convex C-120, C-210, OS 9.0, cc v. 4.0, no vectorization used.
convex:
	$(MAKE) zips CFLAGS="-O"

# AIX Version 3.1 for RISC System/6000 
aix:
	$(MAKE) zips CC="c89" BIND="c89" \
	   CFLAGS="-O -D_POSIX_SOURCE -D_ALL_SOURCE -D_BSD -DAIX"

# MINIX 1.5.10 with Bruce Evans 386 patches and gcc/GNU make
minix:
	$(MAKE) zips CFLAGS="-O -DDIRENT -DMINIX" CC=gcc BIND=gcc
	chmem =262144 zip

# Interactive Systems Corporation System V/386, Rel 3.2--optimizer problems
isc:
	$(MAKE) zips CFLAGS="-DDIRENT"

# DYNIX (R) V3.0.18 (no memset() or memcmp(), rindex() instead of strrchr())
dynix:
	$(MAKE) zips CFLAGS="-O -DZMEM -Dstrrchr=rindex"

# Ultrix
ultrix:
	$(MAKE) zips CFLAGS="-O -Olimit 700"

# DEC OSF/1
dec_osf1:
	$(MAKE) zips CFLAGS="-O -Olimit 1000 -DOSF -D_BSD"

# xos: Olivetti LSX-3005..3045 with X/OS 2.3 or 2.4
xos:
	$(MAKE) zips CFLAGS="-O -DTERMIO"

# clean up after making stuff and installing it
clean:
	rm -f *.o
	rm -f $(ZIPS)

# This one's for Mark:
it:
	$(MAKE) zipsman CFLAGS="-O -Wall -DPROTO"\
	LFLAGS2="-s -object" VPATH="${HOME}/Unix/bin"

# and this one for Jean-loup:
gcc_d:
	$(MAKE) zip CFLAGS="-g -DDEBUG -DDIRENT" CC=gcc BIND=gcc LFLAGS2="-g"
	mv zip zipd

# end of Makefile
