# Generate debug information
#DEBUG = 1
DEBUG = 0

DESTDIR=/bin386/etc
DESTDIR2=/bin386/bin

ALL = bin/crontab bin/cron

COMMON = src/cron.h Makefile

TABOBJ = obj/crontab.o obj/misc.o 

DAEMOBJ = obj/cron.o obj/misc.o obj/cronpipe.o \
	obj/entry.o obj/gethostname.o obj/bedaemon.o

all: $(ALL)
	@echo All done.

bin/cron: $(DAEMOBJ) $(COMMON)
	cc -DDEBUG=$(DEBUG) $(DAEMOBJ) -lmisc -o $(DESTDIR)/cron
	su root chown root $(DESTDIR)/cron
	su root chgrp root $(DESTDIR)/cron
	su root chmod 4511 $(DESTDIR)/cron
#	su root strip	$(DESTDIR)/cron

bin/crontab: $(TABOBJ) $(COMMON)
	cc -DDEBUG=$(DEBUG) -o $(DESTDIR2)/crontab $(TABOBJ)
	su root chown root $(DESTDIR2)/crontab
	su root chgrp root $(DESTDIR2)/crontab
	su root chmod 4511 $(DESTDIR2)/crontab
	su root strip $(DESTDIR2)/crontab

obj/cron.o: src/cron.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/cron.c -o obj/cron.o

obj/crontab.o: src/crontab.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/crontab.c -o obj/crontab.o

obj/cronpipe.o: src/cronpipe.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/cronpipe.c -o obj/cronpipe.o

obj/bedaemon.o: src/bedaemon.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/bedaemon.c -o obj/bedaemon.o

obj/entry.o: src/entry.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/entry.c -o obj/entry.o

obj/gethostname.o: src/gethostname.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/gethostname.c -o obj/gethostname.o

obj/misc.o: src/misc.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/misc.c -o obj/misc.o
