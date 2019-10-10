cchhoowwnn() -- System Call (libc)

Change ownership of a file
#iinncclluuddee <uunniissttdd.hh>
cchhoowwnn(_f_i_l_e, _u_i_d, _g_i_d)
cchhaarr *_f_i_l_e; sshhoorrtt _u_i_d, _g_i_d;

cchhoowwnn() changes the owner of _f_i_l_e to user id _u_i_d and group id _g_i_d.

To change  only the user  id without changing  the group id,  use ssttaatt() to
determine the value of _g_i_d to pass to cchhoowwnn().

cchhoowwnn() is restricted to  the superuser, because granting the ordinary user
the ability  to change the  ownership of files might  circumvent file space
quotas or accounting based upon file ownership.

cchhoowwnn() returns -1  for errors, such as nonexistent _f_i_l_e  or the caller not
being the superuser.

_S_e_e _A_l_s_o
cchhmmoodd(), lliibbcc, ppaasssswwdd, ssttaatt(), uunniissttdd.hh
POSIX Standard, section 5.6.5
