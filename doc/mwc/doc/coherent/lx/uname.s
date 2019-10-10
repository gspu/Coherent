.TH uname() "" "" "System Call (libc)"
.PC "Get the name and version of \*(CO"
.B "#include <sys/utsname.h>
\fBint uname(\fIname\^\fB)\fR
\fBstruct utsname *\fIname\^\fB;\fR
.PP
.II "version^COHERENT"
.II "COHERENT^find version number"
The \*(CO system call
.B uname()
identifies the current release of the \*(CO operating system.
It writes its output into the structure pointed to by
.IR name .
This must be of type
.BR utsname ,
which has the following members:
.DM
.ta 0.5i 3.0i
	char sysname[SYS_NMLN];	/* system name */
	char nodename[SYS_NMLN];	/* UUCP node name */
	char release[SYS_NMLN];	/* current release */
	char version[SYS_NMLN];	/* current version */
	char machine[SYS_NMLN];	/* hardware */
.DE
.PP
.B uname()
returns a non-negative value upon success.
If something went wrong, i.e.,
.I name
points to an invalid address,
.B uname()
returns \-1 and sets \fBerrno\fR to an appropriate value.
.SH "See Also"
.Xr "libc," libc
.Xr "utsname.h" utsname.h
.br
\*(PX Standard, \(sc4.4.1
.SH Notes
The \*(CO implementation of
.B uname()
conforms to \*(PX, which states that
.B uname()
returns a ``non-negative'' value upon success.
To write portable code, your code must check for a return value that is
greater than or equal to zero.
It is an error to check for return value equal to zero, because the
test works on some systems that adhere to the Standard but not on others.
