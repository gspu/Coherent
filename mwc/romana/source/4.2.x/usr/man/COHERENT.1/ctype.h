ccttyyppee.hh -- Header File

Header file for data tests
#iinncclluuddee <ccttyyppee.hh>

ccttyyppee.hh declares  and defines the  following routines, which  can check and
transform character types:

_ttoolloowweerr()Convert an upper-case character to lower case
_ttoouuppppeerr()Convert a lower-case character to upper case
iissaallnnuumm() Test if alphanumeric character
iissaallpphhaa() Test if alphabetic character
iissaasscciiii() Test if ASCII character
iissccnnttrrll() Test if a control character
iissddiiggiitt() Test if a numeric digit
iissggrraapphh() Test if a graphics character
iisslloowweerr() Test if lower-case character
iisspprriinntt() Test if printable character
iissppuunncctt() Test if punctuation mark
iissssppaaccee() Test if a tab, space, or return
iissuuppppeerr() Test if upper-case character
iissxxddiiggiitt()Test if hexadecimal numeral
ttooaasscciiii() Convert a character to ASCII
ttoolloowweerr() Convert an upper-case character to lower case
ttoouuppppeerr() Convert a lower-case character to upper case

_E_x_a_m_p_l_e
The  following   example  demonstrates  iissaallnnuumm(),   iissaallpphhaa(),  iissaasscciiii(),
iissccnnttrrll(),  iissddiiggiitt(), iisslloowweerr(), iisspprriinntt(),  iissppuunncctt(), and  iissssppaaccee(). It
prints information about the type of characters it contains.

#include <ctype.h>
#include <stdio.h>

main()
{
          FILE *fp;
          char fname[20];
          int ch;
          int alnum = 0;
          int alpha = 0;
          int allow = 0;
          int control = 0;
          int printable = 0;
          int punctuation = 0;
          int space = 0;

          printf("Enter name of text file to examine: ");
          fflush(stdout);
          gets(fname);

          if ((fp = fopen(fname, "r")) != NULL) {
          while ((ch = fgetc(fp)) != EOF) {

           if (isascii(ch)) {
            if (isalnum(ch))
             alnum++;
            if (isalpha(ch))
             alpha++;
            if (islower(ch))
             allow++;
            if (iscntrl(ch))
             control++;
            if (isprint(ch))
             printable++;
            if (ispunct(ch))
             punctuation++;
            if (isspace(ch))
              space++;

           } else {
            printf("%s is not ASCII.\n",
              fname);
            exit(1);
           }
          }

          printf("%s has the following:\n", fname);
          printf("%d alphanumeric characters\n", alnum);
          printf("%d alphabetic characters\n", alpha);
          printf("%d alphabetic lower-case characters\n",
           allow);
          printf("%d control characters\n", control);
          printf("%d printable characters\n", printable);
          printf("%d punctuation marks\n", punctuation);
          printf("%d white space characters\n", space);
          exit(0);

          } else  {
          printf("Cannot open \"%s\".\n", fname);
          exit(2);
          }
}

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, lliibbcc
ANSI Standard, section 7.3

_N_o_t_e_s
The  argument for  a  ccttyyppee function  or  macro should  be an  iinntt that  is
representable as  an uunnssiiggnneedd cchhaarr  or EOF --  i.e., [-1, 0,  ..., 255], as
described in the ANSI standard section 4.3.

The functions _ttoolloowweerr(), _ttoouuppppeerr(), iissaasscciiii(), and ttooaasscciiii() are not part
of the  ANSI standard.  Programs that  use them may not  be portable to all
implementations of C.
