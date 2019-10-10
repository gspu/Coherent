.TH write() "" "" "System Call (libc)"
.PC "Write to a file"
.B "#include <unistd.h>"
\fBint write(\fIfd\^\fB, \fIbuffer\^\fB, \fIn\^\fB)\fR
\fBint \fIfd\^\fB; char *\fIbuffer\^\fB; int \fIn\^\fB;\fR
.PP
.B write()
writes
.I n
bytes of data, beginning at address
.IR buffer ,
into the file associated with the file descriptor
.IR fd .
Writing begins at the current write position, as set by the last call
to either
.B write()
or
.BR lseek() .
.B write()
advances the position of the file pointer by the number of characters written.
.SH Example
For an example of how to use this function, see the entry for
.BR open() .
.SH "See Also"
.Xr "libc," libc
.Xr "unistd.h" unistd.h
.br
\*(PX Standard, \(sc6.4.2
.SH Diagnostics
.B write()
returns \-1 if an error occurred before the
.B write()
operation commenced, such as a bad file descriptor
.I fd
or invalid
.I buffer
pointer.
Otherwise, it returns the number of bytes written.
It should be considered an error if this number is not the same as
.IR n .
.SH Notes
.B write()
is a low-level call that passes data directly to \*(CO.
Do not use it with the STDIO routines
.BR fread() ,
.BR fwrite() ,
.BR fputs() ,
or
.BR fprintf() .
