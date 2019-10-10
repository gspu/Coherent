# libm/i387/Makefile 2/4/93

ASFLAGS=-fx
DESTDIR=/bin386/lib/ndp/
OBJ=\
	acos87.o\
	asin87.o\
	atan287.o\
	atan87.o\
	ceil87.o\
	cos87.o\
	cosh87.o\
	error87.o\
	exp87.o\
	fabs87.o\
	fmod.o\
	hypot87.o\
	j0.o\
	j1.o\
	jn.o\
	log87.o\
	pol.o\
	pow87.o\
	sin87.o\
	sqrt87.o\
	tan87.o\
	tanh87.o

libm87.a:	$(OBJ)
	ar rucks $(DESTDIR)libm.a $(OBJ)
#	ar rucks libm87.a $(OBJ)

# Secondary targets.
# Assembler sources.
acos87.o:	acos87.s
asin87.o:	asin87.s
atan287.o:	atan287.s
atan87.o:	atan87.s
ceil87.o:	ceil87.s
cos87.o:	cos87.s
cosh87.o:	cosh87.s
error87.o:	error87.s
exp87.o:	exp87.s
fabs87.o:	fabs87.s
hypot87.o:	hypot87.s
log87.o:	log87.s
pow87.o:	pow87.s
sin87.o:	sin87.s
sqrt87.o:	sqrt87.s
tan87.o:	tan87.s
tanh87.o:	tanh87.s
# C sources.
fmod.o:		fmod.c
j0.o:		j0.c
j1.o:		j1.c
jn.o:		jn.c
pol.o:		pol.c

# end of /usr/src/libm/i387/Makefile
