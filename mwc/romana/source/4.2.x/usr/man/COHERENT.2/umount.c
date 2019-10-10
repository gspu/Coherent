uummoouunntt -- Command

Unmount file system
/eettcc/uummoouunntt _s_p_e_c_i_a_l

uummoouunntt unmounts a file system  _s_p_e_c_i_a_l that was previously mounted with the
mmoouunntt command.

The   script  /bbiinn/uummoouunntt  calls   /eettcc/uummoouunntt,  and   provides  convenient
abbreviations for commonly used devices.  For example, typing

    umount f0

executes the command

    /etc/umount /dev/fha0

The system  administrator should  edit this  script to reflect  the devices
used on your specific system.

_F_i_l_e_s
/eettcc/mmttaabb -- Mount table
/ddeevv/*
/bbiinn/uummoouunntt -- Script that calls /eettcc/uummoouunntt

_S_e_e _A_l_s_o
ccllrrii, ccoommmmaannddss, ffsscckk, iicchheecckk, mmoouunntt

_D_i_a_g_n_o_s_t_i_c_s
Errors can occur if _s_p_e_c_i_a_l does not exist or is not a mounted file system.
