eexxiitt() -- System Call (libc)

Terminate a program gracefully
#iinncclluuddee <ssttddlliibb.hh>
vvooiidd eexxiitt(_s_t_a_t_u_s) iinntt _s_t_a_t_u_s;

eexxiitt()  is  the normal  method  to terminate  a  program directly.   _s_t_a_t_u_s
information is passed to the parent process.  By convention, an exit status
of  zero  indicates  success, whereas  an  exit  status  greater than  zero
indicates  failure.  If  the parent  process  issued a  wwaaiitt() call,  it is
notified of the termination and  is passed the least significant eight bits
of _s_t_a_t_u_s.  As eexxiitt() never  returns, it is always  successful.  Unlike the
related  function _eexxiitt(),  eexxiitt()  does extra  cleanup,  such as  flushing
buffered files and closing open files.

_E_x_a_m_p_l_e
For an example of this function, see the entry for ffooppeenn().

_S_e_e _A_l_s_o
_eexxiitt(), aatteexxiitt(), cclloossee(), lliibbcc, ssttddlliibb.hh, wwaaiitt()
ANSI Standard, section 7.10.4.3
POSIX Standard, section 8.1

_N_o_t_e_s
If a  program leaves  mmaaiinn() by  an error condition,  it usually  returns a
random number.  If you want to  test a program's return code, you must call
eexxiitt() or return from mmaaiinn().
