.TH kill() "" "" "System Call (libc)"
.PC "Kill a system process"
.B "#include <signal.h>"
\fBkill(\fIpid, sig\fB\^)\fR
\fBint \fIpid\^\fB, \fIsig\^\fB;\fR
.PP
.B kill()
is the \*(CO system call that sends a signal to a process.
.I pid
is the process identifier of the process to be signalled, and
.I sig
identifies the signal to be sent, as set in the header file
.BR signal.h .
This system call is most often used to kill processes,
hence its name.
.PP
See the Lexicon article for \fBsignal()\fR for a table of the signals
and what each means.
If
.I sig
is zero,
.B kill()
performs error checking, but sends no signal.
You can use this feature to check the validity of
.IR pid .
.SH Example
For an example of using this system call in a C program, see
.BR signal() .
.SH "See Also"
.Xr "libc," libc
.Xr "signal()," signal
.Xr "signal.h" signal.h
.R
.br
\*(PX Standard, \(sc3.3.2
