.TH chmod "" "" Command
.PC "Change the modes of a file"
\fBchmod +\fImodes file\fR
\fBchmod -\fImodes file\fR
.PP
.HS
.Pp
Mode may be octal or a comma-separated symbolic list:
\fB[\fIwhich\fB]\fIhowperm\fB...[,...]\fR
.SH which:
.IC \fBa\fR
User, group, and other permissions
.IC \fBg\fR
Group permissions
.IC \fBo\fR
Other permissions
.IC \fBu\fR
User permissions
.Pp
Missing \fIwhich\fR implies that `a', `g', `o', and `u' can be combined.
.SH how:
.IC \fB=\fR
Set permissions
.IC \fB+\fR
Add permissions
.IC \fB-\fR
Take away permissions
.SH perm:
.IC \fBg\fR
Current group permissions
.IC \fBo\fR
Current other permissions
.IC \fBr\fR
Read
.IC \fBs\fR
Setuid on execution
.IC \fBt\fR
Sticky bit (save text)
.IC \fBu\fR
Current user permissions
.IC \fBw\fR
Write
.IC \fBx\fR
Execute
.HE
The \*(CO system assigns a
.I mode
to every file, to govern how users access the file.
The mode grants or denies
permission to read, write, or execute a file.
.PP
The mode grants permission separately to
the owner of a file, to users from the owner's group, and to all other users.
For a directory, execute permission grants or denies the right to
search the directory,
whereas write permission grants or denies the right to
create and remove files.
.PP
.II "sticky bit"
In addition, the mode contains three bits that perform special tasks:
the set-user-id bit,
the set-group-id bit, and the save-text or \*(QLsticky\*(QR bit.
See the Lexicon entry for the \*(CO system call
.B chmod()
for more information on how to use these bits.
.PP
The command
.B chmod
changes the permissions of each specified
.I file
according to the given
.I mode
argument.
.I mode
may be either an octal number or a symbolic mode.
Only the owner of a
.I file
or the superuser may change a file's mode.
Only the superuser may set the sticky bit.
.PP
A symbolic mode may have the following form.
No spaces should separate the fields in the actual
.I mode
specification.
.DS
	\fI[which] how perm ... [, ...]\fR
.DE
.PP
.I which
specifies the permissions that are affected by the command.
It may consist of one or more of the following:
.DS
	\fBa\fR	All permissions, equivalent to \fBgou\fR
	\fBg\fR	Group permissions
	\fBo\fR	Other permissions
	\fBu\fR	User permissions
.DE
.PP
If no
.I which
is given,
.B a
is assumed and
.B chmod
uses the file creation mask,
as described in
.BR umask .
.PP
.I how
specifies how the permissions will be changed.
It can be
.DS
	\fB=\fR	Set permissions
	\fB+\fR	Add permissions
	\fB-\fR	Take away permissions
.DE
.PP
.I perm
specifies which permissions are changed.
It may consist of one or more of the following:
.DS
	\fBg\fR	Current group permissions
	\fBo\fR	Current other permissions
	\fBr\fR	Read permission
	\fBs\fR	Setuid upon execution
	\fBt\fR	Save text (sticky bit)
	\fBu\fR	Current user permissions
	\fBw\fR	Write permission
	\fBx\fR	Execute permission
.DE
.PP
Multiple
\fIhow\fR/\fIperm\fR pairs have the same
.I which
applied to them.
One or more specifications separated by commas tell
.B chmod
to apply each specification to the file successively.
.PP
An octal
.I mode
argument to
.B chmod
is obtained by
.IR OR ing
the desired mode bits together.
For a list of the recognized octal modes, see the Lexicon entry for
.BR chmod() .
.SH Examples
The first example below sets the
owner's permissions to read + write + execute,
and the group and other permissions to
read + execute.
The second example adds execute permission for everyone.
.DM
	chmod u=rwx,go=rx file
	chmod +x file
.DE
.SH "See Also"
.Xr "chgrp," chgrp
.Xr "chmod()," chmod
.Xr "chmog," chmog
.Xr "chown," chown
.Xr "commands," commands
.Xr "ls," ls
.Xr "stat," stat
.Xr "umask" umask
