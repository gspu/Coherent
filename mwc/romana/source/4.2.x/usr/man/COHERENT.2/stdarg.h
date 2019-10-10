ssttddaarrgg.hh -- Header File

Header for variable numbers of arguments
#iinncclluuddee <ssttddaarrgg.hh>

ssttddaarrgg.hh is  the header  file that  ANSI C uses  to declare and  define the
routines that  traverse a variable-length  argument list.  It  declares the
type vvaa_lliisstt and defines the macros vvaa_aarrgg(), vvaa_ssttaarrtt(), and vvaa_eenndd().

_E_x_a_m_p_l_e
The following example concatenates multiple strings into a common allocated
string  and returns  the  string's address.   method  of handling  variable
arguments:

#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

char *
multcat(numargs)
int numargs;
{
    va_list argptr;
    char *result;
    int i, siz;

    /* get size required */
    va_start(argptr, numargs);
    for(siz = i = 0; i < numargs; i++)
        siz += strlen(va_arg(argptr, char *));

    if ((result = calloc(siz + 1, 1)) == NULL) {
        fprintf(stderr, "Out of space\n");
        exit(EXIT_FAILURE);
    }
    va_end(argptr);

    va_start(argptr, numargs);
    for(i = 0; i < numargs; i++)
        strcat(result, va_arg(argptr, char *));
    va_end(argptr);
    return(result);
}

int
main()
{
    printf(multcat(5, "One ", "two ", "three ",
        "testing", ".\n"));
}

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, vvaarraarrggss.hh
ANSI Standard, section 7.8

_N_o_t_e_s
The routines defined in <ssttddaarrgg.hh> were first implemented under UNIX System
V,  where they  are declared  in the  header file  <vvaarraarrggss.hh>. The  ANSI C
committee recognized the usefulness of <vvaarraarrggss.hh>, but decided that it had
semantic  problems.  In  particular, <vvaarraarrggss.hh>  introduced the  notion of
declaring ``...'' for  the variable-arguments argument list in the function
prototype.  This, unfortunately, left them with declarations of the form

    void error(...)
    {
        _w_h_a_t_e_v_e_r
    }

and no obvious hook for accessing the parameter list within the body of the
function.   So,  the  ANSI committee  changed  the  header declaration:  it
insisted  on  one  or more  formal  parameters,  followed  by  the list  of
variables.

The committee had  the wisdom to change the name  of its header file, hence
<ssttddaarrgg.hh>  came into  being.  Unfortunately, the  committee kept  the same
macro names, but in one  macro (vvaa_ssttaarrtt()) changed the number of arguments
it takes.

COHERENT includes both <vvaarraarrggss.hh> and <ssttddaarrgg.hh>, to support both ANSI and
System-V code.
