.TH if "" "" Command
.PC "Execute a command conditionally"
\fBif \fIsequence1 \fBthen \fIsequence2 \fB[elif \fIsequence3 \fBthen \fIsequence4\fB] ... [else \fIsequence5\fB] fi\fR
.PP
.HS
Each \fBthen\fR, \fBelif\fR, \fBelse\fR, and \fBfi\fR
must occur unquoted at the start of a line or preceded by `;'.
The shell executes \fBif\fR directly.
.HE
The shell construct
.B if
executes commands conditionally, depending on the
exit status of the execution of other commands.
.PP
First,
.B if
executes the commands in
.IR sequence1 .
If the exit status is zero, it executes the commands in
.I sequence2
and terminates.
Otherwise, it executes the optional
.I sequence3
if given, and executes
.I sequence4
if the exit status is zero.
It executes additional
.B elif
clauses similarly.
If the exit status of each tested command sequence is nonzero, it
executes the optional
.B else
part
.IR sequence5 .
.PP
Because the shell
recognizes a reserved word only as the unquoted first word of a command,
each
.BR then ,
.BR elif ,
.BR else ,
and
.B fi
must either occur unquoted at the start of a line or be preceded by `;'.
.PP
The shell executes
.B if
directly.
.SH Example
For an example of this command, see the entry for
.BR trap .
.SH "See Also"
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "sh," sh
.Xr "test" test
