mmoouunntt -- Command

Mount a file system
/eettcc/mmoouunntt [ _d_e_v_i_c_e _d_i_r_e_c_t_o_r_y [ -rruu ] ]

mmoouunntt  mounts  a  file  system  from  the block-special  file  _d_e_v_i_c_e  onto
_d_i_r_e_c_t_o_r_y in  the system's directory  hierarchy.  This operation  makes the
root directory  of the mounted  file system accessible  using the specified
_d_i_r_e_c_t_o_r_y name.

If  the -rr  option is  specified, the  file system  is read-only.   This is
useful for  preventing inadvertent changes  to precious file  systems.  The
system will  not update information  such as access  times if option  -rr is
used.

The  option -uu  tells mmoouunntt  to write  an entry  into the  mount-table file
/eettcc/mmttaabb without  actually mounting the  file system.  When  this is done,
COHERENT will  hereafter mount the  file system automatically  whenever you
boot COHERENT.

When invoked  with no arguments, mmoouunntt summarizes  the mounted file systems
and where they attach.

The command uummoouunntt unmounts a previously mounted file system.

The   script   /bbiinn/mmoouunntt  calls   /eettcc/mmoouunntt,   and  provides   convenient
abbreviations for commonly used devices.  For example,

    mount f0

executes the command:

    /etc/mount /dev/fha0 /f0

The system  administrator should  edit this  script to reflect  the devices
used on your system.

_F_i_l_e_s
/eettcc/mmttaabb -- Mount table
/eettcc/mmnnttttaabb -- Mount table
/bbiinn/mmoouunntt -- Shell script that calls /eettcc/mmoouunntt

_S_e_e _A_l_s_o
ccoommmmaannddss, ffsscckk, mmkkffss, mmkknnoodd, uummoouunntt

_D_i_a_g_n_o_s_t_i_c_s
Errors can occur  if _d_e_v_i_c_e or _d_i_r_e_c_t_o_r_y does not  exist or if the user has
no permissions on _d_e_v_i_c_e.

The message

    /etc/mtab older than /etc/boottime

indicates  that /eettcc/mmttaabb  has  probably been  invalidated  by booting  the
system.

Attempting to  mmoouunntt a block-special file that does  not contain a COHERENT
file system  can have disastrous  consequences.  _C_a_v_e_a_t _u_t_i_l_i_t_o_r!  You must
first use the  command mmkkffss to create a file  system on a blank disk before
you mount it.
