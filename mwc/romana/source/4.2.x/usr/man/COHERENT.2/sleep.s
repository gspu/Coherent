sslleeeepp() -- General Function (libc)

Suspend execution for interval
#iinncclluuddee <uunniissttdd.hh>
sslleeeepp(_s_e_c_o_n_d_s)
uunnssiiggnneedd _s_e_c_o_n_d_s;

sslleeeepp() suspends execution for not less than _s_e_c_o_n_d_s.

_E_x_a_m_p_l_e
The following example demonstrates how to use sslleeeepp():

#include <unistd.h>
main()
{
    printf("Waiting for Godot ...\n");

    for ( ; ; ) {
        sleep(5);   /* sleep for five seconds */
        printf("... still waiting ...\n");
    }
}

_S_e_e _A_l_s_o
lliibbcc, nnaapp(), uunniissttdd.hh
POSIX Standard, section 3.4.3

_N_o_t_e_s
To make  a program  sleep for  less than one  second, use the  system calls
nnaapp() or ppoollll(). For an example, see the Lexicon article for ppoollll().
