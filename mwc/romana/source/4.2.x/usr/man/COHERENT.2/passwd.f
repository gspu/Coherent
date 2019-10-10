ppaasssswwdd -- System Administration

Define system users

The file  /eettcc/ppaasssswwdd holds information about each  user who has permission
to use the COHERENT system.  This information is read by the commands llooggiinn
and ppaasssswwdd whenever a user attempts  to log in, to ensure that that user is
really himself and not an impostor.

/eettcc/ppaasssswwdd holds one record for  each user; each record, in turn, consists
of seven colon-separated fields, as follows:

    name:password:user_id:group_id:comments:home_dir:shell

_n_a_m_e is the user's login name.

_p_a_s_s_w_o_r_d is his encrypted password.   If this field holds a single asterisk
`*', then the program llooggiinn reads his password out of the file /eettcc/sshhaaddooww.

_u_s_e_r__i_d  is  a unique  number  that  is also  used  to  identify the  user.
_g_r_o_u_p__i_d identifies the group to which the user belongs, if any.

_c_o_m_m_e_n_t_s  holds miscellaneous  data, such as  names, telephone  numbers, or
office numbers.

_h_o_m_e__d_i_r gives the user's home directory.

Finally, _s_h_e_l_l gives the program that  is first executed when the user logs
on; in most instances, this is an interactive shell (default, /bbiinn/sshh).

If you wish, you can set  additional passwords to control users who attempt
to log  into your system  remotely (that is,  via a modem).  You  can set a
different  remote-access password  for each  group of  users, based  on the
program invoked when they log in; for example, you can set one password for
the users who  log in and invoke uuuucciiccoo, and  another for the users who log
in and  invoke the interactive shells  kksshh or sshh. For details  on how to do
this, see the Lexicon entries dd_ppaasssswwdd and ddiiaalluuppss.

When a  user creates a file,  that file is ``owned''  by him.  For example,
whenever user jjooee create a file, that file is ``owned'' by jjooee; and jjooee has
user-level  permissions  on that  file.   The superuser  rroooott  can use  the
command cchhoowwnn to  change the ownership of a file  from one user to another.
For details on this command, see its entry in the Lexicon.

_S_e_e _A_l_s_o
AAddmmiinniisstteerriinngg CCOOHHEERREENNTT, cchhoowwnn, ppaasssswwdd (ccoommmmaanndd)

_N_o_t_e_s
/eettcc/ppaasssswwdd can be read by anyone:  if access to it were refused to a user,
he could not  log on.  Thus, the passwords encrypted  within it can be read
and copied  by anyone, and so may be  vulnerable to brute-force decryption.
For this  reason, close attention should be paid  to passwords: they should
not be common words or names, preferably mix cases or use unique spellings,
and be at least six characters long.
