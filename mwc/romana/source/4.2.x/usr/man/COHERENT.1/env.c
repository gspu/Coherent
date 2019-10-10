eennvv -- Command

Execute a command in an environment
eennvv [-] [_V_A_R_I_A_B_L_E=_v_a_l_u_e ...] [_c_o_m_m_a_n_d _a_r_g_s]

The  command  eennvv  executes  _c_o_m_m_a_n_d  with  _a_r_g_s,  modifying  the  existing
environment by performing the requested assignments.

The `-' option  tells eennvv to replace the environment  with the arguments of
the  form  VVAARRIIAABBLLEE=_v_a_l_u_e;  otherwise  the  assignments  are added  to  the
environment.

If _c_o_m_m_a_n_d is omitted, the resulting environment is printed.

_S_e_e _A_l_s_o
ccoommmmaannddss
