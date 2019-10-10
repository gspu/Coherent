.TH exit() "" "" "General Function (libc)"
.PC "Terminate a program gracefully"
.B "#include <stdlib.h>"
\fBvoid exit(\fIstatus\^\fB) int \fIstatus\^\fB;\fR
.PP
The library function
.B exit()
is the normal method to terminate
a program directly.
.I status
information is passed to the parent process.
By convention, an exit status of zero indicates success, whereas an
exit status greater than zero indicates failure.
If the parent process issued a
.B wait()
call, it is notified of the termination
and is passed the least significant eight bits of
.IR status .
As
.B exit()
never returns, it is always successful.
Unlike the system call
.BR _exit() ,
.B exit()
does extra cleanup, such as flushing buffered files and closing open files.
.SH Example
For an example of this function, see the entry for \fBfopen()\fR.
.SH "See Also"
.Xr "_exit()," _23exit
.Xr "atexit()," atexit
.Xr "close()," close
.Xr "EXIT_FAILURE," exit_fail
.Xr "EXIT_SUCCESS," exit_succ
.Xr "libc," libc
.Xr "stdlib.h," stdlib.h
.Xr "wait()" wait
.br
\*(AS, \(sc7.10.4.3
.br
\*(PX Standard, \(sc8.1
.SH Notes
If you do not explicitly set
.I status
to a value,
the program returns whatever value happens to have been in the register EAX.
You can set
.I status
to either
.B EXIT_SUCCESS
or
.BR EXIT_FAILURE .
