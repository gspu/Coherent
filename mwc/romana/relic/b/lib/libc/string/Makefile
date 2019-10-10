# /usr/src/libc/string/Makefile 7/6/93

OBJ=\
	strcoll.o\
	strdup.o\
	strerror.o\
	strtok.o\
	strxfrm.o\

# These C sources are unused, the assembler sources in $(MCH)
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
all:	STRING string$(MCH)
	: libc/string done

# Pseudotargets.
STRING:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
string$(MCH):
	SRCPATH=$(SRCPATH)/$(MCH) make -e

# Secondary targets.
strcoll.o:	strcoll.c
strdup.o:	strdup.c
strerror.o:	strerror.c
strtok.o:	strtok.c
strxfrm.o:	strxfrm.c

# end of /usr/src/libc/string/Makefile
