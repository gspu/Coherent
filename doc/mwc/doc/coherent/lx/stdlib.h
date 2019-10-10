.TH stdlib.h "" "" "Header File"
.PC "Declare/define general functions"
.B "#include <stdlib.h>"
.PP
.B stdlib.h
is a header file that is defined in the \*(AS.
It declares a set of general utilities and defines attending macros
and data types, as follows.
.SH Types
.LB
\fBdiv_t\fR	Type of object returned by \fBdiv\fR
\fBldiv_t\fR	Type of object returned by \fBldiv\fR
.SH "Manifest Constants"
.LB
\fBEXIT_FAILURE\fR	Value to indicate that program failed to execute properly
\fBEXIT_SUCCESS\fR	Value to indicate that program executed properly
\fBMB_CUR_MAX\fR	Largest size of multibyte character in current locale
\fBMB_LEN_MAX\fR	Largest overall size of multibyte character in any locale
\fBRAND_MAX\fR	Largest size of pseudo-random number
.SH Functions
.LB
\fBabort()\fR	End program immediately
\fBabs()\fR	Compute the absolute value of an integer
\fBatof()\fR	Convert string to floating-point number
\fBatoi()\fR	Convert string to integer
\fBatol()\fR	Convert string to long integer
\fBbsearch()\fR	Search an array
\fBcalloc()\fR	Allocate dynamic memory
\fBdiv()\fR	Perform integer division
\fBexit()\fR	Terminate a program gracefully
\fBfree()\fR	De-allocate dynamic memory to free memory pool
\fBgetenv()\fR	Read environmental variable
\fBlabs()\fR	Compute the absolute value of a long integer
\fBldiv()\fR	Perform long integer division
\fBmalloc()\fR	Allocate dynamic memory
\fBqsort()\fR	Sort an array
\fBrand()\fR	Generate pseudo-random numbers
\fBrealloc()\fR	Reallocate dynamic memory
\fBsrand()\fR	Seed the random-number generator
\fBstrtod()\fR	Convert string to floating-point number
\fBstrtol()\fR	Convert string to long integer
\fBstrtoul()\fR	Convert string to unsigned long integer
\fBsystem()\fR	Suspend a program and execute another
.SH "See Also"
.Xr "header files" header_fi
.br
\*(AS, \(sc7.10
