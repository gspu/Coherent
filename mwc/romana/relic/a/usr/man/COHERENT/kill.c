

kill                         Command                         kill




Signal a process

kkiillll [- _s_i_g_n_a_l ] _p_i_d ...

COHERENT assigns each active process a unique process id, or pid,
and uses  the pid to identify the process.   kill sends signal to
each pid.   signal must  be one of  the numbers described  in the
header <signal.h>  or <sys/msig.h>.  The  signal can be  given by
number or by name, as ddeeffiinneed in these header files.  By default,
signal is SIGTERM, which terminates a given process.

If pid  is zero,  kill signals each  process started by  the user
from the same tty.

The shell sh prints the process id of a process if the command is
detached.  The ps command  prints a list of all active processes,
with process ids and command line arguments.

A  user  can kill  only  the processes  he  owns; the  superuser,
however,  can kill  anything.  A process  cannot ignore  or catch
SIGKILL.

***** Files *****

<ssyyss/mmssiigg.hh> -- Machine-dependent signal numbers
<ssiiggnnaall.hh> -- Machine invariant signal numbers

***** See Also *****

commands, getpid(), init, kill(), ps, sh, signal()


























COHERENT Lexicon                                           Page 1


