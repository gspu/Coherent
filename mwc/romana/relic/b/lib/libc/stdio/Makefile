# /usr/src/libc/stdio/Makefile 1/26/93

# The order of stdio objects in libc.a is significant and a bit complicated.
# To reduce the size of linked objects when possible,
# libc includes both real and fake versions of _dtefg() (floating point output)
# and of _finish() (close open FILEs).
# If a program requires floating point output, the "-f" flag to /bin/cc
# produces an "-u _dtefg_" argument to /bin/ld, forcing the link to include
# the real floating point output routine from _dtefg.o.
# Otherwise, /bin/ld resolves the reference to _dtefg() in vfprintf.o with
# the much smaller fake floating point output routine from _fdtefg.o.
# To avoid linking in extra stdio overhead when not required,
# libc.a should contain _fp.o, _finit.o, exit.o and _finish.o, in that order,
# after stdio objects from $(OBJ) and from the machine-dependent subdirectory,
# If a program uses stdio, /bin/ld will link in _finit.o;
# finit.o defines the real _finish() which closes all open files for exit().
# If a program does not use stdio, /bin/ld will link in exit.o but not _finit.o;
# then it resolves the _finish() reference in exit() with the fake _finish.o,
# which does nothing.

OBJ=\
	_dscan.o\
	_fgetb.o\
	_fgetc.o\
	_fgete.o\
	_fgetstr.o\
	_filbuf.o\
	_flsbuf.o\
	_fopen.o\
	_fpseek.o\
	_fputa.o\
	_fputb.o\
	_fputc.o\
	_fpute.o\
	_fputt.o\
	_scanf.o\
	_stropen.o\
	clearerr.o\
	ecvt.o\
	fclose.o\
	fcvt.o\
	fdopen.o\
	feof.o\
	ferror.o\
	fflush.o\
	fgetc.o\
	fgetpos.o\
	fgets.o\
	fopen.o\
	fputc.o\
	fputs.o\
	fread.o\
	freopen.o\
	fscanf.o\
	fseek.o\
	fsetpos.o\
	ftell.o\
	fwrite.o\
	gcvt.o\
	getc.o\
	getchar.o\
	gets.o\
	perror.o\
	popen.o\
	putc.o\
	putchar.o\
	puts.o\
	remove.o\
	rename.o\
	rewind.o\
	scanf.o\
	setbuf.o\
	setvbuf.o\
	sscanf.o\
	tmpfile.o\
	tmpnam.o\
	ungetc.o

# N.B. The order of objects in $(OBJ2) is significant,
# see the lengthy comment above and be extremely careful if you change it.
OBJ2=\
	_dtefg.o\
	fprintf.o\
	printf.o\
	sprintf.o\
	vprintf.o\
	vsprintf.o\
	vfprintf.o\
	_fdtefg.o\
	_fp.o\
	_finit.o\
	exit.o\
	_finish.o

# Primary target.
all:	stdio$(MCH) STDIO
	: libc/stdio done.

# Pseudotargets.
stdio$(MCH):
	SRCPATH=$(SRCPATH)/$(MCH) make -e
# This assumes that either libc.a is being made from scratch
# or the names of the objects in $(OBJ2) are unchanged.
# Otherwise, "$(AR) rkus lib.c $?" may not preserve the required object order.
# It could be done more correctly with $(AR) positional specifications.
STDIO:	$(OBJ) $(OBJ2)
	$(AR) rkus libc.a $?
	touch $@

# Secondary targets.
_dscan.o:	_dscan.c
_dtefg.o:	_dtefg.c
_fdtefg.o:	_fdtefg.c
_fgetb.o:	_fgetb.c
_fgetc.o:	_fgetc.c
_fgete.o:	_fgete.c
_fgetstr.o:	_fgetstr.c
_filbuf.o:	_filbuf.c
_finish.o:	_finish.c
_finit.o:	_finit.c
_flsbuf.o:	_flsbuf.c
_fopen.o:	_fopen.c
_fp.o:		_fp.c
_fpseek.o:	_fpseek.c
_fputa.o:	_fputa.c
_fputb.o:	_fputb.c
_fputc.o:	_fputc.c
_fpute.o:	_fpute.c
_fputt.o:	_fputt.c
_scanf.o:	_scanf.c
_stropen.o:	_stropen.c
clearerr.o:	clearerr.c
ecvt.o:		ecvt.c
exit.o:		exit.c
fclose.o:	fclose.c
fcvt.o:		fcvt.c
fdopen.o:	fdopen.c
feof.o:		feof.c
ferror.o:	ferror.c
fflush.o:	fflush.c
fgetc.o:	fgetc.c
fgetpos.o:	fgetpos.c
fgets.o:	fgets.c
fopen.o:	fopen.c
fprintf.o:	fprintf.c
fputc.o:	fputc.c
fputs.o:	fputs.c
fread.o:	fread.c
freopen.o:	freopen.c
fscanf.o:	fscanf.c
fseek.o:	fseek.c
fsetpos.o:	fsetpos.c
ftell.o:	ftell.c
fwrite.o:	fwrite.c
gcvt.o:		gcvt.c
getc.o:		getc.c
getchar.o:	getchar.c
gets.o:		gets.c
perror.o:	perror.c
popen.o:	popen.c
printf.o:	printf.c
putc.o:		putc.c
putchar.o:	putchar.c
puts.o:		puts.c
remove.o:	remove.c
rename.o:	rename.c
rewind.o:	rewind.c
scanf.o:	scanf.c
setbuf.o:	setbuf.c
setvbuf.o:	setvbuf.c
sprintf.o:	sprintf.c
sscanf.o:	sscanf.c
tmpfile.o:	tmpfile.c
tmpnam.o:	tmpnam.c
ungetc.o:	ungetc.c
vprintf.o:	vprintf.c
vfprintf.o:	vfprintf.c
vsprintf.o:	vsprintf.c

# end of /usr/src/libc/stdio/Makefile
