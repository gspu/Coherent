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
	dpb \
	fdisk \
	hdparms \
	install \
	kbdinstall \
	mkdev \
	mkserial \
	ttytype

# Primary target.
all:	$E
	: all done

# Executables.
build:		build.o numtab.o serialno.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -f
	strip $(DESTDIR)$@ 
coh_intro:	coh_intro.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
cohpatch:	cohpatch.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
dpb:		dpb.s
	as -xo $(DESTDIR2)$@ $<
	strip $(DESTDIR2)$@
fdisk:		fdisk.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -f
install:	install.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
hdparms:	hdparms.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
kbdinstall:	kbdinstall.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -lcurses
	strip $(DESTDIR)$@ 
mkdev:	mkdev.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
mkserial:	mkserial.o numtab.o
	$(CC) $(LFLAGS) -o $@ $<
ttytype:	ttytype.c
	cc -v -o $(DESTDIR)$@ $<
	chmog 511 bin bin $(DESTDIR)$@

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
