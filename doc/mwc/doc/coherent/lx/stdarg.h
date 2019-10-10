.TH "stdarg.h" "" "" "Header File"
.PC "Header for variable numbers of arguments"
\fB#include <stdarg.h>\fR
.PP
.B stdarg.h
is the header file that ANSI C uses to declare and define the routines
that traverse a variable-length argument list.
It declares the type
.B va_list
and defines the macros
.BR va_arg() ,
.BR va_start() ,
and
.BR va_end() .
.SH Example
The following example
concatenates multiple strings into a common allocated string
and returns the string's address.
method of handling variable arguments:
.DM
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
.DE
.DM
char *
multcat(numargs)
int numargs;
{
	va_list argptr;
	char *result;
	int i, siz;
.DE
.DM
	/* get size required */
	va_start(argptr, numargs);
	for(siz = i = 0; i < numargs; i++)
		siz += strlen(va_arg(argptr, char *));
.DE
.DM
	if ((result = calloc(siz + 1, 1)) == NULL) {
		fprintf(stderr, "Out of space\en");
		exit(EXIT_FAILURE);
	}
	va_end(argptr);
.DE
.DM
	va_start(argptr, numargs);
	for(i = 0; i < numargs; i++)
		strcat(result, va_arg(argptr, char *));
	va_end(argptr);
	return(result);
}
.DE
.DM
int
main()
{
	printf(multcat(5, "One ", "two ", "three ",
		"testing", ".\en"));
}
.DE
.SH "See Also"
.Xr "header files," header_fi
.Xr "varargs.h" varargs.h
.br
\*(AS, \(sc7.8
.SH Notes
The routines defined in
.B <stdarg.h>
were first implemented under \*(UN System V, where they are declared in the
header file
.BR <varargs.h> .
The ANSI C committee recognized the usefulness of
.BR <varargs.h> ,
but decided that it had semantic problems.
In particular,
.B <varargs.h>
introduced the notion of declaring ``...'' for the variable-arguments
argument list in the function prototype.
This, unfortunately, left them with declarations of the form
.DM
	void error(...)
	{
		\fIwhatever\fP
	}
.DE
.PP
and no obvious hook for accessing the parameter list within
the body of the function.
So, the ANSI committee changed the header declaration:
it insisted on one or more formal parameters, followed
by the list of variables.
.PP
The committee had the wisdom to change the name of its header
file, hence
.B <stdarg.h>
came into being.
Unfortunately, the committee kept the same macro names,
but in one macro (\fBva_start()\fR) changed the number of
arguments it takes.
.PP
\*(CO includes both
.B <varargs.h>
and
.BR <stdarg.h> ,
to support both ANSI and System-V code.
