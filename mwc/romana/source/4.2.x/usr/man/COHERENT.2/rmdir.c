rrmmddiirr -- Command

Remove directories
rrmmddiirr [ -ff ] _d_i_r_e_c_t_o_r_y ...

rrmmddiirr removes  each _d_i_r_e_c_t_o_r_y. This will  not be allowed if  a _d_i_r_e_c_t_o_r_y is
the current working directory or is  not empty.  The force option -ff allows
the superuser  to override these  restrictions.  rrmmddiirr removes  the `.' and
`..' entries  automatically.  Note that using the -ff  option on a directory
that is not empty will damage the file system, and require that it be fixed
with ffsscckk.

_S_e_e _A_l_s_o
ccoommmmaannddss, mmkkddiirr, rrmmgg

_N_o_t_e_s
rrmmddiirr -ff does _n_o_t remove files from a nonempty directory; it simply orphans
them.  To remove a nonempty directory and its contents, use rrmm -rr instead.
