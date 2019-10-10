ppwwdd.hh -- Header File

Define password structure
#iinncclluuddee <ppwwdd.hh>

The header file ppwwdd.hh defines the  structure ppaasssswwdd, which is used to build
COHERENT's password file.  ppaasssswwdd is defined as follows:

struct passwd {
     char *pw_name; /* login user name */
     char *pw_passwd;/* login password */
     int  pw_uid;   /* login user id */
     int  pw_gid;   /* login group id */
     int  pw_quota; /* file quota (unused) */
     char *pw_comment;/* comments (unused) */
     char *pw_gecos;/* (unused) */
     char *pw_dir;  /* working directory */
     char *pw_shell;/* initial program */
};

For detailed descriptions of the above fields, see the entry for ppaasssswwdd.

_S_e_e _A_l_s_o
eennddppwweenntt(), ggeettppwweenntt(), ggeettppwwnnaamm(), ggeettppwwuuiidd(), hheeaaddeerr ffiilleess, sseettppwweenntt()
POSIX Standard, section 9.2.2
