# /usr/src/libc/string/Makefile 2/14/91

OBJ=\
	strcoll.o\
	strerror.o\
	strtok.o\
	strxfrm.o\

# These C sources are unused, the assembler sources in i8086
# are used instead for efficiency.
UNUSED=\
	_memxchg.c\
	memchr.c\
	memcmp.c\
	memcpy.c\
	memmove.c\
	memset.c\
	strcat.c\
	strchr.c\
	strcmp.c\
	strcpy.c\
	strcspn.c\
	streq.c\
	strlen.c\
	strncat.c\
	strncmp.c\
	strncpy.c\
	strpbrk.c\
	strrchr.c\
	strspn.c\
	strstr.c\

# Primary target.
all:	STRING stringi8086
	: libc/string done

# Pseudotargets.
STRING:	$(OBJ)
	ar rkus libc.a $?
	touch $@
stringi8086:
	make SRCPATH=$(SRCPATH)/i8086

# Secondary targets.
strcoll.o:	strcoll.c
strerror.o:	strerror.c
strtok.o:	strtok.c
strxfrm.o:	strxfrm.c

# end of /usr/src/libc/string/Makefile
