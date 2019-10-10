# /usr/src/libc/Makefile 1/13/93
# Assumes $(SRCPATH) defines root libc source directory, e.g. /usr/src/libc.
# Assumes $(MCH) defines target machine, e.g. i8086 or i386.
# Assumes $(AR), $(AS), $(AFLAGS), $(CC), $(CFLAGS) defined appropriately.
# To build a new libc.a, move to an empty object directory and
# type "make -e SRCPATH=/usr/src/libc MCH=i8086" (or other pathname as appropriate).
# This takes about 20 minutes from scratch on 20 MHz 386.

# Copyright module is first entry in the archive.
OBJ=\
	copyright.o

# Primary target.
# The order of targets here is significant.
# dstdio must be the last target, because it plays tricks to reduce size
# of executable if stdio is not required.
# dgen is next to last because e.g. perror() references stdio.
all:	ROOT dcrt dndir dstring dsys dstdlib dgen dstdio
	: libc done.

# Primary target for NDP library.
ndp:	ROOT dcrtndp dndir dstring dsys dstdlib dgen dstdio
	: NDP libc done.

# Pseudotargets.
# The lower case pseudotargets are made every time to check each subdirectory.
# The upper case pseudotargets indicate when the objects in a given directory
# were last made, allowing replacement of only the stale object files.
ROOT:	$(OBJ)
	$(AR) rckus libc.a $?
	touch $@
dcrt:	
	SRCPATH=$(SRCPATH)/crt make -e
dcrtndp:	
	SRCPATH=$(SRCPATH)/crt make -e ndp
dgen:
	SRCPATH=$(SRCPATH)/gen make -e
dndir:
	SRCPATH=$(SRCPATH)/ndir make -e
dstdio:
	SRCPATH=$(SRCPATH)/stdio make -e
dstdlib:
	SRCPATH=$(SRCPATH)/stdlib make -e
dstring:
	SRCPATH=$(SRCPATH)/string make -e
dsys:
	SRCPATH=$(SRCPATH)/sys make -e

# Secondary targets.
copyright.o:	$(SRCPATH)/gen/copyright.c

clean:
	rm *.o

# end of /usr/src/libc/Makefile
