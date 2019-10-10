wwaaiitt -- Command

Await completion of background process
wwaaiitt [_p_i_d]

Typing the character  `&' after a command tells the  shell sshh to execute it
as  a _b_a_c_k_g_r_o_u_n_d  (or _d_e_t_a_c_h_e_d)  process;  otherwise, it  is executed  as a
_f_o_r_e_g_r_o_u_n_d process.  You can perform other tasks while a background process
is  being  executed.   The shell  prints  the  process  id  number of  each
background  process when  it is  invoked.  ppss  reports on  currently active
processes.

The  command wwaaiitt  tells the  shell  to suspend  execution until  the child
process with the given _p_i_d is completed.  If no _p_i_d is given, wwaaiitt suspends
execution  until all  background processes are  completed.  If  the process
with  the given  _p_i_d is  not  a child  process of  the current  shell, wwaaiitt
returns immediately.

The shell executes wwaaiitt directly.

_S_e_e _A_l_s_o
ccoommmmaannddss, kksshh, ppss, sshh

_N_o_t_e_s
If a  subshell invokes a background process and  then terminates, wwaaiitt will
return immediately  rather than waiting  for termination of  the grandchild
process.
