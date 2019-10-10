# /usr/src/libc/sys/i8086/Makefile 9/28/92

OBJ=\
	_execve.o\
	access.o\
	acct.o\
	alarm.o\
	alarm2.o\
	brk.o\
	chdir.o\
	chmod.o\
	chown.o\
	chroot.o\
	close.o\
	creat.o\
	dup.o\
	dup2.o\
	execk.o\
	fcntl.o\
	fork.o\
	fstat.o\
	ftime.o\
	getegid.o\
	geteuid.o\
	getgid.o\
	getpgrp.o\
	getpid.o\
	getuid.o\
	ioctl.o\
	kill.o\
	link.o\
	lock.o\
	lseek.o\
	mknod.o\
	mount.o\
	msgop.o\
	nice.o\
	open.o\
	pause.o\
	pipe.o\
	poll.o\
	profil.o\
	ptrace.o\
	read.o\
	sbrk.o\
	semop.o\
	setgid.o\
	setpgrp.o\
	setuid.o\
	shmop.o\
	signal.o\
	sload.o\
	stat.o\
	stime.o\
	suload.o\
	sync.o\
	tick.o\
	time.o\
	times.o\
	umask.o\
	umount.o\
	unique.o\
	unlink.o\
	utime.o\
	wait.o\
	write.o

UNUSED=	_brk.s\
	msgctl.s\
	msgget.s\
	msgrcv.s\
	msgsnd.s
	
# Primary target.
SYSI8086:	$(OBJ)
	ar rkus libc.a $?
	touch $@
	: libc/sys/i8086 done.

# Secondary targets.
# The following are simple system traps.
_execve.o:	_proto.m
	cc -c -DNAME=_execve_  -DVALUE=11 -o $@ $<
access.o:	_proto.m
	cc -c -DNAME=access_  -DVALUE=33 -o $@ $<
acct.o:		_proto.m
	cc -c -DNAME=acct_    -DVALUE=51 -o $@ $<
alarm.o:	_proto.m
	cc -c -DNAME=alarm_   -DVALUE=27 -o $@ $<
alarm2.o:	_proto.m
	cc -c -DNAME=alarm2_  -DVALUE=72 -o $@ $<
brk.o:		_proto.m
	cc -c -DNAME=brk_     -DVALUE=17 -o $@ $<
chdir.o:	_proto.m
	cc -c -DNAME=chdir_   -DVALUE=12 -o $@ $<
chmod.o:	_proto.m
	cc -c -DNAME=chmod_   -DVALUE=15 -o $@ $<
chown.o:	_proto.m
	cc -c -DNAME=chown_   -DVALUE=16 -o $@ $<
chroot.o:	_proto.m
	cc -c -DNAME=chroot_  -DVALUE=61 -o $@ $<
close.o:	_proto.m
	cc -c -DNAME=close_   -DVALUE=6  -o $@ $<
creat.o:	_proto.m
	cc -c -DNAME=creat_   -DVALUE=8  -o $@ $<
dup.o:		_proto.m
	cc -c -DNAME=dup_     -DVALUE=41 -o $@ $<
execk.o:	_proto.m
	cc -c -DNAME=execk_   -DVALUE=62 -o $@ $<
fcntl.o:	_proto.m
	cc -c -DNAME=fcntl_   -DVALUE=66 -o $@ $<
fork.o:		_proto.m
	cc -c -DNAME=fork_    -DVALUE=2  -o $@ $<
fstat.o:	_proto.m
	cc -c -DNAME=fstat_   -DVALUE=28 -o $@ $<
ftime.o:	_proto.m
	cc -c -DNAME=ftime_   -DVALUE=35 -o $@ $<
getegid.o:	_proto.m
	cc -c -DNAME=getegid_ -DVALUE=56 -o $@ $<
geteuid.o:	_proto.m
	cc -c -DNAME=geteuid_ -DVALUE=57 -o $@ $<
getgid.o:	_proto.m
	cc -c -DNAME=getgid_  -DVALUE=47 -o $@ $<
getpid.o:	_proto.m
	cc -c -DNAME=getpid_  -DVALUE=20 -o $@ $<
getuid.o:	_proto.m
	cc -c -DNAME=getuid_  -DVALUE=24 -o $@ $<
ioctl.o:	_proto.m
	cc -c -DNAME=ioctl_   -DVALUE=54 -o $@ $<
kill.o:		_proto.m
	cc -c -DNAME=kill_    -DVALUE=37 -o $@ $<
link.o:		_proto.m
	cc -c -DNAME=link_    -DVALUE=9  -o $@ $<
lock.o:		_proto.m
	cc -c -DNAME=lock_    -DVALUE=53 -o $@ $<
lseek.o:	_proto.m
	cc -c -DNAME=lseek_   -DVALUE=19 -o $@ $<
mknod.o:	_proto.m
	cc -c -DNAME=mknod_   -DVALUE=14 -o $@ $<
mount.o:	_proto.m
	cc -c -DNAME=mount_   -DVALUE=21 -o $@ $<
nice.o:		_proto.m
	cc -c -DNAME=nice_    -DVALUE=34 -o $@ $<
open.o:		_proto.m
	cc -c -DNAME=open_    -DVALUE=5  -o $@ $<
pause.o:	_proto.m
	cc -c -DNAME=pause_   -DVALUE=29 -o $@ $<
pipe.o:		_proto.m
	cc -c -DNAME=pipe_    -DVALUE=42 -o $@ $<
poll.o:		_proto.m
	cc -c -DNAME=poll_    -DVALUE=67 -o $@ $<
profil.o:	_proto.m
	cc -c -DNAME=profil_  -DVALUE=44 -o $@ $<
ptrace.o:	_proto.m
	cc -c -DNAME=ptrace_  -DVALUE=26 -o $@ $<
read.o:		_proto.m
	cc -c -DNAME=read_    -DVALUE=3  -o $@ $<
setgid.o:	_proto.m
	cc -c -DNAME=setgid_  -DVALUE=46 -o $@ $<
setpgrp.o:	_proto.m
	cc -c -DNAME=setpgrp_ -DVALUE=62 -o $@ $<
setuid.o:	_proto.m
	cc -c -DNAME=setuid_  -DVALUE=23 -o $@ $<
sload.o:	_proto.m
	cc -c -DNAME=sload_   -DVALUE=64 -o $@ $<
stat.o:		_proto.m
	cc -c -DNAME=stat_    -DVALUE=18 -o $@ $<
stime.o:	_proto.m
	cc -c -DNAME=stime_   -DVALUE=25 -o $@ $<
suload.o:	_proto.m
	cc -c -DNAME=suload_  -DVALUE=65 -o $@ $<
sync.o:		_proto.m
	cc -c -DNAME=sync_    -DVALUE=36 -o $@ $<
tick.o:		_proto.m
	cc -c -DNAME=tick_    -DVALUE=73 -o $@ $<
times.o:	_proto.m
	cc -c -DNAME=times_   -DVALUE=43 -o $@ $<
umask.o:	_proto.m
	cc -c -DNAME=umask_   -DVALUE=60 -o $@ $<
umount.o:	_proto.m
	cc -c -DNAME=umount_  -DVALUE=22 -o $@ $<
unique.o:	_proto.m
	cc -c -DNAME=unique_  -DVALUE=45 -o $@ $<
unlink.o:	_proto.m
	cc -c -DNAME=unlink_  -DVALUE=10 -o $@ $<
utime.o:	_proto.m
	cc -c -DNAME=utime_   -DVALUE=30 -o $@ $<
wait.o:		_proto.m
	cc -c -DNAME=wait_    -DVALUE=7  -o $@ $<
write.o:	_proto.m
	cc -c -DNAME=write_   -DVALUE=4  -o $@ $<

# The following are not just system traps.
dup2.o:		dup2.c
getpgrp.o:	getpgrp.s
msgop.o:	msgop.c
sbrk.o:		sbrk.c
semop.o:	semop.c
shmop.o:	shmop.c
signal.o:	signal.s
time.o:		time.c

# Unused.
_brk.o:		_proto.m
	cc -c -DNAME=_brk_   -DVALUE=17 -o $@ $<
msgctl.o:	_proto.m
	cc -c -DNAME=msgctl_ -DVALUE=68 -o $@ $<
msgget.o:	_proto.m
	cc -c -DNAME=msgget_ -DVALUE=69 -o $@ $<
msgrcv.o:	_proto.m
	cc -c -DNAME=msgrcv_ -DVALUE=70 -o $@ $<
msgsnd.o:	_proto.m
	cc -c -DNAME=msgsnd_ -DVALUE=71 -o $@ $<

# end of libc/sys/i8086/Makefile
