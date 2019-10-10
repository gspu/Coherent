.TH chmod() "" "" "System Call (libc)"
.PC "Change file-protection modes"
.B "#include <sys/stat.h>"
\fBchmod(\fIfile\^\fB, \fImode\^\fB)\fR
\fBchar *\fIfile\^\fB; int \fImode\^\fB;\fR
.PP
.B chmod()
sets the mode bits for
.IR file .
The mode bits include protection bits, the set-user-id bit, and the sticky bit.
.PP
.I mode
is constructed from the logical OR of the mode constants declared in the
header file
.BR stat.h ,
as follows:
.DS
.ie t .ta 0.5i 1.2i
.el   .ta 0.5i 2.0i
	\fBS_ISUID\fR	Set user identifier on execution
	\fBS_ISGID\fR	Set group identifier on execution
	\fBS_ISVTX\fR	Save file on swap device (\*(QLsticky bit\*(QR)
	\fBS_IRUSR\fR	Read permission for owner
	\fBS_IWUSR\fR	Write permission for owner
	\fBS_IXUSR\fR	Execute permission for owner
	\fBS_IRGRP\fR	Read permission for members of owner's group
	\fBS_IWGRP\fR	Write permission for members of owner's group
	\fBS_IXGRP\fR	Execute permission for members of owner's group
	\fBS_IROTH\fR	Read permission for other users
	\fBS_IWOTH\fR	Write permission for other users
	\fBS_IXOTH\fR	Execute permission for other users
.DE
.PP
For directories, some protection bits have a different meaning:
write permission means files may be created and removed, whereas
execute permission means that the directory may be searched.
.PP
The save-text bit (or \*(QLsticky bit\*(QR) is a flag to the system
when it executes a shared for of a load module.
After the system runs the program, it leaves shared segments on the swap
device to speed subsequent reinvocation of the program.
Setting this bit is restricted to the superuser (to control depletion of
swap space which might result from overuse).
.PP
Only the owner of a file or the superuser may change its mode.
.SH "See Also"
.Xr "creat()," creat
.Xr "libc," libc
.Xr "stat.h" stat.h
.br
\*(PX Standard, \(sc5.6.4
.SH Diagnostics
.B chmod()
returns \-1 for errors, such as
.I file
being nonexistent or the invoker being neither the owner nor the superuser.
