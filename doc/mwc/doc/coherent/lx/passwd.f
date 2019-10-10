.TH passwd "" "" "System Administration"
.PC "Define system users"
.PP
The file
.B /etc/passwd
holds information about each user who has permission to use
the \*(CO system.
This information is read by the commands
.B login
and
.B passwd
whenever a user attempts to log in, to ensure that that user is
really himself and not an impostor.
.PP
.B /etc/passwd
holds one record for each user; each record, in turn, consists of
seven colon-separated fields, as follows:
.DM
	name:password:user_id:group_id:comments:home_dir:shell
.DE
.PP
.I name
is the user's login name.
.PP
.I password
is his encrypted password.
If this field holds a single asterisk `*', then the program
.B login
reads his password out of the file
.BR /etc/shadow .
.PP
.I user_id
is a unique number that is also used to identify the user.
.I group_id
identifies the group to which the user belongs, if any.
.PP
.I comments
holds miscellaneous data, such as names, telephone numbers, or
office numbers.
.PP
.I home_dir
gives the user's home directory.
.PP
Finally,
.I shell
gives the program that is first executed when the user logs on;
in most instances, this is an interactive shell (default, \fB/bin/sh\fR).
.PP
If you wish, you can set additional passwords to control users who
attempt to log into your system remotely (that is, via a modem).
You can set a different remote-access password for each group of users,
based on the program invoked when they log in; for example, you can
set one password for the users who log in and invoke
.BR uucico ,
and another for the users who log in and invoke the interactive shells
.B ksh
or
.BR sh .
For details on how to do this, see the Lexicon entries
.B d_passwd
and
.BR dialups .
.PP
When a user creates a file, that file is ``owned'' by him.
For example, whenever user
.B joe
create a file, that file is ``owned'' by
.BR joe ;
and
.B joe
has user-level permissions on that file.
.II chown
The superuser
.B root
can use the command
.B chown
to change the ownership of a file from one user to another.
For details on this command, see its entry in the Lexicon.
.SH "See Also"
.Xr "Administering COHERENT," administe
.Xr "chown," chown.c
.Xr "passwd [command]" passwd.c
.R
.SH Notes
.B /etc/passwd
can be read by anyone:
if access to it were refused to a user, he could not log on.
Thus, the passwords encrypted within it can be read and copied by anyone,
and so may be vulnerable to brute-force decryption.
For this reason, close attention should be paid to passwords:
they should not be common words or names, preferably mix cases
or use unique spellings, and be at least six characters long.
