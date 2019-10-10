.TH times "" "" Command
.PC "Print total user and system times"
\fBtimes\fR
.PP
.B times
prints the total elapsed user time and system time for the current shell
and all its children.
It gives each time in minutes, seconds and tenths of seconds.
For example,
.DM
	1m11.8s 1m35.8s
.DE
.PP
indicates a total user time of 1 minute 11.8 seconds,
and a total system time of 1 minute 35.8 seconds.
.PP
The shell executes
.B times
directly.
.SH "See Also"
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "time," time
.Xr "sh" sh
