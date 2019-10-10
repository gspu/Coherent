# $Header: /x/usr/sys/RCS/Makefile,v 1.1 91/11/14 14:36:50 hal Exp $
#
# Makefile for ibm specific coherent sources and coherent images.
#
# Requires environment variables
#	USRSYS (e.g. /usr/sys)
#	USRSRC (e.g. /usr/src/sys)
#	KOBJ (e.g. /usr/kobj)
#
# $Log:	Makefile,v $
# Revision 1.1  91/11/14  14:36:50  hal
# Last before going to 286/coh/io tree.
# 
# Revision 1.3	89/06/30  16:26:39 	src
# Bug:	Lack of DMA lock resulted in failed transfers between SCSI drive
# 	and floppy disk.
# Fix:	Added dma lock routines to insure single DMA transfer. (JHB)
# 
# Revision 1.1	88/03/24  17:33:15	src
# Initial revision
# 

# Include directories
USRINC=/usr/include
SYSINC=/usr/include/sys

# Source directories
COHSRC=$(USRSRC)/coh
DRVSRC=$(USRSRC)/io
I86SRC=$(USRSRC)/286

# Library directory
DRVLIB=$(USRSYS)/lib

# Compiler stuff
CC=exec /bin/cc
CFLAGS=

HERE=	$(KOBJ)/md2.o \
	$(KOBJ)/dmac.o \
	$(KOBJ)/pccon.o \
	$(KOBJ)/console.o \
	$(KOBJ)/dmareq.o \
	$(KOBJ)/mmu.o \
	$(KOBJ)/support.o

FIFO=	$(KOBJ)/fifo_open.o \
	$(KOBJ)/fifo_close.o \
	$(KOBJ)/fifo_read.o \
	$(KOBJ)/fifo_write.o \
	$(KOBJ)/fifo_len.o \
	$(KOBJ)/fifo_rewind.o

DOTDOT=	$(KOBJ)/alloc.o \
	$(KOBJ)/as1.o \
	$(KOBJ)/bio.o \
	$(KOBJ)/clist.o \
	$(KOBJ)/clock.o \
	$(KOBJ)/ct.o \
	$(KOBJ)/defer.o \
	$(KOBJ)/dmalock.o	\
	$(KOBJ)/exec.o \
	$(KOBJ)/fd.o \
	$(KOBJ)/fs1.o $(KOBJ)/fs2.o $(KOBJ)/fs3.o \
	$(KOBJ)/krunch.o \
	$(KOBJ)/ld.o $(KOBJ)/ldas.o \
	$(KOBJ)/main.o \
	$(KOBJ)/md1.o \
	$(KOBJ)/misc.o \
	$(KOBJ)/mmain.o \
	$(KOBJ)/null.o \
	$(KOBJ)/pipe.o \
	$(KOBJ)/poll.o \
	$(KOBJ)/printf.o \
	$(KOBJ)/proc.o \
	$(KOBJ)/seg.o \
	$(KOBJ)/sig.o \
	$(KOBJ)/sys1.o $(KOBJ)/sys2.o $(KOBJ)/sys3.o \
	$(KOBJ)/tab.o \
	$(KOBJ)/timeout.o \
	$(KOBJ)/trap.o \
	$(KOBJ)/var.o

kernel:	$(KOBJ) $(USRSYS)/atkernel.o $(DRVLIB)/support.a $(DRVLIB)/tty.a
	@exec /bin/sync

shrink:
	rm -f $(KOBJ)/* $(KOBJ)/* $(KOBJ)/*

$(KOBJ)/pccon.o: \
	$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
				$(SYSINC)/machine.h $(SYSINC)/param.h \
				$(SYSINC)/fun.h \
	$(SYSINC)/con.h		\
	$(USRINC)/mtype.h	\
	$(SYSINC)/stat.h	\
	$(DRVSRC)/pccon.c
	$(CC) $(CFLAGS) -c -o $@ $(DRVSRC)/pccon.c

$(KOBJ)/console.o:	\
	$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
				$(SYSINC)/machine.h $(SYSINC)/param.h \
				$(SYSINC)/fun.h \
	$(SYSINC)/con.h		\
	$(SYSINC)/inode.h	\
	$(SYSINC)/io.h		\
	$(SYSINC)/stat.h	\
	$(DRVSRC)/console.c
	$(CC) $(CFLAGS) -c -o $@ $(DRVSRC)/console.c

$(KOBJ)/dmareq.o:	\
	$(SYSINC)/buf.h		\
	$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
				$(SYSINC)/machine.h $(SYSINC)/param.h \
				$(SYSINC)/fun.h \
	$(SYSINC)/con.h		\
	$(SYSINC)/dmac.h	\
	$(USRINC)/errno.h	\
	$(SYSINC)/io.h		\
	$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
	$(SYSINC)/sched.h	\
	$(SYSINC)/seg.h		\
	$(SYSINC)/stat.h	\
	$(SYSINC)/uproc.h	\
	$(DRVSRC)/dmareq.c
	$(CC) $(CFLAGS) -c -o $@ $(DRVSRC)/dmareq.c

$(USRSYS)/atkernel.o: $(KOBJ)/as2.obj $(HERE) $(DOTDOT)
	exec /bin/ld -r -o $@ $<

$(KOBJ)/as2.obj: $(I86SRC)/as2.s
	exec as -go $@ $<

$(KOBJ)/dmac.o: $(I86SRC)/dmac.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(KOBJ)/dump.o: $(I86SRC)/dump.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(KOBJ)/md2.o: $(I86SRC)/md2.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Define REAL_MODE if real mode is also required
$(KOBJ)/mmu.o: $(I86SRC)/mmu.c
	$(CC) $(CFLAGS) -DREAL_MODE -c -o $@ $<

$(KOBJ)/support.o:		\
	$(SYSINC)/al.h $(SYSINC)/timeout.h $(SYSINC)/ktty.h \
			$(SYSINC)/poll_clk.h \
	$(I86SRC)/support.c
	$(CC) $(CFLAGS) -c -o $@ $(I86SRC)/support.c

# this stuff was in the /usr/src/sys/i8086 Makefile before 90/08/08
C86FLAGS=-c -DNOMONITOR=1
#
$(KOBJ):
	mkdir $(KOBJ)

$(KOBJ)/alloc.o: $(COHSRC)/alloc.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/as1.o: $(I86SRC)/as1.s
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/bio.o: $(COHSRC)/bio.c
	$(CC) $(C86FLAGS) -DREADAHEAD=0 -o $@ $<

# Clists are assembly source on i8086
$(KOBJ)/clist.o: $(I86SRC)/clist.s $(SYSINC)/const.h
	/lib/cpp -E -Isys -o clist.i $(I86SRC)/clist.s
	as -go $@ clist.i
	rm clist.i

$(KOBJ)/clock.o: $(COHSRC)/clock.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/ct.o: $(COHSRC)/ct.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/defer.o: $(I86SRC)/defer.s
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/dmalock.o: $(I86SRC)/dmalock.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/exec.o: $(I86SRC)/exec.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/fd.o: $(COHSRC)/fd.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/fifo_open.o: $(COHSRC)/fifo_open.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@ $(COHSRC)/fifo_open.c

$(KOBJ)/fifo_close.o: $(COHSRC)/fifo_close.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@ $(COHSRC)/fifo_close.c

$(KOBJ)/fifo_read.o: $(COHSRC)/fifo_read.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@ $(COHSRC)/fifo_read.c

$(KOBJ)/fifo_write.o: $(COHSRC)/fifo_write.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@  $(COHSRC)/fifo_write.c

$(KOBJ)/fifo_len.o: $(COHSRC)/fifo_len.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@  $(COHSRC)/fifo_len.c

$(KOBJ)/fifo_rewind.o: $(COHSRC)/fifo_rewind.c $(SYSINC)/typed.h
	$(CC) $(C86FLAGS) -o $@ $(COHSRC)/fifo_rewind.c

$(KOBJ)/fs1.o: $(COHSRC)/fs1.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/fs2.o: $(COHSRC)/fs2.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/fs3.o: $(COHSRC)/fs3.c
	$(CC) $(C86FLAGS) -DTINY=1 -o $@ $<

$(KOBJ)/krunch.o: $(I86SRC)/krunch.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/ld.o: $(I86SRC)/ld.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/ldas.o: $(I86SRC)/ldas.s
	as -gxo $@ $<

$(KOBJ)/main.o: \
	$(USRSYS)/version \
	$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
				$(SYSINC)/machine.h $(SYSINC)/param.h \
				$(SYSINC)/fun.h $(SYSINC)/mmu.h \
	$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
	$(SYSINC)/seg.h \
	$(SYSINC)/uproc.h \
	$(COHSRC)/main.c
	$(CC) $(C86FLAGS) `$(USRSYS)/version` -o $@ $(COHSRC)/main.c

$(KOBJ)/md1.o: $(I86SRC)/md1.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/misc.o: $(COHSRC)/misc.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/mmain.o: $(I86SRC)/mmain.c $(SYSINC)/const.h
	$(CC) $(C86FLAGS) -o $@ $(I86SRC)/mmain.c

$(KOBJ)/null.o: $(COHSRC)/null.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/pipe.o: $(COHSRC)/pipe.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/poll.o: $(COHSRC)/poll.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/printf.o: $(COHSRC)/printf.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/proc.o: $(COHSRC)/proc.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/seg.o: $(COHSRC)/seg.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/sig.o: $(COHSRC)/sig.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/swap.o: $(COHSRC)/swap.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/elog.o: $(COHSRC)/elog.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/sys1.o: $(COHSRC)/sys1.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/sys2.o: $(COHSRC)/sys2.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/sys3.o: $(COHSRC)/sys3.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/tab.o: $(I86SRC)/tab.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/timeout.o: $(COHSRC)/timeout.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/trap.o: $(I86SRC)/trap.c
	$(CC) $(C86FLAGS) -o $@ $<

$(KOBJ)/var.o: $(COHSRC)/var.c
	$(CC) $(C86FLAGS) -o $@ $<

# Additional libraries searched when "config" runs.

$(DRVLIB)/support.a:	$(KOBJ)/cs_sel.o $(KOBJ)/clocked.o $(FIFO)
	rm -f $@
	ar rc $@ $<

$(KOBJ)/cs_sel.o: $(I86SRC)/cs_sel.s
	as -gxo $@ $<

$(KOBJ)/clocked.o: $(I86SRC)/clocked.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(DRVLIB)/tty.a:	$(KOBJ)/tty.o
	rm -f $@
	ar rc $@ $<

$(KOBJ)/tty.o: $(DRVSRC)/tty.c
	$(CC) $(CFLAGS) -c -o $@ $<
