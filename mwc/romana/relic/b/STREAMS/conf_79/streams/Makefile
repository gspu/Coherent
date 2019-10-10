# Simple makefile for parts of STREAMS relating to Coherent.

TMPDIR = .

OBJS= $(TMPDIR)/_putput.o $(TMPDIR)/assert.o $(TMPDIR)/cmn_err.o \
	$(TMPDIR)/confinfo.o $(TMPDIR)/ddi_data.o $(TMPDIR)/defer.o \
	$(TMPDIR)/devno.o $(TMPDIR)/drv_ddi.o $(TMPDIR)/fhsys.o \
	$(TMPDIR)/kmem.o $(TMPDIR)/ksynch.o $(TMPDIR)/poll.o \
	$(TMPDIR)/st_alloc.o $(TMPDIR)/strentry.o $(TMPDIR)/strhead.o \
	$(TMPDIR)/strmisc.o $(TMPDIR)/strmlib.o $(TMPDIR)/struser.o \
	$(TMPDIR)/timeout.o $(TMPDIR)/uio.o $(TMPDIR)/v_proc.o \
	$(TMPDIR)/xcohio.o $(TMPDIR)/xcohlock.o $(TMPDIR)/xcohspl.o \
	$(TMPDIR)/strings.o

all: Driver.o Stub.o

Driver.o : $(OBJS)
	ld -r -o Driver.o $(OBJS)

Stub.o : stub.c
	$(CC) $(CFLAGS) -o Stub.o -c stub.c

clean:
	rm -f $(OBJS) Driver.o Stub.o

install:
	cp Space.c Driver.o Stub.o /etc/conf/streams

$(TMPDIR)/_putput.o : _putput.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/_putput.o -c _putput.c

$(TMPDIR)/assert.o : assert.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/assert.o -c assert.c

$(TMPDIR)/cmn_err.o : cmn_err.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/cmn_err.o -c cmn_err.c

$(TMPDIR)/confinfo.o : confinfo.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $(TMPDIR)/confinfo.o -c confinfo.c

$(TMPDIR)/ddi_data.o : ddi_data.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $(TMPDIR)/ddi_data.o -c ddi_data.c

$(TMPDIR)/defer.o : defer.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/defer.o -c defer.c

$(TMPDIR)/devno.o : devno.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/devno.o -c devno.c

$(TMPDIR)/drv_ddi.o : drv_ddi.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/drv_ddi.o -c drv_ddi.c

$(TMPDIR)/fhsys.o : fhsys.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/fhsys.o -c fhsys.c

$(TMPDIR)/kmem.o : kmem.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/kmem.o -c kmem.c

$(TMPDIR)/ksynch.o : ksynch.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $(TMPDIR)/ksynch.o -c ksynch.c

$(TMPDIR)/poll.o : poll.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/poll.o -c poll.c

$(TMPDIR)/st_alloc.o : st_alloc.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/st_alloc.o -c st_alloc.c

$(TMPDIR)/strentry.o : strentry.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/strentry.o -c strentry.c

$(TMPDIR)/strhead.o : strhead.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/strhead.o -c strhead.c

$(TMPDIR)/strmisc.o : strmisc.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/strmisc.o -c strmisc.c

$(TMPDIR)/strmlib.o : strmlib.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/strmlib.o -c strmlib.c

$(TMPDIR)/struser.o : struser.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/struser.o -c struser.c

$(TMPDIR)/timeout.o : timeout.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/timeout.o -c timeout.c

$(TMPDIR)/uio.o : uio.c
	$(CC) $(CFLAGS) -o $(TMPDIR)/uio.o -c uio.c

$(TMPDIR)/v_proc.o : v_proc.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -D__VOID__=char -o $(TMPDIR)/v_proc.o -c v_proc.c

$(TMPDIR)/xcohio.o : xcohio.s
	$(CC) $(CFLAGS) -o $(TMPDIR)/xcohio.o -c xcohio.s

$(TMPDIR)/xcohlock.o : xcohlock.s
	$(CC) $(CFLAGS) -o $(TMPDIR)/xcohlock.o -c xcohlock.s

$(TMPDIR)/xcohspl.o : xcohspl.s
	$(CC) $(CFLAGS) -o $(TMPDIR)/xcohspl.o -c xcohspl.s

$(TMPDIR)/strings.o : strings.s
	$(CC) $(CFLAGS) -o $(TMPDIR)/strings.o -c strings.s

