# Simple makefile for parts of STREAMS relating to Coherent.

TMPDIR= ../obj
INSTALL_DIR= ..

OBJS= $(TMPDIR)/_putput.o $(TMPDIR)/assert.o $(TMPDIR)/cmn_err.o \
	$(TMPDIR)/ddientry.o $(TMPDIR)/ddi_drv.o $(TMPDIR)/ddi_data.o \
	$(TMPDIR)/defer.o $(TMPDIR)/devno.o $(TMPDIR)/drv_ddi.o \
	$(TMPDIR)/fhsys.o $(TMPDIR)/kmem.o $(TMPDIR)/ksynch.o \
	$(TMPDIR)/poll.o $(TMPDIR)/st_alloc.o $(TMPDIR)/strhead.o \
	$(TMPDIR)/strmem.o $(TMPDIR)/strmisc.o $(TMPDIR)/strmlib.o \
	$(TMPDIR)/struser.o $(TMPDIR)/str_drv.o $(TMPDIR)/timeout.o \
	$(TMPDIR)/uio.o $(TMPDIR)/v_proc.o $(TMPDIR)/xcohio.o \
	$(TMPDIR)/xcohlock.o $(TMPDIR)/xcohspl.o $(TMPDIR)/strings.o

all: $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Stub.o $(INSTALL_DIR)/Space.c

$(INSTALL_DIR)/Driver.o : $(OBJS)
	ld -r -o $@ $(OBJS)

$(INSTALL_DIR)/Stub.o : stub.c
	$(CC) $(CFLAGS) -o $@ -c stub.c

$(INSTALL_DIR)/Space.c: Space.c
	ln Space.c $(INSTALL_DIR)

clean:
	rm -f $(OBJS) $(INSTALL_DIR)/Driver.o $(INSTALL_DIR)/Stub.o

$(TMPDIR)/_putput.o : _putput.c
	$(CC) $(CFLAGS) -o $@ -c _putput.c

$(TMPDIR)/assert.o : assert.c
	$(CC) $(CFLAGS) -o $@ -c assert.c

$(TMPDIR)/cmn_err.o : cmn_err.c
	$(CC) $(CFLAGS) -o $@ -c cmn_err.c

$(TMPDIR)/ddientry.o : ddientry.c
	$(CC) $(CFLAGS) -o $@ -c ddientry.c

$(TMPDIR)/ddi_drv.o : ddi_drv.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $@ -c ddi_drv.c

$(TMPDIR)/ddi_data.o : ddi_data.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $@ -c ddi_data.c

$(TMPDIR)/defer.o : defer.c
	$(CC) $(CFLAGS) -o $@ -c defer.c

$(TMPDIR)/devno.o : devno.c
	$(CC) $(CFLAGS) -o $@ -c devno.c

$(TMPDIR)/drv_ddi.o : drv_ddi.c
	$(CC) $(CFLAGS) -o $@ -c drv_ddi.c

$(TMPDIR)/fhsys.o : fhsys.c
	$(CC) $(CFLAGS) -o $@ -c fhsys.c

$(TMPDIR)/kmem.o : kmem.c
	$(CC) $(CFLAGS) -o $@ -c kmem.c

$(TMPDIR)/ksynch.o : ksynch.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $@ -c ksynch.c

$(TMPDIR)/poll.o : poll.c
	$(CC) $(CFLAGS) -o $@ -c poll.c

$(TMPDIR)/st_alloc.o : st_alloc.c
	$(CC) $(CFLAGS) -o $@ -c st_alloc.c

$(TMPDIR)/strentry.o : strentry.c
	$(CC) $(CFLAGS) -o $@ -c strentry.c

$(TMPDIR)/strhead.o : strhead.c
	$(CC) $(CFLAGS) -o $@ -c strhead.c

$(TMPDIR)/strmem.o : strmem.c
	$(CC) $(CFLAGS) -o $@ -c strmem.c

$(TMPDIR)/strmisc.o : strmisc.c
	$(CC) $(CFLAGS) -o $@ -c strmisc.c

$(TMPDIR)/strmlib.o : strmlib.c
	$(CC) $(CFLAGS) -o $@ -c strmlib.c

$(TMPDIR)/struser.o : struser.c
	$(CC) $(CFLAGS) -o $@ -c struser.c

$(TMPDIR)/str_drv.o : str_drv.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $@ -c str_drv.c

$(TMPDIR)/timeout.o : timeout.c
	$(CC) $(CFLAGS) -o $@ -c timeout.c

$(TMPDIR)/uio.o : uio.c
	$(CC) $(CFLAGS) -o $@ -c uio.c

$(TMPDIR)/v_proc.o : v_proc.c
	$(CC) $(CFLAGS) -D_OLD_COH_POLLING=1 -o $@ -c v_proc.c

$(TMPDIR)/xcohio.o : xcohio.s
	$(CC) $(CFLAGS) -o $@ -c xcohio.s

$(TMPDIR)/xcohlock.o : xcohlock.s
	$(CC) $(CFLAGS) -o $@ -c xcohlock.s

$(TMPDIR)/xcohspl.o : xcohspl.s
	$(CC) $(CFLAGS) -o $@ -c xcohspl.s

$(TMPDIR)/strings.o : strings.s
	$(CC) $(CFLAGS) -o $@ -c strings.s

