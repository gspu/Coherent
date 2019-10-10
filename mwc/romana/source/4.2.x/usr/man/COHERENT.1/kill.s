kkiillll() -- System Call (libc)

Kill a system process
#iinncclluuddee <ssiiggnnaall.hh>
kkiillll(_p_i_d, _s_i_g)
iinntt _p_i_d, _s_i_g;

kkiillll() is the  COHERENT system call that sends a  signal to a process.  _p_i_d
is  the  process  identifier  of  the  process to  be  signalled,  and  _s_i_g
identifies the signal to be sent,  as set in the header file ssiiggnnaall.hh. This
system call is most often used to kill processes, hence its name.

See the  Lexicon article for ssiiggnnaall()  for a table of  the signals and what
each means.  If  _s_i_g is zero, kkiillll() performs error  checking, but sends no
signal.  You can use this feature to check the validity of _p_i_d.

_E_x_a_m_p_l_e
For an example of using this system call in a C program, see ssiiggnnaall().

_S_e_e _A_l_s_o
lliibbcc, ssiiggnnaall(), ssiiggnnaall.hh
POSIX Standard, section 3.3.2
