.TH umask() "" "" "System Call (libc)"
.PC "Set file-creation mask"
.B "#include <sys/stat.h>"
\fBint umask(\fImask\^\fB)\fR
\fBint \fImask\^\fB;\fR
.PP
.B umask()
allows a process to restrict the mode of files it creates.
Commands that create files should specify the maximum reasonable mode.
A parent (e.g. the shell
.BR sh )
usually calls
.B umask()
to restrict access to files created by subsequent commands.
.PP
.I mask
should be constructed from any of the permission bits found by
.B chmod()
(the low-order nine bits).
When a file is created with
.B creat()
or
.BR mknod() ,
every bit set in the
.I mask
is zeroed in
.IR mode ;
thus, bits set in
.I mask
specify permissions that will be denied.
.PP
.B umask()
returns the old value of the file-creation mask.
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
.Xr "creat()," creat
.Xr "libc," libc
.Xr "mknod()," mknod.s
.Xr "sh," sh
.Xr "stat.h" stat.h
.br
\*(PX Standard, \(sc5.3.3
.SH Notes
A file's default permission cannot be set to execute
regardless of the value of
.IR mask .
