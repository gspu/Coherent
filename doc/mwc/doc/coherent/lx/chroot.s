.TH chroot() "" "" "System Call (libc)"
.PC "Change the root directory"
.B "#include <unistd.h>"
\fBint chroot(\fIpath\^\fB)\fR
\fBchar *\fIpath\^\fB;\fR
.PP
The \*(CO system call
.B chroot()
changes the current process's root directory
to that specified by
.IR path .
Once the
.B chroot()
system call completes, all references to absolute directories
(i.e., ones starting with `/') will actually refer to directory pointed to by
.IR path .
It does not change the current directory.
.PP
.B chroot()
is often used to add extra security to special or public login accounts.
.SH "See Also"
.Xr "chroot," chroot
.Xr "libc" libc
.SH Notes
The process that invokes
.B chroot()
must be running as the superuser
.BR root ,
and
.I path
must name a valid directory.
