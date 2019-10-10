.TH msg "" "" Command
.PC "Send a brief message to other users"
\fBmsg \fIuser\fR
\fImessage\fR
.PP
The command
.B msg
prints the one-line
.I message
on the screen of
.IR user .
.PP
The message is sent as soon as you type
.B <return>
on the
.I message
line.
If
.I user
is not logged in or is not known to the system,
.B msg
prints an error message on your screen.
.SH "See Also"
.Xr "commands" commands
