# build/Makefile 07/15/92

O=\
	build.o \
	build0.o \
	cohtune_ent.o \
	fdisk.o \
	hdparms.o \
	idenable_dev.o \
	idtune_var.o \
	install.o \
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
	setdate \
	mkdev \
	uuset

# Primary target.
all:	$E
	: all done

# Executables.

BUILDOBJ=build.o numtab.o serialno.o build0.o idtune_var.o cohtune_ent.o

build:	$(BUILDOBJ)
	$(CC) -s -o $@ $(BUILDOBJ) -f

FDISKOBJ=fdisk.o cohtune_ent.o

fdisk:	$(FDISKOBJ)
	$(CC) -s -o $@ $(FDISKOBJ) -f

INSTALLOBJ=install.o build0.o

install: $(INSTALLOBJ)
	$(CC) -s -o $@  $(INSTALLOBJ)

HDPARMSOBJ=hdparms.o build0.o cohtune_ent.o

hdparms: $(HDPARMSOBJ)
	$(CC) -s -o $@ $(HDPARMSOBJ)

MKDEVOBJ=mkdev.o build0.o idenable_dev.o idtune_var.o

mkdev:	$(MKDEVOBJ)
	$(CC) -s -o $@ $(MKDEVOBJ)

SETDATEOBJ=setdate.o build0.o

setdate: $(SETDATEOBJ)
	$(CC) -s -o $@ $(SETDATEOBJ)

uuset:	uuset.c
	$(CC) -s -o $@ uuset.c

# Unlinked objects.
build.o:	build.c build0.h serialno.h
build0.o:	build0.c build0.h
cohpatch.o:	cohpatch.c build0.h
cohtune_ent.o:	cohtune_ent.c build0.h
fdisk.o:	fdisk.c fdisk0.h
hdparms.o:	hdparms.c build0.h
idenable_dev.o:	idenable_dev.c build0.h
idtune_var.o:	idtune_var.c build0.h
install.o:	install.c build0.h
numtab.o:	numtab.c serialno.h
serialno.o:	serialno.c build0.h serialno.h

# Etc.
clean:
	rm $O
