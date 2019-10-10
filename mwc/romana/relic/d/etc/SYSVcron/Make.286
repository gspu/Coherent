# Generate debug information
#DEBUG = 1
DEBUG = 0

ALL = bin/crontab bin/crond

COMMON = src/cron.h Makefile

TABOBJ = obj/crontab.o obj/misc.o 

DAEMOBJ = obj/cron.o obj/misc.o obj/cronpipe.o \
	obj/entry.o obj/gethostname.o obj/bedaemon.o

all: $(ALL)
	@echo All done.

bin/crond: $(DAEMOBJ)
	cc -DDEBUG=$(DEBUG) $(DAEMOBJ) -lndir -o bin/crond
	su root chown root bin/crond
	su root chgrp root bin/crond
	su root chmod 4511 bin/crond
	su root strip	bin/crond

bin/crontab: $(TABOBJ) 
	cc -DDEBUG=$(DEBUG) -o bin/crontab $(TABOBJ)
	su root chown root bin/crontab
	su root chgrp root bin/crontab
	su root chmod 4511 bin/crontab
	su root strip bin/crontab

obj/bedaemon.o: src/bedaemon.c $(COMMON)
	cc -c src/bedaemon.c -o obj/bedaemon.o

obj/cron.o: src/cron.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/cron.c -o obj/cron.o

obj/crontab.o: src/crontab.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/crontab.c -o obj/crontab.o

obj/cronpipe.o: src/cronpipe.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/cronpipe.c -o obj/cronpipe.o

obj/entry.o: src/entry.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/entry.c -o obj/entry.o

obj/gethostname.o: src/gethostname.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/gethostname.c -o obj/gethostname.o

obj/misc.o: src/misc.c $(COMMON)
	cc -DDEBUG=$(DEBUG) -c src/misc.c -o obj/misc.o
