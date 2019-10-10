

mtab.h                     Header File                     mtab.h




Currently mounted file systems

#include <mtab.h>

The file /etc/mtab contains an entry for each file system mounted
by the  mount command.  This does not include  the root file sys-
tem, which is already mounted when the system boots.

Both the  mount and umount commands  use the following structure,
defined in  mtab.h.  It  contains the  name of each  special file
mounted, the  directory upon which  it is mounted,  and any flags
passed to mount (such as read only).


#define MNAMSIZ 32
struct  mtab {
        char    mt_name[MNAMSIZ];
        char    mt_special[MNAMSIZ];
        int     mt_flag;
};


***** Files *****

/etc/mtab
<mtab.h>

***** See Also *****

header files, mount, umount


























COHERENT Lexicon                                           Page 1


