# Loadable Drivers - Makefile
#

# Include directories
USRINC=/usr/include
SYSINC=/usr/include/sys

CC=exec /bin/cc
CFLAGS=

TARGETS=$(USRSYS)/lib/ldrts0.o	\
	$(USRSYS)/lib/ldmain.o	\
	$(USRSYS)/lib/ldswap.o	\
	$(USRSYS)/lib/ldlib.a

ld_support:	$(TARGETS)
	@sync

$(USRSYS)/lib/ldrts0.o:	ldrts0.s
	as -xo $@ $<

$(USRSYS)/lib/ldmain.o:	ldmain.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(USRSYS)/lib/ldswap.o:	ldswap.c
	$(CC) $(CFLAGS) -DNOMONITOR -c -o $@ $<

# The following script extracts a module from a library.
X_LIB=	OBJFIL=`basename $@ | sed 's/L//'` ;\
	ar x $< $$OBJFIL ;\
	mv $$OBJFIL $@

# The following modules are supported in loadable drivers.
# NOTE: Most of them are interface stubs to kernel code.
#	Some of them are entirely driver resident.
LIBOBJ1=$(LOBJ)/absL.o		\
	$(LOBJ)/allocL.o	\
	$(LOBJ)/bclaimL.o	\
	$(LOBJ)/bdoneL.o	\
	$(LOBJ)/blkmvL.o	\
	$(LOBJ)/bootL.o		\
	$(LOBJ)/breadL.o	\
	$(LOBJ)/breleaseL.o	\
	$(LOBJ)/clockedfL.o	\
	$(LOBJ)/clrqL.o		\
	$(LOBJ)/cs_selfL.o	\
	$(LOBJ)/dblockL.o	\
	$(LOBJ)/dcloseL.o	\
	$(LOBJ)/deferL.o	\
	$(LOBJ)/devmsgL.o	\
	$(LOBJ)/dioctlL.o	\
	$(LOBJ)/dmagoL.o	\
	$(LOBJ)/dmalockL.o	\
	$(LOBJ)/dmaoffL.o	\
	$(LOBJ)/dmaonL.o	\
	$(LOBJ)/dmareqL.o	\
	$(LOBJ)/dopenL.o	\
	$(LOBJ)/dpollL.o	\
	$(LOBJ)/dpowerL.o	\
	$(LOBJ)/dreadL.o	\
	$(LOBJ)/drvmapL.o	\
	$(LOBJ)/dtimeL.o	\
	$(LOBJ)/dwriteL.o	\
	$(LOBJ)/fclearL.o	\
	$(LOBJ)/fdiskL.o	\
	$(LOBJ)/ffbyteL.o	\
	$(LOBJ)/ffwordL.o	\
	$(LOBJ)/ffmemL.o	\
	$(LOBJ)/fkcopyL.o	\
	$(LOBJ)/fpxcopyL.o	\
	$(LOBJ)/freeL.o		\
	$(LOBJ)/fucopyL.o	\
	$(LOBJ)/getcsL.o	\
	$(LOBJ)/getqL.o		\
	$(LOBJ)/getubdL.o	\
	$(LOBJ)/getuwdL.o	\
	$(LOBJ)/inbL.o		\
	$(LOBJ)/int11L.o	\
	$(LOBJ)/iogetcL.o	\
	$(LOBJ)/iomapvpL.o	\
	$(LOBJ)/ioputcL.o	\
	$(LOBJ)/ioreadL.o	\
	$(LOBJ)/ioreqL.o	\
	$(LOBJ)/iowriteL.o	\
	$(LOBJ)/ipcaccessL.o	\
	$(LOBJ)/kcallL.o	\
	$(LOBJ)/kfcopyL.o	\
	$(LOBJ)/kclearL.o	\
	$(LOBJ)/kpcopyL.o	\
	$(LOBJ)/kucopyL.o	\
	$(LOBJ)/lockL.o		\
	$(LOBJ)/lxdivL.o	\
	$(LOBJ)/lxmulL.o	\
	$(LOBJ)/lxremL.o	\
	$(LOBJ)/lxsgnL.o	\
	$(LOBJ)/memsetL.o	\
	$(LOBJ)/memtestL.o	\
	$(LOBJ)/nmidisableL.o	\
	$(LOBJ)/nmienableL.o	\
	$(LOBJ)/nondsigL.o	\
	$(LOBJ)/nonedevL.o	\
	$(LOBJ)/nulldevL.o	\

LIBOBJ2=$(LOBJ)/outbL.o		\
	$(LOBJ)/panicL.o	\
	$(LOBJ)/pclearL.o	\
	$(LOBJ)/pkcopyL.o	\
	$(LOBJ)/plrcopyL.o	\
	$(LOBJ)/pollopenL.o	\
	$(LOBJ)/pollwakeL.o	\
	$(LOBJ)/printfL.o	\
	$(LOBJ)/prlcopyL.o	\
	$(LOBJ)/ptovL.o		\
	$(LOBJ)/pucopyL.o	\
	$(LOBJ)/putcharL.o	\
	$(LOBJ)/putqL.o		\
	$(LOBJ)/putubdL.o	\
	$(LOBJ)/putuwdL.o	\
	$(LOBJ)/rucopyL.o	\
	$(LOBJ)/s5_to_sgL.o	\
	$(LOBJ)/s5_to_tcL.o	\
	$(LOBJ)/sallocL.o	\
	$(LOBJ)/sclearL.o	\
	$(LOBJ)/sendsigL.o	\
	$(LOBJ)/setivecL.o	\
	$(LOBJ)/sfreeL.o	\
	$(LOBJ)/sfbyteL.o	\
	$(LOBJ)/sfwordL.o	\
	$(LOBJ)/sfmemL.o	\
	$(LOBJ)/sg_to_s5L.o	\
	$(LOBJ)/sleepL.o	\
	$(LOBJ)/slrcopyL.o	\
	$(LOBJ)/sphiL.o		\
	$(LOBJ)/splL.o		\
	$(LOBJ)/sploL.o		\
	$(LOBJ)/superL.o	\
	$(LOBJ)/swapioL.o	\
	$(LOBJ)/tc_to_s5L.o	\
	$(LOBJ)/timeoutL.o	\
	$(LOBJ)/ttcloseL.o	\
	$(LOBJ)/tthupL.o	\
	$(LOBJ)/ttflushL.o	\
	$(LOBJ)/ttinL.o		\
	$(LOBJ)/ttioctlL.o	\
	$(LOBJ)/ttopenL.o	\
	$(LOBJ)/ttoutL.o	\
	$(LOBJ)/ttpollL.o	\
	$(LOBJ)/ttreadL.o	\
	$(LOBJ)/ttsetgrpL.o	\
	$(LOBJ)/ttsignalL.o	\
	$(LOBJ)/ttstartL.o	\
	$(LOBJ)/ttwriteL.o	\
	$(LOBJ)/uexitL.o	\
	$(LOBJ)/ufcopyL.o	\
	$(LOBJ)/ukcopyL.o	\
	$(LOBJ)/unlockL.o	\
	$(LOBJ)/upcopyL.o	\
	$(LOBJ)/urcopyL.o	\
	$(LOBJ)/vprintL.o	\
	$(LOBJ)/vrelseL.o	\
	$(LOBJ)/vremapL.o	\
	$(LOBJ)/vtopL.o		\
	$(LOBJ)/vxdivL.o	\
	$(LOBJ)/vxmulL.o	\
	$(LOBJ)/vxremL.o	\
	$(LOBJ)/waitqL.o	\
	$(LOBJ)/wakeupL.o	\

LIBOBJ3=$(LOBJ)/fifo_closeL.o		\
	$(LOBJ)/fifo_lenL.o		\
	$(LOBJ)/fifo_openL.o		\
	$(LOBJ)/fifo_readL.o		\
	$(LOBJ)/fifo_rewindL.o		\
	$(LOBJ)/fifo_writeL.o		\
	$(LOBJ)/uart_senseL.o		\

$(USRSYS)/lib/ldlib.a:	mkstub.m4 $(LIBOBJ1) $(LIBOBJ2) $(LIBOBJ3)
	rm -f  $@
	ar rc  $@ $(LIBOBJ1)
	ar rc  $@ $(LIBOBJ2)
	ar rc  $@ $(LIBOBJ3)
	ranlib $@

# Here is mkstub.m4:
#define(sym, substr(basename, -2, ))dnl
#	.globl	sym`_'
#sym`_':	mov	ax,`$K'sym`_'
#	.byte	0x9A
#	.word	xcalled
#	.word	0x0060
#	ret

MKSTUB=	echo "define(basename, `basename $*`)dnl" | m4 - mkstub.m4 > $*.s ;\
	as -gxo $@ $*.s; rm $*.s

$(LOBJ)/absL.o:
	$(MKSTUB)

$(LOBJ)/allocL.o:
	$(MKSTUB)

$(LOBJ)/bclaimL.o:
	$(MKSTUB)

$(LOBJ)/bdoneL.o:
	$(MKSTUB)

$(LOBJ)/blkmvL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/bootL.o:
	$(MKSTUB)

$(LOBJ)/breadL.o:
	$(MKSTUB)

$(LOBJ)/breleaseL.o:
	$(MKSTUB)

$(LOBJ)/clockedfL.o:	clockedf.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/clrivecL.o:
	$(MKSTUB)

$(LOBJ)/clrqL.o:
	$(MKSTUB)

$(LOBJ)/cs_selfL.o:	cs_self.s
	as -gxo $@ $<

$(LOBJ)/dblockL.o:
	$(MKSTUB)

$(LOBJ)/dcloseL.o:
	$(MKSTUB)

$(LOBJ)/deferL.o:	defer.s
	as -gxo $@ $<

$(LOBJ)/devmsgL.o:
	$(MKSTUB)

$(LOBJ)/dioctlL.o:
	$(MKSTUB)

$(LOBJ)/dmagoL.o:
	$(MKSTUB)

$(LOBJ)/dmalockL.o:	dmalock.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/dmaoffL.o:
	$(MKSTUB)

$(LOBJ)/dmaonL.o:
	$(MKSTUB)

$(LOBJ)/dmareqL.o:
	$(MKSTUB)

$(LOBJ)/dopenL.o:
	$(MKSTUB)

$(LOBJ)/dpollL.o:
	$(MKSTUB)

$(LOBJ)/dpowerL.o:
	$(MKSTUB)

$(LOBJ)/dreadL.o:
	$(MKSTUB)

$(LOBJ)/drvmapL.o:
	$(MKSTUB)

$(LOBJ)/dtimeL.o:
	$(MKSTUB)

$(LOBJ)/dwriteL.o:
	$(MKSTUB)

$(LOBJ)/fclearL.o:
	$(MKSTUB)

$(LOBJ)/fdiskL.o:
	$(MKSTUB)

$(LOBJ)/ffbyteL.o:	ffbyte.s
	as -gxo $@ $<

$(LOBJ)/ffwordL.o:	ffword.s
	as -gxo $@ $<

$(LOBJ)/ffmemL.o:	ffmem.s
	as -gxo $@ $<

$(LOBJ)/fkcopyL.o:
	$(MKSTUB)

$(LOBJ)/fpxcopyL.o:
	$(MKSTUB)

$(LOBJ)/freeL.o:
	$(MKSTUB)

$(LOBJ)/fucopyL.o:
	$(MKSTUB)

$(LOBJ)/getcsL.o:	getcs.s
	as -gxo $@ $<

$(LOBJ)/getqL.o:
	$(MKSTUB)

$(LOBJ)/getubdL.o:
	$(MKSTUB)

$(LOBJ)/getuwdL.o:
	$(MKSTUB)

$(LOBJ)/inbL.o:		inb.s
	as -gxo $@ $<

$(LOBJ)/int11L.o:
	$(MKSTUB)

$(LOBJ)/iogetcL.o:
	$(MKSTUB)

$(LOBJ)/iomapvpL.o:
	$(MKSTUB)

$(LOBJ)/ioputcL.o:
	$(MKSTUB)

$(LOBJ)/ioreadL.o:
	$(MKSTUB)

$(LOBJ)/ioreqL.o:
	$(MKSTUB)

$(LOBJ)/iowriteL.o:
	$(MKSTUB)

$(LOBJ)/ipcaccessL.o:
	$(MKSTUB)

$(LOBJ)/kcallL.o:	kcall.s
	as -gxo $@ $<

$(LOBJ)/kclearL.o:
	$(MKSTUB)

$(LOBJ)/kfcopyL.o:
	$(MKSTUB)

$(LOBJ)/kpcopyL.o:
	$(MKSTUB)

$(LOBJ)/kucopyL.o:
	$(MKSTUB)

$(LOBJ)/lockL.o:
	$(MKSTUB)

$(LOBJ)/lxdivL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/lxmulL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/lxremL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/lxsgnL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/memsetL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/memtestL.o:
	$(MKSTUB)

$(LOBJ)/nmidisableL.o:
	$(MKSTUB)

$(LOBJ)/nmienableL.o:
	$(MKSTUB)

$(LOBJ)/nondsigL.o:
	$(MKSTUB)

$(LOBJ)/nonedevL.o:	nonedev.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/nulldevL.o:	nulldev.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/outbL.o:		outb.s
	as -gxo $@ $<

$(LOBJ)/panicL.o:
	$(MKSTUB)

$(LOBJ)/pclearL.o:
	$(MKSTUB)

$(LOBJ)/plrcopyL.o:
	$(MKSTUB)

$(LOBJ)/pkcopyL.o:
	$(MKSTUB)

$(LOBJ)/pollopenL.o:
	$(MKSTUB)

$(LOBJ)/pollwakeL.o:
	$(MKSTUB)

$(LOBJ)/printfL.o:
	$(MKSTUB)

$(LOBJ)/prlcopyL.o:
	$(MKSTUB)

$(LOBJ)/ptovL.o:
	$(MKSTUB)

$(LOBJ)/pucopyL.o:
	$(MKSTUB)

$(LOBJ)/putcharL.o:
	$(MKSTUB)

$(LOBJ)/putqL.o:
	$(MKSTUB)

$(LOBJ)/putubdL.o:
	$(MKSTUB)

$(LOBJ)/putuwdL.o:
	$(MKSTUB)

$(LOBJ)/rucopyL.o:
	$(MKSTUB)

$(LOBJ)/s5_to_sgL.o:
	$(MKSTUB)

$(LOBJ)/s5_to_tcL.o:
	$(MKSTUB)

$(LOBJ)/sallocL.o:
	$(MKSTUB)

$(LOBJ)/sclearL.o:
	$(MKSTUB)

$(LOBJ)/sendsigL.o:
	$(MKSTUB)

$(LOBJ)/setivecL.o:	setivec.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/sfreeL.o:
	$(MKSTUB)

$(LOBJ)/sfbyteL.o:	sfbyte.s
	as -gxo $@ $<

$(LOBJ)/sfwordL.o:	sfword.s
	as -gxo $@ $<

$(LOBJ)/sfmemL.o:	sfmem.s
	as -gxo $@ $<

$(LOBJ)/sg_to_s5L.o:
	$(MKSTUB)

$(LOBJ)/sleepL.o:
	$(MKSTUB)

$(LOBJ)/slrcopyL.o:
	$(MKSTUB)

$(LOBJ)/sphiL.o:	sphi.s
	as -gxo $@ $<

$(LOBJ)/splL.o:	spl.s
	as -gxo $@ $<

$(LOBJ)/sploL.o:	splo.s
	as -gxo $@ $<

$(LOBJ)/superL.o:
	$(MKSTUB)

$(LOBJ)/swapioL.o:
	$(MKSTUB)

$(LOBJ)/tc_to_s5L.o:
	$(MKSTUB)

$(LOBJ)/timeoutL.o:	timeout.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LOBJ)/ttcloseL.o:
	$(MKSTUB)

$(LOBJ)/ttflushL.o:
	$(MKSTUB)

$(LOBJ)/tthupL.o:
	$(MKSTUB)

$(LOBJ)/ttinL.o:
	$(MKSTUB)

$(LOBJ)/ttioctlL.o:
	$(MKSTUB)

$(LOBJ)/ttopenL.o:
	$(MKSTUB)

$(LOBJ)/ttoutL.o:
	$(MKSTUB)

$(LOBJ)/ttpollL.o:
	$(MKSTUB)

$(LOBJ)/ttreadL.o:
	$(MKSTUB)

$(LOBJ)/ttsetgrpL.o:
	$(MKSTUB)

$(LOBJ)/ttsignalL.o:
	$(MKSTUB)

$(LOBJ)/ttstartL.o:
	$(MKSTUB)

$(LOBJ)/ttwriteL.o:
	$(MKSTUB)

$(LOBJ)/uexitL.o:
	$(MKSTUB)

$(LOBJ)/ufcopyL.o:
	$(MKSTUB)

$(LOBJ)/ukcopyL.o:
	$(MKSTUB)

$(LOBJ)/unlockL.o:
	$(MKSTUB)

$(LOBJ)/upcopyL.o:
	$(MKSTUB)

$(LOBJ)/urcopyL.o:
	$(MKSTUB)

$(LOBJ)/vprintL.o:
	$(MKSTUB)

$(LOBJ)/vrelseL.o:
	$(MKSTUB)

$(LOBJ)/vremapL.o:
	$(MKSTUB)

$(LOBJ)/vtopL.o:
	$(MKSTUB)

$(LOBJ)/vxdivL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/vxmulL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/vxremL.o:	/lib/libc.a
	$(X_LIB)

$(LOBJ)/waitqL.o:
	$(MKSTUB)

$(LOBJ)/wakeupL.o:
	$(MKSTUB)

$(LOBJ)/fifo_closeL.o:
	$(MKSTUB)

$(LOBJ)/fifo_lenL.o:
	$(MKSTUB)

$(LOBJ)/fifo_openL.o:
	$(MKSTUB)

$(LOBJ)/fifo_readL.o:
	$(MKSTUB)

$(LOBJ)/fifo_rewindL.o:
	$(MKSTUB)

$(LOBJ)/fifo_writeL.o:
	$(MKSTUB)

$(LOBJ)/uart_senseL.o:
	$(MKSTUB)
