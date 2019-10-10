# /usr/src/libc/ndir/Makefile 5/12/92

OBJ=\
	closedir.o\
	opendir.o\
	readdir.o\
	rewinddir.o\
	seekdir.o\
	telldir.o
#	getdents.o\

CCFLAGS = -I$(SRCPATH) -DUFS -DBIT_16

# Primary target.
all:	NDIR
	: libc/ndir done

# Pseudotargets. 
NDIR:		$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
closedir.o:	closedir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
getdents.o:	getdents.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
opendir.o:	opendir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
readdir.o:	readdir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
rewinddir.o:	rewinddir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
seekdir.o:	seekdir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<
telldir.o:	telldir.c
	$(CC) -c $(CFLAGS) $(CCFLAGS) $<

# end of /usr/src/libc/ndir/Makefile
