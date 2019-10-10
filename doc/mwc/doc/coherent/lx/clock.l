.TH clock() "" "" "Time Function (libc)"
.PC "Get processor time"
.B "#include <time.h>"
\fBclock_t clock();\fR
.PP
.II "time^measure amount needed to execute program"
.II "program^return time needed to execute"
The function
.B clock()
calculates and returns the amount of processor time
a program has taken to execute to the current point.
Execution time is calculated from the time the program was invoked.
This, in turn, is set as a point from the beginning of an era
that is defined by the implementation.
Under \*(CO, time is recorded as
the number of milliseconds since January 1, 1970, 0h00m00s GMT.
.PP
The value
.B clock()
returns is of type
.BR clock_t ,
which is defined in header file.
.BR time.h .
If
.B clock()
cannot determine execution time, it returns \-1 cast to
.BR clock_t .
.PP
To calculate the execution time in seconds, divide the value returned by
.B clock()
by the value of the macro
.BR CLK_TCK ,
which is also defined in
.BR time.h .
.SH Example
This example measures the number of times a
.B for
loop can run in one second on your system.
This is approximate because
.B CLK_TCK
can be a real number, and because the program
probably will not start at an exact tick boundary.
.DM
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
.DE
.DM
main()
{
	clock_t finish;
	long i;
.DE
.DM
	/* finish = about 1 second from now */
	finish = clock() + CLK_TCK;
	for(i = 0; finish > clock(); i++)
		;
.DE
.DM
	printf("The for() loop ran %ld times in one second.\en", i);
	return(EXIT_SUCCESS);
}
.DE
.SH "See Also"
.Xr "difftime()," difftime
.Xr "libc," libc
.Xr "mktime()," mktime
.Xr "time.h" time.h
.br
\*(AS, \(sc.12.2.1
