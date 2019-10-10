.TH read() "" "" "System Call (libc)"
.PC "Read from a file"
.B "#include <unistd.h>"
\fBint read(\fIfd\^\fB, \fIbuffer\^\fB, \fIn\^\fB)\fR
\fBint \fIfd\^\fB; char *\fIbuffer\^\fB; int \fIn\^\fB;\fR
.PP
.B read()
reads up to
.I n
bytes of data from the file descriptor
.I fd
and writes them into
.IR buffer .
The amount of data actually read may be less than that requested if
.B read()
detects
.BR EOF .
The data are read beginning at the current seek position in the file,
which was set by the most recently executed
.B read()
or
.B lseek()
routine.
.B read()
advances the seek pointer by the number of characters read.
.PP
If all goes well,
.B read()
returns the number of bytes read;
thus, zero bytes signals the end of the file.
It returns \-1 if an error occurs, e.g.,
.I fd
does not describe an open file, or if
.I buffer
contains an illegal address.
.SH Example
For an example of how to use this function, see the entry for
.BR open() .
.SH "See Also"
.Xr "libc," libc
.Xr "unistd.h" unistd.h
.R
.br
\*(PX Standard, \(sc6.4.1
.SH Notes
.B read()
is a low-level call that passes data directly to \*(CO.
It should not be mixed with the STDIO routines
.BR fread() ,
.BR fwrite() ,
or
.BR fopen() .
