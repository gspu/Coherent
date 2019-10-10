# /usr/src/libm/Makefile 2/4/93
# For the moment, this just makes the main directory.
# It should have separate targets for different machines and representations,
# notably i8087.
DESTDIR=/bin386/lib/
OBJ=\
	acos.o\
	asin.o\
	atan.o\
	atan2.o\
	cabs.o\
	ceil.o\
	cos.o\
	cosh.o\
	exp.o\
	fabs.o\
	floor.o\
	fmod.o\
	hypot.o\
	j0.o\
	j1.o\
	jn.o\
	log.o\
	log10.o\
	pol.o\
	pow.o\
	sin.o\
	sinh.o\
	sqrt.o\
	tan.o\
	tanh.o\
	two.o

# Primary target.
libm.a:	$(OBJ)
	$(AR) rckus $(DESTDIR)$@ $?

# Secondary targets.
acos.o:		acos.c
asin.o:		asin.c
atan.o:		atan.c
atan2.o:	atan2.c
cabs.o:		cabs.c
ceil.o:		ceil.c
cos.o:		cos.c
cosh.o:		cosh.c
exp.o:		exp.c
fabs.o:		fabs.c
floor.o:	floor.c
fmod.o:		fmod.c
hypot.o:	hypot.c
j0.o:		j0.c
j1.o:		j1.c
jn.o:		jn.c
log.o:		log.c
log10.o:	log10.c
pol.o:		pol.c
pow.o:		pow.c
sin.o:		sin.c
sinh.o:		sinh.c
sqrt.o:		sqrt.c
tan.o:		tan.c
tanh.o:		tanh.c
two.o:		two.c

clean:
	rm $(OBJ)

# end of /usr/src/libm/Makefile
