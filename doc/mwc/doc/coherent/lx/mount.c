.TH mount  "" "" Command
.PC "Mount a file system"
\fB/etc/mount [ \fIdevice directory \fB[ \-ru ] ]\fR
.PP
.HS
.SH Options:
.IC \fB\-r\fR
Mount device read-only
.IC \fB\-u\fR
Update \fB/etc/mtab\fR entry but do not mount device
.Pp
With no arguments, print devices currently mounted.
\fIdevice\fR names a device-special file;
\fIdirectory\fR names the directory on which to mount it.
File \fB/bin/mount\fR contains useful abbreviations for invoking
\fB/etc/mount\fR.
.HE
The command
.B mount
mounts a file system from
.I device
onto
.IR directory .
In effect, it grafts the root directory of file system on
.I device
onto
.IR directory .
.PP
.PP
If you invoke
.B mount
without any arguments, it displays information about the file systems
that are now mounted.
.PP
If you use option
.BR \-r ,
.B mount
mounts the specified file system in read-only mode.
This is useful if you wish to read a file system without changing it
in any such way, such as when you are backing it up.
Note, however, that when a file system is mounted in read-only mode,
\*(CO does not update file-system information, such the time a file was
last accessed.
.PP
The option
.B \-u
tells
.B mount
to write an entry into the mount-table file
.B /etc/mtab
without actually mounting the file system.
When this is done, \*(CO will hereafter mount the file system automatically
whenever you boot \*(CO.
.PP
Please note that unlike every other \*(CO or \*(UN command ever devised,
.B mount
requires that its options
.I follow
the file names, rather than precede them.
The \*(CO version of
.B mount
follows this convention in order to conform to this established \*(UN practice.
.\" So don't blame us for it.
.PP
To un-mount a file system, use the command
.BR umount .
(NB, this is not a typographical error \(em this command's name contains
only one `n'.)
.PP
The script
.B /bin/mount
calls
.BR /etc/mount ,
and provides convenient abbreviations for commonly used devices.
For example,
.DM
	mount f0
.DE
.PP
executes the command:
.DM
	/etc/mount /dev/fha0 /f0
.DE
.PP
You should edit this script to reflect the devices that you use on your system.
.SH Files
\fB/etc/mtab\fR \(em Mount table
.br
\fB/etc/mnttab\fR \(em Mount table
.br
\fB/bin/mount\fR \(em Shell script that calls \fB/etc/mount\fR
.SH "See Also"
.Xr "commands," commands
.Xr "fsck," fsck
.Xr "mkfs," mkfs
.Xr "mknod," mknod
.Xr "umount" umount
.SH Diagnostics
Errors can occur if
.I device
or
.I directory
does not exist or if you do not have permission to access
.IR device .
.PP
The message
.DM
	/etc/mtab older than /etc/boottime
.DE
.PP
indicates that
.B /etc/mtab
has probably been invalidated by booting the system.
.PP
Attempting to mount a block-special file
that does not contain a \*(CO file system (e.g., a tape device)
can have disastrous consequences.
\fICaveat utilitor!\fR
To build a file system on a block-special device, use the command
.BR /etc/mkfs .
For details, see its entry in the Lexicon.
