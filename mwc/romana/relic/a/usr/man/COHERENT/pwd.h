

pwd.h                      Header File                      pwd.h




Declare password structure

#include <pwd.h>

The  header file  pwd.h declares the  structure passwd,  which is
used  to build  COHERENT's password file.   passwd is  defined as
follows:


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


For detailed descriptions of  the above fields, see the entry for
passwd.

***** See Also *****

endpwent(),  getpwent(),  getpwnam(),  getpwuid(), header  files,
setpwent()



























COHERENT Lexicon                                           Page 1


