

grp.h                      Header File                      grp.h




Declare group structure

#include <grp.h>

The header file grp.h declares the structure group, which is com-
posed as follows:


struct group (
     char *gr_name; /* group name */
     char *gr_passwd;/* group password */
     int  gr_gid;   /* numeric group id */
     char **gr_mem; /* group members */
};


This structure holds information about the group to which a given
user belongs.   It is used  by the functions  endgrent, getgrent,
getgrgid, getgrnam, and setgrent.

***** See Also *****

header files

































COHERENT Lexicon                                           Page 1


