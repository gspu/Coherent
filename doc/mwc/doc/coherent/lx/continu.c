.TH continue "" "" Command
.PC "Terminate current iteration of shell construct"
\fBcontinue \fR[ \fIn\fR ]
.PP
.HS
Terminate current iteration of \fIn\fR (default, one)
\fBfor\fR, \fBuntil\fR, or \fBwhile\fR constructs.
The shell executes \fBcontinue\fR directly.
.HE
The command
.B continue
helps to control the flow of commands given to the shell.
When it is used without an argument,
.B continue
terminates the execution of the current iteration of the innermost
.BR for ,
.BR until ,
or
.B while
shell construct;
that is,
it acts like a branch to the enclosing
.BR done ,
after which loop execution may continue or terminate.
If an argument is given,
.B continue
terminates the current iteration of the
.IR n th
enclosing
.BR for ,
.BR until ,
or
.B while
loop.
.PP
The shell
executes
.B continue
directly.
.SH "See Also"
.Xr "break," break
.Xr "commands," commands
.Xr "for," for
.Xr "ksh," ksh
.Xr "sh," sh
.Xr "until," until
.Xr "while" while
