.TH time() "" "" "System Call (libc)"
.PC "Get current system time"
.B "#include <time.h>"
\fBtime_t time(\fItp\^\fB)\fR
\fBtime_t *\fItp\fB;\fR
.PP
.B time()
reads and returns the current system time.
\*(CO defines the current system time as the
number of seconds since January 1, 1970, 0h00m00s GMT.
.PP
.II time_t
.I tp
points to a data element of the type
.BR time_t ,
which the header file
.B time.h
defines as being equivalent to a
.BR long .
If
.I tp
is initialized to a value other than NULL, then
.B time()
attempts to write the system time into the address to which
.I tp
points.
If, however,
.I tp
is initialized to NULL, then
.B time()
returns the current system time but does not attempt to write it anywhere.
.SH Example
For an example of this call, see the entry for
.BR asctime() .
.SH "See Also"
.Xr "date," date
.Xr "libc," libc
.Xr "time [overview]," time.a
.Xr "time.h" time.h
.br
\*(AS, \(sc7.12.2.4
.br
\*(PX Standard, \(sc4.5.1
.SH Notes
\*(UN System V defines
.B time_t
in header file
.BR <sys/types.h> ,
whereas \*(CO defines it in
.BR time.h .
This should not affect the porting of programs from \*(UN to \*(CO,
but it may affect the porting of programs in the other direction.
