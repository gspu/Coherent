.TH varargs.h "" "" "Header File"
.PC "Declare/define routines for variable arguments"
.B "#include <varargs.h>"
.PP
.II "varargs.h^vs. stdarg.h"
.II "stdarg.h^vs. varargs.h"
The header file
.B <varargs.h>
prototypes and defines the routines used to manage variable arguments.
These routines are modelled after those used in \*(UN System V.
The routines in
.B varargs.h
were designed to give a C compiler a semi-rational way of dealing
with functions (e.g., \fBprintf()\fR\^) that can take a variable
number of arguments.
In brief, these routines consist of the variable-list
.B typedef
.BR va_list ,
the parameter declaration
.BR va_dcl ,
and the three macros
.BR va_start() ,
.BR va_arg() ,
and
.BR va_end() .
The macros respectively
start the argument list, fetch the next
member, and end the argument list.
.SH "See Also"
.Xr "header files," header_fi
.Xr "stdarg.h" stdarg.h
.SH Notes
These routines are also implemented in the header file
.BR <stdarg.h> ,
which is described in the ANSI Standard.
For details on how these implementations differ, see the entry for
.BR stdarg.h .
