# /usr/src/libc/crt/i8086/Makefile 6/26/91

OBJ=\
	blkmv.o\
	count.o\
	ddxad.o\
	ddxdi.o\
	ddxmu.o\
	ddxsu.o\
	dfxad.o\
	dfxdi.o\
	dfxmu.o\
	dfxsu.o\
	dpush.o\
	dtest.o\
	dxadd.o\
	dxcvt.o\
	dxdiv.o\
	dxmul.o\
	dzero.o\
	fdata.o\
	frexp.o\
	ixcmp.o\
	ixcvt.o\
	ldexp.o\
	lxcvt.o\
	lxdiv.o\
	lxmul.o\
	lxrem.o\
	lxsgn.o\
	vdiv.o\
	vxdiv.o\
	vxmul.o\
	vxrem.o

# Primary target.
CRTI8086:	$(OBJ)
	ar rkus libc.a $?
	touch $@
	: libc/crt/i8086 done.

# Secondary targets.
blkmv.o:	blkmv.s
count.o:	count.s
ddxad.o:	ddxad.s
ddxdi.o:	ddxdi.s
ddxmu.o:	ddxmu.s
ddxsu.o:	ddxsu.s
dfxad.o:	dfxad.s
dfxdi.o:	dfxdi.s
dfxmu.o:	dfxmu.s
dfxsu.o:	dfxsu.s
dpush.o:	dpush.s
dtest.o:	dtest.s
dxadd.o:	dxadd.s
dxcvt.o:	dxcvt.s
dxdiv.o:	dxdiv.s
dxmul.o:	dxmul.s
dzero.o:	dzero.s
fdata.o:	fdata.s
frexp.o:	frexp.s
ixcmp.o:	ixcmp.s
ixcvt.o:	ixcvt.s
ldexp.o:	ldexp.s
lxcvt.o:	lxcvt.s
lxdiv.o:	lxdiv.s
lxmul.o:	lxmul.s
lxrem.o:	lxrem.s
lxsgn.o:	lxsgn.s
vdiv.o:		vdiv.m
vxdiv.o:	vxdiv.s
vxmul.o:	vxmul.s
vxrem.o:	vxrem.s

# end of /usr/src/libc/crt/i8086/Makefile
