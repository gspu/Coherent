# /usr/src/libc/sys/Makefile 2/14/91

OBJ=\
	dup2.o\
	execl.o\
	execle.o\
	execlp.o\
	execv.o\
	execvp.o\
	msgop.o\
	semop.o\
	shmop.o\
	time.o

UNUSED=\
	brk.o

# Primary target.
all:	SYS sysi8086
	: libc/sys done.

# Pseudotargets.
SYS:	$(OBJ)
	ar rkus libc.a $?
	touch $@
sysi8086:
	make SRCPATH=$(SRCPATH)/i8086

# Secondary targets.
dup2.o:		dup2.c
execl.o:	execl.c
execle.o:	execle.c
execlp.o:	execlp.c
execv.o:	execv.c
execvp.o:	execvp.c
msgop.o:	msgop.c
semop.o:	semop.c
shmop.o:	shmop.c
time.o:		time.c

# end of /usr/src/libc/sys/Makefile
