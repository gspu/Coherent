ssttaatt.hh -- Header File

Definitions and declarations used to obtain file status
#iinncclluuddee <ssyyss/ssttaatt.hh>

ssttaatt.hh is  a header  file that declares  the structure ssttaatt  plus constants
used by  the routines that manipulate files,  directories, and named pipes.
It holds the prototypes for the routines cchhmmoodd(), ffssttaatt(), mmkkddiirr(), ssttaatt(),
and uummaasskk().

The following summarizes the structure ssttaatt:

struct stat {
     dev_t          st_dev;        /* Device */
     ino_t          st_ino;        /* Inode number */
     mode_t         st_mode;       /* Mode */
     nlink_t        st_nlink;      /* Link count */
     uid_t          st_uid;        /* User id */
     gid_t          st_gid;        /* Group id */
     dev_t          st_rdev;       /* Real device; NB, this is non-POSIX */
     off_t          st_size;       /* Size */
     time_t         st_atime;      /* Access time */
     time_t         st_mtime;      /* Modify time */
     time_t         st_ctime;      /* Change time */
};

sstt_ddeevv  and sstt_iinnoo  together form  a unique description  of the  file.  The
former is the  device on which the file and  its i-node reside, whereas the
latter is the index number of the file.  sstt_mmooddee gives the permission bits,
as outlined below.  sstt_nnlliinnkk gives the number of links to the file.  sstt_uuiidd
and  sstt_ggiidd, respectively  given the  user id  and group  id of  the owner.
sstt_rrddeevv, valid  only for special  files, holds the major  and minor numbers
for the file.   sstt_ssiizzee gives the size of the  file, in bytes.  For a pipe,
the size is the number of bytes waiting to be read from the pipe.

Three entries for each file give  the last occurrences of various events in
the file's history.  sstt_aattiimmee gives time  the file was last read or written
to.  sstt_mmttiimmee  gives the  time of the  last modification, write  for files,
create or delete entry for  directories.  sstt_ccttiimmee gives the last change to
the attributes, not including times and size.

The following manifest constants define file types:

     SS_IIFFMMTT    Type
     SS_IIFFDDIIRR   Directory
     SS_IIFFCCHHRR   Character-special file
     SS_IIFFPPIIPP   Pipe
     SS_IIFFIIFFOO   Pipe
     SS_IIFFBBLLKK   Block-special file
     SS_IIFFRREEGG   Regular file

The following manifest constants define file modes:

     SS_IIRREEAADD   Read permission, owner
     SS_IIWWRRIITTEE  Write permission, owner
     SS_IIEEXXEECC   Execute/search permission, owner
     SS_IIRRWWXXUU   RWX permission, owner
     SS_IIRRUUSSRR   Read permission, owner
     SS_IIWWUUSSRR   Write permission, owner
     SS_IIXXUUSSRR   Execute/search permission, owner
     SS_IIRRWWXXGG   RWX permission, group
     SS_IIRRGGRRPP   Read permission, group
     SS_IIWWGGRRPP   Write permission, group
     SS_IIXXGGRRPP   Execute/search permission, group
     SS_IIRRWWXXOO   RWX permission, other
     SS_IIRROOTTHH   Read permission, other
     SS_IIWWOOTTHH   Write permission, other
     SS_IIXXOOTTHH   Execute/search permission, other

_S_e_e _A_l_s_o
cchhmmoodd(), ffssttaatt(), hheeaaddeerr ffiillee, ssttaatt()
POSIX Standard, section 5.6.1
