.TH rmdir "" "" Command
.PC "Remove directories"
\fBrmdir [ \-f ] \fIdirectory ...\fR
.PP
.HS
.SH Option:
.IC \fB\-f\fR
Force: remove a file without interactive checking
.HE
.B rmdir
removes each
.IR directory .
This will not be allowed if a
.I directory
is the current working directory or is not empty.
The force option
.B \-f
allows the superuser to override these restrictions.
.B rmdir
removes the `.' and `..' entries automatically.
Note that using the \fB\-f\fR option on a directory that is not empty
will damage the file system, and require that it be fixed with
.BR fsck .
.SH "See Also"
.Xr "commands," commands
.Xr "mkdir," mkdir.c
.Xr "rm" rm
.SH Notes
.B "rmdir \-f"
does
.I not
remove files from a nonempty directory:
it simply orphans them.
To remove a nonempty directory and its contents, use
.B "rm \-r"
instead.
