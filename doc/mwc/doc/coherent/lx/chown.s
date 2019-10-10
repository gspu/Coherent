.TH "chown()" "" "" "System Call (libc)"
.PC "Change ownership of a file"
.B "#include <unistd.h>"
\fBchown(\fIfile, uid, gid\fB\^)\fR
\fBchar *\fIfile\^\fB; short \fIuid, gid\^\fB;\fR
.PP
.B chown()
changes the owner of
.I file
to user id
.I uid
and
group id
.I gid.
.PP
To change only the user id without changing the group id,
use
.B stat()
to determine the value of
.I gid
to pass to
.BR chown() .
.PP
.B chown()
is restricted to the superuser,
because granting the ordinary user the ability to change the ownership
of files might circumvent file
space quotas or accounting based upon file ownership.
.PP
.B chown()
returns \-1 for errors, such as nonexistent
.I file
or the caller not being the superuser.
.SH "See Also"
.Xr "chmod()," chmod
.Xr "libc," libc
.Xr "passwd," passwd
.Xr "stat()," stat
.Xr "unistd.h" unistd.h
.br
\*(PX Standard, \(sc5.6.5
