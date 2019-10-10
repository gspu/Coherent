.TH pow() "" "" "Mathematics Function (libm)"
.PC "Compute a power of a number"
.B "#include <math.h>"
\fBdouble pow(\fIz\^\fB, \fIx\^\fB)\fR
\fBdouble \fIz\^\fB, \fIx\^\fB;\fR
.PP
.B pow()
returns \fIz\fR raised to the power of \fIx\fR, or \fIz\fB^\fRx\fR.
If an overflow error occurs (that is, you attempt to compute a number
that is too large to fit into a double-precision floating-point number),
.B pow()
returns a huge value and sets
.B errno
to
.BR ERANGE .
.SH Example
For an example of this function, see the entry for
.BR log10() .
.SH "See Also"
.Xr "libm" libm
.br
\*(AS, \(sc4.5.5.1
.br
\*(PX Standard, \(sc8.1
