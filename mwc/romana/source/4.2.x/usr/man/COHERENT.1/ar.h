aarr.hh -- Header File

Format for archive files
#iinncclluuddee <aarr.hh>

An _a_r_c_h_i_v_e is a file that  has been built from a number of files.  Archives
are  maintained by  the command  aarr. Usually,  an archive  is a  library of
object files used by the linker lldd.

The header  aarr.hh describes  the format of  an archive.  All  archives start
with a  magic number AARRMMAAGG, which  identifies the file as  an archive.  The
members  of the  archive  follow the  magic  number, each  preceded by  the
structure aarr_hhddrr:

#define DIRSIZ  14
#define ARMAG   0177535          /* magic number */

struct ar_hdr {
        char    ar_name[DIRSIZ]; /* member name */
        time_t  ar_date;         /* time inserted */
        short   ar_gid;          /* group owner */
        short   ar_uid;          /* user owner */
        short   ar_mode;         /* file mode */
        size_t  ar_size;         /* file size */
};

The structure at the head of each member is immediately followed by aarr_ssiizzee
bytes, which are the data of the file.

To  enhance the  performance of  lldd, the command  rraannlliibb provides  a random
library facility.   rraannlliibb produces archives  that contain a  special entry
named __.SSYYMMDDEEFF at the beginning.

All  integer  members of  the  structure (everything  but  aarr_nnaammee) are  in
canonical form to ease portability.  See ccaannoonn.hh for more information.

_S_e_e _A_l_s_o
aarr, ccaannoonn.hh, hheeaaddeerr ffiilleess, lldd, rraannlliibb
