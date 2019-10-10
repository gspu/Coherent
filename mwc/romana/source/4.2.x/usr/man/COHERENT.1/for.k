ffoorr -- C Keyword

Control a loop
ffoorr(_i_n_i_t_i_a_l_i_z_a_t_i_o_n; _e_n_d_c_o_n_d_i_t_i_o_n; _m_o_d_i_f_i_c_a_t_i_o_n)

ffoorr is a C keyword that introduces a loop.  It takes three arguments, which
are  separated by  semicolons `;'.  _i_n_i_t_i_a_l_i_z_a_t_i_o_n  is executed  before the
loop  begins.  _e_n_d_c_o_n_d_i_t_i_o_n  describes the  condition  that ends  the loop.
_m_o_d_i_f_i_c_a_t_i_o_n is a statement that modifies _v_a_r_i_a_b_l_e to control the number of
iterations of the loop.  For example,

    for (i=0; i<10; i++)

first sets  the variable  ii to  zero; then it  declares that the  loop will
continue as long  as ii remains less than ten;  and finally, increments ii by
one after  every iteration of  the loop.  This  ensures that the  loop will
iterate exactly ten times (from ii==00 through ii==99). The statement

    for(;;)

will loop  until its execution is  interrupted by a bbrreeaakk,  ggoottoo, or rreettuurrnn
statement.   Also, either  or both of  _i_n_i_t_i_a_l_i_z_a_t_i_o_n and  _m_o_d_i_f_i_c_a_t_i_o_n may
consist of multiple statements that are separated by commas.  For example,

    for (i=0, j=0; i<10; i++, j++)

initializes both  _i and _j, and  increments both with each  iteration of the
loop.

_S_e_e _A_l_s_o
bbrreeaakk, CC kkeeyywwoorrddss, ccoonnttiinnuuee, wwhhiillee
ANSI Standard, section 6.6.5.3
