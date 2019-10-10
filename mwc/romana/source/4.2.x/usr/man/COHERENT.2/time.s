ttiimmee() -- System Call (libc)

Get current system time
#iinncclluuddee <ttiimmee.hh>
ttiimmee_tt ttiimmee(_t_p)
ttiimmee_tt *_t_p;

ttiimmee() reads  and returns  the current  system time.  COHERENT  defines the
current  system  time as  the  number  of seconds  since  January 1,  1970,
0h00m00s GMT.

_t_p  points to  a data  element of  the type ttiimmee_tt,  which the  header file
ttiimmee.hh defines  as being equivalent  to a lloonngg.  If _t_p is  initialized to a
value other than  NULL, then ttiimmee() attempts to write  the system time into
the address  to which _t_p points.   If, however, _t_p is  initialized to NULL,
then ttiimmee() returns  the current system time but does  not attempt to write
it anywhere.

_E_x_a_m_p_l_e
For an example of this call, see the entry for aassccttiimmee().

_S_e_e _A_l_s_o
ddaattee, lliibbcc, ttiimmee [oovveerrvviieeww], ttiimmee.hh
ANSI Standard, section 7.12.2.4
POSIX Standard, section 4.5.1

_N_o_t_e_s
UNIX System V defines ttiimmee_tt in header file <ssyyss/ttyyppeess.hh>, whereas COHERENT
defines it in  ttiimmee.hh. This should not affect the  porting of programs from
UNIX to  COHERENT, but it may  affect the porting of  programs in the other
direction.
