.TH sleep "" "" Command
.PC "Stop executing for a specified time"
\fBsleep \fIseconds\fR
.PP
The command
.B sleep
suspends execution for a specified number of
.IR seconds .
This routine is especially useful with other commands to the shell.
For example, typing
.DM
	(sleep 3600; echo coffee break time) &
.DE
.PP
executes the
.B echo
command in one hour (3,600 seconds)
to indicate an important appointment.
.SH "See Also"
.Xr "alarm()," alarm
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "pause()," pause
.Xr "sh" sh
