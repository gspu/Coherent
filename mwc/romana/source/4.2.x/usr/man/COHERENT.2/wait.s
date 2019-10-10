wwaaiitt() -- System Call (libc)

Await completion of a child process
#iinncclluuddee <ssyyss/wwaaiitt.hh>
wwaaiitt(_s_t_a_t_p)
iinntt *_s_t_a_t_p;

wwaaiitt() suspends  execution of  the invoking  process until a  child process
(created with ffoorrkk()) terminates.  It returns the process identifier of the
terminating child  process.  If  there are no  children or if  an interrupt
occurs, it returns -1.

If  it  is  successful,  wwaaiitt()  returns  the  process  identifier  of  the
terminated child process.  In addition, wwaaiitt() fills in the integer pointed
to by  _s_t_a_t_p with exit-status information about  the completed process.  If
_s_t_a_t_p is NULL, wwaaiitt() discards the exit-status information.

wwaaiitt()  fills in  the  low byte  of  the status-information  word with  the
termination  status  of  the  child  process.   A child  process  may  have
terminated because  of a signal, because  of an exit call,  or have stopped
execution  during  ppttrraaccee().  Termination  with  eexxiitt(),  which  is  normal
completion,  gives status  0.   Other terminations  give  signal values  as
status (as defined in the article  on ssiiggnnaall()). The 00220000 bit of the status
code indicates that  a core dump was produced.  A  status of 00117777 indicates
that the process is waiting for further action from ppttrraaccee().

The high byte of the returned status is the low byte of the argument to the
eexxiitt() system call.

If a parent process does not remain in existence long enough to wwaaiitt() on a
child  process,   the  child   process  is   adopted  by  process   1  (the
initialization process).

_E_x_a_m_p_l_e
For an example of this system call, see the entry for mmssggggeett().

_S_e_e _A_l_s_o
_eexxiitt(), ffoorrkk(), kksshh, lliibbcc, ppttrraaccee(), ssiiggnnaall(), sshh, wwaaiittppiidd(), wwaaiitt.hh
POSIX Standard, section 3.2.1
