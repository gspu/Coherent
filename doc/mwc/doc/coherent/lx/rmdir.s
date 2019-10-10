.TH rmdir() "" "" "System Call (libc)"
.PC "Remove a directory"
.B "#include <unistd.h>"
\fBint rmdir(\fIpath\^\fB)\fR
\fBchar *\fIpath\^\fB;\fR
.PP
The \*(CO system call
.B rmdir()
removes the directory specified by argument
.IR path .
To remove the directory,
the following conditions must apply:
.IP \(bu 0.3i
.I path
must exist and be accessible,
it must be empty (i.e., contain only entries for `.' and `..').
.IP \(bu
You must have permission to remove the directory.
.IP \(bu
The file system that contains
.I path
must not be mounted ``read only''.
.IP \(bu
The directory must not be the current directory for any process.
.IP \(bu
The directory must not be a mount point for another file system.
.PP
If the directory is successfully removed,
.B rmdir()
returns zero.
If an error
occurs, it returns \-1 and sets
.B errno
to an appropriate value.
.SH "See Also"
.Xr "libc," libc
.Xr "mkdir," mkdir.c
.Xr "mkdir()," mkdir.s
.Xr "rmdir," rmdir.c
.Xr "unistd.h," unistd.h
.Xr "unlink()" unlink
.br
\*(PX Standard, \(sc5.5.2
