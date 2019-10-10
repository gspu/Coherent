wwhhiillee -- Command

Execute commands repeatedly
wwhhiillee _s_e_q_u_e_n_c_e_1 [ddoo _s_e_q_u_e_n_c_e_2] ddoonnee

The shell construct wwhhiillee controls  a loop.  It first executes the commands
in _s_e_q_u_e_n_c_e_1. If  the exit status is zero, the  shell executes the commands
in the optional _s_e_q_u_e_n_c_e_2 and repeats  the process until the exit status of
_s_e_q_u_e_n_c_e_1 is nonzero.  Because the shell recognizes a reserved word only as
the unquoted first word of a  command, both ddoo and ddoonnee must occur unquoted
at the start of a line or preceded by `;'.

The shell  commands bbrreeaakk and  ccoonnttiinnuuee may be  used to alter  control flow
within a wwhhiillee  loop.  The uunnttiill construct has the  same form as wwhhiillee, but
the sense of the test is reversed.

The shell executes wwhhiillee directly.

_S_e_e _A_l_s_o
bbrreeaakk, ccoommmmaannddss, ccoonnttiinnuuee, kksshh, sshh, tteesstt, uunnttiill
