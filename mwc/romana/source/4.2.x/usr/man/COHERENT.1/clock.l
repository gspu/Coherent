cclloocckk() -- Time Function (libc)

Get processor time
#iinncclluuddee <ttiimmee.hh>
cclloocckk_tt cclloocckk();

The function cclloocckk() calculates and  returns the amount of processor time a
program  has taken  to execute  to  the current  point.  Execution  time is
calculated from the time the program was invoked.  This, in turn, is set as
a point from the beginning of an era that is defined by the implementation.
Under  COHERENT,  time is  recorded  as the  number  of milliseconds  since
January 1, 1970, 0h00m00s GMT.

The value  cclloocckk() returns is of  type cclloocckk_tt, which is  defined in header
file.  ttiimmee.hh.  If cclloocckk() cannot  determine execution time,  it returns -1
cast to cclloocckk_tt.

To calculate  the execution time  in seconds, divide the  value returned by
cclloocckk() by the value of the macro CCLLKK_TTCCKK, which is also defined in ttiimmee.hh.

_E_x_a_m_p_l_e
This example measures the number of  times a ffoorr loop can run in one second
on your system.  This is approximate  because CCLLKK_TTCCKK can be a real number,
and because the program probably will not start at an exact tick boundary.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

main()
{
    clock_t finish;
    long i;

    /* finish = about 1 second from now */
    finish = clock() + CLK_TCK;
    for(i = 0; finish > clock(); i++)
        ;

    printf("The for() loop ran %ld times in one second.\n", i);
    return(EXIT_SUCCESS);
}

_S_e_e _A_l_s_o
ddiiffffttiimmee(), lliibbcc, mmkkttiimmee(), ttiimmee.hh
ANSI Standard, section .12.2.1
