.TH sleep() "" "" "General Function (libc)"
.PC "Suspend execution for interval"
.B "#include <unistd.h>"
\fBsleep(\fIseconds\fR)
\fBunsigned \fIseconds\^\fB;\fR
.PP
.B sleep()
suspends execution for not less than
.IR seconds .
.SH Example
.II Godot
The following example
demonstrates how to use
.BR sleep() :
.DM
#include <unistd.h>
main()
{
	printf("Waiting for Godot ...\en");
.DE
.DM
	for ( ; ; ) {
		sleep(5);	/* sleep for five seconds */
		printf("... still waiting ...\en");
	}
}
.DE
.SH "See Also"
.Xr "libc," libc
.Xr "nap()," nap
.Xr "unistd.h" unistd.h
.br
\*(PX Standard, \(sc3.4.3
.SH Notes
To make a program sleep for less than one second, use the system calls
.B nap()
or
.BR poll() .
For an example, see the Lexicon article for
.BR poll() .
