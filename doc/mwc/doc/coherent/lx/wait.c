.TH wait "" "" Command
.PC "Await completion of background process"
\fBwait [\fIpid\^\fB]\fR
.PP
.HS
.I pid
identifies the process whose completion is awaited.
If no \fIpid\fR is given,
.B wait
awaits completion of all background processes.
The shell executes \fBwait\fR directly.
.HE
Typing the character `&' after a command tells the shell
.B sh
to execute it as a
.I background
(or
.IR detached )
process; otherwise, it is executed as a
.I foreground
process.
You can perform other tasks while a background process
is being executed.
The shell prints the process id number of each background
process when it is invoked.
.B ps
reports on currently active processes.
.PP
The command
.B wait
tells the shell to
suspend execution until the child process with the given
.I pid
is completed.
If no
.I pid
is given,
.B wait
suspends execution until all background processes are completed.
If the process with the given
.I pid
is not a child process of the current shell,
.B wait
returns immediately.
.PP
The shell executes
.B wait
directly.
.SH "See Also"
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "ps," ps
.Xr "sh" sh
.SH Notes
If a subshell invokes a background process and then terminates,
.B wait
returns immediately rather than waiting for the termination
of the grandchild process.
