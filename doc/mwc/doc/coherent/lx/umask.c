.TH umask "" "" Command
.PC "Set the file-creation mask"
\fBumask [\fIOOO\^\fB]\fR
.PP
.HS
.I OOO
are three octal digits that describe file permissions.
The shell executes \fBumask\fR directly.
.HE
.II "file-creation mask"
.II "mask, default"
.II "permissions^changing default"
The
.I "file-creation mask"
modifies the default mode assigned to each file upon creation.
The mode sets the permissions granted by the file's owner, plus
other important information about a file.
.PP
The command
.B umask
sets the default file-creation mask to
.IR OOO ,
which are three octal numerals.
If invoked without an argument,
.B umask
prints the current file-creation mask in octal.
.PP
Note that zero bits in the mask
correspond to permitted permission bits in the target,
and that execute permission cannot be enabled via any setting of
.IR mask .
See the Lexicon entries for
.B umask()
and
.B chmod
for further details on file mode.
The shell executes \fBumask\fR directly.
.SH Example
Setting
.I mask
to octal 022 (i.e., 000 010 010) causes a file created with mode octal
0666 to actually have permissions of
.DM
	rw- r-- r--
.DE
.PP
Setting
.I mask
to zero (i.e., 000 000 000) causes a file created with mode octal
0666 to actually have permissions of
.DM
	rw- rw- rw-
.DE
.SH "See Also"
.Xr "chmod," chmod.c
.Xr "commands," commands
.Xr "ksh," ksh
.Xr "sh," sh
.Xr "umask()" umask.s
