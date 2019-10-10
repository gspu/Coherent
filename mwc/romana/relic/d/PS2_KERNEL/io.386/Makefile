#
# Makefile for ibm specific coherent sources and coherent images.
#

USRINC=/usr/include
SYSINC=$(USRINC)/sys
AS=exec /bin/as
CC=exec /bin/cc
CFLAGS=
ASFLAGS=-g

IOOBJ = \
	$(K386OBJ)/aha.o \
	$(K386OBJ)/aha_dsl.o \
	$(K386OBJ)/albaud.o \
	$(K386OBJ)/alx.o \
	$(K386OBJ)/asy.o \
	$(K386OBJ)/at.o \
	$(K386OBJ)/bufq.o \
	$(K386OBJ)/com1.o \
	$(K386OBJ)/com2.o \
	$(K386OBJ)/ct.o \
	$(K386OBJ)/dmareq.o \
	$(K386OBJ)/fdisk.o \
	$(K386OBJ)/fl.o \
	$(K386OBJ)/hs.o \
	$(K386OBJ)/ipc.o \
	$(K386OBJ)/kb.o \
	$(K386OBJ)/kb_d.o \
	$(K386OBJ)/kb_f.o \
	$(K386OBJ)/lp.o \
	$(K386OBJ)/mm.o \
	$(K386OBJ)/mmas.o \
	$(K386OBJ)/msg.o \
	$(K386OBJ)/msgcon.o \
	$(K386OBJ)/nkb.o \
	$(K386OBJ)/pty.o \
	$(K386OBJ)/putchar.o \
	$(K386OBJ)/rm.o \
	$(K386OBJ)/scsi.o \
	$(K386OBJ)/sem.o \
	$(K386OBJ)/semcon.o \
	$(K386OBJ)/shm.o \
	$(K386OBJ)/shmcon.o \
	$(K386OBJ)/ss.o \
	$(K386OBJ)/tty.o \
	$(K386OBJ)/vtmm.o \
	$(K386OBJ)/vtmmas.o \
	$(K386OBJ)/vtnkb.o

all: $(IOOBJ)
	@echo "io objects up to date"

$(K386OBJ)/aha.o: aha.c $(SYSINC)/aha154x.h
	$(CC) $(CFLAGS) -c -o $@ aha.c

$(K386OBJ)/aha_dsl.o: aha_dsl.c $(SYSINC)/aha154x.h
	$(CC) $(CFLAGS) -c -o $@ aha_dsl.c

$(K386OBJ)/albaud.o: albaud.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/alx.o:  alx.c \
	$(SYSINC)/clist.h $(SYSINC)/coherent.h \
	$(SYSINC)/types.h $(SYSINC)/timeout.h \
	$(SYSINC)/reg.h $(SYSINC)/param.h $(SYSINC)/fun.h $(SYSINC)/con.h
	$(CC) $(CFLAGS) -c -o $@ alx.c

$(K386OBJ)/asy.o: asy.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/at.o:	at.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/bufq.o: bufq.c $(SYSINC)/buf.h $(SYSINC)/coherent.h
	$(CC) $(CFLAGS) -DALCOM1=1 -c -o $@ bufq.c

$(K386OBJ)/com1.o: al.c \
		$(SYSINC)/clist.h	$(SYSINC)/coherent.h $(SYSINC)/types.h \
		$(SYSINC)/timeout.h $(SYSINC)/reg.h $(SYSINC)/param.h \
		$(SYSINC)/fun.h $(SYSINC)/con.h $(USRINC)/errno.h $(SYSINC)/reg.h \
		$(SYSINC)/ins8250.h $(SYSINC)/sched.h $(SYSINC)/stat.h \
		$(SYSINC)/timeout.h $(SYSINC)/tty.h $(SYSINC)/ktty.h $(SYSINC)/uproc.h
	$(CC) $(CFLAGS) -DALCOM1=1 -c -o $@ al.c

$(K386OBJ)/com2.o: al.c	\
		$(SYSINC)/clist.h	$(SYSINC)/coherent.h $(SYSINC)/types.h \
		$(SYSINC)/timeout.h $(SYSINC)/reg.h $(SYSINC)/param.h \
		$(SYSINC)/fun.h $(SYSINC)/con.h $(USRINC)/errno.h $(SYSINC)/reg.h \
		$(SYSINC)/ins8250.h $(SYSINC)/sched.h $(SYSINC)/stat.h \
		$(SYSINC)/timeout.h $(SYSINC)/tty.h $(SYSINC)/ktty.h $(SYSINC)/uproc.h
	$(CC) $(CFLAGS) -DALCOM2=1 -c -o $@ al.c

$(K386OBJ)/ct.o: ct.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/dmareq.o:			\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
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
		dmareq.c
	$(CC) $(CFLAGS) -c -o $@ dmareq.c

$(K386OBJ)/fdisk.o:			\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/fdisk.h	\
		$(SYSINC)/inode.h	\
		$(SYSINC)/uproc.h	\
		fdisk.c
	$(CC) $(CFLAGS) -c -o $@ fdisk.c

$(K386OBJ)/fl.o:				\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/dmac.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/fdioctl.h	\
		$(SYSINC)/reg.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/uproc.h	\
		fl.c
	$(CC) $(CFLAGS) -c -o $@ fl.c

$(K386OBJ)/fontw.o: fontgen.c
	$(CC) -i fontgen.c -o fontgen
	: ./fontgen > $*.s
	rm fontgen
	: $(AS) -gxo $@ $*.s

$(K386OBJ)/hs.o: hs.c
	$(CC) $(CFLAGS) -c -o $@ hs.c

$(K386OBJ)/ipc.o: ipc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/kb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/reg.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		kb.c
	$(CC) $(CFLAGS) -c -o $@ kb.c

$(K386OBJ)/kb_d.o: kb_d.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/kb_f.o: kb_f.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/lp.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/reg.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/uproc.h	\
		lp.c
	$(CC) $(CFLAGS) -c -o $@ lp.c

$(K386OBJ)/mm.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/sched.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		$(SYSINC)/timeout.h	\
		mm.c
	$(CC) $(CFLAGS) -c -o $@ mm.c

$(K386OBJ)/mmas.o:	mmas.s
	$(AS) -g -o $@ $<

$(K386OBJ)/msg.o: msg.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/msgcon.o: msgcon.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/nkb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/kb.h	\
		$(SYSINC)/reg.h	\
		$(SYSINC)/seg.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		nkb.c
	$(CC) $(CFLAGS) -c -o $@ nkb.c

$(K386OBJ)/pty.o:  pty.c \
	$(SYSINC)/stat.h $(SYSINC)/uproc.h \
	$(SYSINC)/tty.h $(SYSINC)/con.h \
	$(SYSINC)/devices.h $(USRINC)/errno.h
	$(CC) $(CFLAGS) -c -o $@ pty.c

$(K386OBJ)/putchar.o:			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/reg.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/asy.h		\
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h		\
		$(SYSINC)/inode.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/stat.h	\
		putchar.c
	$(CC) $(CFLAGS) -c -o $@ putchar.c

$(K386OBJ)/rm.o: rm.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/scsi.o: scsi.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/sem.o: sem.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/semcon.o: semcon.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/shm.o: shm.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/shmcon.o: shmcon.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/ss.o: ss.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/tty.o: tty.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/vtmm.o: vtmm.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/vtmmas.o:	vtmmas.s
	$(AS) -g -o $@ $<

$(K386OBJ)/vtnkb.o: vtnkb.c
	$(CC) $(CFLAGS) -c -o $@ $<
