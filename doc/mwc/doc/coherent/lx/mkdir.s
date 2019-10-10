.TH mkdir() "" "" "System Call (libc)"
.PC "Create a directory"
.B "#include <sys/types.h>"
.B "#include <sys/stat.h>"
\fBint mkdir(\fIpath\^\fB, \fImode\^\fB)\fR
\fBchar *\fIpath\^\fB;\fR
\fBint \fImode\^\fB;\fR
.PP
The \*(CO system call
.B mkdir()
creates the directory specified by
.I path
and gives it the file mode specified by
.IR mode .
If
.I path
is relative (that is, it doesn't begin with a `/' character),
.B mkdir()
creates the directory relates to the
current directory of the process that calls
.BR mkdir() .
If
.I path
is absolute (i.e., begins with a `/'),
.B path
specifies a directory to be created relative
to the root directory for this process.
See Lexicon article
.B chroot()
for details.
If
.I path
specifies more than one directory level,
all parent names specified must exist,
must be accessible by the calling process,
and actually must be directories.
.PP
Argument
.I mode
is formed by logically OR'ing permissions constants found
in header file
.BR <sys/stat.h> .
These constants begin with
.B S_
and determine the permissions for the directory.
See the Lexicon article
.B stat.h
for details.
.PP
If the directory is successfully created,
.B mkdir()
returns zero.
If an error occurs,
.B mkdir()
returns \-1 and sets
.B errno
to an appropriate value.
.SH "See Also"
.Xr "libc," libc
.Xr "mkdir," mkdir.c
.Xr "rmdir," rmdir.c
.Xr "rmdir()," rmdir.s
.Xr "stat.h" stat.h
.br
\*(PX Standard, \(sc5.4.1
