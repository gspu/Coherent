# Makefile for AT specific Coherent drivers

# System utility directory

# Source directory

# Loadable driver directory
LDRV=$(USRSYS)/ldrv

# Include directories
USRINC=/usr/include
SYSINC=/usr/include/sys

# Object directory
KOBJ=/usr/kobj

ARCHIVES=\
	$(USRSYS)/lib/al.a \
	$(USRSYS)/lib/at.a \
	$(USRSYS)/lib/ati.a \
	$(USRSYS)/lib/fl.a \
	$(USRSYS)/lib/gr.a \
	$(USRSYS)/lib/hs.a \
	$(USRSYS)/lib/kb.a \
	$(USRSYS)/lib/lp.a \
	$(USRSYS)/lib/mm.a \
	$(USRSYS)/lib/ms.a \
	$(USRSYS)/lib/rm.a \
	$(USRSYS)/lib/rs.a \
	$(USRSYS)/lib/st.a \
	$(USRSYS)/lib/tn.a \

DRVOBJ=\
	$(KOBJ)/alx.o \
	$(KOBJ)/at.o \
	$(KOBJ)/atas.o \
	$(KOBJ)/ms.o \
	$(KOBJ)/ati.o \
	$(KOBJ)/com1.o $(KOBJ)/com2.o \
	$(KOBJ)/fdisk.o \
	$(KOBJ)/fl.o \
	$(KOBJ)/fontw.o \
	$(KOBJ)/gr.o $(KOBJ)/gras.o \
	$(KOBJ)/hs.o \
	$(KOBJ)/kb.o \
	$(KOBJ)/mm.o \
	$(KOBJ)/lp.o \
	$(KOBJ)/mmas.o \
	$(KOBJ)/rm.o \
	$(KOBJ)/rs0.o $(KOBJ)/rs1.o $(KOBJ)/rsas.o \
	$(KOBJ)/st.o \
	$(KOBJ)/tn.o $(KOBJ)/tnas.o

DRIVERS=\
	$(LDRV)/al0 \
	$(LDRV)/al1 \
	$(LDRV)/at \
	$(LDRV)/fl \
	$(LDRV)/gr \
	$(LDRV)/hs \
	$(LDRV)/lp \
	$(LDRV)/mm \
	$(LDRV)/ms \
	$(LDRV)/rm

install: $(ARCHIVES) $(DRIVERS)
	@exec /bin/sync

all:	$(DRVOBJ)
	@exec /bin/sync

$(USRSYS)/lib/al.a: $(KOBJ)/com1.o $(KOBJ)/com2.o $(KOBJ)/alx.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/at.a: $(KOBJ)/at.o $(KOBJ)/atas.o $(KOBJ)/fdisk.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/ati.a: $(KOBJ)/mm.o $(KOBJ)/ati.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/fl.a: $(KOBJ)/fl.o
	rm -f $(USRSYS)/lib/fl.a
	ar rc $(USRSYS)/lib/fl.a $(KOBJ)/fl.o
$(USRSYS)/lib/gr.a: $(KOBJ)/mm.o $(KOBJ)/gr.o $(KOBJ)/gras.o \
				$(KOBJ)/fontw.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/hs.a: $(KOBJ)/hs.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/kb.a: $(KOBJ)/kb.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/lp.a: $(KOBJ)/lp.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/mm.a: $(KOBJ)/mm.o $(KOBJ)/mmas.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/ms.a: $(KOBJ)/ms.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/rm.a: $(KOBJ)/rm.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/rs.a: $(KOBJ)/rs0.o $(KOBJ)/rs1.o $(KOBJ)/rsas.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/st.a: $(KOBJ)/st.o
	rm -f $@
	ar rc $@ $<
$(USRSYS)/lib/tn.a: $(KOBJ)/tn.o $(KOBJ)/tnas.o
	rm -f $@
	ar rc $@ $<

$(KOBJ)/alx.o:				\
		$(SYSINC)/clist.h	\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		alx.c
	$(CC) $(CFLAGS) -c -o $@ alx.c

$(KOBJ)/at.o: at.c			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/fdisk.h	\
		$(SYSINC)/hdioctl.h	\
		$(SYSINC)/buf.h		\
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/uproc.h	\
		$(USRINC)/errno.h
	$(CC) $(CFLAGS) -DVERBOSE=1 -c -o $@ at.c

$(KOBJ)/atas.o: atas.s
	$(AS) -go $@ $<

$(KOBJ)/ati.o: ati.m
	$(CC) $(CFLAGS) -DATI_132=1 -c -o $@ ati.m

$(KOBJ)/com1.o: 			\
		$(SYSINC)/al.h		\
		$(SYSINC)/clist.h	\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		al.c
	$(CC) $(CFLAGS) -DALCOM1=1 -c -o $@ al.c

$(KOBJ)/com2.o: 			\
		$(SYSINC)/al.h		\
		$(SYSINC)/clist.h	\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		al.c
	$(CC) $(CFLAGS) -DALCOM2=1 -c -o $@ al.c

$(KOBJ)/fdisk.o:			\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/fdisk.h	\
		$(SYSINC)/inode.h	\
		$(SYSINC)/uproc.h	\
		fdisk.c
	$(CC) $(CFLAGS) -c -o $@ fdisk.c

$(KOBJ)/fl.o:				\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/dmac.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/fdioctl.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/uproc.h	\
		fl.c
	$(CC) $(CFLAGS) -c -o $@ fl.c

$(KOBJ)/fontw.o: tools/fontgen.c
	$(CC) -o tools/fontgen tools/fontgen.c
	exec tools/fontgen > fontw.s
	exec /bin/rm tools/fontgen
	$(AS) -gxo $(KOBJ)/fontw.o fontw.s
	exec /bin/rm fontw.s

$(KOBJ)/gr.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/types.h	\
		$(SYSINC)/uproc.h	\
		gr.c
	$(CC) $(CFLAGS) -c -o $@ gr.c

$(KOBJ)/gras.o: gras.m
	$(CC) $(CFLAGS) -c -o $@ gras.m

$(KOBJ)/hgas.o: gras.s
	$(CC) $(CFLAGS) -c -o $@ -DHERCULES gras.m

$(KOBJ)/hs.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
		$(SYSINC)/stat.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		hs.c
	$(CC) $(CFLAGS) -c -o $@ hs.c

$(KOBJ)/kb.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/sched.h	\
		$(USRINC)/signal.h	\
		$(SYSINC)/stat.h	\
		$(SYSINC)/tty.h		$(SYSINC)/ktty.h \
		$(SYSINC)/uproc.h	\
		kb.c
	$(CC) $(CFLAGS) -c -o $@ kb.c

$(KOBJ)/lp.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/i8086.h	\
		$(SYSINC)/io.h		\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
		$(SYSINC)/stat.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/uproc.h	\
		lp.c
	$(CC) $(CFLAGS) -c -o $@ lp.c

$(KOBJ)/mm.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
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

$(KOBJ)/mmas.o: mmas.m
	$(CC) $(CFLAGS) -c -o $@ mmas.m

$(KOBJ)/ms.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/uproc.h	\
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/ms.h		\
		$(USRINC)/errno.h	\
		ms.c
	$(CC) $(CFLAGS) -c -o $@ ms.c

$(KOBJ)/rm.o: rm.c			\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/buf.h		\
		$(USRINC)/errno.h	\
		$(SYSINC)/uproc.h	\
		$(SYSINC)/seg.h		\
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/inode.h	\
		$(SYSINC)/stat.h
	$(CC) $(CFLAGS) -c -o $@ rm.c

$(KOBJ)/rs0.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(USRINC)/termio.h	\
		$(SYSINC)/uproc.h	\
		rs.c
	$(CC) $(CFLAGS) -DRS0 -c -o $@ rs.c

$(KOBJ)/rs1.o: 				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/ins8250.h	\
		$(SYSINC)/proc.h	$(SYSINC)/types.h $(SYSINC)/poll.h \
		$(SYSINC)/sched.h	\
		$(SYSINC)/stat.h	\
		$(USRINC)/termio.h	\
		$(SYSINC)/uproc.h	\
		rs.c
	$(CC) $(CFLAGS) -DRS1 -c -o $@ rs.c

$(KOBJ)/rsas.o: rsas.s
	$(AS) -gxo $@ rsas.s

$(KOBJ)/st.o:				\
		$(SYSINC)/buf.h		\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(SYSINC)/const.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/inode.h	\
		$(SYSINC)/mtioctl.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/seg.h		\
		$(SYSINC)/stat.h	\
		$(SYSINC)/uproc.h	\
		st.c
	$(CC) $(CFLAGS) -c -o $@ st.c

$(KOBJ)/tn.o:				\
		$(SYSINC)/coherent.h	$(SYSINC)/types.h $(SYSINC)/timeout.h \
					$(SYSINC)/machine.h $(SYSINC)/param.h \
					$(SYSINC)/fun.h \
		$(SYSINC)/con.h		\
		$(SYSINC)/devices.h	\
		$(USRINC)/errno.h	\
		$(SYSINC)/sched.h	\
		$(SYSINC)/timeout.h	\
		$(SYSINC)/types.h	\
		$(SYSINC)/uproc.h	\
		tn.c
	$(CC) $(CFLAGS) -c -o $@ tn.c

$(KOBJ)/tnas.o: tnas.s
	$(AS) -gxo $@ tnas.s

# How to make loadable drivers.

$(LDRV)/al0:	$(USRSYS)/lib/al.a
	( cd $(USRSYS); ldconfig al0 )

$(LDRV)/al1:	$(USRSYS)/lib/al.a
	( cd $(USRSYS); ldconfig al1 )

$(LDRV)/at:	$(USRSYS)/lib/at.a
	( cd $(USRSYS); ldconfig at )

$(LDRV)/fl:	$(USRSYS)/lib/fl.a
	( cd $(USRSYS); ldconfig fl )

$(LDRV)/gr:	$(USRSYS)/lib/gr.a
	( cd $(USRSYS); ldconfig gr )

$(LDRV)/hs:	$(USRSYS)/lib/hs.a
	( cd $(USRSYS); ldconfig hs )

$(LDRV)/lp:	$(USRSYS)/lib/lp.a
	( cd $(USRSYS); ldconfig lp )

$(LDRV)/mm:	$(USRSYS)/lib/mm.a
	( cd $(USRSYS); ldconfig mm )

$(LDRV)/ms:	$(USRSYS)/lib/ms.a
	( cd $(USRSYS); ldconfig ms )

$(LDRV)/rm:	$(USRSYS)/lib/rm.a
	( cd $(USRSYS); ldconfig rm )
