.TH umount() "" "" "System Call (libc)"
.PC "Unmount a file system"
.B "#include <sys/mount.h>"
\fBumount(\fIfilesystem\fB\^)\fR
\fBchar *\fIfilesystem\^\fB;\fR
.PP
.B umount()
is the \*(CO system call that unmounts a file system.
.I filesystem
names the block-special file through which the file system
is accessed.
Note that this must have been previously mounted by a call to
.BR mount() ,
or the call will fail.
.SH "See Also"
.B
libc,
mount()
.R
