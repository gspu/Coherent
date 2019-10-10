tteerrmm -- System Administration

Format of compiled terminfo file

Before it can be used, a file of tteerrmmiinnffoo information must be compiled with
the command ttiicc. It is read by the command sseettuupptteerrmm.

Once compiled,  the binary tteerrmmiinnffoo  file is moved into  a sub-directory of
directory /uussrr/lliibb/tteerrmmiinnffoo.  To avoid a  linear search of  a huge COHERENT
directory,  a  two-level   scheme  is  used  to  name  the  subdirectories:
/uussrr/lliibb/tteerrmmiinnffoo/_C/_n_a_m_e, where _n_a_m_e names  the terminal and _C is the first
character  of  _n_a_m_e. For  example,  the  tteerrmmiinnffoo entry  for  the Wyse  150
terminal is  kept in the file  /uussrr/lliibb/tteerrmmiinnffoo/ww/wwyyssee115500.  Synonyms for a
terminal exist as links to the same compiled file.

The binary  format of a tteerrmmiinnffoo  file has been designed to  be the same on
all hardware.  The file is  divided into six parts: header, terminal names,
boolean flags, numbers, strings, and string table.

_H_e_a_d_e_r
The _h_e_a_d_e_r  section begins the  file.  This section  contains the following
six short integers:

11. The magic number (octal 0432).

22. The size, in bytes, of the _n_a_m_e_s section.

33. The number of bytes in the _b_o_o_l_e_a_n section.

44. The number of short integers in the _n_u_m_b_e_r_s section.

55. The number of offsets (short integers) in the _s_t_r_i_n_g_s section.

66. The size, in bytes, of the _s_t_r_i_n_g table.

A _s_h_o_r_t  _i_n_t_e_g_e_r is  two bytes  long.  Under the  tteerrmm file  format, 0xFFFF
represents -1;  all other  negative value  are illegal.  Minus  1 generally
means that a capability is  missing from this terminal.  All short integers
are aligned on a short-word boundary.

_N_a_m_e_s
The  _n_a_m_e_s section  contains the  first line  of the  tteerrmmiinnffoo description,
which lists the  names for the terminal, each name  separated by a vertical
bar `|'.  The section is  terminated with a NUL.

_B_o_o_l_e_a_n
The _b_o_o_l_e_a_n section contains the boolean flags for terminals.  There is one
flag for each boolean capacity  recognized by tteerrmmiinnffoo. The flags appear in
the order described in the header  file tteerrmm.hh. Each flag is one byte long,
and is set to zero or one, depending upon whether the capacity is absent or
present in this  terminal.  If necessary, this section is  ended with a NUL
to ensure that the next section begins on an even byte.

_N_u_m_b_e_r_s
The _n_u_m_b_e_r_s  section is similar to  the _f_l_a_g_s section.  There  is one entry
for  each numeric  capacity  recognized by  tteerrmmiinnffoo,  each capacity  being
represented by a short integer.  A value of -1 indicates that this terminal
lacks this capability.  Entries appear in the order described in the header
file tteerrmm.hh.

_S_t_r_i_n_g_s
The  _s_t_r_i_n_g_s  section  also contains  one  short  integer  for each  string
capability recognized  by tteerrmmiinnffoo. A value of -1  means that this terminal
lacks  this capability.   Otherwise,  the value  gives an  offset from  the
beginning of  the string table.   Entries appear in the  order described in
the header file tteerrmm.hh.

Special characters  in ^XX  or \_c notation  are stored in  their interpreted
form.  Padding  information and parameter information  are stored intact in
uninterpreted form.

_S_t_r_i_n_g _T_a_b_l_e
The final section is the _s_t_r_i_n_g _t_a_b_l_e. It contains all the values of string
capabilities  referenced  in  the  _s_t_r_i_n_g  section.   Each string  is  null
terminated.

_F_i_l_e_s
/uussrr/lliibb/tteerrmmiinnffoo/* -- Default location of object files

_S_e_e _A_l_s_o
AAddmmiinniisstteerriinngg CCOOHHEERREENNTT, ccuurrsseess, iinnffooccmmpp, ttiicc, tteerrmmiinnffoo
Strang, J.,  Mui, L., O'Reilly, T.: _t_e_r_m_c_a_p  _a_n_d _t_e_r_m_i_n_f_o.  Sebastopol, CA:
O'Reilly & Associates, Inc., 1991.

_N_o_t_e_s
The total  compiled file cannot exceed 4,096 bytes.   The _n_a_m_e field cannot
exceed 128 bytes.
