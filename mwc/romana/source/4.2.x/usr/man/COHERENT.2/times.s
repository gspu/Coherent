ttiimmeess() -- System Call (libc)

Obtain process execution times
#iinncclluuddee <ssyyss/ttiimmeess.hh>
#iinncclluuddee <ttiimmee.hh>
iinntt ttiimmeess(_t_b_p)
ssttrruucctt ttmmss *_t_b_p;

ttiimmeess()  reads CPU  time  information about  the current  process and   its
children, and writes it into the structure pointed to by _t_b_p. The structure
ttmmss is declared in the header file ssyyss/ttiimmeess.hh, as follows:

struct    tms {
     __clock_t      tms_utime;/* process user time */
     __clock_t      tms_stime;/* process system time */
     __clock_t      tms_cutime;/* childrens' user times */
     __clock_t      tms_cstime;/* childrens' system times */
};

All of the times are measured in basic machine cycles, or CCLLKK_TTCCKK.

The childrens' times  include the sum of the times  of all terminated child
processes of  the current process and  of all of their  children.  The _u_s_e_r
time represents execution time of user code, whereas _s_y_s_t_e_m time represents
system overhead,  such as executing  system calls, processing  signals, and
other monitoring functions.

ttiimmeess() returns the number of ticks that have passed since system startup.

_F_i_l_e_s
<ssyyss/ttiimmeess.hh>
<ttiimmee.hh>

_S_e_e _A_l_s_o
aacccctt(), ffttiimmee(), lliibbcc, ttiimmee(), ttiimmeess.hh
POSIX Standard, section 4.5.2
