# /usr/src/libc/sys/i386/Makefile 6/22/93

ASFLAGS=-x

# Objects built from sources in sys/i386.
OBJ=\
	_execve.o\
	brk.o\
	cerror.o\
	dup2.o\
	fork.o\
	gtty.o\
	msgemul.o\
	pipe.o\
	sememul.o\
	setpgrp.o\
	shmemul.o\
	signal.o\
	time.o\
	uname.o\
	ustat.o\
	wait.o

# These objects are built from sources generated automatically by mkscall.
# N.B. getdents.o is not in this list for now, libc.a uses ndir version instead.
# _exit.o used to be included but now is part of crts0.o instead.
AUTOOBJ=\
	_msgsys.o\
	_pgrp.o\
	_semsys.o\
	_shmsys.o\
	_time.o\
	_utssys.o\
	access.o\
	acct.o\
	alarm.o\
	chdir.o\
	chmod.o\
	chown.o\
	chroot.o\
	chsize.o\
	close.o\
	creat.o\
	dup.o\
	fcntl.o\
	fstat.o\
	fstatfs.o\
	ftime.o\
	getegid.o\
	geteuid.o\
	getgid.o\
	getpid.o\
	getppid.o\
	getuid.o\
	ioctl.o\
	kill.o\
	link.o\
	lseek.o\
	mkdir.o\
	mknod.o\
	mount.o\
	nap.o\
	nice.o\
	open.o\
	pause.o\
	plock.o\
	poll.o\
	profil.o\
	ptrace.o\
	read.o\
	rmdir.o\
	setgid.o\
	setuid.o\
	stat.o\
	statfs.o\
	stime.o\
	sync.o\
	sysi86.o\
	times.o\
	uadmin.o\
	ulimit.o\
	umask.o\
	umount.o\
	unlink.o\
	utime.o\
	write.o

# Primary target.
SYSI386:	mkscall SYSI386AUTO $(OBJ)
	$(AR) rkus libc.a $(AUTOOBJ) $(OBJ)
	touch $@
	: libc/sys/i386 done

# Build system call sources automatically from scall.s5.
# N.B. ASFLAGS should not include -f for these calls,
# so this invokes make rather than make -e in the subdirectory.
SYSI386AUTO:	scall.s5
	-mkdir tmp
	cd tmp; ../mkscall -f $<; make $(AUTOOBJ); mv $(AUTOOBJ) ..; cd ..
	rm -r tmp
	touch $@
	
# Secondary targets.
# This does not need to include sources built in object directory by mkscall.
# The .s sources have explicit actions to avoid using -f in ASFLAGS.
_execve.o:	_execve.s
	$(AS) -o $@ $<
brk.o:		brk.s
	$(AS) -o $@ $<
cerror.o:	cerror.s
	$(AS) -o $@ $<
dup2.o:		dup2.c
fork.o:		fork.s
	$(AS) -o $@ $<
gtty.o:		gtty.c
msgemul.o:	msgemul.c
pipe.o:		pipe.s
	$(AS) -o $@ $<
sememul.o:	sememul.c
setpgrp.o:	setpgrp.c
shmemul.o:	shmemul.c
signal.o:	signal.s
	$(AS) -o $@ $<
time.o:		time.c
uname.o:	uname.c
ustat.o:	ustat.c
utsemul.o:	utsemul.c
wait.o:		wait.s
	$(AS) -o $@ $<

# Build mkscall, which builds system call sources automatically.
mkscall:	mkscall.c
	cc $<

# end of /usr/src/libc/sys/i386/Makefile
