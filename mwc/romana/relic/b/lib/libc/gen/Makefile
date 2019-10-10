# /usr/src/libc/gen/Makefile 11/16/92

OBJ=\
	__assertfail.o\
	_addargs.o\
	_cleanup.o\
	_pow10.o\
	abs.o\
	atof.o\
	atoi.o\
	atol.o\
	bsearch.o\
	coffnlist.o\
	crypt.o\
	ctime.o\
	ctol.o\
	decvax_d.o\
	decvax_f.o\
	ftok.o\
	getcwd.o\
	getenv.o\
	getgrent.o\
	getlogin.o\
	getopt.o\
	getpass.o\
	getpw.o\
	getpwent.o\
	getwd.o\
	ieee_d.o\
	ieee_f.o\
	isatty.o\
	labs.o\
	lockf.o\
	ltoc.o\
	mktemp.o\
	monitor.o\
	mtype.o\
	nlist.o\
	notify.o\
	path.o\
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
	ttyname.o\
	ttyslot.o

# Primary target.
all:	GEN genctype gen$(MCH) genmalloc
	: libc/gen done.

# Pseduotargets.
GEN:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
genctype:
	SRCPATH=$(SRCPATH)/ctype make -e
gen$(MCH):
	SRCPATH=$(SRCPATH)/$(MCH) make -e
genmalloc:
	SRCPATH=$(SRCPATH)/malloc make -e

# Secondary targets.
__assertfail.o:	__assertfail.c
_addargs.o:	_addargs.c
_cleanup.o:	_cleanup.c
_pow10.o:	_pow10.c
abs.o:		abs.c
atof.o:		atof.c
atoi.o:		atoi.c
atol.o:		atol.c
bsearch.o:	bsearch.c
coffnlist.o:	coffnlist.c
crypt.o:	crypt.c
ctime.o:	ctime.c
ctol.o:		ctol.c
decvax_d.o:	decvax_d.c
decvax_f.o:	decvax_f.c
ftok.o:		ftok.c
getcwd.o:	getcwd.c
getenv.o:	getenv.c
getgrent.o:	getgrent.c
getlogin.o:	getlogin.c
getopt.o:	getopt.c
getpass.o:	getpass.c
getpw.o:	getpw.c
getpwent.o:	getpwent.c
getwd.o:	getwd.c
ieee_d.o:	ieee_d.c
ieee_f.o:	ieee_f.c
isatty.o:	isatty.c
labs.o:		labs.c
lockf.o:	lockf.c
ltoc.o:		ltoc.c
mktemp.o:	mktemp.c
monitor.o:	monitor.c
mtype.o:	mtype.c
nlist.o:	nlist.c
notify.o:	notify.c
path.o:		path.c
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
# N.B. ttyname compilation requires -DNEWTTYS=1, now #defined in the source.
ttyname.o:	ttyname.c
ttyslot.o:	ttyslot.c

# end of /usr/src/libc/gen/Makefile
