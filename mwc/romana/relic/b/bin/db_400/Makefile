#
#				DB makefile
#				===========
#
#	This makefile creates the 'db' command from the appropriate source
#	files.
#
DESTDIR=/bin386/bin/
LIBNAME=libc.a
CC=exec /bin/cc
AS=exec /bin/as
MV=mv
CPP=/lib/cpp
LD=ld
INCDIR=/usr/include
SYSINCDIR=$(INCDIR)/sys
RUNDIR=.
LCLDIR=.
LFLAGS=-i
LIBNAME=db.a
CPPFLAGS="-D_POSIX_SOURCE=0" "-D__KERNEL__=1"
CFLAGS=$(CPPFLAGS) -O
ARFLAGS=rs
CRT=/lib/crts0.o
LIBC=/lib/libc.a

OBJECTS=trace0.o	\
	trace1.o	\
	trace2.o	\
	trace3.o	\
	trace4.o	\
	trace5.o	\
	trace6.o

MCHOBJS=i8086a.o	\
	i8086b.o	\
	i8086c.o	\
	i8086d.o	

db:		$(OBJECTS) $(MCHOBJS)
	$(CC) -o $(DESTDIR)db $(OBJECTS) $(MCHOBJS)	#	$(LD) $(LFLAGS) $(CRT) $(OBJECTS) $(MCHOBJS) $(LIBC) -o db

trace0.o:	trace0.c		\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace1.o:	trace1.c		\
			$(INCDIR)/stdio.h	\
			$(INCDIR)/ctype.h	\
			$(INCDIR)/canon.h	\
			$(INCDIR)/l.out.h	\
			$(SYSINCDIR)/proc.h	\
			$(INCDIR)/signal.h	\
			$(SYSINCDIR)/uproc.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace2.o:	trace2.c		\
			$(INCDIR)/stdio.h	\
			$(INCDIR)/ctype.h	\
			$(INCDIR)/errno.h	\
			$(INCDIR)/l.out.h	\
			$(INCDIR)/signal.h	\
			$(SYSINCDIR)/uproc.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace3.o:	trace3.c		\
			$(INCDIR)/stdio.h	\
			$(INCDIR)/ctype.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace4.o:	trace4.c		\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace5.o:	trace5.c		\
			$(INCDIR)/stdio.h	\
			$(INCDIR)/ctype.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

trace6.o:	trace6.c		\
			$(INCDIR)/stdio.h	\
			$(INCDIR)/ctype.h	\
			$(INCDIR)/canon.h	\
			$(INCDIR)/l.out.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			$(LCLDIR)/trace.h

i8086a.o:	i8086a.c	\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/param.h 	\
			$(INCDIR)/l.out.h	\
			$(INCDIR)/signal.h	\
			$(SYSINCDIR)/uproc.h	\
			$(LCLDIR)/trace.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			i8086.h

i8086b.o:	i8086b.c	\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/param.h	\
			$(LCLDIR)/trace.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			i8086.h

i8086c.o:	i8086c.c	\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/param.h	\
			$(INCDIR)/l.out.h	\
			$(LCLDIR)/trace.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			i8086.h

i8086d.o:	i8086d.c	\
			$(INCDIR)/stdio.h	\
			$(SYSINCDIR)/param.h	\
			$(INCDIR)/l.out.h	\
			$(LCLDIR)/trace.h	\
			$(SYSINCDIR)/types.h	\
			i8086trace.h	\
			i8086.h
