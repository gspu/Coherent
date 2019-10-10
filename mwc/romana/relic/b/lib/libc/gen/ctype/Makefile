# /usr/src/libc/gen/ctype/Makefile 1/9/92

OBJ=\
	_ctype.o\
	_tolower.o\
	_toupper.o\
	isalnum.o\
	isalpha.o\
	isascii.o\
	iscntrl.o\
	isdigit.o\
	isgraph.o\
	islower.o\
	isprint.o\
	ispunct.o\
	isspace.o\
	isupper.o\
	isxdigit.o\
	toascii.o\
	tolower.o\
	toupper.o

# Primary target.
GENCTYPE:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
	: lib/gen/ctype done

# Secondary targets.
_ctype.o:	_ctype.c
_tolower.o:	_tolower.c
_toupper.o:	_toupper.c
isalnum.o:	isalnum.c
isalpha.o:	isalpha.c
isascii.o:	isascii.c
iscntrl.o:	iscntrl.c
isdigit.o:	isdigit.c
isgraph.o:	isgraph.c
islower.o:	islower.c
isprint.o:	isprint.c
ispunct.o:	ispunct.c
isspace.o:	isspace.c
isupper.o:	isupper.c
isxdigit.o:	isxdigit.c
toascii.o:	toascii.c
tolower.o:	tolower.c
toupper.o:	toupper.c

# end of /usr/src/libc/gen/ctype/Makefile
