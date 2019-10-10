# libmp/Makefile 4/5/90

DESTDIR=/interim/usr/lib/
DESTDIR386=/bin386/usr/lib/

OBJ=\
	gcd.o \
	ispos.o \
	itom.o \
	madd.o \
	mcmp.o \
	mconst.o \
	mcopy.o \
	mdiv.o \
	min.o \
	mintfr.o \
	mitom.o \
	mneg.o \
	mout.o \
	mpalloc.o \
	mperr.o \
	msma.o \
	msqrt.o \
	mtoi.o \
	mtos.o \
	mult.o \
	norm.o \
	pow.o \
	rpow.o \
	sdiv.o \
	smult.o \
	spow.o \
	xgcd.o \
	zerop.o \

libmp.a:	$(OBJ)
	ar ckrs $(DESTDIR)libmp.a $(OBJ)

libmp386.a:	$(OBJ)
	ar crs $(DESTDIR386)libmp.a $(OBJ)
