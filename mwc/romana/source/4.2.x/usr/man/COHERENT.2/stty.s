ssttttyy() -- System Call (libc)

Set terminal modes
#iinncclluuddee <ssggttttyy.hh>
iinntt ssttttyy(_f_d, _s_g_p)
iinntt _f_d;
ssttrruucctt ssggttttyybb *_s_g_p;

The  COHERENT system  call ssttttyy()  sets a  terminal's attributes.   See the
Lexicon article  for ssttttyy for information on  terminal attributes and their
legal values.

_E_x_a_m_p_l_e
This example demonstrates both ssttttyy() and ggttttyy(). It sets terminal input to
read one  character at a  time (that is,  it reads the  terminal in ``raw''
form).   When  you type  `q',  it  restores the  terminal  to its  previous
settings,  and exits.   For  an additional  example, see  the ppiippee  Lexicon
article.

#include <sgtty.h>

main()
{
     struct sgttyb os, ns;
     char buff;

     printf("Waiting for q\n");
     gtty(1, &os);     /* save old state */
     ns = os;          /* get base of new state */
     ns.sg_flags |= RAW;/* prevent <ctl-c> from working */
     ns.sg_flags &= ~(ECHO|CRMOD);/* no echo for now... */
     stty(1, &ns);     /* set mode */

     do {
          buff = getchar();/* wait for the keyboard */
     } while(buff != 'q');

     stty(1, &os);  /* reset mode */
}

_F_i_l_e_s
<ssggttttyy.hh> -- Header file

_S_e_e _A_l_s_o
eexxeecc, ggttttyy(), iiooccttll(), lliibbcc, ooppeenn(), rreeaadd(), ssggttttyy.hh, ssttttyy, wwrriittee()

_N_o_t_e_s
Please note that  if you use ssttttyy() to change  the baud rate on a port, you
must first  invoke sslleeeepp().  If you  do not, the  port reverts back  to its
default settings.
