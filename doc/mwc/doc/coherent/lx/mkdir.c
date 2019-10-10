.TH mkdir "" "" Command
.PC "Create a directory"
\fBmkdir [ \-rp ] \fIdirectory\fR
.PP
.HS
.SH Option:
.IC \fB\-r\fR
Make parent directories recursively as required
.IC \fB\-p\fR
Same as \fB\-r\fR; included for reasons of compatibility
.HE
.B mkdir
creates
.IR directory .
Files or directories with the same name as
.I directory
must not already exist.
.B directory
will be empty except for the entries `.',
the directory's link to itself, and `..', its link to its parent directory.
.PP
Option
.B \-r
creates directories recursively.
For example, the command
.DM
	mkdir -r /foo/bar/baz
.DE
.PP
creates directory
.B foo
in \fB/\fR; then creates directory
.B bar
in the newly created directory
.BR foo ;
and finally creates directory
.B baz
in the newly created directory
.BR bar .
.PP
Option
.B \-p
behaves exactly the same as
.BR \-r .
\*(CO includes it for use by scripts imported from other operating systems.
.SH "See Also"
.Xr "commands," commands
.Xr "mkdir()," mkdir.s
.Xr "rmdir" rmdir.c
.SH Diagnostics
.B mkdir
fails and prints an error message if you do not have permission to
write into directory in which you are attempting to create a new directory
or if the directory in which you attempted to create a new directory
does not exist.
