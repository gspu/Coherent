#
# Makefile for ibm specific coherent sources and coherent images.
#

USRINC=/usr/include
SYSINC=$(USRINC)/sys
AS=exec /bin/as
CC=exec /bin/cc
ASFLAGS=-g
T=/tmp

# HAIINC = $(SYSINC)/haiinst.h $(SYSINC)/haiscsi.h

HAIOBJ = \
	$(K386OBJ)/hai154x.o \
	$(K386OBJ)/haict.o \
	$(K386OBJ)/haiscsi.o \
	$(K386OBJ)/haisd.o

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
	$(K386OBJ)/fdc.o \
	$(K386OBJ)/fl386.o \
	$(K386OBJ)/ft.o \
	$(K386OBJ)/hai.o \
	$(K386OBJ)/hs.o \
	$(K386OBJ)/ipc.o \
	$(K386OBJ)/lp.o \
	$(K386OBJ)/kb.o \
	$(K386OBJ)/mm.o \
	$(K386OBJ)/mmas.o \
	$(K386OBJ)/msg.o \
	$(K386OBJ)/nkb.o \
	$(K386OBJ)/pty.o \
	$(K386OBJ)/putchar.o \
	$(K386OBJ)/rm.o \
	$(K386OBJ)/scsi.o \
	$(K386OBJ)/sem386.o \
	$(K386OBJ)/shm1.o \
	$(K386OBJ)/ss.o \
	$(K386OBJ)/tty.o \
	$(K386OBJ)/vtkb.o \
	$(K386OBJ)/vtnkb.o \
	$(K386OBJ)/vtmm.o \
	$(K386OBJ)/vtmmas.o \
	$(K386OBJ)/xla.o \
	$(K386OBJ)/xl_dec.o \
	$(K386OBJ)/xlft.o
#	$(K386OBJ)/kb.o \
#	$(K386OBJ)/kb_d.o \
#	$(K386OBJ)/kb_f.o \
#	$(K386OBJ)/vtkb.o \
#	$(K386OBJ)/vtkb_d.o \
#	$(K386OBJ)/vtkb_f.o \

all: $(IOOBJ)
	@echo "io objects up to date"

clean:
	@:

$(K386OBJ)/aha.o: aha.c $(SYSINC)/aha154x.h
	$(CC) $(CFLAGS) -c -o $@ aha.c

$(K386OBJ)/aha_dsl.o: aha_dsl.c $(SYSINC)/aha154x.h
	$(CC) $(CFLAGS) -c -o $@ aha_dsl.c

$(K386OBJ)/albaud.o: albaud.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/alx.o:  alx.c \
	$(SYSINC)/clist.h $(SYSINC)/coherent.h \
	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
	$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h $(SYSINC)/con.h
	$(CC) $(CFLAGS) -c -o $@ alx.c

$(K386OBJ)/asy.o: asy.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/at.o:	at.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/bufq.o: bufq.c $(SYSINC)/buf.h $(SYSINC)/coherent.h
	$(CC) $(CFLAGS) -DALCOM1=1 -c -o $@ bufq.c

$(K386OBJ)/com1.o: al.c \
		$(SYSINC)/clist.h $(SYSINC)/coherent.h $(SYSINC)/types.h \
		$(USRINC)/kernel/timeout.h $(USRINC)/kernel/reg.h \
		$(USRINC)/kernel/param.h $(SYSINC)/con.h $(USRINC)/sys/errno.h \
		$(SYSINC)/ins8250.h $(SYSINC)/sched.h $(SYSINC)/stat.h \
		$(SYSINC)/tty.h $(SYSINC)/ktty.h $(SYSINC)/uproc.h
	$(CC) $(CFLAGS) -DALCOM1=1 -c -o $@ al.c

$(K386OBJ)/com2.o: al.c	\
		$(SYSINC)/clist.h $(SYSINC)/coherent.h $(SYSINC)/types.h \
		$(USRINC)/kernel/timeout.h $(USRINC)/kernel/reg.h \
		$(USRINC)/kernel/param.h $(SYSINC)/con.h $(USRINC)/sys/errno.h \
		$(SYSINC)/ins8250.h $(SYSINC)/sched.h $(SYSINC)/stat.h \
		$(SYSINC)/tty.h $(SYSINC)/ktty.h $(SYSINC)/uproc.h
	$(CC) $(CFLAGS) -DALCOM2=1 -c -o $@ al.c

$(K386OBJ)/ct.o: ct.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/dmareq.o:			\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/dmac.h	\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(USRINC)/poll.h \
		$(SYSINC)/sched.h	\
		$(SYSINC)/seg.h		\
		$(SYSINC)/stat.h	\
		$(SYSINC)/uproc.h	\
		dmareq.c
	$(CC) $(CFLAGS) -c -o $@ dmareq.c

$(K386OBJ)/fdisk.o:			\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/fdisk.h	\
		$(SYSINC)/file.h	\
		$(SYSINC)/uproc.h	\
		fdisk.c
	$(CC) $(CFLAGS) -c -o $@ fdisk.c

$(K386OBJ)/fdc.o:				\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/fdc765.h		\
		$(SYSINC)/uproc.h	\
		fdc.c
	$(CC) $(CFLAGS) -c -o $@ fdc.c

$(K386OBJ)/fl386.o:				\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/fdioctl.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/fdc765.h	\
		$(SYSINC)/uproc.h	\
		$(SYSINC)/file.h	\
		fl386.c
	$(CC) $(CFLAGS) -c -o $@ fl386.c

$(K386OBJ)/fontw.o: fontgen.c
	$(CC) -i fontgen.c -o fontgen
	: ./fontgen > $*.s
	rm fontgen
	: $(AS) -gxo $@ $*.s

$(K386OBJ)/ft.o: ft.c
	$(CC) $(CFLAGS) -VSUVAR -c -o $@ $<

$(K386OBJ)/hai.o: $(HAIOBJ) $(HAIINC)
	ld -r -o $@ $(HAIOBJ)

$(K386OBJ)/hai154x.o: hai154x.c $(HAIINC)
	$(CC) $(CFLAGS) -c -o $@ hai154x.c

$(K386OBJ)/haict.o: haict.c $(HAIINC)
	$(CC) $(CFLAGS) -c -o $@ haict.c

$(K386OBJ)/haiscsi.o: haiscsi.c $(HAIINC)
	$(CC) $(CFLAGS) -c -o $@ haiscsi.c

$(K386OBJ)/haisd.o: haisd.c $(HAIINC)
	$(CC) $(CFLAGS) -c -o $@ haisd.c

$(K386OBJ)/hs.o: hs.c
	$(CC) $(CFLAGS) -c -o $@ hs.c

$(K386OBJ)/ipc.o: ipc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/kb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		kb.c
	$(CC) $(CFLAGS) -c -o $@ kb.c

$(K386OBJ)/vtkb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		$(SYSINC)/kb.h		\
		$(SYSINC)/vt.h		\
		vtkb.c
	$(CC) $(CFLAGS) -c -o $@ vtkb.c

$(K386OBJ)/lp.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(USRINC)/poll.h \
		$(SYSINC)/stat.h	\
		$(SYSINC)/uproc.h	\
		lp386.c
	$(CC) $(CFLAGS) -c -o $@ lp386.c

$(K386OBJ)/mm.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/sched.h	\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		mm.c
	$(CC) $(CFLAGS) -c -o $@ mm.c

$(K386OBJ)/mmas.o:	mmas.s
	$(AS) -g -x -o $@ $<

$(K386OBJ)/msg.o: msg386.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/nkb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/param.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/sys/errno.h	\
		$(SYSINC)/kb.h	\
		$(USRINC)/kernel/reg.h	\
		$(SYSINC)/seg.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		nkb.c
	$(CC) $(CFLAGS) -c -o $@ nkb.c

$(K386OBJ)/pty.o:  pty.c \
	$(SYSINC)/stat.h $(SYSINC)/uproc.h \
	$(SYSINC)/tty.h $(SYSINC)/con.h \
	$(SYSINC)/devices.h $(USRINC)/sys/errno.h
	$(CC) $(CFLAGS) -c -o $@ pty.c

$(K386OBJ)/putchar.o:			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(USRINC)/kernel/timeout.h \
					$(USRINC)/kernel/reg.h $(USRINC)/kernel/param.h \
		$(SYSINC)/asy.h		\
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/file.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/stat.h	\
		putchar.c
	$(CC) $(CFLAGS) -c -o $@ putchar.c

$(K386OBJ)/rm.o: rm.c \
		$(SYSINC)/file.h
	$(CC) $(CFLAGS) -c -o $@ rm.c

$(K386OBJ)/scsi.o: scsi.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/sem386.o: sem386.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/shm1.o: shm1.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/ss.o: ss.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/tty.o: tty.c \
		$(SYSINC)/file.h
	$(CC) $(CFLAGS) -c -o $@ tty.c

$(K386OBJ)/vtmm.o: vtmm.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/vtmmas.o:	vtmmas.s
	$(AS) -g -x -o $@ $<

$(K386OBJ)/vtnkb.o: vtnkb.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/xlft.o: xlft.c \
		$(SYSINC)/file.h
	$(CC) $(CFLAGS) -VSUVAR -c -o $@ xlft.c

$(K386OBJ)/xl_dec.o: xl_dec.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K386OBJ)/xla.o:	xla.s
	$(AS) -g -o $@ $<
