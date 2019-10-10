ppaasssswwdd -- Command

Set/change login password
ppaasssswwdd [_u_s_e_r]

ppaasssswwdd sets or changes the password  for the specified _u_s_e_r. If _u_s_e_r is not
specified, ppaasssswwdd changes the password of the caller.

ppaasssswwdd requests  that the  old password  (if any) be  typed, to  ensure the
caller is who  he claims to be.  Next it  requests a new password, and then
requests it again  in case of typing errors.  ppaasssswwdd  will ask for a longer
password if the one given is too short or not unusual enough.

_F_i_l_e_s
/eettcc/ppaasssswwdd -- Encrypted passwords

_S_e_e _A_l_s_o
ccoommmmaannddss, ccrryypptt(), llooggiinn

_N_o_t_e_s
One good  way to construct  a password is  to concatenate two  common words
plus a punctuation mark.  For example, ``dog@collar'' or ``hamlet&horatio''
are passwords that are both easy to remember and difficult to guess.
