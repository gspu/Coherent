mmttaabb.hh -- Header File

Currently mounted file systems
#iinncclluuddee <mmttaabb.hh>

The file  /eettcc/mmttaabb contains an entry  for each file system  mounted by the
command mmoouunntt. This does not include the root file system, which is already
mounted when the system boots.

Both the commands mmoouunntt and  uummoouunntt use the following structure, defined in
mmttaabb.hh. It  contains the name  of each special file  mounted, the directory
upon  which it  is mounted,  and any  flags passed to  mmoouunntt (such  as read
only).

#define MNAMSIZ 32
struct  mtab {
    char    mt_name[MNAMSIZ];
    char    mt_special[MNAMSIZ];
    int mt_flag;
};

_F_i_l_e_s
/eettcc/mmttaabb

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, mmoouunntt, uummoouunntt
