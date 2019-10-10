.TH mknod "" "" Command
.PC "Make a special file or named pipe"
\fB/etc/mknod [ \-f ] \fIfilename type major minor\fR
.br
\fB/etc/mknod [ \-f ] \fIfilename \fBp\fR
.PP
.HS
.SH Option
.IC \fB\-f\fR
Forces creation of a new node, even if one of same name already exists
.Pp
In first form of the command,
\fItype\fR is `b' for block special or `c' for character special;
\fImajor\fR and \fIminor\fR are numbers.
The second form creates a named pipe with the given \fIfilename\fR.
.HE
In the first form,
.B mknod
creates a
.I "special file,"
which provides access to a device by the
.I filename
specified.
Special files are conventionally stored in the
.B /dev
directory.
.PP
.I type
can be either \*(Qlb\*(Qr (for block-special file) or
\*(Qlc\*(Qr (for character-special file).
Block-special files tend to be devices such as disks or magnetic tape,
upon which \*(CO uses an elaborate buffering strategy.
Character-special files are unstructured (character at a time)
devices such as terminals, line printers, or communications devices.
Character-special files may also be random-access devices;
this circumvents system buffering, allowing
transfers of arbitrary size directly between the user and the hardware.
.PP
The
.I major
device number uniquely identifies a device driver to \*(CO.
The
.I minor
device number is a parameter interpreted by the driver;
it might specify the channel of a multiplexor or the unit number of a drive.
.PP
The caller must be the superuser.
.PP
In the second form,
.B mknod
creates a named pipe with the given
.I filename.
Named pipes can be used for communication between processes.
.PP
The
.B \-f
option to
.B mknod
forces the creation of a new node, even if one of the same name already
exists.
.SH Files
.B
/dev/*
.R
.SH "See Also"
.Xr "commands," commands
.Xr "mount" mount
