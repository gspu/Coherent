ccaassee -- Command

Execute commands conditionally according to pattern
ccaassee _t_o_k_e_n iinn [_p_a_t_t_e_r_n [|_p_a_t_t_e_r_n] ...) _s_e_q_u_e_n_c_e ;;] ... eessaacc

ccaassee is a construct that used  by the shell.  It tells the shell to execute
commands conditionally,  according to a pattern.  It  tests the given _t_o_k_e_n
successively against  each _p_a_t_t_e_r_n, in  the order given.   It then executes
the commands  in the _s_e_q_u_e_n_c_e corresponding to  the first matching pattern.
Optional `|' clauses  specify additional patterns corresponding to a single
_s_e_q_u_e_n_c_e. If  no _p_a_t_t_e_r_n matches the _t_o_k_e_n, the  ccaassee construct executes no
commands.

Each _p_a_t_t_e_r_n can  include text characters (which match themselves), special
characters `?' (which matches  any character except newline) and `*' (which
matches  any sequence  of  non-newline characters),  and character  classes
enclosed in  brackets `[  ]'; ranges  of characters within  a class  may be
separated by `-'.   In particular, the last _p_a_t_t_e_r_n in  a ccaassee construct is
often `*', which will match any _t_o_k_e_n.

The shell executes ccaassee directly.

_E_x_a_m_p_l_e
The following example prints a string in response to a command-line option:

    case $1 in
        FOO)    echo "This is option FOO";;
        BAR)    echo "This is option BAR";;
        BAZ)    echo "This is option BAZ";;
        *)  echo "An asterisk marks the default option";;
    esac

_S_e_e _A_l_s_o
ccoommmmaannddss, kksshh, sshh
