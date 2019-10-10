uummoouunntt() -- System Call (libc)

Unmount a file system
#iinncclluuddee <ssyyss/mmoouunntt.hh>
uummoouunntt(_f_i_l_e_s_y_s_t_e_m)
cchhaarr *_f_i_l_e_s_y_s_t_e_m;

uummoouunntt()  is  the  COHERENT  system  call  that  unmounts  a  file  system.
_f_i_l_e_s_y_s_t_e_m names  the block-special file  through which the  file system is
accessed.  Note  that this must have  been previously mounted by  a call to
mmoouunntt(), or the call will fail.

_S_e_e _A_l_s_o
lliibbcc, mmoouunntt()
