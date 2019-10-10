mmkknnoodd -- Command

Make a special file or named pipe
/eettcc/mmkknnoodd [ -ff ] _f_i_l_e_n_a_m_e _t_y_p_e _m_a_j_o_r _m_i_n_o_r
/eettcc/mmkknnoodd [ -ff ] _f_i_l_e_n_a_m_e pp

In the first form, mmkknnoodd creates a _s_p_e_c_i_a_l _f_i_l_e, which provides access to a
device by the  _f_i_l_e_n_a_m_e specified.  Special files are conventionally stored
in the /ddeevv directory.

_t_y_p_e  can be  either `b'  (for block-special file)  or `c'  (for character-
special file).   Block-special files  tend to be  devices such as  disks or
magnetic tape,  upon which COHERENT  uses an elaborate  buffering strategy.
Character-special files are unstructured (character at a time) devices such
as terminals, line  printers, or communications devices.  Character-special
files may also be random-access devices; this circumvents system buffering,
allowing  transfers of  arbitrary size  directly between  the user  and the
hardware.

The _m_a_j_o_r  device number uniquely  identifies a device  driver to COHERENT.
The _m_i_n_o_r device number is a  parameter interpreted by the driver; it might
specify the channel of a multiplexor or the unit number of a drive.

The caller must be the superuser.

In the  second form, mmkknnoodd  creates a named  pipe with the  given _f_i_l_e_n_a_m_e.
Named pipes can be used for communication between processes.

The -ff  option to mmkknnoodd forces  the creation of a new node,  even if one of
the same name already exists.

_F_i_l_e_s
/ddeevv/*

_S_e_e _A_l_s_o
ccoommmmaannddss, mmoouunntt
