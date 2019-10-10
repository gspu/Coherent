iiff -- Command

Execute a command conditionally
iiff _s_e_q_u_e_n_c_e_1 tthheenn _s_e_q_u_e_n_c_e_2 [eelliiff _s_e_q_u_e_n_c_e_3 tthheenn _s_e_q_u_e_n_c_e_4] ... [eellssee _s_e_q_u_e_n_c_e_5] ffii

The shell  construct iiff executes  commands conditionally, depending  on the
exit status of the execution of other commands.

First, iiff executes  the commands in _s_e_q_u_e_n_c_e_1. If the  exit status is zero,
it  executes  the  commands in  _s_e_q_u_e_n_c_e_2  and  terminates.  Otherwise,  it
executes the  optional _s_e_q_u_e_n_c_e_3  if given,  and executes _s_e_q_u_e_n_c_e_4  if the
exit status  is zero.  It  executes additional eelliiff  clauses similarly.  If
the exit status of each tested command sequence is nonzero, it executes the
optional eellssee part _s_e_q_u_e_n_c_e_5.

Because the  shell recognizes  a reserved word  only as the  unquoted first
word of a command, each tthheenn, eelliiff, eellssee, and ffii must either occur unquoted
at the start of a line or be preceded by `;'.

The shell executes iiff directly.

_E_x_a_m_p_l_e
For an example of this command, see the entry for ttrraapp.

_S_e_e _A_l_s_o
ccoommmmaannddss, kksshh, sshh, tteesstt
