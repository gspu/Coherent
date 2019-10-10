.TH mknod() "" "" "System Call (libc)"
.PC "Create a special file"
.B "#include <sys/ino.h>"
.B "#include <sys/stat.h>"
\fBmknod(\fIname, mode, addr\^\fB)\fR
\fBchar *\fIname\^\fB; int \fImode, addr\^\fB;\fR
.PP
.B mknod()
is the \*(CO system call that creates a special file.
A
.I "special file"
is one through which a device is accessed, or a named pipe.
.PP
.I mode
gives the type of special file to be created.
It can be set to
.BR IFBLK ,
for a block-special device, such as a disk driver;
to
.BR IFCHR ,
for a character-special device, such as a serial-port driver;
to
.BR IFDIR ,
for a directory;
or to
.BR IFPIPE ,
for a named pipe.
.I mode
also contains permission mode bits.
.PP
.I address
is a parameter interpreted by the driver;
it might specify the channel of a multiplexor or the unit number of a drive.
Note that this is not used with named pipes.
.PP
If all goes well,
.B mknod()
returns zero.
If an error occurs, it returns a negative value and sets
.B errno
to an appropriate value.
.SH "See Also"
.Xr "libc," libc
.Xr "device drivers," device_dr
.Xr "named pipe," named_pip
.Xr "pipe()" pipe.s
.SH Notes
Only the superuser
.B root
can use
.BR mknod() .
This is a security feature.
