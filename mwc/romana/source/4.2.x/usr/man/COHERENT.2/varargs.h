vvaarraarrggss.hh -- Header File

Declare/define routines for variable arguments
#iinncclluuddee <vvaarraarrggss.hh>

The header  file <vvaarraarrggss.hh>  prototypes and  defines the routines  used to
manage variable arguments.  These routines are modelled after those used in
UNIX  System V.   The  routines in  vvaarraarrggss.hh  were designed  to  give a  C
compiler a semi-rational way of dealing with functions (e.g., pprriinnttff())
that can  take a  variable number of  arguments.  In brief,  these routines
consist  of the  variable-list ttyyppeeddeeff  vvaa_lliisstt, the  parameter declaration
vvaa_ddccll, and the three macros vvaa_ssttaarrtt(), vvaa_aarrgg(), and vvaa_eenndd(). The macros
respectively start  the argument list,  fetch the next member,  and end the
argument list.

_S_e_e _A_l_s_o
hheeaaddeerr ffiilleess, ssttddaarrgg.hh

_N_o_t_e_s
These routines are also implemented in the header file <ssttddaarrgg.hh>, which is
described in  the ANSI Standard.  For details  on how these implementations
differ, see the entry for ssttddaarrgg.hh.
