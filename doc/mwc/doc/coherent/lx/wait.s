.TH wait() "" "" "System Call (libc)"
.PC "Await completion of a child process"
.B "#include <sys/wait.h>"
\fBwait(\fIstatp\^\fB)\fR
\fBint *\fIstatp\^\fB;\fR
.PP
.B wait()
suspends execution of the invoking process until a child process
(created with
.BR fork() )
terminates.
It returns the process identifier of the terminating child process.
If there are no children or if an interrupt occurs, it returns \-1.
.PP
If it is successful,
.B wait()
returns the process identifier of the terminated child process.
In addition,
.B wait()
fills in the integer pointed to by
.I statp
with exit-status information about the completed process.
If
.I statp
is NULL,
.B wait()
discards the exit-status information.
.PP
.B wait()
fills in the low byte of the status-information word with the termination
status of the child process.
A child process may have terminated
because of a signal, because of an exit call,
or have stopped execution during
.BR ptrace() .
Termination with
.BR exit() ,
which is normal completion, gives status 0.
Other terminations give signal values as status (as defined in the
article on
.BR signal() ).
The
.B 0200
bit of the status code indicates that a core dump was produced.
A status of
.B 0177
indicates that the process is waiting for further action from
.BR ptrace() .
.PP
The high byte of the returned status is the low byte of the argument
to the
.B exit()
system call.
.PP
If a parent process does not remain in existence long enough to
.B wait()
on a child process, the child process is adopted by process 1
(the initialization process).
.SH Example
For an example of this system call, see the entry for
.BR msgget() .
.SH "See Also"
.Xr "_exit()," _5Fexit
.Xr "fork()," fork
.Xr "ksh," ksh
.Xr "libc," libc
.Xr "ptrace()," ptrace
.Xr "signal()," signal
.Xr "sh," sh
.Xr "waitpid()," waitpid
.Xr "wait.h" wait.h
.br
\*(PX Standard, \(sc3.2.1
