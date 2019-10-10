.TH sync "" "" Command
.PC "Flush system buffers"
\fBsync\fR
.PP
Most \*(CO commands manipulate files stored on a disk.
To improve system performance, the \*(CO system often
changes a copy of part of the disk in a buffer in memory,
rather than repeatedly performing the time-consuming disk access required.
.PP
.B sync
writes information from the memory buffers to the disk,
updating the disk images of all mounted
file systems which have been changed.
In addition, it writes the date and time on the root file system.
.PP
.B sync
should be executed before system shutdown to ensure the integrity of
the file system.
.SH "See Also"
.Xr "commands" commands
