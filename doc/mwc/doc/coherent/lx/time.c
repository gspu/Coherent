.TH time "" "" Command
.PC "Time the execution of a command"
\fBtime [\fIcommand\^\fB]\fR
.PP
.B time
invokes the given
.I command
with any arguments provided.
Upon termination,
.B time
prints the elapsed real time,
CPU time in the system, and CPU time in the user program
on the standard error output.
.\".PP
.\"If no
.\".I command
.\"is given,
.\".B time
.\"simply invokes
.\".B date
.\"to print the current time of day.
.SH "See Also"
.Xr "commands," commands
.Xr "date," date
.Xr "ps," ps
.Xr "times" times
.SH Diagnostics
If the
.I command
terminates abnormally,
.B time
displays an error message that explains why.
