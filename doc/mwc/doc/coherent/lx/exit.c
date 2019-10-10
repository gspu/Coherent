.TH exit "" "" Command
.PC "Exit from a shell"
\fBexit [\fIstatus\^\fB]\fR
.PP
.HS
The previous status is retained if none is specified.
\fBexit\fR sets the status but does not terminate an interactive shell.
The shell executes \fBexit\fR directly.
.HE
.B exit
terminates a shell.
If the optional
.I status
is specified, the shell returns it;
otherwise, the previous status is unchanged.
From an interactive shell,
.B exit
sets the
.I status
if specified, but does not terminate the shell.
The shell executes
.B exit
directly.
.SH "See Also"
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "sh" sh
