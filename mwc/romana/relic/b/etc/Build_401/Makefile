# build/Makefile 07/15/92

# Objects.
DESTDIR=/bin386/etc/
DESTDIR2=
O=\
	build.o \
	build0.o \
	coh_intro.o \
	cohpatch.o \
	fdisk.o \
	hdparms.o \
	install.o \
	kbdinstall.o \
	mkdev.o \
	mkserial.o \
	numtab.o \
	serialno.o

# Executables.
E=\
	build \
	coh_intro \
	cohpatch \
	fdisk \
	hdparms \
	install \
	kbdinstall \
	mkdev \
	mkserial

# Primary target.
all:	$E
	: all done

# Executables.
build:		build.o numtab.o serialno.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)build $< -f
	strip $(DESTDIR)build
coh_intro:	coh_intro.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)coh_intro $<
cohpatch:	cohpatch.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)coh_intro $<
#dpb:		dpb.s
#	as -xo $(DESTDIR2)$@ $<
#	strip $(DESTDIR2)$@
fdisk:		fdisk.o
	$(CC) $(LFLAGS) -o $(DESTDIR)fdisk $< -f
install:	install.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)install $<
hdparms:	hdparms.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)hdparms $<
kbdinstall:	kbdinstall.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)kbdinstall $< -lcurses
	strip $(DESTDIR)kbdinstall
mkdev:	mkdev.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)mkdev $<
mkserial:	mkserial.o numtab.o
	$(CC) $(LFLAGS) -o mkserial $<

# Unlinked objects.
build.o:	build.c build0.h serialno.h
build0.o:	build0.c build0.h
coh_intro.o:	coh_intro.c build0.h
cohpatch.o:	cohpatch.c build0.h
fdisk.o:	fdisk.c fdisk0.h
hdparms.o:	hdparms.c build0.h
install.o:	install.c build0.h
kbdinstall.o:	kbdinstall.c
mkserial.o:	mkserial.c serialno.h
numtab.o:	numtab.c serialno.h
serialno.o:	serialno.c build0.h serialno.h

# Etc.
strip:	$E
	strip $E
clean:
	rm $O
