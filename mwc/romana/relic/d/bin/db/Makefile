# Makefile for the db symbolic debugger.
# This assumes TARG has been defined to be the target environment.
#	MACROS

DESTDIR=/interim/bin/
CC = exec /bin/cc
CFLAGS = -c -O -I/usr/src/sys/h -I/usr/src/sys/$(TARG)/h -I$(TARG) -I.
LDFLAGS = -f
TARG=i8086

OBJS =	$(TARG)/trace0.o $(TARG)/trace1.o $(TARG)/trace2.o \
	$(TARG)/trace3.o $(TARG)/trace4.o $(TARG)/trace5.o $(TARG)/trace6.o

I86OBJS = i8086/i8086a.o i8086/i8086b.o i8086/i8086c.o i8086/i8086d.o


#	TARGETS
i8086/db: $(OBJS) $(I86OBJS)
	$(CC) $(LDFLAGS) -o $(DESTDIR)db $(OBJS) $(I86OBJS)
	su root chmog 511 bin bin $(DESTDIR)db

#	MACHINE INDEPENDENT OBJECTS
$(TARG)/trace0.o: trace.h $(TARG)/mtrace.h trace0.c
	$(CC) $(CFLAGS) trace0.c
	mv trace0.o $(TARG)/trace0.o

$(TARG)/trace1.o: trace.h $(TARG)/mtrace.h trace1.c
	$(CC) $(CFLAGS) trace1.c
	mv trace1.o $(TARG)/trace1.o

$(TARG)/trace2.o: trace.h $(TARG)/mtrace.h trace2.c
	$(CC) $(CFLAGS) trace2.c
	mv trace2.o $(TARG)/trace2.o

$(TARG)/trace3.o: trace.h $(TARG)/mtrace.h trace3.c
	$(CC) $(CFLAGS) trace3.c
	mv trace3.o $(TARG)/trace3.o

$(TARG)/trace4.o: trace.h $(TARG)/mtrace.h trace4.c
	$(CC) $(CFLAGS) trace4.c
	mv trace4.o $(TARG)/trace4.o

$(TARG)/trace5.o: trace.h $(TARG)/mtrace.h trace5.c
	$(CC) $(CFLAGS) trace5.c
	mv trace5.o $(TARG)/trace5.o

$(TARG)/trace6.o: trace.h $(TARG)/mtrace.h trace6.c
	$(CC) $(CFLAGS) trace6.c
	mv trace6.o $(TARG)/trace6.o

#	MACHINE DEPENDENT OBJECTS
$(TARG)/$(TARG)a.o: trace.h $(TARG)/$(TARG).h $(TARG)/$(TARG)a.c
	$(CC) $(CFLAGS) $(TARG)/$(TARG)a.c
	mv $(TARG)a.o $(TARG)/$(TARG)a.o

$(TARG)/$(TARG)b.o: trace.h $(TARG)/$(TARG).h $(TARG)/$(TARG)b.c
	$(CC) $(CFLAGS) $(TARG)/$(TARG)b.c
	mv $(TARG)b.o $(TARG)/$(TARG)b.o

$(TARG)/$(TARG)c.o: trace.h $(TARG)/$(TARG).h $(TARG)/$(TARG)c.c
	$(CC) $(CFLAGS) $(TARG)/$(TARG)c.c
	mv $(TARG)c.o $(TARG)/$(TARG)c.o

$(TARG)/$(TARG)d.o: trace.h $(TARG)/$(TARG).h $(TARG)/$(TARG)d.c
	$(CC) $(CFLAGS) $(TARG)/$(TARG)d.c
	mv $(TARG)d.o $(TARG)/$(TARG)d.o


#	SOURCES
SRCS =	trace0.c trace1.c trace2.c trace3.c trace4.c trace5.c trace6.c

I86SRCS = i8086/i8086a.c i8086/i8086b.c i8086/i8086c.c i8086/i8086d.c

