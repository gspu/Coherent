# /usr/src/libc/sys/Makefile 9/28/92

OBJ=\
	execl.o\
	execle.o\
	execlp.o\
	execlpe.o\
	execv.o\
	execve.o\
	execvp.o\
	execvpe.o

UNUSED=\
	brk.o

# Primary target.
all:	SYS sys$(MCH)
	: libc/sys done.

# Pseudotargets.
SYS:	$(OBJ)
	$(AR) rkus libc.a $?
	touch $@
sys$(MCH):
	SRCPATH=$(SRCPATH)/$(MCH) make -e

# Secondary targets.
execl.o:	execl.c
execle.o:	execle.c
execlp.o:	execlp.c
execlpe.o:	execlpe.c
execv.o:	execv.c
execve.o:	execve.c
execvp.o:	execvp.c
execvpe.o:	execvpe.c

# end of /usr/src/libc/sys/Makefile
