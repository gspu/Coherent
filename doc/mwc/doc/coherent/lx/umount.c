.TH umount "" "" Command
.PC "Unmount file system"
\fB/etc/umount \fIspecial\fR
.PP
.B umount
unmounts a file system
.I special
that was previously mounted with the
.B mount
command.
.PP
The script
.B /bin/umount
calls
.BR /etc/umount ,
and provides convenient abbreviations for commonly used devices.
For example, typing
.DM
	umount f0
.DE
.PP
executes the command
.DM
	/etc/umount /dev/fha0	
.DE
.PP
The system administrator should edit this script to reflect the devices
used on your specific system.
.SH Files
\fB/etc/mtab\fR \(em Mount table
.br
\fB/dev/*\fR
.br
\fB/bin/umount\fR \(em Script that calls \fB/etc/umount\fR
.SH "See Also"
.Xr "clri," clri
.Xr "commands," commands
.Xr "fsck," fsck
.Xr "icheck," icheck
.Xr "mount" mount.c
.SH Diagnostics
Errors can occur if
.I special
does not exist or is not a mounted file system.
