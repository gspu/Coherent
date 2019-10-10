.TH  grp.h "" "" "Header File"
.PC "Declare group structure"
.B "#include <grp.h>"
.PP
.II "group structure"
.II "structure^group"
The header file
.B grp.h
declares the structure
.BR group ,
which is composed as follows:
.DM
.ta 0.5i 1.0i 2.0i
struct group (
	char *gr_name;	/* group name */
	char *gr_passwd;	/* group password */
	int  gr_gid;	/* numeric group id */
	char **gr_mem;	/* group members */
};
.DE
.PP
.II group
This structure holds information about the group to which
a given user belongs, as defined in the file
.BR /etc/group .
It is used by the functions
.BR endgrent() ,
.BR getgrent() ,
.BR getgrgid() ,
.BR getgrnam() ,
and
.BR setgrent() .
.SH "See Also"
.Xr "header files" header_fi
.br
\*(PX Standard, \(sc9.2.1
