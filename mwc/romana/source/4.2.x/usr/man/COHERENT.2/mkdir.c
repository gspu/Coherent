mmkkddiirr -- Command

Create a directory
mmkkddiirr [ -rrpp ] _d_i_r_e_c_t_o_r_y

mmkkddiirr  creates  _d_i_r_e_c_t_o_r_y.  Files or  directories  with  the  same name  as
_d_i_r_e_c_t_o_r_y must  not already exist.  ddiirreeccttoorryy will be  empty except for the
entries `.',  the directory's  link to  itself, and `..',  its link  to its
parent directory.

Option -rr creates directories recursively.  For example, the command

    mkdir -r /foo/bar/baz

creates directory ffoooo in /; then creates directory bbaarr in the newly created
directory  ffoooo; and  finally  creates directory  bbaazz in  the newly  created
directory bbaarr.

Option -pp behaves  exactly the same as -rr. COHERENT  includes it for use by
scripts imported from other operating systems.

_S_e_e _A_l_s_o
ccoommmmaannddss, mmkkddiirr(), rrmmddiirr

_D_i_a_g_n_o_s_t_i_c_s
mmkkddiirr fails  and prints an error  message if you do  not have permission to
write into directory in which you are attempting to create a new directory,
or if the  directory in which you attempted to  create a new directory does
not exist.
