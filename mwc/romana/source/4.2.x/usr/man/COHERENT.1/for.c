ffoorr -- Command

Execute commands for tokens in list
ffoorr _n_a_m_e [iinn _t_o_k_e_n ...] ddoo _s_e_q_u_e_n_c_e ddoonnee

The shell  command ffoorr controls  a loop.  It  assigns to the  variable _n_a_m_e
each successive  _t_o_k_e_n in the list,  and then executes the  commands in the
given _s_e_q_u_e_n_c_e. If the iinn  clause is omitted, ffoorr successively assigns _n_a_m_e
the  value  of each  positional  parameter to  the  current script  (`$@').
Because the  shell recognizes  a reserved word  only as the  unquoted first
word of a command, both ddoo and ddoonnee must either occur unquoted at the start
of a command or be preceded by ';'.

The shell  commands bbrreeaakk and  ccoonnttiinnuuee may be  used to alter  control flow
within a ffoorr loop.

The shell executes ffoorr directly.

_S_e_e _A_l_s_o
bbrreeaakk, ccoommmmaannddss, ccoonnttiinnuuee, kksshh, sshh
