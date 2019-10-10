rrmmddiirr() -- System Call (libc)

Remove a directory
#iinncclluuddee <uunniissttdd.hh>
iinntt rrmmddiirr(_p_a_t_h)
cchhaarr *_p_a_t_h;

The  COHERENT  system  call  rrmmddiirr()  removes  the directory  specified  by
argument  _p_a_t_h. To  remove  the directory,  the  following conditions  must
apply:

-> _p_a_t_h must exist and be accessible,  it must be empty (i.e., contain only
   entries for `.' and `..').

-> You must have permission to remove the directory.

-> The file system that contains _p_a_t_h must not be mounted ``read only''.

-> The directory must not be the current directory for any process.

-> The directory must not be a mount point for another file system.

If  the directory  is successfully  removed, rrmmddiirr()  returns zero.   If an
error occurs, it returns -1 and sets eerrrrnnoo to an appropriate value.

_S_e_e _A_l_s_o
lliibbcc, mmkkddiirr, mmkkddiirr(), rrmmddiirr, uunniissttdd.hh, uunnlliinnkk()
POSIX Standard, section 5.5.2
