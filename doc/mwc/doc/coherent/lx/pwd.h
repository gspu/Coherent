.TH pwd.h "" "" "Header File"
.PC "Define password structure"
.B "#include <pwd.h>"
.PP
The header file
.B pwd.h
defines the structure
.BR passwd ,
which is used to build \*(CO's password file.
.B passwd
is defined as follows:
.DM
.ta 0.5i 1.0i 2.0i
struct passwd {
	char	*pw_name;	/* login user name */
	char	*pw_passwd;	/* login password */
	int	pw_uid;	/* login user id */
	int	pw_gid;	/* login group id */
	int	pw_quota;	/* file quota (unused) */
	char	*pw_comment;	/* comments (unused) */
	char	*pw_gecos;	/* (unused) */
	char	*pw_dir;	/* working directory */
	char	*pw_shell;	/* initial program */
};
.DE
.PP
For detailed descriptions of the above fields, see the entry for
.BR passwd .
.SH "See Also"
.Xr "endpwent()," endpwent
.Xr "getpwent()," getpwent
.Xr "getpwnam()," getpwnam
.Xr "getpwuid()," getpwuid
.Xr "header files," header_fi
.Xr "setpwent()" setpwent
.br
\*(PX Standard, \(sc9.2.2
