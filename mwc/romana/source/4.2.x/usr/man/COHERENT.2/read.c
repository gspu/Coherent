rreeaadd -- Command

Assign values to shell variables
rreeaadd _n_a_m_e ...

rreeaadd reads  a line from the  standard input.  It assigns  each token of the
input to the corresponding shell variable _n_a_m_e. If the input contains fewer
tokens than the number of names  specified, rreeaadd assigns the null string to
each extra variable.  If the input  contains more tokens than the number of
names specified,  rreeaadd assigns the last  _n_a_m_e in the list  the remainder of
the input.

The shell executes rreeaadd directly.

_E_x_a_m_p_l_e
The command

    read foo bar baz
    hello how are you

parses  the  line  ``hello  how  are  you''  and  assigns  the  tokens  to,
respectively, the shell variables ffoooo, bbaarr, and bbaazz. If you further type

    echo $foo
    echo $bar
    echo $baz

you will see:

    hello
    how
    are you

_S_e_e _A_l_s_o
ccoommmmaannddss, kksshh, sshh

_D_i_a_g_n_o_s_t_i_c_s
rreeaadd normally returns an exit status of zero.  If it encounters end of file
or is interrupted while reading the standard input, it then returns one.
