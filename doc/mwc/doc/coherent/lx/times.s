.TH times() "" "" "System Call (libc)"
.PC "Obtain process execution times"
.B "#include <sys/times.h>"
.B "#include <time.h>"
\fBint times(\fItbp\^\fB)\fR
\fBstruct tms *\fItbp\^\fB;\fR
.PP
.B times()
reads CPU time information about the current process and 
its children, and writes it into the structure pointed to by
.IR tbp .
The structure
.B tms
is declared in the header file
.BR sys/times.h ,
as follows:
.DM
.ta 0.5i 3.0i
struct tms {
	clock_t tms_utime;	/* process user time */
	clock_t tms_stime;	/* process system time */
	clock_t tms_cutime;	/* childrens' user times */
	clock_t tms_cstime;	/* childrens' system times */
};
.DE
.PP
All of the times are measured in basic machine cycles, or
.BR CLK_TCK .
.PP
The childrens' times include the sum of the times of all terminated
child processes of the current process and of all of their children.
The
.I user
time represents execution time of user code, whereas
.I system
time represents system overhead, such as executing system calls,
processing signals,
and other monitoring functions.
.PP
.II "system startup^time since"
.B times()
returns the number of ticks that have passed since system startup.
.SH Files
.B <sys/times.h>
.br
.B <time.h>
.SH "See Also"
.Xr "acct()," acct
.Xr "ftime()," ftime
.Xr "libc," libc
.Xr "time()" time.s
.Xr "times.h," times.h
.br
\*(PX Standard, \(sc4.5.2
