.TH for "" "" Command
.PC "Execute commands for tokens in list"
\fBfor \fIname \fB[in \fItoken ...\fB] do \fIsequence\fB done\fR
.PP
.HS
If \fIin\fR clause is omitted, list of positional parameters to
current script is assumed.
Both \fBdo\fR and \fBdone\fR must be first token on line or preceded by `;'.
The shell executes \fBfor\fR directly.
.HE
The shell command
.B for
controls a loop.
It assigns to the variable
.I name
each successive
.I token
in the list, and then executes the commands in the given
.IR sequence .
If the
.B in
clause is omitted,
.B for
successively assigns
.I name
the value of each positional parameter to the current script (`$@').
Because the shell recognizes a reserved word only as the
unquoted first word of a command, both
.B do
and
.B done
must either occur unquoted at the start of a command or be preceded by ';'.
.PP
The shell commands
.B break
and
.B continue
may be used to alter control flow within a
.B for
loop.
.PP
The shell executes
.B for
directly.
.SH "See Also"
.Xr "break," break.c
.Xr "commands," commands
.Xr "continue," continuec
.Xr "ksh," ksh
.Xr "sh" sh
