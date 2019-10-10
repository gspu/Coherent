.TH shadow.h "" "" "Header File"
.PC "Definitions used with shadow passwords"
.B "#include <shadow.h>"
.PP
The header file
.B <shadow.h>
declares and defines the functions, macros, structures, and constants
used to manipulate shadow passwords.
.PP
.B <shadow.h>
holds defines the structure
.BR spwd ,
which describes the records that are stored in file
.BR /etc/shadow .
.B <shadow.h>
gives two definitions
.BR spwd :
one implements the structure used by \*(UN System V, release 4; and the
other implements the structure used by \*(UN System V, release 3.
.PP
The following gives the form of
.B spwd
that is used by some releases of \*(UN System V, release 4:
.DM
struct spwd {
	char *sp_namp;	/* User Name */
	char *sp_pwdp;	/* Encrypted password */
	long sp_lstchg;	/* Last changed date */
	long sp_min;
	long sp_max;
	long sp_warn;
	long sp_inact;
	long sp_expire;	/* Acct expiration date. */
	unsigned long sp_flag;
};
.DE
.PP
The following gives the version of
.B spwd
used by \*(UN System V, release 3:
.DM
struct	spwd {
	char *sp_name; /* User name */
	char *sp_passwd; /* Encrypted password - non-POSIX */
	int sp_uid; /* User id */
	int sp_gid; /* Group id */
	int sp_quota; /* File space quota  - non-POSIX*/
	char *sp_comment; /* Comments - non-POSIX */
	char *sp_gecos; /* Gecos box number - non-POSIX */
	char *sp_dir; /* Working directory */
	char *sp_shell; /* Shell */
};
.DE
.PP
By default, \*(CO uses the System V, release 3, version of
.BR spwd .
.PP
For information on how to select a given version of
.BR spwd ,
see the discussion of compilation environments in the Lexicon article
.BR "header files" .
.SH "See Also"
.Xr "header files," header_fi
.Xr "endspent()," endspent
.Xr "getspent()," getspent
.Xr "getspnam()," getspnam
.Xr "libc," libc
.Xr "setspent()," setspent
.Xr "shadow" shadow
