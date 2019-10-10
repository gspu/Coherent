.TH crypt() "" "" "General Function (libc)"
.PC "Encryption using rotor algorithm"
\fBchar *crypt(\fIkey, extra\^\fB); char *\fIkey\^\fB, *\fIextra\^\fB;\fR
.PP
.B crypt()
implements a version of rotor encryption.
It produces encrypted passwords that are verified by comparing
the encrypted clear text against an original encryption.
.PP
.I key
is an ASCII string that contains the user's password.
.I extra
is a ``salt'' string of two additional characters that are
stored in the password file with the encrypted password.
Each character must come from an alphabet of 64 symbols, which
consists of the upper-case and lower-case letters, digits, the period
\*(Ql.\*(Qr, and the slash \*(Ql/\*(Qr.
.PP
.B crypt()
returns a string built from the 64-character alphabet
described above; the first two characters returned are the
.I extra
argument, and the rest contain the encrypted
password.
.SH "See Also"
.Xr "libc" libc
