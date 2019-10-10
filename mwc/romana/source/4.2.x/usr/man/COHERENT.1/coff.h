ccooffff.hh -- Header File

Format for COFF objects
#iinncclluuddee <ccooffff.hh>

ccooffff.hh describes the Common Object  File Format (COFF), which is the object
format used by COHERENT 386.

_W_h_a_t _I_s _C_O_F_F?
In brief, COFF is the UNIX  System V standard for file formats.  It defines
the formats  for relocatable object modules, for  executable files, and for
archives.

A COFF file is built around three sections, or _s_e_g_m_e_n_t_s:

tteexxtt This  holds executable  machine code.   It is  write protected  -- the
     operating system is forbidden to overwrite it.  (This is why operating
     systems  that  use  COFF  or  similar  formats  are  said  to  run  in
     ``protected mode.'')

ddaattaa This holds initialized data, that  is, the data that the program finds
     when it  begins execution.  The  program can read and  write into this
     segment.

bbssss  This segment  holds unitialized  data.  It is  simply a mass  of space
     that  is  initialized  to zeroes.   It  is  contiguous  with the  ddaattaa
     segment.  The term bbssss from the old IBM mainframe days, and stands for
     ``block started by symbol''.

Not all  segments have to  be included in  every COFF file.   Further, some
implementations of  COFF define their own  segments that manipulate special
features of the operating system or hardware.

The following describes the structure of a COFF file.  The areas within the
file are described in the order in which they appear.

11. _f_i_l_e _h_e_a_d_e_r
     This holds information set when the file was created, such as the date
     and time it was created, the number of segments in the file, a pointer
     to the symbol table, and status flags.

22. _o_p_t_i_o_n_a_l _h_e_a_d_e_r
     This gives  information set  at run-time, such  as the address  of the
     program entry point, and the size of the code and data segments.

33. _s_e_g_m_e_n_t _h_e_a_d_e_r_s
     The  next area  holds a  header for  each segment  in the  file.  Each
     header describes  its segment's characteristics  and contains pointers
     to  the   segment's  contents,  relocation   information,  line-number
     information, and other useful addresses.

44. _s_e_g_m_e_n_t _c_o_n_t_e_n_t_s
     The next area holds the contents of the segments used in this file.

55. _r_e_l_o_c_a_t_i_o_n _i_n_f_o_r_m_a_t_i_o_n
     The fifth  area gives relocation  information, one set  of information
     for each segment in the file.   The linker lldd uses this information to
     generate the executable file at link time.

66. _l_i_n_e-_n_u_m_b_e_r _i_n_f_o_r_m_a_t_i_o_n
     This area  holds debug  information, one  set of information  for each
     segment.  This area is optional.

77. _s_y_m_b_o_l _t_a_b_l_e
     This area holds information used by both the linker and the debugger.

88. _s_t_r_i_n_g _t_a_b_l_e
     This table holds very long names of variables.

Most of  this information is  irrelevant to the  average user, or  even the
average developer  of software.  To  the average user, COFF  is ``a machine
that would go of itself''; you can run or compile programs without worrying
what the  linker puts where, or  why.  These details, however,  can be very
important if you are writing  tools that manipulate the internals of files,
such as  archivers or debuggers.  If you need  detailed information on COFF
and how to manipulate it, see  _U_n_d_e_r_s_t_a_n_d_i_n_g _a_n_d _U_s_i_n_g _C_O_F_F, by Gintaras R.
Gircys (Sebastopol, Calif, O'Reilly & Associates, Inc., 1988).

For more  information on  how the  COFF format affects  COHERENT's language
tools, see the Lexicon articles for aarr, aass, cccc, ddbb, and lldd.

_S_e_e _A_l_s_o
aarr, aass, cccc, ccddmmpp, ccooffffnnlliisstt(), ffiillee ffoorrmmaattss, hheeaaddeerr ffiilleess, lldd
Gircys, G.R.: _U_n_d_e_r_s_t_a_n_d_i_n_g  _a_n_d _U_s_i_n_g _C_O_F_F.  Sebastopol, Calif, O'Reilly &
Associates, Inc., 1988.
