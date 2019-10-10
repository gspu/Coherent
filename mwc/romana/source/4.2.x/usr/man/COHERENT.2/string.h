ssttrriinngg.hh -- Header File

Declarations for string library
#iinncclluuddee <ssttrriinngg.hh>

ssttrriinngg.hh is the header that holds  the prototypes of all ANSI routines that
handle strings and buffers.  It declares the following routines:

ffnnmmaattcchh()......Match a string with a normal expression
iinnddeexx()........Search string for a character; use ssttrrcchhrr() instead
mmeemmccccppyy()......Copy a region of memory up to a set character
mmeemmcchhrr().......Search a region of memory for a character
mmeemmccmmpp().......Compare two regions of memory
mmeemmccppyy().......Copy one region of memory into another
mmeemmmmoovvee()......Copy one region of memory into another with which it overlaps
mmeemmsseett().......Fill a region of memory with a character
ppnnmmaattcchh()......Match string pattern
ssttrrccaatt().......Concatenate two strings
ssttrrccmmpp().......Compare two strings
ssttrrnnccaatt()......Append one string onto another
ssttrrnnccmmpp()......Compare two lengths for a set number of bytes
ssttrrccppyy().......Copy a string
ssttrrnnccppyy()......Copy a portion of a string
ssttrrccoollll()......Compare two strings, using locale information
ssttrrccssppnn()......Return length one string excludes characters in another
ssttrrdduupp().......Duplicate a string
ssttrreerrrroorr().....Translate an error number into a string
ssttrrlleenn().......Measure a string
ssttrrppbbrrkk()......Find first occurrence in string of character from another string
ssttrrcchhrr().......Find leftmost occurrence of character in a string
ssttrrrrcchhrr()......Find rightmost occurrence of character in a string
ssttrrssppnn().......Return length one string includes character in another
ssttrrssttrr().......Find one string within another string
ssttrrttookk().......Break a string into tokens
ssttrrxxffrrmm()......Transform a string, using locale information

_E_x_a_m_p_l_e
This example reads from ssttddiinn up  to NNNNAAMMEESS names, each of which is no more
than MMAAXXLLEENN  characters long.  It  then removes duplicate  names, sorts the
names, and writes the sorted  list to the standard output.  It demonstrates
the functions sshheellllssoorrtt(), ssttrrccaatt(), ssttrrccmmpp(), ssttrrccppyy(), and ssttrrlleenn().

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NNAMES 512
#define MAXLEN 60

char *array[NNAMES];
char first[MAXLEN], mid[MAXLEN], last[MAXLEN];
char *space = " ";

int compare();

main()
{
               register int index, count, inflag;
               register char *name;

               count = 0;
               while (scanf("%s %s %s\n", first, mid, last) == 3) {
               strcat(first, space);
               strcat(mid, space);
               name = strcat(first, (strcat(mid, last)));
               inflag = 0;

               for (index=0; index < count; index++)
                if (strcmp(array[index], name) == 0)
                 inflag = 1;

               if (inflag == 0) {
                if ((array[count] =
                 malloc(strlen(name) + 1)) == NULL) {
                 fprintf(stderr, "Insufficient memory\n");
                 exit(EXIT_FAILURE);
                }
                strcpy(array[count], name);
                count++;
               }
               }

               shellsort(array, count, sizeof(char *), compare);
               for (index=0; index < count; index++)
               printf("%s\n", array[index]);
               exit(EXIT_SUCCESS);
}

compare(s1, s2)
register char **s1, **s2;
{
               extern int strcmp();
               return(strcmp(*s1, *s2));
}

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, lliibbcc
ANSI Standard, section 7.1.1

_N_o_t_e_s
Some implementations  of UNIX call  this header file ssttrriinnggss.hh.  If you are
porting code  to COHERENT, you  may have to modify  the #iinncclluuddee directives
that invoke this header file.

The ANSI standard allows adjacent string literals, e.g.:

    "hello" "world"

Adjacent  string  literals   are  automatically  concatenated.   Thus,  the
compiler will automatically concatenate the above example into:

    "helloworld"

Because this  departs from the  Kernighan and Ritchie description  of C, it
will generate a warning message if you use the compiler's -VVSSBBOOOOKK option.
