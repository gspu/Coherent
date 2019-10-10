#
#  12/3/90
#  Makefile to build the "libmodemcap.a" library of modem interaction
#  functions.
#
#  Note: the database "modemcap" is expected to live in /etc/modemcap
#  Note: -DCATCH refers to catching HUP and alarms in dial.c
#

LIBNAME=libmodemcap.a

# OS=UNIX_S5	# For System V machines.
OS=UNIX_V7	# For Version 7 machines.

IDIR=../include
CFLAGS=-D$(OS) -DCATCH -I$(IDIR)

LOBJS=	dial.o dialvars.o hangup.o initmodem.o \
	mdial.o merror.o mgetent.o mgetflag.o mgetnum.o mgetstr.o

XOBJS=	fixttyS.o fixttyT.o

all: $(LIBNAME) $(XOBJS)
	@

$(LIBNAME): $(LOBJS)
	ar r $(LIBNAME) $(LOBJS)
	ranlib $(LIBNAME)

clean:
	[ "`echo *.o`" = "*.o" ] || rm *.o

#
#  how to make some specific objects
#

fixttyS.o: fixtty.c
	$(CC) $(CFLAGS) -DSGTTY -c -o $@ $<

fixttyT.o: fixtty.c
	$(CC) $(CFLAGS) -DTERMIO -c -o $@ $<
