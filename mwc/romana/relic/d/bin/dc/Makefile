# stevesf!bc/Makefile 3/29/90

bc=/newbits/bin/bc
CFLAGS=-O -I$(bc)
DESTDIR= /interim/bin/
dco=dc.o dcsub.o undefined.o
bco=bcmch.o bcmutil.o getnum.o globals.o output.o putnum.o
SRCS= dc.c dcsub.c undefined.c $(bc)/bcmch.c $(bc)/bcmutil.c $(bc)/getnum.c \
	$(bc)/globals.c $(bc)/output.c $(bc)/putnum.c

dc:	$(dco) $(bco)
	cc $(CFLAGS) -o $(DESTDIR)dc $(dco) $(bco) -lmp
	su root chown bin $(DESTDIR)dc
	su root chgrp bin $(DESTDIR)dc
	su root chmod 511 $(DESTDIR)dc

bcmch.o: $(bc)/bcmch.c
	cc $(CFLAGS) -c $?
bcmutil.o: $(bc)/bcmutil.c
	cc $(CFLAGS) -c $?
getnum.o: $(bc)/getnum.c
	cc $(CFLAGS) -c $?
globals.o: $(bc)/globals.c
	cc $(CFLAGS) -c $?
output.o: $(bc)/output.c
	cc $(CFLAGS) -c $?
putnum.o: $(bc)/putnum.c
	cc $(CFLAGS) -c $?
