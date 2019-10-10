.TH passwd "" "" Command
.PC "Set/change login password"
\fBpasswd [\fIuser\^\fB]\fR
.PP
.B passwd
sets or changes the password for the specified
.IR user .
If
.I user
is not specified,
.B passwd
changes the password of the caller.
.PP
.B passwd
requests that the old password (if any) be typed, to ensure
the caller is who he claims to be.
Next it requests a new password,
and then requests it again in case of typing errors.
.B passwd
requests a longer password if the one given is too brief or too simple.
.SH Files
\fB/etc/shadow\fR \(em Encrypted passwords
.SH "See Also"
.Xr "commands," commands
.Xr "crypt()," crypt.g
.Xr "login" login
.SH Notes
One good way to construct a password is to concatenate
two common words plus a punctuation mark.
For example, ``dog~collar'' or ``hamlet&horatio'' are passwords that
are both easy to remember and difficult to guess.
