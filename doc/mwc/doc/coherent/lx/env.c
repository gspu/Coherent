.TH env "" "" Command
.PC "Execute a command in an environment"
\fBenv [-] [\fIVARIABLE\fB=\fIvalue ...\^\fB] [\fIcommand args\^\fB]\fR
.PP
The command
.B env
executes
.I command
with
.IR args ,
modifying the existing environment by performing the requested assignments.
.PP
The `-' option tells
.B env
to replace the environment with the arguments of the form
\fBVARIABLE\fB=\fIvalue\fR;
otherwise the assignments are added to the environment.
.PP
If
.I command
is omitted, the resulting environment is printed.
.SH "See Also"
.Xr "commands" commands
