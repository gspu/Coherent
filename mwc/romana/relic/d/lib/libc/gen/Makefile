# /usr/src/libc/gen/Makefile 2/20/91

OBJ=\
	abs.o\
	atof.o\
	atoi.o\
	atol.o\
	bsearch.o\
	crypt.o\
	ctime.o\
	ctol.o\
	getenv.o\
	getgrent.o\
	getlogin.o\
	getopt.o\
	getpass.o\
	getpw.o\
	getpwent.o\
	getwd.o\
	isatty.o\
	labs.o\
	ltoc.o\
	mktemp.o\
	monitor.o\
	mtype.o\
	nlist.o\
	notify.o\
	path.o\
	perror.o\
	pnmatch.o\
	putenv.o\
	qsort.o\
	rand.o\
	shellsort.o\
	sleep.o\
	strtod.o\
	strtol.o\
	swab.o\
	sys_err.o\
	system.o\
	tempnam.o\
	tmpnam.o\
	ttyname.o\
	ttyslot.o

# Primary target.
all:	GEN genctype geni8086 genmalloc
	: libc/gen done.

# Pseduotargets.
GEN:	$(OBJ)
	ar rkus libc.a $?
	touch $@
genctype:
	make SRCPATH=$(SRCPATH)/ctype
geni8086:
	make SRCPATH=$(SRCPATH)/i8086
genmalloc:
	make SRCPATH=$(SRCPATH)/malloc

# Secondary targets.
abs.o:		abs.c
atof.o:		atof.c
atoi.o:		atoi.c
atol.o:		atol.c
bsearch.o:	bsearch.c
crypt.o:	crypt.c
ctime.o:	ctime.c
ctol.o:		ctol.c
getenv.o:	getenv.c
getgrent.o:	getgrent.c
getlogin.o:	getlogin.c
getopt.o:	getopt.c
getpass.o:	getpass.c
getpw.o:	getpw.c
getpwent.o:	getpwent.c
getwd.o:	getwd.c
isatty.o:	isatty.c
labs.o:		labs.c
ltoc.o:		ltoc.c
mktemp.o:	mktemp.c
monitor.o:	monitor.c
mtype.o:	mtype.c
nlist.o:	nlist.c
notify.o:	notify.c
path.o:		path.c
perror.o:	perror.c
pnmatch.o:	pnmatch.c
putenv.o:	putenv.c
qsort.o:	qsort.c
rand.o:		rand.c
shellsort.o:	shellsort.c
sleep.o:	sleep.c
strtod.o:	strtod.c
strtol.o:	strtol.c
swab.o:		swab.c
sys_err.o:	sys_err.c
system.o:	system.c
tempnam.o:	tempnam.c
tmpnam.o:	tmpnam.c
# N.B. ttyname compilation requires -DNEWTTYS=1, now #defined in the source.
ttyname.o:	ttyname.c
ttyslot.o:	ttyslot.c

# end of /usr/src/libc/gen/Makefile
