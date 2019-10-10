.TH chroot "" "" Command
.PC "Change root directory"
\fBchroot \fIdirectory program ...\fR
.PP
The command
.B chroot
runs program
.I program
with root directory
.IR directory .
.SH "See Also"
.Xr "commands" commands
.SH Notes
Only the superuser
.B root
can use
.BR chroot .
