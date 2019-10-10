# /usr/src/libc/crt/i386/Makefile 1/13/93

CRTI386=\
	_fxam.o \
	_modf.o \
	daddsub.o \
	dcmp.o \
	ddiv.o \
	dfcvt.o \
	dicvt.o \
	dmul.o \
	ducvt.o \
	fdcvt.o \
	fops.o \
	frexp.o \
	idcvt.o \
	ifcvt.o \
	ldexp.o \
	modf.o \
	scount.o \
	udcvt.o \
	ufcvt.o

CRTI386NDP=\
	_fxam.o \
	scount.o

# Primary target.
CRTI386:	$(CRTI386)
	$(AR) rkus libc.a $?
	touch $@
	: libc/crt/i386 done

# Primary NDP target.
CRTI386NDP:	$(CRTI386NDP)
	$(AR) rkus libc.a $?
	touch $@
	: NDP libc/crt/i386 done
	
# Secondary targets.
_fxam.o:	_fxam.s
_modf.o:	_modf.s
daddsub.o:	daddsub.s
dcmp.o:		dcmp.s
ddiv.o:		ddiv.s
dfcvt.o:	dfcvt.s
dicvt.o:	dicvt.s
dmul.o:		dmul.s
ducvt.o:	ducvt.s
fdcvt.o:	fdcvt.s
fops.o:		fops.s
frexp.o:	frexp.s
idcvt.o:	idcvt.s
ifcvt.o:	ifcvt.s
ldexp.o:	ldexp.s
modf.o:		modf.c
scount.o:	scount.s
udcvt.o:	udcvt.s
ufcvt.o:	ufcvt.s

# end of /usr/src/libc/crt/i386/Makefile
