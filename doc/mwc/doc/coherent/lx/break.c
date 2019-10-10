.TH break "" "" Command
.PC "Exit from shell construct"
\fBbreak\fR [ \fIn\fR ]
.PP
.HS
Exit from \fIn\fR (default, one) \fBfor\fR, \fBuntil\fR, or \fBwhile\fR
constructs.
The shell executes \fBbreak\fR directly.
.HE
The command
.B break
is used with the shell
to control how it performs loops.
It is analogous to the
.B break
keyword in C.
.PP
When it is used without an argument,
.B break
forces
the shell to exit from the innermost current
.BR for ,
.BR until ,
or
.B while
loop.
If used with an argument,
.B break
exits from
.I n
levels of
.BR for ,
.BR until ,
or
.B while
loops.
.PP
The shell executes
.B break
directly.
.SH "See Also"
.Xr commands, commands
.Xr continue, continue
.Xr for, for
.Xr ksh, ksh
.Xr sh, sh
.Xr until, until
.Xr while while
