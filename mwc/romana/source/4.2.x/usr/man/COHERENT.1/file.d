FFIILLEE -- Definition

Descriptor for a file stream
#iinncclluuddee <ssttddiioo.hh>

FFIILLEE  describes a  _f_i_l_e _s_t_r_e_a_m  which can  be either  a file  on disk  or a
peripheral device  through which  data flow.  It  is defined in  the header
file ssttddiioo.hh.

A pointer to FFIILLEE is  returned by ffooppeenn(), ffrreeooppeenn(), ffddooppeenn(), and related
functions.

The FFIILLEE structure is as follows:

    typedef struct  FILE
    {
        unsigned char *_cp,
                  *_dp,
                  *_bp;
        int _cc;
        int (*_gt)(),
            (*_pt)();
        int _ff;
        char    _fd;
        int _uc;
    } FILE;

_ccpp points to  the current character in the file.   _ddpp points to the start
of the data within the buffer.   _bbpp points to the file buffer.  _cccc is the
number  of  unprocessed  characters in  the  buffer.   _ggtt  and _pptt  point,
respectively, to  the functions ggeettcc()  and ppuuttcc(). _ffff  is a bit  map that
holds the various file flags, as follows:

    _FFIINNUUSSEE 0x01    Unused
    _FFSSTTBBUUFF 0x02    Used by macro sseettbbuuff()
    _FFUUNNGGOOTT 0x04    Used by uunnggeettcc()
    _FFEEOOFF   0x08    Tested by macro ffeeooff()
    _FFEERRRR   0x10    Tested by macro ffeerrrroorr()

_ffdd  is the  file  descriptor, which  is  used by  low-level routines  like
ooppeenn(); it is also used by rreeooppeenn(). Finally, _uucc is the character that has
been ``ungotten'' by the function uunnggeettcc(), should it be used.

_S_e_e _A_l_s_o
ffooppeenn(), ffrreeooppeenn(), PPrrooggrraammmmiinngg CCOOHHEERREENNTT, ssttddiioo.hh, ssttrreeaamm
ANSI Standard, section 7.9.1
