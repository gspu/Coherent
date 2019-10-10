.TH while "" "" Command
.PC "Execute commands repeatedly"
\fBwhile \fIsequence1 \fB[do \fIsequence2\fB] done\fR
.PP
.HS
Both \fBdo\fR and \fBdone\fR
must be the first token on a line or preceded by `;'.
The shell executes \fBwhile\fR directly.
.HE
The shell construct
.B while
controls a loop.
It first executes the commands in
.IR sequence1 .
If the exit status is zero, the shell executes the commands in the optional
.I sequence2
and repeats the process until the exit status of
.I sequence1
is nonzero.
Because the shell
recognizes a reserved word only as the unquoted first word of a command, both
.B do
and
.B done
must occur unquoted at the start of a line or preceded by `;'.
.PP
The shell commands
.B break
and
.B continue
may be used to alter control flow within a
.B while
loop.
The
.B until
construct has the same form as
.BR while ,
but the sense of the test is reversed.
.PP
The shell executes
.B while
directly.
.SH "See Also"
.Xr "break," break.c
.Xr "commands," commands
.Xr "continue," continuec
.Xr "ksh," ksh
.Xr "sh," sh
.Xr "test," test
.Xr "until" until
