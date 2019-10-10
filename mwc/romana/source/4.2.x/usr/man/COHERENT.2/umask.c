uummaasskk -- Command

Set the file-creation mask
uummaasskk [_O_O_O]

The _f_i_l_e-_c_r_e_a_t_i_o_n _m_a_s_k modifies the default mode assigned to each file upon
creation.  The mode sets the  permissions granted by the file's owner, plus
other important information about a file.

The command  uummaasskk sets  the default file-creation  mask to _O_O_O,  which are
three octal  numerals.  If  invoked without  an argument, uummaasskk  prints the
current file-creation mask in octal.

Note that zero bits in the  mask correspond to permitted permission bits in
the target,  and that execute permission cannot be  enabled via any setting
of _m_a_s_k. See the Lexicon entries  for uummaasskk() and cchhmmoodd for further details
on file mode.  The shell executes uummaasskk directly.

_E_x_a_m_p_l_e
Setting _m_a_s_k  to octal 022 (i.e.,  000 010 010) causes  a file created with
mode octal 0666 to actually have permissions of

    rw- r-- r--

Setting _m_a_s_k  to zero (i.e., 000  000 000) causes a  file created with mode
octal 0666 to actually have permissions of

    rw- rw- rw-

_S_e_e _A_l_s_o
cchhmmoodd, ccoommmmaannddss, kksshh, sshh, uummaasskk()
