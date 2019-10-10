# build/Makefile 07/15/92

# Objects.
DESTDIR=bin/
DESTDIR2=
O=\
	build.o \
	build0.o \
	cohtune_ent.o \
	fdisk.o \
	hdparms.o \
	idenable_dev.o \
	idtune_var.o \
	install.o \
	kbdinstall.o \
	mkdev.o \
	numtab.o \
	setdate.o \
	serialno.o \
	uuset.o

# Executables.
E=\
	build \
	fdisk \
	hdparms \
	install \
	kbdinstall \
	setdate \
	mkdev \
	uuset

# Primary target.
all:	$E
	: all done

# Executables.
build:		build.o numtab.o serialno.o build0.o idtune_var.o \
	cohtune_ent.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -f
	strip $(DESTDIR)$@ 

fdisk:		fdisk.o cohtune_ent.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -f
	strip $(DESTDIR)$@ 

install:	install.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<

hdparms:	hdparms.o build0.o cohtune_ent.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
	strip $(DESTDIR)$@ 

kbdinstall:	kbdinstall.o build0.o idenable_dev.o idtune_var.o cohtune_ent.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -lcurses
	strip $(DESTDIR)$@ 

kbd2:	kbdinstall.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $< -ltermcap
	strip $(DESTDIR)$@ 

mkdev:	mkdev.o build0.o idenable_dev.o cohtune_ent.o idtune_var.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
	strip $(DESTDIR)$@ 

setdate:	setdate.o build0.o
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
	strip $(DESTDIR)$@ 

uuset:	uuset.c
	$(CC) $(LFLAGS) -o $(DESTDIR)$@ $<
	strip $(DESTDIR)$@ 

# Unlinked objects.
build.o:	build.c build0.h serialno.h
build0.o:	build0.c build0.h
cohpatch.o:	cohpatch.c build0.h
cohtuen_ent.o:	cohtune_ent.c build0.h
fdisk.o:	fdisk.c fdisk0.h
hdparms.o:	hdparms.c build0.h
idenable_dev.o:	idenable_dev.c build0.h
idtune_var.o:	idtune_var.c build0.h
install.o:	install.c build0.h
kbdinstall.o:	kbdinstall.c
numtab.o:	numtab.c serialno.h
serialno.o:	serialno.c build0.h serialno.h

# Etc.
strip:	$E
	strip $E
clean:
	rm $O
