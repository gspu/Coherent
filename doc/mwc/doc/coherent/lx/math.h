.TH math.h "" "" "Header File"
.PC "Declare mathematics functions"
.B "#include <math.h>"
.PP
.B math.h
is the header file to be included with programs
that use any of \*(CO's mathematics routines.
It includes the following:
definitions for mathematical functions;
error return values, as used by the
.B errno
function;
definitions of mathematical constants, e.g.,
.BR HUGE_VAL ;
the definition of structure \fBcpx\fR, which describes
complex variables;
definitions of internal compiler functions;
and, finally, prototypes of all mathematical functions.
.SH "See Also"
.Xr "header files," header_fi
.Xr "libm" libm
.br
\*(AS \(sc7.5
