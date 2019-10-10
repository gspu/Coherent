.TH mount() "" "" "System Call (libc)"
.PC "Mount a file system"
.B
#include <sys/mount.h>
#include <sys/filsys.h>
\fBint mount (\fIdevice\^\fB, \fIname\^\fB, \fIflag\fB\^)\fR
\fBchar *\fIdevice\^\fB, *\fIname\^\fB; int \fIflag\^\fB;\fR
.PP
.B mount()
is the \*(CO system call that mounts a file system.
.I device
names the physical device that through which the file system is accessed.
.I name
names the root directory of the newly mounted file system.
.I flag
controls the manner in which the file system is mounted, as set in header file
.BR "sys/mount.h" .
.SH "See Also"
.Xr "fd," fd
.Xr "libc," libc
.Xr "mount," mount.c
.Xr "mount.h" mount.h
