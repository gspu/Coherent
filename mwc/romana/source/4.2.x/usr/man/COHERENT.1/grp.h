ggrrpp.hh -- Header File

Declare group structure
#iinncclluuddee <ggrrpp.hh>

The header  file ggrrpp.hh declares  the structure ggrroouupp, which  is composed as
follows:

struct group (
     char *gr_name; /* group name */
     char *gr_passwd;/* group password */
     int  gr_gid;   /* numeric group id */
     char **gr_mem; /* group members */
};

This  structure holds  information about  the group to  which a  given user
belongs, as  defined in the  file /eettcc/ggrroouupp. It  is used by  the functions
eennddggrreenntt(), ggeettggrreenntt(), ggeettggrrggiidd(), ggeettggrrnnaamm(), and sseettggrreenntt().

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess
POSIX Standard, section 9.2.1
