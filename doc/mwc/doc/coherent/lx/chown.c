.TH chown "" "" Command
.PC "Change the owner of files"
\fBchown \fIowner file ...\fR
.PP
.B chown
changes the owner of each
.I file
to
.IR owner .
The
.I owner
may be specified by valid user name or a valid numerical user id.
.PP
Only the superuser may use
.B chown .
.SH Files
\fB/etc/passwd\fR \(em To convert user name to user id
.SH "See Also"
.Xr "chgrp," chgrp
.Xr "chmod," chmod
.Xr "chmog," chmog
.Xr "commands" commands
