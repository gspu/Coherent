

umask                        Command                        umask




Set the file-creation mask

uummaasskk [_m_a_s_k]

The _f_i_l_e-_c_r_e_a_t_i_o_n _m_a_s_k modifies the default mode assigned to each
file upon creation.  The mode sets the permissions granted by the
file's owner, plus other important information about a file.

The command  uummaasskk sets the  default file-creation mask  to _m_a_s_k.
_m_a_s_k is  usually entered  as an octal  number prefixed by  a zero
digit.  If invoked  without an argument, uummaasskk prints the current
file-creation mask in octal.

See the Lexicon entries for uummaasskk() and cchhmmoodd for details on file
mode.  The shell executes uummaasskk directly.

***** Example *****

Setting  _m_a_s_k to  octal 022  (i.e.,  000 010  010) causes  a file
created with mode octal 0666 to actually have permissions of


        rw- r-- r--


Setting _m_a_s_k  to zero (i.e.,  000 000 000) causes  a file created
with mode octal 0666 to actually have permissions of


        rw- rw- rw-


***** See Also *****

cchhmmoodd, ccoommmmaannddss, kksshh, sshh





















COHERENT Lexicon                                           Page 1


