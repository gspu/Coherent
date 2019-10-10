kkiillll -- Command

Signal a process
kkiillll [- _s_i_g_n_a_l ] _p_i_d ...

COHERENT assigns each active process a  unique process id, or _p_i_d, and uses
the pid  to identify the  process.  kkiillll sends  _s_i_g_n_a_l to each  _p_i_d. _s_i_g_n_a_l
must be  one of the  numbers described in  the header file  <ssiiggnnaall.hh>. The
signal can be given by number or by name, as ddeeffiinneed in these header files.
By default, _s_i_g_n_a_l is SSIIGGTTEERRMM, which terminates a given process.

If _p_i_d is  zero, kkiillll signals each process in  the same process group (that
is, every process started by the same user from the same tty).

If _p_i_d is negative (but not  -1), kkiillll signals every process in the process
group whose ID equals the absolute value of _p_i_d.

If _p_i_d is -1, kkiillll signals each process that you own.  If you are logged in
as the  superuser rroooott, this signals every process  except processes 0 (the
kernel) and 1 (iinniitt).

The shell  prints the process id  of a process if  the command is detached.
The command ppss prints a list  of all active processes, with process ids and
command-line arguments.

A user  can kill only  the processes he  owns; the superuser,  however, can
kill anything.  A process cannot ignore or catch SSIIGGKKIILLLL.

See the  Lexicon article for ssiiggnnaall()  for a table of  the signals and what
each means.

_F_i_l_e_s
<ssiiggnnaall.hh> -- Signals

_S_e_e _A_l_s_o
ccoommmmaannddss, ggeettppiidd(), iinniitt, kkiillll(), kksshh, ppss, sshh, ssiiggnnaall()
