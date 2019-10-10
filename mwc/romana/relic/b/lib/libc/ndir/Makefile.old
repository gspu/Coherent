DIRENT=/usr/include/dirent.h
SYSDIR=/usr/include/sys/dir.h
#DESTDIR=/usr/lib/


OBJS=closedir.o opendir.o seekdir.o readdir.o rewinddir.o telldir.o \
	getdents.o 

CFLAGS = -O -DDBG -DUFS -DBIT_16 #-DDBG
LIBNDIR = libndir#

all: $(LIBNDIR).a
	@echo Installation complete.
	@echo

/usr/lib/$(LIBNDIR).a: $(LIBNDIR).a
	cp $(LIBNDIR).a /usr/lib/$(LIBNDIR).a
	chown bin $(DESTDIR)$(LIBNDIR).a
	chgrp bin $(DESTDIR)$(LIBNDIR).a
	chmod 644 $(DESTDIR)$(LIBNDIR).a

$(LIBNDIR).a: $(OBJS)
	ar rsv $(DESTDIR)$(LIBNDIR).a $(OBJS)

closedir.o: closedir.c dirent.h sys.dirent.h
opendir.o: opendir.c dirent.h sys.dirent.h
readdir.o: readdir.c dirent.h sys.dirent.h
rewinddir.o: rewinddir.c dirent.h sys.dirent.h
seekdir.o: seekdir.c dirent.h sys.dirent.h
telldir.o: telldir.c dirent.h sys.dirent.h

#setup: /usr/include/dirent.h /usr/include/sys/dirent.h
#	@echo header files in place;

#/usr/include/dirent.h: dirent.h
#	cp dirent.h /usr/include/dirent.h
#	chown bin /usr/include/dirent.h
#	chgrp bin /usr/include/dirent.h
#	chmod 644 /usr/include/dirent.h

#/usr/include/sys/dirent.h: sys.dirent.h
#	cp sys.dirent.h /usr/include/sys/dirent.h
#	chown bin /usr/include/sys/dirent.h
#	chgrp bin /usr/include/sys/dirent.h
#	chmod 644 /usr/include/sys/dirent.h

#install: setup
#	mv $(LIBNDIR).a /usr/lib
#	if test ! -f /usr/man/update-indx then mv Update-indx /usr/man fi
#	cat man-indx >> /usr/man/user.index
#	if test ! -d /usr/man/2 then mkdir /usr/man/2 fi
#	if test ! -d /usr/man/3 then mkdir /usr/man/3 fi
#	if test ! -d /usr/man/4 then mkdir /usr/man/4 fi
#	nroff -man getdents.2 > /usr/man/2/getdents
#	nroff -man directory.3c > /usr/man/3/directory
#	nroff -man dirent.4 > /usr/man/4/dirent
#	sh /usr/man/Update-indx
#
#clean: 
#	rm *.o *.a *.out tst core
