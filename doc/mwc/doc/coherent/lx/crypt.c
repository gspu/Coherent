.TH crypt "" "" Command
.PC "Encrypt/decrypt text"
\fBcrypt [\fIpassword\^\fB]\fR
.PP
.HS
Password is ten characters or fewer.
The same password encrypts and decrypts.
.HE
The command
.B crypt
encrypts data.
It emulates a rotor-encryption machine,
such as the Enigma or Hagelin C\-48 cipher machines.
Unlike these machines,
.B crypt
uses only one rotor, with a 256-character
alphabet and a keying sequence of period
2^32.
.PP
.B crypt
reads text from standard input and
writes the encrypted text to
standard output.
.I password
is used to construct the model of the machine
and to start the keying sequence.
If no
.I password
is given,
.B crypt
prompts for a password on the terminal and
disables echo while it is being typed in.
The
.I password
may be up to ten characters long,
but must not be empty;
all characters past the first ten are ignored.
All characters are legal,
although it may not be possible
to input certain characters from the terminal.
.PP
.B crypt
uses the same
.I password
for both encryption and decryption.
For example, the commands
.DM
	crypt COHERENT <file1 >file2
	crypt COHERENT <file2 >file3
.DE
.PP
leave
.I file3
identical to
.IR file1 .
.PP
Encrypted files produced by
.B ed
with its
.B -x
option may be read by
.BR crypt ,
and vice versa, as
.B ed
uses
.B crypt
to perform its encryption.
.PP
Security of a cryptosystem depends on several factors:
.IP \fB1.\fR 0.3i
Brute-force attempts to break the system should be infeasible.
Passwords should be at least five characters long;
although the construction of the machine model from the password
takes a substantial fraction of a second, it is still plausible that
encrypted files could be read by a brute-force search of
a portion of the password space (say, all passwords less than
four characters long).
.IP \fB2.\fR
Cryptanalysis of the basic encryption scheme
should be very hard.
Analysis of rotor machines is understood, but it is difficult
and in most cases probably not worth the trouble.
.IP \fB3.\fR
Passwords must be kept secret.
.B crypt
erases
.I password
as soon as it can, to avoid the possibility that
it could appear in the output of
.BR ps .
.IP \fB4.\fR
Privileged access to the system must be guarded.
Under \*(CO, the security of
.B crypt
can be no better than the security governing access to superuser status,
because the superuser can do practically anything.
This is probably
.BR crypt 's
most vulnerable point.
.SH Files
\fB/dev/tty\fR \(em Typed passwords
.SH "See Also"
.Xr "commands," commands
.Xr "passwd," passwd
.Xr "security," security
.Xr "shadow" shadow
