#
#	makefile for Coherent 386 - overall system build
#
#	Do not use this makefile directly!  Use a build script.
#	(Unless you know what you are doing.)
#

all:	$(K386LIB)/k386.a

clean:
	@:

AR=exec /bin/ar
ARFLAGS=rs
AS=exec /bin/as
CC=exec /bin/cc
CFLAGS=-VPSTR

USRSRC=/usr/src
USRINC=/usr/include
SYSINC=$(USRINC)/sys

IOOBJ =\
	$(K386OBJ)/aha.o $(K386OBJ)/aha_dsl.o $(K386OBJ)/scsi.o \
	$(K386OBJ)/at.o \
	$(K386OBJ)/hai.o \
	$(K386OBJ)/ss.o \
	$(K386OBJ)/kb.o $(K386OBJ)/nkb.o $(K386OBJ)/vtkb.o $(K386OBJ)/vtnkb.o \
	$(K386OBJ)/mm.o $(K386OBJ)/mmas.o \
	$(K386OBJ)/vtmm.o $(K386OBJ)/vtmmas.o \
	$(K386OBJ)/xla.o $(K386OBJ)/xlft.o $(K386OBJ)/xl_dec.o \
	$(K386OBJ)/fdc.o $(K386OBJ)/fl386.o \
	$(K386OBJ)/albaud.o \
	$(K386OBJ)/alx.o $(K386OBJ)/com1.o $(K386OBJ)/com2.o \
	$(K386OBJ)/hs.o \
	$(K386OBJ)/asy.o \
	$(K386OBJ)/pty.o \
	$(K386OBJ)/lp.o \
	$(K386OBJ)/bufq.o \
	$(K386OBJ)/ct.o \
	$(K386OBJ)/dmareq.o \
	$(K386OBJ)/fdisk.o \
	$(K386OBJ)/ft.o \
	$(K386OBJ)/ipc.o \
	$(K386OBJ)/msg.o \
	$(K386OBJ)/putchar.o \
	$(K386OBJ)/rm.o \
	$(K386OBJ)/sem386.o \
	$(K386OBJ)/shm1.o \
	$(K386OBJ)/tty.o

COHOBJ =\
	$(K386OBJ)/alloc.o \
	$(K386OBJ)/bio.o \
	$(K386OBJ)/clist.o \
	$(K386OBJ)/clock.o \
	$(K386OBJ)/clocked.o \
	$(K386OBJ)/exec.o \
	$(K386OBJ)/fd.o \
	$(K386OBJ)/fifo.o \
	$(K386OBJ)/fs1.o \
	$(K386OBJ)/fs2.o \
	$(K386OBJ)/fs3.o \
	$(K386OBJ)/main.o \
	$(K386OBJ)/misc.o \
	$(K386OBJ)/nlp.o \
	$(K386OBJ)/null.o \
	$(K386OBJ)/pipe.o \
	$(K386OBJ)/printf.o \
	$(K386OBJ)/proc.o \
	$(K386OBJ)/rlock.o \
	$(K386OBJ)/seg.o \
	$(K386OBJ)/sig.o \
	$(K386OBJ)/support.o \
	$(K386OBJ)/sys1.o \
	$(K386OBJ)/sys2.o \
	$(K386OBJ)/sys3.o \
	$(K386OBJ)/sys4.o \
	$(K386OBJ)/sys5.o \
	$(K386OBJ)/syscoh.o \
	$(K386OBJ)/timeout.o \
	$(K386OBJ)/var.o \
	$(K386OBJ)/ker_data.o \
	$(K386OBJ)/poll.o	# Soon to disappear

I386OBJ0 =\
	$(K386OBJ)/as.o \
	$(K386OBJ)/xdt.o \

I386OBJ1 =\
	$(K386OBJ)/atcon.o \
	$(K386OBJ)/defer.o \
	$(K386OBJ)/die.o \
	$(K386OBJ)/dmac.o \
	$(K386OBJ)/dmalock.o \
	$(K386OBJ)/fakedma.o \
	$(K386OBJ)/ff.o \
	$(K386OBJ)/md.o \
	$(K386OBJ)/mem_cache.o \
	$(K386OBJ)/mmu.o \
	$(K386OBJ)/msig.o \
	$(K386OBJ)/ndp.o \
	$(K386OBJ)/ndpas.o \
	$(K386OBJ)/shm0.o \
	$(K386OBJ)/sys1632.o \
	$(K386OBJ)/tioc.o \
	$(K386OBJ)/trap.o \
	$(K386OBJ)/work.o \
	$(K386OBJ)/memory.o

I386OBJ = $(I386OBJ0) $(I386OBJ1)

STUBS = $(K386OBJ)/stubs.o

# LIBLIST is the .a files needed for 386 installation.
# k386.a is an aggregate used since 92/11/11 for driver kit kernel links.
# kl386.a was used in COH 4.0.0 and 4.0.1 to link a kernel on customer's
#   hard drive during installation.
LIBLIST= \
	$(K386LIB)/k386.a \
	$(K386LIB)/k0.o

# k386.a is used for linking driver kit kernels.
$(K386LIB)/k386.a: $(I386OBJ1) $(COHOBJ) $(IOOBJ)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(I386OBJ1) $(COHOBJ) $(IOOBJ)

$(K386LIB)/k0.o: $(I386OBJ0)
	ld -r -o $@ $(I386OBJ0)

coh:
	cd ../coh.386; make "CFLAGS=$(CFLAGS)" "CC=$(CC)"

io:
	cd ../io.386; make "CFLAGS=$(CFLAGS)" "CC=$(CC)"

libs:	coh io $(LIBLIST)
	@/bin/echo "Installation libraries up to date."

$(K386OBJ)/as.o: as.s as.inc
	$(AS) -g -o $@ as.s

$(K386OBJ)/xdt.o: xdt.s as.inc
	$(AS) -g -o $@ xdt.s

$(K386OBJ)/atcon.o:			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/mtype.h	\
		$(SYSINC)/stat.h	\
		atcon.c
	$(CC) $(CFLAGS) -c -o $@ atcon.c

$(K386OBJ)/defer.o: defer.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/die.o: die.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/dmac.o:	dmac.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/dmalock.o:	dmalock.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/fakedma.o:	fakedma.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/ff.o: ff.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/fvcon.o:			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/mtype.h	\
		$(SYSINC)/stat.h	\
		fvcon.c
	$(CC) $(CFLAGS) -c -o $@ fvcon.c

$(K386OBJ)/md.o: md.c
	$(CC) $(CFLAGS) -D_ENABLE_STREAMS=1 -c -o $@ $<

$(K386OBJ)/mem_cache.o: mem_cache.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/memory.o: memory.s
	$(AS) -o $@ memory.s

$(K386OBJ)/mmu.o: mmu.c
	$(CC) $(CFLAGS) -VSUVAR -c -o $@ $<

$(K386OBJ)/msig.o: msig.c $(SYSINC)/coherent.h
	$(CC) $(CFLAGS) -VSUVAR -c -o $@ msig.c

$(K386OBJ)/ndpas.o: ndpas.s
	$(AS) -g -o $@ ndpas.s

$(K386OBJ)/ndp.o: ndp.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/shm0.o: shm0.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/stubs.o: stubs.s
	$(AS) -g -o $@ stubs.s

$(K386OBJ)/sys1632.o: sys1632.c \
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/acct.h	\
		$(SYSINC)/buf.h		\
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/filsys.h	\
		$(SYSINC)/ino.h		\
		$(SYSINC)/inode.h	\
		$(USRINC)/l.out.h	\
		$(SYSINC)/proc.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/seg.h		\
		$(USRINC)/signal.h	\
		$(SYSINC)/uproc.h	\
		$(USRINC)/kernel/reg.h		\
		$(USRINC)/kernel/systab.h	\
		$(SYSINC)/oldstat.h	\
		$(SYSINC)/timeb.h	\
		$(SYSINC)/fd.h		\
		$(USRINC)/sgtty.h
	$(CC) $(CFLAGS) -c -o $@ sys1632.c

$(K386OBJ)/tioc.o: tioc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/trap.o: trap.c $(USRINC)/kernel/systab.h
	$(CC) $(CFLAGS) -VSUVAR -c -o $@ trap.c

$(K386OBJ)/work.o: work.c
	$(CC) $(CFLAGS) -c -o $@ $<
