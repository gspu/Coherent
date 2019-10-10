.NH Errata 1
.SH "Known Limitations"
.PP
.IP 1. 0.3i
The option
.B \-g
to commands
.B gcc
and
.B g++
is known not to work with this release of gcc for \*(CO.
.IP 2.
The ``soft float'' floating-point emulation option to
.B gcc
and
.B g++
is reported not to work as described in the
.B gcc
and
.B g++
documentation.
Because
.B gcc
and
.B g++
generate hardware floating-point instructions by default,
this should not be a problem unless targeting non-\*(CO systems.
.IP 3.
Several header files in the
.B g++
release contain constructs of the form:
.DM
	struct trick {
		// member list goes here
		char a[1];
	};
.DE
The following header files (at least) contain this trick:
.BR <BitSet.h> ,
.BR <BitString.h> ,
.BR <Fix.h> ,
.BR <Integer.h> ,
and
.BR <String.h> .
The corresponding code actually allocates space for
\fBsizeof(struct trick)+\fIn\fP-1\fR
and uses the last member above as \fBa[\fIn\fP]\fR rather
than \fBa[1]\fR.
As discussed in detail on pp. 172 ff. of
\fIThe Annotated C++ Reference Manual\fR (Ellis & Stroustrup, 1990),
this is no longer legal and the generated code may not work.
The
.B g++
test programs for the corresponding library functions
usually core dumped when executed.
\fICaveat utilitor!\fR
.IP 4.
The \fBf\fR format converter to
.B printf()
and
.B sprintf()
does not always correctly round the integer portion of the output string.
For example, the expression
.DM
	printf("%5.3f\en", (4305.0 /4307.0));
.DE
.IP
prints ``0.000'' instead of the correct value ``1.000''.
.IP 5.
Due to an apparent problem with the
.B gcc
parser, constants of type
.B float
or
.B double
should have a leading digit.
For example, the program
.DM
	#include <math.h>
	main() {
		printf("%g\en", sin(.5));
	}
.DE
.IP
prints `0', whereas the program
.DM
	#include <math.h>
	main() {
		printf("%g\en", sin(0.5));
	}  
.DE
.IP
prints the correct value.
